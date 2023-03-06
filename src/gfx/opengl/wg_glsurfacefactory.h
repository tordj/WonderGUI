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

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Geometry _________________________________________________

		SizeI		maxSize() const override;

		//.____ Misc _______________________________________________________

		Surface_p	createSurface(const Surface::Blueprint& blueprint) override;
		Surface_p	createSurface(const Surface::Blueprint& blueprint, Blob* pBlob, int pitch) override;

		Surface_p	createSurface(const Surface::Blueprint& blueprint, const uint8_t* pPixels,
								  const PixelDescription2& pixelDescription, int pitch = 0,
								  const Color8 * pPalette = nullptr) override;

		Surface_p	createSurface(const Surface::Blueprint& blueprint, const uint8_t* pPixels,
								  PixelFormat format = PixelFormat::Undefined, int pitch = 0,
								  const Color8 * pPalette = nullptr) override;
	protected:
		GlSurfaceFactory() {}
		~GlSurfaceFactory() {}

	};

}


#endif //WG_GLSURFACEFACTORY_DOT_H

