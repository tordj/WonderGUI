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
#include <wg_surface_sdl.h>

//____ Constructor _____________________________________________________________

WgSurfaceSDL::WgSurfaceSDL( SDL_Surface * pSurf )
{
	m_pSurface = pSurf;
}

//____ Destructor ______________________________________________________________

WgSurfaceSDL::~WgSurfaceSDL()
{
	// Free the surface

	SDL_FreeSurface( m_pSurface );
}

//____ GetWidth() ______________________________________________________________

Uint32 WgSurfaceSDL::GetWidth() const
{
	if( m_pSurface )
		return m_pSurface->w;

	return 0;
}

//____ GetHeight() _____________________________________________________________

Uint32 WgSurfaceSDL::GetHeight() const
{
	if( m_pSurface )
		return m_pSurface->h;

	return 0;
}

//____ IsOpaque() ______________________________________________________________

bool WgSurfaceSDL::IsOpaque() const
{
  if( m_pSurface->flags & SDL_SRCALPHA || m_pSurface->flags & SDL_SRCCOLORKEY )
		return true;

	return false;
}

//____ Lock() __________________________________________________________________

void * WgSurfaceSDL::Lock( LockStatus mode )
{
	if( m_lockStatus != UNLOCKED || mode == UNLOCKED )
		return 0;

	SDL_LockSurface(m_pSurface);

	m_lockStatus = READ_WRITE;
	return m_pSurface->pixels;
}

//____ Unlock() ________________________________________________________________

void WgSurfaceSDL::Unlock()
{
	if(m_lockStatus == UNLOCKED )
		return;

	SDL_UnlockSurface(m_pSurface);
	m_lockStatus = UNLOCKED;
}



//____ GetPixel() ______________________________________________________________

Uint32 WgSurfaceSDL::GetPixel( Uint32 x, Uint32 y ) const
{
	SDL_LockSurface( m_pSurface );

	Uint8 * pPixel = (Uint8 *) m_pSurface->pixels;

	pPixel += y*m_pSurface->pitch + x*m_pSurface->format->BytesPerPixel;

	Uint32 val;
    switch( m_pSurface->format->BytesPerPixel )
    {
    	case 1:
     		val = (Uint32) *pPixel;
		case 2:
     		val = (Uint32) ((Uint16*) pPixel)[0];
		case 3:
	    	val = ((Uint32) pPixel[0]) + (((Uint32) pPixel[1]) << 8) + (((Uint32) pPixel[2]) << 16);
		default:
       		val = *((Uint32*) pPixel);
	}

	SDL_UnlockSurface( m_pSurface );
	return val;
}

//____ GetOpacity() ____________________________________________________________

Uint8 WgSurfaceSDL::GetOpacity( Uint32 x, Uint32 y ) const
{
	// All pixels are transparent if we don't have any surface...

	if( !m_pSurface )
		return 0;

	// Opacity tests below done in order to match SDL blitting rules (hopefully).

	// First, if surface has SDL_SRCALPHA + alpha channel we ignore everything
	// else.

	if( m_pSurface->flags & SDL_SRCALPHA && m_pSurface->format->Amask )
		return ((GetPixel(x,y) & m_pSurface->format->Amask) >> m_pSurface->format->Ashift) << m_pSurface->format->Aloss;

	// Secondly, check for colorkey, return transparent if match.

	if( m_pSurface->flags & SDL_SRCCOLORKEY )
	{
		if( pixel( x, y ) == 0 )
			return 0;
	}

	// Third, if surface has SDL_SRCALPHA we return the surace alpha value.

	if( m_pSurface->flags & SDL_SRCALPHA )
		return 0;//m_pSurface->format->alpha;

	// Fourth, all tests passed, pixel is opaque.

	return 255;
}

//____ Col2Pixel() _____________________________________________________________

Uint32 WgSurfaceSDL::Col2Pixel( const WgColor& col ) const
{
	return SDL_MapRGB( m_pSurface->format, col.r, col.g, col.b );
}

//____ Pixel2Col() _____________________________________________________________

WgColor WgSurfaceSDL::Pixel2Col( Uint32 pixel ) const
{
	WgColor	col = 0;
	SDL_GetRGB( pixel, m_pSurface->format, &col.r, &col.g, &col.b );
	return col;
}


