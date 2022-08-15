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
#include <wg_c_gradient.h>


#include <wg_c_bitmapcache.h>
#include <wg_c_canvaslayers.h>
#include <wg_c_font.h>
#include <wg_c_gfxstreamreader.h>
#include <wg_c_gfxdevice.h>


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


struct wg_gfxdevice_calls
{
	int					structSize;

	const wg_typeInfo*	(*deviceSurfaceType)(wg_obj device);
	wg_canvasInfo		(*getCanvas)(wg_obj device);
	wg_canvasInfo		(*getCanvasRef)(wg_obj device, wg_canvasRef ref);
	wg_obj				(*canvasLayers)(wg_obj device);
	wg_obj				(*surfaceFactory)(wg_obj device);
	int					(*maxSegments)();
	wg_sizeSPX			(*canvasSize)(wg_obj device);
	int					(*setClipList)(wg_obj device, int nRectangles, const wg_rectSPX* pRectangles);
	void				(*resetClipList)(wg_obj device);
	int					(*pushClipList)(wg_obj device, int nRectangles, const wg_rectSPX* pRectangles);
	int					(*popClipList)(wg_obj device);
	const wg_rectSPX*	(*getClipList)(wg_obj device);
	int					(*clipListSize)(wg_obj device);
	const wg_rectSPX*	(*clipBounds)(wg_obj device);
	void				(*setTintColor)(wg_obj device, wg_color color);
	const wg_color		(*getTintColor)(wg_obj device);
	void				(*setTintGradient)(wg_obj device, const wg_rectSPX* rect, const wg_gradient* gradient);
	void				(*clearTintGradient)(wg_obj device);
	int					(*setBlendMode)(wg_obj device, wg_blendMode blendMode);
	wg_blendMode 		(*getBlendMode)(wg_obj device);
	int					(*setBlitSource)(wg_obj device, wg_obj surface);
	wg_obj 				(*getBlitSource)(wg_obj device);
	void				(*setMorphFactor)(wg_obj device, float factor);
	float				(*getMorphFactor)(wg_obj device);
	void				(*setRenderLayer)(wg_obj device, int layer);
	int					(*getRenderLayer)(wg_obj device);
	int					(*beginRender)(wg_obj device);
	int					(*endRender)(wg_obj device);
	int					(*isDeviceRendering)(wg_obj device);
	int					(*isDeviceIdle)(wg_obj device);
	void				(*flushDevice)(wg_obj device);
	int					(*beginCanvasUpdateWithRef)(wg_obj device, wg_canvasRef canvas, int nUpdateRects, const wg_rectSPX* pUpdateRects, wg_obj canvasLayers, int startLayer);
	int					(*beginCanvasUpdateWithSurface)(wg_obj device, wg_obj surface, int nUpdateRects, const wg_rectSPX* pUpdateRects, wg_obj canvasLayers, int startLayer);
	void				(*endCanvasUpdate)(wg_obj device);
	void				(*fill)(wg_obj device, wg_color col);
	void				(*fillRect)(wg_obj device, const wg_rectSPX* rect, wg_color col);
	void				(*plotPixels)(wg_obj device, int nCoords, const wg_coordSPX* pCoords, const wg_color* pColors);
	void				(*drawLine)(wg_obj device, wg_coordSPX begin, wg_coordSPX end, wg_color color, wg_spx thickness);
	void				(*drawStraightLine)(wg_obj device, wg_coordSPX begin, wg_direction dir, wg_spx length, wg_color color, wg_spx thickness);
	void				(*blit)(wg_obj device, wg_coordSPX dest);
	void				(*blitRect)(wg_obj device, wg_coordSPX dest, const wg_rectSPX* src);
	void				(*flipBlit)(wg_obj device, wg_coordSPX dest, wg_gfxFlip flip);
	void				(*flipBlitRect)(wg_obj device, wg_coordSPX dest, const wg_rectSPX* src, wg_gfxFlip flip);
	void				(*stretchBlit)(wg_obj device, const wg_rectSPX* dest);
	void				(*stretchBlitRect)(wg_obj device, const wg_rectSPX* dest, const wg_rectSPX* src);
	void				(*stretchFlipBlit)(wg_obj device, const wg_rectSPX* dest, wg_gfxFlip flip);
	void				(*stretchFlipBlitRect)(wg_obj device, const wg_rectSPX* dest, const wg_rectSPX* src, wg_gfxFlip flip);
	void				(*precisionBlit)(wg_obj device, const wg_rectSPX* dest, const wg_rectF* srcSPX);
	void				(*transformBlit)(wg_obj device, const wg_rectSPX* dest, wg_coordF srcSPX, const float transform[2][2]);
	void				(*rotScaleBlit)(wg_obj device, const wg_rectSPX* dest, float rotationDegrees, float scale, wg_coordF srcCenter, wg_coordF destCenter);
	void				(*tile)(wg_obj device, const wg_rectSPX* dest, wg_coordSPX shift);
	void				(*flipTile)(wg_obj device, const wg_rectSPX* dest, wg_gfxFlip flip, wg_coordSPX shift);
	void				(*scaleTile)(wg_obj device, const wg_rectSPX* dest, float scale, wg_coordSPX shift);
	void				(*scaleFlipTile)(wg_obj device, const wg_rectSPX* dest, float scale, wg_gfxFlip flip, wg_coordSPX shift);
	void				(*drawWave)(wg_obj device, const wg_rectSPX* dest, const wg_waveLine* pTopBorder, const wg_waveLine* pBottomBorder, wg_color frontFill, wg_color backFill);
	void				(*flipDrawWave)(wg_obj device, const wg_rectSPX* dest, const wg_waveLine* pTopBorder, const wg_waveLine* pBottomBorder, wg_color frontFill, wg_color backFill, wg_gfxFlip flip);
	void				(*drawElipse)(wg_obj device, const wg_rectSPX* canvas, wg_spx thickness, wg_color color, wg_spx outlineThickness, wg_color outlineColor);
	void				(*drawPieChart)(wg_obj device, const wg_rectSPX* canvas, float start, int nSlices, const float* pSliceSizes, const wg_color* pSliceColors, float hubSize, wg_color hubColor, wg_color backColor, int bRectangular);
	void				(*drawSegments)(wg_obj device, const wg_rectSPX* dest, int nSegments, const wg_color* pSegmentColors, int nEdgeStrips, const int* pEdgeStrips, int edgeStripPitch, wg_tintMode tintMode);
	void				(*flipDrawSegments)(wg_obj device, const wg_rectSPX* dest, int nSegments, const wg_color* pSegmentColors, int nEdgeStrips, const int* pEdgeStrips, int edgeStripPitch, wg_gfxFlip flip, wg_tintMode tintMode);
	void				(*blitNinePatch)(wg_obj device, const wg_rectSPX* dstRect, const wg_borderSPX* dstFrame, const wg_ninePatch* patch, int scale);
};


