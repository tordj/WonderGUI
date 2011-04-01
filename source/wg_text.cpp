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


#include <math.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include <wctype.h>
#include <stdlib.h>
#include <assert.h>
#include <wg_text.h>
#include <wg_font.h>
#include <wg_valueformat.h>
#include <wg_text.h>			// UGLY DEPENDENCY FOR QUICKHACK IN INSERT TEXT METHODS!!!
#include <wg_texttool.h>
#include <wg_charseq.h>
#include <wg_charbuffer.h>
#include <wg_pen.h>
#include <wg_interface_textholder.h>

WgTextLine		WgText::g_emptyLine = {0,0};

//____ Constructor ____________________________________________________________


WgText::WgText()
{
	Init();
}

WgText::WgText( const WgCharSeq& seq )
{
	Init();
	setText( seq );
}

WgText::WgText( const WgCharBuffer * pBuffer )
{
	Init();
	setText( pBuffer );
}

WgText::WgText( const WgString& str )
{
	Init();
	setText( str );
}

//____ Init() _________________________________________________________________

void WgText::Init()
{
	m_pManagerNode = 0;
	m_pHolder = 0;
	m_pCursor = 0;

	m_selColor  = WgColor(0xff, 0x30, 0x80, 0xff);
	m_origo		= WgOrigo::topLeft();
	m_tintMode	= TINTMODE_MULTIPLY;
	m_lineSpaceAdj	= 0;
	m_mode		= WG_MODE_NORMAL;

	m_pHardLines = &WgText::g_emptyLine;
	m_nHardLines = 1;

	m_pSoftLines = &WgText::g_emptyLine;
	m_nSoftLines = 1;

	m_lineWidth		= 0xFFFFFFFF;
	m_bWrap			= true;

	clearSelection();
}



//____ Destructor _____________________________________________________________

WgText::~WgText()
{
	delete m_pManagerNode;

	if( m_pSoftLines != m_pHardLines )
		delete [] m_pSoftLines;

	if( m_pHardLines != &WgText::g_emptyLine )
		delete [] m_pHardLines;

	delete m_pCursor;
}

//____ setManager() ___________________________________________________________

void WgText::setManager( WgTextManager * pManager )
{
	if( m_pManagerNode )
		delete m_pManagerNode;

	if( pManager )
		m_pManagerNode = pManager->NewNode( this );
	else
		m_pManagerNode = 0;
}


//____ regenHardLines() _______________________________________________________

void WgText::regenHardLines()
{
	if( m_buffer.Length() == 0 )
	{
		if( m_pHardLines != &WgText::g_emptyLine && m_pHardLines != m_pSoftLines )
			delete [] m_pHardLines;

		m_pHardLines = &WgText::g_emptyLine;
		m_nHardLines = 1;
	}
	else
	{
		int nLines = WgTextTool::countLines( m_buffer.Chars() );

		if( m_pHardLines == &WgText::g_emptyLine )
		{
			m_pHardLines = new WgTextLine[nLines];
			m_nHardLines = nLines;
		}
		else if( m_nHardLines != nLines )
		{
			if( m_pHardLines != m_pSoftLines )				// Only delete hardlines array if it isn't shared with softlines array.
				delete [] m_pHardLines;

			m_pHardLines = new WgTextLine[nLines];
			m_nHardLines = nLines;
		}

		// Fill in hardlines array.

		int		line = 0;
		int		ofs = 0;
		const WgChar * p = m_buffer.Chars();

		m_pHardLines[line].ofs = ofs;

		while( !p[ofs].IsEndOfText() )
		{
			if( p[ofs].IsEndOfLine() )
			{
				m_pHardLines[line].nChars = ofs - m_pHardLines[line].ofs;
				line++;
				m_pHardLines[line].ofs = ofs+1;					// +1 to skip linebreak character.
			}
			ofs++;
		}
		m_pHardLines[line].nChars = ofs - m_pHardLines[line].ofs;
	}
}


//_____ clear() _______________________________________________________________

void WgText::clear()
{
	m_buffer.Reset();

	if( m_pSoftLines != m_pHardLines )
		delete [] m_pSoftLines;

	if( m_pHardLines != &WgText::g_emptyLine )
		delete [] m_pHardLines;

	m_pHardLines = &WgText::g_emptyLine;
	m_nHardLines = 1;

	m_pSoftLines = &WgText::g_emptyLine;
	m_nSoftLines = 1;

	clearSelection();

	if( m_pCursor )
		m_pCursor->gotoHardPos(m_pCursor->line(), m_pCursor->column());
}


void WgText::setText( const WgCharSeq& seq )
{
	m_buffer.Clear();
	m_buffer.PushBack( seq );
	regenHardLines();
	regenSoftLines();
	clearSelection();

	assert( m_buffer.FindFirst( WG_ESCAPE_CODE ) == -1 );		// Forgotten to wrap text in WgCharSeqEscaped?
}

