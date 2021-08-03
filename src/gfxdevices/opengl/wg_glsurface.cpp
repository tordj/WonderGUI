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
#include <wg_base.h>
#include <wg_context.h>
#include <assert.h>



namespace wg
{
	const TypeInfo GlSurface::TYPEINFO = { "GlSurface", &Surface::TYPEINFO };

	SizeI	GlSurface::s_maxSize;

#define HANDLE_GLERROR(check) { GLenum err = check; if(err != 0) GlGfxDevice::onGlError(err, this, TYPEINFO, __func__, __FILE__, __LINE__ ); }

    unsigned int    GlSurface::g_texturePixels = 0;
    unsigned int    GlSurface::g_backingPixels = 0;

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

	GlSurface_p	GlSurface::create( SizeI size, PixelFormat format, int flags, const Color8 * pClut )
	{
		SizeI max = maxSize();
		if (size.w > max.w || size.h > max.h)
			return GlSurface_p();

		if (format == PixelFormat::Unknown || format == PixelFormat::Custom || format < PixelFormat_min || format > PixelFormat_max || ((format == PixelFormat::CLUT_8 || format == PixelFormat::CLUT_8_sRGB || format == PixelFormat::CLUT_8_linear) && pClut == nullptr))
			return GlSurface_p();
        
		return GlSurface_p(new GlSurface(size,format,flags,pClut));
	}

	GlSurface_p	GlSurface::create( SizeI size, PixelFormat format, Blob * pBlob, int pitch, int flags, const Color8 * pClut )
	{
		SizeI max = maxSize();
		if (size.w > max.w || size.h > max.h)
			return GlSurface_p();

		if (format == PixelFormat::Unknown || format == PixelFormat::Custom || format < PixelFormat_min || format > PixelFormat_max || ((format == PixelFormat::CLUT_8 || format == PixelFormat::CLUT_8_sRGB || format == PixelFormat::CLUT_8_linear) && pClut == nullptr) || !pBlob || pitch % 4 != 0)
			return GlSurface_p();

		return GlSurface_p(new GlSurface(size,format,pBlob,pitch,flags,pClut));
	}

	GlSurface_p	GlSurface::create( SizeI size, PixelFormat format, uint8_t * pPixels, int pitch, const PixelDescription * pPixelDescription, int flags, const Color8 * pClut )
	{
		SizeI max = maxSize();
		if (size.w > max.w || size.h > max.h)
			return GlSurface_p();

		if (format == PixelFormat::Unknown || format == PixelFormat::Custom || format < PixelFormat_min || format > PixelFormat_max ||
			((format == PixelFormat::CLUT_8 || format == PixelFormat::CLUT_8_sRGB || format == PixelFormat::CLUT_8_linear) && pClut == nullptr) || pPixels == nullptr || pitch <= 0 )
			return GlSurface_p();

		return  GlSurface_p(new GlSurface(size,format,pPixels,pitch, pPixelDescription,flags,pClut));
	};

	GlSurface_p	GlSurface::create( Surface * pOther, int flags )
	{
		if (!pOther)
			return GlSurface_p();

		SizeI max = maxSize();
		SizeI size = pOther->pixelSize();
		if (size.w > max.w || size.h > max.h)
			return GlSurface_p();

		return GlSurface_p(new GlSurface( pOther,flags ));
	}



	//____ constructor _____________________________________________________________


	GlSurface::GlSurface( SizeI size, PixelFormat format, int flags, const Color8 * pClut ) : Surface(flags)
	{
//        flags |= (int) SurfaceFlag::Buffered;
		HANDLE_GLERROR(glGetError());

		_setPixelDetails(format);
		m_scaleMode = ScaleMode::Interpolate;
		m_size	= size;
		m_pClut = nullptr;

		if ((flags & SurfaceFlag::Buffered) || m_pixelDescription.bits <= 8)
        {
            g_backingPixels += m_size.w*m_size.h;
            m_pitch = ((size.w * m_pixelDescription.bits / 8) + 3) & 0xFFFFFFFC;
            m_pBlob = Blob::create(m_pitch * m_size.h + (pClut ? 1024 : 0));

            if (pClut)
            {
                m_pClut = (Color8*)((uint8_t*)m_pBlob->data() + m_pitch * size.h);
                memcpy(m_pClut, pClut, 1024);
            }
        }
        else
		{
			m_pitch = 0;

			if (pClut)
			{
				m_pClut = new Color8[256];
				memcpy(m_pClut, pClut, 1024);
			}

			if (m_pixelDescription.A_bits > 0)
				m_pAlphaMap = new uint8_t[size.w * size.h];
		}

		_setupGlTexture(nullptr, 0, flags);

		HANDLE_GLERROR(glGetError());
	}


