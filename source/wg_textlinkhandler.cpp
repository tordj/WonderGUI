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

#include <wg_textlinkhandler.h>
#include <wg_base.h>

//____ WgTextLink::Constructor ________________________________________________

WgTextLink::WgTextLink( const std::string& link, WgTextLinkHandler * pHandler ) : 
			m_bAccessed(false), m_pHandler(pHandler), m_link(link) 
{
}



void WgTextLinkHandler::OnPointerEnter( const WgTextLinkPtr& pLink, const WgCoord& screenPos )
{
}

void WgTextLinkHandler::OnPointerOver( const WgTextLinkPtr& pLink, const WgCoord& screenPos )
{
}

void WgTextLinkHandler::OnPointerExit( const WgTextLinkPtr& pLink, const WgCoord& screenPos )
{
}

void WgTextLinkHandler::OnButtonPress( int button, const WgTextLinkPtr& pLink, const WgCoord& screenPos )
{
}

void WgTextLinkHandler::OnButtonRelease( int button, const WgTextLinkPtr& pLink, const WgCoord& screenPos )
{
}

void WgTextLinkHandler::OnButtonRepeat( int button, const WgTextLinkPtr& pLink, const WgCoord& screenPos )
{
}

void WgTextLinkHandler::OnButtonClick( int button, const WgTextLinkPtr& pLink, const WgCoord& screenPos )
{
}

void WgTextLinkHandler::OnButtonDoubleClick( int button, const WgTextLinkPtr& pLink, const WgCoord& screenPos )
{
}

