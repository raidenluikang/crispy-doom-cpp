//
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
//     Find IWAD and initialize according to IWAD type.
//


#ifndef __D_IWAD__
#define __D_IWAD__

#include "d_mode.hpp"

#define IWAD_MASK_DOOM    ((1 << static_cast<int>(GameMission_t::doom))           \
                         | (1 << static_cast<int>(GameMission_t::doom2))          \
                         | (1 << static_cast<int>(GameMission_t::pack_tnt))       \
                         | (1 << static_cast<int>(GameMission_t::pack_plut))      \
                         | (1 << static_cast<int>(GameMission_t::pack_chex))      \
                         | (1 << static_cast<int>(GameMission_t::pack_hacx)))


#define IWAD_MASK_HERETIC (1 << static_cast<int>(GameMission_t::heretic))
#define IWAD_MASK_HEXEN   (1 << static_cast<int>(GameMission_t::hexen))
#define IWAD_MASK_STRIFE  (1 << static_cast<int>(GameMission_t::strife))

struct iwad_t
{
    const char *name;
    GameMission_t mission;
    GameMode_t mode;
    const char *description;
} ;

boolean D_IsIWADName(const char *name);
char *D_FindWADByName(const char *filename);
char *D_TryFindWADByName(const char *filename);
char *D_FindIWAD(int mask, GameMission_t *mission);
const iwad_t **D_FindAllIWADs(int mask);
const char *D_SaveGameIWADName(GameMission_t gamemission, GameVariant_t gamevariant);
const char *D_SuggestIWADName(GameMission_t mission, GameMode_t mode);
const char *D_SuggestGameName(GameMission_t mission, GameMode_t mode);
void D_CheckCorrectIWAD(GameMission_t mission);

#endif

