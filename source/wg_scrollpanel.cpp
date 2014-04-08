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

#include <wg_scrollpanel.h>
#include <wg_gfxdevice.h>
#include <wg_util.h>
#include <wg_patches.h>
#include <wg_event.h>
#include <wg_eventhandler.h>


const char WgScrollPanel::CLASSNAME[] = {"ScrollPanel"};
const char WgScrollHook::CLASSNAME[] = {"ScrollHook"};



//____ Constructor ____________________________________________________________

WgScrollPanel::WgScrollPanel()
{
	m_scrollbarTargets[0].m_bHorizontal = false;
	m_scrollbarTargets[0].m_pParent = this;

	m_scrollbarTargets[1].m_bHorizontal = true;
	m_scrollbarTargets[1].m_pParent = this;

	m_contentOrigo = WG_NORTHWEST;
	m_widthPolicy = WG_DEFAULT;
	m_heightPolicy = WG_DEFAULT;


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
	
	m_bOverlayScrollbars	= false;

	m_wheelForScrollX	= 2;
	m_wheelForScrollY	= 1;

	m_elements[WINDOW]._setParent(this);
	m_elements[XDRAG]._setParent(this);
	m_elements[YDRAG]._setParent(this);

	_updateElementGeo( WgSize(256,256) );
}

//____ ~WgScrollPanel() __________________________________________________

WgScrollPanel::~WgScrollPanel()
{
}

//____ IsInstanceOf() _________________________________________________________

bool WgScrollPanel::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgPanel::IsInstanceOf(pClassName);
}

//____ ClassName() ____________________________________________________________

const char * WgScrollPanel::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ Cast() _________________________________________________________________

WgScrollPanelPtr WgScrollPanel::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgScrollPanelPtr( static_cast<WgScrollPanel*>(pObject.GetRealPtr()) );

	return 0;
}


//____ StepUp() _______________________________________________________________

bool WgScrollPanel::StepUp()
{
	int ofs = m_viewPixOfs.y - m_stepSizeY;

	if( ofs < 0 )
		ofs = 0;

	return SetViewPixelOfsY( ofs );
}

//____ StepDown() _____________________________________________________________
bool WgScrollPanel::StepDown()
{
	return SetViewPixelOfsY( m_viewPixOfs.y + m_stepSizeY );
}

//____ StepLeft() _____________________________________________________________
bool WgScrollPanel::StepLeft()
{
	int ofs = m_viewPixOfs.x - m_stepSizeX;

	if( ofs < 0 )
		ofs = 0;

	return SetViewPixelOfsX( ofs );
}

//____ StepRight() ____________________________________________________________

bool WgScrollPanel::StepRight()
{
	return SetViewPixelOfsX( m_viewPixOfs.x + m_stepSizeX );
}


//____ JumpUp() _______________________________________________________________

bool WgScrollPanel::JumpUp()
{
	int ofs = m_viewPixOfs.y - (int)(ViewPixelLenY() * m_jumpSizeY);
	if( ofs < 0 )
		ofs = 0;

	return SetViewPixelOfsY( ofs );
}

//____ JumpDown() _____________________________________________________________

bool WgScrollPanel::JumpDown()
{
	return SetViewPixelOfsY( m_viewPixOfs.y + (int)(ViewPixelLenY() * m_jumpSizeY) );
}

//____ JumpLeft() _____________________________________________________________

bool WgScrollPanel::JumpLeft()
{
	int ofs = m_viewPixOfs.x - (int)(ViewPixelLenX() * m_jumpSizeX);
	if( ofs < 0 )
		ofs = 0;

	return SetViewPixelOfsX( ofs );
}

//____ JumpRight() ____________________________________________________________

bool WgScrollPanel::JumpRight()
{
	return SetViewPixelOfsX( m_viewPixOfs.x + (int)(ViewPixelLenX() * m_jumpSizeX) );
}

//____ _wheelRollX() ___________________________________________________________

bool WgScrollPanel::_wheelRollX(int distance)
{
	int ofs = m_viewPixOfs.x - (m_stepSizeX * distance * 3);

	if( ofs < 0 )
		ofs = 0;

	return SetViewPixelOfsX( ofs );
}

//____ _wheelRollY() ___________________________________________________________

bool WgScrollPanel::_wheelRollY(int distance)
{
	int ofs = m_viewPixOfs.y - (m_stepSizeY * distance * 3);

	if( ofs < 0 )
		ofs = 0;

	return SetViewPixelOfsY( ofs );
}

//____ SetJumpSizeX() _________________________________________________________

void WgScrollPanel::SetJumpSizeX( float viewFraction )
{
	if( viewFraction < 0.f )
		viewFraction = 0.f;
	if( viewFraction > 1.f )
		viewFraction = 1.f;

	m_jumpSizeX = viewFraction;
}

//____ SetJumpSizeY() _________________________________________________________

void WgScrollPanel::SetJumpSizeY( float viewFraction )
{
	if( viewFraction < 0.f )
		viewFraction = 0.f;
	if( viewFraction > 1.f )
		viewFraction = 1.f;

	m_jumpSizeY = viewFraction;
}

//____ IsHScrollbarVisible() ____________________________________________________

bool WgScrollPanel::IsHScrollbarVisible()
{
	return m_elements[XDRAG].m_bVisible;
}

//____ IsVScrollbarVisible() ____________________________________________________

bool WgScrollPanel::IsVScrollbarVisible()
{
	return m_elements[YDRAG].m_bVisible;
}

//____ SetOverlayScrollbars() ____________________________________________________

void WgScrollPanel::SetOverlayScrollbars( bool bOverlay )
{
	if( bOverlay != m_bOverlayScrollbars )
	{
		m_bOverlayScrollbars = bOverlay;
		_updateElementGeo( Size() );
	}
}

//____ ViewPixelLenX() ________________________________________________________

int WgScrollPanel::ViewPixelLenX()
{
	return m_elements[WINDOW].m_windowGeo.w;
}

//____ ViewPixelLenY() ________________________________________________________

