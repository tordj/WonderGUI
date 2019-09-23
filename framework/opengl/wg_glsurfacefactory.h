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

#ifndef	WG_GLSURFACEFACTORY_DOT_H
#define	WG_GLSURFACEFACTORY_DOT_H

#ifndef WG_SURFACEFACTORY_DOT_H
#	include <wg_surfacefactory.h>
#endif


//____ WgGlSurfaceFactory _____________________________________________________

class WgGlSurfaceFactory : public WgSurfaceFactory
{
public:
  WgGlSurfaceFactory( int flags = wg::SurfaceFlag::Static ) : m_flags(flags) {}

	WgSize		MaxSize() const;
    WgSurface * CreateSurface( const WgSize& size, WgPixelType type = WgPixelType::BGRA_8,  int flags = 0 ) const;
    WgSurface * CreateSurface( const WgSize& size, WgPixelType type, uint8_t * pPixels, int pitch, const WgPixelFormat& pPixelFormat, int flags = 0 ) const;
protected:
  int   m_flags;

};


#endif //WG_GLSURFACEFACTORY_DOT_H

