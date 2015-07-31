
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

#include <wg_textpropmanager.h>
#include <wg_texttool.h>
#include <wg_memstack.h>
#include <wg_hook.h>
#include <wg_standardpresenter.h>

#ifdef WG_USE_FREETYPE
#	include <ft2build.h>
#	include <wg_vectorglyphs.h>
#	include FT_FREETYPE_H
#endif

namespace wg 
{
	
	WgBase::Data *			WgBase::s_pData = 0;
	
	
	//____ init() __________________________________________________________________
	
	void WgBase::init()
	{
		assert( s_pData == 0 );
		assert( sizeof( WgWeakPtrHub ) == sizeof( WgHook_pHub ) );			// Need to be same as we are sharing object stack!
		s_pData = new Data;
	
		s_pData->pMsgRouter = WgMsgRouter::create();
	
		s_pData->pDefaultCursor = 0;
		s_pData->pPtrPool = new WgMemPool( 128, sizeof( WgWeakPtrHub ) );
		s_pData->pMemStack = new WgMemStack( 4096 );
	
		s_pData->pDefaultPresenter = WgStandardPresenter::create();
		s_pData->pDefaultStyle = WgTextStyle::create();
	
		s_pData->doubleClickTimeTreshold 		= 250;
		s_pData->doubleClickDistanceTreshold 	= 2;
	
		s_pData->buttonRepeatDelay 	= 300;
		s_pData->buttonRepeatRate 	= 200;
	
		s_pData->keyRepeatDelay 	= 300;
		s_pData->keyRepeatRate 		= 150;
	
	
	#ifdef WG_USE_FREETYPE
		s_pData->bFreeTypeInitialized = false;
	#endif
	
		WgTextTool::setDefaultBreakRules();
	}
	
	//____ exit() __________________________________________________________________
	
	int WgBase::exit()
	{
		
		if( s_pData == 0 )
			return -1;					// Base already exited or not intialized.
	
		if( !s_pData->pPtrPool->isEmpty() )
			return -2;					// There are weak pointers or hook pointers left.
	
		if( !s_pData->pMemStack->isEmpty() )
			return -3;					// There is data left in memstack.
	
	#ifdef WG_USE_FREETYPE
	
		WgVectorGlyphs::setSurfaceFactory(0);
		WgVectorGlyphs::clearCache();
	
		if( s_pData->bFreeTypeInitialized )
			FT_Done_FreeType( s_pData->freeTypeLibrary );
	#endif
		s_pData->pDefaultPresenter = 0;
		s_pData->pDefaultStyle = 0;
	
		delete s_pData->pPtrPool;
		delete s_pData->pMemStack;
		delete s_pData;
		s_pData = 0;
		return 0;
	}
	
	//____ allocWeakPtrHub() ______________________________________________________
	
	WgWeakPtrHub * WgBase::allocWeakPtrHub()
	{
		assert( s_pData != 0 );
		return (WgWeakPtrHub*) s_pData->pPtrPool->allocEntry();
	}
	
	//____ freeWeakPtrHub() _______________________________________________________
	
	void WgBase::freeWeakPtrHub( WgWeakPtrHub * pHub )
	{
		assert( s_pData != 0 );
		s_pData->pPtrPool->freeEntry( pHub );
	}
	
	//____ allocHookPtrHub() ______________________________________________________
	
	WgHook_pHub * WgBase::allocHookPtrHub()
	{
		assert( s_pData != 0 );
		return (WgHook_pHub*) s_pData->pPtrPool->allocEntry();
	}
	
	//____ freeHookPtrHub() _______________________________________________________
	
	void WgBase::freeHookPtrHub( WgHook_pHub * pHub )
	{
		assert( s_pData != 0 );
		s_pData->pPtrPool->freeEntry( pHub );
	}
	
	//____ initFreeType() _________________________________________________________
	
	#ifdef WG_USE_FREETYPE
	bool WgBase::initFreeType()
	{
		assert( s_pData != 0 );
		if( s_pData->bFreeTypeInitialized )
			return true;
	
		FT_Error err = FT_Init_FreeType( &s_pData->freeTypeLibrary );
		if( err == 0 )
		{
			s_pData->bFreeTypeInitialized = true;
			return true;
		}
	
		return false;
	}
	#endif
	
	
	
	//____ setDefaultTextManager() _________________________________________________
	
	/*void WgBase::setDefaultTextManager( const WgTextMgrPtr& pManager )
	{
		m_pDefTextMgr = pManager;
	}
	
	
	//____ getDefaultTextManager() _________________________________________________
	
	const WgTextMgrPtr& WgBase::getDefaultTextManager()
	{
		return m_pDefTextMgr;
	}
	*/
	
	//____ setDefaultCaret() _______________________________________________________
	