void WgText::setText( const WgCharBuffer * buffer )
{
	m_buffer = * buffer;
	regenHardLines();
	regenSoftLines();
	clearSelection();

	assert( m_buffer.FindFirst( WG_ESCAPE_CODE ) == -1 );		// Forgotten to wrap text in WgCharSeqEscaped?
}


void WgText::setText( const WgString& str )
{
	m_buffer = str;
	regenHardLines();
	regenSoftLines();
	clearSelection();

	assert( m_buffer.FindFirst( WG_ESCAPE_CODE ) == -1 );		// Forgotten to wrap text in WgCharSeqEscaped?
}


void WgText::setText( const WgText * pText )
{
	// TODO: Optimize, we can simply copy the hardlines array. Softlines needs to be generated though.

	m_buffer = * pText->getBuffer();
	regenHardLines();
	regenSoftLines();
	clearSelection();
}

//_________________________________________________________
void WgText::selectText( Uint32 startLine, Uint32 startCol, Uint32 endLine, Uint32 endCol )
{
	if(endLine < startLine || (endLine == startLine && endCol < startCol) )
	{
		std::swap(endLine, startLine);
		std::swap(endCol, startCol);
	}

	if(startLine >= nbLines())
		startLine = nbLines() - 1;

	if(endLine >= nbLines())
		endLine = nbLines() - 1;

	m_selStartLine = startLine;
	m_selEndLine = endLine;
	m_selStartCol = startCol;
	m_selEndCol = endCol;

	const WgTextLine * pCurLine = &m_pHardLines[startLine];
	const WgTextLine * pLastLine = &m_pHardLines[endLine];
	if(endCol > pCurLine->nChars)
		endCol = pCurLine->nChars;
}

//_________________________________________________________
bool WgText::getSelection( Uint32& startLine, Uint32& startCol, Uint32& endLine, Uint32& endCol ) const
{
	startLine = m_selStartLine;
	endLine = m_selEndLine;
	startCol = m_selStartCol;
	endCol = m_selEndCol;

	return startLine != endLine || startCol != endCol;
}

//_________________________________________________________
void WgText::clearSelection( )
{
	m_selStartLine = 0;
	m_selEndLine = 0;
	m_selStartCol = 0;
	m_selEndCol = 0;
}





//____ getSoftLineWidth() _____________________________________________________

Uint32 WgText::getSoftLineWidth( Uint32 line ) const
{
	//TODO: Should just check lines width-variable when that is maintained.

	if( line >= nbSoftLines() || !m_pProp->GetFont() )
		return 0;

	return WgTextTool::lineWidthPart( m_pManagerNode, m_pProp, m_mode, getSoftLineText(line), getSoftLine(line)->nChars );
}

//____ getSoftLineWidthPart() _________________________________________________

Uint32 WgText::getSoftLineWidthPart( Uint32 _line, Uint32 startCol, Uint32 nCol ) const
{
	if( _line >= nbSoftLines() || !m_pProp->GetFont() )
		return 0;

	const WgTextLine * pLine = getSoftLine(_line);

	if( pLine->nChars - startCol < nCol )
		nCol = pLine->nChars-startCol;

	return WgTextTool::lineWidthPart( m_pManagerNode, m_pProp, m_mode, m_buffer.Chars() + pLine->ofs, nCol );
}


//____ getLineWidth() ____________________________________________________________

Uint32 WgText::getLineWidth( Uint32 line ) const
{
	//TODO: Should just check lines width-variable when that is maintained.

	if( line >= nbLines() || !m_pProp->GetFont() )
		return 0;

	return WgTextTool::lineWidth( m_pManagerNode, m_pProp, m_mode, getLineText(line) );
}

//____ getLineWidthPart() ________________________________________________________

Uint32 WgText::getLineWidthPart( Uint32 _line, Uint32 startCol, Uint32 nCol ) const
{
	if( _line >= nbLines() || !m_pProp->GetFont() )
		return 0;

	const WgTextLine * pLine = getLine(_line);

	if( nCol > pLine->nChars - startCol )
		return 0;

	return WgTextTool::lineWidthPart( m_pManagerNode, m_pProp, m_mode, m_buffer.Chars() + pLine->ofs, nCol );
}

//____ width() ________________________________________________________________

Uint32 WgText::width() const
{
	int		maxWidth = 0;

	Uint32 nLines = nbSoftLines();

	for( Uint32 i = 0 ; i < nLines ; i++ )
	{
		int width = getSoftLineWidth(i);
		if( width > maxWidth )
			maxWidth = width;
	}

	return maxWidth;
}

//____ height()________________________________________________________________

Uint32 WgText::height() const
{
	if( !m_pProp->GetFont() )
		return 0;

	int size = m_pProp->GetSize();
	if( m_pManagerNode )
		size = (int) m_pManagerNode->GetSize( m_pProp->GetFont(), m_pProp->GetStyle(), size );

	Uint32 fontspacing = m_pProp->GetFont()->GetGlyphSet(m_pProp->GetStyle(), size )->GetLineSpacing(size);

	Uint32 fontheight = m_pProp->GetFont()->GetGlyphSet(m_pProp->GetStyle(), size )->GetHeight(size);

	fontspacing +=  m_lineSpaceAdj;
	if( fontspacing < 0 )
		fontspacing = 0;

	return fontheight + fontspacing*(nbSoftLines()-1);		//TODO: Count right even if style changes over the text and lines.
}

