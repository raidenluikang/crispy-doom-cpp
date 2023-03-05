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
//	Game completion, final screen animation.
//


#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

// Functions.
#include "deh_main.hpp"
#include "i_system.hpp"
#include "i_swap.hpp"
#include "z_zone.hpp"
#include "v_video.hpp"
#include "w_wad.hpp"
#include "s_sound.hpp"

// Data.
#include "d_main.hpp"
#include "dstrings.hpp"
#include "sounds.hpp"

#include "doomstat.hpp"
#include "r_state.hpp"
#include "m_controls.hpp" // [crispy] key_*
#include "m_misc.hpp" // [crispy] M_StringDuplicate()
#include "m_random.hpp" // [crispy] Crispy_Random()

typedef enum
{
    F_STAGE_TEXT,
    F_STAGE_ARTSCREEN,
    F_STAGE_CAST,
} finalestage_t;

// ?
//#include "doomstat.hpp"
//#include "r_local.hpp"
//#include "f_finale.hpp"

// Stage of animation:
finalestage_t finalestage;

unsigned int finalecount;

#define	TEXTSPEED	3
#define	TEXTWAIT	250

 struct textscreen_t
{
    GameMission_t mission;
    int episode, level;
    const char *background;
    const char *text;
} ;

static textscreen_t textscreens[] =
{
    { GameMission_t::doom,      1, 8,  "FLOOR4_8",  E1TEXT},
    { GameMission_t::doom,      2, 8,  "SFLR6_1",   E2TEXT},
    { GameMission_t::doom,      3, 8,  "MFLR8_4",   E3TEXT},
    { GameMission_t::doom,      4, 8,  "MFLR8_3",   E4TEXT},
    { GameMission_t::doom,      5, 8,  "FLOOR7_2",  E5TEXT}, // [crispy] Sigil

    { GameMission_t::doom2,     1, 6,  "SLIME16",   C1TEXT},
    { GameMission_t::doom2,     1, 11, "RROCK14",   C2TEXT},
    { GameMission_t::doom2,     1, 20, "RROCK07",   C3TEXT},
    { GameMission_t::doom2,     1, 30, "RROCK17",   C4TEXT},
    { GameMission_t::doom2,     1, 15, "RROCK13",   C5TEXT},
    { GameMission_t::doom2,     1, 31, "RROCK19",   C6TEXT},

    { GameMission_t::pack_tnt,  1, 6,  "SLIME16",   T1TEXT},
    { GameMission_t::pack_tnt,  1, 11, "RROCK14",   T2TEXT},
    { GameMission_t::pack_tnt,  1, 20, "RROCK07",   T3TEXT},
    { GameMission_t::pack_tnt,  1, 30, "RROCK17",   T4TEXT},
    { GameMission_t::pack_tnt,  1, 15, "RROCK13",   T5TEXT},
    { GameMission_t::pack_tnt,  1, 31, "RROCK19",   T6TEXT},

    { GameMission_t::pack_plut, 1, 6,  "SLIME16",   P1TEXT},
    { GameMission_t::pack_plut, 1, 11, "RROCK14",   P2TEXT},
    { GameMission_t::pack_plut, 1, 20, "RROCK07",   P3TEXT},
    { GameMission_t::pack_plut, 1, 30, "RROCK17",   P4TEXT},
    { GameMission_t::pack_plut, 1, 15, "RROCK13",   P5TEXT},
    { GameMission_t::pack_plut, 1, 31, "RROCK19",   P6TEXT},

    { GameMission_t::pack_nerve, 1, 8, "SLIME16",   N1TEXT},
    { GameMission_t:: pack_master, 1, 20, "SLIME16",   M1TEXT},
    { GameMission_t::pack_master, 1, 21, "SLIME16",   M2TEXT},
};

const char *finaletext;
const char *finaleflat;
static char *finaletext_rw;

void	F_StartCast (void);
void	F_CastTicker (void);
boolean F_CastResponder (event_t *ev);
void	F_CastDrawer (void);

