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



#include <wg_cabi.h>

namespace wg
{

	wg_bitmapcache_calls*		CABI::bitmapCache	= nullptr;
	wg_bitmapfont_calls*		CABI::bitmapFont	= nullptr;
	wg_canvaslayers_calls*		CABI::canvasLayers	= nullptr;
	wg_font_calls*				CABI::font			= nullptr;
	wg_object_calls*			CABI::object		= nullptr;
	wg_gfxdevice_calls*			CABI::gfxDevice		= nullptr;
	wg_streambuffer_calls*		CABI::streamBuffer	= nullptr;
	wg_streamplayer_calls*		CABI::streamPlayer	= nullptr;
	wg_streampump_calls*		CABI::streamPump	= nullptr;
	wg_streamreader_calls*		CABI::streamReader	= nullptr;
	wg_surface_calls*			CABI::surface		= nullptr;
	wg_surfacefactory_calls*	CABI::surfaceFactory= nullptr;


	//___ init() ______________________________________________________________

	bool CABI::init(wg_c_callCollection* pCallsCollection)
	{
		if (pCallsCollection->structSize < sizeof(wg_c_callCollection))
			goto	error_too_old_abi;


		if (pCallsCollection->pBitmapCache->structSize < sizeof(wg_bitmapcache_calls))
			goto	error_too_old_abi;

		if (pCallsCollection->pBitmapFont->structSize < sizeof(wg_bitmapfont_calls))
			goto	error_too_old_abi;

		if (pCallsCollection->pCanvasLayers->structSize < sizeof(wg_canvaslayers_calls))
			goto	error_too_old_abi;

		if (pCallsCollection->pFont->structSize < sizeof(wg_font_calls))
			goto	error_too_old_abi;

		if (pCallsCollection->pObject->structSize < sizeof(wg_object_calls))
			goto	error_too_old_abi;

		if (pCallsCollection->pGfxDevice->structSize < sizeof(wg_gfxdevice_calls))
			goto	error_too_old_abi;

		if (pCallsCollection->pStreamBuffer->structSize < sizeof(wg_streambuffer_calls))
			goto	error_too_old_abi;

		if (pCallsCollection->pStreamPlayer->structSize < sizeof(wg_streamplayer_calls))
			goto	error_too_old_abi;

		if (pCallsCollection->pStreamPump->structSize < sizeof(wg_streampump_calls))
			goto	error_too_old_abi;

		if (pCallsCollection->pStreamReader->structSize < sizeof(wg_streamreader_calls))
			goto	error_too_old_abi;

		if (pCallsCollection->pSurface->structSize < sizeof(wg_surface_calls))
			goto	error_too_old_abi;

		if (pCallsCollection->pSurfaceFactory->structSize < sizeof(wg_surfacefactory_calls))
			goto	error_too_old_abi;



		bitmapCache		= pCallsCollection->pBitmapCache;
		bitmapFont		= pCallsCollection->pBitmapFont;
		canvasLayers	= pCallsCollection->pCanvasLayers;
		font			= pCallsCollection ->pFont;
		object			= pCallsCollection->pObject;
		gfxDevice		= pCallsCollection->pGfxDevice;
		streamBuffer	= pCallsCollection->pStreamBuffer;
		streamPlayer	= pCallsCollection->pStreamPlayer;
		streamPump		= pCallsCollection->pStreamPump;
		surface			= pCallsCollection->pSurface;
		surfaceFactory = pCallsCollection->pSurfaceFactory;

		return true;

	error_too_old_abi:
		return false;
	}

}


