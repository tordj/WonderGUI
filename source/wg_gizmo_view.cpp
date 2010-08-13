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

#include <wg_gizmo_view.h>
#include <wg_gfxdevice.h>


using namespace WgSignal;

static const char	c_gizmoType[] = {"TordJ/View"};


//____ Constructor ____________________________________________________________

WgGizmoView::WgGizmoView()
{
	m_stepSizeX		= 1;
	m_stepSizeY		= 1;
	m_jumpSizeX		= 0.75f;
	m_jumpSizeY		= 0.75f;

	m_viewPixOfs.x	= 0;
	m_viewPixOfs.y	= 0;

	m_bAutoHideScrollbarX = false;
	m_bAutoHideScrollbarY = false;

	m_bScrollbarBottom	= true;
	m_bScrollbarRight	= true;

	m_bAutoScrollX		= false;
	m_bAutoScrollY		= false;

	new (&m_elementsCollection) ViewGizmoCollection(this);

	new (&m_elements[WINDOW]) ViewHook( (WgGizmo*)0, this, &m_elementsCollection );
	new (&m_elements[XDRAG]) ViewHook( (WgGizmoHDragbar*)0, this, &m_elementsCollection );
	new (&m_elements[YDRAG]) ViewHook( (WgGizmoVDragbar*)0, this, &m_elementsCollection );

	UpdateElementGeometry( WgSize(256,256), WgSize(0,0) );
}

//____ ~WgGizmoView() __________________________________________________

WgGizmoView::~WgGizmoView()
{
}

//____ Type() _________________________________________________________________

const char * WgGizmoView::Type() const
{
	return GetMyType();
}

const char * WgGizmoView::GetMyType( void )
{
	return c_gizmoType;
}


//____ StepUp() _______________________________________________________________

bool WgGizmoView::StepUp()
{
	Sint32 ofs = m_viewPixOfs.y - m_stepSizeY;

	if( ofs < 0 )
		ofs = 0;

	return SetViewPixelOfsY( ofs );
}

//____ StepDown() _____________________________________________________________
bool WgGizmoView::StepDown()
{ 
	return SetViewPixelOfsY( m_viewPixOfs.y + m_stepSizeY );
}

//____ StepLeft() _____________________________________________________________
bool WgGizmoView::StepLeft()
{ 
	Sint32 ofs = m_viewPixOfs.x - m_stepSizeX;

	if( ofs < 0 )
		ofs = 0;

	return SetViewPixelOfsX( ofs );
}

//____ StepRight() ____________________________________________________________

bool WgGizmoView::StepRight()
{ 
	return SetViewPixelOfsX( m_viewPixOfs.x + m_stepSizeX );
}


//____ JumpUp() _______________________________________________________________

bool WgGizmoView::JumpUp()
{
	Sint32 ofs = m_viewPixOfs.y - (Uint32)(ViewPixelLenY() * m_jumpSizeY);
	if( ofs < 0 )
		ofs = 0;

	return SetViewPixelOfsY( (Uint32) ofs );
}

//____ JumpDown() _____________________________________________________________

bool WgGizmoView::JumpDown()
{
	return SetViewPixelOfsY( (Uint32)(m_viewPixOfs.y + ViewPixelLenY() * m_jumpSizeY) );
}

//____ JumpLeft() _____________________________________________________________

bool WgGizmoView::JumpLeft()
{
	Sint32 ofs = m_viewPixOfs.x - (Uint32)(ViewPixelLenX() * m_jumpSizeX);
	if( ofs < 0 )
		ofs = 0;

	return SetViewPixelOfsX( (Uint32) ofs );
}

//____ JumpRight() ____________________________________________________________

bool WgGizmoView::JumpRight()
{
	return SetViewPixelOfsX( (Uint32)(m_viewPixOfs.x + ViewPixelLenX() * m_jumpSizeX) );
}

//____ WheelRollX() ___________________________________________________________

bool WgGizmoView::WheelRollX(int distance)
{
	int ofs = int(m_viewPixOfs.x) - int(m_stepSizeX) * distance * 3 / 120;

	if( ofs < 0 )
		ofs = 0;

	return SetViewPixelOfsX( ofs );
}

//____ WheelRollY() ___________________________________________________________

