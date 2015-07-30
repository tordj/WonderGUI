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

const char WgPackList::CLASSNAME[] = {"PackList"};
const char WgPackListHook::CLASSNAME[] = {"PackListHook"};


//____ WgPackListHook::isInstanceOf() __________________________________________

bool WgPackListHook::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgListHook::isInstanceOf(pClassName);
}

//____ WgPackListHook::className() _____________________________________________

const char * WgPackListHook::className( void ) const
{ 
	return CLASSNAME; 
}

//____ WgPackListHook::cast() ________________________________________________

WgPackListHookPtr WgPackListHook::cast( const WgHookPtr& pHook )
{
	if( pHook && pHook->isInstanceOf(CLASSNAME) )
		return WgPackListHookPtr( static_cast<WgPackListHook*>(pHook.rawPtr()) );

	return 0;
}

//____ WqPackListHook::pos() _________________________________________________

WgCoord WgPackListHook::pos() const
{
	WgRect	geo;
	m_pParent->_getChildGeo(geo,this);
	return geo.pos();
}

WgSize WgPackListHook::size() const
{
	WgRect	geo;
	m_pParent->_getChildGeo(geo,this);
	return geo.size();
}

WgRect WgPackListHook::geo() const
{
	WgRect	geo;
	m_pParent->_getChildGeo(geo,this);
	return geo;
}

WgCoord WgPackListHook::globalPos() const
{
	WgRect	geo;
	m_pParent->_getChildGeo(geo,this);
	return m_pParent->globalPos() + geo.pos();
}

WgRect WgPackListHook::globalGeo() const
{
	WgRect	geo;
	m_pParent->_getChildGeo(geo,this);
	return geo + globalPos();
}


void WgPackListHook::_requestRender()
{
	m_pParent->_onRequestRender(this);
}

void WgPackListHook::_requestRender( const WgRect& rect )
{
	m_pParent->_onRequestRender(this, rect);
}

void WgPackListHook::_requestResize()
{
	m_pParent->_onRequestResize(this);
}

WgHook *  WgPackListHook::_prevHook() const
{
	return m_pParent->m_hooks.prev(this);
}

WgHook *  WgPackListHook::_nextHook() const
{
	return m_pParent->m_hooks.next(this);
}

WgContainer *  WgPackListHook::_parent() const
{
	return m_pParent;
}

//____ Constructor ____________________________________________________________

WgPackList::WgPackList() : m_header(this), header(&m_header)
{
	m_bSiblingsOverlap = false;
	m_bHorizontal = false;
	m_sortOrder = WG_SORT_ASCENDING;
	m_pSortFunc = 0;

	m_maxEntrySize = WgSize(INT_MAX,INT_MAX);		//TODO: Test so m_maxEntrySize matters!

	m_contentBreadth = 0;
	m_contentLength = 0;
	m_contentPreferredLength = 0;
	m_contentPreferredBreadth = 0;
	m_nbPreferredBreadthEntries = 0;
}

//____ Destructor _____________________________________________________________

WgPackList::~WgPackList()
{
}

//____ isInstanceOf() _________________________________________________________

bool WgPackList::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgList::isInstanceOf(pClassName);
}

//____ className() ____________________________________________________________

const char * WgPackList::className( void ) const
{ 
	return CLASSNAME; 
}

//____ cast() _________________________________________________________________

WgPackListPtr WgPackList::cast( const WgObjectPtr& pObj )
{
	if( pObj && pObj->isInstanceOf(CLASSNAME) )
		return WgPackListPtr( static_cast<WgPackList*>(pObj.rawPtr()) );

	return 0;
}

//____ addWidget() ____________________________________________________________

WgPackListHookPtr WgPackList::addWidget( const WgWidgetPtr& pWidget )
{
	if( !pWidget )
		return 0;

	WgPackListHook * pHook = m_hooks.add();
	pHook->m_pParent = this;
	pHook->_setWidget(pWidget.rawPtr());

	_onWidgetAppeared( pHook );
	return pHook;
}


