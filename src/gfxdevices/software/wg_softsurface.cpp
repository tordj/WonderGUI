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

#include <cassert>
#include <memory.h>
#include <wg_softsurface.h>
#include <wg_util.h>

namespace wg
{

	using namespace std;

	const TypeInfo SoftSurface::TYPEINFO = { "SoftSurface", &Surface::TYPEINFO };

	//____ maxSize() _______________________________________________________________

	SizeI SoftSurface::maxSize()
	{
		return SizeI(65536,65536);
	}

	//____ Create ______________________________________________________________

	SoftSurface_p SoftSurface::create( SizeI size, PixelFormat format, int flags, const Color * pClut )
	{
		if (format == PixelFormat::Unknown || format == PixelFormat::Custom || format < PixelFormat_min || format > PixelFormat_max || ((format == PixelFormat::CLUT_8 || format == PixelFormat::CLUT_8_sRGB || format == PixelFormat::CLUT_8_linear) && pClut == nullptr) )
			return SoftSurface_p();

		return SoftSurface_p(new SoftSurface(size,format,flags,pClut));
	}

	SoftSurface_p SoftSurface::create( SizeI size, PixelFormat format, Blob * pBlob, int pitch, int flags, const Color * pClut)
	{
		if (format == PixelFormat::Unknown || format == PixelFormat::Custom || format < PixelFormat_min || format > PixelFormat_max || ((format == PixelFormat::CLUT_8 || format == PixelFormat::CLUT_8_sRGB || format == PixelFormat::CLUT_8_linear) && pClut == nullptr) || !pBlob || pitch % 4 != 0 )
			return SoftSurface_p();

		return SoftSurface_p(new SoftSurface(size,format,pBlob,pitch,flags,pClut));
	}

	SoftSurface_p SoftSurface::create( SizeI size, PixelFormat format, uint8_t * pPixels, int pitch, const PixelDescription * pPixelDescription, int flags, const Color * pClut )
	{
		if (format == PixelFormat::Unknown || format == PixelFormat::Custom || format < PixelFormat_min || format > PixelFormat_max ||
			 ((format == PixelFormat::CLUT_8 || format == PixelFormat::CLUT_8_sRGB || format == PixelFormat::CLUT_8_linear) && pClut == nullptr) || pPixels == nullptr || pitch <= 0 )
			return SoftSurface_p();

		return  SoftSurface_p(new SoftSurface( size, format, pPixels, pitch, pPixelDescription, flags, pClut ));
	};

	SoftSurface_p SoftSurface::create( Surface * pOther, int flags )
	{
		if( !pOther )
			return SoftSurface_p();

		return SoftSurface_p(new SoftSurface( pOther, flags ));
	}



	//____ constructor ________________________________________________________________

	SoftSurface::SoftSurface( SizeI size, PixelFormat format, int flags, const Color * pClut ) : Surface(flags)
	{
		assert( format != PixelFormat::Unknown && format != PixelFormat::Custom );
		Util::pixelFormatToDescription(format, m_pixelDescription);

		m_pitch = ((size.w+3)&0xFFFFFFFC)*m_pixelDescription.bits/8;
		m_size = size;
		m_pBlob = Blob::create( m_pitch*size.h + (pClut ? 1024 : 0) );
		m_pData = (uint8_t*) m_pBlob->data();

		if (pClut)
		{
			m_pClut = (Color*)(m_pData + m_pitch * size.h);
			memcpy(m_pClut, pClut, 1024);
		}
		else
			m_pClut = nullptr;
	}

	SoftSurface::SoftSurface( SizeI size, PixelFormat format, Blob * pBlob, int pitch, int flags, const Color * pClut ) : Surface(flags)
	{
		assert(format != PixelFormat::Unknown && format != PixelFormat::Custom && pBlob );
		Util::pixelFormatToDescription(format, m_pixelDescription);

		m_pitch = pitch;
		m_size = size;
		m_pBlob = pBlob;
		m_pData = (uint8_t*) m_pBlob->data();
		m_pClut = const_cast<Color*>(pClut);
	}

	SoftSurface::SoftSurface(SizeI size, PixelFormat format, uint8_t * pPixels, int pitch,
							 const PixelDescription * pPixelDescription, int flags, const Color * pClut)
	: Surface(flags)
	{
		Util::pixelFormatToDescription(format, m_pixelDescription);

		m_pitch = ((size.w + 3) & 0xFFFFFFFC)*m_pixelDescription.bits / 8;
		m_size = size;
		m_pBlob = Blob::create(m_pitch*m_size.h + (pClut ? 1024 : 0) );
		m_pData = (uint8_t*)m_pBlob->data();

		m_pPixels = m_pData;	// Simulate a lock
		_copyFrom(pPixelDescription == 0 ? &m_pixelDescription : pPixelDescription, pPixels, pitch, size, size);
		m_pPixels = 0;

		if (pClut)
		{
			m_pClut = (Color*)(m_pData + m_pitch * size.h);
			memcpy(m_pClut, pClut, 1024);
		}
		else
			m_pClut = nullptr;
	}


