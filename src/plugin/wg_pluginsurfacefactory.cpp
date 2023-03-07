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

#include <wg_plugincalls.h>
#include <wg_pluginsurface.h>
#include <wg_pluginsurfacefactory.h>


namespace wg
{

	const TypeInfo PluginSurfaceFactory::TYPEINFO = { "PluginSurfaceFactory", &SurfaceFactory::TYPEINFO };

	//____ constructor ________________________________________________________

	PluginSurfaceFactory::PluginSurfaceFactory( wg_obj cFactory) : m_cFactory(cFactory)
	{
		PluginCalls::object->retain(cFactory);
	}

	//____ destructor ________________________________________________________

	PluginSurfaceFactory::~PluginSurfaceFactory()
	{
		PluginCalls::object->release(m_cFactory);
	}


	//____ typeInfo() _________________________________________________________

	const TypeInfo& PluginSurfaceFactory::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ maxSize() ______________________________________________________________

	SizeI PluginSurfaceFactory::maxSize() const
	{			
		wg_sizeI sz = PluginCalls::surfaceFactory->maxSurfaceSize(m_cFactory);
		return { sz.w, sz.h };
	}

	//____ createSurface() ________________________________________________________

	Surface_p PluginSurfaceFactory::createSurface(const Surface::Blueprint& blueprint)
	{
		wg_surfaceBP	bp;
		_convertBlueprint(&bp, &blueprint);

		auto obj = PluginCalls::surfaceFactory->createSurface(m_cFactory, &bp);
		return PluginSurface::create(obj);
	}

	Surface_p PluginSurfaceFactory::createSurface(const Surface::Blueprint& blueprint, Blob * pBlob, int pitch )
	{
		wg_surfaceBP	bp;
		_convertBlueprint(&bp, &blueprint);

		auto obj = PluginCalls::surfaceFactory->createSurfaceFromBlob(m_cFactory, &bp, static_cast<Object*>(pBlob), pitch );
		return PluginSurface::create(obj);
	}


	Surface_p PluginSurfaceFactory::createSurface(const Surface::Blueprint& blueprint, const uint8_t* pPixels,
		PixelFormat format, int pitch, const Color8* pPalette)
	{
		wg_surfaceBP	bp;
		_convertBlueprint(&bp, &blueprint);

		auto obj = PluginCalls::surfaceFactory->createSurfaceFromBitmap(m_cFactory, &bp, pPixels, wg_pixelFormat(format), pitch, (const wg_color8*) pPalette);
		return PluginSurface::create(obj);
	}

	Surface_p PluginSurfaceFactory::createSurface(const Surface::Blueprint& blueprint, const uint8_t* pPixels,
		const PixelDescription& pixelDescription, int pitch, const Color8* pPalette)
	{
		wg_surfaceBP	bp;
		_convertBlueprint(&bp, &blueprint);

		auto obj = PluginCalls::surfaceFactory->createSurfaceFromRawData(m_cFactory, &bp, pPixels, 
					(const wg_pixelDescription*) &pixelDescription, pitch, (const wg_color8*)pPalette);
		return PluginSurface::create(obj);
	}

	//____ _convertBlueprint() ________________________________________________

	void PluginSurfaceFactory::_convertBlueprint(wg_surfaceBP* pDest, const Surface::Blueprint* pSource)
	{
		pDest->buffered = pSource->buffered;
		pDest->canvas = pSource->canvas;
		pDest->palette = (wg_color8*)pSource->palette;
		pDest->dynamic = pSource->dynamic;
		pDest->format = (wg_pixelFormat)pSource->format;
		pDest->identity = pSource->identity;
		pDest->mipmap = pSource->mipmap;
		pDest->sampleMethod = (wg_sampleMethod)pSource->sampleMethod;
		pDest->scale = pSource->scale;
		pDest->size.w = pSource->size.w;
		pDest->size.h = pSource->size.h;
		pDest->tiling = pSource->tiling;
	}

} // namespace wg
