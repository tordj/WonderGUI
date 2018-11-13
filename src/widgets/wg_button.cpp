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
	
	Button::Button() : m_text(this), m_icon(this), label(&m_text), icon(&m_icon)
	{
		m_bDownOutside	 = false;
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
	
	
	//____ matchingHeight() _______________________________________________________
	
	int Button::matchingHeight( int width ) const
	{
		int height = 0;
	
		if( m_pSkin )
			height = m_pSkin->preferredSize().h;
	
		if( !m_text.isEmpty() )
		{
			Size padding;
	
			if( m_pSkin )
				padding = m_pSkin->contentPadding();
	
			int heightForText = m_text.matchingHeight(width-padding.w) + padding.h;
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
	
		if( !m_text.isEmpty() )
			preferred = m_text.preferredSize();
		
		if( m_pSkin )
			preferred = m_pSkin->sizeForContent(preferred);
	
		//TODO: Take icon into account.
	
		return preferred;
	}
	
	//____ _setState() ______________________________________________________
	
	void Button::_setState( State state )
	{	
		if(m_icon.skin() && !m_icon.skin()->isStateIdentical(state,m_state))
				_requestRender();
	
		m_text.setState(state);
		Widget::_setState(state);
	}
	
	//____ _setSize() ____________________________________________________________
	
	void Button::_setSize( const Size& _size )
	{
		Widget::_setSize(_size);
		
		Rect	contentRect(0,0,_size);
	
		if( m_pSkin )
			contentRect -= m_pSkin->contentPadding();
	
		Rect textRect = m_icon.getTextRect( contentRect, m_icon.getIconRect( contentRect ) );
	
		m_text.setSize( textRect );
	}
	
	
	//____ _render() _____________________________________________________________
	
	void Button::_render( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, const Rect& _clip )
	{
		Widget::_render(pDevice,_canvas,_window,_clip);
	
		Rect	contentRect = _canvas;
	
		if( m_pSkin )
			contentRect = m_pSkin->contentRect(_canvas, m_state);
	
		// Get icon and text rect from content rect
	
		Rect iconRect = m_icon.getIconRect( contentRect );
		Rect textRect = m_icon.getTextRect( contentRect, iconRect );
	
		// Render icon
	
		if( m_icon.skin() )
			m_icon.skin()->render( pDevice, iconRect, m_state, _clip );
	
		// Print text
	
	 	if( !m_text.isEmpty() )
			m_text.render( pDevice, textRect, _clip );
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
	
	
		if( m_bReturnPressed || (m_bPressed && (m_bDownOutside || state.isHovered() )) )
			state.setPressed(true);
		else
			state.setPressed(false);
	
		if( state != m_state )
			_setState(state);
	}
	
	
	//____ _refresh() ____________________________________________________________
	
	void Button::_refresh( void )
	{
		Widget::_refresh();
		m_text.refresh();
	
		//TODO: Handling of icon and text.
	}
	
	//____ setDownWhenMouseOutside() _______________________________________________
	/**
	 * Set whether the button will stay in pressed state even if the mouse goes outside.
	 *
	 * @param bDown		True if button should stay pressed.
	 *
	 * If set to True and the button is pushed, it will stay in its pressed state until the mouse button
	 * is released no matter how the mouse pointer is moved.
	 *
	 * If set to false, the button will return to its normal state when when the mouse
	 * pointer leaves the widget but go back to the pressed state if mouse pointer re-enters the
	 * widgets without the mouse button having been released.
	 *
	 * The effect is only cosmetic, the way that messages are posted is not affected.
	 *
	 * The default setting is false.
	 *
	 * This little odd setting is useful when using buttons as click-areas for resizing or
	 * dragging widgets around on the screen. The button will then not switch between
	 * pressed and unpressed states as the pointer momentarily moves outside the button
	 * before the callback that updates the geometry is called.
	 *
	 **/
	void Button::setDownWhenMouseOutside( bool bDown )
	{
			m_bDownOutside		= bDown;
	}
	
	//____ _cloneContent() _______________________________________________________
	
	void Button::_cloneContent( const Widget * _pOrg )
	{
		Widget::_cloneContent( _pOrg );
				
		Button * pOrg = (Button *) _pOrg;
	
		m_icon.onCloneContent( &pOrg->m_icon );
	
		//TODO: Support cloning for text items.
	//	m_text.clone(&pOrg->m_text);
		
		m_bDownOutside	= pOrg->m_bDownOutside;
	}
	
	//____ _alphaTest() ___________________________________________________________
	
	bool Button::_alphaTest( const Coord& ofs )
	{
		if( m_icon.skin() )
		{
			//TODO: Test against icon.
		}
	
		return Widget::_alphaTest(ofs);
	}
	
	//____ _renderRequested() _________________________________________________________
	
	void Button::_renderRequested( Item * pItem )
	{
		_requestRender();		//TODO: Only requestRender on item
	}

	void Button::_renderRequested( Item * pItem, const Rect& rect )
	{
		_requestRender();		//TODO: Only requestRender on rect of item.
	}
	
	//____ _resizeRequested() ________________________________________________________
	
	void Button::_resizeRequested( Item * pItem )
	{
		_requestResize();
		_requestRender();
	}

	//____ _itemPos() ______________________________________________________________

	Coord Button::_itemPos( const Item * pItem ) const
	{
		Rect	contentRect = m_size;
	
		if( m_pSkin )
			contentRect = m_pSkin->contentRect(contentRect, m_state);
	
		// Get icon and text rect from content rect
	
		Rect iconRect = m_icon.getIconRect( contentRect );
		
		if( pItem == &m_icon )
			return iconRect.pos();
		
		Rect textRect = m_icon.getTextRect( contentRect, iconRect );
		return textRect.pos();
	}
	
	//____ _itemSize() ______________________________________________________________
	
	Size Button::_itemSize( const Item * pItem ) const
	{
		Size	sz = m_size;

		if( m_pSkin )
			sz -= m_pSkin->contentPadding();

		Rect iconRect = m_icon.getIconRect( sz );

		if( pItem == &m_icon )
			return iconRect.size();
		
		Rect textRect = m_icon.getTextRect( sz, iconRect );
		return textRect.size();

	}
	
	//____ _itemGeo() ______________________________________________________________
	
	Rect Button::_itemGeo( const Item * pItem ) const
	{
		Rect	contentRect = m_size;
	
		if( m_pSkin )
			contentRect = m_pSkin->contentRect(contentRect, m_state);
	
		// Get icon and text rect from content rect
	
		Rect iconRect = m_icon.getIconRect( contentRect );
		
		if( pItem == &m_icon )
			return iconRect;
		
		Rect textRect = m_icon.getTextRect( contentRect, iconRect );
		return textRect;
	}


} // namespace wg
