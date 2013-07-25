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


//____ SetBaseChild() _________________________________________________________

WgHook * WgLayer::SetBaseChild( const WgWidgetPtr& pWidget )
{
	m_baseHook._setWidget(pWidget.GetRealPtr());
	_onBaseChanged();
	return &m_baseHook;
}

//____ BaseChild() ____________________________________________________________

WgWidgetPtr WgLayer::BaseChild()
{
	return m_baseHook._widget();
}


//____ RemoveBaseChild() _____________________________________________________

bool WgLayer::RemoveBaseChild()
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
		pCover = pHook->Next();
	else
		pCover = _firstLayerHook();

	while( pCover )
	{
		if( pCover->m_geo.IntersectsWith( rect ) )
			pCover->_widget()->_onMaskPatches( patches, pCover->m_geo, WgRect(0,0,65536,65536 ), _getBlendMode() );

		pCover = pCover->Next();
	}

	// Make request render calls

	for( const WgRect * pRect = patches.Begin() ; pRect < patches.End() ; pRect++ )
		_requestRender( * pRect );
}


//____ _onBaseChanged() _______________________________________________________

void WgLayer::_onBaseChanged()
{
	_onRequestRender( WgRect(0,0,m_size), 0 );
	_requestResize();
}

//_____________________________________________________________________________
void WgLayer::BaseHook::_requestRender()
{
	m_pParent->_onRequestRender( WgRect( 0,0, m_pParent->m_size ), 0 );
}

//_____________________________________________________________________________
void WgLayer::BaseHook::_requestRender( const WgRect& rect )
{
	m_pParent->_onRequestRender( rect, 0 );
}

//_____________________________________________________________________________
void WgLayer::BaseHook::_requestResize()
{
	m_pParent->_requestResize();					// Just forward to our parent
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
WgLayerPtr WgLayerHook::Parent() const
{ 
	return static_cast<WgLayer*>(_parent()); 
}