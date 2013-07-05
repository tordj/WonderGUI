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

#include <wg_textdisplay.h>
#include <wg_key.h>
#include <wg_font.h>
#include <wg_gfxdevice.h>
#include <wg_eventhandler.h>

static const char	c_widgetType[] = {"TextDisplay"};



//____ WgTextDisplay() _________________________________________________________________

WgTextDisplay::WgTextDisplay()
{
	m_pText			= &m_text;
	m_maxLines		= 0;
    m_text.setHolder( this );

	m_text.setLineWidth( Size().w );
	m_text.SetAutoEllipsis(IsAutoEllipsisDefault());
	m_text.SetEditMode( WG_TEXT_STATIC );
	m_bResetCursorOnFocus = true;
}


//____ Destructor _____________________________________________________________

WgTextDisplay::~WgTextDisplay()
{
}


//____ Type() _________________________________________________________________

const char * WgTextDisplay::Type() const
{
	return WgTextDisplay::GetClass();
}

const char * WgTextDisplay::GetClass()
{
	return c_widgetType;
}

//____ GoBOL() ________________________________________________________________
void WgTextDisplay::GoBOL()
{
	if( IsEditable() && m_state.IsFocused() )
		m_pText->goBOL();
}

//____ GoEOL() ________________________________________________________________
void WgTextDisplay::GoEOL()
{
	if( IsEditable() && m_state.IsFocused() )
		m_pText->goEOL();
}

//____ GoBOF() ________________________________________________________________
void WgTextDisplay::GoBOF()
{
	if( IsEditable() && m_state.IsFocused() )
		m_pText->goBOF();
}

//____ GoEOF() ________________________________________________________________
void WgTextDisplay::GoEOF()
{
	if( IsEditable() && m_state.IsFocused() )
		m_pText->goEOF();
}


//_______________________________________________________________
void WgTextDisplay::SetEditMode(WgTextEditMode mode)
{
	m_text.SetEditMode(mode);
}

//____ HeightForWidth() _______________________________________________________

int WgTextDisplay::HeightForWidth( int width ) const
{
	int textHeight = m_text.heightForWidth( width );

	if( m_pSkin )
		textHeight += m_pSkin->ContentPadding().h;

	return textHeight;
}

//____ PreferredSize() _____________________________________________________________

WgSize WgTextDisplay::PreferredSize() const
{
	WgSize contentSize = m_text.unwrappedSize();

	if( m_pSkin )
		return m_pSkin->SizeForContent(contentSize);
	else
		return contentSize;
}

//____ PointerStyle() ________________________________________

WgPointerStyle WgTextDisplay::PointerStyle() const
{
	if( m_text.GetMarkedLink() )
		return WG_POINTER_HAND;

	return m_pointerStyle;
}

//____ TooltipString() _____________________________________________________

WgString WgTextDisplay::TooltipString() const
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

void WgTextDisplay::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip )
{
	WgWidget::_onRender(pDevice,_canvas,_window,_clip);

	WgRect canvas;
	if( m_pSkin )
		canvas = m_pSkin->ContentRect(_canvas, m_state);
	else
		canvas = _canvas;

	pDevice->PrintText( _clip, m_pText, canvas );
}

//____ _onRefresh() _______________________________________________________

void WgTextDisplay::_onRefresh( void )
{
	//TODO: Implement more I believe...

	_requestRender();
}

//____ _onStateChanged() ______________________________________________________

void WgTextDisplay::_onStateChanged( WgState oldState, WgState newState )
{
	WgWidget::_onStateChanged(oldState,newState);

	m_text.setState(newState);
	_requestRender(); //TODO: Only requestRender if text appearance has changed.

	if( IsEditable() )
	{
		if( newState.IsFocused() && !oldState.IsFocused() )
		{
			m_text.showCursor();
			_startReceiveTicks();
			if(	m_bResetCursorOnFocus )
				m_pText->goEOF();
			_requestRender();
		}
		if( !newState.IsFocused() && oldState.IsFocused() )
		{
			m_text.hideCursor();
			_stopReceiveTicks();
			_requestRender();
		}
	}
}


//____ _onEvent() ______________________________________________________________

