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

#include <wg_gfxstreamdevice.h>
#include <wg_gfxstreamsurface.h>
#include <wg_gfxstreamsurfacefactory.h>
#include <wg_gfxbase.h>
#include <assert.h>
#include <math.h>
#include <algorithm>

using namespace std;

namespace wg
{
	const TypeInfo GfxStreamDevice::TYPEINFO = { "GfxStreamDevice", &GfxDevice::TYPEINFO };



	//____ create() _______________________________________________________________

	GfxStreamDevice_p GfxStreamDevice::create( GfxStreamEncoder * pEncoder )
	{
		GfxStreamDevice_p p(new GfxStreamDevice( pEncoder ));
		return p;
	}


	//____ constructor _____________________________________________________________

	GfxStreamDevice::GfxStreamDevice(GfxStreamEncoder* pEncoder)
	{
		m_pEncoder = pEncoder;
		m_bRendering = false;

        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::ProtocolVersion, 0, 2 };
        (*m_pEncoder) << (uint16_t) 0x0100;
	}

	//____ Destructor ______________________________________________________________

	GfxStreamDevice::~GfxStreamDevice()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& GfxStreamDevice::typeInfo(void) const
	{
		return TYPEINFO;
	}

    //____ canvas() ___________________________________________________________

    const CanvasInfo GfxStreamDevice::canvas(CanvasRef ref) const
    {
        auto it = std::find_if(m_definedCanvases.begin(), m_definedCanvases.end(), [ref](const CanvasInfo& entry) { return (ref == entry.ref); });

        if (it != m_definedCanvases.end())
            return *it;

        return m_dummyCanvas;
    }

	//____ surfaceType() _______________________________________________________

	const TypeInfo& GfxStreamDevice::surfaceType( void ) const
	{
		return GfxStreamSurface::TYPEINFO;
	}

	//____ defineCanvas() ____________________________________________________

	bool GfxStreamDevice::defineCanvas( CanvasRef ref, GfxStreamSurface * pSurface )
	{ 
        auto it = std::find_if(m_definedCanvases.begin(), m_definedCanvases.end(), [ref](CanvasInfo& entry) { return (ref == entry.ref); });

        if (it == m_definedCanvases.end())
        {
            if (pSurface)
                m_definedCanvases.push_back(CanvasInfo(ref, pSurface, pSurface->pixelSize() * 64, pSurface->scale()));
        }
        else
        {
            if (pSurface)
            {
                it->pSurface = pSurface;
                it->size = pSurface->pixelSize() * 64;
                it->scale = pSurface->scale();
            }
            else
            {
                m_definedCanvases.erase(it);
            }
        }

        return true;
    }

	bool GfxStreamDevice::defineCanvas( CanvasRef ref, const SizeI& size, int scale )
	{
	
		auto it = std::find_if( m_definedCanvases.begin(), m_definedCanvases.end(), [ref] (CanvasInfo& entry) { return (ref == entry.ref); } );

		if( it == m_definedCanvases.end() )
		{
			if( !size.isEmpty() )
				m_definedCanvases.push_back( CanvasInfo( ref, nullptr, size * 64, scale ) );
		}
		else
		{
			if( !size.isEmpty() )
			{
				it->pSurface = nullptr;
				it->size = size * 64;
                it->scale = scale;
			}
			else
			{
				m_definedCanvases.erase(it);
			}
		}

		return true;
	}

	//____ encodeCanvasList() ______________________________________________________

	void GfxStreamDevice::encodeCanvasList()
	{
		(*m_pEncoder) << GfxStream::Header{ GfxChunkId::SetClipList, 0, (uint16_t)(m_definedCanvases.size()*12) };

		(*m_pEncoder) << (uint16_t) m_definedCanvases.size();
		
		for( auto& canvas : m_definedCanvases )
		{
			(*m_pEncoder) << (uint16_t) canvas.ref;
			(*m_pEncoder) << canvas.size;
			(*m_pEncoder) << (uint16_t) canvas.scale;
		}		
	}

	//____ surfaceFactory() ______________________________________________________

	SurfaceFactory_p GfxStreamDevice::surfaceFactory()
	{
		if( !m_pSurfaceFactory )
			m_pSurfaceFactory = GfxStreamSurfaceFactory::create(m_pEncoder);

		return m_pSurfaceFactory;
	}

    //____ setClipList() _________________________________________________________

    bool GfxStreamDevice::setClipList(int nRectangles, const RectSPX * pRectangles)
    {
        GfxDevice::setClipList(nRectangles, pRectangles);
        
        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::SetClipList, 0, (uint16_t)(nRectangles * 16) };
        (*m_pEncoder) << GfxStream::DataChunk{ nRectangles * 16, pRectangles };
        return true;
    }

    //____ resetClipList() _______________________________________________________

    void GfxStreamDevice::resetClipList()
    {
        GfxDevice::resetClipList();
        
        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::ResetClipList, 0, 0 };
    }

    //____ pushClipList() ________________________________________________________

    bool GfxStreamDevice::pushClipList(int nRectangles, const RectSPX* pRectangles)
    {
        GfxDevice::pushClipList(nRectangles, pRectangles);
        
        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::PushClipList, 0, (uint16_t)(nRectangles * 16) };
        (*m_pEncoder) << GfxStream::DataChunk{ nRectangles * 16, pRectangles };
        return true;
    }

    //____ popClipList() __________________________________________________________

    bool GfxStreamDevice::popClipList()
    {
        GfxDevice::popClipList();
        
        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::PopClipList, 0, 0};
        return true;
    }

    //____ setTintColor() __________________________________________________________

    void GfxStreamDevice::setTintColor( HiColor color )
    {
		if( color == m_tintColor )
			return;
		
        GfxDevice::setTintColor(color);

        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::SetTintColor, 0, 8 };
        (*m_pEncoder) << color;
    }

    //____ setTintGradient() _______________________________________________________

    void GfxStreamDevice::setTintGradient(const RectSPX& rect, const Gradient& gradient)
    {
		if( rect == m_tintGradientRect && m_tintGradient == gradient )
			return;
		
        GfxDevice::setTintGradient(rect, gradient);
        
        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::SetTintGradient, 0, 16 + 34 };
        (*m_pEncoder) << rect;
        (*m_pEncoder) << gradient;
    }

    //____ clearTintGradient() _____________________________________________________

    void GfxStreamDevice::clearTintGradient()
    {
		if( m_bTintGradient == false )
			return;
		
        GfxDevice::clearTintGradient();
        
        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::ClearTintGradient, 0, 0 };
    }

    //____ setBlendMode() __________________________________________________________

    bool GfxStreamDevice::setBlendMode( BlendMode blendMode )
    {
        if( blendMode < BlendMode_min || blendMode > BlendMode_max )
        {
            GfxBase::handleError(ErrorSeverity::Serious, ErrorCode::InvalidParam, "Not a valid blendMode", this, &TYPEINFO, __func__, __FILE__, __LINE__);
            return false;
        }

		if( blendMode == m_blendMode )
			return true;
		
        GfxDevice::setBlendMode(blendMode);

        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::SetBlendMode, 0, 2 };
        (*m_pEncoder) << blendMode;

        return true;
    }

    //____ setBlitSource() _______________________________________________________

    bool GfxStreamDevice::setBlitSource(Surface * pSource)
    {
        if (!pSource || !pSource->isInstanceOf(GfxStreamSurface::TYPEINFO))
        {
            GfxBase::handleError(ErrorSeverity::Serious, ErrorCode::InvalidParam, "Surface is not a GfxStreamSurface", this, &TYPEINFO, __func__, __FILE__, __LINE__);
            return false;
        }

		if( pSource == m_pBlitSource )
			return true;
		
        GfxDevice::setBlitSource(pSource);

        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::SetBlitSource, 0, 2 };
        (*m_pEncoder) << static_cast<GfxStreamSurface*>(pSource)->m_inStreamId;
        return true;
    }

    //____ setMorphFactor() ______________________________________________________

    void GfxStreamDevice::setMorphFactor(float factor)
    {
		if( factor == m_morphFactor )
			return;
		
        GfxDevice::setMorphFactor(factor);
        
        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::SetMorphFactor, 0, 4 };
        (*m_pEncoder) << factor;
    }

    //____ setRenderLayer() _______________________________________________________

    void GfxStreamDevice::setRenderLayer(int layer)
    {
		if( layer == m_renderLayer )
			return;
		
        GfxDevice::setRenderLayer(layer);
        
        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::SetRenderLayer, 0, 2 };
        (*m_pEncoder) << (uint16_t) layer;
    }

	//____ beginRender() ___________________________________________________________

	bool GfxStreamDevice::beginRender()
	{
		if( m_bRendering == true )
			return false;

		(*m_pEncoder) << GfxStream::Header{ GfxChunkId::BeginRender, 0, 0 };

        GfxDevice::setBlitSource(nullptr);                 // We don't optimize blit source between frames.

		m_bRendering = true;
		return true;
	}

	//____ endRender() _____________________________________________________________

	bool GfxStreamDevice::endRender()
	{
		if( m_bRendering == false )
			return false;

		(*m_pEncoder) << GfxStream::Header{ GfxChunkId::EndRender, 0, 0 };
		m_pEncoder->flush();

		m_bRendering = false;
		return true;
	}

    //____ flush() ___________________________________________________________________

    void GfxStreamDevice::flush()
    {
        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::Flush, 0, 0 };
        m_pEncoder->flush();
    }

    //____ endCanvasUpdate() _________________________________________________________

    void GfxStreamDevice::endCanvasUpdate()
    {
        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::EndCanvasUpdate, 0, 0 };

    }

    //____ fill() __________________________________________________________________

    void GfxStreamDevice::fill( HiColor _col )
    {
        if( _col.a  == 0 )
            return;

        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::Fill, 0, 8 };
        (*m_pEncoder) << _col;

        return;
    }

    void GfxStreamDevice::fill( const RectSPX& _rect, HiColor _col )
    {
        if( _col.a  == 0 || _rect.w < 1 || _rect.h < 1 )
            return;

        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::FillRect, 0, 24 };
        (*m_pEncoder) << _rect;
        (*m_pEncoder) << _col;

        return;
    }

    //____ plotPixels() ________________________________________________________

    void GfxStreamDevice::plotPixels(int nCoords, const CoordSPX * pCoords, const HiColor * pColors)
    {
        // Each pixel is 8 + 8 bytes: int32_t x, int32_t y, HiColor
        // All coordinates comes first, then all colors.

        //TODO: Optimize. Clip coordinates before we send them.
        //TODO: Optimize. Remove binals from pixels, store as 16-bit values.

        if (nCoords == 0)
            return;

        int maxChunkCoords = (int)(GfxStream::c_maxBlockSize - sizeof(GfxStream::Header)) / 16;

        int chunkCoords = min(nCoords, maxChunkCoords);

        while (nCoords > 0)
        {
            (*m_pEncoder) << GfxStream::Header{ GfxChunkId::PlotPixels, 0, (uint16_t)(chunkCoords * 16) };
            (*m_pEncoder) << GfxStream::DataChunk{ chunkCoords * 8, pCoords };
            (*m_pEncoder) << GfxStream::DataChunk{ chunkCoords * 8, pColors };

            nCoords -= chunkCoords;
            pCoords += chunkCoords;
            pColors += chunkCoords;
            chunkCoords = min(nCoords, maxChunkCoords);
        }
    }

    //____ drawLine() __________________________________________________________

    void GfxStreamDevice::drawLine(CoordSPX begin, CoordSPX end, HiColor color, spx thickness)
    {
        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::DrawLineFromTo, 0, 28 };
        (*m_pEncoder) << begin;
        (*m_pEncoder) << end;
        (*m_pEncoder) << color;
        (*m_pEncoder) << thickness;
    }

    void GfxStreamDevice::drawLine(CoordSPX begin, Direction dir, int length, HiColor col, spx thickness)
    {
        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::DrawLineStraight, 0, 26 };
        (*m_pEncoder) << begin;
        (*m_pEncoder) << dir;
        (*m_pEncoder) << length;
        (*m_pEncoder) << col;
        (*m_pEncoder) << thickness;
    }

    //____ blit() __________________________________________________________________

    void GfxStreamDevice::blit(CoordSPX dest)
    {
        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::Blit, 0, 8 };
        (*m_pEncoder) << dest;
    }

    void GfxStreamDevice::blit(CoordSPX dest, const RectSPX& _src)
    {
        if (_src.w < 1 || _src.h < 1)
            return;

        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::BlitRect, 0, 24 };
        (*m_pEncoder) << dest;
        (*m_pEncoder) << _src;
    }

    //____ flipBlit() _________________________________________________________

    void GfxStreamDevice::flipBlit(CoordSPX dest, GfxFlip flip )
    {
        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::FlipBlit, 0, 10 };
        (*m_pEncoder) << dest;
        (*m_pEncoder) << flip;
    }

    void GfxStreamDevice::flipBlit(CoordSPX dest, const RectSPX& src, GfxFlip flip )
    {
        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::FlipBlitRect, 0, 26 };
        (*m_pEncoder) << dest;
        (*m_pEncoder) << src;
        (*m_pEncoder) << flip;
    }

    //____ stretchBlit() ___________________________________________________

    void GfxStreamDevice::stretchBlit(const RectSPX& dest)
    {

        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::StretchBlit, 0, 16 };
        (*m_pEncoder) << dest;
    }

    void GfxStreamDevice::stretchBlit(const RectSPX& dest, const RectSPX& source)
    {

        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::StretchBlitRect, 0, 32 };
        (*m_pEncoder) << dest;
        (*m_pEncoder) << source;
    }

    //____ stretchFlipBlit() ___________________________________________________

    void GfxStreamDevice::stretchFlipBlit(const RectSPX& dest, GfxFlip flip)
    {

        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::StretchFlipBlit, 0, 18 };
        (*m_pEncoder) << dest;
        (*m_pEncoder) << flip;
    }

    void GfxStreamDevice::stretchFlipBlit(const RectSPX& dest, const RectSPX& source, GfxFlip flip)
    {
        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::StretchBlitRect, 0, 34 };
        (*m_pEncoder) << dest;
        (*m_pEncoder) << source;
        (*m_pEncoder) << flip;
    }

	//____ precisionBlit() _______________________________________________________

	void GfxStreamDevice::precisionBlit(const RectSPX& dest, const RectF& srcSPX)
	{
		(*m_pEncoder) << GfxStream::Header{ GfxChunkId::PrecisionBlit, 0, 32 };
		(*m_pEncoder) << dest;
		(*m_pEncoder) << srcSPX;
	}

	//____ transformBlit() _______________________________________________________

	void GfxStreamDevice::transformBlit(const RectSPX& dest, CoordF srcSPX, const float transform[2][2])
	{
		(*m_pEncoder) << GfxStream::Header{ GfxChunkId::PrecisionBlit, 0, 40 };
		(*m_pEncoder) << dest;
		(*m_pEncoder) << srcSPX;
		(*m_pEncoder) << transform[0][0];
		(*m_pEncoder) << transform[0][1];
		(*m_pEncoder) << transform[1][0];
		(*m_pEncoder) << transform[1][1];
	}

    //____ rotScaleBlit() ___________________________________________________________

    void GfxStreamDevice::rotScaleBlit(const RectSPX& dest, float rotationDegrees, float scale, CoordF srcCenter, CoordF destCenter)
    {
        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::RotScaleBlit, 0, 40 };
        (*m_pEncoder) << dest;
        (*m_pEncoder) << rotationDegrees;
        (*m_pEncoder) << scale;
        (*m_pEncoder) << srcCenter;
        (*m_pEncoder) << destCenter;
    }

    //____ tile() _____________________________________________________________________

    void GfxStreamDevice::tile(const RectSPX& dest, CoordSPX shift)
    {
        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::Tile, 0, 24 };
        (*m_pEncoder) << dest;
        (*m_pEncoder) << shift;
    }

    void GfxStreamDevice::flipTile(const RectSPX& dest, GfxFlip flip, CoordSPX shift)
    {
        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::FlipTile, 0, 26 };
        (*m_pEncoder) << dest;
        (*m_pEncoder) << flip;
        (*m_pEncoder) << shift;
    }

    void GfxStreamDevice::scaleTile(const RectSPX& dest, float scale, CoordSPX shift)
    {
        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::ScaleTile, 0, 28 };
        (*m_pEncoder) << dest;
        (*m_pEncoder) << scale;
        (*m_pEncoder) << shift;
    }

    void GfxStreamDevice::scaleFlipTile(const RectSPX& dest, float scale, GfxFlip flip, CoordSPX shift)
    {
        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::ScaleFlipTile, 0, 30 };
        (*m_pEncoder) << dest;
        (*m_pEncoder) << scale;
        (*m_pEncoder) << flip;
        (*m_pEncoder) << shift;
    }

    //____ drawWave() ___________________________________________________________________

    void GfxStreamDevice::drawWave(const RectSPX& dest, const WaveLine * pTopBorder, const WaveLine * pBottomBorder, HiColor frontFill, HiColor backFill)
    {
        int size = 16 + 20 + 20 + 8 + 8;
        
        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::DrawWave, 0, (uint16_t) size };
        (*m_pEncoder) << dest;

        (*m_pEncoder) << pTopBorder->length;
        (*m_pEncoder) << pTopBorder->thickness;
        (*m_pEncoder) << pTopBorder->color;
        (*m_pEncoder) << pTopBorder->hold;

        (*m_pEncoder) << pBottomBorder->length;
        (*m_pEncoder) << pBottomBorder->thickness;
        (*m_pEncoder) << pBottomBorder->color;
        (*m_pEncoder) << pBottomBorder->hold;

        (*m_pEncoder) << frontFill;
        (*m_pEncoder) << backFill;

        if( pTopBorder->length > 0 )
            _streamEdgeSamples( pTopBorder->length, 1, 1, pTopBorder->pWave );
        
        if( pBottomBorder->length > 0 )
            _streamEdgeSamples( pBottomBorder->length, 1, 1, pBottomBorder->pWave );
    }

    //____ flipDrawWave() ___________________________________________________________________

    void GfxStreamDevice::flipDrawWave(const RectSPX& dest, const WaveLine * pTopBorder, const WaveLine * pBottomBorder, HiColor frontFill, HiColor backFill, GfxFlip flip)
    {
        uint16_t size = 16 + 20 + 20 + 8 + 8 + 2;
        
        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::FlipDrawWave, 0, size };
        (*m_pEncoder) << dest;

        (*m_pEncoder) << pTopBorder->length;
        (*m_pEncoder) << pTopBorder->thickness;
        (*m_pEncoder) << pTopBorder->color;
        (*m_pEncoder) << pTopBorder->hold;

        (*m_pEncoder) << pBottomBorder->length;
        (*m_pEncoder) << pBottomBorder->thickness;
        (*m_pEncoder) << pBottomBorder->color;
        (*m_pEncoder) << pBottomBorder->hold;

        (*m_pEncoder) << frontFill;
        (*m_pEncoder) << backFill;
        (*m_pEncoder) << flip;

        if( pTopBorder->length > 0 )
            _streamEdgeSamples( pTopBorder->length, 1, 1, pTopBorder->pWave );
        
        if( pBottomBorder->length > 0 )
            _streamEdgeSamples( pBottomBorder->length, 1, 1, pBottomBorder->pWave );
    }

    //____ _streamEdgeSamples() ______________________________________________________

    void GfxStreamDevice::_streamEdgeSamples( int nLines, int samplesPerLine, int linePitch, const int * pSamples )
    {
        int maxSamplesPerChunk = (GfxStream::c_maxBlockSize - sizeof(GfxStream::Header)) / 4;
        int allocSize = 0;
        
        if( samplesPerLine != linePitch )
        {
            allocSize = nLines * samplesPerLine * 4;

            int * pSampleBuffer = (int*) GfxBase::memStackAlloc(allocSize);
            int * wp = pSampleBuffer;

            for (int line = 0; line < nLines; line++)
            {
                for (int i = 0; i < samplesPerLine; i++)
                    *wp++ = pSamples[i];
                pSamples += linePitch;
            }

            pSamples = pSampleBuffer;
        }
        
        int nSamples = nLines * samplesPerLine;
        
        while (nSamples > 0)
        {
            uint16_t chunkSamples = min(nSamples, maxSamplesPerChunk);

            (*m_pEncoder) << GfxStream::Header{ GfxChunkId::EdgeSamples, 0, (uint16_t) (chunkSamples*4) };
            (*m_pEncoder) << GfxStream::DataChunk{ chunkSamples*4, pSamples };

            pSamples += chunkSamples;
            nSamples -= chunkSamples;
        }
        
        if( allocSize > 0 )
            GfxBase::memStackRelease(allocSize);

    }

    //____ _drawElipse() _______________________________________________________________

    void GfxStreamDevice::drawElipse(const RectSPX& canvas, spx thickness, HiColor color, spx outlineThickness, HiColor outlineColor )
    {
        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::DrawElipse, 0, 40 };
        (*m_pEncoder) << canvas;
        (*m_pEncoder) << thickness;
        (*m_pEncoder) << color;
        (*m_pEncoder) << outlineThickness;
        (*m_pEncoder) << outlineColor;
    }

    //____ drawPieChart() ______________________________________________________________

    void GfxStreamDevice::drawPieChart(const RectSPX& canvas, float start, int nSlices, const float * pSliceSizes, const HiColor * pSliceColors, float hubSize, HiColor hubColor, HiColor backColor, bool bRectangular)
    {
        uint16_t size = 16 + 4 + 4 + 4 + 8 + 8 + 2 + nSlices*(4+8);
        
        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::DrawPieChart, 0, size };
        (*m_pEncoder) << canvas;
        (*m_pEncoder) << start;
        (*m_pEncoder) << nSlices;
        (*m_pEncoder) << hubSize;
        (*m_pEncoder) << hubColor;
        (*m_pEncoder) << backColor;
        (*m_pEncoder) << bRectangular;
        
        (*m_pEncoder) << GfxStream::DataChunk{ nSlices*4, pSliceSizes };
        (*m_pEncoder) << GfxStream::DataChunk{ nSlices*8, pSliceColors };
    }

    //____ drawSegments() _________________________________________________________________

    void GfxStreamDevice::drawSegments(const RectSPX& dest, int nSegments, const HiColor * pSegmentColors, int nEdgeStrips, const int * pEdgeStrips, int edgeStripPitch, TintMode tintMode )
    {
        
        int nColors;
        switch( tintMode )
        {
            case TintMode::None:
            case TintMode::Flat:
                nColors = nSegments;
                break;
            case TintMode::GradientX:
            case TintMode::GradientY:
                nColors = nSegments*2;
                break;
            case TintMode::GradientXY:
                nColors = nSegments*4;
                break;
        }

        uint16_t size = 16 + 2 + 2 + 2 + nColors*8;

        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::DrawSegments, 0, size };
        (*m_pEncoder) << dest;
        (*m_pEncoder) << (uint16_t) nSegments;
        (*m_pEncoder) << (uint16_t) nEdgeStrips;
        (*m_pEncoder) << tintMode;

        (*m_pEncoder) << GfxStream::DataChunk{ nColors*8, pSegmentColors };

        _streamEdgeSamples( nEdgeStrips, nSegments-1, edgeStripPitch, pEdgeStrips );
    }

    //____ flipDrawSegments() _______________________________________________________

    void GfxStreamDevice::flipDrawSegments(const RectSPX& dest, int nSegments, const HiColor * pSegmentColors, int nEdgeStrips, const int * pEdgeStrips, int edgeStripPitch, GfxFlip flip, TintMode tintMode)
    {
        int nColors;
        switch( tintMode )
        {
            case TintMode::None:
            case TintMode::Flat:
                nColors = nSegments;
                break;
            case TintMode::GradientX:
            case TintMode::GradientY:
                nColors = nSegments*2;
                break;
            case TintMode::GradientXY:
                nColors = nSegments*4;
                break;
        }

        uint16_t size = 16 + 2 + 2 + 2 + 2 + nColors*8;

        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::FlipDrawSegments, 0, size };
        (*m_pEncoder) << dest;
        (*m_pEncoder) << (uint16_t) nSegments;
        (*m_pEncoder) << (uint16_t) nEdgeStrips;
        (*m_pEncoder) << flip;
        (*m_pEncoder) << tintMode;

        (*m_pEncoder) << GfxStream::DataChunk{ nColors*8, pSegmentColors };

        _streamEdgeSamples( nEdgeStrips, nSegments-1, edgeStripPitch, pEdgeStrips );
    }


    //.____ blitNinePatch() ___________________________________________

    void GfxStreamDevice::blitNinePatch(const RectSPX& dstRect, const BorderSPX& dstFrame, const NinePatch& patch, int scale)
    {
        uint16_t size = 16 + 16 + ( 16 + 8 + 10 + 10 ) + 4;

        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::BlitNinePatch, 0, size };
        (*m_pEncoder) << dstRect;
        (*m_pEncoder) << dstFrame;

        (*m_pEncoder) << patch.block;
        (*m_pEncoder) << patch.frame;

        (*m_pEncoder) << patch.rigidPartXOfs;
        (*m_pEncoder) << patch.rigidPartXLength;
        (*m_pEncoder) << patch.rigidPartXSections;

        (*m_pEncoder) << patch.rigidPartYOfs;
        (*m_pEncoder) << patch.rigidPartYLength;
        (*m_pEncoder) << patch.rigidPartYSections;

        (*m_pEncoder) << scale;
    }

    //.____ _beginCanvasUpdate() __________________________________________

    bool GfxStreamDevice::_beginCanvasUpdate(CanvasRef canvasRef, Surface * pCanvasSurface, int nUpdateRects, const RectSPX* pUpdateRects, CanvasLayers * pLayers, int startLayer)
    {
        if( pLayers )
        {
            // No support for canvas layers yet!
            
            return false;
        }
        
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
		
        uint16_t size = 2 + 1 + 1 + nUpdateRects * 16;

        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::BeginCanvasUpdate, {}, size };
 		(*m_pEncoder) << (pCanvasSurface ? static_cast<GfxStreamSurface*>(pCanvasSurface)->m_inStreamId : (uint16_t) 0);
        (*m_pEncoder) << canvasRef;
        (*m_pEncoder) << (uint8_t) nUpdateRects;
        (*m_pEncoder) << GfxStream::DataChunk{ nUpdateRects*16, pUpdateRects };
		
		m_canvas.ref = canvasRef;
		m_canvas.pSurface = pCanvasSurface;
		
		m_canvas.size = sz;
		
        return true;
    }

    void GfxStreamDevice::_canvasWasChanged()
    {
        //This method should never be called, but is pure virtual in super class.
        
        assert(false);
    }

    void GfxStreamDevice::_renderLayerWasChanged()
    {
        //This method should never be called, but is pure virtual in super class.
        
        assert(false);
    }

    void GfxStreamDevice::_transformBlitSimple(const RectI& dest, CoordI src, const int simpleTransform[2][2])
    {
        //This method should never be called, but is pure virtual in super class.
        
        assert(false);
    }

    void GfxStreamDevice::_transformBlitComplex(const RectI& dest, BinalCoord src, const binalInt complexTransform[2][2])
    {
        //This method should never be called, but is pure virtual in super class.
        
        assert(false);
    }

	//____ _transformDrawSegments() ______________________________________

	void GfxStreamDevice::_transformDrawSegments(const RectI& dest, int nSegments, const HiColor * pSegmentColors, int nEdgeStrips, const int * pEdgeStrips, int edgeStripPitch, TintMode tintMode, const int simpleTransform[2][2])
	{
        //This method should never be called, but is pure virtual in super class.
        
        assert(false);
	}


} // namespace wg

