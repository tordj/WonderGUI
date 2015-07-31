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

#ifndef WG_CARET_DOT_H
#	include <wg_caret.h>
#endif

#ifndef WG_CARET2_DOT_H
#	include <wg_caret2.h>
#endif

#ifndef WG_TEXTSTYLE_DOT_H
#	include <wg_textstyle.h>
#endif

#ifndef WG_TEXTPRESENTER_DOT_H
#	include <wg_textpresenter.h>
#endif

#ifndef WG_MSGROUTER_DOT_H
#	include <wg_msgrouter.h>
#endif

#ifdef WG_USE_FREETYPE

typedef struct FT_LibraryRec_  *FT_Library;

#endif


namespace wg 
{
	class Font;
	class TextpropManager;
	class MemPool;
	class WeakPtrHub;
	class Hook_pHub;
	class MemStack;
	
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
	
	class Base
	{
	public:
		static void init();
		static int exit();
	
	
	#ifdef WG_USE_FREETYPE
		static bool					initFreeType();
		static inline FT_Library	getFreeTypeLibrary() { assert(s_pData!=0); return s_pData->freeTypeLibrary; }
	#endif
	
	
	
	//	static void setDefaultTextManager( const TextMgrPtr& pManager );
	//	static const TextMgrPtr& getDefaultTextManager();
	
		static void 	setDefaultTextprop( const Textprop_p& pProp );
		static Textprop_p getDefaultTextprop() { assert(s_pData!=0); return s_pData->pDefaultTextprop; }
	
		static void 	setDefaultSelectionProp( const Textprop_p& pProp );
		static Textprop_p getDefaultSelectionProp() { assert(s_pData!=0); return s_pData->pDefaultSelectionProp; }
	
		static void 	setDefaultLinkProp( const Textprop_p& pProp );
		static Textprop_p getDefaultLinkProp() { assert(s_pData!=0); return s_pData->pDefaultLinkProp; }
	
		static void 	setDefaultCursor( const Caret_p& pCursor );
		static Caret_p getDefaultCursor() { assert(s_pData!=0); return s_pData->pDefaultCursor; }
	
		static void		setDefaultCaret( const Caret2_p& pCaret );
		static Caret2_p defaultCaret() { assert(s_pData!=0); return s_pData->pDefaultCaret; }
	
		static void		setDefaultPresenter( const TextPresenter_p& pPresenter );
		static TextPresenter_p defaultPresenter() { assert(s_pData!=0); return s_pData->pDefaultPresenter; }
	
		static void		setDefaultStyle( const TextStyle_p& pStyle );
		static TextStyle_p defaultStyle() { assert(s_pData!=0); return s_pData->pDefaultStyle; }
	
	
		static void		mapKey( WgKey translated_keycode, int native_keycode );
		static void		unmapKey( WgKey translated_keycode );
		static void		clearKeyMap();
		static WgKey	translateKey( int native_keycode );
	
		static bool		setMouseButtonRepeat( int delay, int rate );
		static bool		setKeyRepeat( int delay, int rate );
	
		static int		mouseButtonRepeatDelay() { assert(s_pData!=0); return s_pData->buttonRepeatDelay; }
		static int		mouseButtonRepeatRate() { assert(s_pData!=0); return s_pData->buttonRepeatRate; }
	
		static int		keyRepeatDelay() { assert(s_pData!=0); return s_pData->keyRepeatDelay; }
		static int		keyRepeatRate() { assert(s_pData!=0); return s_pData->keyRepeatRate; }
	
		static bool		setDoubleClickTresholds( int time, int distance );
		static int		doubleClickTimeTreshold() { assert(s_pData!=0); return s_pData->doubleClickTimeTreshold; }
		static int		doubleClickDistanceTreshold() { assert(s_pData!=0); return s_pData->doubleClickDistanceTreshold; }
	
		static char *	memStackAlloc( int bytes );
		static void		memStackRelease( int bytes );
	
		static MsgRouter_p	msgRouter() { return s_pData->pMsgRouter; }
	 
		//____
	
		static WeakPtrHub *	allocWeakPtrHub();
		static void			freeWeakPtrHub( WeakPtrHub * pHub );
	
		static Hook_pHub *	allocHookPtrHub();
		static void			freeHookPtrHub( Hook_pHub * pHub );
	
	private:
	
		struct Data
		{
			MsgRouter_p	pMsgRouter;
			
			Textprop_p		pDefaultTextprop;
			Textprop_p		pDefaultSelectionProp;
			Textprop_p		pDefaultLinkProp;
			Caret_p			pDefaultCursor;
	
			Caret2_p			pDefaultCaret;
			TextPresenter_p	pDefaultPresenter;
			TextStyle_p		pDefaultStyle;
	
			// Settings for keyboard/pointer input
	
			int					doubleClickTimeTreshold;		// Maximum millseconds between first and second press to count as a doubleclick.
			int					doubleClickDistanceTreshold;	// Maximum distance the pointer may move between first and second press to count as a doubleclick.
	
			int					buttonRepeatDelay;
			int					buttonRepeatRate;
	
			int					keyRepeatDelay;
			int					keyRepeatRate;
	
			std::map<int,WgKey>	keycodeMap;		// Maps native keycodes to WgKey.
	
			//
	
			MemPool *			pPtrPool;
			MemStack *		pMemStack;
	
	#ifdef WG_USE_FREETYPE
			bool				bFreeTypeInitialized;
			FT_Library			freeTypeLibrary;
	#endif
	
		};
	
		static Data *	s_pData;
	
	};
	
	

} // namespace wg
#endif //WG_BASE_DOT_H
