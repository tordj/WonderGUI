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

#include "wg_surface_soft.h"
#include "wg_geo.h"



//____ Constructor ________________________________________________________________

WgSurfaceSoft::WgSurfaceSoft( Uint32 iWidth, Uint32 iHeight, Uint32 iBitdepth )
{
	assert( iBitdepth == 24 || iBitdepth == 32 );
	
 	m_blitMode = BM_ALPHA;

	m_size = WgSize(iWidth, iHeight);
	m_pData = new Uint8[ iWidth*iHeight*iBitdepth/8 ];
	
	for(int i = 0; i<iWidth*iHeight*iBitdepth/8;i++)
		m_pData[i] = 0;

	m_fScaleAlpha = 1.0f;
	m_pStoredAlpha = 0;

	m_lockStatus 	= UNLOCKED;
	m_pitch 		= iWidth*iBitdepth/8;
	m_pPixels 		= 0;
	m_lockRegion	= WgRect(0,0,0,0);

	switch( iBitdepth )
	{
		case 3:
			m_pixelFormat.type = WgSurface::RGB_8;
			m_pixelFormat.bits = 24;

			m_pixelFormat.R_mask = 0xFF;
			m_pixelFormat.G_mask = 0xFF00;
			m_pixelFormat.B_mask = 0xFF0000;
			m_pixelFormat.A_mask = 0x0;

			m_pixelFormat.R_shift = 0;
			m_pixelFormat.G_shift = 8;
			m_pixelFormat.B_shift = 16;
			m_pixelFormat.A_shift = 0;

			m_pixelFormat.R_bits = 8;
			m_pixelFormat.G_bits = 8;
			m_pixelFormat.B_bits = 8;
			m_pixelFormat.A_bits = 0;

			break;

		case 24:
			m_pixelFormat.type = WgSurface::RGBA_8;
			m_pixelFormat.bits = 32;

			m_pixelFormat.R_mask = 0xFF;
			m_pixelFormat.G_mask = 0xFF00;
			m_pixelFormat.B_mask = 0xFF0000;
			m_pixelFormat.A_mask = 0xFF000000;

			m_pixelFormat.R_shift = 0;
			m_pixelFormat.G_shift = 8;
			m_pixelFormat.B_shift = 16;
			m_pixelFormat.A_shift = 24;

			m_pixelFormat.R_bits = 8;
			m_pixelFormat.G_bits = 8;
			m_pixelFormat.B_bits = 8;
			m_pixelFormat.A_bits = 8;

			break;
	}
}

//____ Destructor ______________________________________________________________

WgSurfaceSoft::~WgSurfaceSoft()
{
	if(m_pStoredAlpha) 
		delete m_pStoredAlpha;

	delete m_pData;
}

//____ GetPixel() _________________________________________________________________

Uint32	WgSurfaceSoft::GetPixel( WgCoord coord ) const
{	
  if( m_bitdepth == 32 )
    {
      Uint32 k = * ((Uint32*) &m_pData[ 4*(m_size.w*coord.y+coord.x) ]);
      return k;
    }
  else 
    {
      Uint32 k = * ((Uint32*) &m_pData[ 3*(m_size.w*coord.y+coord.x) ]);
      return k;
    }

}

//____ GetOpacity() _______________________________________________________________

Uint8	WgSurfaceSoft::GetOpacity( WgCoord coord ) const
{
        WgColor color;

	if( x >= m_width )
		return 0;
	if( y >= m_height )
		return 0;
	
	if( m_bitdepth == 32 ) 
	  {
	    color.argb = *((Uint32*) &m_pData[ 4*(m_size.w*coord.y+coord.x) ]);
	    return (Uint8)(m_fScaleAlpha * (float)color.a);
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

void * WgSurfaceSoft::Lock( LockStatus mode )
{
	m_lockStatus = mode;
	m_pPixels = m_pData;
	m_lockRegion = WgRect(0,0,m_size);
}

//____ LockRegion() ____________________________________________________________

void * WgSurfaceSoft::LockRegion( LockStatus mode, const WgRect& region )
{
	m_lockStatus = mode;
	m_pPixels = m_pData + (m_size.w*region.y+region.x)*m_pixelFormat.bits/8;
	m_lockRegion = region;
}

//____ Unlock() ________________________________________________________________

void WgSurfaceSoft::Unlock()
{
	m_lockStatus = UNLOCKED;
	m_pPixels = 0;
	m_lockRegion.Clear();
}

//____ SetScaleAlpha() _________________________________________________________

void WgSurfaceSoft::setScaleAlpha(float fScaleAlpha)
{
        // This is slightly ugly and involves scaling all the alpha data, not sure if this could be done some other way / Oscar

        m_fScaleAlpha = fScaleAlpha;

	// Only valid for 32-bit bitmaps
	if( m_pixelFormat.type = RGBA_8 ) 
	{
		// If this is the first scaleAlpha change, allocate a backup copy of the alpha data for the entire surface
		if(!m_pStoredAlpha)
		{
			m_pStoredAlpha = new Uint8[m_size.w*m_size.h];
			for(int i=0; i<m_size.w*m_size.h; i++)
			{
				WgColor color;
				color.argb = ((Uint32*)m_pData)[i];
				m_pStoredAlpha[i] = color.a;
			}
		}

	  // Apply alpha scaling
		for(int j=0; j<m_size.w*m_size.h; j++)
		{
			WgColor color;
			color.argb = ((Uint32*)m_pData)[j];
			color.a = (Uint8)((float)m_pStoredAlpha[j] * m_fScaleAlpha);
			((Uint32*)m_pData)[j] = color.argb;
		}
	}
}

//____ WgSurface::setBlitMode() ___________________________________________________________

void WgSurface::setBlitMode( BlitMode mode )
{
	m_blitMode = mode;
}

//____ putPixels() _____________________________________________________________

#define PCLIP(x,y) (((x)>(y))?(y):(x))

void WgSurfaceSoft::putPixels(const vector<int> &x, const vector<int> &y, const vector<Uint32> &col, int length, bool replace)
{
	WgColor color1;
	WgColor color2;
	int ind;

	switch(m_pixelFormat.type)
	{
		case RGB_8:
			break;
		case RGBA_8:
			for(int n=0; n<length; n++)
			{
			  ind = (y[n]*m_size.w + x[n])*4;
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

