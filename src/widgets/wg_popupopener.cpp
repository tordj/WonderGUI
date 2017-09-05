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

#include <wg_popupopener.h>
#include <wg_container.h>
#include <wg_popuplayer.h>

namespace wg
{

	const char PopupOpener::CLASSNAME[] = { "PopupOpener" };

	//____ Constructor ____________________________________________________________

	PopupOpener::PopupOpener() : m_text(this), m_icon(this), label(&m_text), icon(&m_icon), m_attachPoint(Origo::SouthWest), m_bOpenOnHover(false)
	{
	}

	//____ Destructor _____________________________________________________________

	PopupOpener::~PopupOpener()
	{
	}

	//____ isInstanceOf() _________________________________________________________

	bool PopupOpener::isInstanceOf(const char * pClassName) const
	{
		if (pClassName == CLASSNAME)
			return true;

		return Widget::isInstanceOf(pClassName);
	}

	//____ className() ____________________________________________________________

	const char * PopupOpener::className(void) const
	{
		return CLASSNAME;
	}

	//____ cast() _________________________________________________________________

	PopupOpener_p PopupOpener::cast(Object * pObject)
	{
		if (pObject && pObject->isInstanceOf(CLASSNAME))
			return PopupOpener_p(static_cast<PopupOpener*>(pObject));

		return 0;
	}

	//____ setPopup() _________________________________________________________

	void PopupOpener::setPopup(Widget * pPopup)
	{
		m_pPopup = pPopup;
	}

	//____ setOpenOnHover() ___________________________________________________

	void PopupOpener::setOpenOnHover(bool bOpen)
	{
		m_bOpenOnHover = bOpen;
	}

	//____ setAttachPoint() ___________________________________________________

	void PopupOpener::setAttachPoint(Origo attachPoint)
	{
		m_attachPoint = attachPoint;
	}

	//____ _cloneContent() ____________________________________________________

	void PopupOpener::_cloneContent(const Widget * _pOrg)
	{

	}

	//____ _render() __________________________________________________________

	void PopupOpener::_render(GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, const Rect& _clip)
	{
		Widget::_render(pDevice, _canvas, _window, _clip);

		Rect	contentRect = _canvas;

		if (m_pSkin)
			contentRect = m_pSkin->contentRect(_canvas, m_state);

		// Get icon and text rect from content rect

		Rect iconRect = m_icon.getIconRect(contentRect);
		Rect textRect = m_icon.getTextRect(contentRect, iconRect);

		// Render icon

		if (m_icon.skin())
			m_icon.skin()->render(pDevice, iconRect, m_state, _clip);

		// Print text

		if (!m_text.isEmpty())
			m_text.render(pDevice, textRect, _clip);
	}

	void PopupOpener::_setSize(const Size& size)
	{
		Widget::_setSize(size);
	}

	void PopupOpener::_refresh()
	{
		Widget::_refresh();
	}

	void PopupOpener::_receive(Msg * pMsg)
	{
		Widget::_receive(pMsg);

		switch (pMsg->type())
		{
			case MsgType::MouseEnter:
			{
				if (m_bOpenOnHover)
					_open();
				break;
			}

			case MsgType::MousePress:
			{
				if (!m_bOpenOnHover && MouseButtonMsg::cast(pMsg)->button() == MouseButton::Left)
				{
					_open();
					Base::inputHandler()->_yieldButtonEvents(MouseButton::Left, this, _parent()->_getPopupLayer() );
				}
				break;
			}
/*
			case MsgType::MouseRelease:
			{
				auto pButtonMsg = MouseReleaseMsg::cast(pMsg);
				if (!m_bOpenOnHover && pButtonMsg->button() == MouseButton::Left && !pButtonMsg->releaseInside() )
					_close();
				break;
			}
*/
		}

		if (pMsg->isMouseButtonMsg() && MouseButtonMsg::cast(pMsg)->button() == MouseButton::Left)
			pMsg->swallow();

	}

	void PopupOpener::_setState(State state)
	{
		Widget::_setState(state);
		m_text.setState(state);
		_requestRender(); //TODO: Only requestRender if text appearance has changed (let m_text.setState() return if rendering is needed)
	}

	void PopupOpener::_setSkin(Skin * pSkin)
	{
		Widget::_setSkin(pSkin);
	}


	void PopupOpener::_open()
	{
		auto pLayer = _parent()->_getPopupLayer();
		if (pLayer && m_pPopup)
		{
			pLayer->popups.push(m_pPopup, this, globalGeo(), m_attachPoint);
		}
	}

	void PopupOpener::_close()
	{
	}



}	// namespace wg