bool WgGizmoView::WheelRollY(int distance)
{
	int ofs = int(m_viewPixOfs.y) - int(m_stepSizeY) * distance * 3 / 120;

	if( ofs < 0 )
		ofs = 0;

	return SetViewPixelOfsY( ofs );
}

//____ SetJumpSizeX() _________________________________________________________

void WgGizmoView::SetJumpSizeX( float viewFraction )
{
	if( viewFraction < 0.f )
		viewFraction = 0.f;
	if( viewFraction > 1.f )
		viewFraction = 1.f;

	m_jumpSizeX = viewFraction;
}

//____ SetJumpSizeY() _________________________________________________________

void WgGizmoView::SetJumpSizeY( float viewFraction )
{
	if( viewFraction < 0.f )
		viewFraction = 0.f;
	if( viewFraction > 1.f )
		viewFraction = 1.f;

	m_jumpSizeY = viewFraction;
}

//____ ScrollbarXVisible() ____________________________________________________

bool WgGizmoView::ScrollbarXVisible()
{
	return m_elements[XDRAG].m_bShow;
}

//____ ScrollbarYVisible() ____________________________________________________

bool WgGizmoView::ScrollbarYVisible()
{
	return m_elements[YDRAG].m_bShow;
}


//____ ViewPixelLenX() ________________________________________________________

Uint32 WgGizmoView::ViewPixelLenX()
{
	return m_elements[WINDOW].m_geo.w;
}

//____ ViewPixelLenY() ________________________________________________________

Uint32 WgGizmoView::ViewPixelLenY()
{
	return m_elements[WINDOW].m_geo.h;
}

//____ ViewOfsX() _____________________________________________________________

float WgGizmoView::ViewOfsX()
{
	Uint32 size = m_contentSize.w - ViewPixelLenX();
	if( size <= 0 )
		return 0.f;
	else
		return m_viewPixOfs.x / (float) size;

}

//____ ViewOfsY() _____________________________________________________________

float WgGizmoView::ViewOfsY()
{
	Uint32 size = m_contentSize.h - ViewPixelLenY();
	if( size <= 0 )
		return 0.f;
	else
		return m_viewPixOfs.y / (float) size;
}

//____ ViewLenX() _____________________________________________________________

float WgGizmoView::ViewLenX()
{
	if( m_contentSize.w == 0 )
		return 1.0f;

	float len = ViewPixelLenX()/(float)m_contentSize.w;

	if( len > 1.f )
		len = 1.f;

	return len;
}

//____ ViewLenY() _____________________________________________________________

float WgGizmoView::ViewLenY()
{
	if( m_contentSize.h == 0 )
		return 1.0f;

	float len = ViewPixelLenY()/(float)m_contentSize.h;

	if( len > 1.f )
		len = 1.f;

	return len;
}

//____ SetViewPixelOfs() ______________________________________________________

bool WgGizmoView::SetViewPixelOfs( int x, int y )
{
	if( x == m_viewPixOfs.x && y == m_viewPixOfs.y )
		return true;

	int		pixLenX = ViewPixelLenX();
	int		pixLenY = ViewPixelLenY();
	bool	bChangedX = false;
	bool	bChangedY = false;

	bool retVal = true;
	if( y > m_contentSize.h - pixLenY )
	{
		y = m_contentSize.h - pixLenY;
		retVal = false;
	}

	if( x > m_contentSize.w - pixLenX )
	{
		x = m_contentSize.w - pixLenX;
		retVal = false;
	}

	if( x != m_viewPixOfs.x )
	{
		m_viewPixOfs.x = x;
		bChangedX = true;
	}

	if( y != m_viewPixOfs.y )
	{
		m_viewPixOfs.y = y;
		bChangedY = true;
	}

	float	ofsX = ViewOfsX();
	float	ofsY = ViewOfsY();

	if( bChangedX )
	{
		if( m_pHook )
		{
			m_pHook->GetEmitter()->Emit( ViewPosX(), ofsX );
			m_pHook->GetEmitter()->Emit( ViewPosPixelX(), m_viewPixOfs.x );

			m_pHook->GetEmitter()->Emit( ViewPosSizeX(), ofsX, ViewLenX() );
			m_pHook->GetEmitter()->Emit( ViewPosSizePixelX(), m_viewPixOfs.x, pixLenX );
		}
	}

	if( bChangedY )
	{
		if( m_pHook )
		{
			m_pHook->GetEmitter()->Emit( ViewPosY(), ofsY );
			m_pHook->GetEmitter()->Emit( ViewPosPixelY(), m_viewPixOfs.y );

			m_pHook->GetEmitter()->Emit( ViewPosSizeY(), ofsY, ViewLenY() );
			m_pHook->GetEmitter()->Emit( ViewPosSizePixelY(), m_viewPixOfs.y, pixLenY );
		}
	}

	if( bChangedX || bChangedY )
	{
		if( m_pHook )
		{
			m_pHook->GetEmitter()->Emit( ViewPos(), ofsX, ViewOfsY() );
			m_pHook->GetEmitter()->Emit( ViewPosPixel(), m_viewPixOfs.x, m_viewPixOfs.y );
		}
	}

	RequestRender();
	return retVal;
}

