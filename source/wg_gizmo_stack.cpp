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

//____ MinSize() ______________________________________________________________

WgSize WgGizmoStack::MinSize() const
{
	WgSize minSize;

	WgStackHook * pHook = FirstHook();
	while( pHook )
	{
		WgSize sz = pHook->Gizmo()->MinSize();
		if( sz.w > minSize.w )
			minSize.w = sz.w;
		if( sz.h > minSize.h )
			minSize.h = sz.h;
		pHook = pHook->Next();
	}
	return minSize;
}

//____ BestSize() _____________________________________________________________

WgSize WgGizmoStack::BestSize() const
{
	return m_bestSize;
}

//____ MaxSize() ______________________________________________________________

WgSize WgGizmoStack::MaxSize() const
{
	WgSize maxSize(INT_MAX,INT_MAX);

	WgStackHook * pHook = FirstHook();
	while( pHook )
	{
		WgSize sz = pHook->Gizmo()->MaxSize();
		if( sz.w < maxSize.w )
			maxSize.w = sz.w;
		if( sz.h < maxSize.h )
			maxSize.h = sz.h;
		pHook = pHook->Next();
	}
	return maxSize;
}

//____ _onNewSize() ___________________________________________________________

void WgGizmoStack::_onNewSize( const WgSize& size )
{
	m_size = size;
	_adaptChildrenToSize();
}


void WgGizmoStack::_castDirtyRect( const WgRect& geo, const WgRect& clip, WgRectLink * pDirtIn, WgRectChain* pDirtOut )
{
}

void WgGizmoStack::_renderDirtyRects( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, Uint8 _layer )
{
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

void WgGizmoStack::_onResizeRequested( WgOrderedHook * pHook )
{
}

void WgGizmoStack::_onRenderRequested( WgOrderedHook * pHook )
{
	_onRenderRequested(pHook, WgRect(0,0,m_size));
}

void WgGizmoStack::_onRenderRequested( WgOrderedHook * pHook, const WgRect& rect )
{
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

	WgSize best = pInserted->Gizmo()->BestSize();

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
			WgSize sz = pHook->Gizmo()->BestSize();
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
	_refreshBestSize();
	_adaptChildrenToSize();
	RequestResize();
	RequestRender();
}

//____ _newHook() _____________________________________________________________

WgOrderedHook * WgGizmoStack::_newHook()
{
	return new WgStackHook(this);
}

//____ _refreshBestSize() _____________________________________________________

void WgGizmoStack::_refreshBestSize()
{
	WgSize	bestSize;

	WgStackHook * pHook = FirstHook();
	while( pHook )
	{
		WgSize sz = pHook->Gizmo()->BestSize();
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

