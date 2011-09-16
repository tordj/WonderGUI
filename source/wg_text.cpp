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
#include <wg_base.h>
#include <wg_textlinkhandler.h>
#include <wg_string.h>

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

	m_mode		= WG_MODE_NORMAL;

	m_pHardLines	= new WgTextLine[1];
	m_nHardLines	= 1;

	m_pSoftLines	= m_pHardLines;
	m_nSoftLines	= 1;

	m_pHardLines->nChars = 0;
	m_pHardLines->ofs = 0;

	m_lineWidth		= INT_MAX;
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
void WgText::selectText( int startLine, int startCol, int endLine, int endCol )
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
	if(endCol > pCurLine->nChars)
		endCol = pCurLine->nChars;
}

//_________________________________________________________
bool WgText::getSelection( int& startLine, int& startCol, int& endLine, int& endCol ) const
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

int WgText::getSoftLineWidth( int line ) const
{
	if( line < 0 || line >= nbSoftLines() )
		return 0;

	return m_pSoftLines[line].width;
}

//____ getSoftLineWidthPart() _________________________________________________

int WgText::getSoftLineWidthPart( int _line, int startCol, int nCol ) const
{
	if( _line < 0 || _line >= nbSoftLines() || !m_pBaseProp->GetFont() )
		return 0;

	const WgTextLine * pLine = getSoftLine(_line);

	if( pLine->nChars - startCol < nCol )
		nCol = pLine->nChars-startCol;


	const WgChar * pString = m_buffer.Chars() + pLine->ofs;

	WgPen pen;
	Uint16 hProp = 0xFFFF;

	WgTextAttr	attr;

	pen.SetTextNode( m_pManagerNode );

	for( int i = 0 ; i < nCol ; i++ )
	{
		if( pString->IsEndOfLine() )
			break;

		if( pString->PropHandle() != hProp )
		{
			GetCharAttr( attr, pLine->ofs + i );
			pen.SetAttributes( attr );
		}
		pen.SetChar( pString->Glyph() );
		pen.ApplyKerning();
		pen.AdvancePos();
		pString++;
	}

	return pen.GetPosX();



}


//____ getLineWidth() ____________________________________________________________

