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
	
	//____ isInstanceOf() _________________________________________________________
	
	bool WgScrollPanel::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return WgPanel::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * WgScrollPanel::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	WgScrollPanel_p WgScrollPanel::cast( const WgObject_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return WgScrollPanel_p( static_cast<WgScrollPanel*>(pObject.rawPtr()) );
	
		return 0;
	}
	
	
	//____ stepUp() _______________________________________________________________
	
	bool WgScrollPanel::stepUp()
	{
		int ofs = m_viewPixOfs.y - m_stepSizeY;
	
		if( ofs < 0 )
			ofs = 0;
	
		return setViewPixelOfsY( ofs );
	}
	
	//____ stepDown() _____________________________________________________________
	bool WgScrollPanel::stepDown()
	{
		return setViewPixelOfsY( m_viewPixOfs.y + m_stepSizeY );
	}
	
	//____ stepLeft() _____________________________________________________________
	bool WgScrollPanel::stepLeft()
	{
		int ofs = m_viewPixOfs.x - m_stepSizeX;
	
		if( ofs < 0 )
			ofs = 0;
	
		return setViewPixelOfsX( ofs );
	}
	
	//____ stepRight() ____________________________________________________________
	
	bool WgScrollPanel::stepRight()
	{
		return setViewPixelOfsX( m_viewPixOfs.x + m_stepSizeX );
	}
	
	
	//____ jumpUp() _______________________________________________________________
	
	bool WgScrollPanel::jumpUp()
	{
		int ofs = m_viewPixOfs.y - (int)(_paddedViewPixelLenY() * m_jumpSizeY);
		if( ofs < 0 )
			ofs = 0;
	
		return setViewPixelOfsY( ofs );
	}
	
	//____ jumpDown() _____________________________________________________________
	
	bool WgScrollPanel::jumpDown()
	{
		return setViewPixelOfsY( m_viewPixOfs.y + (int)(_paddedViewPixelLenY() * m_jumpSizeY) );
	}
	
	//____ jumpLeft() _____________________________________________________________
	
	bool WgScrollPanel::jumpLeft()
	{
		int ofs = m_viewPixOfs.x - (int)(_paddedViewPixelLenX() * m_jumpSizeX);
		if( ofs < 0 )
			ofs = 0;
	
		return setViewPixelOfsX( ofs );
	}
	
	//____ jumpRight() ____________________________________________________________
	
	bool WgScrollPanel::jumpRight()
	{
		return setViewPixelOfsX( m_viewPixOfs.x + (int)(_paddedViewPixelLenX() * m_jumpSizeX) );
	}
	
	//____ _wheelRollX() ___________________________________________________________
	
	bool WgScrollPanel::_wheelRollX(int distance)
	{
		int ofs = m_viewPixOfs.x - (m_stepSizeX * distance * 3);
	
		if( ofs < 0 )
			ofs = 0;
	
		return setViewPixelOfsX( ofs );
	}
	
	//____ _wheelRollY() ___________________________________________________________
	
	bool WgScrollPanel::_wheelRollY(int distance)
	{
		int ofs = m_viewPixOfs.y - (m_stepSizeY * distance * 3);
	
		if( ofs < 0 )
			ofs = 0;
	
		return setViewPixelOfsY( ofs );
	}
	
	//____ setJumpSizeX() _________________________________________________________
	
	void WgScrollPanel::setJumpSizeX( float viewFraction )
	{
		if( viewFraction < 0.f )
			viewFraction = 0.f;
		if( viewFraction > 1.f )
			viewFraction = 1.f;
	
		m_jumpSizeX = viewFraction;
	}
	
	//____ setJumpSizeY() _________________________________________________________
	
	void WgScrollPanel::setJumpSizeY( float viewFraction )
	{
		if( viewFraction < 0.f )
			viewFraction = 0.f;
		if( viewFraction > 1.f )
			viewFraction = 1.f;
	
		m_jumpSizeY = viewFraction;
	}
	
	//____ isHorizontalScrollbarVisible() ____________________________________________________
	
	bool WgScrollPanel::isHorizontalScrollbarVisible()
	{
		return m_elements[XDRAG].m_bVisible;
	}
	
	//____ isVerticalScrollbarVisible() ____________________________________________________
	
	bool WgScrollPanel::isVerticalScrollbarVisible()
	{
		return m_elements[YDRAG].m_bVisible;
	}
	
	//____ setOverlayScrollbars() ____________________________________________________
	
	void WgScrollPanel::setOverlayScrollbars( bool bOverlay )
	{
		if( bOverlay != m_bOverlayScrollbars )
		{
			m_bOverlayScrollbars = bOverlay;
			_updateElementGeo( size() );
		}
	}
	
	//____ viewPixelLenX() ________________________________________________________
	
	int WgScrollPanel::viewPixelLenX()
	{
		return m_elements[WINDOW].m_windowGeo.w;
	}
	
	//____ viewPixelLenY() ________________________________________________________
	
	int WgScrollPanel::viewPixelLenY()
	{
		return m_elements[WINDOW].m_windowGeo.h;
	}
	
	//____ viewOfsX() _____________________________________________________________
	
	float WgScrollPanel::viewOfsX()
	{
		int size = m_contentSize.w - viewPixelLenX();
		if( size <= 0 )
			return 0.f;
		else
			return m_viewPixOfs.x / (float) size;
	
	}
	
	//____ viewOfsY() _____________________________________________________________
	
	float WgScrollPanel::viewOfsY()
	{
		int size = m_contentSize.h - viewPixelLenY();
		if( size <= 0 )
			return 0.f;
		else
			return m_viewPixOfs.y / (float) size;
	}
	
	//____ viewLenX() _____________________________________________________________
	
	float WgScrollPanel::viewLenX()
	{
		if( m_contentSize.w == 0 )
			return 1.0f;
	
		float len = viewPixelLenX()/(float)m_contentSize.w;
	
		if( len > 1.f )
			len = 1.f;
	
		return len;
	}
	
	//____ viewLenY() _____________________________________________________________
	
	float WgScrollPanel::viewLenY()
	{
		if( m_contentSize.h == 0 )
			return 1.0f;
	
		float len = viewPixelLenY()/(float)m_contentSize.h;
	
		if( len > 1.f )
			len = 1.f;
	
		return len;
	}
	
	//____ _paddedViewPixelLenX() _______________________________________________
	
	int WgScrollPanel::_paddedViewPixelLenX()
	{
		WgSize	windowPadding = m_elements[WINDOW]._widget() ? m_elements[WINDOW]._widget()->_windowPadding() : WgSize(0,0);
		return m_elements[WINDOW].m_windowGeo.w - windowPadding.w;
	}
	
	//____ _paddedViewPixelLenY() ________________________________________________________
	
	int WgScrollPanel::_paddedViewPixelLenY()
	{
		WgSize	windowPadding = m_elements[WINDOW]._widget() ? m_elements[WINDOW]._widget()->_windowPadding() : WgSize(0,0);
		return m_elements[WINDOW].m_windowGeo.h - windowPadding.h;
	}
	
	//____ _paddedViewLenX() _____________________________________________________________
	
	float WgScrollPanel::_paddedViewLenX()
	{
		WgSize	windowPadding = m_elements[WINDOW]._widget() ? m_elements[WINDOW]._widget()->_windowPadding() : WgSize(0,0);
	
		if( m_contentSize.w - windowPadding.w <= 0 )
			return 1.0f;
	
		float len = (m_elements[WINDOW].m_windowGeo.w - windowPadding.w)/(float)(m_contentSize.w - windowPadding.w);
	
		if( len > 1.f )
			len = 1.f;
	
		return len;
	}
	
	//____ _paddedViewLenY() _____________________________________________________________
	
	float WgScrollPanel::_paddedViewLenY()
	{
		WgSize	windowPadding = m_elements[WINDOW]._widget() ? m_elements[WINDOW]._widget()->_windowPadding() : WgSize(0,0);
	
		if( m_contentSize.h - windowPadding.h <= 0 )
			return 1.0f;
	
		float len = (m_elements[WINDOW].m_windowGeo.h - windowPadding.h)/(float)(m_contentSize.h - windowPadding.h);
	
		if( len > 1.f )
			len = 1.f;
	
		return len;
	}
	
	
	//____ setViewPixelOfs() ______________________________________________________
	
	bool WgScrollPanel::setViewPixelOfs( int x, int y )
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
	
	bool WgScrollPanel::setViewPixelOfsX( int x )
	{
		return setViewPixelOfs(x, m_viewPixOfs.y);
	}
	
	//____ setViewPixelOfsY() _____________________________________________________
	
	bool WgScrollPanel::setViewPixelOfsY( int y )
	{
		return setViewPixelOfs(m_viewPixOfs.x, y);
	}
	
	//____ setViewOfs() ___________________________________________________________
	
	bool WgScrollPanel::setViewOfs( float x, float y )
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
	
	bool WgScrollPanel::setViewOfsX( float x )
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
	
	bool WgScrollPanel::setViewOfsY( float y )
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
	
	void WgScrollPanel::setSkin( const WgSkin_p& pSkin )
	{
		WgPanel::setSkin(pSkin);
		m_elements[WINDOW].m_canvasGeo = _genContentCanvasGeo( m_elements[WINDOW].m_windowGeo, m_contentSize, m_contentOrigo, m_viewPixOfs );
	}
	
	
	//____ setContent() ___________________________________________________________
	
	WgScrollHook_p WgScrollPanel::setContent( const WgWidget_p& pContent )
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
	
	WgScrollHook_p WgScrollPanel::setHorizontalScrollbar( const WgScrollbar_p& pScrollbar )
	{
		// Remove us as target target from current Scrollbar (if we have any)
	
		if( m_elements[XDRAG]._widget() )
			((WgScrollbar*)m_elements[XDRAG]._widget())->setScrollbarTarget(0);
	
		// 
	
		pScrollbar->setOrientation( WG_HORIZONTAL );
	
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
	
	WgScrollHook_p WgScrollPanel::setVerticalScrollbar( const WgScrollbar_p& pScrollbar )
	{
		// Remove us as target from current Scrollbar (if we have any)
	
		if( m_elements[YDRAG]._widget() )
			((WgScrollbar*)m_elements[YDRAG]._widget())->setScrollbarTarget(0);
	
		//
	
		pScrollbar->setOrientation( WG_VERTICAL );
	
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
	
	WgScrollbar_p WgScrollPanel::horizontalScrollbar() const 
	{
		return static_cast<WgScrollbar*>(m_elements[XDRAG]._widget()); 
	}
	
	WgScrollbar_p WgScrollPanel::verticalScrollbar() const 
	{ 
		return static_cast<WgScrollbar*>(m_elements[YDRAG]._widget()); 
	}
	
	WgWidget_p WgScrollPanel::content() const 
	{ 
		return m_elements[WINDOW]._widget(); 
	}
	
	
	
	//____ removeWidget() __________________________________________________________
	
	bool WgScrollPanel::removeWidget( const WgWidget_p& pWidget )
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
	
	bool WgScrollPanel::clear()
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
	
	void WgScrollPanel::setScrollbarAutoHide( bool bHideHScrollbar, bool bHideVScrollbar )
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
	
	void WgScrollPanel::setHandlePositions( bool bBottom, bool bRight )
	{
		if( bBottom == m_bScrollbarBottom && bRight == m_bScrollbarRight )
			return;
	
		m_bScrollbarBottom	= bBottom;
		m_bScrollbarRight	= bRight;
	
		_updateElementGeo( size() );
	}
	
	//____ setCornerSkin() ______________________________________________________
	
	void WgScrollPanel::setCornerSkin( const WgSkin_p& pSkin )
	{
		m_pCornerSkin = pSkin;
		_requestRender( m_cornerGeo );
	}
	
	//____ _findWidget() ____________________________________________________________
	
	WgWidget * WgScrollPanel::_findWidget( const WgCoord& pos, WgSearchMode mode )
	{
		// Check XDRAG
	
		WgScrollHook * p = &m_elements[XDRAG];
		if( p->isVisible() && p->_widget() && p->m_windowGeo.contains( pos ) )
		{
			if( mode != WG_SEARCH_MARKPOLICY || p->_widget()->markTest( pos - p->m_windowGeo.pos() ) )
				return p->_widget();
		}
	
		// Check YDRAG
	
		p = &m_elements[YDRAG];
		if( p->isVisible() && p->_widget() && p->m_windowGeo.contains( pos ) )
		{
			if( mode != WG_SEARCH_MARKPOLICY || p->_widget()->markTest( pos - p->m_windowGeo.pos() ) )
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
					WgWidget * pFound = static_cast<WgContainer*>(p->_widget())->_findWidget( pos - p->m_canvasGeo.pos(), mode );
					if( pFound )
						return pFound;
				}
				else if( mode != WG_SEARCH_MARKPOLICY || p->_widget()->markTest( pos - p->m_canvasGeo.pos() ) )
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
	
		if( mode == WG_SEARCH_GEOMETRY || markTest( pos ) )
			return this;
	
		//
	
		return 0;
	}
	
	//____ preferredSize() ___________________________________________________________
	
	WgSize WgScrollPanel::preferredSize() const
	{
		WgSize sz;
	
		if( m_elements[WINDOW]._widget() )
			sz = m_elements[WINDOW]._paddedPreferredSize();
	
		if( m_elements[XDRAG]._widget() && !m_bAutoHideScrollbarX )
		{
			WgSize scrollbar = m_elements[XDRAG]._paddedPreferredSize();
			sz.h += scrollbar.h;
			if( scrollbar.w > sz.w )
				sz.w = scrollbar.w;
		}
	
		if( m_elements[YDRAG]._widget() && !m_bAutoHideScrollbarY )
		{
			WgSize scrollbar = m_elements[YDRAG]._paddedPreferredSize();
			sz.w += scrollbar.w;
			if( scrollbar.h > sz.h )
				sz.h = scrollbar.h;
		}
	
		return sz;
	}
	
	//____ setContentOrigo() _________________________________________________
	
	void  WgScrollPanel::setContentOrigo( WgOrigo origo )
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
	
	//____ setContentSizePolicy() __________________________________________________
	
	void  WgScrollPanel::setContentSizePolicy( WgSizePolicy widthPolicy, WgSizePolicy heightPolicy )
	{
		if( widthPolicy != m_widthPolicy || heightPolicy != m_heightPolicy )
		{
			m_widthPolicy = widthPolicy;
			m_heightPolicy = heightPolicy;
	
			_updateElementGeo( size() );
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
			mySize.h -= m_elements[XDRAG]._widget()->preferredSize().h;
	
		if( !m_bAutoHideScrollbarY && m_elements[YDRAG]._widget() )
			mySize.w -= m_elements[YDRAG]._widget()->preferredSize().w;
	
		//
	
		WgSize contentSize = m_elements[WINDOW]._sizeFromPolicy( mySize, m_widthPolicy, m_heightPolicy );
	
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
				m_elements[XDRAG]._widget()->_onNewSize(newDragX.size());
			if( bShowDragY )
				m_elements[YDRAG]._widget()->_onNewSize(newDragY.size());
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
			m_scrollbarTargets[1]._updateScrollbar( viewOfsX(), _paddedViewLenX() );
	
		if( bNewOfsY || bNewHeight || bNewContentHeight )
			m_scrollbarTargets[0]._updateScrollbar( viewOfsY(), _paddedViewLenY() );
	}
	
	//____ _genContentCanvasGeo() __________________________________________________
	
	WgRect WgScrollPanel::_genContentCanvasGeo( const WgRect& window, WgSize contentSize, WgOrigo origo, WgCoord viewOfs )
	{
		WgRect	out( window.pos() - viewOfs, contentSize);
	
		if( window.w > contentSize.w )
		{
			WgRect r = WgUtil::origoToRect( origo, WgSize(window.w,1), WgSize(contentSize.w,1) );
			out.x = window.x + r.x;
		}
	
		if( window.h > contentSize.h )
		{
			WgRect r = WgUtil::origoToRect( origo, WgSize(1,window.h), WgSize(1,contentSize.h) );
			out.y = window.y + r.y;
		}
	
		out.shrink( m_elements[WINDOW].m_padding );
		return out;
	}
	
	//____ _onRequestResize() _____________________________________________________
	
	void WgScrollPanel::_onRequestResize( WgScrollHook * pHook )
	{
		_updateElementGeo( size() );
	
	
	}
	
	
	//____ _onNewSize() ____________________________________________________________
	
	void WgScrollPanel::_onNewSize( const WgSize& size )
	{
		_updateElementGeo( size );
	}
	
	//____ _onMsg() ______________________________________________________________
	
	void WgScrollPanel::_onMsg( const WgMsg_p& _pMsg )
	{
		WgPanel::_onMsg(_pMsg);
	
		switch( _pMsg->type() )
		{
			case WG_MSG_WHEEL_ROLL:
			{			
				WgWheelRollMsg_p pMsg = WgWheelRollMsg::cast(_pMsg);
	
				if( m_elements[WINDOW].m_windowGeo.contains( toLocal(pMsg->pointerPos())) )
				{
					int wheel = pMsg->wheel();
	
					if( wheel == m_wheelForScrollY )
						_wheelRollY( pMsg->distance() );
					else if( wheel == m_wheelForScrollX )
						_wheelRollX( pMsg->distance() );
				}
	
				_pMsg->swallow();
			}
			break;
	
			default:
				break;
		}
	}
	
	//____ _renderPatches() ________________________________________________________
	
	void WgScrollPanel::_renderPatches( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, WgPatches * _pPatches )
	{
		// We start by eliminating dirt outside our geometry
	
		WgPatches 	patches( _pPatches->size() );								// TODO: Optimize by pre-allocating?
	
		for( const WgRect * pRect = _pPatches->begin() ; pRect != _pPatches->end() ; pRect++ )
		{
			if( _window.intersectsWith( *pRect ) )
				patches.push( WgRect(*pRect,_window) );
		}
	
		//
	
		WgRect	dirtBounds = patches.getUnion();
	
		// Render Window background skin
	
		if( m_pSkin )
		{
			WgRect skinWindow = m_elements[WINDOW].m_windowGeo + _canvas.pos();
	
			WgSize skinSize = WgSize::max(m_contentSize, m_elements[WINDOW].m_windowGeo);
			WgRect skinCanvas = WgRect( skinWindow.pos() - m_viewPixOfs, skinSize );
	
			for( const WgRect * pRect = patches.begin() ; pRect != patches.end() ; pRect++ )
			{
				WgRect clip(*pRect,skinWindow);
				m_pSkin->render( pDevice, skinCanvas, m_state, clip );
			}
		}
	
		//
	
		if( m_elements[WINDOW]._widget() )
		{
			WgRect canvas = m_elements[WINDOW].m_canvasGeo + _canvas.pos();
			WgRect window( canvas, m_elements[WINDOW].m_windowGeo + _canvas.pos() );	// Use intersection in case canvas is smaller than window.
	
			if( window.intersectsWith(dirtBounds) )
				m_elements[WINDOW]._widget()->_renderPatches( pDevice, canvas, window, &patches );
		}
		if( m_elements[XDRAG].m_bVisible )
		{
			WgRect canvas = m_elements[XDRAG].m_windowGeo + _canvas.pos();
			if( canvas.intersectsWith(dirtBounds) )
				m_elements[XDRAG]._widget()->_renderPatches( pDevice, canvas, canvas, &patches );
		}
		if( m_elements[YDRAG].m_bVisible )
		{
			WgRect canvas = m_elements[YDRAG].m_windowGeo + _canvas.pos();
			if( canvas.intersectsWith(dirtBounds) )
				m_elements[YDRAG]._widget()->_renderPatches( pDevice, canvas, canvas, &patches );
		}
	
		if( m_pCornerSkin && m_cornerGeo.w != 0 && m_cornerGeo.h != 0 )
		{
			WgRect canvas = m_cornerGeo + _canvas.pos();
	
			for( const WgRect * pRect = patches.begin() ; pRect != patches.end() ; pRect++ )
			{
				WgRect clip( canvas, *pRect );
				if( clip.w > 0 || clip.h > 0 )
					m_pCornerSkin->render( pDevice, canvas, m_state, clip );
			}
	
		}
	}
	
	
	//____ _onCollectPatches() _______________________________________________________
	
	void WgScrollPanel::_onCollectPatches( WgPatches& container, const WgRect& geo, const WgRect& clip )
	{
		container.add( WgRect(geo,clip) );
	}
	
	//____ _onMaskPatches() __________________________________________________________
	
	void WgScrollPanel::_onMaskPatches( WgPatches& patches, const WgRect& geo, const WgRect& clip, WgBlendMode blendMode )
	{
		//TODO: Don't just check isOpaque() globally, check rect by rect.
		if( (m_bOpaque && blendMode == WG_BLENDMODE_BLEND) || blendMode == WG_BLENDMODE_OPAQUE ||
			(m_pSkin && m_pSkin->isOpaque(m_state)) )
		{
			patches.sub( WgRect(geo,clip) );
			return;
		}
	
		switch( m_maskOp )
		{
			case WG_MASKOP_RECURSE:
			{
				// Mask against view
	
				WgScrollHook * p = &m_elements[WINDOW];
	
				if( m_pSkin && m_pSkin->isOpaque() )
					patches.sub( WgRect( p->m_windowGeo + geo.pos(), clip) );
				else if( p->_widget() )
					p->_widget()->_onMaskPatches( patches, p->m_canvasGeo + geo.pos(), WgRect(p->m_windowGeo + geo.pos(), clip), blendMode );
	
				// Mask against dragbars
	
				p = &m_elements[XDRAG];
				if( p->isVisible() )
					p->_widget()->_onMaskPatches( patches, p->m_windowGeo + geo.pos(), clip, blendMode );
	
				p = &m_elements[YDRAG];
				if( p->isVisible() )
					p->_widget()->_onMaskPatches( patches, p->m_windowGeo + geo.pos(), clip, blendMode );
	
				// Maska against corner piece
	
				if( !m_cornerGeo.isEmpty() && m_pCornerSkin && m_pCornerSkin->isOpaque() )
					patches.sub( WgRect(m_cornerGeo + geo.pos(), clip) );
	
				break;
			}
			case WG_MASKOP_SKIP:
				break;
			case WG_MASKOP_MASK:
				patches.sub( WgRect(geo,clip) );
				break;
		}
	}
	
	//____ _onAlphaTest() ___________________________________________________________
	
	bool WgScrollPanel::_onAlphaTest( const WgCoord& ofs, const WgSize& sz )
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
			WgScrollbar * pScrollbar = (WgScrollbar*) pOrg->m_elements[XDRAG]._widget()->_newOfMyType();
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
			WgScrollbar * pScrollbar = (WgScrollbar*) pOrg->m_elements[YDRAG]._widget()->_newOfMyType();
			pScrollbar->cloneContent( pOrg->m_elements[YDRAG]._widget() );
			pScrollbar->setScrollbarTarget(&m_scrollbarTargets[0]);
			m_elements[YDRAG]._setWidget(pScrollbar);
		}
	
		//
	}
	
	//______________________________________________________________________________
	bool WgScrollPanel::setScrollWheels( int wheelForX, int wheelForY )
	{
		m_wheelForScrollX = wheelForX;
		m_wheelForScrollY = wheelForY;
		return true;
	}
	
	//_____________________________________________________________________________
	bool WgScrollPanel::setAutoscroll( bool bAutoX, bool bAutoY )
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
	
	//____ WgScrollHook::isInstanceOf() __________________________________________
	
	bool WgScrollHook::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return WgPanelHook::isInstanceOf(pClassName);
	}
	
	//____ WgScrollHook::className() _____________________________________________
	
	const char * WgScrollHook::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ WgScrollHook::cast() __________________________________________________
	
	WgScrollHook_p WgScrollHook::cast( const WgHook_p& pHook )
	{
		if( pHook && pHook->isInstanceOf(CLASSNAME) )
			return WgScrollHook_p( static_cast<WgScrollHook*>(pHook.rawPtr()) );
	
		return 0;
	}
	
	
	//____ WgScrollHook::pos() ________________________________________________________
	
	WgCoord WgScrollHook::pos() const
	{
		return m_canvasGeo.pos();
	}
	
	//____ WgScrollHook::size() _______________________________________________________
	
	WgSize WgScrollHook::size() const
	{
		return m_canvasGeo.size();
	}
	
	//____ WgScrollHook::geo() ________________________________________________________
	
	WgRect WgScrollHook::geo() const
	{
		return m_canvasGeo;
	}
	
	//____ WgScrollHook::globalPos() __________________________________________________
	
	WgCoord WgScrollHook::globalPos() const
	{
		return m_pView->globalPos() + m_canvasGeo.pos();
	}
	
	//____ WgScrollHook::globalGeo() __________________________________________________
	
	WgRect WgScrollHook::globalGeo() const
	{
		return m_canvasGeo + m_pView->globalPos();
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
			WgRect r( m_windowGeo, rect + m_canvasGeo.pos() );
	
			if( !r.isEmpty() )
				m_pView->_requestRender( r );
		}
	}
	
	//____ WgScrollHook::_requestResize() ______________________________________________
	
	void WgScrollHook::_requestResize()
	{
		m_pView->_onRequestResize(this);
	}
	
	//____ WgScrollHook::_windowSection() ______________________________________________
	
	WgRect WgScrollHook::_windowSection() const 
	{ 
		WgRect window(m_windowGeo,m_canvasGeo);			// Use intersection in case canvas is smaller than window.
		return window - m_canvasGeo.pos();
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
			m_pParent->stepRight();
			return m_pParent->viewOfsX();
		}
		else
		{
			m_pParent->stepDown();
			return m_pParent->viewOfsY();
		}
	}
	
	float WgScrollPanel::ScrollbarTarget::_stepBwd()
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
	
	float WgScrollPanel::ScrollbarTarget::_jumpFwd()
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
	
	float WgScrollPanel::ScrollbarTarget::_jumpBwd()
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
	
	float WgScrollPanel::ScrollbarTarget::_wheelRolled( int distance )
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
	
	float WgScrollPanel::ScrollbarTarget::_setPosition( float fraction )
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
	
	WgWidget* WgScrollPanel::ScrollbarTarget::_getWidget()
	{
		return m_pParent;
	}
	
	float WgScrollPanel::ScrollbarTarget::_getHandlePosition()
	{
		if( m_bHorizontal )
			return m_pParent->viewOfsX();
		else
			return m_pParent->viewOfsY();
	}
	
	float WgScrollPanel::ScrollbarTarget::_getHandleSize()
	{
		if( m_bHorizontal )
			return m_pParent->viewLenX();
		else
			return m_pParent->viewLenY();
	}
	
	
	

} // namespace wg
