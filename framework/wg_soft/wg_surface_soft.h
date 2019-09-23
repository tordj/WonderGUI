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
#ifndef WG_SURFACE_SOFT_DOT_H
#define WG_SURFACE_SOFT_DOT_H

#ifndef WG_SURFACE_DOT_H
#	include <wg_surface.h>
#endif



//____ Class WgSurfaceSoft _____________________________________________________________________

class WgSurfaceSoft : public WgSurface
{
	friend class WgGfxDeviceSoft;

 public:
	WgSurfaceSoft( WgSize size, WgPixelType type = WgPixelType::BGRA_8, int flags = wg::SurfaceFlag::Static );
	WgSurfaceSoft( WgSize size, WgPixelType type, Uint8 * pPixels, int pitch, int flags = wg::SurfaceFlag::Static );
    WgSurfaceSoft( WgSize size, WgPixelType type, uint8_t * pPixels, int pitch, const WgPixelFormat& pixelFormat, int flags = wg::SurfaceFlag::Static );
	WgSurfaceSoft( const WgSurfaceSoft * pOther, int flags = 0  );
	~WgSurfaceSoft();

	const char *Type() const;
	static const char * GetClass();


	static WgSize	MaxSize();
protected:
};


//========================================================================================
#endif // WG_SURFACE_SOFT_DOT_H
