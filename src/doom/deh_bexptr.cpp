//
// Copyright(C) 2005-2014 Simon Howard
// Copyright(C) 2014 Fabian Greffrath
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
// Parses [CODEPTR] sections in BEX files
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "info.hpp"

#include "deh_io.hpp"
#include "deh_main.hpp"

struct mobj_t;
struct player_t;
struct pspdef_t;

extern void A_Light0(mobj_t*	mobj, player_t*	player, pspdef_t*	psp);
extern void A_WeaponReady(mobj_t*	mobj, player_t*	player, pspdef_t*	psp);
extern void A_Lower(mobj_t*	mobj, player_t*	player, pspdef_t*	psp);
extern void A_Raise(mobj_t*	mobj, player_t*	player, pspdef_t*	psp);
extern void A_Punch(mobj_t*	mobj, player_t*	player, pspdef_t*	psp );
extern void A_ReFire( mobj_t*	mobj, player_t*	player, pspdef_t*	psp);
extern void A_FirePistol( mobj_t*	mobj, player_t*	player, pspdef_t*	psp );
extern void A_Light1(mobj_t*	mobj, player_t*	player, pspdef_t*	psp);
extern void A_FireShotgun( mobj_t*	mobj, player_t*	player, pspdef_t*	psp );
extern void A_Light2(mobj_t *mobj, player_t *player, pspdef_t *psp);
extern void A_FireShotgun2( mobj_t*	mobj, player_t*	player, pspdef_t*	psp );
extern void A_CheckReload( mobj_t*	mobj, player_t*	player, pspdef_t*	psp );
extern void A_OpenShotgun2(mobj_t*	mobj, player_t*	player, pspdef_t*	psp);
extern void A_LoadShotgun2(mobj_t*	mobj, player_t*	player, pspdef_t*	psp);
extern void A_CloseShotgun2(mobj_t*	mobj, player_t*	player, pspdef_t*	psp);
extern void A_FireCGun( mobj_t*	mobj, player_t*	player, pspdef_t*	psp );
extern void A_GunFlash( mobj_t*	mobj, player_t*	player, pspdef_t*	psp );
extern void A_FireMissile( mobj_t*	mobj, player_t*	player, pspdef_t*	psp);
extern void A_Saw( mobj_t* mobj, player_t* player, pspdef_t* psp );
extern void A_FirePlasma( mobj_t*	mobj, player_t*	player, pspdef_t*	psp );
extern void A_BFGsound( mobj_t*	mobj, player_t*	player, pspdef_t*	psp );
extern void A_FireBFG( mobj_t*	mobj, player_t*	player, pspdef_t*	psp );
extern void A_BFGSpray(mobj_t* mo);
extern void A_Explode(mobj_t* thingy);
extern void A_Pain(mobj_t* actor);
extern void A_PlayerScream(mobj_t* mo);
extern void A_Fall(mobj_t *actor);
extern void A_XScream(mobj_t* actor);
extern void A_Look(mobj_t* actor);
extern void A_Chase(mobj_t*	actor);
extern void A_FaceTarget(mobj_t* actor);
extern void A_PosAttack(mobj_t*);
extern void A_Scream(mobj_t* actor);
extern void A_SPosAttack(mobj_t*);
extern void A_VileChase(mobj_t* actor);
extern void A_VileStart(mobj_t* actor);
extern void A_VileTarget(mobj_t*);
extern void A_VileAttack(mobj_t* actor);
extern void A_StartFire(mobj_t* actor);
extern void A_Fire(mobj_t* actor);
extern void A_FireCrackle(mobj_t* actor);
extern void A_Tracer(mobj_t* actor);
extern void A_SkelWhoosh(mobj_t*);
extern void A_SkelFist(mobj_t*);
extern void A_SkelMissile(mobj_t*);
extern void A_FatRaise(mobj_t *actor);
extern void A_FatAttack1(mobj_t*);
extern void A_FatAttack2(mobj_t*);
extern void A_FatAttack3(mobj_t*);
extern void A_BossDeath(mobj_t* mo);
extern void A_CPosAttack(mobj_t*);
extern void A_CPosRefire(mobj_t*);
extern void A_TroopAttack(mobj_t*);
extern void A_SargAttack(mobj_t*);
extern void A_HeadAttack(mobj_t*);
extern void A_BruisAttack(mobj_t*);
extern void A_SkullAttack(mobj_t*);
extern void A_Metal(mobj_t*);
extern void A_SpidRefire(mobj_t* actor);
extern void A_BabyMetal(mobj_t* mo);
extern void A_BspiAttack(mobj_t*);
extern void A_Hoof(mobj_t* mo);
extern void A_CyberAttack(mobj_t*);
extern void A_PainAttack(mobj_t*);
extern void A_PainDie(mobj_t *);
extern void A_KeenDie(mobj_t* mo);
extern void A_BrainPain(mobj_t*	mo);
extern void A_BrainScream(mobj_t*	mo);
extern void A_BrainDie(mobj_t*	mo);
extern void A_BrainAwake(mobj_t* mo);
extern void A_BrainSpit(mobj_t*	mo);
extern void A_SpawnSound(mobj_t* mo);
extern void A_SpawnFly(mobj_t* mo);
extern void A_BrainExplode(mobj_t* mo);
// [crispy] additional BOOM and MBF states, sprites and code pointers
extern void A_Stop(mobj_t*);
extern void A_Die(mobj_t*);
extern void A_FireOldBFG(mobj_t *mobj, player_t *player, pspdef_t *psp);
extern void A_Detonate(mobj_t *mo);
extern void A_Mushroom(mobj_t *actor);
extern void A_BetaSkullAttack(mobj_t *actor);
// [crispy] more MBF code pointers
extern void A_Spawn(mobj_t *mo);
extern void A_Turn(mobj_t *mo);
extern void A_Face(mobj_t *mo);
extern void A_Scratch(mobj_t *mo);
extern void A_PlaySound(mobj_t *mo);
extern void A_RandomJump(mobj_t*mobj, player_t*	player, pspdef_t*	psp);
extern void A_LineEffect(mobj_t *mo);

