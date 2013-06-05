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

static const char	c_widgetType[] = {"Button"};

//____ Constructor ____________________________________________________________

WgButton::WgButton()
{
	m_pText = &m_text;
	m_text.setAlignment( WG_CENTER );
	m_text.setLineWidth(Size().w);					// We start with no textborders...
	m_text.SetAutoEllipsis(IsAutoEllipsisDefault());

 	m_state				= WG_STATE_NORMAL;

	m_bDownOutside	 = false;
	m_bPressed 		 = false;
	m_bReturnPressed = false;
	m_bPointerInside = false;
}

//____ Destructor _____________________________________________________________

WgButton::~WgButton()
{
}

//____ Type() _________________________________________________________________

const char * WgButton::Type( void ) const
{
	return GetClass();
}

//____ GetClass() ____________________________________________________________

const char * WgButton::GetClass()
{
	return c_widgetType;
}

//____ SetSkin() ____________________________________________________________

bool WgButton::SetSkin( const WgSkinPtr& pSkin )
{
	m_pSkin = pSkin;

	if( pSkin && pSkin->IsOpaque() )
		m_bOpaque = true;
	else
		m_bOpaque = false;

	_requestRender();
	return true;
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

	if( m_pSkin )
		preferred = m_pSkin->PreferredSize();

	if( m_text.nbChars() != 0 )
	{
		WgSize textSize = m_text.unwrappedSize();

		if( m_pSkin )
			textSize += m_pSkin->ContentPadding();

		if( textSize.w > preferred.w )
			preferred.w = textSize.w;

		if( textSize.h > preferred.h )
			preferred.h = textSize.h;
	}

	//TODO: Take icon into account.

	return preferred;
}


//____ _onEnable() _____________________________________________________________

void WgButton::_onEnable()
{
	m_state = WG_STATE_NORMAL;
	_requestRender();
}

//____ _onDisable() ____________________________________________________________

void WgButton::_onDisable()
{
	m_state = WG_STATE_DISABLED;
	_requestRender();
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
	WgRect	contentRect = _canvas;

	if( m_pSkin )
	{
		m_pSkin->Render( pDevice, _canvas, m_state, _clip );
		contentRect = m_pSkin->ContentRect(_canvas, m_state);
	}

	// Get icon and text rect from content rect

	WgRect iconRect = _getIconRect( contentRect, m_pIconSkin );
	WgRect textRect = _getTextRect( contentRect, iconRect );

	// Render icon

	if( m_pIconSkin )
		m_pIconSkin->Render( pDevice, iconRect, m_state, _clip );

	// Print text

 	if( !m_text.IsEmpty() )
	{
		m_text.setState(m_state);

		if( m_pSkin )
			m_text.SetColorSkin( m_pSkin );

		WgRect clip(textRect,_clip);
		pDevice->PrintText( clip, &m_text, textRect );
	}
}

//____ _onEvent() ______________________________________________________________