	GlSurface::GlSurface( SizeI size, PixelFormat format, Blob * pBlob, int pitch, int flags, const Color8 * pClut ) : Surface(flags)
	{
//        flags |= (int) SurfaceFlag::Buffered;
		// Set general information

		_setPixelDetails(format);
		m_scaleMode = ScaleMode::Interpolate;
		m_size = size;
		m_pClut = const_cast<Color8*>(pClut);


        if ((flags & SurfaceFlag::Buffered) || m_pixelDescription.bits <= 8)
        {
            g_backingPixels += m_size.w*m_size.h;
            m_pitch = pitch;
            m_pBlob = pBlob;
        }
        else
		{
			m_pitch = 0;

			if (pClut)
			{
				m_pClut = new Color8[256];
				memcpy(m_pClut, pClut, 1024);
			}

			if (m_pixelDescription.A_bits > 0)
			{
				m_pAlphaMap = new uint8_t[size.w * size.h];

				// Setup a fake PixelBuffer for call to _updateAlphaMap
				PixelBuffer buf;
				buf.format = format;
				buf.pClut = m_pClut;
				buf.pitch = pitch;
				buf.pPixels = (uint8_t*) pBlob->data();
				buf.rect = size;

				_updateAlphaMap( buf, size );
			}
		}

		//TODO: Support pitch
		
		_setupGlTexture(pBlob->data(), pitch, flags);
	}

	GlSurface::GlSurface( SizeI size, PixelFormat format, uint8_t * pPixels, int pitch, const PixelDescription * pPixelDescription, int flags, const Color8 * pClut ) : Surface(flags)
	{
//       flags |= (int) SurfaceFlag::Buffered;

        _setPixelDetails(format);
		m_scaleMode = ScaleMode::Interpolate;
		m_size	= size;
		m_pClut = nullptr;

        if ((flags & SurfaceFlag::Buffered) || m_pixelDescription.bits <= 8)
        {
            g_backingPixels += m_size.w*m_size.h;

            m_pitch = ((size.w * m_pixelDescription.bits / 8) + 3) & 0xFFFFFFFC;
            m_pBlob = Blob::create(m_pitch * m_size.h + (pClut ? 1024 : 0));

            _copyFrom(pPixelDescription == 0 ? &m_pixelDescription : pPixelDescription, pPixels, pitch, size, size);

			// Setup CLUT before calling _setupGlTexture().

			if (pClut)
			{
				m_pClut = (Color8*)((uint8_t*)m_pBlob->data() + m_pitch * size.h);
				memcpy(m_pClut, pClut, 1024);
			}

            _setupGlTexture(m_pBlob->data(), m_pitch, flags);
        }
        else
		{
			// Setup CLUT before calling _setupGlTexture().

			if (pClut)
			{
				m_pClut = new Color8[256];
				memcpy(m_pClut, pClut, 1024);
			}

			if( pPixelDescription->format == PixelFormat::Custom )
            {
                m_pitch = ((size.w * m_pixelDescription.bits / 8) + 3) & 0xFFFFFFFC;
                m_pBlob = Blob::create(m_pitch * m_size.h + (pClut ? 1024 : 0));

                _copyFrom(pPixelDescription == 0 ? &m_pixelDescription : pPixelDescription, pPixels, pitch, size, size);

                _setupGlTexture(m_pBlob->data(), m_pitch, flags);
            }
            else
                _setupGlTexture(pPixels, pitch, flags);

            m_pBlob = nullptr;
            m_pitch = 0;

			if (m_pixelDescription.A_bits > 0)
			{
				m_pAlphaMap = new uint8_t[size.w * size.h];

				// Setup a fake PixelBuffer for call to _updateAlphaMap
				PixelBuffer buf;
				buf.format = format;
				buf.pClut = m_pClut;
				buf.pitch = pitch;
				buf.pPixels = pPixels;
				buf.rect = size;

				_updateAlphaMap(buf, size);
			}
		}
	}