//____ heightForWidth() _______________________________________________________

int WgText::heightForWidth( int width ) const
{
	if( m_bWrap )
	{
		//TODO: Implement

		return height();
	}
	else
		return height();
}

//____ softLineHeight() _______________________________________________________

Uint32 WgText::softLineHeight( Uint32 line )
{
	if( line >= nbSoftLines() || !m_pProp->GetFont() )
		return 0;

	int size = m_pProp->GetSize();
	if( m_pManagerNode )
		size = (int) m_pManagerNode->GetSize( m_pProp->GetFont(), m_pProp->GetStyle(), size );

	Uint32 fontheight = m_pProp->GetFont()->GetGlyphSet(m_pProp->GetStyle(), size )->GetHeight(size);

	return fontheight;								//TODO: Count right even if style changes over the text and lines.
}

Uint32 WgText::softLineSpacing( Uint32 line )
{
	if( line >= nbSoftLines() || !m_pProp->GetFont() )
		return 0;

	int size = m_pProp->GetSize();
	if( m_pManagerNode )
		size = (int) m_pManagerNode->GetSize( m_pProp->GetFont(), m_pProp->GetStyle(), size );

	Uint32 spacing = m_pProp->GetFont()->GetGlyphSet(m_pProp->GetStyle(), size )->GetLineSpacing(size);

	return spacing;									//TODO: Count right even if style changes over the text and lines.
}



void WgText::setProperties( const WgTextPropPtr& pProp )
{
	m_pProp = pProp;
	regenSoftLines();
}

void WgText::setColor( const WgColor color )
{
	WgTextProp	prop = * m_pProp;
	prop.SetColor(color);
	m_pProp = prop.Register();
}

void WgText::setColor( const WgColor color, WgMode mode )
{
	WgTextProp	prop = * m_pProp;
	prop.SetColor(color,mode);
	m_pProp = prop.Register();
}

void WgText::setStyle( WgFontStyle style )
{
	WgTextProp	prop = * m_pProp;
	prop.SetStyle(style);
	m_pProp = prop.Register();
	regenSoftLines();
}

void WgText::setStyle( WgFontStyle style, WgMode mode )
{
	WgTextProp	prop = * m_pProp;
	prop.SetStyle(style,mode);
	m_pProp = prop.Register();
	regenSoftLines();
}

void WgText::setFont( WgFont * pFont )
{
	WgTextProp	prop = * m_pProp;
	prop.SetFont(pFont);
	m_pProp = prop.Register();
	regenSoftLines();
}

void WgText::setBreakLevel( int level )
{
	WgTextProp	prop = * m_pProp;
	prop.SetBreakLevel(level);
	m_pProp = prop.Register();
	regenSoftLines();
}

void WgText::clearProperties()
{
	m_pProp = 0;
}

void WgText::clearColor()
{
	WgTextProp	prop = * m_pProp;
	prop.ClearColor();
	m_pProp = prop.Register();
}

void WgText::clearColor( WgMode mode )
{
	WgTextProp	prop = * m_pProp;
	prop.ClearColor(mode);
	m_pProp = prop.Register();
}

void WgText::clearStyle()
{
	WgTextProp	prop = * m_pProp;
	prop.ClearStyle();
	m_pProp = prop.Register();
	regenSoftLines();
}

void WgText::clearStyle( WgMode mode )
{
	WgTextProp	prop = * m_pProp;
	prop.ClearStyle(mode);
	m_pProp = prop.Register();
	regenSoftLines();
}

void WgText::clearBreakLevel()
{
	WgTextProp	prop = * m_pProp;
	prop.ClearBreakLevel();
	m_pProp = prop.Register();
	regenSoftLines();
}

void WgText::clearFont()
{
	WgTextProp	prop = * m_pProp;
	prop.ClearFont();
	m_pProp = prop.Register();
	regenSoftLines();
}

//____ getTextUTF8() __________________________________________________________
/**
	Counts the number of bytes needed to represent the text in UTF-format,
	reserves a memory area just large enough, generates a zero-terminated string
	and returns a pointer to it.

	@returns	Pointer to the string in UTF8-format. Caller needs to delete[]
				the string afterwards.
*/

char *	WgText::getTextUTF8() const
{
	Uint32 bytes = getTextSizeUTF8();
	char * pDest = new char[bytes+1];
	getTextUTF8(pDest, bytes+1);
	return pDest;
}

//____ getTextUTF8() __________________________________________________________
/**
	Generates a representation of the text in UTF8-format in the specified
	memory area.

	@param		maxBytes	Maximum number of bytes for the text, including
							zero-termination.

	@returns		Length of UTF8-string written to the area (excluding
					termination character). No incomplete multibyte characters
					are written, so number of bytes written can be less
					than maxChars-1 even when whole string didn't fit.

*/

