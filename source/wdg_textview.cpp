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

#include	<wdg_textview.h>
#include	<wg_key.h>
#include	<wg_font.h>
#include	<wg_gfx.h>

static const char	Wdg_Type[] = {"TordJ/EditTextView"};

//____ NewOfMyType() __________________________________________________________

WgWidget * Wdg_TextView::NewOfMyType() const
{
	return new Wdg_TextView;
}


//____ Init() _________________________________________________________________

void Wdg_TextView::Init()
{
	m_pText			= &m_text;

	m_newlineKey	= WG_KEY_RETURN;
	m_newlineModif	= WG_MODKEY_NONE;

	m_text.setLineWidth( Width() );
	m_text.SetAutoEllipsis(IsAutoEllipsisDefault());	

}


//____ Destructor _____________________________________________________________

Wdg_TextView::~Wdg_TextView()
{
}


//____ Type() _________________________________________________________________

const char * Wdg_TextView::Type() const
{
	return GetMyType();
}

const char * Wdg_TextView::GetMyType( void )
{
	return Wdg_Type;
}

//______________________________________________________________
void Wdg_TextView::SetEditMode(WgTextEditMode mode)
{
	m_text.SetEditMode( mode );
}

//____ goBOL() ________________________________________________________________
void Wdg_TextView::goBOL()
{
	m_pText->goBOL();
	AdjustViewOfs();
}

//____ goEOL() ________________________________________________________________
void Wdg_TextView::goEOL()
{
	m_pText->goEOL();
	AdjustViewOfs();
}

//____ goBOF() ________________________________________________________________
void Wdg_TextView::goBOF()
{
	m_pText->goBOF();
	AdjustViewOfs();
}

//____ goEOF() ________________________________________________________________
void Wdg_TextView::goEOF()
{
	m_pText->goEOF();
	AdjustViewOfs();
}


//____ DoMyOwnUpdate() ________________________________________________________

void	Wdg_TextView::DoMyOwnUpdate( const WgUpdateInfo& _updateInfo )
{
	if(m_pText->incTime( _updateInfo.msDiff ))
		RequestRender();					//TODO: Should only render the cursor!
}


//____ DoMyOwnRender() ________________________________________________________

void Wdg_TextView::DoMyOwnRender( const WgRect& _window, const WgRect& _clip, Uint8 _layer )
{
	WgText * pText = &m_text;

	WgRect contentRect( _window.x - ViewPixelOfsX(), _window.y - ViewPixelOfsY(), ContentWidth(), ContentHeight() );

	WgGfx::printText( _clip, pText, contentRect );

	if( pText != &m_text )
		delete pText;
}

//____ DoMyOwnRefresh() _______________________________________________________

void Wdg_TextView::DoMyOwnRefresh( void )
{
}

//____ DoMyOwnActionRespond() _________________________________________________

