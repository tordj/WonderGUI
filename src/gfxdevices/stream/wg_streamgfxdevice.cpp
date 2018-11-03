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
	const char StreamGfxDevice::CLASSNAME[] = { "StreamGfxDevice" };



    
    
    
	//____ create() _______________________________________________________________
	
	StreamGfxDevice_p StreamGfxDevice::create( Size canvas, GfxOutStream * pStream )
	{
		StreamGfxDevice_p p(new StreamGfxDevice( canvas, pStream ));		
		return p;
	}


	//____ Constructor _____________________________________________________________

	StreamGfxDevice::StreamGfxDevice( Size canvas, GfxOutStream * pStream ) : GfxDevice(canvas)
	{
		m_pStream = pStream;
		m_bRendering = false;
    }

	//____ Destructor ______________________________________________________________

	StreamGfxDevice::~StreamGfxDevice()
	{
	}

	//____ isInstanceOf() _________________________________________________________

	bool StreamGfxDevice::isInstanceOf( const char * pClassName ) const
	{ 		
		if( pClassName==CLASSNAME )
			return true;

		return GfxDevice::isInstanceOf(pClassName);
	}

	//____ className() ____________________________________________________________

	const char * StreamGfxDevice::className( void ) const
	{ 
		return CLASSNAME; 
	}

	//____ cast() _________________________________________________________________

	StreamGfxDevice_p StreamGfxDevice::cast( Object * pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return StreamGfxDevice_p( static_cast<StreamGfxDevice*>(pObject) );

		return 0;
	}

	//____ surfaceClassName() _______________________________________________________
	
	const char * StreamGfxDevice::surfaceClassName( void ) const
	{
		return StreamSurface::CLASSNAME;
	}
	
	//____ surfaceFactory() ______________________________________________________

	SurfaceFactory_p StreamGfxDevice::surfaceFactory()
	{
		if( !m_pSurfaceFactory )
			m_pSurfaceFactory = StreamSurfaceFactory::create(m_pStream);
	
		return m_pSurfaceFactory;
	}

	//____ setClip() __________________________________________________________

	void StreamGfxDevice::setClip(const Rect& clip)
	{
		(*m_pStream) << GfxStream::Header{ GfxChunkId::SetClip, 8 };
		(*m_pStream) << clip;
	}


	//____ setCanvas() __________________________________________________________________

	bool StreamGfxDevice::setCanvas( Surface * _pSurface )
	{
		if (_pSurface)
		{
			StreamSurface * pSurface = StreamSurface::cast(_pSurface);
			if (!pSurface)
				return false;			// Surface must be of type StreamSurface!
		}

		m_pCanvas		= _pSurface;

		(*m_pStream) << GfxStream::Header{ GfxChunkId::SetCanvas, 2 };
		(*m_pStream) << static_cast<StreamSurface*>(m_pCanvas.rawPtr())->m_inStreamId;

		return true;
	}

	//____ setTintColor() __________________________________________________________

	void StreamGfxDevice::setTintColor( Color color )
	{
		GfxDevice::setTintColor(color);

		(*m_pStream) << GfxStream::Header{ GfxChunkId::SetTintColor, 2 };
		(*m_pStream) << color;
    }

	//____ setBlendMode() __________________________________________________________

	bool StreamGfxDevice::setBlendMode( BlendMode blendMode )
	{
		if( blendMode != BlendMode::Blend && blendMode != BlendMode::Replace && 
			blendMode != BlendMode::Add && blendMode != BlendMode::Subtract && blendMode != BlendMode::Multiply &&
			blendMode != BlendMode::Invert )
				return false;
	 
		GfxDevice::setBlendMode(blendMode);

		(*m_pStream) << GfxStream::Header{ GfxChunkId::SetBlendMode, 2 };
		(*m_pStream) << blendMode;

		return true;
	}

	//____ setBlitSource() _______________________________________________________

	bool StreamGfxDevice::setBlitSource(Surface * pSource)
	{
		if (!pSource)
			return false;

		(*m_pStream) << GfxStream::Header{ GfxChunkId::SetBlitSource, 2 };
		(*m_pStream) << static_cast<StreamSurface*>(m_pCanvas.rawPtr())->m_inStreamId;
		return true;
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

	//____ fill() __________________________________________________________________

	void StreamGfxDevice::fill(const Color& col)
	{
		fill(m_clip, col);
	}

	void StreamGfxDevice::fill( const Rect& _rect, const Color& _col )
	{
		if( _col.a  == 0 || _rect.w < 1 || _rect.h < 1 )
			return;
 
		(*m_pStream) << GfxStream::Header{ GfxChunkId::Fill, 12 };
		(*m_pStream) << _rect;
		(*m_pStream) << _col;
  
		return;
	}

	void StreamGfxDevice::fill(const RectF& rect, const Color& col)
	{
		if (col.a == 0)
			return;

		(*m_pStream) << GfxStream::Header{ GfxChunkId::FillSubpixel, 20 };
		(*m_pStream) << rect;
		(*m_pStream) << col;

		return;
	}

	//____ plotPixels() ________________________________________________________

	void StreamGfxDevice::plotPixels(int nCoords, const Coord * pCoords, const Color * pColors)
	{
		// Each pixel is packed down to 4 + 4 bytes: int16_t x, int16_t y, Color
		// All coordinates comes first, then all colors.

		if (nCoords == 0)
			return;

		int maxChunkCoords = (int)(GfxStream::c_maxBlockSize - sizeof(GfxStream::Header)) / 8;

		int chunkCoords = min(nCoords, maxChunkCoords);

		int bufferSize = chunkCoords * (4);

		int16_t * pBuffer = reinterpret_cast<short*>(Base::memStackAlloc(bufferSize));

		while (nCoords > 0)
		{
			int16_t * p = pBuffer;

			for (int i = 0; i < chunkCoords; i++)
			{
				*p++ = (int16_t)pCoords[i].x;
				*p++ = (int16_t)pCoords[i].y;
			}

			*m_pStream << GfxStream::Header{ GfxChunkId::PlotPixels, chunkCoords * 8 };
			*m_pStream << GfxStream::DataChunk{ chunkCoords * 4, pBuffer };
			*m_pStream << GfxStream::DataChunk{ chunkCoords * 4, pColors };


			nCoords -= chunkCoords;
			pCoords += chunkCoords;
			pColors += chunkCoords;
			chunkCoords = min(nCoords, maxChunkCoords);
		}

		Base::memStackRelease(bufferSize);
	}

	//____ drawLine() __________________________________________________________

	void StreamGfxDevice::drawLine(Coord begin, Coord end, Color color, float thickness)
	{
		(*m_pStream) << GfxStream::Header{ GfxChunkId::DrawLineFromTo, 16 };
		(*m_pStream) << begin;
		(*m_pStream) << end;
		(*m_pStream) << color;
		(*m_pStream) << thickness;
	}
	
	void StreamGfxDevice::drawLine(Coord begin, Direction dir, int length, Color col, float thickness)
	{
		(*m_pStream) << GfxStream::Header{ GfxChunkId::DrawLineStraight, 16 };
		(*m_pStream) << begin;
		(*m_pStream) << dir;
		(*m_pStream) << (uint16_t)length;
		(*m_pStream) << col;
		(*m_pStream) << thickness;
	}

	//____ fillPatches() _________________________________________________________

	void StreamGfxDevice::fillPatches(const Rect& rect, const Color& col, int nPatches, const Rect * pPatches)
	{
		if (nPatches == 0)
			return;

		(*m_pStream) << GfxStream::Header{ GfxChunkId::FillPatches, 14 + nPatches*8 };
		(*m_pStream) << rect;
		(*m_pStream) << col;
		_addPatches(nPatches, pPatches);
	}

	void StreamGfxDevice::fillPatches(const RectF& rect, const Color& col, int nPatches, const Rect * pPatches)
	{
		if (nPatches == 0)
			return;

		(*m_pStream) << GfxStream::Header{ GfxChunkId::FillSubpixelPatches, 22 + nPatches * 8 };
		(*m_pStream) << rect;
		(*m_pStream) << col;
		_addPatches(nPatches, pPatches);
	}

	//____ plotPixelPatches() _________________________________________________

	void StreamGfxDevice::plotPixelPatches(int nCoords, const Coord * pCoords, const Color * pColors, int nPatches, const Rect * pPatches)
	{
		// Each pixel is packed down to 4 + 4 bytes: int16_t x, int16_t y, Color
		// First comes number of patches, then the patches, then all coordinates and last all colors.

		if (nCoords == 0 || nPatches == 0)
			return;

		int maxChunkCoords = (int)(GfxStream::c_maxBlockSize - sizeof(GfxStream::Header) - 2 - nPatches*8) / 8;

		int chunkCoords = min(nCoords, maxChunkCoords);

		int bufferSize = chunkCoords * (4);

		int16_t * pBuffer = reinterpret_cast<short*>(Base::memStackAlloc(bufferSize));

		while (nCoords > 0)
		{
			int16_t * p = pBuffer;

			for (int i = 0; i < chunkCoords; i++)
			{
				*p++ = (int16_t)pCoords[i].x;
				*p++ = (int16_t)pCoords[i].y;
			}

			*m_pStream << GfxStream::Header{ GfxChunkId::PlotPixelPatches, chunkCoords * 8 };
			_addPatches(nPatches, pPatches);

			*m_pStream << GfxStream::DataChunk{ chunkCoords * 4, pBuffer };
			*m_pStream << GfxStream::DataChunk{ chunkCoords * 4, pColors };


			nCoords -= chunkCoords;
			pCoords += chunkCoords;
			pColors += chunkCoords;
			chunkCoords = min(nCoords, maxChunkCoords);
		}

		Base::memStackRelease(bufferSize);
	}

	//____ drawLinePatches() __________________________________________________

	void StreamGfxDevice::drawLinePatches(Coord begin, Coord end, Color color, float thickness, int nPatches, const Rect * pPatches)
	{
		if (nPatches == 0)
			return;

		(*m_pStream) << GfxStream::Header{ GfxChunkId::DrawLineFromTo, 18 + nPatches*8 };
		(*m_pStream) << begin;
		(*m_pStream) << end;
		(*m_pStream) << color;
		(*m_pStream) << thickness;
		_addPatches(nPatches, pPatches);
	}

	void StreamGfxDevice::drawLinePatches(Coord begin, Direction dir, int length, Color col, float thickness, int nPatches, const Rect * pPatches)
	{
		if (nPatches == 0)
			return;

		(*m_pStream) << GfxStream::Header{ GfxChunkId::DrawLineStraight, 18 + nPatches*8 };
		(*m_pStream) << begin;
		(*m_pStream) << dir;
		(*m_pStream) << (uint16_t)length;
		(*m_pStream) << col;
		(*m_pStream) << thickness;
		_addPatches(nPatches, pPatches);
	}

	//____ blit() __________________________________________________________________

	void StreamGfxDevice::blit(Coord dest, const Rect& _src)
	{
		if (_src.w < 1 || _src.h < 1)
			return;

		(*m_pStream) << GfxStream::Header{ GfxChunkId::Blit, 12 };
		(*m_pStream) << dest;
		(*m_pStream) << _src;
	}

	//____ stretchBlit() ___________________________________________________

	void StreamGfxDevice::stretchBlit(const Rect& dest, const RectF& source)
	{

		(*m_pStream) << GfxStream::Header{ GfxChunkId::StretchBlit, 24 };
		(*m_pStream) << dest;
		(*m_pStream) << source;
	}

	//____ transformBlitPatches() _____________________________________________

	void StreamGfxDevice::transformBlitPatches(const Rect& dest, Coord src, const int simpleTransform[2][2], int nPatches, const Rect * pPatches)
	{
		if (nPatches == 0)
			return;

		(*m_pStream) << GfxStream::Header{ GfxChunkId::SimpleTransformBlitPatches, 18 + nPatches*8 };
		(*m_pStream) << dest;
		(*m_pStream) << src;
		(*m_pStream) << simpleTransform;
		_addPatches(nPatches, pPatches);
	}

	void StreamGfxDevice::transformBlitPatches(const Rect& dest, CoordF src, const float complexTransform[2][2], int nPatches, const Rect * pPatches)
	{
		if (nPatches == 0)
			return;

		(*m_pStream) << GfxStream::Header{ GfxChunkId::SimpleTransformBlitPatches, 34 + nPatches * 8 };
		(*m_pStream) << dest;
		(*m_pStream) << src;
		(*m_pStream) << complexTransform;
		_addPatches(nPatches, pPatches);
	}

	//____ transformDrawSegmentPatches() ______________________________________

	void StreamGfxDevice::transformDrawSegmentPatches(const Rect& dest, int nSegments, Color * pSegmentColors, int nEdges, int * pEdges, int edgeStripPitch, const int simpleTransform[2][2], int nPatches, const Rect * pPatches)
	{
		//TODO: Implement!
	}

	//____ _addPatches() ______________________________________________________

	void StreamGfxDevice::_addPatches(int nPatches, const Rect * pPatches)
	{
		(*m_pStream) << (uint16_t) nPatches;

		for (int i = 0; i < nPatches ; i++)
			(*m_pStream) << pPatches[i];
	}



} // namespace wg

