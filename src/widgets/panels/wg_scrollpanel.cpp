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
#include <wg_msg.h>
#include <wg_msgrouter.h>

namespace wg 
{
	
	
	const char ScrollPanel::CLASSNAME[] = {"ScrollPanel"};
	const char ScrollHook::CLASSNAME[] = {"ScrollHook"};
	
	
	
	//____ Constructor ____________________________________________________________
	
	ScrollPanel::ScrollPanel()
	{
		m_scrollbarTargets[0].m_bHorizontal = false;
		m_scrollbarTargets[0].m_pParent = this;
	
		m_scrollbarTargets[1].m_bHorizontal = true;
		m_scrollbarTargets[1].m_pParent = this;
	
		m_contentOrigo = Origo::NorthWest;
		m_widthPolicy = SizePolicy::Default;
		m_heightPolicy = SizePolicy::Default;
	
	
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
	
		m_wheelForScroll	= 1;
	
		m_elements[WINDOW]._setParent(this);
		m_elements[XDRAG]._setParent(this);
		m_elements[YDRAG]._setParent(this);
	
		_updateElementGeo( Size(256,256) );
	}
	
	//____ ~ScrollPanel() __________________________________________________
	
	ScrollPanel::~ScrollPanel()
	{
	}
	
	//____ isInstanceOf() _________________________________________________________
	
	bool ScrollPanel::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return Panel::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * ScrollPanel::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	ScrollPanel_p ScrollPanel::cast( const Object_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return ScrollPanel_p( static_cast<ScrollPanel*>(pObject.rawPtr()) );
	
		return 0;
	}
	
	
	//____ stepUp() _______________________________________________________________
	
	bool ScrollPanel::stepUp()
	{
		int ofs = m_viewPixOfs.y - m_stepSizeY;
	
		if( ofs < 0 )
			ofs = 0;
	
		return setViewPixelOfsY( ofs );
	}
	
	//____ stepDown() _____________________________________________________________
	bool ScrollPanel::stepDown()
	{
		return setViewPixelOfsY( m_viewPixOfs.y + m_stepSizeY );
	}
	
	//____ stepLeft() _____________________________________________________________
	bool ScrollPanel::stepLeft()
	{
		int ofs = m_viewPixOfs.x - m_stepSizeX;
	
		if( ofs < 0 )
			ofs = 0;
	
		return setViewPixelOfsX( ofs );
	}
	
	//____ stepRight() ____________________________________________________________
	
	bool ScrollPanel::stepRight()
	{
		return setViewPixelOfsX( m_viewPixOfs.x + m_stepSizeX );
	}
	
	
	//____ jumpUp() _______________________________________________________________
	
	bool ScrollPanel::jumpUp()
	{
		int ofs = m_viewPixOfs.y - (int)(_paddedViewPixelLenY() * m_jumpSizeY);
		if( ofs < 0 )
			ofs = 0;
	
		return setViewPixelOfsY( ofs );
	}
	
	//____ jumpDown() _____________________________________________________________
	
	bool ScrollPanel::jumpDown()
	{
		return setViewPixelOfsY( m_viewPixOfs.y + (int)(_paddedViewPixelLenY() * m_jumpSizeY) );
	}
	
	//____ jumpLeft() _____________________________________________________________
	
	bool ScrollPanel::jumpLeft()
	{
		int ofs = m_viewPixOfs.x - (int)(_paddedViewPixelLenX() * m_jumpSizeX);
		if( ofs < 0 )
			ofs = 0;
	
		return setViewPixelOfsX( ofs );
	}
	
	//____ jumpRight() ____________________________________________________________
	
	bool ScrollPanel::jumpRight()
	{
		return setViewPixelOfsX( m_viewPixOfs.x + (int)(_paddedViewPixelLenX() * m_jumpSizeX) );
	}
	
	//____ _wheelRollX() ___________________________________________________________
	
	bool ScrollPanel::_wheelRollX(int distance)
	{
		int ofs = m_viewPixOfs.x - (m_stepSizeX * distance * 3);
	
		if( ofs < 0 )
			ofs = 0;
	
		return setViewPixelOfsX( ofs );
	}
	
	//____ _wheelRollY() ___________________________________________________________
	
	bool ScrollPanel::_wheelRollY(int distance)
	{
		int ofs = m_viewPixOfs.y - (m_stepSizeY * distance * 3);
	
		if( ofs < 0 )
			ofs = 0;
	
		return setViewPixelOfsY( ofs );
	}
	
	//____ setJumpSizeX() _________________________________________________________
	
	void ScrollPanel::setJumpSizeX( float viewFraction )
	{
		if( viewFraction < 0.f )
			viewFraction = 0.f;
		if( viewFraction > 1.f )
			viewFraction = 1.f;
	
		m_jumpSizeX = viewFraction;
	}
	
	//____ setJumpSizeY() _________________________________________________________
	
	void ScrollPanel::setJumpSizeY( float viewFraction )
	{
		if( viewFraction < 0.f )
			viewFraction = 0.f;
		if( viewFraction > 1.f )
			viewFraction = 1.f;
	
		m_jumpSizeY = viewFraction;
	}
	
	//____ isHorizontalScrollbarVisible() ____________________________________________________
	
	bool ScrollPanel::isHorizontalScrollbarVisible()
	{
		return m_elements[XDRAG].m_bVisible;
	}
	
	//____ isVerticalScrollbarVisible() ____________________________________________________
	
