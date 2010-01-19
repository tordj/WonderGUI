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
	m_pMyCursor		= 0;
	m_maxCharacters = 0;
	m_bEditable		= false;

	m_newlineKey	= WGKEY_RETURN;
	m_newlineModif	= WG_MODKEY_NONE;

	m_text.setLineWidth( Width() );
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

//____ SetEditable() __________________________________________________________
void Wdg_TextView::SetEditable( bool bEditable )
{
	m_bEditable = bEditable;
}

//____ goBOL() ________________________________________________________________
void Wdg_TextView::goBOL()
{
	if( m_pMyCursor )
		m_pMyCursor->goBOL();
	AdjustViewOfs();
}

//____ goEOL() ________________________________________________________________
void Wdg_TextView::goEOL()
{
	if( m_pMyCursor )
		m_pMyCursor->goEOL();
	AdjustViewOfs();
}

//____ goBOF() ________________________________________________________________
void Wdg_TextView::goBOF()
{
	if( m_pMyCursor )
		m_pMyCursor->goBOF();
	AdjustViewOfs();
}

//____ goEOF() ________________________________________________________________
void Wdg_TextView::goEOF()
{
	if( m_pMyCursor )
		m_pMyCursor->goEOF();
	AdjustViewOfs();
}


//____ DoMyOwnUpdate() ________________________________________________________

void	Wdg_TextView::DoMyOwnUpdate( const WgUpdateInfo& _updateInfo )
{

	if( m_pMyCursor )
	{
		m_pMyCursor->incTime( _updateInfo.msDiff );
		RequestRender();					//TODO: Should only render the cursor!
	}
}


//____ DoMyOwnRender() ________________________________________________________

