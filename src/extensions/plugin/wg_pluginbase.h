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

#ifndef	WG_PLUGINBASE_DOT_H
#define	WG_PLUGINBASE_DOT_H
#pragma once

#include <wg_plugininterface.h>
#include <wg_pluginhostbridge.h>
#include <wg_base.h>

namespace wg
{

	class PluginBase : public Base
	{
	public:

		//.____ Creation __________________________________________

		static bool init( wg_plugin_interface* pInterface, void * pRealHostBridge );
		static bool exit();

		//.____ Misc ________________________________________________

		const static TypeInfo	TYPEINFO;

		static bool			isInitialized() { return s_pluginInitCounter > 0; }

	private:
		static	int				s_pluginInitCounter;

		static	PluginHostBridge *	s_pHostBridge;
	};


} // namespace wg
#endif //WG_PLUGINBASE_DOT_H
