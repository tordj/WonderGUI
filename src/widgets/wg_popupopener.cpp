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

	PopupOpener::PopupOpener() : label(this), icon(this)
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

	void PopupOpener::_render(GfxDevice * pDevice, const RectI& _canvas, const RectI& _window)
	{
		Widget::_render(pDevice, _canvas, _window);

		RectI	contentRect = _canvas;

		if (m_pSkin)
			contentRect = m_pSkin->_contentRect(_canvas, m_state);

		// Get icon and text rect from content rect

		RectI iconRect = _icon()._getIconRect(contentRect);
		RectI textRect = _icon()._getTextRect(contentRect, iconRect);

		// Render icon

		if (_icon().skin())
			_icon().skin()->_render(pDevice, iconRect, m_state);

		// Print text

		if (!_label().isEmpty())
			_label()._render(pDevice, textRect);
	}


	//____ _resize() ____________________________________________________________

	void PopupOpener::_resize(const SizeI& _size)
	{
		Widget::_resize(_size);

		RectI	contentRect(0, 0, _size);

		if (m_pSkin)
			contentRect -= m_pSkin->_contentPaddingSize();

		RectI textRect = _icon()._getTextRect(contentRect, _icon()._getIconRect(contentRect));

		_label()._setSize(textRect);
	}

	//____ _refresh() _________________________________________________________

	void PopupOpener::_refresh()
	{
		Widget::_refresh();
		_label()._refresh();

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

	//____ _matchingHeight() _______________________________________________________

	int PopupOpener::_matchingHeight(int width) const
	{
		int height = 0;

		if (m_pSkin)
			height = m_pSkin->_preferredSize().h;

		if (!_label().isEmpty())
		{
			SizeI padding;

			if (m_pSkin)
				padding = m_pSkin->_contentPaddingSize();

			int heightForText = _label()._matchingHeight(width - padding.w) + padding.h;
			if (heightForText > height)
				height = heightForText;
		}

		//TODO: Take icon into account.

		return height;
	}


	//____ _preferredSize() _____________________________________________________________

	SizeI PopupOpener::_preferredSize() const
	{
		SizeI preferred;

		if (!_label().isEmpty())
			preferred = _label()._preferredSize();

		if (m_pSkin)
			preferred = m_pSkin->_sizeForContent(preferred);

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
		_label()._setState(state);
		_requestRender(); //TODO: Only requestRender if text appearance has changed (let _label().setState() return if rendering is needed)
	}

	//____ _setSkin() _________________________________________________________

	void PopupOpener::_setSkin(Skin * pSkin)
	{
		Widget::_setSkin(pSkin);
	}

	//____ _open() ____________________________________________________________

	void PopupOpener::_open()
	{
		auto pLayer = OO(_parent())->_getPopupLayer();
		if (pLayer && m_pPopup)
		{
			pLayer->popupSlots.pushFront(m_pPopup, this, rawToQpix(_globalGeo()), m_attachPoint, m_bOpenOnHover );
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

	CoordI PopupOpener::_componentPos(const GeoComponent * pComponent) const
	{
		RectI	contentRect = m_size;

		if (m_pSkin)
			contentRect = m_pSkin->_contentRect(contentRect, m_state);

		// Get icon and text rect from content rect

		RectI iconRect = _icon()._getIconRect(contentRect);

		if (pComponent == &icon)
			return iconRect.pos();

		RectI textRect = _icon()._getTextRect(contentRect, iconRect);
		return textRect.pos();
	}

	//____ _componentSize() ______________________________________________________________

	SizeI PopupOpener::_componentSize(const GeoComponent * pComponent) const
	{
		SizeI	sz = m_size;

		if (m_pSkin)
			sz -= m_pSkin->_contentPaddingSize();

		RectI iconRect = _icon()._getIconRect(sz);

		if (pComponent == &icon)
			return iconRect.size();

		RectI textRect = _icon()._getTextRect(sz, iconRect);
		return textRect.size();

	}

	//____ _componentGeo() ______________________________________________________________

	RectI PopupOpener::_componentGeo(const GeoComponent * pComponent) const
	{
		RectI	contentRect = m_size;

		if (m_pSkin)
			contentRect = m_pSkin->_contentRect(contentRect, m_state);

		// Get icon and text rect from content rect

		RectI iconRect = _icon()._getIconRect(contentRect);

		if (pComponent == &icon)
			return iconRect;

		RectI textRect = _icon()._getTextRect(contentRect, iconRect);
		return textRect;
	}



}	// namespace wg
