//
// Copyright(C) 1993-1996 Id Software, Inc.
// Copyright(C) 2005-2014 Simon Howard
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// DESCRIPTION:
//  AutoMap module.
//

#ifndef __AMMAP_H__
#define __AMMAP_H__

#include "d_event.hpp"
#include "m_cheat.hpp"

// Used by ST StatusBar stuff.
#define AM_MSGHEADER ((static_cast<int>('a')<<24) + ( static_cast<int>('m')<<16))
#define AM_MSGENTERED (AM_MSGHEADER | ( static_cast<int>('e')<<8))
#define AM_MSGEXITED (AM_MSGHEADER | ( static_cast<int>('x')<<8))


// Called by main loop.
boolean AM_Responder (event_t* ev);

// Called by main loop.
void AM_Ticker (void);

// Called by main loop,
// called instead of view drawer if automap active.
void AM_Drawer (void);

// Called to force the automap to quit
// if the level is completed while it is up.
void AM_Stop (void);

// [crispy] re-init in G_Responder on toggling spy mode (F12)
void AM_initVariables (void);

extern cheatseq_t cheat_amap;


#endif