//____ insertWidget() ____________________________________________________________

WgPackListHookPtr WgPackList::insertWidget( const WgWidgetPtr& pWidget, const WgWidgetPtr& pSibling )
{
	if( !pWidget )
		return 0;

	int index = 0;
	if( !pSibling )
		index = m_hooks.size();
	else 
	{
		WgPackListHook * pHook = static_cast<WgPackListHook*>(pSibling->_hook());
		if( pHook && pHook->_parent() == this )
			index = m_hooks.index(pHook);
		else
			return 0;
	}

	WgPackListHook * pHook = m_hooks.insert(index);
	pHook->m_pParent = this;
	pHook->_setWidget(pWidget.rawPtr());

	_onWidgetAppeared( pHook );
	return pHook;
}

//____ insertWidgetSorted() ___________________________________________________

WgPackListHookPtr WgPackList::insertWidgetSorted( const WgWidgetPtr& pWidget )
{
	if( !pWidget )
		return 0;

	if( m_hooks.isEmpty() || !m_pSortFunc )
		return addWidget( pWidget );

	int index = _getInsertionPoint( pWidget.rawPtr() );

	WgPackListHook * pHook = m_hooks.insert(index);
	pHook->m_pParent = this;
	pHook->_setWidget(pWidget.rawPtr());

	_onWidgetAppeared( pHook );
	return pHook;
}

//____ removeWidget() _________________________________________________________

bool WgPackList::removeWidget( const WgWidgetPtr& pWidget )
{
	if( !pWidget || !pWidget->_hook() )
		return false;

	WgPackListHook * pHook = static_cast<WgPackListHook*>(pWidget->_hook());
	if( pHook->_parent() != this )
		return false;

	int index = m_hooks.index(pHook);
	_onWidgetDisappeared( pHook );
	m_hooks.remove(index);
	return true;
}

//____ clear() ________________________________________________________________

bool WgPackList::clear()
{
	m_hooks.clear();
	_onRefreshList();
	return true;
}

//____ setOrientation() _______________________________________________________

void WgPackList::setOrientation( WgOrientation orientation )
{
	bool bHorizontal = (orientation == WG_HORIZONTAL);

	if( bHorizontal != m_bHorizontal )
	{
		m_bHorizontal = bHorizontal;
		_onRefreshList();
	}
}

//____ sortWidgets() __________________________________________________________

void WgPackList::sortWidgets()
{
	_sortEntries();
}

//____ setSortOrder() _________________________________________________________

void WgPackList::setSortOrder( WgSortOrder order )
{
	if( order != m_sortOrder )
	{
		m_sortOrder = order;
		_sortEntries();
		_requestRender();		// So we also render the header, which has an arrow with new state.
	}
}

//____ setSortFunction() ______________________________________________________

void WgPackList::setSortFunction( WgWidgetSortFunc pSortFunc )
{
	if( pSortFunc != m_pSortFunc )
	{
		m_pSortFunc = pSortFunc;
		_sortEntries();
	}
}

//____ preferredSize() ________________________________________________________

WgSize WgPackList::preferredSize() const
{
	WgSize sz;
	if( m_bHorizontal )
	{
		sz =  WgSize(m_contentPreferredLength + m_header.m_width, m_contentPreferredBreadth);
		if( m_header.m_height > sz.h )
			sz.h = m_header.m_height;
	}
	else
	{
		sz = WgSize(m_contentPreferredBreadth,m_contentPreferredLength + m_header.m_height );
		if( m_header.m_width > sz.w )
			sz.w = m_header.m_width;
	}

	if( m_pSkin )
		sz += m_pSkin->contentPadding();

	return sz;
}

//____ matchingHeight() _______________________________________________________