void WgButton::_onEvent( const WgEvent::Event * pEvent, WgEventHandler * pHandler )
{
	switch( pEvent->Type() )
	{
		case	WG_EVENT_KEY_PRESS:
		{
			if( static_cast<const WgEvent::KeyPress*>(pEvent)->TranslatedKeyCode() == WG_KEY_RETURN )
				m_bReturnPressed = true;
			else
				pHandler->ForwardEvent( pEvent );
			break;
		}

		case	WG_EVENT_KEY_REPEAT:
		{
			if( static_cast<const WgEvent::KeyPress*>(pEvent)->TranslatedKeyCode() == WG_KEY_RETURN )
			{
			}
			else
				pHandler->ForwardEvent( pEvent );
			break;
		}


		case	WG_EVENT_KEY_RELEASE:
		{
			if( static_cast< const WgEvent::KeyPress*>(pEvent)->TranslatedKeyCode() == WG_KEY_RETURN )
			{
				m_bReturnPressed = false;
				pHandler->QueueEvent( new WgEvent::ButtonPress(this) );
			}
			else
				pHandler->ForwardEvent( pEvent );
			break;
		}

		case	WG_EVENT_MOUSE_ENTER:
			m_bPointerInside = true;
			break;

		case	WG_EVENT_MOUSE_LEAVE:
			m_bPointerInside = false;
			break;

		case WG_EVENT_MOUSEBUTTON_PRESS:
		{
			int button = static_cast<const WgEvent::MouseButtonPress*>(pEvent)->Button();
			if( button == 1 )
				m_bPressed = true;
			else
				pHandler->ForwardEvent( pEvent );
			break;
		}
		case WG_EVENT_MOUSEBUTTON_RELEASE:
		{
			int button = static_cast<const WgEvent::MouseButtonRelease*>(pEvent)->Button();
			if( button == 1 )
				m_bPressed = false;
			else
				pHandler->ForwardEvent( pEvent );
			break;
		}

		case WG_EVENT_MOUSEBUTTON_CLICK:
		{
			int button = static_cast<const WgEvent::MouseButtonClick*>(pEvent)->Button();
			if( button == 1 )
				pHandler->QueueEvent( new WgEvent::ButtonPress(this) );
			else
				pHandler->ForwardEvent( pEvent );
			break;
		}

		case WG_EVENT_MOUSEBUTTON_DOUBLE_CLICK:
		case WG_EVENT_MOUSEBUTTON_REPEAT:
		case WG_EVENT_MOUSEBUTTON_DRAG:
		{
			int button = static_cast<const WgEvent::MouseButtonEvent*>(pEvent)->Button();
			if( button != 1 )
				pHandler->ForwardEvent( pEvent );
			break;
		}


        default:
			pHandler->ForwardEvent( pEvent );
            break;

	}

	WgState newState = _getRenderState();
	if( newState != m_state )
	{
		WgState oldState = m_state;
		m_state = newState;

		if( (m_pSkin && !m_pSkin->IsStateIdentical(newState,oldState)) ||
			(m_pIconSkin && !m_pIconSkin->IsStateIdentical(newState,oldState)) )
			_requestRender();
	}

}


//_____ _getRenderState() ________________________________________________________

WgState WgButton::_getRenderState()
{
	if( !IsEnabled() )
		return WG_STATE_DISABLED;

	if( m_bReturnPressed || (m_bPressed && (m_bPointerInside || m_bDownOutside)) )
		return WG_STATE_PRESSED;

	if( m_bPointerInside )
		return WG_STATE_HOVERED;

	return WG_STATE_NORMAL;
}


//____ _onRefresh() ____________________________________________________________

void WgButton::_onRefresh( void )
{
	if( m_pSkin )
	{
		if( m_pSkin->IsOpaque() )
			m_bOpaque = true;
		else
			m_bOpaque = false;

		_requestRender();
	}
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

	pOrg->Wg_Interface_TextHolder::_cloneInterface( this );
	WgIconHolder::_onCloneContent( pOrg );

	m_text.setText(&pOrg->m_text);
	m_pText = &m_text;
	m_text.setHolder( this );

	m_pSkin		= pOrg->m_pSkin;
	m_pIconSkin		= pOrg->m_pIconSkin;
	m_state			= pOrg->m_state;
}

//____ _onAlphaTest() ___________________________________________________________

bool WgButton::_onAlphaTest( const WgCoord& ofs )
{
	WgSize	sz = Size();
	bool	bMarked = false;


	if( m_pSkin )
		bMarked = m_pSkin->MarkTest( ofs, WgRect(0,0,sz), m_state, m_markOpacity );

	if( !bMarked && m_pIconSkin )
	{
		//TODO: Test against icon.
	}

	return bMarked;
}

//____ _onGotInputFocus() ______________________________________________________

void WgButton::_onGotInputFocus()
{
	m_bFocused = true;
	_requestRender();
}

//____ _onLostInputFocus() _____________________________________________________

void WgButton::_onLostInputFocus()
{
	m_bFocused = false;
	m_bReturnPressed = false;
	_requestRender();
}


//____ _textModified() __________________________________________________________

void WgButton::_textModified()
{
	_requestRender();
}

//____ _iconModified() __________________________________________________________

void WgButton::_iconModified()
{
	//TODO: Should possibly refresh size too.
	_requestRender();
}
