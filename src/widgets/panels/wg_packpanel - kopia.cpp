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

#include <initializer_list>

#include <wg_packpanel.h>
#include <wg_base.h>

#include <wg_cpaddedslotvector.impl.h>

namespace wg
{
	using namespace Util;

	template class CDynamicSlotVector<PackSlot>;
	template class CPaddedSlotVector<PackSlot>;

	const char PackPanel::CLASSNAME[] = {"PackPanel"};


	void PackSlot::setWeight(float weight) 
	{ 
		if (weight != m_weight) 
			static_cast<PackPanel*>(_holder())->_reweightSlots(this, 1, weight); 
	}


	bool CPackSlotVector::setWeight(int index, int amount, float weight)
	{
		if (index < 0 || amount <= 0 || index + amount >= size() || weight < 0.f)
			return false;

		auto pSlot = _slot(index);
		_holder()->_reweightSlots(pSlot, amount, weight);
		return true;
	}

	bool CPackSlotVector::setWeight(const SlotIterator& beg, const SlotIterator& end, float weight)
	{
		//TODO: Add assert

		auto pBeg = static_cast<PackSlot*>(beg._slot());
		auto pEnd = static_cast<PackSlot*>(end._slot());
		_holder()->_reweightSlots(pBeg, int(pEnd - pBeg), weight);
		return true;
	}

	bool CPackSlotVector::setWeight(int index, int amount, const std::initializer_list<float> weights)
	{
		if (index < 0 || amount <= 0 || index + amount >= size() || amount >(int) weights.size())
			return false;

		auto pSlot = _slot(index);
		_holder()->_reweightSlots(pSlot, amount, weights.begin());
		return true;
	}

	bool CPackSlotVector::setWeight(const SlotIterator& beg, const SlotIterator& end, const std::initializer_list<float> weights)
	{
		//TODO: Add assert

		auto pBeg = static_cast<PackSlot*>(beg._slot());
		auto pEnd = static_cast<PackSlot*>(end._slot());
		_holder()->_reweightSlots(pBeg, int(pEnd - pBeg), weights.begin());
		return true;
	}

	//____ Constructor ____________________________________________________________

	PackPanel::PackPanel() : slots(this)
	{
		m_bSiblingsOverlap = false;
		m_bHorizontal = true;
		m_pSizeBroker = 0;
	}

	//____ Destructor _____________________________________________________________

	PackPanel::~PackPanel()
	{
	}

	//____ isInstanceOf() _________________________________________________________