int WgPackList::matchingHeight( int width ) const
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
			WgSize pad = m_pSkin->contentPadding();
			width -= pad.w;
			height += pad.h;
		}
		width -= m_entryPadding.w;

		for( int i = 0 ; i < m_hooks.size() ; i++ )
		{
			WgPackListHook * pHook = m_hooks.hook(i);
			height += pHook->_widget()->matchingHeight(width);
		}
		height += m_entryPadding.h*m_hooks.size();		
		return height;
	}
}

//____ matchingWidth() _______________________________________________________

int WgPackList::matchingWidth( int height ) const
{
	if( m_bHorizontal )
	{
		int width = m_header.m_width;
		if( m_pSkin )
		{
			WgSize pad = m_pSkin->contentPadding();
			height -= pad.w;
			width += pad.h;
		}
		height -= m_entryPadding.h;

		for( int i = 0 ; i < m_hooks.size() ; i++ )
		{
			WgPackListHook * pHook = m_hooks.hook(i);
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

bool WgPackList::setMinEntrySize( WgSize min )
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

bool WgPackList::setMaxEntrySize( WgSize max )
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

void WgPackList::_onCollectPatches( WgPatches& container, const WgRect& geo, const WgRect& clip )
{
	if( m_pSkin )
		container.add( WgRect( geo, clip ) );
	else
	{
		if( m_bHorizontal )
			container.add( WgRect( WgRect( geo.x, geo.y, WgMin(geo.w,m_contentLength), geo.h ), clip ) );
		else
			container.add( WgRect( WgRect( geo.x, geo.y, geo.w, WgMin(geo.h,m_contentLength) ), clip ) );
	}
}

//____ _onMaskPatches() _______________________________________________________

void WgPackList::_onMaskPatches( WgPatches& patches, const WgRect& geo, const WgRect& clip, WgBlendMode blendMode )
{
	if( (m_bOpaque && blendMode == WG_BLENDMODE_BLEND) || blendMode == WG_BLENDMODE_OPAQUE)
		patches.sub( WgRect(geo,clip) );
	else if( m_bOpaqueEntries && blendMode == WG_BLENDMODE_BLEND )
	{
		if( m_bHorizontal )
			patches.sub( WgRect( WgRect( geo.x, geo.y, WgMin(geo.w,m_contentLength), geo.h ), clip ) );
		else
			patches.sub( WgRect( WgRect( geo.x, geo.y, geo.w, WgMin(geo.h,m_contentLength) ), clip ) );
	}
	else
	{
		WgRect childGeo;
		WgPackListHook * p = static_cast<WgPackListHook*>(_firstHookWithGeo( childGeo ));

		while(p)
		{
			if( p->_isVisible() )
				p->_widget()->_onMaskPatches( patches, childGeo + geo.pos(), clip, blendMode );
			p = static_cast<WgPackListHook*>(_nextHookWithGeo( childGeo, p ));
		}
	}

}

//____ _onCloneContent() ______________________________________________________

void WgPackList::_onCloneContent( const WgWidget * _pOrg )
{
	//TODO: Implement!!!
}

//____ _renderPatches() _______________________________________________________

void WgPackList::_renderPatches( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, WgPatches * _pPatches )
{
	// We start by eliminating dirt outside our geometry

	WgPatches 	patches( _pPatches->size() );								// TODO: Optimize by pre-allocating?

	for( const WgRect * pRect = _pPatches->begin() ; pRect != _pPatches->end() ; pRect++ )
	{
		if( _canvas.intersectsWith( *pRect ) )
			patches.push( WgRect(*pRect,_canvas) );
	}

	// Render container itself
	
	for( const WgRect * pRect = patches.begin() ; pRect != patches.end() ; pRect++ )
		_onRender(pDevice, _canvas, _window, *pRect );
		
	
	// Render children

	WgRect	dirtBounds = patches.getUnion();
	
	{
		WgRect childGeo;
		WgPackListHook * p = (WgPackListHook*)_firstHookWithGeo( childGeo );

		while(p)
		{
			WgRect canvas = childGeo + _canvas.pos();
			if( p->_isVisible() && canvas.intersectsWith( dirtBounds ) )
				p->_widget()->_renderPatches( pDevice, canvas, canvas, &patches );
			p = (WgPackListHook*) _nextHookWithGeo( childGeo, p );
		}
	}

	// Render header

	if( m_header.m_height != 0 )
	{
		bool bInvertedSort = (m_sortOrder == WG_SORT_DESCENDING);
		WgRect canvas = _headerGeo() + _canvas.pos();

		for( const WgRect * pRect = patches.begin() ; pRect != patches.end() ; pRect++ )
			_renderHeader( pDevice, canvas, *pRect, m_header.m_pSkin, &m_header.label, &m_header.icon, &m_header.arrow, m_header.m_state, true, bInvertedSort );
	}

	// Render Lasso

	if( m_pLassoSkin && m_lassoBegin != m_lassoEnd )
	{
		WgRect lasso( m_lassoBegin, m_lassoEnd );
		lasso += _canvas.pos();


		for( const WgRect * pRect = patches.begin() ; pRect != patches.end() ; pRect++ )
			m_pLassoSkin->render( pDevice, lasso, m_state, WgRect( lasso, *pRect ) );
	}
}


//____ _onRender() ____________________________________________________________

void WgPackList::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip )
{
	WgRect contentRect = _listCanvas() + _canvas.pos();

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
		WgPackListHook * pHook = m_hooks.hook(i);
		WgWidget * pChild = pHook->_widget();

		// Get entry geometry, skin and state

		WgRect entryGeo( contentRect );
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
		
		WgSkin * pEntrySkin	= m_pEntrySkin[i&0x1].rawPtr();
		WgState	state		= pChild->state();
//		WgRect	childGeo( entryGeo );

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

void WgPackList::_onNewSize( const WgSize& size )
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
			WgPackListHook * pHook = m_hooks.hook(i);
			WgWidget * pWidget = pHook->_widget();

			if( m_bHorizontal )
			{
				int newEntryLength = _paddedLimitedMatchingWidth(pWidget, newContentBreadth );
				pHook->m_ofs = ofs;
				pHook->m_length = newEntryLength;
				ofs += newEntryLength;

				pWidget->_onNewSize( WgSize(newEntryLength, newContentBreadth) );				//TODO: Should be able to do a _onNewSize() that prevents child from doing a _requestRender().
			}
			else
			{
				int newEntryLength = _paddedLimitedMatchingHeight(pWidget, newContentBreadth );
				pHook->m_ofs = ofs;
				pHook->m_length = newEntryLength;
				ofs += newEntryLength;

				pWidget->_onNewSize( WgSize(newContentBreadth, newEntryLength) );				//TODO: Should be able to do a _onNewSize() that prevents child from doing a _requestRender().
			}
		}
		m_contentLength = ofs;
	}

	_requestRender();
}