int WgScrollPanel::ViewPixelLenY()
{
	return m_elements[WINDOW].m_windowGeo.h;
}

//____ ViewOfsX() _____________________________________________________________

float WgScrollPanel::ViewOfsX()
{
	int size = m_contentSize.w - ViewPixelLenX();
	if( size <= 0 )
		return 0.f;
	else
		return m_viewPixOfs.x / (float) size;

}

//____ ViewOfsY() _____________________________________________________________

float WgScrollPanel::ViewOfsY()
{
	int size = m_contentSize.h - ViewPixelLenY();
	if( size <= 0 )
		return 0.f;
	else
		return m_viewPixOfs.y / (float) size;
}

//____ ViewLenX() _____________________________________________________________

float WgScrollPanel::ViewLenX()
{
	if( m_contentSize.w == 0 )
		return 1.0f;

	float len = ViewPixelLenX()/(float)m_contentSize.w;

	if( len > 1.f )
		len = 1.f;

	return len;
}

//____ ViewLenY() _____________________________________________________________

float WgScrollPanel::ViewLenY()
{
	if( m_contentSize.h == 0 )
		return 1.0f;

	float len = ViewPixelLenY()/(float)m_contentSize.h;

	if( len > 1.f )
		len = 1.f;

	return len;
}

//____ SetViewPixelOfs() ______________________________________________________

bool WgScrollPanel::SetViewPixelOfs( int x, int y )
{
	if( x == m_viewPixOfs.x && y == m_viewPixOfs.y )
		return true;

	int		pixLenX = ViewPixelLenX();
	int		pixLenY = ViewPixelLenY();
	bool	bChangedX = false;
	bool	bChangedY = false;

	bool retVal = true;
    if( y < 0 )
    {
        y = 0;
        retVal = false;
    }
    if( y > m_contentSize.h - pixLenY )
	{
		y = m_contentSize.h - pixLenY;
		retVal = false;
	}

    if( x < 0 )
    {
        x = 0;
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
		m_scrollbarTargets[1]._updateScrollbar( ViewOfsX(), ViewLenX() );
	}

	if( bChangedY )
	{
		m_scrollbarTargets[0]._updateScrollbar( ViewOfsY(), ViewLenY() );
	}

	if( bChangedX || bChangedY )
	{
		m_elements[WINDOW].m_canvasGeo = _genContentCanvasGeo( m_elements[WINDOW].m_windowGeo, m_contentSize, m_contentOrigo, m_viewPixOfs );
		_requestRender( m_elements[WINDOW].m_windowGeo );
	}

	return retVal;
}

//____ SetViewPixelOfsX() _____________________________________________________

bool WgScrollPanel::SetViewPixelOfsX( int x )
{
	return SetViewPixelOfs(x, m_viewPixOfs.y);
}

//____ SetViewPixelOfsY() _____________________________________________________

bool WgScrollPanel::SetViewPixelOfsY( int y )
{
	return SetViewPixelOfs(m_viewPixOfs.x, y);
}

//____ SetViewOfs() ___________________________________________________________

bool WgScrollPanel::SetViewOfs( float x, float y )
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


	return SetViewPixelOfs( (int)((width*x)+0.5f), (int)((height*y)+0.5f) );
}

//____ SetViewOfsX() __________________________________________________________

bool WgScrollPanel::SetViewOfsX( float x )
{
	if( x < 0.f )
		x = 0;
	if( x > 1.f )
		x = 1.f;

	int	width = m_contentSize.w - ViewPixelLenX();
	if( width < 0 )
		width = 0;

	return SetViewPixelOfsX( (int)((width*x)+0.5f) );
}

//____ SetViewOfsY() __________________________________________________________

bool WgScrollPanel::SetViewOfsY( float y )
{
	if( y < 0.f )
		y = 0;
	if( y > 1.f )
		y = 1.f;

	int	height = m_contentSize.h - ViewPixelLenY();
	if( height < 0 )
		height = 0;

	return SetViewPixelOfsY( (int)((height*y)+0.5f) );
}

//____ SetSkin() ______________________________________________________________

void WgScrollPanel::SetSkin( const WgSkinPtr& pSkin )
{
	WgPanel::SetSkin(pSkin);
	m_elements[WINDOW].m_canvasGeo = _genContentCanvasGeo( m_elements[WINDOW].m_windowGeo, m_contentSize, m_contentOrigo, m_viewPixOfs );
}


//____ SetContent() ___________________________________________________________

WgScrollHookPtr WgScrollPanel::SetContent( const WgWidgetPtr& pContent )
{
	m_elements[WINDOW]._setWidget(pContent.GetRealPtr());

	_updateElementGeo( Size() );
	_requestRender( m_elements[WINDOW].m_windowGeo );		// If geometry is same as the old one, we need to request render ourselves.

	if( pContent )
		return &m_elements[WINDOW];
	else
		return 0;
}

//____ SetHScrollbar() ________________________________________________________

WgScrollHookPtr WgScrollPanel::SetHScrollbar( const WgHScrollbarPtr& pScrollbar )
{
	// Remove us as target target from current Scrollbar (if we have any)

	if( m_elements[XDRAG]._widget() )
		((WgScrollbar*)m_elements[XDRAG]._widget())->SetScrollbarTarget(0);

	//

	m_elements[XDRAG]._setWidget(pScrollbar.GetRealPtr());
	_updateElementGeo( Size() );
	_requestRender( m_elements[XDRAG].m_windowGeo );		// If geometry is same as the old one, we need to request render ourselves.

	if( pScrollbar )
	{
		pScrollbar->SetScrollbarTarget( &m_scrollbarTargets[1] );
		pScrollbar->SetHandle( ViewOfsX(), ViewLenX() );
		return &m_elements[YDRAG];
	}

	return 0;
}

//____ SetVScrollbar() ________________________________________________________

