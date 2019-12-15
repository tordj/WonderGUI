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

namespace wg
{

	const char Button::CLASSNAME[] = {"Button"};

	//____ Constructor ____________________________________________________________

	Button::Button() : icon(this), label(this)
	{
		m_bPressed 		 = false;
		m_bReturnPressed = false;
	}

	//____ Destructor _____________________________________________________________

	Button::~Button()
	{
	}

	//____ isInstanceOf() _________________________________________________________

	bool Button::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return Widget::isInstanceOf(pClassName);
	}

	//____ className() ____________________________________________________________

	const char * Button::className( void ) const
	{
		return CLASSNAME;
	}

	//____ cast() _________________________________________________________________

	Button_p Button::cast( Object * pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return Button_p( static_cast<Button*>(pObject) );

		return 0;
	}


	//____ _matchingHeight() _______________________________________________________

	int Button::_matchingHeight( int width ) const
	{
		int height = 0;

		if( m_pSkin )
			height = m_pSkin->_preferredSize().h;

		if( !_text().isEmpty() )
		{
			SizeI padding;

			if( m_pSkin )
				padding = m_pSkin->_contentPadding();

			int heightForText = _text()._matchingHeight(width-padding.w) + padding.h;
			if( heightForText > height )
				height = heightForText;
		}

		//TODO: Take icon into account.

		return height;
	}


	//____ _preferredSize() _____________________________________________________________

	SizeI Button::_preferredSize() const
	{
		SizeI preferred;

		if( !_text().isEmpty() )
			preferred = _text()._preferredSize();

		if( m_pSkin )
			preferred = m_pSkin->_sizeForContent(preferred);

		//TODO: Take icon into account.

		return preferred;
	}

	//____ _setState() ______________________________________________________

	void Button::_setState( State state )
	{
		if(_icon().skin() && !_icon().skin()->isStateIdentical(state,m_state))
				_requestRender();

		_text()._setState(state);
		Widget::_setState(state);
	}

	//____ _resize() ____________________________________________________________

	void Button::_resize( const SizeI& _size )
	{
		Widget::_resize(_size);

		RectI	contentRect(0,0,_size);

		if( m_pSkin )
			contentRect -= m_pSkin->_contentPadding();

		RectI textRect = _icon()._getTextRect( contentRect, _icon()._getIconRect( contentRect ) );

		_text()._setSize( textRect );
	}


	//____ _render() _____________________________________________________________

	void Button::_render( GfxDevice * pDevice, const RectI& _canvas, const RectI& _window )
	{
		Widget::_render(pDevice,_canvas,_window);

		RectI	contentRect = _canvas;

		if( m_pSkin )
			contentRect = m_pSkin->_contentRect(_canvas, m_state);

		// Get icon and text rect from content rect

		RectI iconRect = _icon()._getIconRect( contentRect );
		RectI textRect = _icon()._getTextRect( contentRect, iconRect );

		// Render icon

		if( _icon().skin() )
			_icon().skin()->_render( pDevice, iconRect, m_state );

		// Print text

	 	if( !_text().isEmpty() )
			_text()._render( pDevice, textRect );
	}

	//____ _receive() ______________________________________________________________

	void Button::_receive( Msg * _pMsg )
	{
		State state = m_state;
		MsgRouter_p	pHandler = Base::msgRouter();

		switch( _pMsg->type() )
		{
			case MsgType::KeyPress:
				if( KeyPressMsg::cast(_pMsg)->translatedKeyCode() == Key::Return )
				{
					m_bReturnPressed = true;
					_pMsg->swallow();
				}
				break;

			case MsgType::KeyRepeat:
				if( KeyRepeatMsg::cast(_pMsg)->translatedKeyCode() == Key::Return )
					_pMsg->swallow();
				break;

			case MsgType::KeyRelease:
				if( KeyReleaseMsg::cast(_pMsg)->translatedKeyCode() == Key::Return )
				{
					m_bReturnPressed = false;
					pHandler->post( SelectMsg::create(this) );
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
				if( MousePressMsg::cast(_pMsg)->button() == MouseButton::Left )
				{
					m_bPressed = true;
					_pMsg->swallow();
				}
				break;
			case MsgType::MouseRelease:
				if( MouseReleaseMsg::cast(_pMsg)->button() == MouseButton::Left )
				{
					m_bPressed = false;
					_pMsg->swallow();
				}
				break;
			case MsgType::MouseClick:
				if( MouseClickMsg::cast(_pMsg)->button() == MouseButton::Left )
				{
					pHandler->post( SelectMsg::create(this) );
					_pMsg->swallow();
				}
				break;
			case MsgType::MouseDoubleClick:
			case MsgType::MouseRepeat:
			case MsgType::MouseDrag:
				if( MouseButtonMsg::cast(_pMsg)->button() == MouseButton::Left )
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
		_text()._refresh();

		//TODO: Handling of icon and text.
	}


	//____ _cloneContent() _______________________________________________________

	void Button::_cloneContent( const Widget * _pOrg )
	{
		Widget::_cloneContent( _pOrg );

		Button * pOrg = (Button *) _pOrg;

//		_icon().onCloneContent( &pOrg->m_icon );

		//TODO: Support cloning for text components.
	//	_text().clone(&pOrg->m_text);

	}

	//____ _alphaTest() ___________________________________________________________

	bool Button::_alphaTest( const CoordI& ofs )
	{
		if( _icon().skin() )
		{
			//TODO: Test against icon.
		}

		return Widget::_alphaTest(ofs);
	}

	//____ _componentPos() ______________________________________________________________

	CoordI Button::_componentPos( const GeoComponent * pComponent ) const
	{
		RectI	contentRect = m_size;

		if( m_pSkin )
			contentRect = m_pSkin->_contentRect(contentRect, m_state);

		// Get icon and text rect from content rect

		RectI iconRect = _icon()._getIconRect( contentRect );

		if( pComponent == &icon )
			return iconRect.pos();

		RectI textRect = _icon()._getTextRect( contentRect, iconRect );
		return textRect.pos();
	}

	//____ _componentSize() ______________________________________________________________

	SizeI Button::_componentSize( const GeoComponent * pComponent ) const
	{
		SizeI	sz = m_size;

		if( m_pSkin )
			sz -= m_pSkin->_contentPadding();

		RectI iconRect = _icon()._getIconRect( sz );

		if( pComponent == &icon )
			return iconRect.size();

		RectI textRect = _icon()._getTextRect( sz, iconRect );
		return textRect.size();

	}

	//____ _componentGeo() ______________________________________________________________

	RectI Button::_componentGeo( const GeoComponent * pComponent ) const
	{
		RectI	contentRect = m_size;

		if( m_pSkin )
			contentRect = m_pSkin->_contentRect(contentRect, m_state);

		// Get icon and text rect from content rect

		RectI iconRect = _icon()._getIconRect( contentRect );

		if( pComponent == &icon )
			return iconRect;

		RectI textRect = _icon()._getTextRect( contentRect, iconRect );
		return textRect;
	}


} // namespace wg
