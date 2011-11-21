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

#include <wg_gizmo_text.h>
#include <wg_key.h>
#include <wg_font.h>
#include <wg_gfxdevice.h>
#include <wg_eventhandler.h>

static const char	c_gizmoType[] = {"GizmoText"};



//____ WgGizmoText() _________________________________________________________________

WgGizmoText::WgGizmoText()
{
	m_pText			= &m_text;
	m_pText->CreateCursor();
	m_maxCharacters	= 0;
	m_maxLines		= 0;

	m_text.setLineWidth( Size().w );
	m_editMode = WG_TEXT_STATIC;
	m_bResetCursorOnFocus = true;
}


//____ Destructor _____________________________________________________________

WgGizmoText::~WgGizmoText()
{
}


//____ Type() _________________________________________________________________

const char * WgGizmoText::Type() const
{
	return WgGizmoText::GetMyType();
}

const char * WgGizmoText::GetMyType()
{
	return c_gizmoType;
}

//____ goBOL() ________________________________________________________________
void WgGizmoText::goBOL()
{
	if( IsEditable() && m_bFocused )
		m_pText->goBOL();
}

//____ goEOL() ________________________________________________________________
void WgGizmoText::goEOL()
{
	if( IsEditable() && m_bFocused )
		m_pText->goEOL();
}

//____ goBOF() ________________________________________________________________
void WgGizmoText::goBOF()
{
	if( IsEditable() && m_bFocused )
		m_pText->goBOF();
}

//____ goEOF() ________________________________________________________________
void WgGizmoText::goEOF()
{
	if( IsEditable() && m_bFocused )
		m_pText->goEOF();
}


//_______________________________________________________________
void WgGizmoText::SetEditMode(WgTextEditMode mode)
{
	m_editMode = mode;
}

//____ _onUpdate() ________________________________________________________

void WgGizmoText::_onUpdate( const WgUpdateInfo& _updateInfo )
{
	if( IsSelectable() && m_bFocused )
	{
		m_pText->incTime( _updateInfo.msDiff );
		RequestRender();					//TODO: Should only render the cursor and selection!
	}
}

//____ HeightForWidth() _______________________________________________________

int WgGizmoText::HeightForWidth( int width ) const
{
	return m_text.heightForWidth( width );
}

//____ DefaultSize() _____________________________________________________________

WgSize WgGizmoText::DefaultSize() const
{
	//TODO: Fix this so we don't return current size (after wraptext is adapted to width) but size for unwrapped lines.

	return m_text.unwrappedSize();
}

//____ GetPointerStyle() ________________________________________

WgPointerStyle WgGizmoText::GetPointerStyle() const
{
	if( m_text.GetMarkedLink() )
		return WG_POINTER_HAND;

	return m_pointerStyle;
}

//____ GetTooltipString() _____________________________________________________

WgString WgGizmoText::GetTooltipString() const
{ 
	if( !m_tooltip.IsEmpty() )
		return m_tooltip; 
	else	
	{
		WgSize sz = Size();
		if( sz.w < m_text.width() || sz.h < m_text.height() )
			return m_text.getBuffer();
	}

	return 0;
}

//____ _onRender() ________________________________________________________

void WgGizmoText::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip, Uint8 _layer )
{
	WgText * pText = &m_text;

	if( m_bFocused && IsEditable() )
		m_text.showCursor();
	else
		m_text.hideCursor();

	pDevice->PrintText( _clip, pText, _canvas );

	if( pText != &m_text )
		delete pText;
}

//____ _onRefresh() _______________________________________________________

void WgGizmoText::_onRefresh( void )
{
	//TODO: Implement more I believe...

	RequestRender();
}

//____ _onEvent() ______________________________________________________________