WgScrollHookPtr WgScrollPanel::SetVScrollbar( const WgVScrollbarPtr& pScrollbar )
{
	// Remove us as target from current Scrollbar (if we have any)

	if( m_elements[YDRAG]._widget() )
		((WgScrollbar*)m_elements[YDRAG]._widget())->SetScrollbarTarget(0);

	//

	m_elements[YDRAG]._setWidget(pScrollbar.GetRealPtr());
	_updateElementGeo( Size() );
	_requestRender( m_elements[YDRAG].m_windowGeo );		// If geometry is same as the old one, we need to request render ourselves.

	if( pScrollbar )
	{
		pScrollbar->SetScrollbarTarget( &m_scrollbarTargets[0] );
		pScrollbar->SetHandle( ViewOfsY(), ViewLenY() );
		return &m_elements[YDRAG];
	}

	return 0;
}

WgHScrollbarPtr WgScrollPanel::HScrollbar() const 
{
	return (WgHScrollbar*) m_elements[XDRAG]._widget(); 
}

WgVScrollbarPtr WgScrollPanel::VScrollbar() const 
{ 
	return (WgVScrollbar*) m_elements[YDRAG]._widget(); 
}

WgWidgetPtr WgScrollPanel::Content() const 
{ 
	return m_elements[WINDOW]._widget(); 
}



//____ RemoveWidget() __________________________________________________________

bool WgScrollPanel::RemoveWidget( const WgWidgetPtr& pWidget )
{
	if( !pWidget )
		return false;

	if( pWidget == m_elements[XDRAG]._widget() )
		SetHScrollbar(0);
	else if( pWidget == m_elements[YDRAG]._widget() )
		SetVScrollbar(0);
	else if( pWidget == m_elements[WINDOW]._widget() )
		SetContent(0);
	else
		return false;

	return true;
}

//____ Clear() ______________________________________________________

bool WgScrollPanel::Clear()
{
	if( !m_elements[XDRAG]._widget() && !m_elements[YDRAG]._widget() && !m_elements[WINDOW]._widget() )
		return false;

	if( m_elements[XDRAG]._widget() )
		SetHScrollbar(0);
	if( m_elements[YDRAG]._widget() )
		SetVScrollbar(0);
	if( m_elements[WINDOW]._widget() )
		SetContent(0);

	return true;
}

//____ SetScrollbarAutoHide() _________________________________________________

void WgScrollPanel::SetScrollbarAutoHide( bool bHideHScrollbar, bool bHideVScrollbar )
{
	if( bHideHScrollbar == m_bAutoHideScrollbarX && bHideVScrollbar == m_bAutoHideScrollbarY )
		return;

	bool	bWasVisibleX = IsHScrollbarVisible();
	bool	bWasVisibleY = IsVScrollbarVisible();

	m_bAutoHideScrollbarX = bHideHScrollbar;
	m_bAutoHideScrollbarY = bHideVScrollbar;

	// Force a refresh of our subclass if its geometry has been affected.

	if( IsHScrollbarVisible() != bWasVisibleX || IsVScrollbarVisible() != bWasVisibleY )
		_updateElementGeo( Size() );
}

//____ SetHandlePositions() ________________________________________________

void WgScrollPanel::SetHandlePositions( bool bBottom, bool bRight )
{
	if( bBottom == m_bScrollbarBottom && bRight == m_bScrollbarRight )
		return;

	m_bScrollbarBottom	= bBottom;
	m_bScrollbarRight	= bRight;

	_updateElementGeo( Size() );
}

//____ SetCornerSkin() ______________________________________________________

void WgScrollPanel::SetCornerSkin( const WgSkinPtr& pSkin )
{
	m_pCornerSkin = pSkin;
	_requestRender( m_cornerGeo );
}

//____ _findWidget() ____________________________________________________________

WgWidget * WgScrollPanel::_findWidget( const WgCoord& pos, WgSearchMode mode )
{
	// Check XDRAG

	WgScrollHook * p = &m_elements[XDRAG];
	if( p->IsVisible() && p->_widget() && p->m_windowGeo.Contains( pos ) )
	{
		if( mode != WG_SEARCH_MARKPOLICY || p->_widget()->MarkTest( pos - p->m_windowGeo.Pos() ) )
			return p->_widget();
	}

	// Check YDRAG

	p = &m_elements[YDRAG];
	if( p->IsVisible() && p->_widget() && p->m_windowGeo.Contains( pos ) )
	{
		if( mode != WG_SEARCH_MARKPOLICY || p->_widget()->MarkTest( pos - p->m_windowGeo.Pos() ) )
			return p->_widget();
	}

	// Check WINDOW

	p = &m_elements[WINDOW];

	if( p->m_windowGeo.Contains( pos ) )
	{
		if( p->IsVisible() && p->_widget() && p->m_windowGeo.Contains( pos ) )
		{
			if( p->_widget()->IsContainer() )
			{
				WgWidget * pFound = static_cast<WgContainer*>(p->_widget())->_findWidget( pos - p->m_canvasGeo.Pos(), mode );
				if( pFound )
					return pFound;
			}
			else if( mode != WG_SEARCH_MARKPOLICY || p->_widget()->MarkTest( pos - p->m_canvasGeo.Pos() ) )
				return p->_widget();
		}

		// Check window skin

		if( m_pSkin )
		{
			if( m_pSkin->MarkTest( pos - p->m_windowGeo.Pos() + m_viewPixOfs, m_contentSize, m_state, m_markOpacity ) )
				return this;
		}
	}

	// Check our little corner square and geometry

	if( mode == WG_SEARCH_GEOMETRY || MarkTest( pos ) )
		return this;

	//

	return 0;
}

//____ PreferredSize() ___________________________________________________________

WgSize WgScrollPanel::PreferredSize() const
{
	//TODO: Implement!!!

	return WgSize( 128,128 );
}

//____ SetContentOrigo() _________________________________________________

void  WgScrollPanel::SetContentOrigo( WgOrigo origo )
{
	m_contentOrigo = origo;

	WgScrollHook * p = &m_elements[WINDOW];
	WgRect geo = _genContentCanvasGeo( p->m_windowGeo, m_contentSize, m_contentOrigo, m_viewPixOfs );

	if( geo != p->m_canvasGeo )
	{
		p->m_canvasGeo = geo;
		_requestRender( p->m_windowGeo );
	}
}

