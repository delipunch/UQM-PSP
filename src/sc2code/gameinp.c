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

#include "controls.h"
#include "init.h"
#include "planets/planets.h"
#include "settings.h"
#include "sounds.h"
#include "libs/inplib.h"
#include "libs/timelib.h"
#include "libs/threadlib.h"

#include "libs/input/sdl/vcontrol.h"
BATTLE_INPUT_STATE GetDirectionalJoystickInput(int direction, int player);

battle_summary_func ComputerInput, HumanInput[NUM_PLAYERS], NetworkInput;
battle_summary_func PlayerInput[NUM_PLAYERS];

#define ACCELERATION_INCREMENT (ONE_SECOND / 12)
#define MENU_REPEAT_DELAY (ONE_SECOND >> 1)


typedef struct
{
	BOOLEAN (*InputFunc) (PVOID pInputState);
	COUNT MenuRepeatDelay;
} INPUT_STATE_DESC;
typedef INPUT_STATE_DESC *PINPUT_STATE_DESC;

/* These static variables are the values that are set by the controllers. */

typedef struct 
{
	DWORD key [NUM_TEMPLATES][NUM_KEYS];
	DWORD menu [NUM_MENU_KEYS];
} MENU_ANNOTATIONS;


CONTROL_TEMPLATE PlayerOne, PlayerTwo;
CONTROLLER_INPUT_STATE CurrentInputState, PulsedInputState;
static CONTROLLER_INPUT_STATE CachedInputState, OldInputState;
static MENU_ANNOTATIONS RepeatDelays, Times;
static DWORD GestaltRepeatDelay, GestaltTime;
static BOOLEAN OldGestalt, CachedGestalt;
static DWORD _max_accel, _min_accel, _step_accel;
static BOOLEAN _gestalt_keys;
int ExitState;

static MENU_SOUND_FLAGS sound_0, sound_1;

volatile CONTROLLER_INPUT_STATE ImmediateInputState;

static void
_clear_menu_state (void)
{
	int i, j;
	for (i = 0; i < NUM_TEMPLATES; i++)
	{
		for (j = 0; j < NUM_KEYS; j++)
		{
			PulsedInputState.key[i][j] = 0;
			CachedInputState.key[i][j] = 0;
		}
	}
	for (i = 0; i < NUM_MENU_KEYS; i++)
	{
		PulsedInputState.menu[i] = 0;
		CachedInputState.menu[i] = 0;
	}		
	CachedGestalt = FALSE;
}

void
ResetKeyRepeat (void)
{
	DWORD initTime = GetTimeCounter ();
	int i, j;
	for (i = 0; i < NUM_TEMPLATES; i++)
	{
		for (j = 0; j < NUM_KEYS; j++)
		{
			RepeatDelays.key[i][j] = _max_accel;
			Times.key[i][j] = initTime;
		}
	}
	for (i = 0; i < NUM_MENU_KEYS; i++)
	{
		RepeatDelays.menu[i] = _max_accel;
		Times.menu[i] = initTime;
	}
	GestaltRepeatDelay = _max_accel;
	GestaltTime = initTime;
}

static void
_check_for_pulse (int *current, int *cached, int *old, DWORD *accel, DWORD *newtime, DWORD *oldtime)
{
	if (*cached && *old)
	{
		if (*newtime - *oldtime < *accel)
		{
			*current = 0;
		} 
		else
		{
			*current = *cached;
			if (*accel > _min_accel)
				*accel -= _step_accel;
			if (*accel < _min_accel)
				*accel = _min_accel;
			*oldtime = *newtime;
		}
	}
	else
	{
		*current = *cached;
		*oldtime = *newtime;
		*accel = _max_accel;
	}
}

/* BUG: If a key from a currently unused control template is held,
 * this will affect the gestalt repeat rate.  This isn't a problem
 * *yet*, but it will be once the user gets to define control
 * templates on his own --McM */