void WgTextDisplay::_onEvent( const WgEvent::Event * pEvent, WgEventHandler * pHandler )
{
	WgWidget::_onEvent(pEvent,pHandler);

	int type 				= pEvent->Type();
	WgModifierKeys modKeys 	= pEvent->ModKeys();

	if( type == WG_EVENT_TICK )
	{
		if( IsSelectable() && m_state.IsFocused() )
		{
			m_pText->incTime( ((const WgEvent::Tick*)(pEvent))->Millisec() );
			_requestRender();					//TODO: Should only render the cursor and selection!
		}
		return;
	}



	if( m_state.IsFocused() && (type == WG_EVENT_MOUSEBUTTON_PRESS || type == WG_EVENT_MOUSEBUTTON_DRAG) && ((const WgEvent::MouseButtonEvent*)(pEvent))->Button() == 1 )
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
		if(m_state.IsFocused() && ((const WgEvent::MouseButtonEvent*)(pEvent))->Button() == 1)
			m_pText->setSelectionMode(false);
	}
	else if( !m_state.IsFocused() && IsEditable() && type == WG_EVENT_MOUSEBUTTON_PRESS && ((const WgEvent::MouseButtonEvent*)(pEvent))->Button() == 1 )
	{
		GrabFocus();
	}


	if( type == WG_EVENT_CHARACTER )
	{
		if( IsEditable() )
		{
			int  chr = static_cast<const WgEvent::Character*>(pEvent)->Char();

			if( chr >= 32 && chr != 127)
			{
				_insertCharAtCursor(chr);
			}
			else if( chr == 13 )
			{
				_insertCharAtCursor('\n');
			}
			else if( chr == '\t' && m_bTabLock )
			{
				_insertCharAtCursor( '\t' );
			}
		}
	}

	if( type == WG_EVENT_KEY_RELEASE )
	{
		switch( static_cast<const WgEvent::KeyEvent*>(pEvent)->TranslatedKeyCode() )
		{
			case WG_KEY_SHIFT:
				if(!pHandler->IsMouseButtonPressed(1))
					m_pText->setSelectionMode(false);
			break;
		}
	}

	if( (type == WG_EVENT_KEY_PRESS || type == WG_EVENT_KEY_REPEAT) && IsEditable() )
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

	// Forward event depending on rules.

	if( pEvent->IsMouseButtonEvent() )
	{
		if( static_cast<const WgEvent::MouseButtonEvent*>(pEvent)->Button() == 1 )
			pEvent->Swallow();
	}
	else if( pEvent->IsKeyEvent() )
	{
		int key = static_cast<const WgEvent::KeyEvent*>(pEvent)->TranslatedKeyCode();
		if( static_cast<const WgEvent::KeyEvent*>(pEvent)->IsMovementKey() == true ||
			key == WG_KEY_DELETE || key == WG_KEY_BACKSPACE || key == WG_KEY_RETURN || (key == WG_KEY_TAB && m_bTabLock) )
				pEvent->Swallow();
		
		//TODO: Would be good if we didn't forward any character-creating keys either...
	}
	else if( type == WG_EVENT_CHARACTER )
		pEvent->Swallow();

}


//____ _onCloneContent() _______________________________________________________

void WgTextDisplay::_onCloneContent( const WgWidget * _pOrg )
{
	const WgTextDisplay * pOrg = static_cast<const WgTextDisplay*>(_pOrg);

	m_text = pOrg->m_text;
	m_maxLines = pOrg->m_maxLines;
}

//____ _onSkinChanged() _______________________________________________________

void WgTextDisplay::_onSkinChanged( const WgSkinPtr& pOldSkin, const WgSkinPtr& pNewSkin )
{
	WgWidget::_onSkinChanged(pOldSkin,pNewSkin);
	m_text.SetColorSkin(pNewSkin);
}

//____ _onNewSize() ________________________________________________

void WgTextDisplay::_onNewSize( const WgSize& size )
{
	int width = size.w;

	if( m_pSkin )
		width -= m_pSkin->ContentPadding().w;

	m_text.setLineWidth( width );
}


//____ _textModified() _________________________________________________________

void WgTextDisplay::_textModified()
{
	m_bResetCursorOnFocus = true;
    _requestResize();
	_requestRender();
}

//____ InsertTextAtCursor() ___________________________________________________

int WgTextDisplay::InsertTextAtCursor( const WgCharSeq& str )
{
	if( !IsEditable() )
		return 0;

	if( !m_state.IsFocused() )
		if( !GrabFocus() )
			return 0;				// Couldn't get input focus...

	int nChars = m_pText->putText( str );

	if( m_maxLines != 0 && m_maxLines < m_pText->nbSoftLines() )
	{
		m_pText->unputText( nChars );
		nChars = 0;
	}

	return nChars;
}

//____ InsertCharAtCursor() ___________________________________________________

bool WgTextDisplay::InsertCharAtCursor( Uint16 c )
{
	if( !IsEditable() )
		return 0;

	if( !m_state.IsFocused() )
		if( !GrabFocus() )
			return false;				// Couldn't get input focus...

	return _insertCharAtCursor(c);
}

//____ _insertCharAtCursor() ___________________________________________

bool WgTextDisplay::_insertCharAtCursor( Uint16 c )
{
	if(m_pText->hasSelection())
		m_pText->delSelection();
	m_pText->setSelectionMode(false);

	if( m_pText->putChar( c ) == false )
		return false;

	if( m_maxLines != 0 && m_maxLines < (int) m_pText->nbSoftLines() )
	{
		m_pText->delPrevChar();
		return false;
	}

	return true;
}
