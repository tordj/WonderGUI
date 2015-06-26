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
#include <wg_event.h>
#include <wg_eventhandler.h>

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

//____ IsInstanceOf() _________________________________________________________

bool WgButton::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgWidget::IsInstanceOf(pClassName);
}

//____ ClassName() ____________________________________________________________

const char * WgButton::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ Cast() _________________________________________________________________

WgButtonPtr WgButton::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgButtonPtr( static_cast<WgButton*>(pObject.RawPtr()) );

	return 0;
}


//____ MatchingHeight() _______________________________________________________

int WgButton::MatchingHeight( int width ) const
{
	int height = 0;

	if( m_pSkin )
		height = m_pSkin->PreferredSize().h;

	if( !m_text.IsEmpty() )
	{
		WgSize padding;

		if( m_pSkin )
			padding = m_pSkin->ContentPadding();

		int heightForText = m_text.MatchingHeight(width-padding.w) + padding.h;
		if( heightForText > height )
			height = heightForText;
	}

	//TODO: Take icon into account.

	return height;
}


//____ PreferredSize() _____________________________________________________________

WgSize WgButton::PreferredSize() const
{
	WgSize preferred;

	if( !m_text.IsEmpty() )
		preferred = m_text.PreferredSize();
	
	if( m_pSkin )
		preferred = m_pSkin->SizeForContent(preferred);

	//TODO: Take icon into account.

	return preferred;
}

//____ _onStateChanged() ______________________________________________________

void WgButton::_onStateChanged( WgState oldState )
{
	WgWidget::_onStateChanged(oldState);

	if(m_icon.Skin() && !m_icon.Skin()->IsStateIdentical(m_state,oldState))
			_requestRender();

	m_text.SetState(m_state);
}

//____ _onSkinChanged() _______________________________________________________

void WgButton::_onSkinChanged( const WgSkinPtr& pOldSkin, const WgSkinPtr& pNewSkin )
{
	WgWidget::_onSkinChanged(pOldSkin,pNewSkin);
}


//____ _onNewSize() ____________________________________________________________

void WgButton::_onNewSize( const WgSize& size )
{
	WgRect	contentRect(0,0,Size());

	if( m_pSkin )
		contentRect -= m_pSkin->ContentPadding();

	WgRect textRect = m_icon.GetTextRect( contentRect, m_icon.GetIconRect( contentRect ) );

	m_text.OnNewSize( textRect );
}


//____ _onRender() _____________________________________________________________

void WgButton::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip )
{
	WgWidget::_onRender(pDevice,_canvas,_window,_clip);

	WgRect	contentRect = _canvas;

	if( m_pSkin )
		contentRect = m_pSkin->ContentRect(_canvas, m_state);

	// Get icon and text rect from content rect

	WgRect iconRect = m_icon.GetIconRect( contentRect );
	WgRect textRect = m_icon.GetTextRect( contentRect, iconRect );

	// Render icon

	if( m_icon.Skin() )
		m_icon.Skin()->Render( pDevice, iconRect, m_state, _clip );

	// Print text

 	if( !m_text.IsEmpty() )
		m_text.OnRender( pDevice, textRect, _clip );
}

//____ _onEvent() ______________________________________________________________

void WgButton::_onEvent( const WgEventPtr& _pEvent )
{
	WgState oldState = m_state;
	WgEventHandlerPtr	pHandler = WgBase::MsgRouter();

	switch( _pEvent->Type() )
	{
		case WG_EVENT_KEY_PRESS:
			if( WgKeyPressEvent::Cast(_pEvent)->TranslatedKeyCode() == WG_KEY_RETURN )
			{
				m_bReturnPressed = true;
				_pEvent->Swallow();
			}
			break;

		case WG_EVENT_KEY_REPEAT:
			if( WgKeyRepeatEvent::Cast(_pEvent)->TranslatedKeyCode() == WG_KEY_RETURN )
				_pEvent->Swallow();
			break;

		case WG_EVENT_KEY_RELEASE:
			if( WgKeyReleaseEvent::Cast(_pEvent)->TranslatedKeyCode() == WG_KEY_RETURN )
			{
				m_bReturnPressed = false;
				pHandler->QueueEvent( new WgSelectEvent(this) );
				_pEvent->Swallow();
			}
			break;
	
		case WG_EVENT_MOUSE_ENTER:
			m_state.SetHovered(true);
			break;
		case WG_EVENT_MOUSE_LEAVE:
			m_state.SetHovered(false);
			break;
		case WG_EVENT_MOUSE_PRESS:
			if( WgMousePressEvent::Cast(_pEvent)->Button() == WG_BUTTON_LEFT )
			{
				m_bPressed = true;
				_pEvent->Swallow();
			}
			break;
		case WG_EVENT_MOUSE_RELEASE:
			if( WgMouseReleaseEvent::Cast(_pEvent)->Button() == WG_BUTTON_LEFT )
			{
				m_bPressed = false;
				_pEvent->Swallow();
			}
			break;
		case WG_EVENT_MOUSE_CLICK:
			if( WgMouseClickEvent::Cast(_pEvent)->Button() == WG_BUTTON_LEFT )
			{
				pHandler->QueueEvent( new WgSelectEvent(this) );
				_pEvent->Swallow();
			}
			break;
		case WG_EVENT_MOUSE_DOUBLE_CLICK:
		case WG_EVENT_MOUSE_REPEAT:
		case WG_EVENT_MOUSE_DRAG:
			if( WgMouseButtonEvent::Cast(_pEvent)->Button() ==WG_BUTTON_LEFT )
				_pEvent->Swallow();
			break;

		case WG_EVENT_FOCUS_GAINED:
			m_state.SetFocused(true);
			break;
		case WG_EVENT_FOCUS_LOST:
			m_state.SetFocused(false);
			m_bReturnPressed = false;
			m_bPressed = false;
			break;
	}


	if( m_bReturnPressed || (m_bPressed && (m_bDownOutside || m_state.IsHovered() )) )
		m_state.SetPressed(true);
	else
		m_state.SetPressed(false);

	if( m_state != oldState )
		_onStateChanged(oldState);
}


//____ _onRefresh() ____________________________________________________________

void WgButton::_onRefresh( void )
{
	WgWidget::_onRefresh();
	m_text.OnRefresh();

	//TODO: Handling of icon and text.
}

//____ SetDownWhenMouseOutside() _______________________________________________
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
 * The effect is only cosmetic, the way that events are fired is not affected.
 *
 * The default setting is false.
 *
 * This little odd setting is useful when using buttons as click-areas for resizing or
 * dragging widgets around on the screen. The button will then not switch between
 * pressed and unpressed states as the pointer momentarily moves outside the button
 * before the callback that updates the geometry is called.
 *
 **/
void WgButton::SetDownWhenMouseOutside( bool bDown )
{
		m_bDownOutside		= bDown;
}

//____ _onCloneContent() _______________________________________________________

void WgButton::_onCloneContent( const WgWidget * _pOrg )
{
	WgButton * pOrg = (WgButton *) _pOrg;

	m_icon.OnCloneContent( &pOrg->m_icon );

	//TODO: Support cloning for text items.
//	m_text.clone(&pOrg->m_text);
	
	m_bDownOutside	= pOrg->m_bDownOutside;
}

//____ _onAlphaTest() ___________________________________________________________

bool WgButton::_onAlphaTest( const WgCoord& ofs, const WgSize& sz )
{
	if( m_icon.Skin() )
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
