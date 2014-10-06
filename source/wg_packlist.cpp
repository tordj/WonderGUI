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
#include <wg_eventhandler.h>
#include <wg_base.h>

const char WgPackList::CLASSNAME[] = {"PackList"};
const char WgPackListHook::CLASSNAME[] = {"PackListHook"};


//____ WgPackListHook::IsInstanceOf() __________________________________________

bool WgPackListHook::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgListHook::IsInstanceOf(pClassName);
}

//____ WgPackListHook::ClassName() _____________________________________________

const char * WgPackListHook::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ WgPackListHook::Cast() ________________________________________________

WgPackListHookPtr WgPackListHook::Cast( const WgHookPtr& pHook )
{
	if( pHook && pHook->IsInstanceOf(CLASSNAME) )
		return WgPackListHookPtr( static_cast<WgPackListHook*>(pHook.GetRealPtr()) );

	return 0;
}

//____ WqPackListHook::Pos() _________________________________________________

WgCoord WgPackListHook::Pos() const
{
	WgRect	geo;
	m_pParent->_getChildGeo(geo,this);
	return geo.Pos();
}

WgSize WgPackListHook::Size() const
{
	WgRect	geo;
	m_pParent->_getChildGeo(geo,this);
	return geo.Size();
}

WgRect WgPackListHook::Geo() const
{
	WgRect	geo;
	m_pParent->_getChildGeo(geo,this);
	return geo;
}

WgCoord WgPackListHook::GlobalPos() const
{
	WgRect	geo;
	m_pParent->_getChildGeo(geo,this);
	return m_pParent->GlobalPos() + geo.Pos();
}

WgRect WgPackListHook::GlobalGeo() const
{
	WgRect	geo;
	m_pParent->_getChildGeo(geo,this);
	return geo + GlobalPos();
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
	return m_pParent->m_hooks.Prev(this);
}

WgHook *  WgPackListHook::_nextHook() const
{
	return m_pParent->m_hooks.Next(this);
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

	m_pHoveredChild = 0;
}

//____ Destructor _____________________________________________________________

WgPackList::~WgPackList()
{
}

//____ IsInstanceOf() _________________________________________________________

bool WgPackList::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgList::IsInstanceOf(pClassName);
}

//____ ClassName() ____________________________________________________________

const char * WgPackList::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ Cast() _________________________________________________________________

WgPackListPtr WgPackList::Cast( const WgObjectPtr& pObj )
{
	if( pObj && pObj->IsInstanceOf(CLASSNAME) )
		return WgPackListPtr( static_cast<WgPackList*>(pObj.GetRealPtr()) );

	return 0;
}

//____ AddWidget() ____________________________________________________________

WgPackListHookPtr WgPackList::AddWidget( const WgWidgetPtr& pWidget )
{
	if( !pWidget )
		return 0;

	WgPackListHook * pHook = m_hooks.Add();
	pHook->m_pParent = this;
	pHook->_setWidget(pWidget.GetRealPtr());

	_onWidgetAppeared( pHook );
	return pHook;
}


//____ InsertWidget() ____________________________________________________________

WgPackListHookPtr WgPackList::InsertWidget( const WgWidgetPtr& pWidget, const WgWidgetPtr& pSibling )
{
	if( !pWidget )
		return 0;

	int index = 0;
	if( !pSibling )
		index = m_hooks.Size();
	else 
	{
		WgPackListHook * pHook = static_cast<WgPackListHook*>(pSibling->_hook());
		if( pHook && pHook->_parent() == this )
			index = m_hooks.Index(pHook);
		else
			return 0;
	}

	WgPackListHook * pHook = m_hooks.Insert(index);
	pHook->m_pParent = this;
	pHook->_setWidget(pWidget.GetRealPtr());

	_onWidgetAppeared( pHook );
	return pHook;
}

//____ InsertWidgetSorted() ___________________________________________________

WgPackListHookPtr WgPackList::InsertWidgetSorted( const WgWidgetPtr& pWidget )
{
	if( !pWidget )
		return 0;

	if( m_hooks.IsEmpty() || !m_pSortFunc )
		return AddWidget( pWidget );

	int index = _getInsertionPoint( pWidget.GetRealPtr() );

	WgPackListHook * pHook = m_hooks.Insert(index);
	pHook->m_pParent = this;
	pHook->_setWidget(pWidget.GetRealPtr());

	_onWidgetAppeared( pHook );
	return pHook;
}

