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

	using namespace Util;

	//____ constructor ____________________________________________________________

	SplitPanel::SplitPanel() : slots(this), handleSkin(this)
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

	//____ setAxis() _______________________________________________________

	void SplitPanel::setAxis(Axis axis)
	{
		bool bHorizontal = (axis == Axis::X);

		if (bHorizontal != m_bHorizontal)
		{
			m_bHorizontal = bHorizontal;
			_refresh();
		}
	}

	//____ setHandleThickness() __________________________________________________

	void SplitPanel::setHandleThickness(pts thickness)
	{
		//TODO: Assert on negative value.

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

	void SplitPanel::setBrokerFunction(std::function<pts(Widget * pFirst, Widget * pSecond, pts totalLength, float fraction, pts handleMovement)> func)
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

	SizeSPX  SplitPanel::_preferredSize(int scale) const
	{
		scale = _fixScale(scale);

		if (scale == m_scale)
			return m_preferredSize;
		else
			return _calcPreferredSize(scale);
	}

	//____ _handleThickness() ____________________________________________________

	spx SplitPanel::_handleThickness(int scale) const
	{
		spx thickness = align(ptsToSpx(m_handleThickness,scale));
		if (thickness == 0 )
			thickness = m_bHorizontal ? OO(handleSkin)._preferredSize(scale).w : OO(handleSkin)._preferredSize(scale).h;
		return thickness;
	}


	//____ _updatePreferredSize() ________________________________________________

	void SplitPanel::_updatePreferredSize()
	{
		SizeSPX sz = _calcPreferredSize(m_scale);
		//TODO: We can't trust that same preferredSize results in same matchingHeight. We need to find a more robust optimization.
//		if (sz != m_preferredSize)
		{
			m_preferredSize = sz;
			_requestResize();
		}

	}
	
	//____ _calcPreferredSize() _______________________________________________

	SizeSPX SplitPanel::_calcPreferredSize(int scale) const
	{
		SizeSPX firstSz;
		SizeSPX secondSz;

		SizeSPX sz;

		if (slots[0]._widget())
			firstSz = slots[0]._widget()->_preferredSize(scale);

		if (slots[1]._widget())
			secondSz = slots[1]._widget()->_preferredSize(scale);

		if (m_bHorizontal)
		{
			sz.w = firstSz.w + secondSz.w + _handleThickness(scale);
			sz.h = max(firstSz.h, secondSz.h);
			if (OO(handleSkin)._preferredSize(scale).h > sz.h)
				sz.h = OO(handleSkin)._preferredSize(scale).h;
		}
		else
		{
			sz.w = max(firstSz.w, secondSz.w);
			sz.h = firstSz.h + secondSz.h + _handleThickness(scale);
			if (OO(handleSkin)._preferredSize(scale).w > sz.w)
				sz.w = OO(handleSkin)._preferredSize(scale).w;
		}

		// Take skins padding and preferred size into account

		if (!skin.isEmpty())
		{
			sz += OO(skin)._contentPaddingSize(scale);
			SizeSPX skinSz = OO(skin)._preferredSize(scale);
			if (skinSz.w > sz.w)
				sz.w = skinSz.w;
			if (skinSz.h > sz.h)
				sz.h = skinSz.h;
		}

		return sz;
	}

	//____ _updateGeo() __________________________________________________________

	bool SplitPanel::_updateGeo(spx handleMovement)
	{
		RectSPX geo = m_size;
		RectSPX contentGeo = OO(skin)._contentRect(geo, m_scale, m_state);

		RectSPX firstChildGeo;
		RectSPX secondChildGeo;
		RectSPX handleGeo;

		// Calculate new lengths using broker

		spx handleThickness = _handleThickness(m_scale);

		spx totalLength = (m_bHorizontal ? contentGeo.w : contentGeo.h) - handleThickness;
		spx	length;
		spx firstChildLength;
		spx secondChildLength;

		if (m_brokerFunc)
		{
			pts len = m_brokerFunc(slots[0]._widget(), slots[1]._widget(), spxToPts(totalLength,m_scale), m_splitFactor, spxToPts(handleMovement,m_scale));
			length = ptsToSpx(len, m_scale);
		}
		else
			length = _defaultBroker(slots[0]._widget(), slots[1]._widget(), totalLength, m_splitFactor, handleMovement);

		firstChildLength = align(length);

		secondChildLength = totalLength - firstChildLength;

		// Update lengthFraction if we had handle movement

		if (handleMovement != 0 )
			m_splitFactor = (firstChildLength+32) / (float) totalLength;

		// Update geo rectangles

		if( m_bHorizontal )
		{
			firstChildGeo = RectSPX(contentGeo.x, contentGeo.y, firstChildLength, contentGeo.h);
			secondChildGeo = RectSPX(contentGeo.x + contentGeo.w - secondChildLength, contentGeo.y, secondChildLength, contentGeo.h);
			handleGeo = RectSPX(contentGeo.x + firstChildLength, contentGeo.y, handleThickness, contentGeo.h );
		}
		else
		{
			firstChildGeo = RectSPX(contentGeo.x, contentGeo.y, contentGeo.w, firstChildLength);
			secondChildGeo = RectSPX(contentGeo.x, contentGeo.y + contentGeo.h - secondChildLength, contentGeo.w, secondChildLength);
			handleGeo = RectSPX(contentGeo.x, contentGeo.y + firstChildLength, contentGeo.w, handleThickness);
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

	spx SplitPanel::_defaultBroker(Widget * pFirst, Widget * pSecond, spx totalLength, float splitFactor, spx handleMovement)
	{
		spx firstLength;

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
				firstLength = (splitFactor * totalLength) + 0.5f;
				break;
			}
		}
		else
			firstLength = (m_bHorizontal ? slots[0].m_geo.w : slots[0].m_geo.h) + handleMovement;

		spx minLengthFirst = 0;
		spx minLengthSecond = 0;
		spx maxLengthFirst = spx_max;
		spx maxLengthSecond = spx_max;

		if( m_bHorizontal )
		{
			if (pFirst)
			{
				minLengthFirst = slots[0]._widget()->minSize().w;
				maxLengthFirst = slots[0]._widget()->maxSize().w;
			}

			if (pSecond)
			{
				minLengthSecond = slots[1]._widget()->minSize().w;
				maxLengthSecond = slots[1]._widget()->maxSize().w;
			}
		}
		else
		{
			if (pFirst)
			{
				minLengthFirst = slots[0]._widget()->minSize().h;
				maxLengthFirst = slots[0]._widget()->maxSize().h;
			}

			if (pSecond)
			{
				minLengthSecond = slots[1]._widget()->minSize().h;
				maxLengthSecond = slots[1]._widget()->maxSize().h;
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

		return firstLength;
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

				CoordSPX pos = static_cast<InputMsg*>(pMsg)->_pointerPos() - _globalPos();

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

				CoordSPX pos = p->_pointerPos() - _globalPos();
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
					CoordSPX pos = p->_pointerPos() - _globalPos();
					spx diff = (m_bHorizontal ? pos.x - m_handleGeo.x : pos.y - m_handleGeo.y) - m_handlePressOfs;

					_updateGeo(diff);
					pMsg->swallow();
				}
				break;
			}

			default:
				break;
		}

		State oldHandleState = m_handleState;
		m_handleState = handleState;

		OO(handleSkin)._stateChanged(handleState, oldHandleState);
	}


	//____ _render() _____________________________________________________________

	void SplitPanel::_render(GfxDevice * pDevice, const RectSPX& canvas, const RectSPX& window)
	{
		Panel::_render(pDevice, canvas, window);

		OO(handleSkin)._render(pDevice, m_handleGeo + canvas.pos(), m_scale, m_handleState);
	}


	//____ _collectPatches() __________________________________________________

	void SplitPanel::_collectPatches(Patches& container, const RectSPX& geo, const RectSPX& clip)
	{
		if (skin.isEmpty())
		{
			if (slots[0]._widget())
				OO(slots[0]._widget())->_collectPatches(container, slots[0].m_geo + geo.pos(), clip);

			if (!handleSkin.isEmpty())
				container.add(RectSPX(m_handleGeo, clip));

			if (slots[1]._widget())
				OO(slots[1]._widget())->_collectPatches(container, slots[1].m_geo + geo.pos(), clip);
		}
		else
			container.add(RectSPX(geo, clip));
	}

	//____ _maskPatches() _____________________________________________________

	void SplitPanel::_maskPatches(Patches& patches, const RectSPX& geo, const RectSPX& clip, BlendMode blendMode)
	{
		{
			//TODO: Don't just check isOpaque() globally, check rect by rect.
			if ((m_bOpaque && blendMode == BlendMode::Blend) || blendMode == BlendMode::Replace)
				patches.sub(RectSPX(geo, clip));
			else
			{
				if (slots[0]._widget())
					OO(slots[0]._widget())->_maskPatches(patches, slots[0].m_geo + geo.pos(), clip, blendMode );

				if (OO(handleSkin)._isOpaque() )
					patches.sub(RectSPX(m_handleGeo, clip));

				if (slots[1]._widget())
					OO(slots[1]._widget())->_maskPatches(patches, slots[1].m_geo + geo.pos(), clip, blendMode );
			}
		}
	}

	//____ _alphaTest() _______________________________________________________

	bool SplitPanel::_alphaTest(const CoordSPX& ofs)
	{
		bool bHit = Panel::_alphaTest(ofs);

		if( !bHit )
			bHit = OO(handleSkin)._markTest(ofs, m_handleGeo, m_scale, m_handleState, m_markOpacity);

		return bHit;
	}

	//____ _cloneContent() ____________________________________________________

	void SplitPanel::_cloneContent(const Widget * _pOrg)
	{
		//TODO: Implement!!!
	}

	//____ _resize() _________________________________________________________

	void SplitPanel::_resize(const SizeSPX& size, int scale)
	{
		Panel::_resize(size,scale);
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

	CoordSPX SplitPanel::_childPos(const StaticSlot * pSlot) const
	{
		return static_cast<const Slot*>(pSlot)->m_geo.pos();
	}

	//____ _childRequestRender() ______________________________________________

	void SplitPanel::_childRequestRender(StaticSlot * pSlot)
	{
		_requestRender(static_cast<Slot*>(pSlot)->m_geo);
	}

	void SplitPanel::_childRequestRender(StaticSlot * pSlot, const RectSPX& rect)
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
		OO(pNewWidget)->_resize(pSlot->m_geo, m_scale);
		_updatePreferredSize();
		bool bGeoChanged = _updateGeo();
		if (!bGeoChanged)
			_requestRender(pSlot->m_geo);
	}

	//____ _componentState() _______________________________________________________

	State SplitPanel::_componentState(const GeoComponent* pComponent) const
	{
		if (pComponent == &handleSkin)
			return m_handleState;
		else
			return m_state;
	}

	//____ _componentPos() ___________________________________________________

	CoordSPX SplitPanel::_componentPos(const GeoComponent* pComponent) const
	{
		if (pComponent == &handleSkin)
			return m_handleGeo.pos();
		else
			return CoordSPX();
	}

	//____ _componentSize() ___________________________________________________________

	SizeSPX SplitPanel::_componentSize(const GeoComponent* pComponent) const
	{
		if (pComponent == &handleSkin)
			return m_handleGeo.size();
		else
			return m_size;
	}

	//____ _componentGeo() ___________________________________________________________

	RectSPX SplitPanel::_componentGeo(const GeoComponent* pComponent) const
	{
		if (pComponent == &handleSkin)
			return m_handleGeo;
		else
			return m_size;
	}

	//____ _componentRequestRender() _______________________________________________

	void SplitPanel::_componentRequestRender(const GeoComponent* pSlot)
	{
		if (pSlot == &handleSkin)
			_requestRender(m_handleGeo);
		else
			_requestRender();
	}

	void SplitPanel::_componentRequestRender(const GeoComponent* pSlot, const RectSPX& rect)
	{
		if (pSlot == &handleSkin)
			_requestRender(rect + m_handleGeo.pos());
		else
			_requestRender(rect);
	}

	//____ _skinChanged() _____________________________________________________

	void SplitPanel::_skinChanged(const CSkinSlot* pSlot, Skin* pNewSkin, Skin* pOldSkin)
	{
		_updatePreferredSize();
		_updateGeo();
	}

}