int WgText::getLineWidth( int line ) const
{
	if( line >= nbLines() )
		return 0;

	return m_pHardLines[line].width;
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

//____ unwrappedWidth() _______________________________________________________

int WgText::unwrappedWidth() const
{
	int		width = 0;

	for( int i = 0 ; i < m_nHardLines ; i++ )
	{
		if( m_pHardLines[i].width > width )
			width = m_pHardLines[i].width;
	}

	return width;
}

//____ width() ________________________________________________________________

int WgText::width() const
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

int WgText::height() const
{
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

int WgText::softLineHeight( int line )
{
	if( line >= nbSoftLines() )
		return 0;

	return m_pSoftLines[line].height;
}

int WgText::softLineSpacing( int line )
{
	if( line >= nbSoftLines() )
		return 0;

	return m_pSoftLines[line].lineSpacing;
}


void WgText::setLinkProperties( const WgTextPropPtr& pProp )
{
	if( m_pLinkProp != pProp )
	{
		m_pLinkProp = pProp;
		regenSoftLines();		//TODO: Optimize: Only do if we have links.
		refreshAllLines();
	}
}

void WgText::clearLinkProperties()
{
	if( m_pLinkProp )
	{
		m_pLinkProp = 0;
		regenSoftLines();		//TODO: Optimize: Only do if we have links.
		refreshAllLines();
	}
}

void WgText::setSelectionProperties( const WgTextPropPtr& pProp )
{
	if( m_pSelectionProp != pProp )
	{
		m_pSelectionProp = pProp;
		regenSoftLines();		//TODO: Optimize: Only do if we have selected text.
		refreshAllLines();
	}
}

void WgText::clearSelectionProperties()
{
	if( m_pSelectionProp )
	{
		m_pSelectionProp = 0;
		regenSoftLines();		//TODO: Optimize: Only do if we have selected text.
		refreshAllLines();
	}
}


void WgText::setSelectionBgColor(WgColor color )
{
	WgTextProp	prop = * m_pSelectionProp;
	prop.SetBgColor(color);
	m_pSelectionProp = prop.Register();
}


void WgText::setProperties( const WgTextPropPtr& pProp )
{
	m_pBaseProp = pProp;
	regenSoftLines();
	refreshAllLines();
}

void WgText::setColor( const WgColor color )
{
	WgTextProp	prop = * m_pBaseProp;
	prop.SetColor(color);
	m_pBaseProp = prop.Register();
}

void WgText::setColor( const WgColor color, WgMode mode )
{
	WgTextProp	prop = * m_pBaseProp;
	prop.SetColor(color,mode);
	m_pBaseProp = prop.Register();
}

void WgText::setStyle( WgFontStyle style )
{
	WgTextProp	prop = * m_pBaseProp;
	prop.SetStyle(style);
	m_pBaseProp = prop.Register();
	regenSoftLines();
	refreshAllLines();
}

void WgText::setStyle( WgFontStyle style, WgMode mode )
{
	WgTextProp	prop = * m_pBaseProp;
	prop.SetStyle(style,mode);
	m_pBaseProp = prop.Register();
	regenSoftLines();
	refreshAllLines();
}

void WgText::setFont( WgFont * pFont )
{
	WgTextProp	prop = * m_pBaseProp;
	prop.SetFont(pFont);
	m_pBaseProp = prop.Register();
	regenSoftLines();
	refreshAllLines();
}

void WgText::setLink( const WgTextLinkPtr& pLink )
{
	WgTextProp	prop = * m_pBaseProp;
	prop.SetLink(pLink);
	m_pBaseProp = prop.Register();
	regenSoftLines();
	refreshAllLines();
}

void WgText::setBreakLevel( int level )
{
	WgTextProp	prop = * m_pBaseProp;
	prop.SetBreakLevel(level);
	m_pBaseProp = prop.Register();
	regenSoftLines();
	refreshAllLines();
}

void WgText::clearProperties()
{
	if( m_pBaseProp )
	{
		m_pBaseProp = 0;
		regenSoftLines();
		refreshAllLines();
	}
}

void WgText::clearColor()
{
	WgTextProp	prop = * m_pBaseProp;
	prop.ClearColor();
	m_pBaseProp = prop.Register();
}

void WgText::clearColor( WgMode mode )
{
	WgTextProp	prop = * m_pBaseProp;
	prop.ClearColor(mode);
	m_pBaseProp = prop.Register();
}

void WgText::clearStyle()
{
	WgTextProp	prop = * m_pBaseProp;
	prop.ClearStyle();
	m_pBaseProp = prop.Register();
	regenSoftLines();
	refreshAllLines();
}

void WgText::clearStyle( WgMode mode )
{
	WgTextProp	prop = * m_pBaseProp;
	prop.ClearStyle(mode);
	m_pBaseProp = prop.Register();
	regenSoftLines();
	refreshAllLines();
}

void WgText::clearBreakLevel()
{
	WgTextProp	prop = * m_pBaseProp;
	prop.ClearBreakLevel();
	m_pBaseProp = prop.Register();
	regenSoftLines();
	refreshAllLines();
}

void WgText::clearFont()
{
	WgTextProp	prop = * m_pBaseProp;
	prop.ClearFont();
	m_pBaseProp = prop.Register();
	regenSoftLines();
	refreshAllLines();
}

void WgText::clearLink()
{
	WgTextProp	prop = * m_pBaseProp;
	prop.ClearLink();
	m_pBaseProp = prop.Register();
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
	int bytes = getTextSizeUTF8();
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

int	WgText::getTextUTF8( char * pDest, int maxBytes ) const
{
	return WgTextTool::getTextUTF8(getText(), pDest, maxBytes);
}

//____ getTextSizeUTF8() ______________________________________________________

int	WgText::getTextSizeUTF8() const
{
	return WgTextTool::getTextSizeUTF8(getText());
}

//____ getTextFormattedUTF8() _________________________________________________

char * WgText::getTextFormattedUTF8() const
{
	int bytes = getTextSizeFormattedUTF8();
	char * pDest = new char[bytes+1];
	getTextFormattedUTF8(pDest, bytes+1);
	return pDest;
}

//____ getTextFormattedUTF8() __________________________________________________

int WgText::getTextFormattedUTF8( char * pDest, int maxBytes ) const
{
	return WgTextTool::getTextFormattedUTF8( getText(), pDest, maxBytes );
}

//____ getTextSizeFormattedUTF8() ______________________________________________

int WgText::getTextSizeFormattedUTF8() const
{
	return WgTextTool::getTextSizeFormattedUTF8(getText(), INT_MAX);
}


char * WgText::getSelectedTextFormattedUTF8() const
{
	if(m_selStartLine == m_selEndLine && m_selStartCol == m_selEndCol)
		return 0;

	const WgChar* pTextStart = getLineText(m_selStartLine) + m_selStartCol;
	const WgChar* pTextEnd = getLineText(m_selEndLine) + m_selEndCol;
	int nChars = pTextEnd - pTextStart;
	int nBytes = WgTextTool::getTextSizeFormattedUTF8(pTextStart, nChars);
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
	int nBytes = WgTextTool::getTextSizeUTF8(pTextStart, nChars);
	char * pDest = new char[nBytes + 1];
	WgTextTool::getTextUTF8(pTextStart, pDest, nBytes + 1);
	return pDest;
}


//____ compareTo() ____________________________________________________________
/*
int WgText::compareTo( const WgText * _pOther, bool bCheckCase ) const
{

	int	diff = 0;
	Uint32	line = 0;

	WgChar * pMine = getLineText( 0 );
	WgChar * pOther = _pOther->getLineText( 0 );

	if( bCheckCase )
	{
		// default, case sensitive comparison
		while( pMine && pOther )
		{
			int	ofs = -1;

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
			int	ofs = -1;

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

	const WgChar * pSuffix = f.suffix.Chars();

	for( unsigned int i = 0 ; i < f.suffix.Length() && i < 4 ; i++ )
		* p++ = pSuffix[i];

	// Terminate string

	p->SetGlyph(0);
	p++;

	// Write integer part to temp area (backwards)

	Uint16	temp2[32];

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

	const WgChar * pPrefix = f.prefix.Chars();

	for( int i = f.prefix.Length()-1 ; i >= 0 ; i-- )
		* --p = pPrefix[i];

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

	const WgChar * pSuffix = f.suffix.Chars();

	for( unsigned int i = 0 ; i < f.suffix.Length() && i < 4 ; i++ )
		* p++ = pSuffix[i];

	// Terminate string

	p++->SetGlyph(0);						// Just to be sure, might already have been terminated as part of suffix.

	// Write integer part to temp area (backwards)

	Uint16	temp2[32];

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

	const WgChar * pPrefix = f.prefix.Chars();

	for( int i = f.prefix.Length()-1 ; i >= 0 ; i-- )
		* --p = pPrefix[i];

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

int WgText::nbChars() const
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

int WgText::nbLines() const
{
	return m_nHardLines;
}

//____ nbSoftLines() __________________________________________________________

int WgText::nbSoftLines() const
{
	return m_nSoftLines;
}

int WgText::getSoftLineSelectionWidth(int line) const
{
	int w = getSoftLineWidth(line);
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

void WgText::setLineWidth( int width )
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

void WgText::posSoft2Hard( int &line, int &col ) const
{
	// Get a character pointer from soft line/col

	if( line < 0 )
		line = 0;

	if( line >= m_nSoftLines )
		line = m_nSoftLines - 1;

	WgTextLine * pLine = &m_pSoftLines[line];

	const WgChar * pOfs = m_buffer.Chars() + pLine->ofs;

	if( col > pLine->nChars )
		pOfs += pLine->nChars;
	else if( col > 0 )
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

void WgText::posHard2Soft( int &line, int &col ) const
{
	// Get a character pointer from hard line/col

	if( line >= m_nHardLines )
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

//____ countWriteSoftLines() __________________________________________________

int WgText::countWriteSoftLines( const WgChar * pStart, WgTextLine * pWriteLines, int maxWrite )
{
	const WgChar *	p = pStart;
	int			nSoftLines = 0;
	Uint16		hProp = 0xFFFF;					// Force immediate update of textprop.
	bool		bEndOfText = false;

	WgPen		pen;
	WgTextAttr	attr;

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
				GetCharAttr(attr, p - m_buffer.Chars());
				pen.SetAttributes(attr);
				hProp = p->PropHandle();
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

			// Handle a "break-after" from previous character

			if( bBreakAfterPrev )
			{
				pbp = p;
				bBreakSkips = false;
				bBreakAfterPrev = false;
			}


			// Check if we can move the breakpoint up to this character.

			WgBreakRules breakStatus = WgTextTool::isBreakAllowed( p->glyph, attr.breakLevel );
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
					if( !pHyphen || (pen.GetPosX() + pHyphen->Advance()) > m_lineWidth )
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
				break;

			}

			// Increase line length

			pen.SetChar( p->Glyph() );
			pen.ApplyKerning();


			// Check if we need to put a softbreak.

//			Uint32 len = pen.GetBlitPosX() + pen.GetGlyph()->rect.w; // No advance on last character of line, just bearingX + width
			int len = pen.GetPosX() + pen.GetGlyph()->Advance();
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
	Uint16	hProp = 0xFFFF;

	WgPen pen;
	pen.SetTextNode( m_pManagerNode );

	// We must have at least one character, even if it is just CR/LF or EndOfString.

	if( nChars == 0 )
	{
		WgTextAttr	attr;
		GetBaseAttr( attr );
		pen.SetAttributes( attr );

		if( pen.GetGlyphSet() != 0 )
		{
			int height		= pen.GetLineHeight();
			int baseline	= pen.GetBaseline();
			int spacing		= pen.GetLineSpacing();

			maxAscend = baseline;
			maxDescend = height - baseline;
			maxLineDescend = spacing - baseline;
		}
	}
	else
	{
		// Loop through characters of the line, moving pen and updating variables.

		for( int i = 0 ; i < nChars ; i++ )
		{
			if( pChars[i].PropHandle() != hProp )
			{
				WgTextAttr	attr;
				GetCharAttr( attr, pLine->ofs + i );
				pen.SetAttributes( attr );

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
	}

	// Fill in line struct.

	pLine->width		= pen.GetPosX();
	pLine->height		= maxAscend + maxDescend;
	pLine->lineSpacing	= maxAscend + maxLineDescend + m_lineSpaceAdj;
	pLine->baseline		= maxAscend;

	// Add cursor margin to width

	//TODO: Take cursor margins into account.

}

//____ CursorGotoCoord() ________________________________________________________

void WgText::CursorGotoCoord( const WgCord& coord, const WgRect& container )
{
	if( !m_pCursor )
		return;

	WgTextPos pos = CoordToPos( coord, container, true );
	gotoSoftPos(pos.line,pos.col);
}

//____ CursorGotoLine() _________________________________________________________

void WgText::CursorGotoLine( int line, const WgRect& container )
{
	if( !m_pCursor )
		return;

	m_pCursor->gotoSoftLine(line, container );
}

//____ CursorGoUp() _________________________________________________________

void WgText::CursorGoUp( int nbLines, const WgRect& container )
{
	if( !m_pCursor || nbLines == 0 )
		return;

	int line, col;
	getSoftPos( line, col );

	line -= nbLines;
	if( line < 0 )
		line = 0;

	m_pCursor->gotoSoftLine(line, container );
}

//____ CursorGoDown() _________________________________________________________

void WgText::CursorGoDown( int nbLines, const WgRect& container )
{
	if( !m_pCursor || nbLines == 0 )
		return;

	int line, col;
	getSoftPos( line, col );

	line += nbLines;
	if( line >= m_nSoftLines )
		line = m_nSoftLines-1;

	m_pCursor->gotoSoftLine(line, container );
}


//____ LineStartY() ______________________________________________________________

int WgText::LineStartY( int line, const WgRect& container ) const
{
	int		ofs = 0;
	if( m_origo.anchorY() != 0 || m_origo.hotspotY() != 0 )
		ofs = m_origo.calcOfsY( container.h, height() );
	if( line > m_nSoftLines )
		line = m_nSoftLines;

	for( int i = 0 ; i < line ; i++ )
		ofs += m_pSoftLines[i].lineSpacing;

	return ofs + container.y;
}

//____ LineStartX() ______________________________________________________________

int WgText::LineStartX( int line, const WgRect& container ) const
{
	int		ofs = 0;

	if( m_origo.anchorX() != 0 || m_origo.hotspotX() != 0 )
	{
		ofs = m_origo.calcOfsX( container.w, getSoftLineWidth(line) );
		if( ofs < 0 )
			ofs = 0;
	}

	return ofs + container.x;
}

//____ CoordToLine() __________________________________________________________

int WgText::CoordToLine( const WgCord& coord, const WgRect& container, bool bCursorMode ) const
{
	int y = coord.y - LineStartY( 0, container );

	WgTextLine * pLine = m_pSoftLines;
	for( int i = 0 ; i < m_nSoftLines ; i++ )
	{
		if( y < 0 )
		{
			if( bCursorMode )
				return i;
			else
				return -1;		// Above first line or in space between lines.
		}

		if( y < pLine->height )
			return i;

		y -= pLine->lineSpacing;
	}

	//

	if( bCursorMode )
		return m_nSoftLines-1;
	else
		return -1;				// Below last line.
}

//____ CoordToColumn() ___________________________________________________________

int WgText::CoordToColumn( int line, const WgCord& coord, const WgRect& container, bool bCursorMode ) const
{
	//TODO: Take cursor and selection into account!!!

	if( line < 0 || line >= m_nSoftLines )
		return -1;

	WgTextLine * pLine = &m_pSoftLines[line];

	int xStart = LineStartX( line, container );

	if( coord.x < xStart )
		return bCursorMode?0:-1;

	if( coord.x >= xStart + pLine->width )
		return bCursorMode?pLine->nChars:-1;

	const WgChar * pChars = m_buffer.Chars() + pLine->ofs;
	Uint16	hProp = 0xFFFF;
	WgPen pen;
	pen.SetTextNode( m_pManagerNode );
	pen.SetPosX(xStart);

	for( int i = 0 ; i < pLine->nChars ; i++ )
	{
		if( pChars[i].PropHandle()!= hProp )
		{
			WgTextAttr	attr;
			GetCharAttr( attr, pLine->ofs + i );
			pen.SetAttributes( attr );

			hProp = pChars[i].PropHandle();
		}
/*
		if( cursColumn == 0 )
			pen.AdvancePosCursor( * pCursor );
		cursColumn--;
*/
		int charStartX = pen.GetPosX();
		pen.SetChar( pChars[i].Glyph() );
		pen.ApplyKerning();
		pen.AdvancePos();
		int charEndX = pen.GetPosX();

		if( charEndX > coord.x )
		{
			if( bCursorMode && (charEndX - coord.x) <= (coord.x - charStartX) )
				return i+1;
			else
				return i;
		}
	}

	return bCursorMode?pLine->nChars:-1;			// We should never get here...
}

//_____ CoordToPos() __________________________________________________________

WgTextPos WgText::CoordToPos( const WgCord& coord, const WgRect& container, bool bCursorMode ) const
{
	int line = CoordToLine( coord, container, bCursorMode );
	if( line == -1 )
		return WgTextPos(-1,-1);

	int col = CoordToColumn( line, coord, container, bCursorMode );
	if( col == -1 )
		return WgTextPos(-1,-1);

	return WgTextPos(line,col);
}

//____ CoordToOfs() ___________________________________________________________

int WgText::CoordToOfs( const WgCord& coord, const WgRect& container, bool bCursorMode ) const
{
	WgTextPos pos = CoordToPos( coord, container, bCursorMode );

	if( pos.line == -1 || pos.col == -1 )
		return -1;
	else
		return PosToOfs( pos );
}


//____ CoordToLink() __________________________________________________________

WgTextLinkPtr WgText::CoordToLink( const WgCord& coord, const WgRect& container ) const
{
	int ofs = CoordToOfs( coord, container );
	if( ofs >= 0 )
		return GetCharLink( ofs );
	else
		return 0;
}

//____ OfsToCoordY() _________________________________________________________

int WgText::OfsToCoordY( int ofs, const WgRect& container ) const
{
	int y = LineStartY( OfsToPos(ofs).line, container );

	for( int i = 0 ; i < m_nSoftLines ; i++ )
	{
		if( ofs < m_pSoftLines[i].ofs + m_pSoftLines[i].nChars )
			break;

		y += m_pSoftLines[i].lineSpacing;
	}
	return y;
}

//____ OfsToCoordX() _________________________________________________________

int WgText::OfsToCoordX( int ofs, const WgRect& container ) const
{
	return PosToCoordX( OfsToPos(ofs), container );
}

//____ OfsToCoord() ___________________________________________________________

WgCord WgText::OfsToCoord( int ofs, const WgRect& container ) const
{
	return PosToCoord( OfsToPos(ofs), container );
}

//____ OfsToPos() _____________________________________________________________

WgTextPos WgText::OfsToPos( int ofs ) const
{
	if( ofs < 0 )
		return WgTextPos(0,0);

	if( ofs >= (int) m_buffer.Length() )
		return WgTextPos(m_nSoftLines-1, m_pSoftLines[m_nSoftLines-1].nChars);

	int ln = 0;
	while( m_pSoftLines[ln].ofs + m_pSoftLines[ln].nChars < ofs )
		ln++;

	return WgTextPos(ln, ofs - m_pSoftLines[ln].ofs);
}

//____ PosToCoordX() __________________________________________________________

int WgText::PosToCoordX( const WgTextPos& _pos, const WgRect& container ) const
{
	//TODO: Take cursor and selection into account!!!

	WgTextPos	pos = ClampPos(_pos);

	WgTextLine * pLine = &m_pSoftLines[pos.line];

	const WgChar * pChars = m_buffer.Chars() + pLine->ofs;
	Uint16	hProp = 0xFFFF;
	WgPen pen;
	pen.SetTextNode( m_pManagerNode );
	pen.SetPosX(LineStartX( pos.line, container ) );

	for( int i = 0 ; i < pos.col ; i++ )
	{
		if( pChars[i].PropHandle()!= hProp )
		{
			WgTextAttr	attr;
			GetCharAttr( attr, pLine->ofs + i );
			pen.SetAttributes( attr );

			hProp = pChars[i].PropHandle();
		}
/*
		if( cursColumn == 0 )
			pen.AdvancePosCursor( * pCursor );
		cursColumn--;
*/

		pen.SetChar( pChars[i].Glyph() );
		pen.ApplyKerning();
		pen.AdvancePos();
	}

	return pen.GetPosX();
}

//____ PosToCoordY() __________________________________________________________

int WgText::PosToCoordY( const WgTextPos& pos, const WgRect& container ) const
{
	return LineStartY( pos.line, container );
}

//____ PosToCoord() ___________________________________________________________

WgCord WgText::PosToCoord( const WgTextPos& pos, const WgRect& container ) const
{
	return WgCord( PosToCoordX(pos, container), PosToCoordY(pos, container) );
}

//____ PosToOfs() _____________________________________________________________

int WgText::PosToOfs( const WgTextPos& _pos ) const
{
	WgTextPos pos = ClampPos(_pos);
	return m_pSoftLines[pos.line].ofs + pos.col;
}

//____ ClampPos() _____________________________________________________________

WgTextPos WgText::ClampPos( WgTextPos pos ) const
{
	if( pos.line < 0 )
		return WgTextPos(0,0);

	if( pos.line >= m_nSoftLines )
		return WgTextPos(m_nSoftLines-1, m_pSoftLines[m_nSoftLines-1].nChars);

	if( pos.col < 0 )
		return WgTextPos(pos.line,0);

	if( pos.col > m_pSoftLines[pos.line].nChars )
		return WgTextPos(pos.line, m_pSoftLines[pos.line].nChars );

	return pos;
}

//____ LineColToOffset() ______________________________________________________

int WgText::LineColToOffset(int line, int col) const
{
	return m_pHardLines[line].ofs + col;
}

//____ OnAction() _____________________________________________________________

bool WgText::OnAction( WgInput::UserAction action, int button_key, const WgRect& container, const WgCord& pointerOfs )
{
	bool bRefresh = false;
	WgTextLinkHandler * pHandler;

	if( m_pMarkedLink )
	{
		pHandler = m_pMarkedLink->Handler();
		if( !pHandler )
			pHandler = WgBase::GetDefaultTextLinkHandler();
	}

	switch( action )
	{
		case WgInput::POINTER_ENTER:
		case WgInput::POINTER_OVER:
		{
			WgTextLinkPtr pLink = CoordToLink( pointerOfs, container );
			if( m_pMarkedLink && pLink != m_pMarkedLink )
			{
				if( pHandler )
					pHandler->OnPointerExit( m_pMarkedLink, pointerOfs );
				m_pMarkedLink = 0;
				bRefresh = true;
			}

			if( pLink )
			{
				if( pLink != m_pMarkedLink )
				{
					pHandler = pLink->Handler();
					if( !pHandler )
						pHandler = WgBase::GetDefaultTextLinkHandler();

					if( pHandler )
						pHandler->OnPointerEnter( m_pMarkedLink, pointerOfs );

					m_pMarkedLink = pLink;
					m_markedLinkMode = WG_MODE_MARKED;
					bRefresh = true;
				}
				if( pHandler )
					pHandler->OnPointerOver( m_pMarkedLink, pointerOfs );
			}
			break;
		}


		case WgInput::POINTER_EXIT:
		{
			if( m_pMarkedLink )
			{
				if( pHandler )
					pHandler->OnPointerExit( m_pMarkedLink, pointerOfs );
				m_pMarkedLink = 0;
				bRefresh = true;
			}
			break;
		}

		case WgInput::BUTTON_PRESS:
		{
			if( m_pMarkedLink )
			{
				if( pHandler )
					pHandler->OnButtonPress( button_key, m_pMarkedLink, pointerOfs );
				m_markedLinkMode = WG_MODE_SELECTED;
				bRefresh = true;
			}
			break;
		}

		case WgInput::BUTTON_REPEAT:
		{
			if( m_pMarkedLink )
			{
				if( pHandler )
					pHandler->OnButtonRepeat( button_key, m_pMarkedLink, pointerOfs );
			}
			break;
		}

		case WgInput::BUTTON_RELEASE:
		{
			if( m_pMarkedLink )
			{
				if( pHandler )
				{
					pHandler->OnButtonRelease( button_key, m_pMarkedLink, pointerOfs );

					if( m_markedLinkMode == WG_MODE_SELECTED )
						pHandler->OnButtonClick( button_key, m_pMarkedLink, pointerOfs );
				}

				m_markedLinkMode = WG_MODE_MARKED;
				bRefresh = true;
			}
			break;
		}

		case WgInput::BUTTON_DOUBLECLICK:
			if( m_pMarkedLink && pHandler )
				pHandler->OnButtonDoubleClick( button_key, m_pMarkedLink, pointerOfs );
			break;

		default:
			break;
	}

	//TODO: Optimize, we don't always need a complete refresh, sometimes just a render.

	if( bRefresh )
		refresh();

	return bRefresh;
}

//____ GetBaseAttr() __________________________________________________________

void WgText::GetBaseAttr( WgTextAttr& attr ) const
{
	attr.Clear();
	WgTextTool::AddPropAttributes( attr, WgBase::GetDefaultTextProp(), m_mode );
	WgTextTool::SetAttrColor( attr, m_pBgBlockColors, m_mode );
	WgTextTool::SetAttrColor( attr, m_pBaseColors, m_mode );
	WgTextTool::AddPropAttributes( attr, m_pBaseProp, m_mode );
}

//____ GetCharAttr() __________________________________________________________

bool WgText::GetCharAttr( WgTextAttr& attr, int charOfs ) const
{
	if( charOfs >= (int) m_buffer.Length() )
		return false;

	attr.Clear();
	WgTextTool::AddPropAttributes( attr, WgBase::GetDefaultTextProp(), m_mode );
	WgTextTool::SetAttrColor( attr, m_pBgBlockColors, m_mode );
	WgTextTool::SetAttrColor( attr, m_pBaseColors, m_mode );
	WgTextTool::AddPropAttributes( attr, m_pBaseProp, m_mode );

	// Add selection properties if character is selected

	if( m_selStartLine != m_selEndLine || m_selStartCol != m_selEndCol )
	{
		int selStart = LineColToOffset( m_selStartLine, m_selStartCol );
		int selEnd = LineColToOffset( m_selEndLine, m_selEndCol );

		if( charOfs >= selStart && charOfs < selEnd )
		{
			if( m_pSelectionProp )
				WgTextTool::AddPropAttributes( attr, m_pSelectionProp, m_mode );
			else
				WgTextTool::AddPropAttributes( attr, WgBase::GetDefaultSelectionProp(), m_mode );
		}
	}

	// Add link properties if character is part of a link

	WgTextPropPtr pCharProp = m_buffer.Chars()[charOfs].Properties();

	WgTextLinkPtr pLink = pCharProp->GetLink();
	if( !pLink )
		pLink = m_pBaseProp->GetLink();

	if( pLink )
	{
		WgTextPropPtr pProp = m_pLinkProp?m_pLinkProp:WgBase::GetDefaultLinkProp();

		WgMode	mode;

		if( pLink == m_pMarkedLink )
		{
			mode = m_markedLinkMode;
		}
		else
		{
			if( m_mode == WG_MODE_DISABLED )
				mode = WG_MODE_DISABLED;
			else if( pLink->HasBeenAccessed() )
				mode = WG_MODE_SPECIAL;
			else
				mode = WG_MODE_NORMAL;
		}


		WgTextTool::AddPropAttributes( attr, pProp, mode );
	}

	// Add characters own properties

	WgTextTool::AddPropAttributes( attr, pCharProp, m_mode );

	return true;
}

bool WgText::IsCharUnderlined( int charOfs ) const
{
	//TODO: Optimize
	WgTextAttr	attr;
	GetCharAttr(attr,charOfs);
	return attr.bUnderlined;
}

WgColor WgText::GetCharColor( int charOfs ) const
{
	//TODO: Optimize
	WgTextAttr	attr;
	GetCharAttr(attr,charOfs);
	return attr.color;
}

WgColor WgText::GetCharBgColor( int charOfs ) const
{
	//TODO: Optimize
	WgTextAttr	attr;
	GetCharAttr(attr,charOfs);
	return attr.bgColor;
}

WgFontStyle WgText::GetCharStyle( int charOfs ) const
{
	//TODO: Optimize
	WgTextAttr	attr;
	GetCharAttr(attr,charOfs);
	return attr.style;
}

int WgText::GetCharSize( int charOfs ) const
{
	//TODO: Optimize
	WgTextAttr	attr;
	GetCharAttr(attr,charOfs);
	return attr.size;
}

WgFont * WgText::GetCharFont( int charOfs ) const
{
	//TODO: Optimize
	WgTextAttr	attr;
	GetCharAttr(attr,charOfs);
	return attr.pFont;
}

int WgText::GetCharBreakLevel( int charOfs ) const
{
	//TODO: Optimize
	WgTextAttr	attr;
	GetCharAttr(attr,charOfs);
	return attr.breakLevel;
}

WgTextLinkPtr WgText::GetCharLink( int charOfs ) const
{
	//TODO: Optimize
	WgTextAttr	attr;
	GetCharAttr(attr,charOfs);
	return attr.pLink;
}

bool WgText::IsCharLink( int charOfs ) const
{
	//TODO: Optimize
	WgTextAttr	attr;
	GetCharAttr(attr,charOfs);
	return attr.pLink?true:false;
}
