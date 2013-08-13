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

/*
#ifndef WG_TEXTMGR_DOT_H
#	include <wg_textmgr.h>
#endif
*/

#include <assert.h>
#include <map>

#ifndef WG_USERDEFINES_DOT_H
#	include <wg_userdefines.h>
#endif

#ifndef WG_KEY_DOT_H
#	include <wg_key.h>
#endif

#ifndef WG_TEXTPROP_DOT_H
#	include <wg_textprop.h>
#endif

#ifndef WG_CURSOR_DOT_H
#	include <wg_cursor.h>
#endif


#ifdef WG_USE_FREETYPE

typedef struct FT_LibraryRec_  *FT_Library;

#endif


class WgFont;
class WgTextpropManager;
class WgMemPool;
class WgWeakPtrHub;
class WgMemStack;

class WgBase
{
public:
	static void Init();
	static void Exit();


#ifdef WG_USE_FREETYPE
	static bool					InitFreeType();
	static inline FT_Library	GetFreeTypeLibrary() { assert(s_pData!=0); return s_pData->freeTypeLibrary; }
#endif



//	static void SetDefaultTextManager( const WgTextMgrPtr& pManager );
//	static const WgTextMgrPtr& GetDefaultTextManager();

	static void 	SetDefaultTextprop( const WgTextpropPtr& pProp );
	static WgTextpropPtr GetDefaultTextprop() { assert(s_pData!=0); return s_pData->pDefaultTextprop; }

	static void 	SetDefaultSelectionProp( const WgTextpropPtr& pProp );
	static WgTextpropPtr GetDefaultSelectionProp() { assert(s_pData!=0); return s_pData->pDefaultSelectionProp; }

	static void 	SetDefaultLinkProp( const WgTextpropPtr& pProp );
	static WgTextpropPtr GetDefaultLinkProp() { assert(s_pData!=0); return s_pData->pDefaultLinkProp; }

	static void 	SetDefaultCursor( const WgCursorPtr& pCursor );
	static WgCursorPtr GetDefaultCursor() { assert(s_pData!=0); return s_pData->pDefaultCursor; }

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

	static char *	MemStackAlloc( int bytes );
	static void		MemStackRelease( int bytes );


	//____

	static WgWeakPtrHub *	AllocWeakPtrHub();
	static void			FreeWeakPtrHub( WgWeakPtrHub * pHub );

private:

	struct Data
	{
		WgTextpropPtr		pDefaultTextprop;
		WgTextpropPtr		pDefaultSelectionProp;
		WgTextpropPtr		pDefaultLinkProp;
		WgCursorPtr			pDefaultCursor;

		// Settings for keyboard/pointer input

		int					doubleClickTimeTreshold;		// Maximum millseconds between first and second press to count as a doubleclick.
		int					doubleClickDistanceTreshold;	// Maximum distance the pointer may move between first and second press to count as a doubleclick.

		int					buttonRepeatDelay;
		int					buttonRepeatRate;

		int					keyRepeatDelay;
		int					keyRepeatRate;

		std::map<int,WgKey>	keycodeMap;		// Maps native keycodes to WgKey.

		//

		WgMemPool *			pWeakPtrPool;
		WgMemStack *		pMemStack;

#ifdef WG_USE_FREETYPE
		bool				bFreeTypeInitialized;
		FT_Library			freeTypeLibrary;
#endif

	};

	static Data *	s_pData;

};


#endif //WG_BASE_DOT_H