//____ _onRefresh() ___________________________________________________________

void WgPackList::_onRefresh()
{
	_refreshHeader();
	_onRefreshList();	
	WgWidget::_onRefresh();
}

//____ _onRefreshList() _______________________________________________________

void WgPackList::_onRefreshList()
{
	if( m_pEntrySkin[0] )
		m_entryPadding = m_pEntrySkin[0]->contentPadding();

	m_contentPreferredLength = 0;
	m_contentPreferredBreadth = 0;
	m_nbPreferredBreadthEntries = 0;
	int ofs = 0;

	for( int entry = 0 ; entry < m_hooks.size() ; entry++ )
	{
		WgPackListHook * pHook = m_hooks.hook(entry);
		WgWidget * pChild = pHook->_widget();

		WgSize pref = _paddedLimitedPreferredSize( pChild );

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

void WgPackList::_onMsg( const WgMsgPtr& _pMsg )
{
	WgState oldState = m_state;

	switch( _pMsg->type() )
	{
		case WG_MSG_MOUSE_MOVE:
		{
			WgMouseMoveMsgPtr pMsg = WgMouseMoveMsg::cast(_pMsg);
			WgCoord ofs = ToLocal(pMsg->pointerPos());
			WgRect headerGeo = _headerGeo();
			bool bHeaderHovered = headerGeo.contains(ofs) && (!WgBase::msgRouter()->isAnyMouseButtonPressed() || 
															 (WgBase::msgRouter()->isMouseButtonPressed(WG_BUTTON_LEFT) && m_header.m_bPressed));
			if( bHeaderHovered != m_header.m_state.isHovered() )
			{
				m_header.m_state.setHovered(bHeaderHovered);
				_requestRender( headerGeo );
			}
			WgList::_onMsg( _pMsg );
			break;
		}

		case WG_MSG_MOUSE_LEAVE:
		{
			WgMouseLeaveMsgPtr pMsg = WgMouseLeaveMsg::cast(_pMsg);
			if( pMsg->source() == this && m_header.m_state.isHovered() )
			{
				m_header.m_state.setPressed(false);
				m_header.m_state.setHovered(false);
				_requestRender( _headerGeo() );
			}
			WgList::_onMsg( _pMsg );
			break;
		}

		case WG_MSG_MOUSE_PRESS:
		{
			WgMousePressMsgPtr pMsg = WgMousePressMsg::cast(_pMsg);
			WgCoord ofs = ToLocal(pMsg->pointerPos());
			WgRect headerGeo = _headerGeo();
			if(pMsg->button() == WG_BUTTON_LEFT && headerGeo.contains(ofs))
			{
				m_header.m_bPressed = true;
				m_header.m_state.setPressed(true);
				_requestRender( headerGeo );
				pMsg->swallow();
			}
			else
				WgList::_onMsg( _pMsg );
			break;
		}

		case WG_MSG_MOUSE_DRAG:
		{
			WgMouseDragMsgPtr pMsg = WgMouseDragMsg::cast(_pMsg);
			if( m_header.m_bPressed )
			{
				WgCoord ofs = ToLocal(pMsg->pointerPos());
				WgRect headerGeo = _headerGeo();
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
				WgList::_onMsg( _pMsg );
			break;
		}

		case WG_MSG_MOUSE_RELEASE:
		{
			WgMouseReleaseMsgPtr pMsg = WgMouseReleaseMsg::cast(_pMsg);
			if(pMsg->button() == WG_BUTTON_LEFT && m_header.m_bPressed )
			{
				m_header.m_bPressed = false;
				m_header.m_state.setPressed(false);
				WgRect headerGeo = _headerGeo();
				_requestRender( headerGeo );

				WgCoord ofs = ToLocal(pMsg->pointerPos());
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
				WgList::_onMsg( _pMsg );
			break;
		}
		case WG_MSG_KEY_PRESS:
		{
			if( m_selectMode == WG_SELECT_NONE )
				break;

			int				keyCode = WgKeyPressMsg::cast(_pMsg)->translatedKeyCode();
			WgModifierKeys	modKeys = WgKeyPressMsg::cast(_pMsg)->modKeys();
			if( (m_bHorizontal && (keyCode == WG_KEY_LEFT || keyCode == WG_KEY_RIGHT)) || 
				(!m_bHorizontal && (keyCode == WG_KEY_UP || keyCode == WG_KEY_DOWN || keyCode == WG_KEY_PAGE_UP || keyCode == WG_KEY_PAGE_DOWN)) ||
				keyCode == WG_KEY_HOME || keyCode == WG_KEY_END ||
				(m_selectMode == WG_SELECT_FLIP && keyCode == WG_KEY_SPACE ) )
					_pMsg->swallow();
			WgList::_onMsg( _pMsg );
			break;
		}

		case WG_MSG_KEY_REPEAT:
		case WG_MSG_KEY_RELEASE:
		{
			if( m_selectMode == WG_SELECT_NONE )
				break;

			int				keyCode = WgKeyMsg::cast(_pMsg)->translatedKeyCode();
			WgModifierKeys	modKeys = WgKeyMsg::cast(_pMsg)->modKeys();
			if( (m_bHorizontal && (keyCode == WG_KEY_LEFT || keyCode == WG_KEY_RIGHT)) || 
				(!m_bHorizontal && (keyCode == WG_KEY_UP || keyCode == WG_KEY_DOWN || keyCode == WG_KEY_PAGE_UP || keyCode == WG_KEY_PAGE_DOWN)) ||
				keyCode == WG_KEY_HOME || keyCode == WG_KEY_END ||
				(m_selectMode == WG_SELECT_FLIP && keyCode == WG_KEY_SPACE ) )
					_pMsg->swallow();
			WgList::_onMsg( _pMsg );
			break;
		}
	
		default:
			WgList::_onMsg(_pMsg);
			return;
	}

	if( m_state != oldState )
		_onStateChanged(oldState);
}

//____ _onLassoUpdated() ______________________________________________________

void WgPackList::_onLassoUpdated( const WgRect& oldLasso, const WgRect& newLasso )
{
	// Get out content area

	WgRect listArea = _listArea();

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

void WgPackList::_onStateChanged( WgState oldState )
{
	WgList::_onStateChanged(oldState);
}


//____ _onRequestRender() _____________________________________________________

void WgPackList::_onRequestRender( WgPackListHook * pHook )
{
	WgRect geo;
	_getChildGeo(geo, pHook);
	_requestRender(geo);
}

void WgPackList::_onRequestRender( WgPackListHook * pHook, const WgRect& rect )
{
	WgRect geo;
	_getChildGeo(geo, pHook);
	geo.x += rect.x;
	geo.y += rect.y;
	geo.w = rect.w;
	geo.h = rect.h;
	_requestRender(geo);
}

//____ _onRequestResize() _____________________________________________________

void WgPackList::_onRequestResize( WgPackListHook * pHook )
{
	if( !pHook->m_bVisible  || m_minEntrySize == m_maxEntrySize )
		return;

	WgWidget * pChild = pHook->_widget();
	WgSize prefEntrySize = _paddedLimitedPreferredSize(pChild);

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

		WgRect childGeo;
		_getChildGeo(childGeo,pHook);
		pHook->_widget()->_onNewSize(childGeo);
	}



	if( bReqResize )
		_requestResize();
}

//____ _onWidgetAppeared() ____________________________________________________

void WgPackList::_onWidgetAppeared( WgListHook * pInserted )
{
	WgPackListHook * pHook = static_cast<WgPackListHook*>(pInserted);
	WgWidget * pChild = pHook->_widget();

	WgSize pref = _paddedLimitedPreferredSize( pChild );

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

	WgRect childGeo;
	_getChildGeo(childGeo,pHook);
	static_cast<WgPackListHook*>(pInserted)->_widget()->_onNewSize( childGeo );


	// Finish up

	_updateChildOfsFrom( pHook );
	_requestRenderChildrenFrom( pHook );	// Request render on dirty area
	_requestResize();						// This should preferably be done first once we have changed the method.

}

//____ _onWidgetDisappeared() _________________________________________________

void WgPackList::_onWidgetDisappeared( WgListHook * pToBeRemoved )
{
	WgPackListHook * pHook = static_cast<WgPackListHook*>(pToBeRemoved);
	WgWidget * pChild = pHook->_widget();

	WgSize pref = _paddedLimitedPreferredSize( pChild );

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

int WgPackList::_getEntryAt( int pixelofs ) const
{
	int first = 0;
	int last = m_hooks.size() - 1;
	int middle = (first+last)/2;
 
	while( first <= last )
	{
		WgPackListHook * pHook = m_hooks.hook(middle);

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

int WgPackList::_getInsertionPoint( const WgWidget * pWidget ) const
{
	int first = 0;
	int last = m_hooks.size() - 1;
	int middle = (first+last)/2;
 	int negator = m_sortOrder == WG_SORT_ASCENDING ? 1 : -1;

	while( first <= last )
	{
		WgPackListHook * pHook = m_hooks.hook(middle);

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

WgListHook * WgPackList::_findEntry( const WgCoord& ofs )
{
	WgWidget * pResult = 0;
	WgRect list = _listArea();

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

WgWidget * WgPackList::_findWidget( const WgCoord& ofs, WgSearchMode mode )
{
	WgWidget * pResult = 0;
	WgRect list = _listArea();

	if( list.contains(ofs) && _listWindow().contains(ofs) )
	{
		int entry;
		if( m_bHorizontal )
			entry = _getEntryAt(ofs.x-list.x);
		else
			entry = _getEntryAt(ofs.y-list.y);

		if( entry != m_hooks.size() )
		{
			WgPackListHook * pHook = m_hooks.hook(entry);
			WgRect childGeo;
			_getChildGeo( childGeo, pHook );
			if( childGeo.contains(ofs) )
			{
				if( pHook->_widget()->IsContainer() )
				{
					pResult = static_cast<WgContainer*>(pHook->_widget())->_findWidget( ofs - childGeo.pos(), mode );
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

void  WgPackList::_addToContentPreferredSize( int length, int breadth )
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

void  WgPackList::_subFromContentPreferredSize( int length, int breadth )
{
	m_contentPreferredLength -= length;
	
	if( breadth == m_contentPreferredBreadth )
	{
		m_nbPreferredBreadthEntries--;
		if( m_nbPreferredBreadthEntries == 0 )
		{
			int highest = 0;
			for( WgPackListHook * p = m_hooks.begin() ; p < m_hooks.end() ; p++ )
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

void WgPackList::_requestRenderChildrenFrom( WgPackListHook * pHook )
{
	WgRect box = _listArea();

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

void WgPackList::_updateChildOfsFrom( WgPackListHook * pHook )
{
	int ofs = 0;
	WgPackListHook * pPrev = m_hooks.prev(pHook);
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

void WgPackList::_onEntrySkinChanged( WgSize oldPadding, WgSize newPadding )
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

void WgPackList::_getEntryGeo( WgRect& geo, const WgListHook * _pHook ) const
{
	const WgPackListHook * pHook = static_cast<const WgPackListHook*>(_pHook);

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

void WgPackList::_getChildGeo( WgRect& geo, const WgPackListHook * pHook ) const
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

int WgPackList::_paddedLimitedMatchingHeight( WgWidget * pChild, int paddedWidth )
{
	int height = pChild->matchingHeight( paddedWidth - m_entryPadding.w ) + m_entryPadding.h;
	LIMIT( height, m_minEntrySize.h, m_maxEntrySize.h );
	return height;
}

//____ _paddedLimitedMatchingWidth() _________________________________________

int WgPackList::_paddedLimitedMatchingWidth( WgWidget * pChild, int paddedHeight )
{
	int width = pChild->matchingWidth( paddedHeight - m_entryPadding.h ) + m_entryPadding.w;
	LIMIT( width, m_minEntrySize.w, m_maxEntrySize.w );
	return width;
}

//____ _paddedLimitedPreferredSize() __________________________________________

WgSize WgPackList::_paddedLimitedPreferredSize( WgWidget * pChild )
{
	WgSize sz = pChild->preferredSize();
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

WgHook* WgPackList::_firstHook() const
{
	if( m_hooks.size() > 0 )
		return m_hooks.hook(0);

	return 0;
}

//____ _lastHook() ____________________________________________________________

WgHook* WgPackList::_lastHook() const
{
	if( m_hooks.size() > 0 )
		return m_hooks.hook(m_hooks.size()-1);

	return 0;
}

//____ _firstHookWithGeo() ____________________________________________________

WgHook* WgPackList::_firstHookWithGeo( WgRect& geo ) const
{
	if( m_hooks.size() == 0 )
		return 0;

	WgPackListHook * p = m_hooks.hook(0);
	_getChildGeo(geo,p);
	return p;
}

//____ _nextHookWithGeo() _____________________________________________________

WgHook* WgPackList::_nextHookWithGeo( WgRect& geo, WgHook * pHook ) const
{
	WgPackListHook * p = m_hooks.next(static_cast<WgPackListHook*>(pHook));
	if( p )
		_getChildGeo(geo,p);
	return p;
}

//_____ _lastHookWithGeo() ____________________________________________________

WgHook* WgPackList::_lastHookWithGeo( WgRect& geo ) const
{
	if( m_hooks.size() == 0 )
		return 0;

	WgPackListHook * p = m_hooks.hook(m_hooks.size()-1);
	_getChildGeo(geo,p);
	return p;
}

//____ _prevHookWithGeo() _____________________________________________________

WgHook* WgPackList::_prevHookWithGeo( WgRect& geo, WgHook * pHook ) const
{
	WgPackListHook * p = m_hooks.prev(static_cast<WgPackListHook*>(pHook));
	if( p )
		_getChildGeo(geo,p);
	return p;
}

//____ _listArea() ____________________________________________________________

WgRect WgPackList::_listArea() const
{
	WgRect r = _listCanvas();
	if( m_pSkin )
		r = m_pSkin->contentRect( r, m_state );

	return r;
}

//____ _listWindow() ____________________________________________________________

WgRect WgPackList::_listWindow() const
{
	WgRect r = _windowSection();		// Window in widgets own coordinate system.

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

WgRect WgPackList::_listCanvas() const
{
	if( m_bHorizontal )
		return WgRect(m_header.m_width, 0, m_size.w - m_header.m_width, m_size.h );
	else
		return WgRect(0, m_header.m_height, m_size.w, m_size.h - m_header.m_height);	// List canvas in widgets own coordinate system.
}

//____ _headerGeo() ___________________________________________________________

WgRect WgPackList::_headerGeo() const
{
	if( m_bHorizontal )
		return WgRect( _windowSection().x, 0, m_header.m_width, m_size.h );
	else
		return WgRect( 0, _windowSection().y, m_size.w, m_header.m_height );
}

//____ _windowPadding() _______________________________________________________

WgSize WgPackList::_windowPadding() const
{
	if( m_bHorizontal )
		return WgSize( m_header.m_width, 0 );
	else
		return WgSize( 0, m_header.m_height );
}

//____ _refreshHeader() _______________________________________________________

void WgPackList::_refreshHeader()
{
	WgSize wantedIconSize = m_header.icon.preferredSize();
	WgSize wantedArrowSize = m_header.arrow.preferredSize();
	WgSize wantedTextSize = m_header.label.preferredSize();

	WgSize wantedSize;

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

bool WgPackList::_sortEntries()
{
	if( !m_pSortFunc )
		return false;

	if( m_hooks.isEmpty() )
		return true;

	// Create a temporary, sorted list of pointers to hooks

	int listSize = sizeof(int) * m_hooks.size();
	int * pOrderList = (int*) WgBase::memStackAlloc( listSize );
	int negator = m_sortOrder == WG_SORT_ASCENDING ? 1 : -1;

	pOrderList[0] = 0;
	for( int entry = 1 ; entry < m_hooks.size() ; entry++ )
	{
		WgWidget * pWidget = m_hooks.hook(entry)->_widget();

		int first = 0;
		int last = entry-1;
		int middle = (first+last)/2;
 
		while( first <= last )
		{
			WgWidget * pEntry = m_hooks.hook(pOrderList[middle])->_widget();

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
	WgBase::memStackRelease( listSize );

	// Update m_ofs in the hooks

	_updateChildOfsFrom( m_hooks.begin() );
	_requestRenderChildrenFrom( m_hooks.begin() );	// Request render on dirty area
	return true;
}

//____ _onFieldDirty() _________________________________________________________

void WgPackList::_onFieldDirty(WgField * pField)
{
	_refreshHeader();	//TODO: Just request render on header.
}

//____ _onFieldResize() ________________________________________________________

void WgPackList::_onFieldResize(WgField * pField)
{
	_refreshHeader();
}