//____ RemoveWidget() _________________________________________________________

bool WgPackList::RemoveWidget( const WgWidgetPtr& pWidget )
{
	if( !pWidget || !pWidget->_hook() )
		return false;

	WgPackListHook * pHook = static_cast<WgPackListHook*>(pWidget->_hook());
	if( pHook->_parent() != this )
		return false;

	int index = m_hooks.Index(pHook);
	_onWidgetDisappeared( pHook );
	m_hooks.Remove(index);
	return true;
}

//____ Clear() ________________________________________________________________

bool WgPackList::Clear()
{
	m_hooks.Clear();
	_onRefreshList();
	return true;
}

//____ SetOrientation() _______________________________________________________

void WgPackList::SetOrientation( WgOrientation orientation )
{
	bool bHorizontal = (orientation == WG_HORIZONTAL);

	if( bHorizontal != m_bHorizontal )
	{
		m_bHorizontal = bHorizontal;
		_onRefreshList();
	}
}

//____ SortWidgets() __________________________________________________________

void WgPackList::SortWidgets()
{
	_sortEntries();
}

//____ SetSortOrder() _________________________________________________________

void WgPackList::SetSortOrder( WgSortOrder order )
{
	if( order != m_sortOrder )
	{
		m_sortOrder = order;
		_sortEntries();
		_requestRender();		// So we also render the header, which has an arrow with new state.
	}
}

//____ SetSortFunction() ______________________________________________________

void WgPackList::SetSortFunction( WgWidgetSortFunc pSortFunc )
{
	if( pSortFunc != m_pSortFunc )
	{
		m_pSortFunc = pSortFunc;
		_sortEntries();
	}
}

//____ PreferredSize() ________________________________________________________

WgSize WgPackList::PreferredSize() const
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
		sz += m_pSkin->ContentPadding();

	return sz;
}

//____ MatchingHeight() _______________________________________________________

int WgPackList::MatchingHeight( int width ) const
{
	if( m_bHorizontal )
	{
		int height =  m_contentPreferredBreadth;
		if( m_header.m_height > height )
			height = m_header.m_height;

		if( m_pSkin )
			height += m_pSkin->ContentPadding().h;
		return height;
	}
	else
	{
		int height = m_header.m_height;
		if( m_pSkin )
		{
			WgSize pad = m_pSkin->ContentPadding();
			width -= pad.w;
			height += pad.h;
		}
		width -= m_entryPadding.w;

		for( int i = 0 ; i < m_hooks.Size() ; i++ )
		{
			WgPackListHook * pHook = m_hooks.Hook(i);
			height += pHook->_widget()->MatchingHeight(width);
		}
		height += m_entryPadding.h*m_hooks.Size();		
		return height;
	}
}

//____ MatchingWidth() _______________________________________________________

int WgPackList::MatchingWidth( int height ) const
{
	if( m_bHorizontal )
	{
		int width = m_header.m_width;
		if( m_pSkin )
		{
			WgSize pad = m_pSkin->ContentPadding();
			height -= pad.w;
			width += pad.h;
		}
		height -= m_entryPadding.h;

		for( int i = 0 ; i < m_hooks.Size() ; i++ )
		{
			WgPackListHook * pHook = m_hooks.Hook(i);
			width += pHook->_widget()->MatchingWidth(height);
		}
		width += m_entryPadding.w*m_hooks.Size();		
		return width;
	}
	else
	{
		int width =  m_contentPreferredBreadth;
		if( m_header.m_width > width )
			width = m_header.m_width;

		if( m_pSkin )
			width += m_pSkin->ContentPadding().w;
		return width;
	}
}


//____ SetMinEntrySize() ______________________________________________________

bool WgPackList::SetMinEntrySize( WgSize min )
{
	if( min == m_minEntrySize )
		return true;

	if( min.w > m_maxEntrySize.w || min.h > m_maxEntrySize.h )
		return false;

	m_minEntrySize = min;
	_onRefreshList();
	return true;
}

//____ SetMaxEntrySize() ______________________________________________________