void WgGizmoText::_onEvent( const WgEvent::Event * pEvent, WgEventHandler * pHandler )
{
	int type 				= pEvent->Type();
	WgModifierKeys modKeys 	= pEvent->ModKeys();

	if( type == WG_EVENT_TICK )
	{
		if( IsSelectable() && m_bFocused )
		{
			m_pText->incTime( ((const WgEvent::Tick*)(pEvent))->Millisec() );
			RequestRender();					//TODO: Should only render the cursor and selection!
		}
		return;
	}



	if( m_bFocused && (type == WG_EVENT_MOUSEBUTTON_PRESS || type == WG_EVENT_MOUSEBUTTON_DRAG) && ((const WgEvent::MouseButtonEvent*)(pEvent))->Button() == 1 )
	{

		if( IsSelectable() && (modKeys & WG_MODKEY_SHIFT) )
		{
			m_pText->setSelectionMode(true);
		}

		m_pText->CursorGotoCoord( pEvent->PointerPos(), Geo() );

		if(IsSelectable() && type == WG_EVENT_MOUSEBUTTON_PRESS && !(modKeys & WG_MODKEY_SHIFT))
		{
			m_pText->clearSelection();
			m_pText->setSelectionMode(true);
		}
	}
	else if( type == WG_EVENT_MOUSEBUTTON_RELEASE  )
	{
		if(m_bFocused && ((const WgEvent::MouseButtonEvent*)(pEvent))->Button() == 1)
			m_pText->setSelectionMode(false);
	}
	else if( !m_bFocused && IsEditable() && type == WG_EVENT_MOUSEBUTTON_PRESS && ((const WgEvent::MouseButtonEvent*)(pEvent))->Button() == 1 )
	{
		GrabFocus();
	}


	if( type == WG_EVENT_CHARACTER )
	{
		if( IsEditable() && m_bFocused )
		{
			int  chr = static_cast<const WgEvent::Character*>(pEvent)->Char();

			if( chr >= 32 && chr != 127)
			{
				InsertCharAtCursorInternal(chr);
			}
			else if( chr == 13 )
			{
				InsertCharAtCursorInternal('\n');
			}
			else if( chr == '\t' && m_bTabLock )
			{
				InsertCharAtCursorInternal( '\t' );
			}
		}
	}

	if( type == WG_EVENT_KEY_RELEASE && m_bFocused )
	{
		switch( static_cast<const WgEvent::KeyEvent*>(pEvent)->TranslatedKeyCode() )
		{
			case WG_KEY_SHIFT:
				if(!pHandler->IsMouseButtonPressed(1))
					m_pText->setSelectionMode(false);
			break;
		}
	}

	if( (type == WG_EVENT_KEY_PRESS || type == WG_EVENT_KEY_REPEAT) && IsEditable() && m_bFocused )
	{
		switch( static_cast<const WgEvent::KeyEvent*>(pEvent)->TranslatedKeyCode() )
		{
			case WG_KEY_LEFT:
				if( modKeys & WG_MODKEY_SHIFT )
					m_pText->setSelectionMode(true);

				if( modKeys & WG_MODKEY_CTRL )
					m_pText->gotoPrevWord();
				else
					m_pText->goLeft();
				break;
			case WG_KEY_RIGHT:
				if( modKeys & WG_MODKEY_SHIFT )
					m_pText->setSelectionMode(true);

				if( modKeys & WG_MODKEY_CTRL )
					m_pText->gotoNextWord();
				else
					m_pText->goRight();
				break;

			case WG_KEY_UP:
				if( modKeys & WG_MODKEY_SHIFT )
					m_pText->setSelectionMode(true);

				m_pText->CursorGoUp( 1, ScreenGeo() );
				break;

			case WG_KEY_DOWN:
				if( modKeys & WG_MODKEY_SHIFT )
					m_pText->setSelectionMode(true);

				m_pText->CursorGoDown( 1, ScreenGeo() );
				break;

			case WG_KEY_BACKSPACE:
				if(m_pText->hasSelection())
					m_pText->delSelection();
				else if( modKeys & WG_MODKEY_CTRL )
					m_pText->delPrevWord();
				else
					m_pText->delPrevChar();
				break;

			case WG_KEY_DELETE:
				if(m_pText->hasSelection())
					m_pText->delSelection();
				else if( modKeys & WG_MODKEY_CTRL )
					m_pText->delNextWord();
				else
					m_pText->delNextChar();
				break;

			case WG_KEY_HOME:
				if( modKeys & WG_MODKEY_SHIFT )
					m_pText->setSelectionMode(true);

				if( modKeys & WG_MODKEY_CTRL )
					m_pText->goBOF();
				else
					m_pText->goBOL();
				break;

			case WG_KEY_END:
				if( modKeys & WG_MODKEY_SHIFT )
					m_pText->setSelectionMode(true);

				if( modKeys & WG_MODKEY_CTRL )
					m_pText->goEOF();
				else
					m_pText->goEOL();
				break;

			default:
				break;
		}
	}

	// Let text object handle its actions.
/*
	bool bChanged = m_text.OnAction( action, button_key, ScreenGeo(), WgCoord(info.x, info.y) );
	if( bChanged )
		RequestRender();
*/
}

