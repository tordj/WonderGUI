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

#include <wg_surfacefactory.h>

namespace wg
{

	const TypeInfo SurfaceFactory::TYPEINFO = { "SurfaceFactory", &Object::TYPEINFO };


	//____ typeInfo() _________________________________________________________

	const TypeInfo& SurfaceFactory::typeInfo(void) const
	{
		return TYPEINFO;
	}



	Surface_p SurfaceFactory::createSurface(SizeI size, PixelFormat format, int flags, const Color8* pClut)
	{
		Surface::Blueprint bp;

		bp.size = size;
		bp.format = format;

		bp.buffered = (flags & SurfaceFlag::Buffered);
		bp.canvas = (flags & SurfaceFlag::Canvas);
		bp.dynamic = (flags & SurfaceFlag::Dynamic);
		bp.mipmap = (flags & SurfaceFlag::Mipmapped);
		bp.scale = (flags & SurfaceFlag::Scale200) ? 128 : 64;
		bp.clut = pClut;

		return createSurface(bp);
	}


	Surface_p SurfaceFactory::createSurface(SizeI size, PixelFormat format, Blob* pBlob, int pitch, int flags, const Color8* pClut)
	{
		Surface::Blueprint bp;

		bp.size = size;
		bp.format = format;

		bp.buffered = (flags & SurfaceFlag::Buffered);
		bp.canvas = (flags & SurfaceFlag::Canvas);
		bp.dynamic = (flags & SurfaceFlag::Dynamic);
		bp.mipmap = (flags & SurfaceFlag::Mipmapped);
		bp.scale = (flags & SurfaceFlag::Scale200) ? 128 : 64;
		bp.clut = pClut;

		return createSurface(bp, pBlob, pitch);
	}

	Surface_p SurfaceFactory::createSurface(SizeI size, PixelFormat format, uint8_t* pPixels, int pitch,
		const PixelDescription* pPixelDescription, int flags, const Color8* pClut)
	{
		Surface::Blueprint bp;

		bp.size = size;
		bp.format = format;

		bp.buffered = (flags & SurfaceFlag::Buffered);
		bp.canvas = (flags & SurfaceFlag::Canvas);
		bp.dynamic = (flags & SurfaceFlag::Dynamic);
		bp.mipmap = (flags & SurfaceFlag::Mipmapped);
		bp.scale = (flags & SurfaceFlag::Scale200) ? 128 : 64;
		bp.clut = pClut;

		return  createSurface(bp, pPixels, pitch, pPixelDescription);
	}

	Surface_p SurfaceFactory::createSurface(Surface* pOther, int flags)
	{
		Surface::Blueprint bp;

		bp.buffered = (flags & SurfaceFlag::Buffered);
		bp.canvas = (flags & SurfaceFlag::Canvas);
		bp.dynamic = (flags & SurfaceFlag::Dynamic);
		bp.mipmap = (flags & SurfaceFlag::Mipmapped);
		bp.scale = (flags & SurfaceFlag::Scale200) ? 128 : 64;

		return createSurface(bp, pOther);
	}



} // namespace wg
