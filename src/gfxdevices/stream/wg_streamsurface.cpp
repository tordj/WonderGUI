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

#include <memory.h>

#include <wg_streamsurface.h>
#include <wg_util.h>
#include <wg_blob.h>
#include <assert.h>



namespace wg
{
	const char StreamSurface::CLASSNAME[] = {"StreamSurface"};

	//____ maxSize() _______________________________________________________________

	Size StreamSurface::maxSize()
	{
		return Size(4096,4096);
	}

	//____ create ______________________________________________________________

    StreamSurface_p	StreamSurface::create( Size size, PixelType type, SurfaceHint hint )
    {
        if( type != PixelType::BGRA_8 && type != PixelType::BGR_8)
            return StreamSurface_p();
        
        return StreamSurface_p(new StreamSurface(size,type,hint));
    }
    
    StreamSurface_p	StreamSurface::create( Size size, PixelType type, Blob * pBlob, int pitch, SurfaceHint hint )
    {
        if( (type != PixelType::BGRA_8 && type != PixelType::BGR_8) || !pBlob || pitch % 4 != 0 )
            return StreamSurface_p();
        
        return StreamSurface_p(new StreamSurface(size,type,pBlob,pitch,hint));
    }
    
    StreamSurface_p	StreamSurface::create( Size size, PixelType type, uint8_t * pPixels, int pitch, const PixelFormat * pPixelFormat, SurfaceHint hint )
    {
        if( (type != PixelType::BGRA_8 && type != PixelType::BGR_8) || pPixels == 0 )
            return StreamSurface_p();
        
        return  StreamSurface_p(new StreamSurface(size,type,pPixels,pitch, pPixelFormat,hint));
    };
    
    StreamSurface_p	StreamSurface::create( Surface * pOther, SurfaceHint hint )
    {
        return StreamSurface_p(new StreamSurface( pOther,hint ));
    }

    
    
	//____ Constructor _____________________________________________________________


    StreamSurface::StreamSurface( Size size, PixelType type, SurfaceHint hint )
    {
		assert( type == PixelType::BGR_8 || type == PixelType::BGRA_8 );

    }
    
    
	StreamSurface::StreamSurface( Size size, PixelType type, Blob * pBlob, int pitch, SurfaceHint hint )
	{
		assert( (type == PixelType::BGR_8 || type == PixelType::BGRA_8) && pBlob && pitch % 4 == 0 );
	}
   
    StreamSurface::StreamSurface( Size size, PixelType type, uint8_t * pPixels, int pitch, const PixelFormat * pPixelFormat, SurfaceHint hint )
    {
		assert( (type == PixelType::BGR_8 || type == PixelType::BGRA_8) && pPixels != 0 );		
    }


    StreamSurface::StreamSurface( Surface * pOther, SurfaceHint hint )
    {
		assert( pOther );
		
    }
    
    
	//____ Destructor ______________________________________________________________

	StreamSurface::~StreamSurface()
	{
	}

	//____ isInstanceOf() _________________________________________________________

	bool StreamSurface::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;

