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

#include <wg_c_abi.h>

#include <wg_c_canvaslayers.h>
#include <wg_c_object.h>

void wg_populateCallStruc(wg_c_calls_header * pHeader, wg_c_calls_body * pBody)
{
	pBody->canvasLayers.structSize = sizeof(wg_canvaslayers_calls);
	pBody->canvasLayers.baseLayer			= &wg_baseLayer;
	pBody->canvasLayers.canvasLayersSize	= &wg_canvasLayersSize;
	pBody->canvasLayers.createCanvasLayers= &wg_createCanvasLayers;
	pBody->canvasLayers.layerFormat		= &wg_layerFormat;


	pBody->object.structSize = sizeof(wg_object_calls);
	pBody->object.finalizer		= &wg_finalizer;
	pBody->object.getTypeInfo		= &wg_getTypeInfo;
	pBody->object.isInstanceOf	= &wg_isInstanceOf;
	pBody->object.refcount		= &wg_refcount;
	pBody->object.release			= &wg_release;
	pBody->object.retain			= &wg_retain;
	pBody->object.setFinalizer	= &wg_setFinalizer;


	pHeader->structSize = sizeof(wg_c_calls_header);
	pHeader->pBitmapCache = &pBody->bitmapCache;
	pHeader->pBitmapFont = &pBody->bitmapFont;
	pHeader->pCanvasLayers = &pBody->canvasLayers;
	pHeader->pComponent = &pBody->component;
	pHeader->pFont = &pBody->font;
	pHeader->pObject = &pBody->object;

}

