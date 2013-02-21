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

#include <wg_packpanel.h>

WgPackHook::WgPackHook()
{
	m_weight = 1.f;
}

WgPackPanel * WgPackHook::Parent() const 
{ 
	return static_cast<WgPackPanel*>(_parent()); 
}


bool WgPackHook::SetWeight( float weight )
{
	if( weight < 0 )
		return false;

	if( weight != m_weight )
	{
		m_weight = weight;
		Parent()->_setChildLengths();
	}
	return true;
}



void WgPackPanel::SetSizeBroker( WgSizeBroker * pBroker )
{
	if( m_pSizeBroker != pBroker )
	{
		m_pSizeBroker = pBroker;
		_setChildLengths();
	}
}

WgSize WgPackPanel::_preferredSize()
{
	int length = 0;
	int breadth = 0;

	if( !m_pSizeBroker )
	{
		WgPackHook * p = FirstHook();

		if( m_bHorizontal )
		{
			length += p->m_preferredSize.w;
			if( p->m_preferredSize.h > breadth )
				breadth = p->m_preferredSize.h;
			p = p->Next();
		}
		else
		{
			length += p->m_preferredSize.h;
			if( p->m_preferredSize.w > breadth )
				breadth = p->m_preferredSize.w;
			p = p->Next();
		}
	}
	else
	{
		//TODO: Implement!
	}

	if( m_bHorizontal )
		return WgSize(length,breadth);
	else
		return WgSize(breadth,length);
}

void WgPackPanel::_setChildLengths()
{
	int wantedLength = m_bHorizontal?m_preferredSize.w:m_preferredSize.h;
	int givenLength = m_bHorizontal?Geo().w:Geo().h;

	// Optimized special case, just copy preferred to length.

	if( !m_pSizeBroker || wantedLength == givenLength )
	{
		WgPackHook * p = FirstHook();
		while( p )
		{
			p->m_length = m_bHorizontal?p->m_preferredSize.w:m_preferredSize.h;
			p = p->Next();
		}
	}
	else
	{
		//TODO: Implement!
	}

}
