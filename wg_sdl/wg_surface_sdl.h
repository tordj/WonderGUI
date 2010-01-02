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

#include <SDL.h>

//____ WgSurfaceSDL ___________________________________________________________

class WgSurfaceSDL : public WgSurface
{
public:
	WgSurfaceSDL(SDL_Surface * pSurf);
	~WgSurfaceSDL();

	inline	SDL_Surface*	SDL_Surf() const { return m_pSurface; };

	// Methods needed by WgSurface

	Uint32		GetWidth() const;
	Uint32		GetHeight() const;
	bool		IsOpaque() const;

	Uint32		GetPixel( Uint32 x, Uint32 y ) const;
	Uint8		GetOpacity( Uint32 x, Uint32 y ) const;
	Uint32		Col2Pixel( const WgColor& col ) const;
	WgColor		Pixel2Col( Uint32 pixel ) const;

	void *		Lock( LockStatus mode );
	void		Unlock();


private:
	SDL_Surface * 	m_pSurface;

};

#endif //WG_SURFACE_SDL_DOT_H

