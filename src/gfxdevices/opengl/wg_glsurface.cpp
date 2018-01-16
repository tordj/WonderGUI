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

#include <wg_glsurface.h>
#include <wg_util.h>
#include <wg_blob.h>
#include <assert.h>



namespace wg
{
	const char GlSurface::CLASSNAME[] = {"GlSurface"};

	//____ maxSize() _______________________________________________________________

	Size GlSurface::maxSize()
	{
		GLint max = 0;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max);
		return Size(max,max);
	}

	//____ create ______________________________________________________________

    GlSurface_p	GlSurface::create( Size size, PixelType type, int hint )
    {
        if( type != PixelType::BGRA_8 && type != PixelType::BGR_8)
            return GlSurface_p();
        
        return GlSurface_p(new GlSurface(size,type,hint));
    }
    
    GlSurface_p	GlSurface::create( Size size, PixelType type, Blob * pBlob, int pitch, int hint )
    {
        if( (type != PixelType::BGRA_8 && type != PixelType::BGR_8) || !pBlob || pitch % 4 != 0 )
            return GlSurface_p();
        
        return GlSurface_p(new GlSurface(size,type,pBlob,pitch,hint));
    }
    
    GlSurface_p	GlSurface::create( Size size, PixelType type, uint8_t * pPixels, int pitch, const PixelFormat * pPixelFormat, int hint )
    {
        if( (type != PixelType::BGRA_8 && type != PixelType::BGR_8) || pPixels == 0 )
            return GlSurface_p();
        
        return  GlSurface_p(new GlSurface(size,type,pPixels,pitch, pPixelFormat,hint));
    };
    
    GlSurface_p	GlSurface::create( Surface * pOther, int hint )
    {
        return GlSurface_p(new GlSurface( pOther,hint ));
    }

    
    
	//____ Constructor _____________________________________________________________


    GlSurface::GlSurface( Size size, PixelType type, int hint )
    {
		assert( type == PixelType::BGR_8 || type == PixelType::BGRA_8 );

        _setPixelDetails(type);
        m_size	= size;
        m_pitch = ((size.w*m_pixelFormat.bits/8)+3)&0xFFFFFFFC;
		
        glGenBuffers( 1, &m_buffer );
        glBindBuffer( GL_PIXEL_UNPACK_BUFFER, m_buffer );
        glBufferData( GL_PIXEL_UNPACK_BUFFER, m_pitch*size.h, 0, GL_STATIC_DRAW );
        glBindBuffer( GL_PIXEL_UNPACK_BUFFER, 0 );

        glGenTextures( 1, &m_texture );
        glBindTexture( GL_TEXTURE_2D, m_texture );
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

        glTexImage2D( GL_TEXTURE_2D, 0, m_internalFormat, m_size.w, m_size.h, 0,
                     m_accessFormat, GL_UNSIGNED_BYTE, NULL );
    }
    
    
	GlSurface::GlSurface( Size size, PixelType type, Blob * pBlob, int pitch, int hint )
	{
		assert( (type == PixelType::BGR_8 || type == PixelType::BGRA_8) && pBlob && pitch % 4 == 0 );

        // Set general information
        
        _setPixelDetails(type);
        m_size	= size;
        m_pitch = pitch;
		
        glGenBuffers( 1, &m_buffer );
        glBindBuffer( GL_PIXEL_UNPACK_BUFFER, m_buffer );
        glBufferData( GL_PIXEL_UNPACK_BUFFER, m_pitch*size.h, pBlob->data(), GL_STREAM_DRAW );

		glGenTextures( 1, &m_texture );
        glBindTexture( GL_TEXTURE_2D, m_texture );
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

		glTexImage2D( GL_TEXTURE_2D, 0, m_internalFormat, m_size.w, m_size.h, 0,
			m_accessFormat, GL_UNSIGNED_BYTE, NULL );

        glBindBuffer( GL_PIXEL_UNPACK_BUFFER, 0 );
	}
   
    GlSurface::GlSurface( Size size, PixelType type, uint8_t * pPixels, int pitch, const PixelFormat * pPixelFormat, int hint )
    {
		assert( (type == PixelType::BGR_8 || type == PixelType::BGRA_8) && pPixels != 0 );
		
       _setPixelDetails(type);
        m_size	= size;
        m_pitch = ((size.w*m_pixelFormat.bits/8)+3)&0xFFFFFFFC;
        Blob_p pBlob = Blob::create(m_pitch*m_size.h);
        
        m_pPixels = (uint8_t *) pBlob->data();
        _copyFrom( pPixelFormat==0 ? &m_pixelFormat:pPixelFormat, pPixels, pitch, size, size );
        m_pPixels = 0;
        
        glGenBuffers( 1, &m_buffer );
        glBindBuffer( GL_PIXEL_UNPACK_BUFFER, m_buffer );
        glBufferData( GL_PIXEL_UNPACK_BUFFER, m_pitch*size.h, pBlob->data(), GL_STREAM_DRAW );
        
        glGenTextures( 1, &m_texture );
        glBindTexture( GL_TEXTURE_2D, m_texture );
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
		
        glTexImage2D( GL_TEXTURE_2D, 0, m_internalFormat, m_size.w, m_size.h, 0,
                     m_accessFormat, GL_UNSIGNED_BYTE, NULL );
        
        glBindBuffer( GL_PIXEL_UNPACK_BUFFER, 0 );
    }


    GlSurface::GlSurface( Surface * pOther, int hint )
    {
		assert( pOther );
		
        _setPixelDetails(pOther->pixelFormat()->type);
        m_size	= pOther->size();
        m_pitch = m_size.w * m_pixelSize;
        Blob_p pBlob = Blob::create(m_pitch*m_size.h);
        
        m_pPixels = (uint8_t *) pBlob->data();
        _copyFrom( pOther->pixelFormat(), (uint8_t*)pOther->pixels(), pOther->pitch(), m_size, m_size );
        m_pPixels = 0;
        
        glGenBuffers( 1, &m_buffer );
        glBindBuffer( GL_PIXEL_UNPACK_BUFFER, m_buffer );
        glBufferData( GL_PIXEL_UNPACK_BUFFER, m_pitch*m_size.h, pBlob->data(), GL_STREAM_DRAW );
        
        glGenTextures( 1, &m_texture );
        glBindTexture( GL_TEXTURE_2D, m_texture );
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
		
        glTexImage2D( GL_TEXTURE_2D, 0, m_internalFormat, m_size.w, m_size.h, 0,
                     m_accessFormat, GL_UNSIGNED_BYTE, NULL );
        
        glBindBuffer( GL_PIXEL_UNPACK_BUFFER, 0 );
    }
    
    

	void GlSurface::_setPixelDetails( PixelType type )
	{
        if( type == PixelType::BGR_8 )
        {
            m_internalFormat = GL_RGB8;
            m_accessFormat = GL_BGR;
            m_pixelSize = 3;
        }
        else
        {
            m_internalFormat = GL_RGBA8;
            m_accessFormat = GL_BGRA;
            m_pixelSize = 4;
        }
        
        Util::pixelTypeToFormat(type, m_pixelFormat);
	}

	//____ Destructor ______________________________________________________________

	GlSurface::~GlSurface()
	{
		// Free the stuff

		if( m_buffer )
			glDeleteBuffers ( 1, &m_buffer );

		glDeleteTextures( 1, &m_texture );
	}

	//____ isInstanceOf() _________________________________________________________

	bool GlSurface::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;

		return Surface::isInstanceOf(pClassName);
	}

	//____ className() ____________________________________________________________

	const char * GlSurface::className( void ) const
	{ 
		return CLASSNAME; 
	}

	//____ cast() _________________________________________________________________

	GlSurface_p GlSurface::cast( Object * pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return GlSurface_p( static_cast<GlSurface*>(pObject) );

		return 0;
	}

	//____ setScaleMode() __________________________________________________________

	void GlSurface::setScaleMode( ScaleMode mode )
	{
		switch( mode )
		{
			case ScaleMode::Interpolate:
				glBindTexture( GL_TEXTURE_2D, m_texture );
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
				break;
				
			case ScaleMode::Nearest:
			default:
				glBindTexture( GL_TEXTURE_2D, m_texture );
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
				break;
		}
		
		Surface::setScaleMode(mode);
	}

	//____ size() ______________________________________________________________

	Size GlSurface::size() const
	{
		return m_size;
	}

	//____ isOpaque() ______________________________________________________________

	bool GlSurface::isOpaque() const
	{
		if( m_buffer && m_internalFormat == GL_RGB )
			return true;

		return false;
	}

	//____ lock() __________________________________________________________________

	uint8_t * GlSurface::lock( AccessMode mode )
	{
		if( m_accessMode != AccessMode::None || mode == AccessMode::None )
			return 0;

		glBindBuffer( GL_PIXEL_UNPACK_BUFFER, m_buffer );

		switch( mode )
		{
			case AccessMode::ReadOnly:
				m_pPixels = (uint8_t*) glMapBuffer( GL_PIXEL_UNPACK_BUFFER, GL_READ_ONLY );
				break;
			case AccessMode::WriteOnly:
				m_pPixels = (uint8_t*) glMapBuffer( GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY );
				break;
			case AccessMode::ReadWrite:
				m_pPixels = (uint8_t*) glMapBuffer( GL_PIXEL_UNPACK_BUFFER, GL_READ_WRITE );
				break;
			default:
				break;	// Should never happen, just here to avoid compiler warnings...
		}

		m_lockRegion = Rect(0,0,m_size);
		m_accessMode = mode;
		return m_pPixels;
	}

	//____ lockRegion() __________________________________________________________________

	uint8_t * GlSurface::lockRegion( AccessMode mode, const Rect& region )
	{
		if( m_accessMode != AccessMode::None || mode == AccessMode::None )
			return 0;

		if( region.x + region.w > m_size.w || region.y + region.w > m_size.h || region.x < 0 || region.y < 0 )
			return 0;

		glBindBuffer( GL_PIXEL_UNPACK_BUFFER, m_buffer );

		switch( mode )
		{
			case AccessMode::ReadOnly:
				m_pPixels = (uint8_t*) glMapBuffer( GL_PIXEL_UNPACK_BUFFER, GL_READ_ONLY );
				break;
			case AccessMode::WriteOnly:
				m_pPixels = (uint8_t*) glMapBuffer( GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY );
				break;
			case AccessMode::ReadWrite:
				m_pPixels = (uint8_t*) glMapBuffer( GL_PIXEL_UNPACK_BUFFER, GL_READ_WRITE );
				break;
			default:
				break;	// Should never happen, just here to avoid compiler warnings...
		}

		m_lockRegion = region;
		m_accessMode = mode;
		return m_pPixels += (m_size.w*region.y+region.x)*m_pixelSize;
	}


	//____ unlock() ________________________________________________________________

	void GlSurface::unlock()
	{
		AccessMode a = AccessMode::None;
		

		if(a == AccessMode::None )
			return;

		glUnmapBuffer( GL_PIXEL_UNPACK_BUFFER );

		if( m_accessMode != AccessMode::ReadOnly )
		{
			glBindTexture( GL_TEXTURE_2D, m_texture );
	//		glBindBuffer( GL_PIXEL_UNPACK_BUFFER, m_buffer );
			glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, m_size.w, m_size.h, m_accessFormat, GL_UNSIGNED_BYTE, 0 );
	//		glTexSubImage2D( GL_TEXTURE_2D, 0, m_lockRegion.x, m_lockRegion.y, m_lockRegion.w, m_lockRegion.h, GL_RGBA, GL_UNSIGNED_BYTE, 0 );
		}
		glBindBuffer( GL_PIXEL_UNPACK_BUFFER, 0 );
		m_accessMode = AccessMode::None;
		m_pPixels = 0;
		m_lockRegion.w = 0;
		m_lockRegion.h = 0;
	}


	//____ pixel() ______________________________________________________________

	uint32_t GlSurface::pixel( Coord coord ) const
	{
		if( m_accessMode != AccessMode::WriteOnly )
		{
			uint32_t val;

			if( m_accessMode == AccessMode::None )
			{
				// Quick lock of surface

				glBindBuffer( GL_PIXEL_UNPACK_BUFFER, m_buffer );
				uint8_t * pPixel = (uint8_t*) glMapBuffer( GL_PIXEL_UNPACK_BUFFER, GL_READ_ONLY );

				//

				 pPixel += (m_size.w*coord.y+coord.x)*m_pixelSize;
				switch( m_pixelSize )
				{
					case 1:
						val = (uint32_t) *pPixel;
					case 2:
						val = (uint32_t) ((uint16_t*) pPixel)[0];
					case 3:
						val = ((uint32_t) pPixel[0]) + (((uint32_t) pPixel[1]) << 8) + (((uint32_t) pPixel[2]) << 16);
					default:
						val = *((uint32_t*) pPixel);
				}

				// Quick unlock of surface

				glUnmapBuffer( GL_PIXEL_UNPACK_BUFFER );
				glBindBuffer( GL_PIXEL_UNPACK_BUFFER, 0 );
			}
			else
			{
				uint8_t * pPixel = m_pPixels + (m_size.w*coord.y+coord.x)*m_pixelSize;
				switch( m_pixelSize )
				{
					case 1:
						val = (uint32_t) *pPixel;
					case 2:
						val = (uint32_t) ((uint16_t*) pPixel)[0];
					case 3:
					   val = ((uint32_t) pPixel[0]) + (((uint32_t) pPixel[1]) << 8) + (((uint32_t) pPixel[2]) << 16);
					default:
						val = *((uint32_t*) pPixel);
				}
			}

			return val;
		}

		return 0;
	}



	//____ alpha() ____________________________________________________________

	uint8_t GlSurface::alpha( Coord coord ) const
	{
        if( m_accessFormat == GL_RGBA8 && m_accessMode != AccessMode::WriteOnly )
        {
            uint8_t a;
            
            if( m_accessMode == AccessMode::None )
            {
                // Quick lock of surface
                
                glBindBuffer( GL_PIXEL_UNPACK_BUFFER, m_buffer );
                uint8_t * pPixel = (uint8_t*) glMapBuffer( GL_PIXEL_UNPACK_BUFFER, GL_READ_ONLY );
                
                //
                
                a = pPixel[(m_size.w*coord.y+coord.x)*m_pixelSize+3];
                
                // Quick unlock of surface
                
                glUnmapBuffer( GL_PIXEL_UNPACK_BUFFER );
                glBindBuffer( GL_PIXEL_UNPACK_BUFFER, 0 );
                
            }
            else
            {
                a = m_pPixels[(m_size.w*coord.y+coord.x)*m_pixelSize+3];
            }
            
            return a;
        }
        else
            return 255;

	}
} // namespace wg