//____ SetViewPixelOfsX() _____________________________________________________

bool WgGizmoView::SetViewPixelOfsX( int x )
{
	if( x == m_viewPixOfs.x )
		return true;

	int pixLenX = ViewPixelLenX();

	if( pixLenX > (int)m_contentSize.w )
		return false;							// Can't scroll, content is smaller than view.

	bool retVal = true;
	if( x > m_contentSize.w - pixLenX )
	{
		x = m_contentSize.w - pixLenX;
		retVal = false;
	}

	m_viewPixOfs.x = x;

	float	ofsX = ViewOfsX();

	if( m_pHook )
	{
		m_pHook->GetEmitter()->Emit( ViewPosX(), ofsX );
		m_pHook->GetEmitter()->Emit( ViewPosPixelX(), m_viewPixOfs.x );

		m_pHook->GetEmitter()->Emit( ViewPos(), ofsX, ViewOfsY() );
		m_pHook->GetEmitter()->Emit( ViewPosPixel(), m_viewPixOfs.x, m_viewPixOfs.y );

		m_pHook->GetEmitter()->Emit( ViewPosSizeX(), ofsX, ViewLenX() );
		m_pHook->GetEmitter()->Emit( ViewPosSizePixelX(), m_viewPixOfs.x, pixLenX );
	}

	RequestRender();
	return retVal;
}

//____ SetViewPixelOfsY() _____________________________________________________

bool WgGizmoView::SetViewPixelOfsY( int y )
{

	if( y == m_viewPixOfs.y )
		return true;

	int pixLenY = ViewPixelLenY();

	if( pixLenY > (int)m_contentSize.h )
		return false;							// Can't scroll, content is smaller than view.

	bool retVal = true;
	if( y > m_contentSize.h - pixLenY )
	{
		y = m_contentSize.h - pixLenY;
		retVal = false;
	}

	m_viewPixOfs.y = y;

	float	ofsY = ViewOfsY();

	if( m_pHook )
	{
		m_pHook->GetEmitter()->Emit( ViewPosY(), ofsY );
		m_pHook->GetEmitter()->Emit( ViewPosPixelY(), m_viewPixOfs.y );

		m_pHook->GetEmitter()->Emit( ViewPos(), ViewOfsX(), ofsY );
		m_pHook->GetEmitter()->Emit( ViewPosPixel(), m_viewPixOfs.x, m_viewPixOfs.y );

		m_pHook->GetEmitter()->Emit( ViewPosSizeY(), ofsY, ViewLenY() );
		m_pHook->GetEmitter()->Emit( ViewPosSizePixelY(), m_viewPixOfs.y, pixLenY );
	}

	RequestRender();
	return retVal;
}

//____ SetViewOfs() ___________________________________________________________

bool WgGizmoView::SetViewOfs( float x, float y )
{
	if( x < 0.f )
		x = 0;
	if( x > 1.f )
		x = 1.f;

	if( y < 0.f )
		y = 0;
	if( y > 1.f )
		y = 1.f;

	int	width = m_contentSize.w - ViewPixelLenX();
	int	height = m_contentSize.h - ViewPixelLenY();

	if( width < 0 )
		width = 0;

	if( height < 0 )
		height = 0;


	return SetViewPixelOfs( (Uint32)((width*x)+0.5f), (Uint32)((height*y)+0.5f) );
}

//____ SetViewOfsX() __________________________________________________________

