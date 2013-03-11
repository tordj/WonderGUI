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

#include <assert.h>
#include <wg_listpanel.h>


static const char	c_widgetType[] = {"ListPanel"};
static const char	c_hookType[] = {"ListHook"};


WgListHook::WgListHook( WgListPanel * pParent ) : m_pParent(pParent), m_height(0), m_bSelected(false), m_bSelectable(true)
{
}

const char * WgListHook::Type( void ) const
{
	return ClassType();
}

const char * WgListHook::ClassType()
{
	return c_hookType;
}


WgContainer* WgListHook::_parent() const
{
	return m_pParent;
}

//____ Constructor ____________________________________________________________

WgListPanel::WgListPanel() : m_nPreferredWidth(0)
{
	m_bSiblingsOverlap = false;
}

//____ Destructor _____________________________________________________________

WgListPanel::~WgListPanel()
{
}

//____ Type() _________________________________________________________________

const char * WgListPanel::Type() const
{
	return c_widgetType;
}

//____ GetClass() ____________________________________________________________

const char * WgListPanel::GetClass()
{
	return c_widgetType;
}

//____ HeightForWidth() _______________________________________________________

int WgListPanel::HeightForWidth( int width ) const
{
	if( width == m_size.w )
		return m_size.h;

	int height = 0;
	WgListHook * pHook = FirstHook();

	while( pHook )
	{
		height += pHook->Widget()->HeightForWidth(width);
		pHook = pHook->Next();
	}

	return height;
}

//____ WidthForHeight() _______________________________________________________

int WgListPanel::WidthForHeight( int height ) const
{
	return PreferredSize().w;
}

//____ PreferredSize() _____________________________________________________________

WgSize WgListPanel::PreferredSize() const
{
	return m_preferredSize;
}

//____ SetSelectMode() ________________________________________________________

void WgListPanel::SetSelectMode( WgSelectMode mode )
{
	if( mode == m_selectMode )
		return;

	m_selectMode = mode;
}

//____ GetSelected() __________________________________________________________

WgWidgetCollectionPtr WgListPanel::GetSelected() const
{
	return WgWidgetCollectionPtr();
}

//____ SetRowBlocks() _________________________________________________________

void WgListPanel::SetRowBlocks( const WgBlocksetPtr& pRowBlocks )
{
	if( pRowBlocks == m_pRowBlocks[0] && pRowBlocks == m_pRowBlocks[1] )
		return;

	m_pRowBlocks[0] = pRowBlocks;
	m_pRowBlocks[1] = pRowBlocks;
	_requestRender();
}

void WgListPanel::SetRowBlocks( const WgBlocksetPtr& pOddRowBlocks, const WgBlocksetPtr& pEvenRowBlocks )
{
	if( pOddRowBlocks == m_pRowBlocks[0] && pEvenRowBlocks == m_pRowBlocks[1] )
		return;

	m_pRowBlocks[0] = pOddRowBlocks;
	m_pRowBlocks[1] = pEvenRowBlocks;
	_requestRender();
}


//____ _onNewSize() ___________________________________________________________

void WgListPanel::_onNewSize( const WgSize& size )
{
	if( size.w != m_size.w )
	{
		_adaptChildrenToWidth( size.w );
		_requestRender();
	}
}

//____ _hookGeo() _____________________________________________________________

WgRect WgListPanel::_hookGeo( const WgVectorHook * pHook )
{
	WgListHook * p = FirstHook();
	int ofs = 0;

	while( p != pHook )
	{
		ofs += p->m_height;
		p = p->Next();
	}

	return WgRect(0,ofs,m_size.w,p->m_height);
}

//____ _firstHookWithGeo() _____________________________________________________

WgHook * WgListPanel::_firstHookWithGeo( WgRect& geo ) const
{
	WgListHook * p = FirstHook();
	if( p )
	{
		geo = WgRect(0,0,m_size.w,p->m_height);
		if( !p->m_bVisible )
			geo.h = 0;
	}

	return p;
}

//____ _nextHookWithGeo() ______________________________________________________

WgHook * WgListPanel::_nextHookWithGeo( WgRect& geo, WgHook * pHook ) const
{
	WgListHook * p = (WgListHook*) pHook->Next();
	if( p )
	{
		geo.y += geo.h;

		if( p->m_bVisible )
			geo.h = p->m_height;
		else
			geo.h = 0;
	}
	return p;
}

