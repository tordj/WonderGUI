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

#ifndef	WG_SDLSURFACE_DOT_H
#define	WG_SDLSURFACE_DOT_H

#ifndef WG_SURFACE_DOT_H
#	include <wg_surface.h>
#endif

#include <SDL2/SDL.h>

class WgSDLSurface;
typedef	WgSmartPtr<WgSDLSurface,WgSurfacePtr>		WgSDLSurfacePtr;
typedef	WgWeakPtr<WgSDLSurface,WgSurfaceWeakPtr>	WgSDLSurfaceWeakPtr;

//____ WgSDLSurface ___________________________________________________________

class WgSDLSurface : public WgSurface
{
	friend class WgSDLSurfaceFactory;

public:
	static WgSDLSurfacePtr	Create(SDL_Surface * pSurf) { return new WgSDLSurface(pSurf); }
	static WgSDLSurfacePtr	Create( WgSize size, WgPixelType type = WG_PIXEL_ARGB_8 ) { return new WgSDLSurface(size,type); }

	bool					IsInstanceOf( const char * pClassName ) const;
	const char *			ClassName( void ) const;
	static const char		CLASSNAME[];
	static WgSDLSurfacePtr	Cast( const WgObjectPtr& pObject );

	inline	SDL_Surface*	SDL_Surf() const { return m_pSurface; };

	// Methods needed by WgSurface

	WgSize		Size() const;
	bool		IsOpaque() const;

	Uint32		GetPixel( WgCoord coord ) const;
	Uint8		GetOpacity( WgCoord coord ) const;

	void *		Lock( WgAccessMode mode );
	void *		LockRegion( WgAccessMode mode, const WgRect& region );
	void		Unlock();


private:
	WgSDLSurface(SDL_Surface * pSurf);
	WgSDLSurface( WgSize size, WgPixelType type = WG_PIXEL_ARGB_8 );
	~WgSDLSurface();

	SDL_Surface * 	m_pSurface;
};


#endif //WG_SDLSURFACE_DOT_H

