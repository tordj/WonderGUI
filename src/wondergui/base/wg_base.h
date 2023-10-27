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

#ifndef	WG_BASE_DOT_H
#define	WG_BASE_DOT_H
#pragma once

#include <wg_gfxbase.h>

#include <assert.h>

#include <wg_types.h>
#include <wg_hostbridge.h>
#include <wg_string.h>

#include <string>
#include <functional>
#include <unordered_map>
#include <vector>
#include <atomic>



namespace wg
{
	class Font;
	class MsgRouter;
	class NumberLayout;
	class InputHandler;
	class TextLayout;
	class PackLayout;
	class Caret;
	class TextStyle;
	class Receiver;

	typedef	StrongPtr<MsgRouter>		MsgRouter_p;
	typedef	StrongPtr<NumberLayout>		NumberLayout_p;
	typedef	StrongPtr<InputHandler>		InputHandler_p;
	typedef	StrongPtr<TextLayout>		TextLayout_p;
	typedef	StrongPtr<Caret>			Caret_p;
	typedef	StrongPtr<TextStyle>		TextStyle_p;
	typedef	StrongPtr<PackLayout>		PackLayout_p;



	/**
	 * @brief	Static base class for WonderGUI.
	 *
	 * A static base class for WonderGUI that handles initialization and general
	 * housekeeping and resource allocation.
	 *
	 * The first thing that you need to do when starting WonderGUI is to call Base::init()
	 * and the last thing you should do is to call Base::exit().
	 *
	 */

	class Base : public GfxBase
	{
//		friend class Object_wp;
		friend class Object;


	public:

		//.____ Creation __________________________________________

		static bool init( HostBridge * pHostBridge );
		static bool exit();

		//.____ Content _____________________________________________

		static MsgRouter_p	msgRouter();
		static InputHandler_p	inputHandler();

		static void			setDefaultTextLayout( TextLayout * pTextLayout );
		static TextLayout_p defaultTextLayout();

		static void			setDefaultCaret(Caret * pCaret);
		static Caret_p		defaultCaret();

		static void			setDefaultStyle( TextStyle * pStyle );
		static TextStyle_p 	defaultStyle();

		static void			setDefaultNumberLayout(NumberLayout * pFormatter);
		static NumberLayout_p defaultNumberLayout();

		static void			setDefaultPackLayout( PackLayout * pPackLayout );
		static PackLayout_p defaultPackLayout();
		
		static void			setClipboardText( const String& text );
		static String		getClipboardText();
		

		static HostBridge *	hostBridge() { return s_pHostBridge; }


		
		//.____ Misc ________________________________________________

		const static TypeInfo	TYPEINFO;

		static bool			isInitialized() { return s_guiInitCounter > 0; }

		static void			update( int64_t timestamp_microseconds );


		//.____ Internal ____________________________________________________________

		static int64_t		_startReceiveUpdates(Receiver* pReceiver);
		static void			_stopReceiveUpdates(Receiver* pReceiver);

		static TextStyle*	_defaultStyle() { return s_pDefaultStyle.rawPtr(); }



	private:

		static	int				s_guiInitCounter;
		
		static	MsgRouter_p		s_pMsgRouter;
		static	InputHandler_p	s_pInputHandler;
		static	TextLayout_p	s_pDefaultTextLayout;
		static	Caret_p			s_pDefaultCaret;
		static	NumberLayout_p	s_pDefaultNumberLayout;
		static	TextStyle_p		s_pDefaultStyle;
		static	PackLayout_p	s_pDefaultPackLayout;


		static	String			s_clipboardText;

		static HostBridge*					s_pHostBridge;

		static int64_t						s_timestamp;

		static std::vector<Receiver*>		s_updateReceivers;
	};



} // namespace wg
#endif //WG_BASE_DOT_H
