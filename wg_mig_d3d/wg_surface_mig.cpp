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
#include <wg_surface_mig.h>


//____ Constructor _____________________________________________________________

WgSurfaceMIG::WgSurfaceMIG( ETextureDataPtr pTexture )
{
	m_pTexture 		= pTexture;
	m_pitch			= 0;
	m_pLockedSurf	= 0;
}

//____ Destructor ______________________________________________________________

WgSurfaceMIG::~WgSurfaceMIG()
{
	if( m_pLockedSurf )
		Unlock();
}

//____ GetWidth() ______________________________________________________________

Uint32 WgSurfaceMIG::GetWidth() const
{
	return m_pTexture ? m_pTexture->GetWidth() : 0;
}

//____ GetHeight() _____________________________________________________________

Uint32 WgSurfaceMIG::GetHeight() const
{
	return m_pTexture ? m_pTexture->GetHeight() : 0;
}

//____ IsOpaque() ______________________________________________________________

bool WgSurfaceMIG::IsOpaque() const
{
	return false;	// Never assume it is opaque.
}

//____ GetPixel() ______________________________________________________________

Uint32 WgSurfaceMIG::GetPixel( Uint32 x, Uint32 y ) const
{
	Uint32 pixel = 0;
	if(x < GetWidth() && y < GetHeight())
	{
		if( m_pLockedSurf )
			pixel = * ((Uint32*) &m_pLockedSurf[m_pitch*y + 4*x]);
		else if(m_pTexture)
		{
			int iPitch = 0;
			uchar* pcData = m_pTexture->Lock( &iPitch, 0, false );
			if(pcData)
			{
				pixel = * ((Uint32*) &pcData[iPitch*y + 4*x]);
				m_pTexture->Unlock();
			}
		}
	}
	return pixel;
}

//____ GetOpacity() ____________________________________________________________

Uint8 WgSurfaceMIG::GetOpacity( Uint32 x, Uint32 y ) const
{
	Uint8 alpha = 255;
	if(x < GetWidth() && y < GetHeight())
	{
		if( m_pLockedSurf )
			alpha = m_pLockedSurf[m_pitch*y + 4*x + 3];	// Tror att den ligger där, eller + 0.
		else if(m_pTexture)
		{
			int iPitch = 0;
			uchar* pcData = m_pTexture->Lock( &iPitch, 0, false );

			if( pcData )
			{
				alpha = pcData[iPitch*y + 4*x + 3]; // Tror att den ligger där, eller + 0.
				m_pTexture->Unlock();
			}
		}
	}
	return alpha;
}

//____ Col2Pixel() _____________________________________________________________

Uint32 WgSurfaceMIG::Col2Pixel( const WgColor& col ) const
{
	// TODO: implement
	return 0;
}

//____ Pixel2Col() _____________________________________________________________

WgColor WgSurfaceMIG::Pixel2Col( Uint32 pixel ) const
{
	// TODO: implement
	return WgColor(0);
}


