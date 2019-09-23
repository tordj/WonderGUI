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

#include <memory.h>

#include <wg_glsurface.h>
#include <wg_glsurfacefactory.h>


//____ WgGlSurfaceFactory::MaxSize() ___________________________________

WgSize WgGlSurfaceFactory::MaxSize() const
{
	return WgGlSurface::MaxSize();
}

//____ createSurface() ________________________________________________________

WgSurface * WgGlSurfaceFactory::CreateSurface( const WgSize& size, WgPixelType type, int flags ) const
{
    return 	new WgGlSurface(size,type, m_flags | flags);
}



WgSurface * WgGlSurfaceFactory::CreateSurface( const WgSize& size, WgPixelType type, uint8_t * pPixels, int pitch, const WgPixelFormat& pixelFormat, int flags ) const
{
    return 	new WgGlSurface(size,type,pPixels, pitch, pixelFormat, m_flags | flags);	
}
