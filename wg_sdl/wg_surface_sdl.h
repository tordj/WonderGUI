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

#ifndef	WG_SURFACE_SDL_DOT_H
#define	WG_SURFACE_SDL_DOT_H

#ifndef WG_SURFACE_DOT_H
#	include <wg_surface.h>
#endif

#ifdef WIN32
#	include <SDL.h>
#else
#	include <SDL/SDL.h>
#endif

//____ WgSurfaceSDL ___________________________________________________________

class WgSurfaceSDL : public WgSurface
{
public:
	WgSurfaceSDL(SDL_Surface * pSurf);
	WgSurfaceSDL( WgSurface::PixelType type, WgSize size );

	~WgSurfaceSDL();

	inline	SDL_Surface*	SDL_Surf() const { return m_pSurface; };

	// Methods needed by WgSurface

	Uint32		GetWidth() const;
	Uint32		GetHeight() const;
	bool		IsOpaque() const;

	Uint32		GetPixel( Uint32 x, Uint32 y ) const;
	Uint8		GetOpacity( Uint32 x, Uint32 y ) const;

	void *		Lock( LockStatus mode );
	void		Unlock();


private:
	SDL_Surface * 	m_pSurface;

};


//____ WgSurfaceFactorySDL ____________________________________________________

class WgSurfaceFactorySDL : public WgSurfaceFactory
{
public:
	virtual WgSurface * CreateSurface( const WgSize& size, WgSurface::PixelType type = WgSurface::RGBA_8 );
};



#endif //WG_SURFACE_SDL_DOT_H

