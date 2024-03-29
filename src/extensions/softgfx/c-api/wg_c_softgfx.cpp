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

#include <wg_c_softgfx.h>
#include <wg_c_internal.h>
#include <wg_softsurface.h>
#include <wg_softgfxdevice.h>
#include <wg_softsurfacefactory.h>

using namespace wg;

wg_obj wg_createSoftGfxDevice()
{
	
	auto p = SoftGfxDevice::create();
	p->retain();
	return (wg_obj) static_cast<Object*>(p.rawPtr());
}


int wg_defineSoftGfxDeviceCanvas( wg_obj device, wg_canvasRef ref, wg_obj softSurface )
{
	auto pDevice = static_cast<SoftGfxDevice*>(reinterpret_cast<Object*>(device));
	auto pSurface = static_cast<SoftSurface*>(reinterpret_cast<Object*>(softSurface));

	return pDevice->defineCanvas((CanvasRef) ref, pSurface);
}


wg_obj wg_createSoftSurface(const wg_surfaceBP* blueprint)
{
	Surface::Blueprint	bp;
	convertSurfaceBlueprint(&bp, blueprint);

	auto p = SoftSurface::create(bp);
	p->retain();
	return (wg_obj) static_cast<Object*>(p.rawPtr());
}


wg_obj wg_createSoftSurfaceFromBlob(const wg_surfaceBP* blueprint, wg_obj blob, int pitch)
{
	Surface::Blueprint	bp;
	convertSurfaceBlueprint(&bp, blueprint);
	auto p = SoftSurface::create(bp, static_cast<Blob*>(reinterpret_cast<Object*>(blob)), pitch );
	p->retain();
	return (wg_obj) static_cast<Object*>(p.rawPtr());
}

wg_obj	wg_createSoftSurfaceFromBitmap(wg_obj factory, const wg_surfaceBP* blueprint, const uint8_t* pPixels,
									   wg_pixelFormat pixelFormat, int pitch, const wg_color8 * pPalette )
{
	Surface::Blueprint	bp;
	convertSurfaceBlueprint(&bp, blueprint);
	auto p = SoftSurface::create(bp, pPixels, (PixelFormat) pixelFormat, pitch, (const Color8*) pPalette);
	p->retain();
	return (wg_obj) static_cast<Object*>(p.rawPtr());
}

wg_obj	wg_createSoftSurfaceFromRawData(wg_obj factory, const wg_surfaceBP* blueprint, const uint8_t* pPixels,
										const wg_pixelDescription * pPixelDescription, int pitch, const wg_color8 * pPalette )
{
	Surface::Blueprint	bp;
	convertSurfaceBlueprint(&bp, blueprint);
	
	auto p = SoftSurface::create(bp, pPixels, *(PixelDescription*) pPixelDescription, pitch, (const Color8*) pPalette);
	p->retain();
	return (wg_obj) static_cast<Object*>(p.rawPtr());
}


wg_obj wg_createSoftSurfaceFactory()
{
	auto p = SoftSurfaceFactory::create();
	p->retain();
	return (wg_obj) static_cast<Object*>(p.rawPtr());
}
