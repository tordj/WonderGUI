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

#include <algorithm>
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
		m_sortOrder = SortOrder::Ascending;
		m_header.setSortOrder( SortOrder::Ascending );
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
		pWidget->_setHolder( this, (Hook*) pHook );

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
			PackListHook * pHook = static_cast<PackListHook*>(reinterpret_cast<Hook*>(pSibling->_holdersRef()));
			if( pHook && pHook->_parent() == this )
				index = m_hooks.index(pHook);
			else
				return 0;
		}
	
		PackListHook * pHook = m_hooks.insert(index);
		pHook->m_pParent = this;
		pHook->_setWidget(pWidget.rawPtr());
		pWidget->_setHolder( this, (Hook*) pHook );

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
		pWidget->_setHolder( this, (Hook*) pHook );

		_onWidgetAppeared( pHook );
		return pHook;
	}
	
	//____ removeWidget() _________________________________________________________
	
	bool PackList::removeWidget( const Widget_p& pWidget )
	{
		if( !pWidget || pWidget->_parent() != this )
			return false;
	
		PackListHook * pHook = static_cast<PackListHook*>(reinterpret_cast<Hook*>(pWidget->_holdersRef()));
	
		int index = m_hooks.index(pHook);
		_onWidgetDisappeared( pHook );
		pWidget->_setHolder( nullptr, nullptr );
		m_hooks.remove(index);
		return true;
	}
	
	//____ clear() ________________________________________________________________
	
	bool PackList::clear()
	{
		for( int i = 0 ; i < m_hooks.size() ; i++ )
			m_hooks.hook(i)->_widget()->_setHolder( nullptr, nullptr );

		m_hooks.clear();
		_refreshList();
		return true;
	}
	
	//____ setOrientation() _______________________________________________________
	
	void PackList::setOrientation( Orientation orientation )
	{
		bool bHorizontal = (orientation == Orientation::Horizontal);
	
		if( bHorizontal != m_bHorizontal )
		{
			m_bHorizontal = bHorizontal;
			_refreshList();
		}
	}
	
	//____ sortWidgets() __________________________________________________________
	
	void PackList::sortWidgets()
	{
		_sortEntries();
	}
	
	//____ setSortOrder() _________________________________________________________
	
	void PackList::setSortOrder( SortOrder order )
	{
		if( order != m_sortOrder )
		{
			m_sortOrder = order;
			m_header.setSortOrder( order );
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
		Size sz = m_pSkin ? m_pSkin->contentPadding() : Size();
		Size header = m_header.preferredSize();

		if( m_bHorizontal )
		{
			sz +=  Size(m_contentPreferredLength + header.w, m_contentPreferredBreadth);
			if( header.h > sz.h )
				sz.h = header.h;
		}
		else
		{
			sz += Size(m_contentPreferredBreadth,m_contentPreferredLength + header.h );
			if( header.w > sz.w )
				sz.w = header.w;
		}
	
		return sz;
	}
	
	//____ matchingHeight() _______________________________________________________
	
	int PackList::matchingHeight( int width ) const
	{
		if( m_bHorizontal )
		{
			int height = m_contentPreferredBreadth;
			if( m_pSkin )
				height += m_pSkin->contentPadding().h;

			return std::max(height, m_header.preferredSize().h);
		}
		else
		{
			int height = m_header.matchingHeight(width);
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
			int width = m_header.matchingWidth( height );
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
			int width = m_contentPreferredBreadth;
			if( m_pSkin )
				width += m_pSkin->contentPadding().w;

			return std::max(width, m_header.preferredSize().w);
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
		_refreshList();
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
		_refreshList();
		return true;
	}
	
	
	//____ _collectPatches() ____________________________________________________
	
	void PackList::_collectPatches( Patches& container, const Rect& geo, const Rect& clip )
	{
		if( m_pSkin )
			container.add( Rect( geo, clip ) );
		else
		{
			if( m_bHorizontal )
				container.add( Rect( Rect( geo.x, geo.y, wg::min(geo.w,m_contentLength), geo.h ), clip ) );
			else
				container.add( Rect( Rect( geo.x, geo.y, geo.w, wg::min(geo.h,m_contentLength) ), clip ) );
		}
	}
	
	//____ _maskPatches() _______________________________________________________
	
	void PackList::_maskPatches( Patches& patches, const Rect& geo, const Rect& clip, BlendMode blendMode )
	{
		if( (m_bOpaque && blendMode == BlendMode::Blend) || blendMode == BlendMode::Replace)
			patches.sub( Rect(geo,clip) );
		else if( m_bOpaqueEntries && blendMode == BlendMode::Blend )
		{
			if( m_bHorizontal )
				patches.sub( Rect( Rect( geo.x, geo.y, wg::min(geo.w,m_contentLength), geo.h ), clip ) );
			else
				patches.sub( Rect( Rect( geo.x, geo.y, geo.w, wg::min(geo.h,m_contentLength) ), clip ) );
		}
		else
		{
			Rect childGeo;
			PackListHook * p = static_cast<PackListHook*>(_firstHookWithGeo( childGeo ));
	
			while(p)
			{
				if( p->_isVisible() )
					p->_widget()->_maskPatches( patches, childGeo + geo.pos(), clip, blendMode );
				p = static_cast<PackListHook*>(_nextHookWithGeo( childGeo, p ));
			}
		}
	
	}
	
	//____ _cloneContent() ______________________________________________________
	
	void PackList::_cloneContent( const Widget * _pOrg )
	{
		List::_cloneContent( _pOrg );

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
			_render(pDevice, _canvas, _window, *pRect );
			
		
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
	
		if( m_header.size().h != 0 )
		{
			bool bInvertedSort = (m_sortOrder == SortOrder::Descending);
			Rect canvas = _headerGeo() + _canvas.pos();
	
			for( const Rect * pRect = patches.begin() ; pRect != patches.end() ; pRect++ )
				m_header.render( pDevice, canvas, *pRect );
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
	
	
	//____ _render() ____________________________________________________________
	
	void PackList::_render( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, const Rect& _clip )
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
	
	//		pChild->_render( pDevice, childGeo, childGeo, _clip );
	
		}
	}
	
	//____ _setSize() ___________________________________________________________
	
	void PackList::_setSize( const Size& _size )
	{
		List::_setSize(_size);

		Size headerSize = m_bHorizontal ? Size(m_header.matchingWidth(_size.h), _size.h) : Size( _size.w, m_header.matchingHeight( _size.w ));
		m_header.setSize( headerSize );

		Size size = _size;
		if( m_pSkin )
			size -= m_pSkin->contentPadding();

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
	
					pWidget->_setSize( Size(newEntryLength, newContentBreadth) );				//TODO: Should be able to do a _setSize() that prevents child from doing a _requestRender().
				}
				else
				{
					int newEntryLength = _paddedLimitedMatchingHeight(pWidget, newContentBreadth );
					pHook->m_ofs = ofs;
					pHook->m_length = newEntryLength;
					ofs += newEntryLength;
	
					pWidget->_setSize( Size(newContentBreadth, newEntryLength) );				//TODO: Should be able to do a _setSize() that prevents child from doing a _requestRender().
				}
			}
			m_contentLength = ofs;
		}
	
		_requestRender();
	}
	
	//____ _refresh() ___________________________________________________________
	
	void PackList::_refresh()
	{
		_refreshHeader();
		_refreshList();	
		Widget::_refresh();
	}
	
	//____ _refreshList() _______________________________________________________
	
	void PackList::_refreshList()
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
	
	//____ _receive() _____________________________________________________________
	
	void PackList::_receive( const Msg_p& _pMsg )
	{
		bool bSwallowed = m_header.receive(_pMsg);

		if( !bSwallowed )
		{
			switch( _pMsg->type() )
			{
				case MsgType::KeyPress:
				{
					if( m_selectMode == SelectMode::Unselectable )
						break;

					Key keyCode = KeyPressMsg::cast(_pMsg)->translatedKeyCode();
					ModifierKeys	modKeys = KeyPressMsg::cast(_pMsg)->modKeys();
					if( (m_bHorizontal && (keyCode == Key::Left || keyCode == Key::Right)) || 
						(!m_bHorizontal && (keyCode == Key::Up || keyCode == Key::Down || keyCode == Key::PageUp || keyCode == Key::PageDown)) ||
						keyCode == Key::Home || keyCode == Key::End ||
						(m_selectMode == SelectMode::FlipOnSelect && keyCode == Key::Space ) )
						_pMsg->swallow();
					break;
				}

				case MsgType::KeyRepeat:
				case MsgType::KeyRelease:
				{
					if( m_selectMode == SelectMode::Unselectable )
						break;

					Key keyCode = KeyMsg::cast(_pMsg)->translatedKeyCode();
					ModifierKeys	modKeys = KeyMsg::cast(_pMsg)->modKeys();
					if( (m_bHorizontal && (keyCode == Key::Left || keyCode == Key::Right)) || 
						(!m_bHorizontal && (keyCode == Key::Up || keyCode == Key::Down || keyCode == Key::PageUp || keyCode == Key::PageDown)) ||
						keyCode == Key::Home || keyCode == Key::End ||
						(m_selectMode == SelectMode::FlipOnSelect && keyCode == Key::Space ) )
						_pMsg->swallow();
					break;
				}			
			}	

			List::_receive( _pMsg );
		}
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
				int beg = wg::min(oldFirst,newFirst);
				int end = wg::max(oldFirst,newFirst)-1;
	
				_flipRange( m_hooks.hook(beg), m_hooks.hook(end), true );
			}
	
			if( oldLast != newLast )
			{
				int beg = wg::min(oldLast,newLast)+1;
				int end = wg::max(oldLast,newLast);
	
				_flipRange( m_hooks.hook(beg), m_hooks.hook(end), true );
			}
		}
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
		static_cast<PackListHook*>(pInserted)->_widget()->_setSize( childGeo );
	
	
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
	 	int negator = m_sortOrder == SortOrder::Ascending ? 1 : -1;
	
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
	
	Widget * PackList::_findWidget( const Coord& ofs, SearchMode mode )
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
					else if( mode == SearchMode::Geometry || pHook->_widget()->markTest( ofs - childGeo.pos() ) )
					{
							pResult = pHook->_widget();
					}
				}
	
				if( !pResult && mode == SearchMode::ActionTarget )
					pResult = pHook->_widget();						// Entries are opaque as action targets.
	
			}
		}
	
		// Check against ourselves
	
		if( !pResult && ( mode == SearchMode::Geometry || markTest(ofs)) )
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
						m_nbPreferredBreadthEntries = 1;
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
				breadthDiff = (newPadding.h - oldPadding.h);
			}
			else
			{
				lengthDiff = (newPadding.h - oldPadding.h)*nEntries;
				breadthDiff = (newPadding.w - oldPadding.w);
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
		limit( height, m_minEntrySize.h, m_maxEntrySize.h );
		return height;
	}
	
	//____ _paddedLimitedMatchingWidth() _________________________________________
	
	int PackList::_paddedLimitedMatchingWidth( Widget * pChild, int paddedHeight )
	{
		int width = pChild->matchingWidth( paddedHeight - m_entryPadding.h ) + m_entryPadding.w;
		limit( width, m_minEntrySize.w, m_maxEntrySize.w );
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
			limit(h, m_minEntrySize.h, m_maxEntrySize.h );
		}
		else if( sz.h > m_maxEntrySize.h )
		{
			int w = pChild->matchingWidth(m_maxEntrySize.h-m_entryPadding.h) + m_entryPadding.w;
			limit(w, m_minEntrySize.w, m_maxEntrySize.w );
		}
	
		return sz;
	}

	//____ _childPos() ________________________________________________________

	Coord PackList::_childPos( void * pChildRef ) const
	{
		return ((Hook*)pChildRef)->pos();
	}

	//____ _childSize() __________________________________________________________

	Size PackList::_childSize( void * pChildRef ) const
	{
		return ((Hook*)pChildRef)->size();
	}

	//____ _childRequestRender() _________________________________________________

	void PackList::_childRequestRender( void * pChildRef )
	{
		PackListHook * pHook = static_cast<PackListHook*>(reinterpret_cast<Hook*>(pChildRef));

		Rect geo;
		_getChildGeo(geo, pHook);
		_requestRender(geo);
	}

	void PackList::_childRequestRender( void * pChildRef, const Rect& rect )
	{
		PackListHook * pHook = static_cast<PackListHook*>(reinterpret_cast<Hook*>(pChildRef));

		Rect geo;
		_getChildGeo(geo, pHook);
		geo.x += rect.x;
		geo.y += rect.y;
		geo.w = rect.w;
		geo.h = rect.h;
		_requestRender(geo);
	}

	//____ _childRequestResize() _________________________________________________

	void PackList::_childRequestResize( void * pChildRef )
	{
		PackListHook * pHook = static_cast<PackListHook*>(reinterpret_cast<Hook*>(pChildRef));

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
			// NOTE: Order here is important!

			_addToContentPreferredSize( prefLength, prefBreadth );
			int oldPrefBreadth = pHook->m_prefBreadth;
			pHook->m_prefBreadth = prefBreadth;
			_subFromContentPreferredSize( pHook->m_length, oldPrefBreadth );

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
			pHook->_widget()->_setSize(childGeo);
		}



		if( bReqResize )
			_requestResize();
	}

	//____ _prevChild() __________________________________________________________

	Widget * PackList::_prevChild( void * pChildRef ) const
	{
		Hook *p = ((Hook*)pChildRef)->_prevHook();
		return p ? p->_widget() : nullptr;
	}

	//____ _nextChild() __________________________________________________________

	Widget * PackList::_nextChild( void * pChildRef ) const
	{
		Hook *p = ((Hook*)pChildRef)->_nextHook();
		return p ? p->_widget() : nullptr;
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
			r.x += m_header.size().w;
			r.w -= m_header.size().w;
		}
		else
		{
			r.y += m_header.size().h;
			r.h -= m_header.size().h;
		}
		return r;
	}
	
	//____ _listCanvas() ____________________________________________________________
	
	Rect PackList::_listCanvas() const
	{
		Size header = m_header.size();

		if( m_bHorizontal )
			return Rect(header.w, 0, m_size.w - header.w, m_size.h );
		else
			return Rect(0, header.h, m_size.w, m_size.h - header.h);	// List canvas in widgets own coordinate system.
	}
	
	//____ _headerGeo() ___________________________________________________________
	
	Rect PackList::_headerGeo() const
	{
		if( m_bHorizontal )
			return Rect( _windowSection().x, 0, m_header.size().w, m_size.h );
		else
			return Rect( 0, _windowSection().y, m_size.w, m_header.size().h );
	}
	
	//____ _windowPadding() _______________________________________________________
	
	Size PackList::_windowPadding() const
	{
		if( m_bHorizontal )
			return Size( m_header.size().w, 0 );
		else
			return Size( 0, m_header.size().h );
	}
	
	//____ _refreshHeader() _______________________________________________________
	
	void PackList::_refreshHeader()
	{
		Size wantedSize = m_header.preferredSize();
		Size currentSize = m_header.size();

		bool	bRequestResize = false;
		
		// Update headers size, possibly request resize.
	
		if( wantedSize.h != currentSize.h )
		{
			if( !m_bHorizontal || (wantedSize.h > m_contentPreferredBreadth || currentSize.h > m_contentPreferredBreadth) )
				bRequestResize = true;	
		}
	
		if( wantedSize.w != currentSize.w )
		{
			if( m_bHorizontal || (wantedSize.w > m_contentPreferredBreadth || currentSize.w > m_contentPreferredBreadth) )
				bRequestResize = true;	
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
		int negator = m_sortOrder == SortOrder::Ascending ? 1 : -1;
	
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

	//____ _itemPos() ____________________________________________________________

	Coord PackList::_itemPos( const Item * pItem ) const
	{
		Coord c = m_bHorizontal ? Coord(_windowSection().x, 0 ) : Coord(0, _windowSection().y );
		return c;
	}

	//____ _itemSize() _________________________________________________________

	Size PackList::_itemSize( const Item * pItem ) const
	{
		return m_header.size();		// We store size internally in the header.
	}

	//____ _itemGeo() __________________________________________________________

	Rect PackList::_itemGeo( const Item * pItem ) const
	{
		return Rect( _itemPos(pItem), m_header.size() );
	}

	//____ _itemNotified() _____________________________________________________

	void PackList::_itemNotified( Item * pItem, ItemNotif notification, void * pData )
	{
		if( notification == ItemNotif::SortOrderChanged )
		{
			m_sortOrder = m_header.sortOrder();
			_sortEntries();
		}
	}


} // namespace wg
	