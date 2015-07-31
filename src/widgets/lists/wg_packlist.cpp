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

#include <wg_packlist.h>
#include <wg_patches.h>
#include <wg_msgrouter.h>
#include <wg_base.h>

namespace wg 
{
	
	const char PackList::CLASSNAME[] = {"PackList"};
	const char PackListHook::CLASSNAME[] = {"PackListHook"};
	
	
	//____ PackListHook::isInstanceOf() __________________________________________
	
	bool PackListHook::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return ListHook::isInstanceOf(pClassName);
	}
	
	//____ PackListHook::className() _____________________________________________
	
	const char * PackListHook::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ PackListHook::cast() ________________________________________________
	
	PackListHook_p PackListHook::cast( const Hook_p& pHook )
	{
		if( pHook && pHook->isInstanceOf(CLASSNAME) )
			return PackListHook_p( static_cast<PackListHook*>(pHook.rawPtr()) );
	
		return 0;
	}
	
	//____ WqPackListHook::pos() _________________________________________________
	
	Coord PackListHook::pos() const
	{
		Rect	geo;
		m_pParent->_getChildGeo(geo,this);
		return geo.pos();
	}
	
	Size PackListHook::size() const
	{
		Rect	geo;
		m_pParent->_getChildGeo(geo,this);
		return geo.size();
	}
	
	Rect PackListHook::geo() const
	{
		Rect	geo;
		m_pParent->_getChildGeo(geo,this);
		return geo;
	}
	
	Coord PackListHook::globalPos() const
	{
		Rect	geo;
		m_pParent->_getChildGeo(geo,this);
		return m_pParent->globalPos() + geo.pos();
	}
	
	Rect PackListHook::globalGeo() const
	{
		Rect	geo;
		m_pParent->_getChildGeo(geo,this);
		return geo + globalPos();
	}
	
	
	void PackListHook::_requestRender()
	{
		m_pParent->_onRequestRender(this);
	}
	
	void PackListHook::_requestRender( const Rect& rect )
	{
		m_pParent->_onRequestRender(this, rect);
	}
	
	void PackListHook::_requestResize()
	{
		m_pParent->_onRequestResize(this);
	}
	
	Hook *  PackListHook::_prevHook() const
	{
		return m_pParent->m_hooks.prev(this);
	}
	
	Hook *  PackListHook::_nextHook() const
	{
		return m_pParent->m_hooks.next(this);
	}
	
	Container *  PackListHook::_parent() const
	{
		return m_pParent;
	}
	
	//____ Constructor ____________________________________________________________
	
	PackList::PackList() : m_header(this), header(&m_header)
	{
		m_bSiblingsOverlap = false;
		m_bHorizontal = false;
		m_sortOrder = WG_SORT_ASCENDING;
		m_pSortFunc = 0;
	
		m_maxEntrySize = Size(INT_MAX,INT_MAX);		//TODO: Test so m_maxEntrySize matters!
	
		m_contentBreadth = 0;
		m_contentLength = 0;
		m_contentPreferredLength = 0;
		m_contentPreferredBreadth = 0;
		m_nbPreferredBreadthEntries = 0;
	}
	
	//____ Destructor _____________________________________________________________
	
	PackList::~PackList()
	{
	}
	
	//____ isInstanceOf() _________________________________________________________
	
	bool PackList::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return List::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * PackList::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	PackList_p PackList::cast( const Object_p& pObj )
	{
		if( pObj && pObj->isInstanceOf(CLASSNAME) )
			return PackList_p( static_cast<PackList*>(pObj.rawPtr()) );
	
		return 0;
	}
	
	//____ addWidget() ____________________________________________________________
	
	PackListHook_p PackList::addWidget( const Widget_p& pWidget )
	{
		if( !pWidget )
			return 0;
	
		PackListHook * pHook = m_hooks.add();
		pHook->m_pParent = this;
		pHook->_setWidget(pWidget.rawPtr());
	
		_onWidgetAppeared( pHook );
		return pHook;
	}
	
	
	//____ insertWidget() ____________________________________________________________
	
	PackListHook_p PackList::insertWidget( const Widget_p& pWidget, const Widget_p& pSibling )
	{
		if( !pWidget )
			return 0;
	
		int index = 0;
		if( !pSibling )
			index = m_hooks.size();
		else 
		{
			PackListHook * pHook = static_cast<PackListHook*>(pSibling->_hook());
			if( pHook && pHook->_parent() == this )
				index = m_hooks.index(pHook);
			else
				return 0;
		}
	
		PackListHook * pHook = m_hooks.insert(index);
		pHook->m_pParent = this;
		pHook->_setWidget(pWidget.rawPtr());
	
		_onWidgetAppeared( pHook );
		return pHook;
	}
	
	//____ insertWidgetSorted() ___________________________________________________
	
	PackListHook_p PackList::insertWidgetSorted( const Widget_p& pWidget )
	{
		if( !pWidget )
			return 0;
	
		if( m_hooks.isEmpty() || !m_pSortFunc )
			return addWidget( pWidget );
	
		int index = _getInsertionPoint( pWidget.rawPtr() );
	
		PackListHook * pHook = m_hooks.insert(index);
		pHook->m_pParent = this;
		pHook->_setWidget(pWidget.rawPtr());
	
		_onWidgetAppeared( pHook );
		return pHook;
	}
	
	//____ removeWidget() _________________________________________________________
	
	bool PackList::removeWidget( const Widget_p& pWidget )
	{
		if( !pWidget || !pWidget->_hook() )
			return false;
	
		PackListHook * pHook = static_cast<PackListHook*>(pWidget->_hook());
		if( pHook->_parent() != this )
			return false;
	
		int index = m_hooks.index(pHook);
		_onWidgetDisappeared( pHook );
		m_hooks.remove(index);
		return true;
	}
	
	//____ clear() ________________________________________________________________
	
	bool PackList::clear()
	{
		m_hooks.clear();
		_onRefreshList();
		return true;
	}
	
	//____ setOrientation() _______________________________________________________
	
	void PackList::setOrientation( WgOrientation orientation )
	{
		bool bHorizontal = (orientation == WG_HORIZONTAL);
	
		if( bHorizontal != m_bHorizontal )
		{
			m_bHorizontal = bHorizontal;
			_onRefreshList();
		}
	}
	
	//____ sortWidgets() __________________________________________________________
	
	void PackList::sortWidgets()
	{
		_sortEntries();
	}
	
	//____ setSortOrder() _________________________________________________________
	
	void PackList::setSortOrder( WgSortOrder order )
	{
		if( order != m_sortOrder )
		{
			m_sortOrder = order;
			_sortEntries();
			_requestRender();		// So we also render the header, which has an arrow with new state.
		}
	}
	
	//____ setSortFunction() ______________________________________________________
	
	void PackList::setSortFunction( WidgetSortFunc pSortFunc )
	{
		if( pSortFunc != m_pSortFunc )
		{
			m_pSortFunc = pSortFunc;
			_sortEntries();
		}
	}
	
	//____ preferredSize() ________________________________________________________
	
	Size PackList::preferredSize() const
	{
		Size sz;
		if( m_bHorizontal )
		{
			sz =  Size(m_contentPreferredLength + m_header.m_width, m_contentPreferredBreadth);
			if( m_header.m_height > sz.h )
				sz.h = m_header.m_height;
		}
		else
		{
			sz = Size(m_contentPreferredBreadth,m_contentPreferredLength + m_header.m_height );
			if( m_header.m_width > sz.w )
				sz.w = m_header.m_width;
		}
	
		if( m_pSkin )
			sz += m_pSkin->contentPadding();
	
		return sz;
	}
	
	//____ matchingHeight() _______________________________________________________
	
	int PackList::matchingHeight( int width ) const
	{
		if( m_bHorizontal )
		{
			int height =  m_contentPreferredBreadth;
			if( m_header.m_height > height )
				height = m_header.m_height;
	
			if( m_pSkin )
				height += m_pSkin->contentPadding().h;
			return height;
		}
		else
		{
			int height = m_header.m_height;
			if( m_pSkin )
			{
				Size pad = m_pSkin->contentPadding();
				width -= pad.w;
				height += pad.h;
			}
			width -= m_entryPadding.w;
	
			for( int i = 0 ; i < m_hooks.size() ; i++ )
			{
				PackListHook * pHook = m_hooks.hook(i);
				height += pHook->_widget()->matchingHeight(width);
			}
			height += m_entryPadding.h*m_hooks.size();		
			return height;
		}
	}
	
	//____ matchingWidth() _______________________________________________________
	
	int PackList::matchingWidth( int height ) const
	{
		if( m_bHorizontal )
		{
			int width = m_header.m_width;
			if( m_pSkin )
			{
				Size pad = m_pSkin->contentPadding();
				height -= pad.w;
				width += pad.h;
			}
			height -= m_entryPadding.h;
	
			for( int i = 0 ; i < m_hooks.size() ; i++ )
			{
				PackListHook * pHook = m_hooks.hook(i);
				width += pHook->_widget()->matchingWidth(height);
			}
			width += m_entryPadding.w*m_hooks.size();		
			return width;
		}
		else
		{
			int width =  m_contentPreferredBreadth;
			if( m_header.m_width > width )
				width = m_header.m_width;
	
			if( m_pSkin )
				width += m_pSkin->contentPadding().w;
			return width;
		}
	}
	
	
	//____ setMinEntrySize() ______________________________________________________
	
	bool PackList::setMinEntrySize( Size min )
	{
		if( min == m_minEntrySize )
			return true;
	
		if( min.w > m_maxEntrySize.w || min.h > m_maxEntrySize.h )
			return false;
	
		m_minEntrySize = min;
		_onRefreshList();
		return true;
	}
	
	//____ setMaxEntrySize() ______________________________________________________
	
	bool PackList::setMaxEntrySize( Size max )
	{
		if( max == m_maxEntrySize )
			return true;
	
		if( max.w < m_minEntrySize.w || max.h < m_minEntrySize.h )
			return false;
	
		m_maxEntrySize = max;
		_onRefreshList();
		return true;
	}
	
	
	//____ _onCollectPatches() ____________________________________________________
	
	void PackList::_onCollectPatches( Patches& container, const Rect& geo, const Rect& clip )
	{
		if( m_pSkin )
			container.add( Rect( geo, clip ) );
		else
		{
			if( m_bHorizontal )
				container.add( Rect( Rect( geo.x, geo.y, WgMin(geo.w,m_contentLength), geo.h ), clip ) );
			else
				container.add( Rect( Rect( geo.x, geo.y, geo.w, WgMin(geo.h,m_contentLength) ), clip ) );
		}
	}
	
	//____ _onMaskPatches() _______________________________________________________
	
	void PackList::_onMaskPatches( Patches& patches, const Rect& geo, const Rect& clip, WgBlendMode blendMode )
	{
		if( (m_bOpaque && blendMode == WG_BLENDMODE_BLEND) || blendMode == WG_BLENDMODE_OPAQUE)
			patches.sub( Rect(geo,clip) );
		else if( m_bOpaqueEntries && blendMode == WG_BLENDMODE_BLEND )
		{
			if( m_bHorizontal )
				patches.sub( Rect( Rect( geo.x, geo.y, WgMin(geo.w,m_contentLength), geo.h ), clip ) );
			else
				patches.sub( Rect( Rect( geo.x, geo.y, geo.w, WgMin(geo.h,m_contentLength) ), clip ) );
		}
		else
		{
			Rect childGeo;
			PackListHook * p = static_cast<PackListHook*>(_firstHookWithGeo( childGeo ));
	
			while(p)
			{
				if( p->_isVisible() )
					p->_widget()->_onMaskPatches( patches, childGeo + geo.pos(), clip, blendMode );
				p = static_cast<PackListHook*>(_nextHookWithGeo( childGeo, p ));
			}
		}
	
	}
	
	//____ _onCloneContent() ______________________________________________________
	
	void PackList::_onCloneContent( const Widget * _pOrg )
	{
		//TODO: Implement!!!
	}
	
	//____ _renderPatches() _______________________________________________________
	
	void PackList::_renderPatches( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, Patches * _pPatches )
	{
		// We start by eliminating dirt outside our geometry
	
		Patches 	patches( _pPatches->size() );								// TODO: Optimize by pre-allocating?
	
		for( const Rect * pRect = _pPatches->begin() ; pRect != _pPatches->end() ; pRect++ )
		{
			if( _canvas.intersectsWith( *pRect ) )
				patches.push( Rect(*pRect,_canvas) );
		}
	
		// Render container itself
		
		for( const Rect * pRect = patches.begin() ; pRect != patches.end() ; pRect++ )
			_onRender(pDevice, _canvas, _window, *pRect );
			
		
		// Render children
	
		Rect	dirtBounds = patches.getUnion();
		
		{
			Rect childGeo;
			PackListHook * p = (PackListHook*)_firstHookWithGeo( childGeo );
	
			while(p)
			{
				Rect canvas = childGeo + _canvas.pos();
				if( p->_isVisible() && canvas.intersectsWith( dirtBounds ) )
					p->_widget()->_renderPatches( pDevice, canvas, canvas, &patches );
				p = (PackListHook*) _nextHookWithGeo( childGeo, p );
			}
		}
	
		// Render header
	
		if( m_header.m_height != 0 )
		{
			bool bInvertedSort = (m_sortOrder == WG_SORT_DESCENDING);
			Rect canvas = _headerGeo() + _canvas.pos();
	
			for( const Rect * pRect = patches.begin() ; pRect != patches.end() ; pRect++ )
				_renderHeader( pDevice, canvas, *pRect, m_header.m_pSkin, &m_header.label, &m_header.icon, &m_header.arrow, m_header.m_state, true, bInvertedSort );
		}
	
		// Render Lasso
	
		if( m_pLassoSkin && m_lassoBegin != m_lassoEnd )
		{
			Rect lasso( m_lassoBegin, m_lassoEnd );
			lasso += _canvas.pos();
	
	
			for( const Rect * pRect = patches.begin() ; pRect != patches.end() ; pRect++ )
				m_pLassoSkin->render( pDevice, lasso, m_state, Rect( lasso, *pRect ) );
		}
	}
	
	
	//____ _onRender() ____________________________________________________________
	
	void PackList::_onRender( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, const Rect& _clip )
	{
		Rect contentRect = _listCanvas() + _canvas.pos();
	
		if( m_pSkin )
		{
			m_pSkin->render( pDevice, contentRect, m_state, _clip );
			contentRect = m_pSkin->contentRect( contentRect, m_state );
		}
	
		int startOfs = m_bHorizontal ? _clip.x-contentRect.x : _clip.y-contentRect.y;
		if( startOfs < 0 )
			startOfs = 0;
	
		for( int i = _getEntryAt( startOfs ) ; i < m_hooks.size() ; i++ )
		{
			PackListHook * pHook = m_hooks.hook(i);
			Widget * pChild = pHook->_widget();
	
			// Get entry geometry, skin and state
	
			Rect entryGeo( contentRect );
			if( m_bHorizontal )
			{
				if( pHook->m_ofs >= contentRect.w )
					break;
	
				entryGeo.x += pHook->m_ofs;
				entryGeo.w = pHook->m_length;
			}
			else
			{
				if( pHook->m_ofs >= contentRect.h )
					break;
	
				entryGeo.y += pHook->m_ofs;
				entryGeo.h = pHook->m_length;
			}
			
			Skin * pEntrySkin	= m_pEntrySkin[i&0x1].rawPtr();
			State	state		= pChild->state();
	//		Rect	childGeo( entryGeo );
	
			// Render entry skin, shrink child geo
	
			if( pEntrySkin )
			{
				pEntrySkin->render( pDevice, entryGeo, state, _clip );
	//			childGeo = pEntrySkin->contentRect( entryGeo, state );
			}
	
			// Render child
	
	//		pChild->_onRender( pDevice, childGeo, childGeo, _clip );
	
		}
	}
	
	//____ _onNewSize() ___________________________________________________________
	
	void PackList::_onNewSize( const Size& size )
	{
		m_size = size;
		int newContentBreadth;
	
		if( m_bHorizontal )
			newContentBreadth = size.h;
		else
			newContentBreadth = size.w;
	
		if( newContentBreadth != m_contentBreadth )
		{
			m_contentBreadth = newContentBreadth;
			int ofs = 0;
	
			for( int i = 0 ; i < m_hooks.size() ; i++ )
			{
				PackListHook * pHook = m_hooks.hook(i);
				Widget * pWidget = pHook->_widget();
	
				if( m_bHorizontal )
				{
					int newEntryLength = _paddedLimitedMatchingWidth(pWidget, newContentBreadth );
					pHook->m_ofs = ofs;
					pHook->m_length = newEntryLength;
					ofs += newEntryLength;
	
					pWidget->_onNewSize( Size(newEntryLength, newContentBreadth) );				//TODO: Should be able to do a _onNewSize() that prevents child from doing a _requestRender().
				}
				else
				{
					int newEntryLength = _paddedLimitedMatchingHeight(pWidget, newContentBreadth );
					pHook->m_ofs = ofs;
					pHook->m_length = newEntryLength;
					ofs += newEntryLength;
	
					pWidget->_onNewSize( Size(newContentBreadth, newEntryLength) );				//TODO: Should be able to do a _onNewSize() that prevents child from doing a _requestRender().
				}
			}
			m_contentLength = ofs;
		}
	
		_requestRender();
	}
	
	//____ _onRefresh() ___________________________________________________________
	
	void PackList::_onRefresh()
	{
		_refreshHeader();
		_onRefreshList();	
		Widget::_onRefresh();
	}
	
	//____ _onRefreshList() _______________________________________________________
	
	void PackList::_onRefreshList()
	{
		if( m_pEntrySkin[0] )
			m_entryPadding = m_pEntrySkin[0]->contentPadding();
	
		m_contentPreferredLength = 0;
		m_contentPreferredBreadth = 0;
		m_nbPreferredBreadthEntries = 0;
		int ofs = 0;
	
		for( int entry = 0 ; entry < m_hooks.size() ; entry++ )
		{
			PackListHook * pHook = m_hooks.hook(entry);
			Widget * pChild = pHook->_widget();
	
			Size pref = _paddedLimitedPreferredSize( pChild );
	
			if( m_bHorizontal )
			{
				_addToContentPreferredSize( pref.w, pref.h );
				pHook->m_ofs = ofs;
	
				// Get entry length and breadth
	
				if( pref.h == m_contentBreadth )
					pHook->m_length = pref.w;
				else
					pHook->m_length	= _paddedLimitedMatchingWidth(pChild, m_contentBreadth);
				pHook->m_prefBreadth = pref.h;
				ofs += pHook->m_length;
	
			}
			else
			{
				_addToContentPreferredSize( pref.h, pref.w );
				pHook->m_ofs = ofs;
	
				// Get entry length and breadth
	
				if( pref.w == m_contentBreadth )
					pHook->m_length = pref.h;
				else
					pHook->m_length = _paddedLimitedMatchingHeight(pChild, m_contentBreadth);
				pHook->m_prefBreadth = pref.w;
				ofs += pHook->m_length;
			}
		}
		m_contentLength = ofs;
	
		// Finish up
	
		_requestRender();
		_requestResize();						// This should preferably be done first once we have changed the method.
	}
	
	//____ _onMsg() _____________________________________________________________
	
	void PackList::_onMsg( const Msg_p& _pMsg )
	{
		State oldState = m_state;
	
		switch( _pMsg->type() )
		{
			case WG_MSG_MOUSE_MOVE:
			{
				MouseMoveMsg_p pMsg = MouseMoveMsg::cast(_pMsg);
				Coord ofs = toLocal(pMsg->pointerPos());
				Rect headerGeo = _headerGeo();
				bool bHeaderHovered = headerGeo.contains(ofs) && (!Base::msgRouter()->isAnyMouseButtonPressed() || 
																 (Base::msgRouter()->isMouseButtonPressed(WG_BUTTON_LEFT) && m_header.m_bPressed));
				if( bHeaderHovered != m_header.m_state.isHovered() )
				{
					m_header.m_state.setHovered(bHeaderHovered);
					_requestRender( headerGeo );
				}
				List::_onMsg( _pMsg );
				break;
			}
	
			case WG_MSG_MOUSE_LEAVE:
			{
				MouseLeaveMsg_p pMsg = MouseLeaveMsg::cast(_pMsg);
				if( pMsg->source() == this && m_header.m_state.isHovered() )
				{
					m_header.m_state.setPressed(false);
					m_header.m_state.setHovered(false);
					_requestRender( _headerGeo() );
				}
				List::_onMsg( _pMsg );
				break;
			}
	
			case WG_MSG_MOUSE_PRESS:
			{
				MousePressMsg_p pMsg = MousePressMsg::cast(_pMsg);
				Coord ofs = toLocal(pMsg->pointerPos());
				Rect headerGeo = _headerGeo();
				if(pMsg->button() == WG_BUTTON_LEFT && headerGeo.contains(ofs))
				{
					m_header.m_bPressed = true;
					m_header.m_state.setPressed(true);
					_requestRender( headerGeo );
					pMsg->swallow();
				}
				else
					List::_onMsg( _pMsg );
				break;
			}
	
			case WG_MSG_MOUSE_DRAG:
			{
				MouseDragMsg_p pMsg = MouseDragMsg::cast(_pMsg);
				if( m_header.m_bPressed )
				{
					Coord ofs = toLocal(pMsg->pointerPos());
					Rect headerGeo = _headerGeo();
					bool bHeaderHovered = headerGeo.contains(ofs);
					if( bHeaderHovered != m_header.m_state.isHovered() )
					{
						m_header.m_state.setHovered(bHeaderHovered);
						m_header.m_state.setPressed(bHeaderHovered);
						_requestRender( headerGeo );
					}
					pMsg->swallow();
				}
				else
					List::_onMsg( _pMsg );
				break;
			}
	
			case WG_MSG_MOUSE_RELEASE:
			{
				MouseReleaseMsg_p pMsg = MouseReleaseMsg::cast(_pMsg);
				if(pMsg->button() == WG_BUTTON_LEFT && m_header.m_bPressed )
				{
					m_header.m_bPressed = false;
					m_header.m_state.setPressed(false);
					Rect headerGeo = _headerGeo();
					_requestRender( headerGeo );
	
					Coord ofs = toLocal(pMsg->pointerPos());
					if( headerGeo.contains(ofs) )
					{
						if( m_sortOrder == WG_SORT_ASCENDING )
							m_sortOrder = WG_SORT_DESCENDING;
						else
							m_sortOrder = WG_SORT_ASCENDING;
						_sortEntries();
					}
					pMsg->swallow();
				}
				else
					List::_onMsg( _pMsg );
				break;
			}
			case WG_MSG_KEY_PRESS:
			{
				if( m_selectMode == WG_SELECT_NONE )
					break;
	
				int				keyCode = KeyPressMsg::cast(_pMsg)->translatedKeyCode();
				WgModifierKeys	modKeys = KeyPressMsg::cast(_pMsg)->modKeys();
				if( (m_bHorizontal && (keyCode == WG_KEY_LEFT || keyCode == WG_KEY_RIGHT)) || 
					(!m_bHorizontal && (keyCode == WG_KEY_UP || keyCode == WG_KEY_DOWN || keyCode == WG_KEY_PAGE_UP || keyCode == WG_KEY_PAGE_DOWN)) ||
					keyCode == WG_KEY_HOME || keyCode == WG_KEY_END ||
					(m_selectMode == WG_SELECT_FLIP && keyCode == WG_KEY_SPACE ) )
						_pMsg->swallow();
				List::_onMsg( _pMsg );
				break;
			}
	
			case WG_MSG_KEY_REPEAT:
			case WG_MSG_KEY_RELEASE:
			{
				if( m_selectMode == WG_SELECT_NONE )
					break;
	
				int				keyCode = KeyMsg::cast(_pMsg)->translatedKeyCode();
				WgModifierKeys	modKeys = KeyMsg::cast(_pMsg)->modKeys();
				if( (m_bHorizontal && (keyCode == WG_KEY_LEFT || keyCode == WG_KEY_RIGHT)) || 
					(!m_bHorizontal && (keyCode == WG_KEY_UP || keyCode == WG_KEY_DOWN || keyCode == WG_KEY_PAGE_UP || keyCode == WG_KEY_PAGE_DOWN)) ||
					keyCode == WG_KEY_HOME || keyCode == WG_KEY_END ||
					(m_selectMode == WG_SELECT_FLIP && keyCode == WG_KEY_SPACE ) )
						_pMsg->swallow();
				List::_onMsg( _pMsg );
				break;
			}
		
			default:
				List::_onMsg(_pMsg);
				return;
		}
	
		if( m_state != oldState )
			_onStateChanged(oldState);
	}
	
	//____ _onLassoUpdated() ______________________________________________________
	
	void PackList::_onLassoUpdated( const Rect& oldLasso, const Rect& newLasso )
	{
		// Get out content area
	
		Rect listArea = _listArea();
	
		// Check if our lassos are inside content area or not.
	
		bool	bOldLassoInside = false;
		bool	bNewLassoInside = false;
	
		if( oldLasso.intersectsWith(listArea ) )
			bOldLassoInside = true;
		if( newLasso.intersectsWith(listArea ) )
			bNewLassoInside = true;
	
		if( !bOldLassoInside && !bNewLassoInside )
			return;										// None of the lassos inside content.
	
		// Get first/last entries marked by old/new lasso
	
		int oldOfs1, oldOfs2;
		int newOfs1, newOfs2;
	
	
		if( m_bHorizontal )
		{
			oldOfs1 = oldLasso.x - listArea.x;
			oldOfs2 = oldLasso.x + oldLasso.w - listArea.x;
			newOfs1 = newLasso.x - listArea.x;
			newOfs2 = newLasso.x + newLasso.w - listArea.x;
		}
		else
		{
			oldOfs1 = oldLasso.y - listArea.y;
			oldOfs2 = oldLasso.y + oldLasso.h - listArea.y;
			newOfs1 = newLasso.y - listArea.y;
			newOfs2 = newLasso.y + newLasso.h - listArea.y;
		}
	
		int oldFirst = _getEntryAt( oldOfs1 );
		int oldLast = _getEntryAt( oldOfs2 );
		int newFirst = _getEntryAt( newOfs1 );
		int newLast = _getEntryAt( newOfs2 );
	
		//
	
		if( bOldLassoInside != bNewLassoInside )
		{
			int beg, end;
			if( bNewLassoInside )
			{
				beg = newFirst;
				end = newLast;
			}
			else
			{
				beg = oldFirst;
				end = oldLast;
			}
	
			_flipRange( m_hooks.hook(beg), m_hooks.hook(end), true );
		}
		else
		{
			if( oldFirst != newFirst )
			{
				int beg = WgMin(oldFirst,newFirst);
				int end = WgMax(oldFirst,newFirst)-1;
	
				_flipRange( m_hooks.hook(beg), m_hooks.hook(end), true );
			}
	
			if( oldLast != newLast )
			{
				int beg = WgMin(oldLast,newLast)+1;
				int end = WgMax(oldLast,newLast);
	
				_flipRange( m_hooks.hook(beg), m_hooks.hook(end), true );
			}
		}
	}
	
	
	//____ _onStateChanged() ______________________________________________________
	
	void PackList::_onStateChanged( State oldState )
	{
		List::_onStateChanged(oldState);
	}
	
	
	//____ _onRequestRender() _____________________________________________________
	
	void PackList::_onRequestRender( PackListHook * pHook )
	{
		Rect geo;
		_getChildGeo(geo, pHook);
		_requestRender(geo);
	}
	
	void PackList::_onRequestRender( PackListHook * pHook, const Rect& rect )
	{
		Rect geo;
		_getChildGeo(geo, pHook);
		geo.x += rect.x;
		geo.y += rect.y;
		geo.w = rect.w;
		geo.h = rect.h;
		_requestRender(geo);
	}
	
	//____ _onRequestResize() _____________________________________________________
	
	void PackList::_onRequestResize( PackListHook * pHook )
	{
		if( !pHook->m_bVisible  || m_minEntrySize == m_maxEntrySize )
			return;
	
		Widget * pChild = pHook->_widget();
		Size prefEntrySize = _paddedLimitedPreferredSize(pChild);
	
		int prefLength = m_bHorizontal ? prefEntrySize.w : prefEntrySize.h;
		int prefBreadth = m_bHorizontal ? prefEntrySize.h : prefEntrySize.w;
	
		bool	bReqResize = false;
	
		// Update preferred sizes
	
		if( prefBreadth != pHook->m_prefBreadth || prefLength != pHook->m_length )
		{
			_subFromContentPreferredSize( pHook->m_length, pHook->m_prefBreadth );
			_addToContentPreferredSize( prefLength, prefBreadth );
	
			pHook->m_prefBreadth = prefBreadth;
			bReqResize = true;
		}
	
		// Calculate new length
	
		int length;
		if( prefBreadth == m_contentBreadth )	
			length = prefLength;
		else
			length = m_bHorizontal ? _paddedLimitedMatchingWidth(pChild, m_contentBreadth ) : _paddedLimitedMatchingHeight(pChild, m_contentBreadth );
	
		// Update if length has changed
	
		if( length != pHook->m_length )
		{
			m_contentLength += length - pHook->m_length;
			pHook->m_length = length;
			bReqResize = true;
	
			_updateChildOfsFrom( pHook );
			_requestRenderChildrenFrom( pHook );
	
			Rect childGeo;
			_getChildGeo(childGeo,pHook);
			pHook->_widget()->_onNewSize(childGeo);
		}
	
	
	
		if( bReqResize )
			_requestResize();
	}
	
	//____ _onWidgetAppeared() ____________________________________________________
	
	void PackList::_onWidgetAppeared( ListHook * pInserted )
	{
		PackListHook * pHook = static_cast<PackListHook*>(pInserted);
		Widget * pChild = pHook->_widget();
	
		Size pref = _paddedLimitedPreferredSize( pChild );
	
		if( m_bHorizontal )
		{
			_addToContentPreferredSize( pref.w, pref.h );
	
			// Get entry length and breadth
	
			if( pref.h == m_contentBreadth )
				pHook->m_length = pref.w;
			else
				pHook->m_length	= _paddedLimitedMatchingWidth(pChild, m_contentBreadth);
			pHook->m_prefBreadth = pref.h;
		}
		else
		{
			_addToContentPreferredSize( pref.h, pref.w );
	
			// Get entry length and breadth
	
			if( pref.w == m_contentBreadth )
				pHook->m_length = pref.h;
			else
				pHook->m_length = _paddedLimitedMatchingHeight(pChild, m_contentBreadth);
			pHook->m_prefBreadth = pref.w;
	
		}
	
		m_contentLength += pHook->m_length;
	
		Rect childGeo;
		_getChildGeo(childGeo,pHook);
		static_cast<PackListHook*>(pInserted)->_widget()->_onNewSize( childGeo );
	
	
		// Finish up
	
		_updateChildOfsFrom( pHook );
		_requestRenderChildrenFrom( pHook );	// Request render on dirty area
		_requestResize();						// This should preferably be done first once we have changed the method.
	
	}
	
	//____ _onWidgetDisappeared() _________________________________________________
	
	void PackList::_onWidgetDisappeared( ListHook * pToBeRemoved )
	{
		PackListHook * pHook = static_cast<PackListHook*>(pToBeRemoved);
		Widget * pChild = pHook->_widget();
	
		Size pref = _paddedLimitedPreferredSize( pChild );
	
		_requestRenderChildrenFrom( pHook );	// Request render on dirty area
	
		if( m_bHorizontal )
			_subFromContentPreferredSize( pref.w, pref.h );
		else
			_subFromContentPreferredSize( pref.h, pref.w );
	
		m_contentLength -= pHook->m_length;
	
		pHook->m_length = 0;
	
		_updateChildOfsFrom( m_hooks.next(pHook) );
		_requestResize();
	}
	
	//____ _getEntryAt() __________________________________________________________
	
	// Pixelofs is counted from beginning of container content, not widget.
	
	int PackList::_getEntryAt( int pixelofs ) const
	{
		int first = 0;
		int last = m_hooks.size() - 1;
		int middle = (first+last)/2;
	 
		while( first <= last )
		{
			PackListHook * pHook = m_hooks.hook(middle);
	
			if( pHook->m_ofs + pHook->m_length < pixelofs )
				first = middle + 1;
			else if( pHook->m_ofs <= pixelofs ) 
				return middle;
			else
				last = middle - 1;
	 
			middle = (first + last)/2;
		}
	
		return m_hooks.size();
	}
	
	//____ _getInsertionPoint() ___________________________________________________
	
	int PackList::_getInsertionPoint( const Widget * pWidget ) const
	{
		int first = 0;
		int last = m_hooks.size() - 1;
		int middle = (first+last)/2;
	 	int negator = m_sortOrder == WG_SORT_ASCENDING ? 1 : -1;
	
		while( first <= last )
		{
			PackListHook * pHook = m_hooks.hook(middle);
	
			int cmpRes = m_pSortFunc( pHook->_widget(), pWidget )*negator;
	
			if( cmpRes < 0 )
				first = middle + 1;
			else if( cmpRes == 0 ) 
				return middle;
			else
				last = middle - 1;
	 
			middle = (first + last)/2;
		}
	
		return first;
	}
	
	
	
	//____ _findEntry() ___________________________________________________________
	
	ListHook * PackList::_findEntry( const Coord& ofs )
	{
		Widget * pResult = 0;
		Rect list = _listArea();
	
		if( list.contains(ofs) && _listWindow().contains(ofs) )
		{
			int entry;
			if( m_bHorizontal )
				entry = _getEntryAt(ofs.x-list.x);
			else
				entry = _getEntryAt(ofs.y-list.y);
	
			if( entry != m_hooks.size() )
				return m_hooks.hook(entry);
		}
	
		return 0;
	}
	
	//____ _findWidget() __________________________________________________________
	
	Widget * PackList::_findWidget( const Coord& ofs, WgSearchMode mode )
	{
		Widget * pResult = 0;
		Rect list = _listArea();
	
		if( list.contains(ofs) && _listWindow().contains(ofs) )
		{
			int entry;
			if( m_bHorizontal )
				entry = _getEntryAt(ofs.x-list.x);
			else
				entry = _getEntryAt(ofs.y-list.y);
	
			if( entry != m_hooks.size() )
			{
				PackListHook * pHook = m_hooks.hook(entry);
				Rect childGeo;
				_getChildGeo( childGeo, pHook );
				if( childGeo.contains(ofs) )
				{
					if( pHook->_widget()->isContainer() )
					{
						pResult = static_cast<Container*>(pHook->_widget())->_findWidget( ofs - childGeo.pos(), mode );
					}
					else if( mode == WG_SEARCH_GEOMETRY || pHook->_widget()->markTest( ofs - childGeo.pos() ) )
					{
							pResult = pHook->_widget();
					}
				}
	
				if( !pResult && mode == WG_SEARCH_ACTION_TARGET )
					pResult = pHook->_widget();						// Entries are opaque as action targets.
	
			}
		}
	
		// Check against ourselves
	
		if( !pResult && ( mode == WG_SEARCH_GEOMETRY || markTest(ofs)) )
			pResult = this;
			
		return pResult;
	}
	
	//____ _addToContentPreferredSize() ___________________________________________
	
	void  PackList::_addToContentPreferredSize( int length, int breadth )
	{
		m_contentPreferredLength += length;
		
		if( breadth == m_contentPreferredBreadth )
			m_nbPreferredBreadthEntries++;
		else if( breadth > m_contentPreferredBreadth )
		{
			m_contentPreferredBreadth = breadth;
			m_nbPreferredBreadthEntries = 1;
		}
	}
	
	//____ _subFromContentPreferredSize() _________________________________________
	
	void  PackList::_subFromContentPreferredSize( int length, int breadth )
	{
		m_contentPreferredLength -= length;
		
		if( breadth == m_contentPreferredBreadth )
		{
			m_nbPreferredBreadthEntries--;
			if( m_nbPreferredBreadthEntries == 0 )
			{
				int highest = 0;
				for( PackListHook * p = m_hooks.begin() ; p < m_hooks.end() ; p++ )
				{
					if( p->m_prefBreadth == highest )
						m_nbPreferredBreadthEntries++;
					else if( p->m_prefBreadth > highest )
					{
						highest = p->m_prefBreadth;
						m_nbPreferredBreadthEntries = 0;
					}
				}
				m_contentPreferredBreadth = highest;
			}
		}
	}
	
	
	//____ _requestRenderChildrenFrom() ___________________________________________
	
	void PackList::_requestRenderChildrenFrom( PackListHook * pHook )
	{
		Rect box = _listArea();
	
		if( m_bHorizontal )
		{
			box.x += pHook->m_ofs;
			box.w = m_contentLength - pHook->m_ofs;
		}
		else
		{
			box.y += pHook->m_ofs;
			box.h = m_contentLength - pHook->m_ofs;
		}
	
		_requestRender( box );
	}
	
	//____ _updateChildOfsFrom() __________________________________________________
	
	void PackList::_updateChildOfsFrom( PackListHook * pHook )
	{
		int ofs = 0;
		PackListHook * pPrev = m_hooks.prev(pHook);
		if( pPrev )
			ofs = pPrev->m_ofs + pPrev->m_length;
	
		while( pHook < m_hooks.end() )
		{
			pHook->m_ofs = ofs;
			ofs += pHook->m_length;
			pHook++;
		}	
	}
	
	//____ _onEntrySkinChanged() __________________________________________________
	
	void PackList::_onEntrySkinChanged( Size oldPadding, Size newPadding )
	{
		_requestRender();
	
		if( oldPadding != newPadding )
		{
			m_entryPadding = newPadding;
			int nEntries = m_hooks.size();
	
			int	lengthDiff, breadthDiff;
			if( m_bHorizontal )
			{
				lengthDiff = (newPadding.w - oldPadding.w)*nEntries;
				breadthDiff = (newPadding.h - oldPadding.h)*nEntries;
			}
			else
			{
				lengthDiff = (newPadding.h - oldPadding.h)*nEntries;
				breadthDiff = (newPadding.w - oldPadding.w)*nEntries;
			}
	
			if( lengthDiff != 0 || breadthDiff != 0 )
			{
				m_contentLength += lengthDiff;
				m_contentBreadth += breadthDiff;
	
				m_contentPreferredLength += lengthDiff;
				m_contentPreferredBreadth += breadthDiff;
	
				_requestResize();
			}
		}
	}
	
	//____ _getEntryGeo() _________________________________________________________
	
	void PackList::_getEntryGeo( Rect& geo, const ListHook * _pHook ) const
	{
		const PackListHook * pHook = static_cast<const PackListHook*>(_pHook);
	
		geo = _listArea();
	
		if( m_bHorizontal )
		{
			geo.x += pHook->m_ofs;
			geo.w = pHook->m_length;
		}
		else
		{
			geo.y += pHook->m_ofs;
			geo.h = pHook->m_length;
		}
	}
	
	
	//____ _getChildGeo() _________________________________________________________
	
	void PackList::_getChildGeo( Rect& geo, const PackListHook * pHook ) const
	{
		geo = _listArea();
	
		if( m_bHorizontal )
		{
			geo.x += pHook->m_ofs;
			geo.w = pHook->m_length;
		}
		else
		{
			geo.y += pHook->m_ofs;
			geo.h = pHook->m_length;
		}
	
		// Apply any padding from the entry skin, if entry visible
	
		if( pHook->m_bVisible )
		{
			int index = m_hooks.index( pHook );
			if( m_pEntrySkin[index&0x1] )
				geo = m_pEntrySkin[index&0x1]->contentRect( geo, pHook->_widget()->state() );
		}
	}
	
	//____ _paddedLimitedMatchingHeight() _________________________________________
	
	int PackList::_paddedLimitedMatchingHeight( Widget * pChild, int paddedWidth )
	{
		int height = pChild->matchingHeight( paddedWidth - m_entryPadding.w ) + m_entryPadding.h;
		LIMIT( height, m_minEntrySize.h, m_maxEntrySize.h );
		return height;
	}
	
	//____ _paddedLimitedMatchingWidth() _________________________________________
	
	int PackList::_paddedLimitedMatchingWidth( Widget * pChild, int paddedHeight )
	{
		int width = pChild->matchingWidth( paddedHeight - m_entryPadding.h ) + m_entryPadding.w;
		LIMIT( width, m_minEntrySize.w, m_maxEntrySize.w );
		return width;
	}
	
	//____ _paddedLimitedPreferredSize() __________________________________________
	
	Size PackList::_paddedLimitedPreferredSize( Widget * pChild )
	{
		Size sz = pChild->preferredSize();
		sz += m_entryPadding;
	
		// Apply limits
	
		if( sz.w < m_minEntrySize.w )
			sz.w = m_minEntrySize.w;
		if( sz.h < m_minEntrySize.h )
			sz.h = m_minEntrySize.h;
	
		if( sz.w > m_maxEntrySize.w )
		{
			int h = pChild->matchingHeight(m_maxEntrySize.w-m_entryPadding.w) + m_entryPadding.h;
			LIMIT(h, m_minEntrySize.h, m_maxEntrySize.h );
		}
		else if( sz.h > m_maxEntrySize.h )
		{
			int w = pChild->matchingWidth(m_maxEntrySize.h-m_entryPadding.h) + m_entryPadding.w;
			LIMIT(w, m_minEntrySize.w, m_maxEntrySize.w );
		}
	
		return sz;
	}
	
	//____ _firstHook() ___________________________________________________________
	
	Hook* PackList::_firstHook() const
	{
		if( m_hooks.size() > 0 )
			return m_hooks.hook(0);
	
		return 0;
	}
	
	//____ _lastHook() ____________________________________________________________
	
	Hook* PackList::_lastHook() const
	{
		if( m_hooks.size() > 0 )
			return m_hooks.hook(m_hooks.size()-1);
	
		return 0;
	}
	
	//____ _firstHookWithGeo() ____________________________________________________
	
	Hook* PackList::_firstHookWithGeo( Rect& geo ) const
	{
		if( m_hooks.size() == 0 )
			return 0;
	
		PackListHook * p = m_hooks.hook(0);
		_getChildGeo(geo,p);
		return p;
	}
	
	//____ _nextHookWithGeo() _____________________________________________________
	
	Hook* PackList::_nextHookWithGeo( Rect& geo, Hook * pHook ) const
	{
		PackListHook * p = m_hooks.next(static_cast<PackListHook*>(pHook));
		if( p )
			_getChildGeo(geo,p);
		return p;
	}
	
	//_____ _lastHookWithGeo() ____________________________________________________
	
	Hook* PackList::_lastHookWithGeo( Rect& geo ) const
	{
		if( m_hooks.size() == 0 )
			return 0;
	
		PackListHook * p = m_hooks.hook(m_hooks.size()-1);
		_getChildGeo(geo,p);
		return p;
	}
	
	//____ _prevHookWithGeo() _____________________________________________________
	
	Hook* PackList::_prevHookWithGeo( Rect& geo, Hook * pHook ) const
	{
		PackListHook * p = m_hooks.prev(static_cast<PackListHook*>(pHook));
		if( p )
			_getChildGeo(geo,p);
		return p;
	}
	
	//____ _listArea() ____________________________________________________________
	
	Rect PackList::_listArea() const
	{
		Rect r = _listCanvas();
		if( m_pSkin )
			r = m_pSkin->contentRect( r, m_state );
	
		return r;
	}
	
	//____ _listWindow() ____________________________________________________________
	
	Rect PackList::_listWindow() const
	{
		Rect r = _windowSection();		// Window in widgets own coordinate system.
	
		if( m_bHorizontal )
		{
			r.x += m_header.m_width;
			r.w -= m_header.m_width;
		}
		else
		{
			r.y += m_header.m_height;
			r.h -= m_header.m_height;
		}
		return r;
	}
	
	//____ _listCanvas() ____________________________________________________________
	
	Rect PackList::_listCanvas() const
	{
		if( m_bHorizontal )
			return Rect(m_header.m_width, 0, m_size.w - m_header.m_width, m_size.h );
		else
			return Rect(0, m_header.m_height, m_size.w, m_size.h - m_header.m_height);	// List canvas in widgets own coordinate system.
	}
	
	//____ _headerGeo() ___________________________________________________________
	
	Rect PackList::_headerGeo() const
	{
		if( m_bHorizontal )
			return Rect( _windowSection().x, 0, m_header.m_width, m_size.h );
		else
			return Rect( 0, _windowSection().y, m_size.w, m_header.m_height );
	}
	
	//____ _windowPadding() _______________________________________________________
	
	Size PackList::_windowPadding() const
	{
		if( m_bHorizontal )
			return Size( m_header.m_width, 0 );
		else
			return Size( 0, m_header.m_height );
	}
	
	//____ _refreshHeader() _______________________________________________________
	
	void PackList::_refreshHeader()
	{
		Size wantedIconSize = m_header.icon.preferredSize();
		Size wantedArrowSize = m_header.arrow.preferredSize();
		Size wantedTextSize = m_header.label.preferredSize();
	
		Size wantedSize;
	
		//TODO: Assumes icon/arrow origos to not be NORTH, SOUTH or CENTER.
		//TODO: Assumes text not wrapping.
	
		wantedSize.h = WgMax(wantedIconSize.h, wantedArrowSize.h, wantedTextSize.h );
		wantedSize.w = wantedTextSize.w;
		if( m_header.icon.overlap() )
			wantedSize.w = WgMax(wantedSize.w,wantedIconSize.w);
		else
			wantedSize.w += wantedIconSize.w;
	
		if( m_header.arrow.overlap() )
			wantedSize.w = WgMax(wantedSize.w,wantedArrowSize.w);
		else
			wantedSize.w += wantedArrowSize.w;
	
		//
	
		if( m_header.m_pSkin )
			wantedSize = m_header.m_pSkin->sizeForContent(wantedSize);
		//
	
		bool	bRequestResize = false;
	
	
		// Update headers size, possibly request resize.
	
		if( wantedSize.h != m_header.m_height )
		{
			if( !m_bHorizontal || (wantedSize.h > m_contentPreferredBreadth || m_header.m_height > m_contentPreferredBreadth) )
				bRequestResize = true;
	
			m_header.m_height = wantedSize.h;
		}
	
		if( wantedSize.w != m_header.m_width )
		{
			if( m_bHorizontal || (wantedSize.w > m_contentPreferredBreadth || m_header.m_width > m_contentPreferredBreadth) )
				bRequestResize = true;
	
			m_header.m_width = wantedSize.w;
		}
	
		if( bRequestResize )
			_requestResize();
	}
	
	//____ _sortEntries() _________________________________________________________
	
	bool PackList::_sortEntries()
	{
		if( !m_pSortFunc )
			return false;
	
		if( m_hooks.isEmpty() )
			return true;
	
		// Create a temporary, sorted list of pointers to hooks
	
		int listSize = sizeof(int) * m_hooks.size();
		int * pOrderList = (int*) Base::memStackAlloc( listSize );
		int negator = m_sortOrder == WG_SORT_ASCENDING ? 1 : -1;
	
		pOrderList[0] = 0;
		for( int entry = 1 ; entry < m_hooks.size() ; entry++ )
		{
			Widget * pWidget = m_hooks.hook(entry)->_widget();
	
			int first = 0;
			int last = entry-1;
			int middle = (first+last)/2;
	 
			while( first <= last )
			{
				Widget * pEntry = m_hooks.hook(pOrderList[middle])->_widget();
	
				int cmpRes = m_pSortFunc( pEntry, pWidget ) * negator;
	
				if( cmpRes < 0 )
					first = middle + 1;
				else if( cmpRes == 0 ) 
				{
					first = middle;
					break;
				}
				else
					last = middle - 1;
	 
				middle = (first + last)/2;
			}
	
			for( int i = entry ; i > first ; i-- )
				pOrderList[i] = pOrderList[i-1];
	
			pOrderList[first] = entry;
		}
	
		m_hooks.reorder( pOrderList );
		Base::memStackRelease( listSize );
	
		// Update m_ofs in the hooks
	
		_updateChildOfsFrom( m_hooks.begin() );
		_requestRenderChildrenFrom( m_hooks.begin() );	// Request render on dirty area
		return true;
	}
	
	//____ _onFieldDirty() _________________________________________________________
	
	void PackList::_onFieldDirty(Field * pField)
	{
		_refreshHeader();	//TODO: Just request render on header.
	}
	
	//____ _onFieldResize() ________________________________________________________
	
	void PackList::_onFieldResize(Field * pField)
	{
		_refreshHeader();
	}
	
	

} // namespace wg