extern void A_RandomJump(mobj_t*mobj, player_t*	player, pspdef_t*	psp);

//
// F_StartFinale
//
void F_StartFinale (void)
{
    size_t i;

    gameaction = ga_nothing;
    gamestate = GS_FINALE;
    viewactive = false;
    automapactive = false;

    if (logical_gamemission == GameMission_t::doom)
    {
        S_ChangeMusic(mus_victor, true);
    }
    else
    {
        S_ChangeMusic(mus_read_m, true);
    }

    // Find the right screen and set the text and background

    for (i=0; i<arrlen(textscreens); ++i)
    {
        textscreen_t *screen = &textscreens[i];

        // Hack for Chex Quest

        if (gameversion == GameVersion_t::exe_chex && screen->mission == GameMission_t::doom)
        {
            screen->level = 5;
        }

        if (logical_gamemission == screen->mission
         && (logical_gamemission != GameMission_t::doom || gameepisode == screen->episode)
         && gamemap == screen->level)
        {
            finaletext = screen->text;
            finaleflat = screen->background;
        }
    }

    // Do dehacked substitutions of strings
  
    finaletext = DEH_String(finaletext);
    finaleflat = DEH_String(finaleflat);
    // [crispy] do the "char* vs. const char*" dance
    if (finaletext_rw)
    {
	free(finaletext_rw);
	finaletext_rw = nullptr;
    }
    finaletext_rw = M_StringDuplicate(finaletext);
    
    finalestage = F_STAGE_TEXT;
    finalecount = 0;
	
}



boolean F_Responder (event_t *event)
{
    if (finalestage == F_STAGE_CAST)
	return F_CastResponder (event);
	
    return false;
}


//
// F_Ticker
//
void F_Ticker (void)
{
    size_t		i;
    
    // check for skipping
    if ( (gamemode == GameMode_t::commercial)
      && ( finalecount > 50) )
    {
      // go on to the next level
      for (i=0 ; i<MAXPLAYERS ; i++)
	if (players[i].cmd.buttons)
	  break;
				
      if (i < MAXPLAYERS)
      {	
	if (gamemission == GameMission_t::pack_nerve && gamemap == 8)
	  F_StartCast ();
	else
	if (gamemission == GameMission_t::pack_master && (gamemap == 20 || gamemap == 21))
	  F_StartCast ();
	else
	if (gamemap == 30)
	  F_StartCast ();
	else
	  gameaction = ga_worlddone;
      }
    }
    
    // advance animation
    finalecount++;
	
    if (finalestage == F_STAGE_CAST)
    {
	F_CastTicker ();
	return;
    }
	
    if ( gamemode == GameMode_t::commercial)
	return;
		
    if (finalestage == F_STAGE_TEXT
     && finalecount>strlen (finaletext)*TEXTSPEED + TEXTWAIT)
    {
	finalecount = 0;
	finalestage = F_STAGE_ARTSCREEN;
	wipegamestate = static_cast<gamestate_t>(-1);		// force a wipe
	if (gameepisode == 3)
	    S_StartMusic (mus_bunny);
    }
}



//
// F_TextWrite
//

#include "hu_stuff.hpp"

// [crispy] add line breaks for lines exceeding screenwidth
static inline boolean F_AddLineBreak (char *c)
{
    while (c-- > finaletext_rw)
    {
	if (*c == '\n')
	{
	    return false;
	}
	else
	if (*c == ' ')
	{
	    *c = '\n';
	    return true;
	}
    }

    return false;
}