struct bex_codeptr_t
{
    const char *mnemonic;
    const actionf_t pointer;
} ;

static const bex_codeptr_t bex_codeptrtable[] = {
    {"Light0", {.acp3 = A_Light0}},
    {"WeaponReady", {.acp3 = A_WeaponReady}},
    {"Lower", {.acp3 = A_Lower}},
    {"Raise", {.acp3 = A_Raise}},
    {"Punch", {.acp3 = A_Punch}},
    {"ReFire", {.acp3 = A_ReFire}},
    {"FirePistol", {.acp3 = A_FirePistol}},
    {"Light1", {.acp3 = A_Light1}},
    {"FireShotgun", {.acp3=A_FireShotgun}},
    {"Light2", {.acp3=A_Light2}},
    {"FireShotgun2", {.acp3 = A_FireShotgun2}},
    {"CheckReload", {.acp3 = A_CheckReload}},
    {"OpenShotgun2", {.acp3 = A_OpenShotgun2}},
    {"LoadShotgun2", {.acp3 = A_LoadShotgun2}},
    {"CloseShotgun2", {.acp3 = A_CloseShotgun2}},
    {"FireCGun", {.acp3 = A_FireCGun}},
    {"GunFlash", {.acp3 = A_GunFlash}},
    {"FireMissile", {.acp3 = A_FireMissile}},
    {"Saw", {.acp3 = A_Saw}},
    {"FirePlasma", {.acp3 = A_FirePlasma}},
    {"BFGsound", {.acp3 = A_BFGsound}},
    {"FireBFG", {.acp3 = A_FireBFG}},
    {"BFGSpray", {.acp1 = A_BFGSpray}},
    {"Explode", {.acp1=A_Explode}},
    {"Pain", {.acp1=A_Pain}},
    {"PlayerScream", {.acp1=A_PlayerScream}},
    {"Fall", {.acp1=A_Fall}},
    {"XScream", {.acp1=A_XScream}},
    {"Look", {.acp1=A_Look}},
    {"Chase", {.acp1=A_Chase}},
    {"FaceTarget", {.acp1=A_FaceTarget}},
    {"PosAttack", {.acp1 = A_PosAttack}},
    {"Scream", {.acp1=A_Scream}},
    {"SPosAttack", {.acp1 = A_SPosAttack}},
    {"VileChase", {.acp1=A_VileChase}},
    {"VileStart", {.acp1=A_VileStart}},
    {"VileTarget", {.acp1 = A_VileTarget}},
    {"VileAttack", {.acp1 = A_VileAttack}},
    {"StartFire", {.acp1=A_StartFire}},
    {"Fire", {.acp1=A_Fire}},
    {"FireCrackle", {.acp1=A_FireCrackle}},
    {"Tracer", {.acp1=A_Tracer}},
    {"SkelWhoosh", {.acp1 = A_SkelWhoosh}},
    {"SkelFist", {.acp1 = A_SkelFist}},
    {"SkelMissile", {.acp1=A_SkelMissile}},
    {"FatRaise", {.acp1=A_FatRaise}},
    {"FatAttack1", {.acp1= A_FatAttack1}},
    {"FatAttack2", {.acp1 = A_FatAttack2}},
    {"FatAttack3", {.acp1 = A_FatAttack3}},
    {"BossDeath",  {.acp1 = A_BossDeath}},
    {"CPosAttack", {.acp1 = A_CPosAttack}},
    {"CPosRefire", {.acp1 = A_CPosRefire}},
    {"TroopAttack", {.acp1 = A_TroopAttack}},
    {"SargAttack", {.acp1 = A_SargAttack}},
    {"HeadAttack", {.acp1 = A_HeadAttack}},
    {"BruisAttack", {.acp1 = A_BruisAttack}},
    {"SkullAttack", {.acp1 = A_SkullAttack}},
    {"Metal", {.acp1 = A_Metal}},
    {"SpidRefire", {.acp1=A_SpidRefire}},
    {"BabyMetal", {.acp1=A_BabyMetal}},
    {"BspiAttack", {.acp1 = A_BspiAttack}},
    {"Hoof", {.acp1=A_Hoof}},
    {"CyberAttack", {.acp1 = A_CyberAttack}},
    {"PainAttack", {.acp1 = A_PainAttack}},
    {"PainDie", {.acp1=A_PainDie}},
    {"KeenDie", {.acp1=A_KeenDie}},
    {"BrainPain", {.acp1=A_BrainPain}},
    {"BrainScream", {.acp1=A_BrainScream}},
    {"BrainDie", {.acp1=A_BrainDie}},
    {"BrainAwake", {.acp1=A_BrainAwake}},
    {"BrainSpit", {.acp1=A_BrainSpit}},
    {"SpawnSound", {.acp1=A_SpawnSound}},
    {"SpawnFly", {.acp1=A_SpawnFly}},
    {"BrainExplode", {.acp1=A_BrainExplode}},
    // [crispy] additional BOOM and MBF states, sprites and code pointers
    {"Stop", {.acp1 = A_Stop}},
    {"Die", {.acp1 = A_Die}},
    {"FireOldBFG", {.acp3=A_FireOldBFG}},
    {"Detonate", {.acp1=A_Detonate}},
    {"Mushroom", {.acp1=A_Mushroom}},
    {"BetaSkullAttack", {.acp1=A_BetaSkullAttack}},
    // [crispy] more MBF code pointers
    {"Spawn", {.acp1=A_Spawn}},
    {"Turn", {.acp1=A_Turn}},
    {"Face", {.acp1=A_Face}},
    {"Scratch", {.acp1=A_Scratch}},
    {"PlaySound", {.acp1=A_PlaySound}},
    {"RandomJump", {.acp3 =  A_RandomJump}},
    {"LineEffect", {.acp1=A_LineEffect}},
    {"nullptr", {nullptr}},
};

