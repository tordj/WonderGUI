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
#include <wg_vpanel.h>


static const char	c_widgetType[] = {"VPanel"};
static const char	c_hookType[] = {"VHook"};


WgVHook::WgVHook( WgVPanel * pParent ) : m_pParent(pParent)
{
}

const char * WgVHook::Type( void ) const
{
	return ClassType();
}

const char * WgVHook::ClassType()
{
	return c_hookType;
}


WgPanel* WgVHook::_parent() const
{
	return m_pParent;
}

//____ Constructor ____________________________________________________________

WgVPanel::WgVPanel() : m_nBestWidth(0)
{
	m_bSiblingsOverlap = false;
}

//____ Destructor _____________________________________________________________

WgVPanel::~WgVPanel()
{
}

//____ Type() _________________________________________________________________

const char * WgVPanel::Type() const
{
	return c_widgetType;
}

//____ GetClass() ____________________________________________________________

const char * WgVPanel::GetClass()
{
	return c_widgetType;
}

//____ HeightForWidth() _______________________________________________________

int WgVPanel::HeightForWidth( int width ) const
{
	if( width == m_size.w )
		return m_size.h;

	int height = 0;
	WgVHook * pHook = FirstHook();

	while( pHook )
	{
		height += pHook->Widget()->HeightForWidth(width);
		pHook = pHook->Next();
	}

	return height;
}

//____ WidthForHeight() _______________________________________________________

int WgVPanel::WidthForHeight( int height ) const
{
	return -1; // No recommendation. Should this maybe be DefaultSize().w instead?
}

//____ DefaultSize() _____________________________________________________________

WgSize WgVPanel::DefaultSize() const
{
	return m_bestSize;
}

//____ _onNewSize() ___________________________________________________________

void WgVPanel::_onNewSize( const WgSize& size )
{
	if( size.w != m_size.w )
	{
		_adaptChildrenToWidth( size.w );
		_requestRender();
	}
}

//____ _hookGeo() _____________________________________________________________

WgRect WgVPanel::_hookGeo( const WgSortableHook * pHook )
{
	WgVHook * p = FirstHook();
	int ofs = 0;

	while( p != pHook )
	{
		ofs += p->m_height;
		p = p->Next();
	}

	return WgRect(0,ofs,m_size.w,p->m_height);
}

//____ _firstHookWithGeo() _____________________________________________________

WgHook * WgVPanel::_firstHookWithGeo( WgRect& geo ) const
{
	WgVHook * p = FirstHook();
	if( p )
	{
		geo = WgRect(0,0,m_size.w,p->m_height);
		if( !p->m_bVisible )
			geo.h = 0;
	}

	return p;
}

//____ _nextHookWithGeo() ______________________________________________________

