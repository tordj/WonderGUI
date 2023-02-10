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
#include <wg_gfxutil.h>
#include <wg_gfxbase.h>
#include <wg_softgfxdevice.h>

namespace wg
{

	using namespace std;

	const TypeInfo SoftSurface::TYPEINFO = { "SoftSurface", &Surface::TYPEINFO };

	//____ maxSize() _______________________________________________________________

	SizeI SoftSurface::maxSize()
	{
#if WG_IS_64_BITS
		return SizeI(1048576,1048576);
#else
		return SizeI(65534, 65534);
#endif
	}

	//____ Create ______________________________________________________________

	SoftSurface_p SoftSurface::create(const Blueprint& blueprint)
	{
		return SoftSurface_p(new SoftSurface(blueprint));
	}

	SoftSurface_p SoftSurface::create(const Blueprint& blueprint, Blob* pBlob, int pitch)
	{
		return SoftSurface_p(new SoftSurface(blueprint, pBlob, pitch));
	}

	SoftSurface_p SoftSurface::create(const Blueprint& blueprint, uint8_t* pPixels, int pitch, const PixelDescription* pPixelDescription)
	{
		return SoftSurface_p(new SoftSurface(blueprint, pPixels, pitch, pPixelDescription));
	}

	SoftSurface_p SoftSurface::create(const Blueprint& blueprint, Surface* pOther)
	{
		return SoftSurface_p(new SoftSurface(blueprint, pOther));
	}

	SoftSurface_p SoftSurface::createInPlace(const Blueprint& blueprint, uint8_t* pPixels, int pitch)
	{
		return SoftSurface_p(new SoftSurface(blueprint, pPixels, pitch));
	}


	SoftSurface_p SoftSurface::create( SizeI size, PixelFormat format, int flags, const Color8 * pPalette )
	{
		if (format == PixelFormat::Undefined || format < PixelFormat_min || format > PixelFormat_max || ((format == PixelFormat::Index_8 || format == PixelFormat::Index_8_sRGB || format == PixelFormat::Index_8_linear) && pPalette == nullptr))
			return SoftSurface_p();

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

		return SoftSurface_p(new SoftSurface(bp));
	}

	SoftSurface_p SoftSurface::create( SizeI size, PixelFormat format, Blob * pBlob, int pitch, int flags, const Color8 * pPalette)
	{
		if (format == PixelFormat::Undefined || format < PixelFormat_min || format > PixelFormat_max || ((format == PixelFormat::Index_8 || format == PixelFormat::Index_8_sRGB || format == PixelFormat::Index_8_linear) && pPalette == nullptr) || !pBlob || pitch % 4 != 0 )
			return SoftSurface_p();

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

		return SoftSurface_p(new SoftSurface(bp, pBlob, pitch));
	}

	SoftSurface_p SoftSurface::create( SizeI size, PixelFormat format, uint8_t * pPixels, int pitch, const PixelDescription * pPixelDescription, int flags, const Color8 * pPalette )
	{
		if (format == PixelFormat::Undefined || format < PixelFormat_min || format > PixelFormat_max ||
			 ((format == PixelFormat::Index_8 || format == PixelFormat::Index_8_sRGB || format == PixelFormat::Index_8_linear) && pPalette == nullptr) || pPixels == nullptr || pitch <= 0 )
			return SoftSurface_p();

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

		return  SoftSurface_p(new SoftSurface( bp, pPixels, pitch, pPixelDescription ));
	};

	SoftSurface_p SoftSurface::create( Surface * pOther, int flags )
	{
		if( !pOther )
			return SoftSurface_p();

		Blueprint bp;

		bp.buffered = (flags & SurfaceFlag::Buffered);
		bp.canvas = (flags & SurfaceFlag::Canvas);
		bp.dynamic = (flags & SurfaceFlag::Dynamic);
		bp.mipmap = (flags & SurfaceFlag::Mipmapped);
		bp.scale = (flags & SurfaceFlag::Scale200) ? 128 : 64;

		if( flags & SurfaceFlag::Bilinear )
			bp.sampleMethod = SampleMethod::Bilinear;

		return SoftSurface_p(new SoftSurface( bp, pOther ));
	}