//____ SetContentSizePolicy() __________________________________________________

void  WgScrollPanel::SetContentSizePolicy( WgSizePolicy widthPolicy, WgSizePolicy heightPolicy )
{
	if( widthPolicy != m_widthPolicy || heightPolicy != m_heightPolicy )
	{
		m_widthPolicy = widthPolicy;
		m_heightPolicy = heightPolicy;

		_updateElementGeo( Size() );
	}
}

//____ _calcContentSize() ______________________________________________________

WgSize WgScrollPanel::_calcContentSize( WgSize mySize )
{
	WgWidget * pContent = m_elements[WINDOW]._widget();

	if( !pContent )
		return WgSize(0,0);

	//

	if( !m_bAutoHideScrollbarX && m_elements[XDRAG]._widget() )
		mySize.h -= m_elements[XDRAG]._widget()->PreferredSize().h;

	if( !m_bAutoHideScrollbarY && m_elements[YDRAG]._widget() )
		mySize.w -= m_elements[YDRAG]._widget()->PreferredSize().w;

	//

	WgSize contentSize = m_elements[WINDOW]._sizeFromPolicy( mySize, m_widthPolicy, m_heightPolicy );

	if( contentSize.h > mySize.h && m_bAutoHideScrollbarY && m_elements[YDRAG]._widget() )
	{
		mySize.w -= m_elements[YDRAG]._widget()->PreferredSize().w;
		contentSize = m_elements[WINDOW]._sizeFromPolicy( mySize, m_widthPolicy, m_heightPolicy );

		if( contentSize.w > mySize.w && m_bAutoHideScrollbarX && m_elements[XDRAG]._widget() )
		{
			mySize.h -= m_elements[XDRAG]._widget()->PreferredSize().h;
			contentSize = m_elements[WINDOW]._sizeFromPolicy( mySize, m_widthPolicy, m_heightPolicy );
		}
	}
	else if( contentSize.w > mySize.w && m_bAutoHideScrollbarX && m_elements[XDRAG]._widget() )
	{
		mySize.h -= m_elements[XDRAG]._widget()->PreferredSize().h;
		contentSize = m_elements[WINDOW]._sizeFromPolicy( mySize, m_widthPolicy, m_heightPolicy );

		if( contentSize.h > mySize.h && m_bAutoHideScrollbarY && m_elements[YDRAG]._widget() )
		{
			mySize.w -= m_elements[YDRAG]._widget()->PreferredSize().w;
			contentSize = m_elements[WINDOW]._sizeFromPolicy( mySize, m_widthPolicy, m_heightPolicy );
		}
	}

	return contentSize;
}

//____ _updateElementGeo() _____________________________________________________

void WgScrollPanel::_updateElementGeo( WgSize mySize )
{

	WgSize newContentSize = _calcContentSize( mySize );

	WgRect	newDragX, newDragY, newWindow, newCorner;
	bool	bShowDragX = false, bShowDragY = false;

	// First get "default geometry" for them all, ignoring overlaps.

	newWindow = mySize;

	if( m_elements[XDRAG]._widget() )
	{
		newDragX.h = m_elements[XDRAG]._paddedPreferredSize().h;
		newDragX.w = newWindow.w;
		newDragX.x = 0;

		if( m_bScrollbarBottom )
			newDragX.y = newWindow.h - newDragX.h;
		else
			newDragX.y = 0;
	}

	if( m_elements[YDRAG]._widget() )
	{
		newDragY.w = m_elements[YDRAG]._paddedPreferredSize().w;
		newDragY.h = newWindow.h;
		newDragY.y = 0;

		if( m_bScrollbarRight )
			newDragY.x = newWindow.w - newDragY.w;
		else
			newDragY.x = 0;
	}

	// Determine which dragbars we need to show, using basic rules

	if( m_elements[XDRAG]._widget() && (newContentSize.w > newWindow.w || !m_bAutoHideScrollbarX) )
		bShowDragX = true;

	if( m_elements[YDRAG]._widget() && (newContentSize.h > newWindow.h || !m_bAutoHideScrollbarY) )
		bShowDragY = true;

	// See if showing one forces us to show the other

	if( bShowDragY && m_elements[XDRAG]._widget() && newContentSize.w > (newWindow.w - newDragY.w) )
		bShowDragX = true;

	if( bShowDragX && m_elements[YDRAG]._widget() && newContentSize.h > (newWindow.h - newDragY.h) )
		bShowDragY = true;

	// We need to adjust view layout to accomodate visible scrollbars. 

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

	// If both dragbars are visible we need to avoid overlap and include a corner to fill the empty square.

	if( bShowDragX && bShowDragY )
	{
		newDragX.x = newWindow.x;
		newDragX.w = newWindow.w;

		newDragY.y = newWindow.y;
		newDragY.h = newWindow.h;
	}

	// Display corner if both dragbars are visible and they are outside the view

	if( bShowDragX && bShowDragY && !m_bOverlayScrollbars )
	{
		m_cornerGeo.x = newDragY.x;
		m_cornerGeo.y = newDragX.y;
		m_cornerGeo.w = newDragY.w;
		m_cornerGeo.h = newDragX.h;
	}
	else
		m_cornerGeo.Clear();

	// Remove padding from dragbars now all geometry calculations have been done

	newDragX.Shrink( m_elements[XDRAG].m_padding );
	newDragY.Shrink( m_elements[YDRAG].m_padding );

	// Expand view area again if scrollbars are in overlay mode
	// The previous calculations were still needed for correct
	// scrollbar placement.

	if( m_bOverlayScrollbars )
		newWindow = mySize;


	// Determine changes to views position and size over content before
	// we overwrite old values, so we can emit right signals once all is updated.
	// Update m_viewPixOfs at the same time.

	bool	bNewOfsX = false;
	bool	bNewOfsY = false;
	bool	bNewWidth = false;
	bool	bNewHeight = false;

	if( newWindow.h != m_elements[WINDOW].m_windowGeo.h )
		bNewHeight = true;

	if( newWindow.w != m_elements[WINDOW].m_windowGeo.w )
		bNewWidth = true;


	if( m_bAutoScrollX )
	{
		if( m_viewPixOfs.x + m_elements[WINDOW].m_windowGeo.w >= m_contentSize.w && newWindow.w < newContentSize.w )
		{
			bNewOfsX = true;
			m_viewPixOfs.x = newContentSize.w - newWindow.w;
		}
	}

	if( m_bAutoScrollY )
	{
		if( m_viewPixOfs.y + m_elements[WINDOW].m_windowGeo.h >= m_contentSize.h && newWindow.h < newContentSize.h )
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
			bNewOfsY = true;
			m_viewPixOfs.y = y;
		}
	}

	// If something visible has changed we need to update element geometry and request render.
	// This is more optimized than it looks like...

	if( newWindow != m_elements[WINDOW].m_windowGeo || bShowDragX != m_elements[XDRAG].m_bVisible || bShowDragY != m_elements[YDRAG].m_bVisible || newContentSize != m_contentSize )
	{
		m_elements[WINDOW].m_windowGeo = newWindow;
		m_elements[XDRAG].m_windowGeo = newDragX;
		m_elements[YDRAG].m_windowGeo = newDragY;
		m_elements[WINDOW].m_canvasGeo = _genContentCanvasGeo( newWindow, newContentSize, m_contentOrigo, m_viewPixOfs );
		m_elements[XDRAG].m_canvasGeo = newDragX;
		m_elements[YDRAG].m_canvasGeo = newDragY;
		m_elements[XDRAG].m_bVisible = bShowDragX;
		m_elements[YDRAG].m_bVisible = bShowDragY;

		_requestRender();

		// Notify scrollbars of their new size.

		if( bShowDragX )
			m_elements[XDRAG]._widget()->_onNewSize(newDragX.Size());
		if( bShowDragY )
			m_elements[YDRAG]._widget()->_onNewSize(newDragY.Size());
	}

	// If contentSize has changed we save changes and set flags

	bool bNewContentWidth = false;
	bool bNewContentHeight = false;

	if( newContentSize.w != m_contentSize.w )
		bNewContentWidth = true;

	if( newContentSize.h != m_contentSize.h )
		bNewContentHeight = true;

	if( bNewContentWidth || bNewContentHeight )
	{
		m_contentSize = newContentSize;

		// Notify content of its new size.

		if( m_elements[WINDOW]._widget() )
			m_elements[WINDOW]._widget()->_onNewSize(newContentSize);
	}

	// Notify scrollbars of any change to content size, view size or view offset.

	if( bNewOfsX || bNewWidth || bNewContentWidth )
		m_scrollbarTargets[1]._updateScrollbar( ViewOfsX(), ViewLenX() );

	if( bNewOfsY || bNewHeight || bNewContentHeight )
		m_scrollbarTargets[0]._updateScrollbar( ViewOfsY(), ViewLenY() );

