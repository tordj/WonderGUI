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

#ifndef WG_SURFACEFACTORY_DOT_H
#define	WG_SURFACEFACTORY_DOT_H
//==============================================================================

#ifndef	WG_SURFACE_DOT_H
#	include <wg_surface.h>
#endif


class WgSurfaceFactory;
typedef	WgStrongPtr<WgSurfaceFactory,WgObjectPtr>	WgSurfaceFactoryPtr;
typedef	WgWeakPtr<WgSurfaceFactory,WgObjectWeakPtr>	WgSurfaceFactoryWeakPtr;


//____ WgSurfaceFactory _______________________________________________________

class WgSurfaceFactory : public WgObject
{
public:
	bool						IsInstanceOf( const char * pClassName ) const;
	const char *				ClassName( void ) const;
	static const char			CLASSNAME[];
	static WgSurfaceFactoryPtr	Cast( const WgObjectPtr& pObject );

	virtual WgSurfacePtr CreateSurface( const WgSize& size, WgPixelType type = WG_PIXEL_ARGB_8 ) const = 0;
protected:
	virtual ~WgSurfaceFactory() {}
};


//==============================================================================
#endif // WG_SURFACE_DOT_H
