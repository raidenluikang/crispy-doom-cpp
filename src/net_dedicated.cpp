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
// Dedicated server code.
// 

#include <stdio.h>
#include <stdlib.h>

#include "doomtype.hpp"

#include "i_system.hpp"
#include "i_timer.hpp"

#include "m_argv.hpp"

#include "net_common.hpp"
#include "net_sdl.hpp"
#include "net_server.hpp"

// 
// People can become confused about how dedicated servers work.  Game
// options are specified to the controlling player who is the first to
// join a game.  Bomb out with an error message if game options are
// specified to a dedicated server.
//

static const char *not_dedicated_options[] =
{
    "-deh", "-iwad", "-cdrom", "-gameversion", "-nomonsters", "-respawn",
    "-fast", "-altdeath", "-deathmatch", "-turbo", "-merge", "-af", "-as",
    "-aa", "-file", "-wart", "-skill", "-episode", "-timer", "-avg", "-warp",
    "-loadgame", "-longtics", "-extratics", "-dup", "-shorttics", nullptr,
};

static void CheckForClientOptions(void)
{
    int i;

    for (i=0; not_dedicated_options[i] != nullptr; ++i)
    {
        if (M_CheckParm(not_dedicated_options[i]) > 0)
        {
            I_Error("The command line parameter '%s' was specified to a "
                    "dedicated server.\nGame parameters should be specified "
                    "to the first player to join a server, \nnot to the "
                    "server itself. ",
                    not_dedicated_options[i]);
        }
    }
}

void NET_DedicatedServer(void)
{
    CheckForClientOptions();

    NET_OpenLog();
    NET_SV_Init();
    NET_SV_AddModule(&net_sdl_module);
    NET_SV_RegisterWithMaster();

    while (true)
    {
        NET_SV_Run();
        // TODO: Block on socket instead of polling.
        I_Sleep(1);
    }
}

