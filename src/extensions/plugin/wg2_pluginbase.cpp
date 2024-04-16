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



#include <wg2_pluginbase.h>
#include <wg_plugincalls.h>

using namespace wg;


int					WgPluginBase::s_pluginInitCounter = 0;
WgPluginContext_p	WgPluginBase::s_pContext;




//___ init() ______________________________________________________________

bool WgPluginBase::init(wg_plugin_interface* pCallsCollection, void * pRealHostBridge,
						std::function<void(WgPluginContext *)> contextSwitchCallback )
{
	// Plugin initialization works differently from GearBase/GfxBase/Base in that a
	// new context is setup for each call to init.
	
	if( !PluginCalls::_init(pCallsCollection) )
		return false;								// Host has too old ABI!

	auto pHostBridge = new PluginHostBridge(pRealHostBridge);
	
	if( s_pluginInitCounter == 0 )
		WgBase::Init(pHostBridge);
	else
		WgBase::setContext(nullptr);

	s_pContext = new WgPluginContext();
	
	s_pContext->pBaseContext = WgBase::context();
	s_pContext->callsCollection = * pCallsCollection;
	s_pContext->pHostBridge = pHostBridge;
	s_pContext->contextSwitchCallback = contextSwitchCallback;
	
	s_pluginInitCounter++;
	return true;
}

//____ exit() __________________________________________________________________

bool WgPluginBase::exit()
{
	if( s_pluginInitCounter <= 0 )
	{
		throwError(ErrorLevel::SilentError, ErrorCode::IllegalCall, "Call to WgPluginBase::exit() ignored, not initialized or already exited.", nullptr, &TYPEINFO, __func__, __FILE__, __LINE__);
		return false;
	}
	
	if( s_pluginInitCounter > 1 )
	{
		s_pluginInitCounter--;			// This belongs to GfxBase, but we do like this anyway.
		return true;
	}

	WgBase::Exit();

	s_pContext = nullptr;

	s_pluginInitCounter = 0;
	return true;
}

//____ setContext() ________________________________________________________

WgPluginContext_p WgPluginBase::setContext( const WgPluginContext_p& pNewContext )
{
	// Set context for plugins works differently than for GearBase/GfxBase/Base.
	// If you want a new context you should call init() again.
	
	if( pNewContext == nullptr )
		return s_pContext;
	
	auto pOldContext = s_pContext;
	
	s_pContext = pNewContext;
	WgBase::setContext(pNewContext->pBaseContext);
	
	PluginCalls::_init(&pNewContext->callsCollection);
	
	if( pNewContext->contextSwitchCallback )
		pNewContext->contextSwitchCallback(pNewContext);
	
	return pOldContext;
}