Uint32	WgText::getTextUTF8( char * pDest, Uint32 maxBytes ) const
{
	return WgTextTool::getTextUTF8(getText(), pDest, maxBytes);
}

//____ getTextSizeUTF8() ______________________________________________________

Uint32	WgText::getTextSizeUTF8() const
{
	return WgTextTool::getTextSizeUTF8(getText());
}

//____ getTextFormattedUTF8() _________________________________________________

char * WgText::getTextFormattedUTF8() const
{
	Uint32 bytes = getTextSizeFormattedUTF8();
	char * pDest = new char[bytes+1];
	getTextFormattedUTF8(pDest, bytes+1);
	return pDest;
}

//____ getTextFormattedUTF8() __________________________________________________

Uint32 WgText::getTextFormattedUTF8( char * pDest, Uint32 maxBytes ) const
{
	return WgTextTool::getTextFormattedUTF8( getText(), pDest, maxBytes );
}

//____ getTextSizeFormattedUTF8() ______________________________________________

Uint32 WgText::getTextSizeFormattedUTF8() const
{
	return WgTextTool::getTextSizeFormattedUTF8(getText(), 0xFFFFFFFF);
}


char * WgText::getSelectedTextFormattedUTF8() const
{
	if(m_selStartLine == m_selEndLine && m_selStartCol == m_selEndCol)
		return 0;

	const WgChar* pTextStart = getLineText(m_selStartLine) + m_selStartCol;
	const WgChar* pTextEnd = getLineText(m_selEndLine) + m_selEndCol;
	int nChars = pTextEnd - pTextStart;
	Uint32 nBytes = WgTextTool::getTextSizeFormattedUTF8(pTextStart, nChars);
	char * pDest = new char[nBytes + 1];
	WgTextTool::getTextFormattedUTF8(pTextStart, pDest, nBytes + 1);
	return pDest;
}

char * WgText::getSelectedTextUTF8() const
{
	if(m_selStartLine == m_selEndLine && m_selStartCol == m_selEndCol)
		return 0;

	const WgChar* pTextStart = getLineText(m_selStartLine) + m_selStartCol;
	const WgChar* pTextEnd = getLineText(m_selEndLine) + m_selEndCol;
	int nChars = pTextEnd - pTextStart;
	Uint32 nBytes = WgTextTool::getTextSizeUTF8(pTextStart, nChars);
	char * pDest = new char[nBytes + 1];
	WgTextTool::getTextUTF8(pTextStart, pDest, nBytes + 1);
	return pDest;
}


//____ compareTo() ____________________________________________________________
/*
Sint32 WgText::compareTo( const WgText * _pOther, bool bCheckCase ) const
{

	Sint32	diff = 0;
	Uint32	line = 0;

	WgChar * pMine = getLineText( 0 );
	WgChar * pOther = _pOther->getLineText( 0 );

	if( bCheckCase )
	{
		// default, case sensitive comparison
		while( pMine && pOther )
		{
			Sint32	ofs = -1;

			do
			{
				ofs++;
				diff = pMine[ofs].GetGlyph() - pOther[ofs].GetGlyph();
				if( diff != 0 )
					return diff;

			} while( pMine[ofs].GetGlyph() != 0 ); // Both lines must be terminated for a termination to occur but not create a diff, so we only have to compare against one...


			line++;
			pMine = getLineText(line);
			pOther = _pOther->getLineText(line);

		}
	}
	else
	{
		// case insensitive
		while( pMine && pOther )
		{
			Sint32	ofs = -1;

			do
			{
				ofs++;
				diff = towlower( pMine[ofs].GetGlyph() ) - towlower( pOther[ofs].GetGlyph() );	// this is the only difference from above
				if( diff != 0 )
					return diff;

			} while( pMine[ofs].GetGlyph() != 0 ); // Both lines must be terminated for a termination to occur but not create a diff, so we only have to compare against one...


			line++;
			pMine = getLineText(line);
			pOther = _pOther->getLineText(line);

		}
	}

	if( pMine )
		return 1;

	if( pOther )
		return -1;

	return 0;
}
*/


//____ setValue() _____________________________________________________________

void WgText::setValue( double value, const WgValueFormat& form )
{
	WgChar	str[parseBufLen];
	WgChar * pStr = parseValue( value, form, str );
	setText( pStr );
}

//____ setScaledValue() _______________________________________________________

void WgText::setScaledValue( Sint64 value, Uint32 scale, const WgValueFormat& form )
{
	WgChar	str[parseBufLen];
	WgChar * pStr = parseScaledValue( value, scale, form, str );
	setText( pStr );
}


//____ parseValue() ___________________________________________________________

