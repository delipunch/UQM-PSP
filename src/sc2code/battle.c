//Copyright Paul Reiche, Fred Ford. 1992-2002

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "battle.h"

#include "controls.h"
#include "init.h"
#include "intel.h"
#ifdef NETPLAY
#	include "netplay/netmelee.h"
#	ifdef NETPLAY_CHECKSUM
#		include "netplay/checksum.h"
#	endif
#endif
#include "resinst.h"
#include "nameref.h"
#include "setup.h"
#include "settings.h"
#include "sounds.h"
#include "libs/graphics/gfx_common.h"
#include "libs/log.h"
#include "libs/mathlib.h"


QUEUE disp_q;
SIZE battle_counter;
BOOLEAN instantVictory;
size_t battleInputOrder[NUM_SIDES];
		// Indices of the sides in the order their input is processed.
		// Network sides are last so that the sides will never be waiting
		// on eachother, and games with a 0 frame delay are theoretically
		// possible.
#ifdef NETPLAY
BattleFrameCounter battleFrameCount;
		// Used for synchronisation purposes during netplay.
COUNT currentDeadSide;
		// When a ship has been destroyed, each side of a network
		// connection waits until the other side is ready.
		// When two ships die at the same time, this is handled for one
		// ship after the other. This variable indicate for which player
		// we're currently doing this.
#endif

static BOOLEAN
RunAwayAllowed (void)
{
	return (LOBYTE (GLOBAL (CurrentActivity)) == IN_ENCOUNTER
			|| LOBYTE (GLOBAL (CurrentActivity)) == IN_LAST_BATTLE)
			&& GET_GAME_STATE (STARBASE_AVAILABLE)
			&& !GET_GAME_STATE (BOMB_CARRIER);
}

static void
DoRunAway (STARSHIPPTR StarShipPtr)
{
	ELEMENTPTR ElementPtr;

	LockElement (StarShipPtr->hShip, &ElementPtr);
	if (GetPrimType (&DisplayArray[ElementPtr->PrimIndex]) == STAMP_PRIM
			&& ElementPtr->life_span == NORMAL_LIFE
			&& !(ElementPtr->state_flags & FINITE_LIFE)
			&& ElementPtr->mass_points != MAX_SHIP_MASS * 10
			&& !(ElementPtr->state_flags & APPEARING))
	{
		extern void flee_preprocess (PELEMENT);

		battle_counter -= MAKE_WORD (1, 0);

		ElementPtr->turn_wait = 3;
		ElementPtr->thrust_wait = MAKE_BYTE (4, 0);
		ElementPtr->preprocess_func = flee_preprocess;
		ElementPtr->mass_points = MAX_SHIP_MASS * 10;
		ZeroVelocityComponents (&ElementPtr->velocity);
		StarShipPtr->cur_status_flags &=
				~(SHIP_AT_MAX_SPEED | SHIP_BEYOND_MAX_SPEED);

		SetPrimColor (&DisplayArray[ElementPtr->PrimIndex],
				BUILD_COLOR (MAKE_RGB15 (0x0B, 0x00, 0x00), 0x2E));
		SetPrimType (&DisplayArray[ElementPtr->PrimIndex], STAMPFILL_PRIM);
	
		CyborgDescPtr->ship_input_state = 0;
	}
	UnlockElement (StarShipPtr->hShip);
}

static void
setupBattleInputOrder(void)
{
	size_t i;

#ifndef NETPLAY
	for (i = 0; i < NUM_SIDES; i++)
		battleInputOrder[i] = i;
#else
	int j;

	i = 0;
	// First put the locally controlled players in the array.
	for (j = 0; j < NUM_SIDES; j++) {
		if (!(PlayerControl[j] & NETWORK_CONTROL)) {
			battleInputOrder[i] = j;
			i++;
		}
	}
	
	// Next put the network controlled players in the array.
	for (j = 0; j < NUM_SIDES; j++) {
		if (PlayerControl[j] & NETWORK_CONTROL) {
			battleInputOrder[i] = j;
			i++;
		}
	}
#endif
}

