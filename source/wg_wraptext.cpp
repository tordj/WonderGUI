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

#include <wg_wraptext.h>
#include <wg_texttool.h>
#include <wg_font.h>

//_____ WgWrapText() __________________________________________________________

WgWrapText::WgWrapText()
{
	m_pHardLines = &WgText::g_emptyLine;
	m_nHardLines = 1;

	m_pSoftLines = &WgText::g_emptyLine;
	m_nSoftLines = 1;

	m_lineWidth		= 0xFFFFFFFF;
	m_bWrap			= true;
}

WgWrapText::WgWrapText( const char * pText )
{
	m_pHardLines = &WgText::g_emptyLine;
	m_nHardLines = 1;

	m_pSoftLines = &WgText::g_emptyLine;
	m_nSoftLines = 1;

	m_lineWidth		= 0xFFFFFFFF;
	m_bWrap			= true;

	setText( pText );
}

WgWrapText::WgWrapText( const Uint16 * pText )
{
	m_pHardLines = &WgText::g_emptyLine;
	m_nHardLines = 1;

	m_pSoftLines = &WgText::g_emptyLine;
	m_nSoftLines = 1;

	m_lineWidth		= 0xFFFFFFFF;
	m_bWrap			= true;

	setText( pText );
}

//_____ ~WgWrapText() _________________________________________________________

WgWrapText::~WgWrapText()
{
	if( m_pSoftLines != m_pHardLines )
		delete [] m_pSoftLines;

	if( m_pHardLines != &WgText::g_emptyLine )
		delete [] m_pHardLines;
}


//____ getText() ______________________________________________________________

const WgChar * WgWrapText::getText() const
{
	return m_pHardLines->pText;
}

//____ nbLines() ______________________________________________________________

Uint32 WgWrapText::nbLines() const
{
	return m_nHardLines;
}

//____ nbSoftLines() __________________________________________________________

Uint32 WgWrapText::nbSoftLines() const
{
	return m_nSoftLines;
}

//____ getLines() _____________________________________________________________

const WgTextLine * WgWrapText::getLines() const
{
	return m_pHardLines;
}

//____ getSoftLines() _________________________________________________________

const WgTextLine * WgWrapText::getSoftLines() const
{
	return m_pSoftLines;
}

//____ getLine() ______________________________________________________________

WgTextLine * WgWrapText::getLine( Uint32 line ) const
{
	if( line >= m_nHardLines )
		return 0;

	return m_pHardLines + line;
}

//____ getSoftLine() __________________________________________________________

WgTextLine * WgWrapText::getSoftLine( Uint32 line ) const
{
	if( line >= m_nSoftLines )
		return 0;

	return m_pSoftLines + line;
}

//____ getLineText() __________________________________________________________

WgChar * WgWrapText::getLineText( Uint32 line ) const
{
	if( line >= m_nHardLines )
		return 0;

	return m_pHardLines[line].pText;
}

//____ getSoftLineText() ______________________________________________________

WgChar * WgWrapText::getSoftLineText( Uint32 line ) const
{
	if( line >= m_nSoftLines )
		return 0;

	return m_pSoftLines[line].pText;
}

//____ SetWrap() ______________________________________________________________

void WgWrapText::SetWrap( bool bWrap )
{
	if( m_bWrap != bWrap )
	{
		m_bWrap = bWrap;
		regenSoftLines();
	}
}

//____ setLineWidth() _________________________________________________________

void WgWrapText::setLineWidth( Uint32 width )
{
	if( width != m_lineWidth )
	{
		m_lineWidth = width;

		if( m_bWrap )
			regenSoftLines();
	}
}

//____ posSoft2Hard() _________________________________________________________

void WgWrapText::posSoft2Hard( Uint32 &line, Uint32 &col ) const
{
	// Get a character pointer from soft line/col

	if( line >= m_nSoftLines )
		line = m_nSoftLines - 1;

	WgTextLine * pLine = &m_pSoftLines[line];

	WgChar * pOfs = pLine->pText;
	if( col > pLine->nChars )
		pOfs += pLine->nChars;
	else
		pOfs += col;

	// Convert character pointer to hard line/col

	int ln = 0;
	while( pOfs > m_pHardLines[ln].pText + m_pHardLines[ln].nChars )
		ln++;

	line = ln;
	col = pOfs - m_pHardLines[ln].pText;
	return;
}