bool WgPackList::SetMaxEntrySize( WgSize max )
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
		container.Add( WgRect( geo, clip ) );
	else
	{
		if( m_bHorizontal )
			container.Add( WgRect( WgRect( geo.x, geo.y, WgMin(geo.w,m_contentLength), geo.h ), clip ) );
		else
			container.Add( WgRect( WgRect( geo.x, geo.y, geo.w, WgMin(geo.h,m_contentLength) ), clip ) );
	}
}

//____ _onMaskPatches() _______________________________________________________

void WgPackList::_onMaskPatches( WgPatches& patches, const WgRect& geo, const WgRect& clip, WgBlendMode blendMode )
{
	if( (m_bOpaque && blendMode == WG_BLENDMODE_BLEND) || blendMode == WG_BLENDMODE_OPAQUE)
		patches.Sub( WgRect(geo,clip) );
	else if( m_bOpaqueEntries && blendMode == WG_BLENDMODE_BLEND )
	{
		if( m_bHorizontal )
			patches.Sub( WgRect( WgRect( geo.x, geo.y, WgMin(geo.w,m_contentLength), geo.h ), clip ) );
		else
			patches.Sub( WgRect( WgRect( geo.x, geo.y, geo.w, WgMin(geo.h,m_contentLength) ), clip ) );
	}
	else
	{
		WgRect childGeo;
		WgPackListHook * p = static_cast<WgPackListHook*>(_firstHookWithGeo( childGeo ));

		while(p)
		{
			if( p->_isVisible() )
				p->_widget()->_onMaskPatches( patches, childGeo + geo.Pos(), clip, blendMode );
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

	WgPatches 	patches( _pPatches->Size() );								// TODO: Optimize by pre-allocating?

	for( const WgRect * pRect = _pPatches->Begin() ; pRect != _pPatches->End() ; pRect++ )
	{
		if( _canvas.IntersectsWith( *pRect ) )
			patches.Push( WgRect(*pRect,_canvas) );
	}

	// Render container itself
	
	for( const WgRect * pRect = patches.Begin() ; pRect != patches.End() ; pRect++ )
		_onRender(pDevice, _canvas, _window, *pRect );
		
	
	// Render children

	WgRect	dirtBounds = patches.Union();
	
	{
		WgRect childGeo;
		WgPackListHook * p = (WgPackListHook*)_firstHookWithGeo( childGeo );

		while(p)
		{
			WgRect canvas = childGeo + _canvas.Pos();
			if( p->_isVisible() && canvas.IntersectsWith( dirtBounds ) )
				p->_widget()->_renderPatches( pDevice, canvas, canvas, &patches );
			p = (WgPackListHook*) _nextHookWithGeo( childGeo, p );
		}
	}

	// Render header

	if( m_header.m_height != 0 )
	{
		bool bInvertedSort = (m_sortOrder == WG_SORT_DESCENDING);
		WgRect canvas = _headerGeo() + _canvas.Pos();

		for( const WgRect * pRect = patches.Begin() ; pRect != patches.End() ; pRect++ )
			_renderHeader( pDevice, canvas, *pRect, m_header.m_pSkin, &m_header.label, &m_header.icon, &m_header.arrow, m_header.m_state, true, bInvertedSort );
	}

	// Render Lasso

	if( m_pLassoSkin && m_lassoBegin != m_lassoEnd )
	{
		WgRect lasso( m_lassoBegin, m_lassoEnd );
		lasso += _canvas.Pos();


		for( const WgRect * pRect = patches.Begin() ; pRect != patches.End() ; pRect++ )
			m_pLassoSkin->Render( pDevice, lasso, m_state, WgRect( lasso, *pRect ) );
	}
}


//____ _onRender() ____________________________________________________________

void WgPackList::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip )
{
	WgRect contentRect = _listCanvas() + _canvas.Pos();

	if( m_pSkin )
	{
		m_pSkin->Render( pDevice, contentRect, m_state, _clip );
		contentRect = m_pSkin->ContentRect( contentRect, m_state );
	}

	int startOfs = m_bHorizontal ? _clip.x-contentRect.x : _clip.y-contentRect.y;
	if( startOfs < 0 )
		startOfs = 0;

	for( int i = _getEntryAt( startOfs ) ; i < m_hooks.Size() ; i++ )
	{
		WgPackListHook * pHook = m_hooks.Hook(i);
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
		
		WgSkin * pEntrySkin	= m_pEntrySkin[i&0x1].GetRealPtr();
		WgState	state		= pChild->State();
//		WgRect	childGeo( entryGeo );

		// Render entry skin, shrink child geo

		if( pEntrySkin )
		{
			pEntrySkin->Render( pDevice, entryGeo, state, _clip );
//			childGeo = pEntrySkin->ContentRect( entryGeo, state );
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

		for( int i = 0 ; i < m_hooks.Size() ; i++ )
		{
			WgPackListHook * pHook = m_hooks.Hook(i);
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
		m_entryPadding = m_pEntrySkin[0]->ContentPadding();

	m_contentPreferredLength = 0;
	m_contentPreferredBreadth = 0;
	m_nbPreferredBreadthEntries = 0;
	int ofs = 0;

	for( int entry = 0 ; entry < m_hooks.Size() ; entry++ )
	{
		WgPackListHook * pHook = m_hooks.Hook(entry);
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

//____ _onEvent() _____________________________________________________________

void WgPackList::_onEvent( const WgEventPtr& _pEvent, WgEventHandler * pHandler )
{
	WgState oldState = m_state;

	switch( _pEvent->Type() )
	{
		case WG_EVENT_MOUSE_MOVE:
		{
			WgMouseMoveEventPtr pEvent = WgMouseMoveEvent::Cast(_pEvent);
			WgCoord ofs = ToLocal(pEvent->PointerGlobalPos());
			WgRect headerGeo = _headerGeo();
			bool bHeaderHovered = headerGeo.Contains(ofs) && (!pHandler->IsAnyMouseButtonPressed() || (pHandler->IsMouseButtonPressed(WG_BUTTON_LEFT) && m_header.m_bPressed));
			if( bHeaderHovered != m_header.m_state.IsHovered() )
			{
				m_header.m_state.SetHovered(bHeaderHovered);
				_requestRender( headerGeo );
			}
			WgList::_onEvent( _pEvent, pHandler );
			break;
		}

		case WG_EVENT_MOUSE_LEAVE:
		{
			WgMouseLeaveEventPtr pEvent = WgMouseLeaveEvent::Cast(_pEvent);
			if( pEvent->Widget() == this && m_header.m_state.IsHovered() )
			{
				m_header.m_state.SetPressed(false);
				m_header.m_state.SetHovered(false);
				_requestRender( _headerGeo() );
			}
			WgList::_onEvent( _pEvent, pHandler );
			break;
		}

		case WG_EVENT_MOUSE_PRESS:
		{
			WgMousePressEventPtr pEvent = WgMousePressEvent::Cast(_pEvent);
			WgCoord ofs = ToLocal(pEvent->PointerGlobalPos());
			WgRect headerGeo = _headerGeo();
			if(pEvent->Button() == WG_BUTTON_LEFT && headerGeo.Contains(ofs))
			{
				m_header.m_bPressed = true;
				m_header.m_state.SetPressed(true);
				_requestRender( headerGeo );
				pHandler->SwallowEvent( pEvent );
			}
			else
				WgList::_onEvent( _pEvent, pHandler );
			break;
		}

		case WG_EVENT_MOUSE_DRAG:
		{
			WgMouseDragEventPtr pEvent = WgMouseDragEvent::Cast(_pEvent);
			if( m_header.m_bPressed )
			{
				WgCoord ofs = ToLocal(pEvent->PointerGlobalPos());
				WgRect headerGeo = _headerGeo();
				bool bHeaderHovered = headerGeo.Contains(ofs);
				if( bHeaderHovered != m_header.m_state.IsHovered() )
				{
					m_header.m_state.SetHovered(bHeaderHovered);
					m_header.m_state.SetPressed(bHeaderHovered);
					_requestRender( headerGeo );
				}
				pHandler->SwallowEvent(pEvent);
			}
			else
				WgList::_onEvent( _pEvent, pHandler );
			break;
		}

		case WG_EVENT_MOUSE_RELEASE:
		{
			WgMouseReleaseEventPtr pEvent = WgMouseReleaseEvent::Cast(_pEvent);
			if(pEvent->Button() == WG_BUTTON_LEFT && m_header.m_bPressed )
			{
				m_header.m_bPressed = false;
				m_header.m_state.SetPressed(false);
				WgRect headerGeo = _headerGeo();
				_requestRender( headerGeo );

				WgCoord ofs = ToLocal(pEvent->PointerGlobalPos());
				if( headerGeo.Contains(ofs) )
				{
					if( m_sortOrder == WG_SORT_ASCENDING )
						m_sortOrder = WG_SORT_DESCENDING;
					else
						m_sortOrder = WG_SORT_ASCENDING;
					_sortEntries();
				}
				pHandler->SwallowEvent(pEvent);
			}
			else
				WgList::_onEvent( _pEvent, pHandler );
			break;
		}
		case WG_EVENT_KEY_PRESS:
		{
			if( m_selectMode == WG_SELECT_NONE )
				break;

			int				keyCode = WgKeyPressEvent::Cast(_pEvent)->TranslatedKeyCode();
			WgModifierKeys	modKeys = WgKeyPressEvent::Cast(_pEvent)->ModKeys();
			if( (m_bHorizontal && (keyCode == WG_KEY_LEFT || keyCode == WG_KEY_RIGHT)) || 
				(!m_bHorizontal && (keyCode == WG_KEY_UP || keyCode == WG_KEY_DOWN || keyCode == WG_KEY_PAGE_UP || keyCode == WG_KEY_PAGE_DOWN)) ||
				keyCode == WG_KEY_HOME || keyCode == WG_KEY_END ||
				(m_selectMode == WG_SELECT_FLIP && keyCode == WG_KEY_SPACE ) )
					pHandler->SwallowEvent(_pEvent);
			WgList::_onEvent( _pEvent, pHandler );
			break;
		}

		case WG_EVENT_KEY_REPEAT:
		case WG_EVENT_KEY_RELEASE:
		{
			if( m_selectMode == WG_SELECT_NONE )
				break;

			int				keyCode = WgKeyEvent::Cast(_pEvent)->TranslatedKeyCode();
			WgModifierKeys	modKeys = WgKeyEvent::Cast(_pEvent)->ModKeys();
			if( (m_bHorizontal && (keyCode == WG_KEY_LEFT || keyCode == WG_KEY_RIGHT)) || 
				(!m_bHorizontal && (keyCode == WG_KEY_UP || keyCode == WG_KEY_DOWN || keyCode == WG_KEY_PAGE_UP || keyCode == WG_KEY_PAGE_DOWN)) ||
				keyCode == WG_KEY_HOME || keyCode == WG_KEY_END ||
				(m_selectMode == WG_SELECT_FLIP && keyCode == WG_KEY_SPACE ) )
					pHandler->SwallowEvent(_pEvent);
			WgList::_onEvent( _pEvent, pHandler );
			break;
		}
	
		default:
			WgList::_onEvent(_pEvent, pHandler);
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

	if( oldLasso.IntersectsWith(listArea ) )
		bOldLassoInside = true;
	if( newLasso.IntersectsWith(listArea ) )
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

		_flipRange( m_hooks.Hook(beg), m_hooks.Hook(end), true );
	}
	else
	{
		if( oldFirst != newFirst )
		{
			int beg = WgMin(oldFirst,newFirst);
			int end = WgMax(oldFirst,newFirst)-1;

			_flipRange( m_hooks.Hook(beg), m_hooks.Hook(end), true );
		}

		if( oldLast != newLast )
		{
			int beg = WgMin(oldLast,newLast)+1;
			int end = WgMax(oldLast,newLast);

			_flipRange( m_hooks.Hook(beg), m_hooks.Hook(end), true );
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

	_updateChildOfsFrom( m_hooks.Next(pHook) );
	_requestResize();
}

//____ _getEntryAt() __________________________________________________________

// Pixelofs is counted from beginning of container content, not widget.

int WgPackList::_getEntryAt( int pixelofs ) const
{
	int first = 0;
	int last = m_hooks.Size() - 1;
	int middle = (first+last)/2;
 
	while( first <= last )
	{
		WgPackListHook * pHook = m_hooks.Hook(middle);

		if( pHook->m_ofs + pHook->m_length < pixelofs )
			first = middle + 1;
		else if( pHook->m_ofs <= pixelofs ) 
			return middle;
		else
			last = middle - 1;
 
		middle = (first + last)/2;
	}

	return m_hooks.Size();
}

//____ _getInsertionPoint() ___________________________________________________

int WgPackList::_getInsertionPoint( const WgWidget * pWidget ) const
{
	int first = 0;
	int last = m_hooks.Size() - 1;
	int middle = (first+last)/2;
 	int negator = m_sortOrder == WG_SORT_ASCENDING ? 1 : -1;

	while( first <= last )
	{
		WgPackListHook * pHook = m_hooks.Hook(middle);

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

	if( list.Contains(ofs) && _listWindow().Contains(ofs) )
	{
		int entry;
		if( m_bHorizontal )
			entry = _getEntryAt(ofs.x-list.x);
		else
			entry = _getEntryAt(ofs.y-list.y);

		if( entry != m_hooks.Size() )
			return m_hooks.Hook(entry);
	}

	return 0;
}

//____ _findWidget() __________________________________________________________

WgWidget * WgPackList::_findWidget( const WgCoord& ofs, WgSearchMode mode )
{
	WgWidget * pResult = 0;
	WgRect list = _listArea();

	if( list.Contains(ofs) && _listWindow().Contains(ofs) )
	{
		int entry;
		if( m_bHorizontal )
			entry = _getEntryAt(ofs.x-list.x);
		else
			entry = _getEntryAt(ofs.y-list.y);

		if( entry != m_hooks.Size() )
		{
			WgPackListHook * pHook = m_hooks.Hook(entry);
			WgRect childGeo;
			_getChildGeo( childGeo, pHook );
			if( childGeo.Contains(ofs) )
			{
				if( pHook->_widget()->IsContainer() )
				{
					pResult = static_cast<WgContainer*>(pHook->_widget())->_findWidget( ofs - childGeo.Pos(), mode );
				}
				else if( mode == WG_SEARCH_GEOMETRY || pHook->_widget()->MarkTest( ofs - childGeo.Pos() ) )
				{
						pResult = pHook->_widget();
				}
			}

			if( !pResult && mode == WG_SEARCH_ACTION_TARGET )
				pResult = pHook->_widget();						// Entries are opaque as action targets.

		}
	}

	// Check against ourselves

	if( !pResult && ( mode == WG_SEARCH_GEOMETRY || MarkTest(ofs)) )
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
			for( WgPackListHook * p = m_hooks.Begin() ; p < m_hooks.End() ; p++ )
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
	WgPackListHook * pPrev = m_hooks.Prev(pHook);
	if( pPrev )
		ofs = pPrev->m_ofs + pPrev->m_length;

	while( pHook < m_hooks.End() )
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
		int nEntries = m_hooks.Size();

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
		int index = m_hooks.Index( pHook );
		if( m_pEntrySkin[index&0x1] )
			geo = m_pEntrySkin[index&0x1]->ContentRect( geo, pHook->_widget()->State() );
	}
}

//____ _paddedLimitedMatchingHeight() _________________________________________

int WgPackList::_paddedLimitedMatchingHeight( WgWidget * pChild, int paddedWidth )
{
	int height = pChild->MatchingHeight( paddedWidth - m_entryPadding.w ) + m_entryPadding.h;
	WG_LIMIT( height, m_minEntrySize.h, m_maxEntrySize.h );
	return height;
}

//____ _paddedLimitedMatchingWidth() _________________________________________

int WgPackList::_paddedLimitedMatchingWidth( WgWidget * pChild, int paddedHeight )
{
	int width = pChild->MatchingWidth( paddedHeight - m_entryPadding.h ) + m_entryPadding.w;
	WG_LIMIT( width, m_minEntrySize.w, m_maxEntrySize.w );
	return width;
}

//____ _paddedLimitedPreferredSize() __________________________________________

WgSize WgPackList::_paddedLimitedPreferredSize( WgWidget * pChild )
{
	WgSize sz = pChild->PreferredSize();
	sz += m_entryPadding;

	// Apply limits

	if( sz.w < m_minEntrySize.w )
		sz.w = m_minEntrySize.w;
	if( sz.h < m_minEntrySize.h )
		sz.h = m_minEntrySize.h;

	if( sz.w > m_maxEntrySize.w )
	{
		int h = pChild->MatchingHeight(m_maxEntrySize.w-m_entryPadding.w) + m_entryPadding.h;
		WG_LIMIT(h, m_minEntrySize.h, m_maxEntrySize.h );
	}
	else if( sz.h > m_maxEntrySize.h )
	{
		int w = pChild->MatchingWidth(m_maxEntrySize.h-m_entryPadding.h) + m_entryPadding.w;
		WG_LIMIT(w, m_minEntrySize.w, m_maxEntrySize.w );
	}

	return sz;
}

//____ _firstHook() ___________________________________________________________

WgHook* WgPackList::_firstHook() const
{
	if( m_hooks.Size() > 0 )
		return m_hooks.Hook(0);

	return 0;
}

//____ _lastHook() ____________________________________________________________

WgHook* WgPackList::_lastHook() const
{
	if( m_hooks.Size() > 0 )
		return m_hooks.Hook(m_hooks.Size()-1);

	return 0;
}

//____ _firstHookWithGeo() ____________________________________________________

WgHook* WgPackList::_firstHookWithGeo( WgRect& geo ) const
{
	if( m_hooks.Size() == 0 )
		return 0;

	WgPackListHook * p = m_hooks.Hook(0);
	_getChildGeo(geo,p);
	return p;
}

//____ _nextHookWithGeo() _____________________________________________________

WgHook* WgPackList::_nextHookWithGeo( WgRect& geo, WgHook * pHook ) const
{
	WgPackListHook * p = m_hooks.Next(static_cast<WgPackListHook*>(pHook));
	if( p )
		_getChildGeo(geo,p);
	return p;
}

//_____ _lastHookWithGeo() ____________________________________________________

WgHook* WgPackList::_lastHookWithGeo( WgRect& geo ) const
{
	if( m_hooks.Size() == 0 )
		return 0;

	WgPackListHook * p = m_hooks.Hook(m_hooks.Size()-1);
	_getChildGeo(geo,p);
	return p;
}

//____ _prevHookWithGeo() _____________________________________________________

WgHook* WgPackList::_prevHookWithGeo( WgRect& geo, WgHook * pHook ) const
{
	WgPackListHook * p = m_hooks.Prev(static_cast<WgPackListHook*>(pHook));
	if( p )
		_getChildGeo(geo,p);
	return p;
}

//____ _listArea() ____________________________________________________________

WgRect WgPackList::_listArea() const
{
	WgRect r = _listCanvas();
	if( m_pSkin )
		r = m_pSkin->ContentRect( r, m_state );

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
	WgSize wantedIconSize = m_header.icon.PreferredSize();
	WgSize wantedArrowSize = m_header.arrow.PreferredSize();
	WgSize wantedTextSize = m_header.label.unwrappedSize();

	WgSize wantedSize;

	//TODO: Assumes icon/arrow origos to not be NORTH, SOUTH or CENTER.
	//TODO: Assumes text not wrapping.

	wantedSize.h = WgMax(wantedIconSize.h, wantedArrowSize.h, wantedTextSize.h );
	wantedSize.w = wantedTextSize.w;
	if( m_header.icon.Overlap() )
		wantedSize.w = WgMax(wantedSize.w,wantedIconSize.w);
	else
		wantedSize.w += wantedIconSize.w;

	if( m_header.arrow.Overlap() )
		wantedSize.w = WgMax(wantedSize.w,wantedArrowSize.w);
	else
		wantedSize.w += wantedArrowSize.w;

	//

	if( m_header.m_pSkin )
		wantedSize = m_header.m_pSkin->SizeForContent(wantedSize);
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

	if( m_hooks.IsEmpty() )
		return true;

	// Create a temporary, sorted list of pointers to hooks

	int listSize = sizeof(int) * m_hooks.Size();
	int * pOrderList = (int*) WgBase::MemStackAlloc( listSize );
	int negator = m_sortOrder == WG_SORT_ASCENDING ? 1 : -1;

	pOrderList[0] = 0;
	for( int entry = 1 ; entry < m_hooks.Size() ; entry++ )
	{
		WgWidget * pWidget = m_hooks.Hook(entry)->_widget();

		int first = 0;
		int last = entry-1;
		int middle = (first+last)/2;
 
		while( first <= last )
		{
			WgWidget * pEntry = m_hooks.Hook(pOrderList[middle])->_widget();

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

	m_hooks.Reorder( pOrderList );
	WgBase::MemStackRelease( listSize );

	// Update m_ofs in the hooks

	_updateChildOfsFrom( m_hooks.Begin() );
	_requestRenderChildrenFrom( m_hooks.Begin() );	// Request render on dirty area
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


