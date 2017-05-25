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

/*
#include <wg_textmgr.h>
*/

#include <assert.h>
#include <map>

#include <wg_userdefines.h>
#include <wg_key.h>
#include <wg_caret.h>
#include <wg_textstyle.h>
#include <wg_textmapper.h>
#include <wg_msgrouter.h>
#include <wg_inputhandler.h>

#ifdef USE_FREETYPE

typedef struct FT_LibraryRec_  *FT_Library;

#endif


namespace wg 
{
	class Font;
	class MemPool;
	class WeakPtrHub;
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
//		friend class Object_wp;
		friend class Interface_wp;
		friend class WeakPtrHub;
	public:

		//.____ Creation __________________________________________

		static void init();
		static int exit();
	
	
	#ifdef USE_FREETYPE
		static bool					initFreeType();
		static inline FT_Library	getFreeTypeLibrary() { assert(s_pData!=0); return s_pData->freeTypeLibrary; }
	#endif

		//.____ Content _____________________________________________

		static MsgRouter_p	msgRouter() { return s_pData->pMsgRouter; }
		static InputHandler_p	inputHandler() { return s_pData->pInputHandler; }

		static void			setDefaultTextMapper( TextMapper * pTextMapper );
		static TextMapper_p defaultTextMapper() { assert(s_pData!=0); return s_pData->pDefaultTextMapper; }
	
		static void			setDefaultStyle( TextStyle * pStyle );
		static TextStyle_p 	defaultStyle() { assert(s_pData!=0); return s_pData->pDefaultStyle; }
	
		//.____ Misc ________________________________________________

		static char *		memStackAlloc( int bytes );
		static void			memStackRelease( int bytes );
		
	private:

		static WeakPtrHub *	_allocWeakPtrHub();
		static void			_freeWeakPtrHub(WeakPtrHub * pHub);

		struct Data
		{
			MsgRouter_p		pMsgRouter;
			InputHandler_p	pInputHandler;
			
	
			TextMapper_p	pDefaultTextMapper;
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
