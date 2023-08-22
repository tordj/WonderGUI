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

#include <wg_twoslotpanel.h>
#include <wg_patches.h>
#include <wg_msg.h>

//TODO: Set opaque if there is no skin (or skin has no borders) and both children and the handle are opaque.

namespace wg
{
	const TypeInfo TwoSlotPanel::TYPEINFO = { "TwoSlotPanel", &Panel::TYPEINFO };
	const TypeInfo TwoSlotPanel::Slot::TYPEINFO = { "TwoSlotPanel::Slot", &DynamicSlot::TYPEINFO };

	using namespace Util;


	//____ Slot::_setBlueprint() _________________________________________________

	bool TwoSlotPanel::Slot::_setBlueprint( const Blueprint& bp )
	{
		m_weight		= bp.weight;
		return true;
	}

	//____ constructor ____________________________________________________________

	TwoSlotPanel::TwoSlotPanel() : slots(this)
	{
		m_bHorizontal = false;
		m_bSiblingsOverlap = false;
	}

	//____ Destructor _____________________________________________________________

	TwoSlotPanel::~TwoSlotPanel()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& TwoSlotPanel::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setAxis() _______________________________________________________

	void TwoSlotPanel::setAxis(Axis axis)
	{
		bool bHorizontal = (axis == Axis::X);

		if (bHorizontal != m_bHorizontal)
		{
			m_bHorizontal = bHorizontal;
			m_defaultSize = _calcDefaultSize(m_scale);

			// Clear these so _updateGeo() doesn't skip resize despite
			// orientation change.

			slots[0].m_length = 0;
			slots[1].m_length = 0;

			_requestResize();
/*
			// This should only be done if _requestResize() fails.
			{
				_requestRender();
				_updateGeo();
			}
*/
		}
	}

	//____ setLayout() ____________________________________________________

	void TwoSlotPanel::setLayout(PackLayout* pLayout)
	{
		if (pLayout != m_pLayout)
		{
			m_pLayout = pLayout;
			m_defaultSize = _calcDefaultSize(m_scale);
			_requestResize();
// This will only be needed once _requestResize can fail.
//			_updateGeo();
		}
	}



	//____ _defaultSize() _______________________________________________________

	SizeSPX  TwoSlotPanel::_defaultSize(int scale) const
	{
		if (scale == m_scale)
			return m_defaultSize;
		else
			return _calcDefaultSize(scale);
	}

	//____ _updateDefaultSize() ________________________________________________

	void TwoSlotPanel::_updateDefaultSize()
	{
		SizeSPX sz = _calcDefaultSize(m_scale);
		//TODO: We can't trust that same defaultSize results in same matchingHeight. We need to find a more robust optimization.
//		if (sz != m_defaultSize)
		{
			m_defaultSize = sz;
			_requestResize();
		}

	}
	
	//____ _calcDefaultSize() _______________________________________________

	SizeSPX TwoSlotPanel::_calcDefaultSize(int scale) const
	{
		//TODO: Use layouts default size function if present!

		SizeSPX firstSz;
		SizeSPX secondSz;

		SizeSPX sz;

		if (slots[0]._widget())
			firstSz = slots[0]._widget()->_defaultSize(scale);

		if (slots[1]._widget())
			secondSz = slots[1]._widget()->_defaultSize(scale);

		if (m_bHorizontal)
		{
			sz.w = firstSz.w + secondSz.w;
			sz.h = std::max(firstSz.h, secondSz.h);
		}
		else
		{
			sz.w = std::max(firstSz.w, secondSz.w);
			sz.h = firstSz.h + secondSz.h;
		}

		// Take skins padding and default size into account

		if (!m_skin.isEmpty())
		{
			sz += m_skin.contentPaddingSize(scale);
			SizeSPX skinSz = m_skin.defaultSize(scale);
			if (skinSz.w > sz.w)
				sz.w = skinSz.w;
			if (skinSz.h > sz.h)
				sz.h = skinSz.h;
		}

		return sz;
	}

	//____ _updateGeo() __________________________________________________________

