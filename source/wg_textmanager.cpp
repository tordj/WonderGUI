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

#include <wg_textmanager.h>

#include <wg_text.h>


//____ Constructor ____________________________________________________________

WgTextManager::WgTextManager()
{
	m_scale = 0;
}

//____ Destructor _____________________________________________________________

WgTextManager::~WgTextManager()
{
}

//____ NewNode() ______________________________________________________________

WgTextNode * WgTextManager::NewNode( WgText * pText )
{
	if( !pText )
		return 0;

	WgTextNode * p = new WgTextNode( this, pText );
	m_nodes.push_back( p );
	return p;
}

//____ SetTextScale() _________________________________________________________

void WgTextManager::SetTextScale( float scale )
{
	if( scale != m_scale )
	{
		m_scale = scale;

		WgTextNode * p = m_nodes.getFirst();
		while( p )
		{
			p->Refresh();
			p = p->getNext();
		}
	}
}


//____ WgTextNode::Constructor ________________________________________________

WgTextNode::WgTextNode( WgTextManager * pManager, WgText * pText )
{
	m_pManager = pManager;
	m_pText = pText;

	if( pText->m_pManagerNode )
		delete pText->m_pManagerNode;
	pText->m_pManagerNode = this;
}

//____ WgTextNode::Destructor _________________________________________________

WgTextNode::~WgTextNode()
{
	m_pText->m_pManagerNode = 0;
}

//____ WgTextNode::GetSize() __________________________________________________

float WgTextNode::GetSize( const WgFont * m_pFont, WgFontStyle style, int size )
{
	return (float) size;
}

//____ WgTextNode::Refresh() __________________________________________________

void WgTextNode::Refresh()
{
	m_pText->refresh();
}

