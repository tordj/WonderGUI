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

#ifdef WIN32
#	include <SDL.h>
#else
#	include <SDL/SDL.h>
#endif
#include <wg_surface_sdl.h>
 
//____ Constructor _____________________________________________________________

WgSurfaceSDL::WgSurfaceSDL( SDL_Surface * pSurf )
{
	m_pSurface = pSurf;

	m_pitch = pSurf->pitch;

	m_pixelFormat.type = SPECIFIED;
	m_pixelFormat.bits = pSurf->format->BitsPerPixel;

	m_pixelFormat.R_mask = pSurf->format->Rmask;
	m_pixelFormat.G_mask = pSurf->format->Gmask;
	m_pixelFormat.B_mask = pSurf->format->Bmask;
	m_pixelFormat.A_mask = pSurf->format->Amask;

	m_pixelFormat.R_shift = pSurf->format->Rshift - pSurf->format->Rloss;
	m_pixelFormat.G_shift = pSurf->format->Gshift - pSurf->format->Gloss;
	m_pixelFormat.B_shift = pSurf->format->Bshift - pSurf->format->Bloss;
	m_pixelFormat.A_shift = pSurf->format->Ashift - pSurf->format->Aloss;

	m_pixelFormat.R_bits = 8 - pSurf->format->Rloss;
	m_pixelFormat.G_bits = 8 - pSurf->format->Gloss;
	m_pixelFormat.B_bits = 8 - pSurf->format->Bloss;
	m_pixelFormat.A_bits = 8 - pSurf->format->Aloss;
}

WgSurfaceSDL::WgSurfaceSDL( WgSurface::PixelType type, WgSize size )
{
	int flags = SDL_HWSURFACE | SDL_SRCALPHA;

	SDL_Surface * pSurf;

	switch( type )
	{
	case WgSurface::RGB_8:
		pSurf = SDL_CreateRGBSurface( flags, size.w, size.h, 24, 0xFF, 0xFF00, 0xFF0000, 0x00 );
		m_pixelFormat.type = WgSurface::RGB_8;
		break;
	case WgSurface::RGBA_8:
		pSurf = SDL_CreateRGBSurface( flags, size.w, size.h, 32, 0xFF, 0xFF00, 0xFF0000, 0xFF000000 );
		m_pixelFormat.type = WgSurface::RGBA_8;
		break;
	default:
		pSurf = 0;
		m_pixelFormat.type = UNSPECIFIED;
		break;
	}

	m_pSurface = pSurf;

	m_pitch = pSurf->pitch;

	m_pixelFormat.bits = pSurf->format->BitsPerPixel;

	m_pixelFormat.R_mask = pSurf->format->Rmask;
	m_pixelFormat.G_mask = pSurf->format->Gmask;
	m_pixelFormat.B_mask = pSurf->format->Bmask;
	m_pixelFormat.A_mask = pSurf->format->Amask;

	m_pixelFormat.R_shift = pSurf->format->Rshift - pSurf->format->Rloss;
	m_pixelFormat.G_shift = pSurf->format->Gshift - pSurf->format->Gloss;
	m_pixelFormat.B_shift = pSurf->format->Bshift - pSurf->format->Bloss;
	m_pixelFormat.A_shift = pSurf->format->Ashift - pSurf->format->Aloss;

	m_pixelFormat.R_bits = 8 - pSurf->format->Rloss;
	m_pixelFormat.G_bits = 8 - pSurf->format->Gloss;
	m_pixelFormat.B_bits = 8 - pSurf->format->Bloss;
	m_pixelFormat.A_bits = 8 - pSurf->format->Aloss;

}



//____ Destructor ______________________________________________________________

WgSurfaceSDL::~WgSurfaceSDL()
{
	// Free the surface

	SDL_FreeSurface( m_pSurface );
}

//____ Size() _________________________________________________________________

WgSize WgSurfaceSDL::Size() const
{
	if( m_pSurface )
		return WgSize( m_pSurface->w, m_pSurface->h );

	return WgSize(0,0);
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

	m_pPixels = (Uint8*) m_pSurface->pixels;
	m_lockRegion = WgRect( 0, 0, m_pSurface->w, m_pSurface->h );
	return m_pSurface->pixels;
}

//____ LockRegion() ___________________________________________________________

void * WgSurfaceSDL::LockRegion( LockStatus mode, const WgRect& region )
{
	if( !m_pSurface )
		return 0;

	int width = m_pSurface->w;
	int height = m_pSurface->h;

	if( region.x < 0 || region.y < 0 || region.x + region.w > width || region.y + region.h > height )
		return 0;

	Lock( mode );

	if( m_pPixels )
	{
		m_lockRegion = region;
		return m_pPixels + (width*region.y+region.x)*m_pSurface->format->BytesPerPixel;
	}
	return 0;

}


//____ Unlock() ________________________________________________________________

void WgSurfaceSDL::Unlock()
{
	if(m_lockStatus == UNLOCKED )
		return;

	SDL_UnlockSurface(m_pSurface);
	m_lockStatus = UNLOCKED;
	m_lockRegion.w = 0;
	m_lockRegion.h = 0;
	m_pPixels = 0;
}



//____ GetPixel() ______________________________________________________________

Uint32 WgSurfaceSDL::GetPixel( WgCoord coord ) const
{
	SDL_LockSurface( m_pSurface );

	Uint8 * pPixel = (Uint8 *) m_pSurface->pixels;

	pPixel += coord.y*m_pSurface->pitch + coord.x*m_pSurface->format->BytesPerPixel;

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

Uint8 WgSurfaceSDL::GetOpacity( WgCoord coord ) const
{
	// All pixels are transparent if we don't have any surface...

	if( !m_pSurface )
		return 0;

	// Opacity tests below done in order to match SDL blitting rules (hopefully).

	// First, if surface has SDL_SRCALPHA + alpha channel we ignore everything
	// else.

	if( m_pSurface->flags & SDL_SRCALPHA && m_pSurface->format->Amask )
		return ((GetPixel(coord) & m_pSurface->format->Amask) >> m_pSurface->format->Ashift) << m_pSurface->format->Aloss;

	// Secondly, check for colorkey, return transparent if match.

	if( m_pSurface->flags & SDL_SRCCOLORKEY )
	{
		if( GetPixel( coord ) == 0 )
			return 0;
	}

	// Third, if surface has SDL_SRCALPHA we return the surace alpha value.

	if( m_pSurface->flags & SDL_SRCALPHA )
		return 0;//m_pSurface->format->alpha;

	// Fourth, all tests passed, pixel is opaque.

	return 255;
}


//____ WgSurfaceFactorySDL::CreateSurface() ___________________________________

WgSurface * WgSurfaceFactorySDL::CreateSurface( const WgSize& size, WgSurface::PixelType type )
{
	return new WgSurfaceSDL( type, size );
}
