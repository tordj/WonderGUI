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
#include <wg_guicontext.h>

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
	class Receiver;


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
		
		static GUIContext_p			context() { assert(s_pContext); return s_pContext; }
		static bool					setContext( GUIContext * pContext );
		
		static void			setClipboardText( const String& text );
		static String		getClipboardText();
		
		static HostBridge *	hostBridge() { return s_pHostBridge; }

		//.____ Misc ________________________________________________

		static bool			isInitialized() { return s_guiInitCounter > 0; }

		static void			update( int64_t timestamp_microseconds );

		static int64_t 		timestamp() { return s_timestamp; }

		//.____ Internal ____________________________________________________________

		static int64_t		_startReceiveUpdates(Receiver* pReceiver);
		static void			_stopReceiveUpdates(Receiver* pReceiver);


	private:

		static	int				s_guiInitCounter;
		static GUIContext_p		s_pContext;

		static	String			s_clipboardText;

		static HostBridge*		s_pHostBridge;

		static int64_t			s_timestamp;

		static std::vector<Receiver*>		s_updateReceivers;
	};



} // namespace wg
#endif //WG_BASE_DOT_H
