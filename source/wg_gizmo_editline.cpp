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

#include <wg_gizmo_editline.h>

#include	<wg_key.h>
#include	<wg_font.h>
#include <wg_gfxdevice.h>
#include <wg_pen.h>

static const char	c_gizmoType[] = {"TordJ/Editline"};

//____ Constructor ____________________________________________________________

WgGizmoEditline::WgGizmoEditline()
{
	m_pText			= &m_text;
	m_pText->CreateCursor();
	m_text.setHolder( this );
	m_text.SetWrap(false);
	m_bPasswordMode = false;
	m_pwGlyph		= '*';
	m_bHasFocus		= false;
	m_viewOfs		= 0;
	m_maxCharacters = 0;
	m_inputMode		= Editable;
	m_cursorStyle	= WG_CURSOR_IBEAM;
}

//____ Destructor _____________________________________________________________

WgGizmoEditline::~WgGizmoEditline()
{
}

//____ Type() _________________________________________________________________

const char * WgGizmoEditline::Type( void ) const
{
	return GetMyType();
}

//____ GetMyType() ____________________________________________________________

const char * WgGizmoEditline::GetMyType()
{
	return c_gizmoType;
}

//______________________________________________________________
void WgGizmoEditline::SetInputMode(InputMode mode)
{
	m_inputMode = mode;

	if( IsSelectable() )
	{
		m_cursorStyle = WG_CURSOR_IBEAM;
	}
	else
	{
		m_cursorStyle = WG_CURSOR_DEFAULT;
	}
}

//____ goBOL() ________________________________________________________________

void WgGizmoEditline::goBOL()
{
	if( IsEditable() && m_bHasFocus )
		m_pText->goBOL();
}

//____ goEOL() ________________________________________________________________

void WgGizmoEditline::goEOL()
{
	if( IsEditable() && m_bHasFocus )
		m_pText->goEOL();
}

//____ SetPasswordGlyph() _____________________________________________________

void WgGizmoEditline::SetPasswordGlyph( Uint16 glyph )
{
	assert(glyph);
	if(glyph)
	{
		m_pwGlyph = glyph;
	}
}

//____ InsertTextAtCursor() ___________________________________________________

Uint32 WgGizmoEditline::InsertTextAtCursor( const WgCharSeq& str )
{
	if( !IsEditable() )
		return 0;

	if( !m_bHasFocus )
		if( !GrabFocus() )
			return 0;				// Couldn't get input focus...

	Uint32 retVal = 0;

	if( m_maxCharacters == 0 || ((unsigned) str.Length()) < m_maxCharacters - m_pText->nbChars() )
	{
		Uint32 line = m_pText->line();
		Uint32 column = m_pText->column();

		m_pText->putText( str );
		retVal = str.Length();
	}
	else
	{
		retVal = m_maxCharacters - m_pText->nbChars();
		m_pText->putText( WgCharSeq( str, 0, retVal ) );
	}

	AdjustViewOfs();

	return retVal;
}

//____ InsertCharAtCursor() ___________________________________________________

bool WgGizmoEditline::InsertCharAtCursor( Uint16 c )
{
	if( !IsEditable() )
		return 0;

	if( !m_bHasFocus )
		if( !GrabFocus() )
			return false;				// Couldn't get input focus...

	if( m_maxCharacters != 0 && m_maxCharacters < m_pText->nbChars() )
		return false;

	m_pText->putChar( c );
	AdjustViewOfs();
	return true;
}


//____ SetTextWrap() __________________________________________________________

bool WgGizmoEditline::SetTextWrap(bool bWrap)
{
	return !bWrap;
}

//____ OnUpdate() _____________________________________________________________

void WgGizmoEditline::OnUpdate( const WgUpdateInfo& _updateInfo )
{
	if( IsSelectable() && m_bHasFocus )
	{
		m_pText->incTime( _updateInfo.msDiff );
		RequestRender();					//TODO: Should only render the cursor and selection!
	}
}



//____ OnCloneContent() _______________________________________________________

void WgGizmoEditline::OnCloneContent( const WgGizmo * _pOrg )
{
	WgGizmoEditline * pOrg = (WgGizmoEditline*) _pOrg;

	Wg_Interface_TextHolder::OnCloneContent( pOrg );

	//TODO: Implement!
}

//____ OnRender() _____________________________________________________________