WgChar * WgText::parseValue( double value, const WgValueFormat& f, WgChar tempstring[parseBufLen] )
{

	// Write period and decimal part

	// length of buffer - decimals - suffix - end color - null terminator
	int fractionOffset = parseBufLen - 16 - 4 - 1 - 1;

	WgChar * p = tempstring + fractionOffset;
	if( f.decimals || f.bForcePeriod )
	{
		if( 0 == f.noDecimalThreshold || (int)value < f.noDecimalThreshold )
		{
			double decPart = value - (int) value ;

			if( f.bForceDecimals || decPart != 0. )
			{
				p->SetGlyph(f.period);
				p++;

				for( int i = f.decimals; i > 0 ; i-- )
				{
					decPart *= 10;
					p->SetGlyph((Uint16)decPart + 0x30);
					p++;
					decPart = decPart - (int) decPart;
				}
			}
		}
	}

	// Add suffix

	for( int i = 0 ; i < 4 && f.suffix[i] != 0 ; i++ )
	{
		p->SetGlyph(f.suffix[i]);
		p++;
	}

	// Terminate string

	p->SetGlyph(0);
	p++;

	// Write integer part to temp area (backwards)

	Uint16	temp2[16];

	Uint32	intPart = (Uint32) value;
	int			n = 0;

	while( intPart > 0 )
	{
		temp2[n++] = (intPart % 10) + 0x30;
		intPart /= 10;
	}

	// Copy integer part acknowledge grouping

	p = tempstring + fractionOffset;
	if( f.grouping == 0 )
	{
		for( int i = 0 ; i < n ; i++ )
		{
			p--;
			p->SetGlyph(temp2[i]);
		}
	}
	else
	{
		for( int i = 0 ; i < n ; i++ )
		{
			if( i != 0 && (i % f.grouping) == 0 )
			{
				p--;
				p->SetGlyph(f.separator);
			}

			p--;
			p->SetGlyph(temp2[i]);
		}
	}

	// Possibly fill out with zeroes, acknowledge grouping

	if( n < f.integers )
	{
		for( int i = n ; i < f.integers ; i++ )
		{
			if( i != 0 && (i % f.grouping) == 0 )
			{
				p--;
				p->SetGlyph(f.separator);
			}

			p--;
			p->SetGlyph(0x30);
		}

	}

	// Possibly put a prefix at the start

	if( f.prefix[0] != 0 )
	{
		if( f.prefix[1] != 0 )
		{
			if( f.prefix[2] != 0 )
			{
				if( f.prefix[3] != 0 )
				{
					p--;
					p->SetGlyph(f.prefix[3]);
				}

				p--;
				p->SetGlyph(f.prefix[2]);
			}

			p--;
			p->SetGlyph(f.prefix[1]);
		}

		p--;
		p->SetGlyph(f.prefix[0]);
	}

	// Possibly put a plus or minus sign before prefix.

	if( value < 0 )
	{
		p--;
		p->SetGlyph('-');
	}
	else if( f.bPlus )
	{
		p--;
		p->SetGlyph('+');
	}

	// Set character attributes

	if( f.bSetTextProp )
		WgTextTool::SetProperties( f.pTextProperties, tempstring, parseBufLen );

	return p;
}

//____ parseScaledValue() _____________________________________________________

WgChar * WgText::parseScaledValue( Sint64 value, Uint32 scale, const WgValueFormat& f, WgChar tempstring[parseBufLen] )
{

	Sint64 absVal = value >= 0 ? value : -value;

	Sint64 intPart = absVal / scale;
	Sint64 decPart = absVal % scale;

	// Write period and decimal part

	// length of buffer - decimals - suffix - end color - null terminator
	int fractionOffset = parseBufLen - 16 - 4 - 1 - 1;

	WgChar * p = tempstring + fractionOffset;
	if( f.decimals || f.bForcePeriod )
	{
		if( 0 == f.noDecimalThreshold || (int)absVal < f.noDecimalThreshold )
		{
			if( f.bForceDecimals || decPart != 0 )
			{
				p++->SetGlyph(f.period);

				for( int i = f.decimals; i > 0 ; i-- )
				{
					decPart *= 10;
					p++->SetGlyph(((Uint16)(decPart/scale)) + 0x30);
					decPart = decPart % scale;
				}
			}
		}
	}

	// Add suffix

	for( int i = 0 ; i < 4 && f.suffix[i] != 0 ; i++ )
		p++->SetGlyph(f.suffix[i]);


	// Terminate string

	p++->SetGlyph(0);						// Just to be sure, might already have been terminated as part of suffix.

	// Write integer part to temp area (backwards)

	Uint16	temp2[16];

	int			n = 0;

	while( intPart > 0 )
	{
		temp2[n++] = (Uint16) ((intPart % 10) + 0x30);
		intPart /= 10;
	}

	// Copy integer part acknowledge grouping

	p = tempstring + fractionOffset;
	if( f.grouping == 0 )
	{
		for( int i = 0 ; i < n ; i++ )
			(--p)->SetGlyph(temp2[i]);
	}
	else
	{
		for( int i = 0 ; i < n ; i++ )
		{
			if( i != 0 && (i % f.grouping) == 0 )
				(--p)->SetGlyph(f.separator);

			(--p)->SetGlyph(temp2[i]);
		}
	}

	// Possibly fill out with zeroes, acknowledge grouping

	if( n < f.integers )
	{
		for( int i = n ; i < f.integers ; i++ )
		{
			if( i != 0 && (i % f.grouping) == 0 )
				(--p)->SetGlyph(f.separator);

			(--p)->SetGlyph(0x30);
		}

	}

	// Possibly put a prefix at the start

	if( f.prefix[0] != 0 )
	{
		if( f.prefix[1] != 0 )
		{
			if( f.prefix[2] != 0 )
			{
				if( f.prefix[3] != 0 )
					(--p)->SetGlyph(f.prefix[3]);

				(--p)->SetGlyph(f.prefix[2]);
			}

			(--p)->SetGlyph(f.prefix[1]);
		}

		(--p)->SetGlyph(f.prefix[0]);
	}

	// Possibly put a plus or minus sign at the very start.

	if( value < 0 || (value == 0 && f.bZeroIsNegative) )
		(--p)->SetGlyph((short) '-');
	else if( f.bPlus )
		(--p)->SetGlyph((short) '+');

	// Set character attributes

	if( f.bSetTextProp )
		WgTextTool::SetProperties(f.pTextProperties, tempstring, parseBufLen );

	return p;
}