	bool TwoSlotPanel::_updateGeo(bool bForce)
	{
		RectSPX contentGeo = _contentRect();


		// Calculate new lengths using broker


		spx totalLength = (m_bHorizontal ? contentGeo.w : contentGeo.h);
		spx len1 = 0, len2 = 0;

		if (m_pLayout)
		{
			PackLayout::Item items[2];
			PackLayout::Item* pI = items;

			spx	results[2];

			if (m_bHorizontal)
			{
				for (int i = 0; i < 2; i++)
				{
					if (slots[i].m_pWidget)
					{
						pI->def = slots[i].m_pWidget->_matchingWidth(contentGeo.h, m_scale);
						pI->min = slots[i].m_pWidget->_minSize(m_scale).w;
						pI->max = slots[i].m_pWidget->_maxSize(m_scale).w;
						pI->weight = slots[i].m_weight;
						pI++;
					}
				}
			}
			else
			{
				for (int i = 0; i < 2; i++)
				{
					if (slots[i].m_pWidget)
					{
						pI->def = slots[i].m_pWidget->_matchingHeight(contentGeo.w, m_scale);
						pI->min = slots[i].m_pWidget->_minSize(m_scale).h;
						pI->max = slots[i].m_pWidget->_maxSize(m_scale).h;
						pI->weight = slots[i].m_weight;
						pI++;
					}
				}
			}

			if (pI > items)
			{
				spx combinedLength =  m_pLayout->getItemSizes(results, m_bHorizontal ? contentGeo.w : contentGeo.h, m_scale, pI - items, items );

				if (slots[0].m_pWidget)
				{
					len1 = results[0];
					combinedLength -= len1;
				}

				len2 = combinedLength;
			}

		}
		else
		{
			// Default behavior is to divide the length between the two widgets
			// relative to their preferred length, but keep them within their min/max lengths.

			spx minLen1 = 0, minLen2 = 0;
			spx maxLen1 = 0, maxLen2 = 0;
			if (m_bHorizontal)
			{
				if (slots[0].m_pWidget)
				{
					len1 = slots[0].m_pWidget->_matchingWidth(contentGeo.h, m_scale);
					minLen1 = slots[0].m_pWidget->_minSize(m_scale).w;
					maxLen1 = slots[0].m_pWidget->_maxSize(m_scale).w;
				}

				if (slots[1].m_pWidget)
				{
					len2 = slots[1].m_pWidget->_matchingWidth(contentGeo.h, m_scale);
					minLen2 = slots[1].m_pWidget->_minSize(m_scale).w;
					maxLen2 = slots[1].m_pWidget->_maxSize(m_scale).w;
				}
			}
			else
			{
				if (slots[0].m_pWidget)
				{
					len1 = slots[0].m_pWidget->_matchingHeight(contentGeo.w, m_scale);
					minLen1 = slots[0].m_pWidget->_minSize(m_scale).h;
					maxLen1 = slots[0].m_pWidget->_maxSize(m_scale).h;
				}

				if (slots[1].m_pWidget)
				{
					len2 = slots[1].m_pWidget->_matchingHeight(contentGeo.w, m_scale);
					minLen2 = slots[1].m_pWidget->_minSize(m_scale).h;
					maxLen2 = slots[1].m_pWidget->_maxSize(m_scale).h;
				}
			}

			if (minLen1 + minLen2 > totalLength)
			{
				len1 = minLen1;
				len2 = minLen2;
			}
			else if (maxLen1 < totalLength && maxLen2 < totalLength && maxLen1 + maxLen2 < totalLength)
			{
				// ^ Checked them individually first, so we don't get overflow when adding two large numbers.

				len1 = maxLen1;
				len2 = maxLen2;
			}
			else
			{
				len1 = align(int32_t(int64_t(len1 * totalLength) / (len1 + len2)));

				if (len1 > maxLen1)
					len1 = maxLen1;
				else if (len1 < minLen1)
					len1 = minLen1;

				len2 = totalLength - len1;
				if (len2 > maxLen2)
				{
					len2 = maxLen2;
					len1 = totalLength - len2;
				}
				else if (len2 < minLen2)
				{
					len2 = minLen2;
					len1 = totalLength - len2;
				}
			}
		}
	
		// Request render and set sizes
		
		if (bForce || len1 != slots[0].m_length || len2 != slots[1].m_length)
		{
			_requestRender(contentGeo);

			slots[0].m_length = len1;
			if( slots[0]._widget() )
				slots[0]._setSize(_slotOneRect(contentGeo), m_scale);

			slots[1].m_length = len2;
			if (slots[1]._widget())
				slots[1]._setSize(_slotTwoRect(contentGeo), m_scale);

			return true;
		}

		return false;
	}


	//____ _collectPatches() __________________________________________________

	void TwoSlotPanel::_collectPatches(PatchesSPX& container, const RectSPX& geo, const RectSPX& clip)
	{
		if (m_skin.isEmpty())
		{
			RectSPX contentRect = _contentRect(geo);

			if (slots[0]._widget())
				slots[0]._widget()->_collectPatches(container, _slotOneRect(contentRect), clip);

			if (slots[1]._widget())
				slots[1]._widget()->_collectPatches(container, _slotTwoRect(contentRect), clip);
		}
		else
			container.add(RectSPX::overlap(geo, clip));
	}

	//____ _maskPatches() _____________________________________________________

	void TwoSlotPanel::_maskPatches(PatchesSPX& patches, const RectSPX& geo, const RectSPX& clip, BlendMode blendMode)
	{
		{
			//TODO: Don't just check isOpaque() globally, check rect by rect.
			if ((m_bOpaque && blendMode == BlendMode::Blend) || blendMode == BlendMode::Replace)
				patches.sub(RectSPX::overlap(geo, clip));
			else
			{
				RectSPX contentRect = _contentRect(geo);

				if (slots[0]._widget())
					slots[0]._widget()->_maskPatches(patches, _slotOneRect(contentRect), clip, blendMode );

				if (slots[1]._widget())
					slots[1]._widget()->_maskPatches(patches, _slotTwoRect(contentRect), clip, blendMode );
			}
		}
	}

