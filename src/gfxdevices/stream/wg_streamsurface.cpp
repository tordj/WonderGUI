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

#include <cstring>
#include <algorithm>



namespace wg
{
	const TypeInfo StreamSurface::TYPEINFO = { "StreamSurface", &Surface::TYPEINFO };

	//____ maxSize() _______________________________________________________________

	SizeI StreamSurface::maxSize()
	{
		return SizeI(4096,4096);
	}

	//____ create ______________________________________________________________

	StreamSurface_p StreamSurface::create(GfxStreamEncoder * pEncoder, const Blueprint& blueprint)
	{
		return StreamSurface_p(new StreamSurface(pEncoder, blueprint));
	}

	StreamSurface_p StreamSurface::create(GfxStreamEncoder * pEncoder, const Blueprint& blueprint, Blob* pBlob, int pitch)
	{
		return StreamSurface_p(new StreamSurface(pEncoder, blueprint, pBlob, pitch));
	}

	StreamSurface_p StreamSurface::create(GfxStreamEncoder * pEncoder, const Blueprint& blueprint, uint8_t* pPixels, int pitch, const PixelDescription* pPixelDescription)
	{
		return StreamSurface_p(new StreamSurface(pEncoder, blueprint, pPixels, pitch, pPixelDescription));
	}

	StreamSurface_p StreamSurface::create(GfxStreamEncoder * pEncoder, const Blueprint& blueprint, Surface* pOther)
	{
		return StreamSurface_p(new StreamSurface(pEncoder, blueprint, pOther));
	}


	StreamSurface_p	StreamSurface::create( GfxStreamEncoder * pEncoder, SizeI size, PixelFormat format, int flags, const Color8 * pClut )
	{
		if (format == PixelFormat::Undefined || format == PixelFormat::Custom || format < PixelFormat_min || format > PixelFormat_max || ((format == PixelFormat::CLUT_8 || format == PixelFormat::CLUT_8_sRGB || format == PixelFormat::CLUT_8_linear) && pClut == nullptr))
			return StreamSurface_p();

		Blueprint bp;

		bp.size = size;
		bp.format = format;

		bp.buffered = (flags & SurfaceFlag::Buffered);
		bp.canvas = (flags & SurfaceFlag::Canvas);
		bp.dynamic = (flags & SurfaceFlag::Dynamic);
		bp.mipmap = (flags & SurfaceFlag::Mipmapped);
		bp.scale = (flags & SurfaceFlag::Scale200) ? 128 : 64;
		bp.clut = pClut;

		return StreamSurface_p(new StreamSurface(pEncoder, bp));
	}

	StreamSurface_p	StreamSurface::create( GfxStreamEncoder * pEncoder, SizeI size, PixelFormat format, Blob * pBlob, int pitch, int flags, const Color8 * pClut )
	{
		Blueprint bp;

		bp.size = size;
		bp.format = format;

		bp.buffered = (flags & SurfaceFlag::Buffered);
		bp.canvas = (flags & SurfaceFlag::Canvas);
		bp.dynamic = (flags & SurfaceFlag::Dynamic);
		bp.mipmap = (flags & SurfaceFlag::Mipmapped);
		bp.scale = (flags & SurfaceFlag::Scale200) ? 128 : 64;
		bp.clut = pClut;

		return StreamSurface_p(new StreamSurface(pEncoder, bp, pBlob, pitch));
	}

	StreamSurface_p	StreamSurface::create( GfxStreamEncoder * pEncoder,SizeI size, PixelFormat format, uint8_t * pPixels, int pitch, const PixelDescription * pPixelDescription, int flags, const Color8 * pClut )
	{
		if (format == PixelFormat::Undefined || format == PixelFormat::Custom || format < PixelFormat_min || format > PixelFormat_max ||
			((format == PixelFormat::CLUT_8 || format == PixelFormat::CLUT_8_sRGB || format == PixelFormat::CLUT_8_linear) && pClut == nullptr) || pPixels == nullptr || pitch <= 0 || pPixelDescription == nullptr)
			return StreamSurface_p();

		Blueprint bp;

		bp.size = size;
		bp.format = format;

		bp.buffered = (flags & SurfaceFlag::Buffered);
		bp.canvas = (flags & SurfaceFlag::Canvas);
		bp.dynamic = (flags & SurfaceFlag::Dynamic);
		bp.mipmap = (flags & SurfaceFlag::Mipmapped);
		bp.scale = (flags & SurfaceFlag::Scale200) ? 128 : 64;
		bp.clut = pClut;

		return  StreamSurface_p(new StreamSurface(pEncoder, bp, pPixels, pitch, pPixelDescription));
	};

