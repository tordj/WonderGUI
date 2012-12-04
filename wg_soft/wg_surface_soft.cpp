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
#include <wg_surface_soft.h>
#include <wg_util.h>


using namespace std;

static const char	c_surfaceType[] = {"Software"};

//____ Constructor ________________________________________________________________

WgSurfaceSoft::WgSurfaceSoft( WgSize size, WgPixelType type )
{
	assert( type == WG_PIXEL_RGB_8 || type == WG_PIXEL_ARGB_8 );
	WgUtil::PixelTypeToFormat(type, m_pixelFormat);

	m_pitch = ((size.w+3)&0xFFFFFFFC)*m_pixelFormat.bits/8;
	m_size = size;
	m_pData = new Uint8[ m_pitch*size.h ];
	m_bOwnsData = true;
	m_fScaleAlpha = 1.f;
}

WgSurfaceSoft::WgSurfaceSoft( WgSize size, WgPixelType type, Uint8 * pPixels, int pitch )
{
	assert( type == WG_PIXEL_RGB_8 || type == WG_PIXEL_ARGB_8 );
	WgUtil::PixelTypeToFormat(type, m_pixelFormat);

	m_pitch = pitch;
	m_size = size;
	m_pData = pPixels;
	m_bOwnsData = false;
	m_fScaleAlpha = 1.f;
}

WgSurfaceSoft::WgSurfaceSoft( const WgSurfaceSoft * pOther )
{
	_copy( pOther );
}

//____ Destructor ______________________________________________________________

WgSurfaceSoft::~WgSurfaceSoft()
{
	if(m_bOwnsData)
		delete m_pData;
}

//____ Type() __________________________________________________________________

const char * WgSurfaceSoft::Type() const
{
	return GetClass();
}

//____ GetClass() _____________________________________________________________

const char * WgSurfaceSoft::GetClass()
{
	return c_surfaceType;
}


//____ _copy() _________________________________________________________________

void WgSurfaceSoft::_copy(const WgSurfaceSoft * pOther)
{
	m_pixelFormat 	= pOther->m_pixelFormat;
	m_pitch 		= ((pOther->m_size.w+3)&0xFFFFFFFC)*pOther->m_pixelFormat.bits/8;
	m_size 			= pOther->m_size;
	m_bOwnsData		= true;
	m_fScaleAlpha 	= pOther->m_fScaleAlpha;

	m_pData = new Uint8[ m_pitch*m_size.h ];

	int linebytes = m_size.w * m_pixelFormat.bits/8;
	for( int y = 0 ; y < m_size.h ; y++ )
		memcpy( m_pData+y*m_pitch, pOther->m_pData+y*pOther->m_pitch, linebytes );
}

//____ GetPixel() _________________________________________________________________

Uint32 WgSurfaceSoft::GetPixel( WgCoord coord ) const
{
	if( coord.x >= m_size.w || coord.x < 0 ||
		coord.y >= m_size.h || coord.y < 0  )
		return 0;

	if( m_pixelFormat.type == WG_PIXEL_ARGB_8 )
    {
		Uint32 k = * ((Uint32*) &m_pData[ m_pitch*coord.y+coord.x*4 ]);
		return k;
    }
	else
    {
		Uint8 * pPixel = m_pData + m_pitch*coord.y + coord.x*3;

		Uint32 k = pPixel[0] + (((Uint32)pPixel[1]) << 8) + (((Uint32)pPixel[2]) << 8);
		return k;
    }

}

//____ GetOpacity() _______________________________________________________________

Uint8 WgSurfaceSoft::GetOpacity( WgCoord coord ) const
{
	if( coord.x >= m_size.w || coord.x < 0 ||
		coord.y >= m_size.h || coord.y < 0  )
		return 0;

	if( m_pixelFormat.type == WG_PIXEL_ARGB_8 )
	  {
		Uint8 * pPixel = m_pData + m_pitch*coord.y + coord.x*4;
	    return (Uint8)(m_fScaleAlpha * (float)pPixel[3]);
	  }
	else
	  return 0xff;
}

//____ Size() __________________________________________________________________

WgSize WgSurfaceSoft::Size() const
{
	return m_size;
}

//____ IsOpaque() ______________________________________________________________

bool WgSurfaceSoft::IsOpaque() const
{
	return m_pixelFormat.A_bits==0?true:false;
}

//____ Lock() __________________________________________________________________

void * WgSurfaceSoft::Lock( WgAccessMode mode )
{
	m_accessMode = WG_READ_WRITE;
	m_pPixels = m_pData;
	m_lockRegion = WgRect(0,0,m_size);
	return m_pPixels;
}

//____ LockRegion() ____________________________________________________________

void * WgSurfaceSoft::LockRegion( WgAccessMode mode, const WgRect& region )
{
	m_accessMode = mode;
	m_pPixels = m_pData + m_pitch*region.y + region.x*m_pixelFormat.bits/8;
	m_lockRegion = region;
	return m_pPixels;
}

//____ Unlock() ________________________________________________________________

void WgSurfaceSoft::Unlock()
{
	m_accessMode = WG_NO_ACCESS;
	m_pPixels = 0;
	m_lockRegion.Clear();
}

//____ SetScaleAlpha() _________________________________________________________

void WgSurfaceSoft::SetScaleAlpha(float fScaleAlpha)
{
        m_fScaleAlpha = fScaleAlpha;
}

//____ PutPixels() _____________________________________________________________

#define PCLIP(x,y) (((x)>(y))?(y):(x))

void WgSurfaceSoft::PutPixels(const vector<int> &x, const vector<int> &y, const vector<Uint32> &col, int length, bool replace)
{
	WgColor color1;
	WgColor color2;
	int ind;

	switch(m_pixelFormat.type)
	{
		case WG_PIXEL_RGB_8:
			break;
		case WG_PIXEL_ARGB_8:
			for(int n=0; n<length; n++)
			{
			  ind = y[n]*m_pitch + x[n]*4;
			  if(!replace) {
				// Get old (1) and new (2) pixel
				color1.argb = *((Uint32*)&m_pData[ind]);
				color2.argb = col[n];
				// Blend
				color1.r = (Uint8)(PCLIP((int)color1.r + (int)color2.r,0xFF));
				color1.g = (Uint8)(PCLIP((int)color1.g + (int)color2.g,0xFF));
				color1.b = (Uint8)(PCLIP((int)color1.b + (int)color2.b,0xFF));
				color1.a = (Uint8)(PCLIP((int)color1.a + (int)color2.a,0xFF));
				// Store
				*((Uint32*)&m_pData[ind]) = color1.argb;
			  } else {
				// Overwrite old pixel with new pixel
				*((Uint32*)&m_pData[ind]) = col[n];
			  }
			}
			break;
		default:
			break;
    }
}

//____ WgSurfaceFactorySoft::CreateSurface() ___________________________________

WgSurface * WgSurfaceFactorySoft::CreateSurface( const WgSize& size, WgPixelType type ) const
{
	return new WgSurfaceSoft( size, type );
}
