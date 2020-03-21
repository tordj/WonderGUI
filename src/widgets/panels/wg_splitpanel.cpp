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

#include <wg_splitpanel.h>
#include <wg_patches.h>
#include <wg_msg.h>
#include <wg_internal.h>

//TODO: Set opaque if there is no skin (or skin has no borders) and both children and the handle are opaque.

namespace wg
{
	const TypeInfo SplitPanel::TYPEINFO = { "SplitPanel", &Panel::TYPEINFO };
	const TypeInfo SplitPanel::Slot::TYPEINFO = { "SplitPanel::Slot", &DynamicSlot::TYPEINFO };

	//____ constructor ____________________________________________________________

	SplitPanel::SplitPanel() : slots(this)
	{
		m_bHorizontal = false;
		m_handleThickness = 0;
		m_splitFactor = 0.5f;
		m_scaleBehavior = ScaleBehavior::ScaleBoth;

		m_bSiblingsOverlap = false;
	}

	//____ Destructor _____________________________________________________________

	SplitPanel::~SplitPanel()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& SplitPanel::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setOrientation() _______________________________________________________

	void SplitPanel::setOrientation(Orientation orientation)
	{
		bool bHorizontal = (orientation == Orientation::Horizontal);

		if (bHorizontal != m_bHorizontal)
		{
			m_bHorizontal = bHorizontal;
			_refresh();
		}
	}

	//____ setHandleSkin() _______________________________________________________

	void SplitPanel::setHandleSkin(Skin * pSkin)
	{
		if (pSkin != m_pHandleSkin)
		{
			m_pHandleSkin = pSkin;
			_updatePreferredSize();
			bool bGeoChanged = _updateGeo();
			if (!bGeoChanged)
				_requestRender(m_handleGeo);
		}
	}

	//____ setHandleThickness() __________________________________________________

	void SplitPanel::setHandleThickness(QPix _thickness)
	{
		//TODO: Assert on negative value.

		int thickness = _thickness.rawAligned();

		if (thickness != m_handleThickness)
		{
			m_handleThickness = thickness;
			_updatePreferredSize();
			_updateGeo();
		}
	}

	//____ setScaleBehavior() ___________________________________________________

	void SplitPanel::setScaleBehavior(ScaleBehavior behavior)
	{
		if (behavior != m_scaleBehavior)
		{
			m_scaleBehavior = behavior;
		}
	}

	//____ setBrokerFunction() ___________________________________________________

	void SplitPanel::setBrokerFunction(std::function<QPix(Widget * pFirst, Widget * pSecond, QPix totalLength, float fraction, QPix handleMovement)> func)
	{
		m_brokerFunc = func;
		_updateGeo();
	}

	//____ setSplitFactor() _________________________________________________

	void SplitPanel::setSplitFactor(float factor)
	{
		limit(factor, 0.f, 1.f);
		m_splitFactor = factor;
		_updateGeo();
	}

	//____ _preferredSize() _______________________________________________________

	SizeI  SplitPanel::_preferredSize() const
	{
		return m_preferredSize;
	}

	//____ _handleThickness() ____________________________________________________

	int SplitPanel::_handleThickness()
	{
		int thickness = m_handleThickness;
		if (thickness == 0 && m_pHandleSkin)
			thickness = m_bHorizontal ? m_pHandleSkin->_preferredSize().w : m_pHandleSkin->_preferredSize().h;
		return thickness;
	}


	//____ _updatePreferredSize() ________________________________________________

	void SplitPanel::_updatePreferredSize()
	{
		SizeI firstSz;
		SizeI secondSz;

		SizeI sz;

		if (slots[0]._widget())
			firstSz = slots[0]._preferredSize();

		if (slots[1]._widget())
			secondSz = slots[1]._preferredSize();

		if (m_bHorizontal)
		{
			sz.w = firstSz.w + secondSz.w + _handleThickness();
			sz.h = max(firstSz.h, secondSz.h);
			if (m_pHandleSkin && m_pHandleSkin->_preferredSize().h > sz.h)
				sz.h = m_pHandleSkin->_preferredSize().h;
		}
		else
		{
			sz.w = max(firstSz.w, secondSz.w);
			sz.h = firstSz.h + secondSz.h + _handleThickness();
			if (m_pHandleSkin && m_pHandleSkin->_preferredSize().w > sz.w)
				sz.w = m_pHandleSkin->_preferredSize().w;
		}

		// Take skins padding and preferred size into account

		if (m_pSkin)
		{
			sz += m_pSkin->_contentPaddingSize();
			SizeI skinSz = m_pSkin->_preferredSize();
			if (skinSz.w > sz.w)
				sz.w = skinSz.w;
			if (skinSz.h > sz.h)
				sz.h = skinSz.h;
		}

//TODO: We can't trust that same preferredSize results in same matchingHeight. We need to find a more robust optimization.
//		if (sz != m_preferredSize)
		{
			m_preferredSize = sz;
			_requestResize();
		}
	}