static void
ProcessInput (void)
{
	BOOLEAN CanRunAway;
	size_t sideI;

#ifdef NETPLAY
	netInput ();
#endif

	CanRunAway = RunAwayAllowed ();
		
	for (sideI = 0; sideI < NUM_SIDES; sideI++)
	{
		HSTARSHIP hBattleShip, hNextShip;

		cur_player = battleInputOrder[sideI];

		for (hBattleShip = GetHeadLink (&race_q[cur_player]);
				hBattleShip != 0; hBattleShip = hNextShip)
		{
			BATTLE_INPUT_STATE InputState;
			STARSHIPPTR StarShipPtr;

			StarShipPtr = LockStarShip (&race_q[cur_player], hBattleShip);
			hNextShip = _GetSuccLink (StarShipPtr);

			if (StarShipPtr->hShip)
			{
				CyborgDescPtr = StarShipPtr;

				InputState = (*(PlayerInput[cur_player]))(cur_player,
						StarShipPtr);
				//Correct place!
				InputState |= GetDirectionalJoystickInput(StarShipPtr->ShipFacing,cur_player);
#if CREATE_JOURNAL
				JournalInput (InputState);
#endif /* CREATE_JOURNAL */
#ifdef NETPLAY
				if (!(PlayerControl[cur_player] & NETWORK_CONTROL))
				{
					BattleInputBuffer *bib = getBattleInputBuffer(cur_player);
					sendBattleInputConnections (InputState);
					flushPacketQueues ();

					BattleInputBuffer_push (bib, InputState);
							// Add this input to the end of the buffer.
					BattleInputBuffer_pop (bib, &InputState);
							// Get the input from the front of the buffer.
				}
#endif

				CyborgDescPtr->ship_input_state = 0;
				if (CyborgDescPtr->RaceDescPtr->ship_info.crew_level)
				{
					
					if (InputState & BATTLE_LEFT)
						CyborgDescPtr->ship_input_state |= LEFT;
					else if (InputState & BATTLE_RIGHT)
						CyborgDescPtr->ship_input_state |= RIGHT;
					if (InputState & BATTLE_THRUST)
						CyborgDescPtr->ship_input_state |= THRUST;
					if (InputState & BATTLE_WEAPON)
						CyborgDescPtr->ship_input_state |= WEAPON;
					if (InputState & BATTLE_SPECIAL)
						CyborgDescPtr->ship_input_state |= SPECIAL;

					if (CanRunAway && cur_player == 0 &&
							(InputState & BATTLE_ESCAPE))
						DoRunAway (StarShipPtr);
				}
			}
			//Wrong place!
			//InputState |= GetDirectionalJoystickInput(StarShipPtr->ShipFacing,cur_player);
 
			UnlockStarShip (&race_q[cur_player], hBattleShip);
		}
	}
	
#ifdef NETPLAY
	flushPacketQueues ();
#endif

	if (GLOBAL (CurrentActivity) & (CHECK_LOAD | CHECK_ABORT))
		GLOBAL (CurrentActivity) &= ~IN_BATTLE;
}

#if DEMO_MODE || CREATE_JOURNAL
DWORD BattleSeed;
#endif /* DEMO_MODE */

static MUSIC_REF BattleRef;

void
BattleSong (BOOLEAN DoPlay)
{
	if (BattleRef == 0)
	{
		if (LOBYTE (GLOBAL (CurrentActivity)) != IN_HYPERSPACE)
			BattleRef = LoadMusic (BATTLE_MUSIC);
		else if (GET_GAME_STATE (ARILOU_SPACE_SIDE) <= 1)
			BattleRef = LoadMusic (HYPERSPACE_MUSIC);
		else
			BattleRef = LoadMusic (QUASISPACE_MUSIC);
	}

	if (DoPlay)
		PlayMusic (BattleRef, TRUE, 1);
}

void
FreeBattleSong (void)
{
	DestroyMusic (BattleRef);
	BattleRef = 0;
}

