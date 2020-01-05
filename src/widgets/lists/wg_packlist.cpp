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
#include <wg_gfxdevice.h>

#include <wg_cselectableslotarray.impl.h>

namespace wg
{
	using namespace Util;

	template class CSelectableSlotArray<PackListSlot>;
	template class CDynamicSlotArray<PackListSlot>;

	const char PackList::CLASSNAME[] = {"PackList"};


	//____ insertSorted() ___________________________________________________

	CPackListSlotArray::iterator CPackListSlotArray::insertSorted(Widget * pWidget)
	{
		//TODO: Replace with assert
//		if (!pWidget)
//			return false;

		if (isEmpty() || !_holder()->_hasSortFunction())
			return pushBack(pWidget);

		int index = _holder()->_getInsertionPoint(pWidget);
		insert(index, pWidget);
		return iterator(_slot(index));
	}

	//____ sort() __________________________________________________________

	void CPackListSlotArray::sort()
	{
		_holder()->_sortEntries();
	}


	//____ Constructor ____________________________________________________________

	PackList::PackList() : header(this), slots(this)
	{
		m_sizeOfSlot = sizeof(PackListSlot);
		m_bSiblingsOverlap = false;
		m_bHorizontal = false;
		m_sortOrder = SortOrder::Ascending;
		_header()._setSortOrder( SortOrder::Ascending );

		m_maxEntrySize = SizeI(INT_MAX,INT_MAX);		//TODO: Test so m_maxEntrySize matters!

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
			_header()._setSortOrder( order );
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

	//____ setMinEntrySize() ______________________________________________________

	bool PackList::setMinEntrySize( Size _min )
	{
		SizeI min = qpixToRaw(_min);

		if( min == m_minEntrySize )
			return true;

		if( min.w > m_maxEntrySize.w || min.h > m_maxEntrySize.h )
			return false;

		m_minEntrySize = min;
		_refreshList();
		return true;
	}

	//____ setMaxEntrySize() ______________________________________________________

	bool PackList::setMaxEntrySize( Size _max )
	{
		SizeI max = qpixToRaw(_max);

		if( max == m_maxEntrySize )
			return true;

		if( max.w < m_minEntrySize.w || max.h < m_minEntrySize.h )
			return false;

		m_maxEntrySize = max;
		_refreshList();
		return true;
	}

	//____ _preferredSize() ________________________________________________________

	SizeI PackList::_preferredSize() const
	{
		SizeI sz = m_pSkin ? m_pSkin->_contentPadding() : SizeI();
		SizeI headerSize = _header()._preferredSize();

		if (m_bHorizontal)
		{
			sz += SizeI(m_contentPreferredLength + headerSize.w, m_contentPreferredBreadth);
			if (headerSize.h > sz.h)
				sz.h = headerSize.h;
		}
		else
		{
			sz += SizeI(m_contentPreferredBreadth, m_contentPreferredLength + headerSize.h);
			if (headerSize.w > sz.w)
				sz.w = headerSize.w;
		}

		return sz;
	}

	//____ _matchingHeight() _______________________________________________________

	int PackList::_matchingHeight(int width) const
	{
		if (m_bHorizontal)
		{
			int height = m_contentPreferredBreadth;
			if (m_pSkin)
				height += m_pSkin->_contentPadding().h;

			return std::max(height, _header()._preferredSize().h);
		}
		else
		{
			int height = _header()._matchingHeight(width);
			if (m_pSkin)
			{
				SizeI pad = m_pSkin->_contentPadding();
				width -= pad.w;
				height += pad.h;
			}
			width -= m_entryPadding.w;

			for (auto pSlot = slots._begin(); pSlot < slots._end(); pSlot++)
				height += pSlot->_matchingHeight(width);

			height += m_entryPadding.h*slots.size();
			return height;
		}
	}

	//____ _matchingWidth() _______________________________________________________

	int PackList::_matchingWidth(int height) const
	{
		if (m_bHorizontal)
		{
			int width = _header()._matchingWidth(height);
			if (m_pSkin)
			{
				SizeI pad = m_pSkin->_contentPadding();
				height -= pad.w;
				width += pad.h;
			}
			height -= m_entryPadding.h;

			for (auto pSlot = slots._begin(); pSlot < slots._end(); pSlot++)
				width += pSlot->_matchingWidth(width);

			width += m_entryPadding.w*slots.size();
			return width;
		}
		else
		{
			int width = m_contentPreferredBreadth;
			if (m_pSkin)
				width += m_pSkin->_contentPadding().w;

			return std::max(width, _header()._preferredSize().w);
		}
	}


	//____ _collectPatches() ____________________________________________________

	void PackList::_collectPatches( Patches& container, const RectI& geo, const RectI& clip )
	{
		if( m_pSkin )
			container.add( RectI( geo, clip ) );
		else
		{
			if( m_bHorizontal )
				container.add( RectI( RectI( geo.x, geo.y, wg::min(geo.w,m_contentLength), geo.h ), clip ) );
			else
				container.add( RectI( RectI( geo.x, geo.y, geo.w, wg::min(geo.h,m_contentLength) ), clip ) );
		}
	}

	//____ _maskPatches() _______________________________________________________

	void PackList::_maskPatches( Patches& patches, const RectI& geo, const RectI& clip, BlendMode blendMode )
	{
		if( (m_bOpaque && blendMode == BlendMode::Blend) || blendMode == BlendMode::Replace)
			patches.sub( RectI(geo,clip) );
		else if( m_bOpaqueEntries && blendMode == BlendMode::Blend )
		{
			if( m_bHorizontal )
				patches.sub( RectI( RectI( geo.x, geo.y, wg::min(geo.w,m_contentLength), geo.h ), clip ) );
			else
				patches.sub( RectI( RectI( geo.x, geo.y, geo.w, wg::min(geo.h,m_contentLength) ), clip ) );
		}
		else
		{
			RectI	myClip(geo, clip);				// Need to limit clip to our geo. Otherwise children outside might mask what they shouldn't.
			SlotWithGeo child;
			_firstSlotWithGeo( child );

			while(child.pSlot)
			{
				static_cast<const PackListSlot*>(child.pSlot)->_widget()->_maskPatches( patches, child.geo + geo.pos(), myClip, blendMode );
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

	//____ _render() _______________________________________________________

	void PackList::_render( GfxDevice * pDevice, const RectI& _canvas, const RectI& _window )
	{
		// Render container itself

		RectI contentRect = _listCanvas() + _canvas.pos();

		RectI clip = pDevice->clipBounds();

		if( m_pSkin )
		{
			m_pSkin->_render( pDevice, contentRect, m_state );
			contentRect = m_pSkin->_contentRect( contentRect, m_state );
		}

		int startOfs = m_bHorizontal ? clip.x-contentRect.x : clip.y-contentRect.y;
		if( startOfs < 0 )
			startOfs = 0;

		for( int i = _getEntryAt( startOfs ) ; i < slots.size() ; i++ )
		{
			PackListSlot * pSlot = slots._slot(i);
			Widget * pChild = pSlot->_widget();

			// Get entry geometry, skin and state

			RectI entryGeo( contentRect );
			if( m_bHorizontal )
			{
				if( pSlot->m_ofs >= contentRect.w )
					break;

				entryGeo.x += pSlot->m_ofs;
				entryGeo.w = pSlot->m_length;
			}
			else
			{
				if( pSlot->m_ofs >= contentRect.h )
					break;

				entryGeo.y += pSlot->m_ofs;
				entryGeo.h = pSlot->m_length;
			}

			Skin * pEntrySkin    = m_pEntrySkin[i&0x1].rawPtr();
			State    state        = pChild->state();
			//        RectI    childGeo( entryGeo );

			// Render entry skin, shrink child geo

			if( pEntrySkin )
			{
				pEntrySkin->_render( pDevice, entryGeo, state );
				//            childGeo = pEntrySkin->_contentRect( entryGeo, state );
			}

			// Render child

			//        pChild->_render( pDevice, childGeo, childGeo, _clip );

		}

		// Render children

		RectI	dirtBounds = pixelsToRaw(pDevice->clipBounds());

		{
			SlotWithGeo child;
			_firstSlotWithGeo( child );

			while(child.pSlot)
			{
				RectI canvas = child.geo + _canvas.pos();
				if (canvas.intersectsWith(dirtBounds))
				{
					ClipPopData clipPop = limitClipList(pDevice, rawToPixels(canvas));
					if( pDevice->clipListSize() > 0 )
						static_cast<const PackListSlot*>(child.pSlot)->_widget()->_render(pDevice, canvas, canvas);
					popClipList(pDevice,clipPop);
				}
				_nextSlotWithGeo( child );
			}
		}

		// Render header

		if( _header()._size().h != 0 )
		{
//			bool bInvertedSort = (m_sortOrder == SortOrder::Descending);
			RectI canvas = _headerGeo() + _canvas.pos();

			_header()._render( pDevice, canvas );
		}

		// Render Lasso

		if( m_pLassoSkin && m_lassoBegin != m_lassoEnd )
		{
			RectI lasso( m_lassoBegin, m_lassoEnd );
			lasso += _canvas.pos();

			m_pLassoSkin->_render(pDevice, lasso, m_state );
		}
	}

	//____ _resize() ___________________________________________________________

	void PackList::_resize( const SizeI& _size )
	{
		List::_resize(_size);

		SizeI headerSize = m_bHorizontal ? SizeI(_header()._matchingWidth(_size.h), _size.h) : SizeI( _size.w, _header()._matchingHeight( _size.w ));
		_header()._setSize( headerSize );

		SizeI size = _size;
		if( m_pSkin )
			size -= m_pSkin->_contentPadding();

		int newContentBreadth;

		if( m_bHorizontal )
			newContentBreadth = size.h;
		else
			newContentBreadth = size.w;

		if( newContentBreadth != m_contentBreadth )
		{
			m_contentBreadth = newContentBreadth;
			int ofs = 0;

			for( auto pSlot = slots._begin() ; pSlot < slots._end() ; pSlot++ )
			{
				Widget * pWidget = pSlot->_widget();

				if( m_bHorizontal )
				{
					int newEntryLength = _paddedLimitedMatchingWidth(pSlot, newContentBreadth );
					pSlot->m_ofs = ofs;
					pSlot->m_length = newEntryLength;
					ofs += newEntryLength;

					pWidget->_resize( SizeI(newEntryLength, newContentBreadth) );				//TODO: Should be able to do a _setSize() that prevents child from doing a _requestRender().
				}
				else
				{
					int newEntryLength = _paddedLimitedMatchingHeight(pSlot, newContentBreadth );
					pSlot->m_ofs = ofs;
					pSlot->m_length = newEntryLength;
					ofs += newEntryLength;

					pWidget->_resize( SizeI(newContentBreadth, newEntryLength) );				//TODO: Should be able to do a _setSize() that prevents child from doing a _requestRender().
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
			m_entryPadding = m_pEntrySkin[0]->_contentPadding();

		m_contentPreferredLength = 0;
		m_contentPreferredBreadth = 0;
		m_nbPreferredBreadthEntries = 0;
		int ofs = 0;

		for (auto pSlot = slots._begin(); pSlot < slots._end(); pSlot++)
		{
			SizeI pref = _paddedLimitedPreferredSize( pSlot );

			if( m_bHorizontal )
			{
				_addToContentPreferredSize( pref.w, pref.h );
				pSlot->m_ofs = ofs;

				// Get entry length and breadth

				if( pref.h == m_contentBreadth )
					pSlot->m_length = pref.w;
				else
					pSlot->m_length	= _paddedLimitedMatchingWidth(pSlot, m_contentBreadth);
				pSlot->m_prefBreadth = pref.h;
				ofs += pSlot->m_length;

			}
			else
			{
				_addToContentPreferredSize( pref.h, pref.w );
				pSlot->m_ofs = ofs;

				// Get entry length and breadth

				if( pref.w == m_contentBreadth )
					pSlot->m_length = pref.h;
				else
					pSlot->m_length = _paddedLimitedMatchingHeight(pSlot, m_contentBreadth);
				pSlot->m_prefBreadth = pref.w;
				ofs += pSlot->m_length;
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
		bool bSwallowed = _header()._receive(_pMsg);

		if( !bSwallowed )
		{
			switch( _pMsg->type() )
			{
				case MsgType::KeyPress:
				{
					if( m_selectMode == SelectMode::Unselectable )
						break;

					Key keyCode = KeyPressMsg::cast(_pMsg)->translatedKeyCode();
//					ModifierKeys	modKeys = KeyPressMsg::cast(_pMsg)->modKeys();
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
//					ModifierKeys	modKeys = KeyMsg::cast(_pMsg)->modKeys();
					if( (m_bHorizontal && (keyCode == Key::Left || keyCode == Key::Right)) ||
						(!m_bHorizontal && (keyCode == Key::Up || keyCode == Key::Down || keyCode == Key::PageUp || keyCode == Key::PageDown)) ||
						keyCode == Key::Home || keyCode == Key::End ||
						(m_selectMode == SelectMode::FlipOnSelect && keyCode == Key::Space ) )
						_pMsg->swallow();
					break;
				}
				default:
					break;
			}


			List::_receive( _pMsg );
		}
	}

	//____ _onLassoUpdated() ______________________________________________________

	void PackList::_onLassoUpdated( const RectI& oldLasso, const RectI& newLasso )
	{
		// Get out content area

		RectI listArea = _listArea();

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

			_flipSelection( slots._slot(beg), slots._slot(end), true );
		}
		else
		{
			if( oldFirst != newFirst )
			{
				int beg = wg::min(oldFirst,newFirst);
				int end = wg::max(oldFirst,newFirst);

				_flipSelection( slots._slot(beg), slots._slot(end), true );
			}

			if( oldLast != newLast )
			{
				int beg = wg::min(oldLast,newLast)+1;
				int end = wg::max(oldLast,newLast)+1;

				_flipSelection( slots._slot(beg), slots._slot(end), true );
			}
		}
	}

	//____ _didAddSlots() _____________________________________________________

	void PackList::_didAddSlots(StaticSlot * pSlot, int nb)
	{
		_unhideSlots(pSlot, nb);
	}

	//____ _didMoveSlots() _______________________________________________________

	void PackList::_didMoveSlots(StaticSlot * pFrom, StaticSlot * pTo, int nb)
	{
		StaticSlot * pBeg = min(pFrom, pTo);
		StaticSlot * pEnd = pFrom == pBeg ? pTo + 1 : pFrom + nb;

		_requestRenderChildren((PackListSlot*)pBeg, (PackListSlot*)pEnd);	// Request render on dirty area
	}


	//____ _willEraseSlots() _____________________________________________________

	void PackList::_willEraseSlots(StaticSlot * _pSlot, int nb)
	{
		_hideSlots(_pSlot, nb);

		// Unselect slots that will be removed.

		ListSlot * pSlot = (ListSlot*)_pSlot;
		ListSlot * pEnd = (ListSlot*)(((char *)_pSlot) + m_sizeOfSlot * nb);
		_setSlotSelection(pSlot, pEnd, false, true);
	}

	//____ _hideSlots() _____________________________________________________

	void PackList::_hideSlots(StaticSlot * _pSlot, int nb)
	{
		PackListSlot * pSlot = static_cast<PackListSlot*>(_pSlot);

		_requestRenderChildren(pSlot,slots._end());	// Request render on dirty area

		for (int i = 0; i < nb; i++)
		{
			if (pSlot[i].m_bVisible)
			{
				pSlot[i].m_bVisible = false;

				SizeI pref = _paddedLimitedPreferredSize(pSlot);
				if (m_bHorizontal)
					_subFromContentPreferredSize(pref.w, pref.h);
				else
					_subFromContentPreferredSize(pref.h, pref.w);

				m_contentLength -= pSlot[i].m_length;
				pSlot[i].m_length = 0;
			}
		}

		_updateChildOfsFrom(pSlot);
		_requestResize();
	}

	//____ _unhideSlots() _____________________________________________________

	void PackList::_unhideSlots(StaticSlot * _pSlot, int nb)
	{
		PackListSlot * pSlot = static_cast<PackListSlot*>(_pSlot);

		for (int i = 0; i < nb; i++)
		{
			if (!pSlot[i].m_bVisible)
			{
				pSlot[i].m_bVisible = true;

				Widget * pChild = pSlot[i]._widget();
				SizeI pref = _paddedLimitedPreferredSize(pSlot);

				if (m_bHorizontal)
				{
					_addToContentPreferredSize(pref.w, pref.h);

					// Get entry length and breadth

					if (pref.h == m_contentBreadth)
						pSlot[i].m_length = pref.w;
					else
						pSlot[i].m_length = _paddedLimitedMatchingWidth(pSlot, m_contentBreadth);
					pSlot[i].m_prefBreadth = pref.h;
				}
				else
				{
					_addToContentPreferredSize(pref.h, pref.w);

					// Get entry length and breadth

					if (pref.w == m_contentBreadth)
						pSlot[i].m_length = pref.h;
					else
						pSlot[i].m_length = _paddedLimitedMatchingHeight(pSlot, m_contentBreadth);
					pSlot[i].m_prefBreadth = pref.w;

				}

				m_contentLength += pSlot[i].m_length;

				RectI childGeo;
				_getChildGeo(childGeo, pSlot + i);
				pChild->_resize(childGeo);
			}
		}

		// Finish up

		_updateChildOfsFrom(pSlot);
		_requestRenderChildren(pSlot, slots._end());	// Request render on dirty area
		_requestResize();						// This should preferably be done first once we have changed the method.	}
	}

	//____ _selectSlots() ________________________________________________________

	void PackList::_selectSlots(StaticSlot * pSlot, int nb)
	{
		_setSlotSelection(static_cast<ListSlot*>(pSlot), static_cast<ListSlot*>(pSlot) + nb, true, false);
	}

	//____ _unselectSlots() ______________________________________________________

	void PackList::_unselectSlots(StaticSlot * pSlot, int nb)
	{
		_setSlotSelection(static_cast<ListSlot*>(pSlot), static_cast<ListSlot*>(pSlot) + nb, false, false);
	}


	//____ _beginSlots() _____________________________________________________

	ListSlot * PackList::_beginSlots() const
	{
		return slots._begin();
	}

	//____ _endSlots() _____________________________________________________

	ListSlot * PackList::_endSlots() const
	{
		return slots._end();
	}


	//____ _getEntryAt() __________________________________________________________

	// Pixelofs is counted from beginning of container content, not widget.

	int PackList::_getEntryAt( int pixelofs ) const
	{
		int first = 0;
		int last = slots.size() - 1;
		int middle = (first+last)/2;

		while( first <= last )
		{
			PackListSlot * pSlot = slots._slot(middle);

			if( pSlot->m_ofs + pSlot->m_length < pixelofs )
				first = middle + 1;
			else if( pSlot->m_ofs <= pixelofs )
				return middle;
			else
				last = middle - 1;

			middle = (first + last)/2;
		}

		return slots.size();
	}

	//____ _getInsertionPoint() ___________________________________________________

	int PackList::_getInsertionPoint( const Widget * pWidget ) const
	{
		int first = 0;
		int last = slots.size() - 1;
		int middle = (first+last)/2;
	 	int negator = m_sortOrder == SortOrder::Ascending ? 1 : -1;

		while( first <= last )
		{
			PackListSlot * pSlot = slots._slot(middle);

			int cmpRes = m_sortFunc( pSlot->_widget(), pWidget )*negator;

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

	ListSlot * PackList::_findEntry( const CoordI& ofs )
	{
		RectI list = _listArea();

		if( list.contains(ofs) && _listWindow().contains(ofs) )
		{
			int entry;
			if( m_bHorizontal )
				entry = _getEntryAt(ofs.x-list.x);
			else
				entry = _getEntryAt(ofs.y-list.y);

			if( entry != slots.size() )
				return slots._slot(entry);
		}

		return 0;
	}

	//____ _findWidget() __________________________________________________________

	Widget * PackList::_findWidget( const CoordI& ofs, SearchMode mode )
	{
		Widget * pResult = 0;
		RectI list = _listArea();

		if( list.contains(ofs) && _listWindow().contains(ofs) )
		{
			int entry;
			if( m_bHorizontal )
				entry = _getEntryAt(ofs.x-list.x);
			else
				entry = _getEntryAt(ofs.y-list.y);

			if( entry != slots.size() )
			{
				PackListSlot * pSlot = slots._slot(entry);
				RectI childGeo;
				_getChildGeo( childGeo, pSlot );
				if( childGeo.contains(ofs) )
				{
					if( pSlot->_widget()->isContainer() )
					{
						pResult = static_cast<Container*>(pSlot->_widget())->_findWidget( ofs - childGeo.pos(), mode );
					}
					else if( mode == SearchMode::Geometry || pSlot->_markTest( ofs - childGeo.pos() ) )
					{
							pResult = pSlot->_widget();
					}
				}

				if( !pResult && mode == SearchMode::ActionTarget )
					pResult = pSlot->_widget();						// Entries are opaque as action targets.

			}
		}

		// Check against ourselves

		if( !pResult && ( mode == SearchMode::Geometry || _markTest(ofs)) )
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
				for( PackListSlot * p = slots._begin() ; p < slots._end() ; p++ )
				{
					if (p->m_prefBreadth == breadth)
						continue;

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


	//____ _requestRenderChildren() ___________________________________________

	void PackList::_requestRenderChildren(PackListSlot * pBegin, PackListSlot * pEnd)
	{
		RectI box = _listArea();

		int beg = pBegin->m_ofs;
		int end = pEnd < slots._end() ? pEnd->m_ofs + pEnd->m_length : m_contentLength;


		if (m_bHorizontal)
		{
			box.x += beg;
			box.w = end - beg;
		}
		else
		{
			box.y += beg;
			box.h = end - beg;
		}

		_requestRender(box);
	}


	//____ _updateChildOfsFrom() __________________________________________________

	void PackList::_updateChildOfsFrom( PackListSlot * pSlot )
	{
		int ofs = 0;
		if( pSlot > slots._begin() )
			ofs = pSlot[-1].m_ofs + pSlot[-1].m_length;

		while( pSlot < slots._end() )
		{
			pSlot->m_ofs = ofs;
			ofs += pSlot->m_length;
			pSlot++;
		}
	}

	//____ _onEntrySkinChanged() __________________________________________________

	void PackList::_onEntrySkinChanged( SizeI oldPadding, SizeI newPadding )
	{
		_requestRender();

		if( oldPadding != newPadding )
		{
			m_entryPadding = newPadding;
			int nEntries = slots.size();

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

	void PackList::_getEntryGeo( RectI& geo, const ListSlot * _pSlot ) const
	{
		const PackListSlot * pSlot = static_cast<const PackListSlot*>(_pSlot);

		geo = _listArea();

		if( m_bHorizontal )
		{
			geo.x += pSlot->m_ofs;
			geo.w = pSlot->m_length;
		}
		else
		{
			geo.y += pSlot->m_ofs;
			geo.h = pSlot->m_length;
		}
	}


	//____ _getChildGeo() _________________________________________________________

	void PackList::_getChildGeo( RectI& geo, const PackListSlot * pSlot ) const
	{
		geo = _listArea();

		if( m_bHorizontal )
		{
			geo.x += pSlot->m_ofs;
			geo.w = pSlot->m_length;
		}
		else
		{
			geo.y += pSlot->m_ofs;
			geo.h = pSlot->m_length;
		}

		// Apply any padding from the entry skin, if entry visible

		if( pSlot->m_bVisible )
		{
			int index = slots._index( pSlot );
			if( m_pEntrySkin[index&0x1] )
				geo = m_pEntrySkin[index&0x1]->_contentRect( geo, pSlot->_widget()->state() );
		}
	}

	//____ _paddedLimitedMatchingHeight() _________________________________________

	int PackList::_paddedLimitedMatchingHeight( StaticSlot * _pSlot, int paddedWidth )
	{
		auto pSlot = static_cast<PackListSlot*>(_pSlot);

		int height = pSlot->_matchingHeight( paddedWidth - m_entryPadding.w ) + m_entryPadding.h;
		limit( height, m_minEntrySize.h, m_maxEntrySize.h );
		return height;
	}

	//____ _paddedLimitedMatchingWidth() _________________________________________

	int PackList::_paddedLimitedMatchingWidth( StaticSlot * _pSlot, int paddedHeight )
	{
		auto pSlot = static_cast<PackListSlot*>(_pSlot);

		int width = pSlot->_matchingWidth( paddedHeight - m_entryPadding.h ) + m_entryPadding.w;
		limit( width, m_minEntrySize.w, m_maxEntrySize.w );
		return width;
	}

	//____ _paddedLimitedPreferredSize() __________________________________________

	SizeI PackList::_paddedLimitedPreferredSize( StaticSlot * _pSlot )
	{
		auto pSlot = static_cast<PackListSlot*>(_pSlot);

		SizeI sz = pSlot->_preferredSize();
		sz += m_entryPadding;

		// Apply limits

		if( sz.w < m_minEntrySize.w )
			sz.w = m_minEntrySize.w;
		if( sz.h < m_minEntrySize.h )
			sz.h = m_minEntrySize.h;

		if( sz.w > m_maxEntrySize.w )
		{
			int h = pSlot->_matchingHeight(m_maxEntrySize.w-m_entryPadding.w) + m_entryPadding.h;
			limit(h, m_minEntrySize.h, m_maxEntrySize.h );
		}
		else if( sz.h > m_maxEntrySize.h )
		{
			int w = pSlot->_matchingWidth(m_maxEntrySize.h-m_entryPadding.h) + m_entryPadding.w;
			limit(w, m_minEntrySize.w, m_maxEntrySize.w );
		}

		return sz;
	}

	//____ _childPos() ________________________________________________________

	CoordI PackList::_childPos( const StaticSlot * _pSlot ) const
	{
		const PackListSlot * pSlot = reinterpret_cast<const PackListSlot*>(_pSlot);

		RectI geo;
		_getChildGeo(geo, pSlot);

		return geo.pos();
	}

	//____ _childRequestRender() _________________________________________________

	void PackList::_childRequestRender( StaticSlot * _pSlot )
	{
		PackListSlot * pSlot = reinterpret_cast<PackListSlot*>(_pSlot);

		RectI geo;
		_getChildGeo(geo, pSlot);
		_requestRender(geo);
	}

	void PackList::_childRequestRender( StaticSlot * _pSlot, const RectI& rect )
	{
		PackListSlot * pSlot = reinterpret_cast<PackListSlot*>(_pSlot);

		RectI geo;
		_getChildGeo(geo, pSlot);
		geo.x += rect.x;
		geo.y += rect.y;
		geo.w = rect.w;
		geo.h = rect.h;
		_requestRender(geo);
	}

	//____ _childRequestResize() _________________________________________________

	void PackList::_childRequestResize( StaticSlot * _pSlot )
	{
		bool	bReqResize = false;

		PackListSlot * pSlot = reinterpret_cast<PackListSlot*>(_pSlot);

		if (pSlot->m_bVisible && m_minEntrySize != m_maxEntrySize)
		{
			SizeI prefEntrySize = _paddedLimitedPreferredSize(pSlot);

			int prefLength = m_bHorizontal ? prefEntrySize.w : prefEntrySize.h;
			int prefBreadth = m_bHorizontal ? prefEntrySize.h : prefEntrySize.w;

			// Update preferred sizes

			if( prefBreadth != pSlot->m_prefBreadth || prefLength != pSlot->m_length )
			{
				// NOTE: Order here is important!

				_addToContentPreferredSize( prefLength, prefBreadth );
				int oldPrefBreadth = pSlot->m_prefBreadth;
				pSlot->m_prefBreadth = prefBreadth;
				_subFromContentPreferredSize( pSlot->m_length, oldPrefBreadth );

				bReqResize = true;
			}

			// Calculate new length

			int length;
			if( prefBreadth == m_contentBreadth )
				length = prefLength;
			else
				length = m_bHorizontal ? _paddedLimitedMatchingWidth(pSlot, m_contentBreadth ) : _paddedLimitedMatchingHeight(pSlot, m_contentBreadth );

			// Update if length has changed

			if( length != pSlot->m_length )
			{
				m_contentLength += length - pSlot->m_length;
				pSlot->m_length = length;
				bReqResize = true;

				_updateChildOfsFrom( pSlot );
				_requestRenderChildren( pSlot, slots._end() );

				RectI childGeo;
				_getChildGeo(childGeo,pSlot);
				pSlot->_widget()->_resize(childGeo);
			}
		}

		if( bReqResize )
			_requestResize();

		// Always set size on request since our _resize() won't do it unless breadth has changed.

		RectI geo;
		_getChildGeo(geo, pSlot);
		pSlot->_setSize(geo.size());
	}

	//____ _prevChild() __________________________________________________________

	Widget * PackList::_prevChild( const StaticSlot * _pSlot ) const
	{
		auto pSlot = static_cast<const PackListSlot*>(_pSlot);

		if (pSlot > slots._begin())
			return pSlot[-1]._widget();

		return nullptr;
	}

	//____ _nextChild() __________________________________________________________

	Widget * PackList::_nextChild( const StaticSlot * _pSlot ) const
	{
		auto pSlot = static_cast<const PackListSlot*>(_pSlot);

		if (pSlot < slots._last())
			return pSlot[1]._widget();

		return nullptr;
	}

	//____ _releaseChild() ____________________________________________________

	void PackList::_releaseChild(StaticSlot * pSlot)
	{
		_willEraseSlots(pSlot, 1);
		slots._erase(static_cast<PackListSlot*>(pSlot));
	}

	//____ _replaceChild() _____________________________________________________

	void PackList::_replaceChild(StaticSlot * pSlot, Widget * pNewChild)
	{
		static_cast<PackListSlot*>(pSlot)->_setWidget(pNewChild);
		_childRequestResize(pSlot);
	}

	//____ _firstSlotWithGeo() ____________________________________________________

	void PackList::_firstSlotWithGeo( SlotWithGeo& package ) const
	{
		if (slots.isEmpty())
			package.pSlot = nullptr;
		else
		{
			PackListSlot * pSlot = slots._first();
			package.pSlot = pSlot;
			_getChildGeo(package.geo, pSlot);
		}
	}

	//____ _nextSlotWithGeo() _____________________________________________________

	void PackList::_nextSlotWithGeo( SlotWithGeo& package ) const
	{
		PackListSlot * pSlot = (PackListSlot*)package.pSlot;

		if (pSlot == slots._last())
			package.pSlot = nullptr;
		else
		{
			pSlot++;
			package.pSlot = pSlot;
			_getChildGeo(package.geo, pSlot);
		}
	}


	//____ _listArea() ____________________________________________________________

	RectI PackList::_listArea() const
	{
		RectI r = _listCanvas();
		if( m_pSkin )
			r = m_pSkin->_contentRect( r, m_state );

		return r;
	}

	//____ _listWindow() ____________________________________________________________

	RectI PackList::_listWindow() const
	{
		RectI r = _windowSection();		// Window in widgets own coordinate system.

		if( m_bHorizontal )
		{
			r.x += _header()._size().w;
			r.w -= _header()._size().w;
		}
		else
		{
			r.y += _header()._size().h;
			r.h -= _header()._size().h;
		}
		return r;
	}

	//____ _listCanvas() ____________________________________________________________

	RectI PackList::_listCanvas() const
	{
		SizeI headerSize = _header()._size();

		if( m_bHorizontal )
			return RectI(headerSize.w, 0, m_size.w - headerSize.w, m_size.h );
		else
			return RectI(0, headerSize.h, m_size.w, m_size.h - headerSize.h);	// List canvas in widgets own coordinate system.
	}

	//____ _headerGeo() ___________________________________________________________

	RectI PackList::_headerGeo() const
	{
		if( m_bHorizontal )
			return RectI( _windowSection().x, 0, _header()._size().w, m_size.h );
		else
			return RectI( 0, _windowSection().y, m_size.w, _header()._size().h );
	}

	//____ _windowPadding() _______________________________________________________

	SizeI PackList::_windowPadding() const
	{
		if( m_bHorizontal )
			return SizeI( _header()._size().w, 0 );
		else
			return SizeI( 0, _header()._size().h );
	}

	//____ _refreshHeader() _______________________________________________________

	void PackList::_refreshHeader()
	{
		SizeI wantedSize = _header()._preferredSize();
		SizeI currentSize = _header()._size();

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

		if( slots.isEmpty() )
			return true;

		// Create a temporary, sorted list of pointers to slots

		int listSize = sizeof(int) * slots.size();
		int * pOrderList = (int*) Base::memStackAlloc( listSize );
		int negator = m_sortOrder == SortOrder::Ascending ? 1 : -1;

		pOrderList[0] = 0;
		for( int entry = 1 ; entry < slots.size() ; entry++ )
		{
			Widget * pWidget = slots._slot(entry)->_widget();

			int first = 0;
			int last = entry-1;
			int middle = (first+last)/2;

			while( first <= last )
			{
				Widget * pEntry = slots._slot(pOrderList[middle])->_widget();

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

		slots._reorder( pOrderList );
		Base::memStackRelease( listSize );

		// Update ofs in the slots

		_updateChildOfsFrom( slots._begin() );
		_requestRenderChildren( slots._begin(), slots._end() );	// Request render on dirty area
		return true;
	}

	//____ _componentPos() ____________________________________________________________

	CoordI PackList::_componentPos( const GeoComponent * pComponent ) const
	{
		CoordI c = m_bHorizontal ? CoordI(_windowSection().x, 0 ) : CoordI(0, _windowSection().y );
		return c;
	}

	//____ _componentSize() _________________________________________________________

	SizeI PackList::_componentSize( const GeoComponent * pComponent ) const
	{
		return _header()._size();		// We store size internally in the _header().
	}

	//____ _componentGeo() __________________________________________________________

	RectI PackList::_componentGeo( const GeoComponent * pComponent ) const
	{
		return RectI( _componentPos(pComponent), _header()._size() );
	}

	//____ _receiveComponentNotif() _____________________________________________________

	void PackList::_receiveComponentNotif( GeoComponent * pComponent, ComponentNotif notification, int value, void * pData )
	{
		if( notification == ComponentNotif::SortOrderChanged )
		{
			m_sortOrder = _header()._sortOrder();
			_sortEntries();
		}
	}


} // namespace wg