bool WgGizmoView::SetViewOfsX( float x )
{
	if( x < 0.f )
		x = 0;
	if( x > 1.f )
		x = 1.f;

	int	width = m_contentSize.w - ViewPixelLenX();
	if( width < 0 )
		width = 0;

	return SetViewPixelOfsX( (Uint32)((width*x)+0.5f) );
}

//____ SetViewOfsY() __________________________________________________________

bool WgGizmoView::SetViewOfsY( float y )
{
	if( y < 0.f )
		y = 0;
	if( y > 1.f )
		y = 1.f;

	int	height = m_contentSize.h - ViewPixelLenY();
	if( height < 0 )
		height = 0;

	return SetViewPixelOfsY( (Uint32)((height*y)+0.5f) );
}

//____ SetScrollbarX() ________________________________________________________

bool WgGizmoView::SetScrollbarX( WgGizmoHDragbar* pScrollbar )
{
	// Remove callbacks to current scrollbar (if we have any)

	if( m_elements[XDRAG].Gizmo() )
		m_pHook->GetEmitter()->RemoveCallbacks(m_elements[XDRAG].Gizmo());

	// Delete previous hook and gizmo by explicitly calling its destructor.
	// Placement new for a new hook holding this gizmo.

	m_elements[XDRAG].~ViewHook();
	new (&m_elements[XDRAG])ViewHook(pScrollbar, this, &m_elementsCollection);

	//

	if( pScrollbar )
	{
		m_elements[XDRAG].GetEmitter()->AddCallback( WgSignal::SliderPos(), WgGizmoView::cbSetViewOfsX, this );
		m_elements[XDRAG].GetEmitter()->AddCallback( WgSignal::PrevPage(), WgGizmoView::cbJumpLeft, this );
		m_elements[XDRAG].GetEmitter()->AddCallback( WgSignal::NextPage(), WgGizmoView::cbJumpRight, this );
		m_elements[XDRAG].GetEmitter()->AddCallback( WgSignal::Forward(), WgGizmoView::cbStepRight, this );
		m_elements[XDRAG].GetEmitter()->AddCallback( WgSignal::Back(), WgGizmoView::cbStepLeft, this );
		m_elements[XDRAG].GetEmitter()->AddCallback( WgSignal::WheelRoll(2), WgGizmoView::cbWheelRollX, this );

		m_pHook->GetEmitter()->AddCallback( WgSignal::ViewPosSizeX(), WgGizmoHDragbar::cbSetSlider, pScrollbar );
		m_pHook->GetEmitter()->AddCallback( WgSignal::WheelRoll(2), WgGizmoView::cbWheelRollX, this );
	}

	UpdateElementGeometry( Size(), m_contentSize );
	RequestRender( m_elements[XDRAG].m_geo );		// If geometry is same as the old one, we need to request render ourselves.
	return true;
}

//____ SetScrollbarY() ________________________________________________________

bool WgGizmoView::SetScrollbarY( WgGizmoVDragbar* pScrollbar )
{
	// Remove callbacks to current scrollbar (if we have any)

	if( m_elements[YDRAG].Gizmo() )
		m_pHook->GetEmitter()->RemoveCallbacks(m_elements[YDRAG].Gizmo());

	// Delete previous hook and gizmo by explicitly calling its destructor.
	// Placement new for a new hook holding this gizmo.

	m_elements[YDRAG].~ViewHook();
	new (&m_elements[YDRAG])ViewHook(pScrollbar, this, &m_elementsCollection);

	//

	if( pScrollbar )
	{
		m_elements[YDRAG].GetEmitter()->AddCallback( WgSignal::SliderPos(), WgGizmoView::cbSetViewOfsY, this );
		m_elements[YDRAG].GetEmitter()->AddCallback( WgSignal::PrevPage(), WgGizmoView::cbJumpUp, this );
		m_elements[YDRAG].GetEmitter()->AddCallback( WgSignal::NextPage(), WgGizmoView::cbJumpDown, this );
		m_elements[YDRAG].GetEmitter()->AddCallback( WgSignal::Forward(), WgGizmoView::cbStepDown, this );
		m_elements[YDRAG].GetEmitter()->AddCallback( WgSignal::Back(), WgGizmoView::cbStepUp, this );
		m_elements[YDRAG].GetEmitter()->AddCallback( WgSignal::WheelRoll(1), WgGizmoView::cbWheelRollY, this );

		m_pHook->GetEmitter()->AddCallback( WgSignal::ViewPosSizeY(), WgGizmoVDragbar::cbSetSlider, pScrollbar );
		m_pHook->GetEmitter()->AddCallback( WgSignal::WheelRoll(1), WgGizmoView::cbWheelRollY, this );
	}

	UpdateElementGeometry( Size(), m_contentSize );
	RequestRender( m_elements[XDRAG].m_geo );		// If geometry is same as the old one, we need to request render ourselves.
	return true;
}