static BOOLEAN
DoBattle (BATTLE_STATE *bs)
{
	extern UWORD nth_frame;
	RECT r;
	BYTE battle_speed;

	bs->MenuRepeatDelay = 0;
	SetMenuSounds (MENU_SOUND_NONE, MENU_SOUND_NONE);

#if defined (NETPLAY) && defined (NETPLAY_CHECKSUM)
	if (getNumNetConnections() > 0 &&
			battleFrameCount % NETPLAY_CHECKSUM_INTERVAL == 0)
	{
		crc_State state;
		Checksum checksum;

		crc_init(&state);
		crc_processState (&state);
		checksum = (Checksum) crc_finish (&state);

		sendChecksumConnections ((uint32) battleFrameCount,
				(uint32) checksum);
		flushPacketQueues ();
		addLocalChecksum (battleFrameCount, checksum);
	}
#endif
	ProcessInput ();
			// Also calls NetInput()
#if defined (NETPLAY) && defined (NETPLAY_CHECKSUM)
	if (getNumNetConnections() > 0)
	{
		size_t delay = getBattleInputDelay();

		if (battleFrameCount >= delay
				&& (battleFrameCount - delay) % NETPLAY_CHECKSUM_INTERVAL == 0)
		{
			if (!(GLOBAL (CurrentActivity) & CHECK_ABORT))
			{
				if (!verifyChecksums (battleFrameCount - delay)) {
					GLOBAL(CurrentActivity) |= CHECK_ABORT;
					resetConnections (ResetReason_syncLoss);
				}
			}
		}
	}
#endif

	LockMutex (GraphicsLock);
	if (bs->first_time)
	{
		r.corner.x = SIS_ORG_X;
		r.corner.y = SIS_ORG_Y;
		r.extent.width = SIS_SCREEN_WIDTH;
		r.extent.height = SIS_SCREEN_HEIGHT;
		SetTransitionSource (&r);
	}
	BatchGraphics ();
	if ((LOBYTE (GLOBAL (CurrentActivity)) == IN_HYPERSPACE) &&
			!(GLOBAL (CurrentActivity) & (CHECK_ABORT | CHECK_LOAD)))
		SeedUniverse ();
	RedrawQueue (TRUE);

	if (bs->first_time)
	{
		bs->first_time = FALSE;
		ScreenTransition (3, &r);
	}
	UnbatchGraphics ();
	UnlockMutex (GraphicsLock);
	if ((!(GLOBAL (CurrentActivity) & IN_BATTLE)) ||
			(GLOBAL (CurrentActivity) & (CHECK_ABORT | CHECK_LOAD)))
	{
		return FALSE;
	}

	battle_speed = HIBYTE (nth_frame);
	if (battle_speed == (BYTE)~0)
	{	// maximum speed, nothing rendered at all
		TaskSwitch ();
	}
	else
	{
		SleepThreadUntil (bs->NextTime
				+ BATTLE_FRAME_RATE / (battle_speed + 1));
		bs->NextTime = GetTimeCounter ();
	}

	if ((GLOBAL (CurrentActivity) & IN_BATTLE) == 0)
		return FALSE;

#ifdef NETPLAY
	battleFrameCount++;
#endif
	return TRUE;
}

// Let each player pick his ship.
static BOOLEAN
selectAllShips (SIZE num_ships)
{
#ifndef NETPLAY
	while (num_ships--)
	{
		if (!GetNextStarShip (NULL_PTR, num_ships == 1))
			return FALSE;
	}
	return TRUE;
#else
	// On network play, what is the top player to one party may be
	// the bottom player the other. To keep both sides synchronised
	// the order is always the same.
	SIZE order[2];

	if (num_ships == 1) {
		// HyperSpace in full game.
		return GetNextStarShip (NULL_PTR, 0);
	}

	if (num_ships != 2)
	{
		log_add (log_Error, "More than two players is not supported.\n");
		return FALSE;
	}

	if ((PlayerControl[0] & NETWORK_CONTROL) &&
			(PlayerControl[1] & NETWORK_CONTROL))
	{
		log_add (log_Error, "Only one side at a time can be network "
				"controlled.\n");
		return FALSE;
	}

	// Iff 'myTurn' is set on a connection, the local party will be
	// processed first.
	// If neither is network controlled, the top player (1) is handled first.
	if (((PlayerControl[0] & NETWORK_CONTROL) &&
			!NetConnection_getDiscriminant (netConnections[0])) ||
			((PlayerControl[1] & NETWORK_CONTROL) &&
			NetConnection_getDiscriminant (netConnections[1])))
	{
		order[0] = 0;
		order[1] = 1;
	}
	else
	{
		order[0] = 1;
		order[1] = 0;
	}

	{
		size_t i;
		for (i = 0; i < 2; i++)
		{
			if (!GetNextStarShip (NULL_PTR, order[i]))
				return FALSE;
		}
	}
	return TRUE;
#endif	
}

