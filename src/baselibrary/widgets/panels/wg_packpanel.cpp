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
#include <wg_internal.h>

#include <wg_cdynamicslotvector.impl.h>
#include <wg_slotextras.impl.h>

namespace wg
{
	using namespace Util;

	template class CDynamicSlotVector<PackPanel::Slot>;
	template class PaddedSlotCollectionMethods<PackPanel::Slot,PackPanel>;

	const TypeInfo PackPanel::TYPEINFO = { "PackPanel", &Panel::TYPEINFO };
	const TypeInfo PackPanel::Slot::TYPEINFO = { "PackPanel::Slot", &PaddedSlot::TYPEINFO };


	void PackPanel::Slot::setWeight(float weight) 
	{ 
		if (weight != m_weight) 
			static_cast<PackPanel*>(_holder())->_reweightSlots(this, 1, weight); 
	}


	bool PackPanel::CSlots::setWeight(int index, int amount, float weight)
	{
		if (index < 0 || amount <= 0 || index + amount >= size() || weight < 0.f)
			return false;

		auto pSlot = _slot(index);
		_holder()->_reweightSlots(pSlot, amount, weight);
		return true;
	}

	bool PackPanel::CSlots::setWeight(const SlotIterator& beg, const SlotIterator& end, float weight)
	{
		//TODO: Add assert

		auto pBeg = static_cast<Slot*>(beg._slot());
		auto pEnd = static_cast<Slot*>(end._slot());
		_holder()->_reweightSlots(pBeg, int(pEnd - pBeg), weight);
		return true;
	}

	bool PackPanel::CSlots::setWeight(int index, int amount, std::initializer_list<float> weights)
	{
		if (index < 0 || amount <= 0 || index + amount >= size() || amount >(int) weights.size())
			return false;

		auto pSlot = _slot(index);
		_holder()->_reweightSlots(pSlot, amount, weights.begin());
		return true;
	}

	bool PackPanel::CSlots::setWeight(const SlotIterator& beg, const SlotIterator& end, std::initializer_list<float> weights)
	{
		//TODO: Add assert

		auto pBeg = static_cast<Slot*>(beg._slot());
		auto pEnd = static_cast<Slot*>(end._slot());
		_holder()->_reweightSlots(pBeg, int(pEnd - pBeg), weights.begin());
		return true;
	}

	//____ constructor ____________________________________________________________

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

	//____ typeInfo() _________________________________________________________

	const TypeInfo& PackPanel::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setAxis() ______________________________________________________

	void PackPanel::setAxis( Axis axis )
	{
		bool bHorizontal = axis==Axis::X?true:false;
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

	SizeSPX PackPanel::_preferredSize(int scale) const
	{
		scale = _fixScale(scale);
		if (scale != m_scale)
		{
			//TODO: Implement!
			assert(false);
			return SizeSPX();
		}
		else
			return m_preferredContentSize + m_skin.contentPaddingSize(scale);
	}

	//____ _matchingHeight() _______________________________________________________

	spx PackPanel::_matchingHeight( spx width, int scale ) const
	{
		scale = _fixScale(scale);

		spx height = 0;

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
						spx itemHeight = pS->_paddedMatchingHeight( pI->outSPX, scale );
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
				if (scale == m_scale)
				{
					for( auto pS = slots._begin() ; pS != slots._end() ; pS++ )
					{
						if( pS->m_bVisible && pS->m_preferredSize.h > height )
								height = pS->m_preferredSize.h;
					}
				}
				else
				{
					for (auto pS = slots._begin(); pS != slots._end(); pS++)
					{
						if (pS->m_bVisible)
						{
							spx h = pS->_paddedPreferredSize(scale).h;
							if (h > height)
								height = h;
						}
					}
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
						height += pS->_paddedMatchingHeight( width, scale );
				}
			}
		}

		height += m_skin.contentPaddingSize(scale).h;