/*
	// Send signals if views size or position over content has changed
	// or contents size has changed.

	if( bNewOfsX || bNewContentWidth )
		Emit( ViewPosX(), ViewOfsX() );
	if( bNewOfsX )
		Emit( ViewPosPixelX(), m_viewPixOfs.x );

	if( bNewOfsY || bNewContentHeight )
		Emit( ViewPosY(), ViewOfsY() );
	if( bNewOfsY )
		Emit( ViewPosPixelY(), m_viewPixOfs.y );

	if( bNewWidth || bNewContentWidth )
		Emit( ViewSizeX(), ViewLenX() );
	if( bNewWidth )
		Emit( ViewSizePixelX(), ViewPixelLenX() );

	if( bNewHeight || bNewContentHeight )
		Emit( ViewSizeY(), ViewLenY() );
	if( bNewHeight )
		Emit( ViewSizePixelY(), ViewPixelLenY() );

	if( bNewOfsX || bNewWidth || bNewContentWidth )
		Emit( ViewPosSizeX(), ViewOfsX(), ViewLenX() );
	if( bNewOfsX || bNewWidth )
		Emit( ViewPosSizePixelX(), m_viewPixOfs.x, ViewPixelLenX() );

	if( bNewOfsY || bNewHeight || bNewContentHeight )
		Emit( ViewPosSizeY(), ViewOfsY(), ViewLenY() );
	if( bNewOfsY || bNewHeight )
		Emit( ViewPosSizePixelY(), m_viewPixOfs.y, ViewPixelLenY() );

	if( bNewOfsX || bNewOfsY || bNewContentHeight || bNewContentWidth )
		Emit( ViewPos(), ViewOfsX(), ViewOfsY() );
	if( bNewOfsX || bNewOfsY )
		Emit( ViewPosPixel(), m_viewPixOfs.x, m_viewPixOfs.y );

	if( bNewWidth || bNewHeight || bNewContentHeight || bNewContentWidth )
		Emit( ViewSize(), ViewLenX(), ViewLenY() );
	if( bNewWidth || bNewHeight )
		Emit( ViewSizePixel(), ViewPixelLenX(), ViewPixelLenY() );
*/
}

//____ _genContentCanvasGeo() __________________________________________________

WgRect WgScrollPanel::_genContentCanvasGeo( const WgRect& window, WgSize contentSize, WgOrigo origo, WgCoord viewOfs )
{
	WgRect	out( window.Pos() - viewOfs, contentSize);

	if( window.w > contentSize.w )
	{
		WgRect r = WgUtil::OrigoToRect( origo, WgSize(window.w,1), WgSize(contentSize.w,1) );
		out.x = window.x + r.x;
	}

	if( window.h > contentSize.h )
	{
		WgRect r = WgUtil::OrigoToRect( origo, WgSize(1,window.h), WgSize(1,contentSize.h) );
		out.y = window.y + r.y;
	}

	out.Shrink( m_elements[WINDOW].m_padding );
	return out;
}

//____ _onNewSize() ____________________________________________________________

void WgScrollPanel::_onNewSize( const WgSize& size )
{
	_updateElementGeo( size );
}

