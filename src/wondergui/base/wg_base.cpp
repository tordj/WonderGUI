
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


#include <wg_base.h>

#include <wg_msgrouter.h>
#include <wg_basictextlayout.h>
#include <wg_basicnumberlayout.h>
#include <wg_inputhandler.h>

#include <wg_dummyfont.h>
#include <wg_memstack.h>
#include <wg_mempool.h>
#include <wg_textstyle.h>
#include <wg_texttool.h>
#include <wg_textstylemanager.h>
#include <wg_skinslotmanager.h>
#include <wg_bitmapcache.h>
#include <wg_packlayout.h>

#include <iostream>
#include <algorithm>

namespace wg
{
	int				Base::s_guiInitCounter = 0;
	GUIContext_p	Base::s_pContext;
	String			Base::s_clipboardText;
	HostBridge*		Base::s_pHostBridge = nullptr;
	int64_t			Base::s_timestamp = 0;

	std::vector<Receiver*>		Base::s_updateReceivers;


	//____ init() __________________________________________________________________

	bool Base::init( HostBridge * pHostBridge, GUIContext * pContext )
	{
		if( s_guiInitCounter > 0 )
		{
			if( pContext )
				s_pContext = pContext;
			
			s_guiInitCounter++;
			return true;
		}

		if( !pContext )
			pContext = GUIContext::create();
		
		if( !GfxBase::init(pContext) )
			return false;
		
		s_pHostBridge = pHostBridge;

		TextTool::setDefaultBreakRules();

		TextStyleManager::init();
		SkinSlotManager::init();

		s_pContext = pContext;
		s_guiInitCounter = 1;
		return true;
	}

	//____ exit() __________________________________________________________________

	bool Base::exit()
	{
		if( s_guiInitCounter <= 0 )
		{
			throwError(ErrorLevel::SilentError, ErrorCode::IllegalCall, "Call to Base::exit() ignored, not initialized or already exited.", nullptr, nullptr, __func__, __FILE__, __LINE__);
			return false;
		}
		
		if( s_guiInitCounter > 1 )
		{
			s_guiInitCounter--;
			return true;
		}
	
		s_clipboardText.clear();

		s_pHostBridge = nullptr;

		s_timestamp = 0;
		s_updateReceivers.clear();

		SkinSlotManager::exit();
		TextStyleManager::exit();

		s_guiInitCounter = 0;
		s_pContext = nullptr;

		return GfxBase::exit();
	}

	//____ setContext() __________________________________________________________

	bool Base::setContext( GUIContext * pContext )
	{
		s_pContext = pContext;
		GfxBase::s_pContext = pContext;
		GearBase::s_pContext = pContext;
		
		TextStyle::s_pDefaultStyle = s_pContext->defaultStyle();
		return true;
	}

	//____ setClipboardText() ____________________________________________________

	void Base::setClipboardText( const String& text )
	{
		s_clipboardText = text;
		if( s_pHostBridge )
		{
			auto stdString = CharSeq(text).getStdString();
			
			s_pHostBridge->setClipboardText(stdString);
		}
	}

	//____ getClipboardText() ____________________________________________________

	String Base::getClipboardText()
	{
		if( s_pHostBridge )
		{
			auto stdString = s_pHostBridge->getClipboardText();
			
			return String(stdString);
		}
		else
		{
			return s_clipboardText;
		}
	}

	//____ update() ______________________________________________________________

	void Base::update( int64_t timestamp )
	{
		int microPassed = int(timestamp - s_timestamp);
		s_timestamp = timestamp;

		// Update wondergui systems

		s_pContext->inputHandler()->_update(timestamp/1000);
        SkinSlotManager::update(microPassed/1000);

		// Update widgets
		for (auto pReceiver : s_updateReceivers)
			pReceiver->_update(microPassed, timestamp);
	}


	//____ _startReceiveUpdates() ________________________________________________

	int64_t Base::_startReceiveUpdates(Receiver* pReceiver)
	{
		s_updateReceivers.push_back(pReceiver);
		return s_timestamp;
	}

	//____ _stopReceiveUpdates() _________________________________________________

	void Base::_stopReceiveUpdates(Receiver* pReceiver)
	{
		s_updateReceivers.erase(std::remove(s_updateReceivers.begin(), s_updateReceivers.end(), pReceiver));
	}


} // namespace wg
