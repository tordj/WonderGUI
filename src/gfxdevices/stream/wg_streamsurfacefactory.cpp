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

#include <wg_streamsurface.h>
#include <wg_streamsurfacefactory.h>
#include <wg_util.h>



namespace wg
{

	const TypeInfo StreamSurfaceFactory::TYPEINFO = { "StreamSurfaceFactory", &SurfaceFactory::TYPEINFO };

	//____ constructor ________________________________________________________

	StreamSurfaceFactory::StreamSurfaceFactory(CGfxOutStream& stream) : m_pStream(&stream)
	{

	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& StreamSurfaceFactory::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ maxSize() ______________________________________________________________

	SizeI StreamSurfaceFactory::maxSize() const
	{
		return StreamSurface::maxSize();
	}

	//____ createSurface() ________________________________________________________

	Surface_p StreamSurfaceFactory::createSurface( SizeI size, PixelFormat format, int flags, const Color * pClut ) const
	{
		return StreamSurface::create(*m_pStream,size,format,flags,pClut);
	}

	Surface_p StreamSurfaceFactory::createSurface( SizeI size, PixelFormat format, Blob * pBlob, int pitch, int flags, const Color * pClut ) const
	{
		return StreamSurface::create(*m_pStream,size,format, pBlob,pitch,flags,pClut);
	}

	Surface_p StreamSurfaceFactory::createSurface( SizeI size, PixelFormat format, uint8_t * pPixels, int pitch, const PixelDescription * pPixelDescription, int flags, const Color * pClut ) const
	{
		return StreamSurface::create(*m_pStream,size,format, pPixels, pitch, pPixelDescription,flags,pClut);
	}

	Surface_p StreamSurfaceFactory::createSurface( Surface * pOther, int flags ) const
	{
		return StreamSurface::create(*m_pStream,pOther, flags );
	}


} // namespace wg