	bool ScrollPanel::isVerticalScrollbarVisible()
	{
		return m_elements[YDRAG].m_bVisible;
	}
	
	//____ setOverlayScrollbars() ____________________________________________________
	
	void ScrollPanel::setOverlayScrollbars( bool bOverlay )
	{
		if( bOverlay != m_bOverlayScrollbars )
		{
			m_bOverlayScrollbars = bOverlay;
			_updateElementGeo( size() );
		}
	}
	
	//____ viewPixelLenX() ________________________________________________________
	
	int ScrollPanel::viewPixelLenX()
	{
		return m_elements[WINDOW].m_windowGeo.w;
	}
	
	//____ viewPixelLenY() ________________________________________________________
	
	int ScrollPanel::viewPixelLenY()
	{
		return m_elements[WINDOW].m_windowGeo.h;
	}
	
	//____ viewOfsX() _____________________________________________________________
	
	float ScrollPanel::viewOfsX()
	{
		int size = m_contentSize.w - viewPixelLenX();
		if( size <= 0 )
			return 0.f;
		else
			return m_viewPixOfs.x / (float) size;
	
	}
	
	//____ viewOfsY() _____________________________________________________________
	
	float ScrollPanel::viewOfsY()
	{
		int size = m_contentSize.h - viewPixelLenY();
		if( size <= 0 )
			return 0.f;
		else
			return m_viewPixOfs.y / (float) size;
	}
	
	//____ viewLenX() _____________________________________________________________
	
	float ScrollPanel::viewLenX()
	{
		if( m_contentSize.w == 0 )
			return 1.0f;
	
		float len = viewPixelLenX()/(float)m_contentSize.w;
	
		if( len > 1.f )
			len = 1.f;
	
		return len;
	}
	
	//____ viewLenY() _____________________________________________________________
	
	float ScrollPanel::viewLenY()
	{
		if( m_contentSize.h == 0 )
			return 1.0f;
	
		float len = viewPixelLenY()/(float)m_contentSize.h;
	
		if( len > 1.f )
			len = 1.f;
	
		return len;
	}
	
	//____ _paddedViewPixelLenX() _______________________________________________
	
	int ScrollPanel::_paddedViewPixelLenX()
	{
		Size	windowPadding = m_elements[WINDOW]._widget() ? m_elements[WINDOW]._widget()->_windowPadding() : Size(0,0);
		return m_elements[WINDOW].m_windowGeo.w - windowPadding.w;
	}
	
	//____ _paddedViewPixelLenY() ________________________________________________________
	
	int ScrollPanel::_paddedViewPixelLenY()
	{
		Size	windowPadding = m_elements[WINDOW]._widget() ? m_elements[WINDOW]._widget()->_windowPadding() : Size(0,0);
		return m_elements[WINDOW].m_windowGeo.h - windowPadding.h;
	}
	
	//____ _paddedViewLenX() _____________________________________________________________
	
	float ScrollPanel::_paddedViewLenX()
	{
		Size	windowPadding = m_elements[WINDOW]._widget() ? m_elements[WINDOW]._widget()->_windowPadding() : Size(0,0);
	
		if( m_contentSize.w - windowPadding.w <= 0 )
			return 1.0f;
	
		float len = (m_elements[WINDOW].m_windowGeo.w - windowPadding.w)/(float)(m_contentSize.w - windowPadding.w);
	
		if( len > 1.f )
			len = 1.f;
	
		return len;
	}
	
	//____ _paddedViewLenY() _____________________________________________________________
	
	float ScrollPanel::_paddedViewLenY()
	{
		Size	windowPadding = m_elements[WINDOW]._widget() ? m_elements[WINDOW]._widget()->_windowPadding() : Size(0,0);
	
		if( m_contentSize.h - windowPadding.h <= 0 )
			return 1.0f;
	
		float len = (m_elements[WINDOW].m_windowGeo.h - windowPadding.h)/(float)(m_contentSize.h - windowPadding.h);
	
		if( len > 1.f )
			len = 1.f;
	
		return len;
	}
	
	
	//____ setViewPixelOfs() ______________________________________________________
	
	bool ScrollPanel::setViewPixelOfs( int x, int y )
	{
		if( x == m_viewPixOfs.x && y == m_viewPixOfs.y )
			return true;
	
		int		pixLenX = viewPixelLenX();
		int		pixLenY = viewPixelLenY();
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
	
		if( bChangedX || bChangedY )
		{
			if( bChangedX )
				m_scrollbarTargets[1]._updateScrollbar( viewOfsX(), _paddedViewLenX() );
	
			if( bChangedY )
				m_scrollbarTargets[0]._updateScrollbar( viewOfsY(), _paddedViewLenY() );
	
			m_elements[WINDOW].m_canvasGeo = _genContentCanvasGeo( m_elements[WINDOW].m_windowGeo, m_contentSize, m_contentOrigo, m_viewPixOfs );
			_requestRender( m_elements[WINDOW].m_windowGeo );
		}
	
		return retVal;
	}
	
	//____ setViewPixelOfsX() _____________________________________________________
	
	bool ScrollPanel::setViewPixelOfsX( int x )
	{
		return setViewPixelOfs(x, m_viewPixOfs.y);
	}
	
	//____ setViewPixelOfsY() _____________________________________________________
	
	bool ScrollPanel::setViewPixelOfsY( int y )
	{
		return setViewPixelOfs(m_viewPixOfs.x, y);
	}
	