struct wg_streambuffer_calls
{
	int					structSize;

	wg_obj				(*createStreamBuffer)(int initialCapacity);
	wg_component		(*getStreamBufferOutput)(wg_obj streamBuffer);
	wg_component		(*getStreamBufferInput)(wg_obj streamBuffer);
	int					(*streamBufferCapacity)(wg_obj streamBuffer);
	int					(*streamBufferHasChunk)(wg_obj streamBuffer);
	int					(*streamBufferBytes)(wg_obj streamBuffer);
};

struct wg_streamplayer_calls
{
	int					structSize;

	wg_obj				(*createStreamPlayer)(wg_obj device, wg_obj surfaceFactory);
	wg_component		(*getStreamPlayerInput)(wg_obj streamPlayer);
	void				(*setStreamPlayerStoreDirtyRects)(wg_obj streamPlayer, int bStore);
	void				(*setStreamPlayerMaxDirtyRects)(wg_obj streamPlayer, int max);
	int					(*getStreamPlayerDirtyRects)(wg_obj streamPlayer, wg_canvasRef canvas, const wg_rectI** pRects);
	void				(*clearStreamPlayerDirtyRects)(wg_obj streamPlayer);
};

struct wg_streampump_calls
{
	int					structSize;

	wg_obj				(*createStreamPump)();
	wg_obj				(*createStreamPumpWithInputOutput)(wg_component input, wg_component output);
	void				(*setStreamPumpInput)(wg_obj streamPump, wg_component input);
	void				(*setStreamPumpOutput)(wg_obj streamPump, wg_component output);
	wg_gfxChunkId		(*peekChunk)(wg_obj streamPump);
	int					(*pumpChunk)(wg_obj streamPump);
	int					(*pumpUntilFrame)(wg_obj streamPump);
	int					(*pumpFrame)(wg_obj streamPump);
	int					(*pumpAll)(wg_obj streamPump);
};

struct wg_streamreader_calls
{
	int					structSize;

	wg_obj				(*createStreamReader)(wg_readStream_func callback);
	wg_component		(*getStreamReaderOutput)(wg_obj streamReader);
	int					(*streamReaderCapacity)(wg_obj streamReader);
	int					(*streamReaderHasChunk)(wg_obj streamReader);
	int					(*streamReaderBytes)(wg_obj streamReader);
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


struct wg_surface_calls
{
	int					structSize;

