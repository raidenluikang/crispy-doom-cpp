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
//
// DESCRIPTION:
//   Functions and definitions relating to the game type and operational
//   mode.
//

#include "doomtype.hpp"
#include "d_mode.hpp"

// Valid game mode/mission combinations, with the number of
// episodes/maps for each.

static constexpr struct
{
    GameMission_t mission;
    GameMode_t mode;
    int episode;
    int map;
} valid_modes[] = {
    { GameMission_t::pack_chex, GameMode_t::retail,     1, 5 },
    { GameMission_t::doom,      GameMode_t::shareware,  1, 9 },
    { GameMission_t::doom,      GameMode_t::registered, 3, 9 },
    { GameMission_t::doom,      GameMode_t::retail,     4, 9 },
    { GameMission_t::doom2,     GameMode_t::commercial, 1, 32 },
    { GameMission_t::pack_tnt,  GameMode_t::commercial, 1, 32 },
    { GameMission_t::pack_plut, GameMode_t::commercial, 1, 32 },
    { GameMission_t::pack_hacx, GameMode_t::commercial, 1, 32 },
    { GameMission_t::pack_nerve, GameMode_t::commercial, 1, 9 },
    { GameMission_t::pack_master, GameMode_t::commercial, 1, 21 },
    { GameMission_t::heretic,   GameMode_t::shareware,  1, 9 },
    { GameMission_t::heretic,   GameMode_t::registered, 3, 9 },
    { GameMission_t::heretic,   GameMode_t::retail,     5, 9 },
    { GameMission_t::hexen,     GameMode_t::commercial, 1, 60 },
    { GameMission_t::strife,    GameMode_t::commercial, 1, 34 },
};

// Check that a gamemode+gamemission received over the network is valid.

boolean D_ValidGameMode(GameMission_t mission, GameMode_t mode)
{
    for (size_t i=0; i<arrlen(valid_modes); ++i)
    {
        if (valid_modes[i].mode == mode && valid_modes[i].mission == mission)
        {
            return true;
        }
    }

    return false;
}

boolean D_ValidEpisodeMap(GameMission_t mission, GameMode_t mode,
                          int episode, int map)
{
    // Hacks for Heretic secret episodes

    if (mission == GameMission_t::heretic)
    {
        if (mode == GameMode_t::retail && episode == 6)
        {
            return map >= 1 && map <= 3;
        }
        else if (mode == GameMode_t::registered && episode == 4)
        {
            return map == 1;
        }
    }

    // Find the table entry for this mission/mode combination.

    for (size_t i=0; i<arrlen(valid_modes); ++i) 
    {
        if (mission == valid_modes[i].mission
         && mode == valid_modes[i].mode)
        {
            return episode >= 1 && episode <= valid_modes[i].episode
                && map >= 1 && map <= valid_modes[i].map;
        }
    }

    // Unknown mode/mission combination

    return false;
}

// Get the number of valid episodes for the specified mission/mode.

int D_GetNumEpisodes(GameMission_t mission, GameMode_t mode)
{
    int episode;

    episode = 1;

    while (D_ValidEpisodeMap(mission, mode, episode, 1))
    {
        ++episode;
    }

    return episode - 1;
}

// Table of valid versions

static constexpr struct {
    GameMission_t mission;
    GameVersion_t version;
} valid_versions[] = {
    { GameMission_t::doom,     GameVersion_t::exe_doom_1_2 },
    { GameMission_t::doom,     GameVersion_t::exe_doom_1_666 },
    { GameMission_t::doom,     GameVersion_t::exe_doom_1_7 },
    { GameMission_t::doom,     GameVersion_t::exe_doom_1_8 },
    { GameMission_t::doom,     GameVersion_t::exe_doom_1_9 },
    { GameMission_t::doom,     GameVersion_t::exe_hacx },
    { GameMission_t::doom,     GameVersion_t::exe_ultimate },
    { GameMission_t::doom,     GameVersion_t::exe_final },
    { GameMission_t::doom,     GameVersion_t::exe_final2 },
    { GameMission_t::doom,     GameVersion_t::exe_chex },
    { GameMission_t::heretic,  GameVersion_t::exe_heretic_1_3 },
    { GameMission_t::hexen,    GameVersion_t::exe_hexen_1_1 },
    { GameMission_t::strife,   GameVersion_t::exe_strife_1_2 },
    { GameMission_t::strife,   GameVersion_t::exe_strife_1_31 },
};

boolean D_ValidGameVersion(GameMission_t mission, GameVersion_t version)
{
    // All Doom variants can use the Doom versions.

    if (mission == GameMission_t::doom2 || mission == GameMission_t::pack_plut || mission == GameMission_t::pack_tnt
     || mission == GameMission_t::pack_hacx || mission == GameMission_t::pack_chex
     || mission == GameMission_t::pack_nerve || mission == GameMission_t::pack_master)
    {
        mission = GameMission_t::doom;
    }

    for (size_t i=0; i<arrlen(valid_versions); ++i) 
    {
        if (valid_versions[i].mission == mission 
         && valid_versions[i].version == version)
        {
            return true;
        }
    }

    return false;
}

// Does this mission type use ExMy form, rather than MAPxy form?

boolean D_IsEpisodeMap(GameMission_t mission)
{
    switch (mission)
    {
        case GameMission_t::doom:
        case GameMission_t::heretic:
        case GameMission_t::pack_chex:
            return true;

        case GameMission_t::none:
        case GameMission_t::hexen:
        case GameMission_t::doom2:
        case GameMission_t::pack_hacx:
        case GameMission_t::pack_tnt:
        case GameMission_t::pack_plut:
        case GameMission_t::pack_nerve:
        case GameMission_t::pack_master:
        case GameMission_t::strife:
        default:
            return false;
    }
}

const char *D_GameMissionString(GameMission_t mission)
{
    switch (mission)
    {
        case GameMission_t::none:
        default:
            return "none";
        case GameMission_t::doom:
            return "doom";
        case GameMission_t::doom2:
            return "doom2";
        case GameMission_t::pack_tnt:
            return "tnt";
        case GameMission_t::pack_plut:
            return "plutonia";
        case GameMission_t::pack_hacx:
            return "hacx";
        case GameMission_t::pack_chex:
            return "chex";
        case GameMission_t::heretic:
            return "heretic";
        case GameMission_t::hexen:
            return "hexen";
        case GameMission_t::strife:
            return "strife";
    }
    return "<unknown>";
}

const char *D_GameModeString(GameMode_t mode)
{
    switch (mode)
    {
        case GameMode_t::shareware:
            return "shareware";
        case GameMode_t::registered:
            return "registered";
        case GameMode_t::commercial:
            return "commercial";
        case GameMode_t::retail:
            return "retail";
        case GameMode_t::indetermined:
        default:
            return "unknown";
    }
}