void Wdg_TextView::DoMyOwnRender( const WgRect& _window, const WgRect& _clip, Uint8 _layer )
{
	WgText * pText = &m_text;

	WgRect contentRect( _window.x - ViewPixelOfsX(), _window.y - ViewPixelOfsY(), ContentWidth(), ContentHeight() );

	if( m_pMyCursor )
		WgGfx::clipPrintTextWithCursor( _clip, pText, *m_pMyCursor, contentRect );
	else
		WgGfx::clipPrintText( _clip, pText, contentRect );

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
	if( action == WgInput::BUTTON_PRESS && button_key == 1 )
	{
		if( m_pMyCursor )
		{
			int x = info.x;
			int y = info.y;
			Abs2local( &x, &y );

			m_pMyCursor->gotoPixel(x,y);
			AdjustViewOfs();
		}
		else
			GrabInputFocus();
	}

	if( m_pMyCursor && action == WgInput::CHARACTER )
	{
		if( button_key >= 32 )
		{
			// by default - no max limit
			if( m_maxCharacters == 0 || m_maxCharacters > m_pMyCursor->text()->nbChars() )
				m_pMyCursor->putChar( button_key );
		}
//		if( button_key == 13 )
//				m_pMyCursor->putChar( '\n' );

		if( button_key == '\t' )
				m_pMyCursor->putChar( '\t' );

		SetContentSize( m_text.width(), m_text.height() );
		AdjustViewOfs();
	}

	if( m_pMyCursor && (action == WgInput::KEY_PRESS || action == WgInput::KEY_REPEAT) )
	{
		switch( button_key )
		{
			case WGKEY_LEFT:
				if( info.modifier == WG_MODKEY_CTRL )
					m_pMyCursor->gotoPrevWord();
				else
					m_pMyCursor->goLeft();
				AdjustViewOfs();
				break;
			case WGKEY_RIGHT:
				if( info.modifier == WG_MODKEY_CTRL )
					m_pMyCursor->gotoNextWord();
				else
					m_pMyCursor->goRight();
				AdjustViewOfs();
				break;

			case WGKEY_UP:
				m_pMyCursor->goUp();
				AdjustViewOfs();
				break;

			case WGKEY_DOWN:
				m_pMyCursor->goDown();
				AdjustViewOfs();
				break;

			case WGKEY_BACKSPACE:
				m_pMyCursor->delPrevChar();
				AdjustViewOfs();
				break;

			case WGKEY_DELETE:
				m_pMyCursor->delNextChar();
				AdjustViewOfs();
				break;

			case WGKEY_HOME:
				if( info.modifier == WG_MODKEY_CTRL )
					m_pMyCursor->goBOF();
				else
					m_pMyCursor->goBOL();
				AdjustViewOfs();
				break;

			case WGKEY_END:
				if( info.modifier == WG_MODKEY_CTRL )
					m_pMyCursor->goEOF();
				else
					m_pMyCursor->goEOL();
				AdjustViewOfs();
				break;

			default:
				if( button_key == m_newlineKey && info.modifier == m_newlineModif )
					m_pMyCursor->putChar( '\n' );
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
		if( m_bEditable )
		{
			m_pMyCursor = new WgCursorInstance( m_text );
			AdjustViewOfs();
		}
	}
	else
	{
		delete m_pMyCursor;
		m_pMyCursor = 0;
	}

	RequestRender();
}

//____ TextModified() _________________________________________________________

void Wdg_TextView::TextModified()
{
	if( m_pMyCursor )
	{
		m_pMyCursor->gotoHardPos( m_pMyCursor->line(), m_pMyCursor->column() );
	}

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
	if( !m_bEditable )
		return 0;

	if( !m_pMyCursor )
		if( !GrabInputFocus() )
			return 0;				// Couldn't get input focus...

	Uint32 retVal = 0;

	if( m_maxCharacters == 0 || ((unsigned) str.Length()) < m_maxCharacters - m_pMyCursor->text()->nbChars() )
	{
		m_pMyCursor->putText( str.GetUnicode().ptr , str.Length() );
		retVal = str.Length();
	}
	else
	{
		retVal = m_maxCharacters - m_pMyCursor->text()->nbChars();
		m_pMyCursor->putText( str.GetUnicode().ptr, retVal );
	}

	AdjustViewOfs();

	return retVal;
}

//____ InsertCharAtCursor() ___________________________________________________

bool Wdg_TextView::InsertCharAtCursor( Uint16 c )
{
	if( !m_bEditable )
		return 0;

	if( !m_pMyCursor )
		if( !GrabInputFocus() )
			return false;				// Couldn't get input focus...

	if( m_maxCharacters != 0 && m_maxCharacters < m_pMyCursor->text()->nbChars() )
		return false;

	m_pMyCursor->putChar( c );
	AdjustViewOfs();
	return true;
}


//____ AdjustViewOfs() ________________________________________________________

void Wdg_TextView::AdjustViewOfs()
{
	// Possibly move viewOfs horizontally so that:
	//	1 Cursor remains inside view.
	//  2 At least one character is displayed before the cursor
	//  3 At least one character is displayed after the cursor (if there is one).

	if( m_pMyCursor && m_pText->getFontSet() )
	{
		Uint32 cursCol, cursLine;

		m_pMyCursor->getSoftPos(cursLine, cursCol);
		int cursWidth	= m_pText->getFontSet()->GetCursor()->spacing(m_pMyCursor->mode() );

		int cursOfs;		// Cursor offset from beginning of line in pixels.
		int maxOfs;			// Max allowed view offset in pixels.
		int minOfs;			// Min allowed view offset in pixels.


		// Calculate cursOfs

		cursOfs	= m_pText->getLineWidthPart( 0, 0, cursCol );

		// Calculate maxOfs

		if( cursCol > 0 )
			maxOfs = m_pText->getLineWidthPart( 0, 0, cursCol-1 );
		else
			maxOfs = cursOfs;

		// Calculate minOfs

		Uint32 geoWidth = ViewPixelLenX();

		if( cursCol < m_pText->getLine(0)->nChars )
			minOfs = m_pText->getLineWidthPart( 0, 0, cursCol+1 ) + cursWidth - geoWidth;	// Not 100% right, cursor might affect linewidth different from its own width.
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

		for( Uint32 i = 0 ; i < cursLine ; i++ )
			lineBegY += m_text.softLineHeight(i);

		Uint32 lineEndY = lineBegY + m_text.softLineHeight(cursLine);

		Uint32 viewOfsY = ViewPixelOfsY();
		Uint32 viewLenY = ViewPixelLenY();

		if( lineBegY < viewOfsY )
			SetViewPixelOfsY( lineBegY );

		if( lineEndY > (viewOfsY + viewLenY) )
			SetViewPixelOfsY( lineEndY - viewLenY );

	}




}
