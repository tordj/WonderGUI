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

#include <wg_dynamicslotvector.impl.h>
#include <wg_slotextras.impl.h>

namespace wg
{
	using namespace Util;

	template class DynamicSlotVector<PackPanel::Slot>;
	template class PaddedSlotCollectionMethods<PackPanel::Slot,PackPanel::iterator,PackPanel>;
	template class HideableSlotCollectionMethods<PackPanel::Slot, PackPanel::iterator, PackPanel>;

	const TypeInfo PackPanel::TYPEINFO = { "PackPanel", &Panel::TYPEINFO };
	const TypeInfo PackPanel::Slot::TYPEINFO = { "PackPanel::Slot", &DynamicSlot::TYPEINFO };


	bool PackPanel::Slot::_setBlueprint( const Blueprint& bp )
	{
		m_weight = bp.weight;
		m_bVisible = bp.visible;
		m_padding = bp.padding;
		return true;
	}

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

	bool PackPanel::CSlots::setWeight(iterator beg, iterator end, float weight)
	{
		//TODO: Add assert

		auto pBeg = beg;
		auto pEnd = end;
		_holder()->_reweightSlots(pBeg, int(pEnd - pBeg), weight);
		return true;
	}

	bool PackPanel::CSlots::setWeight(int index, int amount, std::initializer_list<float> weights)
	{
		if (index < 0 || amount <= 0 || index + amount > size() || amount >(int) weights.size())
			return false;

		auto pSlot = _slot(index);
		_holder()->_reweightSlots(pSlot, amount, weights.begin());
		return true;
	}

	bool PackPanel::CSlots::setWeight(iterator beg, iterator end, std::initializer_list<float> weights)
	{
		//TODO: Add assert

		auto pBeg = beg;
		auto pEnd = end;
		_holder()->_reweightSlots(pBeg, int(pEnd - pBeg), weights.begin());
		return true;
	}

	//____ constructor ____________________________________________________________