void F_TextWrite (void)
{
    byte*	src;
    pixel_t*	dest;
    
    int		x,y,w;
    signed int	count;
    char *ch; // [crispy] un-const
    int		c;
    int		cx;
    int		cy;
    
    // erase the entire screen to a tiled background
    src =  W_CacheLumpName_byte ( finaleflat , PU_CACHE);
    dest = I_VideoBuffer;
	
    for (y=0 ; y<SCREENHEIGHT ; y++)
    {
#ifndef CRISPY_TRUECOLOR
	for (x=0 ; x<SCREENWIDTH/64 ; x++)
	{
	    memcpy (dest, src+((y&63)<<6), 64);
	    dest += 64;
	}
	if (SCREENWIDTH&63)
	{
	    memcpy (dest, src+((y&63)<<6), SCREENWIDTH&63);
	    dest += (SCREENWIDTH&63);
	}
#else
	for (x=0 ; x<SCREENWIDTH ; x++)
	{
		*dest++ = colormaps[src[((y&63)<<6) + (x&63)]];
	}
#endif
    }

    V_MarkRect (0, 0, SCREENWIDTH, SCREENHEIGHT);
    
    // draw some of the text onto the screen
    cx = 10;
    cy = 10;
    ch = finaletext_rw;
	
    count = ((signed int) finalecount - 10) / TEXTSPEED;
    if (count < 0)
	count = 0;
    for ( ; count ; count-- )
    {
	c = *ch++;
	if (!c)
	    break;
	if (c == '\n')
	{
	    cx = 10;
	    cy += 11;
	    continue;
	}
		
	c = toupper(c) - HU_FONTSTART;
	if (c < 0 || c> HU_FONTSIZE)
	{
	    cx += 4;
	    continue;
	}
		
	w = SHORT (hu_font[c]->width);
	if (cx+w > ORIGWIDTH)
	{
	    // [crispy] add line breaks for lines exceeding screenwidth
	    if (F_AddLineBreak(ch))
	    {
		continue;
	    }
	    else
	    break;
	}
	// [cispy] prevent text from being drawn off-screen vertically
	if (cy + SHORT(hu_font[c]->height) > ORIGHEIGHT)
	{
	    break;
	}
	V_DrawPatch(cx, cy, hu_font[c]);
	cx+=w;
    }
	
}
static_assert(__cplusplus >= 201703L, "!");
//
// Final DOOM 2 animation
// Casting by id Software.
//   in order of appearance
//
struct castinfo_t
{
    const char	*name;
    mobjtype_t	type;
} ;

castinfo_t	castorder[] = {
    {CC_ZOMBIE, MT_POSSESSED},
    {CC_SHOTGUN, MT_SHOTGUY},
    {CC_HEAVY, MT_CHAINGUY},
    {CC_IMP, MT_TROOP},
    {CC_DEMON, MT_SERGEANT},
    {CC_LOST, MT_SKULL},
    {CC_CACO, MT_HEAD},
    {CC_HELL, MT_KNIGHT},
    {CC_BARON, MT_BRUISER},
    {CC_ARACH, MT_BABY},
    {CC_PAIN, MT_PAIN},
    {CC_REVEN, MT_UNDEAD},
    {CC_MANCU, MT_FATSO},
    {CC_ARCH, MT_VILE},
    {CC_SPIDER, MT_SPIDER},
    {CC_CYBER, MT_CYBORG},
    {CC_HERO, MT_PLAYER},

    {nullptr, mobjtype_t{}}
};


int		castnum;
int		casttics;
state_t*	caststate;
boolean		castdeath;
int		castframes;
int		castonmelee;
boolean		castattacking;
static signed char	castangle; // [crispy] turnable cast
static signed char	castskip; // [crispy] skippable cast
static boolean	castflip; // [crispy] flippable death sequence

// [crispy] randomize seestate and deathstate sounds in the cast
static int F_RandomizeSound (int sound)
{
	if (!crispy->soundfix)
		return sound;

	switch (sound)
	{
		// [crispy] actor->info->seesound, from p_enemy.c:A_Look()
		case sfx_posit1:
		case sfx_posit2:
		case sfx_posit3:
			return sfx_posit1 + Crispy_Random()%3;
			break;

		case sfx_bgsit1:
		case sfx_bgsit2:
			return sfx_bgsit1 + Crispy_Random()%2;
			break;

		// [crispy] actor->info->deathsound, from p_enemy.c:A_Scream()
		case sfx_podth1:
		case sfx_podth2:
		case sfx_podth3:
			return sfx_podth1 + Crispy_Random()%3;
			break;

		case sfx_bgdth1:
		case sfx_bgdth2:
			return sfx_bgdth1 + Crispy_Random()%2;
			break;

		default:
			return sound;
			break;
	}
}

