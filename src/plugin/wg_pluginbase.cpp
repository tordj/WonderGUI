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
#include <wg_plugincalls.h>

namespace wg
{

	const TypeInfo	PluginBase::TYPEINFO = { "PluginBase", &Base::TYPEINFO };

	int					PluginBase::s_pluginInitCounter = 0;
	PluginHostBridge *	PluginBase::s_pHostBridge = nullptr;


	//___ init() ______________________________________________________________

	bool PluginBase::init(wg_plugin_interface* pCallsCollection, void * pRealHostBridge)
	{
		if( s_pluginInitCounter > 0 )
		{
			s_pluginInitCounter++;
			return true;
		}
		
		if( !PluginCalls::_init(pCallsCollection) )
			return false;								// Host has too old ABI.
		
		s_pHostBridge = new PluginHostBridge(pRealHostBridge);
				
		Base::init(s_pHostBridge);
		
		s_pluginInitCounter = 1;
		return true;
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


