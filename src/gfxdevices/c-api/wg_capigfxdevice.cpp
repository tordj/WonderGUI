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

#include <wg_capigfxdevice.h>
#include <wg_capisurface.h>
#include <wg_capisurfacefactory.h>
#include <wg_capicanvaslayers.h>

#include <wg_c_object.h>
#include <wg_c_gfxdevice.h>
#include <wg_c_surface.h>
#include <wg_c_surfacefactory.h>
#include <wg_c_canvaslayers.h>

#include <wg_base.h>
#include <assert.h>

using namespace std;

namespace wg
{
	const TypeInfo CAPIGfxDevice::TYPEINFO = { "CAPIGfxDevice", &GfxDevice::TYPEINFO };



	//____ create() _______________________________________________________________

	CAPIGfxDevice_p CAPIGfxDevice::create( wg_obj object, CAPISurfaceFactory * pFactory )
	{
		CAPIGfxDevice_p p(new CAPIGfxDevice( object, pFactory ));
		return p;
	}


	//____ constructor _____________________________________________________________

	CAPIGfxDevice::CAPIGfxDevice( wg_obj object, CAPISurfaceFactory * pFactory )
	{
        wg_retain(object);
		m_cDevice = object;
        m_pSurfaceFactory = pFactory;
		m_bRendering = false;

	}

	//____ Destructor ______________________________________________________________