//____ nbChars() ________________________________________________________________
/**
	Returns number of characters in text, including any linebreak, excluding
	the zero terminating the string.

	(note: This behaviour has changed with rewrite of textsystem, did previously
	not include the linebreaks).

*/

Uint32 WgText::nbChars() const
{
	const WgTextLine * pLines = getLines();
	Uint32 nLines = nbLines();

	int		nChars	= 0;

	for( int n = 0 ; n < (int) nLines ; n++ )
		nChars += pLines[n].nChars;

	nChars += nLines-1;			// Include all break characters

	return nChars;
}

//____ refresh() ______________________________________________________________

void WgText::refresh()
{
	regenHardLines();
	regenSoftLines();

	if( m_pCursor )
		m_pCursor->gotoHardPos(m_pCursor->line(), m_pCursor->column());

	if( m_pHolder )
		m_pHolder->TextModified( this );
}

//____ addChar() ______________________________________________________________

int WgText::addChar( const WgChar& character )
{
	int nAdded = m_buffer.PushBack( character );
	regenHardLines();
	regenSoftLines();

	return nAdded;
}


//____ addText() ______________________________________________________________

int WgText::addText( const WgCharSeq& seq )
{
	int nAdded = m_buffer.PushBack( seq );
	regenHardLines();
	regenSoftLines();

	assert( m_buffer.FindFirst( WG_ESCAPE_CODE ) == -1 );		// Forgotten to wrap text in WgCharSeqEscaped?

	return nAdded;
 }

//____ insertText() ___________________________________________________________

int WgText::insertText( int ofs, const WgCharSeq& seq )
{
	int nInserted = m_buffer.Insert( ofs, seq );
	regenHardLines();
	regenSoftLines();

	assert( m_buffer.FindFirst( WG_ESCAPE_CODE ) == -1 );		// Forgotten to wrap text in WgCharSeqEscaped?

	return nInserted;
}


//____ replaceText() __________________________________________________________

int WgText::replaceText( int ofs, int nDelete, const WgCharSeq& seq )
{
	int nInserted = m_buffer.Replace( ofs, nDelete, seq );
	regenHardLines();
	regenSoftLines();

	assert( m_buffer.FindFirst( WG_ESCAPE_CODE ) == -1 );		// Forgotten to wrap text in WgCharSeqEscaped?

	return nInserted;
}

//____ deleteText() ___________________________________________________________

int WgText::deleteText( int ofs, int nChars )
{
	int nDeleted = m_buffer.Delete( ofs, nChars );
	regenHardLines();
	regenSoftLines();
	return nDeleted;
}


//____ replaceChar() __________________________________________________________

int WgText::replaceChar( int ofs, const WgChar& character )
{
	int nReplaced = m_buffer.Replace( ofs, character );
	regenHardLines();
	regenSoftLines();
	return nReplaced;
}


//____ insertChar() ___________________________________________________________

int WgText::insertChar( int ofs, const WgChar& character )
{
	int nInserted = m_buffer.Insert( ofs, character );
	regenHardLines();
	regenSoftLines();

	return nInserted;
}

//____ deleteChar() ___________________________________________________________

int WgText::deleteChar( int ofs )
{
	int nDeleted = m_buffer.Delete( ofs, 1 );
	regenHardLines();
	regenSoftLines();
	return nDeleted;
}

//____ deleteSelectedText() ___________________________________________________

void WgText::deleteSelectedText()
{
	int line = m_selStartLine;
	int column = m_selStartCol;

	int start = LineColToOffset( m_selStartLine, m_selStartCol );
	int end = LineColToOffset( m_selEndLine, m_selEndCol );


	deleteText( start, end-start );
	if(m_pCursor)
	{
		m_pCursor->setSelectionMode(false);
		m_pCursor->gotoHardPos(line, column);
	}
}

//____ nbLines() ______________________________________________________________

Uint32 WgText::nbLines() const
{
	return m_nHardLines;
}

//____ nbSoftLines() __________________________________________________________

