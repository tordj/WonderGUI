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



namespace wg
{
	const TypeInfo StreamSurface::TYPEINFO = { "StreamSurface", &Surface::TYPEINFO };

	//____ maxSize() _______________________________________________________________

	SizeI StreamSurface::maxSize()
	{
		return SizeI(4096,4096);
	}

	//____ create ______________________________________________________________

	StreamSurface_p	StreamSurface::create( CGfxOutStream& stream, SizeI size, PixelFormat format, int flags, const Color8 * pClut )
	{
		if (format == PixelFormat::Unknown || format == PixelFormat::Custom || format < PixelFormat_min || format > PixelFormat_max || ((format == PixelFormat::CLUT_8 || format == PixelFormat::CLUT_8_sRGB || format == PixelFormat::CLUT_8_linear) && pClut == nullptr))
			return StreamSurface_p();

		return StreamSurface_p(new StreamSurface(stream,size,format,flags,pClut));
	}

	StreamSurface_p	StreamSurface::create( CGfxOutStream& stream, SizeI size, PixelFormat format, Blob * pBlob, int pitch, int flags, const Color8 * pClut )
	{
		if (format == PixelFormat::Unknown || format == PixelFormat::Custom || format < PixelFormat_min || format > PixelFormat_max || ((format == PixelFormat::CLUT_8 || format == PixelFormat::CLUT_8_sRGB || format == PixelFormat::CLUT_8_linear) && pClut == nullptr) || !pBlob || pitch % 4 != 0)
			return StreamSurface_p();

		return StreamSurface_p(new StreamSurface(stream,size,format,pBlob,pitch,flags,pClut));
	}

	StreamSurface_p	StreamSurface::create( CGfxOutStream& stream,SizeI size, PixelFormat format, uint8_t * pPixels, int pitch, const PixelDescription * pPixelDescription, int flags, const Color8 * pClut )
	{
		if (format == PixelFormat::Unknown || format == PixelFormat::Custom || format < PixelFormat_min || format > PixelFormat_max ||
			((format == PixelFormat::CLUT_8 || format == PixelFormat::CLUT_8_sRGB || format == PixelFormat::CLUT_8_linear) && pClut == nullptr) || pPixels == nullptr || pitch <= 0 || pPixelDescription == nullptr)
			return StreamSurface_p();

		return  StreamSurface_p(new StreamSurface(stream,size,format,pPixels,pitch, pPixelDescription,flags,pClut));
	};

	StreamSurface_p	StreamSurface::create( CGfxOutStream& stream, Surface * pOther, int flags )
	{
		return StreamSurface_p(new StreamSurface( stream,pOther,flags ));
	}



	//____ constructor _____________________________________________________________

	StreamSurface::StreamSurface( CGfxOutStream& stream,SizeI size, PixelFormat format, int flags, const Color8 * pClut) : Surface(flags)
	{
		Util::pixelFormatToDescription(format, m_pixelDescription);

		m_pStream = &stream;
		m_size = size;
		m_pitch = ((size.w + 3) & 0xFFFFFFFC)*m_pixelDescription.bits / 8;

		m_inStreamId = _sendCreateSurface(size, format, flags, pClut);

		if (m_pixelDescription.bits <= 8 || (flags & SurfaceFlag::Buffered))
		{
			m_pBlob = Blob::create(m_pitch*size.h + (pClut ? 4096 : 0) );
			std::memset(m_pBlob->data(), 0, m_pitch*size.h);

			if (pClut)
			{
				m_pClut = (Color8*)((uint8_t*)m_pBlob->data() + m_pitch * size.h);
				memcpy(m_pClut, pClut, 4096);
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
				m_pAlphaLayer = new uint8_t[size.w*size.h];
				std::memset(m_pAlphaLayer, 0, size.w*size.h*2);
			}
		}
	}

	StreamSurface::StreamSurface( CGfxOutStream& stream,SizeI size, PixelFormat format, Blob * pBlob, int pitch, int flags, const Color8 * pClut) : Surface(flags)
	{
		Util::pixelFormatToDescription(format, m_pixelDescription);

		m_pStream = &stream;
		m_size = size;
		m_pitch = pitch;

		m_inStreamId = _sendCreateSurface(size, format, flags, pClut);

		if (m_pixelDescription.bits <= 8 || (flags & SurfaceFlag::Buffered))
		{
			m_pBlob = pBlob;
			m_pClut = const_cast<Color8*>(pClut);
			m_pAlphaLayer = nullptr;
		}
		else
		{
			if (m_pixelDescription.A_bits == 0)
				m_pAlphaLayer = nullptr;
			else
				m_pAlphaLayer = _genAlphaLayer((char*)pBlob->data(), pitch);
		}

		_sendPixels(size, (uint8_t*) pBlob->data(), pitch);
	}