	StreamSurface_p	StreamSurface::create( GfxStreamEncoder * pEncoder, Surface * pOther, int flags )
	{
		if (!pOther)
			return StreamSurface_p();

		Blueprint bp;

		bp.buffered = (flags & SurfaceFlag::Buffered);
		bp.canvas = (flags & SurfaceFlag::Canvas);
		bp.dynamic = (flags & SurfaceFlag::Dynamic);
		bp.mipmap = (flags & SurfaceFlag::Mipmapped);
		bp.scale = (flags & SurfaceFlag::Scale200) ? 128 : 64;

		return StreamSurface_p(new StreamSurface(pEncoder, bp, pOther));
	}



	//____ constructor _____________________________________________________________

	StreamSurface::StreamSurface( GfxStreamEncoder * pEncoder, const Blueprint& bp) : Surface(bp, pEncoder->defaultPixelFormat(), pEncoder->defaultSampleMethod() )
	{
		m_pEncoder = pEncoder;
		m_pitch = ((bp.size.w + 3) & 0xFFFFFFFC)*m_pixelDescription.bits / 8;
		m_bDynamic = bp.dynamic;

		m_inStreamId = _sendCreateSurface(bp);

		if (m_pixelDescription.bits <= 8 || bp.buffered)
		{
			m_pBlob = Blob::create(m_pitch*bp.size.h + (bp.clut ? 1024 : 0) );
			std::memset(m_pBlob->data(), 0, m_pitch*bp.size.h);

			if (bp.clut)
			{
				m_pClut = (Color8*)((uint8_t*)m_pBlob->data() + m_pitch * bp.size.h);
				memcpy(m_pClut, bp.clut, 1024);
			}
			else
				m_pClut = nullptr;

			m_pAlphaLayer = nullptr;
		}
		else
		{
			if (m_pixelDescription.A_bits == 0)
				m_pAlphaLayer = nullptr;
			else
			{
				m_pAlphaLayer = new uint8_t[bp.size.w*bp.size.h];
				std::memset(m_pAlphaLayer, 0, bp.size.w*bp.size.h);
			}
		}
	}

	StreamSurface::StreamSurface( GfxStreamEncoder * pEncoder, const Blueprint& bp, Blob* pBlob, int pitch )
		: Surface(bp, pEncoder->defaultPixelFormat(), pEncoder->defaultSampleMethod())
	{
		m_pEncoder = pEncoder;
		m_pitch = pitch;
		m_bDynamic = bp.dynamic;

		m_inStreamId = _sendCreateSurface(bp);

		if (m_pixelDescription.bits <= 8 || bp.buffered)
		{
			m_pBlob = pBlob;
			m_pClut = const_cast<Color8*>(bp.clut);
			m_pAlphaLayer = nullptr;
		}
		else
		{
			if (m_pixelDescription.A_bits == 0)
				m_pAlphaLayer = nullptr;
			else
				m_pAlphaLayer = _genAlphaLayer((char*)pBlob->data(), pitch);
		}

		_sendPixels(m_pEncoder, bp.size, (uint8_t*) pBlob->data(), pitch);
		m_pEncoder->flush();
	}