//____ SetScrollbarAutoHide() _________________________________________________

void WgGizmoView::SetScrollbarAutoHide( bool bHideX, bool bHideY )
{
	if( bHideX == m_bAutoHideScrollbarX && bHideY == m_bAutoHideScrollbarY )
		return;

	bool	bWasVisibleX = ScrollbarXVisible();
	bool	bWasVisibleY = ScrollbarYVisible();

	m_bAutoHideScrollbarX = bHideX;
	m_bAutoHideScrollbarY = bHideY;

	// Force a refresh of our subclass if its geometry has been affected.

	if( ScrollbarXVisible() != bWasVisibleX || ScrollbarYVisible() != bWasVisibleY )
		UpdateElementGeometry( Size(), m_contentSize );
}

//____ SetScrollbarPositions() ________________________________________________

void WgGizmoView::SetScrollbarPositions( bool bBottom, bool bRight )
{
	if( bBottom == m_bScrollbarBottom && bRight == m_bScrollbarRight )
		return;

	m_bScrollbarBottom	= bBottom;
	m_bScrollbarRight	= bRight;

	UpdateElementGeometry( Size(), m_contentSize );
}

//____ SetFillerSource() ______________________________________________________

void WgGizmoView::SetFillerSource( const WgBlockSetPtr& pBlocks )
{
	m_pFillerBlocks = pBlocks;
	RequestRender( m_geoFiller );
}



//____ UpdateElementGeometry() ________________________________________________