	SoftSurface::SoftSurface( Surface * pOther, int flags ) : Surface(flags)
	{
		assert( pOther );

		PixelFormat format = pOther->pixelFormat();
		uint8_t * pPixels = (uint8_t*) pOther->lock( AccessMode::ReadOnly );
		int pitch = pOther->pitch();
		SizeI size = pOther->size();

		Util::pixelFormatToDescription(format, m_pixelDescription);

		m_pitch = ((size.w+3)&0xFFFFFFFC)*m_pixelDescription.bits/8;
		m_size = size;
		m_pBlob = Blob::create(m_pitch*m_size.h + (pOther->clut() ? 1024 : 0) );
		m_pData = (uint8_t*) m_pBlob->data();

		m_pPixels = m_pData;	// Simulate a lock
		_copyFrom( &m_pixelDescription, pPixels, pitch, RectI(size), RectI(size) );
		m_pPixels = 0;

		if ( pOther->clut() )
		{
			m_pClut = (Color*)(m_pData + m_pitch * size.h);
			memcpy(m_pClut, pOther->clut(), 1024);
		}
		else
			m_pClut = nullptr;

		pOther->unlock();
	}

	//____ Destructor ______________________________________________________________

	SoftSurface::~SoftSurface()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& SoftSurface::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ pixel() _________________________________________________________________

	uint32_t SoftSurface::pixel( CoordI coord ) const
	{
		//TODO: Take endianess into account.

		switch (m_pixelDescription.bits)
		{
			case 8:
			{
				uint32_t k = (uint32_t)m_pData[m_pitch*coord.y + coord.x];
				return k;
			}
			case 16:
			{
				uint8_t * pPixel = m_pData + m_pitch * coord.y + coord.x * 2;
				uint32_t k = pPixel[0] + (((uint32_t)pPixel[1]) << 8);
				return k;
			}
			case 24:
			{
				uint8_t * pPixel = m_pData + m_pitch * coord.y + coord.x * 3;
				uint32_t k = pPixel[0] + (((uint32_t)pPixel[1]) << 8) + (((uint32_t)pPixel[2]) << 16);
				return k;
			}
			case 32:
			{
				uint32_t k = *((uint32_t*)&m_pData[m_pitch*coord.y + coord.x * 4]);
				return k;
			}
			default:
				return 0;
		}
	}

	//____ alpha() _______________________________________________________________

	uint8_t SoftSurface::alpha( CoordI coord ) const
	{
		//TODO: Take endianess into account.

		switch (m_pixelDescription.format)
		{
			case PixelFormat::CLUT_8_sRGB:
			case PixelFormat::CLUT_8_linear:
			{
				uint8_t index = m_pData[m_pitch * coord.y + coord.x];
				return m_pClut[index].a;
			}
			case PixelFormat::A_8:
			{
				uint8_t * pPixel = m_pData + m_pitch * coord.y + coord.x;
				return pPixel[0];
			}
			case PixelFormat::BGRA_4_linear:
			{
				uint16_t pixel = * (uint16_t *)(m_pData + m_pitch * coord.y + coord.x);
				const uint8_t * pConvTab = s_pixelConvTabs[4];

				return ((pConvTab[(pixel & m_pixelDescription.A_mask) >> m_pixelDescription.A_shift] >> m_pixelDescription.A_loss) << m_pixelDescription.A_shift);
			}
			case PixelFormat::BGRA_8_sRGB:
			case PixelFormat::BGRA_8_linear:
			{
				uint8_t * pPixel = m_pData + m_pitch * coord.y + coord.x * 4;
				return pPixel[3];
			}
			case PixelFormat::Custom:
			{
				//TODO: Implement!
			}
			default:
				return 0xFF;
		}
	}

	//____ isOpaque() ______________________________________________________________

	bool SoftSurface::isOpaque() const
	{
		return m_pixelDescription.A_bits==0?true:false;
	}

	//____ lock() __________________________________________________________________

	uint8_t * SoftSurface::lock( AccessMode mode )
	{
		m_accessMode = AccessMode::ReadWrite;
		m_pPixels = m_pData;
		m_lockRegion = RectI(0,0,m_size);
		return m_pPixels;
	}

	//____ lockRegion() ____________________________________________________________

	uint8_t * SoftSurface::lockRegion( AccessMode mode, const RectI& region )
	{
		m_accessMode = mode;
		m_pPixels = m_pData + m_pitch*region.y + region.x*m_pixelDescription.bits/8;
		m_lockRegion = region;
		return m_pPixels;
	}

	//____ unlock() ________________________________________________________________

	void SoftSurface::unlock()
	{
		m_accessMode = AccessMode::None;
		m_pPixels = 0;
		m_lockRegion.clear();
	}

	//____ putPixels() _____________________________________________________________

	#define PCLIP(x,y) (((x)>(y))?(y):(x))

	void SoftSurface::putPixels(const vector<int> &x, const vector<int> &y, const vector<uint32_t> &col, int length, bool replace)
	{
		Color color1;
		Color color2;
		int ind;

		switch(m_pixelDescription.format)
		{
			case PixelFormat::BGR_8:
				break;
			case PixelFormat::BGRA_8:
				for(int n=0; n<length; n++)
				{
				  ind = y[n]*m_pitch + x[n]*4;
				  if(!replace) {
					// Get old (1) and new (2) pixel
					color1.argb = *((uint32_t*)&m_pData[ind]);
					color2.argb = col[n];
					// Blend
					color1.r = (uint8_t)(PCLIP((int)color1.r + (int)color2.r,0xFF));
					color1.g = (uint8_t)(PCLIP((int)color1.g + (int)color2.g,0xFF));
					color1.b = (uint8_t)(PCLIP((int)color1.b + (int)color2.b,0xFF));
					color1.a = (uint8_t)(PCLIP((int)color1.a + (int)color2.a,0xFF));
					// Store
					*((uint32_t*)&m_pData[ind]) = color1.argb;
				  } else {
					// Overwrite old pixel with new pixel
					*((uint32_t*)&m_pData[ind]) = col[n];
				  }
				}
				break;
			default:
				break;
		}
	}

} // namespace wg
