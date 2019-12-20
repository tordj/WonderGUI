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

	const char ScrollPanel::CLASSNAME[] = {"ScrollPanel"};


	float ViewSlot::_windowFractionX() const
	{
		if (m_contentSize.w == 0)
			return 1.f;
		else
		{
			float w = m_windowGeo.w / (float)m_contentSize.w;

			if (w > 1.f)
				w = 1.f;

			return w;
		}
	}

	float ViewSlot::_windowFractionY() const
	{
		if (m_contentSize.h == 0)
			return 1.f;
		else
		{
			float h = m_windowGeo.h / (float)m_contentSize.h;

			if (h > 1.f)
				h = 1.f;

			return h;
		}
	}

	float ViewSlot::_windowOffsetX() const
	{
		int size = m_contentSize.w - m_windowGeo.w;
		if (size > 0)
			return m_viewPixOfs.x / (float)size;

		return 0;
	}

	float ViewSlot::_windowOffsetY() const
	{
		int size = m_contentSize.h - m_windowGeo.h;
		if (size > 0)
			return m_viewPixOfs.y / (float)size;

		return 0;
	}



	//____ updateCanvasGeo() __________________________________________________

	bool ViewSlot::_updateCanvasGeo()
	{
		RectI	out(m_windowGeo.pos() - m_viewPixOfs, m_contentSize);

		if (m_windowGeo.w > m_contentSize.w)
		{
			RectI r = Util::origoToRect(m_contentOrigo, SizeI(m_windowGeo.w, 1), SizeI(m_contentSize.w, 1));
			out.x = m_windowGeo.x + r.x;
		}

		if (m_windowGeo.h > m_contentSize.h)
		{
			RectI r = Util::origoToRect(m_contentOrigo, SizeI(1, m_windowGeo.h), SizeI(1, m_contentSize.h));
			out.y = m_windowGeo.y + r.y;
		}

		out.shrink(padding);

		if (out != m_canvasGeo)
		{
			m_canvasGeo = out;
			return true;
		}
		return false;
	}

	//____ sizeFromPolicy() ________________________________________________________

	SizeI ViewSlot::_sizeFromPolicy(SizeI specifiedSize) const
	{
		SizeI	defaultSize = _paddedPreferredSize();

		SizeI	sz;

		switch (m_widthPolicy)
		{
		case SizePolicy::Default:
		{
			sz.h = Util::sizeFromPolicy(defaultSize.h, specifiedSize.h, m_heightPolicy);
			sz.w = _paddedMatchingWidth(sz.h);
			break;
		case SizePolicy::Bound:
			sz.w = specifiedSize.w;
			sz.h = Util::sizeFromPolicy(_paddedMatchingHeight(sz.w), specifiedSize.h, m_heightPolicy);
			break;
		case SizePolicy::Confined:
			if (defaultSize.w > specifiedSize.w)
			{
				sz.w = specifiedSize.w;
				sz.h = Util::sizeFromPolicy(_paddedMatchingHeight(sz.w), specifiedSize.h, m_heightPolicy);
			}
			else
			{
				sz.h = Util::sizeFromPolicy(defaultSize.h, specifiedSize.h, m_heightPolicy);
				sz.w = _paddedMatchingWidth(sz.h);
				if (sz.w > specifiedSize.w)
					sz.w = specifiedSize.w;
			}
			break;
		case SizePolicy::Expanded:
			if (defaultSize.w < specifiedSize.w)
			{
				sz.w = specifiedSize.w;
				sz.h = Util::sizeFromPolicy(_paddedMatchingHeight(sz.w), specifiedSize.h, m_heightPolicy);
			}
			else
			{
				sz.h = Util::sizeFromPolicy(defaultSize.h, specifiedSize.h, m_heightPolicy);
				sz.w = _paddedMatchingWidth(sz.h);
				if (sz.w < specifiedSize.w)
					sz.w = specifiedSize.w;
			}
			break;
		}
		}

		return sz;
	}

	//____ setWindowPos() ______________________________________________________

	bool ViewSlot::_setWindowPos(CoordI pos)
	{
		if (pos == m_viewPixOfs)
			return true;

		bool retVal = true;
 		if (pos.y < 0)
		{
			pos.y = 0;
			retVal = false;
		}
		if (pos.y > m_contentSize.h - m_windowGeo.h)
		{
			pos.y = std::max(0,m_contentSize.h - m_windowGeo.h);
			retVal = false;
		}

		if (pos.x < 0)
		{
			pos.x = 0;
			retVal = false;
		}
		if (pos.x > m_contentSize.w - m_windowGeo.w)
		{
			pos.x = std::max(0,m_contentSize.w - m_windowGeo.w);
			retVal = false;
		}

		if (m_viewPixOfs != pos)
		{
			m_viewPixOfs = pos;
			_updateCanvasGeo();
		}

		return retVal;
	}


	//____ setWindowOffset() ___________________________________________________________

	bool ViewSlot::_setWindowOffset( CoordF ofs)
	{
		limit(ofs.x, 0.f, 1.f);
		limit(ofs.y, 0.f, 1.f);

		int	width = max(0, m_contentSize.w - m_windowGeo.w);
		int	height = max(0, m_contentSize.h - m_windowGeo.h);

		return _setWindowPos({ (int)((width*ofs.x) + 0.5f), (int)((height*ofs.y) + 0.5f) });
	}

	//____ setWindowOffsetX() ___________________________________________________________

	bool ViewSlot::_setWindowOffsetX(float ofs)
	{
		limit(ofs, 0.f, 1.f);

		int	width = max(0, m_contentSize.w - m_windowGeo.w);
		return _setWindowPos({ (int)((width*ofs) + 0.5f), m_viewPixOfs.y });
	}

	//____ setWindowOffsetY() ___________________________________________________________

	bool ViewSlot::_setWindowOffsetY(float ofs)
	{
		limit(ofs, 0.f, 1.f);

		int	height = max(0, m_contentSize.h - m_windowGeo.h);

		return _setWindowPos({ m_viewPixOfs.y, (int)((height*ofs) + 0.5f) });
	}

	//____ paddedWindowPixelLenX() _______________________________________________

	int ViewSlot::_paddedWindowPixelLenX()
	{
		SizeI	windowPadding = m_pWidget ? m_pWidget->_windowPadding() : SizeI(0, 0);
		return	m_windowGeo.w - windowPadding.w;
	}

	//____ paddedWindowPixelLenY() ________________________________________________________

	int ViewSlot::_paddedWindowPixelLenY()
	{
		SizeI	windowPadding = m_pWidget ? m_pWidget->_windowPadding() : SizeI(0, 0);
		return m_windowGeo.h - windowPadding.h;
	}

	//____ paddedWindowLenX() _____________________________________________________________

	float ViewSlot::_paddedWindowLenX()
	{
		SizeI	windowPadding = m_pWidget ? m_pWidget->_windowPadding() : SizeI(0, 0);

		if (m_contentSize.w - windowPadding.w <= 0)
			return 1.f;

		float len = (m_windowGeo.w - windowPadding.w) / (float)(m_contentSize.w - windowPadding.w);

		if (len > 1.f)
			len = 1.f;

		return len;
	}

	//____ paddedWindowLenY() _____________________________________________________________

	float ViewSlot::_paddedWindowLenY()
	{
		SizeI	windowPadding = m_pWidget ? m_pWidget->_windowPadding() : SizeI(0, 0);

		if (m_contentSize.h - windowPadding.h <= 0)
			return 1.f;

		float len = (m_windowGeo.h - windowPadding.h) / (float)(m_contentSize.h - windowPadding.h);

		if (len > 1.f)
			len = 1.f;

		return len;
	}

	CViewSlot& CViewSlot::operator=(Widget * pWidget)
	{
		if (pWidget)
			pWidget->releaseFromParent();
		_holder()->_replaceChild(this, pWidget);
		return *this;
	}

	void CViewSlot::setOrigo(Origo origo)
	{
		if (origo == m_contentOrigo)
			return;

		m_contentOrigo = origo;

		bool bModified = _updateCanvasGeo();
		if (bModified)
			_holder()->_requestRender(m_windowGeo);
	}

	void CViewSlot::setWidthPolicy(SizePolicy policy)
	{
		if (policy != m_widthPolicy)
		{
			m_widthPolicy = policy;
			_holder()->_updateViewGeo();
		}
	}

	void CViewSlot::setHeightPolicy(SizePolicy policy)
	{
		if (policy != m_heightPolicy)
		{
			m_heightPolicy = policy;
			_holder()->_updateViewGeo();
		}
	}

	Rect CViewSlot::windowRect() const
	{
		return Rect( rawToQpix(m_viewPixOfs), rawToQpix(m_windowGeo.size()));
	}

	Size CViewSlot::windowSize() const
	{
		return rawToQpix(m_windowGeo.size());
	}

	RectF CViewSlot::windowSection() const
	{
		return RectF(_windowOffsetX(), _windowOffsetY(), _windowFractionX(), _windowFractionY());
	}

	CoordF CViewSlot::windowOffset() const
	{
		return CoordF(_windowOffsetX(), _windowOffsetY() );
	}

	SizeF CViewSlot::windowFraction() const
	{
		return SizeF(_windowFractionX(), _windowFractionY());
	}

	bool CViewSlot::setWindowPos(Coord pos)
	{
		return _holder()->_setWindowPos(qpixToRaw(pos));
	}

	bool CViewSlot::setWindowOffset(CoordF ofs)
	{
		return _holder()->_setWindowOffset(ofs);
	}

	bool CViewSlot::step(Direction dir)
	{
		return _holder()->_step(dir);
	}

	bool CViewSlot::jump(Direction dir)
	{
		return _holder()->_jump(dir);
	}

	bool CViewSlot::setWindowFocus(Origo canvasOrigo, Coord canvasOfs, Origo viewOrigo, Coord viewOfs)
	{
		//TODO: Implement!

		return false;
	}

	void CViewSlot::setScrollBorder(Border border)
	{
		m_scrollBorder = border;
	}

	void CViewSlot::setRubberBorder(Border border)
	{
		m_rubberBorder = border;
	}

	void CViewSlot::setDragButton(MouseButton button)
	{
		m_dragButton = button;
	}


	void ScrollbarEntry::setAutoHide(bool autohide)
	{
		if (autohide == m_pSlot->bAutoHide)
			return;

		m_pSlot->bAutoHide = autohide;
		m_pHolder->_updateElementGeo(m_pHolder->m_size);
	}

	bool ScrollbarEntry::setAutoScroll(bool autoscroll)
	{
		m_pSlot->bAutoScroll = autoscroll;
		return true;
	}

	bool ScrollbarEntry::setPlacement(Direction placement)
	{
		if ((placement == Direction::Up || placement == Direction::Down) && (m_pSlot->placement == Direction::Left || m_pSlot->placement == Direction::Right))
			return false;

		if ((placement == Direction::Left || placement == Direction::Right) && (m_pSlot->placement == Direction::Up || m_pSlot->placement == Direction::Down))
			return false;

		if (placement != m_pSlot->placement)
		{
			m_pSlot->placement = placement;
			m_pHolder->_updateElementGeo(m_pHolder->m_size);
		}
		return true;
	}

	ScrollbarEntry ScrollbarEntry::operator=(Scrollbar * pWidget)
	{
		if (pWidget)
			pWidget->releaseFromParent();
		m_pHolder->_replaceChild(m_pSlot, pWidget);
		return *this;
	}

	void ScrollbarEntry::clear()
	{
		m_pHolder->_replaceChild(m_pSlot, nullptr);
	}

	Object * ScrollbarEntry::_object()
	{
		return m_pHolder;
	}

	const Object * ScrollbarEntry::_object() const
	{
		return m_pHolder;
	}


	//____ Constructor ____________________________________________________________

	ScrollPanel::ScrollPanel() : viewSlot(this), vscrollbar(&m_scrollbarSlots[1], this), hscrollbar(&m_scrollbarSlots[0], this)
	{
		m_scrollbarSlots[0].m_pHolder = (ViewSlot::Holder*) this;
		m_scrollbarSlots[1].m_pHolder = (ViewSlot::Holder*) this;


		m_scrollbarSlots[0].placement = Direction::Down;
		m_scrollbarSlots[1].placement = Direction::Right;

		m_scrollbarTargets[0].m_bHorizontal = false;
		m_scrollbarTargets[0].m_pParent = this;

		m_scrollbarTargets[1].m_bHorizontal = true;
		m_scrollbarTargets[1].m_pParent = this;

		m_pStepFunction = nullptr;
		m_pJumpFunction = nullptr;
		m_pWheelRollFunction = nullptr;

		m_bOverlayScrollbars	= false;
		m_wheelForScroll	= 1;

		_updateElementGeo( SizeI(256,256) );
	}

	//____ ~ScrollPanel() __________________________________________________

	ScrollPanel::~ScrollPanel()
	{
	}

	//____ isInstanceOf() _________________________________________________________

	bool ScrollPanel::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return Panel::isInstanceOf(pClassName);
	}

	//____ className() ____________________________________________________________

	const char * ScrollPanel::className( void ) const
	{
		return CLASSNAME;
	}

	//____ cast() _________________________________________________________________

	ScrollPanel_p ScrollPanel::cast( Object * pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return ScrollPanel_p( static_cast<ScrollPanel*>(pObject) );

		return 0;
	}

	//____ setStepFunc() ____________________________________________________

	void ScrollPanel::setStepFunc(std::function<int(Direction, int steps)> function)
	{
		m_pStepFunction = function;
	}

	//____ setJumpFunc() ____________________________________________________

	void ScrollPanel::setJumpFunc(std::function<int(Direction, int steps)> function)
	{
		m_pJumpFunction = function;
	}

	//____ setWheelRollFunc() ____________________________________________________

	void ScrollPanel::setWheelRollFunc(std::function<int(Direction, int steps)> function)
	{
		m_pWheelRollFunction = function;
	}

	//____ _step() _______________________________________________________________

	bool ScrollPanel::_step(Direction dir, int nSteps)
	{
		int offset;
		if (m_pStepFunction)
			offset = m_pStepFunction(dir, nSteps);
		else
			offset = _defaultStepFunction(dir, 1);

		CoordI pos = viewSlot.m_viewPixOfs;

		switch (dir)
		{
		case Direction::Up:
			pos.y -= offset;
			break;
		case Direction::Down:
			pos.y += offset;
			break;
		case Direction::Left:
			pos.x -= offset;
			break;
		case Direction::Right:
			pos.x += offset;
			break;
		}
		return _setWindowPos(pos);
	}

	//____ _jump() _______________________________________________________________

	bool ScrollPanel::_jump(Direction dir, int nJumps)
	{
		int offset;
		if (m_pJumpFunction)
			offset = m_pJumpFunction(dir, 1);
		else
			offset = _defaultJumpFunction(dir, 1);

		CoordI pos = viewSlot.m_viewPixOfs;

		switch (dir)
		{
		case Direction::Up:
			pos.y -= offset;
			break;
		case Direction::Down:
			pos.y += offset;
			break;
		case Direction::Left:
			pos.x -= offset;
			break;
		case Direction::Right:
			pos.x += offset;
			break;
		}
		return _setWindowPos(pos);
	}

	//____ _wheelRoll() ___________________________________________________________

	bool ScrollPanel::_wheelRoll(Direction dir, int steps)
	{
		int offset;
		if (m_pWheelRollFunction)
		{
			offset = m_pWheelRollFunction(dir, 1);

			CoordI pos = viewSlot.m_viewPixOfs;

			switch (dir)
			{
			case Direction::Up:
				pos.y -= offset;
				break;
			case Direction::Down:
				pos.y += offset;
				break;
			case Direction::Left:
				pos.x -= offset;
				break;
			case Direction::Right:
				pos.x += offset;
				break;
			}
			return _setWindowPos(pos);
		}
		else
			return _step(dir,steps*3);
	}

	//____ _defaultStepFunction() ________________________________________________

	int ScrollPanel::_defaultStepFunction(Direction dir, int steps)
	{
		// Default is 10% of window size

		int windowLen;
		if( dir == Direction::Up || dir == Direction::Down )
			windowLen = viewSlot._paddedWindowPixelLenY();
		else
			windowLen = viewSlot._paddedWindowPixelLenX();

		return max(1, windowLen*steps / 10 );
	}

	//____ _defaultJumpFunction() ________________________________________________

	int ScrollPanel::_defaultJumpFunction(Direction dir, int steps)
	{
		// Default is window size minus one step.

		int windowLen;
		if (dir == Direction::Up || dir == Direction::Down)
			windowLen = viewSlot._paddedWindowPixelLenY();
		else
			windowLen = viewSlot._paddedWindowPixelLenX();

		if (m_pStepFunction)
			windowLen -= m_pStepFunction(dir, 1);
		else
			windowLen -= _defaultStepFunction(dir, 1);

		return windowLen*steps;
	}


	//____ setOverlayScrollbars() ____________________________________________________

	void ScrollPanel::setOverlayScrollbars( bool bOverlay )
	{
		if( bOverlay != m_bOverlayScrollbars )
		{
			m_bOverlayScrollbars = bOverlay;
			_updateElementGeo( m_size );
		}
	}

	//____ setSkin() ______________________________________________________________

	void ScrollPanel::setSkin( Skin * pSkin )
	{
		Panel::setSkin(pSkin);
		viewSlot._updateCanvasGeo();

		//TODO: Should force some kind of update here!
	}

	//____ setCornerSkin() ______________________________________________________

	void ScrollPanel::setCornerSkin( Skin * pSkin )
	{
		m_pCornerSkin = pSkin;
		_requestRender( m_cornerGeo );
	}

	//____ _setWindowPos() ____________________________________________________

	bool ScrollPanel::_setWindowPos(CoordI pos)
	{
		CoordI oldPos = viewSlot.m_viewPixOfs;

		bool retVal = viewSlot._setWindowPos(pos);
		if (viewSlot.m_viewPixOfs != oldPos)
		{
			if (viewSlot.m_viewPixOfs.x != oldPos.x)
				m_scrollbarTargets[1]._updateScrollbar(viewSlot._windowOffsetX(), viewSlot._paddedWindowLenX());

			if (viewSlot.m_viewPixOfs.y != oldPos.y)
				m_scrollbarTargets[0]._updateScrollbar(viewSlot._windowOffsetY(), viewSlot._paddedWindowLenY());

			_requestRender(viewSlot.m_windowGeo);
		}
		return retVal;
	}

	//____ _setWindowOffset() ____________________________________________________

	bool ScrollPanel::_setWindowOffset(CoordF ofs)
	{
		CoordI oldPos = viewSlot.m_viewPixOfs;

		bool retVal = viewSlot._setWindowOffset(ofs);
		if (viewSlot.m_viewPixOfs != oldPos)
		{
			if (viewSlot.m_viewPixOfs.x != oldPos.x)
				m_scrollbarTargets[1]._updateScrollbar(viewSlot._windowOffsetX(), viewSlot._paddedWindowLenX());

			if (viewSlot.m_viewPixOfs.y != oldPos.y)
				m_scrollbarTargets[0]._updateScrollbar(viewSlot._windowOffsetY(), viewSlot._paddedWindowLenY());

			_requestRender(viewSlot.m_windowGeo);
		}
		return retVal;
	}


	//____ _findWidget() ____________________________________________________________

	Widget * ScrollPanel::_findWidget( const CoordI& pos, SearchMode mode )
	{
		// Check scrollbars

		for (int i = 0; i < 2; i++)
		{
			ScrollbarSlot * p = &m_scrollbarSlots[i];
			if (p->bVisible && p->_widget() && p->geo.contains(pos))
			{
				if (mode != SearchMode::MarkPolicy || p->_markTest(pos - p->geo.pos()))
					return p->_widget();
			}
		}

		// Check view

		ViewSlot * p = &viewSlot;

		if( p->m_windowGeo.contains( pos ) )
		{
			if( p->bVisible && p->_widget() && p->m_windowGeo.contains( pos ) )
			{
				if( p->_widget()->isContainer() )
				{
					Widget * pFound = static_cast<Container*>(p->_widget())->_findWidget( pos - p->m_canvasGeo.pos(), mode );
					if( pFound )
						return pFound;
				}
				else if( mode != SearchMode::MarkPolicy || p->_markTest( pos - p->m_canvasGeo.pos() ) )
					return p->_widget();
			}

			// Check window skin

			if( m_pSkin )
			{
				if( m_pSkin->_markTest( pos - p->m_windowGeo.pos() + p->m_viewPixOfs, p->m_contentSize, m_state, m_markOpacity ) )
					return this;
			}
		}

		// Check our little corner square and geometry

		if( mode == SearchMode::Geometry || _markTest( pos ) )
			return this;

		//

		return nullptr;
	}

	//____ _preferredSize() ___________________________________________________________

	SizeI ScrollPanel::_preferredSize() const
	{
		SizeI sz;

		if( viewSlot._widget())
			sz = viewSlot._paddedPreferredSize();

		if( m_scrollbarSlots[0]._widget() && !m_scrollbarSlots[0].bAutoHide )
		{
			SizeI scrollbar = m_scrollbarSlots[0]._paddedPreferredSize();
			sz.h += scrollbar.h;
			if( scrollbar.w > sz.w )
				sz.w = scrollbar.w;
		}

		if( m_scrollbarSlots[1]._widget() && !m_scrollbarSlots[1].bAutoHide )
		{
			SizeI scrollbar = m_scrollbarSlots[1]._paddedPreferredSize();
			sz.w += scrollbar.w;
			if( scrollbar.h > sz.h )
				sz.h = scrollbar.h;
		}

		return sz;
	}


	//____ _calcContentSize() ______________________________________________________

	SizeI ScrollPanel::_calcContentSize( SizeI mySize )
	{
		Widget * pContent = viewSlot._widget();

		if( !pContent )
			return SizeI(0,0);

		//

		if( !m_scrollbarSlots[0].bAutoHide && m_scrollbarSlots[0]._widget())
			mySize.h -= m_scrollbarSlots[0]._preferredSize().h;

		if (!m_scrollbarSlots[1].bAutoHide && m_scrollbarSlots[1]._widget())
			mySize.w -= m_scrollbarSlots[1]._preferredSize().w;

		//

		SizeI contentSize = viewSlot._sizeFromPolicy( mySize );

		if( contentSize.h > mySize.h && m_scrollbarSlots[1].bAutoHide && m_scrollbarSlots[1]._widget())
		{
			mySize.w -= m_scrollbarSlots[1]._preferredSize().w;
			contentSize = viewSlot._sizeFromPolicy( mySize );

			if( contentSize.w > mySize.w && m_scrollbarSlots[0].bAutoHide && m_scrollbarSlots[0]._widget())
			{
				mySize.h -= m_scrollbarSlots[0]._preferredSize().h;
				contentSize = viewSlot._sizeFromPolicy( mySize );
			}
		}
		else if( contentSize.w > mySize.w && m_scrollbarSlots[0].bAutoHide && m_scrollbarSlots[0]._widget())
		{
			mySize.h -= m_scrollbarSlots[0]._preferredSize().h;
			contentSize = viewSlot._sizeFromPolicy( mySize );

			if( contentSize.h > mySize.h && m_scrollbarSlots[1].bAutoHide && m_scrollbarSlots[1]._widget())
			{
				mySize.w -= m_scrollbarSlots[1]._preferredSize().w;
				contentSize = viewSlot._sizeFromPolicy( mySize );
			}
		}

		return contentSize;
	}

	//____ _updateElementGeo() _____________________________________________________

	void ScrollPanel::_updateElementGeo( SizeI mySize, StaticSlot* pForceUpdate )
	{

		SizeI newContentSize = _calcContentSize( mySize );

		RectI	newDragX, newDragY, newWindow, newCorner;
		bool	bShowDragX = false, bShowDragY = false;

		// First get "default geometry" for them all, ignoring overlaps.

		newWindow = mySize;

		if( m_scrollbarSlots[0]._widget())
		{
			newDragX.h = m_scrollbarSlots[0]._paddedPreferredSize().h;
			newDragX.w = newWindow.w;
			newDragX.x = 0;

			if( m_scrollbarSlots[0].placement == Direction::Down )
				newDragX.y = newWindow.h - newDragX.h;
			else
				newDragX.y = 0;
		}

		if(m_scrollbarSlots[1]._widget())
		{
			newDragY.w = m_scrollbarSlots[1]._paddedPreferredSize().w;
			newDragY.h = newWindow.h;
			newDragY.y = 0;

			if (m_scrollbarSlots[1].placement == Direction::Right)
				newDragY.x = newWindow.w - newDragY.w;
			else
				newDragY.x = 0;
		}

		// Determine which dragbars we need to show, using basic rules

		if(m_scrollbarSlots[0]._widget() && (newContentSize.w > newWindow.w || !m_scrollbarSlots[0].bAutoHide) )
			bShowDragX = true;

		if(m_scrollbarSlots[1]._widget() && (newContentSize.h > newWindow.h || !m_scrollbarSlots[1].bAutoHide) )
			bShowDragY = true;

		// See if showing one forces us to show the other

		if( bShowDragY && m_scrollbarSlots[0]._widget() && newContentSize.w > (newWindow.w - newDragY.w) )
			bShowDragX = true;

		if( bShowDragX && m_scrollbarSlots[1]._widget() && newContentSize.h > (newWindow.h - newDragX.h) )
			bShowDragY = true;

		// We need to adjust view layout to accomodate visible scrollbars.

		if( bShowDragY )
		{
			newWindow.w -= newDragY.w;
			if (m_scrollbarSlots[1].placement == Direction::Left)
				newWindow.x += newDragY.w;
		}

		if( bShowDragX )
		{
			newWindow.h -= newDragX.h;
			if (m_scrollbarSlots[0].placement == Direction::Up)
				newWindow.y += newDragX.h;
		}

		// If both dragbars are visible we need to avoid overlap and include a corner to fill the empty square.

		if( bShowDragX && bShowDragY )
		{
			newDragX.x = newWindow.x;
			newDragX.w = newWindow.w;

			newDragY.y = newWindow.y;
			newDragY.h = newWindow.h;
		}

		// Display corner if both dragbars are visible and they are outside the view

		if( bShowDragX && bShowDragY && !m_bOverlayScrollbars )
		{
			m_cornerGeo.x = newDragY.x;
			m_cornerGeo.y = newDragX.y;
			m_cornerGeo.w = newDragY.w;
			m_cornerGeo.h = newDragX.h;
		}
		else
			m_cornerGeo.clear();

		// Remove padding from dragbars now all geometry calculations have been done

		newDragX.shrink(m_scrollbarSlots[0].padding );
		newDragY.shrink(m_scrollbarSlots[1].padding );

		// Expand view area again if scrollbars are in overlay mode
		// The previous calculations were still needed for correct
		// scrollbar placement.

		if( m_bOverlayScrollbars )
			newWindow = mySize;


		// Determine changes to views position and size over content before
		// we overwrite old values, so we can emit right signals once all is updated.
		// Update m_viewPixOfs at the same time.

		bool	bNewOfsX = false;
		bool	bNewOfsY = false;
		bool	bNewWidth = false;
		bool	bNewHeight = false;

		if( newWindow.h != viewSlot.m_windowGeo.h )
			bNewHeight = true;

		if( newWindow.w != viewSlot.m_windowGeo.w )
			bNewWidth = true;


		if(m_scrollbarSlots[0].bAutoScroll )
		{
			if( viewSlot.m_viewPixOfs.x + viewSlot.m_windowGeo.w >= viewSlot.m_contentSize.w && newWindow.w < newContentSize.w )
			{
				bNewOfsX = true;
				viewSlot.m_viewPixOfs.x = newContentSize.w - newWindow.w;
			}
		}

		if(m_scrollbarSlots[1].bAutoScroll )
		{
			if( viewSlot.m_viewPixOfs.y + viewSlot.m_windowGeo.h >= viewSlot.m_contentSize.h && newWindow.h < newContentSize.h )
			{
				bNewOfsY = true;
				viewSlot.m_viewPixOfs.y = newContentSize.h - newWindow.h;
			}
		}

		if( viewSlot.m_viewPixOfs.x + newWindow.w > newContentSize.w )
		{
			int x = newContentSize.w - newWindow.w;
			if( x < 0 )
				x = 0;
			if( x != viewSlot.m_viewPixOfs.x )
			{
				bNewOfsX = true;
				viewSlot.m_viewPixOfs.x = x;
			}
		}

		if( viewSlot.m_viewPixOfs.y + newWindow.h > newContentSize.h )
		{
			int y = newContentSize.h - newWindow.h;
			if( y < 0 )
				y = 0;
			if( y != viewSlot.m_viewPixOfs.y )
			{
				bNewOfsY = true;
				viewSlot.m_viewPixOfs.y = y;
			}
		}

		// If contentSize has changed we save changes and set flags

		bool bNewContentWidth = false;
		bool bNewContentHeight = false;

		if (newContentSize.w != viewSlot.m_contentSize.w)
			bNewContentWidth = true;

		if (newContentSize.h != viewSlot.m_contentSize.h)
			bNewContentHeight = true;

		if (bNewContentWidth || bNewContentHeight)
		{
			viewSlot.m_contentSize = newContentSize;

			// Notify content of its new size.

			if (viewSlot._widget())
				viewSlot._widget()->_resize(newContentSize);
		}
		else if( pForceUpdate == &viewSlot && viewSlot._widget())
			viewSlot._widget()->_resize(newContentSize);



		// If something visible has changed we need to update element geometry and request render.
		// This is more optimized than it looks like...

		if( newWindow != viewSlot.m_windowGeo || bShowDragX != m_scrollbarSlots[0].bVisible || bShowDragY != m_scrollbarSlots[1].bVisible || bNewContentWidth || bNewContentHeight )
		{
			viewSlot.m_windowGeo = newWindow;
			viewSlot._updateCanvasGeo();

			m_scrollbarSlots[0].geo = newDragX;
			m_scrollbarSlots[1].geo = newDragY;
			m_scrollbarSlots[0].bVisible = bShowDragX;
			m_scrollbarSlots[1].bVisible = bShowDragY;

			_requestRender();

			// Notify scrollbars of their new size if needed.

			if( (bShowDragX && newDragX.size() != m_scrollbarSlots[0]._size()) || pForceUpdate == &m_scrollbarSlots[0])
				m_scrollbarSlots[0]._widget()->_resize(newDragX.size());
			if( (bShowDragY && newDragY.size() != m_scrollbarSlots[1]._size()) || pForceUpdate == &m_scrollbarSlots[1])
				m_scrollbarSlots[1]._widget()->_resize(newDragY.size());
		}
		else
		{
			if (pForceUpdate == &m_scrollbarSlots[0] && m_scrollbarSlots[0]._widget())
				m_scrollbarSlots[0]._widget()->_resize(newDragX.size());

			if (pForceUpdate == &m_scrollbarSlots[1] && m_scrollbarSlots[1]._widget())
				m_scrollbarSlots[1]._widget()->_resize(newDragX.size());
		}


		// Notify scrollbars of any change to content size, view size or view offset.

		if( bNewOfsX || bNewWidth || bNewContentWidth )
			m_scrollbarTargets[1]._updateScrollbar( viewSlot._windowOffsetX(), viewSlot._paddedWindowLenX() );

		if( bNewOfsY || bNewHeight || bNewContentHeight )
			m_scrollbarTargets[0]._updateScrollbar(viewSlot._windowOffsetY(), viewSlot._paddedWindowLenY() );
	}




	//____ _resize() ____________________________________________________________

	void ScrollPanel::_resize( const SizeI& size )
	{
		Panel::_resize(size);

		_updateElementGeo( size );
	}

	//____ _receive() ______________________________________________________________

	void ScrollPanel::_receive( Msg * _pMsg )
	{
		Panel::_receive(_pMsg);

		switch( _pMsg->type() )
		{
			case MsgType::WheelRoll:
			{
				WheelRollMsg_p pMsg = WheelRollMsg::cast(_pMsg);

				if( pMsg->wheel() == m_wheelForScroll )
				{
					if( viewSlot.m_windowGeo.contains( _toLocal(pMsg->pointerPosRaw())) )
					{
						CoordI dist = pMsg->distance();
						if( dist.x < 0 )
							_wheelRoll( Direction::Left, -dist.x );
						if (dist.x > 0)
							_wheelRoll(Direction::Right, dist.x);

						if (dist.y < 0)
							_wheelRoll(Direction::Up, -dist.y);
						if (dist.y > 0)
							_wheelRoll(Direction::Down, dist.y);
					}
					_pMsg->swallow();
				}
			}
			break;

			default:
				break;
		}
	}

	//____ _render() ________________________________________________________

	void ScrollPanel::_render( GfxDevice * pDevice, const RectI& _canvas, const RectI& _window )
	{
		RectI dirtBounds = pixelsToRaw( pDevice->clipBounds() );

		// Render Window background skin

		if( m_pSkin )
		{
			RectI skinWindow = viewSlot.m_windowGeo + _canvas.pos();

			SizeI skinSize = SizeI::max(viewSlot.m_contentSize, viewSlot.m_windowGeo);
			RectI skinCanvas = RectI( skinWindow.pos() - viewSlot.m_viewPixOfs, skinSize );

			m_pSkin->_render( pDevice, skinCanvas, m_state );
		}

		// Render corner piece

		if (m_pCornerSkin && m_cornerGeo.w != 0 && m_cornerGeo.h != 0)
		{
			RectI canvas = m_cornerGeo + _canvas.pos();
			m_pCornerSkin->_render(pDevice, canvas, m_state);
		}

		// Render view recursively

		if( viewSlot._widget())
		{
			RectI canvas = viewSlot.m_canvasGeo + _canvas.pos();
			RectI window( canvas, viewSlot.m_windowGeo + _canvas.pos() );	// Use intersection in case canvas is smaller than window.

			if (window.intersectsWith(dirtBounds))
			{
				ClipPopData clipPop = limitClipList(pDevice,rawToPixels(window));
				viewSlot._widget()->_render(pDevice, canvas, window);
				popClipList(pDevice, clipPop);
			}
		}

		// Render scrollbars

		for (int i = 0; i < 2; i++)
		{
			if (m_scrollbarSlots[i].bVisible)
			{
				RectI canvas = m_scrollbarSlots[i].geo + _canvas.pos();
				if (canvas.intersectsWith(dirtBounds))
				{
					ClipPopData clipPop = limitClipList(pDevice,rawToPixels(canvas));
					m_scrollbarSlots[i]._widget()->_render(pDevice, canvas, canvas);
					popClipList(pDevice, clipPop);
				}
			}
		}
	}


	//____ _collectPatches() _______________________________________________________

	void ScrollPanel::_collectPatches( Patches& container, const RectI& geo, const RectI& clip )
	{
		container.add( RectI(geo,clip) );
	}

	//____ _maskPatches() __________________________________________________________

	void ScrollPanel::_maskPatches( Patches& patches, const RectI& geo, const RectI& clip, BlendMode blendMode )
	{
		//TODO: Don't just check isOpaque() globally, check rect by rect.
		if( ((m_bOpaque || (m_pSkin && m_pSkin->isOpaque(m_state))) && blendMode == BlendMode::Blend) || blendMode == BlendMode::Replace )
		{
			patches.sub( RectI(geo,clip) );
			return;
		}

		switch( m_maskOp )
		{
			case MaskOp::Recurse:
			{
				// Mask against view

				{
					ViewSlot * p = &viewSlot;

					if (m_pSkin && m_pSkin->isOpaque())
						patches.sub(RectI(p->m_windowGeo + geo.pos(), clip));
					else if (p->_widget())
						p->_widget()->_maskPatches(patches, p->m_canvasGeo + geo.pos(), RectI(p->m_windowGeo + geo.pos(), clip), blendMode);
				}

				// Mask against dragbars

				ScrollbarSlot * p = &m_scrollbarSlots[0];
				if( p->bVisible )
					p->_widget()->_maskPatches( patches, p->geo + geo.pos(), clip, blendMode );

				p++;
				if( p->bVisible )
					p->_widget()->_maskPatches( patches, p->geo + geo.pos(), clip, blendMode );

				// Maska against corner piece

				if( !m_cornerGeo.isEmpty() && m_pCornerSkin && m_pCornerSkin->isOpaque() )
					patches.sub( RectI(m_cornerGeo + geo.pos(), clip) );

				break;
			}
			case MaskOp::Skip:
				break;
			case MaskOp::Mask:
				patches.sub( RectI(geo,clip) );
				break;
		}
	}

	//____ _alphaTest() ___________________________________________________________

	bool ScrollPanel::_alphaTest( const CoordI& ofs )
	{
		if( m_pSkin && viewSlot.m_windowGeo.contains( ofs ) )
		{
			return m_pSkin->_markTest( ofs, viewSlot.m_canvasGeo, m_state, m_markOpacity );
		}

		if( m_pCornerSkin && m_cornerGeo.contains( ofs ) )
		{
			return m_pCornerSkin->_markTest( ofs, m_cornerGeo, m_state, m_markOpacity );
		}

		return false;
	}

	//____ _cloneContent() _______________________________________________________

	void ScrollPanel::_cloneContent( const Widget * _pOrg )
	{
		ScrollPanel* pOrg = (ScrollPanel*)_pOrg;

		Panel::_cloneContent(pOrg);

/*
		m_contentSize = pOrg->m_contentSize;

		m_stepSizeX = pOrg->m_stepSizeX;
		m_stepSizeY = pOrg->m_stepSizeY;
		m_jumpSizeX = pOrg->m_jumpSizeX;
		m_jumpSizeY = pOrg->m_jumpSizeY;

		m_viewPixOfs = pOrg->m_viewPixOfs;

		m_bAutoHideScrollbarX = pOrg->m_bAutoHideScrollbarX;
		m_bAutoHideScrollbarY = pOrg->m_bAutoHideScrollbarY;

		m_bScrollbarBottom = pOrg->m_bScrollbarBottom;
		m_bScrollbarRight = pOrg->m_bScrollbarRight;

		m_bAutoScrollX = pOrg->m_bAutoScrollX;
		m_bAutoScrollY = pOrg->m_bAutoScrollY;

		m_bOverlayScrollbars = pOrg->m_bOverlayScrollbars;

		m_widthPolicy = pOrg->m_widthPolicy;
		m_heightPolicy = pOrg->m_heightPolicy;
		m_contentOrigo = pOrg->m_contentOrigo;

		m_pCornerSkin = pOrg->m_pCornerSkin;
		m_cornerGeo = pOrg->m_cornerGeo;

		//

		m_elements[WINDOW].m_windowGeo = pOrg->m_elements[WINDOW].m_windowGeo;
		// Canvas geo for WINDOW should remain as it is since we don't have any content.

		//

		m_elements[XDRAG].m_windowGeo = pOrg->m_elements[XDRAG].m_windowGeo;
		m_elements[XDRAG].m_canvasGeo = pOrg->m_elements[XDRAG].m_canvasGeo;
		m_elements[XDRAG].m_bVisible = pOrg->m_elements[XDRAG].m_bVisible;
		if( pOrg->m_elements[XDRAG]._widget() )
		{
			Scrollbar * pScrollbar = (Scrollbar*) pOrg->m_elements[XDRAG]._widget()->_clone();
			pScrollbar->setScrollbarTarget(&m_scrollbarTargets[1]);
			m_elements[XDRAG]._setWidget(pScrollbar);
		}

		//

		m_elements[YDRAG].m_windowGeo = pOrg->m_elements[YDRAG].m_windowGeo;
		m_elements[YDRAG].m_canvasGeo = pOrg->m_elements[YDRAG].m_canvasGeo;
		m_elements[YDRAG].m_bVisible = pOrg->m_elements[YDRAG].m_bVisible;
		if( pOrg->m_elements[YDRAG]._widget() )
		{
			Scrollbar * pScrollbar = (Scrollbar*) pOrg->m_elements[YDRAG]._widget()->_clone();
			pScrollbar->setScrollbarTarget(&m_scrollbarTargets[0]);
			m_elements[YDRAG]._setWidget(pScrollbar);
		}
*/
		//
	}

	//____ _setScrollWheel() ___________________________________________________
	bool ScrollPanel::setScrollWheel( int wheel )
	{
		m_wheelForScroll = wheel;
		return true;
	}


	//____ _childPos() ________________________________________________________

	CoordI ScrollPanel::_childPos( const StaticSlot * pSlot ) const
	{
		if (pSlot == &viewSlot)
			return ((const ViewSlot*)pSlot)->m_canvasGeo.pos();
		else
			return ((const ScrollbarSlot*)pSlot)->geo.pos();
	}

	//____ _childRequestRender() _________________________________________________

	void ScrollPanel::_childRequestRender( StaticSlot * _pSlot )
	{
		if (_pSlot == &viewSlot)
		{
			auto * pSlot = (ViewSlot*)_pSlot;

			if (pSlot->bVisible)
				_requestRender(pSlot->m_windowGeo);
		}
		else
		{
			auto * pSlot = (ScrollbarSlot*)_pSlot;

			if (pSlot->bVisible)
				_requestRender(pSlot->geo);
		}
	}

	void ScrollPanel::_childRequestRender( StaticSlot * _pSlot, const RectI& rect )
	{
		if (_pSlot == &viewSlot)
		{
			auto * pSlot = (ViewSlot*)_pSlot;

			if (pSlot->bVisible)
			{
				RectI r(pSlot->m_windowGeo, rect + pSlot->m_canvasGeo.pos());
				if (!r.isEmpty())
					_requestRender(r);
			}
		}
		else
		{
			auto * pSlot = (ScrollbarSlot*)_pSlot;

			if (pSlot->bVisible)
				_requestRender(rect + pSlot->geo.pos());
		}
	}

	//____ _childRequestResize() _________________________________________________

	void ScrollPanel::_childRequestResize( StaticSlot * pSlot )
	{
		_updateElementGeo( m_size, pSlot );
	}

	//____ _childRequestInView() _________________________________________________

	void ScrollPanel::_childRequestInView(StaticSlot * pSlot)
	{
		// Do nothing special, view itself will always be in view.

		Panel::_childRequestInView(pSlot);
	}

	void ScrollPanel::_childRequestInView(StaticSlot * pSlot, const RectI& mustHaveArea, const RectI& niceToHaveArea)
	{
		RectI window = { viewSlot.m_viewPixOfs, viewSlot.m_windowGeo.size() };

		for (int i = 0; i < 2; i++)
		{
			RectI inside = i == 0 ? niceToHaveArea : mustHaveArea;

			int diffLeft = inside.x - window.x;
			int diffRight = inside.right() - window.right();
			int diffTop = inside.y - window.y;
			int diffBottom = inside.bottom() - window.bottom();

			if( diffLeft > 0 && diffRight > 0  )
				window.x += min(diffLeft, diffRight);
			else if( diffLeft < 0 && diffRight < 0 )
				window.x += max(diffLeft, diffRight);

			if( diffTop > 0 && diffBottom > 0 )
				window.y += min(diffTop, diffBottom);
			else if( diffTop < 0 && diffBottom < 0 )
				window.y += max(diffTop, diffBottom);
		}

		if( window.pos() != viewSlot.m_viewPixOfs )
			_setWindowPos( window.pos() );

		// Forward to any outer ScrollPanel

		RectI newMustHaveArea(mustHaveArea - viewSlot.m_viewPixOfs + viewSlot.m_windowGeo.pos(), viewSlot.m_windowGeo);
		RectI newNiceToHaveArea(niceToHaveArea - viewSlot.m_viewPixOfs + viewSlot.m_windowGeo.pos(), viewSlot.m_windowGeo);

		_requestInView(newMustHaveArea, newNiceToHaveArea);
	}


	//____ _prevChild() __________________________________________________________

	Widget * ScrollPanel::_prevChild( const StaticSlot * pSlot ) const
	{
		if( pSlot == &m_scrollbarSlots[1] )
			return m_scrollbarSlots[0]._widget() ? m_scrollbarSlots[0]._widget() : viewSlot._widget();

		if( pSlot == &m_scrollbarSlots[0] )
			return viewSlot._widget();

		return nullptr;
	}

	//____ _nextChild() __________________________________________________________

	Widget * ScrollPanel::_nextChild( const StaticSlot * pSlot ) const
	{
		if (pSlot == &viewSlot)
			return m_scrollbarSlots[0]._widget() ? m_scrollbarSlots[0]._widget() : m_scrollbarSlots[1]._widget();

		if (pSlot == &m_scrollbarSlots[0])
			return m_scrollbarSlots[1]._widget();

		return nullptr;
	}

	//____ _releaseChild() ___________________________________________________

	void ScrollPanel::_releaseChild(StaticSlot * pSlot)
	{
		_replaceChild(pSlot, nullptr);
	}



	//____ _childWindowSection() _________________________________________________

	RectI ScrollPanel::_childWindowSection( const StaticSlot * _pSlot ) const
	{
		if( _pSlot == &viewSlot )
		{
			const ViewSlot * pSlot = (const ViewSlot*) _pSlot;

			RectI window(pSlot->m_windowGeo, pSlot->m_canvasGeo);			// Use intersection in case canvas is smaller than window.
			return window - pSlot->m_canvasGeo.pos();
		}
		else
		{
			const ScrollbarSlot * pSlot = (const ScrollbarSlot*)_pSlot;
			return { 0,0, pSlot->geo.size() };
		}

	}

	//____ _setWidget() __________________________________________________________

	void ScrollPanel::_replaceChild(StaticSlot * _pSlot, Widget * pWidget)
	{
		if (_pSlot == &viewSlot)
		{
			viewSlot._setWidget(pWidget);
			viewSlot.bVisible = true;
			_updateElementGeo(m_size);
			_requestRender(viewSlot.m_windowGeo);		// If geometry is same as the old one, we need to request render ourselves.
		}
		else
		{
			ScrollbarSlot * pSlot = (ScrollbarSlot*)_pSlot;
			Scrollbar * pScrollbar = (Scrollbar*)pWidget;

			if (pSlot->_widget())
				((Scrollbar*)pSlot->_widget())->setScrollbarTarget(nullptr);

			//

			if (pScrollbar)
			{
				if (pSlot == &m_scrollbarSlots[0])
				{
					pScrollbar->setOrientation(Orientation::Horizontal);
					pScrollbar->setScrollbarTarget(&m_scrollbarTargets[1]);
					pScrollbar->setHandle(viewSlot._windowOffsetX(), viewSlot._windowFractionX());
				}
				else
				{
					pScrollbar->setScrollbarTarget(&m_scrollbarTargets[0]);
					pScrollbar->setOrientation(Orientation::Vertical);
					pScrollbar->setHandle(viewSlot._windowOffsetY(), viewSlot._windowFractionY());
				}
			}

			pSlot->_setWidget(pWidget);

			_updateElementGeo(m_size);
			_requestRender(pSlot->geo);					// If geometry is same as the old one, we need to request render ourselves.
		}
	}

	//____ _firstChild() ______________________________________________________

	Widget * ScrollPanel::_firstChild() const
	{
		if (m_scrollbarSlots[1]._widget())
			return m_scrollbarSlots[1]._widget();
		else if (m_scrollbarSlots[0]._widget())
			return m_scrollbarSlots[0]._widget();
		else
			return viewSlot._widget();
	}

	//____ _lastChild() _______________________________________________________

	Widget * ScrollPanel::_lastChild() const
	{
		if (viewSlot._widget())
			return viewSlot._widget();
		else if (m_scrollbarSlots[0]._widget())
			return m_scrollbarSlots[0]._widget();
		else
			return m_scrollbarSlots[1]._widget();
	}


	//____ _firstSlotWithGeo() ___________________________________________________

	void ScrollPanel::_firstSlotWithGeo( SlotWithGeo& package ) const
	{
		for (int i = 1; i >= 0; --i)
			if (m_scrollbarSlots[i]._widget())
			{
				package.pSlot = &m_scrollbarSlots[i];
				package.geo = m_scrollbarSlots[i].geo;
				return;
			}

		if (viewSlot._widget())
		{
			package.pSlot = &viewSlot;
			package.geo = viewSlot.m_canvasGeo;
			return;
		}

		package.pSlot = nullptr;
	}

	//____ _nextSlotWithGeo() ____________________________________________________

	void ScrollPanel::_nextSlotWithGeo( SlotWithGeo& package ) const
	{
		if (package.pSlot == &viewSlot)
			package.pSlot = nullptr;

		if (package.pSlot == &m_scrollbarSlots[1] && m_scrollbarSlots[0]._widget())
		{
			package.pSlot = &m_scrollbarSlots[0];
			package.geo = m_scrollbarSlots[0].geo;
		}
		else if (viewSlot._widget())
		{
			package.pSlot = &viewSlot;
			package.geo = viewSlot.m_canvasGeo;
		}
		package.pSlot = nullptr;
	}

	//____ MyScrollbarTarget methods ____________________________________________________

	float ScrollPanel::MyScrollbarTarget::_stepFwd()
	{
		if( m_bHorizontal )
		{
			m_pParent->_step(Direction::Right);
			return m_pParent->viewSlot._windowOffsetX();
		}
		else
		{
			m_pParent->_step(Direction::Down);
			return m_pParent->viewSlot._windowOffsetY();
		}
	}

	float ScrollPanel::MyScrollbarTarget::_stepBwd()
	{
		if( m_bHorizontal )
		{
			m_pParent->_step(Direction::Left);
			return m_pParent->viewSlot._windowOffsetX();
		}
		else
		{
			m_pParent->_step(Direction::Up);
			return m_pParent->viewSlot._windowOffsetY();
		}
	}

	float ScrollPanel::MyScrollbarTarget::_jumpFwd()
	{
		if( m_bHorizontal )
		{
			m_pParent->_jump(Direction::Right);
			return m_pParent->viewSlot._windowOffsetX();
		}
		else
		{
			m_pParent->_jump(Direction::Down);
			return m_pParent->viewSlot._windowOffsetY();
		}
	}

	float ScrollPanel::MyScrollbarTarget::_jumpBwd()
	{
		if( m_bHorizontal )
		{
			m_pParent->_jump(Direction::Left);
			return m_pParent->viewSlot._windowOffsetX();
		}
		else
		{
			m_pParent->_jump(Direction::Up);
			return m_pParent->viewSlot._windowOffsetY();
		}
	}

	float ScrollPanel::MyScrollbarTarget::_wheelRolled( int distance )
	{
		if( m_bHorizontal )
		{
			if( distance < 0 )
				m_pParent->_wheelRoll(Direction::Left, -distance);
			else
				m_pParent->_wheelRoll(Direction::Left, distance);
			return m_pParent->viewSlot._windowOffsetX();
		}
		else
		{
			if (distance < 0)
				m_pParent->_wheelRoll(Direction::Up, -distance);
			else
				m_pParent->_wheelRoll(Direction::Down, distance);
			return m_pParent->viewSlot._windowOffsetY();
		}
	}

	float ScrollPanel::MyScrollbarTarget::_setPosition( float fraction )
	{
		if( m_bHorizontal )
		{
			m_pParent->_setWindowOffset({ fraction, m_pParent->viewSlot._windowOffsetY()});
			return m_pParent->viewSlot._windowOffsetX();
		}
		else
		{
			m_pParent->_setWindowOffset({ m_pParent->viewSlot._windowOffsetX(),fraction });
			return m_pParent->viewSlot._windowOffsetY();
		}
	}

	Widget* ScrollPanel::MyScrollbarTarget::_getWidget()
	{
		return m_pParent;
	}

	float ScrollPanel::MyScrollbarTarget::_getHandlePosition()
	{
		if( m_bHorizontal )
			return m_pParent->viewSlot._windowOffsetX();
		else
			return m_pParent->viewSlot._windowOffsetY();
	}

	float ScrollPanel::MyScrollbarTarget::_getHandleSize()
	{
		if( m_bHorizontal )
			return m_pParent->viewSlot._windowFractionX();
		else
			return m_pParent->viewSlot._windowFractionY();
	}




} // namespace wg