extern void A_BruisAttack(mobj_t*);
extern void A_BspiAttack(mobj_t*);
extern void A_CPosAttack(mobj_t*);
extern void A_CPosRefire(mobj_t*);
extern void A_CyberAttack(mobj_t*);
extern void A_FatAttack1(mobj_t*);
extern void A_FatAttack2(mobj_t*);
extern void A_FatAttack3(mobj_t*);
extern void A_HeadAttack(mobj_t*);
extern void A_PainAttack(mobj_t*);
extern void A_PosAttack(mobj_t*);
extern void A_SargAttack(mobj_t*);
extern void A_SkelFist(mobj_t*);
extern void A_SkelMissile(mobj_t*);
extern void A_SkelWhoosh(mobj_t*);
extern void A_SkullAttack(mobj_t*);
extern void A_SPosAttack(mobj_t*);
extern void A_TroopAttack(mobj_t*);
extern void A_VileTarget(mobj_t*);


struct actionsound_t
{
    union u
    {
        void(*av0)();
        void(*av1)(mobj_t*);
        void(*av2)(mobj_t*, pspdef_t*);
        void(*av3)(mobj_t*, player_t*, pspdef_t*);
    };

    const u action;
	const int sound;
	const boolean early;
} ;

static const actionsound_t actionsounds[] =
{
	{{.av1 = A_PosAttack},   sfx_pistol, false},
	{{.av1 = A_SPosAttack},  sfx_shotgn, false},
	{{.av1 = A_CPosAttack},  sfx_shotgn, false},
	{{.av1 = A_CPosRefire},  sfx_shotgn, false},
	{{.av1 = A_VileTarget},  sfx_vilatk, true},
	{{.av1 = A_SkelWhoosh},  sfx_skeswg, false},
	{{.av1 = A_SkelFist},    sfx_skepch, false},
	{{.av1 = A_SkelMissile}, sfx_skeatk, true},
	{{.av1 = A_FatAttack1},  sfx_firsht, false},
	{{.av1 = A_FatAttack2},  sfx_firsht, false},
	{{.av1 = A_FatAttack3},  sfx_firsht, false},
	{{.av1 = A_HeadAttack},  sfx_firsht, true},
	{{.av1 = A_BruisAttack}, sfx_firsht, true},
	{{.av1 = A_TroopAttack}, sfx_claw,   false},
	{{.av1 = A_SargAttack},  sfx_sgtatk, true},
	{{.av1 = A_SkullAttack}, sfx_sklatk, false},
	{{.av1 = A_PainAttack},  sfx_sklatk, true},
	{{.av1 = A_BspiAttack},  sfx_plasma, false},
	{{.av1 = A_CyberAttack}, sfx_rlaunc, false},
};

// [crispy] play attack sound based on state action function (instead of state number)
static int F_SoundForState (int st)
{
	void *const castaction = (void *) caststate->action.acv;
	void *const nextaction = (void *) (&states[caststate->nextstate])->action.acv;

	// [crispy] fix Doomguy in casting sequence
	if (castaction == nullptr)
	{
		if (st == S_PLAY_ATK2)
			return sfx_dshtgn;
		else
			return 0;
	}
	else
	{
		for (size_t i = 0; i < arrlen(actionsounds); i++)
		{
			const actionsound_t *const as = &actionsounds[i];

			if ((!as->early && castaction == (void*)as->action.av0) ||
			    (as->early && nextaction == (void*)as->action.av0))
			{
				return as->sound;
			}
		}
	}

	return 0;
}

