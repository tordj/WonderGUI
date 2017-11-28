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

#include <assert.h>
#include <memory.h>
#include <wg_softsurface.h>
#include <wg_util.h>

namespace wg 
{
	
	using namespace std;
	
	const char SoftSurface::CLASSNAME[] = {"SoftSurface"};
	
	//____ maxSize() _______________________________________________________________

	Size SoftSurface::maxSize()
	{
		return Size(65536,65536);
	}

	//____ Create ______________________________________________________________
	
	SoftSurface_p SoftSurface::create( Size size, PixelType type, int hint )
	{ 
		if(type != PixelType::BGRA_8 && type != PixelType::BGR_8) 
			return SoftSurface_p(); 

		return SoftSurface_p(new SoftSurface(size,type));
	}
	
	SoftSurface_p SoftSurface::create( Size size, PixelType type, Blob * pBlob, int pitch, int hint )
	{ 
		if( (type != PixelType::BGRA_8 && type != PixelType::BGR_8) || !pBlob || pitch % 4 != 0 )
			return SoftSurface_p();
		
		return SoftSurface_p(new SoftSurface(size,type,pBlob,pitch));
	}
		
	SoftSurface_p SoftSurface::create( Size size, PixelType type, uint8_t * pPixels, int pitch, const PixelFormat * pPixelFormat, int hint )
	{ 
		if( (type != PixelType::BGRA_8 && type != PixelType::BGR_8) || pPixels == 0 )
			return SoftSurface_p();

		return  SoftSurface_p(new SoftSurface(size,type,pPixels,pitch,pPixelFormat)); 
	};

	SoftSurface_p SoftSurface::create( Surface * pOther, int hint )
	{
		if( !pOther )
			return SoftSurface_p();
			
		return SoftSurface_p(new SoftSurface( pOther )); 
	}
	
	
	
	//____ Constructor ________________________________________________________________
	
	SoftSurface::SoftSurface( Size size, PixelType type )
	{
		assert( type == PixelType::BGR_8 || type == PixelType::BGRA_8 );
		Util::pixelTypeToFormat(type, m_pixelFormat);
	
		m_pitch = ((size.w+3)&0xFFFFFFFC)*m_pixelFormat.bits/8;
		m_size = size;
		m_pBlob = Blob::create( m_pitch*size.h );
		m_pData = (uint8_t*) m_pBlob->data();
		m_fScaleAlpha = 1.f;
	}
	
	SoftSurface::SoftSurface( Size size, PixelType type, Blob * pBlob, int pitch )
	{
		assert( (type == PixelType::BGR_8 || type == PixelType::BGRA_8) && pBlob && pitch % 4 == 0 );
		Util::pixelTypeToFormat(type, m_pixelFormat);

		m_pitch = pitch;
		m_size = size;
		m_pBlob = pBlob;
		m_pData = (uint8_t*) m_pBlob->data();
		m_fScaleAlpha = 1.f;
	}
	
	SoftSurface::SoftSurface( Size size, PixelType type, uint8_t * pPixels, int pitch, const PixelFormat * pPixelFormat )
	{
		assert( (type == PixelType::BGR_8 || type == PixelType::BGRA_8) && pPixels != 0 );
		Util::pixelTypeToFormat(type, m_pixelFormat);

		m_pitch = ((size.w+3)&0xFFFFFFFC)*m_pixelFormat.bits/8;
		m_size = size;
		m_pBlob = Blob::create(m_pitch*m_size.h);
		m_pData = (uint8_t*) m_pBlob->data();
		m_fScaleAlpha = 1.f;
		
		m_pPixels = m_pData;	// Simulate a lock
        _copyFrom( pPixelFormat==0 ? &m_pixelFormat:pPixelFormat, pPixels, pitch, size, size );
		m_pPixels = 0;
	}
	
	
	
