/*=========================================================================

						 >>> WonderGUI <<<

  This file is part of Tord Jansson's WonderGUI Graphics Toolkit
  and copyright (c) Tord Jansson, Sweden [tord.jansson@gmail.com].

							-----------

  The WonderGUI Graphics Toolkit is free software; you can redistribute
  this file and/or modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

							-----------

  The WonderGUI Graphics Toolkit is also available for use in commercial
  closed-source projects under a separate license. Interested parties
  should contact Tord Jansson [tord.jansson@gmail.com] for details.

=========================================================================*/

#ifndef WG_C_PATCHES_DOT_H
#define WG_C_PATCHES_DOT_H
#pragma once

#include <wg_c_geo.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void* wg_patches;

wg_patches	wg_createPatches();
void		wg_deletePatches(wg_patches obj);

int			wg_setPatchesCapacity( wg_patches obj, int capacity );

void		wg_clearPatches(wg_patches obj);

void		wg_addPatch( wg_patches obj, wg_rectSPX rect );
void		wg_addPatches( wg_patches dest, wg_patches from );
void		wg_addPatchesSubset( wg_patches dest, wg_patches from, int ofs, int len );

void		wg_subPatch( wg_patches obj, wg_rectSPX rect );
void		wg_subPatches( wg_patches dest, wg_patches from );
void		wg_subPatchesSubset( wg_patches dest, wg_patches from, int ofs, int len );

void		wg_pushPatch( wg_patches obj, wg_rectSPX rect );
void		wg_pushPatches( wg_patches dest, wg_patches from );
void		wg_pushPatchesSubset( wg_patches dest, wg_patches from, int ofs, int len );

void		wg_popPatch( wg_patches obj );
void		wg_trimPushPatches( wg_patches dest, wg_patches from, wg_rectSPX trim );

int			wg_removePatches( wg_patches dest, int ofs, int len );

//.____ Misc __________________________________________________________

void		wg_clipPatches(	wg_patches obj, wg_rectSPX clip );
wg_rectSPX	wg_patchesBounds( wg_patches obj );


//.____ Content ________________________________________________________

const wg_rectSPX * wg_patchesBegin( wg_patches obj );
const wg_rectSPX * wg_patchesEnd( wg_patches obj );

int			wg_patchesSize( wg_patches obj );
int			wg_patchesCapacity( wg_patches obj );
int			wg_patchesIsEmpty( wg_patches obj );


#ifdef __cplusplus
}
#endif

#endif
