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
#include <wg_input.h>

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
	m_pManagerNode	= 0;
	m_pHolder		= 0;
	m_pCursor		= 0;
	m_pCursorStyle	= 0;

	m_markedLinkMode = WG_MODE_NORMAL;

	m_origo			= WgOrigo::topLeft();
	m_tintMode		= TINTMODE_MULTIPLY;
	m_lineSpaceAdj	= 0;

	m_attr.mode		= WG_MODE_NORMAL;

	m_pHardLines	= new WgTextLine[1];
	m_nHardLines	= 1;

	m_pSoftLines	= m_pHardLines;
	m_nSoftLines	= 1;

	m_pHardLines->nChars = 0;
	m_pHardLines->ofs = 0;

	m_lineWidth		= 0xFFFFFFFF;
	m_bWrap			= true;

	refreshAllLines();
	clearSelection();
}



//____ Destructor _____________________________________________________________

WgText::~WgText()
{
	delete m_pManagerNode;

	if( m_pSoftLines != m_pHardLines )
		delete [] m_pSoftLines;

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

//____ setCursorStyle() _______________________________________________________

void WgText::setCursorStyle( WgCursor * pCursor )
{
	if( pCursor != m_pCursorStyle )
	{
		m_pCursorStyle = pCursor;
		regenSoftLines();					// Doesn't affect linelenght now, but should in the future,
		refreshAllLines();					// so let's call these two methods anyway.
	}
}

//____ regenHardLines() _______________________________________________________

void WgText::regenHardLines()
{
	int nLines = WgTextTool::countLines( m_buffer.Chars() );

	if( m_nHardLines != nLines )
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


//_____ clear() _______________________________________________________________

void WgText::clear()
{
	m_buffer.Reset();

	if( m_pSoftLines != m_pHardLines )
		delete [] m_pSoftLines;

	if( m_nHardLines != 1 )
	{
		delete [] m_pHardLines;

		m_pHardLines = new WgTextLine[1];
		m_nHardLines = 1;
	}

	m_pHardLines->nChars = 0;
	m_pHardLines->ofs = 0;

	m_pSoftLines = m_pHardLines;
	m_nSoftLines = 1;

	refreshAllLines();
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
	refreshAllLines();
	clearSelection();

	assert( m_buffer.FindFirst( WG_ESCAPE_CODE ) == -1 );		// Forgotten to wrap text in WgCharSeqEscaped?
}

void WgText::setText( const WgCharBuffer * buffer )
{
	m_buffer = * buffer;
	regenHardLines();
	regenSoftLines();
	refreshAllLines();
	clearSelection();

	assert( m_buffer.FindFirst( WG_ESCAPE_CODE ) == -1 );		// Forgotten to wrap text in WgCharSeqEscaped?
}


void WgText::setText( const WgString& str )
{
	m_buffer = str;
	regenHardLines();
	regenSoftLines();
	refreshAllLines();
	clearSelection();

	assert( m_buffer.FindFirst( WG_ESCAPE_CODE ) == -1 );		// Forgotten to wrap text in WgCharSeqEscaped?
}


void WgText::setText( const WgText * pText )
{
	// TODO: Optimize, we can simply copy the hardlines array. Softlines needs to be generated though.

	m_buffer = * pText->getBuffer();
	regenHardLines();
	regenSoftLines();
	refreshAllLines();
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

	if(startCol > m_pHardLines[startLine].nChars)
		startCol = m_pHardLines[startLine].nChars;

	if(endCol > m_pHardLines[endLine].nChars)
		endCol = m_pHardLines[endLine].nChars;

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
	if( line >= nbSoftLines() )
		return 0;

	return m_pSoftLines[line].width;
}

//____ getSoftLineWidthPart() _________________________________________________

Uint32 WgText::getSoftLineWidthPart( Uint32 _line, Uint32 startCol, Uint32 nCol ) const
{
	if( _line >= nbSoftLines() || !m_attr.pBaseProp->GetFont() )
		return 0;

	const WgTextLine * pLine = getSoftLine(_line);

	if( pLine->nChars - startCol < nCol )
		nCol = pLine->nChars-startCol;

	return WgTextTool::lineWidthPart( m_pManagerNode, &m_attr, m_buffer.Chars() + pLine->ofs, nCol );
}


//____ getLineWidth() ____________________________________________________________

Uint32 WgText::getLineWidth( Uint32 line ) const
{
	if( line >= nbLines() )
		return 0;

	return m_pHardLines[line].width;
}

//____ getLineWidthPart() ________________________________________________________

Uint32 WgText::getLineWidthPart( Uint32 _line, Uint32 startCol, Uint32 nCol ) const
{
	if( _line >= nbLines() || !m_attr.pBaseProp->GetFont() )
		return 0;

	const WgTextLine * pLine = getLine(_line);

	if( nCol > pLine->nChars - startCol )
		return 0;

	return WgTextTool::lineWidthPart( m_pManagerNode, &m_attr, m_buffer.Chars() + pLine->ofs, nCol );
}

//____ getLineOfsY() __________________________________________________________

int WgText::getLineOfsY( int line ) const
{
	if( line >= m_nSoftLines )
		return 0;

	int ofs = 0;
	for( int i = 0 ; i < line ; i++ )
		ofs += m_pSoftLines[i].lineSpacing;

	return ofs;
}


//____ width() ________________________________________________________________

Uint32 WgText::width() const
{
	int		width = 0;

	for( int i = 0 ; i < m_nSoftLines ; i++ )
	{
		if( m_pSoftLines[i].width > width )
			width = m_pSoftLines[i].width;
	}

	return width;
}

//____ height()________________________________________________________________

Uint32 WgText::height() const
{
	if( !m_attr.pBaseProp->GetFont() )
		return 0;

	int height = 0;
	for( int i = 0 ; i < m_nSoftLines-1 ; i++ )
		height += m_pSoftLines[i].lineSpacing;

	height += m_pSoftLines[m_nSoftLines-1].height;
	return height;
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
	if( line >= nbSoftLines() )
		return 0;

	return m_pSoftLines[line].height;
}

Uint32 WgText::softLineSpacing( Uint32 line )
{
	if( line >= nbSoftLines() )
		return 0;

	return m_pSoftLines[line].lineSpacing;
}


void WgText::setLinkProperties( const WgTextPropPtr& pProp )
{
	if( m_attr.pLinkProp != pProp )
	{
		m_attr.pLinkProp = pProp;
		regenSoftLines();		//TODO: Optimize: Only do if we have links.
		refreshAllLines();
	}
}

void WgText::clearLinkProperties()
{
	if( m_attr.pLinkProp )
	{
		m_attr.pLinkProp = 0;
		regenSoftLines();		//TODO: Optimize: Only do if we have links.	
		refreshAllLines();
	}
}

void WgText::setSelectionProperties( const WgTextPropPtr& pProp )
{
	if( m_attr.pSelectionProp != pProp )
	{
		m_attr.pSelectionProp = pProp;
		regenSoftLines();		//TODO: Optimize: Only do if we have selected text.
		refreshAllLines();
	}
}

void WgText::clearSelectionProperties()
{
	if( m_attr.pSelectionProp )
	{
		m_attr.pSelectionProp = 0;
		regenSoftLines();		//TODO: Optimize: Only do if we have selected text.
		refreshAllLines();
	}
}


void WgText::setSelectionBgColor(WgColor color )
{
	WgTextProp	prop = * m_attr.pSelectionProp;
	prop.SetBgColor(color);
	m_attr.pSelectionProp = prop.Register();
}


void WgText::setProperties( const WgTextPropPtr& pProp )
{
	m_attr.pBaseProp = pProp;
	regenSoftLines();
	refreshAllLines();
}

void WgText::setColor( const WgColor color )
{
	WgTextProp	prop = * m_attr.pBaseProp;
	prop.SetColor(color);
	m_attr.pBaseProp = prop.Register();
}

void WgText::setColor( const WgColor color, WgMode mode )
{
	WgTextProp	prop = * m_attr.pBaseProp;
	prop.SetColor(color,mode);
	m_attr.pBaseProp = prop.Register();
}

void WgText::setStyle( WgFontStyle style )
{
	WgTextProp	prop = * m_attr.pBaseProp;
	prop.SetStyle(style);
	m_attr.pBaseProp = prop.Register();
	regenSoftLines();
	refreshAllLines();
}

void WgText::setStyle( WgFontStyle style, WgMode mode )
{
	WgTextProp	prop = * m_attr.pBaseProp;
	prop.SetStyle(style,mode);
	m_attr.pBaseProp = prop.Register();
	regenSoftLines();
	refreshAllLines();
}

void WgText::setFont( WgFont * pFont )
{
	WgTextProp	prop = * m_attr.pBaseProp;
	prop.SetFont(pFont);
	m_attr.pBaseProp = prop.Register();
	regenSoftLines();
	refreshAllLines();
}

void WgText::setBreakLevel( int level )
{
	WgTextProp	prop = * m_attr.pBaseProp;
	prop.SetBreakLevel(level);
	m_attr.pBaseProp = prop.Register();
	regenSoftLines();
	refreshAllLines();
}

void WgText::clearProperties()
{
	if( m_attr.pBaseProp )
	{
		m_attr.pBaseProp = 0;
		regenSoftLines();
		refreshAllLines();
	}
}

void WgText::clearColor()
{
	WgTextProp	prop = * m_attr.pBaseProp;
	prop.ClearColor();
	m_attr.pBaseProp = prop.Register();
}

void WgText::clearColor( WgMode mode )
{
	WgTextProp	prop = * m_attr.pBaseProp;
	prop.ClearColor(mode);
	m_attr.pBaseProp = prop.Register();
}

void WgText::clearStyle()
{
	WgTextProp	prop = * m_attr.pBaseProp;
	prop.ClearStyle();
	m_attr.pBaseProp = prop.Register();
	regenSoftLines();
	refreshAllLines();
}

void WgText::clearStyle( WgMode mode )
{
	WgTextProp	prop = * m_attr.pBaseProp;
	prop.ClearStyle(mode);
	m_attr.pBaseProp = prop.Register();
	regenSoftLines();
	refreshAllLines();
}

void WgText::clearBreakLevel()
{
	WgTextProp	prop = * m_attr.pBaseProp;
	prop.ClearBreakLevel();
	m_attr.pBaseProp = prop.Register();
	regenSoftLines();
	refreshAllLines();
}

void WgText::clearFont()
{
	WgTextProp	prop = * m_attr.pBaseProp;
	prop.ClearFont();
	m_attr.pBaseProp = prop.Register();
	regenSoftLines();
	refreshAllLines();
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
	return m_buffer.Length();
}

//____ refresh() ______________________________________________________________

void WgText::refresh()
{
	regenHardLines();
	regenSoftLines();
	refreshAllLines();

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
	refreshAllLines();

	return nAdded;
}


//____ addText() ______________________________________________________________

int WgText::addText( const WgCharSeq& seq )
{
	int nAdded = m_buffer.PushBack( seq );
	regenHardLines();
	regenSoftLines();
	refreshAllLines();

	assert( m_buffer.FindFirst( WG_ESCAPE_CODE ) == -1 );		// Forgotten to wrap text in WgCharSeqEscaped?

	return nAdded;
 }

//____ insertText() ___________________________________________________________

int WgText::insertText( int ofs, const WgCharSeq& seq )
{
	int nInserted = m_buffer.Insert( ofs, seq );
	regenHardLines();
	regenSoftLines();
	refreshAllLines();

	assert( m_buffer.FindFirst( WG_ESCAPE_CODE ) == -1 );		// Forgotten to wrap text in WgCharSeqEscaped?

	return nInserted;
}


//____ replaceText() __________________________________________________________

int WgText::replaceText( int ofs, int nDelete, const WgCharSeq& seq )
{
	int nInserted = m_buffer.Replace( ofs, nDelete, seq );
	regenHardLines();
	regenSoftLines();
	refreshAllLines();

	assert( m_buffer.FindFirst( WG_ESCAPE_CODE ) == -1 );		// Forgotten to wrap text in WgCharSeqEscaped?

	return nInserted;
}

//____ deleteText() ___________________________________________________________

int WgText::deleteText( int ofs, int nChars )
{
	int nDeleted = m_buffer.Delete( ofs, nChars );
	regenHardLines();
	regenSoftLines();
	refreshAllLines();
	return nDeleted;
}


//____ replaceChar() __________________________________________________________

int WgText::replaceChar( int ofs, const WgChar& character )
{
	int nReplaced = m_buffer.Replace( ofs, character );
	regenHardLines();
	regenSoftLines();
	refreshAllLines();
	return nReplaced;
}


//____ insertChar() ___________________________________________________________

int WgText::insertChar( int ofs, const WgChar& character )
{
	int nInserted = m_buffer.Insert( ofs, character );
	regenHardLines();
	regenSoftLines();
	refreshAllLines();

	return nInserted;
}

//____ deleteChar() ___________________________________________________________

int WgText::deleteChar( int ofs )
{
	int nDeleted = m_buffer.Delete( ofs, 1 );
	regenHardLines();
	regenSoftLines();
	refreshAllLines();
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
		refreshAllLines();
	}
}

//____ setLineWidth() _________________________________________________________

void WgText::setLineWidth( Uint32 width )
{
	if( width != m_lineWidth )
	{
		m_lineWidth = width;

		if( m_bWrap )
		{
			regenSoftLines();
			refreshAllLines();
		}
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

Uint32 WgText::LineColToOffset(int line, int col) const
{
	return m_pHardLines[line].ofs + col;
}

//____ OffsetToSoftLineCol() ______________________________________________________

void WgText::OffsetToSoftLineCol(int ofs, int* wpLine, int* wpCol) const
{
	int ln = 0;
	while( m_pSoftLines[ln].ofs + m_pSoftLines[ln].nChars < (unsigned) ofs )
		ln++;

	* wpLine = ln;
	* wpCol = ofs - m_pSoftLines[ln].ofs;
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
	pen.SetTextAttr( &m_attr );

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
				pen.SetCharProp( p->PropHandle() );
				hProp = p->PropHandle();
				breakLevel = WgTextTool::GetCharBreakLevel( hProp, &m_attr );
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
	bool	bHasSoftLineArray = (m_pSoftLines != m_pHardLines)?true:false;

	// If we don't wrap, we have no softlines...

	if( !m_bWrap || m_buffer.Length() == 0 )
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

//____ refreshAllLines() _________________________________________________________

void WgText::refreshAllLines()
{
	for( int i = 0 ; i < m_nHardLines ; i++ )
		refreshLineInfo( &m_pHardLines[i] );

	if( m_pSoftLines != m_pHardLines )
	{
		for( int i = 0 ; i < m_nSoftLines ; i++ )
			refreshLineInfo( &m_pSoftLines[i] );
	}
}


//____ refreshLineInfo() ____________________________________________________

// Refreshes the width, height, baseline and linespacing parameters of a
// WgTextLine.


void WgText::refreshLineInfo( WgTextLine * pLine )
{
	int maxAscend = 0;
	int maxDescend = 0;
	int maxLineDescend = 0;

	const WgChar * pChars = m_buffer.Chars() + pLine->ofs;
	int		nChars = pLine->nChars;
	Uint16	hProp = -1;

	WgPen pen;
	pen.SetTextNode( m_pManagerNode );
	pen.SetTextAttr( &m_attr );

	// We must have at least one character, even if it is just CR/LF or EndOfString.

	if( nChars == 0 )
		nChars = 1;			

	// Loop through characters of the line, moving pen and updating variables.

	for( int i = 0 ; i < nChars ; i++ )
	{
		if( pChars[i].PropHandle() != hProp )
		{
			pen.SetCharProp( pChars[i].PropHandle() );
			hProp = pChars[i].PropHandle();

			if( pen.GetGlyphSet() == 0 )
				break;									// Bail out instead of crashing.

			int height		= pen.GetLineHeight();
			int baseline	= pen.GetBaseline();
			int spacing		= pen.GetLineSpacing();

			if( baseline > maxAscend )
				maxAscend = baseline;

			if( height - baseline > maxDescend )
				maxDescend = height - baseline;

			if( spacing - baseline > maxLineDescend )
				maxLineDescend = spacing - baseline;
		}

		pen.SetChar( pChars[i].Glyph() );
		pen.ApplyKerning();
		pen.AdvancePos();
	}

	// Fill in line struct.

	pLine->width		= pen.GetPosX();
	pLine->height		= maxAscend + maxDescend;
	pLine->lineSpacing	= maxAscend + maxLineDescend + m_lineSpaceAdj;
	pLine->baseline		= maxAscend;

	// Add cursor margin to width

	//TODO: Take cursor margins into account.

}

//____ ScreenY() ______________________________________________________________

int WgText::ScreenY( const WgRect& container ) const
{
	int		ofs = 0;
	if( m_origo.anchorY() != 0 && m_origo.hotspotY() != 0 )
	{
		ofs = m_origo.calcOfsY( container.h, height() );
		if( ofs < 0 )
			ofs = 0;
	}
	return ofs + container.y;

}

//____ ScreenX() ______________________________________________________________

int WgText::ScreenX( int line, const WgRect& container ) const
{
	int		ofs = 0;

	if( m_origo.anchorX() != 0 && m_origo.hotspotX() != 0 )
	{
		ofs = m_origo.calcOfsX( container.w, getSoftLineWidth(line) );
		if( ofs < 0 )
			ofs = 0;
	}
	return ofs + container.x;
}

//____ CoordToLine() __________________________________________________________

int WgText::CoordToLine( const WgCord& coord, const WgRect& container ) const
{
	int y = coord.y - ScreenY( container );

	WgTextLine * pLine = m_pSoftLines;
	for( int i = 0 ; i < m_nSoftLines ; i++ )
	{
		if( y < 0 )
			return -1;

		if( y < pLine->height )
			return i;

		y -= pLine->lineSpacing;
	}

	return -1;
}

//____ CoordToOfs() ___________________________________________________________

int WgText::CoordToOfs( const WgCord& coord, const WgRect& container ) const
{
	//TODO: Take cursor and selection into account!!!

	int line = CoordToLine( coord, container );
	if( line == -1 )
		return -1;

	WgTextLine * pLine = &m_pSoftLines[line];

	int xStart = ScreenX( line, container );
	if( coord.x < xStart || coord.x >= xStart + pLine->width )
		return -1;

	const WgChar * pChars = m_buffer.Chars() + pLine->ofs;
	Uint16	hProp = pChars->PropHandle();
	WgPen pen;
	pen.SetPosX(xStart);
	pen.SetAllProps( hProp, &m_attr, m_attr.mode, 0 );

	for( unsigned int i = 0 ; i < pLine->nChars ; i++ )
	{
		if( pChars[i].PropHandle()!= hProp )
		{
			hProp = pChars[i].PropHandle();
			pen.SetCharProp( hProp );
		}
/*
		if( cursColumn == 0 )
			pen.AdvancePosCursor( * pCursor );
		cursColumn--;
*/

		pen.SetChar( pChars[i].Glyph() );
		pen.ApplyKerning();
		pen.AdvancePos();

		if( pen.GetPosX() > coord.x )
			return i;
	}

	return -1;		// We should never get here...
}

//____ CoordToLink() __________________________________________________________

WgTextLinkPtr WgText::CoordToLink( const WgCord& coord, const WgRect& container ) const
{
	int ofs = CoordToOfs( coord, container );
	if( ofs >= 0 )
		return WgTextTool::GetCharLink( m_buffer.Chars()[ofs].PropHandle(), &m_attr );
	else
		return 0;
}


//____ onAction() _____________________________________________________________

bool WgText::OnAction( WgInput::UserAction action, int button_key, const WgRect& container, const WgCord& pointerOfs )
{
	bool bRefresh = false;

	switch( action )
	{
		case WgInput::POINTER_ENTER:
		case WgInput::POINTER_OVER:
		{
			WgTextLinkPtr pLink = CoordToLink( pointerOfs, container );
			if( m_pMarkedLink && pLink != m_pMarkedLink )
			{
				m_pMarkedLink->OnAction( WgInput::POINTER_EXIT, button_key );
				m_pMarkedLink = 0;
				bRefresh = true;
			}

			if( pLink )
			{
				if( pLink != m_pMarkedLink )
				{
					m_pMarkedLink->OnAction( WgInput::POINTER_ENTER, button_key );
					m_pMarkedLink = pLink;
					m_markedLinkMode = WG_MODE_MARKED;
					bRefresh = true;
				}
				m_pMarkedLink->OnAction( WgInput::POINTER_OVER, button_key );
			}
			break;
		}
			

		case WgInput::POINTER_EXIT:
		{
			if( m_pMarkedLink )
			{			
				m_pMarkedLink->OnAction( WgInput::POINTER_EXIT, button_key );
				m_pMarkedLink = 0;
				bRefresh = true;
			}
			break;
		}

		case WgInput::BUTTON_PRESS:
		{
			if( m_pMarkedLink )
			{	
				m_pMarkedLink->OnAction( WgInput::BUTTON_PRESS, button_key );
				m_markedLinkMode = WG_MODE_SELECTED;
				bRefresh = true;
			}
			break;
		}
		case WgInput::BUTTON_DOWN:
		{
			if( m_pMarkedLink )
				m_pMarkedLink->OnAction( WgInput::BUTTON_DOWN, button_key );
			break;
		}

		case WgInput::BUTTON_REPEAT:
		{
			if( m_pMarkedLink )
				m_pMarkedLink->OnAction( WgInput::BUTTON_REPEAT, button_key );
			break;
		}

		case WgInput::BUTTON_RELEASE:
		{
			if( m_pMarkedLink )
			{
				m_pMarkedLink->OnAction( WgInput::BUTTON_RELEASE, button_key );

				if( m_markedLinkMode == WG_MODE_SELECTED )
					m_pMarkedLink->OnAction( WgInput::BUTTON_CLICK, button_key );
				
				m_markedLinkMode = WG_MODE_MARKED;
				bRefresh = true;
			}
			break;
		}

		case WgInput::BUTTON_DOUBLECLICK:
			if( m_pMarkedLink )
				m_pMarkedLink->OnAction( WgInput::BUTTON_DOUBLECLICK, button_key );
			break;
	}

	//TODO: Optimize, we don't always need a complete request, sometimes just a render.

	if( bRefresh )
		refresh();

	return bRefresh;
}

