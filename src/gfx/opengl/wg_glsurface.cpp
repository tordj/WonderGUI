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
#include <wg_gfxutil.h>
#include <wg_blob.h>
#include <wg_gfxbase.h>
#include <assert.h>



namespace wg
{
	const TypeInfo GlSurface::TYPEINFO = { "GlSurface", &Surface::TYPEINFO };

	SizeI	GlSurface::s_maxSize;

#define HANDLE_GLERROR(check) { GLenum err = check; if(err != 0) GlGfxDevice::onGlError(err, this, &TYPEINFO, __func__, __FILE__, __LINE__ ); }

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

	//____ Create ______________________________________________________________

	GlSurface_p GlSurface::create(const Blueprint& bp)
	{
		if( !_isBlueprintValid(bp, maxSize()) )
			return GlSurface_p();

		return GlSurface_p(new GlSurface(bp));
	}

	GlSurface_p GlSurface::create(const Blueprint& bp, Blob* pBlob, int pitch)
	{
		if (!_isBlueprintValid(bp, maxSize()) )
			return GlSurface_p();

		if ( !pBlob || (pitch > 0 && pitch % 4 != 0))
			return GlSurface_p();


		return GlSurface_p(new GlSurface(bp, pBlob, pitch));
	}

	GlSurface_p GlSurface::create(const Blueprint& bp, uint8_t* pPixels, int pitch, const PixelDescription* pPixelDescription)
	{
		if (!_isBlueprintValid(bp, maxSize()))
			return GlSurface_p();

		return GlSurface_p(new GlSurface(bp, pPixels, pitch, pPixelDescription));
	}

	GlSurface_p GlSurface::create(const Blueprint& bp, Surface* pOther)
	{
		if (!_isBlueprintValid(bp, maxSize(), pOther))
			return GlSurface_p();

		return GlSurface_p(new GlSurface(bp, pOther));
	}

	GlSurface_p GlSurface::create(SizeI size, PixelFormat format, int flags, const Color8* pPalette)
	{
		Blueprint bp;

		bp.size = size;
		bp.format = format;

		bp.buffered = (flags & SurfaceFlag::Buffered);
		bp.canvas = (flags & SurfaceFlag::Canvas);
		bp.dynamic = (flags & SurfaceFlag::Dynamic);
		bp.mipmap = (flags & SurfaceFlag::Mipmapped);
		bp.scale = (flags & SurfaceFlag::Scale200) ? 128 : 64;
		bp.palette = pPalette;

		if( flags & SurfaceFlag::Bilinear )
			bp.sampleMethod = SampleMethod::Bilinear;
		
		if (!_isBlueprintValid(bp, maxSize()))
			return GlSurface_p();

		return GlSurface_p(new GlSurface(bp));
	}

	GlSurface_p GlSurface::create(SizeI size, PixelFormat format, Blob* pBlob, int pitch, int flags, const Color8* pPalette)
	{
		Blueprint bp;

		bp.size = size;
		bp.format = format;

		bp.buffered = (flags & SurfaceFlag::Buffered);
		bp.canvas = (flags & SurfaceFlag::Canvas);
		bp.dynamic = (flags & SurfaceFlag::Dynamic);
		bp.mipmap = (flags & SurfaceFlag::Mipmapped);
		bp.scale = (flags & SurfaceFlag::Scale200) ? 128 : 64;
		bp.palette = pPalette;

		if( flags & SurfaceFlag::Bilinear )
			bp.sampleMethod = SampleMethod::Bilinear;

		if (!_isBlueprintValid(bp, maxSize()))
			return GlSurface_p();

		return GlSurface_p(new GlSurface(bp, pBlob, pitch));
	}

	GlSurface_p GlSurface::create(SizeI size, PixelFormat format, uint8_t* pPixels, int pitch, const PixelDescription* pPixelDescription, int flags, const Color8* pPalette)
	{
		Blueprint bp;

		bp.size = size;
		bp.format = format;

		bp.buffered = (flags & SurfaceFlag::Buffered);
		bp.canvas = (flags & SurfaceFlag::Canvas);
		bp.dynamic = (flags & SurfaceFlag::Dynamic);
		bp.mipmap = (flags & SurfaceFlag::Mipmapped);
		bp.scale = (flags & SurfaceFlag::Scale200) ? 128 : 64;
		bp.palette = pPalette;

		if( flags & SurfaceFlag::Bilinear )
			bp.sampleMethod = SampleMethod::Bilinear;

		if (!_isBlueprintValid(bp, maxSize()))
			return GlSurface_p();

		return  GlSurface_p(new GlSurface(bp, pPixels, pitch, pPixelDescription));
	};

