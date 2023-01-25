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



#include <wg_pluginbase.h>

namespace wg
{

	const TypeInfo	PluginBase::TYPEINFO = { "PluginBase", &Base::TYPEINFO };

	int					PluginBase::s_pluginInitCounter = 0;
	PluginHostBridge *	PluginBase::s_pHostBridge = nullptr;



	wg_bitmapcache_calls*		PluginBase::bitmapCache		= nullptr;
	wg_bitmapfont_calls*		PluginBase::bitmapFont		= nullptr;
	wg_canvaslayers_calls*		PluginBase::canvasLayers 	= nullptr;
	wg_font_calls*				PluginBase::font			= nullptr;
	wg_object_calls*			PluginBase::object			= nullptr;
	wg_gfxdevice_calls*			PluginBase::gfxDevice		= nullptr;
	wg_streambuffer_calls*		PluginBase::streamBuffer	= nullptr;
	wg_streamplayer_calls*		PluginBase::streamPlayer	= nullptr;
	wg_streampump_calls*		PluginBase::streamPump		= nullptr;
	wg_streamreader_calls*		PluginBase::streamReader	= nullptr;
	wg_surface_calls*			PluginBase::surface			= nullptr;
	wg_surfacefactory_calls*	PluginBase::surfaceFactory 	= nullptr;
	wg_hostbridge_calls*		PluginBase::hostBridge		= nullptr;
	wg_plugincapsule_calls*		PluginBase::pluginCapsule	= nullptr;


	//___ init() ______________________________________________________________

	bool PluginBase::init(wg_plugin_interface* pCallsCollection, void * pRealHostBridge)
	{
		if( s_pluginInitCounter > 0 )
		{
			s_pluginInitCounter++;
			return true;
		}
			
		if (pCallsCollection->structSize < sizeof(wg_plugin_interface))
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

		if (pCallsCollection->pHostBridge->structSize < sizeof(wg_hostbridge_calls))
			goto	error_too_old_abi;

		if (pCallsCollection->pPluginCapsule->structSize < sizeof(wg_plugincapsule_calls))
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
		surfaceFactory	= pCallsCollection->pSurfaceFactory;
		hostBridge		= pCallsCollection->pHostBridge;
		pluginCapsule	= pCallsCollection->pPluginCapsule;

		s_pHostBridge = new PluginHostBridge(pRealHostBridge);
				
		Base::init(s_pHostBridge);
		
		s_pluginInitCounter = 1;
		return true;

	error_too_old_abi:
		return false;
	}

	//____ exit() __________________________________________________________________

	bool PluginBase::exit()
	{
		if( s_pluginInitCounter <= 0 )
		{
			handleError(ErrorSeverity::SilentFail, ErrorCode::IllegalCall, "Call to PluginBase::exit() ignored, not initialized or already exited.", nullptr, &TYPEINFO, __func__, __FILE__, __LINE__);
			return false;
		}
		
		if( s_pluginInitCounter > 1 )
		{
			s_pluginInitCounter--;			// This belongs to GfxBase, but we do like this anyway.
			return true;
		}

		bool res = Base::exit();

		delete s_pHostBridge;
		s_pHostBridge = nullptr;

		s_pluginInitCounter = 0;
		return res;
	}


}


