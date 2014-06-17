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
#ifndef WG_SURFACEFACTORY_SOFT_DOT_H
#define WG_SURFACEFACTORY_SOFT_DOT_H

#ifndef WG_SOFTSURFACE_DOT_H
#	include <wg_softsurface.h>
#endif

#ifndef WG_SURFACEFACTORY_DOT_H
#	include <wg_surfacefactory.h>
#endif

#include <vector>

class WgSoftSurfaceFactory;
typedef	WgStrongPtr<WgSoftSurfaceFactory,WgSurfaceFactoryPtr>	WgSoftSurfaceFactoryPtr;
typedef	WgWeakPtr<WgSoftSurfaceFactory,WgSurfaceFactoryWeakPtr>	WgSoftSurfaceFactoryWeakPtr;

//____ WgSoftSurfaceFactory _______________________________________________________

class WgSoftSurfaceFactory : public WgSurfaceFactory
{
public:
	static WgSoftSurfaceFactoryPtr	Create() { return WgSoftSurfaceFactoryPtr(new WgSoftSurfaceFactory()); }

	bool							IsInstanceOf( const char * pClassName ) const;
	const char *					ClassName( void ) const;
	static const char				CLASSNAME[];
	static WgSoftSurfaceFactoryPtr	Cast( const WgObjectPtr& pObject );

	virtual WgSurfacePtr CreateSurface( const WgSize& size, WgPixelType type = WG_PIXEL_ARGB_8 ) const;
protected:
	virtual ~WgSoftSurfaceFactory() {}
};

//========================================================================================
#endif // WG_SOFTSURFACEFACTORY_DOT_H