//____ _onEvent() ______________________________________________________________

void WgScrollPanel::_onEvent( const WgEventPtr& _pEvent, WgEventHandler * pHandler )
{
	switch( _pEvent->Type() )
	{
		case WG_EVENT_WHEEL_ROLL:
		{			
			WgWheelRollEventPtr pEvent = WgWheelRollEvent::Cast(_pEvent);

			if( m_elements[WINDOW].m_windowGeo.Contains(pEvent->PointerPos()) )
			{
				int wheel = pEvent->Wheel();

				if( wheel == m_wheelForScrollY )
					_wheelRollY( pEvent->Distance() );
				else if( wheel == m_wheelForScrollX )
					_wheelRollX( pEvent->Distance() );
			}

			pHandler->SwallowEvent(_pEvent);
		}
		break;

		default:
			break;
	}

	WgPanel::_onEvent(_pEvent,pHandler);
}

//____ _renderPatches() ________________________________________________________

void WgScrollPanel::_renderPatches( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, WgPatches * _pPatches )
{
	// We start by eliminating dirt outside our geometry

	WgPatches 	patches( _pPatches->Size() );								// TODO: Optimize by pre-allocating?

	for( const WgRect * pRect = _pPatches->Begin() ; pRect != _pPatches->End() ; pRect++ )
	{
		if( _window.IntersectsWith( *pRect ) )
			patches.Push( WgRect(*pRect,_window) );
	}

	//

	WgRect	dirtBounds = patches.Union();

	// Render Window background skin

	if( m_pSkin )
	{
		WgRect skinWindow = m_elements[WINDOW].m_windowGeo + _canvas.Pos();
		WgRect skinCanvas = WgRect( skinWindow.Pos() - m_viewPixOfs, m_contentSize );

		for( const WgRect * pRect = patches.Begin() ; pRect != patches.End() ; pRect++ )
		{
			WgRect clip(*pRect,skinWindow);
			m_pSkin->Render( pDevice, skinCanvas, m_state, clip );
		}
	}

	//

	if( m_elements[WINDOW]._widget() )
	{
		WgRect canvas = m_elements[WINDOW].m_canvasGeo + _canvas.Pos();
		WgRect window( canvas, m_elements[WINDOW].m_windowGeo + _canvas.Pos() );	// Use intersection in case canvas is smaller than window.

		if( window.IntersectsWith(dirtBounds) )
			m_elements[WINDOW]._widget()->_renderPatches( pDevice, canvas, window, &patches );
	}
	if( m_elements[XDRAG].m_bVisible )
	{
		WgRect canvas = m_elements[XDRAG].m_windowGeo + _canvas.Pos();
		if( canvas.IntersectsWith(dirtBounds) )
			m_elements[XDRAG]._widget()->_renderPatches( pDevice, canvas, canvas, &patches );
	}
	if( m_elements[YDRAG].m_bVisible )
	{
		WgRect canvas = m_elements[YDRAG].m_windowGeo + _canvas.Pos();
		if( canvas.IntersectsWith(dirtBounds) )
			m_elements[YDRAG]._widget()->_renderPatches( pDevice, canvas, canvas, &patches );
	}

	if( m_pCornerSkin && m_cornerGeo.w != 0 && m_cornerGeo.h != 0 )
	{
		WgRect canvas = m_cornerGeo + _canvas.Pos();

		for( const WgRect * pRect = patches.Begin() ; pRect != patches.End() ; pRect++ )
		{
			WgRect clip( canvas, *pRect );
			if( clip.w > 0 || clip.h > 0 )
				m_pCornerSkin->Render( pDevice, canvas, m_state, clip );
		}

	}
}


//____ _onCollectPatches() _______________________________________________________

void WgScrollPanel::_onCollectPatches( WgPatches& container, const WgRect& geo, const WgRect& clip )
{
	container.Add( WgRect(geo,clip) );
}

//____ _onMaskPatches() __________________________________________________________

void WgScrollPanel::_onMaskPatches( WgPatches& patches, const WgRect& geo, const WgRect& clip, WgBlendMode blendMode )
{
	//TODO: Don't just check IsOpaque() globally, check rect by rect.
	if( (m_bOpaque && blendMode == WG_BLENDMODE_BLEND) || blendMode == WG_BLENDMODE_OPAQUE ||
		(m_pSkin && m_pSkin->IsOpaque(m_state)) )
	{
		patches.Sub( WgRect(geo,clip) );
		return;
	}

	switch( m_maskOp )
	{
		case WG_MASKOP_RECURSE:
		{
			// Mask against view

			WgScrollHook * p = &m_elements[WINDOW];

			if( m_pSkin && m_pSkin->IsOpaque() )
				patches.Sub( WgRect( p->m_windowGeo + geo.Pos(), clip) );
			else if( p->_widget() )
				p->_widget()->_onMaskPatches( patches, p->m_canvasGeo + geo.Pos(), WgRect(p->m_windowGeo + geo.Pos(), clip), blendMode );

			// Mask against dragbars

			p = &m_elements[XDRAG];
			if( p->IsVisible() )
				p->_widget()->_onMaskPatches( patches, p->m_windowGeo + geo.Pos(), clip, blendMode );

			p = &m_elements[YDRAG];
			if( p->IsVisible() )
				p->_widget()->_onMaskPatches( patches, p->m_windowGeo + geo.Pos(), clip, blendMode );

			// Maska against corner piece

			if( !m_cornerGeo.IsEmpty() && m_pCornerSkin && m_pCornerSkin->IsOpaque() )
				patches.Sub( WgRect(m_cornerGeo + geo.Pos(), clip) );

			break;
		}
		case WG_MASKOP_SKIP:
			break;
		case WG_MASKOP_MASK:
			patches.Sub( WgRect(geo,clip) );
			break;
	}
}

//____ _onAlphaTest() ___________________________________________________________

