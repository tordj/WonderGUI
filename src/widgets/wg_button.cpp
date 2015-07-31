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
	
	const char WgButton::CLASSNAME[] = {"Button"};
	
	//____ Constructor ____________________________________________________________
	
	WgButton::WgButton() : m_text(this), m_icon(this), label(&m_text), icon(&m_icon)
	{
		m_bDownOutside	 = false;
		m_bPressed 		 = false;
		m_bReturnPressed = false;
	}
	
	//____ Destructor _____________________________________________________________
	
	WgButton::~WgButton()
	{
	}
	
	//____ isInstanceOf() _________________________________________________________
	
	bool WgButton::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return WgWidget::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * WgButton::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	WgButton_p WgButton::cast( const WgObject_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return WgButton_p( static_cast<WgButton*>(pObject.rawPtr()) );
	
		return 0;
	}
	
	
	//____ matchingHeight() _______________________________________________________
	
	int WgButton::matchingHeight( int width ) const
	{
		int height = 0;
	
		if( m_pSkin )
			height = m_pSkin->preferredSize().h;
	
		if( !m_text.isEmpty() )
		{
			WgSize padding;
	
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
	
	WgSize WgButton::preferredSize() const
	{
		WgSize preferred;
	
		if( !m_text.isEmpty() )
			preferred = m_text.preferredSize();
		
		if( m_pSkin )
			preferred = m_pSkin->sizeForContent(preferred);
	
		//TODO: Take icon into account.
	
		return preferred;
	}
	
	//____ _onStateChanged() ______________________________________________________
	
	void WgButton::_onStateChanged( WgState oldState )
	{
		WgWidget::_onStateChanged(oldState);
	
		if(m_icon.skin() && !m_icon.skin()->isStateIdentical(m_state,oldState))
				_requestRender();
	
		m_text.setState(m_state);
	}
	
	//____ _onSkinChanged() _______________________________________________________
	
	void WgButton::_onSkinChanged( const WgSkin_p& pOldSkin, const WgSkin_p& pNewSkin )
	{
		WgWidget::_onSkinChanged(pOldSkin,pNewSkin);
	}
	
	
	//____ _onNewSize() ____________________________________________________________
	
	void WgButton::_onNewSize( const WgSize& _size )
	{
		WgRect	contentRect(0,0,_size);
	
		if( m_pSkin )
			contentRect -= m_pSkin->contentPadding();
	
		WgRect textRect = m_icon.getTextRect( contentRect, m_icon.getIconRect( contentRect ) );
	
		m_text.onNewSize( textRect );
	}
	
	
	//____ _onRender() _____________________________________________________________
	
	void WgButton::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip )
	{
		WgWidget::_onRender(pDevice,_canvas,_window,_clip);
	
		WgRect	contentRect = _canvas;
	
		if( m_pSkin )
			contentRect = m_pSkin->contentRect(_canvas, m_state);
	
		// Get icon and text rect from content rect
	
		WgRect iconRect = m_icon.getIconRect( contentRect );
		WgRect textRect = m_icon.getTextRect( contentRect, iconRect );
	
		// Render icon
	
		if( m_icon.skin() )
			m_icon.skin()->render( pDevice, iconRect, m_state, _clip );
	
		// Print text
	
	 	if( !m_text.isEmpty() )
			m_text.onRender( pDevice, textRect, _clip );
	}
	
	//____ _onMsg() ______________________________________________________________
	
	void WgButton::_onMsg( const WgMsg_p& _pMsg )
	{
		WgState oldState = m_state;
		WgMsgRouter_p	pHandler = WgBase::msgRouter();
	
		switch( _pMsg->type() )
		{
			case WG_MSG_KEY_PRESS:
				if( WgKeyPressMsg::cast(_pMsg)->translatedKeyCode() == WG_KEY_RETURN )
				{
					m_bReturnPressed = true;
					_pMsg->swallow();
				}
				break;
	
			case WG_MSG_KEY_REPEAT:
				if( WgKeyRepeatMsg::cast(_pMsg)->translatedKeyCode() == WG_KEY_RETURN )
					_pMsg->swallow();
				break;
	
			case WG_MSG_KEY_RELEASE:
				if( WgKeyReleaseMsg::cast(_pMsg)->translatedKeyCode() == WG_KEY_RETURN )
				{
					m_bReturnPressed = false;
					pHandler->post( new WgSelectMsg(this) );
					_pMsg->swallow();
				}
				break;
		
			case WG_MSG_MOUSE_ENTER:
				m_state.setHovered(true);
				break;
			case WG_MSG_MOUSE_LEAVE:
				m_state.setHovered(false);
				break;
			case WG_MSG_MOUSE_PRESS:
				if( WgMousePressMsg::cast(_pMsg)->button() == WG_BUTTON_LEFT )
				{
					m_bPressed = true;
					_pMsg->swallow();
				}
				break;
			case WG_MSG_MOUSE_RELEASE:
				if( WgMouseReleaseMsg::cast(_pMsg)->button() == WG_BUTTON_LEFT )
				{
					m_bPressed = false;
					_pMsg->swallow();
				}
				break;
			case WG_MSG_MOUSE_CLICK:
				if( WgMouseClickMsg::cast(_pMsg)->button() == WG_BUTTON_LEFT )
				{
					pHandler->post( new WgSelectMsg(this) );
					_pMsg->swallow();
				}
				break;
			case WG_MSG_MOUSE_DOUBLE_CLICK:
			case WG_MSG_MOUSE_REPEAT:
			case WG_MSG_MOUSE_DRAG:
				if( WgMouseButtonMsg::cast(_pMsg)->button() ==WG_BUTTON_LEFT )
					_pMsg->swallow();
				break;
	
			case WG_MSG_FOCUS_GAINED:
				m_state.setFocused(true);
				break;
			case WG_MSG_FOCUS_LOST:
				m_state.setFocused(false);
				m_bReturnPressed = false;
				m_bPressed = false;
				break;
		}
	
	
		if( m_bReturnPressed || (m_bPressed && (m_bDownOutside || m_state.isHovered() )) )
			m_state.setPressed(true);
		else
			m_state.setPressed(false);
	
		if( m_state != oldState )
			_onStateChanged(oldState);
	}
	
	
	//____ _onRefresh() ____________________________________________________________
	
	void WgButton::_onRefresh( void )
	{
		WgWidget::_onRefresh();
		m_text.onRefresh();
	
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
	void WgButton::setDownWhenMouseOutside( bool bDown )
	{
			m_bDownOutside		= bDown;
	}
	
	//____ _onCloneContent() _______________________________________________________
	
	void WgButton::_onCloneContent( const WgWidget * _pOrg )
	{
		WgButton * pOrg = (WgButton *) _pOrg;
	
		m_icon.onCloneContent( &pOrg->m_icon );
	
		//TODO: Support cloning for text items.
	//	m_text.clone(&pOrg->m_text);
		
		m_bDownOutside	= pOrg->m_bDownOutside;
	}
	
	//____ _onAlphaTest() ___________________________________________________________
	
	bool WgButton::_onAlphaTest( const WgCoord& ofs, const WgSize& sz )
	{
		if( m_icon.skin() )
		{
			//TODO: Test against icon.
		}
	
		return WgWidget::_onAlphaTest(ofs,sz);
	}
	
	//____ _onFieldDirty() _________________________________________________________
	
	void WgButton::_onFieldDirty( WgField * pField )
	{
		_requestRender();
	}
	
	//____ _onFieldResize() ________________________________________________________
	
	void WgButton::_onFieldResize( WgField * pField )
	{
		_requestResize();
		_requestRender();
	}

} // namespace wg