BOOLEAN
Battle (void)
{
	SIZE num_ships;

	LockMutex (GraphicsLock);

	SetResourceIndex (hResIndex);

#if !(DEMO_MODE || CREATE_JOURNAL)
	if (LOBYTE (GLOBAL (CurrentActivity)) != SUPER_MELEE) {
		// In Supermelee, the RNG is already initialised.
		TFB_SeedRandom (GetTimeCounter ());
	}
#else /* DEMO_MODE */
	if (BattleSeed == 0)
		BattleSeed = TFB_Random ();
	TFB_SeedRandom (BattleSeed);
	BattleSeed = TFB_Random (); /* get next battle seed */
#endif /* DEMO_MODE */

	BattleSong (FALSE);
	
	num_ships = InitShips ();

	if (instantVictory)
	{
		num_ships = 0;  // no ships were harmed in the making of this battle
		battle_counter = 1;  // a winner is you!
		instantVictory = FALSE;
	}
	
	if (num_ships)
	{
		BATTLE_STATE bs;

		GLOBAL (CurrentActivity) |= IN_BATTLE;
		battle_counter = MAKE_WORD (
				CountLinks (&race_q[0]),
				CountLinks (&race_q[1])
				);

		setupBattleInputOrder ();
#ifdef NETPLAY
		initBattleInputBuffers ();
#ifdef NETPLAY_CHECKSUM
		initChecksumBuffers ();
#endif  /* NETPLAY_CHECKSUM */
		battleFrameCount = 0;
		currentDeadSide = (COUNT)~0;
		setBattleStateConnections (&bs);
#endif  /* NETPLAY */

		if (!selectAllShips (num_ships)) {
			GLOBAL (CurrentActivity) |= CHECK_ABORT;
			goto AbortBattle;
		}

		BattleSong (TRUE);
		bs.NextTime = 0;
#ifdef NETPLAY
		initBattleStateDataConnections ();
		{
			bool allOk = negotiateReadyConnections (true, NetState_inBattle);
			if (!allOk) {
				GLOBAL (CurrentActivity) |= CHECK_ABORT;
				goto AbortBattle;
			}
		}
#endif  /* NETPLAY */
		bs.InputFunc = DoBattle;
		bs.first_time = (BOOLEAN)(LOBYTE (GLOBAL (CurrentActivity)) ==
				IN_HYPERSPACE);

		UnlockMutex (GraphicsLock);
		DoInput ((PVOID)&bs, FALSE);
		LockMutex (GraphicsLock);

AbortBattle:
		if (LOBYTE (GLOBAL (CurrentActivity)) == SUPER_MELEE &&
				(GLOBAL (CurrentActivity) & CHECK_ABORT))
		{
			// Do not return to the main menu when a game is aborted,
			// (just to the supermelee menu).
#ifdef NETPLAY
			UnlockMutex (GraphicsLock);
			waitResetConnections(NetState_inSetup);
					// A connection may already be in inSetup (set from
					// GetMeleeStarship). This is not a problem, although
					// it will generate a warning in debug mode.
			LockMutex (GraphicsLock);
#endif

			GLOBAL (CurrentActivity) &= ~CHECK_ABORT;
		}

#ifdef NETPLAY
		uninitBattleInputBuffers();
#ifdef NETPLAY_CHECKSUM
		uninitChecksumBuffers ();
#endif  /* NETPLAY_CHECKSUM */
		setBattleStateConnections (NULL);
#endif  /* NETPLAY */

		StopMusic ();
		StopSound ();
	}

	UninitShips ();
	FreeBattleSong ();

	UnlockMutex (GraphicsLock);
	
	return (BOOLEAN) (num_ships < 0);
}