//
// F_StartCast
//
void F_StartCast (void)
{
    wipegamestate = GS_INVALID;		// force a screen wipe
    castnum = 0;
    caststate = &states[mobjinfo[castorder[castnum].type].seestate];
    casttics = caststate->tics;
    castdeath = false;
    finalestage = F_STAGE_CAST;
    castframes = 0;
    castonmelee = 0;
    castattacking = false;
    S_ChangeMusic(mus_evil, true);
}


//
// F_CastTicker
//
void F_CastTicker (void)
{
    int		st;
    int		sfx;
	
    if (--casttics > 0)
	    return;			// not time to change state yet
		
    if (caststate->tics == -1 || caststate->nextstate == S_NULL || castskip) // [crispy] skippable cast
    {
        if (castskip)
        {
            castnum += castskip;
            castskip = 0;
        }
        else
            // switch from deathstate to next monster
            castnum++;

        castdeath = false;
        if (castorder[castnum].name == nullptr)
            castnum = 0;

        if (mobjinfo[castorder[castnum].type].seesound)
            S_StartSound (nullptr, F_RandomizeSound(mobjinfo[castorder[castnum].type].seesound));

        caststate = &states[mobjinfo[castorder[castnum].type].seestate];
        castframes = 0;
        castangle = 0; // [crispy] turnable cast
        castflip = false; // [crispy] flippable death sequence
    }
    else
    {
        // just advance to next state in animation
        // [crispy] fix Doomguy in casting sequence
        /*
        if (!castdeath && caststate == &states[S_PLAY_ATK1])
            goto stopattack;	// Oh, gross hack!
        */
        // [crispy] Allow A_RandomJump() in deaths in cast sequence
        if (caststate->action.acp3 == A_RandomJump && Crispy_Random() < caststate->misc2)
        {
            st = caststate->misc1;
        }
        else
        {
            // [crispy] fix Doomguy in casting sequence
            if (!castdeath && caststate == &states[S_PLAY_ATK1])
                st = S_PLAY_ATK2;
            else
            if (!castdeath && caststate == &states[S_PLAY_ATK2])
                goto stopattack;	// Oh, gross hack!
            else
                st = caststate->nextstate;
        }
        caststate = &states[st];
        castframes++;
        
        sfx = F_SoundForState(st);
/*
	// sound hacks....
	switch (st)
	{
	  case S_PLAY_ATK2:	sfx = sfx_dshtgn; break; // [crispy] fix Doomguy in casting sequence
	  case S_POSS_ATK2:	sfx = sfx_pistol; break;
	  case S_SPOS_ATK2:	sfx = sfx_shotgn; break;
	  case S_VILE_ATK2:	sfx = sfx_vilatk; break;
	  case S_SKEL_FIST2:	sfx = sfx_skeswg; break;
	  case S_SKEL_FIST4:	sfx = sfx_skepch; break;
	  case S_SKEL_MISS2:	sfx = sfx_skeatk; break;
	  case S_FATT_ATK8:
	  case S_FATT_ATK5:
	  case S_FATT_ATK2:	sfx = sfx_firsht; break;
	  case S_CPOS_ATK2:
	  case S_CPOS_ATK3:
	  case S_CPOS_ATK4:	sfx = sfx_shotgn; break;
	  case S_TROO_ATK3:	sfx = sfx_claw; break;
	  case S_SARG_ATK2:	sfx = sfx_sgtatk; break;
	  case S_BOSS_ATK2:
	  case S_BOS2_ATK2:
	  case S_HEAD_ATK2:	sfx = sfx_firsht; break;
	  case S_SKULL_ATK2:	sfx = sfx_sklatk; break;
	  case S_SPID_ATK2:
	  case S_SPID_ATK3:	sfx = sfx_shotgn; break;
	  case S_BSPI_ATK2:	sfx = sfx_plasma; break;
	  case S_CYBER_ATK2:
	  case S_CYBER_ATK4:
	  case S_CYBER_ATK6:	sfx = sfx_rlaunc; break;
	  case S_PAIN_ATK3:	sfx = sfx_sklatk; break;
	  default: sfx = 0; break;
	}
		
*/
        if (sfx)
            S_StartSound (nullptr, sfx);
    }
	
    if (!castdeath && castframes == 12)
    {
	// go into attack frame
	castattacking = true;
	if (castonmelee)
	    caststate=&states[mobjinfo[castorder[castnum].type].meleestate];
	else
	    caststate=&states[mobjinfo[castorder[castnum].type].missilestate];
	castonmelee ^= 1;
	if (caststate == &states[S_NULL])
	{
	    if (castonmelee)
		caststate=
		    &states[mobjinfo[castorder[castnum].type].meleestate];
	    else
		caststate=
		    &states[mobjinfo[castorder[castnum].type].missilestate];
	}
    }
	
    if (castattacking)
    {
	if (castframes == 24
	    ||	caststate == &states[mobjinfo[castorder[castnum].type].seestate] )
	{
	  stopattack:
	    castattacking = false;
	    castframes = 0;
	    caststate = &states[mobjinfo[castorder[castnum].type].seestate];
	}
    }
	
    casttics = caststate->tics;
    if (casttics == -1)
    {
	// [crispy] Allow A_RandomJump() in deaths in cast sequence
	if (caststate->action.acp3 == A_RandomJump)
	{
	    if (Crispy_Random() < caststate->misc2)
	    {
		caststate = &states[caststate->misc1];
	    }
	    else
	    {
		caststate = &states[caststate->nextstate];
	    }

	    casttics = caststate->tics;
	}

	if (casttics == -1)
	{
	casttics = 15;
	}
    }
}


