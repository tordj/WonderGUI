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

#include <wg_c_gfxdevice.h>
#include <wg_gfxdevice.h>

using namespace wg;

const wg_typeInfo* wg_deviceSurfaceType(wg_obj device)
{
	auto& typeInfo = static_cast<GfxDevice*>(reinterpret_cast<Object*>(device))->surfaceType();
	return reinterpret_cast<const wg_typeInfo*>(&typeInfo);
}


wg_canvasInfo wg_getCanvas(wg_obj device)
{
	auto& canvasInfo = static_cast<GfxDevice*>(reinterpret_cast<Object*>(device))->canvas();
	return { (wg_canvasRef)canvasInfo.ref, canvasInfo.pSurface, {canvasInfo.size.w, canvasInfo.size.h}, canvasInfo.scale };
}


wg_canvasInfo wg_getCanvasRef(wg_obj device, wg_canvasRef ref)
{
	auto& canvasInfo = static_cast<GfxDevice*>(reinterpret_cast<Object*>(device))->canvas((CanvasRef)ref);
	return { (wg_canvasRef)canvasInfo.ref, canvasInfo.pSurface, {canvasInfo.size.w, canvasInfo.size.h}, canvasInfo.scale };
}


wg_obj	wg_getCanvasLayers(wg_obj device)
{
	return static_cast<GfxDevice*>(reinterpret_cast<Object*>(device))->canvasLayers().rawPtr();
}


wg_obj wg_getSurfaceFactory(wg_obj device)
{
	return static_cast<GfxDevice*>(reinterpret_cast<Object*>(device))->surfaceFactory().rawPtr();
}


int	wg_maxSegments()
{
	return GfxDevice::maxSegments();
}


wg_sizeSPX wg_canvasSize(wg_obj device)
{
	auto canvasSize = static_cast<GfxDevice*>(reinterpret_cast<Object*>(device))->canvasSize();
	return { canvasSize.w, canvasSize.h };
}


int wg_setClipList(wg_obj device, int nRectangles, const wg_rectSPX* pRectangles)
{
	return static_cast<GfxDevice*>(reinterpret_cast<Object*>(device))->setClipList(nRectangles, (RectSPX*)pRectangles);
}


void wg_resetClipList(wg_obj device)
{
	static_cast<GfxDevice*>(reinterpret_cast<Object*>(device))->resetClipList();
}


int	wg_pushClipList(wg_obj device, int nRectangles, const wg_rectSPX* pRectangles)
{
	return static_cast<GfxDevice*>(reinterpret_cast<Object*>(device))->pushClipList(nRectangles, (RectSPX*)pRectangles);
}


int	wg_popClipList(wg_obj device)
{
	return static_cast<GfxDevice*>(reinterpret_cast<Object*>(device))->popClipList();
}


const wg_rectSPX* wg_getClipList(wg_obj device)
{
	return (wg_rectSPX*) static_cast<GfxDevice*>(reinterpret_cast<Object*>(device))->clipList();
}


int wg_clipListSize(wg_obj device)
{
 	return static_cast<GfxDevice*>(reinterpret_cast<Object*>(device))->clipListSize();
}


const wg_rectSPX* wg_clipBounds(wg_obj device)
{
	return (wg_rectSPX*) &static_cast<GfxDevice*>(reinterpret_cast<Object*>(device))->clipBounds();
}


void wg_setTintColor(wg_obj device, wg_color color)
{
	static_cast<GfxDevice*>(reinterpret_cast<Object*>(device))->setTintColor( HiColor( color.r, color.g, color.b, color.a) );
}


const wg_color wg_getTintColor(wg_obj device)
{
	auto color = static_cast<GfxDevice*>(reinterpret_cast<Object*>(device))->tintColor();
	return { color.b, color.g, color.r, color.a };
}


void wg_setTintGradient(wg_obj device, const wg_rectSPX* rect, const wg_gradient* gradient)
{
	static_cast<GfxDevice*>(reinterpret_cast<Object*>(device))->setTintGradient( *(const RectSPX*)rect, *(const Gradient*)gradient );
}


void wg_clearTintGradient(wg_obj device)
{
	static_cast<GfxDevice*>(reinterpret_cast<Object*>(device))->clearTintGradient();
}


