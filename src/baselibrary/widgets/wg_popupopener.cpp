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
#include <wg_popupoverlay.h>
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

	void PopupOpener::setAttachPoint(Placement attachPoint)
	{
		m_attachPoint = attachPoint;
	}

	//____ _matchingHeight() _______________________________________________________

	spx PopupOpener::_matchingHeight(spx width, int scale) const
	{
		scale = _fixScale(scale);

		spx height = m_skin.defaultSize(scale).h;

		if (!OO(text).isEmpty())
		{
			SizeSPX padding = m_skin.contentPaddingSize(scale);

			spx heightForText = OO(text)._matchingHeight(width - padding.w, scale) + padding.h;
			if (heightForText > height)
				height = heightForText;
		}

		//TODO: Take icon into account.

		return height;
	}


	//____ _defaultSize() _____________________________________________________________

	SizeSPX PopupOpener::_defaultSize(int scale) const
	{
		scale = _fixScale(scale);

		SizeSPX defaultSize;

		if (!OO(text).isEmpty())
			defaultSize = OO(text)._defaultSize(scale);

		defaultSize = m_skin.sizeForContent(defaultSize,scale);

		//TODO: Take icon into account.

		return defaultSize;
	}

	//____ _render() __________________________________________________________

	void PopupOpener::_render(GfxDevice * pDevice, const RectSPX& _canvas, const RectSPX& _window)
	{
		Widget::_render(pDevice, _canvas, _window);

		RectSPX	contentRect = m_skin.contentRect(_canvas, m_scale, m_state);

		// Get icon and text rect from content rect

		RectSPX iconRect = _icon()._getIconRect(contentRect,m_scale);
		RectSPX textRect = _icon()._getTextRect(contentRect, iconRect,m_scale);

		// Render icon

		if (_icon().skin())
			_icon().skin()->_render(pDevice, iconRect, m_scale, m_state);

		// Print text

		if (!OO(text).isEmpty())
			OO(text)._render(pDevice, textRect);
	}


	//____ _resize() ____________________________________________________________

	void PopupOpener::_resize(const SizeSPX& _size, int scale)
	{
		Widget::_resize(_size,scale);

		RectSPX	contentRect(0, 0, _size);

		contentRect -= m_skin.contentPaddingSize(m_scale);

		RectSPX textRect = _icon()._getTextRect(contentRect, _icon()._getIconRect(contentRect,m_scale),m_scale);

		OO(text)._setSize(textRect,m_scale);
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
						Base::inputHandler()->_yieldButtonEvents(MouseButton::Left, this, m_pPopup);
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

	//____ _open() ____________________________________________________________

	void PopupOpener::_open()
	{
		auto pLayer = OO(_parent())->_getPopupOverlay();
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
		auto pLayer = OO(_parent())->_getPopupOverlay();
		if (pLayer && m_pPopup)
		{
			pLayer->popupSlots.pop(m_pPopup.rawPtr());
		}
	}

	//____ _componentPos() ______________________________________________________________

	CoordSPX PopupOpener::_componentPos(const WidgetComponent * pComponent) const
	{
		RectSPX contentRect = m_skin.contentRect(m_size, m_scale, m_state);

		// Get icon and text rect from content rect

		RectSPX iconRect = _icon()._getIconRect(contentRect, m_scale);

		if (pComponent == &icon)
			return iconRect.pos();

		RectSPX textRect = _icon()._getTextRect(contentRect, iconRect, m_scale);
		return textRect.pos();
	}

	//____ _componentSize() ______________________________________________________________

	SizeSPX PopupOpener::_componentSize(const WidgetComponent * pComponent) const
	{
		SizeSPX sz = m_size - m_skin.contentPaddingSize(m_scale);

		RectSPX iconRect = _icon()._getIconRect(sz,m_scale);

		if (pComponent == &icon)
			return iconRect.size();

		RectSPX textRect = _icon()._getTextRect(sz, iconRect, m_scale);
		return textRect.size();

	}

	//____ _componentGeo() ______________________________________________________________

	RectSPX PopupOpener::_componentGeo(const WidgetComponent * pComponent) const
	{
		RectSPX	contentRect = m_skin.contentRect(m_size, m_scale, m_state);

		// Get icon and text rect from content rect

		RectSPX iconRect = _icon()._getIconRect(contentRect, m_scale);

		if (pComponent == &icon)
			return iconRect;

		RectSPX textRect = _icon()._getTextRect(contentRect, iconRect, m_scale);
		return textRect;
	}

}	// namespace wg
