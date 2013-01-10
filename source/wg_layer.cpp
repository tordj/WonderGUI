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


//____ Constructor ____________________________________________________________

WgLayer::WgLayer()
{
	m_baseHook.m_pParent = this;
}

//____ IsLayer() ______________________________________________________________

bool WgLayer::IsLayer() const
{
	return true;
}

//____ CastToLayer() _______________________________________________________

WgLayer * WgLayer::CastToLayer()
{
	return this;
}

const WgLayer * WgLayer::CastToLayer() const
{
	return this;
}

//____ SetBase() _________________________________________________________

WgHook * WgLayer::SetBase( WgWidget * pWidget )
{
	// Replace Widget

	WgWidget * pOldWidget = m_baseHook._releaseWidget();
	if( pOldWidget )
		delete pOldWidget;
	m_baseHook._attachWidget(pWidget);
	_onBaseChanged();
	return &m_baseHook;
}

//____ Base() ____________________________________________________________

WgWidget * WgLayer::Base()
{
	return m_baseHook.Widget();
}

//____ DeleteBase() ______________________________________________________

bool WgLayer::DeleteBase()
{
	WgWidget * pWidget = m_baseHook._releaseWidget();
	if( pWidget )
	{
		_onBaseChanged();
		delete pWidget;
		return true;
	}

	return false;
}

//____ ReleaseBase() _____________________________________________________

WgWidget * WgLayer::ReleaseBase()
{
	WgWidget * pWidget = m_baseHook._releaseWidget();
	if( pWidget )
		_onBaseChanged();

	return pWidget;
}

//____ HeightForWidth() _______________________________________________________

int WgLayer::HeightForWidth( int width ) const
{
	if( m_baseHook.Widget() )
		return m_baseHook.Widget()->HeightForWidth( width );
	else
		return WgWidget::HeightForWidth(width);
}

//____ WidthForHeight() _______________________________________________________

int WgLayer::WidthForHeight( int height ) const
{
	if( m_baseHook.Widget() )
		return m_baseHook.Widget()->WidthForHeight( height );
	else
		return WgWidget::WidthForHeight(height);
}

//____ PreferredSize() _____________________________________________________________

WgSize WgLayer::PreferredSize() const
{
	if( m_baseHook.Widget() )
		return m_baseHook.Widget()->PreferredSize();
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
			pCover->Widget()->_onMaskPatches( patches, pCover->m_geo, WgRect(0,0,65536,65536 ), _getBlendMode() );

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
WgLayer* WgLayerHook::Parent() const
{ 
	return static_cast<WgLayer*>(_parent()); 
}