	//____ setViewOfs() ___________________________________________________________
	
	bool ScrollPanel::setViewOfs( float x, float y )
	{
		if( x < 0.f )
			x = 0;
		if( x > 1.f )
			x = 1.f;
	
		if( y < 0.f )
			y = 0;
		if( y > 1.f )
			y = 1.f;
	
		int	width = m_contentSize.w - viewPixelLenX();
		int	height = m_contentSize.h - viewPixelLenY();
	
		if( width < 0 )
			width = 0;
	
		if( height < 0 )
			height = 0;
	
	
		return setViewPixelOfs( (int)((width*x)+0.5f), (int)((height*y)+0.5f) );
	}
	
	//____ setViewOfsX() __________________________________________________________
	
	bool ScrollPanel::setViewOfsX( float x )
	{
		if( x < 0.f )
			x = 0;
		if( x > 1.f )
			x = 1.f;
	
		int	width = m_contentSize.w - viewPixelLenX();
		if( width < 0 )
			width = 0;
	
		return setViewPixelOfsX( (int)((width*x)+0.5f) );
	}
	
	//____ setViewOfsY() __________________________________________________________
	
	bool ScrollPanel::setViewOfsY( float y )
	{
		if( y < 0.f )
			y = 0;
		if( y > 1.f )
			y = 1.f;
	
		int	height = m_contentSize.h - viewPixelLenY();
		if( height < 0 )
			height = 0;
	
		return setViewPixelOfsY( (int)((height*y)+0.5f) );
	}
	
	//____ setSkin() ______________________________________________________________
	
	void ScrollPanel::setSkin( const Skin_p& pSkin )
	{
		Panel::setSkin(pSkin);
		m_elements[WINDOW].m_canvasGeo = _genContentCanvasGeo( m_elements[WINDOW].m_windowGeo, m_contentSize, m_contentOrigo, m_viewPixOfs );
	}
	
	
	//____ setContent() ___________________________________________________________
	
	ScrollHook_p ScrollPanel::setContent( const Widget_p& pContent )
	{
		m_elements[WINDOW]._setWidget(pContent.rawPtr());
	
		_updateElementGeo( size() );
		_requestRender( m_elements[WINDOW].m_windowGeo );		// If geometry is same as the old one, we need to request render ourselves.
	
		if( pContent )
			return &m_elements[WINDOW];
		else
			return 0;
	}
	
	//____ setHorizontalScrollbar() ________________________________________________________
	
	ScrollHook_p ScrollPanel::setHorizontalScrollbar( const Scrollbar_p& pScrollbar )
	{
		// Remove us as target target from current Scrollbar (if we have any)
	
		if( m_elements[XDRAG]._widget() )
			((Scrollbar*)m_elements[XDRAG]._widget())->setScrollbarTarget(0);
	
		// 
	
		pScrollbar->setOrientation( Orientation::Horizontal );
	
		m_elements[XDRAG]._setWidget(pScrollbar.rawPtr());
		_updateElementGeo( size() );
		_requestRender( m_elements[XDRAG].m_windowGeo );		// If geometry is same as the old one, we need to request render ourselves.
	
		if( pScrollbar )
		{
			pScrollbar->setScrollbarTarget( &m_scrollbarTargets[1] );
			pScrollbar->setHandle( viewOfsX(), viewLenX() );
			return &m_elements[YDRAG];
		}
	
		return 0;
	}
	
	//____ setVerticalScrollbar() ________________________________________________________
	
	ScrollHook_p ScrollPanel::setVerticalScrollbar( const Scrollbar_p& pScrollbar )
	{
		// Remove us as target from current Scrollbar (if we have any)
	
		if( m_elements[YDRAG]._widget() )
			((Scrollbar*)m_elements[YDRAG]._widget())->setScrollbarTarget(0);
	
		//
	
		pScrollbar->setOrientation( Orientation::Vertical );
	
		m_elements[YDRAG]._setWidget(pScrollbar.rawPtr());
		_updateElementGeo( size() );
		_requestRender( m_elements[YDRAG].m_windowGeo );		// If geometry is same as the old one, we need to request render ourselves.
	
		if( pScrollbar )
		{
			pScrollbar->setScrollbarTarget( &m_scrollbarTargets[0] );
			pScrollbar->setHandle( viewOfsY(), viewLenY() );
			return &m_elements[YDRAG];
		}
	
		return 0;
	}
	
	Scrollbar_p ScrollPanel::horizontalScrollbar() const 
	{
		return static_cast<Scrollbar*>(m_elements[XDRAG]._widget()); 
	}
	
	Scrollbar_p ScrollPanel::verticalScrollbar() const 
	{ 
		return static_cast<Scrollbar*>(m_elements[YDRAG]._widget()); 
	}
	
	Widget_p ScrollPanel::content() const 
	{ 
		return m_elements[WINDOW]._widget(); 
	}
	
	
	
	//____ removeWidget() __________________________________________________________
	
	bool ScrollPanel::removeWidget( const Widget_p& pWidget )
	{
		if( !pWidget )
			return false;
	
		if( pWidget == m_elements[XDRAG]._widget() )
			setHorizontalScrollbar(0);
		else if( pWidget == m_elements[YDRAG]._widget() )
			setVerticalScrollbar(0);
		else if( pWidget == m_elements[WINDOW]._widget() )
			setContent(0);
		else
			return false;
	
		return true;
	}
	
	//____ clear() ______________________________________________________
	
