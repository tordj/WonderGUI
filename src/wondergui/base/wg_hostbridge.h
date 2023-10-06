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
#ifndef WG_HOSTBRIDGE_DOT_H
#define WG_HOSTBRIDGE_DOT_H
#pragma once

#include <string>

namespace wg
{
	/**
		@brief Visitor class for WG to make callbacks to host platform.

		HostBridge is an abstract class used by WonderGUI to make callbacks
		to host platform. It is provided as a parameter to Base::init() and
		used throughout the GUI lifecycle until Base::exit() is called.
	*/

	class HostBridge
	{
	public:
		virtual bool		hidePointer() = 0;
		virtual bool		showPointer() = 0;
		
		virtual std::string	getClipboardText() = 0;
		virtual bool		setClipboardText(const std::string& text) = 0;
		
		virtual bool		requestFocus(uintptr_t windowRef) = 0;
		virtual bool		yieldFocus(uintptr_t windowRef) = 0;
	};
}



#endif //WG_HOSTBRIDGE_DOT_H
