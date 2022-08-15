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

#ifndef WG_C_BITMAPCACHE_DOT_H
#define WG_C_BITMAPCACHE_DOT_H
#pragma once

#include <wg_c_types.h>
#include <wg_c_geo.h>

#ifdef __cplusplus
extern "C" {
#endif


struct wg_cacheSlot
{
	wg_obj		surface;
	wg_coordI	coord;
};

typedef void(*wg_cacheAddedSurfaceFunc)(wg_obj surface, void * pData, int data);
typedef void(*wg_cacheRemovedSurfacesFunc)(int nSurfaces, wg_obj* pSurfaces, void * pData, int data);
typedef void(*wg_cacheClearedFunc)(void * pData, int data);

typedef void* wg_cacheListener;

struct	wg_cacheListenerBP
{
	wg_cacheAddedSurfaceFunc	addFunc;
	void *						addPtr;
	int							addInt;
	
	wg_cacheRemovedSurfacesFunc	removeFunc;
	void *						removePtr;
	int							removeInt;
	
	wg_cacheClearedFunc			clearFunc;
	void *						clearPtr;
	int							clearInt;
};


wg_obj				wg_createBitmapCache( int maxSize, wg_obj surfaceFactory );

void 				wg_setCacheLimit(wg_obj bitmapCache, int maxBytes);
int					wg_cacheLimit(wg_obj bitmapCache);
int					wg_cacheSize(wg_obj bitmapCache);
void				wg_clearCache(wg_obj bitmapCache);

wg_cacheListener	wg_addCacheListener(wg_obj bitmapCache, wg_cacheListenerBP blueprint );
void				wg_removeCacheListener(wg_obj bitmapCache, wg_cacheListener listener );

wg_cacheSlot 		wg_getCacheSlot(wg_obj bitmapCache, wg_sizeI size);
int					wg_getNbCacheSurfaces(wg_obj bitmapCache);
int					wg_getCacheSurfaces(wg_obj bitmapCache, int maxSurfaces, wg_obj * pArray );


#ifdef __cplusplus
}
#endif


#endif
