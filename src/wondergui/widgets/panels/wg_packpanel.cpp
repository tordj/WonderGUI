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
#include <wg_panel.impl.h>


namespace wg
{
	using namespace Util;

	template class DynamicSlotVector<PackPanelSlot>;
	template class Panel<PackPanelSlot>;

	const TypeInfo PackPanel::TYPEINFO = { "PackPanel", &Panel::TYPEINFO };
	const TypeInfo PackPanelSlot::TYPEINFO = { "PackPanelSlot", &DynamicSlot::TYPEINFO };


	bool PackPanelSlot::_setBlueprint( const Blueprint& bp )
	{
		m_weight = bp.weight;
		m_bVisible = bp.visible;
		return true;
	}

	void PackPanelSlot::setWeight(float weight)
	{ 
		if (weight != m_weight) 
			static_cast<PackPanel*>(_holder())->_reweightSlots(this, 1, weight); 
	}

	//____ constructor ____________________________________________________________

	PackPanel::PackPanel()
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

	//____ setSpacing() _______________________________________________________

	void PackPanel::setSpacing(pts before, pts between, pts after)
	{
		if (before == m_spacingBefore && between == m_spacingBetween && after == m_spacingAfter)
			return;

		m_spacingBefore = before;
		m_spacingBetween = between;
		m_spacingAfter = after;
		_refreshGeometries();
	}

	void PackPanel::setSpacing(pts between)
	{
		if (0 == m_spacingBefore && between == m_spacingBetween && 0 == m_spacingAfter)
			return;

		m_spacingBefore = 0;
		m_spacingBetween = between;
		m_spacingAfter = 0;
		_refreshGeometries();
	}

	//____ setWeight() ___________________________________________________________

	bool PackPanel::setSlotWeight(int index, int amount, float weight)
	{
		if (index < 0 || amount <= 0 || index + amount > slots.size() || weight < 0.f)
			return false;

		_reweightSlots( &slots[index], amount, weight);
		return true;
	}

	bool PackPanel::setSlotWeight(iterator beg, iterator end, float weight)
	{
		_reweightSlots(beg, int(end - beg), weight);
		return true;
	}

	bool PackPanel::setSlotWeight(int index, int amount, std::initializer_list<float> weights)
	{
		if (index < 0 || amount <= 0 || index + amount > slots.size() || amount >(int) weights.size())
			return false;

		_reweightSlots(&slots[index], amount, weights.begin());
		return true;
	}

	bool PackPanel::setSlotWeight(iterator beg, iterator end, std::initializer_list<float> weights)
	{
		if( weights.size() < (end - beg) )
			return false;

		_reweightSlots(beg, int(end - beg), weights.begin());
		return true;
	}



	//____ _defaultSize() _______________________________________________________

	SizeSPX PackPanel::_defaultSize(int scale) const
	{
		if (scale != m_scale)
			return _calcDefaultSize(scale);
		else
			return m_defaultContentSize + m_skin.contentBorderSize(scale);
	}

	//____ _matchingHeight() _______________________________________________________