void WgGizmoView::UpdateElementGeometry( const WgSize& mySize, const WgSize& newContentSize ) 
{
	WgRect	newDragX, newDragY, newWindow, newFiller;
	bool	bShowDragX = false, bShowDragY = false;

	// First get "default geometry" for them all, ignoring overlaps.

	newWindow = mySize;

	if( m_elements[XDRAG].Gizmo() )
	{
		newDragX.h = m_elements[XDRAG].Gizmo()->BestSize().h;
		newDragX.w = newWindow.w;
		newDragX.x = 0;

		if( m_bScrollbarBottom )
			newDragX.y = newWindow.h - newDragX.h;
		else
			newDragX.y = 0;
	}

	if( m_elements[YDRAG].Gizmo() )
	{
		newDragY.w = m_elements[YDRAG].Gizmo()->BestSize().w;
		newDragY.h = newWindow.h;
		newDragY.y = 0;

		if( m_bScrollbarRight )
			newDragY.x = newWindow.w - newDragY.w;
		else
			newDragY.x = 0;
	}

	// Determine which dragbars we need to show, using basic rules

	if( m_elements[XDRAG].Gizmo() && (newContentSize.w > newWindow.w || !m_bAutoHideScrollbarX) )
		bShowDragX = true;

	if( m_elements[YDRAG].Gizmo() && (newContentSize.h > newWindow.h || !m_bAutoHideScrollbarY) )
		bShowDragY = true;

	// See if showing one forces us to show the other

	if( bShowDragY && m_elements[XDRAG].Gizmo() && newContentSize.w > (newWindow.w - newDragY.w) )
		bShowDragX = true;

	if( bShowDragX && m_elements[YDRAG].Gizmo() && newContentSize.h > (newWindow.h - newDragY.h) )
		bShowDragY = true;


	// Adjust view layout to accomodate visible dragbars

	if( bShowDragY )
	{
		newWindow.w -= newDragY.w;
		if( !m_bScrollbarRight )
			newWindow.x += newDragY.w;
	}

	if( bShowDragX )
	{
		newWindow.h -= newDragX.h;
		if( !m_bScrollbarBottom )
			newWindow.y += newDragX.h;
	}

	// If both dragbars are visible we need to avoid overlap
	// and include a filler for the empty square.

	if( bShowDragX && bShowDragY )
	{
		newDragX.x = newWindow.x;
		newDragX.w = newWindow.w;

		newDragY.y = newWindow.y;
		newDragY.h = newWindow.h;

		newFiller.x = newDragY.x;
		newFiller.y = newDragX.y;
		newFiller.w = newDragY.w;
		newFiller.h = newDragX.h;
	}

	// Determine changes to views position and size over content before
	// we overwrite old values, so we can emit right signals once all is updated.
	// Update m_viewPixOfs at the same time.

	bool	bNewOfsX = false;
	bool	bNewOfsY = false;
	bool	bNewWidth = false;
	bool	bNewHeight = false;

	if( newWindow.h != m_elements[WINDOW].m_geo.h )
		bNewHeight = true;

	if( newWindow.w != m_elements[WINDOW].m_geo.w )
		bNewWidth = true;


	if( m_bAutoScrollX )
	{
		if( m_viewPixOfs.x + m_elements[WINDOW].m_geo.w >= m_contentSize.w && newWindow.w < newContentSize.w )
		{
			bNewOfsX = true;
			m_viewPixOfs.x = newContentSize.w - newWindow.w;
		}
	}

	if( m_bAutoScrollY )
	{
		if( m_viewPixOfs.y + m_elements[WINDOW].m_geo.h >= m_contentSize.h && newWindow.h < newContentSize.h )
		{
			bNewOfsY = true;
			m_viewPixOfs.y = newContentSize.h - newWindow.h;
		}
	}

	if( m_viewPixOfs.x + newWindow.w > newContentSize.w )
	{
		int x = newContentSize.w - newWindow.w;
		if( x < 0 )
			x = 0;
		if( x != m_viewPixOfs.x )
		{
			bNewOfsX = true;
			m_viewPixOfs.x = x;
		}
	}

	if( m_viewPixOfs.y + newWindow.h > newContentSize.h )
	{
		int y = newContentSize.h - newWindow.h;
		if( y < 0 )
			y = 0;
		if( y != m_viewPixOfs.y )
		{
			bNewOfsX = true;
			m_viewPixOfs.y = y;
		}
	}

	// If something visible has changed we need to update element geometry and request render.
	// This is more optimized than it looks like...

	if( newWindow != m_elements[WINDOW].m_geo || bShowDragX != m_elements[XDRAG].m_bShow || bShowDragY != m_elements[YDRAG].m_bShow )
	{
		m_elements[WINDOW].m_geo = newWindow;
		m_elements[XDRAG].m_geo = newDragX;
		m_elements[YDRAG].m_geo = newDragY;
		m_elements[XDRAG].m_bShow = bShowDragX;
		m_elements[YDRAG].m_bShow = bShowDragY;

		RequestRender();
	}

	// If contentSize has changed we save changes and set flags

	bool bNewContentWidth = false;
	bool bNewContentHeight = false;

	if( newContentSize.w != m_contentSize.w )
		bNewContentWidth = true;

	if( newContentSize.h != m_contentSize.h )
		bNewContentHeight = true;

	m_contentSize = newContentSize;


	// Send signals if views size or position over content has changed
	// or contents size has changed.

	if( m_pHook )
	{
		WgEmitter * pEmitter = m_pHook->GetEmitter();

		if( bNewOfsX || bNewContentWidth )
			pEmitter->Emit( ViewPosX(), ViewOfsX() );
		if( bNewOfsX )
			pEmitter->Emit( ViewPosPixelX(), m_viewPixOfs.x );

		if( bNewOfsY || bNewContentHeight )
			pEmitter->Emit( ViewPosY(), ViewOfsY() );
		if( bNewOfsY )
			pEmitter->Emit( ViewPosPixelY(), m_viewPixOfs.y );

		if( bNewWidth || bNewContentWidth )
			pEmitter->Emit( ViewSizeX(), ViewLenX() );
		if( bNewWidth )
			pEmitter->Emit( ViewSizePixelX(), ViewPixelLenX() );

		if( bNewHeight || bNewContentHeight )
			pEmitter->Emit( ViewSizeY(), ViewLenY() );
		if( bNewHeight )
			pEmitter->Emit( ViewSizePixelY(), ViewPixelLenY() );

		if( bNewOfsX || bNewWidth || bNewContentWidth )
			pEmitter->Emit( ViewPosSizeX(), ViewOfsX(), ViewLenX() );

		if( bNewOfsX || bNewWidth )
			pEmitter->Emit( ViewPosSizePixelX(), m_viewPixOfs.x, ViewPixelLenX() );

		if( bNewOfsY || bNewHeight || bNewContentHeight )
			pEmitter->Emit( ViewPosSizeY(), ViewOfsY(), ViewLenY() );
		if( bNewOfsY || bNewHeight )
			pEmitter->Emit( ViewPosSizePixelY(), m_viewPixOfs.y, ViewPixelLenY() );
		
		if( bNewOfsX || bNewOfsY || bNewContentHeight || bNewContentWidth )
			pEmitter->Emit( ViewPos(), ViewOfsX(), ViewOfsY() );
		if( bNewOfsX || bNewOfsY )
			pEmitter->Emit( ViewPosPixel(), m_viewPixOfs.x, m_viewPixOfs.y );

		if( bNewWidth || bNewHeight || bNewContentHeight || bNewContentWidth )
			pEmitter->Emit( ViewSize(), ViewLenX(), ViewLenY() );
		if( bNewWidth || bNewHeight )
			pEmitter->Emit( ViewSizePixel(), ViewPixelLenX(), ViewPixelLenY() );
	}

}


