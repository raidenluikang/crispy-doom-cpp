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

#include <stdlib.h>
#include <string.h>

#include "doomtype.hpp"

#include "config.h"
#include "textscreen.hpp"

#include "doomtype.hpp"
#include "d_mode.hpp"
#include "d_iwad.hpp"
#include "i_system.hpp"
#include "m_argv.hpp"
#include "m_config.hpp"
#include "m_controls.hpp"
#include "m_misc.hpp"

#include "accessibility.hpp"
#include "compatibility.hpp"
#include "display.hpp"
#include "joystick.hpp"
#include "keyboard.hpp"
#include "mouse.hpp"
#include "multiplayer.hpp"
#include "sound.hpp"

#include "mode.hpp"

GameMission_t gamemission;
static const iwad_t **iwads;

typedef struct
{
    const char *label;
    GameMission_t mission;
    int mask;
    const char *name;
    const char *config_file;
    const char *extra_config_file;
    const char *executable;
} mission_config_t;

// Default mission to fall back on, if no IWADs are found at all:

#define DEFAULT_MISSION (&mission_configs[0])

static mission_config_t mission_configs[] =
{
    {
        "Doom",
        GameMission_t::doom,
        IWAD_MASK_DOOM,
        "doom",
        "default.cfg",
        PROGRAM_PREFIX "doom.cfg",
        PROGRAM_PREFIX "doom"
    },
    {
        "Heretic",
       GameMission_t:: heretic,
        IWAD_MASK_HERETIC,
        "heretic",
        "heretic.cfg",
        PROGRAM_PREFIX "heretic.cfg",
        PROGRAM_PREFIX "heretic"
    },
    {
        "Hexen",
        GameMission_t::hexen,
        IWAD_MASK_HEXEN,
        "hexen",
        "hexen.cfg",
        PROGRAM_PREFIX "hexen.cfg",
        PROGRAM_PREFIX "hexen"
    },
    {
        "Strife",
        GameMission_t::strife,
        IWAD_MASK_STRIFE,
        "strife",
        "strife.cfg",
        PROGRAM_PREFIX "strife.cfg",
        PROGRAM_PREFIX "strife"
    }
};

static GameSelectCallback game_selected_callback;

// Miscellaneous variables that aren't used in setup.

static int showMessages = 1;
static int screenblocks = 10;
static int detailLevel = 0;
static const char *savedir = nullptr;
static char *executable = nullptr;
static const char *game_title = "Doom";
static const char *back_flat = "F_PAVE01";
static int comport = 0;
static const char *nickname = nullptr;

static void BindMiscVariables(void)
{
    if (gamemission == GameMission_t::doom)
    {
        M_BindIntVariable("detaillevel",   &detailLevel);
        M_BindIntVariable("show_messages", &showMessages);
    }

    if (gamemission ==GameMission_t:: hexen)
    {
        M_BindStringVariable("savedir", &savedir);
        M_BindIntVariable("messageson", &showMessages);

        // Hexen has a variable to control the savegame directory
        // that is used.

        savedir = M_GetSaveGameDir("hexen.wad");

        // On Windows, hexndata\ is the default.

        if (!strcmp(savedir, ""))
        {
            //free((void*)savedir); //--> frees incorrect
            savedir = "hexndata" DIR_SEPARATOR_S;
        }
    }

    if (gamemission == GameMission_t::strife)
    {
        // Strife has a different default value than the other games
        screenblocks = 10;

        M_BindStringVariable("back_flat",   &back_flat);
        M_BindStringVariable("nickname",    &nickname);

        M_BindIntVariable("screensize",     &screenblocks);
        M_BindIntVariable("comport",        &comport);
    }
    else
    {
        M_BindIntVariable("screenblocks",   &screenblocks);
    }

}

//
// Initialise all configuration file bindings.
//

void InitBindings(void)
{
    M_ApplyPlatformDefaults();

    // Keyboard, mouse, joystick controls

    M_BindBaseControls();
    M_BindWeaponControls();
    M_BindMapControls();
    M_BindMenuControls();

    if (gamemission == GameMission_t::heretic || gamemission == GameMission_t::hexen)
    {
        M_BindHereticControls();
    }

    if (gamemission == GameMission_t::hexen)
    {
        M_BindHexenControls();
    }

    if (gamemission == GameMission_t::strife)
    {
        M_BindStrifeControls();
    }

    // All other variables

    BindAccessibilityVariables();
    BindCompatibilityVariables();
    BindDisplayVariables();
    BindJoystickVariables();
    BindKeyboardVariables();
    BindMouseVariables();
    BindSoundVariables();
    BindMiscVariables();
    BindMultiplayerVariables();
}

