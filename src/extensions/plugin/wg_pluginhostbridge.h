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

#ifndef	WG_PLUGINHOSTBRIDGE_DOT_H
#define	WG_PLUGINHOSTBRIDGE_DOT_H
#pragma once

#include <wg_hostbridge.h>

#include <vector>


namespace wg
{

	class Root;

	//____ PluginHostBridge _____________________________________________________

	class PluginHostBridge : public HostBridge
	{
	public:
		
		PluginHostBridge(void * pHostBridge) : m_pHostBridge(pHostBridge) {}
		// Needed to avoid compiler warnings/errors, as HostBridge has virtual functions
		virtual ~PluginHostBridge() = default;

		
		bool		hidePointer() override;
		bool		showPointer() override;

		bool		setPointerStyle( PointerStyle style ) override;
		
		std::string	getClipboardText() override;
		bool		setClipboardText(const std::string& text) override;
		
		bool		requestFocus(uintptr_t windowRef) override;
		bool		yieldFocus(uintptr_t windowRef) override;
		bool		lockHidePointer() override;
		bool		unlockShowPointer() override;
		
	protected:

		void * m_pHostBridge;
	};
}


#endif //WG_PLUGINHOSTBRIDGE_DOT_H

