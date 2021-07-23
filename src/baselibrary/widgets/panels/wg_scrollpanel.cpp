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
/*
	TODO: Do not draw skin behind content if content is opaque.
	TODO: Minimize amount and size of _requestRender/redraws in general.
	TODO: Common flow through _updateRegions(), _updateCanvasSize() etc can probably be optimized, i.e. we are calling preferredSize on dragbars twice.
*/


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
	/**
		@brief Set placement of child smaller than view.

		Sets the placement of a child that is smaller than the view of the ScrollPanel.
		
		Default is NorthWest.
	*/

	void ScrollPanel::setPlacement(Placement placement)
	{
		if (placement == m_smallChildPlacement)
			return;

		if (m_childCanvas.w < m_viewRegion.w || m_childCanvas.h < m_viewRegion.h)
		{
			_childWindowCorrection();
		}

	}

	//____ setSizeConstraints() _______________________________________________
	/**
		@brief Set constraints on child widgets size, relative view.

		Constrains the width and/or height of the child widget to never be larger or
		smaller (or both) than that of the view.

		@param width	Constraint rule to apply to width of child.
		@param height	Constraint rule to applyu to height of child.

		@return TRUE if set, FALSE if constraints would break rule below.

		Only one of the two parameters can be set to any of the SizeConstraint rules
		(None, Equal, LessOrEqual or GreaterOrEqual), the other parameter must be set
		to either None or GreaterOrEqual, e.g. only the width or height can be restricted
		to the view, not both.

		This is how the different values set to the width parameter would affect the childs width:

		- None: Childs width is not restricted in any way.
		- Equal: Childs width is set to exactly that of the view.
		- LessOrEqual: Childs width is allowed to be smaller but not higher than that of the view.
		- GreaterOrEqual: Childs width is at least that of the view.

	*/

	bool ScrollPanel::setSizeConstraints(SizeConstraint width, SizeConstraint height)
	{
		if ((width == SizeConstraint::Equal || width == SizeConstraint::LessOrEqual)
			&& (height == SizeConstraint::Equal || height == SizeConstraint::LessOrEqual))
		{
			Base::handleError(ErrorSeverity::SilentFail, ErrorCode::InvalidParam, "Only one dimension can be constrained to Equal/LessOrEqual", this, TYPEINFO, __func__, __FILE__, __LINE__);
			return false;
		}

		m_widthConstraint = width;
		m_heightConstraint = height;
		true;
	}

	//____ setAutohideScrollbars() ____________________________________________
	/**
		@brief Set scrollbars to autohide when content fits without scrolling.

		Setting a scrollbar to autohide will make it disappear when content fits without
		scrolling. If scrollbar is not set to autohide, it will change to state disabled,
		but not disappear.

		@param scrollbarX	Set to TRUE to make horizontal scrollbar disappear when not needed.
		@param scrollbarY	Set to TRUE to make vertical scrollbar disappear when not needed.

	*/
	void ScrollPanel::setAutohideScrollbars(bool scrollbarX, bool scrollbarY)
	{
		if (scrollbarX == m_bAutohideScrollbarX && scrollbarY == m_bAutohideScrollbarY)
			return;

		m_bAutohideScrollbarX = scrollbarX;
		m_bAutohideScrollbarY = scrollbarY;
		_updateCanvasSize();
		_updateRegions();
		_requestRender();
	}

	//____ setOverlayScrollbars() _____________________________________________
	/**
		@brief Set scrollbars to appear inside view instead of outside.

		Setting a scrollbar to overlay-mode will make it appear inside the content
		view instead of next to it. Thus the content view will not shrink/grow when the
		scrollbar appears/disappears, instead the scrollbar will cover part of the content.

		@param scrollbarX	Set to TRUE to make horizontal scrollbar appear inside content view.
		@param scrollbarY	Set to TRUE to make vertical scrollbar appear inside content view.

	*/
	void ScrollPanel::setOverlayScrollbars(bool scrollbarX, bool scrollbarY)
	{
		if (scrollbarX == m_bOverlayScrollbarX && scrollbarY == m_bOverlayScrollbarY)
			return;

		m_bOverlayScrollbarX = scrollbarX;
		m_bOverlayScrollbarY = scrollbarY;
		_updateCanvasSize();
		_updateRegions();

	}

	//____ setStepSize() ______________________________________________________
	/**
	*	@brief Set how much view will scroll when scrollbar button is pressed.
	* 
	*	StepSize determines how much the view content will scroll when a scrollbar
	*	button is pressed.
	* 
	*	@param stepSize		Number of points to scroll the content when scrollbar button is pressed.
	* 
	*	This method sets the same stepSize for horizontal and vertical movement.
	* 
	*	Default value is 8.
	*/

	void ScrollPanel::setStepSize(pts stepSize)
	{
		m_stepSizeX = stepSize;
		m_stepSizeY = stepSize;
	}

	/**
	*	@brief Set how much view will scroll when scrollbar button is pressed.
	*
	*	StepSize determines how much the view content will scroll when a scrollbar
	*	button is pressed.
	*
	*	@param x		Number of points to scroll the content horizontally when scrollbar button is pressed.
	*	@param y		Number of points to scroll the content vertically when scrollbar button is pressed.
	*
	*	Default value is 8 for both.
	*/

	void ScrollPanel::setStepSize(pts x, pts y)
	{
		m_stepSizeX = x;
		m_stepSizeY = y;
	}

	//____ setWheelStepSize() _________________________________________________
	/**
	*	@brief Set how much view will scroll when mouse wheel is rolled.
	*
	*	WheelStepSize determines how much the view content will scroll for each 'click' the
	*	mouse wheel is rolled.
	*
	*	@param stepSize		Number of points to scroll the content when mouse wheel is rolled.
	*
	*	This method sets the same wheelStepSize for horizontal and vertical scrolling.
	*
	*	Default value is 24 (three times default value of button stepSize).
	*/

	void ScrollPanel::setWheelStepSize(pts stepSize)
	{
		m_wheelStepSizeX = stepSize;
		m_wheelStepSizeY = stepSize;
	}

	/**
	*	@brief Set how much view will scroll when mouse wheel is rolled.
	*
	*	WheelStepSize determines how much the view content will scroll for each 'click' the
	*	mouse wheel is rolled. This can be controlled individually for horizontal and vertical
	*   scrolling.
	*
	*	@param x		Number of points to scroll the content horizontally when mouse wheel is rolled.
	*	@param y		Number of points to scroll the content vertically when mouse wheel is rolled.
	*
	*	Default value is 24 for both (three times default value of button stepSize).
	* 
	*	Use setScrollWheels() to determine what wheel scrolls in what direction. 
	*	Also see setScrollWheelAxisShift().
	*/

	void ScrollPanel::setWheelStepSize(pts x, pts y)
	{
		m_wheelStepSizeX = x;
		m_wheelStepSizeY = y;
	}

	//____ setPageOverlap() ___________________________________________________

	/**
	*	@brief Set how much of previous content should remain visible when jumping pages.
	* 
	*	A scrollbar can be made to jump a 'page' when pressing above or below the bar itself.
	*
	*	When jumping a page it is often desirable to keep a small section of the previous content
	*	visible, like the last line of displayed text. PageOverlap controls the amount of content
	*	that should be kept visible.
	* 
	*	@param overlap	Set the overlapping of pages in points, for both horizontal and vertical page jumps.
	* 
	*	Default value is 8.
	* 
	*/

	void ScrollPanel::setPageOverlap(pts overlap)
	{
		m_pageOverlapX = overlap;
		m_pageOverlapY = overlap;
	}

	/**
	*	@brief Set how much of previous content should remain visible when jumping pages.
	*
	*	A scrollbar can be made to jump a 'page' when pressing above or below the bar itself.
	*
	*	When jumping a page it is often desirable to keep a small section of the previous content
	*	visible, like the last line of displayed text. PageOverlap controls the amount of content
	*	that should be kept visible.
	*
	*	@param x	Set the overlapping of pages in points for horizontal page jumps.
	*	@param y	Set the overlapping of pages in points for vertical page jumps.
	*
	*	Default value is 8 for both.
	*
	*/

	void ScrollPanel::setPageOverlap(pts x, pts y)
	{
		m_pageOverlapX = x;
		m_pageOverlapY = y;
	}

	//____ setScrollWheels() __________________________________________________
	/**
	*	@brief Set which scroll wheels controls horizontal and vertical scrolling respectively.
	* 
	*	@param wheelForX	Scroll wheel to control horizontal scrolling.
	*	@param wheelForY	Scroll wheel to control vertical scrolling.
	*
	*	The scroll wheels are enumerated starting with 1, which then refers to the only scroll wheel
	*	of a normal mouse and 2 referring to any second scroll wheel on the mouse etc.
	* 
	*	Any parameter can also be set to 0 to disable scrolling with scroll wheel in that direction.
	*	
	*	Default values are 2 for X and 1 for Y, making the first scroll wheel control vertical
	*	scrolling and any second scroll wheel control horizontal scrolling.
	* 
	*	Note that setScrollWheelAxisShift() can be used to set a modifier key to invert what wheel
	*	is used for what axis, thus enable scrolling in both directions using only one mouse wheel.
	* 
	*	Also note that this setting doesn't affect how the scrollbars (if any) reacts to scroll wheels.
	*	Hovering over a scrollbar and rolling wheel 1 will move the scrollbar forward/backward independent
	*	of the scrollbars orientation. To disable this behavior you can call setStealWheelFromScrollbars().
	* 
	*/
	void ScrollPanel::setScrollWheels(int wheelForX, int wheelForY)
	{
		m_wheelForScrollX = wheelForX;
		m_wheelForScrollY = wheelForY;
	}

	//____ setScrollWheelAxisShift() __________________________________________
	/**
	*	@brief Set modifier keys used to shift axis scrolled by mouse wheel.
	* 
	*	Set a (combination of) modifier key(s) that, when pressed, swaps axis of
	*	a scroll wheel. Making it scroll the view content horizontally instead of
	*	vertically or vice versa.
	* 
	*	@param axisShift	Modifier keys to shift axis of mouse wheel.
	* 
	*	Default axisShift modifier key is ALT. To disable this feature, set axisShift to MODKEY_NONE.
	*/
	void ScrollPanel::setScrollWheelAxisShift(ModifierKeys axisShift)
	{
		m_wheelAxisShiftCombo = axisShift;
	}

	//____ setStealWheelFromScrollbars() ________________________________________
	/**
	*	@brief Set scroll wheel to follow ScrollPanels settings when hovering a scrollbar.
	* 
	*	The scrollbars have their own way of handling the scroll wheel. Hovering over a scrollbar
	*	and rolling the mouse wheel will make the scrollbar move forward or backward independently
	*	of if it is a horizontal or vertical scrollbar.
	* 
	*	In some cases that is not what you need, instead you want the scroll wheel to behave the same
	*	way no matter if it is hovering a scrollbar or not. This method lets you take control over that.
	* 
	*	@param bSteal	Set to TRUE to override scrollbars scroll wheel behavior with that of ScrollPanel.
	* 
	*/

	void ScrollPanel::setStealWheelFromScrollbars(bool bSteal)
	{
		m_bStealWheelFromScrollbars = bSteal;
	}

	//____ setAutoscroll() ____________________________________________________
	/**
	*	@brief Set view to scroll when at end of content and new content is added.
	* 
	*	When Autoscroll is enabled, the view will automatically scroll to the end of
	*	its content when new content is added, if it already was at the end.
	* 
	*	Example: If you have a ScrollPanel showing a TextDisplay to which new lines are
	*	added continually (like a logfile), you might want the ScrollPanel to automatically
	*	scroll down when new content is added if it already is showing the last line. Enabling
	*   Autoscroll for the vertical dimension will make this happen.
	* 
	*	@param autoscrollX	Set to TRUE to enable horizontal autoscroll.
	*	@param autoscrollY	Set to TRUE to enable vertical autoscroll.
	* 	* 
	*	Autoscroll is by default disabled for both dimensions.
	*/

	void ScrollPanel::setAutoscroll(bool autoscrollX, bool autoscrollY)
	{
		m_bAutoscrollX = autoscrollX;
		m_bAutoscrollY = autoscrollY;
	}



	//____ _updateRegions() ___________________________________________________

	void ScrollPanel::_updateRegions()
	{
		auto content = _contentRect();

		spx scrollbarYwidth = scrollbarY._preferredSize(m_scale).w;
		spx scrollbarXheight = scrollbarX._preferredSize(m_scale).h;

		bool bShowScrollbarY = scrollbarX.inWorkingOrder() && (!m_bAutohideScrollbarY || content.h < m_childCanvas.h);
		bool bShowScrollbarX = scrollbarY.inWorkingOrder() && (!m_bAutohideScrollbarX || content.w < m_childCanvas.w);


		if (bShowScrollbarY && !m_bOverlayScrollbarY && !bShowScrollbarX)
			bShowScrollbarX = (content.w - scrollbarYwidth < m_childCanvas.w);

		if (bShowScrollbarX && !m_bOverlayScrollbarX && !bShowScrollbarY)
			bShowScrollbarY = (content.h - scrollbarXheight < m_childCanvas.h);


		if (bShowScrollbarY)
			m_scrollbarYRegion = {	content.x + content.w - scrollbarYwidth, content.y, 
									scrollbarYwidth, bShowScrollbarX ? content.h - scrollbarXheight : content.h };
		else
			m_scrollbarYRegion.clear();

		if (bShowScrollbarX)
			m_scrollbarXRegion = {	content.x, content.y + content.h - scrollbarXheight, 
									bShowScrollbarY ? content.w - scrollbarYwidth : content.w, scrollbarXheight };
		else
			m_scrollbarXRegion.clear();


		if (bShowScrollbarY && !m_bOverlayScrollbarY)
			content.w -= scrollbarYwidth;

		if (bShowScrollbarX && !m_bOverlayScrollbarX)
			content.h -= scrollbarXheight;

		m_viewRegion = content;

		_requestRender();
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

			if ( m_childWindow.x + m_childWindow.w > m_childCanvas.x + m_childCanvas.w)
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
			// Calc viewMaxSize & viewMinSize

			SizeSPX viewMaxSize = _contentRect().size();
			SizeSPX viewMinSize;

			if (scrollbarX.inWorkingOrder() && !m_bOverlayScrollbarX)
			{
				spx scrollbarXheight = scrollbarX._preferredSize(m_scale).h;
				if (m_bAutohideScrollbarX)
					viewMinSize.h = viewMaxSize.h - scrollbarXheight;
				else
				{
					viewMaxSize.h = viewMaxSize.h - scrollbarXheight;
					viewMinSize.h = viewMaxSize.h;
				}
			}

			if (scrollbarY.inWorkingOrder() && !m_bOverlayScrollbarY)
			{
				spx scrollbarYwidth = scrollbarY._preferredSize(m_scale).w;
				if (m_bAutohideScrollbarY)
					viewMinSize.w = viewMaxSize.w - scrollbarYwidth;
				else
				{
					viewMaxSize.w= viewMaxSize.w - scrollbarYwidth;
					viewMinSize.w = viewMaxSize.w;
				}
			}

			//

			auto size = _calcCanvasSize(viewMinSize, viewMaxSize);

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

		// Give our scrollbars the opportunity to process

		if (!(m_bStealWheelFromScrollbars && pMsg->type() == MsgType::WheelRoll))
		{
			if (scrollbarX.inWorkingOrder())
				bX = scrollbarX._receive(pMsg);

			if (scrollbarY.inWorkingOrder())
				bY = scrollbarY._receive(pMsg);
		}

		// Leave if processed by our scrollbars.

		if (bX || bY)
			return;

		// Process any messages not handled by our scrollbars

		switch (pMsg->type())
		{
			case MsgType::WheelRoll:
			{
				auto p = static_cast<WheelRollMsg*>(pMsg);

				int wheelX = m_wheelForScrollX;
				int wheelY = m_wheelForScrollY;

				if (m_wheelAxisShiftCombo == p->modKeys() && m_wheelAxisShiftCombo != ModifierKeys::MODKEY_NONE)
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
		_updateRegions();
		_childWindowCorrection();

		// Handle autoscrolling, taking advantage of the fact that childCanvas & childWindow has been
		// updated but not yet m_viewXOfs, m_viewXLen and m_canvasXLen.

		if (m_bAutoscrollX && m_childWindow.w < m_childCanvas.w && m_viewXOfs + m_viewXLen == m_canvasXLen)
			m_childCanvas.x = m_childWindow.x + m_childWindow.w - m_childCanvas.w;

		if (m_bAutoscrollY && m_childWindow.h < m_childCanvas.h && m_viewYOfs + m_viewYLen == m_canvasYLen)
			m_childCanvas.y = m_childWindow.y + m_childWindow.h - m_childCanvas.h;


		_updateScrollbars();
		_requestResize();
	}

	//____ _childRequestInView() ______________________________________________

	void ScrollPanel::_childRequestInView(StaticSlot* pSlot)
	{
		// Our only child is always in view as much as possible, so just pass it on like any
		// normal container.

		_requestInView( m_childWindow, m_childWindow);
	}

	void ScrollPanel::_childRequestInView(StaticSlot* pSlot, const RectSPX& mustHaveArea, const RectSPX& niceToHaveArea)
	{
		if (!m_childWindow.contains(niceToHaveArea + m_childCanvas.pos()))
		{
/*
			RectSPX window = { m_viewPixOfs, m_elements[WINDOW].m_windowGeo.size() };

			for (int i = 0; i < 2; i++)
			{
				WgRect inside = i == 0 ? niceToHaveArea : mustHaveArea;

				int diffLeft = inside.x - window.x;
				int diffRight = inside.right() - window.right();
				int diffTop = inside.y - window.y;
				int diffBottom = inside.bottom() - window.bottom();

				if (diffLeft > 0 && diffRight > 0)
					window.x += std::min(diffLeft, diffRight);
				else if (diffLeft < 0 && diffRight < 0)
					window.x += std::max(diffLeft, diffRight);

				if (diffTop > 0 && diffBottom > 0)
					window.y += std::min(diffTop, diffBottom);
				else if (diffTop < 0 && diffBottom < 0)
					window.y += std::max(diffTop, diffBottom);
			}

			if (window.pos() != m_viewPixOfs)
				SetViewPixelOfs(window.x, window.y);

			// Forward to any outer ScrollPanel

			WgRect newMustHaveArea(mustHaveArea - m_viewPixOfs + m_elements[WINDOW].m_windowGeo.pos(), m_elements[WINDOW].m_windowGeo);
			WgRect newNiceToHaveArea(niceToHaveArea - m_viewPixOfs + m_elements[WINDOW].m_windowGeo.pos(), m_elements[WINDOW].m_windowGeo);

			_requestInView(newMustHaveArea, newNiceToHaveArea);

*/
		}
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

	//____ _calcCanvasSize() __________________________________________________

	SizeSPX ScrollPanel::_calcCanvasSize(SizeSPX viewMinSize, SizeSPX viewMaxSize)
	{
		auto pWidget = OO(slot)._widget();
		if (!pWidget)
			return SizeSPX();

		auto prefSize = pWidget->_preferredSize(m_scale);
		auto size = prefSize;

		auto viewSize = viewMaxSize;

		Axis	mainAxis = Axis::X;

		if (m_widthConstraint == SizeConstraint::Equal || m_widthConstraint == SizeConstraint::LessOrEqual)
			mainAxis = Axis::Y;

		if (mainAxis == Axis::Y)
		{
			// Calc canvas width/height from widthConstraint and a view with no scrollbars.

			switch (m_widthConstraint)
			{
			case SizeConstraint::None:
				break;
			case SizeConstraint::Equal:
				size.w = viewSize.w;
				size.h = pWidget->_matchingHeight(size.w, m_scale);
				break;
			case SizeConstraint::LessOrEqual:
				if (size.w > viewSize.w)
				{
					size.w = viewSize.w;
					size.h = pWidget->_matchingHeight(size.w, m_scale);
				}
				break;
			case SizeConstraint::GreaterOrEqual:
				if (size.w < viewSize.w)
				{
					size.w = viewSize.w;
					size.h = pWidget->_matchingHeight(size.w, m_scale);
				}
				break;
			}

			if (size.w > viewSize.w)
				viewSize.h = viewMinSize.h;			// Horizontal scrollbar introduced.

			if (size.h > viewSize.h && viewMinSize.w != viewMaxSize.w )
			{
				viewSize.w = viewMinSize.w;			// Vertical scrollbar introduced.

				// Recalc size from constraints with new smaller width.

				switch (m_widthConstraint)
				{
					case SizeConstraint::None:
						break;
					case SizeConstraint::Equal:
						size.w = viewSize.w;
						size.h = pWidget->_matchingHeight(size.w, m_scale);
						break;
					case SizeConstraint::LessOrEqual:
						if (prefSize.w > viewSize.w)
						{
							size.w = viewSize.w;
							size.h = pWidget->_matchingHeight(size.w, m_scale);
						}
						break;
					case SizeConstraint::GreaterOrEqual:
						if (prefSize.w < viewSize.w)
						{
							size.w = viewSize.w;
							size.h = pWidget->_matchingHeight(size.w, m_scale);
						}
						break;
				}
			}

			if (m_heightConstraint == SizeConstraint::GreaterOrEqual && size.h < viewSize.h)
				size.h = viewSize.h;
		}
		else // (mainAxis == Axis::X)
		{
			// Calc canvas width/height from widthConstraint and a view with no scrollbars.

			switch (m_heightConstraint)
			{
			case SizeConstraint::None:
				break;
			case SizeConstraint::Equal:
				size.h = viewSize.h;
				size.w = pWidget->_matchingWidth(size.h, m_scale);
				break;
			case SizeConstraint::LessOrEqual:
				if (size.h > viewSize.h)
				{
					size.h = viewSize.h;
					size.w = pWidget->_matchingWidth(size.h, m_scale);
				}
				break;
			case SizeConstraint::GreaterOrEqual:
				if (size.h < viewSize.h)
				{
					size.h = viewSize.h;
					size.w = pWidget->_matchingWidth(size.h, m_scale);
				}
				break;
			}

			if (size.h > viewSize.h)
				viewSize.w = viewMinSize.w;			// Vertical scrollbar introduced.

			if (size.w > viewSize.w && viewMinSize.h != viewMaxSize.h)
			{
				viewSize.h = viewMinSize.h;			// Horizontal scrollbar introduced.

				// Recalc size from constraints with new smaller width.

				switch (m_heightConstraint)
				{
				case SizeConstraint::None:
					break;
				case SizeConstraint::Equal:
					size.h = viewSize.h;
					size.w = pWidget->_matchingWidth(size.h, m_scale);
					break;
				case SizeConstraint::LessOrEqual:
					if (prefSize.h > viewSize.h)
					{
						size.h = viewSize.h;
						size.w = pWidget->_matchingWidth(size.h, m_scale);
					}
					break;
				case SizeConstraint::GreaterOrEqual:
					if (prefSize.h < viewSize.h)
					{
						size.h = viewSize.h;
						size.w = pWidget->_matchingWidth(size.h, m_scale);
					}
					break;
				}
			}

			if (m_widthConstraint == SizeConstraint::GreaterOrEqual && size.w < viewSize.w)
				size.w = viewSize.w;
		}


		return size;
	}



} // namespace wg
