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
#include <wg_util.h>



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

	Surface_p GlSurfaceFactory::createSurface( SizeI size, PixelFormat format, int flags, const Color * pClut ) const
	{
		return GlSurface::create(size,format,flags | m_flags,pClut);
	}


	Surface_p GlSurfaceFactory::createSurface( SizeI size, PixelFormat format, Blob * pBlob, int pitch, int flags, const Color * pClut ) const
	{
		return GlSurface::create(size, format, pBlob, pitch, flags | m_flags, pClut);
	}

	Surface_p GlSurfaceFactory::createSurface( SizeI size, PixelFormat format, uint8_t * pPixels, int pitch, const PixelDescription * pPixelDescription, int flags, const Color * pClut ) const
	{
		return GlSurface::create(size,format, pPixels, pitch, pPixelDescription, flags | m_flags, pClut);
	}

	Surface_p GlSurfaceFactory::createSurface( Surface * pOther, int flags ) const
	{
		return GlSurface::create( pOther,flags | m_flags);
	}


} // namespace wg