//____ _lastHookWithGeo() _____________________________________________________

WgHook * WgListPanel::_lastHookWithGeo( WgRect& geo ) const
{
	WgListHook * p = LastHook();
	if( p )
	{
		geo = WgRect(0,m_size.h - p->m_height,m_size.w,p->m_height);
		if( !p->m_bVisible )
			geo.h = 0;
	}

	return p;
}

//____ _prevHookWithGeo() ______________________________________________________

WgHook * WgListPanel::_prevHookWithGeo( WgRect& geo, WgHook * pHook ) const
{
	WgListHook * p = (WgListHook*) pHook->Prev();
	if( p )
	{
		int h = p->m_bVisible ? p->m_height : 0;

		geo.h = h;
		geo.y -= h;
	}
	return p;
}


//____ _onResizeRequested() ___________________________________________________

void WgListPanel::_onResizeRequested( WgVectorHook * _pHook )
{
	WgListHook * pHook = static_cast<WgListHook*>(_pHook);

	// Update PreferredSize

	WgSize oldPreferredSize = pHook->m_preferredSize;
	pHook->m_preferredSize = pHook->Widget()->PreferredSize();

	m_preferredSize.h += pHook->m_preferredSize.h - oldPreferredSize.h;

	if( pHook->m_preferredSize.w != oldPreferredSize.w )
	{
		if( pHook->m_preferredSize.w > m_preferredSize.w )				// Is new best size bigger than previous?
		{
			m_preferredSize.w = pHook->m_preferredSize.w;
			m_nPreferredWidth = 1;
		}
		else if( oldPreferredSize.w == m_preferredSize.w )			// Was this our widest child (or one of them)?
		{
			m_nPreferredWidth--;
			if( m_nPreferredWidth == 0 )
				_refreshDefaultWidth();
		}
	}

	// We accept any change to height for current width ourselves

	int newHeight = pHook->Widget()->HeightForWidth(m_size.w);

	bool bRequestRender = false;
	if( newHeight != m_size.h )
		bRequestRender = true;			// Request render once we have resized.

	m_size.h += newHeight - pHook->m_height;
	pHook->m_height = newHeight;

	// Now we have up-to-date data for HeightForWidth() and PreferredSize() requests,
	// we notify our parent that we might need a resize.

	_requestResize();

	// Force a render since we can't know for sure if RequestResize() got us one.

	_renderFromChildOnward( pHook );
}

//____ _onRenderRequested() ___________________________________________________

void  WgListPanel::_onRenderRequested( WgVectorHook * pHook )
{
	if( !pHook->IsVisible() )
		return;

	WgRect rect = _hookGeo(pHook);
	if( !rect.IsEmpty() )
		_requestRender(rect);
}

void  WgListPanel::_onRenderRequested( WgVectorHook * pHook, const WgRect& rect )
{
	if( !pHook->IsVisible() )
		return;

	WgRect hookGeo = _hookGeo(pHook);
	WgRect clippedRect( hookGeo, rect + hookGeo.Pos() );

	if( !clippedRect.IsEmpty() )
		_requestRender(clippedRect);
}

//____ _onWidgetAppeared() _____________________________________________________

void  WgListPanel::_onWidgetAppeared( WgVectorHook * pInserted )
{
	WgListHook * pHook = static_cast<WgListHook*>(pInserted);

	// Update stored PreferredSize

	pHook->m_preferredSize = pHook->Widget()->PreferredSize();

	if( m_preferredSize.w == pHook->m_preferredSize.w )
	{
		m_nPreferredWidth++;
	}
	else if( m_preferredSize.w < pHook->m_preferredSize.w )
	{
		m_preferredSize.w = pHook->m_preferredSize.w;
		m_nPreferredWidth = 1;
	}

	m_preferredSize.h += pHook->m_preferredSize.h;

	// We set Widget to same width as ours to start with, our parent will
	// expand us in RequestResize() if it wants to.

	int	height = pHook->Widget()->HeightForWidth(m_size.w);

	pHook->m_height = height;
	m_size.h += height;

	pHook->Widget()->_onNewSize( WgSize(m_size.w,height) );

	// Request and handle possible resize.

	_requestResize();

	// Request render on affected part.

	_renderFromChildOnward(pHook);
}

