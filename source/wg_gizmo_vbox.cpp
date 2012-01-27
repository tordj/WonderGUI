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
#include <wg_gizmo_vbox.h>


static const char	c_gizmoType[] = {"VBoxLayout"};


WgVBoxHook::WgVBoxHook( WgVBoxLayout * pParent ) : m_pParent(pParent)
{
}


WgGizmoContainer* WgVBoxHook::_parent() const
{
	return m_pParent;
}

//____ Constructor ____________________________________________________________

WgVBoxLayout::WgVBoxLayout() : m_nBestWidth(0)
{
}

//____ Destructor _____________________________________________________________

WgVBoxLayout::~WgVBoxLayout()
{
}

//____ Type() _________________________________________________________________

const char * WgVBoxLayout::Type() const
{
	return c_gizmoType;
}

//____ GetMyType() ____________________________________________________________

const char * WgVBoxLayout::GetMyType()
{
	return c_gizmoType;
}

//____ HeightForWidth() _______________________________________________________

int WgVBoxLayout::HeightForWidth( int width ) const
{
	if( width == m_size.w )
		return m_size.h;

	int height = 0;
	WgVBoxHook * pHook = FirstHook();

	while( pHook )
	{
		height += pHook->Gizmo()->HeightForWidth(width);
		pHook = pHook->Next();
	}

	return height;
}

//____ WidthForHeight() _______________________________________________________

int WgVBoxLayout::WidthForHeight( int height ) const
{
	return -1; // No recommendation. Should this maybe be DefaultSize().w instead?
}

//____ DefaultSize() _____________________________________________________________

WgSize WgVBoxLayout::DefaultSize() const
{
	return m_bestSize;
}

//____ _onNewSize() ___________________________________________________________

void WgVBoxLayout::_onNewSize( const WgSize& size )
{
	if( size.w != m_size.w )
	{
		_adaptChildrenToWidth( size.w );
		RequestRender();
	}
}

//____ _hookGeo() _____________________________________________________________

WgRect WgVBoxLayout::_hookGeo( const WgOrderedHook * pHook )
{
	WgVBoxHook * p = FirstHook();
	int ofs = 0;

	while( p != pHook )
	{
		ofs += p->m_height;
		p = p->Next();
	}

	return WgRect(0,ofs,m_size.w,p->m_height);
}

//____ _firstHookWithGeo() _____________________________________________________

WgHook * WgVBoxLayout::_firstHookWithGeo( WgRect& geo ) const
{
	WgVBoxHook * p = FirstHook();
	if( p )
	{
		geo = WgRect(0,0,m_size.w,p->m_height);
		if( p->m_bHidden )
			geo.h = 0;
	}

	return p;
}

//____ _nextHookWithGeo() ______________________________________________________

WgHook * WgVBoxLayout::_nextHookWithGeo( WgRect& geo, WgHook * pHook ) const
{
	WgVBoxHook * p = (WgVBoxHook*) pHook->Next();
	if( p )
	{
		geo.y += geo.h;

		if( p->m_bHidden )
			geo.h = 0;
		else
			geo.h = p->m_height;
	}	
	return p;
}

//____ _onResizeRequested() ___________________________________________________

void WgVBoxLayout::_onResizeRequested( WgOrderedHook * _pHook )
{
	WgVBoxHook * pHook = static_cast<WgVBoxHook*>(_pHook);

	// Update BestSize

	WgSize oldBestSize = pHook->m_bestSize;
	pHook->m_bestSize = pHook->Gizmo()->DefaultSize();

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

	int newHeight = pHook->Gizmo()->HeightForWidth(m_size.w);

	bool bRequestRender = false;
	if( newHeight != m_size.h )
		bRequestRender = true;			// Request render once we have resized.

	m_size.h += newHeight - pHook->m_height;
	pHook->m_height = newHeight;

	// Now we have up-to-date data for HeightForWidth() and DefaultSize() requests,
	// we notify our parent that we might need a resize.

	RequestResize();

	// Force a render if since we can't know for sure if RequestResize() got us one.

	_renderFromChildOnward( pHook );
}

//____ _onRenderRequested() ___________________________________________________

void  WgVBoxLayout::_onRenderRequested( WgOrderedHook * pHook )
{
	if( pHook->Hidden() )
		return;
	
	WgRect rect = _hookGeo(pHook);
	if( !rect.IsEmpty() )
		RequestRender(rect);
}