//____ posHard2Soft() _________________________________________________________

void WgWrapText::posHard2Soft( Uint32 &line, Uint32 &col ) const
{
	// Get a character pointer from hard line/col

	if( line >= m_nHardLines )
		line = m_nHardLines - 1;

	WgTextLine * pLine = &m_pHardLines[line];

	WgChar * pOfs = pLine->pText;
	if( col > pLine->nChars )
		pOfs += pLine->nChars;
	else
		pOfs += col;

	// Convert character pointer to soft line/col

	int ln = line;							// Corresponding soft line needs to be same or higher...
	while( pOfs > m_pSoftLines[ln].pText + m_pSoftLines[ln].nChars )
		ln++;

	line = ln;
	col = pOfs - m_pSoftLines[ln].pText;
	return;
}




//____ beginChange() __________________________________________________________

WgChar * WgWrapText::beginChange( Uint32 line, Uint32 col, Uint32 addChar, Uint32 delChar,
								 Uint32 addLines, Uint32 delLines, WgChar ch )
{
	// Calculate old and new size

	int oldSize = sizeof(WgTextLine)*m_nHardLines;

	for( int i = 0 ; i < (int) m_nHardLines ; i++ )
		oldSize += (m_pHardLines[i].nChars+1)*sizeof(WgChar);

	int newSize = oldSize + (addLines-delLines)*sizeof(WgTextLine) + (addChar-delChar)*sizeof(WgChar);

	// Reallocate and copy content

	int	nLines = m_nHardLines + addLines - delLines;

	char * pBuffer = new char[newSize];
	WgChar * wpContent	= (WgChar *) (pBuffer + sizeof(WgTextLine)* nLines);
	WgTextLine * wpLine = (WgTextLine *) pBuffer;

	int textReposition = wpContent - m_pHardLines[0].pText;

	// Copy text preceding change

	int nChars = (m_pHardLines[line].pText + col - m_pHardLines[0].pText);
	memcpy( wpContent, m_pHardLines[0].pText, nChars * sizeof(WgChar) );

	wpContent += nChars;

	// Remove property references for text that will disappear.

	WgTextTool::ClearProperties( m_pHardLines[0].pText+nChars, delChar );

	// Fill any new empty area with the default character,
	// empty area first so we don't dereference random WgTextProp.

	memset(wpContent, 0, addChar*sizeof(WgChar));

	for( Uint32 i = 0 ; i < addChar ; i++ )
		wpContent[i] = ch;

	// Copy text succeeding change

	WgChar * pSource = m_pHardLines[line].pText + col + delChar;
	nChars = (m_pHardLines[m_nHardLines-1].pText + m_pHardLines[m_nHardLines-1].nChars+1) - pSource;

	memcpy( wpContent+addChar, pSource, nChars * sizeof(WgChar) );

	// Copy lines preceeding change (including WgTextLine for first changed line).

	wpLine = (WgTextLine *) pBuffer;

	for( Uint32 i = 0 ; i < line+1 ; i++ )
	{
		wpLine[i].nChars = m_pHardLines[i].nChars;
		wpLine[i].pText = m_pHardLines[i].pText + textReposition;
	}

	// Copy lines succeeding change

	wpLine = ((WgTextLine *) pBuffer) + line + 1 + addLines;
	Uint32 src = line + 1 + delLines;

	while( src < m_nHardLines )
	{
		wpLine->nChars = m_pHardLines[src].nChars;
		wpLine->pText = m_pHardLines[src].pText + (textReposition + addChar - delChar);

		wpLine++;
		src++;
	}

	// Finishing

	if( m_pHardLines != &WgText::g_emptyLine )
	{
		delete [] m_pHardLines;

		if( m_pSoftLines == m_pHardLines )
		{
			m_pSoftLines = (WgTextLine *) pBuffer;
			m_nSoftLines = m_nHardLines + addLines - delLines;
		}
	}
	m_pHardLines = (WgTextLine *) pBuffer;
	m_nHardLines += addLines - delLines;

	return wpContent;
}