	SoftSurface::SoftSurface( Surface * pOther )
	{
		assert( pOther );

		PixelType type = pOther->pixelFormat()->type;
		uint8_t * pPixels = (uint8_t*) pOther->lock( AccessMode::ReadOnly );
		int pitch = pOther->pitch();
		Size size = pOther->size();
		
		assert( type == PixelType::BGR_8 || type == PixelType::BGRA_8 );
		Util::pixelTypeToFormat(type, m_pixelFormat);
		
		m_pitch = ((size.w+3)&0xFFFFFFFC)*m_pixelFormat.bits/8;
		m_size = size;
		m_pBlob = Blob::create(m_pitch*m_size.h);
		m_pData = (uint8_t*) m_pBlob->data();
		m_fScaleAlpha = 1.f;
		
		m_pPixels = m_pData;	// Simulate a lock
		_copyFrom( &m_pixelFormat, pPixels, pitch, Rect(size), Rect(size) );
		m_pPixels = 0;

		pOther->unlock();
	}
	
	//____ Destructor ______________________________________________________________
	
	SoftSurface::~SoftSurface()
	{
	}
	
	//____ isInstanceOf() _________________________________________________________
	
	bool SoftSurface::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return Surface::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * SoftSurface::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	SoftSurface_p SoftSurface::cast( Object * pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return SoftSurface_p( static_cast<SoftSurface*>(pObject) );
	
		return 0;
	}
	
	//____ pixel() _________________________________________________________________
	
	uint32_t SoftSurface::pixel( Coord coord ) const
	{
		if( m_pixelFormat.type == PixelType::BGRA_8 )
	    {
			uint32_t k = * ((uint32_t*) &m_pData[ m_pitch*coord.y+coord.x*4 ]);
			return k;
	    }
		else
	    {
			uint8_t * pPixel = m_pData + m_pitch*coord.y + coord.x*3;

#if WG_IS_BIG_ENDIAN
			uint32_t k = pPixel[2] + (((uint32_t)pPixel[1]) << 8) + (((uint32_t)pPixel[0]) << 16);
#else
			uint32_t k = pPixel[0] + (((uint32_t)pPixel[1]) << 8) + (((uint32_t)pPixel[2]) << 16);
#endif
			return k;
	    }
	
	}
	
	//____ alpha() _______________________________________________________________
	
	uint8_t SoftSurface::alpha( Coord coord ) const
	{
		if( m_pixelFormat.type == PixelType::BGRA_8 )
		  {
			uint8_t * pPixel = m_pData + m_pitch*coord.y + coord.x*4;
		    return (uint8_t)(m_fScaleAlpha * (float)pPixel[3]);
		  }
		else
		  return 0xff;
	}
	
	
	//____ size() __________________________________________________________________
	
	Size SoftSurface::size() const
	{
		return m_size;
	}
	
	//____ isOpaque() ______________________________________________________________
	
	bool SoftSurface::isOpaque() const
	{
		return m_pixelFormat.A_bits==0?true:false;
	}
	
	//____ lock() __________________________________________________________________
	
	uint8_t * SoftSurface::lock( AccessMode mode )
	{
		m_accessMode = AccessMode::ReadWrite;
		m_pPixels = m_pData;
		m_lockRegion = Rect(0,0,m_size);
		return m_pPixels;
	}
	
	//____ lockRegion() ____________________________________________________________
	
	uint8_t * SoftSurface::lockRegion( AccessMode mode, const Rect& region )
	{
		m_accessMode = mode;
		m_pPixels = m_pData + m_pitch*region.y + region.x*m_pixelFormat.bits/8;
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
	
	
	//____ setScaleAlpha() _________________________________________________________
	
	void SoftSurface::setScaleAlpha(float fScaleAlpha)
	{
	        m_fScaleAlpha = fScaleAlpha;
	}
	
	//____ putPixels() _____________________________________________________________
	
	#define PCLIP(x,y) (((x)>(y))?(y):(x))
	
	void SoftSurface::putPixels(const vector<int> &x, const vector<int> &y, const vector<uint32_t> &col, int length, bool replace)
	{
		Color color1;
		Color color2;
		int ind;
	
		switch(m_pixelFormat.type)
		{
			case PixelType::BGR_8:
				break;
			case PixelType::BGRA_8:
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