void Wdg_TextView::DoMyOwnActionRespond( WgInput::UserAction action, int button_key, const WgActionDetails& info, const WgInput& inputObj )
{

	if( m_pText->GetCursor() && (action == WgInput::BUTTON_PRESS || action == WgInput::BUTTON_DOWN) && button_key == 1 )
	{
		if( (info.modifier & WG_MODKEY_SHIFT) )
		{
			m_pText->setSelectionMode(true);
		}

		int x = info.x;
		int y = info.y;
		Abs2local( &x, &y );

		m_pText->CursorGotoCoord( WgCoord(x,y), WgRect(0-m_viewPixOfsX,0-m_viewPixOfsY,m_contentWidth,m_contentHeight) );
		AdjustViewOfs();
//		m_pText->CursorGotoCoord( WgCoord(info.x, info.y), ScreenGeometry() );

		if( action == WgInput::BUTTON_PRESS && !(info.modifier & WG_MODKEY_SHIFT))
		{
			m_pText->clearSelection();
			m_pText->setSelectionMode(true);
		}
	}
	else if( action == WgInput::BUTTON_RELEASE || action == WgInput::BUTTON_RELEASE_OUTSIDE )
	{
		if(m_pText->GetCursor() && button_key == 1)
			m_pText->setSelectionMode(false);
	}
	else if( !m_pText->GetCursor() && IsEditable() && action == WgInput::BUTTON_PRESS && button_key == 1 )
	{
		GrabInputFocus();
	}

	if( m_pText->GetCursor() && action == WgInput::CHARACTER )
	{
		if( (button_key >= 32  && button_key != 127) || button_key == '\t' /* button_key == 13 */ )
		{
			InsertCharAtCursorInternal( button_key );
		}
	}

	if( action == WgInput::KEY_RELEASE && m_pText->GetCursor() )
	{
		switch( button_key )
		{
			case WG_KEY_SHIFT:
				if(!inputObj.isButtonDown(1))
					m_pText->setSelectionMode(false);
			break;
		}
	}

	if( m_pText->GetCursor() && (action == WgInput::KEY_PRESS || action == WgInput::KEY_REPEAT) )
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
				AdjustViewOfs();
				break;
			case WG_KEY_RIGHT:
				if( info.modifier & WG_MODKEY_SHIFT )
					m_pText->setSelectionMode(true);

				if( info.modifier & WG_MODKEY_CTRL )
					m_pText->gotoNextWord();
				else
					m_pText->goRight();
				AdjustViewOfs();
				break;

			case WG_KEY_UP:
				if( info.modifier & WG_MODKEY_SHIFT )
					m_pText->setSelectionMode(true);

				m_pText->CursorGoUp(1,m_geo);
				AdjustViewOfs();
				break;

			case WG_KEY_DOWN:
				if( info.modifier & WG_MODKEY_SHIFT )
					m_pText->setSelectionMode(true);

				m_pText->CursorGoDown(1,m_geo);
				AdjustViewOfs();
				break;

			case WG_KEY_BACKSPACE:
				if(m_pText->hasSelection())
					m_pText->delSelection();
				else if( info.modifier & WG_MODKEY_CTRL )
					m_pText->delPrevWord();
				else
					m_pText->delPrevChar();
				AdjustViewOfs();
				break;

			case WG_KEY_DELETE:
				if(m_pText->hasSelection())
					m_pText->delSelection();
				else if( info.modifier & WG_MODKEY_CTRL )
					m_pText->delNextWord();
				else
					m_pText->delNextChar();
				AdjustViewOfs();
				break;

			case WG_KEY_HOME:
				if( info.modifier & WG_MODKEY_SHIFT )
					m_pText->setSelectionMode(true);

				if( info.modifier & WG_MODKEY_CTRL )
					m_pText->goBOF();
				else
					m_pText->goBOL();
				AdjustViewOfs();
				break;

			case WG_KEY_END:
				if( info.modifier & WG_MODKEY_SHIFT )
					m_pText->setSelectionMode(true);

				if( info.modifier & WG_MODKEY_CTRL )
					m_pText->goEOF();
				else
					m_pText->goEOL();
				AdjustViewOfs();
				break;

			default:
				if( button_key == m_newlineKey && info.modifier == m_newlineModif && button_key != WG_KEY_UNMAPPED )
					m_pText->putChar( '\n' );
				break;
		}
	}
}

//____ DoMyOwnCloning() _______________________________________________________

void Wdg_TextView::DoMyOwnCloning( WgWidget * _pClone, const WgWidget * _pCloneRoot, const WgWidget * _pBranchRoot )
{
}

//____ DoMyOwnMarkTest() ______________________________________________________

bool Wdg_TextView::DoMyOwnMarkTest( int _x, int _y )
{
	return true;																				// Accept all at least for now...
}

//____ DoMyOwnDisOrEnable() ___________________________________________________

void Wdg_TextView::DoMyOwnDisOrEnable( void )
{
	if( m_bEnabled )
		m_text.setMode(WG_MODE_NORMAL);
	else
		m_text.setMode(WG_MODE_DISABLED);

	RequestRender();
}


//____ DoMyOwnGeometryChangeSubclass() ________________________________________

void	Wdg_TextView::DoMyOwnGeometryChangeSubclass( WgRect& oldGeo, WgRect& newGeo )
{
	if( newGeo.w != oldGeo.w )
	{
		m_text.setLineWidth( newGeo.w );
		SetContentSize( m_text.width(), m_text.height() );
	}
}


//____ DoMyOwnInputFocusChange() ______________________________________________

