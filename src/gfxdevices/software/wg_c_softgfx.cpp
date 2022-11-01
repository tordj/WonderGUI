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

#include <wg_softkernels_default.h>
#include <wg_softkernels_rgb565be_base.h>
#include <wg_softkernels_rgb565be_extras.h>

using namespace wg;

wg_obj wg_createSoftGfxDevice()
{
	
	auto p = SoftGfxDevice::create();
	p->retain();
	return (wg_obj) static_cast<Object*>(p.rawPtr());
}

int	wg_addDefaultSoftKernels( wg_obj device )
{
	auto pDevice = static_cast<SoftGfxDevice*>(reinterpret_cast<Object*>(device));

	return addDefaultSoftKernels(pDevice);
}

int	wg_addBaseSoftKernelsForRGB565BECanvas( wg_obj device )
{
	auto pDevice = static_cast<SoftGfxDevice*>(reinterpret_cast<Object*>(device));

	return addBaseSoftKernelsForRGB565BECanvas(pDevice);
}

int	wg_addExtraSoftKernelsForRGB565BECanvas( wg_obj device )
{
	auto pDevice = static_cast<SoftGfxDevice*>(reinterpret_cast<Object*>(device));

	return addExtraSoftKernelsForRGB565BECanvas(pDevice);
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


wg_obj wg_createSoftSurfaceFromBitmap(const wg_surfaceBP* blueprint, uint8_t* pPixels, int pitch, const wg_pixelDescription* pPixelDescription)
{
	Surface::Blueprint	bp;
	convertSurfaceBlueprint(&bp, blueprint);
	auto p = SoftSurface::create(bp, pPixels, pitch, (PixelDescription*) pPixelDescription);
	p->retain();
	return (wg_obj) static_cast<Object*>(p.rawPtr());
}


wg_obj wg_createSoftSurfaceFromSurface(const wg_surfaceBP* blueprint, wg_obj fromSurface)
{
	Surface::Blueprint	bp;
	convertSurfaceBlueprint(&bp, blueprint);
	auto p = SoftSurface::create(bp, static_cast<Surface*>(reinterpret_cast<Object*>(fromSurface)));
	p->retain();
	return (wg_obj) static_cast<Object*>(p.rawPtr());
}


wg_obj wg_createSoftSurfaceFactory()
{
	auto p = SoftSurfaceFactory::create();
	p->retain();
	return (wg_obj) static_cast<Object*>(p.rawPtr());
}
