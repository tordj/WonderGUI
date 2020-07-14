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

	Button::Button() : icon(this), text(this)
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

	//____ matchingHeight() _______________________________________________________

	MU Button::matchingHeight( MU width ) const
	{
		MU height = 0;

		if( m_pSkin )
			height = m_pSkin->preferredSize().h;

		if( !OO(text).isEmpty() )
		{
			Size padding;

			if( m_pSkin )
				padding = m_pSkin->contentPaddingSize();

			MU heightForText = OO(text)._matchingHeight(width-padding.w) + padding.h;
			if( heightForText > height )
				height = heightForText;
		}

		//TODO: Take icon into account.

		return height;
	}


	//____ preferredSize() _____________________________________________________________

	Size Button::preferredSize() const
	{
		Size preferred;

		if( !OO(text).isEmpty() )
			preferred = OO(text)._preferredSize();

		if( m_pSkin )
			preferred = m_pSkin->sizeForContent(preferred);

		//TODO: Take icon into account.

		return preferred;
	}

	//____ _setState() ______________________________________________________

	void Button::_setState( State state )
	{
		if(_icon().skin() && !_icon().skin()->isStateIdentical(state,m_state))
				_requestRender();

		OO(text)._setState(state);
		Widget::_setState(state);
	}

	//____ _resize() ____________________________________________________________

	void Button::_resize( const Size& _size )
	{
		Widget::_resize(_size);

		Rect	contentRect(0,0,_size);

		if( m_pSkin )
			contentRect -= m_pSkin->contentPaddingSize();

		Rect textRect = _icon()._getTextRect( contentRect, _icon()._getIconRect( contentRect ) );

		OO(text)._setSize( textRect );
	}


	//____ _render() _____________________________________________________________

	void Button::_render( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window )
	{
		Widget::_render(pDevice,_canvas,_window);

		Rect	contentRect = _canvas;

		if( m_pSkin )
			contentRect = m_pSkin->contentRect(_canvas, m_state);

		// Get icon and text rect from content rect

		Rect iconRect = _icon()._getIconRect( contentRect );
		Rect textRect = _icon()._getTextRect( contentRect, iconRect );

		// Render icon

		if( _icon().skin() )
			_icon().skin()->render( pDevice, iconRect, m_state );

		// Print text

	 	if( !OO(text).isEmpty() )
			OO(text)._render( pDevice, textRect );
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
		OO(text)._refresh();

		//TODO: Handling of icon and text.
	}


	//____ _cloneContent() _______________________________________________________

	void Button::_cloneContent( const Widget * _pOrg )
	{
		Widget::_cloneContent( _pOrg );

//		Button * pOrg = (Button *) _pOrg;

//		_icon().onCloneContent( &pOrg->m_icon );

		//TODO: Support cloning for text components.
	//	OO(text).clone(&pOrg->m_text);

	}

	//____ _alphaTest() ___________________________________________________________

	bool Button::_alphaTest( const Coord& ofs )
	{
		if( _icon().skin() )
		{
			//TODO: Test against icon.
		}

		return Widget::_alphaTest(ofs);
	}

	//____ _componentPos() ______________________________________________________________

	Coord Button::_componentPos( const GeoComponent * pComponent ) const
	{
		Rect	contentRect = m_size;

		if( m_pSkin )
			contentRect = m_pSkin->contentRect(contentRect, m_state);

		// Get icon and text rect from content rect

		Rect iconRect = _icon()._getIconRect( contentRect );

		if( pComponent == &icon )
			return iconRect.pos();

		Rect textRect = _icon()._getTextRect( contentRect, iconRect );
		return textRect.pos();
	}

	//____ _componentSize() ______________________________________________________________

	Size Button::_componentSize( const GeoComponent * pComponent ) const
	{
		Size	sz = m_size;

		if( m_pSkin )
			sz -= m_pSkin->contentPaddingSize();

		Rect iconRect = _icon()._getIconRect( sz );

		if( pComponent == &icon )
			return iconRect.size();

		Rect textRect = _icon()._getTextRect( sz, iconRect );
		return textRect.size();

	}

	//____ _componentGeo() ______________________________________________________________

	Rect Button::_componentGeo( const GeoComponent * pComponent ) const
	{
		Rect	contentRect = m_size;

		if( m_pSkin )
			contentRect = m_pSkin->contentRect(contentRect, m_state);

		// Get icon and text rect from content rect

		Rect iconRect = _icon()._getIconRect( contentRect );

		if( pComponent == &icon )
			return iconRect;

		Rect textRect = _icon()._getTextRect( contentRect, iconRect );
		return textRect;
	}


} // namespace wg