void Wdg_TextView::DoMyOwnInputFocusChange( bool _bFocus )
{
	if( _bFocus )
	{
		if( IsEditable() )
		{
			m_pText->showCursor();
			AdjustViewOfs();
		}
	}
	else
	{
		m_pText->hideCursor();
	}

	RequestRender();
}

//____ _textModified() _________________________________________________________

void Wdg_TextView::_textModified()
{
	SetContentSize( m_text.width(), m_text.height() );
	RequestRender();
}

//_____________________________________________________________________________
void Wdg_TextView::SetNewlineCombo( WgKey key, WgModifierKeys modifiers )
{
	m_newlineKey	= key;
	m_newlineModif	= modifiers;
}


//____ InsertTextAtCursor() ___________________________________________________

Uint32 Wdg_TextView::InsertTextAtCursor( const WgCharSeq& str )
{
	if( !IsEditable() )
		return 0;

	if( !m_pText->GetCursor() )
		if( !GrabInputFocus() )
			return 0;				// Couldn't get input focus...

	Uint32 retVal = m_pText->putText( str );

	AdjustViewOfs();

	return retVal;
}

//____ InsertCharAtCursor() ___________________________________________________

bool Wdg_TextView::InsertCharAtCursor( Uint16 c )
{
	if( !IsEditable() )
		return 0;

	if( !m_pText->GetCursor() )
		if( !GrabInputFocus() )
			return false;				// Couldn't get input focus...

	return InsertCharAtCursorInternal(c);
}

bool Wdg_TextView::InsertCharAtCursorInternal( Uint16 c )
{
	if(m_pText->hasSelection())
		m_pText->delSelection();
	m_pText->setSelectionMode(false);

	if( m_pText->putChar( c ) )
	{
		SetContentSize( m_text.width(), m_text.height() );
		AdjustViewOfs();
	}
	return true;
}


//____ AdjustViewOfs() ________________________________________________________

void Wdg_TextView::AdjustViewOfs()
{
	// Possibly move viewOfs horizontally so that:
	//	1 Cursor remains inside view.
	//  2 At least one character is displayed before the cursor
	//  3 At least one character is displayed after the cursor (if there is one).

	if( m_pText->GetCursor() && m_pText->getFont() )
	{
		int cursCol, cursLine;

		m_pText->getSoftPos(cursLine, cursCol);

		WgCursor * pCursor = WgTextTool::GetCursor( m_pText );
		if( !pCursor )
			return;

		int cursWidth	= pCursor->Advance(m_pText->cursorMode() );

		int cursOfs;		// Cursor offset from beginning of line in pixels.
		int maxOfs;			// Max allowed view offset in pixels.
		int minOfs;			// Min allowed view offset in pixels.


		// Calculate cursOfs

		cursOfs	= m_pText->getSoftLineWidthPart( 0, 0, cursCol );

		// Calculate maxOfs

		if( cursCol > 0 )
			maxOfs = m_pText->getSoftLineWidthPart( 0, 0, cursCol-1 );
		else
			maxOfs = cursOfs;

		// Calculate minOfs

		Uint32 geoWidth = ViewPixelLenX();

		if( cursCol < m_pText->getLine(0)->nChars )
			minOfs = m_pText->getSoftLineWidthPart( 0, 0, cursCol+1 ) + cursWidth - geoWidth;	// Not 100% right, cursor might affect linewidth different from its own width.
		else
			minOfs = cursOfs + cursWidth - geoWidth;

		// Check boundaries and update

		if( (int) ViewPixelOfsX() > maxOfs )
			SetViewPixelOfsX( maxOfs );

		if( (int) ViewPixelOfsX() < minOfs )
			SetViewPixelOfsX( minOfs );

		//
		// Make sure that cursors line is scrolled into view
		//

		Uint32 lineBegY = 0;

		for( int i = 0 ; i < cursLine ; i++ )
			lineBegY += m_text.softLineSpacing(i);

		Uint32 lineEndY = lineBegY + m_text.softLineHeight(cursLine);

		Uint32 viewOfsY = ViewPixelOfsY();
		Uint32 viewLenY = ViewPixelLenY();

		if( lineBegY < viewOfsY )
			SetViewPixelOfsY( lineBegY );

		if( lineEndY > (viewOfsY + viewLenY) )
			SetViewPixelOfsY( lineEndY - viewLenY );

	}




}
