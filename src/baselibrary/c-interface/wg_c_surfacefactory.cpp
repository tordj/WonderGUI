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

#include <wg_c_surfacefactory.h>
#include <wg_surfacefactory.h>

using namespace wg;

inline SurfaceFactory* getPtr(wg_obj obj) {
	return static_cast<SurfaceFactory*>(reinterpret_cast<Object*>(obj));
}


void convertBlueprint(Surface::Blueprint* pDest, const wg_surfaceBP* pSource)
{
	pDest->buffered = pSource->buffered;
	pDest->canvas = pSource->canvas;
	pDest->clut = (Color8*) pSource->clut;
	pDest->dynamic = pSource->dynamic;
	pDest->format = (PixelFormat) pSource->format;
	pDest->id = pSource->id;
	pDest->mipmap = pSource->mipmap;
	pDest->sampleMethod = (SampleMethod) pSource->sampleMethod;
	pDest->scale = pSource->scale;
	pDest->size.w = pSource->size.w;
	pDest->size.h = pSource->size.h;
	pDest->tiling = pSource->tiling;
}


wg_sizeI wg_maxSurfaceSize(wg_obj surfaceFactory)
{
	return *(wg_sizeI*) &getPtr(surfaceFactory)->maxSize();
}


wg_obj	createSurface(wg_obj factory, const wg_surfaceBP* blueprint)
{
	Surface::Blueprint	bp;
	convertBlueprint(&bp, blueprint);
	return (wg_obj) getPtr(factory)->createSurface(bp);
}


wg_obj	createSurface(wg_obj factory, const wg_surfaceBP* blueprint, wg_obj blob, int pitch)
{
	Surface::Blueprint	bp;
	convertBlueprint(&bp, blueprint);
	return (wg_obj)getPtr(factory)->createSurface(bp, static_cast<Blob*>(reinterpret_cast<Object*>(blob)), pitch );
}


wg_obj	createSurface(wg_obj factory, const wg_surfaceBP* blueprint, uint8_t* pPixels, int pitch, const wg_pixelDescription* pPixelDescription)
{
	Surface::Blueprint	bp;
	convertBlueprint(&bp, blueprint);
	return (wg_obj)getPtr(factory)->createSurface(bp, pPixels, pitch, (PixelDescription*) pPixelDescription);
}


wg_obj	createSurface(wg_obj factory, const wg_surfaceBP* blueprint, wg_obj fromSurface)
{
	Surface::Blueprint	bp;
	convertBlueprint(&bp, blueprint);
	return (wg_obj)getPtr(factory)->createSurface(bp, static_cast<Surface*>(reinterpret_cast<Object*>(fromSurface)));
}