	bool ScrollPanel::clear()
	{
		if( !m_elements[XDRAG]._widget() && !m_elements[YDRAG]._widget() && !m_elements[WINDOW]._widget() )
			return false;
	
		if( m_elements[XDRAG]._widget() )
			setHorizontalScrollbar(0);
		if( m_elements[YDRAG]._widget() )
			setVerticalScrollbar(0);
		if( m_elements[WINDOW]._widget() )
			setContent(0);
	
		return true;
	}
	
	//____ setScrollbarAutoHide() _________________________________________________
	
	void ScrollPanel::setScrollbarAutoHide( bool bHideHScrollbar, bool bHideVScrollbar )
	{
		if( bHideHScrollbar == m_bAutoHideScrollbarX && bHideVScrollbar == m_bAutoHideScrollbarY )
			return;
	
		bool	bWasVisibleX = isHorizontalScrollbarVisible();
		bool	bWasVisibleY = isVerticalScrollbarVisible();
	
		m_bAutoHideScrollbarX = bHideHScrollbar;
		m_bAutoHideScrollbarY = bHideVScrollbar;
	
		// Force a refresh of our subclass if its geometry has been affected.
	
		if( isHorizontalScrollbarVisible() != bWasVisibleX || isVerticalScrollbarVisible() != bWasVisibleY )
			_updateElementGeo( size() );
	}
	
	//____ setHandlePositions() ________________________________________________
	
	void ScrollPanel::setHandlePositions( bool bBottom, bool bRight )
	{
		if( bBottom == m_bScrollbarBottom && bRight == m_bScrollbarRight )
			return;
	
		m_bScrollbarBottom	= bBottom;
		m_bScrollbarRight	= bRight;
	
		_updateElementGeo( size() );
	}
	
	//____ setCornerSkin() ______________________________________________________
	
	void ScrollPanel::setCornerSkin( const Skin_p& pSkin )
	{
		m_pCornerSkin = pSkin;
		_requestRender( m_cornerGeo );
	}
	
	//____ _findWidget() ____________________________________________________________
	
	Widget * ScrollPanel::_findWidget( const Coord& pos, SearchMode mode )
	{
		// Check XDRAG
	
		ScrollHook * p = &m_elements[XDRAG];
		if( p->isVisible() && p->_widget() && p->m_windowGeo.contains( pos ) )
		{
			if( mode != SearchMode::MarkPolicy || p->_widget()->markTest( pos - p->m_windowGeo.pos() ) )
				return p->_widget();
		}
	
		// Check YDRAG
	
		p = &m_elements[YDRAG];
		if( p->isVisible() && p->_widget() && p->m_windowGeo.contains( pos ) )
		{
			if( mode != SearchMode::MarkPolicy || p->_widget()->markTest( pos - p->m_windowGeo.pos() ) )
				return p->_widget();
		}
	
		// Check WINDOW
	
		p = &m_elements[WINDOW];
	
		if( p->m_windowGeo.contains( pos ) )
		{
			if( p->isVisible() && p->_widget() && p->m_windowGeo.contains( pos ) )
			{
				if( p->_widget()->isContainer() )
				{
					Widget * pFound = static_cast<Container*>(p->_widget())->_findWidget( pos - p->m_canvasGeo.pos(), mode );
					if( pFound )
						return pFound;
				}
				else if( mode != SearchMode::MarkPolicy || p->_widget()->markTest( pos - p->m_canvasGeo.pos() ) )
					return p->_widget();
			}
	
			// Check window skin
	
			if( m_pSkin )
			{
				if( m_pSkin->markTest( pos - p->m_windowGeo.pos() + m_viewPixOfs, m_contentSize, m_state, m_markOpacity ) )
					return this;
			}
		}
	
		// Check our little corner square and geometry
	
		if( mode == SearchMode::Geometry || markTest( pos ) )
			return this;
	
		//
	
		return 0;
	}
	
	//____ preferredSize() ___________________________________________________________
	
	Size ScrollPanel::preferredSize() const
	{
		Size sz;
	
		if( m_elements[WINDOW]._widget() )
			sz = m_elements[WINDOW]._paddedPreferredSize();
	
		if( m_elements[XDRAG]._widget() && !m_bAutoHideScrollbarX )
		{
			Size scrollbar = m_elements[XDRAG]._paddedPreferredSize();
			sz.h += scrollbar.h;
			if( scrollbar.w > sz.w )
				sz.w = scrollbar.w;
		}
	
		if( m_elements[YDRAG]._widget() && !m_bAutoHideScrollbarY )
		{
			Size scrollbar = m_elements[YDRAG]._paddedPreferredSize();
			sz.w += scrollbar.w;
			if( scrollbar.h > sz.h )
				sz.h = scrollbar.h;
		}
	
		return sz;
	}
	
	//____ setContentOrigo() _________________________________________________
	
	void  ScrollPanel::setContentOrigo( Origo origo )
	{
		m_contentOrigo = origo;
	
		ScrollHook * p = &m_elements[WINDOW];
		Rect geo = _genContentCanvasGeo( p->m_windowGeo, m_contentSize, m_contentOrigo, m_viewPixOfs );
	
		if( geo != p->m_canvasGeo )
		{
			p->m_canvasGeo = geo;
			_requestRender( p->m_windowGeo );
		}
	}
	
	//____ setContentSizePolicy() __________________________________________________
	