	//____ _resize() _________________________________________________________

	void TwoSlotPanel::_resize(const SizeSPX& size, int scale)
	{
		spx breadthDiff = m_bHorizontal ? m_size.w - size.w : m_size.h - size.h;
		bool bForceUpdate = (scale != m_scale || breadthDiff != 0);
			
		Panel::_resize(size,scale);
		_updateGeo( bForceUpdate );
			
	}

	//____ _slotTypeInfo() ________________________________________________________

	const TypeInfo&	TwoSlotPanel::_slotTypeInfo(const StaticSlot * pSlot) const
	{
		return Slot::TYPEINFO;
	}

	//_____ _firstChild() _____________________________________________________

	Widget * TwoSlotPanel::_firstChild() const
	{
		if (slots[0]._widget())
			return slots[0]._widget();
		else
			return slots[1]._widget();
	}

	//_____ _lastChild() ______________________________________________________

	Widget * TwoSlotPanel::_lastChild() const
	{
		if (slots[1]._widget())
			return slots[1]._widget();
		else
			return slots[0]._widget();
	}

	//_____ _firstSlotWithGeo() _______________________________________________

	void TwoSlotPanel::_firstSlotWithGeo(SlotWithGeo& package) const
	{
		if (slots[0]._widget())
		{
			package.geo = _slotOneRect(_contentRect());
			package.pSlot = &slots[0];
		}
		else if (slots[1]._widget())
		{
			package.geo = _slotTwoRect(_contentRect());
			package.pSlot = &slots[1];
		}
		else
		{
			package.pSlot = nullptr;
		}
	}

	//____ _nextSlotWithGeo() _________________________________________________

	void TwoSlotPanel::_nextSlotWithGeo(SlotWithGeo& package) const
	{
		if (package.pSlot == &slots[0] && slots[1]._widget())
		{
			package.geo = _slotTwoRect(_contentRect());
			package.pSlot = &slots[1];
		}
		else
			package.pSlot = nullptr;
	}

	//_____ _childPos() _______________________________________________________

	CoordSPX TwoSlotPanel::_childPos(const StaticSlot * pSlot) const
	{
		CoordSPX pos = m_skin.contentOfs(m_scale, m_state);

		if (pSlot == &slots[1])
		{
			if (m_bHorizontal)
				pos.x += slots[0].m_length;
			else
				pos.y += slots[0].m_length;
		}

		return pos;
	}

	//____ _childRequestRender() ______________________________________________

	void TwoSlotPanel::_childRequestRender(StaticSlot * pSlot)
	{
		if (pSlot == &slots[0])
			_requestRender(_slotOneRect(_contentRect()));
		else
			_requestRender(_slotTwoRect(_contentRect()));
	}

	void TwoSlotPanel::_childRequestRender(StaticSlot * pSlot, const RectSPX& rect)
	{
		CoordSPX contentPos = m_skin.contentOfs(m_scale, m_state);

		if (pSlot == &slots[1])
		{
			if (m_bHorizontal)
				contentPos.x += slots[0].m_length;
			else
				contentPos.y += slots[0].m_length;
		}

		_requestRender(rect + contentPos);
	}

	//____ _childRequestResize() ______________________________________________

	void TwoSlotPanel::_childRequestResize(StaticSlot * pSlot)
	{
		m_defaultSize = _calcDefaultSize(m_scale);
		_requestResize();
	}

	//____ _prevChild() _______________________________________________________

	Widget * TwoSlotPanel::_prevChild(const StaticSlot * pSlot) const
	{
		if (pSlot == &slots[1])
			return slots[0]._widget();
		else
			return nullptr;
	}

	//____ _nextChild() _______________________________________________________

	Widget * TwoSlotPanel::_nextChild(const StaticSlot * pSlot) const
	{
		if (pSlot == &slots[0])
			return slots[1]._widget();
		else
			return nullptr;
	}

	//____ _releaseChild() ____________________________________________________

	void TwoSlotPanel::_releaseChild(StaticSlot * pSlot)
	{
		_replaceChild(pSlot, nullptr);
	}

	//____ _replaceChild() _______________________________________________________

	void TwoSlotPanel::_replaceChild(StaticSlot* _pSlot, Widget* pNewWidget)
	{
		auto pSlot = static_cast<Slot*>(_pSlot);

		pSlot->_setWidget(pNewWidget);

		RectSPX rect;

		if (pSlot == &slots[0])
			rect = _slotOneRect(_contentRect());
		else
			rect = _slotTwoRect(_contentRect());

		//TODO: This is in most cases an unnecessary resize call, should be avoided when we can.
		pNewWidget->_resize(rect.size(), m_scale);
		
		m_defaultSize = _calcDefaultSize(m_scale);
		_requestResize();
	
/*
*	Needs to be called if we are not resized.
* 
		_updateGeo();
*/
	}


}