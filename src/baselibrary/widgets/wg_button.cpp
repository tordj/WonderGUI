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



#include <wg_button.h>
#include <wg_gfxdevice.h>
#include <wg_util.h>
#include <wg_key.h>
#include <wg_msg.h>
#include <wg_msgrouter.h>
#include <wg_internal.h>

namespace wg
{

	const TypeInfo Button::TYPEINFO = { "Button", &Widget::TYPEINFO };

	//____ constructor ____________________________________________________________

	Button::Button() : icon(this), label(this)
	{
		m_bPressed 		 = false;
		m_bReturnPressed = false;
	}

	//____ Destructor _____________________________________________________________

	Button::~Button()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& Button::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ _matchingHeight() _______________________________________________________

	spx Button::_matchingHeight( spx width, int scale ) const
	{
		scale = _fixScale(scale);

		spx height = m_skin.defaultSize(scale).h;

		if( !OO(label).isEmpty() )
		{
			SizeSPX padding = m_skin.contentPaddingSize(scale);

			spx heightForText = OO(label)._matchingHeight(width-padding.w,scale) + padding.h;
			if( heightForText > height )
				height = heightForText;
		}

		//TODO: Take icon into account.

		return height;
	}


	//____ _defaultSize() _____________________________________________________________

	SizeSPX Button::_defaultSize(int scale) const
	{
		scale = _fixScale(scale);

		SizeSPX defaultSize;

		if( !OO(label).isEmpty() )
			defaultSize = OO(label)._defaultSize(scale);

		defaultSize = m_skin.sizeForContent(defaultSize, scale);

		//TODO: Take icon into account.

		return defaultSize;
	}

	//____ _setState() ______________________________________________________

	void Button::_setState( State state )
	{
		if (!_icon().isEmpty())
		{
			//TODO: Remove once icon uses CSkinSlot.
			_requestRender();
		}

		OO(label)._setState(state);
		Widget::_setState(state);
	}

	//____ _resize() ____________________________________________________________

	void Button::_resize( const SizeSPX& _size, int scale )
	{
		Widget::_resize(_size, scale);

		RectSPX	contentRect = m_skin.contentRect(_size,m_scale,m_state);

		RectSPX textRect = _icon()._getTextRect( contentRect, _icon()._getIconRect( contentRect, m_scale ), m_scale );

		OO(label)._setSize( textRect, m_scale );
	}

	//____ _render() _____________________________________________________________

	void Button::_render( GfxDevice * pDevice, const RectSPX& _canvas, const RectSPX& _window )
	{
		Widget::_render(pDevice,_canvas,_window);

		RectSPX	contentRect = m_skin.contentRect(_canvas, m_scale, m_state);

		// Get icon and text rect from content rect

		RectSPX iconRect = _icon()._getIconRect( contentRect, m_scale );
		RectSPX textRect = _icon()._getTextRect( contentRect, iconRect, m_scale );

		// Render icon

		if( _icon().skin() )
			_icon().skin()->_render( pDevice, iconRect, m_scale, m_state );

		// Print text

	 	if( !OO(label).isEmpty() )
			OO(label)._render( pDevice, textRect );
	}

	//____ _receive() ______________________________________________________________

	void Button::_receive( Msg * _pMsg )
	{
		State state = m_state;
		MsgRouter_p	pRouter = Base::msgRouter();

		switch( _pMsg->type() )
		{
			case MsgType::KeyPress:
				if( static_cast<KeyPressMsg*>(_pMsg)->translatedKeyCode() == Key::Return )
				{
					m_bReturnPressed = true;
					_pMsg->swallow();
				}
				break;

			case MsgType::KeyRepeat:
				if( static_cast<KeyRepeatMsg*>(_pMsg)->translatedKeyCode() == Key::Return )
					_pMsg->swallow();
				break;

			case MsgType::KeyRelease:
				if( static_cast<KeyReleaseMsg*>(_pMsg)->translatedKeyCode() == Key::Return )
				{
					m_bReturnPressed = false;
					pRouter->post( SelectMsg::create(this) );
					_pMsg->swallow();
				}
				break;

			case MsgType::MouseEnter:
				state.setHovered(true);
				break;
			case MsgType::MouseLeave:
				state.setHovered(false);
				break;
			case MsgType::MousePress:
				if( static_cast<MousePressMsg*>(_pMsg)->button() == MouseButton::Left )
				{
					m_bPressed = true;
					_pMsg->swallow();
				}
				break;
			case MsgType::MouseRelease:
				if( static_cast<MouseReleaseMsg*>(_pMsg)->button() == MouseButton::Left )
				{
					m_bPressed = false;
					_pMsg->swallow();
				}
				break;
			case MsgType::MouseClick:
				if( static_cast<MouseClickMsg*>(_pMsg)->button() == MouseButton::Left )
				{
					pRouter->post( SelectMsg::create(this) );
					_pMsg->swallow();
				}
				break;
			case MsgType::MouseDoubleClick:
			case MsgType::MouseRepeat:
			case MsgType::MouseDrag:
				if( static_cast<MouseButtonMsg*>(_pMsg)->button() == MouseButton::Left )
					_pMsg->swallow();
				break;

			case MsgType::FocusGained:
				state.setFocused(true);
				break;
			case MsgType::FocusLost:
				state.setFocused(false);
				m_bReturnPressed = false;
				m_bPressed = false;
				break;

			default:
				break;
		}

		state.setPressed(m_bReturnPressed || m_bPressed);

		if( state != m_state )
			_setState(state);
	}

	//____ _refresh() ____________________________________________________________

	void Button::_refresh( void )
	{
		Widget::_refresh();
		OO(label)._refresh();

		//TODO: Handling of icon and text.
	}

	//____ _alphaTest() ___________________________________________________________

	bool Button::_alphaTest( const CoordSPX& ofs )
	{
		if( _icon().skin() )
		{
			//TODO: Test against icon.
		}

		return Widget::_alphaTest(ofs);
	}

	//____ _componentPos() ______________________________________________________________

	CoordSPX Button::_componentPos( const WidgetComponent * pComponent ) const
	{
		RectSPX contentRect = m_skin.contentRect(contentRect, m_scale, m_state);

		// Get icon and text rect from content rect

		RectSPX iconRect = _icon()._getIconRect( contentRect, m_scale );

		if( pComponent == &icon )
			return iconRect.pos();

		RectSPX textRect = _icon()._getTextRect( contentRect, iconRect, m_scale );
		return textRect.pos();
	}

	//____ _componentSize() ______________________________________________________________

	SizeSPX Button::_componentSize( const WidgetComponent * pComponent ) const
	{
		SizeSPX	sz = m_size - m_skin.contentPaddingSize(m_scale);

		RectSPX iconRect = _icon()._getIconRect( sz, m_scale );

		if( pComponent == &icon )
			return iconRect.size();

		RectSPX textRect = _icon()._getTextRect( sz, iconRect, m_scale );
		return textRect.size();

	}

	//____ _componentGeo() ______________________________________________________________

	RectSPX Button::_componentGeo( const WidgetComponent * pComponent ) const
	{
		RectSPX	contentRect = m_skin.contentRect(m_size, m_scale, m_state);

		// Get icon and text rect from content rect

		RectSPX iconRect = _icon()._getIconRect( contentRect, m_scale );

		if( pComponent == &icon )
			return iconRect;

		RectSPX textRect = _icon()._getTextRect( contentRect, iconRect, m_scale );
		return textRect;
	}

} // namespace wg
