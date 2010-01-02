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


#include <wg_cursorinstance.h>

#ifndef WG_TEXT_DOT_H
#	include <wg_text.h>
#endif

#ifndef WG_TEXTTOOL_DOT_H
#	include <wg_texttool.h>
#endif

#ifndef WG_FONTSET_DOT_H
#	include <wg_font.h>
#endif


//____ Constructor() __________________________________________________________

WgCursorInstance::WgCursorInstance( WgText& text )
{
	m_pText 		= &text;
	m_bInsert		= true;
	m_wantedOfsX	= -1;

	m_time 			= 0;

	m_line 			= text.nbLines()-1;
	m_column 		= text.getLine(m_line)->nChars;
}

//____ incTime() _______________________________________________________________

bool WgCursorInstance::incTime( Uint32 ms )
{
	m_time += ms;
	return true; //TODO: Should only return true if cursor needs to be re-rendered.
}

//____ insertMode() ____________________________________________________________

void WgCursorInstance::insertMode( bool bInsert )
{
	m_bInsert = bInsert;
}


//____ gotoHardLine() _________________________________________________________

void WgCursorInstance::gotoHardLine( Uint32 line )
{
	Uint32 maxLine = m_pText->nbLines()-1;

	if( m_wantedOfsX == -1 )
		m_wantedOfsX = ofsX();

	if( line > maxLine )
		line = maxLine;

	m_line = line;
	m_column = WgTextTool::ofsX2column( m_pText->getDefaultProperties(), m_pText->mode(), m_wantedOfsX, m_pText->getLineText(line) );
}

//____ gotoSoftLine() _________________________________________________________

void WgCursorInstance::gotoSoftLine( Uint32 line )
{
	// First we need to get our soft column...

	Uint32	ln = m_line;
	Uint32	col = m_column;
	m_pText->posHard2Soft( ln, col );

	if( m_wantedOfsX == -1 )
		m_wantedOfsX = m_pText->getSoftLineWidthPart( ln, 0, col );

	// Set our line and convert to hard	

	ln = line;

	WgChar * pStr = m_pText->getSoftLineText(ln);
	if( pStr )
		col = WgTextTool::ofsX2column( m_pText->getDefaultProperties(), m_pText->mode(), m_wantedOfsX, pStr );


	m_pText->posSoft2Hard( ln, col );

	// Set the result

	m_line = ln;
	m_column = col;
}


//____ gotoColumn() ____________________________________________________________

void WgCursorInstance::gotoColumn( Sint32 col )
{
	const Sint32 minLine = 0;
	const Sint32 maxLine = m_pText->nbLines()-1;

	Uint32 maxCol = m_pText->getLine(m_line)->nChars;


	// Check whether to hop down/up on lines
	if(col < 0)
	{
		Sint32 line = m_line - 1;

		// Have we reached the top?
		if(line < minLine)
		{
			// Yes
			m_line = minLine;
			col = 0;
		}
		else
		{
			// No, set cursor column to end of line
			m_line = line;
			col = m_pText->getLine(m_line)->nChars;

		}
	}
	else if( col > (Sint32)maxCol )
	{
		Sint32 line = m_line + 1;

		// Have we reached the bottom?
		if(line > maxLine)
		{
			// Yes, set cursor on end of line
			m_line = maxLine;
			col = m_pText->getLine(m_line)->nChars;
		}
		else
		{
			// No, set cursor column to 0
			m_line = line;
			col = 0;

		}
	}


	m_column = col;
	m_wantedOfsX = -1;
}

//____ gotoPrevWord() _________________________________________________________

void WgCursorInstance::gotoPrevWord()
{
	WgChar * pLine = m_pText->getLineText(m_line);
	Uint32 col = m_column;

	while( col > 0 && (pLine[col-1].GetGlyph() == ' ' || pLine[col-1].GetGlyph() == '\t') )
		col--;

	while( col > 0 && pLine[col-1].GetGlyph() != ' ' && pLine[col-1].GetGlyph() != '\t' && pLine[col].GetGlyph() != ',' && pLine[col].GetGlyph() != '.' && pLine[col].GetGlyph() != ')' && pLine[col].GetGlyph() != '(' )
		col--;

	m_column = col;
	m_wantedOfsX = -1;
}


