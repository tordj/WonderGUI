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

#include <wg_pluginbase.h>
#include <wg_plugingfxdevice.h>
#include <wg_pluginsurface.h>
#include <wg_pluginsurfacefactory.h>
#include <wg_plugincanvaslayers.h>

#include <wg_base.h>
#include <assert.h>

using namespace std;

namespace wg
{
	const TypeInfo PluginGfxDevice::TYPEINFO = { "PluginGfxDevice", &GfxDevice::TYPEINFO };



	//____ create() _______________________________________________________________

	PluginGfxDevice_p PluginGfxDevice::create( wg_obj object, PluginSurfaceFactory * pFactory )
	{
		PluginGfxDevice_p p(new PluginGfxDevice( object, pFactory ));
		return p;
	}


	//____ constructor _____________________________________________________________

	PluginGfxDevice::PluginGfxDevice( wg_obj object, PluginSurfaceFactory * pFactory )
	{
        PluginBase::object->retain(object);
		m_cDevice = object;
        m_pSurfaceFactory = pFactory;
		m_bRendering = false;
		m_bIsProxyDevice = true;
	}

	//____ Destructor ______________________________________________________________

	PluginGfxDevice::~PluginGfxDevice()
	{
        PluginBase::object->release(m_cDevice);
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& PluginGfxDevice::typeInfo(void) const
	{
		return TYPEINFO;
	}

    //____ canvas() ___________________________________________________________

    const CanvasInfo PluginGfxDevice::canvas(CanvasRef ref) const
    {
        auto info = PluginBase::gfxDevice->getCanvasRef(m_cDevice, (wg_canvasRef)ref);

        CanvasInfo info2;
        info2.ref = (CanvasRef) info.ref;
        info2.scale = info.scale;
        info2.size.w = info.size.w;
        info2.size.h = info.size.h;
        info2.pSurface = nullptr;       // Not supported. Can't give access to C++ object on other side of CABI-wall.
        return info2;
    }

	//____ surfaceType() _______________________________________________________

	const TypeInfo& PluginGfxDevice::surfaceType( void ) const
	{
		return PluginSurface::TYPEINFO;
	}

	//____ surfaceFactory() ______________________________________________________

	SurfaceFactory_p PluginGfxDevice::surfaceFactory()
	{
        return m_pSurfaceFactory;
	}

    //____ setClipList() _________________________________________________________

    bool PluginGfxDevice::setClipList(int nRectangles, const RectSPX * pRectangles)
    {
        int retVal = PluginBase::gfxDevice->setClipList(m_cDevice, nRectangles, (wg_rectSPX*)pRectangles);

        if (retVal)
            GfxDevice::setClipList(nRectangles, pRectangles);

        return retVal;
    }

    //____ resetClipList() _______________________________________________________

    void PluginGfxDevice::resetClipList()
    {
        PluginBase::gfxDevice->resetClipList(m_cDevice);
        GfxDevice::resetClipList();
    }

    //____ pushClipList() ________________________________________________________

    bool PluginGfxDevice::pushClipList(int nRectangles, const RectSPX* pRectangles)
    {
        int retVal = PluginBase::gfxDevice->pushClipList(m_cDevice, nRectangles, (wg_rectSPX*)pRectangles);

        if( retVal )
            GfxDevice::pushClipList(nRectangles, pRectangles);

        return retVal;
    }

    //____ popClipList() ______________________________________________________

    bool PluginGfxDevice::popClipList()
    {
        int retVal = PluginBase::gfxDevice->popClipList(m_cDevice);
        if( retVal )
            GfxDevice::popClipList();
        
        return retVal;
    }

    //____ clipList() _________________________________________________________

    const RectSPX* PluginGfxDevice::clipList() const
    {
        return (RectSPX*) PluginBase::gfxDevice->getClipList(m_cDevice);
    }

    //____ clipListSize() _____________________________________________________

    int PluginGfxDevice::clipListSize() const
    {
        return PluginBase::gfxDevice->clipListSize(m_cDevice);
    }

    //____ clipBounds() _______________________________________________________

    const RectSPX& PluginGfxDevice::clipBounds() const
    {
        return  * (RectSPX*) PluginBase::gfxDevice->clipBounds(m_cDevice);
    }

    //____ setTintColor() _____________________________________________________

    void PluginGfxDevice::setTintColor( HiColor color )
    {
        GfxDevice::setTintColor(color);

        PluginBase::gfxDevice->setTintColor(m_cDevice, *(wg_color*)&color);
    }

    //____ tintColor() ________________________________________________________

    HiColor PluginGfxDevice::tintColor() const
    {
        wg_color col = PluginBase::gfxDevice->getTintColor(m_cDevice);

        return *(HiColor*)&col;
    }

    //____ setTintGradient() __________________________________________________

    void PluginGfxDevice::setTintGradient(const RectSPX& rect, const Gradient& gradient)
    {
        GfxDevice::setTintGradient(rect, gradient);
        
        PluginBase::gfxDevice->setTintGradient(m_cDevice, (const wg_rectSPX*)&rect, (const wg_gradient*)&gradient);
    }

    //____ clearTintGradient() ________________________________________________

    void PluginGfxDevice::clearTintGradient()
    {
        GfxDevice::clearTintGradient();

        PluginBase::gfxDevice->clearTintGradient(m_cDevice);
    }

    //____ setBlendMode() __________________________________________________________

    bool PluginGfxDevice::setBlendMode( BlendMode blendMode )
    {
        if( blendMode < BlendMode_min || blendMode > BlendMode_max )
            return false;

        int retVal = PluginBase::gfxDevice->setBlendMode(m_cDevice, (wg_blendMode)blendMode);

        if( retVal )
            GfxDevice::setBlendMode(blendMode);

        return retVal;
    }

    //____ blendMode() ________________________________________________________

    BlendMode  PluginGfxDevice::blendMode() const
    {
        return (BlendMode) PluginBase::gfxDevice->getBlendMode(m_cDevice);
    }


    //____ setBlitSource() _______________________________________________________

    bool PluginGfxDevice::setBlitSource(Surface * pSource)
    {
        if (!pSource || !pSource->isInstanceOf(PluginSurface::TYPEINFO) )
            return false;

        int retVal = PluginBase::gfxDevice->setBlitSource(m_cDevice, static_cast<PluginSurface*>(pSource)->cObject());

        if( retVal )
            GfxDevice::setBlitSource(pSource);

        return retVal;
    }

    //____ PluginGfxDevice::blitSource() ________________________________________

    Surface_p PluginGfxDevice::blitSource() const
    {
        // This method can not be correctly implemented without huge performance loss.

        return nullptr;
    }

    //____ setMorphFactor() ______________________________________________________

    void PluginGfxDevice::setMorphFactor(float factor)
    {
        GfxDevice::setMorphFactor(factor);

        PluginBase::gfxDevice->setMorphFactor(m_cDevice, m_morphFactor);
    }

    //____ morphFactor() ______________________________________________________

    float PluginGfxDevice::morphFactor() const
    {
        return PluginBase::gfxDevice->getMorphFactor(m_cDevice);
    }

    //____ setRenderLayer() _______________________________________________________

    void PluginGfxDevice::setRenderLayer(int layer)
    {
        GfxDevice::setRenderLayer(layer);
       
        PluginBase::gfxDevice->setRenderLayer(m_cDevice, m_renderLayer);
    }

    //____ renderLayer() ______________________________________________________

    int PluginGfxDevice::renderLayer() const
    {
        return PluginBase::gfxDevice->getRenderLayer(m_cDevice);
    }

	//____ beginRender() ___________________________________________________________

	bool PluginGfxDevice::beginRender()
	{
        int bOk = PluginBase::gfxDevice->beginRender(m_cDevice);

        if (bOk)
            GfxDevice::beginRender();
        return bOk;
	}

	//____ endRender() _____________________________________________________________

	bool PluginGfxDevice::endRender()
	{
        int bOk = PluginBase::gfxDevice->endRender(m_cDevice);

        if (bOk)
            GfxDevice::endRender();
        return bOk;
    }

    //____ flush() ___________________________________________________________________

    void PluginGfxDevice::flush()
    {
        PluginBase::gfxDevice->flushDevice(m_cDevice);
        GfxDevice::flush();
    }

    //____ endCanvasUpdate() _________________________________________________________

    void PluginGfxDevice::endCanvasUpdate()
    {
		GfxDevice::endCanvasUpdate();
        PluginBase::gfxDevice->endCanvasUpdate(m_cDevice);
    }

    //____ fill() __________________________________________________________________

    void PluginGfxDevice::fill( HiColor _col )
    {
        PluginBase::gfxDevice->fill( m_cDevice, *(wg_color*)&_col );
    }

    void PluginGfxDevice::fill( const RectSPX& _rect, HiColor _col )
    {
        PluginBase::gfxDevice->fillRect(m_cDevice, (wg_rectSPX*) &_rect, *(wg_color*)&_col);
    }

    //____ plotPixels() ________________________________________________________

    void PluginGfxDevice::plotPixels(int nCoords, const CoordSPX * pCoords, const HiColor * pColors)
    {
        PluginBase::gfxDevice->plotPixels(m_cDevice, nCoords, (wg_coordSPX*)pCoords, (wg_color*)pColors);
    }

    //____ drawLine() __________________________________________________________

    void PluginGfxDevice::drawLine(CoordSPX begin, CoordSPX end, HiColor color, spx thickness)
    {
        PluginBase::gfxDevice->drawLine(m_cDevice, { begin.x, begin.y }, { end.x, end.y }, *(wg_color*)&color, thickness);
    }

    void PluginGfxDevice::drawLine(CoordSPX begin, Direction dir, int length, HiColor color, spx thickness)
    {
        PluginBase::gfxDevice->drawStraightLine(m_cDevice, { begin.x, begin.y }, (wg_direction) dir, length, * (wg_color*)&color, thickness);
    }

    //____ blit() __________________________________________________________________

    void PluginGfxDevice::blit(CoordSPX dest)
    {
        PluginBase::gfxDevice->blit(m_cDevice, { dest.x, dest.y });
    }

    void PluginGfxDevice::blit(CoordSPX dest, const RectSPX& src)
    {
        PluginBase::gfxDevice->blitRect(m_cDevice, { dest.x, dest.y }, (wg_rectSPX*) &src );
    }

    //____ flipBlit() _________________________________________________________

    void PluginGfxDevice::flipBlit(CoordSPX dest, GfxFlip flip )
    {
        PluginBase::gfxDevice->flipBlit(m_cDevice, { dest.x, dest.y }, (wg_gfxFlip) flip);
    }

    void PluginGfxDevice::flipBlit(CoordSPX dest, const RectSPX& src, GfxFlip flip )
    {
        PluginBase::gfxDevice->flipBlitRect(m_cDevice, { dest.x, dest.y }, (wg_rectSPX*)&src, (wg_gfxFlip)flip);
    }

    //____ stretchBlit() ______________________________________________________

    void PluginGfxDevice::stretchBlit(const RectSPX& dest)
    {
        PluginBase::gfxDevice->stretchBlit(m_cDevice, (wg_rectSPX*) &dest);
    }

    void PluginGfxDevice::stretchBlit(const RectSPX& dest, const RectSPX& source)
    {
        PluginBase::gfxDevice->stretchBlitRect(m_cDevice, (wg_rectSPX*)&dest, (wg_rectSPX*)&source);
    }

    //____ stretchFlipBlit() __________________________________________________

    void PluginGfxDevice::stretchFlipBlit(const RectSPX& dest, GfxFlip flip)
    {
        PluginBase::gfxDevice->stretchFlipBlit(m_cDevice, (wg_rectSPX*)&dest, (wg_gfxFlip) flip);
    }

    void PluginGfxDevice::stretchFlipBlit(const RectSPX& dest, const RectSPX& source, GfxFlip flip)
    {
        PluginBase::gfxDevice->stretchFlipBlitRect(m_cDevice, (wg_rectSPX*)&dest, (wg_rectSPX*)&source, (wg_gfxFlip)flip );
    }

	//____ precisionBlit() _______________________________________________________

	void PluginGfxDevice::precisionBlit(const RectSPX& dest, const RectF& srcSPX)
	{
        PluginBase::gfxDevice->precisionBlit(m_cDevice, (wg_rectSPX*)&dest, (wg_rectF*)&srcSPX);
	}

	//____ transformBlit() _______________________________________________________

	void PluginGfxDevice::transformBlit(const RectSPX& dest, CoordF srcSPX, const float transform[2][2])
	{
        PluginBase::gfxDevice->transformBlit(m_cDevice, (wg_rectSPX*)&dest, {srcSPX.x,srcSPX.y}, transform);
	}

    //____ rotScaleBlit() _____________________________________________________
    void PluginGfxDevice::rotScaleBlit(const RectSPX& dest, float rotationDegrees, float scale, CoordF srcCenter, CoordF destCenter)
    {
        PluginBase::gfxDevice->rotScaleBlit(m_cDevice, (wg_rectSPX*)&dest, rotationDegrees, scale, { srcCenter.x, srcCenter.y }, { destCenter.x, destCenter.y });
    }

    //____ tile() _____________________________________________________________

    void PluginGfxDevice::tile(const RectSPX& dest, CoordSPX shift)
    {
        PluginBase::gfxDevice->tile(m_cDevice, (wg_rectSPX*)&dest, { shift.x,shift.y });
    }

    //____ flipTile() _________________________________________________________

    void PluginGfxDevice::flipTile(const RectSPX& dest, GfxFlip flip, CoordSPX shift)
    {
        PluginBase::gfxDevice->flipTile(m_cDevice, (wg_rectSPX*)&dest, (wg_gfxFlip) flip, { shift.x, shift.y });
    }

    //____ scaleTile() ________________________________________________________

    void PluginGfxDevice::scaleTile(const RectSPX& dest, float scale, CoordSPX shift)
    {
        PluginBase::gfxDevice->scaleTile(m_cDevice, (wg_rectSPX*)&dest, scale, { shift.x,shift.y });
    }

    //____ scaleFlipTile() ____________________________________________________

    void PluginGfxDevice::scaleFlipTile(const RectSPX& dest, float scale, GfxFlip flip, CoordSPX shift)
    {
        PluginBase::gfxDevice->scaleFlipTile(m_cDevice, (wg_rectSPX*)&dest, scale, (wg_gfxFlip) flip, { shift.x, shift.y });
    }

    //____ drawWave() _________________________________________________________

    void PluginGfxDevice::drawWave(const RectSPX& dest, const WaveLine * pTopBorder, const WaveLine * pBottomBorder, HiColor frontFill, HiColor backFill)
    {
        PluginBase::gfxDevice->drawWave(m_cDevice, (wg_rectSPX*)&dest, (wg_waveLine*)pTopBorder, (wg_waveLine*)pBottomBorder, *(wg_color*)&frontFill, *(wg_color*)&backFill);
    }

    //____ flipDrawWave() _____________________________________________________

    void PluginGfxDevice::flipDrawWave(const RectSPX& dest, const WaveLine * pTopBorder, const WaveLine * pBottomBorder, HiColor frontFill, HiColor backFill, GfxFlip flip)
    {
        PluginBase::gfxDevice->flipDrawWave(m_cDevice, (wg_rectSPX*)&dest, (wg_waveLine*)pTopBorder, (wg_waveLine*)pBottomBorder, *(wg_color*)&frontFill, *(wg_color*)&backFill, (wg_gfxFlip) flip );
    }

    //____ _drawElipse() ______________________________________________________

    void PluginGfxDevice::drawElipse(const RectSPX& canvas, spx thickness, HiColor color, spx outlineThickness, HiColor outlineColor )
    {
        PluginBase::gfxDevice->drawElipse(m_cDevice,(wg_rectSPX*)&canvas, thickness, *(wg_color*)&color, outlineThickness, *(wg_color*)&outlineColor);
    }

    //____ drawPieChart() _____________________________________________________

    void PluginGfxDevice::drawPieChart(const RectSPX& canvas, float start, int nSlices, const float * pSliceSizes, const HiColor * pSliceColors, float hubSize, HiColor hubColor, HiColor backColor, bool bRectangular)
    {
        PluginBase::gfxDevice->drawPieChart(m_cDevice, (wg_rectSPX*)&canvas, start, nSlices, pSliceSizes, (const wg_color*)pSliceColors, hubSize, *(wg_color*)&hubColor, *(wg_color*)&backColor, bRectangular);
    }

    //____ drawSegments() _____________________________________________________

    void PluginGfxDevice::drawSegments(const RectSPX& dest, int nSegments, const HiColor * pSegmentColors, int nEdgeStrips, const int * pEdgeStrips, int edgeStripPitch, TintMode tintMode )
    {
        PluginBase::gfxDevice->drawSegments(m_cDevice, (wg_rectSPX*)&dest, nSegments, (const wg_color*)pSegmentColors, nEdgeStrips, pEdgeStrips, edgeStripPitch, (wg_tintMode)tintMode);
    }

    //____ flipDrawSegments() _________________________________________________

    void PluginGfxDevice::flipDrawSegments(const RectSPX& dest, int nSegments, const HiColor * pSegmentColors, int nEdgeStrips, const int * pEdgeStrips, int edgeStripPitch, GfxFlip flip, TintMode tintMode)
    {
        PluginBase::gfxDevice->flipDrawSegments(m_cDevice, (wg_rectSPX*)&dest, nSegments, (const wg_color*)pSegmentColors, nEdgeStrips, pEdgeStrips, edgeStripPitch, (wg_gfxFlip) flip, (wg_tintMode)tintMode);
    }

    //.____ blitNinePatch() ___________________________________________________

    void PluginGfxDevice::blitNinePatch(const RectSPX& dstRect, const BorderSPX& dstFrame, const NinePatch& patch, int scale)
    {
        PluginBase::gfxDevice->blitNinePatch(m_cDevice, (wg_rectSPX*)&dstRect, (wg_borderSPX*)&dstFrame, (wg_ninePatch*)&patch, scale);
    }

    //.____ _beginCanvasUpdate() ______________________________________________

    bool PluginGfxDevice::_beginCanvasUpdate(CanvasRef canvasRef, Surface * pCanvasSurface, int nUpdateRects, const RectSPX* pUpdateRects, CanvasLayers * pLayers, int startLayer)
    {     
		if( GfxDevice::_beginCanvasUpdate(canvasRef, pCanvasSurface, nUpdateRects, pUpdateRects, pLayers, startLayer) )
		{
			auto pCABILayers = wg_dynamic_cast<PluginCanvasLayers_p>(pLayers);
			wg_obj cAPILayers = pCABILayers ? pCABILayers->cObject() : 0;

			if (canvasRef != CanvasRef::None)
			{
                PluginBase::gfxDevice->beginCanvasUpdateWithRef(m_cDevice, (wg_canvasRef)canvasRef, nUpdateRects, (wg_rectSPX*)pUpdateRects, cAPILayers, startLayer);
			}
			else
			{
                PluginBase::gfxDevice->beginCanvasUpdateWithSurface(m_cDevice, static_cast<PluginSurface*>(pCanvasSurface)->cObject(), nUpdateRects, (wg_rectSPX*)pUpdateRects, cAPILayers, startLayer);
			}
			return true;
		}
		else
			return false;
    }

    //____ _canvasWasChanged() ________________________________________________

    void PluginGfxDevice::_canvasWasChanged()
    {
        // Do nothing
    }

    //____ _renderLayerWasChanged() ___________________________________________

    void PluginGfxDevice::_renderLayerWasChanged()
    {
        // Do nothing.
    }

    //____ _transformBlitSimple() ___________________________________________________

    void PluginGfxDevice::_transformBlitSimple(const RectI& dest, CoordI src, const int simpleTransform[2][2])
    {
        //This method should never be called, but is pure virtual in super class.
        
        assert(false);
    }

    //____ _transformBlitComplex() ___________________________________________________

    void PluginGfxDevice::_transformBlitComplex(const RectI& dest, BinalCoord src, const binalInt complexTransform[2][2])
    {
        //This method should never be called, but is pure virtual in super class.
        
        assert(false);
    }

	//____ _transformDrawSegments() ___________________________________________

	void PluginGfxDevice::_transformDrawSegments(const RectI& dest, int nSegments, const HiColor * pSegmentColors, int nEdgeStrips, const int * pEdgeStrips, int edgeStripPitch, TintMode tintMode, const int simpleTransform[2][2])
	{
        //This method should never be called, but is pure virtual in super class.
        
        assert(false);
	}


} // namespace wg

