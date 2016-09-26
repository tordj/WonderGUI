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
	
	class SoftSurfaceFactory;
	typedef	StrongPtr<SoftSurfaceFactory,SurfaceFactory_p>	SoftSurfaceFactory_p;
	typedef	WeakPtr<SoftSurfaceFactory,SurfaceFactory_wp>	SoftSurfaceFactory_wp;
	
	//____ SoftSurfaceFactory _______________________________________________________
	
	class SoftSurfaceFactory : public SurfaceFactory
	{
	public:
		static SoftSurfaceFactory_p	create() { return SoftSurfaceFactory_p(new SoftSurfaceFactory()); }
	
		bool							isInstanceOf( const char * pClassName ) const;
		const char *					className( void ) const;
		static const char				CLASSNAME[];
		static SoftSurfaceFactory_p	cast( const Object_p& pObject );
	
		virtual Surface_p	createSurface( Size size, PixelType type = PixelType::BGRA_8 ) const;
        virtual Surface_p	createSurface( Size size, PixelType type, const Blob_p& pBlob, int pitch ) const;
        virtual Surface_p	createSurface( Size size, PixelType type, uint8_t * pPixels, int pitch, const PixelFormat * pPixelFormat = 0 ) const;
        virtual Surface_p	createSurface( const Surface_p& pOther ) const;
		
	protected:
		virtual ~SoftSurfaceFactory() {}
	};
	
	//========================================================================================

} // namespace wg
#endif // WG_SOFTSURFACEFACTORY_DOT_H
