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

#ifndef WG_C_CANVASLAYERS_DOT_H
#define WG_C_CANVASLAYERS_DOT_H
#pragma once

#include <wg_c_types.h>

#ifdef __cplusplus
extern "C" {
#endif


#define		WG_MAX_CANVAS_LAYERS	16							// Should be synced with CanvasLayers::c_maxLayers.
	typedef	void(*wg_canvasLayers_func)(wg_obj gfxDevice, void * pData, int data );


	struct wg_canvasLayersEntryBP
	{
		wg_pixelFormat			format;
		wg_canvasLayers_func	canvasPreparer;
		void*					canvasPreparerPtr;
		int						canvasPreparerInt;
		wg_canvasLayers_func	initializer;
		void*					initializerPtr;
		int						initializerInt;
		wg_canvasLayers_func	finalizer;
		void*					finalizerPtr;
		int						finalizerInt;
		wg_canvasLayers_func	blender;
		void*					blenderPtr;
		int						blenderInt;
	};


	struct wg_canvasLayersBP
	{
		wg_canvasLayers_func	canvasInitializer;
		void*					canvasInitializerPtr;
		int						canvasInitializerInt;
		wg_canvasLayers_func	canvasFinalizer;
		void*					canvasFinalizerPtr;
		int						canvasFinalizerInt;

		int						defaultLayer;
		wg_canvasLayersEntryBP	layers[];				// Need to end list with entry where format = WG_PIXFMT_UNDEFINED unless full.

	};

	wg_obj			wg_createCanvasLayers(const wg_canvasLayersBP* pBlueprint);

	int				wg_canvasLayersSize(wg_obj canvasLayers);
	wg_pixelFormat	wg_layerFormat(wg_obj canvasLayers, int layer);
	int				wg_defaultLayer(wg_obj canvasLayers);


#ifdef __cplusplus
}
#endif

#endif