	//____ _updateGeo() __________________________________________________________

	bool SplitPanel::_updateGeo(int handleMovement)
	{
		RectI geo = m_size;
		RectI contentGeo = m_pSkin ? m_pSkin->_contentRect(geo, m_state) : geo;

		RectI firstChildGeo;
		RectI secondChildGeo;
		RectI handleGeo;

		// Calculate new lengths using broker

		int handleThickness = _handleThickness();

		int totalLength = (m_bHorizontal ? contentGeo.w : contentGeo.h) - handleThickness;
		QPix	length;
		int firstChildLength;
		int secondChildLength;

		if (m_brokerFunc)
			length = m_brokerFunc(slots[0]._widget(), slots[1]._widget(), QPix::fromRaw(totalLength), m_splitFactor, QPix::fromRaw(handleMovement));
		else
			length = _defaultBroker(slots[0]._widget(), slots[1]._widget(), QPix::fromRaw(totalLength), m_splitFactor, QPix::fromRaw(handleMovement));

		firstChildLength = length.rawAligned();

		secondChildLength = totalLength - firstChildLength;

		// Update lengthFraction if we had handle movement

		if (handleMovement )
			m_splitFactor = (firstChildLength+0.5f) / totalLength;

		// Update geo rectangles

		if( m_bHorizontal )
		{
			firstChildGeo = RectI(contentGeo.x, contentGeo.y, firstChildLength, contentGeo.h);
			secondChildGeo = RectI(contentGeo.x + contentGeo.w - secondChildLength, contentGeo.y, secondChildLength, contentGeo.h);
			handleGeo = RectI(contentGeo.x + firstChildLength, contentGeo.y, handleThickness, contentGeo.h );
		}
		else
		{
			firstChildGeo = RectI(contentGeo.x, contentGeo.y, contentGeo.w, firstChildLength);
			secondChildGeo = RectI(contentGeo.x, contentGeo.y + contentGeo.h - secondChildLength, contentGeo.w, secondChildLength);
			handleGeo = RectI(contentGeo.x, contentGeo.y + firstChildLength, contentGeo.w, handleThickness);
		}

		// Request render and set sizes

		if (handleGeo != m_handleGeo || firstChildGeo != slots[0].m_geo || secondChildGeo != slots[1].m_geo)
		{
			_requestRender(contentGeo);

			slots[0].m_geo = firstChildGeo;
			if( slots[0]._widget() )
				OO(slots[0]._widget())->_resize(firstChildGeo);

			slots[1].m_geo = secondChildGeo;
			if (slots[1]._widget())
				OO(slots[1]._widget())->_resize(secondChildGeo);

			m_handleGeo = handleGeo;

			return true;
		}

		return false;
	}

	//____ _defaultBroker() ___________________________________________________

	QPix SplitPanel::_defaultBroker(Widget * pFirst, Widget * pSecond, QPix _totalLength, float splitFactor, QPix _handleMovement)
	{
		int handleMovement = _handleMovement.raw;
		int totalLength = _totalLength.raw;

		int firstLength;

		if (handleMovement == 0)
		{
			switch (m_scaleBehavior)
			{
			case ScaleBehavior::ScaleFirst:
				firstLength = totalLength - (m_bHorizontal ? slots[1].m_geo.w : slots[1].m_geo.h);
				break;
			case ScaleBehavior::ScaleSecond:
				firstLength = m_bHorizontal ? slots[0].m_geo.w : slots[0].m_geo.h;
				break;
			case ScaleBehavior::ScaleBoth:
				firstLength = (int)((splitFactor * totalLength) + 0.5f);
				break;
			}
		}
		else
			firstLength = (m_bHorizontal ? slots[0].m_geo.w : slots[0].m_geo.h) + handleMovement;

		int minLengthFirst = 0;
		int minLengthSecond = 0;
		int maxLengthFirst = INT_MAX;
		int maxLengthSecond = INT_MAX;

		if( m_bHorizontal )
		{
			if (pFirst)
			{
				minLengthFirst = slots[0]._minSize().w;
				maxLengthFirst = slots[0]._maxSize().w;
			}

			if (pSecond)
			{
				minLengthSecond = slots[1]._minSize().w;
				maxLengthSecond = slots[1]._maxSize().w;
			}
		}
		else
		{
			if (pFirst)
			{
				minLengthFirst = slots[0]._minSize().h;
				maxLengthFirst = slots[0]._maxSize().h;
			}

			if (pSecond)
			{
				minLengthSecond = slots[1]._minSize().h;
				maxLengthSecond = slots[1]._maxSize().h;
			}
		}

		if (totalLength - firstLength < minLengthSecond)
			firstLength = totalLength - minLengthSecond;

		if (firstLength < minLengthFirst)
			firstLength = minLengthFirst;

		if (totalLength - firstLength > maxLengthSecond)
			firstLength = totalLength - maxLengthSecond;

		if (firstLength > maxLengthFirst)
			firstLength = maxLengthFirst;

		return QPix::fromRaw(firstLength);
	}

