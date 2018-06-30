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
#pragma once

#include <wg_surfacefactory.h>

namespace wg
{

	class GlSurfaceFactory;
	typedef	StrongPtr<GlSurfaceFactory>		GlSurfaceFactory_p;
	typedef	WeakPtr<GlSurfaceFactory>	GlSurfaceFactory_wp;

	//____ GlSurfaceFactory _____________________________________________________

	class GlSurfaceFactory : public SurfaceFactory
	{
	public:
		//.____ Creation __________________________________________

		static GlSurfaceFactory_p	create() { return GlSurfaceFactory_p(new GlSurfaceFactory()); }

		//.____ Identification __________________________________________

		bool						isInstanceOf( const char * pClassName ) const;
		const char *				className( void ) const;
		static const char			CLASSNAME[];
		static GlSurfaceFactory_p	cast( Object * pObject );

		//.____ Geometry _________________________________________________

		Size		maxSize() const;

		//.____ Misc _______________________________________________________

		Surface_p	createSurface( Size size, PixelFormat format = PixelFormat::BGRA_8, int hint = SurfaceHint::Static ) const;
        Surface_p	createSurface( Size size, PixelFormat format, Blob * pBlob, int pitch, int hint = SurfaceHint::Static ) const;
        Surface_p	createSurface( Size size, PixelFormat format, uint8_t * pPixels, int pitch, const PixelDescription * pPixelDescription = 0, int hint = SurfaceHint::Static ) const ;
        Surface_p	createSurface( Surface * pOther, int hint = SurfaceHint::Static ) const;
	};
}


#endif //WG_GLSURFACEFACTORY_DOT_H

