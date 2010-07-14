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

	SetPixelFormat( pTexture );
}

//____ SetPixelFormat() _______________________________________________________

void WgSurfaceMIG::SetPixelFormat( ETextureDataPtr pTexture )
{
	EPixelFormat format = pTexture->GetPixelFormat();

	switch( format )
	{
	case EPixelFormat::eRGBA32:

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

	case EPixelFormat::eRGB24:
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

	default:
		m_pixelFormat.type = WgSurface::UNSPECIFIED;
		m_pixelFormat.bits = 0;

		m_pixelFormat.R_mask = 0;
		m_pixelFormat.G_mask = 0;
		m_pixelFormat.B_mask = 0;
		m_pixelFormat.A_mask = 0;
		
		m_pixelFormat.R_shift = 0;
		m_pixelFormat.G_shift = 0;
		m_pixelFormat.B_shift = 0;
		m_pixelFormat.A_shift = 0;

		m_pixelFormat.R_bits = 0;
		m_pixelFormat.G_bits = 0;
		m_pixelFormat.B_bits = 0;
		m_pixelFormat.A_bits = 0;
	}
}


//____ Destructor ______________________________________________________________

WgSurfaceMIG::~WgSurfaceMIG()
{
	if( m_pPixels )
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
	if( m_pTexture->GetPixelFormat() != EPixelFormat::eRGBA32 )
		return pixel;	// TODO: support other formats?
	if(x < GetWidth() && y < GetHeight())
	{
		if( m_pPixels )
			pixel = * ((Uint32*) &m_pPixels[m_pitch*y + 4*x]);
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
	if( m_pTexture->GetPixelFormat() != EPixelFormat::eRGBA32 )
		return alpha;	// TODO: support other formats?
	if(x < GetWidth() && y < GetHeight())
	{
		if( m_pPixels )
			alpha = m_pPixels[m_pitch*y + 4*x + 3];	// Tror att den ligger där, eller + 0.
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

//____ WgSurfaceFactoryMIG::CreateSurface() ___________________________________

WgSurface * WgSurfaceFactoryMIG::CreateSurface( const WgSize& size, WgSurface::PixelType type )
{
	EPixelFormat	format;

	switch( type )
	{
	case WgSurface::RGB_8:
		format = EPixelFormat::eRGB24;
		break;
	case WgSurface::RGBA_8:
		format = EPixelFormat::eRGBA32;
		break;
	default:
		return 0;

	}

	ETextureDataPtr p = ETextureData::Create( size.w, size.h, format );

	if( p )
		return new WgSurfaceMIG( p );
	else
		return 0;
}


