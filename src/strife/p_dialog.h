// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// Copyright(C) 1993-1996 Id Software, Inc.
// Copyright(C) 1996 Rogue Entertainment / Velocity, Inc.
// Copyright(C) 2010 James Haley, Samuel Villareal
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
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.
//
// DESCRIPTION:
//
// [STRIFE] New Module
//
// Dialog Engine for Strife
//
//-----------------------------------------------------------------------------

#ifndef P_DIALOG_H__
#define P_DIALOG_H__

typedef struct mapdlgchoice_s
{
    int giveitem;    // item given when successful
    int needitem1;   // first item needed for success
    int needitem2;   // second item needed for success, if any
    int needitem3;   // third item needed for success, if any
    int needamount1; // amount of first item needed
    int needamount2; // amount of second item needed
    int needamount3; // amount of third item needed
    char text[32];   // normal text
    char textok[80]; // message given on success
    int next;        // next dialog?
    int objective;   // ???
    char textno[80]; // message given on failure
} mapdlgchoice_t;

typedef struct mapdialog_s
{
    int speakerid;   // script ID# for thingtype that will use this dialog
    int dropitem;    // item to drop if that thingtype is killed
    int checkitem1;  // first item needed to see this dialog
    int checkitem2;  // second item needed to see this dialog, if any
    int checkitem3;  // third item needed to see this dialog, if any
    int jumptoconv;  // conversation to jump to when... ?
    char name[16];   // name of speaker
    char voice[8];   // voice file to play
    char backpic[8]; // backdrop pic for character, if any
    char text[320];  // main message text
    
    mapdlgchoice_t choices[5]; // options that this dialog gives the player
} mapdialog_t;


#endif

// EOF

