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

#include <wg_layer.h>
#include <wg_patches.h>

const char WgLayer::CLASSNAME[] = {"Layer"};
const char WgLayerHook::CLASSNAME[] = {"LayerHook"};


//____ Constructor ____________________________________________________________

WgLayer::WgLayer()
{
	m_baseHook.m_pParent = this;
}

//____ isInstanceOf() _________________________________________________________

bool WgLayer::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgContainer::isInstanceOf(pClassName);
}

//____ className() ____________________________________________________________

const char * WgLayer::className( void ) const
{ 
	return CLASSNAME; 
}

//____ cast() _________________________________________________________________

WgLayer_p WgLayer::cast( const WgObject_p& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgLayer_p( static_cast<WgLayer*>(pObject.rawPtr()) );

	return 0;
}


//____ setBaseWidget() _________________________________________________________

WgHook_p WgLayer::setBaseWidget( const WgWidget_p& pWidget )
{
	m_baseHook._setWidget(pWidget.rawPtr());
	_onBaseChanged();
	return &m_baseHook;
}

//____ baseWidget() ____________________________________________________________

WgWidget_p WgLayer::baseWidget()
{
	return m_baseHook._widget();
}


//____ removeBaseWidget() _____________________________________________________

bool WgLayer::removeBaseWidget()
{
	if( !m_baseHook._widget() )
		return false;

	m_baseHook._setWidget(0);
	_onBaseChanged();

	return true;
}

//____ matchingHeight() _______________________________________________________

int WgLayer::matchingHeight( int width ) const
{
	if( m_baseHook._widget() )
		return m_baseHook._widget()->matchingHeight( width );
	else
		return WgWidget::matchingHeight(width);
}

//____ matchingWidth() _______________________________________________________

int WgLayer::matchingWidth( int height ) const
{
	if( m_baseHook._widget() )
		return m_baseHook._widget()->matchingWidth( height );
	else
		return WgWidget::matchingWidth(height);
}

//____ preferredSize() _____________________________________________________________

WgSize WgLayer::preferredSize() const
{
	if( m_baseHook._widget() )
		return m_baseHook._widget()->preferredSize();
	else
		return WgSize(1,1);
}


//____ _onRequestRender() _____________________________________________________

void WgLayer::_onRequestRender( const WgRect& rect, const WgLayerHook * pHook )
{
	// Clip our geometry and put it in a dirtyrect-list

	WgPatches patches;
	patches.add( WgRect( rect, WgRect(0,0,m_size)) );

	// Remove portions of dirty rect that are covered by opaque upper siblings,
	// possibly filling list with many small dirty rects instead.

	WgLayerHook * pCover;

	if( pHook )
		pCover = pHook->_nextLayerHook();
	else
		pCover = _firstLayerHook();

	while( pCover )
	{
		if( pCover->m_geo.intersectsWith( rect ) )
			pCover->_widget()->_onMaskPatches( patches, pCover->m_geo, WgRect(0,0,65536,65536 ), _getBlendMode() );

		pCover = pCover->_nextLayerHook();
	}

	// Make request render calls

	for( const WgRect * pRect = patches.begin() ; pRect < patches.end() ; pRect++ )
		_requestRender( * pRect );
}


//____ _firstHook() ___________________________________________________________

WgHook* WgLayer::_firstHook() const
{
	if( m_baseHook._widget() )
		return const_cast<_BaseHook*>(&m_baseHook);
	else
		return _firstLayerHook();
}

//____ _lastHook() ____________________________________________________________

WgHook* WgLayer::_lastHook() const
{
	WgHook * p = _lastLayerHook();

	if( !p )
	{
		if( m_baseHook._widget() )
			return const_cast<_BaseHook*>(&m_baseHook);	
		return 0;
	}

	return p;
}

//____ _firstHookWithGeo() _____________________________________________________

