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
#include <wg_pixeltools.h>

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

	GfxStreamSurface_p GfxStreamSurface::create(GfxStreamEncoder * pEncoder, const Blueprint& blueprint, const uint8_t* pPixels,
												const PixelDescription& pixelDescription, int pitch, const Color8 * pPalette)
	{
		return GfxStreamSurface_p(new GfxStreamSurface(pEncoder, blueprint, pPixels, pixelDescription, pitch, pPalette));
	}

	GfxStreamSurface_p GfxStreamSurface::create(GfxStreamEncoder * pEncoder, const Blueprint& blueprint, const uint8_t* pPixels,
												PixelFormat format, int pitch, const Color8 * pPalette)
	{
		return GfxStreamSurface_p(new GfxStreamSurface(pEncoder, blueprint, pPixels, format, pitch, pPalette));
	}



	//____ constructor _____________________________________________________________

	GfxStreamSurface::GfxStreamSurface( GfxStreamEncoder * pEncoder, const Blueprint& bp) : Surface(bp, pEncoder->defaultPixelFormat(), pEncoder->defaultSampleMethod() )
	{
		m_pEncoder = pEncoder;
		m_pitch = ((bp.size.w + 3) & 0xFFFFFFFC)*m_pPixelDescription->bits / 8;
		m_bDynamic = bp.dynamic;

		m_inStreamId = _sendCreateSurface(bp);

		if (m_pPixelDescription->bits <= 8 || bp.buffered)
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
			if (m_pPixelDescription->A_mask == 0)
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

		if (m_pPixelDescription->bits <= 8 || bp.buffered)
		{
			m_pBlob = pBlob;
			m_pPalette = const_cast<Color8*>(bp.palette);
			m_pAlphaLayer = nullptr;
		}
		else
		{
			if (m_pPixelDescription->A_mask == 0)
				m_pAlphaLayer = nullptr;
			else
				m_pAlphaLayer = _genAlphaLayer((char*)pBlob->data(), pitch);
		}

		_sendPixels(m_pEncoder, bp.size, (uint8_t*) pBlob->data(), pitch);
		m_pEncoder->flush();
	}

	GfxStreamSurface::GfxStreamSurface( GfxStreamEncoder * pEncoder, const Blueprint& bp, const uint8_t * pPixels, const PixelDescription& pixelDescription, int pitch, const Color8 * pPalette )
		: Surface(bp, pEncoder->defaultPixelFormat(), pEncoder->defaultSampleMethod())
	{
		m_pEncoder = pEncoder;
		m_pitch = ((bp.size.w + 3) & 0xFFFFFFFC)*m_pPixelDescription->bits / 8;
		m_bDynamic = bp.dynamic;

		m_inStreamId = _sendCreateSurface(bp);

		// We always convert the data even if we throw it away, since we need to stream the converted data.
		// (but we could optimize and skip conversion if format already is correct)

		m_pBlob = Blob::create(m_pitch*m_size.h + (bp.palette ? 1024 : 0) );

		
		int dstPaletteEntries = 256;
		int srcPitchAdd = pitch == 0 ? 0 : pitch - pixelDescription.bits/8 * m_size.w;

		PixelTools::copyPixels(m_size.w, m_size.h, pPixels, pixelDescription, srcPitchAdd,
							 (uint8_t*) m_pBlob->data(), m_pixelFormat, m_pitch - m_pPixelDescription->bits/8 * m_size.w, pPalette,
							 const_cast<Color8*>(bp.palette), 256, dstPaletteEntries, 256);
		
		_sendPixels(m_pEncoder, m_size, (uint8_t*) m_pBlob->data(), m_pitch);
		m_pEncoder->flush();

		if (m_pPixelDescription->bits <= 8 || bp.buffered)
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

			if (m_pPixelDescription->A_mask == 0)
				m_pAlphaLayer = nullptr;
			else
				m_pAlphaLayer = _genAlphaLayer((char*)pPixels, pitch);
		}
	}

	GfxStreamSurface::GfxStreamSurface( GfxStreamEncoder * pEncoder, const Blueprint& bp, const uint8_t * pPixels, PixelFormat srcFormat, int pitch, const Color8 * pPalette )
		: Surface(bp, pEncoder->defaultPixelFormat(), pEncoder->defaultSampleMethod())
	{
		m_pEncoder = pEncoder;
		m_pitch = ((bp.size.w + 3) & 0xFFFFFFFC)*m_pPixelDescription->bits / 8;
		m_bDynamic = bp.dynamic;

		m_inStreamId = _sendCreateSurface(bp);

		// We always convert the data even if we throw it away, since we need to stream the converted data.
		// (but we could optimize and skip conversion if format already is correct)

		m_pBlob = Blob::create(m_pitch*m_size.h + (bp.palette ? 1024 : 0) );

		int dstPaletteEntries = 256;
		int srcPitchAdd = pitch == 0 ? 0 : pitch - Util::pixelFormatToDescription(srcFormat).bits/8 * m_size.w;

		PixelTools::copyPixels(m_size.w, m_size.h, pPixels, srcFormat, srcPitchAdd,
							 (uint8_t*) m_pBlob->data(), m_pixelFormat, m_pitch - m_pPixelDescription->bits/8 * m_size.w, pPalette,
							 const_cast<Color8*>(bp.palette), 256, dstPaletteEntries, 256);
		
		_sendPixels(m_pEncoder, m_size, (uint8_t*) m_pBlob->data(), m_pitch);
		m_pEncoder->flush();

		if (m_pPixelDescription->bits <= 8 || bp.buffered)
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

			if (m_pPixelDescription->A_mask == 0)
				m_pAlphaLayer = nullptr;
			else
				m_pAlphaLayer = _genAlphaLayer((char*)pPixels, pitch);
		}
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
			buf.pixels = (uint8_t*) m_pBlob->data() + m_pitch * rect.y + rect.x * m_pPixelDescription->bits / 8;
			buf.palette = m_pPalette;
			buf.format = m_pixelFormat;
			buf.rect = rect;
			buf.pitch = m_pitch;
		}
		else
		{
			buf.pitch = ((rect.w + 3) & 0xFFFFFFFC) * m_pPixelDescription->bits / 8;
			buf.pixels = new uint8_t[buf.pitch*rect.h];
			buf.palette = m_pPalette;
			buf.format = m_pixelFormat;
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
		_sendPixels(m_pEncoder, buffer.rect, buffer.pixels, buffer.pitch);
		m_pEncoder->flush();
	}

	//____ freePixelBuffer() __________________________________________________

	void GfxStreamSurface::freePixelBuffer(const PixelBuffer& buffer)
	{
		if (!m_pBlob)
			delete[] buffer.pixels;
	}

	//____ alpha() ____________________________________________________________

	int GfxStreamSurface::alpha( CoordSPX coord )
	{
		PixelBuffer	buffer;

		if (m_pAlphaLayer)
		{
			buffer.format = PixelFormat::Alpha_8;
			buffer.palette = nullptr;
			buffer.pitch = m_size.w;
			buffer.pixels = m_pAlphaLayer;
			buffer.rect = { 0,0,m_size };
		}
		else
		{
			buffer.format = m_pixelFormat;
			buffer.palette = m_pPalette;
			buffer.pitch = m_pitch;
			buffer.pixels = (uint8_t*) m_pBlob->data();
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

		*m_pEncoder << GfxStream::Header{ GfxChunkId::FillSurface, GfxStream::SpxFormat::Int32_dec, 14 };
		*m_pEncoder << m_inStreamId;
		*m_pEncoder << region;
		*m_pEncoder << color;
		m_pEncoder->flush();

		// Update local copy or alpha channel (if any)

		if (m_pBlob)
		{
			PixelTools::fillBitmap((uint8_t*) m_pBlob->data(), m_pixelFormat, m_pitch, region, color);
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

		*pEncoder << GfxStream::Header{ GfxChunkId::CreateSurface, GfxStream::SpxFormat::Int32_dec, blockSize };
		*pEncoder << m_inStreamId;
		*pEncoder << m_bCanvas;
		*pEncoder << m_bDynamic;
		*pEncoder << m_pixelFormat;
		*pEncoder << m_id;
		*pEncoder << m_bMipmapped;
		*pEncoder << m_sampleMethod;
		*pEncoder << m_scale;
		*pEncoder << m_size;
		*pEncoder << m_bTiling;

		if (m_pPalette)
			*pEncoder << GfxStream::WriteBytes{ 1024, m_pPalette };

		_sendPixels(pEncoder, m_size, (uint8_t*)m_pBlob->data(), m_pitch);
		pEncoder->flush();
		return true;
	}


	//____ _sendCreateSurface() _______________________________________________

	uint16_t GfxStreamSurface::_sendCreateSurface(const Blueprint& bp)
	{
		uint16_t surfaceId = m_pEncoder->allocObjectId();

		uint16_t blockSize = 30 + (bp.palette ? 1024 : 0);

		*m_pEncoder << GfxStream::Header{ GfxChunkId::CreateSurface, GfxStream::SpxFormat::Int32_dec, blockSize };
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
			*m_pEncoder << GfxStream::WriteBytes{ 1024, bp.palette };

		return surfaceId;
	}

	//____ _genAlphaLayer() _________________________________________________________

	uint8_t * GfxStreamSurface::_genAlphaLayer(const char * pSource, int pitch)
	{
		uint8_t * pAlphaLayer = new uint8_t[m_size.w*m_size.h];

		//TODO: Support more than 8 bit alpha.

		uint8_t * pDest = pAlphaLayer;
		int pixelPitch = m_pPixelDescription->bits / 8;
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
		*pEncoder << GfxStream::Header{ GfxChunkId::BeginSurfaceUpdate, GfxStream::SpxFormat::Int32_dec, 18 };
		*pEncoder << m_inStreamId;
		*pEncoder << rect;

		int	pixelSize = m_pPixelDescription->bits / 8;
		int dataSize = rect.w * rect.h * pixelSize;

		const uint8_t * pLine = pSource;
		int ofs = 0;				// Offset in bytes within the current line.

		while( dataSize > 0 )
		{
			uint16_t chunkSize = std::min(dataSize, (int)(GfxStream::c_maxBlockSize - sizeof(GfxStream::Header)));
			dataSize -= chunkSize;

			*pEncoder << GfxStream::Header{ GfxChunkId::SurfacePixels, GfxStream::SpxFormat::Int32_dec, uint16_t((chunkSize+1)&0xFFFE) };

			while (chunkSize > 0)
			{
				int len = rect.w * pixelSize - ofs;
				if (chunkSize < len)
				{
					*pEncoder << GfxStream::WriteBytes{ chunkSize, (void *) (pLine + ofs) };
					ofs += chunkSize;
					chunkSize = 0;
				}
				else
				{
					*pEncoder << GfxStream::WriteBytes{ len, (void *)(pLine + ofs) };
					ofs = 0;
					chunkSize -= len;
					pLine += pitch;
				}
			}
			
			pEncoder->align();
		}

		*pEncoder << GfxStream::Header{ GfxChunkId::EndSurfaceUpdate, GfxStream::SpxFormat::Int32_dec, 0 };
	}

	//____ _sendDeleteSurface() _______________________________________________

	void GfxStreamSurface::_sendDeleteSurface()
	{
		*m_pEncoder << GfxStream::Header{ GfxChunkId::DeleteSurface, GfxStream::SpxFormat::Int32_dec, 2 };
		*m_pEncoder << m_inStreamId;

	}

} // namespace wg
