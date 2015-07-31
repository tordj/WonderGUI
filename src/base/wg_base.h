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
	class WgFont;
	class WgTextpropManager;
	class WgMemPool;
	class WgWeakPtrHub;
	class WgHook_pHub;
	class WgMemStack;
	
	/**
	 * @brief	Static base class for WonderGUI.
	 *
	 * A static base class for WonderGUI that handles initialization and general
	 * housekeeping and resource allocation.
	 *
	 * The first thing that you need to do when starting WonderGUI is to call WgBase::init()
	 * and the last thing you should do is to call WgBase::exit().
	 * 
	 */
	
	class WgBase
	{
	public:
		static void init();
		static int exit();
	
	
	#ifdef WG_USE_FREETYPE
		static bool					initFreeType();
		static inline FT_Library	getFreeTypeLibrary() { assert(s_pData!=0); return s_pData->freeTypeLibrary; }
	#endif
	
	
	
	//	static void setDefaultTextManager( const WgTextMgrPtr& pManager );
	//	static const WgTextMgrPtr& getDefaultTextManager();
	
		static void 	setDefaultTextprop( const WgTextprop_p& pProp );
		static WgTextprop_p getDefaultTextprop() { assert(s_pData!=0); return s_pData->pDefaultTextprop; }
	
		static void 	setDefaultSelectionProp( const WgTextprop_p& pProp );
		static WgTextprop_p getDefaultSelectionProp() { assert(s_pData!=0); return s_pData->pDefaultSelectionProp; }
	
		static void 	setDefaultLinkProp( const WgTextprop_p& pProp );
		static WgTextprop_p getDefaultLinkProp() { assert(s_pData!=0); return s_pData->pDefaultLinkProp; }
	
		static void 	setDefaultCursor( const WgCaret_p& pCursor );
		static WgCaret_p getDefaultCursor() { assert(s_pData!=0); return s_pData->pDefaultCursor; }
	
		static void		setDefaultCaret( const WgCaret2_p& pCaret );
		static WgCaret2_p defaultCaret() { assert(s_pData!=0); return s_pData->pDefaultCaret; }
	
		static void		setDefaultPresenter( const WgTextPresenter_p& pPresenter );
		static WgTextPresenter_p defaultPresenter() { assert(s_pData!=0); return s_pData->pDefaultPresenter; }
	
		static void		setDefaultStyle( const WgTextStyle_p& pStyle );
		static WgTextStyle_p defaultStyle() { assert(s_pData!=0); return s_pData->pDefaultStyle; }
	
	
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
	
		static WgMsgRouter_p	msgRouter() { return s_pData->pMsgRouter; }
	 
		//____
	
		static WgWeakPtrHub *	allocWeakPtrHub();
		static void			freeWeakPtrHub( WgWeakPtrHub * pHub );
	
		static WgHook_pHub *	allocHookPtrHub();
		static void			freeHookPtrHub( WgHook_pHub * pHub );
	
	private:
	
		struct Data
		{
			WgMsgRouter_p	pMsgRouter;
			
			WgTextprop_p		pDefaultTextprop;
			WgTextprop_p		pDefaultSelectionProp;
			WgTextprop_p		pDefaultLinkProp;
			WgCaret_p			pDefaultCursor;
	
			WgCaret2_p			pDefaultCaret;
			WgTextPresenter_p	pDefaultPresenter;
			WgTextStyle_p		pDefaultStyle;
	
			// Settings for keyboard/pointer input
	
			int					doubleClickTimeTreshold;		// Maximum millseconds between first and second press to count as a doubleclick.
			int					doubleClickDistanceTreshold;	// Maximum distance the pointer may move between first and second press to count as a doubleclick.
	
			int					buttonRepeatDelay;
			int					buttonRepeatRate;
	
			int					keyRepeatDelay;
			int					keyRepeatRate;
	
			std::map<int,WgKey>	keycodeMap;		// Maps native keycodes to WgKey.
	
			//
	
			WgMemPool *			pPtrPool;
			WgMemStack *		pMemStack;
	
	#ifdef WG_USE_FREETYPE
			bool				bFreeTypeInitialized;
			FT_Library			freeTypeLibrary;
	#endif
	
		};
	
		static Data *	s_pData;
	
	};
	
	

} // namespace wg
#endif //WG_BASE_DOT_H
