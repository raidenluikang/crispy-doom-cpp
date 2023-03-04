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
//	WAD I/O functions.
//


#ifndef __W_WAD__
#define __W_WAD__

#include <stdio.h>
#include <type_traits>

#include "doomtype.hpp"
#include "w_file.hpp"
#include "v_patch.hpp"

//
// TYPES
//

//
// WADFILE I/O related stuff.
//

typedef struct lumpinfo_s lumpinfo_t;
typedef int lumpindex_t;

struct lumpinfo_s
{
    char	name[8];
    wad_file_t *wad_file;
    int		position;
    int		size;
    void       *cache;

    // Used for hash table lookups
    lumpindex_t next;
};


extern lumpinfo_t **lumpinfo;
extern unsigned int numlumps;

wad_file_t *W_AddFile(const char *filename);
void W_Reload(void);

lumpindex_t W_CheckNumForName(const char *name);
lumpindex_t W_GetNumForName(const char *name);
lumpindex_t W_CheckNumForNameFromTo(const char *name, int from, int to);

int W_LumpLength(lumpindex_t lump);
void W_ReadLump(lumpindex_t lump, void *dest);

void *W_CacheLumpNum(lumpindex_t lump, int tag);

template <typename T>
std::enable_if_t<std::is_pointer<T>::value, T> W_CacheLumpNum_cast(lumpindex_t lump, int tag)
{
    return static_cast<T>(W_CacheLumpNum(lump, tag));
}

void *W_CacheLumpName(const char *name, int tag);

// Helper functions
inline byte* W_CacheLumpName_byte(const char* name, int tag){
    return static_cast<byte*>(W_CacheLumpName(name, tag));
}
inline patch_t* W_CacheLumpName_patch(const char* name, int tag){
    return static_cast<patch_t*>(W_CacheLumpName(name, tag));
}


void W_GenerateHashTable(void);

extern unsigned int W_LumpNameHash(const char *s);

void W_ReleaseLumpNum(lumpindex_t lump);
void W_ReleaseLumpName(const char *name);

const char *W_WadNameForLump(const lumpinfo_t *lump);
boolean W_IsIWADLump(const lumpinfo_t *lump);

char **W_GetWADFileNames(void);

#endif