	//____ constructor ________________________________________________________________

	SoftSurface::SoftSurface( const Blueprint& bp ) : Surface(bp, PixelFormat::BGRA_8, SampleMethod::Nearest )
	{
		m_pitch = ((bp.size.w+3)&0xFFFFFFFC)*m_pixelDescription.bits/8;
		m_pBlob = Blob::create( m_pitch*bp.size.h + (bp.palette ? 1024 : 0) );
		m_pData = (uint8_t*) m_pBlob->data();

		if (bp.palette)
		{
			m_pPalette = (Color8*)(m_pData + m_pitch * bp.size.h);
			memcpy(m_pPalette, bp.palette, 1024);
			_makePalette4096();
		}
		else
			m_pPalette = nullptr;

		_initTiling();
	}

	SoftSurface::SoftSurface(const Blueprint& bp, Blob * pBlob, int pitch ) : Surface(bp, PixelFormat::BGRA_8, SampleMethod::Nearest)
	{
		m_pitch = pitch > 0 ? pitch : bp.size.w * m_pixelDescription.bits/8;
		m_pBlob = pBlob;
		m_pData = (uint8_t*) m_pBlob->data();
		m_pPalette = const_cast<Color8*>(bp.palette);
		if( m_pPalette )
			_makePalette4096();

		_initTiling();
	}

	SoftSurface::SoftSurface(const Blueprint& bp, uint8_t * pPixels, int pitch) : Surface(bp, PixelFormat::BGRA_8, SampleMethod::Nearest)
	{
		// This constructor creates the surface in place, using the pixels (and palette if present in BP) where they are.
		
		if( pitch == 0 )
			pitch = bp.size.w * m_pixelDescription.bits/8;
		
		m_pitch = pitch;
		m_pBlob = nullptr;
		m_pData = pPixels;

		if (bp.palette)
		{
			m_pPalette = const_cast<Color8*>(bp.palette);
			_makePalette4096();
		}
		else
			m_pPalette = nullptr;

		_initTiling();
	}


	SoftSurface::SoftSurface(const Blueprint& bp, uint8_t * pPixels, int pitch,
							 const PixelDescription * pPixelDescription) : Surface(bp, PixelFormat::BGRA_8, SampleMethod::Nearest)
	{
		//TODO: Convert pixeldescription to format and use that if bp.format == Undefined && pixeldesc is valid.

		if( !pPixelDescription )
			pPixelDescription = &m_pixelDescription;
		
		if( pitch == 0 )
			pitch = bp.size.w * pPixelDescription->bits/8;
		
		m_pitch = ((bp.size.w + 3) & 0xFFFFFFFC)*m_pixelDescription.bits / 8;
		m_pBlob = Blob::create(m_pitch*m_size.h + (bp.palette ? 1024 : 0) );
		m_pData = (uint8_t*)m_pBlob->data();

		_copy(bp.size, pPixelDescription, pPixels, pitch, bp.size);

		if (bp.palette)
		{
			m_pPalette = (Color8*)(m_pData + m_pitch * bp.size.h);
			memcpy(m_pPalette, bp.palette, 1024);
			_makePalette4096();
		}
		else
			m_pPalette = nullptr;

		_initTiling();
	}

	 
	SoftSurface::SoftSurface( const Blueprint& bp, Surface * pOther ) : Surface(bp, pOther->pixelFormat(), pOther->sampleMethod())
	{
		assert( pOther );

		auto pixelbuffer = pOther->allocPixelBuffer();
		bool bPushed = pOther->pushPixels(pixelbuffer);
		if (!bPushed)
		{
			//TODO: Error handling.
		}

		int pitch = pixelbuffer.pitch;
		SizeI size = pixelbuffer.rect.size();

		m_size = size;
		m_pitch = ((size.w+3)&0xFFFFFFFC)*m_pixelDescription.bits/8;
		m_pBlob = Blob::create(m_pitch*m_size.h + (pOther->palette() ? 1024 : 0) );
		m_pData = (uint8_t*) m_pBlob->data();

		_copy(RectI(size), pOther->pixelDescription(), pixelbuffer.pPixels, pitch, RectI(size) );

		if ( pOther->palette() )
		{
			m_pPalette = (Color8*)(m_pData + m_pitch * size.h);
			memcpy(m_pPalette, pOther->palette(), 1024);
			_makePalette4096();
		}
		else
			m_pPalette = nullptr;

		pOther->freePixelBuffer(pixelbuffer);

		_initTiling();
	}

