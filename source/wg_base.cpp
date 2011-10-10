
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
#include <wg_rectchain.h>
#include <wg_textpropmanager.h>
#include <wg_texttool.h>

#ifdef WG_USE_FREETYPE
#	include <ft2build.h>
#	include <wg_vectorglyphs.h>
#	include FT_FREETYPE_H
#endif

WgBase::Data *			WgBase::s_pData = 0;


//____ Init() __________________________________________________________________

void WgBase::Init()
{
	assert( s_pData == 0 );
	s_pData = new Data;
	
	s_pData->pDefaultTextLinkHandler = 0;
	s_pData->pDefaultCursor = 0;
	s_pData->pWeakPtrPool = new WgMemPool( 128, sizeof( WgWeakPtrHub ) );

#ifdef WG_USE_FREETYPE
	s_pData->bFreeTypeInitialized = false;
#endif

	WgTextTool::setDefaultBreakRules();
	WgRectChain::Init();
}

//____ Exit() __________________________________________________________________

void WgBase::Exit()
{
	assert( s_pData != 0 );
	
#ifdef WG_USE_FREETYPE

	WgVectorGlyphs::SetSurfaceFactory(0);
	WgVectorGlyphs::ClearCache();

	if( s_pData->bFreeTypeInitialized )
		FT_Done_FreeType( s_pData->freeTypeLibrary );
#endif
	WgRectChain::Exit();

	delete s_pData->pWeakPtrPool;
	delete s_pData;
	s_pData = 0;
}

//____ AllocWeakPtrHub() ______________________________________________________

WgWeakPtrHub * WgBase::AllocWeakPtrHub()
{
	assert( s_pData != 0 );
	return (WgWeakPtrHub*) s_pData->pWeakPtrPool->allocEntry();
}

//____ FreeWeakPtrHub() _______________________________________________________

void WgBase::FreeWeakPtrHub( WgWeakPtrHub * pHub )
{
	assert( s_pData != 0 );
	s_pData->pWeakPtrPool->freeEntry( pHub );
}


//____ InitFreeType() _________________________________________________________

#ifdef WG_USE_FREETYPE
bool WgBase::InitFreeType()
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



//____ SetDefaultTextManager() _________________________________________________

/*void WgBase::SetDefaultTextManager( const WgTextMgrPtr& pManager )
{
	m_pDefTextMgr = pManager;
}


//____ GetDefaultTextManager() _________________________________________________

const WgTextMgrPtr& WgBase::GetDefaultTextManager()
{
	return m_pDefTextMgr;
}
*/

//____ SetDefaultTextProp() ___________________________________________________

void WgBase::SetDefaultTextProp( const WgTextPropPtr& pProp )
{
	assert( s_pData != 0 );	
	s_pData->pDefaultTextProp = pProp;
}

//____ SetDefaultSelectionProp() ___________________________________________________

void WgBase::SetDefaultSelectionProp( const WgTextPropPtr& pProp )
{
	assert( s_pData != 0 );
	s_pData->pDefaultSelectionProp = pProp;
}

//____ SetDefaultLinkProp() ___________________________________________________

void WgBase::SetDefaultLinkProp( const WgTextPropPtr& pProp )
{
	assert( s_pData != 0 );
	s_pData->pDefaultLinkProp = pProp;
}


//____ SetDefaultCursor() ___________________________________________________

void WgBase::SetDefaultCursor( WgCursor * pCursor )
{
	assert( s_pData != 0 );
	s_pData->pDefaultCursor = pCursor;
}

//____ SetDefaultTextLinkHandler() ____________________________________________

void WgBase::SetDefaultTextLinkHandler( WgTextLinkHandler * pHandler )
{
	assert( s_pData != 0 );
	s_pData->pDefaultTextLinkHandler = pHandler;
}