	CAPIGfxDevice::~CAPIGfxDevice()
	{
        wg_release(m_cDevice);
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& CAPIGfxDevice::typeInfo(void) const
	{
		return TYPEINFO;
	}

    //____ canvas() ___________________________________________________________

    const CanvasInfo CAPIGfxDevice::canvas(CanvasRef ref) const
    {
        auto info = wg_getCanvasRef(m_cDevice, (wg_canvasRef)ref);

        CanvasInfo info2;
        info2.ref = (CanvasRef) info.ref;
        info2.scale = info.scale;
        info2.size.w = info.size.w;
        info2.size.h = info.size.h;
        info2.pSurface = nullptr;       // Not supported. Can't give access to C++ object on other side of CAPI-wall.
        return info2;
    }

	//____ surfaceType() _______________________________________________________

	const TypeInfo& CAPIGfxDevice::surfaceType( void ) const
	{
		return CAPISurface::TYPEINFO;
	}

	//____ surfaceFactory() ______________________________________________________

	SurfaceFactory_p CAPIGfxDevice::surfaceFactory()
	{
        return m_pSurfaceFactory;
	}

    //____ setClipList() _________________________________________________________

    bool CAPIGfxDevice::setClipList(int nRectangles, const RectSPX * pRectangles)
    {
        int retVal = wg_setClipList(m_cDevice, nRectangles, (wg_rectSPX*)pRectangles);

        if (retVal)
            GfxDevice::setClipList(nRectangles, pRectangles);

        return retVal;
    }

    //____ resetClipList() _______________________________________________________

    void CAPIGfxDevice::resetClipList()
    {
        wg_resetClipList(m_cDevice);
        GfxDevice::resetClipList();
    }

    //____ pushClipList() ________________________________________________________

    bool CAPIGfxDevice::pushClipList(int nRectangles, const RectSPX* pRectangles)
    {
        int retVal = wg_pushClipList(m_cDevice, nRectangles, (wg_rectSPX*)pRectangles);

        if( retVal )
            GfxDevice::pushClipList(nRectangles, pRectangles);

        return retVal;
    }

    //____ popClipList() __________________________________________________________

    bool CAPIGfxDevice::popClipList()
    {
        int retVal = wg_popClipList(m_cDevice);
        if( retVal )
            GfxDevice::popClipList();
        
        return retVal;
    }

    //____ setTintColor() __________________________________________________________

    void CAPIGfxDevice::setTintColor( HiColor color )
    {
        GfxDevice::setTintColor(color);

        wg_setTintColor(m_cDevice, *(wg_color*)&color);
    }

    //____ setTintGradient() _______________________________________________________

    void CAPIGfxDevice::setTintGradient(const RectSPX& rect, const Gradient& gradient)
    {
        GfxDevice::setTintGradient(rect, gradient);
        
        wg_setTintGradient(m_cDevice, (const wg_rectSPX*)&rect, (const wg_gradient*)&gradient);
    }

    //____ clearTintGradient() _____________________________________________________

    void CAPIGfxDevice::clearTintGradient()
    {
        GfxDevice::clearTintGradient();

        wg_clearTintGradient(m_cDevice);
    }

    //____ setBlendMode() __________________________________________________________

    bool CAPIGfxDevice::setBlendMode( BlendMode blendMode )
    {
        if( blendMode < BlendMode_min || blendMode > BlendMode_max )
            return false;

        int retVal = wg_setBlendMode(m_cDevice, (wg_blendMode)blendMode);

        if( retVal )
            GfxDevice::setBlendMode(blendMode);

        return retVal;
    }

    //____ setBlitSource() _______________________________________________________

    bool CAPIGfxDevice::setBlitSource(Surface * pSource)
    {
        if (!pSource || !pSource->isInstanceOf(CAPIGfxDevice::TYPEINFO) )
            return false;

        int retVal = wg_setBlitSource(m_cDevice, static_cast<CAPISurface*>(pSource)->cObject());

        if( retVal )
            GfxDevice::setBlitSource(pSource);

        return retVal;
    }

    //____ setMorphFactor() ______________________________________________________

    void CAPIGfxDevice::setMorphFactor(float factor)
    {
        GfxDevice::setMorphFactor(factor);

        wg_setMorphFactor(m_cDevice, m_morphFactor);        
    }

    //____ setRenderLayer() _______________________________________________________

    void CAPIGfxDevice::setRenderLayer(int layer)
    {
        GfxDevice::setRenderLayer(layer);
       
       wg_setRenderLayer(m_cDevice, m_renderLayer);
    }

	//____ beginRender() ___________________________________________________________

	bool CAPIGfxDevice::beginRender()
	{
        int bOk = wg_beginRender(m_cDevice);

        if (bOk)
            GfxDevice::beginRender();
        return bOk;
	}

	//____ endRender() _____________________________________________________________

	bool CAPIGfxDevice::endRender()
	{
        int bOk = wg_endRender(m_cDevice);

        if (bOk)
            GfxDevice::endRender();
        return bOk;
    }

    //____ flush() ___________________________________________________________________

    void CAPIGfxDevice::flush()
    {
        wg_flushDevice(m_cDevice);
        GfxDevice::flush();
    }

    //____ endCanvasUpdate() _________________________________________________________

    void CAPIGfxDevice::endCanvasUpdate()
    {
    }

    //____ fill() __________________________________________________________________

    void CAPIGfxDevice::fill( HiColor _col )
    {
        wg_fill( m_cDevice, *(wg_color*)&_col );
    }

    void CAPIGfxDevice::fill( const RectSPX& _rect, HiColor _col )
    {
        wg_fillRect(m_cDevice, (wg_rectSPX*) &_rect, *(wg_color*)&_col);
    }

    //____ plotPixels() ________________________________________________________

    void CAPIGfxDevice::plotPixels(int nCoords, const CoordSPX * pCoords, const HiColor * pColors)
    {
        wg_plotPixels(m_cDevice, nCoords, (wg_coordSPX*)pCoords, (wg_color*)pColors);
    }

    //____ drawLine() __________________________________________________________

    void CAPIGfxDevice::drawLine(CoordSPX begin, CoordSPX end, HiColor color, spx thickness)
    {
        wg_drawLine(m_cDevice, { begin.x, begin.y }, { end.x, end.y }, *(wg_color*)&color, thickness);
    }

    void CAPIGfxDevice::drawLine(CoordSPX begin, Direction dir, int length, HiColor color, spx thickness)
    {
        wg_drawStraightLine(m_cDevice, { begin.x, begin.y }, (wg_direction) dir, length, * (wg_color*)&color, thickness);
    }

    //____ blit() __________________________________________________________________

    void CAPIGfxDevice::blit(CoordSPX dest)
    {
        wg_blit(m_cDevice, { dest.x, dest.y });
    }

    void CAPIGfxDevice::blit(CoordSPX dest, const RectSPX& src)
    {
        wg_blitRect(m_cDevice, { dest.x, dest.y }, (wg_rectSPX*) &src );
    }

    //____ flipBlit() _________________________________________________________

    void CAPIGfxDevice::flipBlit(CoordSPX dest, GfxFlip flip )
    {
        wg_flipBlit(m_cDevice, { dest.x, dest.y }, (wg_gfxFlip) flip);
    }

    void CAPIGfxDevice::flipBlit(CoordSPX dest, const RectSPX& src, GfxFlip flip )
    {
        wg_flipBlitRect(m_cDevice, { dest.x, dest.y }, (wg_rectSPX*)&src, (wg_gfxFlip)flip);
    }

    //____ stretchBlit() ______________________________________________________

    void CAPIGfxDevice::stretchBlit(const RectSPX& dest)
    {
        wg_stretchBlit(m_cDevice, (wg_rectSPX*) &dest);
    }

    void CAPIGfxDevice::stretchBlit(const RectSPX& dest, const RectSPX& source)
    {
        wg_stretchBlitRect(m_cDevice, (wg_rectSPX*)&dest, (wg_rectSPX*)&source);
    }

    //____ stretchFlipBlit() __________________________________________________

    void CAPIGfxDevice::stretchFlipBlit(const RectSPX& dest, GfxFlip flip)
    {
        wg_stretchFlipBlit(m_cDevice, (wg_rectSPX*)&dest, (wg_gfxFlip) flip);
    }

    void CAPIGfxDevice::stretchFlipBlit(const RectSPX& dest, const RectSPX& source, GfxFlip flip)
    {
        wg_stretchFlipBlitRect(m_cDevice, (wg_rectSPX*)&dest, (wg_rectSPX*)&source, (wg_gfxFlip)flip );
    }

	//____ precisionBlit() _______________________________________________________

	void CAPIGfxDevice::precisionBlit(const RectSPX& dest, const RectF& srcSPX)
	{
		wg_precisionBlit(m_cDevice, (wg_rectSPX*)&dest, (wg_rectF*)&srcSPX);
	}

	//____ transformBlit() _______________________________________________________

	void CAPIGfxDevice::transformBlit(const RectSPX& dest, CoordF srcSPX, const float transform[2][2])
	{
		wg_transformBlit(m_cDevice, (wg_rectSPX*)&dest, {srcSPX.x,srcSPX.y}, tranform);
	}

    //____ rotScaleBlit() _____________________________________________________
    void CAPIGfxDevice::rotScaleBlit(const RectSPX& dest, float rotationDegrees, float scale, CoordF srcCenter, CoordF destCenter)
    {
        wg_rotScaleBlit(m_cDevice, (wg_rectSPX*)&dest, rotationDegrees, scale, { srcCenter.x, srcCenter.y }, { destCenter.x, destCenter.y });
    }

    //____ tile() _____________________________________________________________

    void CAPIGfxDevice::tile(const RectSPX& dest, CoordSPX shift)
    {
        wg_tile(m_cDevice, (wg_rectSPX*)&dest, { shift.x,shift.y });
    }

    //____ flipTile() _________________________________________________________

    void CAPIGfxDevice::flipTile(const RectSPX& dest, GfxFlip flip, CoordSPX shift)
    {
        wg_flipTile(m_cDevice, (wg_rectSPX*)&dest, (wg_gfxFlip) flip, { shift.x, shift.y });
    }

    //____ scaleTile() ________________________________________________________

    void CAPIGfxDevice::scaleTile(const RectSPX& dest, float scale, CoordSPX shift)
    {
        wg_scaleTile(m_cDevice, (wg_rectSPX*)&dest, scale, { shift.x,shift.y });
    }

    //____ scaleFlipTile() ____________________________________________________

    void CAPIGfxDevice::scaleFlipTile(const RectSPX& dest, float scale, GfxFlip flip, CoordSPX shift)
    {
        wg_scaleFlipTile(m_cDevice, (wg_rectSPX*)&dest, scale, (wg_gfxFlip) flip, { shift.x, shift.y });
    }

    //____ drawWave() _________________________________________________________

    void CAPIGfxDevice::drawWave(const RectSPX& dest, const WaveLine * pTopBorder, const WaveLine * pBottomBorder, HiColor frontFill, HiColor backFill)
    {
        wg_drawWave(m_cDevice, (wg_rectSPX*)&dest, (wg_waveLine*)pTopBorder, (wg_waveLine*)pBottomBorder, *(wg_color*)&frontFill, *(wg_color*)&backFill);
    }

    //____ flipDrawWave() _____________________________________________________

    void CAPIGfxDevice::flipDrawWave(const RectSPX& dest, const WaveLine * pTopBorder, const WaveLine * pBottomBorder, HiColor frontFill, HiColor backFill, GfxFlip flip)
    {
        wg_flipDrawWave(m_cDevice, (wg_rectSPX*)&dest, (wg_waveLine*)pTopBorder, (wg_waveLine*)pBottomBorder, *(wg_color*)&frontFill, *(wg_color*)&backFill, (wg_gfxFlip) flip );
    }

    //____ _drawElipse() ______________________________________________________

    void CAPIGfxDevice::drawElipse(const RectSPX& canvas, spx thickness, HiColor color, spx outlineThickness, HiColor outlineColor )
    {
        wg_drawElipse(m_cDevice,(wg_rectSPX*)&canvas, thickness, *(wg_color*)&color, outlineThickness, *(wg_color*)&outlineColor);
    }

    //____ drawPieChart() _____________________________________________________

    void CAPIGfxDevice::drawPieChart(const RectSPX& canvas, float start, int nSlices, const float * pSliceSizes, const HiColor * pSliceColors, float hubSize, HiColor hubColor, HiColor backColor, bool bRectangular)
    {
        wg_drawPieChart(m_cDevice, (wg_rectSPX*)&canvas, start, nSlices, pSliceSizes, (const wg_color*)pSliceColors, hubSize, *(wg_color*)&hubColor, *(wg_color*)&backColor, bRectangular);
    }

    //____ drawSegments() _____________________________________________________

    void CAPIGfxDevice::drawSegments(const RectSPX& dest, int nSegments, const HiColor * pSegmentColors, int nEdgeStrips, const int * pEdgeStrips, int edgeStripPitch, TintMode tintMode )
    {
        wg_drawSegments(m_cDevice, (wg_rectSPX*)&dest, nSegments, (const wg_color*)pSegmentColors, nEdgeStrips, pEdgeStrips, edgeStripPitch, (wg_tintMode)tintMode);
    }

    //____ flipDrawSegments() _________________________________________________

    void CAPIGfxDevice::flipDrawSegments(const RectSPX& dest, int nSegments, const HiColor * pSegmentColors, int nEdgeStrips, const int * pEdgeStrips, int edgeStripPitch, GfxFlip flip, TintMode tintMode)
    {
        wg_flipDrawSegments(m_cDevice, (wg_rectSPX*)&dest, nSegments, (const wg_color*)pSegmentColors, nEdgeStrips, pEdgeStrips, edgeStripPitch, (wg_gfxFlip) flip, (wg_tintMode)tintMode);
    }

    //.____ blitNinePatch() ___________________________________________________

    void CAPIGfxDevice::blitNinePatch(const RectSPX& dstRect, const BorderSPX& dstFrame, const NinePatch& patch, int scale)
    {
        wg_blitNinePatch(m_cDevice, (wg_rectSPX*)&dstRect, (wg_borderSPX*)&dstFrame, (wg_ninePatch*)&patch, scale);
    }

    //.____ _beginCanvasUpdate() ______________________________________________

    bool CAPIGfxDevice::_beginCanvasUpdate(CanvasRef canvasRef, Surface * pCanvasSurface, int nUpdateRects, const RectSPX* pUpdateRects, CanvasLayers * pLayers, int startLayer)
    {     
		if( (canvasRef == CanvasRef::None && pCanvasSurface == nullptr) ||
		    (canvasRef != CanvasRef::None && pCanvasSurface != nullptr )  )
		{
			//TODO: Error handling!
			return false;
		}
		
		SizeI sz = (canvasRef != CanvasRef::None) ? canvas(canvasRef).size : pCanvasSurface->pixelSize();
		if( sz.isEmpty() )
		{
			// TODO: Error handling!
			return false;
		}

        auto pCAPILayers = wg_dynamic_cast<CAPICanvasLayers_p>(pLayers);


        if (canvasRef != CanvasRef::None)
        {
            wg_beginCanvasUpdateWithRef(m_cDevice, (wg_canvasRef)canvasRef, nUpdateRects, (wg_rectSPX*)pUpdateRects, pCAPILayers->cObject(), startLayer);
        }
        else
        {
            wg_beginCanvasUpdateWithSurface(m_cDevice, (wg_obj) static_cast<Object*>(pCanvasSurface), nUpdateRects, (wg_rectSPX*)pUpdateRects, pCAPILayers->cObject(), startLayer);
        }

		m_canvas.ref = canvasRef;
		m_canvas.pSurface = pCanvasSurface;
		
		m_canvas.size = sz;
		
        return true;
    }

    //____ _canvasWasChanged() ________________________________________________

    void CAPIGfxDevice::_canvasWasChanged()
    {
        //This method should never be called, but is pure virtual in super class.
        
        assert(false);
    }

    //____ _renderLayerWasChanged() ___________________________________________

    void CAPIGfxDevice::_renderLayerWasChanged()
    {
        // Do nothing.
    }

    //____ _transformBlit() ___________________________________________________

    void CAPIGfxDevice::_transformBlit(const RectI& dest, CoordI src, const int simpleTransform[2][2])
    {
        //This method should never be called, but is pure virtual in super class.
        
        assert(false);
    }

    //____ _transformBlit() ___________________________________________________

    void CAPIGfxDevice::_transformBlit(const RectI& dest, CoordF src, const float complexTransform[2][2])
    {
        //This method should never be called, but is pure virtual in super class.
        
        assert(false);
    }

	//____ _transformDrawSegments() ___________________________________________

	void CAPIGfxDevice::_transformDrawSegments(const RectI& dest, int nSegments, const HiColor * pSegmentColors, int nEdgeStrips, const int * pEdgeStrips, int edgeStripPitch, TintMode tintMode, const int simpleTransform[2][2])
	{
        //This method should never be called, but is pure virtual in super class.
        
        assert(false);
	}


} // namespace wg

