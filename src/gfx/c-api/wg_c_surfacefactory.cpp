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
#include <wg_c_internal.h>
#include <wg_surfacefactory.h>

using namespace wg;

inline SurfaceFactory* getPtr(wg_obj obj) {
	return static_cast<SurfaceFactory*>(reinterpret_cast<Object*>(obj));
}




wg_sizeI wg_maxSurfaceSize(wg_obj surfaceFactory)
{
    SizeI size = getPtr(surfaceFactory)->maxSize();
    return *(wg_sizeI*) &size;
}


wg_obj wg_createSurface(wg_obj factory, const wg_surfaceBP* blueprint)
{
	Surface::Blueprint	bp;
	convertSurfaceBlueprint(&bp, blueprint);
	
	auto p = getPtr(factory)->createSurface(bp);
	p->retain();
	return (wg_obj) static_cast<Object*>(p.rawPtr());
}


wg_obj wg_createSurfaceFromBlob(wg_obj factory, const wg_surfaceBP* blueprint, wg_obj blob, int pitch)
{
	Surface::Blueprint	bp;
	convertSurfaceBlueprint(&bp, blueprint);
	auto p = getPtr(factory)->createSurface(bp, static_cast<Blob*>(reinterpret_cast<Object*>(blob)), pitch );
	p->retain();
	return (wg_obj) static_cast<Object*>(p.rawPtr());
}

wg_obj	wg_createSurfaceFromRawData(wg_obj factory, const wg_surfaceBP* blueprint, const uint8_t* pPixels,
									const wg_pixelDescription2 * pPixelDescription, int pitch, const wg_color8 * pPalette );

wg_obj	wg_createSurfaceFromBitmap(wg_obj factory, const wg_surfaceBP* blueprint, const uint8_t* pPixels,
								   wg_pixelFormat pixelFormat, int pitch, const wg_color8 * pPalette )
{
	Surface::Blueprint	bp;
	convertSurfaceBlueprint(&bp, blueprint);
	auto p = getPtr(factory)->createSurface(bp, pPixels, (PixelFormat) pixelFormat, pitch, (Color8*) pPalette);
	p->retain();
	return (wg_obj) static_cast<Object*>(p.rawPtr());
}

wg_obj	wg_createSurfaceFromRawData(wg_obj factory, const wg_surfaceBP* blueprint, const uint8_t* pPixels,
									const wg_pixelDescription2 * pPixelDescription, int pitch, const wg_color8 * pPalette )
{
	Surface::Blueprint	bp;
	convertSurfaceBlueprint(&bp, blueprint);
	auto p = getPtr(factory)->createSurface(bp, pPixels, *(PixelDescription2*)pPixelDescription, pitch, (Color8*) pPalette);
	p->retain();
	return (wg_obj) static_cast<Object*>(p.rawPtr());
}