//____ OnNewSize() ____________________________________________________________

void WgGizmoView::OnNewSize( const WgSize& size )
{
	UpdateElementGeometry( size, m_contentSize );
}


//____ OnRender() _____________________________________________________________

void WgGizmoView::OnRender( WgGfxDevice * pDevice, const WgRect& _window, const WgRect& _clip, Uint8 _layer )
{
	if( m_elements[WINDOW].Gizmo() )
		m_elements[WINDOW].Gizmo()->OnRender( pDevice, m_elements[WINDOW].m_geo + _window.Pos(), _clip, _layer );

	if( m_elements[XDRAG].m_bShow )
		m_elements[XDRAG].Gizmo()->OnRender( pDevice, m_elements[XDRAG].m_geo + _window.Pos(), _clip, _layer );

	if( m_elements[YDRAG].m_bShow )
		m_elements[YDRAG].Gizmo()->OnRender( pDevice, m_elements[YDRAG].m_geo + _window.Pos(), _clip, _layer );

	WgMode mode = m_bEnabled?WG_MODE_NORMAL:WG_MODE_DISABLED;
	pDevice->BlitBlock( m_pFillerBlocks->GetBlock( mode, m_geoFiller ), m_geoFiller + _window.Pos() );
}



//____ OnCloneContent() _______________________________________________________

void WgGizmoView::OnCloneContent( const WgGizmo * _pOrg )
{
	WgGizmoView* pOrg = (WgGizmoView*)_pOrg;

	m_contentSize	= pOrg->m_contentSize;
	m_stepSizeX		= pOrg->m_stepSizeX;
	m_stepSizeY		= pOrg->m_stepSizeY;
	m_jumpSizeX		= pOrg->m_jumpSizeX;
	m_jumpSizeY		= pOrg->m_jumpSizeY;

	m_viewPixOfs	= pOrg->m_viewPixOfs;
					
/*
	m_pScrollbarX	= 0;
	m_pScrollbarY	= 0;

	//TODO: Needs to have real hooks and stuff, needs to have callbacks etc set.
	if( pOrg->m_pScrollbarX )
	{
		m_pScrollbarX = new WgGizmoHDragbar();
		m_pScrollbarX->CloneContent( pOrg->m_pScrollbarX );
	}

	if( pOrg->m_pScrollbarY )
	{
		m_pScrollbarY = new WgGizmoVDragbar();
		m_pScrollbarY->CloneContent( pOrg->m_pScrollbarY );
	}
*/
	m_bAutoHideScrollbarX	= pOrg->m_bAutoHideScrollbarX;
	m_bAutoHideScrollbarY	= pOrg->m_bAutoHideScrollbarY;

	m_bScrollbarBottom		= pOrg->m_bScrollbarBottom;
	m_bScrollbarRight		= pOrg->m_bScrollbarRight;

	m_bAutoScrollX			= pOrg->m_bAutoScrollX;
	m_bAutoScrollY			= pOrg->m_bAutoScrollY;
}