//____ gotoNextWord() _________________________________________________________

void WgCursorInstance::gotoNextWord()
{
	WgChar *pLine	= m_pText->getLineText(m_line);
	Uint32	maxCol	= m_pText->getLine(m_line)->nChars;
	Uint32	col		= m_column;


	while( col < maxCol && pLine[col].GetGlyph() != ' ' && pLine[col].GetGlyph() != '\t' && pLine[col].GetGlyph() != ',' && pLine[col].GetGlyph() != '.' && pLine[col].GetGlyph() != ')' && pLine[col].GetGlyph() != '(' )
		col++;

	while( col < maxCol && (pLine[col].GetGlyph() == ' ' || pLine[col].GetGlyph() == '\t') )
		col++;

	m_column = col;
	m_wantedOfsX = -1;
}

//____ gotoHardPos() __________________________________________________________

void WgCursorInstance::gotoHardPos( Uint32 line, Uint32 col )
{
	gotoPos( line, col );
	m_wantedOfsX = -1;
}


//____ gotoPos() ______________________________________________________________

void WgCursorInstance::gotoPos( Uint32 line, Uint32 col )
{
	Uint32 maxLine = m_pText->nbLines()-1;
	if( line > maxLine )
		line = maxLine;

	m_line = line;

	Uint32 maxCol = m_pText->getLine(m_line)->nChars;
	if( col > maxCol )
		col = maxCol;

	m_column = col;
}

//____ gotoPixel() _____________________________________________________________

void WgCursorInstance::gotoPixel( Sint32 x, Sint32 y )
{

//	TODO: Needs to be able to handle text which isn't left-justified.
//  TODO: Needs to be able to handle text which varies in height.

	WgGlyphSet * pFont = m_pText->getFontSet()->GetGlyphSet(WG_STYLE_NORMAL);
	Uint32 line = y / pFont->height();								//TODO: Need a better way here...

	if( line < 0 )
		line = 0;
	else if( line >= m_pText->nbSoftLines() )
		line = m_pText->nbSoftLines()-1;

	WgCursorInstance * pCursorOnLine = 0;

	if( m_line == line )
		pCursorOnLine = this;
	else
		m_line = line;

	m_column = WgTextTool::ofsX2column( m_pText->getDefaultProperties(), m_pText->mode(), x, m_pText->getSoftLineText(line), pCursorOnLine );

	m_pText->posSoft2Hard( m_line, m_column );	

	m_wantedOfsX = -1;
}


//____ putChar() ______________________________________________________________

bool WgCursorInstance::putChar( Uint16 character )
{
	m_wantedOfsX = -1;

	bool ret;

	if( m_bInsert )
		ret = m_pText->insertChar( m_line, m_column++, character );
	else
		ret = m_pText->replaceChar( m_line, m_column++, character );

	if( character == '\n' )
	{
		m_line++;
		m_column = 0;
	}

	return ret;
}

//____ putText() ______________________________________________________________

Uint32	WgCursorInstance::putText( const Uint16 * pString )
{
	m_wantedOfsX = -1;

	Uint32 nInserted;

	if( m_bInsert )
		nInserted = m_pText->insertText( m_line, m_column, pString );
	else
	{
		int nChar = 0;
		while( pString[nChar] != 0 )
			nChar++;

		nInserted = m_pText->replaceText( m_line, m_column, nChar, pString );
	}
	m_column += nInserted;
	return nInserted;
}

Uint32	WgCursorInstance::putText( const Uint16 * pString, int nChar )
{
	m_wantedOfsX = -1;

	Uint32 nInserted;

	if( m_bInsert )
		nInserted = m_pText->insertText( m_line, m_column, pString, nChar );
	else
		nInserted = m_pText->replaceText( m_line, m_column, nChar, pString );

	m_column += nInserted;
	return nInserted;
}

//____ delPrevChar() __________________________________________________________

bool WgCursorInstance::delPrevChar()
{
	m_wantedOfsX = -1;

	if( m_column == 0 )
	{
		if( m_line == 0 )
			return false;

		m_line -= 1;
		m_column = m_pText->getLine(m_line)->nChars;
		if( m_pText->joinLines( m_line ) == 0 )
			return false;

		return true;
	}
	else
 		return m_pText->removeChar( m_line, --m_column );
}