extern actionf_t codeptrs[NUMSTATES];

static void *DEH_BEXPtrStart(deh_context_t *context, char *line)
{
    char s[10];

    if (sscanf(line, "%9s", s) == 0 || strcmp("[CODEPTR]", s))
    {
	DEH_Warning(context, "Parse error on section start");
    }

    return nullptr;
}

static void DEH_BEXPtrParseLine(deh_context_t *context, char *line, void *tag)
{
    state_t *state;
    char *variable_name, *value, frame_str[6];
    int frame_number;

    // parse "FRAME nn = mnemonic", where
    // variable_name = "FRAME nn" and value = "mnemonic"
    if (!DEH_ParseAssignment(line, &variable_name, &value))
    {
	    DEH_Warning(context, "Failed to parse assignment: %s", line);
	    return;
    }

    // parse "FRAME nn", where frame_number = "nn"
    if (sscanf(variable_name, "%5s %32d", frame_str, &frame_number) != 2 ||
        strcasecmp(frame_str, "FRAME"))
    {
	    DEH_Warning(context, "Failed to parse assignment: %s", variable_name);
	    return;
    }

    if (frame_number < 0 || frame_number >= NUMSTATES)
    {
	    DEH_Warning(context, "Invalid frame number: %i", frame_number);
	    return;
    }

    state =  &states[frame_number];

    for (size_t i = 0; i < arrlen(bex_codeptrtable); i++)
    {
        if (!strcasecmp(bex_codeptrtable[i].mnemonic, value))
        {
            state->action = bex_codeptrtable[i].pointer;
            return;
        }
    }

    DEH_Warning(context, "Invalid mnemonic '%s'", value);
}

deh_section_t deh_section_bexptr =
{
    "[CODEPTR]",
    nullptr,
    DEH_BEXPtrStart,
    DEH_BEXPtrParseLine,
    nullptr,
    nullptr,
};
