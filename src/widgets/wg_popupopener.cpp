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
#include <wg_msg.h>
#include <wg_inputhandler.h>
#include <wg_internal.h>

namespace wg
{
	using namespace Util;

	const TypeInfo PopupOpener::TYPEINFO = { "PopupOpener", &Widget::TYPEINFO };

	//____ constructor ____________________________________________________________

	PopupOpener::PopupOpener() : text(this), icon(this)
	{
		m_bSelectable = false;
	}

	//____ Destructor _____________________________________________________________

	PopupOpener::~PopupOpener()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& PopupOpener::typeInfo(void) const
	{
		return TYPEINFO;
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

	void PopupOpener::_render(GfxDevice * pDevice, const Rect& _canvas, const Rect& _window)
	{
		Widget::_render(pDevice, _canvas, _window);

		Rect	contentRect = _canvas;

		if (m_pSkin)
			contentRect = m_pSkin->contentRect(_canvas, m_state);

		// Get icon and text rect from content rect

		Rect iconRect = _icon()._getIconRect(contentRect);
		Rect textRect = _icon()._getTextRect(contentRect, iconRect);

		// Render icon

		if (_icon().skin())
			_icon().skin()->render(pDevice, iconRect, m_state);

		// Print text

		if (!OO(text).isEmpty())
			OO(text)._render(pDevice, textRect);
	}


	//____ _resize() ____________________________________________________________

	void PopupOpener::_resize(const Size& _size)
	{
		Widget::_resize(_size);

		Rect	contentRect(0, 0, _size);

		if (m_pSkin)
			contentRect -= m_pSkin->contentPaddingSize();

		Rect textRect = _icon()._getTextRect(contentRect, _icon()._getIconRect(contentRect));

		OO(text)._setSize(textRect);
	}

	//____ _refresh() _________________________________________________________

	void PopupOpener::_refresh()
	{
		Widget::_refresh();
		OO(text)._refresh();

		//TODO: Handling of icon and text.
	}

	//____ _receive() _________________________________________________________

	void PopupOpener::_receive(Msg * pMsg)
	{
		Widget::_receive(pMsg);

		switch (pMsg->type())
		{
			case MsgType::PopupClosed:
			{
				m_bOpen = false;
				_setState(m_closeState);
				break;
			}

			case MsgType::MouseEnter:
			{
				if (m_bOpenOnHover && !m_bOpen)
					_open();
				break;
			}

			case MsgType::MousePress:
			{
				if (!m_bOpenOnHover && static_cast<MouseButtonMsg*>(pMsg)->button() == MouseButton::Left)
				{
					if (m_bOpen)
					{
						_close();
					}
					else
					{
						_open();
						Base::inputHandler()->_yieldButtonEvents(MouseButton::Left, this, OO(_parent())->_getPopupLayer());
						m_bPressed = false;		// We have yielded our press...
					}
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

			default:
				break;
		}

		if (pMsg->isMouseButtonMsg() && static_cast<MouseButtonMsg*>(pMsg)->button() == MouseButton::Left)
			pMsg->swallow();

	}

	//____ matchingHeight() _______________________________________________________

	MU PopupOpener::matchingHeight(MU width) const
	{
		MU height = 0;

		if (m_pSkin)
			height = m_pSkin->preferredSize().h;

		if (!OO(text).isEmpty())
		{
			Size padding;

			if (m_pSkin)
				padding = m_pSkin->contentPaddingSize();

			MU heightForText = OO(text)._matchingHeight(width - padding.w) + padding.h;
			if (heightForText > height)
				height = heightForText;
		}

		//TODO: Take icon into account.

		return height;
	}


	//____ preferredSize() _____________________________________________________________

	Size PopupOpener::preferredSize() const
	{
		Size preferred;

		if (!OO(text).isEmpty())
			preferred = OO(text)._preferredSize();

		if (m_pSkin)
			preferred = m_pSkin->sizeForContent(preferred);

		//TODO: Take icon into account.

		return preferred;
	}

	//____ _setState() ________________________________________________________

	void PopupOpener::_setState(State state)
	{
		if (m_bOpen)
		{
			m_closeState = state;
			state.setPressed(true);			// Force pressed state when popup is open.
		}
		Widget::_setState(state);
		OO(text)._setState(state);
		_requestRender(); //TODO: Only requestRender if text appearance has changed (let OO(text).setState() return if rendering is needed)
	}

	//____ setSkin() _________________________________________________________

	void PopupOpener::setSkin(Skin * pSkin)
	{
		//TODO: Set canvas size for components.

		Widget::setSkin(pSkin);
	}

	//____ _open() ____________________________________________________________

	void PopupOpener::_open()
	{
		auto pLayer = OO(_parent())->_getPopupLayer();
		if (pLayer && m_pPopup)
		{
			pLayer->popupSlots.pushFront(m_pPopup, this, globalGeo(), m_attachPoint, m_bOpenOnHover );
			m_bOpen = true;
			m_closeState = m_state;
		}
	}

	//____ _close() ___________________________________________________________

	void PopupOpener::_close()
	{
		auto pLayer = OO(_parent())->_getPopupLayer();
		if (pLayer && m_pPopup)
		{
			pLayer->popupSlots.pop(m_pPopup.rawPtr());
		}
	}

	//____ _componentPos() ______________________________________________________________

	Coord PopupOpener::_componentPos(const GeoComponent * pComponent) const
	{
		Rect	contentRect = m_size;

		if (m_pSkin)
			contentRect = m_pSkin->contentRect(contentRect, m_state);

		// Get icon and text rect from content rect

		Rect iconRect = _icon()._getIconRect(contentRect);

		if (pComponent == &icon)
			return iconRect.pos();

		Rect textRect = _icon()._getTextRect(contentRect, iconRect);
		return textRect.pos();
	}

	//____ _componentSize() ______________________________________________________________

	Size PopupOpener::_componentSize(const GeoComponent * pComponent) const
	{
		Size	sz = m_size;

		if (m_pSkin)
			sz -= m_pSkin->contentPaddingSize();

		Rect iconRect = _icon()._getIconRect(sz);

		if (pComponent == &icon)
			return iconRect.size();

		Rect textRect = _icon()._getTextRect(sz, iconRect);
		return textRect.size();

	}

	//____ _componentGeo() ______________________________________________________________

	Rect PopupOpener::_componentGeo(const GeoComponent * pComponent) const
	{
		Rect	contentRect = m_size;

		if (m_pSkin)
			contentRect = m_pSkin->contentRect(contentRect, m_state);

		// Get icon and text rect from content rect

		Rect iconRect = _icon()._getIconRect(contentRect);

		if (pComponent == &icon)
			return iconRect;

		Rect textRect = _icon()._getTextRect(contentRect, iconRect);
		return textRect;
	}



}	// namespace wg