void  WgVBoxLayout::_onRenderRequested( WgOrderedHook * pHook, const WgRect& rect )
{
	if( pHook->Hidden() )
		return;

	WgRect hookGeo = _hookGeo(pHook);
	WgRect clippedRect( hookGeo, rect + hookGeo.Pos() );

	if( !clippedRect.IsEmpty() )
		RequestRender(clippedRect);
}

//____ _onGizmoAppeared() _____________________________________________________

void  WgVBoxLayout::_onGizmoAppeared( WgOrderedHook * pInserted )
{
	WgVBoxHook * pHook = static_cast<WgVBoxHook*>(pInserted);

	// Update stored BestSize

	pHook->m_bestSize = pHook->Gizmo()->DefaultSize();

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

	// We set Gizmo to same width as ours to start with, our parent will
	// expand us in RequestResize() if it wants to.

	int	height = pHook->Gizmo()->HeightForWidth(m_size.w);

	pHook->m_height = height;
	m_size.h += height;

	pHook->Gizmo()->_onNewSize( WgSize(m_size.w,height) );

	// Request and handle possible resize.

	RequestResize();

	// Request render on affected part.

	_renderFromChildOnward(pHook);
}

//____ _onGizmoDisappeared() __________________________________________________

void WgVBoxLayout::_onGizmoDisappeared( WgOrderedHook * pToBeRemoved )
{
	WgVBoxHook * pHook = static_cast<WgVBoxHook*>(pToBeRemoved);

	// Update stored BestSize

	m_bestSize.h -= pHook->m_bestSize.h;

	if( m_bestSize.w == pHook->m_bestSize.w )
	{
		m_nBestWidth--;
		if( m_nBestWidth == 0 )
		{
			// Refresh best size, ignoring Gizmo to be removed.

			int w = pHook->m_bestSize.w;
			pHook->m_bestSize.w = 0;
			_refreshDefaultWidth();
			pHook->m_bestSize.w = w;
		}
	}

	//

	_renderFromChildOnward(pHook);
	m_size.h -= pHook->m_height;
	pHook->m_height = 0;				// If gizmo is just being hidden it needs to have m_height set to 0.
	RequestResize();
}

//____ _onGizmosReordered() ___________________________________________________

void  WgVBoxLayout::_onGizmosReordered()
{
	RequestRender();
}

//____ _adaptChildrenToWidth() ________________________________________________
// Adapts all non-hidden gizmos to our width and updates m_size.

void WgVBoxLayout::_adaptChildrenToWidth( int width )
{
	m_size.h = 0;
	m_size.w = width;
	WgVBoxHook * pHook = FirstHook();

	while( pHook )
	{
		if( !pHook->m_bHidden )
		{
			int height = pHook->Gizmo()->HeightForWidth( width );
			if( height == -1 )
				height = pHook->m_bestSize.h;

			pHook->Gizmo()->_onNewSize( WgSize(width,height) );
			pHook->m_height = height;
			m_size.h += height;
		}
		pHook = pHook->Next();
	}
}

//____ _refreshDefaultWidth() ______________________________________________________
// Updates m_bestSize.w and m_nBestWidth. Relies on m_bestSize of the visible
// hooks to have up-to-date data.

void WgVBoxLayout::_refreshDefaultWidth()
{
	m_bestSize.w = 0;
	m_nBestWidth = 0;
	WgVBoxHook * pHook = FirstHook();

	while( pHook )
	{
		if( !pHook->m_bHidden && pHook->m_bestSize.w >= m_bestSize.w )
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

void WgVBoxLayout::_refreshDefaultSize()
{
	m_bestSize.Clear();
	m_nBestWidth = 0;
	WgVBoxHook * pHook = FirstHook();

	while( pHook )
	{
		if( !pHook->m_bHidden )
		{
			pHook->m_bestSize = pHook->Gizmo()->DefaultSize();

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

//____ _refreshAllGizmos() ____________________________________________________

void  WgVBoxLayout::_refreshAllGizmos()
{
	_refreshDefaultSize();
	_adaptChildrenToWidth( m_size.w );
	RequestResize();
	RequestRender();
}

//____ _renderFromChildOnward() _______________________________________________

void WgVBoxLayout::_renderFromChildOnward( WgOrderedHook * pHook )
{
	WgRect geo = _hookGeo(pHook);
	geo.h = m_size.h - geo.y;
	RequestRender( geo );
}

//____ _newHook() _____________________________________________________________

WgOrderedHook *  WgVBoxLayout::_newHook()
{
	return new WgVBoxHook( this );
}

