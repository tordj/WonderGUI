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

WgChar		WgText::g_emptyText = WgChar(0);
WgTextLine	WgText::g_emptyLine = {0,&g_emptyText};

//____ Constructor ____________________________________________________________


WgText::WgText()
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

WgText::WgText( const char * pText )
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

	setText( pText );
}

WgText::WgText( const Uint16 * pText )
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

	setText( pText );
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


//_____ clear() _______________________________________________________________

void WgText::clear()
{
	beginChange( 0,0, 0, nbChars(), 0,nbLines()-1 );
	endChange( 0, 1 );
}


void WgText::setText( const WgChar * pText, Uint32 nChar )
{
	// If pText is null we should just clear the text.

	if( !pText )
	{
		clear();
		return;
	}

	// Count number of characters and lines

	Uint32		nLines	= 0;
	Uint32		nChars  = 0;

	WgTextTool::countCharsLines( pText, nChars, nLines, nChar );

	// Update text

	WgChar * wp = beginChange( 0, 0, nChars, nbChars(), nLines, nbLines() );

	for( Uint32 i = 0 ; i < nChars ; i++ )
		wp[i] = pText[i];

	endChange( 0, nLines );

	return;
}

//_________________________________________________________
void WgText::selectText( Uint32 startLine, Uint32 startCol, Uint32 endLine, Uint32 endCol )
{
	if(endLine < startLine || endLine == startLine && endCol < startCol)
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


//____ setFormattedText() _____________________________________________________

Uint32 WgText::setFormattedText( const char * pText, Uint32 nChar, const WgResDB * pResDB )
{
	Uint32 len = WgTextTool::countNonFormattingChars( pText );

	Uint32	nLines = WgTextTool::countLines( pText );

	if( len > nChar )
		len = nChar;

	WgChar * wp = beginChange( 0, 0, len, nbChars(), nLines, nbLines() );
	WgTextTool::readFormattedString( pText, wp, len, pResDB );
	endChange( 0, nLines );

	return len;
}

Uint32 WgText::setFormattedText( const Uint16 * pText, Uint32 nChar, const WgResDB * pResDB )
{
	Uint32 len = WgTextTool::countNonFormattingChars( pText );

	Uint32	nLines = WgTextTool::countLines( pText );

	if( len > nChar )
		len = nChar;

	WgChar * wp = beginChange( 0, 0, len, nbChars(), nLines, nbLines() );
	WgTextTool::readFormattedString( pText, wp, len, pResDB );
	endChange( 0, nLines );

	return len;
}



//____ getSoftLineWidth() _____________________________________________________

Uint32 WgText::getSoftLineWidth( Uint32 line ) const
{
	//TODO: Should just check lines width-variable when that is maintained.

	if( line >= nbSoftLines() || !m_pProp->GetFont() )
		return 0;

	return WgTextTool::lineWidthSoft( m_pManagerNode, m_pProp, m_mode, getSoftLineText(line) );
}

//____ getSoftLineWidthPart() _________________________________________________

Uint32 WgText::getSoftLineWidthPart( Uint32 _line, Uint32 startCol, Uint32 nCol ) const
{
	if( _line >= nbSoftLines() || !m_pProp->GetFont() )
		return 0;

	const WgTextLine * pLine = getSoftLine(_line);

	if( nCol > pLine->nChars )
		return 0;

	return WgTextTool::lineWidthPartSoft( m_pManagerNode, m_pProp, m_mode, pLine->pText, nCol );
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

	return WgTextTool::lineWidthPart( m_pManagerNode, m_pProp, m_mode, pLine->pText, nCol );
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

	Uint32 fontheight = m_pProp->GetFont()->GetGlyphSet(m_pProp->GetStyle(), size )->GetHeight(size);

	int h2 = fontheight+m_lineSpaceAdj;
	if( h2 < 0 )
		h2 = 0;

	return fontheight + h2*(nbSoftLines()-1);		//TODO: Count right even if style changes over the text and lines.
}

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


void WgText::setDefaultProperties( const WgTextPropPtr& pProp )
{
	m_pProp = pProp;
	regenSoftLines();
}

void WgText::setDefaultColor( const WgColor color )
{
	WgTextProp	prop = * m_pProp;
	prop.SetColor(color);
	m_pProp = prop.Register();
}

void WgText::setDefaultColor( const WgColor color, WgMode mode )
{
	WgTextProp	prop = * m_pProp;
	prop.SetColor(color,mode);
	m_pProp = prop.Register();
}

void WgText::setDefaultStyle( WgFontStyle style )
{
	WgTextProp	prop = * m_pProp;
	prop.SetStyle(style);
	m_pProp = prop.Register();
	regenSoftLines();
}

void WgText::setDefaultStyle( WgFontStyle style, WgMode mode )
{
	WgTextProp	prop = * m_pProp;
	prop.SetStyle(style,mode);
	m_pProp = prop.Register();
	regenSoftLines();
}

void WgText::setDefaultFont( WgFont * pFont )
{
	WgTextProp	prop = * m_pProp;
	prop.SetFont(pFont);
	m_pProp = prop.Register();
	regenSoftLines();
}

void WgText::clearDefaultProperties()
{
	m_pProp = 0;
}

void WgText::clearDefaultColor()
{
	WgTextProp	prop = * m_pProp;
	prop.ClearColor();
	m_pProp = prop.Register();
}

void WgText::clearDefaultColor( WgMode mode )
{
	WgTextProp	prop = * m_pProp;
	prop.ClearColor(mode);
	m_pProp = prop.Register();
}

void WgText::clearDefaultStyle()
{
	WgTextProp	prop = * m_pProp;
	prop.ClearStyle();
	m_pProp = prop.Register();
	regenSoftLines();
}

void WgText::clearDefaultStyle( WgMode mode )
{
	WgTextProp	prop = * m_pProp;
	prop.ClearStyle(mode);
	m_pProp = prop.Register();
	regenSoftLines();
}

void WgText::clearDefaultFont()
{
	WgTextProp	prop = * m_pProp;
	prop.ClearFont();
	m_pProp = prop.Register();
	regenSoftLines();
}




//____ setProperties() _________________________________________________________

void WgText::setProperties( const WgTextPropPtr& pProp )
{
	m_pProp = pProp;
	WgTextTool::SetProperties( 0, (WgChar *) getText(), nbChars() );
	regenSoftLines();
}


//____ setColor()______________________________________________________________
/*
void WgText::setColor( const WgColor color )
{
	WgTextProp	prop = * m_pProp;
	prop.SetColor(color);
	m_pProp = prop.Register();

	WgTextTool::ModifyProperties( WgCharSeqRW(this), WgTextTool::PropColorClearer() );
}
*/

void WgText::setColor( const WgColor color, WgMode mode )
{
	WgTextProp	prop = * m_pProp;
	prop.SetColor(color,mode);
	m_pProp = prop.Register();

	WgTextTool::ModifyProperties( WgTextTool::PropColorClearer(mode), (WgChar*) getText(), nbChars() );
}


//____ clearColor() ___________________________________________________________

void WgText::clearColor()
{
	WgTextProp	prop = * m_pProp;
	prop.ClearColor();
	m_pProp = prop.Register();

	WgTextTool::ModifyProperties( WgTextTool::PropColorClearer(WG_MODE_ALL), (WgChar*) getText(), nbChars() );
}


void WgText::clearColor( WgMode mode )
{
	WgTextProp	prop = * m_pProp;
	prop.ClearColor(mode);
	m_pProp = prop.Register();

	WgTextTool::ModifyProperties( WgTextTool::PropColorClearer(mode), (WgChar*) getText(), nbChars() );
}



//____ setFont() ______________________________________________________________

void WgText::setFont( WgFont * pFont )
{
	WgTextProp	prop = * m_pProp;
	prop.SetFont(pFont);
	m_pProp = prop.Register();

	WgTextTool::ModifyProperties( WgTextTool::PropFontModifier(pFont), (WgChar*) getText(), nbChars() );
	regenSoftLines();
}


//____ clearFont() ____________________________________________________________

void WgText::clearFont()
{
	WgTextProp	prop = * m_pProp;
	prop.ClearFont();
	m_pProp = prop.Register();

	WgTextTool::ModifyProperties( WgTextTool::PropFontModifier(0), (WgChar*) getText(), nbChars() );
}


//____ setStyle() ______________________________________________________________

void WgText::setStyle( WgFontStyle style )
{
	WgTextProp	prop = * m_pProp;
	prop.SetStyle(style);
	m_pProp = prop.Register();

	WgTextTool::ModifyProperties( WgTextTool::PropStyleModifier(style, WG_MODE_ALL), (WgChar*) getText(), nbChars() );
}



void WgText::setStyle( WgFontStyle style, WgMode mode )
{
	WgTextProp	prop = * m_pProp;
	prop.SetStyle(style,mode);
	m_pProp = prop.Register();

	WgTextTool::ModifyProperties( WgTextTool::PropStyleModifier(style,mode), (WgChar*) getText(), nbChars() );
}



//____ clearStyle() ____________________________________________________________

void WgText::clearStyle()
{
	WgTextProp	prop = * m_pProp;
	prop.ClearStyle();
	m_pProp = prop.Register();

	WgTextTool::ModifyProperties( WgTextTool::PropStyleModifier(WG_STYLE_NORMAL, WG_MODE_ALL), (WgChar*) getText(), nbChars() );
}


void WgText::clearStyle( WgMode mode )
{
	WgTextProp	prop = * m_pProp;
	prop.ClearStyle(mode);
	m_pProp = prop.Register();

	WgTextTool::ModifyProperties( WgTextTool::PropStyleModifier(WG_STYLE_NORMAL, mode), (WgChar*) getText(), nbChars() );
}



//____ setUnderlined() ________________________________________________________

void WgText::setUnderlined()
{
	WgTextProp	prop = * m_pProp;
	prop.SetUnderlined();
	m_pProp = prop.Register();

	WgTextTool::ModifyProperties( WgTextTool::PropUnderlinedModifier(false, WG_MODE_ALL), (WgChar*) getText(), nbChars() );
}

void WgText::setUnderlined(WgMode mode)
{
	WgTextProp	prop = * m_pProp;
	prop.SetUnderlined(mode);
	m_pProp = prop.Register();

	WgTextTool::ModifyProperties( WgTextTool::PropUnderlinedModifier(false,mode), (WgChar*) getText(), nbChars() );
}



//____ clearUnderlined() ______________________________________________________

void WgText::clearUnderlined()
{
	WgTextProp	prop = * m_pProp;
	prop.ClearUnderlined();
	m_pProp = prop.Register();

	WgTextTool::ModifyProperties( WgTextTool::PropUnderlinedModifier(false,WG_MODE_ALL), (WgChar*) getText(), nbChars() );
}

void WgText::clearUnderlined( WgMode mode)
{
	WgTextProp	prop = * m_pProp;
	prop.ClearUnderlined(mode);
	m_pProp = prop.Register();

	WgTextTool::ModifyProperties( WgTextTool::PropUnderlinedModifier(false,mode), (WgChar*) getText(), nbChars() );
}



//____ getLineUTF8() __________________________________________________________
/**
	Counts the number of bytes needed to represent the line in UTF-format,
	reserves a memory area just large enough, generates a zero-terminated string
	and returns a pointer to it.

	@returns	Pointer to the string in UTF8-format. Caller needs to delete[]
				the string afterwards.
*/
char *	WgText::getLineUTF8( Uint32 _line ) const
{
	if( _line >= nbLines() )
		return 0;

	Uint32 bytes = getLineSizeUTF8(_line)+1;
	char * pDest = new char[bytes];
	getLineUTF8(_line, pDest, bytes);
	return pDest;
}

//____ getLineUTF8() __________________________________________________________
/**
	Generates a representation of the line in UTF8-format in the specified
	memory area.

	@param		maxBytes	Maximum number of bytes for the line, including
							zero-termination.

	@returns		Length of UTF8-string written to the area (excluding
					termination character). No incomplete multibyte characters
					are written, so number of bytes written can be less
					than maxChars-1 even when whole string didn't fit.

*/
Uint32	WgText::getLineUTF8( Uint32 _line, char * pDest, Uint32 maxBytes ) const
{
	if( _line >= nbLines() )
		return 0;

	Uint32 bytes = getLineSizeUTF8(_line)+1;
	if( bytes < maxBytes )
		maxBytes = bytes;

	WgTextLine * pLine = getLine(_line);
	return WgTextTool::getTextUTF8( pLine->pText, pDest, maxBytes );
}

//____ getLineSizeUTF8() ______________________________________________________
/**
	@returns	The number of bytes needed to represent the line in UTF8-format.
				String terminator not included.
*/
Uint32	WgText::getLineSizeUTF8( Uint32 _line ) const
{
	if( _line >= nbLines() )
		return 0;

	WgTextLine * pLine = getLine(_line);
	return WgTextTool::getTextSizeUTF8(pLine->pText, pLine->nChars);
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

//____ adjustBlock() __________________________________________________________

bool WgText::adjustBlock( WgTextBlock * pBlock ) const
{
	Uint32 lines = nbLines();

	// Some sanity check...

	if( pBlock->begLn > pBlock->endLn )
		return false;

	// Adjust beginning of block

    if( pBlock->begLn >= lines )
		return false;

	Uint32 begLnLen = getLine(pBlock->begLn)->nChars;

	if( pBlock->begCol > begLnLen )
	{
		if( pBlock->begLn == lines-1)
			return false;					// This was first and last line...

		pBlock->begLn = begLnLen;			// We want to keep CR-symbol...
	}

	// Adjust ending of block

	if( pBlock->endLn >= lines )
	{
		pBlock->endLn = lines-1;
		pBlock->endCol = getLine(pBlock->endLn)->nChars;
		return true;
	}

	Uint32 endLnLen = getLine(pBlock->endLn)->nChars;

	if( pBlock->endCol > endLnLen )
		pBlock->endCol = endLnLen;

	// A final sanity check...

	if( pBlock->begLn == pBlock->endLn && pBlock->begCol >= pBlock->endCol )
		return false;

	return true;
}

//____ setValue() _____________________________________________________________

void WgText::setValue( double value, const WgValueFormat& form )
{
	WgChar	str[parseBufLen];
	WgChar * pStr = parseValue( value, form, str );
	setText( pStr );
}

//____ setScaledValue() _______________________________________________________

void WgText::setScaledValue( Sint32 value, Uint32 scale, const WgValueFormat& form )
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

WgChar * WgText::parseScaledValue( Sint32 value, Uint32 scale, const WgValueFormat& f, WgChar tempstring[parseBufLen] )
{

	int	absVal	= abs(value);

	int intPart = absVal / scale;
	int decPart = absVal % scale;

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
		temp2[n++] = (intPart % 10) + 0x30;
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


//____ chars() ________________________________________________________________
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
	regenSoftLines();

	if( m_pCursor )
		m_pCursor->gotoHardPos(m_pCursor->line(), m_pCursor->column());

	if( m_pHolder )
		m_pHolder->TextModified( this );
}

//____ addChar() ______________________________________________________________

bool WgText::addChar( Uint16 character )
{
	WgChar * p = beginChange( nbLines()-1, getLine(nbLines()-1)->nChars, 1, 0, 0, 0 );
	p->SetGlyph(character);
	endChange( nbLines()-1, 1 );
	return true;
}


//____ addText() ______________________________________________________________

Uint32 WgText::addText( const char * pText, Uint32 nChar )
{
	return insertText( nbLines()-1, getLine(nbLines()-1)->nChars, pText, nChar );
}

Uint32 WgText::addText( const Uint16 * pText, Uint32 nChar )
{
	return insertText( nbLines()-1, getLine(nbLines()-1)->nChars, pText, nChar );
}

Uint32 WgText::addText( const WgChar * pText, Uint32 nChar )
{
	return insertText( nbLines()-1, getLine(nbLines()-1)->nChars, pText, nChar );
}

Uint32 WgText::addText( const WgCharBuffer * pText, Uint32 nChar )
{
	return addText( pText->GetChars(), nChar );
}

Uint32 WgText::addText( const WgText * pText, Uint32 fromLine, Uint32 nLines )
{
	return insertText( nbLines()-1, getLine(nbLines()-1)->nChars, pText, fromLine, nLines );
}

//____ addFormattedText() _____________________________________________________

Uint32 WgText::addFormattedText( const char * pText, Uint32 nChar,  const WgResDB * pResDB )
{
	return insertFormattedText( nbLines()-1, getLine(nbLines()-1)->nChars, pText, nChar, pResDB );
}


//____ replaceText() __________________________________________________________

Uint32 WgText::replaceText( Uint32 line, Uint32 col, Uint32 nChar, const char * pNewText, Uint32 nNewChar )
{
	removeText( line, col, nChar );
	return insertText( line, col, pNewText, nNewChar );
}

Uint32 WgText::replaceText( Uint32 line, Uint32 col, Uint32 nChar, const Uint16 * pNewText, Uint32 nNewChar )
{
	removeText( line, col, nChar );
	return insertText( line, col, pNewText, nNewChar );
}


Uint32 WgText::replaceText( Uint32 line, Uint32 col, Uint32 nChar, const WgText * pNewText )
{
	removeText( line, col, nChar );
	return insertText( line, col, pNewText );
}

//____ addLine() ______________________________________________________________

bool WgText::addLine( const char * pText, Uint32 nChar )
{
	return insertLine( nbLines(), pText, nChar );
}

bool WgText::addLine( const Uint16 * pText, Uint32 nChar )
{
	return insertLine( nbLines(), pText, nChar );
}

bool WgText::addLine( const WgChar * pText, Uint32 nChar )
{
	return insertLine( nbLines(), pText, nChar );
}

//____ replaceLine() __________________________________________________________

bool WgText::replaceLine( Uint32 line, const char * pText, Uint32 nChar )
{
	removeLine( line );
	return insertLine( line, pText, nChar );
}

bool WgText::replaceLine( Uint32 line, const Uint16 * pText, Uint32 nChar )
{
	removeLine( line );
	return insertLine( line, pText, nChar );
}

bool WgText::replaceLine( Uint32 _line, const WgChar * pText, Uint32 nChar )
{
	if( _line >= nbLines() || pText == 0 )
		return false;

	removeLine( _line );
	return insertLine(_line, pText, nChar );
}


//____ replaceChar() __________________________________________________________

bool WgText::replaceChar( Uint32 _line, Uint32 col, Uint16 character )
{
	WgTextLine * pLine = getLine(_line);

	if( col >= pLine->nChars )
		return false;

	pLine->pText[col].SetGlyph(character);
	return true;
}


//____ splitLine() ____________________________________________________________

bool WgText::splitLine( Uint32 _line, Uint32 col )
{
	if( _line >= nbLines() )
		return false;

	Uint32 nChars = getLine(_line)->nChars;
	if( col > nChars )
		col = nChars;

	WgChar * wp = beginChange( _line, col, 1, 0, 1, 0 );
	wp->SetGlyph('\n');
	endChange( _line, 2 );
	return true;
}


//____ removeLines() __________________________________________________________

Uint32 WgText::removeLines( Uint32 start, Uint32 nLines )
{
	if( nLines == 0xFFFFFFFF || start + nLines > nbLines() )
		nLines = nbLines() - start;

	if( ((int) nLines) <= 0 )
		return 0;

	int chardiff = 0;
	for( Uint16 i = 0 ; i < nLines ; i++ )
		chardiff += getLine(start+i)->nChars +1;

	beginChange( start, 0, 0, chardiff, 0, nLines );
	endChange( start, 0 );

	return nLines;
}


//____ insertLine() ___________________________________________________________

bool WgText::insertLine( Uint32 line, const char * pText, Uint32 nChar )
{
	if( pText == 0 || nChar == 0 || line > nbLines() )
		return false;

	Uint32 n = WgTextTool::countLineChars( pText, nChar );

	WgChar * wp = beginChange( line, 0, n+1, 0, 1, 0 );
	if( !wp )
		return false;

	WgTextTool::readLine( pText, wp, n+1 );
	wp[n].SetGlyph('\n');							// End line with \n, not zero.

	endChange( line, 1 );

	return true;
}

bool WgText::insertLine( Uint32 line, const Uint16 * pText, Uint32 nChar )
{
	if( pText == 0 || nChar == 0 || line > nbLines() )
		return false;

	Uint32 n = WgTextTool::countLineChars( pText, nChar );



	WgChar * wp = beginChange( line, 0, n+1, 0, 1, 0 );
	if( !wp )
		return false;

	for( Uint32 i = 0 ; i < n ; i++ )
		wp[i].SetGlyph(pText[i]);
	wp[n].SetGlyph('\n');							// End line with \n, not zero.

	endChange( line, 1 );
	return true;
}

bool WgText::insertLine( Uint32 line, const WgChar * pText, Uint32 nChar )
{
	if( pText == 0 || nChar == 0 || line > nbLines() )
		return false;

	Uint32 n = WgTextTool::countLineChars( pText, nChar );

	WgChar * wp = beginChange( line, 0, n+1, 0, 1, 0 );
	if( !wp )
		return false;

	for( Uint32 i = 0 ; i < n ; i++ )
		wp[i] =  pText[i];
	wp[n].SetGlyph('\n');							// End line with \n, not zero.

	endChange( line, 1 );
	return true;
}

//____ insertChar() ___________________________________________________________

bool WgText::insertChar( Uint32 _line, Uint32 _col, Uint16 character )
{
	// Sanity check

	if( _line >= nbLines() )
		return false;

	WgTextLine * pLine = getLine(_line);
	if( _col > pLine->nChars )
		_col = pLine->nChars;

	// Extra line if character is EOL

	Uint32 addLines = 0;
	if( character == '\n' )
		addLines = 1;

	// Insert the character

	WgChar * wp = beginChange( _line, _col, 1, 0, addLines, 0 );
	wp->SetGlyph(character);
	endChange( _line, 1+addLines );
	return true;
}

//____ removeChar() ___________________________________________________________

bool WgText::removeChar( Uint32 _line, Uint32 _col )
{
	// Some sanity checking

	if( _line >= nbLines() )
		return false;

	WgTextLine * pLine = getLine(_line);
	if( _col >= pLine->nChars )
		return joinLines( _line );

	// Do removal

	beginChange( _line, _col, 0, 1, 0, 0 );
	endChange( _line, 1 );

	return true;
}


//____ removeText() ___________________________________________________________

Uint32 WgText::removeText( Uint32 _line, Uint32 _col, Uint32 nChar )
{
	// Some sanity checking

	if( _line >= nbLines() )
		return 0;

	if( _col > getLine(_line)->nChars )
		_col = getLine(_line)->nChars;


	// Count number of lines we affect and check so nChar isn't too high
	// at the same time...

	getText();					// Force the text to stick together...

	const WgChar * p = getLineText(_line) + _col;

	int	nLines = 0;
	for( Uint32 i = 0 ; i < nChar ; i++ )
	{
		if( p->IsEndOfText() )
		{
			nChar = i;
			break;						// nChar was too big, corrected now...
		}
		else if( p->IsEndOfLine() )
			nLines++;
	}

	beginChange( _line, _col, 0, nChar, 0, nLines );
	endChange( _line, 1 );

	return nChar;
}

void WgText::removeSelectedText()
{
	int line = m_selStartLine;
	int column = m_selStartCol;
	removeText( m_selStartLine, m_selStartCol, m_selEndLine, m_selEndCol);
	if(m_pCursor)
	{
		m_pCursor->setSelectionMode(false);
		m_pCursor->gotoHardPos(line, column);
	}
}

void WgText::removeText( Uint32 startLine, Uint32 startCol, Uint32 endLine, Uint32 endCol)
{
	if( startLine >= nbLines() || startLine > endLine || startLine == endLine && startCol >= endCol )
		return;

	if(startCol > getLine(startLine)->nChars )
		startCol = getLine(startLine)->nChars;

	if(endCol > getLine(endLine)->nChars )
		endCol = getLine(endLine)->nChars;

	Uint32 nChars;
	if(startLine == endLine)
	{
		nChars = endCol - startCol;
	}
	else
	{
		nChars = 1 + m_pHardLines[startLine].nChars - startCol;
		for(Uint32 i = startLine + 1; i < endLine; i++)
			nChars += m_pHardLines[i].nChars + 1;
		nChars += endCol;
	}

	int	nLines = endLine - startLine;

	beginChange( startLine, startCol, 0, nChars, 0, nLines );
	endChange( startLine, 1 );
}

//____ insertText() ___________________________________________________________

Uint32 WgText::insertText( Uint32 _line, Uint32 _col, const char * pText, Uint32 nChar )
{
	if( _line >= nbLines() )
	{
		_line	= nbLines()-1;
		_col	= 0xFFFFFFFF;
	}

	if( _col > getLine(_line)->nChars )
		_col = getLine(_line)->nChars;

	Uint32	addChars, addLines;

	WgTextTool::countCharsLines(pText, addChars, addLines, nChar);
	addLines--;

	WgChar * dp =beginChange( _line, _col, addChars, 0, addLines, 0 );

	for( Uint32 i = 0 ; i < addChars ; i++ )
		dp[i].SetGlyph(WgTextTool::readChar( (const char *&) pText ));

	endChange(_line, addLines+1 );
	return addChars;
}


Uint32 WgText::insertText( Uint32 _line, Uint32 _col, const Uint16 * pText, Uint32 nChar )
{
	if( _line >= nbLines() )
	{
		_line	= nbLines()-1;
		_col	= 0xFFFFFFFF;
	}

	if( _col > getLine(_line)->nChars )
		_col = getLine(_line)->nChars;

	Uint32	addChars, addLines;

	WgTextTool::countCharsLines(pText, addChars, addLines, nChar);
	addLines--;

	WgChar * dp =beginChange( _line, _col, addChars, 0, addLines, 0 );

	for( Uint32 i = 0 ; i < addChars ; i++ )
		dp[i].SetGlyph(pText[i]);

	endChange(_line, addLines+1 );
	return addChars;
}

Uint32 WgText::insertText( Uint32 _line, Uint32 _col, const WgChar * pText, Uint32 nChar )
{
	if( _line >= nbLines() )
	{
		_line	= nbLines()-1;
		_col	= 0xFFFFFFFF;
	}

	if( _col > getLine(_line)->nChars )
		_col = getLine(_line)->nChars;

	Uint32	addChars, addLines;

	WgTextTool::countCharsLines(pText, addChars, addLines, nChar);
	addLines--;

	WgChar * dp =beginChange( _line, _col, addChars, 0, addLines, 0 );

	for( Uint32 i = 0 ; i < addChars ; i++ )
		dp[i] = pText[i];

	endChange(_line, addLines+1 );
	return addChars;
}


Uint32 WgText::insertText( Uint32 _line, Uint32 _col, const WgText * pText, Uint32 fromLine, Uint32 nLines )
{
	if( _line >= nbLines() )
	{
		_line	= nbLines()-1;
		_col	= 0xFFFFFFFF;
	}

	if( _col > getLine(_line)->nChars )
		_col = getLine(_line)->nChars;

	if( fromLine > pText->nbLines() )
		return 0;

	if( pText->nbLines() - fromLine < nLines )
		nLines = pText->nbLines() - fromLine;

	WgChar * sp = pText->getLineText(fromLine);
	int	nChars = WgTextTool::strlen( sp );

	WgChar * dp =beginChange( _line, _col, nChars, 0, nLines-1, 0 );

	if( dp == 0 )
		return 0;

	for( int i = 0 ; i < nChars ; i++ )
		* dp++ = * sp++;

	endChange( _line, nLines );
	return nChars;
}

Uint32 WgText::insertText( Uint32 line, Uint32 col, const WgCharBuffer * pText, Uint32 nChar )
{
	return insertText( line, col, pText->GetChars(), nChar );
}

//____ insertFormattedText() __________________________________________________

Uint32 WgText::insertFormattedText( Uint32 _line, Uint32 _col, const char * pText, Uint32 nChar,  const WgResDB * pResDB )
{
	if( _line >= nbLines() )
	{
		_line	= nbLines()-1;
		_col	= 0xFFFFFFFF;
	}

	if( _col > getLine(_line)->nChars )
		_col = getLine(_line)->nChars;

	Uint32 addChars = WgTextTool::countNonFormattingChars( pText );
	Uint32 addLines = WgTextTool::countLines( pText )-1;

	if( nChar > addChars )
		nChar = addChars;

	WgChar * dp =beginChange( _line, _col, addChars, 0, addLines, 0 );

	WgTextTool::readFormattedString( pText, dp, addChars, pResDB );

	endChange(_line, addLines+1 );
	return addChars;
}


//____ joinLines() ____________________________________________________________

bool WgText::joinLines( Uint32 firstLine )
{
	if( firstLine + 1 >= nbLines() )
		return false;

	Uint32 col = getLine(firstLine)->nChars;

	beginChange( firstLine, col, 0, 1, 0, 1 );
	endChange( firstLine, 1 );

	return true;
}

//____ getText() ______________________________________________________________

const WgChar * WgText::getText() const
{
	return m_pHardLines->pText;
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

WgTextLine * WgText::getLine( Uint32 line ) const
{
	if( line >= m_nHardLines )
		return 0;

	return m_pHardLines + line;
}

//____ getSoftLine() __________________________________________________________

WgTextLine * WgText::getSoftLine( Uint32 line ) const
{
	if( line >= m_nSoftLines )
		return 0;

	return m_pSoftLines + line;
}

//____ getLineText() __________________________________________________________

WgChar * WgText::getLineText( Uint32 line ) const
{
	if( line >= m_nHardLines )
		return 0;

	return m_pHardLines[line].pText;
}

//____ getSoftLineText() ______________________________________________________

WgChar * WgText::getSoftLineText( Uint32 line ) const
{
	if( line >= m_nSoftLines )
		return 0;

	return m_pSoftLines[line].pText;
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

void WgText::posHard2Soft( Uint32 &line, Uint32 &col ) const
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

WgChar * WgText::beginChange( Uint32 line, Uint32 col, Uint32 addChar, Uint32 delChar, Uint32 addLines, Uint32 delLines, WgChar ch )
{
	if(delLines > 0 || delChar > 0)
	{
		if( m_pCursor )
			m_pCursor->clearSelection();
		else
			clearSelection();
	}

	if(m_pCursor)
	{
		Uint32 cursorLine= m_pCursor->line();
		Uint32 cursorCol = m_pCursor->column();
		Uint32 startPos = LineColToOffset(line, col);
		Uint32 endPos0 = startPos + delChar;
		Uint32 endPos1 = startPos + addChar;
		Uint32 cursorPos = LineColToOffset(cursorLine, cursorCol);
		if(cursorPos >= startPos)
		{
			if(cursorPos >= endPos0)
				cursorPos += endPos1 - endPos0;
			else if(cursorPos > endPos1)
				cursorPos = endPos1;				
		}
		m_newCursorPos = cursorPos;
	}

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


Uint32 WgText::LineColToOffset(int line, int col)
{
	return m_pHardLines[line].pText - m_pHardLines[0].pText + col;		
}

//____ endChange() ____________________________________________________________

void WgText::endChange( Uint32 startline, Uint32 nLines )
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

	if(m_pCursor)
	{
		Uint32 iLine = 0;
		for(; m_newCursorPos > 0 && iLine < nbLines(); iLine++)
			m_newCursorPos -= m_pHardLines[iLine].nChars;

		iLine = iLine - 1;

		Uint32 iCol = m_pHardLines[iLine].nChars + m_newCursorPos;

		m_pCursor->gotoHardPos(iLine, iCol);





	}
}

void WgText::regenSoftLines()
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

	WgPen			pen;
	pen.SetTextNode( m_pManagerNode );
	Uint16			hProp = 0xFFFF;


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
					// We don't take kerning into account here, not so important.

					const WgGlyph * pHyphen = pen.GetGlyphSet()->GetGlyph( '-', pen.GetSize() );
					if( pHyphen && Uint32(pen.GetPosX() + pHyphen->advance) < m_lineWidth )
						pbp = p;
				}
				else
					pbp = p;					// We can put a softbreak here if necessary...
			}

			// Increase line length

			Uint32 oldLen = pen.GetPosX();
			
			if( p->GetPropHandle() != hProp )
			{
				pen.SetTextProp( m_pProp.GetHandle(), p->GetPropHandle(), m_mode );
				hProp = p->GetPropHandle();
			}

			pen.SetChar( p->GetGlyph() );
			pen.ApplyKerning();


			// Check if we need to put a softbreak.

			if( Uint32(pen.GetPosX() + pen.GetGlyph()->bearingX + pen.GetGlyph()->rect.w) > m_lineWidth )			// No advance on last character of line, just bearingX + width
			{
				if( pbp != 0 )
				{
					pbp->setSoftBreak();
					p = pbp;
					break;
				}
				else if( oldLen != 0 )		// Can't set a softbreak on previous char if there is no previous char...
				{
					p--;
					p->setSoftBreak();
					break;
				}
			}
			else
				pen.AdvancePos();

			//

			p++;
		}

		if( !p->IsEndOfText() )
		{
			p++;			// Skip the break-point, it doesn't belong to any line.
			nSoftLines++;	// This was not the last line...
			pen.SetPosX(0);			// Reset position
			pen.FlushChar();		// Flush current character so it won't affect kerning for first character of next line.
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
