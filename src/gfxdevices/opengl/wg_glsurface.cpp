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
#include <wg_glgfxdevice.h>
#include <wg_util.h>
#include <wg_blob.h>
#include <assert.h>



namespace wg
{
	const TypeInfo GlSurface::TYPEINFO = { "GlSurface", &Surface::TYPEINFO };

	SizeI	GlSurface::s_maxSize;

#define HANDLE_GLERROR(check) { GLenum err = check; if(err != 0) GlGfxDevice::onGlError(err, this, TYPEINFO, __func__, __FILE__, __LINE__ ); }



	//____ maxSize() _______________________________________________________________

	SizeI GlSurface::maxSize()
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

	GlSurface_p	GlSurface::create( SizeI size, PixelFormat format, int flags, const Color * pClut )
	{
		SizeI max = maxSize();
		if (size.w > max.w || size.h > max.h)
			return GlSurface_p();

		if (format == PixelFormat::Unknown || format == PixelFormat::Custom || format < PixelFormat_min || format > PixelFormat_max || (format == PixelFormat::I8 && pClut == nullptr))
			return GlSurface_p();

		return GlSurface_p(new GlSurface(size,format,flags,pClut));
	}

	GlSurface_p	GlSurface::create( SizeI size, PixelFormat format, Blob * pBlob, int pitch, int flags, const Color * pClut )
	{
		SizeI max = maxSize();
		if (size.w > max.w || size.h > max.h)
			return GlSurface_p();

		if (format == PixelFormat::Unknown || format == PixelFormat::Custom || format < PixelFormat_min || format > PixelFormat_max || (format == PixelFormat::I8 && pClut == nullptr) || !pBlob || pitch % 4 != 0)
			return GlSurface_p();

		return GlSurface_p(new GlSurface(size,format,pBlob,pitch,flags,pClut));
	}

	GlSurface_p	GlSurface::create( SizeI size, PixelFormat format, uint8_t * pPixels, int pitch, const PixelDescription * pPixelDescription, int flags, const Color * pClut )
	{
		SizeI max = maxSize();
		if (size.w > max.w || size.h > max.h)
			return GlSurface_p();

		if (format == PixelFormat::Unknown || format == PixelFormat::Custom || format < PixelFormat_min || format > PixelFormat_max ||
			(format == PixelFormat::I8 && pClut == nullptr) || pPixels == nullptr || pitch <= 0 )
			return GlSurface_p();

		return  GlSurface_p(new GlSurface(size,format,pPixels,pitch, pPixelDescription,flags,pClut));
	};

	GlSurface_p	GlSurface::create( Surface * pOther, int flags )
	{
		if (!pOther)
			return GlSurface_p();

		SizeI max = maxSize();
		SizeI size = pOther->size();
		if (size.w > max.w || size.h > max.h)
			return GlSurface_p();

		return GlSurface_p(new GlSurface( pOther,flags ));
	}



	//____ constructor _____________________________________________________________


	GlSurface::GlSurface( SizeI size, PixelFormat format, int flags, const Color * pClut ) : Surface(flags)
	{
		HANDLE_GLERROR(glGetError());
/*
		// @TODO: TMP workaround for annoying error (MAIN-16725).
		auto glError = glGetError();

		if(glError == 1281)
		{
			glGetError();
		}
		else
		{
			HANDLE_GLERROR(glError);
		}
*/
		_setPixelDetails(format);
		m_scaleMode = ScaleMode::Interpolate;
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

		_setupGlTexture(nullptr, flags);

		HANDLE_GLERROR(glGetError());
	}


	GlSurface::GlSurface( SizeI size, PixelFormat format, Blob * pBlob, int pitch, int flags, const Color * pClut ) : Surface(flags)
	{
		// Set general information

		_setPixelDetails(format);
		m_scaleMode = ScaleMode::Interpolate;
		m_size	= size;
		m_pitch = pitch;
		m_pBlob = pBlob;
		m_pClut = const_cast<Color*>(pClut);

		_setupGlTexture(m_pBlob->data(), flags);
	}