//
// F_CastResponder
//

boolean F_CastResponder (event_t* ev)
{
    boolean xdeath = false;

    if (ev->type != evtype_t::ev_keydown)
	return false;

    // [crispy] make monsters turnable in cast ...
    if (ev->data1 == key_left)
    {
	if (++castangle > 7)
	    castangle = 0;
	return false;
    }
    else
    if (ev->data1 == key_right)
    {
	if (--castangle < 0)
	    castangle = 7;
	return false;
    }
    else
    // [crispy] ... and allow to skip through them ..
    if (ev->data1 == key_strafeleft || ev->data1 == key_alt_strafeleft)
    {
	castskip = castnum ? -1 : arrlen(castorder)-2;
	return false;
    }
    else
    if (ev->data1 == key_straferight || ev->data1 == key_alt_straferight)
    {
	castskip = +1;
	return false;
    }
    // [crispy] ... and finally turn them into gibbs
    if (ev->data1 == key_speed)
	xdeath = true;
		
    if (castdeath)
	return true;			// already in dying frames
		
    // go into death frame
    castdeath = true;
    if (xdeath && mobjinfo[castorder[castnum].type].xdeathstate)
	caststate = &states[mobjinfo[castorder[castnum].type].xdeathstate];
    else
    caststate = &states[mobjinfo[castorder[castnum].type].deathstate];
    casttics = caststate->tics;
    // [crispy] Allow A_RandomJump() in deaths in cast sequence
    if (casttics == -1 && caststate->action.acp3 == A_RandomJump)
    {
        if (Crispy_Random() < caststate->misc2)
        {
            caststate = &states [caststate->misc1];
        }
        else
        {
            caststate = &states [caststate->nextstate];
        }
        casttics = caststate->tics;
    }
    castframes = 0;
    castattacking = false;
    if (xdeath && mobjinfo[castorder[castnum].type].xdeathstate)
        S_StartSound (nullptr, sfx_slop);
    else
    if (mobjinfo[castorder[castnum].type].deathsound)
	S_StartSound (nullptr, F_RandomizeSound(mobjinfo[castorder[castnum].type].deathsound));
	
    // [crispy] flippable death sequence
    castflip = crispy->flipcorpses &&
	castdeath &&
	(mobjinfo[castorder[castnum].type].flags & MF_FLIPPABLE) &&
	(Crispy_Random() & 1);

    return true;
}


