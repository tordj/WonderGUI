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

#include <wg_sizescaler.h>

WgSizeScalerPtr WgSizeScaler::Create() 
{
	return new WgSizeScaler(1.f,1.f);
}

WgSizeScalerPtr WgSizeScaler::Create( float scale ) 
{
	return new WgSizeScaler(scale,scale);
}

WgSizeScalerPtr WgSizeScaler::Create( float scaleX, float scaleY ) 
{
	return new WgSizeScaler(scaleX,scaleY);
}

WgSizeScaler::WgSizeScaler( float scaleX, float scaleY )
{
	m_scaleX = scaleX;
	m_scaleY = scaleY;
}

void WgSizeScaler::SetScale( float scale )
{
	SetScale(scale,scale);
}

void WgSizeScaler::SetScale( float scaleX, float scaleY )
{
	if( scaleX != m_scaleX || scaleY != m_scaleY )
	{
		m_scaleX = scaleX;
		m_scaleY = scaleY;

		Node * p = m_nodes.First();
		while( p )
		{
			p->m_pScalable->_onScaleChanged();
			p = p->Next();
		}
	}
}

void WgSizeScaler::SetScaleX( float scaleX )
{
	SetScale(scaleX,m_scaleY);
}

void WgSizeScaler::SetScaleY( float scaleY )
{
	SetScale(m_scaleX,scaleY);
}

void WgSizeScaler::AddItem( WgScalable * pScalable )
{
	if( !pScalable )
		return;

	Node * p = new Node();
	p->m_pScalable = pScalable;
	m_nodes.PushBack(p);
	pScalable->m_pScaler = this;
	pScalable->_onScaleChanged();
}

void WgSizeScaler::RemoveItem( WgScalable * pScalable )
{
	Node * p = m_nodes.First();
	while( p )
	{
		if( p->m_pScalable == pScalable )
		{
			pScalable->m_pScaler = 0;
			pScalable->_onScaleChanged();
			delete p;
			return;
		}
		p = p->Next();
	}
}

void WgSizeScaler::RemoveAllItems()
{
	Node * p = m_nodes.First();
	while( p )
	{
		p->m_pScalable->m_pScaler = 0;
		p->m_pScalable->_onScaleChanged();
		p = p->Next();
	}
	m_nodes.Clear();
}

void WgSizeScaler::_removeDeadItem( WgScalable * pScalable )
{
	Node * p = m_nodes.First();
	while( p )
	{
		if( p->m_pScalable == pScalable )
		{
			delete p;
			return;
		}
		p = p->Next();
	}
}

WgScalable::~WgScalable()
{
	if( m_pScaler )
		m_pScaler->_removeDeadItem(this);
}
