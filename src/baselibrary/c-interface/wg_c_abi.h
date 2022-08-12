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


#ifndef WG_C_ABI_DOT_H
#define WG_C_ABI_DOT_H
#pragma once

#include <wg_c_types.h>
#include <wg_c_geo.h>
#include <wg_c_color.h>

#include <wg_c_bitmapcache.h>
#include <wg_c_canvaslayers.h>
#include <wg_c_font.h>

#ifdef __cplusplus
extern "C" {
#endif


struct wg_bitmapcache_calls
{
	int					structSize;

	wg_obj				(*createBitmapCache)( int maxSize, wg_obj surfaceFactory );
	void				(*setCacheLimit)(wg_obj bitmapCache, int maxBytes);
	int                 (*cacheLimit)(wg_obj bitmapCache);
	int                 (*cacheSize)(wg_obj bitmapCache);
	void                (*clearCache)(wg_obj bitmapCache);
	wg_cacheListener    (*addCacheListener)(wg_obj bitmapCache, wg_cacheListenerBP blueprint );
	void                (*removeCacheListener)(wg_obj bitmapCache, wg_cacheListener listener );
	wg_cacheSlot        (*getCacheSlot)(wg_obj bitmapCache, wg_sizeI size);
	int                 (*getNbCacheSurfaces)(wg_obj bitmapCache);
	int                 (*getCacheSurfaces)(wg_obj bitmapCache, int maxSurfaces, wg_obj * pArray );
};


struct wg_bitmapfont_calls
{
	int					structSize;

	wg_obj				(*createBitmapFont)(wg_obj surface, char* pGlyphSpec, wg_obj backupFont);
	wg_obj				(*getBitmapFontSurface)(wg_obj bitmapFont);
};

struct wg_canvaslayers_calls
{
	int					structSize;

	wg_obj				(*createCanvasLayers)(const wg_canvasLayersBP* pBlueprint);
	int					(*canvasLayersSize)(wg_obj canvasLayers);
	wg_pixelFormat		(*layerFormat)(wg_obj canvasLayers, int layer);
	int					(*baseLayer)(wg_obj canvasLayers);
};

struct wg_component_calls
{
	int					structSize;

	const wg_typeInfo*	(*getComponentTypeInfo)(wg_component);
	int					(*isComponentInstanceOf)(wg_component, const wg_typeInfo* pTypeInfo);
	wg_obj				(*getComponentObject)(wg_component);
};

struct wg_font_calls
{
	int					structSize;

	int					(*setFontSize)(wg_obj font, wg_spx size);
	wg_spx				(*fontSize)(wg_obj font);
	void				(*getGlyphWithoutBitmap)(wg_obj font, uint16_t chr, wg_glyph* pGlyph);
	void				(*getGlyphWithBitmap)(wg_obj font, uint16_t chr, wg_glyph* pGlyph);
	wg_spx				(*getKerning)(wg_obj font, wg_glyph* pLeftGlyph, wg_glyph* p_rightGlyph);
	wg_spx				(*lineGap)(wg_obj font);
	wg_spx				(*whitespaceAdvance)(wg_obj font);
	wg_spx				(*maxAdvance)(wg_obj font);
	wg_spx				(*maxAscend)(wg_obj font);
	wg_spx 				(*maxDescend)(wg_obj font);
	int					(*nbGlyphs)(wg_obj font);
	int					(*hasGlyphs)(wg_obj font);
	int					(*hasGlyph)(wg_obj font, uint16_t chr);
	int					(*isMonospace)(wg_obj font);
	int					(*isMonochrome)(wg_obj font);
	wg_obj				(*getBackupFont)(wg_obj font);
	wg_obj				(*createSurfaceFromGlyph)(wg_obj font, uint16_t chr, const wg_surfaceBP* blueprint, wg_obj surfaceFactory);
};


struct wg_object_calls
{
	int					structSize;

	const wg_typeInfo*	(*getTypeInfo)(wg_obj);
	int					(*isInstanceOf)(wg_obj, const wg_typeInfo* pTypeInfo);

	void				(*setFinalizer)(wg_obj, wg_finalizer_func pFinalizer);
	wg_finalizer_func	(*finalizer)(wg_obj);

	void				(*retain)(wg_obj);
	void				(*release)(wg_obj);

	int					(*refcount)(wg_obj);

};


struct wg_c_calls_body
{
	wg_bitmapcache_calls		bitmapCache;
	wg_bitmapfont_calls			bitmapFont;
	wg_canvaslayers_calls		canvasLayers;
	wg_component_calls			component;
	wg_font_calls				font;
	wg_object_calls				object;
};

struct wg_c_calls_header
{
	int							structSize;

	wg_bitmapcache_calls *		pBitmapCache;
	wg_bitmapfont_calls *		pBitmapFont;
	wg_canvaslayers_calls *		pCanvasLayers;
	wg_component_calls *		pComponent;
	wg_font_calls *				pFont;
	wg_object_calls *			pObject;
};




void	wg_populateCallStruc(wg_c_calls_header * pHeader, wg_c_calls_body * pStruct);


#ifdef __cplusplus
}
#endif

#endif