	StreamSurface::StreamSurface( GfxStreamEncoder * pEncoder, const Blueprint& bp, uint8_t * pPixels, int pitch, const PixelDescription * pPixelDescription )
		: Surface(bp, pEncoder->defaultPixelFormat(), pEncoder->defaultSampleMethod())
	{
		m_pEncoder = pEncoder;
		m_pitch = ((bp.size.w + 3) & 0xFFFFFFFC)*m_pixelDescription.bits / 8;
		m_bDynamic = bp.dynamic;

		m_inStreamId = _sendCreateSurface(bp);

		// We always convert the data even if we throw it away, since we need to stream the converted data.
		// (but we could optimize and skip conversion if format already is correct)

		m_pBlob = Blob::create(m_pitch*m_size.h + (bp.clut ? 1024 : 0) );

		_copyFrom(pPixelDescription == 0 ? &m_pixelDescription : pPixelDescription, pPixels, pitch, bp.size, bp.size);

		// No _sendPixels() needed here, _copyFrom() calls pullPixels() which calls _sendPixels().

		if (m_pixelDescription.bits <= 8 || bp.buffered)
		{
			if (bp.clut)
			{
				m_pClut = (Color8*)((uint8_t*)m_pBlob->data() + m_pitch * bp.size.h);
				memcpy(m_pClut, bp.clut, 1024);
			}
			else
				m_pClut = nullptr;

			m_pAlphaLayer = nullptr;
		}
		else
		{
			m_pBlob = nullptr;

			if (m_pixelDescription.A_bits == 0)
				m_pAlphaLayer = nullptr;
			else
				m_pAlphaLayer = _genAlphaLayer((char*)pPixels, pitch);
		}

		m_pEncoder->flush();
	}


	StreamSurface::StreamSurface(GfxStreamEncoder * pEncoder, const Blueprint& bp, Surface* pOther)
		: Surface(bp, pEncoder->defaultPixelFormat(), pEncoder->defaultSampleMethod())
	{
		//TODO: This only works now if blueprint and pOther agrees on size, pixelformat, etc.

		assert(pOther);

		PixelFormat format = pOther->pixelFormat();
		auto pixelbuffer = pOther->allocPixelBuffer();
		bool bPushed = pOther->pushPixels(pixelbuffer);

		//TODO: Fail in a good way (return nullptr from create()) if we can't push pixels from pOther.

		int pitch = pixelbuffer.pitch;
		SizeI size = pixelbuffer.rect.size();

		m_pEncoder = pEncoder;
		m_size = size;
		m_pitch = ((size.w + 3) & 0xFFFFFFFC)*m_pixelDescription.bits / 8;
		m_bDynamic = bp.dynamic;

		Util::pixelFormatToDescription(format, m_pixelDescription);

		Blueprint myBP = bp;
		myBP.size = m_size;
		myBP.sampleMethod = m_sampleMethod;
		myBP.format = m_pixelDescription.format;

        m_inStreamId = _sendCreateSurface(myBP);
        
		if (m_pixelDescription.bits <= 8 || bp.buffered)
		{
			m_pBlob = Blob::create(m_pitch*m_size.h + (pOther->clut() ? 1024 : 0));

            // _copyFrom() implicitly calls _sendPixels().
			_copyFrom(&m_pixelDescription, pixelbuffer.pPixels, pitch, RectI(size), RectI(size));

			if (pOther->clut())
			{
				m_pClut = (Color8*)((uint8_t*)m_pBlob->data() + m_pitch * m_size.h);
				memcpy(m_pClut, pOther->clut(), 1024);
			}
			else
				m_pClut = nullptr;
		}
		else
		{
            _sendPixels(m_pEncoder, size, pixelbuffer.pPixels, pitch);

            if (m_pixelDescription.A_bits == 0)
				m_pAlphaLayer = nullptr;
			else
				m_pAlphaLayer = _genAlphaLayer((char*)pixelbuffer.pPixels, pitch);
		}

		pOther->freePixelBuffer(pixelbuffer);
		m_pEncoder->flush();
	}


	//____ Destructor ______________________________________________________________

