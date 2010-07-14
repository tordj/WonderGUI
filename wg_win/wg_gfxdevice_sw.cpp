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

#include <SDL.h>
#include <wg_gfxdevice_sw.h>
#include <wg_surface_sw.h>

//____ Constructor _____________________________________________________________

WgGfxDeviceSW::WgGfxDeviceSW( WgSurfaceSW * pCanvas)
{
	SetCanvas( pCanvas );
}

//____ Destructor ______________________________________________________________

WgGfxDeviceSW::~WgGfxDeviceSW()
{
}

//____ SetCanvas() __________________________________________________________________

void WgGfxDeviceSW::SetCanvas( WgSurfaceSW * pCanvas )
{
	m_pCanvas 		= pCanvas;

	if( pCanvas )
	{
		m_canvasHeight 	= pCanvas->GetHeight();
		m_canvasWidth 	= pCanvas->GetWidth();
	}
	else
	{
		m_canvasHeight 	= 0;
		m_canvasWidth 	= 0;
	}
}


//____ Fill() __________________________________________________________________

void WgGfxDeviceSW::Fill( const WgRect& _rect, const WgColor& _col )
{
}

//____ Blit() __________________________________________________________________

void Blit24( Uint8* pDest, Uint32 destpitch, const Uint8* pSource, Uint32 sourcepitch, Uint32 w, Uint32 h )
{
	for( Uint32 y=0; y<h; y++ )
	{
		memcpy( pDest, pSource, w*3 );
		pSource += sourcepitch;
		pDest += destpitch;
	}
}

void Blit32( Uint8* pDest, Uint32 destpitch, const Uint8* pSource, Uint32 sourcepitch, Uint32 w, Uint32 h )
{
	for( Uint32 y=0; y<h; y++ )
	{
		for( Uint32 x=0; x<w; x++ )
		{
			pDest[ 3*x + 0 ] = pSource[ 4*x + 0 ];
			pDest[ 3*x + 1 ] = pSource[ 4*x + 1 ];
			pDest[ 3*x + 2 ] = pSource[ 4*x + 2 ];
		}

		pSource += sourcepitch;
		pDest += destpitch;
	}
}

inline Uint8 Blend( Uint8 a, Uint8 b, Uint8 k )
{
	if( k == 0 )
		return a;
	Uint32 r = Uint32(a)*Uint32(0xff-k) + Uint32(b)*Uint32(k);
	return Uint8( r >> 8 );
}

void Blend32( Uint8* pDest, Uint32 destpitch, const Uint8* pSource, Uint32 sourcepitch, Uint32 w, Uint32 h )
{
	for( Uint32 y=0; y<h; y++ )
	{
		for( Uint32 x=0; x<w; x++ )
		{
			Uint8 alpha = pSource[ 4*x + 3 ];
			if( alpha == 0 )
				continue;
			else if( alpha == 0xff )
			{
				pDest[ 3*x + 0 ] = pSource[ 4*x + 0 ];
				pDest[ 3*x + 1 ] = pSource[ 4*x + 1 ];
				pDest[ 3*x + 2 ] = pSource[ 4*x + 2 ];
			}
			else
			{
				pDest[ 3*x + 0 ] = Blend( pDest[ 3*x + 0 ], pSource[ 4*x + 0 ], alpha );
				pDest[ 3*x + 1 ] = Blend( pDest[ 3*x + 1 ], pSource[ 4*x + 1 ], alpha );
				pDest[ 3*x + 2 ] = Blend( pDest[ 3*x + 2 ], pSource[ 4*x + 2 ], alpha );
			}
		}

		pSource += sourcepitch;
		pDest += destpitch;
	}
}



void WgGfxDeviceSW::Blit( const WgSurface* _pSrc, const WgRect& _src, Sint32 _dx, Sint32 _dy  )
{
	if( !m_pCanvas || !_pSrc )
		return;

	if( m_pCanvas->GetBitdepth() != 24 )
		return;

	WgSurfaceSW * pSrc = (WgSurfaceSW*) _pSrc;

	Uint32 destpitch = m_pCanvas->pitch();
	Uint32 sourcepitch = pSrc->pitch();

	Uint32 bits = pSrc->GetBitdepth();
	Uint8* pSource = ((Uint8*)pSrc->Lock()) + _src.y*sourcepitch + (bits/8)*_src.x;
	Uint8* pDest = ((Uint8*)m_pCanvas->Lock(true)) + _dy*destpitch + 3*_dx;

	Uint32 w = _src.w;
	Uint32 h = _src.h;

	if( bits == 24 )
		Blit24( pDest, destpitch, pSource, sourcepitch, w, h );
	else
		Blend32( pDest, destpitch, pSource, sourcepitch, w, h );

	pSrc->Unlock();
	m_pCanvas->Unlock();
}

//____ StretchBlitSubPixel() ___________________________________________________

void WgGfxDeviceSW::StretchBlitSubPixel( const WgSurface * pSrc, float sx, float sy, float sw, float sh,
								   		  float dx, float dy, float dw, float dh )
{
	// Not implemented yet!
}