	spx PackPanel::_matchingHeight( spx width, int scale ) const
	{
		spx height = 0;

		if (slots.size() > 0)
		{
			if( m_bHorizontal )
			{
				width -= align(ptsToSpx(m_spacingBefore, scale)) +
					align(ptsToSpx(m_spacingAfter, scale)) +
					align(ptsToSpx(m_spacingBetween, scale)) * (slots.size() - 1);

				if( m_pLayout )
				{
					// Allocate and populate Layout array

					int arrayBytes = (sizeof(PackLayout::Item) + sizeof(spx)) * slots.size();
					PackLayout::Item * pItemArea = reinterpret_cast<PackLayout::Item*>(Base::memStackAlloc(arrayBytes));
					spx* pOutput = (spx*)&pItemArea[slots.size()];

					int nItems = _populateLayoutArray(pItemArea);

					// Retrieve item lengths and find height of highest item.

					m_pLayout->getItemSizes(pOutput, width, m_scale, nItems, pItemArea);

					for( auto pS = slots.begin() ; pS != slots.end() ; pS++ )
					{
						if( pS->m_bVisible )
						{
							spx itemHeight = pS->_widget()->_matchingHeight( *pOutput, scale );
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
						for( auto pS = slots.begin() ; pS != slots.end() ; pS++ )
						{
							if( pS->m_bVisible && pS->m_defaultSize.h > height )
									height = pS->m_defaultSize.h;
						}
					}
					else
					{
						for (auto pS = slots.begin(); pS != slots.end(); pS++)
						{
							if (pS->m_bVisible)
							{
								spx h = pS->_widget()->_defaultSize(scale).h;
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
					for( auto pS = slots.begin() ; pS != slots.end() ; pS++ )
					{
						if( pS->m_bVisible )
							height += pS->_widget()->_matchingHeight( width, scale );
					}
				}

				height += align(ptsToSpx(m_spacingBefore, scale)) +
					align(ptsToSpx(m_spacingAfter, scale)) +
					align(ptsToSpx(m_spacingBetween, scale)) * (slots.size() - 1);
			}
		}

		height += m_skin.contentBorderSize(scale).h;

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
				height -= align(ptsToSpx(m_spacingBefore, scale)) +
					align(ptsToSpx(m_spacingAfter, scale)) +
					align(ptsToSpx(m_spacingBetween, scale)) * (slots.size() - 1);

				if( m_pLayout )
				{
					// Allocate and populate SizeBroker array

					int arrayBytes = (sizeof(PackLayout::Item)+sizeof(spx)) * slots.size();
					PackLayout::Item* pItemArea = reinterpret_cast<PackLayout::Item*>(Base::memStackAlloc(arrayBytes));
					spx* pOutput = (spx*)&pItemArea[slots.size()];

					int nItems = _populateLayoutArray(pItemArea);

					// Retrieve item lengths and find height of highest item.


					m_pLayout->getItemSizes(pOutput, height, m_scale, nItems, pItemArea);

					for( auto pS = slots.begin() ; pS != slots.end() ; pS++ )
					{
						if( pS->m_bVisible )
						{
							spx itemWidth = pS->_widget()->_matchingWidth( *pOutput, scale );
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
						for (auto pS = slots.begin(); pS != slots.end(); pS++)
						{
							if (pS->m_bVisible && pS->m_defaultSize.w > width)
								width = pS->m_defaultSize.w;
						}
					}
					else
					{
						for (auto pS = slots.begin(); pS != slots.end(); pS++)
						{
							if (pS->m_bVisible)
							{
								spx w = pS->_widget()->_defaultSize(scale).w;
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
					for( auto pS = slots.begin() ; pS != slots.end() ; pS++ )
					{
						if( pS->m_bVisible )
							width += pS->_widget()->_matchingWidth( height, scale );
					}
				}

				width += align(ptsToSpx(m_spacingBefore, scale)) +
					align(ptsToSpx(m_spacingAfter, scale)) +
					align(ptsToSpx(m_spacingBetween, scale)) * (slots.size() - 1);
			}
		}

		width += m_skin.contentBorderSize(scale).w;

		return width;
	}

	//____ _slotTypeInfo() ________________________________________________________

	const TypeInfo&	PackPanel::_slotTypeInfo(const StaticSlot * pSlot) const
	{
		return PackPanelSlot::TYPEINFO;
	}


	//____ _didAddSlots() _____________________________________________________

	void PackPanel::_didAddSlots(StaticSlot * _pSlot, int nb)
	{
		auto pSlot = static_cast<PackPanelSlot*>(_pSlot);

		for (int i = 0; i < nb; i++)
		{
			if (pSlot[i].m_bVisible == true)
				pSlot[i].m_defaultSize = pSlot[i]._widget()->_defaultSize(m_scale);
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
		_hideChildren((PackPanelSlot*) pSlot, nb);
	}

	//____ _hideSlots() _______________________________________________________

	void PackPanel::_hideSlots(StaticSlot * pSlot, int nb)
	{
		_hideChildren((PackPanelSlot*) pSlot, nb);
	}

	//____ _unhideSlots() _____________________________________________________

	void PackPanel::_unhideSlots(StaticSlot * pSlot, int nb)
	{
		_unhideChildren((PackPanelSlot*) pSlot, nb);
	}

	//____ _reweightSlots() ______________________________________________________

	void PackPanel::_reweightSlots(PackPanelSlot * pSlot, int nb, float weight)
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
				_refreshChildGeo(true);
		}
	}

	void PackPanel::_reweightSlots(PackPanelSlot * pSlot, int nb, const float * pWeights)
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
				_refreshChildGeo(true);
		}
	}

	//____ _childRequestRender() ______________________________________________

	void PackPanel::_childRequestRender(StaticSlot * pSlot)
	{
		_requestRender( ((PackPanelSlot*)pSlot)->m_geo );
	}

	void PackPanel::_childRequestRender(StaticSlot * pSlot, const RectSPX& rect)
	{
		_requestRender(rect + ((PackPanelSlot*)pSlot)->m_geo.pos());
	}

	//____ _childRequestResize() ______________________________________________

	void PackPanel::_childRequestResize(StaticSlot * _pSlot)
	{
		// Update cached preferred size of child

		auto pSlot = static_cast<PackPanelSlot*>(_pSlot);
		pSlot->m_defaultSize = pSlot->_widget()->_defaultSize(m_scale);
		pSlot->m_bResizeRequired = true;

		_refreshGeometries();
	}

	//____ _releaseChild() ____________________________________________________

	void PackPanel::_releaseChild(StaticSlot * pSlot)
	{
		_willEraseSlots(pSlot, 1);
		slots.erase(static_cast<PackPanelSlot*>(pSlot));
	}

	//____ _replaceChild() _____________________________________________________

	void PackPanel::_replaceChild(StaticSlot * _pSlot, Widget * pNewChild)
	{
		//TODO: Looks like we need to refresh geometry here or something...

		auto pSlot = static_cast<PackPanelSlot* > (_pSlot);
		slots._releaseGuardPointer(pNewChild, &pSlot);

		pSlot->_setWidget(pNewChild);

		if (pSlot->m_bVisible)
		{
			pSlot->m_bVisible = true;
			pSlot->m_defaultSize = pSlot->_widget()->_defaultSize(m_scale);
		}

	}


	//____ _unhideChildren() _______________________________________________________

	void PackPanel::_unhideChildren(PackPanelSlot * pSlot, int nb)
	{
		for (int i = 0; i < nb; i++)
		{
			if (pSlot[i].m_bVisible == false)
			{
				pSlot[i].m_bVisible = true;
				pSlot[i].m_defaultSize = pSlot[i]._widget()->_defaultSize(m_scale);
			}
		}

		_refreshGeometries();
	}

	//____ _hideChildren() _______________________________________________________

	void PackPanel::_hideChildren(PackPanelSlot * pSlot, int nb)
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
		SizeSPX newDefaultSize = newDefaultContentSize + m_skin.contentBorderSize(m_scale);

		// request resize or just refresh child geo, depending on what is needed.

		if (newDefaultContentSize != m_defaultContentSize || newDefaultSize != m_size)
		{
			m_defaultContentSize = newDefaultContentSize;
			_requestResize();
		}
		else
			_refreshChildGeo(true);
	}


	//____ _resize() ____________________________________________________________

	void PackPanel::_resize( const SizeSPX& size, int scale )
	{
		if( scale != m_scale )
		{
			for (auto pSlot = slots.begin(); pSlot != slots.end(); pSlot++)
			{
				pSlot->m_bResizeRequired = true;
				pSlot->m_defaultSize = pSlot->_widget()->_defaultSize(scale);
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

		if (slots.size() > 0)
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
				for (auto pS = slots.begin(); pS != slots.end(); pS++)
				{
					if( pS->m_bVisible )
					{
						spx b = m_bHorizontal?pS->_widget()->_matchingHeight(*pOutput, scale):pS->_widget()->_matchingWidth(*pOutput, scale);
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
						for (auto p = slots.begin(); p != slots.end(); p++)
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
						for (auto p = slots.begin(); p != slots.end(); p++)
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
						for (auto p = slots.begin(); p != slots.end(); p++)
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
						for (auto p = slots.begin(); p != slots.end(); p++)
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

			length += align(ptsToSpx(m_spacingBefore, m_scale)) +
				align(ptsToSpx(m_spacingAfter, m_scale)) +
				align(ptsToSpx(m_spacingBetween, m_scale)) * (slots.size() - 1);
		}

		//


		return m_bHorizontal?SizeSPX(length,breadth):SizeSPX(breadth,length);
	}

	//____ _refreshChildGeo() _________________________________________________________

	void PackPanel::_refreshChildGeo( bool bRequestRender )
	{
		if( slots.isEmpty() )
			return;

		SizeSPX sz = m_size - m_skin.contentBorderSize(m_scale);
		CoordSPX contentOfs = m_skin.contentOfs(m_scale, State::Normal);			//TODO: Support offset changing in different states.

		spx wantedLength = m_bHorizontal?m_defaultContentSize.w:m_defaultContentSize.h;
		spx givenLength = m_bHorizontal?sz.w:sz.h;
		spx givenBreadth = m_bHorizontal?sz.h:sz.w;

		spx spacingBefore = align(ptsToSpx(m_spacingBefore, m_scale));
		spx spacingAfter = align(ptsToSpx(m_spacingAfter, m_scale));
		spx spacingBetween = align(ptsToSpx(m_spacingBetween, m_scale));

		givenLength -= spacingBefore + spacingAfter + spacingBetween * (slots.size()-1);

		// Optimized special case, just copy preferred to length.
		//TODO: We probably need to use matchingWidth()/matchingHeight() here anyway... prefered length might change with given breadth

		if( !m_pLayout || (wantedLength == givenLength && !m_pLayout->doesCalcWantedLength()) )
		{
			CoordSPX pos;
			RectSPX geo;

			if (m_bHorizontal)
				pos.x = spacingBefore;
			else
				pos.y = spacingBefore;

			for (auto p = slots.begin(); p != slots.end(); p++)
			{
				if( p->m_bVisible )
				{
					geo.x = pos.x;
					geo.y = pos.y;
					if( m_bHorizontal )
					{
						geo.w = p->m_defaultSize.w;
						geo.h = sz.h;
						pos.x += p->m_defaultSize.w + spacingBetween;
					}
					else
					{
						geo.w = sz.w;
						geo.h = p->m_defaultSize.h;
						pos.y += p->m_defaultSize.h + spacingBetween;
					}
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

			if (m_bHorizontal)
				pos.x = spacingBefore;
			else
				pos.y = spacingBefore;

			for (auto p = slots.begin(); p != slots.end(); p++)
			{
				if( p->m_bVisible )
				{
					geo.x = pos.x;
					geo.y = pos.y;
					if( m_bHorizontal )
					{
						geo.w = *pOutput;
						geo.h = sz.h;
						pos.x += *pOutput + spacingBetween;
					}
					else
					{
						geo.w = sz.w;
						geo.h = *pOutput;
						pos.y += *pOutput + spacingBetween;
					}
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
			for (auto pS = slots.begin(); pS != slots.end(); pS++)
			{
				if( pS->m_bVisible )
				{
					pI->def = pS->m_defaultSize.w;
					pI->min = pS->_widget()->_minSize(m_scale).w;
					pI->max = pS->_widget()->_maxSize(m_scale).w;
					pI->weight = pS->m_weight*65536;
					pI++;
				}
			}
		}
		else
		{
			for (auto pS = slots.begin(); pS != slots.end(); pS++)
			{
				if( pS->m_bVisible )
				{
					pI->def = pS->m_defaultSize.h;
					pI->min = pS->_widget()->_minSize(m_scale).h;
					pI->max = pS->_widget()->_maxSize(m_scale).h;
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
			for (auto pS = slots.begin(); pS != slots.end(); pS++)
			{
				if( pS->m_bVisible )
				{
					pI->def = pS->_widget()->_matchingWidth(forcedBreadth,m_scale);
					pI->min = pS->_widget()->_minSize(m_scale).w;
					pI->max = pS->_widget()->_maxSize(m_scale).w;
					pI->weight = pS->m_weight*65536;
					pI++;
				}
			}
		}
		else
		{
			for (auto pS = slots.begin(); pS != slots.end(); pS++)
			{
				if( pS->m_bVisible )
				{
					pI->def = pS->_widget()->_matchingHeight(forcedBreadth,m_scale);
					pI->min = pS->_widget()->_minSize(m_scale).h;
					pI->max = pS->_widget()->_maxSize(m_scale).h;
					pI->weight = pS->m_weight*65536;
					pI++;
				}
			}
		}

		return int(pI - pArray);
	}


} // namespace wg
