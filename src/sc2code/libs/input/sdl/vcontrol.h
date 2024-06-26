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

#ifndef _VCONTROL_H_
#define _VCONTROL_H_

#include "port.h"
#include "libs/uio.h"
#include SDL_INCLUDE(SDL.h)

/* Initialization routines */
void VControl_Init (void);
void VControl_Uninit (void);

/* Structures for representing actual VControl Inputs.  Returned by
   iterators and used to construct bindings. */

typedef enum {
	VCONTROL_KEY,
	VCONTROL_JOYAXIS,
	VCONTROL_JOYBUTTON,
	VCONTROL_JOYHAT,
	NUM_VCONTROL_GESTURES
} VCONTROL_GESTURE_TYPE;

typedef struct {
	VCONTROL_GESTURE_TYPE type;
	union {
		SDLKey key;
		struct { int port, index, polarity; } axis;
		struct { int port, index; } button;
		struct { int port, index; Uint8 dir; } hat;
	} gesture;
} VCONTROL_GESTURE;			

/* Control of bindings */
int  VControl_AddBinding (SDL_Event *e, int *target);
void VControl_RemoveBinding (SDL_Event *e, int *target);

int  VControl_AddGestureBinding (VCONTROL_GESTURE *g, int *target);
void VControl_RemoveGestureBinding (VCONTROL_GESTURE *g, int *target);

int  VControl_AddKeyBinding (SDLKey symbol, int *target);
void VControl_RemoveKeyBinding (SDLKey symbol, int *target);
int  VControl_AddJoyAxisBinding (int port, int axis, int polarity, int *target);
void VControl_RemoveJoyAxisBinding (int port, int axis, int polarity, int *target);
int  VControl_SetJoyThreshold (int port, int threshold);
int  VControl_AddJoyButtonBinding (int port, int button, int *target);
void VControl_RemoveJoyButtonBinding (int port, int button, int *target);
int  VControl_AddJoyHatBinding (int port, int which, Uint8 dir, int *target);
void VControl_RemoveJoyHatBinding (int port, int which, Uint8 dir, int *target);

void VControl_RemoveAllBindings (void);

/* The listener.  Routines besides HandleEvent may be used to 'fake' inputs without 
 * fabricating an SDL_Event. 
 */
void VControl_HandleEvent (const SDL_Event *e);
void VControl_ProcessKeyDown (SDLKey symbol);
void VControl_ProcessKeyUp (SDLKey symbol);
void VControl_ProcessJoyButtonDown (int port, int button);
void VControl_ProcessJoyButtonUp (int port, int button);
void VControl_ProcessJoyAxis (int port, int axis, int value);
void VControl_ProcessJoyHat (int port, int which, Uint8 value);

/* Force the input into the blank state.  For preventing "sticky" keys. */
void VControl_ResetInput (void);

/* Name control.  To provide a table of names and bindings, declare
 * a persistent, unchanging array of VControl_NameBinding and end it
 * with a {0, 0} entry.  Pass this array to VControl_RegisterNameTable.
 * Only one name table may be registered at a time; subsequent calls
 * replace the previous values. */

typedef struct vcontrol_namebinding {
	char *name;
	int *target;
} VControl_NameBinding;

void VControl_RegisterNameTable (VControl_NameBinding *table);

/* Version number control */
int VControl_GetConfigFileVersion (void);
void VControl_SetConfigFileVersion (int v);

/* Dump a configuration file corresponding to the current bindings and names. */
void VControl_Dump (uio_Stream *out);
void VControl_SaveFilename (uio_DirHandle *path, const char *fname);


/* Read a configuration file.  Returns number of errors encountered. */
int VControl_ReadConfiguration (uio_Stream *in);
int VControl_GetErrorCount (void);
int VControl_GetValidCount (void);


/* Iterator control.  Start an iteration with StartIter or
   StartIterByName; then call NextBindingName until it returns 0.  
   Produces gestures. */

void VControl_StartIter (int *target);
void VControl_StartIterByName (char *targetname);
int  VControl_NextBinding (VCONTROL_GESTURE *gesture);

/* Tracking the "last interesting event."  Used to poll to find new
   control keys. */

void VControl_ClearEvent (void);
int  VControl_GetLastEvent (SDL_Event *e);

int VControl_GetJoyAxis(int port, int axis);

#endif