	//____ _refresh() ____________________________________________________________

	void SplitPanel::_refresh()
	{
		_updateGeo();
		_requestRender();
	}

	//____ _receive() ___________________________________________________________

	void SplitPanel::_receive(Msg * pMsg)
	{
		//TODO: Implement!!!

		State handleState = m_handleState;

		switch (pMsg->type())
		{
			case MsgType::MouseEnter:
			case MsgType::MouseMove:
			{
				if (m_handleState.isPressed())
					return;

				CoordI pos = static_cast<InputMsg*>(pMsg)->pointerPosRaw() - _globalPos();

				bool bHovered = m_handleGeo.contains(pos);
				handleState.setHovered(bHovered);
				break;
			}

			case MsgType::MouseLeave:
			{
				// Unhover handle unless it is pressed

				if (!handleState.isPressed())
					handleState.setHovered(false);
				break;
			}
			case MsgType::MousePress:
			{
				auto p = static_cast<MouseButtonMsg*>(pMsg);

				if (p->button() != MouseButton::Left)
					return;

				CoordI pos = p->pointerPosRaw() - _globalPos();
				if (m_handleGeo.contains(pos))
				{
					m_handlePressOfs = m_bHorizontal ? pos.x - m_handleGeo.x : pos.y - m_handleGeo.y;
					handleState.setPressed(true);
					pMsg->swallow();
				}
				break;
			}

			case MsgType::MouseRelease:
			{
				auto p = static_cast<MouseButtonMsg*>(pMsg);

				if (p->button() != MouseButton::Left)
					return;

				if (handleState.isPressed() )
				{
					handleState.setPressed(false);
					pMsg->swallow();
				}
				break;
			}

			case MsgType::MouseDrag:
			{
				auto p = static_cast<MouseButtonMsg*>(pMsg);

				if (p->button() != MouseButton::Left)
					return;

				if (handleState.isPressed())
				{
					CoordI pos = p->pointerPosRaw() - _globalPos();
					int diff = (m_bHorizontal ? pos.x - m_handleGeo.x : pos.y - m_handleGeo.y) - m_handlePressOfs;

					_updateGeo(diff);
					pMsg->swallow();
				}
				break;
			}

			default:
				break;
		}

		if (handleState != m_handleState && m_pHandleSkin)
		{
			if (!m_pHandleSkin->isStateIdentical(handleState, m_handleState))
				_requestRender(m_handleGeo);

			m_handleState = handleState;
		}
	}


	//____ _render() _____________________________________________________________

	void SplitPanel::_render(GfxDevice * pDevice, const RectI& _canvas, const RectI& _window)
	{
		Panel::_render(pDevice, _canvas, _window);

		if (m_pHandleSkin)
			m_pHandleSkin->_render(pDevice, m_handleGeo, m_handleState);
	}


	//____ _collectPatches() __________________________________________________

	void SplitPanel::_collectPatches(Patches& container, const RectI& geo, const RectI& clip)
	{
		if (m_pSkin)
			container.add(RectI(geo, clip));
		else
		{
			if (slots[0]._widget())
				OO(slots[0]._widget())->_collectPatches(container, slots[0].m_geo + geo.pos(), clip );

			if( m_pHandleSkin )
				container.add(RectI(m_handleGeo, clip));

			if (slots[1]._widget())
				OO(slots[1]._widget())->_collectPatches(container, slots[1].m_geo + geo.pos(), clip );
		}
	}

	//____ _maskPatches() _____________________________________________________

	void SplitPanel::_maskPatches(Patches& patches, const RectI& geo, const RectI& clip, BlendMode blendMode)
	{
		{
			//TODO: Don't just check isOpaque() globally, check rect by rect.
			if ((m_bOpaque && blendMode == BlendMode::Blend) || blendMode == BlendMode::Replace)
				patches.sub(RectI(geo, clip));
			else
			{
				if (slots[0]._widget())
					OO(slots[0]._widget())->_maskPatches(patches, slots[0].m_geo + geo.pos(), clip, blendMode );

				if (m_pHandleSkin && m_pHandleSkin->isOpaque() )
					patches.sub(RectI(m_handleGeo, clip));

				if (slots[1]._widget())
					OO(slots[1]._widget())->_maskPatches(patches, slots[1].m_geo + geo.pos(), clip, blendMode );
			}
		}
	}

