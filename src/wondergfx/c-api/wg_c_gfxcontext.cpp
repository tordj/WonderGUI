
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

#include <wg_c_gfxcontext.h>
#include <wg_gfxcontext.h>

using namespace wg;

inline GfxContext* getPtr(wg_obj obj) {
	return static_cast<GfxContext*>(reinterpret_cast<Object*>(obj));
}

wg_obj wg_createGfxContext()
{
	auto pContext = GfxContext::create();
	pContext->retain();
	return static_cast<Object*>(pContext);
}
wg_obj wg_defaultBitmapCache( wg_obj context )
{
	BitmapCache* pCache = getPtr(context)->defaultBitmapCache();
	return (wg_obj) static_cast<Object*>(pCache);
}

void wg_setDefaultSurfaceFactory( wg_obj context, wg_obj factory )
{
	SurfaceFactory* pFactory = static_cast<SurfaceFactory*>(reinterpret_cast<Object*>(factory));
	return getPtr(context)->setDefaultSurfaceFactory(pFactory);
}

wg_obj wg_defaultSurfaceFactory(wg_obj context)
{
	SurfaceFactory * pFactory = getPtr(context)->defaultSurfaceFactory();
	return (wg_obj) static_cast<Object*>(pFactory);
}

void wg_setDefaultGfxDevice( wg_obj context, wg_obj device )
{
	GfxDevice* pDevice = static_cast<GfxDevice*>(reinterpret_cast<Object*>(device));
	return getPtr(context)->setDefaultGfxDevice(pDevice);
}

wg_obj wg_defaultGfxDevice(wg_obj context)
{
	GfxDevice * pDevice = getPtr(context)->defaultGfxDevice();
	return (wg_obj) static_cast<Object*>(pDevice);
}

void wg_setDefaultToSRGB( wg_obj context, int bSRGB )
{
	getPtr(context)->setDefaultToSRGB(bSRGB);
}

int wg_defaultToSRGB(wg_obj context)
{
	return getPtr(context)->defaultToSRGB();
}
