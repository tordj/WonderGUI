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

WgButton::WgButton()
{
	m_pText = &m_text;
	m_text.setAlignment( WG_CENTER );
	m_text.setLineWidth(Size().w);					// We start with no textborders...
	m_text.SetAutoEllipsis(IsAutoEllipsisDefault());
	m_text.setHolder(this);

 	m_state				= WG_STATE_NORMAL;

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
		return WgButtonPtr( static_cast<WgButton*>(pObject.GetRealPtr()) );

	return 0;
}


//____ SetIcon() ______________________________________________________________

void WgButton::SetIcon( const WgSkinPtr& pIconGfx )
{
	m_pIconSkin = pIconGfx;
	_iconModified();
}

bool WgButton::SetIcon( const WgSkinPtr& pIconGfx, WgOrigo origo, WgBorders padding, float scale, bool bPushText )
{
	if( scale < 0 || scale > 1.f )
		return false;

	m_pIconSkin = pIconGfx;
	m_iconOrigo = origo;
	m_iconBorders = padding;
	m_iconScale = scale;
	m_bIconPushText = bPushText;

	_iconModified();
	return true;
}




//____ TextAreaWidth() _____________________________________________________

int WgButton::TextAreaWidth()
{
	WgRect	contentRect(0,0,Size());

	if( m_pSkin )
		contentRect = m_pSkin->SizeForContent( contentRect );

	WgRect textRect = _getTextRect( contentRect, _getIconRect( contentRect, m_pIconSkin ) );

	return textRect.w;
}

//____ HeightForWidth() _______________________________________________________

int WgButton::HeightForWidth( int width ) const
{
	int height = 0;

	if( m_pSkin )
		height = m_pSkin->PreferredSize().h;

	if( m_text.nbChars() != 0 )
	{
		WgSize padding;

		if( m_pSkin )
			padding = m_pSkin->ContentPadding();

		int heightForText = m_text.heightForWidth(width-padding.w) + padding.h;
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

	if( m_text.nbChars() != 0 )
		preferred = m_text.unwrappedSize();
	
	if( m_pSkin )
		preferred = m_pSkin->SizeForContent(preferred);

	//TODO: Take icon into account.

	return preferred;
}

//____ _onStateChanged() ______________________________________________________

void WgButton::_onStateChanged( WgState oldState, WgState newState )
{
	WgWidget::_onStateChanged(oldState,newState);

	if(m_pIconSkin && !m_pIconSkin->IsStateIdentical(newState,oldState))
			_requestRender();

	m_text.setState(newState);

	//TODO: Request render if text properties have changed.

}

//____ _onSkinChanged() _______________________________________________________

void WgButton::_onSkinChanged( const WgSkinPtr& pOldSkin, const WgSkinPtr& pNewSkin )
{
	WgWidget::_onSkinChanged(pOldSkin,pNewSkin);
	m_text.SetColorSkin(pNewSkin);
}


//____ _onNewSize() ____________________________________________________________

void WgButton::_onNewSize( const WgSize& size )
{
	WgRect	contentRect(0,0,Size());

	if( m_pSkin )
		contentRect -= m_pSkin->ContentPadding();

	WgRect textRect = _getTextRect( contentRect, _getIconRect( contentRect, m_pIconSkin ) );

	m_text.setLineWidth(textRect.w);
}


//____ _onRender() _____________________________________________________________

void WgButton::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip )
{
	WgWidget::_onRender(pDevice,_canvas,_window,_clip);

	WgRect	contentRect = _canvas;

	if( m_pSkin )
		contentRect = m_pSkin->ContentRect(_canvas, m_state);

	// Get icon and text rect from content rect

	WgRect iconRect = _getIconRect( contentRect, m_pIconSkin );
	WgRect textRect = _getTextRect( contentRect, iconRect );

	// Render icon

	if( m_pIconSkin )
		m_pIconSkin->Render( pDevice, iconRect, m_state, _clip );

	// Print text

 	if( !m_text.IsEmpty() )
		pDevice->PrintText( WgRect(textRect,_clip), &m_text, textRect );
}