void F_CastPrint (const char *text)
{
    const char *ch;
    int		c;
    int		cx;
    int		w;
    int		width;
    
    // find width
    ch = text;
    width = 0;
	
    while (ch)
    {
	c = *ch++;
	if (!c)
	    break;
	c = toupper(c) - HU_FONTSTART;
	if (c < 0 || c> HU_FONTSIZE)
	{
	    width += 4;
	    continue;
	}
		
	w = SHORT (hu_font[c]->width);
	width += w;
    }
    
    // draw it
    cx = ORIGWIDTH/2-width/2;
    ch = text;
    while (ch)
    {
	c = *ch++;
	if (!c)
	    break;
	c = toupper(c) - HU_FONTSTART;
	if (c < 0 || c> HU_FONTSIZE)
	{
	    cx += 4;
	    continue;
	}
		
	w = SHORT (hu_font[c]->width);
	V_DrawPatch(cx, 180, hu_font[c]);
	cx+=w;
    }
	
}


//
// F_CastDrawer
//

void F_CastDrawer (void)
{
    spritedef_t*	sprdef;
    spriteframe_t*	sprframe;
    int			lump;
    boolean		flip;
    patch_t*		patch;
    
    // erase the entire screen to a background
    V_DrawPatchFullScreen (W_CacheLumpName_patch (DEH_String("BOSSBACK"), PU_CACHE), false);

    F_CastPrint (DEH_String(castorder[castnum].name));
    
    // draw the current frame in the middle of the screen
    sprdef = &sprites[caststate->sprite];
    // [crispy] the TNT1 sprite is not supposed to be rendered anyway
    if (!sprdef->numframes && caststate->sprite == SPR_TNT1)
    {
	return;
    }
    sprframe = &sprdef->spriteframes[ caststate->frame & FF_FRAMEMASK];
    lump = sprframe->lump[castangle]; // [crispy] turnable cast
    flip = (boolean)sprframe->flip[castangle] ^ castflip; // [crispy] turnable cast, flippable death sequence
			
    patch = (patch_t*)W_CacheLumpNum (lump+firstspritelump, PU_CACHE);
    if (flip)
	V_DrawPatchFlipped(ORIGWIDTH/2, 170, patch);
    else
	V_DrawPatch(ORIGWIDTH/2, 170, patch);
}


//
// F_DrawPatchCol
//
static fixed_t dxi, dy, dyi;

void
F_DrawPatchCol
( int		x,
  patch_t*	patch,
  int		col )
{
    column_t*	column;
    byte*	source;
    pixel_t*	dest;
    pixel_t*	desttop;
    int		count;
	
    column = (column_t *)((byte *)patch + LONG(patch->columnofs[col]));
    desttop = I_VideoBuffer + x;

    // step through the posts in a column
    while (column->topdelta != 0xff )
    {
	int srccol = 0;
	source = (byte *)column + 3;
	dest = desttop + ((column->topdelta * dy) >> FRACBITS)*SCREENWIDTH;
	count = (column->length * dy) >> FRACBITS;
		
	while (count--)
	{
#ifndef CRISPY_TRUECOLOR
	    *dest = source[srccol >> FRACBITS];
#else
	    *dest = colormaps[source[srccol >> FRACBITS]];
#endif
	    srccol += dyi;
	    dest += SCREENWIDTH;
	}
	column = (column_t *)(  (byte *)column + column->length + 4 );
    }
}