static void
_check_gestalt (DWORD NewTime)
{
	BOOLEAN CurrentGestalt;
	int i,j;
	OldGestalt = CachedGestalt;

	CachedGestalt = 0;
	CurrentGestalt = 0;
	for (i = 0; i < NUM_TEMPLATES; i++)
	{
		for (j = 0; j < NUM_KEYS; j++)
		{
			CachedGestalt |= ImmediateInputState.key[i][j];
			CurrentGestalt |= PulsedInputState.key[i][j];
		}
	}
	for (i = 0; i < NUM_MENU_KEYS; i++)
	{
		CachedGestalt |= ImmediateInputState.menu[i];
		CurrentGestalt |= PulsedInputState.menu[i];
	}

	if (OldGestalt && CachedGestalt)
	{
		if (NewTime - GestaltTime < GestaltRepeatDelay)
		{
			for (i = 0; i < NUM_TEMPLATES; i++)
			{
				for (j = 0; j < NUM_KEYS; j++)
				{
					PulsedInputState.key[i][j] = 0;
				}
			}
			for (i = 0; i < NUM_MENU_KEYS; i++)
			{
				PulsedInputState.menu[i] = 0;
			}
		}
		else
		{
			for (i = 0; i < NUM_TEMPLATES; i++)
			{
				for (j = 0; j < NUM_KEYS; j++)
				{
					PulsedInputState.key[i][j] = CachedInputState.key[i][j];
				}
			}
			for (i = 0; i < NUM_MENU_KEYS; i++)
			{
				PulsedInputState.menu[i] = CachedInputState.menu[i];
			}
			if (GestaltRepeatDelay > _min_accel)
				GestaltRepeatDelay -= _step_accel;
			if (GestaltRepeatDelay < _min_accel)
				GestaltRepeatDelay = _min_accel;
			GestaltTime = NewTime;
		}
	}
	else
	{
		for (i = 0; i < NUM_TEMPLATES; i++)
		{
			for (j = 0; j < NUM_KEYS; j++)
			{
				PulsedInputState.key[i][j] = CachedInputState.key[i][j];
			}
		}
		for (i = 0; i < NUM_MENU_KEYS; i++)
		{
			PulsedInputState.menu[i] = CachedInputState.menu[i];
		}
		GestaltTime = NewTime;
		GestaltRepeatDelay = _max_accel;
	}
}

void
UpdateInputState (void)
{
	DWORD NewTime;
	/* First, if the game is, in fact, paused, we stall until
	 * unpaused.  Every thread with control over game logic calls
	 * UpdateInputState routinely, so we handle pause and exit
	 * state updates here. */

	if (GamePaused)
		PauseGame ();

	if (ExitRequested)
	{
		ConfirmExit ();
	}

	CurrentInputState = ImmediateInputState;
	OldInputState = CachedInputState;
	CachedInputState = ImmediateInputState;
	NewTime = GetTimeCounter ();
	if (_gestalt_keys)
	{
		_check_gestalt (NewTime);
	}
	else
	{
		int i, j;
		for (i = 0; i < NUM_TEMPLATES; i++)
		{
			for (j = 0; j < NUM_KEYS; j++)
			{
				_check_for_pulse (&PulsedInputState.key[i][j],
						&CachedInputState.key[i][j], &OldInputState.key[i][j],
						&RepeatDelays.key[i][j], &NewTime, &Times.key[i][j]);
			}
		}
		for (i = 0; i < NUM_MENU_KEYS; i++)
		{
			_check_for_pulse (&PulsedInputState.menu[i],
					&CachedInputState.menu[i], &OldInputState.menu[i],
					&RepeatDelays.menu[i], &NewTime, &Times.menu[i]);
		}
	}

	if (CurrentInputState.menu[KEY_PAUSE])
		GamePaused = TRUE;

	if (CurrentInputState.menu[KEY_EXIT])
		ExitRequested = TRUE;
}


void
SetMenuRepeatDelay (DWORD min, DWORD max, DWORD step, BOOLEAN gestalt)
{
	_min_accel = min;
	_max_accel = max;
	_step_accel = step;
	_gestalt_keys = gestalt;
	_clear_menu_state ();
	ResetKeyRepeat ();
}

void
SetDefaultMenuRepeatDelay ()
{
	_min_accel = ACCELERATION_INCREMENT;
	_max_accel = MENU_REPEAT_DELAY;
	_step_accel = ACCELERATION_INCREMENT;
	_gestalt_keys = FALSE;
	_clear_menu_state ();
	ResetKeyRepeat ();
}

void
FlushInputState (void)
{
	_clear_menu_state ();
}

