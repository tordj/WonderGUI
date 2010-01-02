
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


WgFont * WgBase::s_pDefFont = 0;


//____ Init() __________________________________________________________________

void WgBase::Init()
{
}

//____ Exit() __________________________________________________________________

void WgBase::Exit()
{
}


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

//____ SetDefaultFont() ________________________________________________________

void WgBase::SetDefaultFont( WgFont * pFont )
{
	s_pDefFont = pFont;
}

//____ GetDefaultFont() ________________________________________________________

WgFont * WgBase::GetDefaultFont()
{
	return s_pDefFont;
}