	GlSurface_p GlSurface::create(Surface* pOther, int flags)
	{
		if (!pOther)
			return GlSurface_p();

		Blueprint bp;

		bp.buffered = (flags & SurfaceFlag::Buffered);
		bp.canvas = (flags & SurfaceFlag::Canvas);
		bp.dynamic = (flags & SurfaceFlag::Dynamic);
		bp.mipmap = (flags & SurfaceFlag::Mipmapped);
		bp.scale = (flags & SurfaceFlag::Scale200) ? 128 : 64;

		if( flags & SurfaceFlag::Bilinear )
			bp.sampleMethod = SampleMethod::Bilinear;

		if (!_isBlueprintValid(bp, maxSize(), pOther))
			return GlSurface_p();

		return GlSurface_p(new GlSurface(bp, pOther));
	}

	//____ constructor _____________________________________________________________


	GlSurface::GlSurface( const Blueprint& bp ) : Surface(bp, PixelFormat::BGRA_8, SampleMethod::Bilinear)
	{
		HANDLE_GLERROR(glGetError());

		_setPixelDetails(m_pixelDescription.format);
		m_pPalette = nullptr;

		if (bp.buffered || m_pixelDescription.bits <= 8)
        {
            g_backingPixels += m_size.w*m_size.h;
            m_pitch = ((m_size.w * m_pixelDescription.bits / 8) + 3) & 0xFFFFFFFC;
            m_pBlob = Blob::create(m_pitch * m_size.h + (bp.palette ? 1024 : 0));

            if (bp.palette)
            {
                m_pPalette = (Color8*)((uint8_t*)m_pBlob->data() + m_pitch * m_size.h);
                memcpy(m_pPalette, bp.palette, 1024);
            }
        }
        else
		{
			m_pitch = 0;

			if (bp.palette)
			{
				m_pPalette = new Color8[256];
				memcpy(m_pPalette, bp.palette, 1024);
			}

			if (m_pixelDescription.A_bits > 0)
				m_pAlphaMap = new uint8_t[m_size.w * m_size.h];
		}

		_setupGlTexture(nullptr, 0);

		HANDLE_GLERROR(glGetError());
	}


	GlSurface::GlSurface(const Blueprint& bp, Blob* pBlob, int pitch) : Surface(bp, PixelFormat::BGRA_8, SampleMethod::Bilinear)
	{
		// Set general information


		_setPixelDetails(m_pixelDescription.format);
		m_pPalette = const_cast<Color8*>(bp.palette);

		if (pitch == 0)
			pitch = bp.size.w * m_pixelDescription.bits / 8;

        if ((bp.buffered) || m_pixelDescription.bits <= 8)
        {
            g_backingPixels += m_size.w*m_size.h;
            m_pitch = pitch;
            m_pBlob = pBlob;
        }
        else
		{
			m_pitch = 0;

			if (bp.palette)
			{
				m_pPalette = new Color8[256];
				memcpy(m_pPalette, bp.palette, 1024);
			}

			if (m_pixelDescription.A_bits > 0)
			{
				m_pAlphaMap = new uint8_t[m_size.w * m_size.h];

				// Setup a fake PixelBuffer for call to _updateAlphaMap
				PixelBuffer buf;
				buf.format = m_pixelDescription.format;
				buf.pPalette = m_pPalette;
				buf.pitch = pitch;
				buf.pPixels = (uint8_t*) pBlob->data();
				buf.rect = m_size;

				_updateAlphaMap( buf, m_size );
			}
		}

		//TODO: Support pitch
		
		_setupGlTexture(pBlob->data(), pitch);
	}