void
DoInput (PVOID pInputState, BOOLEAN resetInput)
{
	SetMenuRepeatDelay (ACCELERATION_INCREMENT, MENU_REPEAT_DELAY, ACCELERATION_INCREMENT, FALSE);
	if (resetInput)
	{
		TFB_ResetControls ();
	}
	do
	{
		MENU_SOUND_FLAGS input;
		TaskSwitch ();

		UpdateInputState ();

#if DEMO_MODE || CREATE_JOURNAL
		if (ArrowInput != DemoInput)
#endif
		{
#if CREATE_JOURNAL
			JournalInput (InputState);
#endif /* CREATE_JOURNAL */
		}

		if (CurrentInputState.menu[KEY_EXIT])
			ExitState = ConfirmExit ();

		input = MENU_SOUND_NONE;
		if (PulsedInputState.menu[KEY_MENU_UP]) input |= MENU_SOUND_UP;
		if (PulsedInputState.menu[KEY_MENU_DOWN]) input |= MENU_SOUND_DOWN;
		if (PulsedInputState.menu[KEY_MENU_LEFT]) input |= MENU_SOUND_LEFT;
		if (PulsedInputState.menu[KEY_MENU_RIGHT]) input |= MENU_SOUND_RIGHT;
		if (PulsedInputState.menu[KEY_MENU_SELECT]) input |= MENU_SOUND_SELECT;
		if (PulsedInputState.menu[KEY_MENU_CANCEL]) input |= MENU_SOUND_CANCEL;
		if (PulsedInputState.menu[KEY_MENU_SPECIAL]) input |= MENU_SOUND_SPECIAL;
		if (PulsedInputState.menu[KEY_MENU_PAGE_UP]) input |= MENU_SOUND_PAGEUP;
		if (PulsedInputState.menu[KEY_MENU_PAGE_DOWN]) input |= MENU_SOUND_PAGEDOWN;
		if (PulsedInputState.menu[KEY_MENU_DELETE]) input |= MENU_SOUND_DELETE;
		if (PulsedInputState.menu[KEY_MENU_BACKSPACE]) input |= MENU_SOUND_DELETE;
			
		if (MenuSounds
				&& (pSolarSysState == 0
						/* see if in menu */
				|| pSolarSysState->MenuState.CurState
				|| pSolarSysState->MenuState.Initialized > 2)
		                && (input & (sound_0 | sound_1))
#ifdef NEVER
				&& !PLRPlaying ((MUSIC_REF)~0)
#endif /* NEVER */
				)
		{
			SOUND S;

			S = MenuSounds;
			if (input & sound_1)
				S = SetAbsSoundIndex (S, MENU_SOUND_SUCCESS);

			PlaySoundEffect (S, 0, NotPositional (), NULL, 0);
		}
	} while ((*((PINPUT_STATE_DESC)pInputState)->InputFunc)
			(pInputState));
	if (resetInput)
	{
		TFB_ResetControls ();
	}

}

void
SetMenuSounds (MENU_SOUND_FLAGS s0, MENU_SOUND_FLAGS s1)
{
	sound_0 = s0;
	sound_1 = s1;
}

void
GetMenuSounds (MENU_SOUND_FLAGS *s0, MENU_SOUND_FLAGS *s1)
{
	*s0 = sound_0;
	*s1 = sound_1;
}

/* These can really be refactored */

BATTLE_INPUT_STATE
p1_combat_summary (COUNT player, STARSHIPPTR StarShipPtr)
{
	BATTLE_INPUT_STATE InputState = 0;
	if (CurrentInputState.key[PlayerOne][KEY_UP])
		InputState |= BATTLE_THRUST;
	if (CurrentInputState.key[PlayerOne][KEY_LEFT])
		InputState |= BATTLE_LEFT;
	if (CurrentInputState.key[PlayerOne][KEY_RIGHT])
		InputState |= BATTLE_RIGHT;
	if (CurrentInputState.key[PlayerOne][KEY_WEAPON])
		InputState |= BATTLE_WEAPON;
	if (CurrentInputState.key[PlayerOne][KEY_SPECIAL])
		InputState |= BATTLE_SPECIAL;
	if (CurrentInputState.key[PlayerOne][KEY_ESCAPE])
		InputState |= BATTLE_ESCAPE;
	if (CurrentInputState.key[PlayerOne][KEY_DOWN])
		InputState |= BATTLE_DOWN;

	(void) player;
	(void) StarShipPtr;
	return InputState;
}

