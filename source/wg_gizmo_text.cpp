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

static const char	Wdg_Type[] = {"TordJ/GizmoText"};



//____ WgGizmoText() _________________________________________________________________

WgGizmoText::WgGizmoText()
{
	m_pText			= &m_text;
	m_pMyCursor		= 0;
	m_maxCharacters = 0;

	m_text.setLineWidth( Size().w );
	m_bEditable = false;
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
	return Wdg_Type;
}

//____ goBOL() ________________________________________________________________
void WgGizmoText::goBOL()
{
	if( m_pMyCursor )
		m_pMyCursor->goBOL();
}

//____ goEOL() ________________________________________________________________
void WgGizmoText::goEOL()
{
	if( m_pMyCursor )
		m_pMyCursor->goEOL();
}

//____ goBOF() ________________________________________________________________
void WgGizmoText::goBOF()
{
	if( m_pMyCursor )
		m_pMyCursor->goBOF();
}

//____ goEOF() ________________________________________________________________
void WgGizmoText::goEOF()
{
	if( m_pMyCursor )
		m_pMyCursor->goEOF();
}


//____ SetEditable() __________________________________________________________
void WgGizmoText::SetEditable(bool bEditable)
{
	m_bEditable = bEditable;
}



//____ OnUpdate() ________________________________________________________

void WgGizmoText::OnUpdate( const WgUpdateInfo& _updateInfo )
{
	if( m_pMyCursor )
	{
		m_pMyCursor->incTime( _updateInfo.msDiff );
		RequestRender();					//TODO: Should only render the cursor!
	}
}


//____ OnRender() ________________________________________________________

void WgGizmoText::OnRender( WgGfxDevice * pDevice, const WgRect& _window, const WgRect& _clip, Uint8 _layer )
{
	WgText * pText = &m_text;

	if( m_pMyCursor )
		pDevice->ClipPrintTextWithCursor( _clip, pText, *m_pMyCursor, _window );
	else
		pDevice->ClipPrintText( _clip, pText, _window );		

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
	if( action == WgInput::BUTTON_PRESS && button_key == 1 )
	{		
		if( m_pMyCursor )
		{
			WgCord pos = Abs2local( WgCord(info.x, info.y) );

			Sint32	x = pos.x;
			Sint32	y = pos.y;
			
			m_pMyCursor->gotoPixel(x,y);
		}
	}

	if( action == WgInput::CHARACTER )
	{
		if( m_pMyCursor )
		{
			if( button_key >= 32 )
			{
				// by default - no max limit
				if( m_maxCharacters == 0 || m_maxCharacters > m_pMyCursor->text()->nbChars() )
					m_pMyCursor->putChar( button_key );
			}
			if( button_key == 13 )
					m_pMyCursor->putChar( '\n' );

			if( button_key == '\t' )
					m_pMyCursor->putChar( '\t' );
		}
	}

	if( action == WgInput::KEY_PRESS || action == WgInput::KEY_REPEAT )
	{
		if( m_pMyCursor )
		{
			switch( button_key )
			{
				case WGKEY_LEFT:
					if( info.modifier == WG_MODKEY_CTRL )
						m_pMyCursor->gotoPrevWord();
					else
						m_pMyCursor->goLeft();
					break;
				case WGKEY_RIGHT:
					if( info.modifier == WG_MODKEY_CTRL )
						m_pMyCursor->gotoNextWord();
					else
					m_pMyCursor->goRight();
					break;

				case WGKEY_UP:
					m_pMyCursor->goUp();
					break;

				case WGKEY_DOWN:
					m_pMyCursor->goDown();
					break;

				case WGKEY_BACKSPACE:
					m_pMyCursor->delPrevChar();
					break;

				case WGKEY_DELETE:
					m_pMyCursor->delNextChar();
					break;

				case WGKEY_HOME:
					if( info.modifier == WG_MODKEY_CTRL )
						m_pMyCursor->goBOF();
					else
						m_pMyCursor->goBOL();
					break;

				case WGKEY_END:
					if( info.modifier == WG_MODKEY_CTRL )
						m_pMyCursor->goEOF();
					else
						m_pMyCursor->goEOL();
					break;

				default:
					break;
			}

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
	if( m_bEditable )
	{
		m_pMyCursor = new WgCursorInstance( m_text );
		RequestRender();
	}
}

//____ OnLostInputFocus() ______________________________________________

void WgGizmoText::OnLostInputFocus()
{
	if( m_pMyCursor )
	{
		delete m_pMyCursor;
		m_pMyCursor = 0;
		
		RequestRender();
	}
}



//____ TextModified() _________________________________________________________

void WgGizmoText::TextModified()
{
	if( m_pMyCursor )
	{
		m_pMyCursor->gotoHardPos( m_pMyCursor->line(), m_pMyCursor->column() );
	}

	RequestRender();
}

//____ InsertTextAtCursor() ___________________________________________________

Uint32 WgGizmoText::InsertTextAtCursor( const WgCharSeq& str )
{
	if( !m_bEditable )
		return 0;

	if( !m_pMyCursor )
		if( !GrabFocus() )
			return 0;				// Couldn't get input focus...

	Uint32 retVal = 0;

	if( m_maxCharacters == 0 || ((unsigned)str.Length()) < m_maxCharacters - m_pMyCursor->text()->nbChars() )
	{
		m_pMyCursor->putText( str.GetUnicode().ptr, str.Length() );
		retVal = str.Length();
	}
	else
	{
		retVal = m_maxCharacters - m_pMyCursor->text()->nbChars();
		m_pMyCursor->putText( str.GetUnicode().ptr, retVal );
	}
	
	return retVal;
}

//____ InsertCharAtCursor() ___________________________________________________

bool WgGizmoText::InsertCharAtCursor( Uint16 c )
{
	if( !m_bEditable )
		return 0;

	if( !m_pMyCursor )
		if( !GrabFocus() )
			return false;				// Couldn't get input focus...

	if( m_maxCharacters != 0 && m_maxCharacters < m_pMyCursor->text()->nbChars() )
		return false;

	m_pMyCursor->putChar( c );
	return true;
}
