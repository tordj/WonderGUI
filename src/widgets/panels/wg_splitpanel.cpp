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

namespace wg
{
	const char SplitPanel::CLASSNAME[] = { "SplitPanel" };

	//____ Constructor ____________________________________________________________

	SplitPanel::SplitPanel() : first(&m_firstChild, this), second(&m_secondChild,this)
	{
		m_bHorizontal = false;
		m_handleThickness = 0;
		m_lengthFraction = 32768;
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



	void SplitPanel::setHandleSkin(Skin * pSkin)
	{
	}

	void SplitPanel::setHandleThickness(int thickness)
	{
	}

	void SplitPanel::setBrokerFunction(std::function<int(Widget * pFirst, Widget * pSecond, int sizeChange)> func)
	{
	}


	bool SplitPanel::_updateGeo()
	{
		Rect geo = m_size;
		Rect contentGeo = m_pSkin ? m_pSkin->contentRect(geo, m_state) : geo;

		Rect firstChildGeo;
		Rect secondChildGeo;
		Rect handleGeo;

		int handleThickness = m_handleThickness;
		if (handleThickness == 0 && m_pHandleSkin)
			handleThickness = m_bHorizontal ? m_pHandleSkin->preferredSize().w : m_pHandleSkin->preferredSize().h;

		int firstChildLength = ((contentGeo.h - handleThickness) * m_lengthFraction + 32767) >> 16;
		int secondChildLength = (contentGeo.h - handleThickness) - firstChildLength;

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



	void SplitPanel::_refresh()
	{
	}

	void SplitPanel::_receive(Msg * pMsg)
	{
	}

	void SplitPanel::_renderPatches(GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, Patches * _pPatches)
	{
	}

	void SplitPanel::_collectPatches(Patches& container, const Rect& geo, const Rect& clip)
	{
	}

	void SplitPanel::_maskPatches(Patches& patches, const Rect& geo, const Rect& clip, BlendMode blendMode)
	{
	}

	bool SplitPanel::_alphaTest(const Coord& ofs)
	{
	}

	void SplitPanel::_cloneContent(const Widget * _pOrg)
	{
	}

	void SplitPanel::_setSize(const Size& size)
	{
	}

	Widget * SplitPanel::_firstChild() const
	{
		if (m_firstChild.pWidget)
			return m_firstChild.pWidget;
		else
			return m_secondChild.pWidget;
	}

	Widget * SplitPanel::_lastChild() const
	{
		if (m_secondChild.pWidget)
			return m_secondChild.pWidget;
		else
			return m_firstChild.pWidget;
	}

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

	void SplitPanel::_setWidget(Slot * pSlot, Widget * pNewWidget)
	{
	}

	Coord SplitPanel::_childPos(Slot * pSlot) const
	{
	}

	Size SplitPanel::_childSize(Slot * pSlot) const
	{
	}

	void SplitPanel::_childRequestRender(Slot * pSlot)
	{
	}

	void SplitPanel::_childRequestRender(Slot * pSlot, const Rect& rect)
	{
	}

	void SplitPanel::_childRequestResize(Slot * pSlot)
	{
	}

	Widget * SplitPanel::_prevChild(Slot * pSlot) const
	{
		if (pSlot == &m_secondChild)
			return m_secondChild.pWidget;
		else
			return nullptr;
	}

	Widget * SplitPanel::_nextChild(Slot * pSlot) const
	{
		if (pSlot == &m_firstChild)
			return m_secondChild.pWidget;
		else
			return nullptr;
	}




}