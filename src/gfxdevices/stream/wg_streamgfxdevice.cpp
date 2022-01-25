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

#include <wg_streamgfxdevice.h>
#include <wg_streamsurface.h>
#include <wg_streamsurfacefactory.h>
#include <wg_base.h>
#include <assert.h>
#include <math.h>
#include <algorithm>

using namespace std;

namespace wg
{
	const TypeInfo StreamGfxDevice::TYPEINFO = { "StreamGfxDevice", &GfxDevice::TYPEINFO };



	//____ create() _______________________________________________________________

	StreamGfxDevice_p StreamGfxDevice::create( GfxStreamEncoder * pEncoder )
	{
		StreamGfxDevice_p p(new StreamGfxDevice( pEncoder ));
		return p;
	}


	//____ constructor _____________________________________________________________

	StreamGfxDevice::StreamGfxDevice(GfxStreamEncoder* pEncoder)
	{
		m_pEncoder = pEncoder;
		m_bRendering = false;
	}

	//____ Destructor ______________________________________________________________

	StreamGfxDevice::~StreamGfxDevice()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& StreamGfxDevice::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ surfaceType() _______________________________________________________

	const TypeInfo& StreamGfxDevice::surfaceType( void ) const
	{
		return StreamSurface::TYPEINFO;
	}

	//____ defineCanvas() ____________________________________________________

	bool StreamGfxDevice::defineCanvas( CanvasRef ref, StreamSurface * pSurface )
	{
		if( (pSurface->m_flags & SurfaceFlag::Canvas) == 0 )
		{
			//TODO: Error handling!
			return false;
		}
		
		auto it = std::find_if( m_definedCanvases.begin(), m_definedCanvases.end(), [ref] (CanvasInfo& entry) { return (ref == entry.ref); } );

		if( it == m_definedCanvases.end() )
		{
			if( pSurface )
				m_definedCanvases.push_back( CanvasInfo( ref, pSurface, pSurface->size() ) );
		}
		else
		{
			if( pSurface )
			{
				it->pSurface = pSurface;
				it->size = pSurface->size();
			}
			else
			{
				m_definedCanvases.erase(it);
			}
		}

		return true;
	}

	bool StreamGfxDevice::defineCanvas( CanvasRef ref, const SizeI& size )
	{
	
		auto it = std::find_if( m_definedCanvases.begin(), m_definedCanvases.end(), [ref] (CanvasInfo& entry) { return (ref == entry.ref); } );

		if( it == m_definedCanvases.end() )
		{
			if( !size.isEmpty() )
				m_definedCanvases.push_back( CanvasInfo( ref, nullptr, size ) );
		}
		else
		{
			if( !size.isEmpty() )
			{
				it->pSurface = nullptr;
				it->size = size;
			}
			else
			{
				m_definedCanvases.erase(it);
			}
		}

		return true;
	}

	//____ surfaceFactory() ______________________________________________________

	SurfaceFactory_p StreamGfxDevice::surfaceFactory()
	{
		if( !m_pSurfaceFactory )
			m_pSurfaceFactory = StreamSurfaceFactory::create(m_pEncoder);

		return m_pSurfaceFactory;
	}

	//____ canvasSize() _____________________________________________________

	SizeI StreamGfxDevice::canvasSize(CanvasRef ref) const
	{
		auto it = std::find_if( m_definedCanvases.begin(), m_definedCanvases.end(), [ref] (const CanvasInfo& entry) { return (ref == entry.ref); } );

		if( it != m_definedCanvases.end() )
            return it->size;
		
		return SizeI();
	}

    //____ setClipList() _________________________________________________________

