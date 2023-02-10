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

#include <wg_gfxstreamsurface.h>
#include <wg_gfxutil.h>
#include <wg_blob.h>
#include <assert.h>

#include <cstring>
#include <algorithm>



namespace wg
{
	const TypeInfo GfxStreamSurface::TYPEINFO = { "GfxStreamSurface", &Surface::TYPEINFO };

	//____ maxSize() _______________________________________________________________

	SizeI GfxStreamSurface::maxSize()
	{
		return SizeI(4096,4096);
	}

	//____ create ______________________________________________________________

	GfxStreamSurface_p GfxStreamSurface::create(GfxStreamEncoder * pEncoder, const Blueprint& blueprint)
	{
		return GfxStreamSurface_p(new GfxStreamSurface(pEncoder, blueprint));
	}

	GfxStreamSurface_p GfxStreamSurface::create(GfxStreamEncoder * pEncoder, const Blueprint& blueprint, Blob* pBlob, int pitch)
	{
		return GfxStreamSurface_p(new GfxStreamSurface(pEncoder, blueprint, pBlob, pitch));
	}

	GfxStreamSurface_p GfxStreamSurface::create(GfxStreamEncoder * pEncoder, const Blueprint& blueprint, uint8_t* pPixels, int pitch, const PixelDescription* pPixelDescription)
	{
		return GfxStreamSurface_p(new GfxStreamSurface(pEncoder, blueprint, pPixels, pitch, pPixelDescription));
	}

	GfxStreamSurface_p GfxStreamSurface::create(GfxStreamEncoder * pEncoder, const Blueprint& blueprint, Surface* pOther)
	{
		return GfxStreamSurface_p(new GfxStreamSurface(pEncoder, blueprint, pOther));
	}


	GfxStreamSurface_p	GfxStreamSurface::create( GfxStreamEncoder * pEncoder, SizeI size, PixelFormat format, int flags, const Color8 * pPalette )
	{
		if (format == PixelFormat::Undefined || format < PixelFormat_min || format > PixelFormat_max || ((format == PixelFormat::Index_8 || format == PixelFormat::Index_8_sRGB || format == PixelFormat::Index_8_linear) && pPalette == nullptr))
			return GfxStreamSurface_p();

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

		return GfxStreamSurface_p(new GfxStreamSurface(pEncoder, bp));
	}

	GfxStreamSurface_p	GfxStreamSurface::create( GfxStreamEncoder * pEncoder, SizeI size, PixelFormat format, Blob * pBlob, int pitch, int flags, const Color8 * pPalette )
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

