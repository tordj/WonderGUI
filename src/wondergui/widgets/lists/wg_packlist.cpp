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

#include <wg_dynamicslotvector.impl.h>

namespace wg
{
	using namespace Util;

	template class DynamicSlotVector<PackList::Slot>;

	const TypeInfo PackList::TYPEINFO = { "PackList", &List::TYPEINFO };
	const TypeInfo PackList::Slot::TYPEINFO = { "PackList::Slot", &List::Slot::TYPEINFO };


	//____ Slot::_setBlueprint() ________________________________________

	bool PackList::Slot::_setBlueprint( const Blueprint& bp )
	{
		m_bVisible = bp.visible;
		return true;
	}


	//____ constructor ____________________________________________________________

	PackList::PackList() : header(this), slots(this)
	{
		m_sizeOfSlot = sizeof(Slot);
		m_bSiblingsOverlap = false;
		m_bHorizontal = false;
		m_sortOrder = SortOrder::Ascending;
		_header()._setSortOrder( SortOrder::Ascending );

		m_contentBreadth = 0;
		m_contentLength = 0;
		m_contentDefaultLength = 0;
		m_contentDefaultBreadth = 0;
		m_nbEntriesOfDefaultBreadth = 0;
	}

	//____ Destructor _____________________________________________________________

	PackList::~PackList()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& PackList::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setAxis() _______________________________________________________

