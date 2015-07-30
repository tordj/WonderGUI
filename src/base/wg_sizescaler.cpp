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

WgSizeScaler_p WgSizeScaler::create() 
{
	return new WgSizeScaler(1.f,1.f);
}

WgSizeScaler_p WgSizeScaler::create( float scale ) 
{
	return new WgSizeScaler(scale,scale);
}

WgSizeScaler_p WgSizeScaler::create( float scaleX, float scaleY ) 
{
	return new WgSizeScaler(scaleX,scaleY);
}

WgSizeScaler::WgSizeScaler( float scaleX, float scaleY )
{
	m_scaleX = scaleX;
	m_scaleY = scaleY;
}

void WgSizeScaler::setScale( float scale )
{
	setScale(scale,scale);
}

void WgSizeScaler::setScale( float scaleX, float scaleY )
{
	if( scaleX != m_scaleX || scaleY != m_scaleY )
	{
		m_scaleX = scaleX;
		m_scaleY = scaleY;

		Node * p = m_nodes.first();
		while( p )
		{
			p->m_pScalable->_onScaleChanged();
			p = p->next();
		}
	}
}

void WgSizeScaler::setScaleX( float scaleX )
{
	setScale(scaleX,m_scaleY);
}

void WgSizeScaler::setScaleY( float scaleY )
{
	setScale(m_scaleX,scaleY);
}

void WgSizeScaler::addItem( WgScalable * pScalable )
{
	if( !pScalable )
		return;

	Node * p = new Node();
	p->m_pScalable = pScalable;
	m_nodes.pushBack(p);
	pScalable->m_pScaler = this;
	pScalable->_onScaleChanged();
}

void WgSizeScaler::removeItem( WgScalable * pScalable )
{
	Node * p = m_nodes.first();
	while( p )
	{
		if( p->m_pScalable == pScalable )
		{
			pScalable->m_pScaler = 0;
			pScalable->_onScaleChanged();
			delete p;
			return;
		}
		p = p->next();
	}
}

void WgSizeScaler::removeAllItems()
{
	Node * p = m_nodes.first();
	while( p )
	{
		p->m_pScalable->m_pScaler = 0;
		p->m_pScalable->_onScaleChanged();
		p = p->next();
	}
	m_nodes.clear();
}

void WgSizeScaler::_removeDeadItem( WgScalable * pScalable )
{
	Node * p = m_nodes.first();
	while( p )
	{
		if( p->m_pScalable == pScalable )
		{
			delete p;
			return;
		}
		p = p->next();
	}
}

WgScalable::~WgScalable()
{
	if( m_pScaler )
		m_pScaler->_removeDeadItem(this);
}