//____ _onEvent() ______________________________________________________________

void WgButton::_onEvent( const WgEvent::Event * _pEvent, WgEventHandler * pHandler )
{
	WgState oldState = m_state;

	switch( _pEvent->Type() )
	{
		case WG_EVENT_KEY_PRESS:
			if( static_cast<const WgEvent::KeyPress*>(_pEvent)->TranslatedKeyCode() == WG_KEY_RETURN )
			{
				m_bReturnPressed = true;
				_pEvent->Swallow();
			}
			break;

		case WG_EVENT_KEY_REPEAT:
			if( static_cast<const WgEvent::KeyPress*>(_pEvent)->TranslatedKeyCode() == WG_KEY_RETURN )
				_pEvent->Swallow();
			break;

		case WG_EVENT_KEY_RELEASE:
			if( static_cast< const WgEvent::KeyPress*>(_pEvent)->TranslatedKeyCode() == WG_KEY_RETURN )
			{
				m_bReturnPressed = false;
				pHandler->QueueEvent( new WgEvent::ButtonPress(this) );
				_pEvent->Swallow();
			}
			break;
	
		case WG_EVENT_MOUSE_ENTER:
			m_state.SetHovered(true);
			break;
		case WG_EVENT_MOUSE_LEAVE:
			m_state.SetHovered(false);
			break;
		case WG_EVENT_MOUSEBUTTON_PRESS:
			if( static_cast<const WgEvent::MouseButtonPress*>(_pEvent)->Button() == 1 )
			{
				m_bPressed = true;
				_pEvent->Swallow();
			}
			break;
		case WG_EVENT_MOUSEBUTTON_RELEASE:
			if( static_cast<const WgEvent::MouseButtonRelease*>(_pEvent)->Button() == 1 )
			{
				m_bPressed = false;
				_pEvent->Swallow();
			}
			break;
		case WG_EVENT_MOUSEBUTTON_CLICK:
			if( static_cast<const WgEvent::MouseButtonClick*>(_pEvent)->Button() == 1 )
			{
				pHandler->QueueEvent( new WgEvent::ButtonPress(this) );
				_pEvent->Swallow();
			}
			break;
		case WG_EVENT_MOUSEBUTTON_DOUBLE_CLICK:
		case WG_EVENT_MOUSEBUTTON_REPEAT:
		case WG_EVENT_MOUSEBUTTON_DRAG:
			if( static_cast<const WgEvent::MouseButtonEvent*>(_pEvent)->Button() == 1 )
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
		_onStateChanged(oldState,m_state);
}


//____ _onRefresh() ____________________________________________________________

void WgButton::_onRefresh( void )
{
	WgWidget::_onRefresh();

	//TODO: Handling of icon and text.
}

//____ SetDownWhenMouseOutside() _______________________________________________

void WgButton::SetDownWhenMouseOutside( bool bDown )
{
		m_bDownOutside		= bDown;
}

//____ _onCloneContent() _______________________________________________________

void WgButton::_onCloneContent( const WgWidget * _pOrg )
{
	WgButton * pOrg = (WgButton *) _pOrg;

	pOrg->Wg_Interface_TextHolder::_onCloneContent( this );
	WgIconHolder::_onCloneContent( pOrg );

	m_text.setText(&pOrg->m_text);

	m_pIconSkin		= pOrg->m_pIconSkin;
	m_bDownOutside	= pOrg->m_bDownOutside;
}

//____ _onAlphaTest() ___________________________________________________________

bool WgButton::_onAlphaTest( const WgCoord& ofs )
{
	if( m_pIconSkin )
	{
		//TODO: Test against icon.
	}

	return WgWidget::_onAlphaTest(ofs);
}

//____ _textModified() __________________________________________________________

void WgButton::_textModified()
{
	//TODO: Should possibly refresh size too.
	_requestRender();
}

//____ _iconModified() __________________________________________________________

void WgButton::_iconModified()
{
	//TODO: Should possibly refresh size too.
	_requestRender();
}
