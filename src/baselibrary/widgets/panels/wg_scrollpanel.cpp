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

#include <algorithm>

namespace wg
{
	using namespace Util;

	const TypeInfo ScrollPanel::TYPEINFO = { "ScrollPanel", &Panel::TYPEINFO };

	//____ Constructor _______________________________________________________

	ScrollPanel::ScrollPanel() : slot(this), scrollbarX(this, this, Axis::X), scrollbarY(this, this, Axis::Y)
	{

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

	void ScrollPanel::setAutohideScrollbars(bool scrollbarX, bool scrollbarY)
	{
		if (scrollbarX == m_bAutohideScrollbarX && scrollbarY == m_bAutohideScrollbarY)
			return;

		m_bAutohideScrollbarX = scrollbarX;
		m_bAutohideScrollbarY = scrollbarY;
		_updateRegions();
	}

	void ScrollPanel::setOverlayScrollbars(bool scrollbarX, bool scrollbarY)
	{
		if (scrollbarX == m_bOverlayScrollbarX && scrollbarY == m_bOverlayScrollbarY)
			return;

		m_bOverlayScrollbarX = scrollbarX;
		m_bOverlayScrollbarY = scrollbarY;
		_updateRegions();
	}

	void ScrollPanel::_updateRegions()
	{
		auto content = _contentRect();

		spx scrollbarYwidth = scrollbarY._preferredSize(m_scale).w;
		spx scrollbarXwidth = scrollbarX._preferredSize(m_scale).h;

		bool bShowScrollbarY = false;
		bool bShowScrollbarX = false;

		bool bCanHaveScrollbarX = scrollbarX.inWorkingOrder() && (m_widthConstraint == SizeConstraint::None || m_widthConstraint == SizeConstraint::GreaterOrEqual);
		bool bCanHaveScrollbarY = scrollbarY.inWorkingOrder() && (m_heightConstraint == SizeConstraint::None || m_heightConstraint == SizeConstraint::GreaterOrEqual);

		if (m_bAutohideScrollbarX == false  || (content.w < m_childCanvas.w && bCanHaveScrollbarX))
			bShowScrollbarX = true;




	}



	void ScrollPanel::_childWindowCorrection()
	{
		if (m_childWindow.w > m_childCanvas.w)
		{
			// Should repaint as needed...
		}
	}


	void ScrollPanel::_resize(const SizeSPX& size, int scale)
	{
		scale = _fixScale(scale);

		bool bSizeChanged = (size != m_size);
		bool bScaleChanged = (scale != m_scale);

		m_size = size;
		m_scale = scale;

		if( bSizeChanged )
			_updateRegions();



		if (bScaleChanged)
		{
			Widget* pChild = slot._widget();

			if (pChild)
			{
				auto childSize = pChild->_preferredSize(scale);
				m_childCanvas.setSize(childSize);
				pChild->_resize(childSize, scale);
				_childWindowCorrection();
			}
		}
	}

	void ScrollPanel::_receive(Msg* pMsg)
	{

	}

	void ScrollPanel::_render(GfxDevice* pDevice, const RectSPX& canvas, const RectSPX& window)
	{
		Widget::_render(pDevice, canvas, window);

		if (!slot.isEmpty())
			slot._widget()->_render(pDevice, m_childCanvas + canvas.pos(), m_childWindow + canvas.pos());

		if (!m_scrollbarXRegion.isEmpty())
			scrollbarX._render(pDevice, m_scrollbarXRegion + canvas.pos(), m_scale);

		if (!m_scrollbarYRegion.isEmpty())
			scrollbarY._render(pDevice, m_scrollbarYRegion + canvas.pos(), m_scale);
	}

	void ScrollPanel::_collectPatches(Patches& container, const RectSPX& geo, const RectSPX& clip)
	{
		if (!skin.isEmpty() && OO(skin)._isOpaque())
			container.add(RectSPX(geo, clip));
		else
		{
			if (slot._widget())
				OO(slot._widget())->_collectPatches(container, m_childCanvas+geo.pos(), RectSPX(clip,m_childWindow+geo.pos()));
			if( !m_scrollbarXRegion.isEmpty() )
				container.add(RectSPX(m_scrollbarXRegion + geo.pos(), clip));
			if (!m_scrollbarYRegion.isEmpty())
				container.add(RectSPX(m_scrollbarYRegion + geo.pos(), clip));
		}
	}

	void ScrollPanel::_maskPatches(Patches& patches, const RectSPX& geo, const RectSPX& clip, BlendMode blendMode)
	{

	}

	bool ScrollPanel::_alphaTest(const CoordSPX& ofs)
	{

	}
	
	void ScrollPanel::_cloneContent(const Widget* pOrg)
	{

	}

	const TypeInfo& ScrollPanel::_slotTypeInfo(const StaticSlot* pSlot) const
	{
		return DynamicSlot::TYPEINFO;
	}

	Widget* ScrollPanel::_findWidget(const CoordSPX& pos, SearchMode mode)
	{

	}

	Widget* ScrollPanel::_firstChild() const
	{
		return slot._widget();
	}

	Widget* ScrollPanel::_lastChild() const
	{
		return slot._widget();
	}

	void ScrollPanel::_firstSlotWithGeo(SlotWithGeo& package) const
	{
		package.pSlot = &slot;
	}

	void ScrollPanel::_nextSlotWithGeo(SlotWithGeo& package) const
	{
		package.pSlot = nullptr;
	}

	CoordSPX ScrollPanel::_childPos(const StaticSlot* pSlot) const
	{
		return { m_childCanvas.x, m_childCanvas.y };
	}

	RectSPX ScrollPanel::_childWindowSection(const StaticSlot* pSlot) const
	{
		return m_childWindow - m_childCanvas.pos();
	}

	void ScrollPanel::_childRequestRender(StaticSlot* pSlot)
	{
		_requestRender(m_viewRegion);
	}

	void ScrollPanel::_childRequestRender(StaticSlot* pSlot, const RectSPX& rect)
	{
		RectSPX visible(rect + m_childCanvas.pos(), m_viewRegion);
		if (!visible.isEmpty())
			_requestRender(visible);
	}

	void ScrollPanel::_childRequestResize(StaticSlot* pSlot)
	{
		_requestResize();
	}

	void ScrollPanel::_childRequestInView(StaticSlot* pSlot)
	{

	}

	void ScrollPanel::_childRequestInView(StaticSlot* pSlot, const RectSPX& mustHaveArea, const RectSPX& niceToHaveArea)
	{

	}

	Widget* ScrollPanel::_prevChild(const StaticSlot* pSlot) const
	{
		return nullptr;
	}

	Widget* ScrollPanel::_nextChild(const StaticSlot* pSlot) const
	{
		return nullptr;
	}

	void ScrollPanel::_releaseChild(StaticSlot* pSlot)
	{

	}

	void ScrollPanel::_replaceChild(StaticSlot* pSlot, Widget* pWidget)
	{

	}

	void ScrollPanel::_scrollbarStep(const CScrollbar* pComponent, int dir)
	{

	}

	void ScrollPanel::_scrollbarPage(const CScrollbar* pComponent, int dir)
	{

	}
	
	void ScrollPanel::_scrollbarWheel(const CScrollbar* pComponent, int dir)
	{

	}

	spx ScrollPanel::_scrollbarMove(const CScrollbar* pComponent, spx pos)
	{

	}

	std::tuple<spx, spx, spx> ScrollPanel::_scrollbarOfsLenContent(const CScrollbar* pComponent)
	{

	}

} // namespace wg
