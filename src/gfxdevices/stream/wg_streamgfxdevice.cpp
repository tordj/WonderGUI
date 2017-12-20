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

		if (m_bRendering)
			_setFramebuffer();

		return true;
	}

	//____ _setFramebuffer() ____________________________________________________

	void StreamGfxDevice::_setFramebuffer()
	{

	}

	//____ setTintColor() __________________________________________________________

	void StreamGfxDevice::setTintColor( Color color )
	{
		GfxDevice::setTintColor(color);

    }

	//____ setBlendMode() __________________________________________________________

	bool StreamGfxDevice::setBlendMode( BlendMode blendMode )
	{
		if( blendMode != BlendMode::Blend && blendMode != BlendMode::Replace && 
			blendMode != BlendMode::Add && blendMode != BlendMode::Subtract && blendMode != BlendMode::Multiply &&
			blendMode != BlendMode::Invert )
				return false;
	 
		GfxDevice::setBlendMode(blendMode);
		if( m_bRendering )
			_setBlendMode(blendMode);

		return true;
	}

	//____ beginRender() ___________________________________________________________

	bool StreamGfxDevice::beginRender()
	{

        if( m_bRendering == true )
			return false;

		(*m_pStream) << GfxStream::Header{ GfxChunkId::BeginRender, 0 };

		// Set correct framebuffer

		_setFramebuffer();


		// Set correct blend mode

		_setBlendMode(m_blendMode);

        //

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

	void StreamGfxDevice::fill( const Rect& _rect, const Color& _col )
	{
		if( _col.a  == 0 || _rect.w < 1 || _rect.h < 1 )
			return;
 
		(*m_pStream) << GfxStream::Header{ GfxChunkId::Fill, 12 };
		(*m_pStream) << _rect;
		(*m_pStream) << _col;
  
		return;
	}

	//____ blit() __________________________________________________________________

	void StreamGfxDevice::blit( Surface * _pSrc, const Rect& _src, Coord dest  )
	{
        if( !_pSrc )
			return;       
	}

	//____ fillSubPixel() ______________________________________________________

	void StreamGfxDevice::fillSubPixel( const RectF& rect, const Color& col )
	{
        if( col.a  == 0 )
            return;
        
		(*m_pStream) << GfxStream::Header{ GfxChunkId::FillSubPixel, 20 };
		(*m_pStream) << rect;
		(*m_pStream) << col;

        return;
	}


	//____ stretchBlitSubPixel() ___________________________________________________

	void StreamGfxDevice::stretchBlitSubPixel( Surface * pSrc, float sx, float sy,
											 float sw, float sh,
											 float dx, float dy, float dw, float dh )
	{
		if( !pSrc )
			return;

    }

	//____ _setBlendMode() ____________________________________________________

	void StreamGfxDevice::_setBlendMode( BlendMode blendMode )
	{
		(*m_pStream) << GfxStream::Header{ GfxChunkId::SetBlendMode, 2 };
		(*m_pStream) << blendMode;
	}

	//____ _drawStraightLine() ________________________________________________

	void StreamGfxDevice::_drawStraightLine(Coord start, Orientation orientation, int length, const Color& color)
	{
		(*m_pStream) << GfxStream::Header{ GfxChunkId::DrawStraightLine, 12 };
		(*m_pStream) << start;
		(*m_pStream) << orientation;
		(*m_pStream) << (uint16_t) length;
		(*m_pStream) << color;

	}
	
	//____ clipPlotPixels() ________________________________________________________
	
	void StreamGfxDevice::clipPlotPixels( const Rect& clip, int nCoords, const Coord * pCoords, const Color * pColors)
    {
		if (nCoords == 0 || pCoords == nullptr || pColors == nullptr)
			return;
	}
    

	//____ plotPixels() ________________________________________________________
    
    void StreamGfxDevice::plotPixels( int nCoords, const Coord * pCoords, const Color * pColors)
    {
        if( nCoords == 0 )
            return;
        
	}


	//____ drawLine() __________________________________________________________

	void StreamGfxDevice::drawLine( Coord begin, Coord end, Color color, float thickness )
	{
		(*m_pStream) << GfxStream::Header{ GfxChunkId::DrawLine, 16 };
		(*m_pStream) << begin;
		(*m_pStream) << end;
		(*m_pStream) << color;
		(*m_pStream) << thickness;
	}
	
	//____ clipDrawLine() ______________________________________________________
	
	void StreamGfxDevice::clipDrawLine( const Rect& clip, Coord begin, Coord end, Color color, float thickness )
	{
		(*m_pStream) << GfxStream::Header{ GfxChunkId::ClipDrawLine, 24 };
		(*m_pStream) << clip;
		(*m_pStream) << begin;
		(*m_pStream) << end;
		(*m_pStream) << color;
		(*m_pStream) << thickness;
	}

	//____ clipDrawHorrWave() _____________________________________________________

	void StreamGfxDevice::clipDrawHorrWave(const Rect&clip, Coord begin, int length, const WaveLine* topBorder, const WaveLine* bottomBorder, Color frontFill, Color backFill)
	{
		// Need some smart optimizations here, so we don't send wave-values far outside clip or length.
	}
	
	
	
} // namespace wg