	void WgBase::setDefaultCaret( const WgCaret2_p& pCaret )
	{
		assert( s_pData != 0 );
		s_pData->pDefaultCaret = pCaret;	
	}
	
	//____ setDefaultPresenter() ___________________________________________________
	
	void WgBase::setDefaultPresenter( const WgTextPresenter_p& pPresenter )
	{
		assert( s_pData != 0 );
		s_pData->pDefaultPresenter = pPresenter;	
	}
	
	//____ setDefaultStyle() _______________________________________________________
	
	void WgBase::setDefaultStyle( const WgTextStyle_p& pStyle )
	{
		assert( s_pData != 0 );
		s_pData->pDefaultStyle = pStyle;	
	}
	
	
	
	//____ setDefaultTextprop() ___________________________________________________
	
	void WgBase::setDefaultTextprop( const WgTextprop_p& pProp )
	{
		assert( s_pData != 0 );
		s_pData->pDefaultTextprop = pProp;
	}
	
	//____ setDefaultSelectionProp() ___________________________________________________
	
	void WgBase::setDefaultSelectionProp( const WgTextprop_p& pProp )
	{
		assert( s_pData != 0 );
		s_pData->pDefaultSelectionProp = pProp;
	}
	
	//____ setDefaultLinkProp() ___________________________________________________
	
	void WgBase::setDefaultLinkProp( const WgTextprop_p& pProp )
	{
		assert( s_pData != 0 );
		s_pData->pDefaultLinkProp = pProp;
	}
	
	
	//____ setDefaultCursor() ___________________________________________________
	
	void WgBase::setDefaultCursor( const WgCaret_p& pCursor )
	{
		assert( s_pData != 0 );
		s_pData->pDefaultCursor = pCursor;
	}
	
	//____ setDoubleClickTresholds() _______________________________________________
	
	bool WgBase::setDoubleClickTresholds( int time, int distance )
	{
		assert( s_pData != 0 );
		if( time <= 0 || distance <= 0 )
			return false;
	
		s_pData->doubleClickTimeTreshold		= time;
		s_pData->doubleClickDistanceTreshold	= distance;
		return true;
	}
	
	//____ setMouseButtonRepeat() ______________________________________________________
	
	bool WgBase::setMouseButtonRepeat( int delay, int rate )
	{
		assert( s_pData != 0 );
		if( delay <= 0 || rate <= 0 )
			return false;
	
		s_pData->buttonRepeatDelay	= delay;
		s_pData->buttonRepeatRate	= rate;
		return true;
	}
	
	//____ setKeyRepeat() _________________________________________________________
	
	bool WgBase::setKeyRepeat( int delay, int rate )
	{
		assert( s_pData != 0 );
		if( delay <= 0 || rate <= 0 )
			return false;
	
		s_pData->keyRepeatDelay	= delay;
		s_pData->keyRepeatRate	= rate;
		return true;
	}
	
	//____ mapKey() _______________________________________________________________
	
	void WgBase::mapKey( WgKey translated_keycode, int native_keycode )
	{
		assert( s_pData != 0 );
		s_pData->keycodeMap[native_keycode] = translated_keycode;
	}
	
	
	//____ unmapKey() _____________________________________________________________
	
	void WgBase::unmapKey( WgKey translated_keycode )
	{
		assert( s_pData != 0 );
		std::map<int,WgKey>::iterator it = s_pData->keycodeMap.begin();
	
		while( it != s_pData->keycodeMap.end() )
		{
			if( it->second == translated_keycode )
			{
				std::map<int,WgKey>::iterator it2 = it++;
				s_pData->keycodeMap.erase(it2);
			}
			else
				++it;
		}
	}
	
	//____ clearKeyMap() __________________________________________________________
	
	void WgBase::clearKeyMap()
	{
		assert( s_pData != 0 );
		s_pData->keycodeMap.clear();
	}
	
	//____ translateKey() __________________________________________________________
	
	WgKey WgBase::translateKey( int native_keycode )
	{
		assert( s_pData != 0 );
		std::map<int,WgKey>::iterator it = s_pData->keycodeMap.find(native_keycode);
		if( it != s_pData->keycodeMap.end() )
			return  it->second;
		else
			return WG_KEY_UNMAPPED;
	}
	
	//____ memStackAlloc() ________________________________________________________
	
	char * WgBase::memStackAlloc( int bytes )
	{ 
		assert(s_pData!=0); 
		return s_pData->pMemStack->alloc(bytes);
	}
	
	//____ memStackRelease() ______________________________________________________
	
	void WgBase::memStackRelease( int bytes )
	{	assert(s_pData!=0); 
		return s_pData->pMemStack->release(bytes); 
	}

} // namespace wg