// Set the name of the executable program to run the game:

static void SetExecutable(mission_config_t *config)
{
    const char *extension;

    free(executable);

#ifdef _WIN32
    extension = ".exe";
#else
    extension = "";
#endif

    executable = M_StringJoin(config->executable, extension, nullptr);
}

static void SetMission(mission_config_t *config)
{
    iwads = D_FindAllIWADs(config->mask);
    gamemission = config->mission;
    SetExecutable(config);
    game_title = config->label;
    M_SetConfigFilenames(config->config_file, config->extra_config_file);
}

static mission_config_t *GetMissionForName(const char *name)
{
    int i;

    for (i=0; i<arrlen(mission_configs); ++i)
    {
        if (!strcmp(mission_configs[i].name, name))
        {
            return &mission_configs[i];
        }
    }

    return nullptr;
}

// Check the name of the executable.  If it contains one of the game
// names (eg. chocolate-hexen-setup.exe) then use that game.

static boolean CheckExecutableName(GameSelectCallback callback)
{
    mission_config_t *config;
    const char *exe_name;
    int i;

    exe_name = M_GetExecutableName();

    for (i=0; i<arrlen(mission_configs); ++i)
    {
        config = &mission_configs[i];

        if (strstr(exe_name, config->name) != nullptr)
        {
            SetMission(config);
            callback();
            return true;
        }
    }

    return false;
}

static void GameSelected(TXT_UNCAST_ARG(widget), TXT_UNCAST_ARG(config))
{
    TXT_CAST_ARG(mission_config_t, config);

    SetMission(config);
    game_selected_callback();
}

static void OpenGameSelectDialog(GameSelectCallback callback)
{
    mission_config_t *mission = nullptr;
    txt_window_t *window;
    const iwad_t **iwads;
    int num_games;
    int i;

    window = TXT_NewWindow("Select game");

    TXT_AddWidget(window, TXT_NewLabel("Select a game to configure:\n"));
    num_games = 0;

    // Add a button for each game.

    for (i=0; i<arrlen(mission_configs); ++i)
    {
        // Do we have any IWADs for this game installed?
        // If so, add a button.

        iwads = D_FindAllIWADs(mission_configs[i].mask);

        if (iwads[0] != nullptr)
        {
            mission = &mission_configs[i];
            TXT_AddWidget(window, TXT_NewButton2(mission_configs[i].label,
                                                 GameSelected,
                                                 &mission_configs[i]));
            ++num_games;
        }

        free(iwads);
    }

    TXT_AddWidget(window, TXT_NewStrut(0, 1));

    // No IWADs found at all?  Fall back to doom, then.

    if (num_games == 0)
    {
        TXT_CloseWindow(window);
        SetMission(DEFAULT_MISSION);
        callback();
        return;
    }

    // Only one game? Use that game, and don't bother with a dialog.

    if (num_games == 1)
    {
        TXT_CloseWindow(window);
        SetMission(mission);
        callback();
        return;
    }

    game_selected_callback = callback;
}

void SetupMission(GameSelectCallback callback)
{
    mission_config_t *config;
    const char *mission_name;
    int p;

    //!
    // @arg <game>
    //
    // Specify the game to configure the settings for.  Valid
    // values are 'doom', 'heretic', 'hexen' and 'strife'.
    //

    p = M_CheckParm("-game");

    if (p > 0)
    {
        mission_name = myargv[p + 1];

        config = GetMissionForName(mission_name);

        if (config == nullptr)
        {
            I_Error("Invalid parameter - '%s'", mission_name);
        }

        SetMission(config);
        callback();
    }
    else if (!CheckExecutableName(callback))
    {
        OpenGameSelectDialog(callback);
    }
}

const char *GetExecutableName(void)
{
    return executable;
}

const char *GetGameTitle(void)
{
    return game_title;
}

const iwad_t **GetIwads(void)
{
    return iwads;
}