	GlSurface::GlSurface( SizeI size, PixelFormat format, uint8_t * pPixels, int pitch, const PixelDescription * pPixelDescription, int flags, const Color * pClut ) : Surface(flags)
	{
	   _setPixelDetails(format);
		m_scaleMode = ScaleMode::Interpolate;
		m_size	= size;
		m_pitch = ((size.w*m_pixelDescription.bits/8)+3)&0xFFFFFFFC;
		m_pBlob = Blob::create(m_pitch*m_size.h + (pClut ? 1024 : 0));

		m_pPixels = (uint8_t *) m_pBlob->data();
		_copyFrom( pPixelDescription==0 ? &m_pixelDescription:pPixelDescription, pPixels, pitch, size, size );
		m_pPixels = 0;

		if (pClut)
		{
			m_pClut = (Color*)((uint8_t*)m_pBlob->data() + m_pitch * size.h);
			memcpy(m_pClut, pClut, 1024);
		}
		else
			m_pClut = nullptr;

		_setupGlTexture(m_pBlob->data(), flags);
	}


	GlSurface::GlSurface( Surface * pOther, int flags ) : Surface(flags)
	{
		_setPixelDetails(pOther->pixelFormat());
		m_scaleMode = ScaleMode::Interpolate;
		m_size	= pOther->size();
		m_pitch = m_size.w * m_pixelSize;
		m_pBlob = Blob::create(m_pitch*m_size.h + (pOther->clut() ? 1024 : 0) );

		m_pPixels = (uint8_t *) m_pBlob->data();
		_copyFrom( pOther->pixelDescription(), (uint8_t*)pOther->pixels(), pOther->pitch(), m_size, m_size );
		m_pPixels = 0;

		if (pOther->clut())
		{
			m_pClut = (Color*)((uint8_t*)m_pBlob->data() + m_pitch * m_size.h);
			memcpy(m_pClut, pOther->clut(), 1024);
		}
		else
			m_pClut = nullptr;

		_setupGlTexture(m_pBlob->data(), flags);
	}

	void GlSurface::_setupGlTexture(void * pPixelsToUpload, int flags)
	{
		GLint oldBinding;
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &oldBinding);


		glGenTextures(1, &m_texture);
		glBindTexture(GL_TEXTURE_2D, m_texture);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

		HANDLE_GLERROR(glGetError());
		glTexImage2D(GL_TEXTURE_2D, 0, m_internalFormat, m_size.w, m_size.h, 0, m_accessFormat, m_pixelDataType, pPixelsToUpload);
		HANDLE_GLERROR(glGetError());


		if (m_pClut)
		{
			uint8_t clut[1024];

			for (int i = 0; i < 256; i++)
			{
				clut[i * 4] = m_pClut[i].r;
				clut[i * 4 + 1] = m_pClut[i].g;
				clut[i * 4 + 2] = m_pClut[i].b;
				clut[i * 4 + 3] = m_pClut[i].a;
			}

			// Create a TextureBufferObject for providing extra data to our shaders

			glGenBuffers(1, &m_clutBufferId);
			glBindBuffer(GL_TEXTURE_BUFFER, m_clutBufferId);
			glBufferData(GL_TEXTURE_BUFFER, 256*sizeof(GLuint), clut, GL_STATIC_DRAW);

			HANDLE_GLERROR(glGetError());

			glGenTextures(1, &m_clutTexture);
//			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_BUFFER, m_clutTexture);
			glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA8, m_clutBufferId);