WgHook * WgLayer::_firstHookWithGeo( WgRect& geo ) const
{
	if( m_baseHook._widget() )
	{
		geo = WgRect(0,0,m_size);
		return const_cast<_BaseHook*>(&m_baseHook);
	}
	else
	{
		WgLayerHook * p = _firstLayerHook();
		if( p )
			geo = p->m_geo;

		return p;
	}
}

//____ _nextHookWithGeo() _______________________________________________________

WgHook * WgLayer::_nextHookWithGeo( WgRect& geo, WgHook * pHook ) const
{
	WgHook * p = pHook->_nextHook();
	if( p )
		geo = ((WgLayerHook*)p)->m_geo;

	return p;
}


//____ _lastHookWithGeo() _____________________________________________________

WgHook * WgLayer::_lastHookWithGeo( WgRect& geo ) const
{
	WgLayerHook * p = _lastLayerHook();
	if( p )
	{
		geo = p->m_geo;
		return p;
	}
	else if( m_baseHook._widget() )
	{
		geo = WgRect(0,0,m_size);
		return const_cast<_BaseHook*>(&m_baseHook);
	}
	else
		return 0;
}

//____ _prevHookWithGeo() _______________________________________________________

WgHook * WgLayer::_prevHookWithGeo( WgRect& geo, WgHook * pHook ) const
{
	WgHook * p = pHook->_prevHook();
	if( p )
		geo = p->geo();

	return p;
}


//____ _onBaseChanged() _______________________________________________________

void WgLayer::_onBaseChanged()
{
	_onRequestRender( WgRect(0,0,m_size), 0 );
	_requestResize();
}

//_____________________________________________________________________________
void WgLayer::_BaseHook::_requestRender()
{
	m_pParent->_onRequestRender( WgRect( 0,0, m_pParent->m_size ), 0 );
}

//_____________________________________________________________________________
void WgLayer::_BaseHook::_requestRender( const WgRect& rect )
{
	m_pParent->_onRequestRender( rect, 0 );
}

//_____________________________________________________________________________
void WgLayer::_BaseHook::_requestResize()
{
	m_pParent->_requestResize();					// Just forward to our parent
}


//____ WgLayerHook::isInstanceOf() __________________________________________

bool WgLayerHook::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgHook::isInstanceOf(pClassName);
}

//____ WgLayerHook::className() _____________________________________________

const char * WgLayerHook::className( void ) const
{ 
	return CLASSNAME; 
}

//____ WgLayerHook::cast() __________________________________________________

WgLayerHook_p WgLayerHook::cast( const WgHook_p& pHook )
{
	if( pHook && pHook->isInstanceOf(CLASSNAME) )
		return WgLayerHook_p( static_cast<WgLayerHook*>(pHook.rawPtr()) );

	return 0;
}

//_____________________________________________________________________________
WgCoord WgLayerHook::globalPos() const
{
	return parent()->globalPos() + m_geo.pos();
}

//_____________________________________________________________________________
WgRect WgLayerHook::globalGeo() const
{
	return m_geo + parent()->globalPos();
}

//_____________________________________________________________________________
void WgLayerHook::_requestRender()
{
	parent()->_onRequestRender( m_geo, this );
}

//_____________________________________________________________________________
void WgLayerHook::_requestRender( const WgRect& rect )
{
	parent()->_onRequestRender( rect + m_geo.pos(), this );
}

//_____________________________________________________________________________
WgHook * WgLayerHook::_prevHook() const
{
	WgHook * p = _prevLayerHook();
	if( !p )
	{
		WgContainer * c = _parent();
		if( c != 0 )
		{
			WgLayer * l = static_cast<WgLayer*>(c);
			if( l->m_baseHook._widget() )	
				p = &l->m_baseHook;
		}
	}
	return p;
}

//_____________________________________________________________________________
WgHook * WgLayerHook::_nextHook() const
{
	return _nextLayerHook();
}


//_____________________________________________________________________________
WgLayer_p WgLayerHook::parent() const
{ 
	return static_cast<WgLayer*>(_parent()); 
}