BATTLE_INPUT_STATE
p2_combat_summary (COUNT player, STARSHIPPTR StarShipPtr)
{
	BATTLE_INPUT_STATE InputState = 0;
	if (CurrentInputState.key[PlayerTwo][KEY_UP])
		InputState |= BATTLE_THRUST;
	if (CurrentInputState.key[PlayerTwo][KEY_LEFT])
		InputState |= BATTLE_LEFT;
	if (CurrentInputState.key[PlayerTwo][KEY_RIGHT])
		InputState |= BATTLE_RIGHT;
	if (CurrentInputState.key[PlayerTwo][KEY_WEAPON])
		InputState |= BATTLE_WEAPON;
	if (CurrentInputState.key[PlayerTwo][KEY_SPECIAL])
		InputState |= BATTLE_SPECIAL;
	if (CurrentInputState.key[PlayerTwo][KEY_DOWN])
		InputState |= BATTLE_DOWN;

	(void) player;
	(void) StarShipPtr;
	return InputState;
}

BOOLEAN
AnyButtonPress (BOOLEAN CheckSpecial)
{
	int i, j;
	(void) CheckSpecial;   // Ignored
	UpdateInputState ();
	for (i = 0; i < NUM_TEMPLATES; i++)
	{
		for (j = 0; j < NUM_KEYS; j++)
		{
			if (CurrentInputState.key[i][j])
				return TRUE;
		}
	}
	for (i = 0; i < NUM_MENU_KEYS; i++)
	{
		if (CurrentInputState.menu[i])
			return TRUE;
	}
	return FALSE;
}

BOOLEAN
ConfirmExit (void)
{
	DWORD old_max_accel, old_min_accel, old_step_accel;
	BOOLEAN old_gestalt_keys, result;

	old_max_accel = _max_accel;
	old_min_accel = _min_accel;
	old_step_accel = _step_accel;
	old_gestalt_keys = _gestalt_keys;
		
	SetDefaultMenuRepeatDelay ();
		
	result = DoConfirmExit ();
	
	SetMenuRepeatDelay (old_min_accel, old_max_accel, old_step_accel, old_gestalt_keys);
	return result;
}

// directional joystick input code, taken from the android port of UQM
// https://github.com/pelya/commandergenius
// https://libsdl-android.sourceforge.io/

// Fast arctan2, returns angle in radians as integer, with fractional part in lower 16 bits
// Stolen from http://www.dspguru.com/dsp/tricks/fixed-point-atan2-with-self-normalization , precision is said to be 0.07 rads

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
enum { atan2i_coeff_1 = ((int)(M_PI*65536.0/4)), atan2i_coeff_2 = (3*atan2i_coeff_1), atan2i_PI = (int)(M_PI * 65536.0), SHIP_DIRECTIONS = 16 };

static inline int atan2i(int y, int x)
{
   int angle;
   int abs_y = abs(y);
   if( abs_y == 0 )
      abs_y = 1;
   if (x>=0)
   {
      angle = atan2i_coeff_1 - atan2i_coeff_1 * (x - abs_y) / (x + abs_y);
   }
   else
   {
      angle = atan2i_coeff_2 - atan2i_coeff_1 * (x + abs_y) / (abs_y - x);
   }
   if (y < 0)
      return(-angle);     // negate if in quad III or IV
   else
      return(angle);
}