//____ _onAction() _________________________________________________

void WgGizmoText::_onAction( WgInput::UserAction action, int button_key, const WgActionDetails& info, const WgInput& inputObj )
{
#ifdef WG_LEGACY

	if( m_bFocused && (action == WgInput::BUTTON_PRESS || action == WgInput::BUTTON_DOWN) && button_key == 1 )
	{

		if( IsSelectable() && (info.modifier & WG_MODKEY_SHIFT) )
		{
			m_pText->setSelectionMode(true);
		}

		m_pText->CursorGotoCoord( WgCoord(info.x, info.y), ScreenGeo() );

		if(IsSelectable() && action == WgInput::BUTTON_PRESS && !(info.modifier & WG_MODKEY_SHIFT))
		{
			m_pText->clearSelection();
			m_pText->setSelectionMode(true);
		}
	}
	else if( action == WgInput::BUTTON_RELEASE || action == WgInput::BUTTON_RELEASE_OUTSIDE )
	{
		if(m_bFocused && button_key == 1)
			m_pText->setSelectionMode(false);
	}
	else if( !m_bFocused && IsEditable() && action == WgInput::BUTTON_PRESS && button_key == 1 )
	{
		GrabFocus();
	}


	if( action == WgInput::CHARACTER )
	{
		if( IsEditable() && m_bFocused )
		{
			if( button_key >= 32 && button_key != 127)
			{
				InsertCharAtCursorInternal(button_key);
			}
			else if( button_key == 13 )
			{
				InsertCharAtCursorInternal('\n');
			}
			else if( button_key == '\t' )
			{
				InsertCharAtCursorInternal( '\t' );
			}
		}
	}

	if( action == WgInput::KEY_RELEASE && m_bFocused )
	{
		switch( button_key )
		{
			case WG_KEY_SHIFT:
				if(!inputObj.isButtonDown(1))
					m_pText->setSelectionMode(false);
			break;
		}
	}

	if( (action == WgInput::KEY_PRESS || action == WgInput::KEY_REPEAT) && IsEditable() && m_bFocused )
	{
		switch( button_key )
		{
			case WG_KEY_LEFT:
				if( info.modifier & WG_MODKEY_SHIFT )
					m_pText->setSelectionMode(true);

				if( info.modifier & WG_MODKEY_CTRL )
					m_pText->gotoPrevWord();
				else
					m_pText->goLeft();
				break;
			case WG_KEY_RIGHT:
				if( info.modifier & WG_MODKEY_SHIFT )
					m_pText->setSelectionMode(true);

				if( info.modifier & WG_MODKEY_CTRL )
					m_pText->gotoNextWord();
				else
					m_pText->goRight();
				break;

			case WG_KEY_UP:
				if( info.modifier & WG_MODKEY_SHIFT )
					m_pText->setSelectionMode(true);

				m_pText->CursorGoUp( 1, ScreenGeo() );
				break;

			case WG_KEY_DOWN:
				if( info.modifier & WG_MODKEY_SHIFT )
					m_pText->setSelectionMode(true);

				m_pText->CursorGoDown( 1, ScreenGeo() );
				break;

			case WG_KEY_BACKSPACE:
				if(m_pText->hasSelection())
					m_pText->delSelection();
				else if( info.modifier & WG_MODKEY_CTRL )
					m_pText->delPrevWord();
				else
					m_pText->delPrevChar();
				break;

			case WG_KEY_DELETE:
				if(m_pText->hasSelection())
					m_pText->delSelection();
				else if( info.modifier & WG_MODKEY_CTRL )
					m_pText->delNextWord();
				else
					m_pText->delNextChar();
				break;

			case WG_KEY_HOME:
				if( info.modifier & WG_MODKEY_SHIFT )
					m_pText->setSelectionMode(true);

				if( info.modifier & WG_MODKEY_CTRL )
					m_pText->goBOF();
				else
					m_pText->goBOL();
				break;

			case WG_KEY_END:
				if( info.modifier & WG_MODKEY_SHIFT )
					m_pText->setSelectionMode(true);

				if( info.modifier & WG_MODKEY_CTRL )
					m_pText->goEOF();
				else
					m_pText->goEOL();
				break;

			default:
				break;
		}
	}

	// Let text object handle its actions.

	bool bChanged = m_text.OnAction( action, button_key, ScreenGeo(), WgCoord(info.x, info.y) );
	if( bChanged )
		RequestRender();

#endif //WG_LEGACY
}

