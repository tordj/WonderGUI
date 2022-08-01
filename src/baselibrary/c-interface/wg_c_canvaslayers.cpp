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

#include <wg_c_canvaslayers.h>
#include <wg_canvaslayers.h>
#include <wg_gfxdevice.h>

using namespace wg;

inline CanvasLayers* getPtr(wg_obj obj) {
	return static_cast<CanvasLayers*>(reinterpret_cast<Object*>(obj));
}

wg_obj wg_createCanvasLayers(const wg_canvasLayersBP* pBP)
{

	//TODO: Can we assign function directly without a lambda between? 
	// For this too work, the two casts below need to give the same result:
	// (void*) pDevice
	// reinterpret_cast<void*>(static_cast<Object*> pDevice);

	CanvasLayers::Blueprint bp;

	wg_canvasLayers_func func = pBP->canvasFinalizer;
	void*	pointer = pBP->canvasFinalizerPtr;
	int		integer = pBP->canvasFinalizerInt;
	if (func != NULL)
		bp.canvasFinalizer = [func,pointer,integer](GfxDevice* pDevice) { func(static_cast<Object*>(pDevice),pointer,integer); };

	func = pBP->canvasInitializer;
	pointer = pBP->canvasInitializerPtr;
	integer = pBP->canvasInitializerInt;
	if (func != NULL)
		bp.canvasInitializer = [func,pointer,integer](GfxDevice* pDevice) { func(static_cast<Object*>(pDevice),pointer,integer); };


	bp.defaultLayer = pBP->defaultLayer;

	for (int i = 0; i < WG_MAX_CANVAS_LAYERS && pBP->layers[i].format != WG_PIXFMT_UNDEFINED; i++)
	{
		CanvasLayers::LayerBP layer;

		layer.format = (PixelFormat) pBP->layers[i].format;

		func = pBP->layers[i].canvasPreparer;
		pointer = pBP->layers[i].canvasPreparerPtr;
		integer = pBP->layers[i].canvasPreparerInt;
		if( func != NULL )
			layer.canvasPreparer = [func,pointer,integer](GfxDevice* pDevice) { func(static_cast<Object*>(pDevice), pointer, integer); };

		func = pBP->layers[i].initializer;
		pointer = pBP->layers[i].initializerPtr;
		integer = pBP->layers[i].initializerInt;
		if (func != NULL)
			layer.initializer = [func, pointer, integer](GfxDevice* pDevice) { func(static_cast<Object*>(pDevice), pointer, integer); };

		func = pBP->layers[i].blender;
		pointer = pBP->layers[i].blenderPtr;
		integer = pBP->layers[i].blenderInt;
		if (func != NULL)
			layer.blender = [func, pointer, integer](GfxDevice* pDevice) { func(static_cast<Object*>(pDevice), pointer, integer); };

		func = pBP->layers[i].finalizer;
		pointer = pBP->layers[i].finalizerPtr;
		integer = pBP->layers[i].finalizerInt;
		if (func != NULL)
			layer.finalizer = [func, pointer, integer](GfxDevice* pDevice) { func(static_cast<Object*>(pDevice), pointer, integer); };

		bp.layers.push_back(layer);
	}

	return static_cast<Object*>(CanvasLayers::create(bp));
}

int wg_canvasLayersSize(wg_obj canvasLayers)
{
	return getPtr(canvasLayers)->size();
}

wg_pixelFormat wg_layerFormat(wg_obj canvasLayers, int layer)
{
	return (wg_pixelFormat) getPtr(canvasLayers)->layerFormat(layer);
}

int wg_defaultLayer(wg_obj canvasLayers)
{
	return getPtr(canvasLayers)->defaultLayer();
}