		return GfxStreamSurface_p(new GfxStreamSurface(pEncoder, bp, pBlob, pitch));
	}

	GfxStreamSurface_p	GfxStreamSurface::create( GfxStreamEncoder * pEncoder,SizeI size, PixelFormat format, uint8_t * pPixels, int pitch, const PixelDescription * pPixelDescription, int flags, const Color8 * pPalette )
	{
		if (format == PixelFormat::Undefined || format < PixelFormat_min || format > PixelFormat_max ||
			((format == PixelFormat::Index_8 || format == PixelFormat::Index_8_sRGB || format == PixelFormat::Index_8_linear) && pPalette == nullptr) || pPixels == nullptr || pitch <= 0 || pPixelDescription == nullptr)
			return GfxStreamSurface_p();

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

		return  GfxStreamSurface_p(new GfxStreamSurface(pEncoder, bp, pPixels, pitch, pPixelDescription));
	};

	GfxStreamSurface_p	GfxStreamSurface::create( GfxStreamEncoder * pEncoder, Surface * pOther, int flags )
	{
		if (!pOther)
			return GfxStreamSurface_p();

		Blueprint bp;

		bp.buffered = (flags & SurfaceFlag::Buffered);
		bp.canvas = (flags & SurfaceFlag::Canvas);
		bp.dynamic = (flags & SurfaceFlag::Dynamic);
		bp.mipmap = (flags & SurfaceFlag::Mipmapped);
		bp.scale = (flags & SurfaceFlag::Scale200) ? 128 : 64;

		if( flags & SurfaceFlag::Bilinear )
			bp.sampleMethod = SampleMethod::Bilinear;

		return GfxStreamSurface_p(new GfxStreamSurface(pEncoder, bp, pOther));
	}



	//____ constructor _____________________________________________________________

	GfxStreamSurface::GfxStreamSurface( GfxStreamEncoder * pEncoder, const Blueprint& bp) : Surface(bp, pEncoder->defaultPixelFormat(), pEncoder->defaultSampleMethod() )
	{
		m_pEncoder = pEncoder;
		m_pitch = ((bp.size.w + 3) & 0xFFFFFFFC)*m_pixelDescription.bits / 8;
		m_bDynamic = bp.dynamic;

		m_inStreamId = _sendCreateSurface(bp);

		if (m_pixelDescription.bits <= 8 || bp.buffered)
		{
			m_pBlob = Blob::create(m_pitch*bp.size.h + (bp.palette ? 1024 : 0) );
			std::memset(m_pBlob->data(), 0, m_pitch*bp.size.h);

			if (bp.palette)
			{
				m_pPalette = (Color8*)((uint8_t*)m_pBlob->data() + m_pitch * bp.size.h);
				memcpy(m_pPalette, bp.palette, 1024);
			}
			else
				m_pPalette = nullptr;

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
		m_pEncoder->flush();
	}

	GfxStreamSurface::GfxStreamSurface( GfxStreamEncoder * pEncoder, const Blueprint& bp, Blob* pBlob, int pitch )
		: Surface(bp, pEncoder->defaultPixelFormat(), pEncoder->defaultSampleMethod())
	{
		m_pEncoder = pEncoder;
		m_pitch = pitch;
		m_bDynamic = bp.dynamic;

		m_inStreamId = _sendCreateSurface(bp);

		if (m_pixelDescription.bits <= 8 || bp.buffered)
		{
			m_pBlob = pBlob;
			m_pPalette = const_cast<Color8*>(bp.palette);
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

	GfxStreamSurface::GfxStreamSurface( GfxStreamEncoder * pEncoder, const Blueprint& bp, uint8_t * pPixels, int pitch, const PixelDescription * pPixelDescription )
		: Surface(bp, pEncoder->defaultPixelFormat(), pEncoder->defaultSampleMethod())
	{
		m_pEncoder = pEncoder;
		m_pitch = ((bp.size.w + 3) & 0xFFFFFFFC)*m_pixelDescription.bits / 8;
		m_bDynamic = bp.dynamic;

		m_inStreamId = _sendCreateSurface(bp);

		// We always convert the data even if we throw it away, since we need to stream the converted data.
		// (but we could optimize and skip conversion if format already is correct)

		m_pBlob = Blob::create(m_pitch*m_size.h + (bp.palette ? 1024 : 0) );

		_copy(bp.size, pPixelDescription == 0 ? &m_pixelDescription : pPixelDescription, pPixels, pitch, bp.size);

		// No _sendPixels() needed here, _copyFrom() calls pullPixels() which calls _sendPixels().

		if (m_pixelDescription.bits <= 8 || bp.buffered)
		{
			if (bp.palette)
			{
				m_pPalette = (Color8*)((uint8_t*)m_pBlob->data() + m_pitch * bp.size.h);
				memcpy(m_pPalette, bp.palette, 1024);
			}
			else
				m_pPalette = nullptr;

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


	GfxStreamSurface::GfxStreamSurface(GfxStreamEncoder * pEncoder, const Blueprint& bp, Surface* pOther)
		: Surface(bp, pOther->pixelFormat(), pOther->sampleMethod())
	{
		//TODO: This only works now if blueprint and pOther agrees on size, etc.

		assert(pOther);

		auto pixelbuffer = pOther->allocPixelBuffer();
		bool bPushed = pOther->pushPixels(pixelbuffer);

		//TODO: Fail in a good way (return nullptr from create()) if we can't push pixels from pOther.

		int pitch = pixelbuffer.pitch;
		SizeI size = pixelbuffer.rect.size();

		m_pEncoder = pEncoder;
		m_size = size;
		m_pitch = ((size.w + 3) & 0xFFFFFFFC)*m_pixelDescription.bits / 8;
		m_bDynamic = bp.dynamic;

		Blueprint myBP = bp;
		myBP.size = m_size;
		myBP.sampleMethod = m_sampleMethod;
		myBP.format = m_pixelDescription.format;

        m_inStreamId = _sendCreateSurface(myBP);

		m_pBlob = Blob::create(m_pitch*m_size.h + (pOther->palette() ? 1024 : 0));

		// _copy() implicitly calls _sendPixels().
		_copy(RectI(size), pOther->pixelDescription(), pixelbuffer.pPixels, pitch, RectI(size));
		
		if (m_pixelDescription.bits <= 8 || bp.buffered)
		{
			if (bp.palette)
			{
				m_pPalette = (Color8*)((uint8_t*)m_pBlob->data() + m_pitch * bp.size.h);
				memcpy(m_pPalette, bp.palette, 1024);
			}
			else
				m_pPalette = nullptr;

			m_pAlphaLayer = nullptr;
		}
		else
		{
			m_pBlob = nullptr;

			if (m_pixelDescription.A_bits == 0)
				m_pAlphaLayer = nullptr;
			else
				m_pAlphaLayer = _genAlphaLayer((char*)pixelbuffer.pPixels, pitch);
		}

		pOther->freePixelBuffer(pixelbuffer);
		m_pEncoder->flush();
	}


	//____ Destructor ______________________________________________________________

	GfxStreamSurface::~GfxStreamSurface()
	{
		_sendDeleteSurface();
		m_pEncoder->freeObjectId(m_inStreamId);
		m_pEncoder->flush();
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& GfxStreamSurface::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ allocPixelBuffer() _________________________________________________

	const PixelBuffer GfxStreamSurface::allocPixelBuffer(const RectI& rect)
	{
		PixelBuffer	buf;

		if (m_pBlob)
		{
			buf.pPixels = (uint8_t*) m_pBlob->data() + m_pitch * rect.y + rect.x * m_pixelDescription.bits / 8;
			buf.pPalette = m_pPalette;
			buf.format = m_pixelDescription.format;
			buf.rect = rect;
			buf.pitch = m_pitch;
		}
		else
		{
			buf.pitch = ((rect.w + 3) & 0xFFFFFFFC) * m_pixelDescription.bits / 8;
			buf.pPixels = new uint8_t[buf.pitch*rect.h];
			buf.pPalette = m_pPalette;
			buf.format = m_pixelDescription.format;
			buf.rect = rect;
		}

		return buf;
	}

	//____ pushPixels() _______________________________________________________

	bool GfxStreamSurface::pushPixels(const PixelBuffer& buffer, const RectI& bufferRect)
	{
		if (m_pBlob)
			return true;
		else
			return false;
	}

	//____ pullPixels() _______________________________________________________

	void GfxStreamSurface::pullPixels(const PixelBuffer& buffer, const RectI& bufferRect)
	{
		_sendPixels(m_pEncoder, buffer.rect, buffer.pPixels, buffer.pitch);
		m_pEncoder->flush();
	}

	//____ freePixelBuffer() __________________________________________________

	void GfxStreamSurface::freePixelBuffer(const PixelBuffer& buffer)
	{
		if (!m_pBlob)
			delete[] buffer.pPixels;
	}

	//____ alpha() ____________________________________________________________

	int GfxStreamSurface::alpha( CoordSPX coord )
	{
		PixelBuffer	buffer;

		if (m_pAlphaLayer)
		{
			buffer.format = PixelFormat::Alpha_8;
			buffer.pPalette = nullptr;
			buffer.pitch = m_size.w;
			buffer.pPixels = m_pAlphaLayer;
			buffer.rect = { 0,0,m_size };
		}
		else
		{
			buffer.format = m_pixelDescription.format;
			buffer.pPalette = m_pPalette;
			buffer.pitch = m_pitch;
			buffer.pPixels = (uint8_t*) m_pBlob->data();
			buffer.rect = { 0,0,m_size };
		}

		return _alpha(coord, buffer);
	}

	//____ fill() _____________________________________________________________

	bool GfxStreamSurface::fill(HiColor color)
	{
		return fill(RectI(0, 0, pixelSize()), color );
	}

	bool GfxStreamSurface::fill(const RectI& region, HiColor color )
	{
		// Stream the call

		*m_pEncoder << GfxStream::Header{ GfxChunkId::FillSurface, 0, 14 };
		*m_pEncoder << m_inStreamId;
		*m_pEncoder << region;
		*m_pEncoder << color;
		m_pEncoder->flush();

		// Update local copy or alpha channel (if any)

		if (m_pBlob)
		{
			uint32_t pixel = colorToPixel(color);
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
				m_pAlphaLayer[i] = color.a;
		}

		//
		return true;
	}

	//____ streamAsNew() ______________________________________________________

	bool GfxStreamSurface::streamAsNew(GfxStreamEncoder* pEncoder)
	{
		if (!m_pBlob)
			return false;

		uint16_t blockSize = 30 + (m_pPalette ? 1024 : 0);

		*pEncoder << GfxStream::Header{ GfxChunkId::CreateSurface, 0, blockSize };
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

		if (m_pPalette)
			*pEncoder << GfxStream::DataChunk{ 1024, m_pPalette };

		_sendPixels(pEncoder, m_size, (uint8_t*)m_pBlob->data(), m_pitch);
		pEncoder->flush();
		return true;
	}


	//____ _sendCreateSurface() _______________________________________________

	uint16_t GfxStreamSurface::_sendCreateSurface(const Blueprint& bp)
	{
		uint16_t surfaceId = m_pEncoder->allocObjectId();

		uint16_t blockSize = 30 + (bp.palette ? 1024 : 0);

		*m_pEncoder << GfxStream::Header{ GfxChunkId::CreateSurface, 0, blockSize };
		*m_pEncoder << surfaceId;
		*m_pEncoder << bp.canvas;
		*m_pEncoder << bp.dynamic;
		*m_pEncoder << bp.format;
		*m_pEncoder << bp.identity;
		*m_pEncoder << bp.mipmap;
		*m_pEncoder << bp.sampleMethod;
		*m_pEncoder << bp.scale;
		*m_pEncoder << bp.size;
		*m_pEncoder << bp.tiling;

		if(bp.palette)
			*m_pEncoder << GfxStream::DataChunk{ 1024, bp.palette };

		return surfaceId;
	}

	//____ _genAlphaLayer() _________________________________________________________

	uint8_t * GfxStreamSurface::_genAlphaLayer(const char * pSource, int pitch)
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

	void GfxStreamSurface::_sendPixels(GfxStreamEncoder* pEncoder, RectI rect, const uint8_t * pSource, int pitch)
	{
		*pEncoder << GfxStream::Header{ GfxChunkId::BeginSurfaceUpdate, 0, 18 };
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

			*pEncoder << GfxStream::Header{ GfxChunkId::SurfacePixels, 0, uint16_t((chunkSize+1)&0xFFFE) };

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
			
			pEncoder->align();
		}

		*pEncoder << GfxStream::Header{ GfxChunkId::EndSurfaceUpdate, 0, 0 };
	}

	//____ _sendDeleteSurface() _______________________________________________

	void GfxStreamSurface::_sendDeleteSurface()
	{
		*m_pEncoder << GfxStream::Header{ GfxChunkId::DeleteSurface, 0, 2 };
		*m_pEncoder << m_inStreamId;

	}

} // namespace wg
