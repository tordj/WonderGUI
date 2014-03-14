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

//____ IsInstanceOf() _________________________________________________________

bool WgLayer::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgContainer::IsInstanceOf(pClassName);
}

//____ ClassName() ____________________________________________________________

const char * WgLayer::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ Cast() _________________________________________________________________

WgLayerPtr WgLayer::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgLayerPtr( static_cast<WgLayer*>(pObject.GetRealPtr()) );

	return 0;
}


//____ SetBaseWidget() _________________________________________________________

WgHookPtr WgLayer::SetBaseWidget( const WgWidgetPtr& pWidget )
{
	m_baseHook._setWidget(pWidget.GetRealPtr());
	_onBaseChanged();
	return &m_baseHook;
}

//____ BaseWidget() ____________________________________________________________

WgWidgetPtr WgLayer::BaseWidget()
{
	return m_baseHook._widget();
}


//____ RemoveBaseWidget() _____________________________________________________

bool WgLayer::RemoveBaseWidget()
{
	if( !m_baseHook._widget() )
		return false;

	m_baseHook._setWidget(0);
	_onBaseChanged();

	return true;
}

//____ HeightForWidth() _______________________________________________________

int WgLayer::HeightForWidth( int width ) const
{
	if( m_baseHook._widget() )
		return m_baseHook._widget()->HeightForWidth( width );
	else
		return WgWidget::HeightForWidth(width);
}

//____ WidthForHeight() _______________________________________________________

int WgLayer::WidthForHeight( int height ) const
{
	if( m_baseHook._widget() )
		return m_baseHook._widget()->WidthForHeight( height );
	else
		return WgWidget::WidthForHeight(height);
}

//____ PreferredSize() _____________________________________________________________

WgSize WgLayer::PreferredSize() const
{
	if( m_baseHook._widget() )
		return m_baseHook._widget()->PreferredSize();
	else
		return WgSize(1,1);
}


//____ _onRequestRender() _____________________________________________________

void WgLayer::_onRequestRender( const WgRect& rect, const WgLayerHook * pHook )
{
	// Clip our geometry and put it in a dirtyrect-list

	WgPatches patches;
	patches.Add( WgRect( rect, WgRect(0,0,m_size)) );

	// Remove portions of dirty rect that are covered by opaque upper siblings,
	// possibly filling list with many small dirty rects instead.

	WgLayerHook * pCover;

	if( pHook )
		pCover = pHook->_nextLayerHook();
	else
		pCover = _firstLayerHook();

	while( pCover )
	{
		if( pCover->m_geo.IntersectsWith( rect ) )
			pCover->_widget()->_onMaskPatches( patches, pCover->m_geo, WgRect(0,0,65536,65536 ), _getBlendMode() );

		pCover = pCover->_nextLayerHook();
	}

	// Make request render calls

	for( const WgRect * pRect = patches.Begin() ; pRect < patches.End() ; pRect++ )
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
		geo = p->Geo();

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


//____ WgLayerHook::IsInstanceOf() __________________________________________

bool WgLayerHook::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgHook::IsInstanceOf(pClassName);
}

//____ WgLayerHook::ClassName() _____________________________________________

const char * WgLayerHook::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ WgLayerHook::Cast() __________________________________________________

WgLayerHookPtr WgLayerHook::Cast( const WgHookPtr& pHook )
{
	if( pHook && pHook->IsInstanceOf(CLASSNAME) )
		return WgLayerHookPtr( static_cast<WgLayerHook*>(pHook.GetRealPtr()) );

	return 0;
}

//_____________________________________________________________________________
WgCoord WgLayerHook::ScreenPos() const
{
	return Parent()->ScreenPos() + m_geo.Pos();
}

//_____________________________________________________________________________
WgRect WgLayerHook::ScreenGeo() const
{
	return m_geo + Parent()->ScreenPos();
}

//_____________________________________________________________________________
void WgLayerHook::_requestRender()
{
	Parent()->_onRequestRender( m_geo, this );
}

//_____________________________________________________________________________
void WgLayerHook::_requestRender( const WgRect& rect )
{
	Parent()->_onRequestRender( rect + m_geo.Pos(), this );
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
WgLayerPtr WgLayerHook::Parent() const
{ 
	return static_cast<WgLayer*>(_parent()); 
}