    bool StreamGfxDevice::setClipList(int nRectangles, const RectI * pRectangles)
    {
        GfxDevice::setClipList(nRectangles, pRectangles);
        
        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::SetClipList, (uint16_t) (nRectangles * 16) };
        (*m_pEncoder) << GfxStream::DataChunk{ nRectangles * 16, pRectangles };
        return true;
    }

    //____ clearClipList() _______________________________________________________

    void StreamGfxDevice::clearClipList()
    {
        GfxDevice::clearClipList();
        
        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::ClearClipList, 0 };
    }

    //____ pushClipList() ________________________________________________________

    bool StreamGfxDevice::pushClipList(int nRectangles, const RectI* pRectangles)
    {
        GfxDevice::pushClipList(nRectangles, pRectangles);
        
        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::PushClipList, (uint16_t) (nRectangles * 16) };
        (*m_pEncoder) << GfxStream::DataChunk{ nRectangles * 16, pRectangles };
        return true;
    }

    //____ popClipList() __________________________________________________________

    bool StreamGfxDevice::popClipList()
    {
        GfxDevice::popClipList();
        
        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::PopClipList, 0 };
        return true;
    }

    //____ setTintColor() __________________________________________________________

    void StreamGfxDevice::setTintColor( HiColor color )
    {
        GfxDevice::setTintColor(color);

        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::SetTintColor, 8 };
        (*m_pEncoder) << color;
    }

    //____ setTintGradient() _______________________________________________________

    void StreamGfxDevice::setTintGradient(const RectI& rect, const Gradient& gradient)
    {
        GfxDevice::setTintGradient(rect, gradient);
        
        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::SetTintGradient, 16 + 4*8 };
        (*m_pEncoder) << rect;
        (*m_pEncoder) << gradient.topLeft;
        (*m_pEncoder) << gradient.topRight;
        (*m_pEncoder) << gradient.bottomRight;
        (*m_pEncoder) << gradient.bottomLeft;
    }

    //____ clearTintGradient() _____________________________________________________

    void StreamGfxDevice::clearTintGradient()
    {
        GfxDevice::clearTintGradient();
        
        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::ClearTintGradient, 0 };
    }

    //____ setBlendMode() __________________________________________________________

    bool StreamGfxDevice::setBlendMode( BlendMode blendMode )
    {
        if( blendMode < BlendMode_min || blendMode > BlendMode_max )
                return false;

        GfxDevice::setBlendMode(blendMode);

        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::SetBlendMode, 2 };
        (*m_pEncoder) << blendMode;

        return true;
    }

    //____ setBlitSource() _______________________________________________________

    bool StreamGfxDevice::setBlitSource(Surface * pSource)
    {
        if (!pSource || !pSource->isInstanceOf(StreamSurface::TYPEINFO) )
            return false;

        GfxDevice::setBlitSource(pSource);

        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::SetBlitSource, 2 };
        (*m_pEncoder) << static_cast<StreamSurface*>(pSource)->m_inStreamId;
        return true;
    }

    //____ setMorphFactor() ______________________________________________________

    void StreamGfxDevice::setMorphFactor(float factor)
    {
        GfxDevice::setMorphFactor(factor);
        
        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::SetMorphFactor, 4 };
        (*m_pEncoder) << factor;
    }

    //____ setRenderLayer() _______________________________________________________

    void StreamGfxDevice::setRenderLayer(int layer)
    {
        GfxDevice::setRenderLayer(layer);
        
        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::SetRenderLayer, 2 };
        (*m_pEncoder) << (uint16_t) layer;
    }

	//____ beginRender() ___________________________________________________________

	bool StreamGfxDevice::beginRender()
	{
		if( m_bRendering == true )
			return false;

		(*m_pEncoder) << GfxStream::Header{ GfxChunkId::BeginRender, 0 };

		m_bRendering = true;
		return true;
	}

	//____ endRender() _____________________________________________________________

	bool StreamGfxDevice::endRender()
	{
		if( m_bRendering == false )
			return false;

		(*m_pEncoder) << GfxStream::Header{ GfxChunkId::EndRender, 0 };
		m_pEncoder->flush();

		m_bRendering = false;
		return true;
	}

    //____ flush() ___________________________________________________________________

    void StreamGfxDevice::flush()
    {
        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::Flush, 0 };
        m_pEncoder->flush();
    }

    //____ endCanvasUpdate() _________________________________________________________

    void StreamGfxDevice::endCanvasUpdate()
    {
        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::EndCanvasUpdate, 0 };

    }

    //____ fill() __________________________________________________________________

    void StreamGfxDevice::fill( HiColor _col )
    {
        if( _col.a  == 0 )
            return;

        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::Fill, 8 };
        (*m_pEncoder) << _col;

        return;
    }

    void StreamGfxDevice::fill( const RectI& _rect, HiColor _col )
    {
        if( _col.a  == 0 || _rect.w < 1 || _rect.h < 1 )
            return;

        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::FillRectI, 24 };
        (*m_pEncoder) << _rect;
        (*m_pEncoder) << _col;

        return;
    }

    void StreamGfxDevice::fill(const RectF& rect, HiColor col)
    {
        if (col.a == 0)
            return;

        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::FillRectF, 24 };
        (*m_pEncoder) << rect;
        (*m_pEncoder) << col;

        return;
    }

    //____ plotPixels() ________________________________________________________

    void StreamGfxDevice::plotPixels(int nCoords, const CoordI * pCoords, const HiColor * pColors)
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
            (*m_pEncoder) << GfxStream::Header{ GfxChunkId::PlotPixels, (uint16_t) (chunkCoords * 16) };
            (*m_pEncoder) << GfxStream::DataChunk{ chunkCoords * 8, pCoords };
            (*m_pEncoder) << GfxStream::DataChunk{ chunkCoords * 8, pColors };

            nCoords -= chunkCoords;
            pCoords += chunkCoords;
            pColors += chunkCoords;
            chunkCoords = min(nCoords, maxChunkCoords);
        }
    }

    //____ drawLine() __________________________________________________________

    void StreamGfxDevice::drawLine(CoordI begin, CoordI end, HiColor color, float thickness)
    {
        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::DrawLineFromTo, 28 };
        (*m_pEncoder) << begin;
        (*m_pEncoder) << end;
        (*m_pEncoder) << color;
        (*m_pEncoder) << thickness;
    }

    void StreamGfxDevice::drawLine(CoordI begin, Direction dir, int length, HiColor col, float thickness)
    {
        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::DrawLineStraight, 26 };
        (*m_pEncoder) << begin;
        (*m_pEncoder) << dir;
        (*m_pEncoder) << length;
        (*m_pEncoder) << col;
        (*m_pEncoder) << thickness;
    }

    //____ blit() __________________________________________________________________

    void StreamGfxDevice::blit(CoordI dest)
    {
        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::Blit, 8 };
        (*m_pEncoder) << dest;
    }

    void StreamGfxDevice::blit(CoordI dest, const RectI& _src)
    {
        if (_src.w < 1 || _src.h < 1)
            return;

        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::BlitRectI, 24 };
        (*m_pEncoder) << dest;
        (*m_pEncoder) << _src;
    }

    void StreamGfxDevice::flipBlit(CoordI dest, GfxFlip flip )
    {
        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::FlipBlit, 10 };
        (*m_pEncoder) << dest;
        (*m_pEncoder) << flip;
    }

    void StreamGfxDevice::flipBlit(CoordI dest, const RectI& src, GfxFlip flip )
    {
        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::FlipBlitRectI, 26 };
        (*m_pEncoder) << dest;
        (*m_pEncoder) << src;
        (*m_pEncoder) << flip;
    }

    //____ stretchBlit() ___________________________________________________

    void StreamGfxDevice::stretchBlit(const RectI& dest)
    {

        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::StretchBlit, 16 };
        (*m_pEncoder) << dest;
    }

    void StreamGfxDevice::stretchBlit(const RectI& dest, const RectI& source)
    {

        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::StretchBlitRectI, 32 };
        (*m_pEncoder) << dest;
        (*m_pEncoder) << source;
    }

    void StreamGfxDevice::stretchBlit(const RectI& dest, const RectF& source)
    {

        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::StretchBlitRectF, 32 };
        (*m_pEncoder) << dest;
        (*m_pEncoder) << source;
    }

    //____ stretchFlipBlit() ___________________________________________________

    void StreamGfxDevice::stretchFlipBlit(const RectI& dest, GfxFlip flip)
    {

        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::StretchFlipBlit, 18 };
        (*m_pEncoder) << dest;
        (*m_pEncoder) << flip;
    }

    void StreamGfxDevice::stretchFlipBlit(const RectI& dest, const RectI& source, GfxFlip flip)
    {

        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::StretchBlitRectI, 34 };
        (*m_pEncoder) << dest;
        (*m_pEncoder) << source;
        (*m_pEncoder) << flip;
    }

    void StreamGfxDevice::stretchFlipBlit(const RectI& dest, const RectF& source, GfxFlip flip)
    {

        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::StretchBlitRectF, 34 };
        (*m_pEncoder) << dest;
        (*m_pEncoder) << source;
        (*m_pEncoder) << flip;
    }

    //____ rotScaleBlit() ___________________________________________________________

    void StreamGfxDevice::rotScaleBlit(const RectI& dest, float rotationDegrees, float scale, CoordF srcCenter, CoordF destCenter)
    {
        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::RotScaleBlit, 40 };
        (*m_pEncoder) << dest;
        (*m_pEncoder) << rotationDegrees;
        (*m_pEncoder) << scale;
        (*m_pEncoder) << srcCenter;
        (*m_pEncoder) << destCenter;
    }

    //____ tile() _____________________________________________________________________

    void StreamGfxDevice::tile(const RectI& dest, CoordI shift)
    {
        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::Tile, 24 };
        (*m_pEncoder) << dest;
        (*m_pEncoder) << shift;
    }

    void StreamGfxDevice::flipTile(const RectI& dest, GfxFlip flip, CoordI shift)
    {
        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::FlipTile, 26 };
        (*m_pEncoder) << dest;
        (*m_pEncoder) << flip;
        (*m_pEncoder) << shift;
    }

    void StreamGfxDevice::scaleTile(const RectI& dest, float scale, CoordI shift)
    {
        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::ScaleTile, 28 };
        (*m_pEncoder) << dest;
        (*m_pEncoder) << scale;
        (*m_pEncoder) << shift;
    }

    void StreamGfxDevice::scaleFlipTile(const RectI& dest, float scale, GfxFlip flip, CoordI shift)
    {
        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::ScaleFlipTile, 30 };
        (*m_pEncoder) << dest;
        (*m_pEncoder) << scale;
        (*m_pEncoder) << flip;
        (*m_pEncoder) << shift;
    }

    //____ drawWave() ___________________________________________________________________

    void StreamGfxDevice::drawWave(const RectI& dest, const WaveLine * pTopBorder, const WaveLine * pBottomBorder, HiColor frontFill, HiColor backFill)
    {
        int size = 16 + 20 + 20 + 8 + 8;
        
        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::DrawWave, (uint16_t) size };
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

    void StreamGfxDevice::flipDrawWave(const RectI& dest, const WaveLine * pTopBorder, const WaveLine * pBottomBorder, HiColor frontFill, HiColor backFill, GfxFlip flip)
    {
        uint16_t size = 16 + 20 + 20 + 8 + 8 + 2;
        
        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::FlipDrawWave, size };
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

    void StreamGfxDevice::_streamEdgeSamples( int nLines, int samplesPerLine, int linePitch, const int * pSamples )
    {
        int maxSamplesPerChunk = (GfxStream::c_maxBlockSize - sizeof(GfxStream::Header)) / 4;
        int allocSize = 0;
        
        if( samplesPerLine != linePitch )
        {
            allocSize = nLines * samplesPerLine * 4;

            int * pSampleBuffer = (int*) Base::memStackAlloc(allocSize);
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

            (*m_pEncoder) << GfxStream::Header{ GfxChunkId::EdgeSamples , (uint16_t) (chunkSamples*4) };
            (*m_pEncoder) << GfxStream::DataChunk{ chunkSamples*4, pSamples };

            pSamples += chunkSamples;
            nSamples -= chunkSamples;
        }
        
        if( allocSize > 0 )
            Base::memStackRelease(allocSize);

    }

    //____ _drawElipse() _______________________________________________________________

    void StreamGfxDevice::drawElipse(const RectF& canvas, float thickness, HiColor color, float outlineThickness, HiColor outlineColor )
    {
        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::DrawElipse, 40 };
        (*m_pEncoder) << canvas;
        (*m_pEncoder) << thickness;
        (*m_pEncoder) << color;
        (*m_pEncoder) << outlineThickness;
        (*m_pEncoder) << outlineColor;
    }

    //____ drawPieChart() ______________________________________________________________

    void StreamGfxDevice::drawPieChart(const RectI& canvas, float start, int nSlices, const float * pSliceSizes, const HiColor * pSliceColors, float hubSize, HiColor hubColor, HiColor backColor, bool bRectangular)
    {
        uint16_t size = 16 + 4 + 4 + 4 + 8 + 8 + 2 + nSlices*(4+8);
        
        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::DrawElipse, size };
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

    void StreamGfxDevice::drawSegments(const RectI& dest, int nSegments, const HiColor * pSegmentColors, int nEdgeStrips, const int * pEdgeStrips, int edgeStripPitch, TintMode tintMode )
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

        uint16_t size = 16 + 4 + 4 + 2 + nColors*8;

        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::DrawSegments, size };
        (*m_pEncoder) << dest;
        (*m_pEncoder) << (uint16_t) nSegments;
        (*m_pEncoder) << (uint16_t) nEdgeStrips;
        (*m_pEncoder) << tintMode;

        (*m_pEncoder) << GfxStream::DataChunk{ nColors*8, pSegmentColors };

        _streamEdgeSamples( nEdgeStrips, nSegments-1, edgeStripPitch, pEdgeStrips );
    }

    //____ flipDrawSegments() _______________________________________________________

    void StreamGfxDevice::flipDrawSegments(const RectI& dest, int nSegments, const HiColor * pSegmentColors, int nEdgeStrips, const int * pEdgeStrips, int edgeStripPitch, GfxFlip flip, TintMode tintMode)
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

        uint16_t size = 16 + 4 + 4 + 2 + 2 + nColors*8;

        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::FlipDrawSegments, size };
        (*m_pEncoder) << dest;
        (*m_pEncoder) << nSegments;
        (*m_pEncoder) << nEdgeStrips;
        (*m_pEncoder) << flip;
        (*m_pEncoder) << tintMode;

        (*m_pEncoder) << GfxStream::DataChunk{ nColors*8, pSegmentColors };

        _streamEdgeSamples( nEdgeStrips, nSegments-1, edgeStripPitch, pEdgeStrips );
    }


    //.____ blitNinePatch() ___________________________________________

    void StreamGfxDevice::blitNinePatch(const RectI& dstRect, const BorderI& dstFrame, const NinePatch& patch)
    {
        uint16_t size = 16 + 16 + ( 16 + 16 + 10 + 10 );

        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::BlitNinePatch, size };
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
    }

    //.____ blitHorrBar() ________________________________________________

    void StreamGfxDevice::blitHorrBar( const RectI& _src, const BorderI& _borders, bool _bTile, CoordI dest, int _len )
    {
        // This method is deprecated and should not be used. Let's fix the calling code instead.
        
        assert(false);
    }

    //.____ blitVertBar() ________________________________________________

    void StreamGfxDevice::blitVertBar( const RectI& _src, const BorderI& _borders, bool _bTile, CoordI dest, int _len )
    {
        // This method is deprecated and should not be used. Let's fix the calling code instead.

        assert(false);
    }

    //.____ _beginCanvasUpdate() __________________________________________

    bool StreamGfxDevice::_beginCanvasUpdate(CanvasRef canvasRef, Surface * pCanvasSurface, int nUpdateRects, const RectI* pUpdateRects, CanvasLayers * pLayers, int startLayer)
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
		
		SizeI sz = (canvasRef != CanvasRef::None) ? canvasSize(canvasRef) : pCanvasSurface->size();
		if( sz.isEmpty() )
		{
			// TODO: Error handling!
			return false;
		}
		
        uint16_t size = 2 + 2 + 4 + nUpdateRects * 16;

        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::BeginCanvasUpdate, size };
        (*m_pEncoder) << canvasRef;
		(*m_pEncoder) << (pCanvasSurface ? static_cast<StreamSurface*>(pCanvasSurface)->m_inStreamId : (uint16_t) 0);
        (*m_pEncoder) << nUpdateRects;
        (*m_pEncoder) << GfxStream::DataChunk{ nUpdateRects*16, pUpdateRects };
		
		m_canvas.ref = canvasRef;
		m_canvas.pSurface = pCanvasSurface;
		
		m_canvas.size = sz;
		
        return true;
    }

    void StreamGfxDevice::_canvasWasChanged()
    {
        //This method should never be called, but is pure virtual in super class.
        
        assert(false);
    }

    void StreamGfxDevice::_renderLayerWasChanged()
    {
        //This method should never be called, but is pure virtual in super class.
        
        assert(false);
    }

    void StreamGfxDevice::_transformBlit(const RectI& dest, CoordI src, const int simpleTransform[2][2])
    {
        //This method should never be called, but is pure virtual in super class.
        
        assert(false);
    }

    void StreamGfxDevice::_transformBlit(const RectI& dest, CoordF src, const float complexTransform[2][2])
    {
        //This method should never be called, but is pure virtual in super class.
        
        assert(false);
    }

	//____ _transformDrawSegments() ______________________________________

	void StreamGfxDevice::_transformDrawSegments(const RectI& dest, int nSegments, const HiColor * pSegmentColors, int nEdgeStrips, const int * pEdgeStrips, int edgeStripPitch, TintMode tintMode, const int simpleTransform[2][2])
	{
        //This method should never be called, but is pure virtual in super class.
        
        assert(false);
	}


} // namespace wg