	//____ Destructor ______________________________________________________________

	SoftSurface::~SoftSurface()
	{
		delete [] m_pPalette4096;
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& SoftSurface::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ alpha() _______________________________________________________________

	int SoftSurface::alpha( CoordSPX coord )
	{
		PixelBuffer	buffer;
		buffer.format = m_pixelDescription.format;
		buffer.pPalette = m_pPalette;
		buffer.pitch = m_pitch;
		buffer.pPixels = m_pData;
		buffer.rect = { 0,0,m_size };

		return _alpha(coord, buffer);
	}

	//____ _initTiling() ________________________________________________________

	void SoftSurface::_initTiling()
	{
		if (m_bTiling)
		{
			int xBits = Util::mostSignificantBit(m_size.w);
			int yBits = Util::mostSignificantBit(m_size.h);

			if (m_size.w != 1 << xBits || m_size.h != 1 << yBits)
			{
				GfxBase::handleError(ErrorSeverity::Warning, ErrorCode::FailedPrerequisite, "Surface of non-two-factor size set to tile. Tiling will happen at largest two-factor boundary.", this, &TYPEINFO, __func__, __FILE__, __LINE__);
			}

			m_srcPosMaskX = (1 << xBits) -1;
			m_srcPosMaskY = (1 << yBits) -1;
		}
		else
		{
			m_srcPosMaskX = 0;
			m_srcPosMaskY = 0;
		}
	}

	//____ allocPixelBuffer() _________________________________________________

	const PixelBuffer SoftSurface::allocPixelBuffer(const RectI& rect)
	{
		PixelBuffer	buf;
		buf.pPixels = m_pData + m_pitch*rect.y + rect.x*m_pixelDescription.bits/8;
		buf.pPalette = m_pPalette;
		buf.format = m_pixelDescription.format;
		buf.rect = rect;
		buf.pitch = m_pitch;
		return buf;
	}

	//____ pushPixels() _______________________________________________________

	bool SoftSurface::pushPixels(const PixelBuffer& buffer, const RectI& bufferRect)
	{
		return true;
	}

	//____ pullPixels() _______________________________________________________

	void SoftSurface::pullPixels(const PixelBuffer& buffer, const RectI& bufferRect)
	{
		// Nothing to do here.

		Surface::pullPixels(buffer, bufferRect);
	}

	//____ freePixelBuffer() __________________________________________________

	void SoftSurface::freePixelBuffer(const PixelBuffer& buffer)
	{
		// Nothing to do here.
	}

	//____ putPixels() _____________________________________________________________

	#define PCLIP(x,y) (((x)>(y))?(y):(x))

	void SoftSurface::putPixels(const vector<int> &x, const vector<int> &y, const vector<uint32_t> &col, int length, bool replace)
	{
		Color8 color1;
		Color8 color2;
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

	//____ _makePalette4096() ____________________________________________________

	void SoftSurface::_makePalette4096()
	{
		m_pPalette4096 = new HiColor[256];

		HiColor * p = m_pPalette4096;
		const int16_t* pUnpackTab = GfxBase::defaultToSRGB() ? HiColor::unpackSRGBTab : HiColor::unpackLinearTab;

		for (int i = 0; i < 256; i++)
		{
			p[i].r = pUnpackTab[m_pPalette[i].r];
			p[i].g = pUnpackTab[m_pPalette[i].g];
			p[i].b = pUnpackTab[m_pPalette[i].b];
			p[i].a = HiColor::unpackLinearTab[m_pPalette[i].a];
		}
	}


} // namespace wg
