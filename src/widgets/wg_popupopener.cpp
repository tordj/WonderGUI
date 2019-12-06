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

namespace wg
{
	using namespace Util;

	const char PopupOpener::CLASSNAME[] = { "PopupOpener" };

	//____ Constructor ____________________________________________________________

	PopupOpener::PopupOpener() : m_text(this), m_icon(this), label(&m_text), icon(&m_icon), m_attachPoint(Origo::SouthWest), m_bOpenOnHover(false), m_bOpen(false)
	{
		m_bSelectable = false;
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

	void PopupOpener::_render(GfxDevice * pDevice, const RectI& _canvas, const RectI& _window)
	{
		Widget::_render(pDevice, _canvas, _window);

		RectI	contentRect = _canvas;

		if (m_pSkin)
			contentRect = m_pSkin->_contentRect(_canvas, m_state);

		// Get icon and text rect from content rect

		RectI iconRect = m_icon.getIconRect(contentRect);
		RectI textRect = m_icon.getTextRect(contentRect, iconRect);

		// Render icon

		if (m_icon.skin())
			m_icon.skin()->_render(pDevice, iconRect, m_state);

		// Print text

		if (!m_text.isEmpty())
			m_text.render(pDevice, textRect);
	}


	//____ _resize() ____________________________________________________________

	void PopupOpener::_resize(const SizeI& _size)
	{
		Widget::_resize(_size);

		RectI	contentRect(0, 0, _size);

		if (m_pSkin)
			contentRect -= m_pSkin->_contentPadding();

		RectI textRect = m_icon.getTextRect(contentRect, m_icon.getIconRect(contentRect));

		m_text.setSize(textRect);
	}

	//____ _refresh() _________________________________________________________

	void PopupOpener::_refresh()
	{
		Widget::_refresh();
		m_text.refresh();

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
				if (!m_bOpenOnHover && MouseButtonMsg::cast(pMsg)->button() == MouseButton::Left)
				{
					if (m_bOpen)
					{
						_close();
					}
					else
					{
						_open();
						Base::inputHandler()->_yieldButtonEvents(MouseButton::Left, this, _parent()->_getPopupLayer());
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

		if (pMsg->isMouseButtonMsg() && MouseButtonMsg::cast(pMsg)->button() == MouseButton::Left)
			pMsg->swallow();

	}

	//____ _matchingHeight() _______________________________________________________

	int PopupOpener::_matchingHeight(int width) const
	{
		int height = 0;

		if (m_pSkin)
			height = m_pSkin->_preferredSize().h;

		if (!m_text.isEmpty())
		{
			SizeI padding;

			if (m_pSkin)
				padding = m_pSkin->_contentPadding();

			int heightForText = m_text.matchingHeight(width - padding.w) + padding.h;
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

		if (!m_text.isEmpty())
			preferred = m_text.preferredSize();

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
		m_text.setState(state);
		_requestRender(); //TODO: Only requestRender if text appearance has changed (let m_text.setState() return if rendering is needed)
	}

	//____ _setSkin() _________________________________________________________

	void PopupOpener::_setSkin(Skin * pSkin)
	{
		Widget::_setSkin(pSkin);
	}

	//____ _open() ____________________________________________________________

	void PopupOpener::_open()
	{
		auto pLayer = _parent()->_getPopupLayer();
		if (pLayer && m_pPopup)
		{
			pLayer->popups.push(m_pPopup, this, rawToQpix(_globalGeo()), m_attachPoint, m_bOpenOnHover );
			m_bOpen = true;
			m_closeState = m_state;
		}
	}

	//____ _close() ___________________________________________________________

	void PopupOpener::_close()
	{
		auto pLayer = _parent()->_getPopupLayer();
		if (pLayer && m_pPopup)
		{
			pLayer->popups.pop(m_pPopup.rawPtr());
		}
	}

	//____ _componentPos() ______________________________________________________________

	CoordI PopupOpener::_componentPos(const GeoComponent * pComponent) const
	{
		RectI	contentRect = m_size;

		if (m_pSkin)
			contentRect = m_pSkin->_contentRect(contentRect, m_state);

		// Get icon and text rect from content rect

		RectI iconRect = m_icon.getIconRect(contentRect);

		if (pComponent == &m_icon)
			return iconRect.pos();

		RectI textRect = m_icon.getTextRect(contentRect, iconRect);
		return textRect.pos();
	}

	//____ _componentSize() ______________________________________________________________

	SizeI PopupOpener::_componentSize(const GeoComponent * pComponent) const
	{
		SizeI	sz = m_size;

		if (m_pSkin)
			sz -= m_pSkin->_contentPadding();

		RectI iconRect = m_icon.getIconRect(sz);

		if (pComponent == &m_icon)
			return iconRect.size();

		RectI textRect = m_icon.getTextRect(sz, iconRect);
		return textRect.size();

	}

	//____ _componentGeo() ______________________________________________________________

	RectI PopupOpener::_componentGeo(const GeoComponent * pComponent) const
	{
		RectI	contentRect = m_size;

		if (m_pSkin)
			contentRect = m_pSkin->_contentRect(contentRect, m_state);

		// Get icon and text rect from content rect

		RectI iconRect = m_icon.getIconRect(contentRect);

		if (pComponent == &m_icon)
			return iconRect;

		RectI textRect = m_icon.getTextRect(contentRect, iconRect);
		return textRect;
	}



}	// namespace wg
