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

#include <wg3_streamgfxdevice.h>
#include <wg3_streamsurface.h>
#include <wg3_streamsurfacefactory.h>
#include <wg3_base.h>
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

	//____ surfaceFactory() ______________________________________________________

	SurfaceFactory_p StreamGfxDevice::surfaceFactory()
	{
		if( !m_pSurfaceFactory )
			m_pSurfaceFactory = StreamSurfaceFactory::create(*m_pStream);

		return m_pSurfaceFactory;
	}

    //____ setClipList() _________________________________________________________

    bool StreamGfxDevice::setClipList(int nRectangles, const RectI * pRectangles)
    {
        
    }

    //____ clearClipList() _______________________________________________________

    void StreamGfxDevice::clearClipList()
    {
        
    }

    //____ pushClipList() ________________________________________________________

    bool StreamGfxDevice::pushClipList(int nRectangles, const RectI* pRectangles)
    {
        
    }

    //____ popClipList() __________________________________________________________

    bool StreamGfxDevice::popClipList()
    {
        
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
        
    }

    //____ clearTintGradient() _____________________________________________________

    void StreamGfxDevice::clearTintGradient()
    {
        
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
        
    }

    //____ endCanvasUpdate() _________________________________________________________

    void StreamGfxDevice::endCanvasUpdate()
    {
        
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
            *m_pStream << GfxStream::Header{ GfxChunkId::PlotPixels, chunkCoords * 16 };
            *m_pStream << GfxStream::DataChunk{ chunkCoords * 8, pCoords };
            *m_pStream << GfxStream::DataChunk{ chunkCoords * 8, pColors };

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

// Blit methods

void    blit(CoordI dest) override;
void    blit(CoordI dest, const RectI& src) override;

void    flipBlit(CoordI dest, GfxFlip flip ) override;
void    flipBlit(CoordI dest, const RectI& src, GfxFlip flip ) override;

void    stretchBlit(const RectI& dest) override;
void    stretchBlit(const RectI& dest, const RectI& src) override;
void    stretchBlit(const RectI& dest, const RectF& src) override;

void    stretchFlipBlit(const RectI& dest, GfxFlip flip) override;
void    stretchFlipBlit(const RectI& dest, const RectI& src, GfxFlip flip) override;
void    stretchFlipBlit(const RectI& dest, const RectF& src, GfxFlip flip) override;

void    rotScaleBlit(const RectI& dest, float rotationDegrees, float scale, CoordF srcCenter = { 0.5f, 0.5f }, CoordF destCenter = { 0.5f,0.5f }) override;

void    tile(const RectI& dest, CoordI shift = { 0,0 }) override;
void    flipTile(const RectI& dest, GfxFlip flip, CoordI shift = { 0,0 }) override;

void    scaleTile(const RectI& dest, float scale, CoordI shift = { 0,0 }) override;
void    scaleFlipTile(const RectI& dest, float scale, GfxFlip flip, CoordI shift = { 0,0 }) override;


// Draw segments methods

void    drawWave(const RectI& dest, const WaveLine * pTopBorder, const WaveLine * pBottomBorder, HiColor frontFill, HiColor backFill) override;
void    flipDrawWave(const RectI& dest, const WaveLine * pTopBorder, const WaveLine * pBottomBorder, HiColor frontFill, HiColor backFill, GfxFlip flip) override;

void    drawElipse(const RectF& canvas, float thickness, HiColor color, float outlineThickness = 0.f, HiColor outlineColor = Color::Black) override;

void    drawPieChart(const RectI& canvas, float start, int nSlices, const float * pSliceSizes, const HiColor * pSliceColors, float hubSize = 0.f, HiColor hubColor = Color::Transparent, HiColor backColor = Color::Transparent, bool bRectangular = false) override;

void    drawSegments(const RectI& dest, int nSegments, const HiColor * pSegmentColors, int nEdgeStrips, const int * pEdgeStrips, int edgeStripPitch, TintMode tintMode = TintMode::Flat ) override;
void    flipDrawSegments(const RectI& dest, int nSegments, const HiColor * pSegmentColors, int nEdgeStrips, const int * pEdgeStrips, int edgeStripPitch, GfxFlip flip, TintMode tintMode = TintMode::Flat) override;


// Special draw/blit methods

void    blitNinePatch(const RectI& dstRect, const BorderI& dstFrame, const NinePatch& patch) override;




//.____ Deprecated ________________________________________________

void    blitHorrBar( const RectI& _src, const BorderI& _borders, bool _bTile, CoordI dest, int _len ) override;

void    blitVertBar( const RectI& _src, const BorderI& _borders, bool _bTile, CoordI dest, int _len ) override;





	//____ blit() __________________________________________________________________

	void StreamGfxDevice::blit(CoordI dest, const RectI& _src)
	{
		if (_src.w < 1 || _src.h < 1)
			return;

		(*m_pStream) << GfxStream::Header{ GfxChunkId::Blit, 12 };
		(*m_pStream) << dest;
		(*m_pStream) << _src;
	}

	//____ stretchBlit() ___________________________________________________

	void StreamGfxDevice::stretchBlit(const RectI& dest, const RectF& source)
	{

		(*m_pStream) << GfxStream::Header{ GfxChunkId::StretchBlit, 24 };
		(*m_pStream) << dest;
		(*m_pStream) << source;
	}

	//____ _clipListWasChanged() ______________________________________________

	void StreamGfxDevice::_clipListWasChanged()
	{
		(*m_pStream) << GfxStream::Header{ GfxChunkId::SetClip, 8 * m_nClipRects };

		for (int i = 0; i < m_nClipRects; i++)
			(*m_pStream) << m_pClipRects[i];
	}


	//____ _transformBlit() _____________________________________________

	void StreamGfxDevice::_transformBlit(const RectI& dest, CoordI src, const int simpleTransform[2][2])
	{
		(*m_pStream) << GfxStream::Header{ GfxChunkId::SimpleTransformBlit, 18 };
		(*m_pStream) << dest;
		(*m_pStream) << src;
		(*m_pStream) << simpleTransform;
	}

	void StreamGfxDevice::_transformBlit(const RectI& dest, CoordF src, const float complexTransform[2][2])
	{
		(*m_pStream) << GfxStream::Header{ GfxChunkId::ComplexTransformBlit, 34 };
		(*m_pStream) << dest;
		(*m_pStream) << src;
		(*m_pStream) << complexTransform;
	}

	//____ _transformDrawSegments() ______________________________________

	void StreamGfxDevice::_transformDrawSegments(const RectI& dest, int nSegments, const HiColor * pSegmentColors, int nEdgeStrips, const int * pEdgeStrips, int edgeStripPitch, TintMode tintMode, const int simpleTransform[2][2])
	{
		//TODO: TintMode not accounted for.

		//NOTE: Precision of edge data is scaled down to 4 binals and there is a limitation of 4095 pixels height of the segment waveform to keep data compact.

		// Generate the TransformDrawSegmentPatches chunk.

		(*m_pStream) << GfxStream::Header{ GfxChunkId::TransformDrawSegments, 18 + nSegments * 8 };
		(*m_pStream) << dest;
		(*m_pStream) << (uint16_t) nSegments;
		(*m_pStream) << (uint16_t) nEdgeStrips;
		(*m_pStream) << simpleTransform;

		for( int i = 0 ; i < nSegments; i++ )
			(*m_pStream) << pSegmentColors[i];

		// Compress our edge data

		int nEdges = nSegments - 1;
		int nEdgeEntries = nEdgeStrips * nEdges;
		int allocSize = nEdgeEntries * 2;

		int16_t * pPackedEdges = (int16_t*) Base::memStackAlloc(allocSize);
		int16_t * wp = pPackedEdges;

		for (int strip = 0; strip < nEdgeStrips; strip++)
		{
			for (int i = 0; i < nEdges; i++)
				*wp++ = (int16_t) (pEdgeStrips[i] >> 4);
			pEdgeStrips += edgeStripPitch;
		}

		// Write the chunks of edge samples

		int maxEntriesPerChunk = (GfxStream::c_maxBlockSize - sizeof(GfxStream::Header)) / 2;

		while (nEdgeEntries > 0)
		{
			int chunkEntries = min(nEdgeEntries, maxEntriesPerChunk);

			(*m_pStream) << GfxStream::Header{ GfxChunkId::EdgeSamples, chunkEntries*2 };
			(*m_pStream) << GfxStream::DataChunk{ chunkEntries*2, pPackedEdges };

			pPackedEdges += chunkEntries;
			nEdgeEntries -= chunkEntries;
		}

		// Clean up

		Base::memStackRelease(allocSize);
	}


} // namespace wg