WgHook * WgVPanel::_nextHookWithGeo( WgRect& geo, WgHook * pHook ) const
{
	WgVHook * p = (WgVHook*) pHook->Next();
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

WgHook * WgVPanel::_lastHookWithGeo( WgRect& geo ) const
{
	WgVHook * p = LastHook();
	if( p )
	{
		geo = WgRect(0,m_size.h - p->m_height,m_size.w,p->m_height);
		if( !p->m_bVisible )
			geo.h = 0;
	}

	return p;
}

//____ _prevHookWithGeo() ______________________________________________________

WgHook * WgVPanel::_prevHookWithGeo( WgRect& geo, WgHook * pHook ) const
{
	WgVHook * p = (WgVHook*) pHook->Prev();
	if( p )
	{
		int h = p->m_bVisible ? p->m_height : 0;

		geo.h = h;
		geo.y -= h;
	}
	return p;
}


//____ _onResizeRequested() ___________________________________________________

void WgVPanel::_onResizeRequested( WgSortableHook * _pHook )
{
	WgVHook * pHook = static_cast<WgVHook*>(_pHook);

	// Update BestSize

	WgSize oldBestSize = pHook->m_bestSize;
	pHook->m_bestSize = pHook->Widget()->DefaultSize();

	m_bestSize.h += pHook->m_bestSize.h - oldBestSize.h;

	if( pHook->m_bestSize.w != oldBestSize.w )
	{
		if( pHook->m_bestSize.w > m_bestSize.w )				// Is new best size bigger than previous?
		{
			m_bestSize.w = pHook->m_bestSize.w;
			m_nBestWidth = 1;
		}
		else if( oldBestSize.w == m_bestSize.w )			// Was this our widest child (or one of them)?
		{
			m_nBestWidth--;
			if( m_nBestWidth == 0 )
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

	// Now we have up-to-date data for HeightForWidth() and DefaultSize() requests,
	// we notify our parent that we might need a resize.

	_requestResize();

	// Force a render since we can't know for sure if RequestResize() got us one.

	_renderFromChildOnward( pHook );
}

//____ _onRenderRequested() ___________________________________________________

void  WgVPanel::_onRenderRequested( WgSortableHook * pHook )
{
	if( !pHook->IsVisible() )
		return;

	WgRect rect = _hookGeo(pHook);
	if( !rect.IsEmpty() )
		_requestRender(rect);
}

void  WgVPanel::_onRenderRequested( WgSortableHook * pHook, const WgRect& rect )
{
	if( !pHook->IsVisible() )
		return;

	WgRect hookGeo = _hookGeo(pHook);
	WgRect clippedRect( hookGeo, rect + hookGeo.Pos() );

	if( !clippedRect.IsEmpty() )
		_requestRender(clippedRect);
}

//____ _onWidgetAppeared() _____________________________________________________

void  WgVPanel::_onWidgetAppeared( WgSortableHook * pInserted )
{
	WgVHook * pHook = static_cast<WgVHook*>(pInserted);

	// Update stored BestSize

	pHook->m_bestSize = pHook->Widget()->DefaultSize();

	if( m_bestSize.w == pHook->m_bestSize.w )
	{
		m_nBestWidth++;
	}
	else if( m_bestSize.w < pHook->m_bestSize.w )
	{
		m_bestSize.w = pHook->m_bestSize.w;
		m_nBestWidth = 1;
	}

	m_bestSize.h += pHook->m_bestSize.h;

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

void WgVPanel::_onWidgetDisappeared( WgSortableHook * pToBeRemoved )
{
	WgVHook * pHook = static_cast<WgVHook*>(pToBeRemoved);

	// Update stored BestSize

	m_bestSize.h -= pHook->m_bestSize.h;

	if( m_bestSize.w == pHook->m_bestSize.w )
	{
		m_nBestWidth--;
		if( m_nBestWidth == 0 )
		{
			// Refresh best size, ignoring Widget to be removed.

			int w = pHook->m_bestSize.w;
			pHook->m_bestSize.w = 0;
			_refreshDefaultWidth();
			pHook->m_bestSize.w = w;
		}
	}

	//

	_renderFromChildOnward(pHook);
	m_size.h -= pHook->m_height;
	pHook->m_height = 0;				// If widget is just being hidden it needs to have m_height set to 0.
	_requestResize();
}

//____ _onWidgetsReordered() ___________________________________________________

void  WgVPanel::_onWidgetsReordered()
{
	_requestRender();
}

//____ _onSpaceReallocated() _____________________________________________

void WgVPanel::_onSpaceReallocated()
{

}

//____ _adaptChildrenToWidth() ________________________________________________
// Adapts all non-hidden widgets to our width and updates m_size.

void WgVPanel::_adaptChildrenToWidth( int width )
{
	m_size.h = 0;
	m_size.w = width;
	WgVHook * pHook = FirstHook();

	while( pHook )
	{
		if( pHook->m_bVisible )
		{
			int height = pHook->Widget()->HeightForWidth( width );
			if( height == -1 )
				height = pHook->m_bestSize.h;

			pHook->Widget()->_onNewSize( WgSize(width,height) );
			pHook->m_height = height;
			m_size.h += height;
		}
		pHook = pHook->Next();
	}
}

//____ _refreshDefaultWidth() ______________________________________________________
// Updates m_bestSize.w and m_nBestWidth. Relies on m_bestSize of the visible
// hooks to have up-to-date data.

void WgVPanel::_refreshDefaultWidth()
{
	m_bestSize.w = 0;
	m_nBestWidth = 0;
	WgVHook * pHook = FirstHook();

	while( pHook )
	{
		if( pHook->m_bVisible && pHook->m_bestSize.w >= m_bestSize.w )
		{
			if( pHook->m_bestSize.w > m_bestSize.w )
			{
				m_bestSize.w = pHook->m_bestSize.w;
				m_nBestWidth = 1;
			}
			else
				m_nBestWidth++;
		}
		pHook = pHook->Next();
	}
}

//____ _refreshDefaultSize() ______________________________________________________
// Refreshes m_bestSize for all visible hooks, m_bestSize and m_nBestWidth with fresh
// info straight from the children.

void WgVPanel::_refreshDefaultSize()
{
	m_bestSize.Clear();
	m_nBestWidth = 0;
	WgVHook * pHook = FirstHook();

	while( pHook )
	{
		if( pHook->m_bVisible )
		{
			pHook->m_bestSize = pHook->Widget()->DefaultSize();

			if( pHook->m_bestSize.w > m_bestSize.w )
			{
				m_bestSize.w = pHook->m_bestSize.w;
				m_nBestWidth = 1;
			}
			else if( pHook->m_bestSize.w == m_bestSize.w )
				m_nBestWidth++;

			m_bestSize.h += pHook->m_bestSize.h;
		}
		pHook = pHook->Next();
	}
}

//____ _refreshAllWidgets() ____________________________________________________

void  WgVPanel::_refreshAllWidgets()
{
	_refreshDefaultSize();
	_adaptChildrenToWidth( m_size.w );
	_requestResize();
	_requestRender();
}

//____ _renderFromChildOnward() _______________________________________________

void WgVPanel::_renderFromChildOnward( WgSortableHook * pHook )
{
	WgRect geo = _hookGeo(pHook);
	geo.h = m_size.h - geo.y;
	_requestRender( geo );
}

//____ _newHook() _____________________________________________________________

WgSortableHook *  WgVPanel::_newHook()
{
	return new WgVHook( this );
}

