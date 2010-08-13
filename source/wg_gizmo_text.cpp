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

#include	<wg_gizmo_text.h>
#include	<wg_key.h>
#include	<wg_font.h>
#include	<wg_gfxdevice.h>

static const char	c_gizmoType[] = {"TordJ/GizmoText"};



//____ WgGizmoText() _________________________________________________________________

WgGizmoText::WgGizmoText()
{
	m_pText			= &m_text;
	m_pText->CreateCursor();
	m_bHasFocus		= false;
	m_maxCharacters	= 0;
	m_maxLines		= 0;

	m_text.setLineWidth( Size().w );
	m_inputMode = Static;
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
	if( IsEditable() && m_bHasFocus )
		m_pText->goBOL();
}

//____ goEOL() ________________________________________________________________
void WgGizmoText::goEOL()
{
	if( IsEditable() && m_bHasFocus )
		m_pText->goEOL();
}

//____ goBOF() ________________________________________________________________
void WgGizmoText::goBOF()
{
	if( IsEditable() && m_bHasFocus )
		m_pText->goBOF();
}

//____ goEOF() ________________________________________________________________
void WgGizmoText::goEOF()
{
	if( IsEditable() && m_bHasFocus )
		m_pText->goEOF();
}


//_______________________________________________________________
void WgGizmoText::SetInputMode(InputMode mode)
{
	m_inputMode = mode;
}

//____ OnUpdate() ________________________________________________________

void WgGizmoText::OnUpdate( const WgUpdateInfo& _updateInfo )
{
	if( IsSelectable() && m_bHasFocus )
	{
		m_pText->incTime( _updateInfo.msDiff );
		RequestRender();					//TODO: Should only render the cursor and selection!
	}
}


//____ OnRender() ________________________________________________________

void WgGizmoText::OnRender( WgGfxDevice * pDevice, const WgRect& _window, const WgRect& _clip, Uint8 _layer )
{
	WgText * pText = &m_text;

	if( m_bHasFocus && IsEditable() )
		pDevice->PrintTextWithCursor( _clip, pText, *m_pText->GetCursor(), _window );
	else
		pDevice->PrintText( _clip, pText, _window );

	if( pText != &m_text )
		delete pText;
}

//____ OnRefresh() _______________________________________________________

void WgGizmoText::OnRefresh( void )
{
	//TODO: Implement more I believe...

	RequestRender();
}

//____ OnAction() _________________________________________________

