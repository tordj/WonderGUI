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

#include <wg_cabi.h>
#include <wg_cabi_surface.h>
#include <wg_cabi_surfacefactory.h>


namespace wg
{

	const TypeInfo CABISurfaceFactory::TYPEINFO = { "CABISurfaceFactory", &SurfaceFactory::TYPEINFO };

	//____ constructor ________________________________________________________

	CABISurfaceFactory::CABISurfaceFactory( wg_obj cFactory) : m_cFactory(cFactory)
	{
		CABI::object->retain(cFactory);
	}

	//____ destructor ________________________________________________________

	CABISurfaceFactory::~CABISurfaceFactory()
	{
		CABI::object->release(m_cFactory);
	}


	//____ typeInfo() _________________________________________________________

	const TypeInfo& CABISurfaceFactory::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ maxSize() ______________________________________________________________

	SizeI CABISurfaceFactory::maxSize() const
	{			
		wg_sizeI sz = CABI::surfaceFactory->maxSurfaceSize(m_cFactory);
		return { sz.w, sz.h };
	}

	//____ createSurface() ________________________________________________________

	Surface_p CABISurfaceFactory::createSurface(const Surface::Blueprint& blueprint)
	{
		wg_surfaceBP	bp;
		_convertBlueprint(&bp, &blueprint);

		auto obj = CABI::surfaceFactory->createSurface(m_cFactory, &bp);
		return CABISurface::create(obj);
	}

	Surface_p CABISurfaceFactory::createSurface(const Surface::Blueprint& blueprint, Blob * pBlob, int pitch )
	{
		wg_surfaceBP	bp;
		_convertBlueprint(&bp, &blueprint);

		auto obj = CABI::surfaceFactory->createSurfaceFromBlob(m_cFactory, &bp, static_cast<Object*>(pBlob), pitch );
		return CABISurface::create(obj);
	}

	Surface_p CABISurfaceFactory::createSurface( const Surface::Blueprint& blueprint, uint8_t * pPixels, int pitch, const PixelDescription * pPixelDescription )
	{
		wg_surfaceBP	bp;
		_convertBlueprint(&bp, &blueprint);

		auto obj = CABI::surfaceFactory->createSurfaceFromBitmap(m_cFactory, &bp, pPixels, pitch, (wg_pixelDescription*) pPixelDescription);
		return CABISurface::create(obj);
	}

	Surface_p CABISurfaceFactory::createSurface( const Surface::Blueprint& blueprint, Surface* pOther )
	{
		wg_surfaceBP	bp;
		_convertBlueprint(&bp, &blueprint);

		auto obj = CABI::surfaceFactory->createSurfaceFromSurface(m_cFactory, &bp, static_cast<Object*>(pOther) );
		return CABISurface::create(obj);
	}


	//____ _convertBlueprint() ________________________________________________

	void CABISurfaceFactory::_convertBlueprint(wg_surfaceBP* pDest, const Surface::Blueprint* pSource)
	{
		pDest->buffered = pSource->buffered;
		pDest->canvas = pSource->canvas;
		pDest->clut = (wg_color8*)pSource->clut;
		pDest->dynamic = pSource->dynamic;
		pDest->format = (wg_pixelFormat)pSource->format;
		pDest->id = pSource->id;
		pDest->mipmap = pSource->mipmap;
		pDest->sampleMethod = (wg_sampleMethod)pSource->sampleMethod;
		pDest->scale = pSource->scale;
		pDest->size.w = pSource->size.w;
		pDest->size.h = pSource->size.h;
		pDest->tiling = pSource->tiling;
	}

} // namespace wg
