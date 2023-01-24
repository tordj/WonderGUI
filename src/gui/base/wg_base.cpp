
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

#include <iostream>
#include <algorithm>

namespace wg
{
	const TypeInfo	Base::TYPEINFO = { "Base", &GfxBase::TYPEINFO };

	int				Base::s_guiInitCounter = 0;

	MsgRouter_p		Base::s_pMsgRouter;
	InputHandler_p	Base::s_pInputHandler;
	TextLayout_p	Base::s_pDefaultTextLayout;
	Caret_p			Base::s_pDefaultCaret;
	NumberLayout_p	Base::s_pDefaultNumberLayout;
	TextStyle_p		Base::s_pDefaultStyle;

	String			Base::s_clipboardText;

	HostBridge*					Base::s_pHostBridge = nullptr;

	int64_t						Base::s_timestamp = 0;
	std::vector<Receiver*>		Base::s_updateReceivers;




	//____ init() __________________________________________________________________

	bool Base::init( HostBridge * pHostBridge )
	{
		if( s_guiInitCounter > 0 )
		{
			s_guiInitCounter++;
			return true;
		}

		if( !GfxBase::init() )
			return false;
		
		s_pHostBridge = pHostBridge;

		TextTool::setDefaultBreakRules();
		HiColor::_initTables();


		TextStyleManager::init();
		SkinSlotManager::init();

		TextStyle::Blueprint textStyleBP;
		textStyleBP.font = DummyFont::create();

		s_pDefaultStyle = TextStyle::create( textStyleBP );

		s_pDefaultCaret = Caret::create();

		s_pDefaultTextLayout = BasicTextLayout::create({});


		s_pDefaultNumberLayout = BasicNumberLayout::create( BasicNumberLayout::Blueprint() );

		s_pMsgRouter = MsgRouter::create();
      	s_pInputHandler = InputHandler::create();

		s_guiInitCounter = 1;
		return true;
	}

	//____ exit() __________________________________________________________________

	bool Base::exit()
	{
		if( s_guiInitCounter <= 0 )
		{
			handleError(ErrorSeverity::SilentFail, ErrorCode::IllegalCall, "Call to GuiBase::exit() ignored, not initialized or already exited.", nullptr, &TYPEINFO, __func__, __FILE__, __LINE__);
			return false;
		}
		
		if( s_guiInitCounter > 1 )
		{
			s_guiInitCounter--;			// This belongs to GfxBase, but we do like this anyway.
			return true;
		}
	
		s_pMsgRouter = nullptr;
		s_pInputHandler = nullptr;
		s_pDefaultTextLayout = nullptr;
		s_pDefaultCaret = nullptr;
		s_pDefaultNumberLayout = nullptr;
		s_pDefaultStyle = nullptr;

		s_clipboardText.clear();

		s_pHostBridge = nullptr;

		s_timestamp = 0;
		s_updateReceivers.clear();

		SkinSlotManager::exit();
		TextStyleManager::exit();

		s_guiInitCounter = 0;
		return GfxBase::exit();
	}


	//____ msgRouter() _________________________________________________________

	MsgRouter_p	Base::msgRouter()
	{
		return s_pMsgRouter;
	}

	//____ inputHandler() ______________________________________________________

	InputHandler_p Base::inputHandler()
	{
		return s_pInputHandler;
	}

	//____ defaultCaret() ______________________________________________________

	Caret_p Base::defaultCaret()
	{
		return s_pDefaultCaret;
	}

	//____ setDefaultCaret() ___________________________________________________

	void Base::setDefaultCaret( Caret * pCaret )
	{
		s_pDefaultCaret = pCaret;
	}

	//_____ defaultTextLayout() ________________________________________________

	TextLayout_p Base::defaultTextLayout()
	{
		return s_pDefaultTextLayout;
	}


	//____ setDefaultTextLayout() ___________________________________________________

	void Base::setDefaultTextLayout( TextLayout * pTextLayout )
	{
		s_pDefaultTextLayout = pTextLayout;
	}

	//____ defaultNumberLayout() _____________________________________________

	NumberLayout_p Base::defaultNumberLayout()
	{
		return s_pDefaultNumberLayout;
	}

	//____ setDefaultNumberLayout() _______________________________________________________

	void Base::setDefaultNumberLayout(NumberLayout * pFormatter)
	{
		s_pDefaultNumberLayout = pFormatter;
	}

	//____ defaultStyle() ______________________________________________________

	TextStyle_p Base::defaultStyle()
	{
		return s_pDefaultStyle;
	}

	//____ setDefaultStyle() _______________________________________________________

	void Base::setDefaultStyle( TextStyle * pStyle )
	{
		s_pDefaultStyle = pStyle;
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

        s_pInputHandler->_update(timestamp/1000);
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