	GlSurface::GlSurface( Surface * pOther, int flags ) : Surface(flags)
	{
//        flags |= (int) SurfaceFlag::Buffered;
		_setPixelDetails(pOther->pixelFormat());
		m_scaleMode = ScaleMode::Interpolate;
		m_size	= pOther->pixelSize();
		m_pClut = nullptr;

		auto pixbuf = pOther->allocPixelBuffer();
		if( !pOther->pushPixels(pixbuf) )
		{
			// Error handling
		}

        if ((flags & SurfaceFlag::Buffered) || m_pixelDescription.bits <= 8)
        {
            g_backingPixels += m_size.w*m_size.h;

            m_pitch = m_size.w * m_pixelSize;
            m_pBlob = Blob::create(m_pitch * m_size.h + (pOther->clut() ? 1024 : 0));
            _copyFrom(pOther->pixelDescription(), pixbuf.pPixels, pixbuf.pitch, m_size, m_size);

            if (pOther->clut())
            {
                m_pClut = (Color8*)((uint8_t*)m_pBlob->data() + m_pitch * m_size.h);
                memcpy(m_pClut, pOther->clut(), 1024);
            }
        }
        else
		{
			m_pitch = 0;

			if (pOther->clut())
			{
				m_pClut = new Color8[256];
				memcpy(m_pClut, pOther->clut(), 1024);
			}

			if (m_pixelDescription.A_bits > 0)
			{
				m_pAlphaMap = new uint8_t[m_size.w * m_size.h];
				_updateAlphaMap(pixbuf, m_size);
			}
		}

		//TODO: Support pitch

        _setupGlTexture(pixbuf.pPixels, pixbuf.pitch, flags);

		pOther->freePixelBuffer(pixbuf);
	}

	void GlSurface::_setupGlTexture(void * pPixelsToUpload, int pitch, int flags)
	{
		GLint oldBinding;
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &oldBinding);

        g_texturePixels += m_size.w*m_size.h;


		glGenTextures(1, &m_texture);
		glBindTexture(GL_TEXTURE_2D, m_texture);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

		HANDLE_GLERROR(glGetError());
        glPixelStorei(GL_UNPACK_ROW_LENGTH, pitch/m_pixelSize);
        glTexImage2D(GL_TEXTURE_2D, 0, m_internalFormat, m_size.w, m_size.h, 0, m_accessFormat, m_pixelDataType, pPixelsToUpload);
        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		HANDLE_GLERROR(glGetError());