int wg_setBlendMode(wg_obj device, wg_blendMode blendMode)
{
	return static_cast<GfxDevice*>(reinterpret_cast<Object*>(device))->setBlendMode( (BlendMode) blendMode );
}


wg_blendMode wg_getBlendMode(wg_obj device)
{
	return (wg_blendMode) static_cast<GfxDevice*>(reinterpret_cast<Object*>(device))->blendMode();
}


int wg_setBlitSource(wg_obj device, wg_obj surface)
{
	return static_cast<GfxDevice*>(reinterpret_cast<Object*>(device))->setBlitSource( static_cast<Surface*>(reinterpret_cast<Object*>(surface)) );
}


wg_obj wg_getBlitSource(wg_obj device)
{
	return (wg_obj) static_cast<GfxDevice*>(reinterpret_cast<Object*>(device))->blitSource();
}


void wg_setMorphFactor(wg_obj device, float factor)
{
	static_cast<GfxDevice*>(reinterpret_cast<Object*>(device))->setMorphFactor(factor);
}


float wg_getMorphFactor(wg_obj device)
{
	return static_cast<GfxDevice*>(reinterpret_cast<Object*>(device))->morphFactor();
}


void wg_setRenderLayer(wg_obj device, int layer)
{
	static_cast<GfxDevice*>(reinterpret_cast<Object*>(device))->setRenderLayer(layer);
}


int wg_getRenderLayer(wg_obj device)
{
	return static_cast<GfxDevice*>(reinterpret_cast<Object*>(device))->renderLayer();
}


int wg_beginRender(wg_obj device)
{
	return static_cast<GfxDevice*>(reinterpret_cast<Object*>(device))->beginRender();
}


int wg_endRender(wg_obj device)
{
	return static_cast<GfxDevice*>(reinterpret_cast<Object*>(device))->endRender();
}


int wg_isDeviceRendering(wg_obj device)
{
	return static_cast<GfxDevice*>(reinterpret_cast<Object*>(device))->isRendering();
}


int wg_isDeviceIdle(wg_obj device)
{
	return static_cast<GfxDevice*>(reinterpret_cast<Object*>(device))->isIdle();
}


void wg_flushDevice(wg_obj device)
{
	static_cast<GfxDevice*>(reinterpret_cast<Object*>(device))->flush();
}


int wg_beginCanvasUpdateWithRef(wg_obj device, wg_canvasRef canvas, int nUpdateRects, const wg_rectSPX* pUpdateRects, wg_obj canvasLayers, int startLayer)
{
	return static_cast<GfxDevice*>(reinterpret_cast<Object*>(device))->beginCanvasUpdate( (CanvasRef) canvas, nUpdateRects, (const RectSPX*) pUpdateRects, 
			static_cast<CanvasLayers*>(reinterpret_cast<Object*>(canvasLayers)), startLayer );
}


int wg_beginCanvasUpdateWithSurface(wg_obj device, wg_obj surface, int nUpdateRects, const wg_rectSPX* pUpdateRects, wg_obj canvasLayers, int startLayer)
{
	return static_cast<GfxDevice*>(reinterpret_cast<Object*>(device))->beginCanvasUpdate(static_cast<Surface*>(reinterpret_cast<Object*>(surface)), nUpdateRects, (const RectSPX*)pUpdateRects,
		static_cast<CanvasLayers*>(reinterpret_cast<Object*>(canvasLayers)), startLayer);
}


void wg_endCanvasUpdate(wg_obj device)
{
	static_cast<GfxDevice*>(reinterpret_cast<Object*>(device))->endCanvasUpdate();
}


void wg_fill(wg_obj device, wg_color col)
{
	static_cast<GfxDevice*>(reinterpret_cast<Object*>(device))->fill( HiColor(col.r, col.g, col.b, col.a) );
}

void wg_fillRect(wg_obj device, const wg_rectSPX* rect, wg_color col)
{
	static_cast<GfxDevice*>(reinterpret_cast<Object*>(device))->fill( *(const RectSPX*) rect, HiColor(col.r, col.g, col.b, col.a) );
}


