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
//	Put all global tate variables here.
//

#include <stdio.h>

#include "doomstat.hpp"


// Game Mode - identify IWAD as shareware, retail etc.
GameMode_t gamemode = GameMode_t::indetermined;
GameMission_t	gamemission = GameMission_t::doom;
GameVersion_t   gameversion = GameVersion_t::exe_strife_1_31;
const char *gamedescription;

// Set if homebrew PWAD stuff has been added.
boolean	modifiedgame;