	bool PackPanel::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return Panel::isInstanceOf(pClassName);
	}

	//____ className() ____________________________________________________________

	const char * PackPanel::className( void ) const
	{
		return CLASSNAME;
	}

	//____ cast() _________________________________________________________________

	PackPanel_p PackPanel::cast( Object * pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return PackPanel_p( static_cast<PackPanel*>(pObject) );

		return 0;
	}

	//____ setOrientation() ______________________________________________________

	void PackPanel::setOrientation( Orientation orientation )
	{
		bool bHorizontal = orientation==Orientation::Horizontal?true:false;
		if( m_bHorizontal != bHorizontal )
		{
			m_bHorizontal = bHorizontal;
			_refreshGeometries();

		}
	}


	//____ setSizeBroker() _______________________________________________________

	void PackPanel::setSizeBroker( SizeBroker * pBroker )
	{
		if( m_pSizeBroker.rawPtr() != pBroker )
		{
			m_pSizeBroker = pBroker;
			_refreshGeometries();

		}
	}

	//____ _preferredSize() _______________________________________________________

	SizeI PackPanel::_preferredSize() const
	{
		SizeI size = m_preferredContentSize;
		if (m_pSkin)
			size += m_pSkin->_contentPaddingSize();

		return size;
	}

	//____ _matchingHeight() _______________________________________________________

	int PackPanel::_matchingHeight( int width ) const
	{
		int height = 0;

		if( m_bHorizontal )
		{
			if( m_pSizeBroker )
			{
				// Allocate and populate SizeBroker array

				int arrayBytes = sizeof(SizeBrokerItem)*slots.size();
				SizeBrokerItem * pItemArea = reinterpret_cast<SizeBrokerItem*>(Base::memStackAlloc(arrayBytes));

				int nItems = _populateSizeBrokerArray(pItemArea);

				// Retrieve item lengths and find height of highest item.

				_setItemLengths( pItemArea, nItems, width );

				SizeBrokerItem * pI = pItemArea;

				for( auto pS = slots._begin() ; pS != slots._end() ; pS++ )
				{
					if( pS->m_bVisible )
					{
						int itemHeight = pS->_paddedMatchingHeight( pI->output );
						if( itemHeight > height )
							height = itemHeight;
						pI++;
					}
				}

				// Release temporary memory area

				Base::memStackRelease(arrayBytes);
			}
			else
			{
				for( auto pS = slots._begin() ; pS != slots._end() ; pS++ )
				{
					if( pS->m_bVisible && pS->m_preferredSize.h > height )
							height = pS->m_preferredSize.h;
				}
			}
		}
		else
		{
			if( m_pSizeBroker && m_pSizeBroker->mayAlterPreferredLengths() )
			{
				// Allocate and populate SizeBroker array

				int arrayBytes = sizeof(SizeBrokerItem)*slots.size();
				SizeBrokerItem * pItemArea = reinterpret_cast<SizeBrokerItem*>(Base::memStackAlloc(arrayBytes));

				int nItems = _populateSizeBrokerArray(pItemArea, width);

				// Retrieve preferred length

				height = _setPreferredLengths( pItemArea, nItems );

				// Release temporary memory area

				Base::memStackRelease(arrayBytes);
			}
			else
			{
				for( auto pS = slots._begin() ; pS != slots._end() ; pS++ )
				{
					if( pS->m_bVisible )
						height += pS->_paddedMatchingHeight( width );
				}
			}
		}

		if (m_pSkin)
			height += m_pSkin->_contentPaddingSize().h;

		return height;
	}

	//____ _matchingWidth() _______________________________________________________

	int PackPanel::_matchingWidth( int height ) const
	{
		int width = 0;

		if( !m_bHorizontal )
		{
			if( m_pSizeBroker )
			{
				// Allocate and populate SizeBroker array

				int arrayBytes = sizeof(SizeBrokerItem)*slots.size();
				SizeBrokerItem * pItemArea = reinterpret_cast<SizeBrokerItem*>(Base::memStackAlloc(arrayBytes));

				int nItems = _populateSizeBrokerArray(pItemArea);

				// Retrieve item lengths and find height of highest item.

				_setItemLengths( pItemArea, nItems, height );

				SizeBrokerItem * pI = pItemArea;

				for( auto pS = slots._begin() ; pS != slots._end() ; pS++ )
				{
					if( pS->m_bVisible )
					{
						int itemWidth = pS->_paddedMatchingWidth( pI->output.raw );
						if( itemWidth > width )
							width = itemWidth;
						pI++;
					}
				}

				// Release temporary memory area

				Base::memStackRelease(arrayBytes);
			}
			else
			{
				for( auto pS = slots._begin() ; pS != slots._end() ; pS++ )
				{
					if( pS->m_bVisible && pS->m_preferredSize.w > width )
							width = pS->m_preferredSize.w;
				}
			}
		}
		else
		{
			if( m_pSizeBroker && m_pSizeBroker->mayAlterPreferredLengths() )
			{
				// Allocate and populate SizeBroker array

				int arrayBytes = sizeof(SizeBrokerItem)*slots.size();
				SizeBrokerItem * pItemArea = reinterpret_cast<SizeBrokerItem*>(Base::memStackAlloc(arrayBytes));

				int nItems = _populateSizeBrokerArray(pItemArea, height);

				// Retrieve preferred length

				width = _setPreferredLengths( pItemArea, nItems );

				// Release temporary memory area

				Base::memStackRelease(arrayBytes);
			}
			else
			{
				for( auto pS = slots._begin() ; pS != slots._end() ; pS++ )
				{
					if( pS->m_bVisible )
						width += pS->_paddedMatchingWidth( height );
				}
			}
		}

		if (m_pSkin)
			width += m_pSkin->_contentPaddingSize().w;

		return width;
	}

	//____ _firstChild() __________________________________________________________

	Widget * PackPanel::_firstChild() const
	{
		if (slots.isEmpty())
			return nullptr;

		return slots._first()->_widget();
	}

	//____ _lastChild() __________________________________________________________

	Widget * PackPanel::_lastChild() const
	{
		if (slots.isEmpty())
			return nullptr;

		return slots._last()->_widget();
	}


	//____ _firstSlotWithGeo() _____________________________________________________

	void PackPanel::_firstSlotWithGeo( SlotWithGeo& package ) const
	{
		if( slots.isEmpty() )
			package.pSlot = nullptr;
		else
		{
			PackSlot * pSlot = slots._first();
			package.pSlot = pSlot;
			package.geo = pSlot->m_geo;
		}
	}

	//____ _nextSlotWithGeo() _____________________________________________________

	void PackPanel::_nextSlotWithGeo( SlotWithGeo& package ) const
	{
		PackSlot * pSlot = (PackSlot*) package.pSlot;

		if( pSlot == slots._last() )
			package.pSlot = nullptr;
		else
		{
			pSlot++;
			package.pSlot = pSlot;
			package.geo = pSlot->m_geo;
		}
	}

	//____ _didAddSlots() _____________________________________________________

	void PackPanel::_didAddSlots(Slot * pSlot, int nb)
	{
		_unhideChildren((PackSlot*) pSlot, nb);
	}

	//____ _didMoveSlots() _____________________________________________________

	void PackPanel::_didMoveSlots(Slot * pFrom, Slot * pTo, int nb)
	{
		//TODO: Optimize! Only update and re-render what is needed, but take
		// into account that SizeBroker might have weird rules and might affect
		// sizes in various ways when children change place...

		_refreshGeometries();
	}

	//____ _willEraseSlots() _________________________________________________

	void PackPanel::_willEraseSlots(Slot * pSlot, int nb)
	{
		_hideChildren((PackSlot*) pSlot, nb);
	}

	//____ _hideSlots() _______________________________________________________

	void PackPanel::_hideSlots(Slot * pSlot, int nb)
	{
		_hideChildren((PackSlot*) pSlot, nb);
	}

	//____ _unhideSlots() _____________________________________________________

	void PackPanel::_unhideSlots(Slot * pSlot, int nb)
	{
		_unhideChildren((PackSlot*) pSlot, nb);
	}

	//____ _repadSlots() ______________________________________________________

	void PackPanel::_repadSlots(Slot * pSlot, int nb, BorderI padding)
	{
		for (int i = 0; i < nb; i++)
			((PackSlot*)pSlot)[i].m_padding = padding;

		_refreshGeometries();
	}

	void PackPanel::_repadSlots(Slot * pSlot, int nb, const BorderI * pPaddings)
	{
		for (int i = 0; i < nb; i++)
			((PackSlot*)pSlot)[i].m_padding = * pPaddings++;

		_refreshGeometries();
	}

	//____ _reweightSlots() ______________________________________________________

	void PackPanel::_reweightSlots(PackSlot * pSlot, int nb, float weight)
	{
		bool bModified = false;

		for (int i = 0; i < nb; i++)
		{
			if (pSlot[i].m_weight != weight)
			{
				bModified = true;
				pSlot[i].m_weight = weight;
			}
		}

		if (bModified)
		{
			if (m_pSizeBroker && m_pSizeBroker->mayAlterPreferredLengths())
				_refreshGeometries();
			else
				_refreshChildGeo();
		}
	}

	void PackPanel::_reweightSlots(PackSlot * pSlot, int nb, const float * pWeights)
	{
		bool bModified = false;

		for (int i = 0; i < nb; i++)
		{
			if (pSlot[i].m_weight != *pWeights)
			{
				bModified = true;
				pSlot[i].m_weight = * pWeights++;
			}
		}

		if (bModified)
		{
			if (m_pSizeBroker && m_pSizeBroker->mayAlterPreferredLengths())
				_refreshGeometries();
			else
				_refreshChildGeo();
		}
	}

	//____ _childPos() _______________________________________________________

	CoordI PackPanel::_childPos(const Slot * pSlot) const
	{
		return ((PackSlot*)pSlot)->m_geo;
	}

	//____ _childRequestRender() ______________________________________________

	void PackPanel::_childRequestRender(Slot * pSlot)
	{
		_requestRender( ((PackSlot*)pSlot)->m_geo );

	}

	void PackPanel::_childRequestRender(Slot * pSlot, const RectI& rect)
	{
		_requestRender(rect + ((PackSlot*)pSlot)->m_geo.pos());
	}

	//____ _childRequestResize() ______________________________________________

	void PackPanel::_childRequestResize(Slot * _pSlot)
	{
		// Update cached preferred size of child

		PackSlot * pSlot = static_cast<PackSlot*>(_pSlot);
		pSlot->m_preferredSize = pSlot->_paddedPreferredSize();
		pSlot->m_bResizeRequired = true;

		_refreshGeometries();
	}

	//____ _prevChild() _______________________________________________________

	Widget * PackPanel::_prevChild(const Slot * _pSlot) const
	{
		auto pSlot = static_cast<const PackSlot*>(_pSlot);

		if (pSlot > slots._begin())
			return pSlot[-1]._widget();

		return nullptr;
	}

	//____ _nextChild() _______________________________________________________

	Widget * PackPanel::_nextChild(const Slot * _pSlot) const
	{
		auto pSlot = static_cast<const PackSlot*>(_pSlot);

		if (pSlot < slots._last())
			return pSlot[1]._widget();

		return nullptr;
	}

	//____ _releaseChild() ____________________________________________________

	void PackPanel::_releaseChild(Slot * pSlot)
	{
		_willEraseSlots(pSlot, 1);
		slots._erase(static_cast<PackSlot*>(pSlot));
	}

	//____ _replaceChild() _____________________________________________________

	void PackPanel::_replaceChild(Slot * _pSlot, Widget * pNewChild)
	{
		auto pSlot = static_cast<PackSlot* > (_pSlot);
		slots._releaseGuardPointer(pNewChild, &pSlot);

		pSlot->_setWidget(pNewChild);

		if (pSlot->m_bVisible)
		{
			pSlot->m_bVisible = true;
			pSlot->m_preferredSize = pSlot->_paddedPreferredSize();
		}

	}


	//____ _unhideChildren() _______________________________________________________

	void PackPanel::_unhideChildren(PackSlot * pSlot, int nb)
	{
		for (int i = 0; i < nb; i++)
		{
			if (pSlot[i].m_bVisible == false)
			{
				pSlot[i].m_bVisible = true;
				pSlot[i].m_preferredSize = pSlot[i]._paddedPreferredSize();
			}
		}

		_refreshGeometries();
	}

	//____ _hideChildren() _______________________________________________________

	void PackPanel::_hideChildren(PackSlot * pSlot, int nb)
	{
		for (int i = 0; i < nb; i++)
			pSlot[i].m_bVisible = false;

		_refreshGeometries();
	}


	//____ _onWidgetsReordered() ____________________________________________________