	void  ScrollPanel::setContentSizePolicy( SizePolicy widthPolicy, SizePolicy heightPolicy )
	{
		if( widthPolicy != m_widthPolicy || heightPolicy != m_heightPolicy )
		{
			m_widthPolicy = widthPolicy;
			m_heightPolicy = heightPolicy;
	
			_updateElementGeo( size() );
		}
	}
	
	//____ _calcContentSize() ______________________________________________________
	
	Size ScrollPanel::_calcContentSize( Size mySize )
	{
		Widget * pContent = m_elements[WINDOW]._widget();
	
		if( !pContent )
			return Size(0,0);
	
		//
	
		if( !m_bAutoHideScrollbarX && m_elements[XDRAG]._widget() )
			mySize.h -= m_elements[XDRAG]._widget()->preferredSize().h;
	
		if( !m_bAutoHideScrollbarY && m_elements[YDRAG]._widget() )
			mySize.w -= m_elements[YDRAG]._widget()->preferredSize().w;
	
		//
	
		Size contentSize = m_elements[WINDOW]._sizeFromPolicy( mySize, m_widthPolicy, m_heightPolicy );
	
		if( contentSize.h > mySize.h && m_bAutoHideScrollbarY && m_elements[YDRAG]._widget() )
		{
			mySize.w -= m_elements[YDRAG]._widget()->preferredSize().w;
			contentSize = m_elements[WINDOW]._sizeFromPolicy( mySize, m_widthPolicy, m_heightPolicy );
	
			if( contentSize.w > mySize.w && m_bAutoHideScrollbarX && m_elements[XDRAG]._widget() )
			{
				mySize.h -= m_elements[XDRAG]._widget()->preferredSize().h;
				contentSize = m_elements[WINDOW]._sizeFromPolicy( mySize, m_widthPolicy, m_heightPolicy );
			}
		}
		else if( contentSize.w > mySize.w && m_bAutoHideScrollbarX && m_elements[XDRAG]._widget() )
		{
			mySize.h -= m_elements[XDRAG]._widget()->preferredSize().h;
			contentSize = m_elements[WINDOW]._sizeFromPolicy( mySize, m_widthPolicy, m_heightPolicy );
	
			if( contentSize.h > mySize.h && m_bAutoHideScrollbarY && m_elements[YDRAG]._widget() )
			{
				mySize.w -= m_elements[YDRAG]._widget()->preferredSize().w;
				contentSize = m_elements[WINDOW]._sizeFromPolicy( mySize, m_widthPolicy, m_heightPolicy );
			}
		}
	
		return contentSize;
	}
	
	//____ _updateElementGeo() _____________________________________________________
	
	void ScrollPanel::_updateElementGeo( Size mySize )
	{
	
		Size newContentSize = _calcContentSize( mySize );
	
		Rect	newDragX, newDragY, newWindow, newCorner;
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
	
		if( bShowDragX && m_elements[YDRAG]._widget() && newContentSize.h > (newWindow.h - newDragX.h) )
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
			m_cornerGeo.clear();
	
		// Remove padding from dragbars now all geometry calculations have been done
	
		newDragX.shrink( m_elements[XDRAG].m_padding );
		newDragY.shrink( m_elements[YDRAG].m_padding );
	
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
				m_elements[XDRAG]._widget()->_setSize(newDragX.size());
			if( bShowDragY )
				m_elements[YDRAG]._widget()->_setSize(newDragY.size());
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
				m_elements[WINDOW]._widget()->_setSize(newContentSize);
		}
	
		// Notify scrollbars of any change to content size, view size or view offset.
	
		if( bNewOfsX || bNewWidth || bNewContentWidth )
			m_scrollbarTargets[1]._updateScrollbar( viewOfsX(), _paddedViewLenX() );
	
