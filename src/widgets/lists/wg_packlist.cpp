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
	

	//____ insertSorted() ___________________________________________________

	PackListChildren::iterator PackListChildren::insertSorted(Widget * pWidget)
	{
		//TODO: Replace with assert
//		if (!pWidget)
//			return false;

		if (m_pSlotArray->isEmpty() || !m_pHolder->m_sortFunc)
			return add(pWidget);

		int index = m_pHolder->_getInsertionPoint(pWidget);
		return insert(index, pWidget);
	}

	//____ sort() __________________________________________________________

	void PackListChildren::sort()
	{
		m_pHolder->_sortEntries();
	}


	//____ Constructor ____________________________________________________________
	
	PackList::PackList() : m_header(this), header(&m_header), children(&m_children, this)
	{
		m_sizeOfSlot = sizeof(PackListSlot);
		m_bSiblingsOverlap = false;
		m_bHorizontal = false;
		m_sortOrder = SortOrder::Ascending;
		m_header.setSortOrder( SortOrder::Ascending );
	
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
	
	PackList_p PackList::cast( Object * pObj )
	{
		if( pObj && pObj->isInstanceOf(CLASSNAME) )
			return PackList_p( static_cast<PackList*>(pObj) );
	
		return 0;
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
	
	void PackList::setSortFunction( std::function<int(const Widget *,const Widget *)> func )
	{
		m_sortFunc = func;
		_sortEntries();
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

			for( auto pSlot = m_children.begin(); pSlot < m_children.end(); pSlot++ )
				height += pSlot->pWidget->matchingHeight(width);

			height += m_entryPadding.h*m_children.size();		
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
	
			for (auto pSlot = m_children.begin(); pSlot < m_children.end(); pSlot++)
				width += pSlot->pWidget->matchingWidth(width);

			width += m_entryPadding.w*m_children.size();		
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
			SlotWithGeo child;
			_firstSlotWithGeo( child );
	
			while(child.pSlot)
			{
				child.pSlot->pWidget->_maskPatches( patches, child.geo + geo.pos(), clip, blendMode );
				_nextSlotWithGeo( child );
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
			SlotWithGeo child;
			_firstSlotWithGeo( child );
	
			while(child.pSlot)
			{
				Rect canvas = child.geo + _canvas.pos();
				if( canvas.intersectsWith( dirtBounds ) )
					child.pSlot->pWidget->_renderPatches( pDevice, canvas, canvas, &patches );
				_nextSlotWithGeo( child );
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
	
		for( int i = _getEntryAt( startOfs ) ; i < m_children.size() ; i++ )
		{
			PackListSlot * pSlot = m_children.slot(i);
			Widget * pChild = pSlot->pWidget;
	
			// Get entry geometry, skin and state
	
			Rect entryGeo( contentRect );
			if( m_bHorizontal )
			{
				if( pSlot->ofs >= contentRect.w )
					break;
	
				entryGeo.x += pSlot->ofs;
				entryGeo.w = pSlot->length;
			}
			else
			{
				if( pSlot->ofs >= contentRect.h )
					break;
	
				entryGeo.y += pSlot->ofs;
				entryGeo.h = pSlot->length;
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
	
			for( auto pSlot = m_children.begin() ; pSlot < m_children.end() ; pSlot++ )
			{
				Widget * pWidget = pSlot->pWidget;
	
				if( m_bHorizontal )
				{
					int newEntryLength = _paddedLimitedMatchingWidth(pWidget, newContentBreadth );
					pSlot->ofs = ofs;
					pSlot->length = newEntryLength;
					ofs += newEntryLength;
	
					pWidget->_setSize( Size(newEntryLength, newContentBreadth) );				//TODO: Should be able to do a _setSize() that prevents child from doing a _requestRender().
				}
				else
				{
					int newEntryLength = _paddedLimitedMatchingHeight(pWidget, newContentBreadth );
					pSlot->ofs = ofs;
					pSlot->length = newEntryLength;
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
	
		for (auto pSlot = m_children.begin(); pSlot < m_children.end(); pSlot++)
		{
			Widget * pChild = pSlot->pWidget;
	
			Size pref = _paddedLimitedPreferredSize( pChild );
	
			if( m_bHorizontal )
			{
				_addToContentPreferredSize( pref.w, pref.h );
				pSlot->ofs = ofs;
	
				// Get entry length and breadth
	
				if( pref.h == m_contentBreadth )
					pSlot->length = pref.w;
				else
					pSlot->length	= _paddedLimitedMatchingWidth(pChild, m_contentBreadth);
				pSlot->prefBreadth = pref.h;
				ofs += pSlot->length;
	
			}
			else
			{
				_addToContentPreferredSize( pref.h, pref.w );
				pSlot->ofs = ofs;
	
				// Get entry length and breadth
	
				if( pref.w == m_contentBreadth )
					pSlot->length = pref.h;
				else
					pSlot->length = _paddedLimitedMatchingHeight(pChild, m_contentBreadth);
				pSlot->prefBreadth = pref.w;
				ofs += pSlot->length;
			}
		}
		m_contentLength = ofs;
	
		// Finish up
	
		_requestRender();
		_requestResize();						// This should preferably be done first once we have changed the method.
	}
	
	//____ _receive() _____________________________________________________________
	
	void PackList::_receive( Msg * _pMsg )
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
				end = newLast+1;
			}
			else
			{
				beg = oldFirst;
				end = oldLast+1;
			}
	
			_flipSelection( m_children.slot(beg), m_children.slot(end), true );
		}
		else
		{
			if( oldFirst != newFirst )
			{
				int beg = wg::min(oldFirst,newFirst);
				int end = wg::max(oldFirst,newFirst);
	
				_flipSelection( m_children.slot(beg), m_children.slot(end), true );
			}
	
			if( oldLast != newLast )
			{
				int beg = wg::min(oldLast,newLast)+1;
				int end = wg::max(oldLast,newLast)+1;
	
				_flipSelection( m_children.slot(beg), m_children.slot(end), true );
			}
		}
	}

	//____ _didAddSlots() _____________________________________________________

	void PackList::_didAddSlots(Slot * pSlot, int nb)
	{
		_unhideSlots((ListSlot*)pSlot, nb);
		List::_didAddSlots(pSlot, nb);
	}

	//____ _willRemoveSlots() _____________________________________________________

	void PackList::_willRemoveSlots(Slot * pSlot, int nb)
	{
		_hideSlots((ListSlot*)pSlot, nb);
		List::_willRemoveSlots(pSlot, nb);
	}

	//____ _hideSlots() _____________________________________________________

	void PackList::_hideSlots(ListSlot * _pSlot, int nb)
	{
		PackListSlot * pSlot = static_cast<PackListSlot*>(_pSlot);

		_requestRenderChildrenFrom(pSlot);	// Request render on dirty area

		for (int i = 0; i < nb; i++)
		{
			Widget * pChild = pSlot[i].pWidget;
			Size pref = _paddedLimitedPreferredSize(pChild);
			if (m_bHorizontal)
				_subFromContentPreferredSize(pref.w, pref.h);
			else
				_subFromContentPreferredSize(pref.h, pref.w);

			m_contentLength -= pSlot[i].length;
			pSlot[i].length = 0;
		}

		_updateChildOfsFrom(pSlot);
		_requestResize();
	}

	//____ _unhideSlots() _____________________________________________________

	void PackList::_unhideSlots(ListSlot * _pSlot, int nb)
	{
		PackListSlot * pSlot = static_cast<PackListSlot*>(_pSlot);

		for (int i = 0; i < nb; i++)
		{
			Widget * pChild = pSlot[i].pWidget;
			Size pref = _paddedLimitedPreferredSize(pChild);

			if (m_bHorizontal)
			{
				_addToContentPreferredSize(pref.w, pref.h);

				// Get entry length and breadth

				if (pref.h == m_contentBreadth)
					pSlot[i].length = pref.w;
				else
					pSlot[i].length = _paddedLimitedMatchingWidth(pChild, m_contentBreadth);
				pSlot[i].prefBreadth = pref.h;
			}
			else
			{
				_addToContentPreferredSize(pref.h, pref.w);

				// Get entry length and breadth

				if (pref.w == m_contentBreadth)
					pSlot[i].length = pref.h;
				else
					pSlot[i].length = _paddedLimitedMatchingHeight(pChild, m_contentBreadth);
				pSlot[i].prefBreadth = pref.w;

			}

			m_contentLength += pSlot[i].length;

			Rect childGeo;
			_getChildGeo(childGeo, pSlot+i);
			pChild->_setSize(childGeo);
		}

		// Finish up

		_updateChildOfsFrom(pSlot);
		_requestRenderChildrenFrom(pSlot);	// Request render on dirty area
		_requestResize();						// This should preferably be done first once we have changed the method.	}
	}

	//____ _beginSlots() _____________________________________________________

	ListSlot * PackList::_beginSlots() const
	{
		return m_children.begin();
	}

	//____ _endSlots() _____________________________________________________

	ListSlot * PackList::_endSlots() const
	{
		return m_children.end();
	}

	
	//____ _getEntryAt() __________________________________________________________
	
	// Pixelofs is counted from beginning of container content, not widget.
	
	int PackList::_getEntryAt( int pixelofs ) const
	{
		int first = 0;
		int last = m_children.size() - 1;
		int middle = (first+last)/2;
	 
		while( first <= last )
		{
			PackListSlot * pSlot = m_children.slot(middle);
	
			if( pSlot->ofs + pSlot->length < pixelofs )
				first = middle + 1;
			else if( pSlot->ofs <= pixelofs ) 
				return middle;
			else
				last = middle - 1;
	 
			middle = (first + last)/2;
		}
	
		return m_children.size();
	}
	
	//____ _getInsertionPoint() ___________________________________________________
	
	int PackList::_getInsertionPoint( const Widget * pWidget ) const
	{
		int first = 0;
		int last = m_children.size() - 1;
		int middle = (first+last)/2;
	 	int negator = m_sortOrder == SortOrder::Ascending ? 1 : -1;
	
		while( first <= last )
		{
			PackListSlot * pSlot = m_children.slot(middle);
	
			int cmpRes = m_sortFunc( pSlot->pWidget, pWidget )*negator;
	
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
	
	ListSlot * PackList::_findEntry( const Coord& ofs )
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
	
			if( entry != m_children.size() )
				return m_children.slot(entry);
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
	
			if( entry != m_children.size() )
			{
				PackListSlot * pSlot = m_children.slot(entry);
				Rect childGeo;
				_getChildGeo( childGeo, pSlot );
				if( childGeo.contains(ofs) )
				{
					if( pSlot->pWidget->isContainer() )
					{
						pResult = static_cast<Container*>(pSlot->pWidget)->_findWidget( ofs - childGeo.pos(), mode );
					}
					else if( mode == SearchMode::Geometry || pSlot->pWidget->markTest( ofs - childGeo.pos() ) )
					{
							pResult = pSlot->pWidget;
					}
				}
	
				if( !pResult && mode == SearchMode::ActionTarget )
					pResult = pSlot->pWidget;						// Entries are opaque as action targets.
	
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
				for( PackListSlot * p = m_children.begin() ; p < m_children.end() ; p++ )
				{
					if( p->prefBreadth == highest )
						m_nbPreferredBreadthEntries++;
					else if( p->prefBreadth > highest )
					{
						highest = p->prefBreadth;
						m_nbPreferredBreadthEntries = 1;
					}
				}
				m_contentPreferredBreadth = highest;
			}
		}
	}
	
	
	//____ _requestRenderChildrenFrom() ___________________________________________
	
	void PackList::_requestRenderChildrenFrom( PackListSlot * pSlot )
	{
		Rect box = _listArea();
	
		if( m_bHorizontal )
		{
			box.x += pSlot->ofs;
			box.w = m_contentLength - pSlot->ofs;
		}
		else
		{
			box.y += pSlot->ofs;
			box.h = m_contentLength - pSlot->ofs;
		}
	
		_requestRender( box );
	}
	
	//____ _updateChildOfsFrom() __________________________________________________
	
	void PackList::_updateChildOfsFrom( PackListSlot * pSlot )
	{
		int ofs = 0;
		if( pSlot > m_children.begin() )
			ofs = pSlot[-1].ofs + pSlot[-1].length;
	
		while( pSlot < m_children.end() )
		{
			pSlot->ofs = ofs;
			ofs += pSlot->length;
			pSlot++;
		}	
	}
	
	//____ _onEntrySkinChanged() __________________________________________________
	
	void PackList::_onEntrySkinChanged( Size oldPadding, Size newPadding )
	{
		_requestRender();
	
		if( oldPadding != newPadding )
		{
			m_entryPadding = newPadding;
			int nEntries = m_children.size();
	
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
	
	void PackList::_getEntryGeo( Rect& geo, const ListSlot * _pSlot ) const
	{
		const PackListSlot * pSlot = static_cast<const PackListSlot*>(_pSlot);
	
		geo = _listArea();
	
		if( m_bHorizontal )
		{
			geo.x += pSlot->ofs;
			geo.w = pSlot->length;
		}
		else
		{
			geo.y += pSlot->ofs;
			geo.h = pSlot->length;
		}
	}
	
	
	//____ _getChildGeo() _________________________________________________________
	
	void PackList::_getChildGeo( Rect& geo, const PackListSlot * pSlot ) const
	{
		geo = _listArea();
	
		if( m_bHorizontal )
		{
			geo.x += pSlot->ofs;
			geo.w = pSlot->length;
		}
		else
		{
			geo.y += pSlot->ofs;
			geo.h = pSlot->length;
		}
	
		// Apply any padding from the entry skin, if entry visible
	
		if( pSlot->bVisible )
		{
			int index = m_children.index( pSlot );
			if( m_pEntrySkin[index&0x1] )
				geo = m_pEntrySkin[index&0x1]->contentRect( geo, pSlot->pWidget->state() );
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

	Coord PackList::_childPos( Slot * _pSlot ) const
	{
		PackListSlot * pSlot = reinterpret_cast<PackListSlot*>(_pSlot);

		Rect geo;
		_getChildGeo(geo, pSlot);

		return geo.pos();
	}

	//____ _childSize() __________________________________________________________

	Size PackList::_childSize( Slot * _pSlot ) const
	{
		PackListSlot * pSlot = reinterpret_cast<PackListSlot*>(_pSlot);

		Rect geo;
		_getChildGeo(geo, pSlot);

		return geo.size();
	}

	//____ _childRequestRender() _________________________________________________

	void PackList::_childRequestRender( Slot * _pSlot )
	{
		PackListSlot * pSlot = reinterpret_cast<PackListSlot*>(_pSlot);

		Rect geo;
		_getChildGeo(geo, pSlot);
		_requestRender(geo);
	}

	void PackList::_childRequestRender( Slot * _pSlot, const Rect& rect )
	{
		PackListSlot * pSlot = reinterpret_cast<PackListSlot*>(_pSlot);

		Rect geo;
		_getChildGeo(geo, pSlot);
		geo.x += rect.x;
		geo.y += rect.y;
		geo.w = rect.w;
		geo.h = rect.h;
		_requestRender(geo);
	}

	//____ _childRequestResize() _________________________________________________

	void PackList::_childRequestResize( Slot * _pSlot )
	{
		PackListSlot * pSlot = reinterpret_cast<PackListSlot*>(_pSlot);

		if( !pSlot->bVisible  || m_minEntrySize == m_maxEntrySize )
			return;

		Widget * pChild = pSlot->pWidget;
		Size prefEntrySize = _paddedLimitedPreferredSize(pChild);

		int prefLength = m_bHorizontal ? prefEntrySize.w : prefEntrySize.h;
		int prefBreadth = m_bHorizontal ? prefEntrySize.h : prefEntrySize.w;

		bool	bReqResize = false;

		// Update preferred sizes

		if( prefBreadth != pSlot->prefBreadth || prefLength != pSlot->length )
		{
			// NOTE: Order here is important!

			_addToContentPreferredSize( prefLength, prefBreadth );
			int oldPrefBreadth = pSlot->prefBreadth;
			pSlot->prefBreadth = prefBreadth;
			_subFromContentPreferredSize( pSlot->length, oldPrefBreadth );

			bReqResize = true;
		}

		// Calculate new length

		int length;
		if( prefBreadth == m_contentBreadth )	
			length = prefLength;
		else
			length = m_bHorizontal ? _paddedLimitedMatchingWidth(pChild, m_contentBreadth ) : _paddedLimitedMatchingHeight(pChild, m_contentBreadth );

		// Update if length has changed

		if( length != pSlot->length )
		{
			m_contentLength += length - pSlot->length;
			pSlot->length = length;
			bReqResize = true;

			_updateChildOfsFrom( pSlot );
			_requestRenderChildrenFrom( pSlot );

			Rect childGeo;
			_getChildGeo(childGeo,pSlot);
			pSlot->pWidget->_setSize(childGeo);
		}



		if( bReqResize )
			_requestResize();
	}

	//____ _prevChild() __________________________________________________________

	Widget * PackList::_prevChild( Slot * _pSlot ) const
	{
		PackListSlot * pSlot = reinterpret_cast<PackListSlot*>(_pSlot);

		if (pSlot > m_children.begin())
			return pSlot[-1].pWidget;

		return nullptr;
	}

	//____ _nextChild() __________________________________________________________

	Widget * PackList::_nextChild( Slot * _pSlot ) const
	{
		PackListSlot * pSlot = reinterpret_cast<PackListSlot*>(_pSlot);

		if (pSlot < m_children.last())
			return pSlot[1].pWidget;

		return nullptr;
	}
		
	//____ _firstSlotWithGeo() ____________________________________________________
	
	void PackList::_firstSlotWithGeo( SlotWithGeo& package ) const
	{
		if (m_children.isEmpty())
			package.pSlot = nullptr;
		else
		{
			PackListSlot * pSlot = m_children.first();
			package.pSlot = pSlot;
			_getChildGeo(package.geo, pSlot);
		}
	}
	
	//____ _nextSlotWithGeo() _____________________________________________________
	
	void PackList::_nextSlotWithGeo( SlotWithGeo& package ) const
	{
		PackListSlot * pSlot = (PackListSlot*)package.pSlot;

		if (pSlot == m_children.last())
			package.pSlot = nullptr;
		else
		{
			pSlot++;
			package.pSlot = pSlot;
			_getChildGeo(package.geo, pSlot);
		}
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
		if( !m_sortFunc )
			return false;
	
		if( m_children.isEmpty() )
			return true;
	
		// Create a temporary, sorted list of pointers to slots
	
		int listSize = sizeof(int) * m_children.size();
		int * pOrderList = (int*) Base::memStackAlloc( listSize );
		int negator = m_sortOrder == SortOrder::Ascending ? 1 : -1;
	
		pOrderList[0] = 0;
		for( int entry = 1 ; entry < m_children.size() ; entry++ )
		{
			Widget * pWidget = m_children.slot(entry)->pWidget;
	
			int first = 0;
			int last = entry-1;
			int middle = (first+last)/2;
	 
			while( first <= last )
			{
				Widget * pEntry = m_children.slot(pOrderList[middle])->pWidget;
	
				int cmpRes = m_sortFunc( pEntry, pWidget ) * negator;
	
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
	
		m_children.reorder( pOrderList );
		Base::memStackRelease( listSize );
	
		// Update ofs in the slots
	
		_updateChildOfsFrom( m_children.begin() );
		_requestRenderChildrenFrom( m_children.begin() );	// Request render on dirty area
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
	
