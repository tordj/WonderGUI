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

#ifndef WG_USERDEFINES_DOT_H
#	include <wg_userdefines.h>
#endif

#ifndef WG_TEXTPROP_DOT_H
#	include <wg_textprop.h>
#endif

#ifndef WG_TEXTLINKHANDLER_DOT_H
#	include <wg_textlinkhandler.h>
#endif



#ifdef WG_USE_FREETYPE

typedef struct FT_LibraryRec_  *FT_Library;

#endif


class WgFont;
class WgTextPropManager;
class WgMemPool;
class WgWeakPtrHub;
class WgCursor;

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

	static void SetDefaultTextProp( const WgTextPropPtr& pProp );
	static WgTextPropPtr GetDefaultTextProp() { assert(s_pData!=0); return s_pData->pDefaultTextProp; }

	static void SetDefaultSelectionProp( const WgTextPropPtr& pProp );
	static WgTextPropPtr GetDefaultSelectionProp() { assert(s_pData!=0); return s_pData->pDefaultSelectionProp; }

	static void SetDefaultLinkProp( const WgTextPropPtr& pProp );
	static WgTextPropPtr GetDefaultLinkProp() { assert(s_pData!=0); return s_pData->pDefaultLinkProp; }

	static void SetDefaultCursor( WgCursor * pCursor );
	static WgCursor * GetDefaultCursor() { assert(s_pData!=0); return s_pData->pDefaultCursor; }

	static void SetDefaultTextLinkHandler( WgTextLinkHandler * pHandler );
	static WgTextLinkHandler * GetDefaultTextLinkHandler() { assert(s_pData!=0); return s_pData->pDefaultTextLinkHandler; }

	static WgWeakPtrHub *	AllocWeakPtrHub();
	static void			FreeWeakPtrHub( WgWeakPtrHub * pHub );

private:

	struct Data
	{
		WgTextPropPtr		pDefaultTextProp;
		WgTextPropPtr		pDefaultSelectionProp;
		WgTextPropPtr		pDefaultLinkProp;
		WgTextLinkHandler * pDefaultTextLinkHandler;
		WgCursor *			pDefaultCursor;

		WgMemPool *			pWeakPtrPool;

#ifdef WG_USE_FREETYPE
		bool				bFreeTypeInitialized;
		FT_Library			freeTypeLibrary;
#endif

	};

	static Data *	s_pData;

};


#endif //WG_BASE_DOT_H
