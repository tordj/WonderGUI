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

	Size	GlSurface::s_maxSize;


	//____ maxSize() _______________________________________________________________

	Size GlSurface::maxSize()
	{
		if (s_maxSize.w == 0)
		{
			GLint max = 1024;
			glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max);
			s_maxSize.w = max;
			s_maxSize.h = max;
		}

		return s_maxSize;
	}

	//____ create ______________________________________________________________

    GlSurface_p	GlSurface::create( Size size, PixelFormat format, int hint, const Color * pClut )
    {
		Size max = maxSize();
		if (size.w > max.w || size.h > max.h)
			return GlSurface_p();

		if (format == PixelFormat::Unknown || format == PixelFormat::Custom || format < PixelFormat_min || format > PixelFormat_max || (format == PixelFormat::I8 && pClut == nullptr))
            return GlSurface_p();
        
        return GlSurface_p(new GlSurface(size,format,hint,pClut));
    }
    
    GlSurface_p	GlSurface::create( Size size, PixelFormat format, Blob * pBlob, int pitch, int hint, const Color * pClut )
    {
		Size max = maxSize();
		if (size.w > max.w || size.h > max.h)
			return GlSurface_p();

		if (format == PixelFormat::Unknown || format == PixelFormat::Custom || format < PixelFormat_min || format > PixelFormat_max || (format == PixelFormat::I8 && pClut == nullptr) || !pBlob || pitch % 4 != 0)
			return GlSurface_p();
        
        return GlSurface_p(new GlSurface(size,format,pBlob,pitch,hint,pClut));
    }
    
    GlSurface_p	GlSurface::create( Size size, PixelFormat format, uint8_t * pPixels, int pitch, const PixelDescription * pPixelDescription, int hint, const Color * pClut )
    {
		Size max = maxSize();
		if (size.w > max.w || size.h > max.h)
			return GlSurface_p();

		if (format == PixelFormat::Unknown || format == PixelFormat::Custom || format < PixelFormat_min || format > PixelFormat_max ||
			(format == PixelFormat::I8 && pClut == nullptr) || pPixels == nullptr || pitch <= 0 || pPixelDescription == nullptr)
			return GlSurface_p();
        
        return  GlSurface_p(new GlSurface(size,format,pPixels,pitch, pPixelDescription,hint,pClut));
    };
    
    GlSurface_p	GlSurface::create( Surface * pOther, int hint )
    {
		if (!pOther)
			return GlSurface_p();

		Size max = maxSize();
		Size size = pOther->size();
		if (size.w > max.w || size.h > max.h)
			return GlSurface_p();
		
		return GlSurface_p(new GlSurface( pOther,hint ));
    }

    
    
	//____ Constructor _____________________________________________________________


    GlSurface::GlSurface( Size size, PixelFormat format, int hint, const Color * pClut )
    {
		assert(glGetError() == 0);
		_setPixelDetails(format);
        m_size	= size;
        m_pitch = ((size.w*m_pixelDescription.bits/8)+3)&0xFFFFFFFC;
    	m_pBlob = Blob::create(m_pitch*m_size.h + (pClut ? 4096 : 0));
	
		if (pClut)
		{
			m_pClut = (Color*)((uint8_t*)m_pBlob->data() + m_pitch * size.h);
			memcpy(m_pClut, pClut, 4096);
		}
		else
			m_pClut = nullptr;

        glGenTextures( 1, &m_texture );
        glBindTexture( GL_TEXTURE_2D, m_texture );
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

        glTexImage2D( GL_TEXTURE_2D, 0, m_internalFormat, m_size.w, m_size.h, 0,
                     m_accessFormat, m_pixelDataType, NULL );

        assert( glGetError() == 0 );
    }
    
    
	GlSurface::GlSurface( Size size, PixelFormat format, Blob * pBlob, int pitch, int hint, const Color * pClut )
	{
        // Set general information
        
        _setPixelDetails(format);
        m_size	= size;
        m_pitch = pitch;
		m_pBlob = pBlob;
		m_pClut = const_cast<Color*>(pClut);

		glGenTextures( 1, &m_texture );
        glBindTexture( GL_TEXTURE_2D, m_texture );
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

		glTexImage2D( GL_TEXTURE_2D, 0, m_internalFormat, m_size.w, m_size.h, 0,
			m_accessFormat, m_pixelDataType, m_pBlob->data() );

		assert( glGetError() == 0);
	}
   
    GlSurface::GlSurface( Size size, PixelFormat format, uint8_t * pPixels, int pitch, const PixelDescription * pPixelDescription, int hint, const Color * pClut )
    {
       _setPixelDetails(format);
        m_size	= size;
        m_pitch = ((size.w*m_pixelDescription.bits/8)+3)&0xFFFFFFFC;
        m_pBlob = Blob::create(m_pitch*m_size.h + (pClut ? 4096 : 0));
        
        m_pPixels = (uint8_t *) m_pBlob->data();
        _copyFrom( pPixelDescription==0 ? &m_pixelDescription:pPixelDescription, pPixels, pitch, size, size );
        m_pPixels = 0;
       
		if (pClut)
		{
			m_pClut = (Color*)((uint8_t*)m_pBlob->data() + m_pitch * size.h);
			memcpy(m_pClut, pClut, 4096);
		}
		else
			m_pClut = nullptr;

        glGenTextures( 1, &m_texture );
        glBindTexture( GL_TEXTURE_2D, m_texture );
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
		
        glTexImage2D( GL_TEXTURE_2D, 0, m_internalFormat, m_size.w, m_size.h, 0,
                     m_accessFormat, m_pixelDataType, m_pBlob->data() );
        
 		assert( glGetError() == 0);
    }


    GlSurface::GlSurface( Surface * pOther, int hint )
    {
        _setPixelDetails(pOther->pixelFormat());
        m_size	= pOther->size();
        m_pitch = m_size.w * m_pixelSize;
        m_pBlob = Blob::create(m_pitch*m_size.h + (pOther->clut() ? 4096 : 0) );
        
        m_pPixels = (uint8_t *) m_pBlob->data();
        _copyFrom( pOther->pixelDescription(), (uint8_t*)pOther->pixels(), pOther->pitch(), m_size, m_size );
        m_pPixels = 0;
        
		if (pOther->clut())
		{
			m_pClut = (Color*)((uint8_t*)m_pBlob->data() + m_pitch * m_size.h);
			memcpy(m_pClut, pOther->clut(), 4096);
		}
		else
			m_pClut = nullptr;

        glGenTextures( 1, &m_texture );
        glBindTexture( GL_TEXTURE_2D, m_texture );
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
		
        glTexImage2D( GL_TEXTURE_2D, 0, m_internalFormat, m_size.w, m_size.h, 0,
                     m_accessFormat, m_pixelDataType, m_pBlob->data() );
        
		assert( glGetError() == 0);
    }
    
    

	void GlSurface::_setPixelDetails( PixelFormat format )
	{
		switch (format)
		{
			case PixelFormat::BGR_8:
				m_internalFormat = GL_RGB8;
				m_accessFormat = GL_BGR;
				m_pixelDataType = GL_UNSIGNED_BYTE;
				m_pixelSize = 3;
				break;

			case PixelFormat::BGRA_8:
				m_internalFormat = GL_RGBA8;
				m_accessFormat = GL_BGRA;
				m_pixelDataType = GL_UNSIGNED_BYTE;
				m_pixelSize = 4;
				break;

			case PixelFormat::BGRX_8:
				m_internalFormat = GL_RGB8;
				m_accessFormat = GL_BGRA;
				m_pixelDataType = GL_UNSIGNED_BYTE;
				m_pixelSize = 4;
				break;

			case PixelFormat::BGR_565:
				m_internalFormat = GL_RGB565;					// NOTE: We lose one bit of precision on green here...
				m_accessFormat = GL_RGB;
				m_pixelDataType = GL_UNSIGNED_SHORT_5_6_5_REV;		// or should we use GL_UNSIGNED_SHORT_5_6_5_REV?
				m_pixelSize = 2;
				break;

			case PixelFormat::BGRA_4:
				m_internalFormat = GL_RGBA4;
				m_accessFormat = GL_BGRA;
				m_pixelDataType = GL_UNSIGNED_SHORT_4_4_4_4;	// or should we use GL_UNSIGNED_SHORT_4_4_4_4_REV?
				m_pixelSize = 2;
				break;

			case PixelFormat::A8:
				m_internalFormat = GL_R8;
				m_accessFormat = GL_RED;
				m_pixelDataType = GL_UNSIGNED_BYTE;
				m_pixelSize = 1;
				break;

			case PixelFormat::I8:
				m_internalFormat = GL_R8UI;
				m_accessFormat = GL_RED;
				m_pixelDataType = GL_UNSIGNED_BYTE;
				m_pixelSize = 1;
				break;
            default:
                assert(false);           // Should never get here, just avoiding compiler warnings.
                break;

		}


        
        Util::pixelFormatToDescription(format, m_pixelDescription);
	}

	//____ Destructor ______________________________________________________________

	GlSurface::~GlSurface()
	{
		// Free the stuff

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
        assert( glGetError() == 0 );
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
        assert( glGetError() == 0 );
	}

	//____ size() ______________________________________________________________

	Size GlSurface::size() const
	{
		return m_size;
	}

	//____ isOpaque() ______________________________________________________________

	bool GlSurface::isOpaque() const
	{
		if( m_internalFormat == GL_RGB )
			return true;

		return false;
	}

	//____ lock() __________________________________________________________________

	uint8_t * GlSurface::lock( AccessMode mode )
	{
		if( m_accessMode != AccessMode::None || mode == AccessMode::None )
			return 0;

		if (m_bBackingBufferStale)
			_refreshBackingBuffer();

    	m_pPixels = (uint8_t*) m_pBlob->data();
		m_lockRegion = Rect(0,0,m_size);
		m_accessMode = mode;
		return m_pPixels;
	}

	//____ lockRegion() __________________________________________________________________

	uint8_t * GlSurface::lockRegion( AccessMode mode, const Rect& region )
	{
		if( m_accessMode != AccessMode::None || mode == AccessMode::None )
			return 0;

		if (m_bBackingBufferStale)
			_refreshBackingBuffer();

		if( region.x + region.w > m_size.w || region.y + region.w > m_size.h || region.x < 0 || region.y < 0 )
			return 0;

    	m_pPixels = (uint8_t*) m_pBlob->data();
		m_lockRegion = region;
		m_accessMode = mode;
		return m_pPixels += (m_size.w*region.y+region.x)*m_pixelSize;
	}


	//____ unlock() ________________________________________________________________

	void GlSurface::unlock()
	{
        assert( glGetError() == 0 );
		if(m_accessMode == AccessMode::None )
			return;

		if( m_accessMode != AccessMode::ReadOnly )
		{
			glBindTexture( GL_TEXTURE_2D, m_texture );
        	glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, m_size.w, m_size.h, m_accessFormat, m_pixelDataType, m_pBlob->data() );
	//		glTexSubImage2D( GL_TEXTURE_2D, 0, m_lockRegion.x, m_lockRegion.y, m_lockRegion.w, m_lockRegion.h, GL_RGBA, GL_UNSIGNED_BYTE, 0 );
		}
		m_accessMode = AccessMode::None;
		m_pPixels = 0;
		m_lockRegion.w = 0;
		m_lockRegion.h = 0;
        assert( glGetError() == 0 );
	}

	//____ pixel() ______________________________________________________________

	uint32_t GlSurface::pixel( Coord coord ) const
	{
//		if (m_bBackingBufferStale)
//			_refreshBackingBuffer();

		if( m_accessMode != AccessMode::WriteOnly )
		{
			uint32_t val;

				uint8_t * pPixel = (uint8_t*) m_pBlob->data();

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

			return val;
		}

		return 0;
	}



	//____ alpha() ____________________________________________________________

	uint8_t GlSurface::alpha( Coord coord ) const
	{
//		if (m_bBackingBufferStale)
//			_refreshBackingBuffer();

		if( m_pixelDescription.format == PixelFormat::BGRA_8 )
        {
			uint8_t * p = (uint8_t*) m_pBlob->data();
			return p[coord.y*m_pitch+coord.x*4+3];            
		}
		else
			return 255;            
	}
                
	//____ unload() ___________________________________________________________

	bool GlSurface::unload()
	{
		if( m_texture == 0 )
			return true;
                
		glDeleteTextures( 1, &m_texture );
		m_texture = 0;
                
		assert(glGetError() == 0);	
		return true;
	}

	//____ isLoaded() _________________________________________________________

	bool GlSurface::isLoaded()
	{
		return (m_texture == 0);
	}

	//____ reloaded() _________________________________________________________

	void GlSurface::reload()
	{
		assert(glGetError() == 0);

		glGenTextures( 1, &m_texture );
		glBindTexture( GL_TEXTURE_2D, m_texture );
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	
		glTexImage2D( GL_TEXTURE_2D, 0, m_internalFormat, m_size.w, m_size.h, 0,
					 m_accessFormat, m_pixelDataType, m_pBlob->data() );
    
		assert( glGetError() == 0);	
	}

	//____ _refreshBackingBuffer() ____________________________________________

	void GlSurface::_refreshBackingBuffer()
	{
		assert(glGetError() == 0);

		GLenum	type;

		switch (m_pixelDescription.format)
		{
		case PixelFormat::BGR_8:
			type = GL_UNSIGNED_BYTE;
			break;
		case PixelFormat::BGRA_8:
		case PixelFormat::BGRX_8:
			type = GL_UNSIGNED_INT_8_8_8_8_REV;
			break;
		case PixelFormat::BGRA_4:
			type = GL_UNSIGNED_SHORT_4_4_4_4_REV;
			break;
		case PixelFormat::BGR_565:
			type = GL_UNSIGNED_SHORT_5_6_5_REV;
			break;
		case PixelFormat::A8:
			type = GL_UNSIGNED_BYTE;
			break;
		case PixelFormat::I8:
			type = GL_UNSIGNED_BYTE;
			break;
        default:
            assert(false);   // Should never get here! This code is just to avoid compiler warnings.
            break;
		}


		glBindTexture(GL_TEXTURE_2D, m_texture);
		glGetTexImage(GL_TEXTURE_2D, 0, m_accessFormat, type, m_pBlob->data());

		GLenum err;
		assert(0 == (err = glGetError()));

		m_bBackingBufferStale = false;
	}




} // namespace wg
