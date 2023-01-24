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

#include <memory.h>

#include <wg_glsurface.h>
#include <wg_glsurfacefactory.h>
#include <wg_gfxutil.h>



namespace wg
{

	const TypeInfo GlSurfaceFactory::TYPEINFO = { "GlSurfaceFactory", &SurfaceFactory::TYPEINFO };

	//____ typeInfo() _________________________________________________________

	const TypeInfo& GlSurfaceFactory::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ maxSize() ______________________________________________________________

	SizeI GlSurfaceFactory::maxSize() const
	{
		return GlSurface::maxSize();
	}

	//____ createSurface() ________________________________________________________

	Surface_p GlSurfaceFactory::createSurface(const Surface::Blueprint& blueprint)
	{
		return GlSurface::create(blueprint);
	}

	Surface_p GlSurfaceFactory::createSurface(const Surface::Blueprint& blueprint, Blob* pBlob, int pitch)
	{
		return GlSurface::create(blueprint, pBlob, pitch);
	}

	Surface_p GlSurfaceFactory::createSurface(const Surface::Blueprint& blueprint, uint8_t* pPixels, int pitch, const PixelDescription* pPixelDescription)
	{
		return GlSurface::create(blueprint, pPixels, pitch, pPixelDescription);
	}

	Surface_p GlSurfaceFactory::createSurface(const Surface::Blueprint& blueprint, Surface* pOther)
	{
		return GlSurface::create( blueprint, pOther );
	}


} // namespace wg