BATTLE_INPUT_STATE GetDirectionalJoystickInput(int direction, int player)
{
	BATTLE_INPUT_STATE InputState = 0;
	static BOOLEAN JoystickThrust[NUM_PLAYERS] = { FALSE, FALSE };
	static BOOLEAN JoystickTapFlag[NUM_PLAYERS] = { FALSE, FALSE };
	static TimeCount JoystickTapTime[NUM_PLAYERS] = { 0, 0 };

	/* if (CurrentInputState.key[PlayerControls[player]][KEY_THRUST]) */
	/* 	InputState |= BATTLE_THRUST; */

	/* if( VControl_GetJoysticksAmount() <= 0 ) */
	/* { */
	/* 	if (CurrentInputState.key[PlayerControls[player]][KEY_LEFT]) */
	/* 		InputState |= BATTLE_LEFT; */
	/* 	if (CurrentInputState.key[PlayerControls[player]][KEY_RIGHT]) */
	/* 		InputState |= BATTLE_RIGHT; */
	/* 	if (CurrentInputState.key[PlayerControls[player]][KEY_UP]) */
	/* 		InputState |= BATTLE_THRUST; */
	/* 	return InputState; */
	/* } */

	int axisX = VControl_GetJoyAxis(0, player * 2), axisY = VControl_GetJoyAxis(0, player * 2 + 1);
	//int axisX = SDL_JoystickGetAxis(&joysticks[0].stick, player * 2), axisY = SDL_JoystickGetAxis(&joysticks[0].stick, player * 2 + 1);
	
	if( axisX == 0 && axisY == 0 )
	{
		// Some basic gamepad input support
		axisX = VControl_GetJoyAxis(2, player * 2);
		axisY = VControl_GetJoyAxis(2, player * 2 + 1);
		if( abs( axisX ) > 5000 || abs( axisY ) > 5000 ) // Deadspot at the center
		{
			//if( !JoystickTapFlag[player] )
			//	TFB_SetOnScreenKeyboard_HiddenPermanently (); // Gamepad used - hide on-screen keys
			JoystickTapFlag[player] = TRUE;
			JoystickThrust[player] = FALSE;
			// Turning thrust with joystick is uncomfortable
			//if( abs( axisX ) > 25000 || abs( axisY ) > 25000 )
			//	JoystickThrust[player] = TRUE;
		}
		else
		{
			axisX = 0;
			axisY = 0;
		}
	}

	/*if( axisX == 0 && axisY == 0 )
	{
		// Process keyboard input only when joystick is not used
		if (CurrentInputState.key[PlayerControls[player]][KEY_LEFT])
			InputState |= BATTLE_LEFT;
		if (CurrentInputState.key[PlayerControls[player]][KEY_RIGHT])
			InputState |= BATTLE_RIGHT;
		if (CurrentInputState.key[PlayerControls[player]][KEY_UP])
			InputState |= BATTLE_THRUST;
	}*/

	/*if( !optDirectionalJoystick )
	{
		if( player == 1 )
		{
			axisX = - axisX;
			axisY = - axisY;
		}
		if( axisX < -10000 )
			InputState |= BATTLE_LEFT;
		if( axisX > 10000 )
			InputState |= BATTLE_RIGHT;
		if( axisY < 0 )
			InputState |= BATTLE_THRUST;
		return InputState;
	}*/

	if( axisX != 0 || axisY != 0 )
	{
		int angle = atan2i(axisY, axisX), diff;
		// Convert it to 16 directions used by Melee
		angle += atan2i_PI / SHIP_DIRECTIONS;
		if( angle < 0 )
			angle += atan2i_PI * 2;
		if( angle > atan2i_PI * 2 )
			angle -= atan2i_PI * 2;
		angle = angle * SHIP_DIRECTIONS / atan2i_PI / 2;

		diff = angle - direction - SHIP_DIRECTIONS / 4;
		while( diff >= SHIP_DIRECTIONS )
			diff -= SHIP_DIRECTIONS;
		while( diff < 0 )
			diff += SHIP_DIRECTIONS;

		if( diff < SHIP_DIRECTIONS / 2 )
			InputState |= BATTLE_LEFT;
		if( diff > SHIP_DIRECTIONS / 2 )
			InputState |= BATTLE_RIGHT;

		if( !JoystickTapFlag[player] )
		{
			JoystickTapFlag[player] = TRUE;
			if( GetTimeCounter() < JoystickTapTime[player] + ONE_SECOND )
				JoystickThrust[player] = !JoystickThrust[player];
			else
				JoystickThrust[player] = TRUE;
		}
		//For now, we dont need the below. Maybe when I add this to navigation for the lander or in space 
		/*if( JoystickThrust[player] )
			InputState |= BATTLE_THRUST;
		*/
	}
	else
	{
		if( JoystickTapFlag[player] )
		{
			JoystickTapFlag[player] = FALSE;
			JoystickTapTime[player] = GetTimeCounter();
		}
	}
	return InputState;
}