
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
#include <wg_dirtyrect.h>
#include <wg_textpropmanager.h>
#include <wg_texttool.h>

#ifdef WG_USE_FREETYPE
#	include <ft2build.h>
#	include <wg_vectorglyphs.h>
#	include FT_FREETYPE_H

	bool		WgBase::s_bFreeTypeInitialized;
	FT_Library	WgBase::s_freeTypeLibrary;
#endif


WgTextPropPtr	WgBase::s_pDefaultTextProp;



//____ Init() __________________________________________________________________

void WgBase::Init()
{
	WgTextTool::setDefaultBreakRules();

	WgDirtyRectObj::Init();
#ifdef WG_USE_FREETYPE
	s_bFreeTypeInitialized = false;
#endif
}

//____ Exit() __________________________________________________________________

void WgBase::Exit()
{
#ifdef WG_USE_FREETYPE

	WgVectorGlyphs::SetSurfaceFactory(0);
	WgVectorGlyphs::ClearCache();

	if( s_bFreeTypeInitialized )
		FT_Done_FreeType( s_freeTypeLibrary );
#endif
	WgDirtyRectObj::Exit();
	s_pDefaultTextProp = 0;
}


//____ InitFreeType() _________________________________________________________

#ifdef WG_USE_FREETYPE
bool WgBase::InitFreeType()
{
	if( s_bFreeTypeInitialized )
		return true;

	FT_Error err = FT_Init_FreeType( &s_freeTypeLibrary );
	if( err == 0 )
	{
		s_bFreeTypeInitialized = true;
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
	s_pDefaultTextProp = pProp;
}