//____ _onCloneContent() _______________________________________________________

void WgGizmoText::_onCloneContent( const WgGizmo * _pOrg )
{
	//TODO: Implement!
}

//____ _onAlphaTest() ______________________________________________________

bool WgGizmoText::_onAlphaTest( const WgCoord& ofs )
{
	return true;																				// Accept all at least for now...
}

//____ _onEnable() ___________________________________________________

void WgGizmoText::_onEnable( void )
{
	m_text.setMode(WG_MODE_NORMAL);
	RequestRender();
}

//____ _onDisable() ___________________________________________________

void WgGizmoText::_onDisable( void )
{
	m_text.setMode(WG_MODE_DISABLED);
	RequestRender();
}

//____ _onNewSize() ________________________________________________

void WgGizmoText::_onNewSize( const WgSize& size )
{
	m_text.setLineWidth( size.w );
}


//____ _onGotInputFocus() ______________________________________________

void WgGizmoText::_onGotInputFocus()
{
	m_bFocused = true;
	if( IsEditable() ) // render with cursor on
	{
		_startReceiveTicks();
		if(	m_bResetCursorOnFocus )
			m_pText->GetCursor()->goEOF();
		RequestRender();
	}
}

//____ _onLostInputFocus() ______________________________________________

void WgGizmoText::_onLostInputFocus()
{
	m_bFocused = false;
	m_bResetCursorOnFocus = false;
	if( IsEditable() )
	{
		_stopReceiveTicks();
		RequestRender();
	}
}



//____ _textModified() _________________________________________________________

void WgGizmoText::_textModified()
{
	m_bResetCursorOnFocus = true;
	RequestRender();
}

//____ InsertTextAtCursor() ___________________________________________________

Uint32 WgGizmoText::InsertTextAtCursor( const WgCharSeq& str )
{
	if( !IsEditable() )
		return 0;

	if( m_bFocused )
		if( !GrabFocus() )
			return 0;				// Couldn't get input focus...

	Uint32 nChars = 0;

	if( m_maxCharacters == 0 || str.Length() < m_maxCharacters - m_pText->nbChars() )
	{
		m_pText->putText( str );
		nChars = str.Length();
	}
	else
	{
		nChars = m_maxCharacters - m_pText->nbChars();
		m_pText->putText( WgCharSeq( str, 0, nChars) );
	}

	if( m_maxLines != 0 && m_maxLines < (int) m_pText->nbSoftLines() )
	{
		m_pText->unputText( nChars );
		nChars = 0;
	}

	return nChars;
}

//____ InsertCharAtCursor() ___________________________________________________

bool WgGizmoText::InsertCharAtCursor( Uint16 c )
{
	if( !IsEditable() )
		return 0;

	if( m_bFocused )
		if( !GrabFocus() )
			return false;				// Couldn't get input focus...

	return InsertCharAtCursorInternal(c);
}

bool WgGizmoText::InsertCharAtCursorInternal( Uint16 c )
{
	if(m_pText->hasSelection())
		m_pText->delSelection();
	m_pText->setSelectionMode(false);

	if( m_maxCharacters != 0 && m_maxCharacters < (int) m_pText->nbChars() )
		return false;

	m_pText->putChar( c );

	if( m_maxLines != 0 && m_maxLines < (int) m_pText->nbSoftLines() )
	{
		m_pText->delPrevChar();
		return false;
	}

	return true;
}