	StreamSurface::~StreamSurface()
	{
		_sendDeleteSurface();
		m_pEncoder->freeObjectId(m_inStreamId);
		m_pEncoder->flush();
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& StreamSurface::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ allocPixelBuffer() _________________________________________________

	const PixelBuffer StreamSurface::allocPixelBuffer(const RectI& rect)
	{
		PixelBuffer	buf;

		if (m_pBlob)
		{
			buf.pPixels = (uint8_t*) m_pBlob->data() + m_pitch * rect.y + rect.x * m_pixelDescription.bits / 8;
			buf.pClut = m_pClut;
			buf.format = m_pixelDescription.format;
			buf.rect = rect;
			buf.pitch = m_pitch;
		}
		else
		{
			buf.pitch = ((rect.w + 3) & 0xFFFFFFFC) * m_pixelDescription.bits / 8;
			buf.pPixels = new uint8_t[buf.pitch*rect.h];
			buf.pClut = m_pClut;
			buf.format = m_pixelDescription.format;
			buf.rect = rect;
		}

		return buf;
	}

	//____ pushPixels() _______________________________________________________

	bool StreamSurface::pushPixels(const PixelBuffer& buffer, const RectI& bufferRect)
	{
		if (m_pBlob)
			return true;
		else
			return false;
	}

	//____ pullPixels() _______________________________________________________

	void StreamSurface::pullPixels(const PixelBuffer& buffer, const RectI& bufferRect)
	{
		_sendPixels(m_pEncoder, buffer.rect, buffer.pPixels, buffer.pitch);
		m_pEncoder->flush();
	}

	//____ freePixelBuffer() __________________________________________________

	void StreamSurface::freePixelBuffer(const PixelBuffer& buffer)
	{
		if (!m_pBlob)
			delete[] buffer.pPixels;
	}

	//____ alpha() ____________________________________________________________

	int StreamSurface::alpha( CoordSPX coord )
	{
		PixelBuffer	buffer;

		if (m_pAlphaLayer)
		{
			buffer.format = PixelFormat::A_8;
			buffer.pClut = nullptr;
			buffer.pitch = m_size.w;
			buffer.pPixels = m_pAlphaLayer;
			buffer.rect = { 0,0,m_size };
		}
		else
		{
			buffer.format = m_pixelDescription.format;
			buffer.pClut = m_pClut;
			buffer.pitch = m_pitch;
			buffer.pPixels = (uint8_t*) m_pBlob->data();
			buffer.rect = { 0,0,m_size };
		}

		return _alpha(coord, buffer);
	}

	//____ fill() _____________________________________________________________

	bool StreamSurface::fill(HiColor col)
	{
		return fill(col, RectI(0, 0, pixelSize()));
	}

	bool StreamSurface::fill(HiColor col, const RectI& region)
	{
		// Stream the call

		*m_pEncoder << GfxStream::Header{ GfxChunkId::FillSurface, 14 };
		*m_pEncoder << m_inStreamId;
		*m_pEncoder << region;
		*m_pEncoder << col;
		m_pEncoder->flush();

		// Update local copy or alpha channel (if any)

		if (m_pBlob)
		{
			uint32_t pixel = colorToPixel(col);
			int w = region.w;
			int h = region.h;
			int p = m_pitch;
			uint8_t * pDest = reinterpret_cast<uint8_t*>(m_pBlob->data()) + region.y * p + region.x*m_pixelDescription.bits / 8;

			switch (m_pixelDescription.bits)
			{
			case 8:
				for (int y = 0; y < h; y++)
				{
					for (int x = 0; x < w; x++)
						pDest[x] = (uint8_t)pixel;
					pDest += p;
				}
				break;
			case 16:
				for (int y = 0; y < h; y++)
				{
					for (int x = 0; x < w; x++)
						((uint16_t*)pDest)[x] = (uint16_t)pixel;
					pDest += p;
				}
				break;
			case 24:
			{
				uint8_t one = (uint8_t)pixel;
				uint8_t two = (uint8_t)(pixel >> 8);
				uint8_t three = (uint8_t)(pixel >> 16);

				for (int y = 0; y < h; y++)
				{
					for (int x = 0; x < w; x++)
					{
						pDest[x++] = one;
						pDest[x++] = two;
						pDest[x++] = three;
					}
					pDest += p - w * 3;
				}
				break;
			}
			case 32:
				for (int y = 0; y < h; y++)
				{
					for (int x = 0; x < w; x++)
						((uint32_t*)pDest)[x] = pixel;
					pDest += p;
				}
				break;
			default:
				assert(false);
			}
		}
		else if (m_pAlphaLayer)
		{
			for (int i = 0; i < m_size.w*m_size.h; i++)
				m_pAlphaLayer[i] = col.a;
		}

		//
		return true;
	}

	//____ streamAsNew() ______________________________________________________

	bool StreamSurface::streamAsNew(GfxStreamEncoder* pEncoder)
	{
		if (!m_pBlob)
			return false;

		uint16_t blockSize = 30 + (m_pClut ? 1024 : 0);

		*pEncoder << GfxStream::Header{ GfxChunkId::CreateSurface, blockSize };
		*pEncoder << m_inStreamId;
		*pEncoder << m_bCanvas;
		*pEncoder << m_bDynamic;
		*pEncoder << m_pixelDescription.format;
		*pEncoder << m_id;
		*pEncoder << m_bMipmapped;
		*pEncoder << m_sampleMethod;
		*pEncoder << m_scale;
		*pEncoder << m_size;
		*pEncoder << m_bTiling;

		if (m_pClut)
			*pEncoder << GfxStream::DataChunk{ 1024, m_pClut };

		_sendPixels(pEncoder, m_size, (uint8_t*)m_pBlob->data(), m_pitch);
		m_pEncoder->flush();
		return true;
	}


	//____ _sendCreateSurface() _______________________________________________

	uint16_t StreamSurface::_sendCreateSurface(const Blueprint& bp)
	{
		uint16_t surfaceId = m_pEncoder->allocObjectId();

		uint16_t blockSize = 30 + (bp.clut ? 1024 : 0);

		*m_pEncoder << GfxStream::Header{ GfxChunkId::CreateSurface, blockSize };
		*m_pEncoder << surfaceId;
		*m_pEncoder << bp.canvas;
		*m_pEncoder << bp.dynamic;
		*m_pEncoder << bp.format;
		*m_pEncoder << bp.id;
		*m_pEncoder << bp.mipmap;
		*m_pEncoder << bp.sampleMethod;
		*m_pEncoder << bp.scale;
		*m_pEncoder << bp.size;
		*m_pEncoder << bp.tiling;

		if(bp.clut)
			*m_pEncoder << GfxStream::DataChunk{ 1024, bp.clut };

		return surfaceId;
	}

	//____ _genAlphaLayer() _________________________________________________________

	uint8_t * StreamSurface::_genAlphaLayer(const char * pSource, int pitch)
	{
		uint8_t * pAlphaLayer = new uint8_t[m_size.w*m_size.h];

		//TODO: Support more than 8 bit alpha.

		uint8_t * pDest = pAlphaLayer;
		int pixelPitch = m_pixelDescription.bits / 8;
		for (int y = 0; y < m_size.h; y++)
		{
			const char * pPixelAlpha = pSource + y*pitch + pixelPitch -1;

			for (int x = 0; x < m_size.w; x++)
			{
				*pDest++ = *pPixelAlpha;
				pPixelAlpha += pixelPitch;
			}
		}
		return pAlphaLayer;
	}

	//____ _sendPixels() _________________________________________________________

	void StreamSurface::_sendPixels(GfxStreamEncoder* pEncoder, RectI rect, const uint8_t * pSource, int pitch)
	{
		*pEncoder << GfxStream::Header{ GfxChunkId::BeginSurfaceUpdate, 18 };
		*pEncoder << m_inStreamId;
		*pEncoder << rect;

		int	pixelSize = m_pixelDescription.bits / 8;
		int dataSize = rect.w * rect.h * pixelSize;

		const uint8_t * pLine = pSource;
		int ofs = 0;				// Offset in bytes within the current line.

		while( dataSize > 0 )
		{
			uint16_t chunkSize = std::min(dataSize, (int)(GfxStream::c_maxBlockSize - sizeof(GfxStream::Header)));
			dataSize -= chunkSize;

			*pEncoder << GfxStream::Header{ GfxChunkId::SurfacePixels, chunkSize };

			while (chunkSize > 0)
			{
				int len = rect.w * pixelSize - ofs;
				if (chunkSize < len)
				{
					*pEncoder << GfxStream::DataChunk{ chunkSize, (void *) (pLine + ofs) };
					ofs += chunkSize;
					chunkSize = 0;
				}
				else
				{
					*pEncoder << GfxStream::DataChunk{ len, (void *)(pLine + ofs) };
					ofs = 0;
					chunkSize -= len;
					pLine += pitch;
				}
			}
		}

		*pEncoder << GfxStream::Header{ GfxChunkId::EndSurfaceUpdate, 0 };
	}

	//____ _sendDeleteSurface() _______________________________________________

	void StreamSurface::_sendDeleteSurface()
	{
		*m_pEncoder << GfxStream::Header{ GfxChunkId::DeleteSurface, 2 };
		*m_pEncoder << m_inStreamId;

	}

} // namespace wg