		if (m_pClut)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);


			HANDLE_GLERROR(glGetError());

			glGenTextures(1, &m_clutTexture);
			glBindTexture(GL_TEXTURE_2D, m_clutTexture);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

			glTexImage2D(GL_TEXTURE_2D, 0, m_pixelDescription.bLinear ? GL_RGBA8 : GL_SRGB8_ALPHA8, 256, 1, 0, GL_BGRA, GL_UNSIGNED_BYTE, m_pClut);

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
				m_internalFormat = Base::activeContext()->gammaCorrection() ? GL_SRGB8 : GL_RGB8;
				m_accessFormat = GL_BGR;
				m_pixelDataType = GL_UNSIGNED_BYTE;
				m_pixelSize = 3;
				break;

			case PixelFormat::BGR_8_sRGB:
				m_internalFormat = GL_SRGB8;
				m_accessFormat = GL_BGR;
				m_pixelDataType = GL_UNSIGNED_BYTE;
				m_pixelSize = 3;
				break;

			case PixelFormat::BGR_8_linear:
				m_internalFormat = GL_RGB8;
				m_accessFormat = GL_BGR;
				m_pixelDataType = GL_UNSIGNED_BYTE;
				m_pixelSize = 3;
				break;

			case PixelFormat::BGRX_8:
				m_internalFormat = Base::activeContext()->gammaCorrection() ? GL_SRGB8 : GL_RGB8;
				m_accessFormat = GL_BGRA;
				m_pixelDataType = GL_UNSIGNED_BYTE;
				m_pixelSize = 4;
				break;

			case PixelFormat::BGRX_8_sRGB:
				m_internalFormat = GL_SRGB8;
				m_accessFormat = GL_BGRA;
				m_pixelDataType = GL_UNSIGNED_BYTE;
				m_pixelSize = 4;
				break;

			case PixelFormat::BGRX_8_linear:
				m_internalFormat = GL_RGB8;
				m_accessFormat = GL_BGRA;
				m_pixelDataType = GL_UNSIGNED_BYTE;
				m_pixelSize = 4;
				break;

			case PixelFormat::BGRA_8:
				m_internalFormat = Base::activeContext()->gammaCorrection() ? GL_SRGB8_ALPHA8 : GL_RGBA8;
				m_accessFormat = GL_BGRA;
				m_pixelDataType = GL_UNSIGNED_BYTE;
				m_pixelSize = 4;
				break;

			case PixelFormat::BGRA_8_sRGB:
				m_internalFormat = GL_SRGB8_ALPHA8;
				m_accessFormat = GL_BGRA;
				m_pixelDataType = GL_UNSIGNED_BYTE;
				m_pixelSize = 4;
				break;

			case PixelFormat::BGRA_8_linear:
				m_internalFormat = GL_RGBA8;
				m_accessFormat = GL_BGRA;
				m_pixelDataType = GL_UNSIGNED_BYTE;
				m_pixelSize = 4;
				break;


			case PixelFormat::BGR_565_linear:
				m_internalFormat = GL_RGB565;					// NOTE: We lose one bit of precision on green here...
				m_accessFormat = GL_RGB;
				m_pixelDataType = GL_UNSIGNED_SHORT_5_6_5_REV;		// or should we use GL_UNSIGNED_SHORT_5_6_5_REV?
				m_pixelSize = 2;
				break;

			case PixelFormat::BGRA_4_linear:
				m_internalFormat = GL_RGBA4;
				m_accessFormat = GL_BGRA;
				m_pixelDataType = GL_UNSIGNED_SHORT_4_4_4_4;	// or should we use GL_UNSIGNED_SHORT_4_4_4_4_REV?
				m_pixelSize = 2;
				break;

			case PixelFormat::CLUT_8:
			case PixelFormat::CLUT_8_sRGB:
			case PixelFormat::CLUT_8_linear:
				m_internalFormat = GL_R8;
				m_accessFormat = GL_RED;
				m_pixelDataType = GL_UNSIGNED_BYTE;
				m_pixelSize = 1;
				break;

			case PixelFormat::A_8:
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
        
        g_texturePixels -= m_size.w*m_size.h;

        if( m_pBlob )
            g_backingPixels -= m_size.w*m_size.h;

        
		// Free the stuff

		glDeleteTextures( 1, &m_texture );

		if (m_pClut)
		{
			glDeleteTextures(1, &m_clutTexture);
			if (!m_pBlob)
				delete[] m_pClut;		// Clut is not part of the blob.
		}

		if (m_pAlphaMap)
		{
			delete[] m_pAlphaMap;
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

	//____ setTiling() ________________________________________________________

	bool GlSurface::setTiling(bool bTiling)
	{
		GLint mode = bTiling ? GL_REPEAT : GL_CLAMP_TO_EDGE;

		glBindTexture(GL_TEXTURE_2D, m_texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mode);

		m_bTiling = bTiling;
		return true;
	}

	//____ allocPixelBuffer() _________________________________________________

	const PixelBuffer GlSurface::allocPixelBuffer(const RectI& rect)
	{
		PixelBuffer pixbuf;

		if (m_pBlob)
		{
			pixbuf.format = m_pixelDescription.format;
			pixbuf.pClut = m_pClut;
			pixbuf.pitch = m_pitch;
			pixbuf.pPixels = ((uint8_t*)m_pBlob->data()) + rect.y * m_pitch + rect.x * m_pixelSize;
			pixbuf.rect = rect;
		}
		else
		{
			pixbuf.format = m_pixelDescription.format;
			pixbuf.pClut = m_pClut;
			pixbuf.pitch = ((rect.w * m_pixelDescription.bits / 8) + 3) & 0xFFFFFFFC;
            pixbuf.pPixels = new uint8_t[rect.h * pixbuf.pitch];
			pixbuf.rect = rect;
		}
		return pixbuf;
	}

	//____ pushPixels() _______________________________________________________

	bool GlSurface::pushPixels(const PixelBuffer& buffer, const RectI& bufferRect)
	{
		if (m_pBlob)
		{
			if (m_bBackingBufferStale)
				_readBackTexture(m_pBlob->data());

			return true;
		}
        else if( bufferRect.size() == m_size )
		{
            // This is OpenGL pre 4.5, we can only read back a whole texture :(
            
            _readBackTexture(buffer.pPixels);
			return false;
		}
        else
            return false;
	}

	//____ pullPixels() _______________________________________________________

	void GlSurface::pullPixels(const PixelBuffer& buffer, const RectI& bufferRect)
	{
		// Handle special case where we use _copyFrom() in constructor
		// before texture has been created.

		if (m_texture == 0)
			return;

		//

		if (m_bPendingReads)
			GlGfxDevice::s_pActiveDevice->flush();

		GLint oldBinding;
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &oldBinding);
		glBindTexture(GL_TEXTURE_2D, m_texture);

		RectI texRect = { buffer.rect.pos() + bufferRect.pos(), bufferRect.size() };
		uint8_t* pSrc = buffer.pPixels + bufferRect.y * buffer.pitch + bufferRect.x * m_pixelSize;

		glPixelStorei(GL_UNPACK_ROW_LENGTH, buffer.pitch/m_pixelSize);
		glTexSubImage2D(GL_TEXTURE_2D, 0, texRect.x, texRect.y, texRect.w, texRect.h, m_accessFormat, m_pixelDataType, pSrc);

		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		glBindTexture(GL_TEXTURE_2D, oldBinding);
		m_bMipmapStale = m_bMipmapped;

		HANDLE_GLERROR(glGetError());

		if (m_pAlphaMap)
			_updateAlphaMap(buffer, bufferRect);
	}

	//____ freePixelBuffer() ____________________________________________________

	void GlSurface::freePixelBuffer(const PixelBuffer& buffer)
	{
		if (!m_pBlob)
			delete [] buffer.pPixels;
	}

	//____ alpha() ____________________________________________________________

	int GlSurface::alpha( CoordSPX _coord )
	{
//		if (m_bBackingBufferStale)
//			_refreshBackingBuffer();

		//TODO: Take endianess into account.
		//TODO: Take advantage of subpixel precision and interpolate alpha value if surface set to interpolate.

		CoordI coord(((_coord.x + 32) / 64) % m_size.w, ((_coord.y + 32) / 64) % m_size.h);

		if (m_pBlob)
		{
			uint8_t* pPixel = (uint8_t*)m_pBlob->data();
			pPixel += coord.y * m_pitch + coord.x * (m_pixelSize);

			if (m_pixelDescription.bIndexed)
			{
				return HiColor::unpackLinearTab[m_pClut[*pPixel].a];
			}
			else if (m_pixelDescription.A_bits == 0)
				return 4096;
			else
			{
				uint32_t val;

				switch (m_pixelSize)
				{
				case 1:
					val = (uint32_t)*pPixel;
				case 2:
					val = (uint32_t)((uint16_t*)pPixel)[0];
				case 3:
					val = ((uint32_t)pPixel[0]) + (((uint32_t)pPixel[1]) << 8) + (((uint32_t)pPixel[2]) << 16);
				default:
					val = *((uint32_t*)pPixel);
				}

				const uint8_t* pConvTab = s_pixelConvTabs[m_pixelDescription.A_bits];

				return HiColor::unpackLinearTab[pConvTab[(val & m_pixelDescription.A_mask) >> m_pixelDescription.A_shift]];
			}
		}
		else if (m_pAlphaMap)
			return HiColor::unpackLinearTab[m_pAlphaMap[coord.y * m_size.w + coord.x]];
		else
			return 4096;
	}

	//____ unload() ___________________________________________________________

	bool GlSurface::unload()
	{
		if (!m_pBlob)
			return false;

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
		return (m_texture != 0);
	}

	//____ reloaded() _________________________________________________________

	void GlSurface::reload()
	{
		if (m_texture != 0)
			return;

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

	//____ _updateAlphaMap() __________________________________________________

	void GlSurface::_updateAlphaMap(const PixelBuffer& buffer, const RectI& bufferRect)
	{
		uint8_t* pDst = m_pAlphaMap + (buffer.rect.y + bufferRect.y) * m_size.w + (buffer.rect.x + bufferRect.x);
		uint8_t* pSrc = buffer.pPixels + bufferRect.y * buffer.pitch + bufferRect.x * m_pixelSize;

		int srcPitchAdd = buffer.pitch - bufferRect.w * m_pixelSize;
		int dstPitchAdd = m_size.w - bufferRect.w;

		PixelFormat fmt = m_pixelDescription.format;
		
		if (fmt == PixelFormat::BGRA_8_linear || fmt == PixelFormat::BGRA_8_sRGB)
		{
			for (int y = 0; y < bufferRect.h; y++)
			{
				for (int x = 0; x < bufferRect.w; x++)
				{
					*pDst++ = pSrc[3];
					pSrc += 4;
				}
				pSrc += srcPitchAdd;
				pDst += dstPitchAdd;
			}
		}
		else
		{
			const uint8_t* pConvTab = s_pixelConvTabs[m_pixelDescription.A_bits];
			switch (m_pixelSize)
			{
				case 2:
				{
					for (int y = 0; y < bufferRect.h; y++)
					{
						for (int x = 0; x < bufferRect.w; x++)
						{
							uint32_t val = (uint32_t)((uint16_t*)pSrc)[0];
							*pDst++ = pConvTab[(val & m_pixelDescription.A_mask) >> m_pixelDescription.A_shift];
							pSrc += m_pixelSize;
						}
						pSrc += srcPitchAdd;
						pDst += dstPitchAdd;
					}
					break;
				}
				case 3:
				{
					for (int y = 0; y < bufferRect.h; y++)
					{
						for (int x = 0; x < bufferRect.w; x++)
						{
							uint32_t val = ((uint32_t)pSrc[0]) + (((uint32_t)pSrc[1]) << 8) + (((uint32_t)pSrc[2]) << 16);
							*pDst++ = pConvTab[(val & m_pixelDescription.A_mask) >> m_pixelDescription.A_shift];
							pSrc += m_pixelSize;
						}
						pSrc += srcPitchAdd;
						pDst += dstPitchAdd;
					}
					break;
				}
				case 4:
				{
					for (int y = 0; y < bufferRect.h; y++)
					{
						for (int x = 0; x < bufferRect.w; x++)
						{
							uint32_t val = *((uint32_t*)pSrc);
							*pDst++ = pConvTab[(val & m_pixelDescription.A_mask) >> m_pixelDescription.A_shift];
							pSrc += m_pixelSize;
						}
						pSrc += srcPitchAdd;
						pDst += dstPitchAdd;
					}
					break;
				}
				default:
					break;
			}
		}
	
	}

	//____ _readBackTexture() ____________________________________________

	void GlSurface::_readBackTexture(void * pDest)
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
		case PixelFormat::BGR_8_sRGB:
		case PixelFormat::BGR_8_linear:
			type = GL_UNSIGNED_BYTE;
			break;
		case PixelFormat::BGRA_8:
		case PixelFormat::BGRA_8_sRGB:
		case PixelFormat::BGRA_8_linear:
		case PixelFormat::BGRX_8:
		case PixelFormat::BGRX_8_sRGB:
		case PixelFormat::BGRX_8_linear:
			type = GL_UNSIGNED_INT_8_8_8_8_REV;
			break;
		case PixelFormat::BGRA_4_linear:
			type = GL_UNSIGNED_SHORT_4_4_4_4_REV;
			break;
		case PixelFormat::BGR_565_linear:
			type = GL_UNSIGNED_SHORT_5_6_5_REV;
			break;
		case PixelFormat::A_8:
			type = GL_UNSIGNED_BYTE;
			break;
		case PixelFormat::CLUT_8:
		case PixelFormat::CLUT_8_sRGB:
		case PixelFormat::CLUT_8_linear:
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
		glGetTexImage(GL_TEXTURE_2D, 0, m_accessFormat, type, pDest);
		glPixelStorei(GL_PACK_ROW_LENGTH, 0);
		glBindTexture(GL_TEXTURE_2D, oldBinding);

		HANDLE_GLERROR(glGetError());

		m_bBackingBufferStale = false;
	}




} // namespace wg