	void				(*setSurfaceIdentity)(wg_obj surface, int id);
	int					(*getSurfaceIdentity)(wg_obj surface);
	wg_sizeI			(*surfacePixelSize)(wg_obj surface);
	int					(*surfacePixelWidth)(wg_obj surface);
	int					(*surfacePixelHeight)(wg_obj surface);
	wg_size				(*surfacePointSize)(wg_obj surface);
	wg_pts				(*surfacePointWidth)(wg_obj surface);
	wg_pts				(*surfacePointHeight)(wg_obj surface);
	void				(*setSurfaceScale)(wg_obj surface, int scale);
	int					(*getSurfaceScale)(wg_obj surface);
	wg_sampleMethod		(*surfaceSampleMethod)(wg_obj surface);
	int					(*surfaceIsTiling)(wg_obj surface);
	int					(*surfaceIsMipmapped)(wg_obj surface);
	int					(*surfaceAlpha)(wg_obj surface, wg_coordSPX coord);
	uint32_t			(*colorToPixel)(wg_obj surface, const wg_color* col);
	wg_color			(*pixelToColor)(wg_obj surface, uint32_t pixel);
	const wg_color8*	(*surfaceClut)(wg_obj surface);
	const wg_pixelDescription* (*surfacePixelDescription)(wg_obj surface);
	wg_pixelFormat		(*surfacePixelFormat)(wg_obj surface);
	int					(*surfacePixelBytes)(wg_obj surface);
	int					(*surfaceIsOpaque)(wg_obj surface);
	int					(*surfaceCanBeCanvas)(wg_obj surface);
	wg_pixelBuffer		(*allocPixelBuffer)(wg_obj surface);
	wg_pixelBuffer		(*allocPixelBufferFromRect)(wg_obj surface, const wg_rectI* rect);
	int					(*pushPixels)(wg_obj surface, const wg_pixelBuffer* buffer);
	int					(*pushPixelsFromRect)(wg_obj surface, const wg_pixelBuffer* buffer, const wg_rectI* bufferRect);
	void				(*pullPixels)(wg_obj surface, const wg_pixelBuffer* buffer);
	void				(*pullPixelsFromRect)(wg_obj surface, const wg_pixelBuffer* buffer, const wg_rectI* bufferRect);
	void				(*freePixelBuffer)(wg_obj surface, const wg_pixelBuffer* buffer);
	int					(*fillSurface)(wg_obj surface, wg_color col);
	int					(*fillSurfaceRect)(wg_obj surface, const wg_rectI* region, wg_color col);
	int					(*copySurface)(wg_obj destSurface, wg_coordI dst, wg_obj sourceSurface);
	int					(*copySurfaceRect)(wg_obj destSurface, wg_coordI dst, wg_obj sourceSurface, const wg_rectI* srcRect);
	void				(*setSurfaceBaggage)(wg_obj surface, wg_obj baggage);
	wg_obj				(*getSurfaceBaggage)(wg_obj surface);
	int					(*addSurfaceObserver)(wg_obj surface, wg_surfaceObserver_func, void* pData, int data);
	int					(*removeSurfaceObserver)(wg_obj surface, int observerId);
	wg_surfaceBP		(*getSurfaceBlueprint)(wg_obj surface);
};


struct wg_surfacefactory_calls
{
	int					structSize;

	wg_sizeI			(*maxSurfaceSize)(wg_obj surfaceFactory);
	wg_obj				(*createSurface)(wg_obj factory, const wg_surfaceBP* blueprint);
	wg_obj				(*createSurfaceFromBlob)(wg_obj factory, const wg_surfaceBP* blueprint, wg_obj blob, int pitch);
	wg_obj				(*createSurfaceFromBitmap)(wg_obj factory, const wg_surfaceBP* blueprint, uint8_t* pPixels, int pitch, const wg_pixelDescription* pPixelDescription);
	wg_obj				(*createSurfaceFromSurface)(wg_obj factory, const wg_surfaceBP* blueprint, wg_obj fromSurface);
};



struct wg_c_callCollection
{
	int							structSize;

	wg_bitmapcache_calls *		pBitmapCache;
	wg_bitmapfont_calls *		pBitmapFont;
	wg_canvaslayers_calls *		pCanvasLayers;
	wg_component_calls *		pComponent;
	wg_font_calls *				pFont;
	wg_object_calls *			pObject;
	wg_gfxdevice_calls *		pGfxDevice;
	wg_streambuffer_calls *		pStreamBuffer;
	wg_streamplayer_calls *		pStreamPlayer;
	wg_streampump_calls *		pStreamPump;
	wg_streamreader_calls *		pStreamReader;
	wg_surface_calls *			pSurface;
	wg_surfacefactory_calls *	pSurfaceFactory;
};



void	wg_populateCallCollection(wg_c_callCollection * pStruct);


#ifdef __cplusplus
}
#endif

#endif
