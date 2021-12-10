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

	StreamGfxDevice_p StreamGfxDevice::create( CGfxOutStream& stream )
	{
		StreamGfxDevice_p p(new StreamGfxDevice( stream ));
		return p;
	}


	//____ constructor _____________________________________________________________

	StreamGfxDevice::StreamGfxDevice( CGfxOutStream& stream )
	{
		m_pStream = &stream;
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
			m_pSurfaceFactory = StreamSurfaceFactory::create(*m_pStream);

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
        
        (*m_pStream) << GfxStream::Header{ GfxChunkId::SetClipList, (uint16_t) (nRectangles * 16) };
        (*m_pStream) << GfxStream::DataChunk{ nRectangles * 16, pRectangles };
    }

    //____ clearClipList() _______________________________________________________

    void StreamGfxDevice::clearClipList()
    {
        GfxDevice::clearClipList();
        
        (*m_pStream) << GfxStream::Header{ GfxChunkId::ClearClipList, 0 };
    }

    //____ pushClipList() ________________________________________________________

    bool StreamGfxDevice::pushClipList(int nRectangles, const RectI* pRectangles)
    {
        GfxDevice::pushClipList(nRectangles, pRectangles);
        
        (*m_pStream) << GfxStream::Header{ GfxChunkId::PushClipList, (uint16_t) (nRectangles * 16) };
        (*m_pStream) << GfxStream::DataChunk{ nRectangles * 16, pRectangles };
    }

    //____ popClipList() __________________________________________________________

    bool StreamGfxDevice::popClipList()
    {
        GfxDevice::popClipList();
        
        (*m_pStream) << GfxStream::Header{ GfxChunkId::PopClipList, 0 };
    }

    //____ setTintColor() __________________________________________________________

    void StreamGfxDevice::setTintColor( HiColor color )
    {
        GfxDevice::setTintColor(color);

        (*m_pStream) << GfxStream::Header{ GfxChunkId::SetTintColor, 8 };
        (*m_pStream) << color;
    }

    //____ setTintGradient() _______________________________________________________

    void StreamGfxDevice::setTintGradient(const RectI& rect, const Gradient& gradient)
    {
        GfxDevice::setTintGradient(rect, gradient);
        
        (*m_pStream) << GfxStream::Header{ GfxChunkId::SetTintGradient, 16 + 4*8 };
        (*m_pStream) << rect;
        (*m_pStream) << gradient.topLeft;
        (*m_pStream) << gradient.topRight;
        (*m_pStream) << gradient.bottomRight;
        (*m_pStream) << gradient.bottomLeft;
    }

    //____ clearTintGradient() _____________________________________________________

    void StreamGfxDevice::clearTintGradient()
    {
        GfxDevice::clearTintGradient();
        
        (*m_pStream) << GfxStream::Header{ GfxChunkId::ClearTintGradient, 0 };
    }

    //____ setBlendMode() __________________________________________________________

    bool StreamGfxDevice::setBlendMode( BlendMode blendMode )
    {
        if( blendMode < BlendMode_min || blendMode > BlendMode_max )
                return false;

        GfxDevice::setBlendMode(blendMode);

        (*m_pStream) << GfxStream::Header{ GfxChunkId::SetBlendMode, 2 };
        (*m_pStream) << blendMode;

        return true;
    }

    //____ setBlitSource() _______________________________________________________

    bool StreamGfxDevice::setBlitSource(Surface * pSource)
    {
        if (!pSource || !pSource->isInstanceOf(StreamSurface::TYPEINFO) )
            return false;

        GfxDevice::setBlitSource(pSource);

        (*m_pStream) << GfxStream::Header{ GfxChunkId::SetBlitSource, 2 };
        (*m_pStream) << static_cast<StreamSurface*>(pSource)->m_inStreamId;
        return true;
    }

    //____ setMorphFactor() ______________________________________________________

    void StreamGfxDevice::setMorphFactor(float factor)
    {
        GfxDevice::setMorphFactor(factor);
        
        (*m_pStream) << GfxStream::Header{ GfxChunkId::SetMorphFactor, 4 };
        (*m_pStream) << factor;
    }

    //____ setRenderLayer() _______________________________________________________

    void StreamGfxDevice::setRenderLayer(int layer)
    {
        GfxDevice::setRenderLayer(layer);
        
        (*m_pStream) << GfxStream::Header{ GfxChunkId::SetRenderLayer, 2 };
        (*m_pStream) << (uint16_t) layer;
    }

	//____ beginRender() ___________________________________________________________

	bool StreamGfxDevice::beginRender()
	{
		if( m_bRendering == true )
			return false;

		(*m_pStream) << GfxStream::Header{ GfxChunkId::BeginRender, 0 };

		m_bRendering = true;
		return true;
	}

	//____ endRender() _____________________________________________________________

	bool StreamGfxDevice::endRender()
	{
		if( m_bRendering == false )
			return false;

		(*m_pStream) << GfxStream::Header{ GfxChunkId::EndRender, 0 };
		m_pStream->flush();

		m_bRendering = false;
		return true;
	}

    //____ flush() ___________________________________________________________________

    void StreamGfxDevice::flush()
    {
        (*m_pStream) << GfxStream::Header{ GfxChunkId::Flush, 0 };
        m_pStream->flush();
    }

    //____ endCanvasUpdate() _________________________________________________________

    void StreamGfxDevice::endCanvasUpdate()
    {
        (*m_pStream) << GfxStream::Header{ GfxChunkId::EndCanvasUpdate, 0 };

    }

    //____ fill() __________________________________________________________________

    void StreamGfxDevice::fill( HiColor _col )
    {
        if( _col.a  == 0 )
            return;

        (*m_pStream) << GfxStream::Header{ GfxChunkId::Fill, 8 };
        (*m_pStream) << _col;

        return;
    }

    void StreamGfxDevice::fill( const RectI& _rect, HiColor _col )
    {
        if( _col.a  == 0 || _rect.w < 1 || _rect.h < 1 )
            return;

        (*m_pStream) << GfxStream::Header{ GfxChunkId::FillRectI, 24 };
        (*m_pStream) << _rect;
        (*m_pStream) << _col;

        return;
    }

    void StreamGfxDevice::fill(const RectF& rect, HiColor col)
    {
        if (col.a == 0)
            return;

        (*m_pStream) << GfxStream::Header{ GfxChunkId::FillRectF, 24 };
        (*m_pStream) << rect;
        (*m_pStream) << col;

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
            (*m_pStream) << GfxStream::Header{ GfxChunkId::PlotPixels, (uint16_t) (chunkCoords * 16) };
            (*m_pStream) << GfxStream::DataChunk{ chunkCoords * 8, pCoords };
            (*m_pStream) << GfxStream::DataChunk{ chunkCoords * 8, pColors };

            nCoords -= chunkCoords;
            pCoords += chunkCoords;
            pColors += chunkCoords;
            chunkCoords = min(nCoords, maxChunkCoords);
        }
    }

    //____ drawLine() __________________________________________________________

    void StreamGfxDevice::drawLine(CoordI begin, CoordI end, HiColor color, float thickness)
    {
        (*m_pStream) << GfxStream::Header{ GfxChunkId::DrawLineFromTo, 28 };
        (*m_pStream) << begin;
        (*m_pStream) << end;
        (*m_pStream) << color;
        (*m_pStream) << thickness;
    }

    void StreamGfxDevice::drawLine(CoordI begin, Direction dir, int length, HiColor col, float thickness)
    {
        (*m_pStream) << GfxStream::Header{ GfxChunkId::DrawLineStraight, 26 };
        (*m_pStream) << begin;
        (*m_pStream) << dir;
        (*m_pStream) << length;
        (*m_pStream) << col;
        (*m_pStream) << thickness;
    }

    //____ blit() __________________________________________________________________

    void StreamGfxDevice::blit(CoordI dest)
    {
        (*m_pStream) << GfxStream::Header{ GfxChunkId::Blit, 8 };
        (*m_pStream) << dest;
    }

    void StreamGfxDevice::blit(CoordI dest, const RectI& _src)
    {
        if (_src.w < 1 || _src.h < 1)
            return;

        (*m_pStream) << GfxStream::Header{ GfxChunkId::BlitRectI, 24 };
        (*m_pStream) << dest;
        (*m_pStream) << _src;
    }

    void StreamGfxDevice::flipBlit(CoordI dest, GfxFlip flip )
    {
        (*m_pStream) << GfxStream::Header{ GfxChunkId::FlipBlit, 10 };
        (*m_pStream) << dest;
        (*m_pStream) << flip;
    }

    void StreamGfxDevice::flipBlit(CoordI dest, const RectI& src, GfxFlip flip )
    {
        (*m_pStream) << GfxStream::Header{ GfxChunkId::FlipBlitRectI, 26 };
        (*m_pStream) << dest;
        (*m_pStream) << src;
        (*m_pStream) << flip;
    }

    //____ stretchBlit() ___________________________________________________

    void StreamGfxDevice::stretchBlit(const RectI& dest)
    {

        (*m_pStream) << GfxStream::Header{ GfxChunkId::StretchBlit, 16 };
        (*m_pStream) << dest;
    }

    void StreamGfxDevice::stretchBlit(const RectI& dest, const RectI& source)
    {

        (*m_pStream) << GfxStream::Header{ GfxChunkId::StretchBlitRectI, 32 };
        (*m_pStream) << dest;
        (*m_pStream) << source;
    }

    void StreamGfxDevice::stretchBlit(const RectI& dest, const RectF& source)
    {

        (*m_pStream) << GfxStream::Header{ GfxChunkId::StretchBlitRectF, 32 };
        (*m_pStream) << dest;
        (*m_pStream) << source;
    }

    //____ stretchFlipBlit() ___________________________________________________

    void StreamGfxDevice::stretchFlipBlit(const RectI& dest, GfxFlip flip)
    {

        (*m_pStream) << GfxStream::Header{ GfxChunkId::StretchFlipBlit, 18 };
        (*m_pStream) << dest;
        (*m_pStream) << flip;
    }

    void StreamGfxDevice::stretchFlipBlit(const RectI& dest, const RectI& source, GfxFlip flip)
    {

        (*m_pStream) << GfxStream::Header{ GfxChunkId::StretchBlitRectI, 34 };
        (*m_pStream) << dest;
        (*m_pStream) << source;
        (*m_pStream) << flip;
    }

    void StreamGfxDevice::stretchFlipBlit(const RectI& dest, const RectF& source, GfxFlip flip)
    {

        (*m_pStream) << GfxStream::Header{ GfxChunkId::StretchBlitRectF, 34 };
        (*m_pStream) << dest;
        (*m_pStream) << source;
        (*m_pStream) << flip;
    }

    //____ rotScaleBlit() ___________________________________________________________

    void StreamGfxDevice::rotScaleBlit(const RectI& dest, float rotationDegrees, float scale, CoordF srcCenter, CoordF destCenter)
    {
        (*m_pStream) << GfxStream::Header{ GfxChunkId::RotScaleBlit, 40 };
        (*m_pStream) << dest;
        (*m_pStream) << rotationDegrees;
        (*m_pStream) << scale;
        (*m_pStream) << srcCenter;
        (*m_pStream) << destCenter;
    }

    //____ tile() _____________________________________________________________________

    void StreamGfxDevice::tile(const RectI& dest, CoordI shift)
    {
        (*m_pStream) << GfxStream::Header{ GfxChunkId::Tile, 24 };
        (*m_pStream) << dest;
        (*m_pStream) << shift;
    }

    void StreamGfxDevice::flipTile(const RectI& dest, GfxFlip flip, CoordI shift)
    {
        (*m_pStream) << GfxStream::Header{ GfxChunkId::FlipTile, 26 };
        (*m_pStream) << dest;
        (*m_pStream) << flip;
        (*m_pStream) << shift;
    }

    void StreamGfxDevice::scaleTile(const RectI& dest, float scale, CoordI shift)
    {
        (*m_pStream) << GfxStream::Header{ GfxChunkId::ScaleTile, 28 };
        (*m_pStream) << dest;
        (*m_pStream) << scale;
        (*m_pStream) << shift;
    }

    void StreamGfxDevice::scaleFlipTile(const RectI& dest, float scale, GfxFlip flip, CoordI shift)
    {
        (*m_pStream) << GfxStream::Header{ GfxChunkId::ScaleFlipTile, 30 };
        (*m_pStream) << dest;
        (*m_pStream) << scale;
        (*m_pStream) << flip;
        (*m_pStream) << shift;
    }

    //____ drawWave() ___________________________________________________________________

    void StreamGfxDevice::drawWave(const RectI& dest, const WaveLine * pTopBorder, const WaveLine * pBottomBorder, HiColor frontFill, HiColor backFill)
    {
        int size = 16 + 20 + 20 + 8 + 8;
        
        (*m_pStream) << GfxStream::Header{ GfxChunkId::DrawWave, (uint16_t) size };
        (*m_pStream) << dest;

        (*m_pStream) << pTopBorder->length;
        (*m_pStream) << pTopBorder->thickness;
        (*m_pStream) << pTopBorder->color;
        (*m_pStream) << pTopBorder->hold;

        (*m_pStream) << pBottomBorder->length;
        (*m_pStream) << pBottomBorder->thickness;
        (*m_pStream) << pBottomBorder->color;
        (*m_pStream) << pBottomBorder->hold;

        (*m_pStream) << frontFill;
        (*m_pStream) << backFill;

        if( pTopBorder->length > 0 )
            _streamEdgeSamples( pTopBorder->length, 1, 1, pTopBorder->pWave );
        
        if( pBottomBorder->length > 0 )
            _streamEdgeSamples( pBottomBorder->length, 1, 1, pBottomBorder->pWave );
    }

    //____ flipDrawWave() ___________________________________________________________________

    void StreamGfxDevice::flipDrawWave(const RectI& dest, const WaveLine * pTopBorder, const WaveLine * pBottomBorder, HiColor frontFill, HiColor backFill, GfxFlip flip)
    {
        uint16_t size = 16 + 20 + 20 + 8 + 8 + 2;
        
        (*m_pStream) << GfxStream::Header{ GfxChunkId::FlipDrawWave, size };
        (*m_pStream) << dest;

        (*m_pStream) << pTopBorder->length;
        (*m_pStream) << pTopBorder->thickness;
        (*m_pStream) << pTopBorder->color;
        (*m_pStream) << pTopBorder->hold;

        (*m_pStream) << pBottomBorder->length;
        (*m_pStream) << pBottomBorder->thickness;
        (*m_pStream) << pBottomBorder->color;
        (*m_pStream) << pBottomBorder->hold;

        (*m_pStream) << frontFill;
        (*m_pStream) << backFill;
        (*m_pStream) << flip;

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

            (*m_pStream) << GfxStream::Header{ GfxChunkId::EdgeSamples , (uint16_t) (chunkSamples*4) };
            (*m_pStream) << GfxStream::DataChunk{ chunkSamples*4, pSamples };

            pSamples += chunkSamples;
            nSamples -= chunkSamples;
        }
        
        if( allocSize > 0 )
            Base::memStackRelease(allocSize);

    }

    //____ _drawElipse() _______________________________________________________________

    void StreamGfxDevice::drawElipse(const RectF& canvas, float thickness, HiColor color, float outlineThickness, HiColor outlineColor )
    {
        (*m_pStream) << GfxStream::Header{ GfxChunkId::DrawElipse, 40 };
        (*m_pStream) << canvas;
        (*m_pStream) << thickness;
        (*m_pStream) << color;
        (*m_pStream) << outlineThickness;
        (*m_pStream) << outlineColor;
    }

    //____ drawPieChart() ______________________________________________________________

    void StreamGfxDevice::drawPieChart(const RectI& canvas, float start, int nSlices, const float * pSliceSizes, const HiColor * pSliceColors, float hubSize, HiColor hubColor, HiColor backColor, bool bRectangular)
    {
        uint16_t size = 16 + 4 + 4 + 4 + 8 + 8 + 2 + nSlices*(4+8);
        
        (*m_pStream) << GfxStream::Header{ GfxChunkId::DrawElipse, size };
        (*m_pStream) << canvas;
        (*m_pStream) << start;
        (*m_pStream) << nSlices;
        (*m_pStream) << hubSize;
        (*m_pStream) << hubColor;
        (*m_pStream) << backColor;
        (*m_pStream) << bRectangular;
        
        (*m_pStream) << GfxStream::DataChunk{ nSlices*4, pSliceSizes };
        (*m_pStream) << GfxStream::DataChunk{ nSlices*8, pSliceColors };
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

        (*m_pStream) << GfxStream::Header{ GfxChunkId::DrawSegments, size };
        (*m_pStream) << dest;
        (*m_pStream) << nSegments;
        (*m_pStream) << nEdgeStrips;
        (*m_pStream) << tintMode;

        (*m_pStream) << GfxStream::DataChunk{ nColors*8, pSegmentColors };

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

        (*m_pStream) << GfxStream::Header{ GfxChunkId::FlipDrawSegments, size };
        (*m_pStream) << dest;
        (*m_pStream) << nSegments;
        (*m_pStream) << nEdgeStrips;
        (*m_pStream) << flip;
        (*m_pStream) << tintMode;

        (*m_pStream) << GfxStream::DataChunk{ nColors*8, pSegmentColors };

        _streamEdgeSamples( nEdgeStrips, nSegments-1, edgeStripPitch, pEdgeStrips );
    }


    //.____ blitNinePatch() ___________________________________________

    void StreamGfxDevice::blitNinePatch(const RectI& dstRect, const BorderI& dstFrame, const NinePatch& patch)
    {
        uint16_t size = 16 + 16 + ( 16 + 16 + 10 + 10 );

        (*m_pStream) << GfxStream::Header{ GfxChunkId::BlitNinePatch, size };
        (*m_pStream) << dstRect;
        (*m_pStream) << dstFrame;

        (*m_pStream) << patch.block;
        (*m_pStream) << patch.frame;

        (*m_pStream) << patch.rigidPartXOfs;
        (*m_pStream) << patch.rigidPartXLength;
        (*m_pStream) << patch.rigidPartXSections;

        (*m_pStream) << patch.rigidPartYOfs;
        (*m_pStream) << patch.rigidPartYLength;
        (*m_pStream) << patch.rigidPartYSections;
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

        (*m_pStream) << GfxStream::Header{ GfxChunkId::BeginCanvasUpdate, size };
        (*m_pStream) << canvasRef;
		(*m_pStream) << (pCanvasSurface ? static_cast<StreamSurface*>(pCanvasSurface)->m_inStreamId : (uint16_t) 0);
        (*m_pStream) << nUpdateRects;
        (*m_pStream) << GfxStream::DataChunk{ nUpdateRects*16, pUpdateRects };
		
		m_canvas.ref = canvasRef;
		m_canvas.pSurface = pCanvasSurface;
		
		m_canvas.size = sz;
		
		
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

