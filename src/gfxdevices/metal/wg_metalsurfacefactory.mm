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

#include <wg_metalsurface.h>
#include <wg_metalsurfacefactory.h>
#include <wg_util.h>



namespace wg
{

	const TypeInfo MetalSurfaceFactory::TYPEINFO = { "MetalSurfaceFactory", &SurfaceFactory::TYPEINFO };

	//____ typeInfo() _________________________________________________________

	const TypeInfo& MetalSurfaceFactory::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ maxSize() ______________________________________________________________

	SizeI MetalSurfaceFactory::maxSize() const
	{
		return MetalSurface::maxSize();
	}

	//____ createSurface() ________________________________________________________

	Surface_p MetalSurfaceFactory::createSurface( SizeI size, PixelFormat format, int flags, const Color * pClut )
	{
		return MetalSurface::create(size,format,flags | m_flags,pClut);
	}


	Surface_p MetalSurfaceFactory::createSurface( SizeI size, PixelFormat format, Blob * pBlob, int pitch, int flags, const Color * pClut )
	{
		return MetalSurface::create(size, format, pBlob, pitch, flags | m_flags, pClut);
	}

	Surface_p MetalSurfaceFactory::createSurface( SizeI size, PixelFormat format, uint8_t * pPixels, int pitch, const PixelDescription * pPixelDescription, int flags, const Color * pClut )
	{
		return MetalSurface::create(size,format, pPixels, pitch, pPixelDescription, flags | m_flags, pClut);
	}

	Surface_p MetalSurfaceFactory::createSurface( Surface * pOther, int flags )
	{
		return MetalSurface::create( pOther,flags | m_flags);
	}


} // namespace wg
