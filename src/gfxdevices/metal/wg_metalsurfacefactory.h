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

#ifndef	WG_METALSURFACEFACTORY_DOT_H
#define	WG_METALSURFACEFACTORY_DOT_H
#pragma once

#include <wg_surfacefactory.h>

namespace wg
{

	class MetalSurfaceFactory;
	typedef	StrongPtr<MetalSurfaceFactory>	MetalSurfaceFactory_p;
	typedef	WeakPtr<MetalSurfaceFactory>	MetalSurfaceFactory_wp;

	//____ MetalSurfaceFactory _____________________________________________________

	class MetalSurfaceFactory : public SurfaceFactory
	{
	public:
		//.____ Creation __________________________________________

		static MetalSurfaceFactory_p	create( int flags = SurfaceFlag::Static ) { return MetalSurfaceFactory_p(new MetalSurfaceFactory(flags)); }

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Geometry _________________________________________________

		SizeI		maxSize() const override;

		//.____ Misc _______________________________________________________

		Surface_p	createSurface( SizeI size, PixelFormat format = PixelFormat::BGRA_8, int flags = 0, const Color * pClut = nullptr ) const override;
		Surface_p	createSurface( SizeI size, PixelFormat format, Blob * pBlob, int pitch, int flags = 0, const Color * pClut = nullptr ) const override;
		Surface_p	createSurface( SizeI size, PixelFormat format, uint8_t * pPixels, int pitch, const PixelDescription * pPixelDescription = 0, int flags = 0, const Color * pClut = nullptr ) const override;
		Surface_p	createSurface( Surface * pOther, int flags = 0 ) const override;

	protected:
		MetalSurfaceFactory(int flags) : m_flags(flags) {}

		int m_flags;

	};

}


#endif //WG_METALSURFACEFACTORY_DOT_H

