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
	WgSurfaceSDL( WgSize size, WgPixelType type = WG_PIXEL_ARGB_8 );

	~WgSurfaceSDL();

	inline	SDL_Surface*	SDL_Surf() const { return m_pSurface; };

	// Methods needed by WgSurface

	const char *Type() const;
	static const char * GetMyType();

	WgSize		Size() const;
	bool		IsOpaque() const;

	Uint32		GetPixel( WgCoord coord ) const;
	Uint8		GetOpacity( WgCoord coord ) const;

	void *		Lock( WgAccessMode mode );
	void *		LockRegion( WgAccessMode mode, const WgRect& region );
	void		Unlock();


private:
	SDL_Surface * 	m_pSurface;

};


//____ WgSurfaceFactorySDL ____________________________________________________

class WgSurfaceFactorySDL : public WgSurfaceFactory
{
public:
	virtual WgSurface * CreateSurface( const WgSize& size, WgPixelType type = WG_PIXEL_ARGB_8 ) const;
};



#endif //WG_SURFACE_SDL_DOT_H

