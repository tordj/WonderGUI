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

#include <wg_gizmo_stack.h>

static const char	c_gizmoType[] = {"Stack"};


bool WgStackHook::Up()
{
}

bool WgStackHook::Down()
{
}

void WgStackHook::Top()
{
}

void WgStackHook::Bottom()
{
}

WgStackHook::WgStackHook( WgGizmoStack * pParent ): m_pParent(pParent)
{
}


WgGizmoContainer * WgStackHook::_parent() const
{
	return m_pParent;
}


//____ Constructor ____________________________________________________________

WgGizmoStack::WgGizmoStack()
{
}

//____ Destructor _____________________________________________________________

WgGizmoStack::~WgGizmoStack()
{
}

//____ Type() _________________________________________________________________

const char * WgGizmoStack::Type( void ) const
{
	return GetMyType();
}

//____ GetMyType() ____________________________________________________________

const char * WgGizmoStack::GetMyType()
{
	return c_gizmoType;
}

//____ HeightForWidth() _______________________________________________________

int WgGizmoStack::HeightForWidth( int width ) const
{
	int height = 0;

	WgStackHook * pHook = FirstHook();
	while( pHook )
	{
		int h = pHook->Gizmo()->HeightForWidth(width);
		if( h > height )
			height = h;
		pHook = pHook->Next();
	}

	return height;
}

//____ WidthForHeight() _______________________________________________________

int WgGizmoStack::WidthForHeight( int height ) const
{
	int width = 0;

	WgStackHook * pHook = FirstHook();
	while( pHook )
	{
		int w = pHook->Gizmo()->WidthForHeight(height);
		if( w > width )
			width = w;
		pHook = pHook->Next();
	}

	return width;
}


//____ DefaultSize() _____________________________________________________________

WgSize WgGizmoStack::DefaultSize() const
{
	return m_bestSize;
}

//____ _onNewSize() ___________________________________________________________

void WgGizmoStack::_onNewSize( const WgSize& size )
{
	m_size = size;
	_adaptChildrenToSize();
}


//____ _hookGeo() _____________________________________________________________

WgRect WgGizmoStack::_hookGeo( const WgOrderedHook * pHook )
{
	return WgRect(0,0,m_size);
}

//____ _advanceGeoToHook() ____________________________________________________

void WgGizmoStack::_advanceGeoToHook( WgRect& prevHookGeo, const WgOrderedHook * pHook )
{
	return;
}

//____ _onResizeRequested() ____________________________________________________

void WgGizmoStack::_onResizeRequested( WgOrderedHook * _pHook )
{
	WgSize	bestSize;

	WgStackHook * pHook = FirstHook();
	while( pHook )
	{
		WgSize sz = pHook->Gizmo()->DefaultSize();
		if( sz.w > bestSize.w )
			bestSize.w = sz.w;
		if( sz.h > bestSize.h )
			bestSize.h = sz.h;
		pHook = pHook->Next();
	}

	if( bestSize != m_bestSize )
	{
		m_bestSize = bestSize;
		RequestResize();
	}
}

void WgGizmoStack::_onRenderRequested( WgOrderedHook * pHook )
{
	if( !pHook->Hidden() )
		_onRenderRequested(pHook, WgRect(0,0,m_size));
}

void WgGizmoStack::_onRenderRequested( WgOrderedHook * pHook, const WgRect& rect )
{
/*	if( pHook->IsHidden() )
		return;

	// Put our geometry and in a dirtyrect-list

	WgRectChain rects;
	rects.Add( rect );

	// Remove portions of dirty rect that are covered by opaque upper siblings,
	// possibly filling list with many small dirty rects instead.

	WgOrderedHook * pCover = pHook->Next();
	while( pCover )
	{
		pCover->Gizmo()->_onMaskRects( rects, pCover->m_realGeo, WgRect(0,0,65536,65536 ) );

		pCover = pCover->Next();
	}

	// Make request render calls

	WgRectLink * pRect = rects.pRectList;
	while( pRect )
	{
		RequestRender( * pRect );
		pRect = pRect->pNext;
	}
*/
}

//____ _onGizmoAppeared() _____________________________________________________

void WgGizmoStack::_onGizmoAppeared( WgOrderedHook * pInserted )
{
	bool	bRequestResize = false;

	// Check if we need to resize to fit Gizmo in current width

	int height = pInserted->Gizmo()->HeightForWidth(m_size.w);
	if( height > m_size.h )
		bRequestResize = true;

	// Update bestSize

	WgSize best = pInserted->Gizmo()->DefaultSize();

	if( best.w > m_bestSize.w )
	{
		m_bestSize.w = best.w;
		bRequestResize = true;
	}
	if( best.h > m_bestSize.h )
	{
		m_bestSize.h = best.h;
		bRequestResize = true;
	}

	if( bRequestResize )
		RequestResize();

	// Adapt inserted Gizmo to our size

	pInserted->Gizmo()->_onNewSize(m_size);

	// Force a render.

	_onRenderRequested( pInserted );
}

//____ _onGizmoDisappeared() __________________________________________________

void WgGizmoStack::_onGizmoDisappeared( WgOrderedHook * pToBeRemoved )
{
	bool	bRequestResize = false;

	// Get dirty rectangles for all visible sections of pToBeRemoved.

	_onRenderRequested( pToBeRemoved );

	// Update m_bestSize, skiping pToBeRemoved

	WgSize	bestSize;
	WgStackHook * pHook = FirstHook();
	while( pHook )
	{
		if( pHook != pToBeRemoved )
		{
			WgSize sz = pHook->Gizmo()->DefaultSize();
			if( sz.w > bestSize.w )
				bestSize.w = sz.w;
			if( sz.h > bestSize.h )
				bestSize.h = sz.h;
		}
		pHook = pHook->Next();
	}

	if( bestSize != m_bestSize )
		bRequestResize = true;

	m_bestSize = bestSize;

	// Check if removal might affect height for current width

	int height = pToBeRemoved->Gizmo()->HeightForWidth(m_size.w);
	if( height >= m_size.h )
		bRequestResize = true;

	//

	if( bRequestResize )
		RequestResize();
}

//____ _onGizmosReordered() ___________________________________________________

void WgGizmoStack::_onGizmosReordered()
{
	RequestRender();
}

//____ _refreshAllGizmos() ____________________________________________________

void WgGizmoStack::_refreshAllGizmos()
{
	_refreshDefaultSize();
	_adaptChildrenToSize();
	RequestResize();
	RequestRender();
}

//____ _newHook() _____________________________________________________________

WgOrderedHook * WgGizmoStack::_newHook()
{
	return new WgStackHook(this);
}

//____ _refreshDefaultSize() _____________________________________________________

void WgGizmoStack::_refreshDefaultSize()
{
	WgSize	bestSize;

	WgStackHook * pHook = FirstHook();
	while( pHook )
	{
		WgSize sz = pHook->Gizmo()->DefaultSize();
		if( sz.w > bestSize.w )
			bestSize.w = sz.w;
		if( sz.h > bestSize.h )
			bestSize.h = sz.h;
		pHook = pHook->Next();
	}

	m_bestSize = bestSize;
}


//____ _adaptChildrenToSize() ___________________________________________________________

void WgGizmoStack::_adaptChildrenToSize()
{
	WgStackHook * pHook = FirstHook();
	while( pHook )
	{
		pHook->Gizmo()->_onNewSize( m_size );
		pHook = pHook->Next();
	}
}

