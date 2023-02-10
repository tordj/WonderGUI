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
PluginHostBridge *	WgPluginBase::s_pHostBridge = nullptr;




//___ init() ______________________________________________________________

bool WgPluginBase::init(wg_plugin_interface* pCallsCollection, void * pRealHostBridge)
{
	if( s_pluginInitCounter > 0 )
	{
		s_pluginInitCounter++;
		return true;
	}
		
	if( !PluginCalls::_init(pCallsCollection) )
		return false;								// Host has too old ABI!
	
	s_pHostBridge = new PluginHostBridge(pRealHostBridge);
			
	WgBase::Init(s_pHostBridge);
	
	s_pluginInitCounter = 1;
	return true;

error_too_old_abi:
	return false;
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

	delete s_pHostBridge;
	s_pHostBridge = nullptr;

	s_pluginInitCounter = 0;
	return true;
}
