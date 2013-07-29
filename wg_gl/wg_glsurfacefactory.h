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

class WgGLSurfaceFactory;
typedef	WgSmartPtr<WgGLSurfaceFactory,WgSurfaceFactoryPtr>		WgGLSurfaceFactoryPtr;
typedef	WgWeakPtr<WgGLSurfaceFactory,WgSurfaceFactoryWeakPtr>	WgGLSurfaceFactoryWeakPtr;

//____ WgGLSurfaceFactory _____________________________________________________

class WgGLSurfaceFactory : public WgSurfaceFactory
{
public:
	static WgGLSurfaceFactoryPtr	Create() { return WgGLSurfaceFactoryPtr(new WgGLSurfaceFactory()); }

	bool							IsInstanceOf( const char * pClassName ) const;
	const char *					ClassName( void ) const;
	static const char				CLASSNAME[];
	static WgGLSurfaceFactoryPtr	Cast( const WgObjectPtr& pObject );

	WgSurfacePtr CreateSurface( const WgSize& size, WgPixelType type = WG_PIXEL_ARGB_8 ) const;
};



#endif //WG_GLSURFACEFACTORY_DOT_H