	GlSurface::GlSurface(const Blueprint& bp, uint8_t* pPixels, int pitch, const PixelDescription* pPixelDescription ) 
		: Surface(bp, PixelFormat::BGRA_8, SampleMethod::Bilinear)
	{
		//TODO: Not just default to BGRA_8 if PixelFormat not specified in Blueprint. Instead we should take the most suitable PixelFormat base on pPixelDescription
		
		_setPixelDetails(m_pixelDescription.format);
		m_pPalette = nullptr;

		if( !pPixelDescription )
			pPixelDescription = &m_pixelDescription;
		
		if( pitch == 0 )
			pitch = bp.size.w * pPixelDescription->bits/8;
		
        if (bp.buffered || m_pixelDescription.bits <= 8)
        {
            g_backingPixels += m_size.w*m_size.h;

            m_pitch = ((m_size.w * m_pixelDescription.bits / 8) + 3) & 0xFFFFFFFC;
            m_pBlob = Blob::create(m_pitch * m_size.h + (bp.palette ? 1024 : 0));

            _copy(m_size, pPixelDescription, pPixels, pitch, m_size);

			// Setup palette before calling _setupGlTexture().

			if (bp.palette)
			{
				m_pPalette = (Color8*)((uint8_t*)m_pBlob->data() + m_pitch * m_size.h);
				memcpy(m_pPalette, bp.palette, 1024);
			}

            _setupGlTexture(m_pBlob->data(), m_pitch);
        }
        else
		{
			// Setup palette before calling _setupGlTexture().

			if (bp.palette)
			{
				m_pPalette = new Color8[256];
				memcpy(m_pPalette, bp.palette, 1024);
			}

			// Set blob and pitch temporarily so we can use _copy().
						
            m_pitch = ((m_size.w * m_pixelDescription.bits / 8) + 3) & 0xFFFFFFFC;
            m_pBlob = Blob::create(m_pitch * m_size.h + (bp.palette ? 1024 : 0));

            _copy(m_size, pPixelDescription == 0 ? &m_pixelDescription : pPixelDescription, pPixels, pitch, m_size);

            _setupGlTexture(m_pBlob->data(), m_pitch);

            m_pBlob = nullptr;
            m_pitch = 0;

			if (m_pixelDescription.A_bits > 0)
			{
				m_pAlphaMap = new uint8_t[m_size.w * m_size.h];

				// Setup a fake PixelBuffer for call to _updateAlphaMap
				PixelBuffer buf;
				buf.format = m_pixelDescription.format;
				buf.pPalette = m_pPalette;
				buf.pitch = pitch;
				buf.pPixels = pPixels;
				buf.rect = m_size;

				_updateAlphaMap(buf, m_size);
			}
		}
	}


	GlSurface::GlSurface(const Blueprint& bp, Surface* pOther) : Surface(bp, pOther->pixelFormat(), pOther->sampleMethod() )
	{
		_setPixelDetails(m_pixelDescription.format);
		m_size	= pOther->pixelSize();
		m_pPalette = nullptr;

		auto pixbuf = pOther->allocPixelBuffer();
		if( !pOther->pushPixels(pixbuf) )
		{
			//TODO: Error handling
		}

        if (bp.buffered || m_pixelDescription.bits <= 8)
        {
            g_backingPixels += m_size.w*m_size.h;

            m_pitch = m_size.w * m_pixelSize;
            m_pBlob = Blob::create(m_pitch * m_size.h + (m_pixelDescription.bIndexed ? 1024 : 0));
            _copy(m_size, pOther->pixelDescription(), pixbuf.pPixels, pixbuf.pitch, m_size);

            if (m_pixelDescription.bIndexed)
            {
                m_pPalette = (Color8*)((uint8_t*)m_pBlob->data() + m_pitch * m_size.h);
                memcpy(m_pPalette, pOther->palette(), 1024);
            }
        }
        else
		{
			m_pitch = 0;

			if (m_pixelDescription.bIndexed)
			{
				m_pPalette = new Color8[256];
				memcpy(m_pPalette, pOther->palette(), 1024);
			}

			if (m_pixelDescription.A_bits > 0)
			{
				m_pAlphaMap = new uint8_t[m_size.w * m_size.h];
				_updateAlphaMap(pixbuf, m_size);
			}
		}

		//TODO: Support pitch

        _setupGlTexture(pixbuf.pPixels, pixbuf.pitch);

		pOther->freePixelBuffer(pixbuf);
	}

	//____ _setupGlTexture() __________________________________________________

	void GlSurface::_setupGlTexture(void* pPixelsToUpload, int pitch)
	{
		GLint oldBinding;
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &oldBinding);

        g_texturePixels += m_size.w*m_size.h;

		// Create and bind texture

		glGenTextures(1, &m_texture);
		glBindTexture(GL_TEXTURE_2D, m_texture);

		// Set tiling