		return height;
	}

	//____ _matchingWidth() _______________________________________________________

	spx PackPanel::_matchingWidth( spx height, int scale ) const
	{
		spx width = 0;

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
						spx itemWidth = pS->_paddedMatchingWidth( pI->outSPX, scale );
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
				if (scale == m_scale)
				{
					for (auto pS = slots._begin(); pS != slots._end(); pS++)
					{
						if (pS->m_bVisible && pS->m_preferredSize.w > width)
							width = pS->m_preferredSize.w;
					}
				}
				else
				{
					for (auto pS = slots._begin(); pS != slots._end(); pS++)
					{
						if (pS->m_bVisible)
						{
							spx w = pS->_paddedPreferredSize(scale).w;
							if (w > width)
								width = w;
						}
					}
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
						width += pS->_paddedMatchingWidth( height, scale );
				}
			}
		}

		width += m_skin.contentPaddingSize(scale).w;

		return width;
	}

	//____ _slotTypeInfo() ________________________________________________________

	const TypeInfo&	PackPanel::_slotTypeInfo(const StaticSlot * pSlot) const
	{
		return Slot::TYPEINFO;
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
			Slot * pSlot = slots._first();
			package.pSlot = pSlot;
			package.geo = pSlot->m_geo;
		}
	}

	//____ _nextSlotWithGeo() _____________________________________________________

	void PackPanel::_nextSlotWithGeo( SlotWithGeo& package ) const
	{
		Slot * pSlot = (Slot*) package.pSlot;

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

	void PackPanel::_didAddSlots(StaticSlot * pSlot, int nb)
	{
		_unhideChildren((Slot*) pSlot, nb);
	}

	//____ _didMoveSlots() _____________________________________________________

	void PackPanel::_didMoveSlots(StaticSlot * pFrom, StaticSlot * pTo, int nb)
	{
		//TODO: Optimize! Only update and re-render what is needed, but take
		// into account that SizeBroker might have weird rules and might affect
		// sizes in various ways when children change place...

		_refreshGeometries();
	}

	//____ _willEraseSlots() _________________________________________________

	void PackPanel::_willEraseSlots(StaticSlot * pSlot, int nb)
	{
		_hideChildren((Slot*) pSlot, nb);
	}

	//____ _hideSlots() _______________________________________________________

	void PackPanel::_hideSlots(StaticSlot * pSlot, int nb)
	{
		_hideChildren((Slot*) pSlot, nb);
	}

	//____ _unhideSlots() _____________________________________________________

	void PackPanel::_unhideSlots(StaticSlot * pSlot, int nb)
	{
		_unhideChildren((Slot*) pSlot, nb);
	}

	//____ _repadSlots() ______________________________________________________

	void PackPanel::_repadSlots(StaticSlot * pSlot, int nb, Border padding)
	{
//		TJFIX!!!  Also set m_paddingSPX and use that for geometry instead!

		for (int i = 0; i < nb; i++)
			((Slot*)pSlot)[i].m_padding = padding;

		_refreshGeometries();
	}

	void PackPanel::_repadSlots(StaticSlot * pSlot, int nb, const Border * pPaddings)
	{
		for (int i = 0; i < nb; i++)
			((Slot*)pSlot)[i].m_padding = * pPaddings++;

		_refreshGeometries();
	}

	//____ _reweightSlots() ______________________________________________________

	void PackPanel::_reweightSlots(Slot * pSlot, int nb, float weight)
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

	void PackPanel::_reweightSlots(Slot * pSlot, int nb, const float * pWeights)
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

	CoordSPX PackPanel::_childPos(const StaticSlot * pSlot) const
	{
		return ((Slot*)pSlot)->m_geo;
	}

	//____ _childRequestRender() ______________________________________________

	void PackPanel::_childRequestRender(StaticSlot * pSlot)
	{
		_requestRender( ((Slot*)pSlot)->m_geo );

	}

	void PackPanel::_childRequestRender(StaticSlot * pSlot, const RectSPX& rect)
	{
		_requestRender(rect + ((Slot*)pSlot)->m_geo.pos());
	}

	//____ _childRequestResize() ______________________________________________

	void PackPanel::_childRequestResize(StaticSlot * _pSlot)
	{
		// Update cached preferred size of child

		Slot * pSlot = static_cast<Slot*>(_pSlot);
		pSlot->m_preferredSize = pSlot->_paddedPreferredSize(m_scale);
		pSlot->m_bResizeRequired = true;

		_refreshGeometries();
	}

	//____ _prevChild() _______________________________________________________

	Widget * PackPanel::_prevChild(const StaticSlot * _pSlot) const
	{
		auto pSlot = static_cast<const Slot*>(_pSlot);

		if (pSlot > slots._begin())
			return pSlot[-1]._widget();

		return nullptr;
	}

	//____ _nextChild() _______________________________________________________

	Widget * PackPanel::_nextChild(const StaticSlot * _pSlot) const
	{
		auto pSlot = static_cast<const Slot*>(_pSlot);

		if (pSlot < slots._last())
			return pSlot[1]._widget();

		return nullptr;
	}

	//____ _releaseChild() ____________________________________________________

	void PackPanel::_releaseChild(StaticSlot * pSlot)
	{
		_willEraseSlots(pSlot, 1);
		slots._erase(static_cast<Slot*>(pSlot));
	}

	//____ _replaceChild() _____________________________________________________

	void PackPanel::_replaceChild(StaticSlot * _pSlot, Widget * pNewChild)
	{
		//TODO: Looks like we need to refresh geometry here or something...

		auto pSlot = static_cast<Slot* > (_pSlot);
		slots._releaseGuardPointer(pNewChild, &pSlot);

		pSlot->_setWidget(pNewChild);

		if (pSlot->m_bVisible)
		{
			pSlot->m_bVisible = true;
			pSlot->m_preferredSize = pSlot->_paddedPreferredSize(m_scale);
		}

	}


	//____ _unhideChildren() _______________________________________________________

	void PackPanel::_unhideChildren(Slot * pSlot, int nb)
	{
		for (int i = 0; i < nb; i++)
		{
			if (pSlot[i].m_bVisible == false)
			{
				pSlot[i].m_bVisible = true;
				pSlot[i].m_preferredSize = pSlot[i]._paddedPreferredSize(m_scale);
			}
		}

		_refreshGeometries();
	}

	//____ _hideChildren() _______________________________________________________

	void PackPanel::_hideChildren(Slot * pSlot, int nb)
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

		SizeSPX newPreferredContentSize = _calcPreferredSize();
		SizeSPX newPreferredSize = newPreferredContentSize + m_skin.contentPaddingSize(m_scale);

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

	void PackPanel::_resize( const SizeSPX& size, int scale )
	{
		Panel::_resize(size, scale);
		_refreshChildGeo(false);
	}

	//____ _calcPreferredSize() ______________________________________________________

	SizeSPX PackPanel::_calcPreferredSize()
	{
		spx length = 0;
		spx breadth = 0;

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
					spx b = m_bHorizontal?pS->_paddedMatchingHeight(pI->outSPX, m_scale):pS->_paddedMatchingWidth(pI->outSPX, m_scale);
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

		return m_bHorizontal?SizeSPX(length,breadth):SizeSPX(breadth,length);
	}

	//____ _refreshChildGeo() _________________________________________________________

	void PackPanel::_refreshChildGeo( bool bRequestRender )
	{
		if( slots.isEmpty() )
			return;

		SizeSPX sz = m_size - m_skin.contentPaddingSize(m_scale);
		CoordSPX contentOfs = m_skin.contentOfs(m_scale, StateEnum::Normal);			//TODO: Support offset changing in different states.

		spx wantedLength = m_bHorizontal?m_preferredContentSize.w:m_preferredContentSize.h;
		spx givenLength = m_bHorizontal?sz.w:sz.h;
		spx givenBreadth = m_bHorizontal?sz.h:sz.w;

		// Optimized special case, just copy preferred to length.
		//TODO: We probably need to use matchingWidth()/matchingHeight() here anyway... prefered length might change with given breadth

		if( !m_pSizeBroker || (wantedLength == givenLength && !m_pSizeBroker->mayAlterPreferredLengths()) )
		{
			CoordSPX pos;
			RectSPX geo;
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
					geo -= align(ptsToSpx(p->m_padding,m_scale));
					geo += contentOfs;

					if( geo != p->m_geo )
					{
						if (bRequestRender)
						{
							_requestRender(geo);
							_requestRender(p->m_geo);
						}

						spx oldW = p->m_geo.w;
						spx oldH = p->m_geo.h;
						p->m_geo = geo;
						if( geo.w != oldW || geo.h != oldH )
						{
							OO(p->_widget())->_resize( geo.size() );
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
					OO(p->_widget())->_resize(geo.size());
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

			CoordSPX pos;
			RectSPX geo;
			for (auto p = slots._begin(); p != slots._end(); p++)
			{
				if( p->m_bVisible )
				{
					geo.x = pos.x;
					geo.y = pos.y;
					if( m_bHorizontal )
					{
						geo.w = pI->outSPX;
						geo.h = sz.h;
						pos.x += pI->outSPX;
					}
					else
					{
						geo.w = sz.w;
						geo.h = pI->outSPX;
						pos.y += pI->outSPX;
					}
					geo -= align(ptsToSpx(p->m_padding,m_scale));
					geo += contentOfs;

					if( geo != p->m_geo )
					{
						if( bRequestRender )
						{
							_requestRender(geo);
							_requestRender(p->m_geo);
						}
						spx oldW = p->m_geo.w;
						spx oldH = p->m_geo.h;
						p->m_geo = geo;
						if( geo.w != oldW || geo.h != oldH )
						{
							OO(p->_widget())->_resize( geo.size() );
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
					OO(p->_widget())->_resize(geo.size());
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
					pI->preferred = spxToPts(pS->m_preferredSize.w, m_scale);
					pI->min = spxToPts(pS->_paddedMinSize(m_scale).w, m_scale);
					pI->max = spxToPts(pS->_paddedMaxSize(m_scale).w, m_scale);
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
					pI->preferred = spxToPts(pS->m_preferredSize.h, m_scale);
					pI->min = spxToPts(pS->_paddedMinSize(m_scale).h, m_scale);
					pI->max = spxToPts(pS->_paddedMaxSize(m_scale).h, m_scale);
					pI->weight = pS->m_weight;
					pI++;
				}
			}
		}

		return int(pI - pArray);
	}

	int PackPanel::_populateSizeBrokerArray( SizeBrokerItem * pArray, spx forcedBreadth ) const
	{
		SizeBrokerItem * pI = pArray;

		if( m_bHorizontal )
		{
			for (auto pS = slots._begin(); pS != slots._end(); pS++)
			{
				if( pS->m_bVisible )
				{
					pI->preferred = spxToPts(pS->_paddedMatchingWidth(forcedBreadth,m_scale), m_scale);
					pI->min = spxToPts(pS->_paddedMinSize(m_scale).w, m_scale);
					pI->max = spxToPts(pS->_paddedMaxSize(m_scale).w, m_scale);
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
					pI->preferred = spxToPts(pS->_paddedMatchingHeight(forcedBreadth,m_scale), m_scale);
					pI->min = spxToPts(pS->_paddedMinSize(m_scale).w, m_scale);
					pI->max = spxToPts(pS->_paddedMaxSize(m_scale).w, m_scale);
					pI->weight = pS->m_weight;
					pI++;
				}
			}
		}

		return int(pI - pArray);
	}

	//____ _setItemLengths() ___________________________________________________________

	spx PackPanel::_setItemLengths(SizeBrokerItem * pItems, int nItems, spx availableLength) const
	{
		m_pSizeBroker->setItemLengths(pItems, nItems, spxToPts(availableLength,m_scale) );

		// Align outputs so we end up on pixel boundaries

		spx begin = 0;
		pts end = 0;
		for (int i = 0; i < nItems; i++)
		{
			end += pItems[i].output;
			spx conv = align(ptsToSpx(end, m_scale));
			pItems[i].outSPX = conv - begin;
			begin = conv;
		}

		return begin;
	}

	//____ _setPreferredLengths() _______________________________________________________

	spx PackPanel::_setPreferredLengths(SizeBrokerItem * pItems, int nItems) const
	{
		m_pSizeBroker->setPreferredLengths(pItems, nItems);

		// Align outputs so we end up on pixel boundaries

		spx begin = 0;
		pts end = 0;
		for (int i = 0; i < nItems; i++)
		{
			end += pItems[i].output;
			spx conv = align(ptsToSpx(end, m_scale));
			pItems[i].outSPX = conv - begin;
			begin = conv;
		}

		return begin;
	}


} // namespace wg
