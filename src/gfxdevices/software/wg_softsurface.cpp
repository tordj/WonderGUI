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
	
	
	//____ Constructor ________________________________________________________________
	
	SoftSurface::SoftSurface( Size size, PixelType type )
	{
		assert( type == WG_PIXEL_RGB_8 || type == WG_PIXEL_RGBA_8 );
		WgUtil::pixelTypeToFormat(type, m_pixelFormat);
	
		m_pitch = ((size.w+3)&0xFFFFFFFC)*m_pixelFormat.bits/8;
		m_size = size;
		m_pData = new uint8_t[ m_pitch*size.h ];
		m_bOwnsData = true;
		m_fScaleAlpha = 1.f;
	}
	
	SoftSurface::SoftSurface( Size size, PixelType type, uint8_t * pPixels, int pitch, const Object_p& pFinalizer )
	{
		assert( type == WG_PIXEL_RGB_8 || type == WG_PIXEL_RGBA_8 );
		WgUtil::pixelTypeToFormat(type, m_pixelFormat);
		m_pFinalizer = pFinalizer;
		m_pitch = pitch;
		m_size = size;
		m_pData = pPixels;
		m_bOwnsData = false;
		m_fScaleAlpha = 1.f;
	}
	
	SoftSurface::SoftSurface( const SoftSurface * pOther )
	{
		_copy( pOther );
	}
	
	//____ Destructor ______________________________________________________________
	
	SoftSurface::~SoftSurface()
	{
		if(m_bOwnsData)
			delete m_pData;
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
	
	SoftSurface_p SoftSurface::cast( const Object_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return SoftSurface_p( static_cast<SoftSurface*>(pObject.rawPtr()) );
	
		return 0;
	}
	
	//____ _copy() _________________________________________________________________
	
	void SoftSurface::_copy(const SoftSurface * pOther)
	{
		m_pixelFormat 	= pOther->m_pixelFormat;
		m_pitch 		= ((pOther->m_size.w+3)&0xFFFFFFFC)*pOther->m_pixelFormat.bits/8;
		m_size 			= pOther->m_size;
		m_bOwnsData		= true;
		m_fScaleAlpha 	= pOther->m_fScaleAlpha;
	
		m_pData = new uint8_t[ m_pitch*m_size.h ];
	
		int linebytes = m_size.w * m_pixelFormat.bits/8;
		for( int y = 0 ; y < m_size.h ; y++ )
			memcpy( m_pData+y*m_pitch, pOther->m_pData+y*pOther->m_pitch, linebytes );
	}
	
	//____ pixel() _________________________________________________________________
	
	uint32_t SoftSurface::pixel( Coord coord ) const
	{
		if( m_pixelFormat.type == WG_PIXEL_RGBA_8 )
	    {
			uint32_t k = * ((uint32_t*) &m_pData[ m_pitch*coord.y+coord.x*4 ]);
			return k;
	    }
		else
	    {
			uint8_t * pPixel = m_pData + m_pitch*coord.y + coord.x*3;
	
			uint32_t k = pPixel[0] + (((uint32_t)pPixel[1]) << 8) + (((uint32_t)pPixel[2]) << 8);
			return k;
	    }
	
	}
	
	//____ alpha() _______________________________________________________________
	
	uint8_t SoftSurface::alpha( Coord coord ) const
	{
		if( m_pixelFormat.type == WG_PIXEL_RGBA_8 )
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
	
	void * SoftSurface::lock( AccessMode mode )
	{
		m_accessMode = WG_READ_WRITE;
		m_pPixels = m_pData;
		m_lockRegion = Rect(0,0,m_size);
		return m_pPixels;
	}
	
	//____ lockRegion() ____________________________________________________________
	
	void * SoftSurface::lockRegion( AccessMode mode, const Rect& region )
	{
		m_accessMode = mode;
		m_pPixels = m_pData + m_pitch*region.y + region.x*m_pixelFormat.bits/8;
		m_lockRegion = region;
		return m_pPixels;
	}
	
	//____ unlock() ________________________________________________________________
	
	void SoftSurface::unlock()
	{
		m_accessMode = WG_NO_ACCESS;
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
			case WG_PIXEL_RGB_8:
				break;
			case WG_PIXEL_RGBA_8:
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