bool WgScrollPanel::_onAlphaTest( const WgCoord& ofs )
{
	if( m_pSkin && m_elements[WINDOW].m_windowGeo.Contains( ofs ) )
	{
		return m_pSkin->MarkTest( ofs, m_elements[WINDOW].m_canvasGeo, m_state, m_markOpacity );
	}

	if( m_pCornerSkin && m_cornerGeo.Contains( ofs ) )
	{
		return m_pCornerSkin->MarkTest( ofs, m_cornerGeo, m_state, m_markOpacity );
	}

	return false;
}

//____ _onCloneContent() _______________________________________________________

void WgScrollPanel::_onCloneContent( const WgWidget * _pOrg )
{
	WgScrollPanel* pOrg = (WgScrollPanel*)_pOrg;

	WgPanel::_onCloneContent(pOrg);
	

	m_contentSize = pOrg->m_contentSize;

	m_stepSizeX = pOrg->m_stepSizeX;
	m_stepSizeY = pOrg->m_stepSizeY;
	m_jumpSizeX = pOrg->m_jumpSizeX;
	m_jumpSizeY = pOrg->m_jumpSizeY;

	m_viewPixOfs = pOrg->m_viewPixOfs;

	m_bAutoHideScrollbarX = pOrg->m_bAutoHideScrollbarX;
	m_bAutoHideScrollbarY = pOrg->m_bAutoHideScrollbarY;

	m_bScrollbarBottom = pOrg->m_bScrollbarBottom;
	m_bScrollbarRight = pOrg->m_bScrollbarRight;

	m_bAutoScrollX = pOrg->m_bAutoScrollX;
	m_bAutoScrollY = pOrg->m_bAutoScrollY;
	
	m_bOverlayScrollbars = pOrg->m_bOverlayScrollbars;

	m_widthPolicy = pOrg->m_widthPolicy;
	m_heightPolicy = pOrg->m_heightPolicy;
	m_contentOrigo = pOrg->m_contentOrigo;

	m_pCornerSkin = pOrg->m_pCornerSkin;
	m_cornerGeo = pOrg->m_cornerGeo;

	//

	m_elements[WINDOW].m_windowGeo = pOrg->m_elements[WINDOW].m_windowGeo;
	// Canvas geo for WINDOW should remain as it is since we don't have any content.
	
	//
	
	m_elements[XDRAG].m_windowGeo = pOrg->m_elements[XDRAG].m_windowGeo;
	m_elements[XDRAG].m_canvasGeo = pOrg->m_elements[XDRAG].m_canvasGeo;
	m_elements[XDRAG].m_bVisible = pOrg->m_elements[XDRAG].m_bVisible;
	if( pOrg->m_elements[XDRAG]._widget() )
	{
		WgHScrollbar * pScrollbar = (WgHScrollbar*) pOrg->m_elements[XDRAG]._widget()->_newOfMyType();
		pScrollbar->CloneContent( pOrg->m_elements[XDRAG]._widget() );
		pScrollbar->SetScrollbarTarget(&m_scrollbarTargets[1]);
		m_elements[XDRAG]._setWidget(pScrollbar);		
	}

	//
	
	m_elements[YDRAG].m_windowGeo = pOrg->m_elements[YDRAG].m_windowGeo;
	m_elements[YDRAG].m_canvasGeo = pOrg->m_elements[YDRAG].m_canvasGeo;
	m_elements[YDRAG].m_bVisible = pOrg->m_elements[YDRAG].m_bVisible;
	if( pOrg->m_elements[YDRAG]._widget() )
	{
		WgVScrollbar * pScrollbar = (WgVScrollbar*) pOrg->m_elements[YDRAG]._widget()->_newOfMyType();
		pScrollbar->CloneContent( pOrg->m_elements[YDRAG]._widget() );
		pScrollbar->SetScrollbarTarget(&m_scrollbarTargets[0]);
		m_elements[YDRAG]._setWidget(pScrollbar);
	}

	//
}

//______________________________________________________________________________
bool WgScrollPanel::SetScrollWheels( int wheelForX, int wheelForY )
{
	m_wheelForScrollX = wheelForX;
	m_wheelForScrollY = wheelForY;
	return true;
}

//_____________________________________________________________________________
bool WgScrollPanel::SetAutoscroll( bool bAutoX, bool bAutoY )
{
	m_bAutoScrollX = bAutoX;
	m_bAutoScrollY = bAutoY;
	return true;
}


//_____________________________________________________________________________
WgHook*	WgScrollPanel::_firstHook() const
{
	for( int i = 0 ; i < 3 ; i++ )
		if( m_elements[i]._widget() )
			return const_cast<WgScrollHook*>(&m_elements[i]);

	return 0;
}

//_____________________________________________________________________________
WgHook*	WgScrollPanel::_lastHook() const
{
	for( int i = 2 ; i >= 0 ; i++ )
		if( m_elements[i]._widget() )
			return const_cast<WgScrollHook*>(&m_elements[i]);

	return 0;
}


//_____________________________________________________________________________
WgHook * WgScrollPanel::_firstHookWithGeo( WgRect& geo ) const
{
	for( int i = 0 ; i < 3 ; i++ )
		if( m_elements[i]._widget() )
		{
			geo = m_elements[i].m_canvasGeo;
			return const_cast<WgScrollHook*>(&m_elements[i]);
		}

	return 0;
}

//_____________________________________________________________________________
WgHook * WgScrollPanel::_nextHookWithGeo( WgRect& geo, WgHook * pHook ) const
{
	const WgScrollHook * pLast = &m_elements[2];
	WgScrollHook * p = (WgScrollHook*) pHook;


	while( p != pLast )
	{
		p++;
		if( p->_widget() )
		{
			geo = p->m_canvasGeo;
			return const_cast<WgScrollHook*>(p);
		}
	}
	return 0;
}

//_____________________________________________________________________________
WgHook * WgScrollPanel::_lastHookWithGeo( WgRect& geo ) const
{
	for( int i = 2 ; i >= 0 ; i++ )
		if( m_elements[i]._widget() )
		{
			geo = m_elements[i].m_canvasGeo;
			return const_cast<WgScrollHook*>(&m_elements[i]);
		}

	return 0;
}