/*
	void PackPanel::_onWidgetsReordered()
	{
		_refreshChildGeo();
	}
*/
	//____ _refreshGeometries() _____________________________________________________

	void PackPanel::_refreshGeometries()
	{
		// Recalculate preferred sizes for widget and content.

		SizeI newPreferredContentSize = _calcPreferredSize();
		SizeI newPreferredSize = m_pSkin ? newPreferredContentSize + m_pSkin->_contentPaddingSize() : newPreferredContentSize;

		// request resize or just refresh child geo, depending on what is needed.

		if (newPreferredContentSize != m_preferredContentSize || newPreferredSize != m_size)
		{
			m_preferredContentSize = newPreferredContentSize;
			_requestResize();
		}
		else
			_refreshChildGeo();
	}


	//____ _resize() ____________________________________________________________

	void PackPanel::_resize( const SizeI& size )
	{
		Panel::_resize(size);
		_refreshChildGeo(false);
	}

	//____ _calcPreferredSize() ______________________________________________________

	SizeI PackPanel::_calcPreferredSize()
	{
		int length = 0;
		int breadth = 0;

		if( m_pSizeBroker && m_pSizeBroker->mayAlterPreferredLengths() )
		{
			// Allocate and populate SizeBroker array

			int arrayBytes = sizeof(SizeBrokerItem)*slots.size();
			SizeBrokerItem * pItemArea = reinterpret_cast<SizeBrokerItem*>(Base::memStackAlloc(arrayBytes));

			int nItems = _populateSizeBrokerArray(pItemArea);

			// Retrieve preferred length and breadth

			length = _setPreferredLengths( pItemArea, nItems );

			SizeBrokerItem * pI = pItemArea;
			for (auto pS = slots._begin(); pS != slots._end(); pS++)
			{
				if( pS->m_bVisible )
				{
					int b = m_bHorizontal?pS->_paddedMatchingHeight(pI->output.raw):pS->_paddedMatchingWidth(pI->output.raw);
					if( b > breadth )
						breadth = b;
					pI++;
				}
			}

			// Release temporary memory area

			Base::memStackRelease(arrayBytes);

		}
		else
		{
			if( m_bHorizontal )
			{
				for (auto p = slots._begin(); p != slots._end(); p++)
				{
					if( p->m_bVisible )
					{
						length += p->m_preferredSize.w;
						if( p->m_preferredSize.h > breadth )
							breadth = p->m_preferredSize.h;
					}
				}
			}
			else
			{
				for (auto p = slots._begin(); p != slots._end(); p++)
				{
					if( p->m_bVisible )
					{
						length += p->m_preferredSize.h;
						if( p->m_preferredSize.w > breadth )
							breadth = p->m_preferredSize.w;
					}
				}
			}
		}

		//

		return m_bHorizontal?SizeI(length,breadth):SizeI(breadth,length);
	}

	//____ _refreshChildGeo() _________________________________________________________

	void PackPanel::_refreshChildGeo( bool bRequestRender )
	{
		if( slots.isEmpty() )
			return;

		SizeI sz = m_size;
		CoordI contentOfs;

		if (m_pSkin)
		{
			sz -= m_pSkin->_contentPaddingSize();
			contentOfs = m_pSkin->_contentOfs(StateEnum::Normal);			//TODO: Support offset changing in different states.
		}

		int wantedLength = m_bHorizontal?m_preferredContentSize.w:m_preferredContentSize.h;
		int givenLength = m_bHorizontal?sz.w:sz.h;
		int givenBreadth = m_bHorizontal?sz.h:sz.w;

		// Optimized special case, just copy preferred to length.
		//TODO: We probably need to use matchingWidth()/matchingHeight() here anyway... prefered length might change with given breadth

		if( !m_pSizeBroker || (wantedLength == givenLength && !m_pSizeBroker->mayAlterPreferredLengths()) )
		{
			CoordI pos;
			RectI geo;
			for (auto p = slots._begin(); p != slots._end(); p++)
			{
				if( p->m_bVisible )
				{
					geo.x = pos.x;
					geo.y = pos.y;
					if( m_bHorizontal )
					{
						geo.w = p->m_preferredSize.w;
						geo.h = sz.h;
						pos.x += p->m_preferredSize.w;
					}
					else
					{
						geo.w = sz.w;
						geo.h = p->m_preferredSize.h;
						pos.y += p->m_preferredSize.h;
					}
					geo -= p->m_padding;
					geo += contentOfs;

					if( geo != p->m_geo )
					{
						if (bRequestRender)
						{
							_requestRender(geo);
							_requestRender(p->m_geo);
						}

						int oldW = p->m_geo.w;
						int oldH = p->m_geo.h;
						p->m_geo = geo;
						if( geo.w != oldW || geo.h != oldH )
						{
							p->_widget()->_resize( geo.size() );
							p->m_bResizeRequired = false;
						}

					}
				}
				else
				{
					if( bRequestRender && p->m_geo.w != 0 && p->m_geo.h != 0 )
						_requestRender(p->m_geo);

					p->m_geo.x = pos.x + contentOfs.x;
					p->m_geo.y = pos.y + contentOfs.y;
					if( m_bHorizontal )
					{
						geo.w = 0;
						geo.h = sz.h;
					}
					else
					{
						geo.w = sz.w;
						geo.h = 0;
					}
				}

				if( p->m_bResizeRequired )
				{
					p->_widget()->_resize(geo.size());
					p->m_bResizeRequired = false;
				}
			}
		}
		else
		{
			// Allocate and populate SizeBroker array

			int arrayBytes = sizeof(SizeBrokerItem)*slots.size();
			SizeBrokerItem * pItemArea = reinterpret_cast<SizeBrokerItem*>(Base::memStackAlloc(arrayBytes));

			int nItems = _populateSizeBrokerArray(pItemArea, givenBreadth);

			// Retrieve length and set geo for all children, call _requestRender() and _setSize() where needed.

			_setItemLengths( pItemArea, nItems, givenLength );

			SizeBrokerItem * pI = pItemArea;

			CoordI pos;
			RectI geo;
			for (auto p = slots._begin(); p != slots._end(); p++)
			{
				if( p->m_bVisible )
				{
					geo.x = pos.x;
					geo.y = pos.y;
					if( m_bHorizontal )
					{
						geo.w = pI->output.raw;
						geo.h = sz.h;
						pos.x += pI->output.raw;
					}
					else
					{
						geo.w = sz.w;
						geo.h = pI->output.raw;
						pos.y += pI->output.raw;
					}
					geo -= p->m_padding;
					geo += contentOfs;

					if( geo != p->m_geo )
					{
						if( bRequestRender )
						{
							_requestRender(geo);
							_requestRender(p->m_geo);
						}
						int oldW = p->m_geo.w;
						int oldH = p->m_geo.h;
						p->m_geo = geo;
						if( geo.w != oldW || geo.h != oldH )
						{
							p->_widget()->_resize( geo.size() );
							p->m_bResizeRequired = false;
						}
					}
					pI++;
				}
				else
				{
					if( bRequestRender && p->m_geo.w != 0 && p->m_geo.h != 0 )
						_requestRender(p->m_geo);

					p->m_geo.x = pos.x + contentOfs.x;
					p->m_geo.y = pos.y + contentOfs.y;
					if( m_bHorizontal )
					{
						geo.w = 0;
						geo.h = sz.h;
					}
					else
					{
						geo.w = sz.w;
						geo.h = 0;
					}
				}

				if (p->m_bResizeRequired)
				{
					p->_widget()->_resize(geo.size());
					p->m_bResizeRequired = false;
				}
			}

			// Release SizeBroker array

			Base::memStackRelease(arrayBytes);
		}
	}

	//____ _populateSizeBrokerArray() ___________________________________________

	int PackPanel::_populateSizeBrokerArray( SizeBrokerItem * pArray ) const
	{
		SizeBrokerItem * pI = pArray;

		if( m_bHorizontal )
		{
			for (auto pS = slots._begin(); pS != slots._end(); pS++)
			{
				if( pS->m_bVisible )
				{
					pI->preferred = QPix::fromRaw(pS->m_preferredSize.w);
					pI->min = QPix::fromRaw(pS->_paddedMinSize().w);
					pI->max = QPix::fromRaw(pS->_paddedMaxSize().w);
					pI->weight = pS->m_weight;
					pI++;
				}
			}
		}
		else
		{
			for (auto pS = slots._begin(); pS != slots._end(); pS++)
			{
				if( pS->m_bVisible )
				{
					pI->preferred = QPix::fromRaw(pS->m_preferredSize.h);
					pI->min = QPix::fromRaw(pS->_paddedMinSize().h);
					pI->max = QPix::fromRaw(pS->_paddedMaxSize().h);
					pI->weight = pS->m_weight;
					pI++;
				}
			}
		}

		return int(pI - pArray);
	}

	int PackPanel::_populateSizeBrokerArray( SizeBrokerItem * pArray, int forcedBreadth ) const
	{
		SizeBrokerItem * pI = pArray;

		if( m_bHorizontal )
		{
			for (auto pS = slots._begin(); pS != slots._end(); pS++)
			{
				if( pS->m_bVisible )
				{
					pI->preferred = QPix::fromRaw(pS->_paddedMatchingWidth(forcedBreadth));
					pI->min = QPix::fromRaw(pS->_paddedMinSize().w);
					pI->max = QPix::fromRaw(pS->_paddedMaxSize().w);
					pI->weight = pS->m_weight;
					pI++;
				}
			}
		}
		else
		{
			for (auto pS = slots._begin(); pS != slots._end(); pS++)
			{
				if( pS->m_bVisible )
				{
					pI->preferred = QPix::fromRaw(pS->_paddedMatchingHeight(forcedBreadth));
					pI->min = QPix::fromRaw(pS->_paddedMinSize().h);
					pI->max = QPix::fromRaw(pS->_paddedMaxSize().h);
					pI->weight = pS->m_weight;
					pI++;
				}
			}
		}

		return int(pI - pArray);
	}

	//____ _setItemLengths() ___________________________________________________________

	int PackPanel::_setItemLengths(SizeBrokerItem * pItems, int nItems, int availableLength) const
	{
		QPix totalLength = m_pSizeBroker->setItemLengths(pItems, nItems, QPix::fromRaw(availableLength) );

		// Align outputs so we end up on pixel boundaries

		int reminder = 0;
		for (int i = 0; i < nItems; i++)
		{
			reminder += pItems->output.raw;
			pItems->output.raw = reminder & 0xFFFFFFFC;
			reminder &= 0x3;
		}

		return totalLength.raw & 0xFFFFFFFC;
	}

	//____ _setPreferredLengths() _______________________________________________________

	int PackPanel::_setPreferredLengths(SizeBrokerItem * pItems, int nItems) const
	{
		QPix totalLength = m_pSizeBroker->setPreferredLengths(pItems, nItems);

		// Align outputs so we end up on pixel boundaries

		int reminder = 0;
		for (int i = 0; i < nItems; i++)
		{
			reminder += pItems->output.raw;
			pItems->output.raw = reminder & 0xFFFFFFFC;
			reminder &= 0x3;
		}

		return totalLength.raw & 0xFFFFFFFC;
	}


} // namespace wg