void WgGizmoEditline::OnRender( WgGfxDevice * pDevice, const WgRect& _window, const WgRect& _clip, Uint8 _layer )
{
	WgText * pText = &m_text;
	if( m_bPasswordMode )
	{
		int nChars = m_text.nbChars();
		Uint16 * pContent = new Uint16[nChars+1];
		for( int i = 0 ; i < nChars ; i++ )
			pContent[i] = m_pwGlyph;
		pContent[nChars] = 0;
		pText = new WgText( pContent );
		pText->SetWrap(false);
		pText->setAlignment(m_text.alignment());
		pText->setProperties(m_text.getProperties());
		pText->setMode(m_text.mode());
		pText->setSelectionColor(m_text.getSelectionColor());
		Uint32 sl, sc, el, ec;
		if( m_text.getSelection(sl, sc, el, ec) )
			pText->selectText(sl, sc, el, ec);
		delete [] pContent;
	}

	WgRect r = _window;
	r.x -= m_viewOfs;
	r.w += m_viewOfs;

	if( m_bHasFocus && IsEditable() )
		pDevice->PrintTextWithCursor( _clip, pText, *m_pText->GetCursor(), r );
	else
		pDevice->PrintText( _clip, pText, r );

	if( pText != &m_text )
		delete pText;
}


//____ OnAction() _____________________________________________________________

