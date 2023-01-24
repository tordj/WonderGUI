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


#include <wg_cabi_hostbridge.h>
#include <wg_cabi_base.h>

namespace wg
{

	//____ hidePointer() __________________________________________________________

	bool CABIHostBridge::hidePointer()
	{
		return CABI::hostBridge->hidePointer(m_pHostBridge);
	}

	//____ showPointer() __________________________________________________________

	bool CABIHostBridge::showPointer()
	{
		return CABI::hostBridge->showPointer(m_pHostBridge);
	}

	//____ getClipboardText() _____________________________________________________

	std::string CABIHostBridge::getClipboardText()
	{
		char temp[4096];
		
		int len = CABI::hostBridge->getClipboardText(m_pHostBridge, 4096, temp );
		return temp;
	}

	//____ setClipboardText() _____________________________________________________

	bool CABIHostBridge::setClipboardText(const std::string& text)
	{
		return CABI::hostBridge->setClipboardText(m_pHostBridge,text.c_str());
	}

} // namespace wg