//____ delNextChar() __________________________________________________________

bool WgCursorInstance::delNextChar()
{
	m_wantedOfsX = -1;

	if( m_column == m_pText->getLine( m_line )->nChars )
	{
		if( m_line == m_pText->nbLines()-1 )
			return false;

		if( m_pText->joinLines( m_line ) == 0 )
			return false;

		return true;
	}
	else
		return m_pText->removeChar( m_line, m_column );
}

//____ ofsX() __________________________________________________________________

int WgCursorInstance::ofsX() const
{
	Uint32	ln = m_line;
	Uint32	col = m_column;
	m_pText->posHard2Soft( ln, col );

	return m_pText->getSoftLineWidthPart( ln, 0, col );
}

//____ ofsY() __________________________________________________________________

int WgCursorInstance::ofsY() const
{
	//TODO: Need a better way...

	return m_pText->getFontSet()->GetGlyphSet(WG_STYLE_NORMAL)->height()*m_line;
}

//____ mode() __________________________________________________________________

WgCursor::Mode WgCursorInstance::mode() const
{
	if( m_pText->getLine(m_line)->nChars == m_column )
		return WgCursor::EOL;

	if( m_bInsert )
		return WgCursor::INS;

	return WgCursor::OVR;
}

//____ gotoSoftPos() __________________________________________________________

void WgCursorInstance::gotoSoftPos( Uint32 line, Uint32 col )
{
	m_line = line;
	m_column = col;

	m_pText->posSoft2Hard( m_line, m_column );

	m_wantedOfsX = -1;
}

//____ getSoftPos() ___________________________________________________________

void WgCursorInstance::getSoftPos( Uint32 &line, Uint32 &col ) const
{
	line	= m_line;
	col		= m_column;

	m_pText->posHard2Soft( line, col );
}


//____ goBOL() ________________________________________________________________

void WgCursorInstance::goBOL()
{
	Uint32	ln = m_line;
	Uint32	col = m_column;
	m_pText->posHard2Soft( ln, col );

	col = 0;
	m_pText->posSoft2Hard( ln, col );
	gotoHardPos( ln, col );

	m_wantedOfsX = -1;
}

//____ goEOL() ________________________________________________________________

void WgCursorInstance::goEOL()
{
	Uint32	ln = m_line;
	Uint32	col = m_column;
	m_pText->posHard2Soft( ln, col );

	col = 0xFFFFFFFF;

	m_pText->posSoft2Hard( ln, col );
	gotoHardPos( ln, col );

	m_wantedOfsX = -1;
}



//____ goUp() _________________________________________________________________

void WgCursorInstance::goUp( Uint32 nLines )
{
	Uint32	ln = m_line;
	Uint32	col = m_column;
	m_pText->posHard2Soft( ln, col );

	if( m_wantedOfsX == -1 )
		m_wantedOfsX = m_pText->getSoftLineWidthPart( ln, 0, col );

	if( ln != 0 )
	{
		if( nLines > ln )
			ln = 0;
		else
			ln -= nLines;
		
		col = WgTextTool::ofsX2column( m_pText->getDefaultProperties(), m_pText->mode(), m_wantedOfsX, m_pText->getSoftLineText(ln) );

		m_pText->posSoft2Hard(ln,col);
		gotoPos(ln, col);
	}
}

//____ goDown() _______________________________________________________________

void WgCursorInstance::goDown( Uint32 nLines )
{
	Uint32	ln = m_line;
	Uint32	col = m_column;
	m_pText->posHard2Soft( ln, col );

	if( m_wantedOfsX == -1 )
		m_wantedOfsX = m_pText->getSoftLineWidthPart( ln, 0, col );

	if( nLines + ln < ln )
		ln = 0xFFFFFFFF;
	else
		ln += nLines;

	WgChar * pStr = m_pText->getSoftLineText(ln);
	if( pStr )
		col = WgTextTool::ofsX2column( m_pText->getDefaultProperties(), m_pText->mode(), m_wantedOfsX, pStr );

	m_pText->posSoft2Hard(ln,col);
	gotoPos(ln, col);
}

