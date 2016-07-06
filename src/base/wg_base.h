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

#ifndef WG_CARET_DOT_H
#	include <wg_caret.h>
#endif

#ifndef WG_TEXTSTYLE_DOT_H
#	include <wg_textstyle.h>
#endif

#ifndef WG_PRINTER_DOT_H
#	include <wg_printer.h>
#endif

#ifndef WG_MSGROUTER_DOT_H
#	include <wg_msgrouter.h>
#endif

#ifndef WG_INPUTHANDLER_DOT_H
#	include <wg_inputhandler.h>
#endif

#ifdef USE_FREETYPE

typedef struct FT_LibraryRec_  *FT_Library;

#endif


namespace wg 
{
	class Font;
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
	
	
	#ifdef USE_FREETYPE
		static bool					initFreeType();
		static inline FT_Library	getFreeTypeLibrary() { assert(s_pData!=0); return s_pData->freeTypeLibrary; }
	#endif
		
		static void			setDefaultPrinter( const Printer_p& pPrinter );
		static Printer_p defaultPrinter() { assert(s_pData!=0); return s_pData->pDefaultPrinter; }
	
		static void			setDefaultStyle( const TextStyle_p& pStyle );
		static TextStyle_p 	defaultStyle() { assert(s_pData!=0); return s_pData->pDefaultStyle; }
	
		
		static char *		memStackAlloc( int bytes );
		static void			memStackRelease( int bytes );
	
		static MsgRouter_p	msgRouter() { return s_pData->pMsgRouter; }
		static InputHandler_p	inputHandler() { return s_pData->pInputHandler; }
	 
		//____
	
		static WeakPtrHub *	allocWeakPtrHub();
		static void			freeWeakPtrHub( WeakPtrHub * pHub );
	
		static Hook_pHub *	allocHookPtrHub();
		static void			freeHookPtrHub( Hook_pHub * pHub );
	
	private:
	
		struct Data
		{
			MsgRouter_p		pMsgRouter;
			InputHandler_p	pInputHandler;
			
	
			Printer_p		pDefaultPrinter;
			TextStyle_p		pDefaultStyle;
	
			//
	
			MemPool *		pPtrPool;
			MemStack *		pMemStack;
	
	#ifdef USE_FREETYPE
			bool			bFreeTypeInitialized;
			FT_Library		freeTypeLibrary;
	#endif
	
		};
	
		static Data *	s_pData;
	
	};
	
	

} // namespace wg
#endif //WG_BASE_DOT_H
