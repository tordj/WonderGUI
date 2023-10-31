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

#ifndef	WG2_BASE_DOT_H
#define	WG2_BASE_DOT_H

/*
#ifndef WG2_TEXTMGR_DOT_H
#	include <wg2_textmgr.h>
#endif
*/

#include <assert.h>
#include <map>
#include <functional>
#include <string>

#ifndef WG2_USERDEFINES_DOT_H
#	include <wg2_userdefines.h>
#endif

#ifndef WG2_KEY_DOT_H
#	include <wg2_key.h>
#endif

#include <wg_gfxbase.h>
#include <wg_textstyle.h>
#include <wg_hostbridge.h>


class WgCursor;

namespace wg
{
	class Object;
	class MemStack;
    class HostBridge;
};


class WgError
{
public:
	wg::ErrorCode   code;
	std::string     message;
	const wg::Object *  pObject;
	const char *    classname;
	const char *    function;
	const char *    file;
	int             line;
};

class WgBase : public wg::GfxBase
{
public:
	static void Init( wg::HostBridge * pHostBridge );
	static void Exit();

	static void 	SetDefaultCursor( WgCursor * pCursor );
	static WgCursor * GetDefaultCursor() { assert(s_pData!=0); return s_pData->pDefaultCursor; }

	static void		MapKey( WgKey translated_keycode, int native_keycode );
	static void		UnmapKey( WgKey translated_keycode );
	static void		ClearKeyMap();
	static WgKey	TranslateKey( int native_keycode );

	static bool		SetMouseButtonRepeat( int delay, int rate );
	static bool		SetKeyRepeat( int delay, int rate );

	static int		MouseButtonRepeatDelay() { assert(s_pData!=0); return s_pData->buttonRepeatDelay; }
	static int		MouseButtonRepeatRate() { assert(s_pData!=0); return s_pData->buttonRepeatRate; }

	static int		KeyRepeatDelay() { assert(s_pData!=0); return s_pData->keyRepeatDelay; }
	static int		KeyRepeatRate() { assert(s_pData!=0); return s_pData->keyRepeatRate; }

	static bool		SetDoubleClickTresholds( int time, int distance );
	static int		DoubleClickTimeTreshold() { assert(s_pData!=0); return s_pData->doubleClickTimeTreshold; }
	static int		DoubleClickDistanceTreshold() { assert(s_pData!=0); return s_pData->doubleClickDistanceTreshold; }

	static wg::HostBridge * hostBridge() { return s_pHostBridge; }
	
	static void		setDefaultStyle( wg::TextStyle* pStyle );
	static wg::TextStyle_p	defaultStyle() { return wg::TextStyle::s_pDefaultStyle; }

private:

	struct Data
	{
		WgCursor *			pDefaultCursor;

		// Settings for keyboard/pointer input

		int					doubleClickTimeTreshold;		// Maximum millseconds between first and second press to count as a doubleclick.
		int					doubleClickDistanceTreshold;	// Maximum distance the pointer may move between first and second press to count as a doubleclick.

		int					buttonRepeatDelay;
		int					buttonRepeatRate;

		int					keyRepeatDelay;
		int					keyRepeatRate;

		std::map<int,WgKey>	keycodeMap;		// Maps native keycodes to WgKey.

		//

	};
	
	static int                              s_iSoftubeNumberOfInstances;
	static Data *							s_pData;
	
	static wg::HostBridge *					s_pHostBridge;
	
};

#endif //WG2_BASE_DOT_H