	StreamSurface::StreamSurface( CGfxOutStream& stream,SizeI size, PixelFormat format, uint8_t * pPixels, int pitch, const PixelDescription * pPixelDescription, int flags, const Color8 * pClut) : Surface(flags)
	{
		Util::pixelFormatToDescription(format, m_pixelDescription);

		m_pStream = &stream;
		m_size = size;
		m_pitch = ((size.w + 3) & 0xFFFFFFFC)*m_pixelDescription.bits / 8;

		m_inStreamId = _sendCreateSurface(size, format, flags, pClut);

		// We always convert the data even if we throw it away, since we need to stream the converted data.
		// (but we could optimize and skip conversion if format already is correct)

		Blob_p pBlob = Blob::create(m_pitch*m_size.h + (pClut ? 4096 : 0) );

		_copyFrom(pPixelDescription == 0 ? &m_pixelDescription : pPixelDescription, pPixels, pitch, size, size);
		_sendPixels(size, (uint8_t*)pBlob->data(), pitch);


		if (m_pixelDescription.bits <= 8 || (flags & SurfaceFlag::Buffered))
		{
			m_pBlob = pBlob;

			if (pClut)
			{
				m_pClut = (Color8*)((uint8_t*)m_pBlob->data() + m_pitch * size.h);
				memcpy(m_pClut, pClut, 4096);
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
				m_pAlphaLayer = _genAlphaLayer((char*)pPixels, pitch);
		}

	}


	StreamSurface::StreamSurface( CGfxOutStream& stream, Surface * pOther, int flags ) : Surface(flags)
	{
		assert(pOther);

		PixelFormat format = pOther->pixelFormat();
		auto pixelbuffer = pOther->allocPixelBuffer();
		pOther->pushPixels(pixelbuffer);

		int pitch = pixelbuffer.pitch;
		SizeI size = pixelbuffer.rect.size();

		m_pStream = &stream;
		m_size = size;
		m_pitch = ((size.w + 3) & 0xFFFFFFFC)*m_pixelDescription.bits / 8;

		Util::pixelFormatToDescription(format, m_pixelDescription);

		if (m_pixelDescription.bits <= 8 || (flags & SurfaceFlag::Buffered))
		{
			m_pBlob = Blob::create(m_pitch*m_size.h + (pOther->clut() ? 4096 : 0));

			_copyFrom(&m_pixelDescription, pixelbuffer.pPixels, pitch, RectI(size), RectI(size));

			if (pOther->clut())
			{
				m_pClut = (Color8*)((uint8_t*)m_pBlob->data() + m_pitch * m_size.h);
				memcpy(m_pClut, pOther->clut(), 4096);
			}
			else
				m_pClut = nullptr;
		}
		else
		{
			if (m_pixelDescription.A_bits == 0)
				m_pAlphaLayer = nullptr;
			else
				m_pAlphaLayer = _genAlphaLayer((char*)pixelbuffer.pPixels, pitch);
		}

		m_inStreamId = _sendCreateSurface(size, format, flags, pOther->clut());

		_sendPixels(size, pixelbuffer.pPixels, pitch);
		pOther->freePixelBuffer(pixelbuffer);
	}


	//____ Destructor ______________________________________________________________

	StreamSurface::~StreamSurface()
	{
		_sendDeleteSurface();
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& StreamSurface::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setScaleMode() __________________________________________________________

	void StreamSurface::setScaleMode( ScaleMode mode )
	{
		if (mode != m_scaleMode)
		{
			*m_pStream << GfxStream::Header{ GfxChunkId::SetSurfaceScaleMode, 4 };
			*m_pStream << m_inStreamId;
			*m_pStream << mode;

			Surface::setScaleMode(mode);
		}
	}

	//____ isOpaque() ______________________________________________________________

	bool StreamSurface::isOpaque() const
	{
		if (m_pixelDescription.A_bits == 0)
			return true;

		return false;
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
		_sendPixels(buffer.rect.size(), buffer.pPixels, buffer.pitch);

	}

	//____ freePixelBuffer() __________________________________________________

	void StreamSurface::freePixelBuffer(const PixelBuffer& buffer)
	{
		if (!m_pBlob)
			delete[] buffer.pPixels;
	}

	//____ alpha() ____________________________________________________________

	uint8_t StreamSurface::alpha( CoordI coord )
	{
		if (m_pixelDescription.A_bits == 0)
			return 255;

		if (m_pAlphaLayer)
		{
			return m_pAlphaLayer[coord.x + coord.y*m_size.w];
		}
		else
		{
			return ((uint8_t*)m_pBlob->data())[coord.x*m_pixelDescription.bits / 8 + coord.y*m_pitch];
		}
	}

	//____ fill() _____________________________________________________________

	bool StreamSurface::fill(HiColor col)
	{
		return fill(col, RectI(0, 0, size()));
	}

	bool StreamSurface::fill(HiColor col, const RectI& region)
	{
		// Stream the call

		*m_pStream << GfxStream::Header{ GfxChunkId::FillSurface, 14 };
		*m_pStream << m_inStreamId;
		*m_pStream << region;
		*m_pStream << col;

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

	//____ _copyFrom() ________________________________________________________

	bool StreamSurface::copyFrom(Surface * pSrcSurface, CoordI dst)
	{
		if (!pSrcSurface)
			return false;

		return copyFrom(pSrcSurface, RectI(0, 0, pSrcSurface->size()), dst);
	}

	bool StreamSurface::copyFrom(Surface * pSrcSurf, const RectI& srcRect, CoordI dst)
	{
		//TODO: Implement!!!

		assert(false);
		return true;
/*
		// First we update local data without streaming anything (no lock/unlock)

		if (m_pBlob)
		{
			m_pPixels = (uint8_t*)m_pBlob->data();	// Simulate a lock
			_copyFrom(pSrcSurf->pixelDescription(), pPixels, pitch, size, size);
			_sendPixels(size, m_pPixels, pitch);
			m_pPixels = 0;
		}
		else
		{

		}


		// Stream command or modified content

		if (pSrcSurf->className() == StreamSurface::CLASSNAME)
		{
			// Since they both are stream surfaces, we just need to order a copy.

			*m_pStream << GfxStream::Header{ GfxChunkId::CopySurface, 16 };
			*m_pStream << m_inStreamId;
			*m_pStream << static_cast<StreamSurface*>(pSrcSurf)->m_inStreamId;
			*m_pStream << srcRect;
			*m_pStream << dst;
		}
		else
		{
			// Source is not a stream surface, so we need to stream modified content


			_sendPixels(m_lockRegion, m_pPixels, m_pitch);

		}
*/

	}

	//____ _sendCreateSurface() _______________________________________________

	short StreamSurface::_sendCreateSurface(SizeI size, PixelFormat format, int flags, const Color8 * pClut )
	{
		uint16_t surfaceId = m_pStream->allocObjectId();

		int blockSize = 10 + (pClut ? 4096 : 0);

		*m_pStream << GfxStream::Header{ GfxChunkId::CreateSurface, blockSize };
		*m_pStream << surfaceId;
		*m_pStream << format;
		*m_pStream << size;
		*m_pStream << (uint16_t) flags;


		if(pClut)
			*m_pStream << GfxStream::DataChunk{ 4096, pClut };

		return surfaceId;
	}

	//____ _genAlphaLayer() _________________________________________________________

	uint8_t * StreamSurface::_genAlphaLayer(const char * pSource, int pitch)
	{
		uint8_t * pAlphaLayer = new uint8_t[m_size.w*m_size.h];

		//TODO: Support more than 8 bit alpha.

		uint8_t * pDest = m_pAlphaLayer;
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

	void StreamSurface::_sendPixels(RectI rect, const uint8_t * pSource, int pitch)
	{
		*m_pStream << GfxStream::Header{ GfxChunkId::BeginSurfaceUpdate, 10 };
		*m_pStream << m_inStreamId;
		*m_pStream << rect;

		int	pixelSize = m_pixelDescription.bits / 8;
		int dataSize = rect.w * rect.h * pixelSize;

		const uint8_t * pLine = pSource;
		int ofs = 0;				// Offset in bytes within the current line.

		while( dataSize > 0 )
		{
			int chunkSize = min(dataSize, (int)(GfxStream::c_maxBlockSize - sizeof(GfxStream::Header)));
			dataSize -= chunkSize;

			*m_pStream << GfxStream::Header{ GfxChunkId::SurfaceData, chunkSize };

			while (chunkSize > 0)
			{
				int len = rect.w * pixelSize - ofs;
				if (chunkSize < len)
				{
					*m_pStream << GfxStream::DataChunk{ chunkSize, (void *) (pLine + ofs) };
					ofs += chunkSize;
					chunkSize = 0;
				}
				else
				{
					*m_pStream << GfxStream::DataChunk{ len, (void *)(pLine + ofs) };
					ofs = 0;
					chunkSize -= len;
					pLine += pitch;
				}
			}
		}

		*m_pStream << GfxStream::Header{ GfxChunkId::EndSurfaceUpdate, 0 };
	}

	//____ _sendDeleteSurface() _______________________________________________

	void StreamSurface::_sendDeleteSurface()
	{
		*m_pStream << GfxStream::Header{ GfxChunkId::DeleteSurface, 2 };
		*m_pStream << m_inStreamId;
	}

} // namespace wg
