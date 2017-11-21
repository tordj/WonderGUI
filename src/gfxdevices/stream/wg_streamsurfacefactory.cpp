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

	const char StreamSurfaceFactory::CLASSNAME[] = {"StreamSurfaceFactory"};

	//____ Constructor ________________________________________________________

	StreamSurfaceFactory::StreamSurfaceFactory(GfxOutStream * pStream) : m_pStream(pStream)
	{
		
	}

	//____ isInstanceOf() _________________________________________________________

	bool StreamSurfaceFactory::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;

		return SurfaceFactory::isInstanceOf(pClassName);
	}

	//____ className() ____________________________________________________________

	const char * StreamSurfaceFactory::className( void ) const
	{ 
		return CLASSNAME; 
	}

	//____ cast() _________________________________________________________________

	StreamSurfaceFactory_p StreamSurfaceFactory::cast( Object * pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return StreamSurfaceFactory_p( static_cast<StreamSurfaceFactory*>(pObject) );

		return 0;
	}

	//____ maxSize() ______________________________________________________________

	Size StreamSurfaceFactory::maxSize() const
	{
		return StreamSurface::maxSize();
	}

	//____ createSurface() ________________________________________________________

	Surface_p StreamSurfaceFactory::createSurface( Size size, PixelType type, int hint ) const
	{
        return StreamSurface::create(m_pStream,size,type,hint);
	}


	Surface_p StreamSurfaceFactory::createSurface( Size size, PixelType type, Blob * pBlob, int pitch, int hint ) const
	{
		return StreamSurface::create(m_pStream,size,type, pBlob,pitch,hint);
	}
	
	Surface_p StreamSurfaceFactory::createSurface( Size size, PixelType type, uint8_t * pPixels, int pitch, const PixelFormat * pPixelFormat, int hint ) const
	{
		return StreamSurface::create(m_pStream,size,type, pPixels, pitch, pPixelFormat,hint);
	}
	
	Surface_p StreamSurfaceFactory::createSurface( Surface * pOther, int hint ) const
	{
		return StreamSurface::create(m_pStream,pOther,hint );
	}


} // namespace wg