	//____ _alphaTest() _______________________________________________________

	bool SplitPanel::_alphaTest(const CoordI& ofs)
	{
		bool bHit = Panel::_alphaTest(ofs);

		if( !bHit && m_pHandleSkin )
			bHit = m_pHandleSkin->_markTest(ofs, m_handleGeo, m_handleState, m_markOpacity);

		return bHit;
	}

	//____ _cloneContent() ____________________________________________________

	void SplitPanel::_cloneContent(const Widget * _pOrg)
	{
		//TODO: Implement!!!
	}

	//____ _resize() _________________________________________________________

	void SplitPanel::_resize(const SizeI& size)
	{
		Panel::_resize(size);
		_updateGeo();
	}

	//____ _setState() ________________________________________________________

	void SplitPanel::_setState(State state)
	{
		// Disable handle when panel is disabled

		m_handleState.setEnabled(state.isEnabled());
		Panel::_setState(state);
	}

	//____ _slotTypeInfo() ________________________________________________________

	const TypeInfo&	SplitPanel::_slotTypeInfo(const StaticSlot * pSlot) const
	{
		return Slot::TYPEINFO;
	}

	//_____ _firstChild() _____________________________________________________

	Widget * SplitPanel::_firstChild() const
	{
		if (slots[0]._widget())
			return slots[0]._widget();
		else
			return slots[1]._widget();
	}

	//_____ _lastChild() ______________________________________________________

	Widget * SplitPanel::_lastChild() const
	{
		if (slots[1]._widget())
			return slots[1]._widget();
		else
			return slots[0]._widget();
	}

	//_____ _firstSlotWithGeo() _______________________________________________

	void SplitPanel::_firstSlotWithGeo(SlotWithGeo& package) const
	{
		if (slots[0]._widget())
		{
			package.geo = slots[0].m_geo;
			package.pSlot = &slots[0];
		}
		else if (slots[1]._widget())
		{
			package.geo = slots[1].m_geo;
			package.pSlot = &slots[1];
		}
		else
		{
			package.pSlot = nullptr;
		}
	}

	//____ _nextSlotWithGeo() _________________________________________________

	void SplitPanel::_nextSlotWithGeo(SlotWithGeo& package) const
	{
		if (package.pSlot == &slots[0] && slots[1]._widget())
		{
			package.geo = slots[1].m_geo;
			package.pSlot = &slots[1];
		}
		else
			package.pSlot = nullptr;
	}

	//_____ _childPos() _______________________________________________________

	CoordI SplitPanel::_childPos(const StaticSlot * pSlot) const
	{
		return static_cast<const Slot*>(pSlot)->m_geo.pos();
	}

	//____ _childRequestRender() ______________________________________________

	void SplitPanel::_childRequestRender(StaticSlot * pSlot)
	{
		_requestRender(static_cast<Slot*>(pSlot)->m_geo);
	}

	void SplitPanel::_childRequestRender(StaticSlot * pSlot, const RectI& rect)
	{
		_requestRender(rect + static_cast<Slot*>(pSlot)->m_geo.pos());
	}

	//____ _childRequestResize() ______________________________________________

	void SplitPanel::_childRequestResize(StaticSlot * pSlot)
	{
		//TODO: Implement better solution, should be able to adapt width !!!

		auto p = static_cast<Slot*>(pSlot);
		p->_setSize(p->m_geo);
	}

	//____ _prevChild() _______________________________________________________

	Widget * SplitPanel::_prevChild(const StaticSlot * pSlot) const
	{
		if (pSlot == &slots[1])
			return slots[0]._widget();
		else
			return nullptr;
	}

	//____ _nextChild() _______________________________________________________

	Widget * SplitPanel::_nextChild(const StaticSlot * pSlot) const
	{
		if (pSlot == &slots[0])
			return slots[1]._widget();
		else
			return nullptr;
	}

	//____ _releaseChild() ____________________________________________________

	void SplitPanel::_releaseChild(StaticSlot * pSlot)
	{
		_replaceChild(pSlot, nullptr);
	}

	//____ _replaceChild() _______________________________________________________

	void SplitPanel::_replaceChild(StaticSlot * _pSlot, Widget * pNewWidget)
	{
		auto pSlot = static_cast<Slot*>(_pSlot);

		pSlot->_setWidget(pNewWidget);
		OO(pNewWidget)->_resize(pSlot->m_geo);
		_updatePreferredSize();
		bool bGeoChanged = _updateGeo();
		if (!bGeoChanged)
			_requestRender(pSlot->m_geo);
	}
}