	PackPanel::PackPanel() : slots(this)
	{
		m_bSiblingsOverlap = false;
		m_bHorizontal = true;
		m_pLayout = 0;
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


	//____ setLayout() _______________________________________________________

	void PackPanel::setLayout( PackLayout * pLayout )
	{
		if( m_pLayout.rawPtr() != pLayout )
		{
			m_pLayout = pLayout;
			_refreshGeometries();

		}
	}

	//____ _defaultSize() _______________________________________________________

	SizeSPX PackPanel::_defaultSize(int scale) const
	{
		if (scale != m_scale)
			return _calcDefaultSize(scale);
		else
			return m_defaultContentSize + m_skin.contentPaddingSize(scale);
	}

	//____ _matchingHeight() _______________________________________________________

	spx PackPanel::_matchingHeight( spx width, int scale ) const
	{
		spx height = 0;

		if (slots.size() > 0)
		{
			if( m_bHorizontal )
			{
				if( m_pLayout )
				{
					// Allocate and populate Layout array

					int arrayBytes = (sizeof(PackLayout::Item) + sizeof(spx)) * slots.size();
					PackLayout::Item * pItemArea = reinterpret_cast<PackLayout::Item*>(Base::memStackAlloc(arrayBytes));
					spx* pOutput = (spx*)&pItemArea[slots.size()];

					int nItems = _populateLayoutArray(pItemArea);

					// Retrieve item lengths and find height of highest item.

					m_pLayout->getItemSizes(pOutput, width, m_scale, nItems, pItemArea);

					for( auto pS = slots._begin() ; pS != slots._end() ; pS++ )
					{
						if( pS->m_bVisible )
						{
							spx itemHeight = pS->_paddedMatchingHeight( *pOutput, scale );
							if( itemHeight > height )
								height = itemHeight;
							pOutput++;
						}
					}

					// Release temporary memory area

					Base::memStackFree(arrayBytes);
				}
				else
				{
					if (scale == m_scale)
					{
						for( auto pS = slots._begin() ; pS != slots._end() ; pS++ )
						{
							if( pS->m_bVisible && pS->m_defaultSize.h > height )
									height = pS->m_defaultSize.h;
						}
					}
					else
					{
						for (auto pS = slots._begin(); pS != slots._end(); pS++)
						{
							if (pS->m_bVisible)
							{
								spx h = pS->_paddedDefaultSize(scale).h;
								if (h > height)
									height = h;
							}
						}
					}
				}
			}
			else
			{
				if( m_pLayout && m_pLayout->doesCalcWantedLength() )
				{
					// Allocate and populate SizeBroker array

					int arrayBytes = (sizeof(PackLayout::Item) + sizeof(spx)) * slots.size();
					PackLayout::Item * pItemArea = reinterpret_cast<PackLayout::Item*>(Base::memStackAlloc(arrayBytes));
					spx* pOutput = (spx*)&pItemArea[slots.size()];

					int nItems = _populateLayoutArray(pItemArea, width);

					// Retrieve default length

					height = m_pLayout->getWantedSizes(pOutput, m_scale, nItems, pItemArea);

					// Release temporary memory area

					Base::memStackFree(arrayBytes);
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
		}

		height += m_skin.contentPaddingSize(scale).h;

		return height;
	}

	//____ _matchingWidth() _______________________________________________________

	spx PackPanel::_matchingWidth( spx height, int scale ) const
	{
		spx width = 0;
		
		if (slots.size() > 0)
		{
			if( !m_bHorizontal )
			{
				if( m_pLayout )
				{
					// Allocate and populate SizeBroker array

					int arrayBytes = (sizeof(PackLayout::Item)+sizeof(spx)) * slots.size();
					PackLayout::Item* pItemArea = reinterpret_cast<PackLayout::Item*>(Base::memStackAlloc(arrayBytes));
					spx* pOutput = (spx*)&pItemArea[slots.size()];

					int nItems = _populateLayoutArray(pItemArea);

					// Retrieve item lengths and find height of highest item.


					m_pLayout->getItemSizes(pOutput, height, m_scale, nItems, pItemArea);

					for( auto pS = slots._begin() ; pS != slots._end() ; pS++ )
					{
						if( pS->m_bVisible )
						{
							spx itemWidth = pS->_paddedMatchingWidth( *pOutput, scale );
							if( itemWidth > width )
								width = itemWidth;
							pOutput++;
						}
					}

					// Release temporary memory area

					Base::memStackFree(arrayBytes);
				}
				else
				{
					if (scale == m_scale)
					{
						for (auto pS = slots._begin(); pS != slots._end(); pS++)
						{
							if (pS->m_bVisible && pS->m_defaultSize.w > width)
								width = pS->m_defaultSize.w;
						}
					}
					else
					{
						for (auto pS = slots._begin(); pS != slots._end(); pS++)
						{
							if (pS->m_bVisible)
							{
								spx w = pS->_paddedDefaultSize(scale).w;
								if (w > width)
									width = w;
							}
						}
					}
				}
			}
			else
			{
				if( m_pLayout && m_pLayout->doesCalcWantedLength() )
				{
					// Allocate and populate SizeBroker array

					int arrayBytes = (sizeof(PackLayout::Item) + sizeof(spx)) * slots.size();
					PackLayout::Item* pItemArea = reinterpret_cast<PackLayout::Item*>(Base::memStackAlloc(arrayBytes));
					spx* pOutput = (spx*)&pItemArea[slots.size()];

					int nItems = _populateLayoutArray(pItemArea, height);

					// Retrieve preferred length

					width = m_pLayout->getWantedSizes(pOutput, m_scale, nItems, pItemArea);

					// Release temporary memory area

					Base::memStackFree(arrayBytes);
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

			while (!pSlot->m_bVisible)
			{
				if (pSlot == slots._last())
				{
					package.pSlot = nullptr;
					return;
				}
				pSlot++;
			}

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

			while (!pSlot->m_bVisible)
			{
				if (pSlot == slots._last())
				{
					package.pSlot = nullptr;
					return;
				}
				pSlot++;
			}

			package.pSlot = pSlot;
			package.geo = pSlot->m_geo;
		}
	}

	//____ _didAddSlots() _____________________________________________________

	void PackPanel::_didAddSlots(StaticSlot * _pSlot, int nb)
	{
		PackPanel::Slot* pSlot = static_cast<PackPanel::Slot*>(_pSlot);

		for (int i = 0; i < nb; i++)
		{
			if (pSlot[i].m_bVisible == true)
				pSlot[i].m_defaultSize = pSlot[i]._paddedDefaultSize(m_scale);
		}

		_refreshGeometries();
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

	void PackPanel::_repadSlots(StaticSlot * _pSlot, int nb, Border padding)
	{
//		TJFIX!!!  Also set m_paddingSPX and use that for geometry instead!

	//TODO: Optimize. No need to ask widget for defaultSize, just recalc padding in spx

		Slot* pSlot = static_cast<Slot*>(_pSlot);

		for (int i = 0; i < nb; i++)
		{
			pSlot[i].m_padding = padding;
			pSlot[i].m_defaultSize = pSlot[i]._paddedDefaultSize(m_scale);
		}

		_refreshGeometries();
	}

	void PackPanel::_repadSlots(StaticSlot * _pSlot, int nb, const Border * pPaddings)
	{
		//TODO: Optimize. No need to ask widget for defaultSize, just recalc padding in spx

		Slot* pSlot = static_cast<Slot*>(_pSlot);

		for (int i = 0; i < nb; i++)
		{
			pSlot[i].m_padding = * pPaddings++;
			pSlot[i].m_defaultSize = pSlot[i]._paddedDefaultSize(m_scale);
		}

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
			if (m_pLayout && m_pLayout->doesCalcWantedLength())
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
				pSlot[i].m_weight = * pWeights;
			}
			pWeights++;
		}

		if (bModified)
		{
			if (m_pLayout && m_pLayout->doesCalcWantedLength())
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
		pSlot->m_defaultSize = pSlot->_paddedDefaultSize(m_scale);
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
			pSlot->m_defaultSize = pSlot->_paddedDefaultSize(m_scale);
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
				pSlot[i].m_defaultSize = pSlot[i]._paddedDefaultSize(m_scale);
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

		SizeSPX newDefaultContentSize = _calcDefaultSize(m_scale);
		SizeSPX newDefaultSize = newDefaultContentSize + m_skin.contentPaddingSize(m_scale);

		// request resize or just refresh child geo, depending on what is needed.

		if (newDefaultContentSize != m_defaultContentSize || newDefaultSize != m_size)
		{
			m_defaultContentSize = newDefaultContentSize;
			_requestResize();
		}
		else
			_refreshChildGeo();
	}


	//____ _resize() ____________________________________________________________

	void PackPanel::_resize( const SizeSPX& size, int scale )
	{
		if( scale != m_scale )
		{
			for (auto pSlot = slots._begin(); pSlot != slots._end(); pSlot++)
			{
				pSlot->m_bResizeRequired = true;
				pSlot->m_defaultSize = pSlot->_paddedDefaultSize(scale);
			}
		}
		
		Panel::_resize(size, scale);
		_refreshChildGeo(true);			// Needs to requestRender() if size was not changed.
	}

	//____ _calcDefaultSize() ______________________________________________________

	SizeSPX PackPanel::_calcDefaultSize( int scale ) const
	{
		spx length = 0;
		spx breadth = 0;

		if (slots._size() > 0)
		{
			if( m_pLayout && m_pLayout->doesCalcWantedLength() )
			{
				// Allocate and populate SizeBroker array

				int arrayBytes = (sizeof(PackLayout::Item)+sizeof(spx)) * slots.size();
				PackLayout::Item* pItemArea = reinterpret_cast<PackLayout::Item*>(Base::memStackAlloc(arrayBytes));
				spx* pOutput = (spx*) & pItemArea[slots.size()];

				int nItems = _populateLayoutArray(pItemArea);

				// Retrieve preferred length and breadth

				length = m_pLayout->getWantedSizes(pOutput, m_scale, nItems, pItemArea);

				PackLayout::Item* pI = pItemArea;
				for (auto pS = slots._begin(); pS != slots._end(); pS++)
				{
					if( pS->m_bVisible )
					{
						spx b = m_bHorizontal?pS->_paddedMatchingHeight(*pOutput, scale):pS->_paddedMatchingWidth(*pOutput, scale);
						if( b > breadth )
							breadth = b;
						pI++;
						pOutput++;
					}
				}

				// Release temporary memory area

				Base::memStackFree(arrayBytes);

			}
			else
			{
				if( scale == m_scale )
				{
					if( m_bHorizontal )
					{
						for (auto p = slots._begin(); p != slots._end(); p++)
						{
							if( p->m_bVisible )
							{
								length += p->m_defaultSize.w;
								if( p->m_defaultSize.h > breadth )
									breadth = p->m_defaultSize.h;
							}
						}
					}
					else
					{
						for (auto p = slots._begin(); p != slots._end(); p++)
						{
							if( p->m_bVisible )
							{
								length += p->m_defaultSize.h;
								if( p->m_defaultSize.w > breadth )
									breadth = p->m_defaultSize.w;
							}
						}
					}
				}
				else
				{
					if( m_bHorizontal )
					{
						for (auto p = slots._begin(); p != slots._end(); p++)
						{
							if( p->m_bVisible )
							{
								SizeSPX defaultSize = p->_widget()->_defaultSize(scale);
								length += defaultSize.w;
								if( defaultSize.h > breadth )
									breadth = defaultSize.h;
							}
						}
					}
					else
					{
						for (auto p = slots._begin(); p != slots._end(); p++)
						{
							if( p->m_bVisible )
							{
								SizeSPX defaultSize = p->_widget()->_defaultSize(scale);
								length += defaultSize.h;
								if( defaultSize.w > breadth )
									breadth = defaultSize.w;
							}
						}
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
		CoordSPX contentOfs = m_skin.contentOfs(m_scale, State::Normal);			//TODO: Support offset changing in different states.

		spx wantedLength = m_bHorizontal?m_defaultContentSize.w:m_defaultContentSize.h;
		spx givenLength = m_bHorizontal?sz.w:sz.h;
		spx givenBreadth = m_bHorizontal?sz.h:sz.w;

		// Optimized special case, just copy preferred to length.
		//TODO: We probably need to use matchingWidth()/matchingHeight() here anyway... prefered length might change with given breadth

		if( !m_pLayout || (wantedLength == givenLength && !m_pLayout->doesCalcWantedLength()) )
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
						geo.w = p->m_defaultSize.w;
						geo.h = sz.h;
						pos.x += p->m_defaultSize.w;
					}
					else
					{
						geo.w = sz.w;
						geo.h = p->m_defaultSize.h;
						pos.y += p->m_defaultSize.h;
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
							p->_widget()->_resize( geo.size(), m_scale );
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
					p->_widget()->_resize(geo.size(), m_scale);
					p->m_bResizeRequired = false;
				}
			}
		}
		else
		{
			// Allocate and populate SizeBroker array

			int arrayBytes = (sizeof(PackLayout::Item)+sizeof(spx)) * slots.size();
			PackLayout::Item* pItemArea = reinterpret_cast<PackLayout::Item*>(Base::memStackAlloc(arrayBytes));
			spx* pOutput = (spx*) &pItemArea[slots.size()];

			int nItems = _populateLayoutArray(pItemArea, givenBreadth);

			// Retrieve length and set geo for all children, call _requestRender() and _setSize() where needed.


			m_pLayout->getItemSizes(pOutput, givenLength, m_scale, nItems, pItemArea);

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
						geo.w = *pOutput;
						geo.h = sz.h;
						pos.x += *pOutput;
					}
					else
					{
						geo.w = sz.w;
						geo.h = *pOutput;
						pos.y += *pOutput;
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
							p->_widget()->_resize( geo.size(), m_scale );
							p->m_bResizeRequired = false;
						}
					}
					pOutput++;
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
					p->_widget()->_resize(geo.size(), m_scale);
					p->m_bResizeRequired = false;
				}
			}

			// Release SizeBroker array

			Base::memStackFree(arrayBytes);
		}
	}

	//____ _populateLayoutArray() ___________________________________________

	int PackPanel::_populateLayoutArray(PackLayout::Item* pArray ) const
	{
		PackLayout::Item* pI = pArray;

		if( m_bHorizontal )
		{
			for (auto pS = slots._begin(); pS != slots._end(); pS++)
			{
				if( pS->m_bVisible )
				{
					pI->def = pS->m_defaultSize.w;
					pI->min = pS->_paddedMinSize(m_scale).w;
					pI->max = pS->_paddedMaxSize(m_scale).w;
					pI->weight = pS->m_weight*65536;
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
					pI->def = pS->m_defaultSize.h;
					pI->min = pS->_paddedMinSize(m_scale).h;
					pI->max = pS->_paddedMaxSize(m_scale).h;
					pI->weight = pS->m_weight*65536;
					pI++;
				}
			}
		}

		return int(pI - pArray);
	}

	int PackPanel::_populateLayoutArray(PackLayout::Item* pArray, spx forcedBreadth ) const
	{
		PackLayout::Item* pI = pArray;

		if( m_bHorizontal )
		{
			for (auto pS = slots._begin(); pS != slots._end(); pS++)
			{
				if( pS->m_bVisible )
				{
					pI->def = pS->_paddedMatchingWidth(forcedBreadth,m_scale);
					pI->min = pS->_paddedMinSize(m_scale).w;
					pI->max = pS->_paddedMaxSize(m_scale).w;
					pI->weight = pS->m_weight*65536;
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
					pI->def = pS->_paddedMatchingHeight(forcedBreadth,m_scale);
					pI->min = pS->_paddedMinSize(m_scale).h;
					pI->max = pS->_paddedMaxSize(m_scale).h;
					pI->weight = pS->m_weight*65536;
					pI++;
				}
			}
		}

		return int(pI - pArray);
	}


} // namespace wg