void wg_plotPixels(wg_obj device, int nCoords, const wg_coordSPX* pCoords, const wg_color* pColors)
{
	static_cast<GfxDevice*>(reinterpret_cast<Object*>(device))->plotPixels( nCoords, (const CoordSPX*) pCoords, (const HiColor*) pColors);
}


void wg_drawLine(wg_obj device, wg_coordSPX begin, wg_coordSPX end, wg_color color, wg_spx thickness)
{
	static_cast<GfxDevice*>(reinterpret_cast<Object*>(device))->drawLine({ begin.x, begin.y }, { end.x, end.y }, * (HiColor*) &color, (spx) thickness);
}


void wg_drawStraightLine(wg_obj device, wg_coordSPX begin, wg_direction dir, wg_spx length, wg_color color, wg_spx thickness)
{
	static_cast<GfxDevice*>(reinterpret_cast<Object*>(device))->drawLine( { begin.x, begin.y }, (Direction) dir, (spx) length, *(HiColor*)&color, (spx)thickness );
}


void wg_blit(wg_obj device, wg_coordSPX dest)
{
	static_cast<GfxDevice*>(reinterpret_cast<Object*>(device))->blit( {dest.x, dest.y} );
}


void wg_blitRect(wg_obj device, wg_coordSPX dest, const wg_rectSPX* src)
{
	static_cast<GfxDevice*>(reinterpret_cast<Object*>(device))->blit( { dest.x, dest.y }, * (const RectSPX*) src );
}


void wg_flipBlit(wg_obj device, wg_coordSPX dest, wg_gfxFlip flip)
{
	static_cast<GfxDevice*>(reinterpret_cast<Object*>(device))->flipBlit({ dest.x, dest.y }, (GfxFlip) flip );
}


void wg_flipBlitRect(wg_obj device, wg_coordSPX dest, const wg_rectSPX* src, wg_gfxFlip flip)
{
	static_cast<GfxDevice*>(reinterpret_cast<Object*>(device))->flipBlit({ dest.x, dest.y }, *(const RectSPX*)src, (GfxFlip) flip );
}


void wg_stretchBlit(wg_obj device, const wg_rectSPX* dest)
{
	static_cast<GfxDevice*>(reinterpret_cast<Object*>(device))->stretchBlit( *(const RectSPX*)dest );
}


void wg_stretchBlitRect(wg_obj device, const wg_rectSPX* dest, const wg_rectSPX* src)
{
	static_cast<GfxDevice*>(reinterpret_cast<Object*>(device))->stretchBlit( *(const RectSPX*)dest, *(const RectSPX*)src );
}


void wg_stretchFlipBlit(wg_obj device, const wg_rectSPX* dest, wg_gfxFlip flip)
{
	static_cast<GfxDevice*>(reinterpret_cast<Object*>(device))->stretchFlipBlit( *(const RectSPX*)dest, (GfxFlip) flip );
}


void wg_stretchFlipBlitRect(wg_obj device, const wg_rectSPX* dest, const wg_rectSPX* src, wg_gfxFlip flip)
{
	static_cast<GfxDevice*>(reinterpret_cast<Object*>(device))->stretchFlipBlit(*(const RectSPX*)dest, *(const RectSPX*)src, (GfxFlip)flip);
}


void wg_rotScaleBlit(wg_obj device, const wg_rectSPX* dest, float rotationDegrees, float scale, wg_coordF srcCenter, wg_coordF destCenter)
{
	static_cast<GfxDevice*>(reinterpret_cast<Object*>(device))->rotScaleBlit(*(const RectSPX*)dest, rotationDegrees, scale, {srcCenter.x, srcCenter.y}, {destCenter.x, destCenter.y} );
}


void wg_tile(wg_obj device, const wg_rectSPX* dest, wg_coordSPX shift)
{
	static_cast<GfxDevice*>(reinterpret_cast<Object*>(device))->tile(*(const RectSPX*)dest, {shift.x, shift.y} );
}


void wg_flipTile(wg_obj device, const wg_rectSPX* dest, wg_gfxFlip flip, wg_coordSPX shift)
{
	static_cast<GfxDevice*>(reinterpret_cast<Object*>(device))->flipTile(*(const RectSPX*)dest, (GfxFlip) flip, { shift.x, shift.y });
}