		GLint mode = m_bTiling ? GL_REPEAT : GL_CLAMP_TO_EDGE;

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mode);

		m_bTiling = m_bTiling;

		// Push pixels

		HANDLE_GLERROR(glGetError());
        glPixelStorei(GL_UNPACK_ROW_LENGTH, pitch/m_pixelSize);
        glTexImage2D(GL_TEXTURE_2D, 0, m_internalFormat, m_size.w, m_size.h, 0, m_accessFormat, m_pixelDataType, pPixelsToUpload);
        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		HANDLE_GLERROR(glGetError());

		//

		if (m_pPalette)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);


			HANDLE_GLERROR(glGetError());

			glGenTextures(1, &m_paletteTexture);
			glBindTexture(GL_TEXTURE_2D, m_paletteTexture);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

			glTexImage2D(GL_TEXTURE_2D, 0, m_pixelDescription.bLinear ? GL_RGBA8 : GL_SRGB8_ALPHA8, 256, 1, 0, GL_BGRA, GL_UNSIGNED_BYTE, m_pPalette);

			HANDLE_GLERROR(glGetError());
		}
		else
		{
			switch (m_sampleMethod)
			{
			case SampleMethod::Bilinear:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_bMipmapped ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				break;

			case SampleMethod::Nearest:
			default:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				break;
			}

			if (m_bMipmapped)
				m_bMipmapStale = true;
		}

		//

		glBindTexture(GL_TEXTURE_2D, oldBinding);

		HANDLE_GLERROR(glGetError());
	}

	//____ _setPixelDetails() ____________________________________________________

	void GlSurface::_setPixelDetails( PixelFormat format )
	{
		switch (format)
		{
			case PixelFormat::BGR_8:
				m_internalFormat = GfxBase::defaultToSRGB() ? GL_SRGB8 : GL_RGB8;
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
				m_internalFormat = GfxBase::defaultToSRGB() ? GL_SRGB8 : GL_RGB8;
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
				m_internalFormat = GfxBase::defaultToSRGB() ? GL_SRGB8_ALPHA8 : GL_RGBA8;
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

			case PixelFormat::Index_8:
			case PixelFormat::Index_8_sRGB:
			case PixelFormat::Index_8_linear:
				m_internalFormat = GL_R8;
				m_accessFormat = GL_RED;
				m_pixelDataType = GL_UNSIGNED_BYTE;
				m_pixelSize = 1;
				break;

			case PixelFormat::Alpha_8:
				m_internalFormat = GL_R8;
				m_accessFormat = GL_RED;
				m_pixelDataType = GL_UNSIGNED_BYTE;
				m_pixelSize = 1;
				break;

			default:
				assert(false);           // Should never get here, just avoiding compiler warnings.
				break;

		}
	}

	//____ Destructor ______________________________________________________________

	GlSurface::~GlSurface()
	{
        
        g_texturePixels -= m_size.w*m_size.h;

        if( m_pBlob )
            g_backingPixels -= m_size.w*m_size.h;

        
		// Free the stuff

		glDeleteTextures( 1, &m_texture );

		if (m_pPalette)
		{
			glDeleteTextures(1, &m_paletteTexture);
			if (!m_pBlob)
				delete[] m_pPalette;		// Palette is not part of the blob.
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

	//____ allocPixelBuffer() _________________________________________________

	const PixelBuffer GlSurface::allocPixelBuffer(const RectI& rect)
	{
		PixelBuffer pixbuf;

		if (m_pBlob)
		{
			pixbuf.format = m_pixelDescription.format;
			pixbuf.pPalette = m_pPalette;
			pixbuf.pitch = m_pitch;
			pixbuf.pPixels = ((uint8_t*)m_pBlob->data()) + rect.y * m_pitch + rect.x * m_pixelSize;
			pixbuf.rect = rect;
		}
		else
		{
			pixbuf.format = m_pixelDescription.format;
			pixbuf.pPalette = m_pPalette;
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

		Surface::pullPixels(buffer, bufferRect);
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
				return HiColor::unpackLinearTab[m_pPalette[*pPixel].a];
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

		switch (m_sampleMethod)
		{
		case SampleMethod::Bilinear:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			break;

		case SampleMethod::Nearest:
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
		case PixelFormat::Alpha_8:
			type = GL_UNSIGNED_BYTE;
			break;
		case PixelFormat::Index_8:
		case PixelFormat::Index_8_sRGB:
		case PixelFormat::Index_8_linear:
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
