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

namespace wg 
{
	
	class WgSoftSurfaceFactory;
	typedef	WgStrongPtr<WgSoftSurfaceFactory,WgSurfaceFactory_p>	WgSoftSurfaceFactory_p;
	typedef	WgWeakPtr<WgSoftSurfaceFactory,WgSurfaceFactory_wp>	WgSoftSurfaceFactory_wp;
	
	//____ WgSoftSurfaceFactory _______________________________________________________
	
	class WgSoftSurfaceFactory : public WgSurfaceFactory
	{
	public:
		static WgSoftSurfaceFactory_p	create() { return WgSoftSurfaceFactory_p(new WgSoftSurfaceFactory()); }
	
		bool							isInstanceOf( const char * pClassName ) const;
		const char *					className( void ) const;
		static const char				CLASSNAME[];
		static WgSoftSurfaceFactory_p	cast( const WgObject_p& pObject );
	
		virtual WgSurface_p createSurface( const WgSize& size, WgPixelType type = WG_PIXEL_ARGB_8 ) const;
	protected:
		virtual ~WgSoftSurfaceFactory() {}
	};
	
	//========================================================================================

} // namespace wg
#endif // WG_SOFTSURFACEFACTORY_DOT_H