void wg_scaleTile(wg_obj device, const wg_rectSPX* dest, float scale, wg_coordSPX shift)
{
	static_cast<GfxDevice*>(reinterpret_cast<Object*>(device))->scaleTile(*(const RectSPX*)dest, scale, { shift.x, shift.y });
}


void wg_scaleFlipTile(wg_obj device, const wg_rectSPX* dest, float scale, wg_gfxFlip flip, wg_coordSPX shift)
{
	static_cast<GfxDevice*>(reinterpret_cast<Object*>(device))->scaleFlipTile(*(const RectSPX*)dest, scale, (GfxFlip) flip, {shift.x, shift.y});
}


void wg_drawWave(wg_obj device, const wg_rectSPX* dest, const wg_waveLine* pTopBorder, const wg_waveLine* pBottomBorder, wg_color frontFill, wg_color backFill)
{
	static_cast<GfxDevice*>(reinterpret_cast<Object*>(device))->drawWave(*(const RectSPX*)dest, (WaveLine*) pTopBorder, (WaveLine*) pBottomBorder, * (HiColor*) &frontFill, *(HiColor*)&backFill );
}


void wg_flipDrawWave(wg_obj device, const wg_rectSPX* dest, const wg_waveLine* pTopBorder, const wg_waveLine* pBottomBorder, wg_color frontFill, wg_color backFill, wg_gfxFlip flip)
{
	static_cast<GfxDevice*>(reinterpret_cast<Object*>(device))->flipDrawWave(*(const RectSPX*)dest, (WaveLine*)pTopBorder, (WaveLine*)pBottomBorder, *(HiColor*)&frontFill, *(HiColor*)&backFill, (GfxFlip) flip );
}


void wg_drawElipse(wg_obj device, const wg_rectSPX* canvas, wg_spx thickness, wg_color color, wg_spx outlineThickness, wg_color outlineColor)
{
	static_cast<GfxDevice*>(reinterpret_cast<Object*>(device))->drawElipse(*(const RectSPX*)canvas, thickness, *(HiColor*)&color, outlineThickness, *(HiColor*)&outlineColor );
}


void wg_drawPieChart(wg_obj device, const wg_rectSPX* canvas, float start, int nSlices, const float* pSliceSizes, const wg_color* pSliceColors, float hubSize, wg_color hubColor, wg_color backColor, int bRectangular)
{
	static_cast<GfxDevice*>(reinterpret_cast<Object*>(device))->drawPieChart(*(const RectSPX*)canvas, start, nSlices, pSliceSizes, (HiColor*)pSliceColors, hubSize, *(HiColor*)&hubColor, *(HiColor*)&backColor, bRectangular );
}


void wg_drawSegments(wg_obj device, const wg_rectSPX* dest, int nSegments, const wg_color* pSegmentColors, int nEdgeStrips, const int* pEdgeStrips, int edgeStripPitch, wg_tintMode tintMode)
{
	static_cast<GfxDevice*>(reinterpret_cast<Object*>(device))->drawSegments(*(const RectSPX*)dest, nSegments, (HiColor*)pSegmentColors, nEdgeStrips, pEdgeStrips, edgeStripPitch, (TintMode) tintMode );
}


void wg_flipDrawSegments(wg_obj device, const wg_rectSPX* dest, int nSegments, const wg_color* pSegmentColors, int nEdgeStrips, const int* pEdgeStrips, int edgeStripPitch, wg_gfxFlip flip, wg_tintMode tintMode)
{
	static_cast<GfxDevice*>(reinterpret_cast<Object*>(device))->flipDrawSegments(*(const RectSPX*)dest, nSegments, (HiColor*)pSegmentColors, nEdgeStrips, pEdgeStrips, edgeStripPitch, (GfxFlip) flip, (TintMode)tintMode);
}


void wg_blitNinePatch(wg_obj device, const wg_rectSPX* dstRect, const wg_borderSPX* dstFrame, const wg_ninePatch* patch, int scale)
{
	static_cast<GfxDevice*>(reinterpret_cast<Object*>(device))->blitNinePatch(*(const RectSPX*)dstRect, *(BorderSPX*) dstFrame, * (NinePatch*) patch, scale);
}
