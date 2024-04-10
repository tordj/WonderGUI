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

#ifndef	WG2_PLUGINBASE_DOT_H
#define	WG2_PLUGINBASE_DOT_H
#pragma once

#include <wg_plugininterface.h>
#include <wg_pluginhostbridge.h>
#include <wg2_base.h>



class WgPluginContext : public wg::Object
{
	~WgPluginContext() { delete pHostBridge; }
	
	friend class WgPluginBase;
	
	wg_plugin_interface 	callsCollection;
	wg::PluginHostBridge *	pHostBridge;
	
	WgContext_p				pBaseContext;
};

typedef	wg::StrongPtr<WgPluginContext>	WgPluginContext_p;
typedef	wg::WeakPtr<WgPluginContext>	WgPluginContext_wp;



class WgPluginBase : public WgBase
{
public:
	
	//.____ Creation __________________________________________
	
	static bool init( wg_plugin_interface* pInterface, void * pRealHostBridge );
	static bool exit();
	
	//.____ Misc ________________________________________________
	
	static WgPluginContext_p	setContext( const WgPluginContext_p& pNewContext );
	static WgPluginContext_p	context() { return s_pContext; };

	
	static bool			isInitialized() { return s_pluginInitCounter > 0; }
	
private:
	static	int				s_pluginInitCounter;
	
	static WgPluginContext_p	s_pContext;
	
};



#endif //WG2_PLUGINBASE_DOT_H
