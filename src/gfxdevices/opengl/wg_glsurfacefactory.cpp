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

	const char GlSurfaceFactory::CLASSNAME[] = {"GlSurfaceFactory"};

	//____ isInstanceOf() _________________________________________________________

	bool GlSurfaceFactory::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;

		return SurfaceFactory::isInstanceOf(pClassName);
	}

	//____ className() ____________________________________________________________

	const char * GlSurfaceFactory::className( void ) const
	{ 
		return CLASSNAME; 
	}

	//____ cast() _________________________________________________________________

	GlSurfaceFactory_p GlSurfaceFactory::cast( Object * pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return GlSurfaceFactory_p( static_cast<GlSurfaceFactory*>(pObject) );

		return 0;
	}

	//____ maxSize() ______________________________________________________________

	Size GlSurfaceFactory::maxSize() const
	{
		return GlSurface::maxSize();
	}

	//____ createSurface() ________________________________________________________

	Surface_p GlSurfaceFactory::createSurface( Size size, PixelFormat format, int hint ) const
	{
        return GlSurface::create(size,format,hint);
	}


	Surface_p GlSurfaceFactory::createSurface( Size size, PixelFormat format, Blob * pBlob, int pitch, int hint ) const
	{
		return GlSurface::create(size,format, pBlob,pitch,hint);
	}
	
	Surface_p GlSurfaceFactory::createSurface( Size size, PixelFormat format, uint8_t * pPixels, int pitch, const PixelDescription * pPixelDescription, int hint ) const
	{
		return GlSurface::create(size,format, pPixels, pitch, pPixelDescription,hint);
	}
	
	Surface_p GlSurfaceFactory::createSurface( Surface * pOther, int hint ) const
	{
		return GlSurface::create( pOther,hint );
	}


} // namespace wg
