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


#include <string.h>

#include <wg_simpletext.h>
#include <wg_texttool.h>

//_____ WgSimpleText() ________________________________________________________

WgSimpleText::WgSimpleText()
{
	m_pLines = &WgText::g_emptyLine;
	m_nLines = 1;
}

WgSimpleText::WgSimpleText( const char * pText )
{
	m_pLines = &WgText::g_emptyLine;
	m_nLines = 1;
	addText(pText);
}

WgSimpleText::WgSimpleText( const Uint16 * pText )
{
	m_pLines = &WgText::g_emptyLine;
	m_nLines = 1;
	addText(pText);
}

//_____ ~WgSimpleText() _______________________________________________________

WgSimpleText::~WgSimpleText()
{
	if( m_pLines != &WgText::g_emptyLine)
		delete [] (char *) m_pLines;
}

//_____ getLines() ____________________________________________________________

const WgTextLine * WgSimpleText::getLines() const
{
	return m_pLines;
}



//_____ getLine() _____________________________________________________________

WgTextLine * WgSimpleText::getLine( Uint32 line ) const
{
	if( line >= m_nLines )
		return 0;

	return m_pLines + line;
}

//_____ getLineText() _________________________________________________________

WgChar * WgSimpleText::getLineText( Uint32 line ) const
{
	if( line >= m_nLines )
		return 0;

	return m_pLines[line].pText;
}

//____ getSoftLines() _________________________________________________________

const WgTextLine * WgSimpleText::getSoftLines() const
{
	return m_pLines;
}

//____ getSoftLine() __________________________________________________________

WgTextLine * WgSimpleText::getSoftLine( Uint32 line ) const
{
	if( line >= m_nLines )
		return 0;

	return m_pLines + line;
}

//____ getSoftLineText() ______________________________________________________

WgChar * WgSimpleText::getSoftLineText( Uint32 line ) const
{
	if( line >= m_nLines )
		return 0;

	return m_pLines[line].pText;
}

//____ nbSoftLines() __________________________________________________________

Uint32 WgSimpleText::nbSoftLines() const
{
	return	m_nLines;
}

//____ beginChange() __________________________________________________________

WgChar * WgSimpleText::beginChange(   Uint32 line, Uint32 col, Uint32 addChar, Uint32 delChar,
										Uint32 addLines, Uint32 delLines, WgChar ch )
{
	// Calculate old and new size

	int oldSize = sizeof(WgTextLine)*m_nLines;

	for( int i = 0 ; i < (int) m_nLines ; i++ )
		oldSize += (m_pLines[i].nChars+1)*sizeof(WgChar);

	int newSize = oldSize + (addLines-delLines)*sizeof(WgTextLine) + (addChar-delChar)*sizeof(WgChar);

	// Reallocate and copy content

	int	nLines = m_nLines + addLines - delLines;

	char * pBuffer = new char[newSize];
	WgChar * wpContent	= (WgChar *) (pBuffer + sizeof(WgTextLine)* nLines);
	WgTextLine * wpLine = (WgTextLine *) pBuffer;

	int textReposition = wpContent - m_pLines[0].pText;

	// Copy text preceding change

	int nChars = (m_pLines[line].pText + col - m_pLines[0].pText);
	memcpy( wpContent, m_pLines[0].pText, nChars * sizeof(WgChar) );

	wpContent += nChars;

	// Remove attribute references for text that will disappear.

	WgTextTool::ClearProperties( m_pLines[0].pText+nChars, delChar );

	// Fill any new empty area with the default character,
	// empty area first so we don't dereference random attributes.

	memset(wpContent, 0, addChar*sizeof(WgChar));

	for( Uint32 i = 0 ; i < addChar ; i++ )
		wpContent[i] = ch;

	// Copy text succeeding change

	WgChar * pSource = m_pLines[line].pText + col + delChar;
	nChars = (m_pLines[m_nLines-1].pText + m_pLines[m_nLines-1].nChars+1) - pSource;

	memcpy( wpContent + addChar, pSource, nChars * sizeof(WgChar) );

	// Copy lines preceding change

	wpLine = (WgTextLine *) pBuffer;

	for( Uint32 i = 0 ; i < line ; i++ )
	{
		wpLine[i].nChars = m_pLines[i].nChars;
		wpLine[i].pText = m_pLines[i].pText + textReposition;
	}

	// Copy lines succeeding change

	wpLine = ((WgTextLine *) pBuffer) + line + 1 + addLines;
	Uint32 src = line + 1 + delLines;

	while( src < m_nLines )
	{
		wpLine->nChars = m_pLines[src].nChars;
		wpLine->pText = m_pLines[src].pText + textReposition;

		wpLine += 2;
		src++;
	}

	// Finishing

	if( m_pLines != &WgText::g_emptyLine )
		delete [] (char *) m_pLines;

	m_pLines = (WgTextLine *) pBuffer;
	m_nLines += addLines - delLines;

	return wpContent;
}

//____ endChange() _________________________________________________________

void WgSimpleText::endChange( Uint32 startline, Uint32 nLines )
{
	WgChar * pChar = (WgChar *) &m_pLines[m_nLines];

	for( Uint32 ln = startline ; ln < nLines ; ln++ )
	{
		Uint32 col = 0;
		while( pChar[col].IsEndOfLine() == false )
			col++;

		m_pLines[ln].pText = pChar;
		m_pLines[ln].nChars = col;

		pChar += col + 1;
	}
}