//_____________________________________________________________________________
WgHook * WgScrollPanel::_prevHookWithGeo( WgRect& geo, WgHook * pHook ) const
{
	const WgScrollHook * pFirst = &m_elements[0];
	WgScrollHook * p = (WgScrollHook*) pHook;


	while( p != pFirst )
	{
		p--;
		if( p->_widget() )
		{
			geo = p->m_canvasGeo;
			return const_cast<WgScrollHook*>(p);
		}
	}
	return 0;
}

//____ WgScrollHook::Destructor ___________________________________________________

WgScrollHook::~WgScrollHook()
{
}

//____ WgScrollHook::IsInstanceOf() __________________________________________

bool WgScrollHook::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgPanelHook::IsInstanceOf(pClassName);
}

//____ WgScrollHook::ClassName() _____________________________________________

const char * WgScrollHook::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ WgScrollHook::Cast() __________________________________________________

WgScrollHookPtr WgScrollHook::Cast( const WgHookPtr& pHook )
{
	if( pHook && pHook->IsInstanceOf(CLASSNAME) )
		return WgScrollHookPtr( static_cast<WgScrollHook*>(pHook.GetRealPtr()) );

	return 0;
}


//____ WgScrollHook::Pos() ________________________________________________________

WgCoord WgScrollHook::Pos() const
{
	return m_canvasGeo.Pos();
}

//____ WgScrollHook::Size() _______________________________________________________

WgSize WgScrollHook::Size() const
{
	return m_canvasGeo.Size();
}

//____ WgScrollHook::Geo() ________________________________________________________

WgRect WgScrollHook::Geo() const
{
	return m_canvasGeo;
}

//____ WgScrollHook::ScreenPos() __________________________________________________

WgCoord WgScrollHook::ScreenPos() const
{
	return m_pView->ScreenPos() + m_canvasGeo.Pos();
}

//____ WgScrollHook::ScreenGeo() __________________________________________________

WgRect WgScrollHook::ScreenGeo() const
{
	return m_canvasGeo + m_pView->ScreenPos();
}


//____ WgScrollHook::_requestRender() ______________________________________________

void WgScrollHook::_requestRender()
{
	if( m_bVisible )
		m_pView->_requestRender( m_windowGeo );
}

void WgScrollHook::_requestRender( const WgRect& rect )
{
	if( m_bVisible )
	{
		WgRect r( m_windowGeo, rect + m_canvasGeo.Pos() );

		if( !r.IsEmpty() )
			m_pView->_requestRender( r );
	}
}

//____ WgScrollHook::_requestResize() ______________________________________________

void WgScrollHook::_requestResize()
{
	//TODO: Figure out how this should work and implement.
}

//____ WgScrollHook::_prevHook() ___________________________________________________

WgHook * WgScrollHook::_prevHook() const
{
	const WgScrollHook * pFirst = &m_pView->m_elements[0];
	WgScrollHook * p 		= const_cast<WgScrollHook*>(this);

	while( p != pFirst )
	{
		p--;
		if( p->_widget() )
			return p;
	}
	return 0;
}

//____ WgScrollHook::_nextHook() ___________________________________________________

WgHook * WgScrollHook::_nextHook() const
{
	WgScrollHook * pLast = &m_pView->m_elements[2];
	WgScrollHook * p 		= const_cast<WgScrollHook*>(this);

	while( p != pLast )
	{
		p++;
		if( p->_widget() )
			return p;
	}
	return 0;
}

//____ WgScrollHook::_parent() ___________________________________________________

WgContainer * WgScrollHook::_parent() const
{
	return m_pView;
}

//____ ScrollbarTarget methods ____________________________________________________

float WgScrollPanel::ScrollbarTarget::_stepFwd()
{
	if( m_bHorizontal )
	{
		m_pParent->StepRight();
		return m_pParent->ViewOfsX();
	}
	else
	{
		m_pParent->StepDown();
		return m_pParent->ViewOfsY();
	}
}

float WgScrollPanel::ScrollbarTarget::_stepBwd()
{
	if( m_bHorizontal )
	{
		m_pParent->StepLeft();
		return m_pParent->ViewOfsX();
	}
	else
	{
		m_pParent->StepUp();
		return m_pParent->ViewOfsY();
	}
}

float WgScrollPanel::ScrollbarTarget::_jumpFwd()
{
	if( m_bHorizontal )
	{
		m_pParent->JumpRight();
		return m_pParent->ViewOfsX();
	}
	else
	{
		m_pParent->JumpDown();
		return m_pParent->ViewOfsY();
	}
}

float WgScrollPanel::ScrollbarTarget::_jumpBwd()
{
	if( m_bHorizontal )
	{
		m_pParent->JumpLeft();
		return m_pParent->ViewOfsX();
	}
	else
	{
		m_pParent->JumpUp();
		return m_pParent->ViewOfsY();
	}
}

float WgScrollPanel::ScrollbarTarget::_wheelRolled( int distance )
{
	if( m_bHorizontal )
	{
		m_pParent->_wheelRollX(distance);
		return m_pParent->ViewOfsX();
	}
	else
	{	
		m_pParent->_wheelRollY(distance);
		return m_pParent->ViewOfsY();
	}
}

float WgScrollPanel::ScrollbarTarget::_setPosition( float fraction )
{
	if( m_bHorizontal )
	{
		m_pParent->SetViewOfsX(fraction);
		return m_pParent->ViewOfsX();
	}
	else
	{
		m_pParent->SetViewOfsY(fraction);
		return m_pParent->ViewOfsY();
	}
}

WgWidget* WgScrollPanel::ScrollbarTarget::_getWidget()
{
	return m_pParent;
}

float WgScrollPanel::ScrollbarTarget::_getHandlePosition()
{
	if( m_bHorizontal )
		return m_pParent->ViewOfsX();
	else
		return m_pParent->ViewOfsY();
}

float WgScrollPanel::ScrollbarTarget::_getHandleSize()
{
	if( m_bHorizontal )
		return m_pParent->ViewLenX();
	else
		return m_pParent->ViewLenY();
}