void WgGizmoEditline::OnAction( WgEmitter * pEmitter, WgInput::UserAction action, int button_key, const WgActionDetails& info, const WgInput& inputObj )
{
	if( (action == WgInput::BUTTON_PRESS || action == WgInput::BUTTON_DOWN) && button_key == 1 )
	{
		if( !m_bHasFocus )
			GrabFocus();

		if( m_bHasFocus )
		{
			if( IsSelectable() && (info.modifier & WG_MODKEY_SHIFT) )
			{
				m_pText->setSelectionMode(true);
			}

			WgCord ofs = Abs2local(WgCord(info.x,0));

			int x = ofs.x;
			int y = ofs.y;
			x += m_viewOfs;

			if( m_bPasswordMode )
			{
				WgPen	pen;
				pen.SetTextProp( m_pText->getProperties() );
				pen.SetChar(m_pwGlyph);
				pen.AdvancePos();

				int spacing = pen.GetPosX();
				int height = pen.GetLineSpacing();

				int line = y/height;
				int col = (x+spacing/2)/spacing;
				if(col < 0)
				{
					col = 0;
					line = 0;
				}
				m_pText->gotoSoftPos(line,col);
			}
			else
			{
				m_pText->gotoPixel(x, 0);
			}

			if(IsSelectable() && action == WgInput::BUTTON_PRESS && !(info.modifier & WG_MODKEY_SHIFT))
			{
				m_pText->clearSelection();
				m_pText->setSelectionMode(true);
			}
		}
		AdjustViewOfs();
	}

	if( action == WgInput::BUTTON_RELEASE || action == WgInput::BUTTON_RELEASE_OUTSIDE )
	{
		if( m_bHasFocus && button_key == 1 )
			m_pText->setSelectionMode(false);
	}

	if( action == WgInput::CHARACTER )
	{
		if( IsEditable() && m_bHasFocus && button_key >= 32 && button_key != 127)
		{
			// by default - no max limit
			if( m_maxCharacters == 0 || m_maxCharacters > m_pText->nbChars() )
			{
				if(m_pText->hasSelection())
					m_pText->delSelection();
				m_pText->setSelectionMode(false);
				m_pText->putChar( button_key );
				AdjustViewOfs();
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
				{
					if( m_bPasswordMode )
						m_pText->goBOL();
					else
						m_pText->gotoPrevWord();
				}
				else
				{
					m_pText->goLeft();
				}
				break;
			case WGKEY_RIGHT:
				if( info.modifier & WG_MODKEY_SHIFT )
					m_pText->setSelectionMode(true);

				if( info.modifier & WG_MODKEY_CTRL )
				{
					if( m_bPasswordMode )
						m_pText->goEOL();
					else
						m_pText->gotoNextWord();
				}
				else
				{
					m_pText->goRight();
				}
				break;

			case WGKEY_BACKSPACE:
				if(m_pText->hasSelection())
					m_pText->delSelection();
				else if( (info.modifier & WG_MODKEY_CTRL) && !m_bPasswordMode)
					m_pText->delPrevWord();
				else
					m_pText->delPrevChar();
				break;

			case WGKEY_DELETE:
				if(m_pText->hasSelection())
					m_pText->delSelection();
				else if( (info.modifier & WG_MODKEY_CTRL) && !m_bPasswordMode)
					m_pText->delNextWord();
				else
					m_pText->delNextChar();
				break;

			case WGKEY_HOME:

				/*
				 *	I am not sure if this is the proper way to this, but in my opinion, the default
				 *	"actions" has to be separated from any modifier key action combination
				 */
				switch( info.modifier )
				{

				case WG_MODKEY_CTRL:
					break;

				default: // no modifier key was pressed
					if( info.modifier & WG_MODKEY_SHIFT )
						m_pText->setSelectionMode(true);

					m_pText->goBOL();
					break;
				}

				break;

			case WGKEY_END:

				/*
			 	 *	I am not sure if this is the proper way to this, but in my opinion, the default
		 		 *	"actions" has to be separated from any modifier key action combination
				 */
				switch( info.modifier )
				{

				case WG_MODKEY_CTRL:
					break;

				default: // no modifier key was pressed
					if( info.modifier & WG_MODKEY_SHIFT )
						m_pText->setSelectionMode(true);

					m_pText->goEOL();
					break;
				}

				break;

			default:
				break;
		}
		AdjustViewOfs();
	}

}


//____ AdjustViewOfs() ________________________________________________________

void WgGizmoEditline::AdjustViewOfs()
{
	// Possibly move viewOfs so that:
	//	1 Cursor remains inside view.
	//  2 At least one character is displayed before the cursor
	//  3 At least one character is displayed after the cursor (if there is one).

	if( m_bHasFocus && m_pText->getFont() )
	{
		Uint32 cursCol	= m_pText->column();

		WgPen	pen;
		pen.SetTextProp( m_pText->getProperties() );
		pen.SetChar(m_pwGlyph);
		pen.AdvancePos();

		int pwAdvance	= pen.GetPosX();
		int cursWidth	= m_pText->getFont()->GetCursor()->advance(m_pText->cursorMode() );

		int cursOfs;		// Cursor offset from beginning of line in pixels.
		int maxOfs;			// Max allowed view offset in pixels.
		int minOfs;			// Min allowed view offset in pixels.


		// Calculate cursOfs

		if( m_bPasswordMode )
			cursOfs = cursCol * pwAdvance;
		else
			cursOfs	= m_pText->getLineWidthPart( 0, 0, cursCol );

		// Calculate maxOfs

		if( cursCol > 0 )
		{
			if( m_bPasswordMode )
				maxOfs = (cursCol-1) * pwAdvance;
			else
				maxOfs = m_pText->getLineWidthPart( 0, 0, cursCol-1 );
		}
		else
			maxOfs = cursOfs;

		// Calculate minOfs

		Uint32 geoWidth = Size().w;

		if( cursCol < m_pText->getLine(0)->nChars )
		{
			if( m_bPasswordMode )
				minOfs = (cursCol+1) * pwAdvance + cursWidth - geoWidth;
			else
				minOfs = m_pText->getLineWidthPart( 0, 0, cursCol+1 ) + cursWidth - geoWidth;	// Not 100% right, cursor might affect linewidth different from its own width.
		}
		else
			minOfs = cursOfs + cursWidth - geoWidth;

		// Check boundaries and update

		if( m_viewOfs > maxOfs )
			m_viewOfs = maxOfs;

		if( m_viewOfs < minOfs )
			m_viewOfs = minOfs;


	}
	else
		m_viewOfs = 0;				// Show beginning of line when cursor disappears.
}



//____ OnEnable() _____________________________________________________________

void WgGizmoEditline::OnEnable()
{
	m_text.setMode(WG_MODE_NORMAL);
	RequestRender();
}

//____ OnDisable() ____________________________________________________________

void WgGizmoEditline::OnDisable()
{
	m_text.setMode(WG_MODE_DISABLED);
	RequestRender();
}

//____ OnGotInputFocus() ______________________________________________________

void WgGizmoEditline::OnGotInputFocus()
{
	m_bHasFocus = true;

	if( IsEditable() )
	{
		m_pText->GetCursor()->goEOL();
		RequestRender(); // render with cursor on
	}
}

//____ OnLostInputFocus() _____________________________________________________

void WgGizmoEditline::OnLostInputFocus()
{
	m_bHasFocus = false;

	if( IsSelectable() )
	{
		m_pText->clearSelection();
		m_pText->setSelectionMode(false);
	}

	if( IsEditable() || m_viewOfs != 0 )
	{
		m_viewOfs = 0;
		RequestRender();
	}
}

//____ OnNewSize() ____________________________________________________________

void WgGizmoEditline::OnNewSize( const WgSize& size )
{
	AdjustViewOfs();
	RequestRender();
}


//____ TextModified() _________________________________________________________

void WgGizmoEditline::TextModified()
{
	RequestRender();
	AdjustViewOfs();
}

