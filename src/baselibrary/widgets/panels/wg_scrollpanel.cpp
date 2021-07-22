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

#include <wg_scrollpanel.h>
#include <wg_gfxdevice.h>
#include <wg_util.h>
#include <wg_patches.h>
#include <wg_msg.h>
#include <wg_msgrouter.h>
#include <wg_internal.h>

#include <algorithm>

namespace wg
{
	using namespace Util;

	const TypeInfo ScrollPanel::TYPEINFO = { "ScrollPanel", &Panel::TYPEINFO };

	//____ Constructor _______________________________________________________

	ScrollPanel::ScrollPanel() : slot(this), scrollbarX(this, this, Axis::X), scrollbarY(this, this, Axis::Y)
	{
		scrollbarX._setState(StateEnum::Disabled);
		scrollbarY._setState(StateEnum::Disabled);

	}

	//____ Destructor _________________________________________________________

	ScrollPanel::~ScrollPanel()
	{

	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& ScrollPanel::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setPlacement() _____________________________________________________

	void ScrollPanel::setPlacement(Placement placement)
	{
		if (placement == m_smallChildPlacement)
			return;

		if (m_childCanvas.w < m_viewRegion.w || m_childCanvas.h < m_viewRegion.h)
		{
			_childWindowCorrection();
		}

	}

	void ScrollPanel::setWidthConstraint(SizeConstraint rule, pts value)
	{

	}

	void ScrollPanel::setHeightConstraint(SizeConstraint rule, pts value)
	{

	}

	//____ setAutohideScrollbars() ____________________________________________

	void ScrollPanel::setAutohideScrollbars(bool scrollbarX, bool scrollbarY)
	{
		if (scrollbarX == m_bAutohideScrollbarX && scrollbarY == m_bAutohideScrollbarY)
			return;

		m_bAutohideScrollbarX = scrollbarX;
		m_bAutohideScrollbarY = scrollbarY;
		_updateRegions();
	}

	//____ setOverlayScrollbars() _____________________________________________

	void ScrollPanel::setOverlayScrollbars(bool scrollbarX, bool scrollbarY)
	{
		if (scrollbarX == m_bOverlayScrollbarX && scrollbarY == m_bOverlayScrollbarY)
			return;

		m_bOverlayScrollbarX = scrollbarX;
		m_bOverlayScrollbarY = scrollbarY;
		_updateRegions();
	}

	//____ setStepSize() ______________________________________________________

	void ScrollPanel::setStepSize(pts stepSize)
	{
		m_stepSizeX = stepSize;
		m_stepSizeY = stepSize;
	}

	void ScrollPanel::setStepSize(pts x, pts y)
	{
		m_stepSizeX = x;
		m_stepSizeY = y;
	}

	//____ setWheelStepSize() _________________________________________________

	void ScrollPanel::setWheelStepSize(pts stepSize)
	{
		m_wheelStepSizeX = stepSize;
		m_wheelStepSizeY = stepSize;
	}

	void ScrollPanel::setWheelStepSize(pts x, pts y)
	{
		m_wheelStepSizeX = x;
		m_wheelStepSizeY = y;
	}

	//____ setPageOverlap() ___________________________________________________

	void ScrollPanel::setPageOverlap(pts overlap)
	{
		m_pageOverlapX = overlap;
		m_pageOverlapY = overlap;
	}

	void ScrollPanel::setPageOverlap(pts x, pts y)
	{
		m_pageOverlapX = x;
		m_pageOverlapY = y;
	}

	//____ setScrollWheels() __________________________________________________

	void ScrollPanel::setScrollWheels(int wheelForX, int wheelForY)
	{
		m_wheelForScrollX = wheelForX;
		m_wheelForScrollY = wheelForY;
	}

	//____ setScrollWheelAxisShift() __________________________________________

	void ScrollPanel::setScrollWheelAxisShift(ModifierKeys axisShift)
	{
		m_wheelAxisShiftCombo = axisShift;
	}

	//____ setStealWheelFromDragbars() ________________________________________

	void ScrollPanel::setStealWheelFromDragbars(bool bPrevent)
	{
		m_bStealWheelFromDragbars = bPrevent;
	}


	//____ _updateRegions() ___________________________________________________

	void ScrollPanel::_updateRegions()
	{
		auto content = _contentRect();

		spx scrollbarYwidth = scrollbarY._preferredSize(m_scale).w;
		spx scrollbarXwidth = scrollbarX._preferredSize(m_scale).h;

		bool bShowScrollbarY = scrollbarX.inWorkingOrder();
		bool bShowScrollbarX = scrollbarY.inWorkingOrder();

		if (bShowScrollbarY)
			m_scrollbarYRegion = {	content.x + content.w - scrollbarYwidth, content.y, 
									scrollbarYwidth, bShowScrollbarX ? content.h - scrollbarXwidth : content.h };

		if (bShowScrollbarX)
			m_scrollbarXRegion = {	content.x, content.y + content.h - scrollbarXwidth, 
									bShowScrollbarY ? content.w - scrollbarYwidth : content.w, scrollbarXwidth };

		if (bShowScrollbarY && !m_bOverlayScrollbarY)
			content.w -= scrollbarYwidth;

		if (bShowScrollbarX && !m_bOverlayScrollbarX)
			content.h -= scrollbarXwidth;

		m_viewRegion = content;
	}

	//____ _childWindowCorrection() ___________________________________________
	/*
		Recalculates childWindow based on viewRegion, canvas-size and some rules.
		childCanvas position is affected when view changes.
	*/
	
	void ScrollPanel::_childWindowCorrection()
	{
		auto oldWindow = m_childWindow;

		if (m_viewRegion.w > m_childCanvas.w)
		{
			switch (m_smallChildPlacement)
			{
				case Placement::West:
				case Placement::NorthWest:
				case Placement::SouthWest:
					m_childCanvas.x = m_viewRegion.x;
					break;

				case Placement::Center:
				case Placement::North:
				case Placement::South:
					m_childCanvas.x = m_viewRegion.x + align((m_viewRegion.w - m_childCanvas.w) / 2);
					break;

				case Placement::East:
				case Placement::NorthEast:
				case Placement::SouthEast:
					m_childCanvas.x = m_viewRegion.x + m_viewRegion.w - m_childCanvas.w;
					break;
			}

			m_childWindow.x = m_childCanvas.x;
			m_childWindow.w = m_childCanvas.w;
		}
		else
		{
			m_childWindow.x = m_viewRegion.x;
			m_childWindow.w = m_viewRegion.w;

			if (m_childWindow.x < m_childCanvas.x)
				m_childCanvas.x = m_childWindow.x;

			if (m_childWindow.x + m_childWindow.w > m_childCanvas.x + m_childCanvas.w )
				m_childCanvas.x = m_childWindow.x + m_childWindow.w - m_childCanvas.w;
		}

		if (m_viewRegion.h > m_childCanvas.h)
		{
			switch (m_smallChildPlacement)
			{
				case Placement::North:
				case Placement::NorthWest:
				case Placement::NorthEast:
					m_childCanvas.y = m_viewRegion.y;
					break;

				case Placement::Center:
				case Placement::West:
				case Placement::East:
					m_childCanvas.y = m_viewRegion.y + align((m_viewRegion.h - m_childCanvas.h) / 2);
					break;

				case Placement::South:
				case Placement::SouthWest:
				case Placement::SouthEast:
					m_childCanvas.y = m_viewRegion.y + m_viewRegion.h - m_childCanvas.h;
					break;
			}

			m_childWindow.y = m_childCanvas.y;
			m_childWindow.h = m_childCanvas.h;
		}
		else
		{
			m_childWindow.y = m_viewRegion.y;
			m_childWindow.h = m_viewRegion.h;

			if (m_childWindow.y < m_childCanvas.y)
				m_childCanvas.y = m_childWindow.y;

			if (m_childWindow.y + m_childWindow.h > m_childCanvas.y + m_childCanvas.h)
				m_childCanvas.y = m_childWindow.y + m_childWindow.h - m_childCanvas.h;
		}
	}

	//____ _updateCanvasSize() ________________________________________________

	void ScrollPanel::_updateCanvasSize()
	{
		auto pWidget = OO(slot)._widget();
		if (pWidget)
		{
			auto size = pWidget->_preferredSize(m_scale);

			if( size != m_childCanvas.size())
			{
				m_childCanvas.setSize(size);

			}


			if (pWidget->_size() != size || pWidget->scale() != m_scale)
				pWidget->_resize(size, m_scale);
		}
		else
			m_childCanvas.clear();
	}

	//____ _updateScrollbars() ________________________________________________

	void ScrollPanel::_updateScrollbars()
	{

		spx oldViewOfs, oldViewLen, oldCanvasLen;

		if (scrollbarX.inWorkingOrder() )
		{
			oldViewOfs = m_viewXOfs;
			oldViewLen = m_viewXLen;
			oldCanvasLen = m_canvasXLen;

			m_viewXOfs = m_childWindow.x - m_childCanvas.x;
			m_viewXLen = m_childWindow.w;
			m_canvasXLen = m_childCanvas.w;

			if( oldViewOfs != m_viewXOfs || oldViewLen != m_viewXLen || oldCanvasLen != m_canvasXLen )
				scrollbarX._update(m_viewXOfs, oldViewOfs, m_viewXLen, oldViewLen, m_canvasXLen, oldCanvasLen);

			if (m_viewXLen == m_canvasXLen && oldViewLen != oldCanvasLen)
				scrollbarX._setState(StateEnum::Disabled);
			else if (m_viewXLen != m_canvasXLen && oldViewLen == oldCanvasLen)
				scrollbarX._setState(StateEnum::Normal);
		}

		if (scrollbarY.inWorkingOrder())
		{
			oldViewOfs = m_viewYOfs;
			oldViewLen = m_viewYLen;
			oldCanvasLen = m_canvasYLen;

			m_viewYOfs = m_childWindow.y - m_childCanvas.y;
			m_viewYLen = m_childWindow.h;
			m_canvasYLen = m_childCanvas.h;

			if (oldViewOfs != m_viewYOfs || oldViewLen != m_viewYLen || oldCanvasLen != m_canvasYLen)
				scrollbarY._update(m_viewYOfs, oldViewOfs, m_viewYLen, oldViewLen, m_canvasYLen, oldCanvasLen);

			if (m_viewYLen == m_canvasYLen && oldViewLen != oldCanvasLen)
				scrollbarY._setState(StateEnum::Disabled);
			else if (m_viewYLen != m_canvasYLen && oldViewLen == oldCanvasLen)
				scrollbarY._setState(StateEnum::Normal);
		}

	}

	//____ _resize() __________________________________________________________

	void ScrollPanel::_resize(const SizeSPX& size, int scale)
	{
		scale = _fixScale(scale);

		bool bSizeChanged = (size != m_size);
		bool bScaleChanged = (scale != m_scale);

		m_size = size;
		m_scale = scale;

		if (bScaleChanged)
		{
			Widget* pChild = slot._widget();

			if (pChild)
			{
				auto childSize = pChild->_preferredSize(scale);
				m_childCanvas.setSize(childSize);
				pChild->_resize(childSize, scale);
			}
		}

		_updateRegions();
		_childWindowCorrection();
		_updateScrollbars();
	}

	//____ _receive() _________________________________________________________

	void ScrollPanel::_receive(Msg* pMsg)
	{
		bool bX = false, bY = false;

		// Give our dragbars the opportunity to process

		if (!(m_bStealWheelFromDragbars && pMsg->type() == MsgType::WheelRoll))
		{
			if (scrollbarX.inWorkingOrder())
				bX = scrollbarX._receive(pMsg);

			if (scrollbarY.inWorkingOrder())
				bY = scrollbarY._receive(pMsg);
		}

		// Leave if processed by our dragbars.

		if (bX || bY)
			return;

		// Process any messages not handled by our dragbars

		switch (pMsg->type())
		{
			case MsgType::WheelRoll:
			{
				auto p = static_cast<WheelRollMsg*>(pMsg);

				int wheelX = m_wheelForScrollX;
				int wheelY = m_wheelForScrollY;

				if (m_wheelAxisShiftCombo == p->modKeys())
					std::swap(wheelX, wheelY);

				spx movement = 0;

				if( p->wheel() == wheelX )
				{
					movement = p->distance().y * align(ptsToSpx(m_wheelStepSizeX, m_scale));
					m_childCanvas.x += movement;
				}
				else if( p->wheel() == wheelY )
				{
					movement = p->distance().y * align(ptsToSpx(m_wheelStepSizeY, m_scale));
					m_childCanvas.y += movement;
				}

				if (movement != 0)
				{
					_childWindowCorrection();
					_updateScrollbars();
					_requestRender();
					p->swallow();
				}

				break;
			}

			default:
				break;
		}



	}

	//____ _render() __________________________________________________________

	void ScrollPanel::_render(GfxDevice* pDevice, const RectSPX& canvas, const RectSPX& window)
	{
		Widget::_render(pDevice, canvas, window);

		if (!slot.isEmpty())
		{
			if( m_childWindow.size() == m_size )
				slot._widget()->_render(pDevice, m_childCanvas + canvas.pos(), m_childWindow + canvas.pos());
			else
			{
				auto clipPop = limitClipList(pDevice, m_childWindow + canvas.pos() );
				slot._widget()->_render(pDevice, m_childCanvas + canvas.pos(), m_childWindow + canvas.pos());
				popClipList(pDevice, clipPop);
			}
		}

		if (!m_scrollbarXRegion.isEmpty())
			scrollbarX._render(pDevice, m_scrollbarXRegion + canvas.pos(), m_scale);

		if (!m_scrollbarYRegion.isEmpty())
			scrollbarY._render(pDevice, m_scrollbarYRegion + canvas.pos(), m_scale);
	}

	//____ _collectPatches() __________________________________________________

	void ScrollPanel::_collectPatches(Patches& container, const RectSPX& geo, const RectSPX& clip)
	{
//		if( !skin.isEmpty() )
			container.add(RectSPX(geo, clip));
	}

	//____ _maskPatches() _____________________________________________________

	void ScrollPanel::_maskPatches(Patches& patches, const RectSPX& geo, const RectSPX& clip, BlendMode blendMode)
	{
		if (!skin.isEmpty() && OO(skin)._isOpaque())
			patches.sub(RectSPX(geo, clip));
		else
		{
			// Lets make it easy, only mask against our child, not the scrollbars with all their details.

			if (slot._widget())
				OO(slot._widget())->_maskPatches(patches, m_childCanvas + geo.pos(), RectSPX(clip, m_childWindow + geo.pos()), blendMode);
		}
	}

	//____ _alphaTest() _______________________________________________________

	bool ScrollPanel::_alphaTest(const CoordSPX& ofs)
	{
		// Test against our skin

		if (!skin.isEmpty() && skin->_markTest(ofs, m_size, m_scale, m_state, m_markOpacity))
			return true;

		// Test against our scrollbars.

		if (scrollbarX._alphaTest(ofs, m_scrollbarXRegion, m_markOpacity) ||
			scrollbarY._alphaTest(ofs, m_scrollbarYRegion, m_markOpacity))
			return true;

		return false;
	}

	//____ _cloneContent() ____________________________________________________

	void ScrollPanel::_cloneContent(const Widget* pOrg)
	{

	}

	//____ _slotTypeInfo() ____________________________________________________

	const TypeInfo& ScrollPanel::_slotTypeInfo(const StaticSlot* pSlot) const
	{
		return DynamicSlot::TYPEINFO;
	}

	//____ _findWidget() ______________________________________________________

	Widget* ScrollPanel::_findWidget(const CoordSPX& pos, SearchMode mode)
	{
		Widget* pFound = nullptr;

		if (!slot.isEmpty() && m_childWindow.contains(pos))
		{
			auto pChild = slot._widget();
			if (pChild->isContainer())
				pFound = OO(static_cast<Container*>(pChild))->_findWidget(pos - m_childCanvas.pos(), mode);
			else if (mode == SearchMode::Geometry || pChild->_alphaTest(pos - m_childCanvas.pos()))
				pFound = pChild;
		}

		if (!pFound)
		{
			// Test against ourself

			if (mode == SearchMode::Geometry || _alphaTest(pos) )
				pFound = this;
		}

		return pFound;
	}

	//____ _firstChild() ______________________________________________________

	Widget* ScrollPanel::_firstChild() const
	{
		return slot._widget();
	}

	//____ _lastChild() _______________________________________________________

	Widget* ScrollPanel::_lastChild() const
	{
		return slot._widget();
	}

	//____ _firstSlotWithGeo() ________________________________________________

	void ScrollPanel::_firstSlotWithGeo(SlotWithGeo& package) const
	{
		package.pSlot = &slot;
		package.geo = m_childCanvas;
	}

	//____ _nextSlotWithGeo() _________________________________________________

	void ScrollPanel::_nextSlotWithGeo(SlotWithGeo& package) const
	{
		package.pSlot = nullptr;
	}

	//____ _childPos() ________________________________________________________

	CoordSPX ScrollPanel::_childPos(const StaticSlot* pSlot) const
	{
		return m_childCanvas.pos();
	}

	//____ _childWindowSection() ______________________________________________

	RectSPX ScrollPanel::_childWindowSection(const StaticSlot* pSlot) const
	{
		return m_childWindow - m_childCanvas.pos();
	}

	//____ _childRequestRender() ______________________________________________

	void ScrollPanel::_childRequestRender(StaticSlot* pSlot)
	{
		_requestRender(m_viewRegion);
	}

	//____ _childRequestRender() ______________________________________________

	void ScrollPanel::_childRequestRender(StaticSlot* pSlot, const RectSPX& rect)
	{
		RectSPX visible(rect + m_childCanvas.pos(), m_viewRegion);
		if (!visible.isEmpty())
			_requestRender(visible);
	}

	//____ _childRequestResize() ______________________________________________

	void ScrollPanel::_childRequestResize(StaticSlot* pSlot)
	{
		_updateCanvasSize();
		_updateScrollbars();

		_requestResize();
	}

	//____ _childRequestInView() ______________________________________________

	void ScrollPanel::_childRequestInView(StaticSlot* pSlot)
	{
		// Do nothing. Our child is always in view.
	}

	void ScrollPanel::_childRequestInView(StaticSlot* pSlot, const RectSPX& mustHaveArea, const RectSPX& niceToHaveArea)
	{
		if( m_childWindow.contains( niceToHaveArea + m_childCanvas.pos() ) )
			return;

		//TODO: Implement!!!
	}

	//____ _prevChild() _______________________________________________________

	Widget* ScrollPanel::_prevChild(const StaticSlot* pSlot) const
	{
		return nullptr;
	}

	//____ _nextChild() _______________________________________________________

	Widget* ScrollPanel::_nextChild(const StaticSlot* pSlot) const
	{
		return nullptr;
	}

	//____ _releaseChild() ____________________________________________________

	void ScrollPanel::_releaseChild(StaticSlot* pSlot)
	{
		OO(slot)._setWidget(nullptr);

		m_childCanvas.clear();
		_childWindowCorrection();
		_updateScrollbars();

		_requestRender();
		_requestResize();
	}

	//____ _replaceChild() ____________________________________________________

	void ScrollPanel::_replaceChild(StaticSlot* pSlot, Widget* pWidget)
	{
		OO(slot)._setWidget(pWidget);

		_updateCanvasSize();
		_childWindowCorrection();
		_updateScrollbars();

		_requestRender();
		_requestResize();
	}

	//____ _componentPos() ____________________________________________________

	CoordSPX ScrollPanel::_componentPos(const GeoComponent* pComponent) const
	{
		if (pComponent == &skin)
			return CoordSPX();
		else if (pComponent == &scrollbarX)
			return m_scrollbarXRegion.pos();
		else
			return m_scrollbarYRegion.pos();
	}

	//____ _componentSize() ___________________________________________________

	SizeSPX ScrollPanel::_componentSize(const GeoComponent* pComponent) const
	{
		if (pComponent == &skin)
			return m_size;
		else if (pComponent == &scrollbarX)
			return m_scrollbarXRegion.size();
		else
			return m_scrollbarYRegion.size();

	}

	//____ _componentGeo() ____________________________________________________

	RectSPX ScrollPanel::_componentGeo(const GeoComponent* pComponent) const
	{
		if (pComponent == &skin)
			return m_size;
		else if (pComponent == &scrollbarX)
			return m_scrollbarXRegion;
		else
			return m_scrollbarYRegion;
	}

	//____ _scrollbarStep() ___________________________________________________

	void ScrollPanel::_scrollbarStep(const CScrollbar* pComponent, int dir)
	{
		if (pComponent == &scrollbarX)
		{
			spx movement = dir * align(ptsToSpx(m_stepSizeX, m_scale));
			m_childCanvas.x -= movement;
		}
		else
		{
			spx movement = dir * align(ptsToSpx(m_stepSizeY, m_scale));
			m_childCanvas.y -= movement;
		}

		_childWindowCorrection();
		_updateScrollbars();
		_requestRender();
	}

	//____ _scrollbarPage() ___________________________________________________

	void ScrollPanel::_scrollbarPage(const CScrollbar* pComponent, int dir)
	{
		if (pComponent == &scrollbarX)
		{
			spx movement = dir * (m_childWindow.w - align(ptsToSpx(m_pageOverlapX, m_scale)));
			m_childCanvas.x -= movement;
		}
		else
		{
			spx movement = dir * (m_childWindow.h - align(ptsToSpx(m_pageOverlapY, m_scale)));
			m_childCanvas.y -= movement;
		}

		_childWindowCorrection();
		_updateScrollbars();
		_requestRender();
	}
	
	//____ _scrollbarWheel() __________________________________________________

	void ScrollPanel::_scrollbarWheel(const CScrollbar* pComponent, int dir)
	{
		if (pComponent == &scrollbarX)
		{
			spx movement = dir * align(ptsToSpx(m_wheelStepSizeX, m_scale));
			m_childCanvas.x += movement;
		}
		else
		{
			spx movement = dir * align(ptsToSpx(m_wheelStepSizeY, m_scale));
			m_childCanvas.y += movement;
		}

		_childWindowCorrection();
		_updateScrollbars();
		_requestRender();
	}

	//____ _scrollbarMove() ___________________________________________________

	spx ScrollPanel::_scrollbarMove(const CScrollbar* pComponent, spx pos)
	{
		if (pComponent == &scrollbarX)
		{
			m_childCanvas.x = m_childWindow.x - pos;
		}
		else
		{
			m_childCanvas.y = m_childWindow.y - pos;
		}

		_childWindowCorrection();
		_updateScrollbars();
		_requestRender();


		return pos;
	}

	//____ _scrollbarOfsLenContent() __________________________________________

	std::tuple<spx, spx, spx> ScrollPanel::_scrollbarOfsLenContent(const CScrollbar* pComponent)
	{
		if (pComponent == &scrollbarX)
			return std::make_tuple(m_viewXOfs, m_viewXLen, m_canvasXLen);
		else
			return std::make_tuple(m_viewYOfs, m_viewYLen, m_canvasYLen);
	}

} // namespace wg