		if( bNewOfsY || bNewHeight || bNewContentHeight )
			m_scrollbarTargets[0]._updateScrollbar( viewOfsY(), _paddedViewLenY() );
	}
	
	//____ _genContentCanvasGeo() __________________________________________________
	
	Rect ScrollPanel::_genContentCanvasGeo( const Rect& window, Size contentSize, Origo origo, Coord viewOfs )
	{
		Rect	out( window.pos() - viewOfs, contentSize);
	
		if( window.w > contentSize.w )
		{
			Rect r = Util::origoToRect( origo, Size(window.w,1), Size(contentSize.w,1) );
			out.x = window.x + r.x;
		}
	
		if( window.h > contentSize.h )
		{
			Rect r = Util::origoToRect( origo, Size(1,window.h), Size(1,contentSize.h) );
			out.y = window.y + r.y;
		}
	
		out.shrink( m_elements[WINDOW].m_padding );
		return out;
	}
	
	//____ _onRequestResize() _____________________________________________________
	
	void ScrollPanel::_onRequestResize( ScrollHook * pHook )
	{
		_updateElementGeo( size() );
	
	
	}
	
	
	//____ _setSize() ____________________________________________________________
	
	void ScrollPanel::_setSize( const Size& size )
	{
		Panel::_setSize(size);

		_updateElementGeo( size );
	}
	
	//____ _receive() ______________________________________________________________
	
	void ScrollPanel::_receive( const Msg_p& _pMsg )
	{
		Panel::_receive(_pMsg);
	
		switch( _pMsg->type() )
		{
			case MsgType::WheelRoll:
			{			
				WheelRollMsg_p pMsg = WheelRollMsg::cast(_pMsg);

				if( pMsg->wheel() == m_wheelForScroll )
				{
					if( m_elements[WINDOW].m_windowGeo.contains( toLocal(pMsg->pointerPos())) )
					{
						Coord dist = pMsg->distance();
						if( dist.x != 0 )
							_wheelRollX( dist.x );
						if( dist.y != 0 )
							_wheelRollY( dist.y );
					}		
					_pMsg->swallow();
				}
			}
			break;
	
			default:
				break;
		}
	}
	
	//____ _renderPatches() ________________________________________________________
	
	void ScrollPanel::_renderPatches( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, Patches * _pPatches )
	{
		// We start by eliminating dirt outside our geometry
	
		Patches 	patches( _pPatches->size() );								// TODO: Optimize by pre-allocating?
	
		for( const Rect * pRect = _pPatches->begin() ; pRect != _pPatches->end() ; pRect++ )
		{
			if( _window.intersectsWith( *pRect ) )
				patches.push( Rect(*pRect,_window) );
		}
	
		//
	
		Rect	dirtBounds = patches.getUnion();
	
		// Render Window background skin
	
		if( m_pSkin )
		{
			Rect skinWindow = m_elements[WINDOW].m_windowGeo + _canvas.pos();
	
			Size skinSize = Size::max(m_contentSize, m_elements[WINDOW].m_windowGeo);
			Rect skinCanvas = Rect( skinWindow.pos() - m_viewPixOfs, skinSize );
	
			for( const Rect * pRect = patches.begin() ; pRect != patches.end() ; pRect++ )
			{
				Rect clip(*pRect,skinWindow);
				m_pSkin->render( pDevice, skinCanvas, m_state, clip );
			}
		}
	
		//
	
		if( m_elements[WINDOW]._widget() )
		{
			Rect canvas = m_elements[WINDOW].m_canvasGeo + _canvas.pos();
			Rect window( canvas, m_elements[WINDOW].m_windowGeo + _canvas.pos() );	// Use intersection in case canvas is smaller than window.
	
			if( window.intersectsWith(dirtBounds) )
				m_elements[WINDOW]._widget()->_renderPatches( pDevice, canvas, window, &patches );
		}
		if( m_elements[XDRAG].m_bVisible )
		{
			Rect canvas = m_elements[XDRAG].m_windowGeo + _canvas.pos();
			if( canvas.intersectsWith(dirtBounds) )
				m_elements[XDRAG]._widget()->_renderPatches( pDevice, canvas, canvas, &patches );
		}
		if( m_elements[YDRAG].m_bVisible )
		{
			Rect canvas = m_elements[YDRAG].m_windowGeo + _canvas.pos();
			if( canvas.intersectsWith(dirtBounds) )
				m_elements[YDRAG]._widget()->_renderPatches( pDevice, canvas, canvas, &patches );
		}
	
		if( m_pCornerSkin && m_cornerGeo.w != 0 && m_cornerGeo.h != 0 )
		{
			Rect canvas = m_cornerGeo + _canvas.pos();
	
			for( const Rect * pRect = patches.begin() ; pRect != patches.end() ; pRect++ )
			{
				Rect clip( canvas, *pRect );
				if( clip.w > 0 || clip.h > 0 )
					m_pCornerSkin->render( pDevice, canvas, m_state, clip );
			}
	
		}
	}
	
	
	//____ _collectPatches() _______________________________________________________
	
	void ScrollPanel::_collectPatches( Patches& container, const Rect& geo, const Rect& clip )
	{
		container.add( Rect(geo,clip) );
	}
	
	//____ _maskPatches() __________________________________________________________
	
	void ScrollPanel::_maskPatches( Patches& patches, const Rect& geo, const Rect& clip, BlendMode blendMode )
	{
		//TODO: Don't just check isOpaque() globally, check rect by rect.
		if( (m_bOpaque && blendMode == BlendMode::Blend) || blendMode == BlendMode::Opaque ||
			(m_pSkin && m_pSkin->isOpaque(m_state)) )
		{
			patches.sub( Rect(geo,clip) );
			return;
		}
	
		switch( m_maskOp )
		{
			case MaskOp::Recurse:
			{
				// Mask against view
	
				ScrollHook * p = &m_elements[WINDOW];
	
				if( m_pSkin && m_pSkin->isOpaque() )
					patches.sub( Rect( p->m_windowGeo + geo.pos(), clip) );
				else if( p->_widget() )
					p->_widget()->_maskPatches( patches, p->m_canvasGeo + geo.pos(), Rect(p->m_windowGeo + geo.pos(), clip), blendMode );
	
				// Mask against dragbars
	
				p = &m_elements[XDRAG];
				if( p->isVisible() )
					p->_widget()->_maskPatches( patches, p->m_windowGeo + geo.pos(), clip, blendMode );
	
				p = &m_elements[YDRAG];
				if( p->isVisible() )
					p->_widget()->_maskPatches( patches, p->m_windowGeo + geo.pos(), clip, blendMode );
	
				// Maska against corner piece
	
				if( !m_cornerGeo.isEmpty() && m_pCornerSkin && m_pCornerSkin->isOpaque() )
					patches.sub( Rect(m_cornerGeo + geo.pos(), clip) );
	
				break;
			}
			case MaskOp::Skip:
				break;
			case MaskOp::Mask:
				patches.sub( Rect(geo,clip) );
				break;
		}
	}
	
	//____ _alphaTest() ___________________________________________________________
	
	bool ScrollPanel::_alphaTest( const Coord& ofs )
	{
		if( m_pSkin && m_elements[WINDOW].m_windowGeo.contains( ofs ) )
		{
			return m_pSkin->markTest( ofs, m_elements[WINDOW].m_canvasGeo, m_state, m_markOpacity );
		}
	
		if( m_pCornerSkin && m_cornerGeo.contains( ofs ) )
		{
			return m_pCornerSkin->markTest( ofs, m_cornerGeo, m_state, m_markOpacity );
		}
	
		return false;
	}
	
	//____ _cloneContent() _______________________________________________________
	
	void ScrollPanel::_cloneContent( const Widget * _pOrg )
	{
		ScrollPanel* pOrg = (ScrollPanel*)_pOrg;
	
		Panel::_cloneContent(pOrg);
		
	
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
			Scrollbar * pScrollbar = (Scrollbar*) pOrg->m_elements[XDRAG]._widget()->_newOfMyType();
			pScrollbar->cloneContent( pOrg->m_elements[XDRAG]._widget() );
			pScrollbar->setScrollbarTarget(&m_scrollbarTargets[1]);
			m_elements[XDRAG]._setWidget(pScrollbar);		
		}
	
		//
		
		m_elements[YDRAG].m_windowGeo = pOrg->m_elements[YDRAG].m_windowGeo;
		m_elements[YDRAG].m_canvasGeo = pOrg->m_elements[YDRAG].m_canvasGeo;
		m_elements[YDRAG].m_bVisible = pOrg->m_elements[YDRAG].m_bVisible;
		if( pOrg->m_elements[YDRAG]._widget() )
		{
			Scrollbar * pScrollbar = (Scrollbar*) pOrg->m_elements[YDRAG]._widget()->_newOfMyType();
			pScrollbar->cloneContent( pOrg->m_elements[YDRAG]._widget() );
			pScrollbar->setScrollbarTarget(&m_scrollbarTargets[0]);
			m_elements[YDRAG]._setWidget(pScrollbar);
		}
	
		//
	}
	
	//______________________________________________________________________________
	bool ScrollPanel::setScrollWheel( int wheel )
	{
		m_wheelForScroll = wheel;
		return true;
	}
	
	//_____________________________________________________________________________
	bool ScrollPanel::setAutoscroll( bool bAutoX, bool bAutoY )
	{
		m_bAutoScrollX = bAutoX;
		m_bAutoScrollY = bAutoY;
		return true;
	}
	
	
	//_____________________________________________________________________________
	Hook*	ScrollPanel::_firstHook() const
	{
		for( int i = 0 ; i < 3 ; i++ )
			if( m_elements[i]._widget() )
				return const_cast<ScrollHook*>(&m_elements[i]);
	
		return 0;
	}
	
	//_____________________________________________________________________________
	Hook*	ScrollPanel::_lastHook() const
	{
		for( int i = 2 ; i >= 0 ; i++ )
			if( m_elements[i]._widget() )
				return const_cast<ScrollHook*>(&m_elements[i]);
	
		return 0;
	}
	
	
	//_____________________________________________________________________________
	Hook * ScrollPanel::_firstHookWithGeo( Rect& geo ) const
	{
		for( int i = 0 ; i < 3 ; i++ )
			if( m_elements[i]._widget() )
			{
				geo = m_elements[i].m_canvasGeo;
				return const_cast<ScrollHook*>(&m_elements[i]);
			}
	
		return 0;
	}
	
	//_____________________________________________________________________________
	Hook * ScrollPanel::_nextHookWithGeo( Rect& geo, Hook * pHook ) const
	{
		const ScrollHook * pLast = &m_elements[2];
		ScrollHook * p = (ScrollHook*) pHook;
	
	
		while( p != pLast )
		{
			p++;
			if( p->_widget() )
			{
				geo = p->m_canvasGeo;
				return const_cast<ScrollHook*>(p);
			}
		}
		return 0;
	}
	
	//_____________________________________________________________________________
	Hook * ScrollPanel::_lastHookWithGeo( Rect& geo ) const
	{
		for( int i = 2 ; i >= 0 ; i++ )
			if( m_elements[i]._widget() )
			{
				geo = m_elements[i].m_canvasGeo;
				return const_cast<ScrollHook*>(&m_elements[i]);
			}
	
		return 0;
	}
	
	//_____________________________________________________________________________
	Hook * ScrollPanel::_prevHookWithGeo( Rect& geo, Hook * pHook ) const
	{
		const ScrollHook * pFirst = &m_elements[0];
		ScrollHook * p = (ScrollHook*) pHook;
	
	
		while( p != pFirst )
		{
			p--;
			if( p->_widget() )
			{
				geo = p->m_canvasGeo;
				return const_cast<ScrollHook*>(p);
			}
		}
		return 0;
	}
	
	//____ ScrollHook::Destructor ___________________________________________________
	
	ScrollHook::~ScrollHook()
	{
	}
	
	//____ ScrollHook::isInstanceOf() __________________________________________
	
	bool ScrollHook::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return PanelHook::isInstanceOf(pClassName);
	}
	
	//____ ScrollHook::className() _____________________________________________
	
	const char * ScrollHook::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ ScrollHook::cast() __________________________________________________
	
	ScrollHook_p ScrollHook::cast( const Hook_p& pHook )
	{
		if( pHook && pHook->isInstanceOf(CLASSNAME) )
			return ScrollHook_p( static_cast<ScrollHook*>(pHook.rawPtr()) );
	
		return 0;
	}
	
	
	//____ ScrollHook::pos() ________________________________________________________
	
	Coord ScrollHook::pos() const
	{
		return m_canvasGeo.pos();
	}
	
	//____ ScrollHook::size() _______________________________________________________
	
	Size ScrollHook::size() const
	{
		return m_canvasGeo.size();
	}
	
	//____ ScrollHook::geo() ________________________________________________________
	
	Rect ScrollHook::geo() const
	{
		return m_canvasGeo;
	}
	
	//____ ScrollHook::globalPos() __________________________________________________
	
	Coord ScrollHook::globalPos() const
	{
		return m_pView->globalPos() + m_canvasGeo.pos();
	}
	
	//____ ScrollHook::globalGeo() __________________________________________________
	
	Rect ScrollHook::globalGeo() const
	{
		return m_canvasGeo + m_pView->globalPos();
	}
	
	
	//____ ScrollHook::_requestRender() ______________________________________________
	
	void ScrollHook::_requestRender()
	{
		if( m_bVisible )
			m_pView->_requestRender( m_windowGeo );
	}
	
	void ScrollHook::_requestRender( const Rect& rect )
	{
		if( m_bVisible )
		{
			Rect r( m_windowGeo, rect + m_canvasGeo.pos() );
	
			if( !r.isEmpty() )
				m_pView->_requestRender( r );
		}
	}
	
	//____ ScrollHook::_requestResize() ______________________________________________
	
	void ScrollHook::_requestResize()
	{
		m_pView->_onRequestResize(this);
	}
	
	//____ ScrollHook::_windowSection() ______________________________________________
	
	Rect ScrollHook::_windowSection() const 
	{ 
		Rect window(m_windowGeo,m_canvasGeo);			// Use intersection in case canvas is smaller than window.
		return window - m_canvasGeo.pos();
	}
	
	
	//____ ScrollHook::_prevHook() ___________________________________________________
	
	Hook * ScrollHook::_prevHook() const
	{
		const ScrollHook * pFirst = &m_pView->m_elements[0];
		ScrollHook * p 		= const_cast<ScrollHook*>(this);
	
		while( p != pFirst )
		{
			p--;
			if( p->_widget() )
				return p;
		}
		return 0;
	}
	
	//____ ScrollHook::_nextHook() ___________________________________________________
	
	Hook * ScrollHook::_nextHook() const
	{
		ScrollHook * pLast = &m_pView->m_elements[2];
		ScrollHook * p 		= const_cast<ScrollHook*>(this);
	
		while( p != pLast )
		{
			p++;
			if( p->_widget() )
				return p;
		}
		return 0;
	}
	
	//____ ScrollHook::_parent() ___________________________________________________
	
	Container * ScrollHook::_parent() const
	{
		return m_pView;
	}
	
	//____ MyScrollbarTarget methods ____________________________________________________
	
	float ScrollPanel::MyScrollbarTarget::_stepFwd()
	{
		if( m_bHorizontal )
		{
			m_pParent->stepRight();
			return m_pParent->viewOfsX();
		}
		else
		{
			m_pParent->stepDown();
			return m_pParent->viewOfsY();
		}
	}
	
	float ScrollPanel::MyScrollbarTarget::_stepBwd()
	{
		if( m_bHorizontal )
		{
			m_pParent->stepLeft();
			return m_pParent->viewOfsX();
		}
		else
		{
			m_pParent->stepUp();
			return m_pParent->viewOfsY();
		}
	}
	
	float ScrollPanel::MyScrollbarTarget::_jumpFwd()
	{
		if( m_bHorizontal )
		{
			m_pParent->jumpRight();
			return m_pParent->viewOfsX();
		}
		else
		{
			m_pParent->jumpDown();
			return m_pParent->viewOfsY();
		}
	}
	
	float ScrollPanel::MyScrollbarTarget::_jumpBwd()
	{
		if( m_bHorizontal )
		{
			m_pParent->jumpLeft();
			return m_pParent->viewOfsX();
		}
		else
		{
			m_pParent->jumpUp();
			return m_pParent->viewOfsY();
		}
	}
	
	float ScrollPanel::MyScrollbarTarget::_wheelRolled( int distance )
	{
		if( m_bHorizontal )
		{
			m_pParent->_wheelRollX(distance);
			return m_pParent->viewOfsX();
		}
		else
		{	
			m_pParent->_wheelRollY(distance);
			return m_pParent->viewOfsY();
		}
	}
	
	float ScrollPanel::MyScrollbarTarget::_setPosition( float fraction )
	{
		if( m_bHorizontal )
		{
			m_pParent->setViewOfsX(fraction);
			return m_pParent->viewOfsX();
		}
		else
		{
			m_pParent->setViewOfsY(fraction);
			return m_pParent->viewOfsY();
		}
	}
	
	Widget* ScrollPanel::MyScrollbarTarget::_getWidget()
	{
		return m_pParent;
	}
	
	float ScrollPanel::MyScrollbarTarget::_getHandlePosition()
	{
		if( m_bHorizontal )
			return m_pParent->viewOfsX();
		else
			return m_pParent->viewOfsY();
	}
	
	float ScrollPanel::MyScrollbarTarget::_getHandleSize()
	{
		if( m_bHorizontal )
			return m_pParent->viewLenX();
		else
			return m_pParent->viewLenY();
	}
	
	
	

} // namespace wg