	void PackList::setAxis( Axis axis )
	{
		bool bHorizontal = (axis == Axis::X);

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
			sortSlots();
			_requestRender();		// So we also render the header, which has an arrow with new state.
		}
	}

	//____ setSortFunction() ______________________________________________________

	void PackList::setSortFunction( std::function<int(const Widget *,const Widget *)> func )
	{
		m_sortFunc = func;
		sortSlots();
	}

	//____ insertSorted() ___________________________________________________

	PackList::iterator PackList::insertSorted(Widget * pWidget)
	{
		//TODO: Replace with assert
	//		if (!pWidget)
	//			return false;

		if (slots.isEmpty() || !_hasSortFunction())
			return slots.pushBack(pWidget);

		int index = _getInsertionPoint(pWidget);
		slots.insert(index, pWidget);
		return &slots[index];
	}

	//____ setMinEntrySize() ______________________________________________________

	bool PackList::setMinEntrySize( Size min )
	{
		if( min == m_minEntrySize )
			return true;

		if( min.w > m_maxEntrySize.w || min.h > m_maxEntrySize.h )
			return false;

		m_minEntrySize = min;
		m_minEntrySizeSPX = align(ptsToSpx(min, m_scale));
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
		m_maxEntrySizeSPX = align(ptsToSpx(max, m_scale));
		_refreshList();
		return true;
	}


	//____ select() ___________________________________________________________

	void PackList::selectSlots(int index, int amount)
	{
		//TODO: Error handling!

		_selectSlots( &slots[index], amount);
	};

	void PackList::selectSlots(iterator beg, iterator end)
	{
		//TODO: Error handling!

		_selectSlots(beg, int(end - beg));
	};


	//____ unselect() _________________________________________________________

	void PackList::unselectSlots(int index, int amount)
	{
		//TODO: Error handling!

		_unselectSlots( &slots[index], amount);
	};

	void PackList::unselectSlots(iterator beg, iterator end)
	{
		//TODO: Error handling!

		_unselectSlots(beg, int(end - beg));
	};


	//____ _defaultSize() ________________________________________________________

	SizeSPX PackList::_defaultSize(int scale) const
	{
		if( scale == m_scale )
		{
			SizeSPX sz = m_skin.contentBorderSize(scale);
			SizeSPX headerSize = _header()._defaultSize(scale);

			if (m_bHorizontal)
			{
				sz += SizeSPX(m_contentDefaultLength + headerSize.w, m_contentDefaultBreadth);
				if (headerSize.h > sz.h)
					sz.h = headerSize.h;
			}
			else
			{
				sz += SizeSPX(m_contentDefaultBreadth, m_contentDefaultLength + headerSize.h);
				if (headerSize.w > sz.w)
					sz.w = headerSize.w;
			}
			return sz;
		}
		else
		 return _calcDefaultSize(scale);


	}

	//____ _matchingHeight() _______________________________________________________

	spx PackList::_matchingHeight(spx width, int scale) const
	{
		//TODO: Support other scale than what we already have!
		assert(scale == m_scale);

		if (m_bHorizontal)
		{
			spx height = m_contentDefaultBreadth + m_skin.contentBorderSize(scale).h;
			return std::max(height, _header()._defaultSize(scale).h);
		}
		else
		{
			spx height = _header()._matchingHeight(width, scale);
			SizeSPX pad = m_skin.contentBorderSize(scale);
			width -= pad.w;
			height += pad.h;

			width -= m_entryPadding.w;

			for (auto pSlot = slots.begin(); pSlot < slots.end(); pSlot++)
				height += pSlot->_widget()->_matchingHeight(width,scale);

			height += m_entryPadding.h*slots.size();
			return height;
		}
	}

	//____ _matchingWidth() _______________________________________________________

	spx PackList::_matchingWidth(spx height, int scale) const
	{
		//TODO: Support other scale than what we already have!
		assert(scale == m_scale);

		if (m_bHorizontal)
		{
			spx width = _header()._matchingWidth(height,scale);
			SizeSPX pad = m_skin.contentBorderSize(scale);
			height -= pad.w;
			width += pad.h;

			height -= m_entryPadding.h;

			for (auto pSlot = slots.begin(); pSlot < slots.end(); pSlot++)
				width += pSlot->_widget()->_matchingWidth(width, scale);

			width += m_entryPadding.w*slots.size();
			return width;
		}
		else
		{
			spx width = m_contentDefaultBreadth + m_skin.contentBorderSize(scale).w;

			return std::max(width, _header()._defaultSize(scale).w);
		}
	}


	//____ _collectPatches() ____________________________________________________

	void PackList::_collectPatches( PatchesSPX& container, const RectSPX& geo, const RectSPX& clip )
	{
		if( m_skin.isEmpty() )
		{
			if (m_bHorizontal)
				container.add(RectSPX::overlap(RectSPX(geo.x, geo.y, std::min(geo.w, m_contentLength), geo.h), clip));
			else
				container.add(RectSPX::overlap(RectSPX(geo.x, geo.y, geo.w, std::min(geo.h, m_contentLength)), clip));
		}
		else
			container.add( RectSPX::overlap( geo, clip ) );
	}

	//____ _maskPatches() _______________________________________________________

	void PackList::_maskPatches( PatchesSPX& patches, const RectSPX& geo, const RectSPX& clip, BlendMode blendMode )
	{
		if( (m_bOpaque && blendMode == BlendMode::Blend) || blendMode == BlendMode::Replace)
			patches.sub( RectSPX::overlap(geo,clip) );
		else if( m_bOpaqueEntries && blendMode == BlendMode::Blend )
		{
			if( m_bHorizontal )
				patches.sub( RectSPX::overlap( RectSPX( geo.x, geo.y, std::min(geo.w,m_contentLength), geo.h ), clip ) );
			else
				patches.sub( RectSPX::overlap( RectSPX( geo.x, geo.y, geo.w, std::min(geo.h,m_contentLength) ), clip ) );
		}
		else
		{
			RectSPX	myClip = RectSPX::overlap(geo, clip);				// Need to limit clip to our geo. Otherwise children outside might mask what they shouldn't.
			SlotWithGeo child;
			_firstSlotWithGeo( child );

			while(child.pSlot)
			{
				static_cast<const Slot*>(child.pSlot)->_widget()->_maskPatches( patches, child.geo + geo.pos(), myClip, blendMode );
				_nextSlotWithGeo( child );
			}
		}

	}

	//____ _render() _______________________________________________________

	void PackList::_render( GfxDevice * pDevice, const RectSPX& _canvas, const RectSPX& _window )
	{
		// Render container itself

		RectSPX contentRect = _listCanvas() + _canvas.pos();

		RectSPX clip = pDevice->clipBounds();

		m_skin.render( pDevice, contentRect, m_scale, m_state );
		contentRect = m_skin.contentRect( contentRect, m_scale, m_state );

		spx startOfs = m_bHorizontal ? clip.x-contentRect.x : clip.y-contentRect.y;
		if( startOfs < 0 )
			startOfs = 0;

		for( int i = _getEntryAt( startOfs ) ; i < slots.size() ; i++ )
		{
			Slot * pSlot = &slots[i];
			Widget * pChild = pSlot->_widget();

			// Get entry geometry, skin and state

			RectSPX entryGeo( contentRect );
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
				pEntrySkin->_render( pDevice, entryGeo, m_scale, state );
				//            childGeo = pEntrySkin->_contentRect( entryGeo, state );
			}

			// Render child

			//        pChild->_render( pDevice, childGeo, childGeo, _clip );

		}

		// Render children

		RectSPX	dirtBounds = pDevice->clipBounds();

		{
			SlotWithGeo child;
			_firstSlotWithGeo( child );

			while(child.pSlot)
			{
				RectSPX canvas = child.geo + _canvas.pos();
				if (canvas.isOverlapping(dirtBounds))
				{
					ClipPopData clipPop = limitClipList(pDevice, canvas );
					if( pDevice->clipListSize() > 0 )
						static_cast<const Slot*>(child.pSlot)->_widget()->_render(pDevice, canvas, canvas);
					popClipList(pDevice,clipPop);
				}
				_nextSlotWithGeo( child );
			}
		}

		// Render header

		if( _header()._size().h != 0 )
		{
//			bool bInvertedSort = (m_sortOrder == SortOrder::Descending);
			RectSPX canvas = _headerGeo() + _canvas.pos();

			_header()._render( pDevice, canvas );
		}

		// Render Lasso

		if( !m_lassoSkin.isEmpty() && m_lassoBegin != m_lassoEnd )
		{
			RectSPX lassoRect( m_lassoBegin, m_lassoEnd );
			lassoRect += _canvas.pos();

			m_lassoSkin.render(pDevice, lassoRect, m_scale, m_state );
		}
	}

	//____ _resize() ___________________________________________________________

	void PackList::_resize( const SizeSPX& _size, int scale )
	{
		//TODO: Recalc MORE stuff if scale changes.

		if (scale != m_scale)
		{
			m_minEntrySizeSPX = align(ptsToSpx(m_minEntrySize, scale));
			m_maxEntrySizeSPX = align(ptsToSpx(m_maxEntrySize, scale));
			
			if( m_pEntrySkin[0] )
				m_entryPadding = m_pEntrySkin[0]->_contentBorderSize(scale);
			
			m_contentDefaultLength = 0;
			m_contentDefaultBreadth = 0;
			m_nbEntriesOfDefaultBreadth = 0;

			for (auto pSlot = slots.begin(); pSlot < slots.end(); pSlot++)
			{
				SizeSPX pref = _paddedLimitedDefaultSize( pSlot, scale );

				if( m_bHorizontal )
				{
					_addToContentDefaultSize( pref.w, pref.h );
					pSlot->m_prefBreadth = pref.h;
				}
				else
				{
					_addToContentDefaultSize( pref.h, pref.w );
					pSlot->m_prefBreadth = pref.w;
				}
			}
		}

		//

		List::_resize(_size, scale);

		SizeSPX headerSize = m_bHorizontal ? SizeSPX(_header()._matchingWidth(_size.h,m_scale), _size.h) : SizeSPX( _size.w, _header()._matchingHeight( _size.w, m_scale ));
		_header()._setSize( headerSize, m_scale );

		SizeSPX size = _size - m_skin.contentBorderSize(m_scale);

		spx newContentBreadth;

		if( m_bHorizontal )
			newContentBreadth = size.h;
		else
			newContentBreadth = size.w;

		if( newContentBreadth != m_contentBreadth )
		{
			m_contentBreadth = newContentBreadth;
			spx ofs = 0;

			for( auto pSlot = slots.begin() ; pSlot < slots.end() ; pSlot++ )
			{
				Widget * pWidget = pSlot->_widget();

				SizeSPX newEntrySize;
				spx newEntryLength;

				if( m_bHorizontal )
				{
					newEntryLength = _paddedLimitedMatchingWidth(pSlot, newContentBreadth );
					newEntrySize = SizeSPX(newEntryLength, newContentBreadth);
				}
				else
				{
					newEntryLength = _paddedLimitedMatchingHeight(pSlot, newContentBreadth );
					newEntrySize = SizeSPX(newContentBreadth, newEntryLength);
				}
				
				pSlot->m_ofs = ofs;
				pSlot->m_length = newEntryLength;
				ofs += newEntryLength;

				pWidget->_resize( newEntrySize, m_scale );
			}
			m_contentLength = ofs;
		}

		_requestRender();
	}

	//____ _calcDefaultSize() __________________________________________________

	SizeSPX PackList::_calcDefaultSize( int scale ) const
	{
		SizeSPX entryPadding;
		
		if( m_pEntrySkin[0] )
			entryPadding = m_pEntrySkin[0]->_contentBorderSize(scale);

		SizeSPX 	defaultSize;

		SizeSPX		minEntrySize = align(ptsToSpx(m_minEntrySize, scale));
		SizeSPX		maxEntrySize = align(ptsToSpx(m_maxEntrySize, scale));

		
		for (auto pSlot = slots.begin(); pSlot < slots.end(); pSlot++)
		{
			// Calc "_paddedLimitedDefaultSize" for slot
		
			SizeSPX sz = pSlot->_widget()->_defaultSize(scale);
			sz += entryPadding;

			// Apply limits

			if( sz.w < minEntrySize.w )
				sz.w = minEntrySize.w;
			if( sz.h < minEntrySize.h )
				sz.h = minEntrySize.h;

			if( sz.w > maxEntrySize.w )
			{
				sz.h = pSlot->_widget()->_matchingHeight(maxEntrySize.w-entryPadding.w, scale) + entryPadding.h;
				limit(sz.h, minEntrySize.h, maxEntrySize.h );
			}
			else if( sz.h > maxEntrySize.h )
			{
				sz.w = pSlot->_widget()->_matchingWidth(maxEntrySize.h-entryPadding.h, scale) + entryPadding.w;
				limit(sz.w, minEntrySize.w, maxEntrySize.w );
			}
			
			SizeSPX pref = sz;

			//
			
			if( m_bHorizontal )
			{
				defaultSize.w += pref.w;
				if( pref.h > defaultSize.h )
					defaultSize.h = pref.h;
			}
			else
			{
				defaultSize.h += pref.h;
				if( pref.w > defaultSize.w )
					defaultSize.w = pref.w;
			}
		}
		return defaultSize;
	}


	//____ _refreshList() _______________________________________________________

	void PackList::_refreshList()
	{
		if( m_pEntrySkin[0] )
			m_entryPadding = m_pEntrySkin[0]->_contentBorderSize(m_scale);

		m_contentDefaultLength = 0;
		m_contentDefaultBreadth = 0;
		m_nbEntriesOfDefaultBreadth = 0;
		spx ofs = 0;

		for (auto pSlot = slots.begin(); pSlot < slots.end(); pSlot++)
		{
			SizeSPX pref = _paddedLimitedDefaultSize( pSlot, m_scale );

			if( m_bHorizontal )
			{
				_addToContentDefaultSize( pref.w, pref.h );
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
				_addToContentDefaultSize( pref.h, pref.w );
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

					Key keyCode = static_cast<KeyPressMsg*>(_pMsg)->translatedKeyCode();
//					ModKeys	modKeys = KeyPressMsg::cast(_pMsg)->modKeys();
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

					Key keyCode = static_cast<KeyMsg*>(_pMsg)->translatedKeyCode();
//					ModKeys	modKeys = KeyMsg::cast(_pMsg)->modKeys();
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

	void PackList::_onLassoUpdated( const RectSPX& oldLasso, const RectSPX& newLasso )
	{
		// Get out content area

		RectSPX listArea = _listArea();

		// Check if our lassos are inside content area or not.

		bool	bOldLassoInside = false;
		bool	bNewLassoInside = false;

		if( oldLasso.isOverlapping(listArea ) )
			bOldLassoInside = true;
		if( newLasso.isOverlapping(listArea ) )
			bNewLassoInside = true;

		if( !bOldLassoInside && !bNewLassoInside )
			return;										// None of the lassos inside content.

		// Get first/last entries marked by old/new lasso

		spx oldOfs1, oldOfs2;
		spx newOfs1, newOfs2;


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

			_flipSelection( &slots[beg], &slots[end], true );
		}
		else
		{
			if( oldFirst != newFirst )
			{
				int beg = std::min(oldFirst,newFirst);
				int end = std::max(oldFirst,newFirst);

				_flipSelection( &slots[beg], &slots[end], true );
			}

			if( oldLast != newLast )
			{
				int beg = std::min(oldLast,newLast)+1;
				int end = std::max(oldLast,newLast)+1;

				_flipSelection( &slots[beg], &slots[end], true );
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
		StaticSlot * pBeg = std::min(pFrom, pTo);
		StaticSlot * pEnd = pFrom == pBeg ? pTo + 1 : pFrom + nb;

		_requestRenderChildren((Slot*)pBeg, (Slot*)pEnd);	// Request render on dirty area
	}


	//____ _willEraseSlots() _____________________________________________________

	void PackList::_willEraseSlots(StaticSlot * _pSlot, int nb)
	{
		_hideSlots(_pSlot, nb);

		// Unselect slots that will be removed.

		List::Slot * pSlot = (List::Slot*)_pSlot;
		List::Slot * pEnd = (List::Slot*)(((char *)_pSlot) + m_sizeOfSlot * nb);
		_setSlotSelection(pSlot, pEnd, false, true);
	}

	//____ _hideSlots() _____________________________________________________

	void PackList::_hideSlots(StaticSlot * _pSlot, int nb)
	{
		Slot * pSlot = static_cast<Slot*>(_pSlot);

		_requestRenderChildren(pSlot,slots.end());	// Request render on dirty area

		for (int i = 0; i < nb; i++)
		{
			if (pSlot[i].m_bVisible)
			{
				pSlot[i].m_bVisible = false;

				SizeSPX pref = _paddedLimitedDefaultSize(pSlot, m_scale);
				if (m_bHorizontal)
					_subFromContentDefaultSize(pref.w, pref.h);
				else
					_subFromContentDefaultSize(pref.h, pref.w);

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
		Slot * pSlot = static_cast<Slot*>(_pSlot);

		for (int i = 0; i < nb; i++)
		{
			if (!pSlot[i].m_bVisible)
			{
				pSlot[i].m_bVisible = true;

				Widget * pChild = pSlot[i]._widget();
				SizeSPX pref = _paddedLimitedDefaultSize(pSlot, m_scale);

				if (m_bHorizontal)
				{
					_addToContentDefaultSize(pref.w, pref.h);

					// Get entry length and breadth

					if (pref.h == m_contentBreadth)
						pSlot[i].m_length = pref.w;
					else
						pSlot[i].m_length = _paddedLimitedMatchingWidth(pSlot, m_contentBreadth);
					pSlot[i].m_prefBreadth = pref.h;
				}
				else
				{
					_addToContentDefaultSize(pref.h, pref.w);

					// Get entry length and breadth

					if (pref.w == m_contentBreadth)
						pSlot[i].m_length = pref.h;
					else
						pSlot[i].m_length = _paddedLimitedMatchingHeight(pSlot, m_contentBreadth);
					pSlot[i].m_prefBreadth = pref.w;

				}

				m_contentLength += pSlot[i].m_length;

				RectSPX childGeo;
				_getChildGeo(childGeo, pSlot + i);
				pChild->_resize(childGeo, m_scale);
			}
		}

		// Finish up

		_updateChildOfsFrom(pSlot);
		_requestRenderChildren(pSlot, slots.end());	// Request render on dirty area
		_requestResize();						// This should preferably be done first once we have changed the method.	}
	}

	//____ _selectSlots() ________________________________________________________

	void PackList::_selectSlots(StaticSlot * pSlot, int nb)
	{
		_setSlotSelection(static_cast<List::Slot*>(pSlot), static_cast<List::Slot*>(pSlot) + nb, true, false);
	}

	//____ _unselectSlots() ______________________________________________________

	void PackList::_unselectSlots(StaticSlot * pSlot, int nb)
	{
		_setSlotSelection(static_cast<List::Slot*>(pSlot), static_cast<List::Slot*>(pSlot) + nb, false, false);
	}


	//____ _beginSlots() _____________________________________________________

	List::List::Slot * PackList::_beginSlots() const
	{
		return slots.begin();
	}

	//____ _endSlots() _____________________________________________________

	List::List::Slot * PackList::_endSlots() const
	{
		return slots.end();
	}


	//____ _getEntryAt() __________________________________________________________

	// ofs is counted from beginning of container content, not widget.

	int PackList::_getEntryAt( spx ofs ) const
	{
		int first = 0;
		int last = slots.size() - 1;
		int middle = (first+last)/2;

		while( first <= last )
		{
			const Slot * pSlot = &slots[middle];

			if( pSlot->m_ofs + pSlot->m_length < ofs )
				first = middle + 1;
			else if( pSlot->m_ofs <= ofs )
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
			const Slot * pSlot = &slots[middle];

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

	List::List::Slot * PackList::_findEntry( const CoordSPX& ofs )
	{
		RectSPX list = _listArea();

		if( list.contains(ofs) && _listWindow().contains(ofs) )
		{
			int entry;
			if( m_bHorizontal )
				entry = _getEntryAt(ofs.x-list.x);
			else
				entry = _getEntryAt(ofs.y-list.y);

			if( entry != slots.size() )
				return &slots[entry];
		}

		return 0;
	}

	//____ _findWidget() __________________________________________________________

	Widget * PackList::_findWidget( const CoordSPX& ofs, SearchMode mode )
	{
		Widget * pResult = 0;
		RectSPX list = _listArea();

		if( list.contains(ofs) && _listWindow().contains(ofs) )
		{
			int entry;
			if( m_bHorizontal )
				entry = _getEntryAt(ofs.x-list.x);
			else
				entry = _getEntryAt(ofs.y-list.y);

			if( entry != slots.size() )
			{
				Slot * pSlot = &slots[entry];
				RectSPX childGeo;
				_getChildGeo( childGeo, pSlot );
				if( childGeo.contains(ofs) )
				{
					if( pSlot->_widget()->isContainer() )
					{
						pResult = static_cast<Container*>(pSlot->_widget())->_findWidget( ofs - childGeo.pos(), mode );
					}
					else if( mode == SearchMode::Geometry || pSlot->_widget()->_markTest( ofs - childGeo.pos() ) )
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

	//____ _addToContentDefaultSize() ___________________________________________

	void  PackList::_addToContentDefaultSize( spx length, spx breadth )
	{
		m_contentDefaultLength += length;

		if( breadth == m_contentDefaultBreadth )
			m_nbEntriesOfDefaultBreadth++;
		else if( breadth > m_contentDefaultBreadth )
		{
			m_contentDefaultBreadth = breadth;
			m_nbEntriesOfDefaultBreadth = 1;
		}
	}

	//____ _subFromContentDefaultSize() _________________________________________

	void  PackList::_subFromContentDefaultSize( spx length, spx breadth )
	{
		m_contentDefaultLength -= length;

		if( breadth == m_contentDefaultBreadth )
		{
			m_nbEntriesOfDefaultBreadth--;
			if( m_nbEntriesOfDefaultBreadth == 0 )
			{
				spx highest = 0;
				for( Slot * p = slots.begin() ; p < slots.end() ; p++ )
				{
					if (p->m_prefBreadth == breadth)
						continue;

					if( p->m_prefBreadth == highest )
						m_nbEntriesOfDefaultBreadth++;
					else if( p->m_prefBreadth > highest )
					{
						highest = p->m_prefBreadth;
						m_nbEntriesOfDefaultBreadth = 1;
					}
				}
				m_contentDefaultBreadth = highest;
			}
		}
	}


	//____ _requestRenderChildren() ___________________________________________

	void PackList::_requestRenderChildren(Slot * pBegin, Slot * pEnd)
	{
		RectSPX box = _listArea();

		spx beg = pBegin->m_ofs;
		spx end = pEnd < slots.end() ? pEnd->m_ofs + pEnd->m_length : m_contentLength;


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

	void PackList::_updateChildOfsFrom( Slot * pSlot )
	{
		spx ofs = 0;
		if( pSlot > slots.begin() )
			ofs = pSlot[-1].m_ofs + pSlot[-1].m_length;

		while( pSlot < slots.end() )
		{
			pSlot->m_ofs = ofs;
			ofs += pSlot->m_length;
			pSlot++;
		}
	}

	//____ _onEntrySkinChanged() __________________________________________________

	void PackList::_onEntrySkinChanged( SizeSPX oldPadding, SizeSPX newPadding )
	{
		_requestRender();

		if( oldPadding != newPadding )
		{
			m_entryPadding = newPadding;
			int nEntries = slots.size();

			spx	lengthDiff, breadthDiff;
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

				m_contentDefaultLength += lengthDiff;
				m_contentDefaultBreadth += breadthDiff;

				_requestResize();
			}
		}
	}

	//____ _getEntryGeo() _________________________________________________________

	void PackList::_getEntryGeo( RectSPX& geo, const List::Slot * _pSlot ) const
	{
		const Slot * pSlot = static_cast<const Slot*>(_pSlot);

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

	void PackList::_getChildGeo( RectSPX& geo, const Slot * pSlot ) const
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
			int index = int(pSlot - slots.begin());
			if( m_pEntrySkin[index&0x1] )
				geo = m_pEntrySkin[index&0x1]->_contentRect( geo, m_scale, pSlot->_widget()->state() );
		}
	}

	//____ _paddedLimitedMatchingHeight() _________________________________________

	spx PackList::_paddedLimitedMatchingHeight( StaticSlot * _pSlot, spx paddedWidth )
	{
		auto pSlot = static_cast<Slot*>(_pSlot);

		spx height = pSlot->_widget()->_matchingHeight( paddedWidth - m_entryPadding.w, m_scale ) + m_entryPadding.h;
		limit( height, m_minEntrySizeSPX.h, m_maxEntrySizeSPX.h );
		return height;
	}

	//____ _paddedLimitedMatchingWidth() _________________________________________

	spx PackList::_paddedLimitedMatchingWidth( StaticSlot * _pSlot, spx paddedHeight )
	{
		auto pSlot = static_cast<Slot*>(_pSlot);

		spx width = pSlot->_widget()->_matchingWidth( paddedHeight - m_entryPadding.h, m_scale ) + m_entryPadding.w;
		limit( width, m_minEntrySizeSPX.w, m_maxEntrySizeSPX.w );
		return width;
	}

	//____ _paddedLimitedDefaultSize() __________________________________________

	SizeSPX PackList::_paddedLimitedDefaultSize( StaticSlot * _pSlot, int scale )
	{
		auto pSlot = static_cast<Slot*>(_pSlot);

		SizeSPX sz = pSlot->_widget()->_defaultSize(scale);
		sz += m_entryPadding;

		// Apply limits

		if( sz.w < m_minEntrySizeSPX.w )
			sz.w = m_minEntrySizeSPX.w;
		if( sz.h < m_minEntrySizeSPX.h )
			sz.h = m_minEntrySizeSPX.h;

		if( sz.w > m_maxEntrySizeSPX.w )
		{
			spx h = pSlot->_widget()->_matchingHeight(m_maxEntrySizeSPX.w-m_entryPadding.w, scale) + m_entryPadding.h;
			limit(h, m_minEntrySizeSPX.h, m_maxEntrySizeSPX.h );
			sz.h = h;
		}
		else if( sz.h > m_maxEntrySizeSPX.h )
		{
			spx w = pSlot->_widget()->_matchingWidth(m_maxEntrySizeSPX.h-m_entryPadding.h, scale) + m_entryPadding.w;
			limit(w, m_minEntrySizeSPX.w, m_maxEntrySizeSPX.w );
			sz.w = w;
		}

		return sz;
	}

	//____ _childPos() ________________________________________________________

	CoordSPX PackList::_childPos( const StaticSlot * _pSlot ) const
	{
		const Slot * pSlot = reinterpret_cast<const Slot*>(_pSlot);

		RectSPX geo;
		_getChildGeo(geo, pSlot);

		return geo.pos();
	}

	//____ _childRequestRender() _________________________________________________

	void PackList::_childRequestRender( StaticSlot * _pSlot )
	{
		Slot * pSlot = reinterpret_cast<Slot*>(_pSlot);

		RectSPX geo;
		_getChildGeo(geo, pSlot);
		_requestRender(geo);
	}

	void PackList::_childRequestRender( StaticSlot * _pSlot, const RectSPX& rect )
	{
		Slot * pSlot = reinterpret_cast<Slot*>(_pSlot);

		RectSPX geo;
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

		Slot * pSlot = reinterpret_cast<Slot*>(_pSlot);

		if (pSlot->m_bVisible && m_minEntrySizeSPX != m_maxEntrySizeSPX)
		{
			SizeSPX prefEntrySize = _paddedLimitedDefaultSize(pSlot, m_scale);

			spx prefLength = m_bHorizontal ? prefEntrySize.w : prefEntrySize.h;
			spx prefBreadth = m_bHorizontal ? prefEntrySize.h : prefEntrySize.w;

			// Update default sizes

			if( prefBreadth != pSlot->m_prefBreadth || prefLength != pSlot->m_length )
			{
				// NOTE: Order here is important!

				_addToContentDefaultSize( prefLength, prefBreadth );
				spx oldPrefBreadth = pSlot->m_prefBreadth;
				pSlot->m_prefBreadth = prefBreadth;
				_subFromContentDefaultSize( pSlot->m_length, oldPrefBreadth );

				bReqResize = true;
			}

			// Calculate new length

			spx length;
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
				_requestRenderChildren( pSlot, slots.end() );

				RectSPX childGeo;
				_getChildGeo(childGeo,pSlot);
				pSlot->_widget()->_resize(childGeo, m_scale);
			}
		}

		if( bReqResize )
			_requestResize();

		// Always set size on request since our _resize() won't do it unless breadth has changed.

		RectSPX geo;
		_getChildGeo(geo, pSlot);
		pSlot->_setSize(geo.size(), m_scale);
	}

	//____ _prevChild() __________________________________________________________

	Widget * PackList::_prevChild( const StaticSlot * _pSlot ) const
	{
		auto pSlot = static_cast<const Slot*>(_pSlot);

		if (pSlot > slots.begin())
			return pSlot[-1]._widget();

		return nullptr;
	}

	//____ _nextChild() __________________________________________________________

	Widget * PackList::_nextChild( const StaticSlot * _pSlot ) const
	{
		auto pSlot = static_cast<const Slot*>(_pSlot);

		if (pSlot < &slots.back())
			return pSlot[1]._widget();

		return nullptr;
	}

	//____ _releaseChild() ____________________________________________________

	void PackList::_releaseChild(StaticSlot * pSlot)
	{
		_willEraseSlots(pSlot, 1);
		slots.erase(static_cast<Slot*>(pSlot));
	}

	//____ _replaceChild() _____________________________________________________

	void PackList::_replaceChild(StaticSlot * pSlot, Widget * pNewChild)
	{
		static_cast<Slot*>(pSlot)->_setWidget(pNewChild);
		_childRequestResize(pSlot);
	}

	//____ _slotTypeInfo() ________________________________________________________

	const TypeInfo&	PackList::_slotTypeInfo(const StaticSlot * pSlot) const
	{
		return Slot::TYPEINFO;
	}

	//____ _firstSlotWithGeo() ____________________________________________________

	void PackList::_firstSlotWithGeo( SlotWithGeo& package ) const
	{
		if (slots.isEmpty())
			package.pSlot = nullptr;
		else
		{
			Slot * pSlot = slots.begin();
			package.pSlot = pSlot;
			_getChildGeo(package.geo, pSlot);
		}
	}

	//____ _nextSlotWithGeo() _____________________________________________________

	void PackList::_nextSlotWithGeo( SlotWithGeo& package ) const
	{
		Slot * pSlot = (Slot*)package.pSlot;

		if (pSlot == &slots.back())
			package.pSlot = nullptr;
		else
		{
			pSlot++;
			package.pSlot = pSlot;
			_getChildGeo(package.geo, pSlot);
		}
	}


	//____ _listArea() ____________________________________________________________

	RectSPX PackList::_listArea() const
	{
		return m_skin.contentRect( _listCanvas(), m_scale, m_state );
	}

	//____ _listWindow() ____________________________________________________________

	RectSPX PackList::_listWindow() const
	{
		RectSPX r = _windowSection();		// Window in widgets own coordinate system.

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

	RectSPX PackList::_listCanvas() const
	{
		SizeSPX headerSize = _header()._size();

		if( m_bHorizontal )
			return RectSPX(headerSize.w, 0, m_size.w - headerSize.w, m_size.h );
		else
			return RectSPX(0, headerSize.h, m_size.w, m_size.h - headerSize.h);	// List canvas in widgets own coordinate system.
	}

	//____ _headerGeo() ___________________________________________________________

	RectSPX PackList::_headerGeo() const
	{
		if( m_bHorizontal )
			return RectSPX( _windowSection().x, 0, _header()._size().w, m_size.h );
		else
			return RectSPX( 0, _windowSection().y, m_size.w, _header()._size().h );
	}

	//____ _windowPadding() _______________________________________________________

	SizeSPX PackList::_windowPadding() const
	{
		if( m_bHorizontal )
			return SizeSPX( _header()._size().w, 0 );
		else
			return SizeSPX( 0, _header()._size().h );
	}

	//____ _refreshHeader() _______________________________________________________

	void PackList::_refreshHeader()
	{
		SizeSPX wantedSize = _header()._defaultSize(m_scale);
		SizeSPX currentSize = _header()._size();

		bool	bRequestResize = false;

		// Update headers size, possibly request resize.

		if( wantedSize.h != currentSize.h )
		{
			if( !m_bHorizontal || (wantedSize.h > m_contentDefaultBreadth || currentSize.h > m_contentDefaultBreadth) )
				bRequestResize = true;
		}

		if( wantedSize.w != currentSize.w )
		{
			if( m_bHorizontal || (wantedSize.w > m_contentDefaultBreadth || currentSize.w > m_contentDefaultBreadth) )
				bRequestResize = true;
		}

		if( bRequestResize )
			_requestResize();
	}

	//____ sortSlots() _________________________________________________________

	bool PackList::sortSlots()
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
			Widget * pWidget = slots[entry]._widget();

			int first = 0;
			int last = entry-1;
			int middle = (first+last)/2;

			while( first <= last )
			{
				Widget * pEntry = slots[pOrderList[middle]]._widget();

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
		Base::memStackFree( listSize );

		// Update ofs in the slots

		_updateChildOfsFrom( slots.begin() );
		_requestRenderChildren( slots.begin(), slots.end() );	// Request render on dirty area
		return true;
	}

	//____ _componentPos() ____________________________________________________________

	CoordSPX PackList::_componentPos( const Component * pComponent ) const
	{
		CoordSPX c = m_bHorizontal ? CoordSPX(_windowSection().x, 0 ) : CoordSPX(0, _windowSection().y );
		return c;
	}

	//____ _componentSize() _________________________________________________________

	SizeSPX PackList::_componentSize( const Component * pComponent ) const
	{
		return _header()._size();		// We store size internally in the _header().
	}

	//____ _componentGeo() __________________________________________________________

	RectSPX PackList::_componentGeo( const Component * pComponent ) const
	{
		return RectSPX( _componentPos(pComponent), _header()._size() );
	}

	//____ _receiveComponentNotif() _____________________________________________________

	void PackList::_receiveComponentNotif( Component * pComponent, ComponentNotif notification, int value, void * pData )
	{
		if( notification == ComponentNotif::SortOrderChanged )
		{
			m_sortOrder = _header()._sortOrder();
			sortSlots();
		}
	}


} // namespace wg