Uint32 WgText::nbSoftLines() const
{
	return m_nSoftLines;
}

Uint32 WgText::getSoftLineSelectionWidth(Uint32 line) const
{
	Uint32 w = getSoftLineWidth(line);
	if(w == 0)
	{
		// TODO: return white space width
		w = 3;
	}
	return w;
}

//____ getLines() _____________________________________________________________

const WgTextLine * WgText::getLines() const
{
	return m_pHardLines;
}

//____ getSoftLines() _________________________________________________________

const WgTextLine * WgText::getSoftLines() const
{
	return m_pSoftLines;
}

//____ getLine() ______________________________________________________________

WgTextLine * WgText::getLine( int line ) const
{
	if( line < 0 || line >= m_nHardLines )
		return 0;

	return m_pHardLines + line;
}

//____ getSoftLine() __________________________________________________________

WgTextLine * WgText::getSoftLine( int line ) const
{
	if( line < 0 || line >= m_nSoftLines )
		return 0;

	return m_pSoftLines + line;
}

//____ getLineText() __________________________________________________________

const WgChar * WgText::getLineText( int line ) const
{
	if( line < 0 || line >= m_nHardLines )
		return 0;

	return m_buffer.Chars() + m_pHardLines[line].ofs;
}

//____ getSoftLineText() ______________________________________________________

const WgChar * WgText::getSoftLineText( int line ) const
{
	if( line < 0 || line >= m_nSoftLines )
		return 0;

	return m_buffer.Chars() + m_pSoftLines[line].ofs;
}

//____ SetWrap() ______________________________________________________________

void WgText::SetWrap( bool bWrap )
{
	if( m_bWrap != bWrap )
	{
		m_bWrap = bWrap;
		regenSoftLines();
	}
}

//____ setLineWidth() _________________________________________________________

void WgText::setLineWidth( Uint32 width )
{
	if( width != m_lineWidth )
	{
		m_lineWidth = width;

		if( m_bWrap )
			regenSoftLines();
	}
}

//____ posSoft2Hard() _________________________________________________________

void WgText::posSoft2Hard( Uint32 &line, Uint32 &col ) const
{
	// Get a character pointer from soft line/col

	if( line >= (Uint32) m_nSoftLines )
		line = m_nSoftLines - 1;

	WgTextLine * pLine = &m_pSoftLines[line];

	const WgChar * pOfs = m_buffer.Chars() + pLine->ofs;
	if( col > pLine->nChars )
		pOfs += pLine->nChars;
	else
		pOfs += col;

	// Convert character pointer to hard line/col

	int ln = 0;
	while( pOfs > m_buffer.Chars() + m_pHardLines[ln].ofs + m_pHardLines[ln].nChars )
		ln++;

	line = ln;
	col = pOfs - (m_buffer.Chars() + m_pHardLines[ln].ofs);
	return;
}

//____ posHard2Soft() _________________________________________________________

void WgText::posHard2Soft( Uint32 &line, Uint32 &col ) const
{
	// Get a character pointer from hard line/col

	if( line >= (Uint32) m_nHardLines )
		line = m_nHardLines - 1;

	WgTextLine * pLine = &m_pHardLines[line];

	const WgChar * pOfs = m_buffer.Chars() + pLine->ofs;
	if( col > pLine->nChars )
		pOfs += pLine->nChars;
	else
		pOfs += col;

	// Convert character pointer to soft line/col

	int ln = line;							// Corresponding soft line needs to be same or higher...
	while( pOfs > m_buffer.Chars() + m_pSoftLines[ln].ofs + m_pSoftLines[ln].nChars )
		ln++;

	line = ln;
	col = pOfs - (m_buffer.Chars() + m_pSoftLines[ln].ofs);
	return;
}

//____ LineColToOffset() ______________________________________________________

Uint32 WgText::LineColToOffset(int line, int col)
{
	return m_pHardLines[line].ofs + col;
}


//____ countWriteSoftLines() __________________________________________________