//
// F_BunnyScroll
//
void F_BunnyScroll (void)
{
    signed int  scrolled;
    int		x;
    patch_t*	p1;
    patch_t*	p2;
    char	name[10];
    int		stage;
    static int	laststage;
    int         p2offset, p1offset, pillar_width;
		
    dxi = (ORIGWIDTH << FRACBITS) / NONWIDEWIDTH;
    dy = (SCREENHEIGHT << FRACBITS) / ORIGHEIGHT;
    dyi = (ORIGHEIGHT << FRACBITS) / SCREENHEIGHT;

    p1 = W_CacheLumpName_patch (DEH_String("PFUB2"), PU_LEVEL);
    p2 = W_CacheLumpName_patch (DEH_String("PFUB1"), PU_LEVEL);

    // [crispy] fill pillarboxes in widescreen mode
    pillar_width = (SCREENWIDTH - (SHORT(p1->width) << FRACBITS) / dxi) / 2;

    if (pillar_width > 0)
    {
        V_DrawFilledBox(0, 0, pillar_width, SCREENHEIGHT, 0);
        V_DrawFilledBox(SCREENWIDTH - pillar_width, 0, pillar_width, SCREENHEIGHT, 0);
    }
    else
    {
        pillar_width = 0;
    }

    // Calculate the portion of PFUB2 that would be offscreen at original res.
    p1offset = (ORIGWIDTH - SHORT(p1->width)) / 2;

    if (SHORT(p2->width) == ORIGWIDTH)
    {
        // Unity or original PFUBs.
        // PFUB1 only contains the pixels that scroll off.
        p2offset = ORIGWIDTH - p1offset;
    }
    else
    {
        // Widescreen mod PFUBs.
        // Right side of PFUB2 and left side of PFUB1 are identical.
        p2offset = ORIGWIDTH + p1offset;
    }

    V_MarkRect (0, 0, SCREENWIDTH, SCREENHEIGHT);
	
    scrolled = (ORIGWIDTH - ((signed int) finalecount-230)/2);
    if (scrolled > ORIGWIDTH)
	scrolled = ORIGWIDTH;
    if (scrolled < 0)
	scrolled = 0;

    for (x = pillar_width; x < SCREENWIDTH - pillar_width; x++)
    {
        int x2 = ((x * dxi) >> FRACBITS) - WIDESCREENDELTA + scrolled;

        if (x2 < p2offset)
            F_DrawPatchCol (x, p1, x2 - p1offset);
        else
            F_DrawPatchCol (x, p2, x2 - p2offset);
    }
	
    if (finalecount < 1130)
	return;
    if (finalecount < 1180)
    {
        V_DrawPatch((ORIGWIDTH - 13 * 8) / 2,
                    (ORIGHEIGHT - 8 * 8) / 2,
                    W_CacheLumpName_patch(DEH_String("END0"), PU_CACHE));
	laststage = 0;
	return;
    }
	
    stage = (finalecount-1180) / 5;
    if (stage > 6)
	stage = 6;
    if (stage > laststage)
    {
	S_StartSound (nullptr, sfx_pistol);
	laststage = stage;
    }
	
    DEH_snprintf(name, 10, "END%i", stage);
    V_DrawPatch((ORIGWIDTH - 13 * 8) / 2,
                (ORIGHEIGHT - 8 * 8) / 2,
                W_CacheLumpName_patch(name,PU_CACHE));
}

static void F_ArtScreenDrawer(void)
{
    const char *lumpname;
    
    if (gameepisode == 3)
    {
        F_BunnyScroll();
    }
    else
    {
        switch (gameepisode)
        {
            case 1:
                if (gameversion >= GameVersion_t::exe_ultimate)
                {
                    lumpname = "CREDIT";
                }
                else
                {
                    lumpname = "HELP2";
                }
                break;
            case 2:
                lumpname = "VICTORY2";
                break;
            case 4:
                lumpname = "ENDPIC";
                break;
            // [crispy] Sigil
            case 5:
                lumpname = "SIGILEND";
                if (W_CheckNumForName(DEH_String(lumpname)) == -1)
                {
                    return;
                }
                break;
            default:
                return;
        }

        lumpname = DEH_String(lumpname);

        V_DrawPatchFullScreen (W_CacheLumpName_patch(lumpname, PU_CACHE), false);
    }
}

//
// F_Drawer
//
void F_Drawer (void)
{
    switch (finalestage)
    {
        case F_STAGE_CAST:
            F_CastDrawer();
            break;
        case F_STAGE_TEXT:
            F_TextWrite();
            break;
        case F_STAGE_ARTSCREEN:
            F_ArtScreenDrawer();
            break;
    }
}