			HANDLE_GLERROR(glGetError());
		}
		else if (flags & SurfaceFlag::Mipmapped)
		{
			m_bMipmapped = true;
			m_bMipmapStale = true;
		}


		setScaleMode(m_scaleMode);

		glBindTexture(GL_TEXTURE_2D, oldBinding);

		HANDLE_GLERROR(glGetError());
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
				m_internalFormat = GL_R8;
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

		if (m_pClut)
		{
			glDeleteTextures(1, &m_clutTexture);
			glDeleteBuffers(1, &m_clutBufferId);
		}

	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& GlSurface::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setScaleMode() __________________________________________________________

	void GlSurface::setScaleMode( ScaleMode mode )
	{
		HANDLE_GLERROR(glGetError());

		if (m_pClut == nullptr)
		{
			GLint oldBinding;
			glGetIntegerv(GL_TEXTURE_BINDING_2D, &oldBinding);
			glBindTexture(GL_TEXTURE_2D, m_texture);

			switch (mode)
			{
			case ScaleMode::Interpolate:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_bMipmapped ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				break;

			case ScaleMode::Nearest:
			default:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				break;
			}

			glBindTexture(GL_TEXTURE_2D, oldBinding);
		}

		Surface::setScaleMode(mode);
		HANDLE_GLERROR(glGetError());
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

		// Refresh backingBuffer if it is stale.
		// Flush active device if we have pending reads and might write.

		if (m_bBackingBufferStale)
			_refreshBackingBuffer();
		else if (m_bPendingReads && (m_accessMode == AccessMode::ReadWrite || m_accessMode == AccessMode::WriteOnly))
			GlGfxDevice::s_pActiveDevice->flush();

		//

		m_pPixels = (uint8_t*) m_pBlob->data();
		m_lockRegion = RectI(0,0,m_size);
		m_accessMode = mode;
		return m_pPixels;
	}

	//____ lockRegion() __________________________________________________________________

	uint8_t * GlSurface::lockRegion( AccessMode mode, const RectI& region )
	{
		if( m_accessMode != AccessMode::None || mode == AccessMode::None )
			return 0;

		// Refresh backingBuffer if it is stale.
		// Flush active device if we have pending reads and might write.

		if (m_bBackingBufferStale)
			_refreshBackingBuffer();
		else if (m_bPendingReads && (m_accessMode == AccessMode::ReadWrite || m_accessMode == AccessMode::WriteOnly))
			GlGfxDevice::s_pActiveDevice->flush();

		//

		if( region.x + region.w > m_size.w || region.y + region.h > m_size.h || region.x < 0 || region.y < 0 )
			return 0;

		m_pPixels = ((uint8_t*) m_pBlob->data()) + region.y*m_pitch + region.x*m_pixelSize;
		m_lockRegion = region;
		m_accessMode = mode;
		return m_pPixels;
	}


	//____ unlock() ________________________________________________________________

	void GlSurface::unlock()
	{
		HANDLE_GLERROR(glGetError());
		if(m_accessMode == AccessMode::None )
			return;

		if( m_accessMode != AccessMode::ReadOnly )
		{
			GLint oldBinding;
			glGetIntegerv(GL_TEXTURE_BINDING_2D, &oldBinding);

			glBindTexture( GL_TEXTURE_2D, m_texture );
			glPixelStorei( GL_UNPACK_ROW_LENGTH, m_size.w );
			glTexSubImage2D( GL_TEXTURE_2D, 0, m_lockRegion.x, m_lockRegion.y, m_lockRegion.w, m_lockRegion.h, m_accessFormat, m_pixelDataType, m_pPixels );
			glPixelStorei( GL_UNPACK_ROW_LENGTH, 0 );
			glBindTexture( GL_TEXTURE_2D, oldBinding );

			m_bMipmapStale = m_bMipmapped;
		}
		m_accessMode = AccessMode::None;
		m_pPixels = 0;
		m_lockRegion.w = 0;
		m_lockRegion.h = 0;
		HANDLE_GLERROR(glGetError());
	}

	//____ pixel() ______________________________________________________________

	uint32_t GlSurface::pixel( CoordI coord ) const
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

	uint8_t GlSurface::alpha( CoordI coord ) const
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

		HANDLE_GLERROR(glGetError());
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
		HANDLE_GLERROR(glGetError());

		GLint oldBinding;
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &oldBinding);

		glGenTextures( 1, &m_texture );
		glBindTexture( GL_TEXTURE_2D, m_texture );

		switch (m_scaleMode)
		{
		case ScaleMode::Interpolate:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			break;

		case ScaleMode::Nearest:
		default:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			break;
		}

		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

		glTexImage2D( GL_TEXTURE_2D, 0, m_internalFormat, m_size.w, m_size.h, 0,
					 m_accessFormat, m_pixelDataType, m_pBlob->data() );

		glBindTexture( GL_TEXTURE_2D, oldBinding );

		m_bMipmapStale = m_bMipmapped;

		HANDLE_GLERROR(glGetError());
	}

	//____ _refreshBackingBuffer() ____________________________________________

	void GlSurface::_refreshBackingBuffer()
	{
		// Flush any active device to make sure our texture is up-to-date

		if (GlGfxDevice::s_pActiveDevice)
			GlGfxDevice::s_pActiveDevice->flush();

		//

		HANDLE_GLERROR(glGetError());

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

		GLint oldBinding;
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &oldBinding);

		glBindTexture(GL_TEXTURE_2D, m_texture);
		glPixelStorei(GL_PACK_ROW_LENGTH, m_size.w);
		glGetTexImage(GL_TEXTURE_2D, 0, m_accessFormat, type, m_pBlob->data());
		glPixelStorei(GL_PACK_ROW_LENGTH, 0);
		glBindTexture(GL_TEXTURE_2D, oldBinding);

		HANDLE_GLERROR(glGetError());

		m_bBackingBufferStale = false;
	}




} // namespace wg