void WgGizmoText::OnAction( WgEmitter * pEmitter, WgInput::UserAction action, int button_key, const WgActionDetails& info, const WgInput& inputObj )
{
	if( m_bHasFocus && (action == WgInput::BUTTON_PRESS || action == WgInput::BUTTON_DOWN) && button_key == 1 )
	{
		if( IsSelectable() && (info.modifier & WG_MODKEY_SHIFT) )
		{
			m_pText->setSelectionMode(true);
		}

		WgCord pos = Abs2local( WgCord(info.x, info.y) );

		Sint32	x = pos.x;
		Sint32	y = pos.y;

		m_pText->gotoPixel(x,y);

		if(IsSelectable() && action == WgInput::BUTTON_PRESS && !(info.modifier & WG_MODKEY_SHIFT))
		{
			m_pText->clearSelection();
			m_pText->setSelectionMode(true);
		}
	}
	else if( action == WgInput::BUTTON_RELEASE || action == WgInput::BUTTON_RELEASE_OUTSIDE )
	{
		if(m_bHasFocus && button_key == 1)
			m_pText->setSelectionMode(false);
	}

	if( action == WgInput::CHARACTER )
	{
		if( IsEditable() && m_bHasFocus )
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

	if( action == WgInput::KEY_RELEASE && m_bHasFocus )
	{
		switch( button_key )
		{
			case WGKEY_SHIFT:
				if(!inputObj.isButtonDown(1))
					m_pText->setSelectionMode(false);
			break;
		}
	}

	if( (action == WgInput::KEY_PRESS || action == WgInput::KEY_REPEAT) && IsEditable() && m_bHasFocus )
	{
		switch( button_key )
		{
			case WGKEY_LEFT:
				if( info.modifier & WG_MODKEY_SHIFT )
					m_pText->setSelectionMode(true);

				if( info.modifier & WG_MODKEY_CTRL )
					m_pText->gotoPrevWord();
				else
					m_pText->goLeft();
				break;
			case WGKEY_RIGHT:
				if( info.modifier & WG_MODKEY_SHIFT )
					m_pText->setSelectionMode(true);

				if( info.modifier & WG_MODKEY_CTRL )
					m_pText->gotoNextWord();
				else
					m_pText->goRight();
				break;

			case WGKEY_UP:
				if( info.modifier & WG_MODKEY_SHIFT )
					m_pText->setSelectionMode(true);

				m_pText->goUp();
				break;

			case WGKEY_DOWN:
				if( info.modifier & WG_MODKEY_SHIFT )
					m_pText->setSelectionMode(true);

				m_pText->goDown();
				break;

			case WGKEY_BACKSPACE:
				if(m_pText->hasSelection())
					m_pText->delSelection();
				else if( info.modifier & WG_MODKEY_CTRL )
					m_pText->delPrevWord();
				else
					m_pText->delPrevChar();
				break;

			case WGKEY_DELETE:
				if(m_pText->hasSelection())
					m_pText->delSelection();
				else if( info.modifier & WG_MODKEY_CTRL )
					m_pText->delNextWord();
				else
					m_pText->delNextChar();
				break;

			case WGKEY_HOME:
				if( info.modifier & WG_MODKEY_SHIFT )
					m_pText->setSelectionMode(true);

				if( info.modifier & WG_MODKEY_CTRL )
					m_pText->goBOF();
				else
					m_pText->goBOL();
				break;

			case WGKEY_END:
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
}

//____ OnCloneContent() _______________________________________________________

void WgGizmoText::OnCloneContent( const WgGizmo * _pOrg )
{
	//TODO: Implement!
}

//____ OnMarkTest() ______________________________________________________

bool WgGizmoText::OnMarkTest( const WgCord& ofs )
{
	return true;																				// Accept all at least for now...
}

//____ OnEnable() ___________________________________________________

void WgGizmoText::OnEnable( void )
{
	m_text.setMode(WG_MODE_NORMAL);
	RequestRender();
}

//____ OnDisable() ___________________________________________________

void WgGizmoText::OnDisable( void )
{
	m_text.setMode(WG_MODE_DISABLED);
	RequestRender();
}

//____ OnNewSize() ________________________________________________

void WgGizmoText::OnNewSize( const WgSize& size )
{
	m_text.setLineWidth( size.w );
}


//____ OnGotInputFocus() ______________________________________________

void WgGizmoText::OnGotInputFocus()
{
	m_bHasFocus = true;
	if( IsEditable() ) // render with cursor on
	{
		m_pText->GetCursor()->goEOF();
		RequestRender();
	}
}

//____ OnLostInputFocus() ______________________________________________

void WgGizmoText::OnLostInputFocus()
{
	m_bHasFocus = false;
	if( IsEditable() ) // render with cursor off
		RequestRender();
}



//____ TextModified() _________________________________________________________

void WgGizmoText::TextModified()
{
	RequestRender();
}

//____ InsertTextAtCursor() ___________________________________________________

Uint32 WgGizmoText::InsertTextAtCursor( const WgCharSeq& str )
{
	if( !IsEditable() )
		return 0;

	if( m_bHasFocus )
		if( !GrabFocus() )
			return 0;				// Couldn't get input focus...

	Uint32 nChars = 0;

	if( m_maxCharacters == 0 || ((unsigned)str.Length()) < m_maxCharacters - m_pText->nbChars() )
	{
		m_pText->putText( str );
		nChars = str.Length();
	}
	else
	{
		nChars = m_maxCharacters - m_pText->nbChars();
		m_pText->putText( WgCharSeq( str, 0, nChars) );
	}

	if( m_maxLines != 0 && m_maxLines < m_pText->nbSoftLines() )
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

	if( m_bHasFocus )
		if( !GrabFocus() )
			return false;				// Couldn't get input focus...

	return InsertCharAtCursorInternal(c);
}

bool WgGizmoText::InsertCharAtCursorInternal( Uint16 c )
{
	if( m_maxCharacters != 0 && m_maxCharacters < m_pText->nbChars() )
		return false;

	if(m_pText->hasSelection())
		m_pText->delSelection();
	m_pText->setSelectionMode(false);

	m_pText->putChar( c );

	if( m_maxLines != 0 && m_maxLines < m_pText->nbSoftLines() )
	{
		m_pText->delPrevChar();
		return false;
	}

	return true;
}