		return Surface::isInstanceOf(pClassName);
	}

	//____ className() ____________________________________________________________

	const char * StreamSurface::className( void ) const
	{ 
		return CLASSNAME; 
	}

	//____ cast() _________________________________________________________________

	StreamSurface_p StreamSurface::cast( Object * pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return StreamSurface_p( static_cast<StreamSurface*>(pObject) );

		return 0;
	}

	//____ setScaleMode() __________________________________________________________

	void StreamSurface::setScaleMode( ScaleMode mode )
	{
		switch( mode )
		{
			case ScaleMode::Interpolate:
				break;
				
			case ScaleMode::Nearest:
			default:
				break;
		}
		
		Surface::setScaleMode(mode);
	}

	//____ size() ______________________________________________________________

	Size StreamSurface::size() const
	{
		return m_size;
	}

	//____ isOpaque() ______________________________________________________________

	bool StreamSurface::isOpaque() const
	{
//		if( m_buffer && m_internalFormat == Io_RGB )
//			return true;

		return false;
	}

	//____ lock() __________________________________________________________________

	uint8_t * StreamSurface::lock( AccessMode mode )
	{
		if( m_accessMode != AccessMode::None || mode == AccessMode::None )
			return 0;
/*
		IoBindBuffer( Io_PIXEL_UNPACK_BUFFER, m_buffer );

		switch( mode )
		{
			case AccessMode::ReadOnly:
				m_pPixels = (uint8_t*) IoMapBuffer( Io_PIXEL_UNPACK_BUFFER, Io_READ_ONLY );
				break;
			case AccessMode::WriteOnly:
				m_pPixels = (uint8_t*) IoMapBuffer( Io_PIXEL_UNPACK_BUFFER, Io_WRITE_ONLY );
				break;
			case AccessMode::ReadWrite:
				m_pPixels = (uint8_t*) IoMapBuffer( Io_PIXEL_UNPACK_BUFFER, Io_READ_WRITE );
				break;
			default:
				break;	// Should never happen, just here to avoid compiler warnings...
		}
*/
		m_lockRegion = Rect(0,0,m_size);
		m_accessMode = mode;
		return m_pPixels;
	}

	//____ lockRegion() __________________________________________________________________

	uint8_t * StreamSurface::lockRegion( AccessMode mode, const Rect& region )
	{
		if( m_accessMode != AccessMode::None || mode == AccessMode::None )
			return 0;

		if( region.x + region.w > m_size.w || region.y + region.w > m_size.h || region.x < 0 || region.y < 0 )
			return 0;
/*
		IoBindBuffer( Io_PIXEL_UNPACK_BUFFER, m_buffer );

		switch( mode )
		{
			case AccessMode::ReadOnly:
				m_pPixels = (uint8_t*) IoMapBuffer( Io_PIXEL_UNPACK_BUFFER, Io_READ_ONLY );
				break;
			case AccessMode::WriteOnly:
				m_pPixels = (uint8_t*) IoMapBuffer( Io_PIXEL_UNPACK_BUFFER, Io_WRITE_ONLY );
				break;
			case AccessMode::ReadWrite:
				m_pPixels = (uint8_t*) IoMapBuffer( Io_PIXEL_UNPACK_BUFFER, Io_READ_WRITE );
				break;
			default:
				break;	// Should never happen, just here to avoid compiler warnings...
		}
*/
		m_lockRegion = region;
		m_accessMode = mode;
		return m_pPixels += (m_size.w*region.y+region.x)*m_pixelSize;
	}


	//____ unlock() ________________________________________________________________

	void StreamSurface::unlock()
	{
		AccessMode a = AccessMode::None;
		

		if(a == AccessMode::None )
			return;
/*
		IoUnmapBuffer( Io_PIXEL_UNPACK_BUFFER );

		if( m_accessMode != AccessMode::ReadOnly )
		{
			IoBindTexture( Io_TEXTURE_2D, m_texture );
	//		IoBindBuffer( Io_PIXEL_UNPACK_BUFFER, m_buffer );
			IoTexSubImage2D( Io_TEXTURE_2D, 0, 0, 0, m_size.w, m_size.h, m_accessFormat, Io_UNSIGNED_BYTE, 0 );
	//		IoTexSubImage2D( Io_TEXTURE_2D, 0, m_lockRegion.x, m_lockRegion.y, m_lockRegion.w, m_lockRegion.h, Io_RGBA, Io_UNSIGNED_BYTE, 0 );
		}
		IoBindBuffer( Io_PIXEL_UNPACK_BUFFER, 0 );
*/
		m_accessMode = AccessMode::None;
		m_pPixels = 0;
		m_lockRegion.w = 0;
		m_lockRegion.h = 0;
	}


	//____ pixel() ______________________________________________________________

	uint32_t StreamSurface::pixel( Coord coord ) const
	{
		return 0;
	}



	//____ alpha() ____________________________________________________________

	uint8_t StreamSurface::alpha( Coord coord ) const
	{
        return 255;
	}
} // namespace wg
