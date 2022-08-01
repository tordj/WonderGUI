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

#include <wg_capisurface.h>
#include <wg_capisurfacefactory.h>

#include <wg_c_object.h>
#include <wg_c_surfacefactory.h>


namespace wg
{

	const TypeInfo CAPISurfaceFactory::TYPEINFO = { "CAPISurfaceFactory", &SurfaceFactory::TYPEINFO };

	//____ constructor ________________________________________________________

	CAPISurfaceFactory::CAPISurfaceFactory( wg_obj cFactory) : m_cFactory(cFactory)
	{
		wg_retain(cFactory);
	}

	//____ destructor ________________________________________________________

	CAPISurfaceFactory::~CAPISurfaceFactory()
	{
		wg_release(m_cFactory);
	}


	//____ typeInfo() _________________________________________________________

	const TypeInfo& CAPISurfaceFactory::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ maxSize() ______________________________________________________________

	SizeI CAPISurfaceFactory::maxSize() const
	{			
		wg_sizeI sz = wg_maxSurfaceSize(m_cFactory);
		return { sz.w, sz.h };
	}

	//____ createSurface() ________________________________________________________

	Surface_p CAPISurfaceFactory::createSurface(const Surface::Blueprint& blueprint)
	{
		wg_surfaceBP	bp;
		_convertBlueprint(&bp, &blueprint);

		auto obj = wg_createSurface(m_cFactory, &bp);
		return CAPISurface::create(obj);
	}

	Surface_p CAPISurfaceFactory::createSurface(const Surface::Blueprint& blueprint, Blob * pBlob, int pitch )
	{
		wg_surfaceBP	bp;
		_convertBlueprint(&bp, &blueprint);

		auto obj = wg_createSurfaceFromBlob(m_cFactory, &bp, static_cast<Object*>(pBlob), pitch );
		return CAPISurface::create(obj);
	}

	Surface_p CAPISurfaceFactory::createSurface( const Surface::Blueprint& blueprint, uint8_t * pPixels, int pitch, const PixelDescription * pPixelDescription )
	{
		wg_surfaceBP	bp;
		_convertBlueprint(&bp, &blueprint);

		auto obj = wg_createSurfaceFromBitmap(m_cFactory, &bp, pPixels, pitch, (wg_pixelDescription*) pPixelDescription);
		return CAPISurface::create(obj);
	}

	Surface_p CAPISurfaceFactory::createSurface( const Surface::Blueprint& blueprint, Surface* pOther )
	{
		wg_surfaceBP	bp;
		_convertBlueprint(&bp, &blueprint);

		auto obj = wg_createSurfaceFromSurface(m_cFactory, &bp, static_cast<Object*>(pOther) );
		return CAPISurface::create(obj);
	}


	//____ _convertBlueprint() ________________________________________________

	void CAPISurfaceFactory::_convertBlueprint(wg_surfaceBP* pDest, const Surface::Blueprint* pSource)
	{
		pDest->buffered = pSource->buffered;
		pDest->canvas = pSource->canvas;
		pDest->clut = (wg_color*)pSource->clut;
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
