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

//TODO: Set opaque if there is no skin (or skin has no borders) and both children and the handle are opaque.

namespace wg
{
	const char SplitPanel::CLASSNAME[] = { "SplitPanel" };

	//____ Constructor ____________________________________________________________

	SplitPanel::SplitPanel() : first(&m_firstChild, this), second(&m_secondChild,this)
	{
		m_bHorizontal = false;
		m_handleThickness = 0;
		m_lengthFraction = 0.5f;

		m_bSiblingsOverlap = false;
	}

	//____ Destructor _____________________________________________________________

	SplitPanel::~SplitPanel()
	{
	}

	//____ isInstanceOf() _________________________________________________________

	bool SplitPanel::isInstanceOf(const char * pClassName) const
	{
		if (pClassName == CLASSNAME)
			return true;

		return Panel::isInstanceOf(pClassName);
	}

	//____ className() ____________________________________________________________

	const char * SplitPanel::className(void) const
	{
		return CLASSNAME;
	}

	//____ cast() _________________________________________________________________

	SplitPanel_p SplitPanel::cast(Object * pObj)
	{
		if (pObj && pObj->isInstanceOf(CLASSNAME))
			return SplitPanel_p(static_cast<SplitPanel*>(pObj));

		return 0;
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

	//____ preferredSize() _______________________________________________________

	Size  SplitPanel::preferredSize() const
	{
		return m_preferredSize;
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

	void SplitPanel::setHandleThickness(int thickness)
	{
		//TODO: Assert on negative value.

		if (thickness != m_handleThickness)
		{
			m_handleThickness = thickness;
			_updatePreferredSize();
			_updateGeo();
		}
	}

	//____ setBrokerFunction() ___________________________________________________

	void SplitPanel::setBrokerFunction(std::function<int(Widget * pFirst, Widget * pSecond, int totalLength, float fraction)> func)
	{
		m_brokerFunc = func;
		_updateGeo();
	}

	//____ _handleThickness() ____________________________________________________

	int SplitPanel::_handleThickness()
	{
		int thickness = m_handleThickness;
		if (thickness == 0 && m_pHandleSkin)
			thickness = m_bHorizontal ? m_pHandleSkin->preferredSize().w : m_pHandleSkin->preferredSize().h;
		return thickness;
	}


	//____ _updatePreferredSize() ________________________________________________

	void SplitPanel::_updatePreferredSize()
	{
		Size firstSz;
		Size secondSz;

		Size sz;

		int handleThickness = _handleThickness();

		if (m_firstChild.pWidget)
			firstSz = m_firstChild.pWidget->preferredSize();
		
		if (m_secondChild.pWidget)
			secondSz = m_secondChild.pWidget->preferredSize();

		if (m_bHorizontal)
		{
			sz.w = firstSz.w + secondSz.w + _handleThickness();
			sz.h = max(firstSz.h, secondSz.h);
			if (m_pHandleSkin && m_pHandleSkin->preferredSize().h > sz.h)
				sz.h = m_pHandleSkin->preferredSize().h;
		}
		else
		{
			sz.w = max(firstSz.w, secondSz.w);
			sz.h = firstSz.h + secondSz.h + _handleThickness();
			if (m_pHandleSkin && m_pHandleSkin->preferredSize().w > sz.w)
				sz.w = m_pHandleSkin->preferredSize().w;
		}

		// Take skins padding and preferred size into account

		if (m_pSkin)
		{
			sz += m_pSkin->contentPadding();
			Size skinSz = m_pSkin->preferredSize();
			if (skinSz.w > sz.w)
				sz.w = skinSz.w;
			if (skinSz.h > sz.h)
				sz.h = skinSz.h;
		}
		
		if (sz != m_preferredSize)
		{
			m_preferredSize = sz;
			_requestResize();
		}
	}

	//____ _updateGeo() __________________________________________________________

	bool SplitPanel::_updateGeo()
	{
		Rect geo = m_size;
		Rect contentGeo = m_pSkin ? m_pSkin->contentRect(geo, m_state) : geo;

		Rect firstChildGeo;
		Rect secondChildGeo;
		Rect handleGeo;

		int handleThickness = _handleThickness();

		int totalLength = m_bHorizontal ? contentGeo.w : contentGeo.h;
		int firstChildLength;
		int secondChildLength;

		if (m_brokerFunc)
			firstChildLength = m_brokerFunc(m_firstChild.pWidget, m_secondChild.pWidget, totalLength, m_lengthFraction);
		else
			firstChildLength = _defaultBroker(m_firstChild.pWidget, m_secondChild.pWidget, totalLength, m_lengthFraction);

		secondChildLength = totalLength - firstChildLength;

		if( m_bHorizontal )
		{ 
			firstChildGeo = Rect(contentGeo.x, contentGeo.y, firstChildLength, contentGeo.h);
			secondChildGeo = Rect(contentGeo.x + contentGeo.h - secondChildLength, contentGeo.y, secondChildLength, contentGeo.h);
			handleGeo = Rect(contentGeo.x, contentGeo.y + firstChildLength, contentGeo.w, handleThickness);
		}
		else
		{
			firstChildGeo = Rect(contentGeo.x, contentGeo.y, contentGeo.w, firstChildLength);
			secondChildGeo = Rect(contentGeo.x, contentGeo.y + contentGeo.h - secondChildLength, contentGeo.w, secondChildLength);
			handleGeo = Rect(contentGeo.x, contentGeo.y + firstChildLength, contentGeo.w, handleThickness);
		}

		if (handleGeo != m_handleGeo || firstChildGeo != m_firstChild.geo || secondChildGeo != m_secondChild.geo)
		{
			_requestRender(contentGeo);
			
			m_firstChild.geo = firstChildGeo;
			if( m_firstChild.pWidget )
				m_firstChild.pWidget->_setSize(firstChildGeo);

			m_secondChild.geo = secondChildGeo;
			if (m_secondChild.pWidget)
				m_secondChild.pWidget->_setSize(secondChildGeo);

			return true;
		}

		return false;
	}

	//____ _defaultBroker() ___________________________________________________

	int SplitPanel::_defaultBroker(Widget * pFirst, Widget * pSecond, int totalLength, float fraction)
	{
		int firstLength = (int) ((fraction * totalLength) + 0.5f);

		int minLengthFirst = 0;
		int minLengthSecond = 0;
		int maxLengthFirst = INT_MAX;
		int maxLengthSecond = INT_MAX;

		if( m_bHorizontal )
		{
			if (pFirst)
			{
				minLengthFirst = pFirst->minSize().w;
				maxLengthFirst = pFirst->maxSize().w;
			}

			if (pSecond)
			{
				minLengthSecond = pFirst->minSize().w;
				maxLengthSecond = pFirst->maxSize().w;
			}
		}
		else
		{
			if (pFirst)
			{
				minLengthFirst = pFirst->minSize().h;
				maxLengthFirst = pFirst->maxSize().h;
			}

			if (pSecond)
			{
				minLengthSecond = pFirst->minSize().h;
				maxLengthSecond = pFirst->maxSize().h;
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

		switch (pMsg->type())
		{
			case MsgType::MouseEnter:
			break;

			case MsgType::MouseLeave:
			break;

			case MsgType::MouseMove:
			break;

			case MsgType::MousePress:
			break;

			case MsgType::MouseRelease:
				break;

			case MsgType::MouseDrag:
				break;
		}
	}


	//____ _render() _____________________________________________________________

	void SplitPanel::_render(GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, const Rect& _clip)
	{
		Panel::_render(pDevice, _canvas, _window, _clip);

		if (m_pHandleSkin)
			m_pHandleSkin->render(pDevice, m_handleGeo, m_handleState, _clip);
	}


	//____ _collectPatches() __________________________________________________

	void SplitPanel::_collectPatches(Patches& container, const Rect& geo, const Rect& clip)
	{
		if (m_pSkin)
			container.add(Rect(geo, clip));
		else
		{
			if (m_firstChild.pWidget)
				m_firstChild.pWidget->_collectPatches(container, m_firstChild.geo + geo.pos(), clip );

			if( m_pHandleSkin )
				container.add(Rect(m_handleGeo, clip));

			if (m_secondChild.pWidget)
				m_secondChild.pWidget->_collectPatches(container, m_secondChild.geo + geo.pos(), clip );
		}
	}

	//____ _maskPatches() _____________________________________________________

	void SplitPanel::_maskPatches(Patches& patches, const Rect& geo, const Rect& clip, BlendMode blendMode)
	{
		{
			//TODO: Don't just check isOpaque() globally, check rect by rect.
			if ((m_bOpaque && blendMode == BlendMode::Blend) || blendMode == BlendMode::Replace)
				patches.sub(Rect(geo, clip));
			else
			{
				if (m_firstChild.pWidget)
					m_firstChild.pWidget->_maskPatches(patches, m_firstChild.geo + geo.pos(), clip, blendMode );

				if (m_pHandleSkin && m_pHandleSkin->isOpaque() )
					patches.sub(Rect(m_handleGeo, clip));

				if (m_secondChild.pWidget)
					m_secondChild.pWidget->_maskPatches(patches, m_secondChild.geo + geo.pos(), clip, blendMode );
			}
		}
	}

	//____ _alphaTest() _______________________________________________________

	bool SplitPanel::_alphaTest(const Coord& ofs)
	{
		bool bHit = Panel::_alphaTest(ofs);

		if( !bHit && m_pHandleSkin )
			bHit = m_pHandleSkin->markTest(ofs, m_handleGeo, m_handleState, m_markOpacity);

		return bHit;
	}

	//____ _cloneContent() ____________________________________________________

	void SplitPanel::_cloneContent(const Widget * _pOrg)
	{
		//TODO: Implement!!!
	}

	//____ _setSize() _________________________________________________________

	void SplitPanel::_setSize(const Size& size)
	{
		Panel::_setSize(size);
		_updateGeo();
	}

	//____ _setState() ________________________________________________________

	void SplitPanel::_setState(State state)
	{
		// Disable handle when panel is disabled

		m_handleState.setEnabled(state.isEnabled());
		Panel::_setState(state);
	}


	//_____ _firstChild() _____________________________________________________

	Widget * SplitPanel::_firstChild() const
	{
		if (m_firstChild.pWidget)
			return m_firstChild.pWidget;
		else
			return m_secondChild.pWidget;
	}

	//_____ _lastChild() ______________________________________________________

	Widget * SplitPanel::_lastChild() const
	{
		if (m_secondChild.pWidget)
			return m_secondChild.pWidget;
		else
			return m_firstChild.pWidget;
	}

	//_____ _firstSlotWithGeo() _______________________________________________

	void SplitPanel::_firstSlotWithGeo(SlotWithGeo& package) const
	{
		const SplitPanelSlot * p;

		if (m_firstChild.pWidget)
			p = &m_firstChild;
		else if (m_secondChild.pWidget)
			p = &m_secondChild;
		else
		{
			package.pSlot = nullptr;
			return;
		}

		package.geo = p->geo;
		package.pSlot = p;
	}

	//____ _nextSlotWithGeo() _________________________________________________

	void SplitPanel::_nextSlotWithGeo(SlotWithGeo& package) const
	{
		if (package.pSlot == &m_firstChild && m_secondChild.pWidget)
		{
			package.geo = m_secondChild.geo;
			package.pSlot = &m_secondChild;
		}
		else
			package.pSlot = nullptr;
	}

	//____ _setWidget() _______________________________________________________

	void SplitPanel::_setWidget(Slot * pSlot, Widget * pNewWidget)
	{
		pSlot->replaceWidget(this, pNewWidget);
		_updatePreferredSize();
		bool bGeoChanged =_updateGeo();
		if (!bGeoChanged)
			_requestRender(static_cast<SplitPanelSlot*>(pSlot)->geo);

	}

	//_____ _childPos() _______________________________________________________

	Coord SplitPanel::_childPos(Slot * pSlot) const
	{
		return static_cast<SplitPanelSlot*>(pSlot)->geo.pos();
	}

	//_____ _childSize() ______________________________________________________

	Size SplitPanel::_childSize(Slot * pSlot) const
	{
		return static_cast<SplitPanelSlot*>(pSlot)->geo.size();
	}

	//____ _childRequestRender() ______________________________________________

	void SplitPanel::_childRequestRender(Slot * pSlot)
	{
		_requestRender(static_cast<SplitPanelSlot*>(pSlot)->geo);
	}

	void SplitPanel::_childRequestRender(Slot * pSlot, const Rect& rect)
	{
		_requestRender(rect + static_cast<SplitPanelSlot*>(pSlot)->geo.pos());
	}

	//____ _childRequestResize() ______________________________________________

	void SplitPanel::_childRequestResize(Slot * pSlot)
	{
		//TODO: Implement!!!
	}

	//____ _prevChild() _______________________________________________________

	Widget * SplitPanel::_prevChild(Slot * pSlot) const
	{
		if (pSlot == &m_secondChild)
			return m_secondChild.pWidget;
		else
			return nullptr;
	}

	//____ _nextChild() _______________________________________________________

	Widget * SplitPanel::_nextChild(Slot * pSlot) const
	{
		if (pSlot == &m_firstChild)
			return m_secondChild.pWidget;
		else
			return nullptr;
	}




}