//____ _onWidgetDisappeared() __________________________________________________

void WgListPanel::_onWidgetDisappeared( WgVectorHook * pToBeRemoved )
{
	WgListHook * pHook = static_cast<WgListHook*>(pToBeRemoved);

	// Update stored PreferredSize

	m_preferredSize.h -= pHook->m_preferredSize.h;

	if( m_preferredSize.w == pHook->m_preferredSize.w )
	{
		m_nPreferredWidth--;
		if( m_nPreferredWidth == 0 )
		{
			// Refresh best size, ignoring Widget to be removed.

			int w = pHook->m_preferredSize.w;
			pHook->m_preferredSize.w = 0;
			_refreshDefaultWidth();
			pHook->m_preferredSize.w = w;
		}
	}

	//

	_renderFromChildOnward(pHook);
	m_size.h -= pHook->m_height;
	pHook->m_height = 0;				// If widget is just being hidden it needs to have m_height set to 0.
	_requestResize();
}

//____ _onWidgetsReordered() ___________________________________________________

void  WgListPanel::_onWidgetsReordered()
{
	_requestRender();
}

//____ _adaptChildrenToWidth() ________________________________________________
// Adapts all non-hidden widgets to our width and updates m_size.

void WgListPanel::_adaptChildrenToWidth( int width )
{
	m_size.h = 0;
	m_size.w = width;
	WgListHook * pHook = FirstHook();

	while( pHook )
	{
		if( pHook->m_bVisible )
		{
			int height = pHook->Widget()->HeightForWidth( width );
			if( height == -1 )
				height = pHook->m_preferredSize.h;

			pHook->Widget()->_onNewSize( WgSize(width,height) );
			pHook->m_height = height;
			m_size.h += height;
		}
		pHook = pHook->Next();
	}
}

//____ _refreshDefaultWidth() ______________________________________________________
// Updates m_preferredSize.w and m_nPreferredWidth. Relies on m_preferredSize of the visible
// hooks to have up-to-date data.

void WgListPanel::_refreshDefaultWidth()
{
	m_preferredSize.w = 0;
	m_nPreferredWidth = 0;
	WgListHook * pHook = FirstHook();

	while( pHook )
	{
		if( pHook->m_bVisible && pHook->m_preferredSize.w >= m_preferredSize.w )
		{
			if( pHook->m_preferredSize.w > m_preferredSize.w )
			{
				m_preferredSize.w = pHook->m_preferredSize.w;
				m_nPreferredWidth = 1;
			}
			else
				m_nPreferredWidth++;
		}
		pHook = pHook->Next();
	}
}

//____ _refreshPreferredSize() ______________________________________________________
// Refreshes m_preferredSize for all visible hooks, m_preferredSize and m_nPreferredWidth with fresh
// info straight from the children.

void WgListPanel::_refreshPreferredSize()
{
	m_preferredSize.Clear();
	m_nPreferredWidth = 0;
	WgListHook * pHook = FirstHook();

	while( pHook )
	{
		if( pHook->m_bVisible )
		{
			pHook->m_preferredSize = pHook->Widget()->PreferredSize();

			if( pHook->m_preferredSize.w > m_preferredSize.w )
			{
				m_preferredSize.w = pHook->m_preferredSize.w;
				m_nPreferredWidth = 1;
			}
			else if( pHook->m_preferredSize.w == m_preferredSize.w )
				m_nPreferredWidth++;

			m_preferredSize.h += pHook->m_preferredSize.h;
		}
		pHook = pHook->Next();
	}
}

//____ _refreshAllWidgets() ____________________________________________________

void  WgListPanel::_refreshAllWidgets()
{
	_refreshPreferredSize();
	_adaptChildrenToWidth( m_size.w );
	_requestResize();
	_requestRender();
}

//____ _renderFromChildOnward() _______________________________________________

void WgListPanel::_renderFromChildOnward( WgVectorHook * pHook )
{
	WgRect geo = _hookGeo(pHook);
	geo.h = m_size.h - geo.y;
	_requestRender( geo );
}

//____ _newHook() _____________________________________________________________

WgVectorHook *  WgListPanel::_newHook()
{
	return new WgListHook( this );
}