int WgText::countWriteSoftLines( const WgChar * pStart, WgTextLine * pWriteLines, int maxWrite )
{
	const WgChar *	p = pStart;
	int			nSoftLines = 0;
	WgPen		pen;
	Uint16		hProp = 0xFFFF;					// Force immediate update of textprop.
	int			breakLevel;
	bool		bEndOfText = false;

	pen.SetTextNode( m_pManagerNode );

	while( !bEndOfText )
	{

		const WgChar * 	pLineStart = p;
		const WgChar *	pbp = 0;				// BreakPoint-pointer.
		bool			bBreakSkips = false;	// Set if the character on the breakpoint should be skipped.
		bool			bBreakAfterPrev = false;// Set if we can break here due to a WG_BREAK_BEFORE on previous character.

		while( true )
		{

			// Update if textproperties have changed.

			if( p->PropHandle() != hProp )
			{
				pen.SetTextProp( m_pProp.GetHandle(), p->PropHandle(), m_mode );
				hProp = p->PropHandle();
				breakLevel = WgTextTool::GetCombBreakLevel( m_pProp.GetHandle(), p->PropHandle() );
			}

			// Break if this is end of line or end of text

			if( p->Glyph() == '\n' )
			{
				bBreakSkips = true;
				break;
			}
			else if( p->Glyph() == 0 )
			{
				bEndOfText = true;
				bBreakSkips = false;
				break;
			}

			// Check if we can move the breakpoint up to this character.

			WgBreakRules breakStatus = WgTextTool::isBreakAllowed( p->glyph, breakLevel );
			switch( breakStatus )
			{
			case WG_BREAK_BEFORE:
				pbp = p;
				bBreakSkips = false;
				break;

			case WG_BREAK_ON:

				if( p->Glyph() == WG_HYPHEN_BREAK_PERMITTED )
				{
					// Check so a hyphen will fit on the line as well, otherwise we can't break here.
					// We don't take kerning into account here, not so important.

					WgGlyphPtr pHyphen = pen.GetGlyphSet()->GetGlyph( '-', pen.GetSize() );
					if( !pHyphen || Uint32(pen.GetPosX() + pHyphen->Advance()) > m_lineWidth )
						break;			// Can't break here, hyphen wouldn't fit on line.
				}

				pbp = p;
				bBreakSkips = true;
				break;

			case WG_BREAK_AFTER:
				if( bBreakAfterPrev )
				{
					pbp = p;
					bBreakSkips = false;
				}
				bBreakAfterPrev = true;
				break;

			default:				// WG_NO_BREAK

				if( bBreakAfterPrev )
				{
					pbp = p;
					bBreakSkips = false;
					bBreakAfterPrev = false;
				}
				break;

			}

			// Increase line length

			pen.SetChar( p->Glyph() );
			pen.ApplyKerning();


			// Check if we need to put a softbreak.

//			Uint32 len = pen.GetBlitPosX() + pen.GetGlyph()->rect.w; // No advance on last character of line, just bearingX + width
			Uint32 len = pen.GetPosX() + pen.GetGlyph()->Advance();
			if( len > m_lineWidth )			
			{
				if( pbp != 0 && pbp != pLineStart )
				{
					p = pbp;
					break;
				}
				else if( p > pLineStart )		// We want at least one char on each line before a softbreak, otherwise we get eternal number of lines...
				{
					// Force a break here, without a breakpoint.

					bBreakSkips = false;
					break;
				}
			}
			else
				pen.AdvancePos();

			//

			p++;
		}

		if( nSoftLines < maxWrite )
		{
			pWriteLines[nSoftLines].ofs = pLineStart - pStart;
			pWriteLines[nSoftLines].nChars = p - pLineStart;
		}

		if( bBreakSkips )
			p++;				// Skip the break-point or newline character, it doesn't belong to any line.

		nSoftLines++;
		pen.SetPosX(0);			// Reset position
		pen.FlushChar();		// Flush current character so it won't affect kerning for first character of next line.
	}
	return nSoftLines;
}

//____ regenSoftLines() _______________________________________________________

void WgText::regenSoftLines()
{
	bool	bHasSoftLineArray = (m_pSoftLines != m_pHardLines && m_pSoftLines != &WgText::g_emptyLine)?true:false;

	// Take care of our special case (empty text)

	if( m_pHardLines == &WgText::g_emptyLine )
	{
		if( bHasSoftLineArray )
			delete [] m_pSoftLines;

		m_pSoftLines = &WgText::g_emptyLine;
		m_nSoftLines = 1;
		return;
	}

	// If we don't wrap, we have no softlines...

	if( !m_bWrap )
	{
		if( bHasSoftLineArray )
			delete [] m_pSoftLines;

		m_pSoftLines = m_pHardLines;
		m_nSoftLines = m_nHardLines;
		return;
	}

	int nSoftLines;

	if( bHasSoftLineArray )
		nSoftLines = countWriteSoftLines( m_buffer.Chars(), m_pSoftLines, m_nSoftLines );
	else
		nSoftLines = countWriteSoftLines( m_buffer.Chars(), 0, 0 );

	// If we don't have any softbreaks we can just point at
	// the hardlines since they are the same.

	if( nSoftLines == m_nHardLines )
	{
		if( bHasSoftLineArray )
			delete [] m_pSoftLines;

		m_pSoftLines = m_pHardLines;
		m_nSoftLines = m_nHardLines;
		return;
	}


	// If number of softlines are equal to before and we already have filled in the array
	// then we are done.

	if( bHasSoftLineArray && nSoftLines == m_nSoftLines )
		return;

	// Re-allocate if size isn't the same as before...
	//TODO: Optimize! There are still cases when we can avoid calling the expensive countWriteSoftLines() again, or just call it on parts of text.

	if( nSoftLines != m_nSoftLines )
	{
		if( bHasSoftLineArray )
			delete [] m_pSoftLines;

		m_pSoftLines = new WgTextLine[nSoftLines];
		m_nSoftLines = nSoftLines;
	}

	// Fill in the softlines-array.

	countWriteSoftLines( m_buffer.Chars(), m_pSoftLines, m_nSoftLines );
}