//____ endChange() ____________________________________________________________

void WgWrapText::endChange( Uint32 startline, Uint32 nLines )
{
	WgChar * pChar = m_pHardLines[startline].pText;

	for( Uint32 ln = startline ; ln < startline+nLines ; ln++ )
	{
		Uint32 col = 0;
		while( pChar[col].isHardEndOfLine() == false )
			col++;

		m_pHardLines[ln].pText = pChar;
		m_pHardLines[ln].nChars = col;

		pChar += col + 1;
	}

	regenSoftLines();
}

//____ regenSoftLines() _______________________________________________________

void WgWrapText::regenSoftLines()
{
	//TODO: I believe we have memory leaks here...

	// Take care of our special case (empty text)

	if( m_pHardLines == &WgText::g_emptyLine )
	{
		m_pSoftLines = &WgText::g_emptyLine;
		m_nSoftLines = 1;
		return;
	}

	// If we don't wrap, we have no softlines...

	if( !m_bWrap )
	{
		m_pSoftLines = m_pHardLines;
		m_nSoftLines = m_nHardLines;
		return;
	}


	// Clear old softbreaks, set new ones and count number of SoftLines needed.

	WgChar *		p = m_pHardLines[0].pText;
	unsigned int	nSoftLines = 1;				// We always have at least one line...

	WgTextTool::Ruler	ruler( m_pProp, m_mode );

	while( !p->IsEndOfText() )
	{
		WgChar *	pbp = 0;				// BreakPoint-pointer.

		while( !p->isHardEndOfLine() )
		{
			p->clearSoftBreak();

			// If break is permitted we can move the breakpoint up to this character unless it's
			// a hyphen-break where the hyphen takes too much space.

			if( p->IsBreakPermitted() )
			{
				if( p->GetGlyph() == WG_HYPHEN_BREAK_PERMITTED )
				{
					// Check so a hyphen will fit on the line as well, otherwise we can't break here.

					if( (ruler.Length() + ruler.MeasureChar('-')) < m_lineWidth )
						pbp = p;
				}
				else
					pbp = p;					// We can put a softbreak here if necessary...
			}

			// Increase line length

			Uint32 oldLen = ruler.Length();
			Uint32 lineLen = ruler.AddChar( *p );

			// Check if we need to put a softbreak.

			if( lineLen > m_lineWidth )			// Fast check
			{
				if( ruler.EndLine(0) > m_lineWidth ) // More acurate check...
				{
					if( pbp != 0 )
					{
						pbp->setSoftBreak();
						p = pbp;
						ruler.Reset();
						break;
					}
					else if( oldLen != 0 )		// Can't set a softbreak on previous char if there is no previous char...
					{
						p--;
						p->setSoftBreak();
						ruler.Reset();
						break;
					}
				}
			}

			p++;
		}

		if( !p->IsEndOfText() )
		{
			p++;			// Skip the break-point, it doesn't belong to any line.
			nSoftLines++;	// This was not the last line...
			ruler.Reset();
		}
	}


	// If we don't have any softbreaks we can just point at
	// the hardlines since they are the same.

	if( nSoftLines == m_nHardLines )
	{
		if( m_pSoftLines != m_pHardLines && m_pSoftLines != &WgText::g_emptyLine )
			delete [] m_pSoftLines;

		m_pSoftLines = m_pHardLines;
		m_nSoftLines = m_nHardLines;
		return;
	}

	// Re-allocate if size isn't the same as before...

	if( nSoftLines != m_nSoftLines )
	{
		if( m_pSoftLines != m_pHardLines && m_pSoftLines != &WgText::g_emptyLine )
			delete [] m_pSoftLines;

		m_pSoftLines = new WgTextLine[nSoftLines];
		m_nSoftLines = nSoftLines;
	}

	// Fill in the softlines-array.

	WgChar * rp = m_pHardLines[0].pText;
	for( Uint32 i = 0 ; i < m_nSoftLines ; i++ )
	{
		m_pSoftLines[i].pText = rp;

		int n = 0;
		while( false == rp[n].IsEndOfLine() )
			n++;

		m_pSoftLines[i].nChars = n;
		rp += n+1;
	}

}

