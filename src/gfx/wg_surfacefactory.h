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
#pragma once
//==============================================================================

#include <wg_surface.h>
#include <wg_blob.h>


namespace wg
{


	class SurfaceFactory;
	typedef	StrongPtr<SurfaceFactory>	SurfaceFactory_p;
	typedef	WeakPtr<SurfaceFactory>	SurfaceFactory_wp;


	//____ SurfaceFactory _______________________________________________________
	/**
	 * @brief Factory class for creating surfaces.
	 *
	 * SurfaceFactory is the base class for all surface factories. The surface factories
	 * are used by WonderGUI objects that needs to dynamically create surfaces as
	 * part of their operation, like FreeTypeFont.
	 *
	 **/
	class SurfaceFactory : public Object
	{
	public:

		//.____ Identification _________________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Geometry _________________________________________________

		virtual SizeI		maxSize() const = 0;

		//.____ Misc _______________________________________________________

		virtual Surface_p	createSurface(const Surface::Blueprint& blueprint) = 0;
		virtual Surface_p	createSurface(const Surface::Blueprint& blueprint, Blob* pBlob, int pitch = 0) = 0;
		virtual Surface_p	createSurface(const Surface::Blueprint& blueprint, const uint8_t* pPixels,
										  const PixelDescription& pixelDescription, int pitch = 0, const Color8 * pPalette = nullptr, int paletteSize = 0) = 0;
		virtual Surface_p	createSurface(const Surface::Blueprint& blueprint, const uint8_t* pPixels, PixelFormat format = PixelFormat::Undefined, int pitch = 0,
										  const Color8 * pPalette = nullptr, int paletteSize = 0) = 0;

	protected:
		virtual ~SurfaceFactory() {}
	};


	//==============================================================================

} // namespace wg
#endif // WG_SURFACE_DOT_H
