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

namespace wg 
{
	
	
	class SurfaceFactory;
	typedef	WgStrongPtr<SurfaceFactory,Object_p>	SurfaceFactory_p;
	typedef	WgWeakPtr<SurfaceFactory,Object_wp>	SurfaceFactory_wp;
	
	
	//____ SurfaceFactory _______________________________________________________
	/**
	 * @brief Factory class for creating surfaces.
	 *
	 * SurfaceFactory is the base class for all surface factories. The surface factories
	 * are used by WonderGUI components that needs to dynamically create surfaces as
	 * part of their operation, like WgVectorFont.
	 *
	 **/
	class SurfaceFactory : public Object
	{
	public:
		bool						isInstanceOf( const char * pClassName ) const;
		const char *				className( void ) const;
		static const char			CLASSNAME[];
		static SurfaceFactory_p	cast( const Object_p& pObject );
	
		virtual Surface_p createSurface( const Size& size, WgPixelType type = WG_PIXEL_RGBA_8 ) const = 0;
	protected:
		virtual ~SurfaceFactory() {}
	};
	
	
	//==============================================================================

} // namespace wg
#endif // WG_SURFACE_DOT_H
