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

#include <wg_cabi.h>
#include <wg_cabi_gfxdevice.h>
#include <wg_cabi_surface.h>
#include <wg_cabi_surfacefactory.h>
#include <wg_cabi_canvaslayers.h>

#include <wg_base.h>
#include <assert.h>

using namespace std;

namespace wg
{
	const TypeInfo CABIGfxDevice::TYPEINFO = { "CABIGfxDevice", &GfxDevice::TYPEINFO };



	//____ create() _______________________________________________________________

	CABIGfxDevice_p CABIGfxDevice::create( wg_obj object, CABISurfaceFactory * pFactory )
	{
		CABIGfxDevice_p p(new CABIGfxDevice( object, pFactory ));
		return p;
	}


	//____ constructor _____________________________________________________________

	CABIGfxDevice::CABIGfxDevice( wg_obj object, CABISurfaceFactory * pFactory )
	{
        CABI::object->retain(object);
		m_cDevice = object;
        m_pSurfaceFactory = pFactory;
		m_bRendering = false;
		m_bIsProxyDevice = true;
	}

	//____ Destructor ______________________________________________________________

	CABIGfxDevice::~CABIGfxDevice()
	{
        CABI::object->release(m_cDevice);
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& CABIGfxDevice::typeInfo(void) const
	{
		return TYPEINFO;
	}

    //____ canvas() ___________________________________________________________

    const CanvasInfo CABIGfxDevice::canvas(CanvasRef ref) const
    {
        auto info = CABI::gfxDevice->getCanvasRef(m_cDevice, (wg_canvasRef)ref);

        CanvasInfo info2;
        info2.ref = (CanvasRef) info.ref;
        info2.scale = info.scale;
        info2.size.w = info.size.w;
        info2.size.h = info.size.h;
        info2.pSurface = nullptr;       // Not supported. Can't give access to C++ object on other side of CABI-wall.
        return info2;
    }

	//____ surfaceType() _______________________________________________________

	const TypeInfo& CABIGfxDevice::surfaceType( void ) const
	{
		return CABISurface::TYPEINFO;
	}

	//____ surfaceFactory() ______________________________________________________

	SurfaceFactory_p CABIGfxDevice::surfaceFactory()
	{
        return m_pSurfaceFactory;
	}

    //____ setClipList() _________________________________________________________

    bool CABIGfxDevice::setClipList(int nRectangles, const RectSPX * pRectangles)
    {
        int retVal = CABI::gfxDevice->setClipList(m_cDevice, nRectangles, (wg_rectSPX*)pRectangles);

        if (retVal)
            GfxDevice::setClipList(nRectangles, pRectangles);

        return retVal;
    }

    //____ resetClipList() _______________________________________________________

    void CABIGfxDevice::resetClipList()
    {
        CABI::gfxDevice->resetClipList(m_cDevice);
        GfxDevice::resetClipList();
    }

    //____ pushClipList() ________________________________________________________

    bool CABIGfxDevice::pushClipList(int nRectangles, const RectSPX* pRectangles)
    {
        int retVal = CABI::gfxDevice->pushClipList(m_cDevice, nRectangles, (wg_rectSPX*)pRectangles);

        if( retVal )
            GfxDevice::pushClipList(nRectangles, pRectangles);

        return retVal;
    }

    //____ popClipList() __________________________________________________________

    bool CABIGfxDevice::popClipList()
    {
        int retVal = CABI::gfxDevice->popClipList(m_cDevice);
        if( retVal )
            GfxDevice::popClipList();
        
        return retVal;
    }

    //____ setTintColor() __________________________________________________________

    void CABIGfxDevice::setTintColor( HiColor color )
    {
        GfxDevice::setTintColor(color);

        CABI::gfxDevice->setTintColor(m_cDevice, *(wg_color*)&color);
    }

    //____ setTintGradient() _______________________________________________________

    void CABIGfxDevice::setTintGradient(const RectSPX& rect, const Gradient& gradient)
    {
        GfxDevice::setTintGradient(rect, gradient);
        
        CABI::gfxDevice->setTintGradient(m_cDevice, (const wg_rectSPX*)&rect, (const wg_gradient*)&gradient);
    }

    //____ clearTintGradient() _____________________________________________________

    void CABIGfxDevice::clearTintGradient()
    {
        GfxDevice::clearTintGradient();

        CABI::gfxDevice->clearTintGradient(m_cDevice);
    }

    //____ setBlendMode() __________________________________________________________

    bool CABIGfxDevice::setBlendMode( BlendMode blendMode )
    {
        if( blendMode < BlendMode_min || blendMode > BlendMode_max )
            return false;

        int retVal = CABI::gfxDevice->setBlendMode(m_cDevice, (wg_blendMode)blendMode);

        if( retVal )
            GfxDevice::setBlendMode(blendMode);

        return retVal;
    }

    //____ setBlitSource() _______________________________________________________

    bool CABIGfxDevice::setBlitSource(Surface * pSource)
    {
        if (!pSource || !pSource->isInstanceOf(CABISurface::TYPEINFO) )
            return false;

        int retVal = CABI::gfxDevice->setBlitSource(m_cDevice, static_cast<CABISurface*>(pSource)->cObject());

        if( retVal )
            GfxDevice::setBlitSource(pSource);

        return retVal;
    }

    //____ setMorphFactor() ______________________________________________________

    void CABIGfxDevice::setMorphFactor(float factor)
    {
        GfxDevice::setMorphFactor(factor);

        CABI::gfxDevice->setMorphFactor(m_cDevice, m_morphFactor);
    }

    //____ setRenderLayer() _______________________________________________________

    void CABIGfxDevice::setRenderLayer(int layer)
    {
        GfxDevice::setRenderLayer(layer);
       
        CABI::gfxDevice->setRenderLayer(m_cDevice, m_renderLayer);
    }

	//____ beginRender() ___________________________________________________________

	bool CABIGfxDevice::beginRender()
	{
        int bOk = CABI::gfxDevice->beginRender(m_cDevice);

        if (bOk)
            GfxDevice::beginRender();
        return bOk;
	}

	//____ endRender() _____________________________________________________________

	bool CABIGfxDevice::endRender()
	{
        int bOk = CABI::gfxDevice->endRender(m_cDevice);

        if (bOk)
            GfxDevice::endRender();
        return bOk;
    }

    //____ flush() ___________________________________________________________________

    void CABIGfxDevice::flush()
    {
        CABI::gfxDevice->flushDevice(m_cDevice);
        GfxDevice::flush();
    }

    //____ endCanvasUpdate() _________________________________________________________

    void CABIGfxDevice::endCanvasUpdate()
    {
		GfxDevice::endCanvasUpdate();
        CABI::gfxDevice->endCanvasUpdate(m_cDevice);
    }

    //____ fill() __________________________________________________________________

    void CABIGfxDevice::fill( HiColor _col )
    {
        CABI::gfxDevice->fill( m_cDevice, *(wg_color*)&_col );
    }

    void CABIGfxDevice::fill( const RectSPX& _rect, HiColor _col )
    {
        CABI::gfxDevice->fillRect(m_cDevice, (wg_rectSPX*) &_rect, *(wg_color*)&_col);
    }

    //____ plotPixels() ________________________________________________________

    void CABIGfxDevice::plotPixels(int nCoords, const CoordSPX * pCoords, const HiColor * pColors)
    {
        CABI::gfxDevice->plotPixels(m_cDevice, nCoords, (wg_coordSPX*)pCoords, (wg_color*)pColors);
    }

    //____ drawLine() __________________________________________________________

    void CABIGfxDevice::drawLine(CoordSPX begin, CoordSPX end, HiColor color, spx thickness)
    {
        CABI::gfxDevice->drawLine(m_cDevice, { begin.x, begin.y }, { end.x, end.y }, *(wg_color*)&color, thickness);
    }

    void CABIGfxDevice::drawLine(CoordSPX begin, Direction dir, int length, HiColor color, spx thickness)
    {
        CABI::gfxDevice->drawStraightLine(m_cDevice, { begin.x, begin.y }, (wg_direction) dir, length, * (wg_color*)&color, thickness);
    }

    //____ blit() __________________________________________________________________

    void CABIGfxDevice::blit(CoordSPX dest)
    {
        CABI::gfxDevice->blit(m_cDevice, { dest.x, dest.y });
    }

    void CABIGfxDevice::blit(CoordSPX dest, const RectSPX& src)
    {
        CABI::gfxDevice->blitRect(m_cDevice, { dest.x, dest.y }, (wg_rectSPX*) &src );
    }

    //____ flipBlit() _________________________________________________________

    void CABIGfxDevice::flipBlit(CoordSPX dest, GfxFlip flip )
    {
        CABI::gfxDevice->flipBlit(m_cDevice, { dest.x, dest.y }, (wg_gfxFlip) flip);
    }

    void CABIGfxDevice::flipBlit(CoordSPX dest, const RectSPX& src, GfxFlip flip )
    {
        CABI::gfxDevice->flipBlitRect(m_cDevice, { dest.x, dest.y }, (wg_rectSPX*)&src, (wg_gfxFlip)flip);
    }

    //____ stretchBlit() ______________________________________________________

    void CABIGfxDevice::stretchBlit(const RectSPX& dest)
    {
        CABI::gfxDevice->stretchBlit(m_cDevice, (wg_rectSPX*) &dest);
    }

    void CABIGfxDevice::stretchBlit(const RectSPX& dest, const RectSPX& source)
    {
        CABI::gfxDevice->stretchBlitRect(m_cDevice, (wg_rectSPX*)&dest, (wg_rectSPX*)&source);
    }

    //____ stretchFlipBlit() __________________________________________________

    void CABIGfxDevice::stretchFlipBlit(const RectSPX& dest, GfxFlip flip)
    {
        CABI::gfxDevice->stretchFlipBlit(m_cDevice, (wg_rectSPX*)&dest, (wg_gfxFlip) flip);
    }

    void CABIGfxDevice::stretchFlipBlit(const RectSPX& dest, const RectSPX& source, GfxFlip flip)
    {
        CABI::gfxDevice->stretchFlipBlitRect(m_cDevice, (wg_rectSPX*)&dest, (wg_rectSPX*)&source, (wg_gfxFlip)flip );
    }

	//____ precisionBlit() _______________________________________________________

	void CABIGfxDevice::precisionBlit(const RectSPX& dest, const RectF& srcSPX)
	{
        CABI::gfxDevice->precisionBlit(m_cDevice, (wg_rectSPX*)&dest, (wg_rectF*)&srcSPX);
	}

	//____ transformBlit() _______________________________________________________

	void CABIGfxDevice::transformBlit(const RectSPX& dest, CoordF srcSPX, const float transform[2][2])
	{
        CABI::gfxDevice->transformBlit(m_cDevice, (wg_rectSPX*)&dest, {srcSPX.x,srcSPX.y}, transform);
	}

    //____ rotScaleBlit() _____________________________________________________
    void CABIGfxDevice::rotScaleBlit(const RectSPX& dest, float rotationDegrees, float scale, CoordF srcCenter, CoordF destCenter)
    {
        CABI::gfxDevice->rotScaleBlit(m_cDevice, (wg_rectSPX*)&dest, rotationDegrees, scale, { srcCenter.x, srcCenter.y }, { destCenter.x, destCenter.y });
    }

    //____ tile() _____________________________________________________________

    void CABIGfxDevice::tile(const RectSPX& dest, CoordSPX shift)
    {
        CABI::gfxDevice->tile(m_cDevice, (wg_rectSPX*)&dest, { shift.x,shift.y });
    }

    //____ flipTile() _________________________________________________________

    void CABIGfxDevice::flipTile(const RectSPX& dest, GfxFlip flip, CoordSPX shift)
    {
        CABI::gfxDevice->flipTile(m_cDevice, (wg_rectSPX*)&dest, (wg_gfxFlip) flip, { shift.x, shift.y });
    }

    //____ scaleTile() ________________________________________________________

    void CABIGfxDevice::scaleTile(const RectSPX& dest, float scale, CoordSPX shift)
    {
        CABI::gfxDevice->scaleTile(m_cDevice, (wg_rectSPX*)&dest, scale, { shift.x,shift.y });
    }

    //____ scaleFlipTile() ____________________________________________________

    void CABIGfxDevice::scaleFlipTile(const RectSPX& dest, float scale, GfxFlip flip, CoordSPX shift)
    {
        CABI::gfxDevice->scaleFlipTile(m_cDevice, (wg_rectSPX*)&dest, scale, (wg_gfxFlip) flip, { shift.x, shift.y });
    }

    //____ drawWave() _________________________________________________________

    void CABIGfxDevice::drawWave(const RectSPX& dest, const WaveLine * pTopBorder, const WaveLine * pBottomBorder, HiColor frontFill, HiColor backFill)
    {
        CABI::gfxDevice->drawWave(m_cDevice, (wg_rectSPX*)&dest, (wg_waveLine*)pTopBorder, (wg_waveLine*)pBottomBorder, *(wg_color*)&frontFill, *(wg_color*)&backFill);
    }

    //____ flipDrawWave() _____________________________________________________

    void CABIGfxDevice::flipDrawWave(const RectSPX& dest, const WaveLine * pTopBorder, const WaveLine * pBottomBorder, HiColor frontFill, HiColor backFill, GfxFlip flip)
    {
        CABI::gfxDevice->flipDrawWave(m_cDevice, (wg_rectSPX*)&dest, (wg_waveLine*)pTopBorder, (wg_waveLine*)pBottomBorder, *(wg_color*)&frontFill, *(wg_color*)&backFill, (wg_gfxFlip) flip );
    }

    //____ _drawElipse() ______________________________________________________

    void CABIGfxDevice::drawElipse(const RectSPX& canvas, spx thickness, HiColor color, spx outlineThickness, HiColor outlineColor )
    {
        CABI::gfxDevice->drawElipse(m_cDevice,(wg_rectSPX*)&canvas, thickness, *(wg_color*)&color, outlineThickness, *(wg_color*)&outlineColor);
    }

    //____ drawPieChart() _____________________________________________________

    void CABIGfxDevice::drawPieChart(const RectSPX& canvas, float start, int nSlices, const float * pSliceSizes, const HiColor * pSliceColors, float hubSize, HiColor hubColor, HiColor backColor, bool bRectangular)
    {
        CABI::gfxDevice->drawPieChart(m_cDevice, (wg_rectSPX*)&canvas, start, nSlices, pSliceSizes, (const wg_color*)pSliceColors, hubSize, *(wg_color*)&hubColor, *(wg_color*)&backColor, bRectangular);
    }

    //____ drawSegments() _____________________________________________________

    void CABIGfxDevice::drawSegments(const RectSPX& dest, int nSegments, const HiColor * pSegmentColors, int nEdgeStrips, const int * pEdgeStrips, int edgeStripPitch, TintMode tintMode )
    {
        CABI::gfxDevice->drawSegments(m_cDevice, (wg_rectSPX*)&dest, nSegments, (const wg_color*)pSegmentColors, nEdgeStrips, pEdgeStrips, edgeStripPitch, (wg_tintMode)tintMode);
    }

    //____ flipDrawSegments() _________________________________________________

    void CABIGfxDevice::flipDrawSegments(const RectSPX& dest, int nSegments, const HiColor * pSegmentColors, int nEdgeStrips, const int * pEdgeStrips, int edgeStripPitch, GfxFlip flip, TintMode tintMode)
    {
        CABI::gfxDevice->flipDrawSegments(m_cDevice, (wg_rectSPX*)&dest, nSegments, (const wg_color*)pSegmentColors, nEdgeStrips, pEdgeStrips, edgeStripPitch, (wg_gfxFlip) flip, (wg_tintMode)tintMode);
    }

    //.____ blitNinePatch() ___________________________________________________

    void CABIGfxDevice::blitNinePatch(const RectSPX& dstRect, const BorderSPX& dstFrame, const NinePatch& patch, int scale)
    {
        CABI::gfxDevice->blitNinePatch(m_cDevice, (wg_rectSPX*)&dstRect, (wg_borderSPX*)&dstFrame, (wg_ninePatch*)&patch, scale);
    }

    //.____ _beginCanvasUpdate() ______________________________________________

    bool CABIGfxDevice::_beginCanvasUpdate(CanvasRef canvasRef, Surface * pCanvasSurface, int nUpdateRects, const RectSPX* pUpdateRects, CanvasLayers * pLayers, int startLayer)
    {     
		if( GfxDevice::_beginCanvasUpdate(canvasRef, pCanvasSurface, nUpdateRects, pUpdateRects, pLayers, startLayer) )
		{
			auto pCABILayers = wg_dynamic_cast<CABICanvasLayers_p>(pLayers);
			wg_obj cAPILayers = pCABILayers ? pCABILayers->cObject() : 0;

			if (canvasRef != CanvasRef::None)
			{
                CABI::gfxDevice->beginCanvasUpdateWithRef(m_cDevice, (wg_canvasRef)canvasRef, nUpdateRects, (wg_rectSPX*)pUpdateRects, cAPILayers, startLayer);
			}
			else
			{
                CABI::gfxDevice->beginCanvasUpdateWithSurface(m_cDevice, static_cast<CABISurface*>(pCanvasSurface)->cObject(), nUpdateRects, (wg_rectSPX*)pUpdateRects, cAPILayers, startLayer);
			}
			return true;
		}
		else
			return false;
    }

    //____ _canvasWasChanged() ________________________________________________

    void CABIGfxDevice::_canvasWasChanged()
    {
        // Do nothing
    }

    //____ _renderLayerWasChanged() ___________________________________________

    void CABIGfxDevice::_renderLayerWasChanged()
    {
        // Do nothing.
    }

    //____ _transformBlitSimple() ___________________________________________________

    void CABIGfxDevice::_transformBlitSimple(const RectI& dest, CoordI src, const int simpleTransform[2][2])
    {
        //This method should never be called, but is pure virtual in super class.
        
        assert(false);
    }

    //____ _transformBlitComplex() ___________________________________________________

    void CABIGfxDevice::_transformBlitComplex(const RectI& dest, BinalCoord src, const binalInt complexTransform[2][2])
    {
        //This method should never be called, but is pure virtual in super class.
        
        assert(false);
    }

	//____ _transformDrawSegments() ___________________________________________

	void CABIGfxDevice::_transformDrawSegments(const RectI& dest, int nSegments, const HiColor * pSegmentColors, int nEdgeStrips, const int * pEdgeStrips, int edgeStripPitch, TintMode tintMode, const int simpleTransform[2][2])
	{
        //This method should never be called, but is pure virtual in super class.
        
        assert(false);
	}


} // namespace wg