//_____________________________________________________________________________
void WgGizmoView::SetContentSize( const WgSize& size )
{
	UpdateElementGeometry( Size(), WgSize(size) );
}

//_____________________________________________________________________________
bool WgGizmoView::SetAutoscroll( bool bAutoX, bool bAutoY )
{
	m_bAutoScrollX = bAutoX;
	m_bAutoScrollY = bAutoY;
	return true;
}



//____ ViewHook::Constructors _________________________________________________

WgGizmoView::ViewHook::ViewHook( WgGizmoHDragbar * pHDragbar, WgGizmoView * pView, WgGizmoCollection * pCollection ) 
: WgGizmoHook( pHDragbar, pCollection ), m_type(WgGizmoView::XDRAG), m_pView(pView), m_bShow(false) {}
WgGizmoView::ViewHook::ViewHook( WgGizmoVDragbar * pHDragbar, WgGizmoView * pView, WgGizmoCollection * pCollection ) 
			: WgGizmoHook( pHDragbar, pCollection ), m_type(WgGizmoView::YDRAG), m_pView(pView), m_bShow(false) {}
WgGizmoView::ViewHook::ViewHook( WgGizmo * pContent, WgGizmoView * pView, WgGizmoCollection * pCollection ) 
			: WgGizmoHook( pContent, pCollection ), m_type(WgGizmoView::WINDOW), m_pView(pView), m_bShow(true) {}

//____ ViewHook::Destructor ___________________________________________________

WgGizmoView::ViewHook::~ViewHook()
{
	delete m_pGizmo;
}

//____ ViewHook::Pos() ________________________________________________________

WgCord WgGizmoView::ViewHook::Pos() const
{
	return m_geo.Pos();
}

//____ ViewHook::Size() _______________________________________________________

WgSize WgGizmoView::ViewHook::Size() const
{
	return m_geo.Size();
}

//____ ViewHook::Geo() ________________________________________________________

WgRect WgGizmoView::ViewHook::Geo() const
{
	return m_geo;
}

//____ ViewHook::ScreenPos() __________________________________________________

WgCord WgGizmoView::ViewHook::ScreenPos() const
{
	return m_pView->ScreenPos() + m_geo.Pos();
}

//____ ViewHook::ScreenGeo() __________________________________________________

WgRect WgGizmoView::ViewHook::ScreenGeo() const
{
	return m_geo + m_pView->ScreenPos();
}


//____ ViewHook::PrevHook() ___________________________________________________

WgGizmoHook * WgGizmoView::ViewHook::PrevHook() const
{
	if( m_type == 0 )
		return 0;
	else
		return &m_pView->m_elements[m_type-1];
}

//____ ViewHook::NextHook() ___________________________________________________

WgGizmoHook * WgGizmoView::ViewHook::NextHook() const
{
	if( m_type == 2 )
		return 0;
	else
		return &m_pView->m_elements[m_type+1];
}

//____ ViewHook::GetEmitter() _________________________________________________

WgEmitter* WgGizmoView::ViewHook::GetEmitter()
{
	return this;
}

//____ ViewHook::GetRoot() ____________________________________________________

WgWidget* WgGizmoView::ViewHook::GetRoot()
{
	//TODO: Figure out how this should work and implement.

	return false;
}


//____ ViewHook::RequestRender() ______________________________________________

void WgGizmoView::ViewHook::RequestRender()
{
	m_pView->RequestRender( m_geo );
}

void WgGizmoView::ViewHook::RequestRender( const WgRect& rect )
{
	WgRect r = rect;

	r += m_geo.Pos();
	r.Intersection( r, rect );
	m_pView->RequestRender( r );
}

//____ ViewHook::RequestResize() ______________________________________________

void WgGizmoView::ViewHook::RequestResize()
{
	//TODO: Figure out how this should work and implement.
}

//____ ViewHook::RequestFocus() _______________________________________________

bool WgGizmoView::ViewHook::RequestFocus()
{
	//TODO: Figure out how this should work and implement.

	return false;
}

//____ ViewHook::ReleaseFocus() _______________________________________________

bool WgGizmoView::ViewHook::ReleaseFocus()
{
	//TODO: Figure out how this should work and implement.

	return false;
}
