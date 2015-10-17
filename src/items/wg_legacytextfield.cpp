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
#include <wg_legacytextfield.h>
#include <wg_font.h>
#include <wg_valueformat.h>
#include <wg_legacytextfield.h>			// UGLY DEPENDENCY FOR QUICKHACK IN INSERT TEXT METHODS!!!
#include <wg_texttool.h>
#include <wg_charseq.h>
#include <wg_charbuffer.h>
#include <wg_pen.h>
#include <wg_base.h>
#include <wg_textlink.h>
#include <wg_string.h>
#include <wg_msg.h>
#include <wg_msgrouter.h>
#include <wg_widget.h>
#include <wg_util.h>

namespace wg 
{
	
	//____ Constructor ____________________________________________________________
	
	
	LegacyTextField::LegacyTextField( LegacyTextHolder * pHolder ) : Field(pHolder)
	{
		m_pCursor		= 0;
		m_pCursorStyle	= 0;
	
		m_markedLinkState = WG_STATE_NORMAL;
	
		m_alignment		= WG_NORTHWEST;
		m_tintMode		= WG_TINTMODE_MULTIPLY;
		m_lineSpaceAdj	= 1.f;
	
		m_state			= WG_STATE_NORMAL;
	
		m_pHardLines	= new LegacyTextLine[1];
		m_nHardLines	= 1;
	
		m_pSoftLines	= m_pHardLines;
		m_nSoftLines	= 1;
	
		m_pHardLines->nChars = 0;
		m_pHardLines->ofs = 0;
	
		m_maxChars		= INT_MAX;
		m_lineWidth		= INT_MAX;
		m_bWrap			= true;
		m_bAutoEllipsis = true;
	
		m_editMode		= WG_TEXT_STATIC;
	
		m_selStartLine = 0;
		m_selEndLine = 0;
		m_selStartCol = 0;
		m_selEndCol = 0;
	
		_refreshAllLines();
	}
	
	
	
	//____ Destructor _____________________________________________________________
	
	LegacyTextField::~LegacyTextField()
	{
		if( m_pSoftLines != m_pHardLines )
			delete [] m_pSoftLines;
	
		delete [] m_pHardLines;
		delete m_pCursor;
	}
	
	
	//____ setCursorSkin() _______________________________________________________
	
	void LegacyTextField::setCursorSkin( const Caret_p& pCursor )
	{
		if( pCursor != m_pCursorStyle )
		{
			m_pCursorStyle = pCursor;
			_regenSoftLines();
			_refreshAllLines();
	
			_onResize();
		}
	}
	
	//____ _regenHardLines() _______________________________________________________
	
	void LegacyTextField::_regenHardLines()
	{
		int nLines = TextTool::countLines( m_buffer.chars() );
	
		if( m_nHardLines != nLines )
		{
			if( m_pHardLines != m_pSoftLines )				// Only delete hardlines array if it isn't shared with softlines array.
				delete [] m_pHardLines;
	
			m_pHardLines = new LegacyTextLine[nLines];
			m_nHardLines = nLines;
		}
	
		// Fill in hardlines array.
	
		int		line = 0;
		int		ofs = 0;
		const Char * p = m_buffer.chars();
	
		m_pHardLines[line].ofs = ofs;
	
		while( !p[ofs].isEndOfText() )
		{
			if( p[ofs].isEndOfLine() )
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
	
	void LegacyTextField::clear()
	{
		m_buffer.reset();
	
		if( m_pSoftLines != m_pHardLines )
			delete [] m_pSoftLines;
	
		if( m_nHardLines != 1 )
		{
			delete [] m_pHardLines;
	
			m_pHardLines = new LegacyTextLine[1];
			m_nHardLines = 1;
		}
	
		m_pHardLines->nChars = 0;
		m_pHardLines->ofs = 0;
	
		m_pSoftLines = m_pHardLines;
		m_nSoftLines = 1;
	
		_refreshAllLines();
		clearSelection();
	
		if( m_pCursor )
			m_pCursor->gotoHardPos(m_pCursor->line(), m_pCursor->column());
	
		_onResize();
	}
	
	//____ set() __________________________________________________________________
	
	void LegacyTextField::set( const CharSeq& seq )
	{
		m_buffer.clear();
	
		if( seq.length() <= m_maxChars )
			m_buffer.pushBack( seq );
		else
			m_buffer.pushBack( CharSeq(seq, 0, m_maxChars) );
		_regenHardLines();
		_regenSoftLines();
		_refreshAllLines();
		clearSelection();
	
		_onResize();
		assert( m_buffer.findFirst( WG_ESCAPE_CODE ) == -1 );		// Forgotten to wrap text in CharSeqEscaped?
	}
	
	void LegacyTextField::set( const CharBuffer * buffer )
	{
		m_buffer = * buffer;
		if( (int) m_buffer.length() > m_maxChars )
			m_buffer.remove( m_maxChars, INT_MAX );
		_regenHardLines();
		_regenSoftLines();
		_refreshAllLines();
		clearSelection();
	
		_onResize();
		assert( m_buffer.findFirst( WG_ESCAPE_CODE ) == -1 );		// Forgotten to wrap text in CharSeqEscaped?
	}
	
	
	void LegacyTextField::set( const String& str )
	{
		if( (int) str.length() <= m_maxChars )
			m_buffer = str;
		else
			m_buffer = CharSeq(str,0,m_maxChars);
	
		_regenHardLines();
		_regenSoftLines();
		_refreshAllLines();
		clearSelection();
		_onResize();
	}
	
	
	void LegacyTextField::setText( const LegacyTextField * pText )
	{
		// TODO: Optimize, we can simply copy the hardlines array. Softlines needs to be generated though.
	
		m_buffer = * pText->getBuffer();
		if( (int) m_buffer.length() > m_maxChars )
			m_buffer.remove( m_maxChars, INT_MAX );
	
		_regenHardLines();
		_regenSoftLines();
		_refreshAllLines();
		clearSelection();
		_onResize();
	}
	
	//_________________________________________________________
	
	void LegacyTextField::clone( const LegacyTextField * pText )
	{
		//TODO: Implement!!!
	}
	
	//_________________________________________________________
	void LegacyTextField::selectText( int startLine, int startCol, int endLine, int endCol )
	{
		if(endLine < startLine || (endLine == startLine && endCol < startCol) )
		{
			std::swap(endLine, startLine);
			std::swap(endCol, startCol);
		}
	
		if(startLine >= lines())
			startLine = lines() - 1;
	
		if(endLine >= lines())
			endLine = lines() - 1;
	
		if(startCol > m_pHardLines[startLine].nChars)
			startCol = m_pHardLines[startLine].nChars;
	
		if(endCol > m_pHardLines[endLine].nChars)
			endCol = m_pHardLines[endLine].nChars;
	
		m_selStartLine = startLine;
		m_selEndLine = endLine;
		m_selStartCol = startCol;
		m_selEndCol = endCol;
	
		const LegacyTextLine * pCurLine = &m_pHardLines[startLine];
		if(endCol > pCurLine->nChars)
			endCol = pCurLine->nChars;
	}
	
	//_________________________________________________________
	bool LegacyTextField::getSelection( int& startLine, int& startCol, int& endLine, int& endCol ) const
	{
		startLine = m_selStartLine;
		endLine = m_selEndLine;
		startCol = m_selStartCol;
		endCol = m_selEndCol;
	
		return startLine != endLine || startCol != endCol;
	}
	
	//_________________________________________________________
	Range LegacyTextField::getSelection() const
	{
		int beg = lineColToOffset(m_selStartLine,m_selStartCol);
		int end = lineColToOffset(m_selEndLine,m_selEndCol);
	
		return Range(beg,end-beg);
	}
	
	//_________________________________________________________
	int LegacyTextField::selectionStart() const
	{
		return lineColToOffset(m_selStartLine,m_selStartCol);
	}
	
	//_________________________________________________________
	int LegacyTextField::selectionLength() const
	{
		return lineColToOffset(m_selEndLine,m_selEndCol);
	}
	
	//_________________________________________________________
	void LegacyTextField::clearSelection( )
	{
		m_selStartLine = 0;
		m_selEndLine = 0;
		m_selStartCol = 0;
		m_selEndCol = 0;
	
		_onDirty();
	}
	
	//____ getSoftLineWidth() _____________________________________________________
	
	int LegacyTextField::getSoftLineWidth( int line ) const
	{
		if( line < 0 || line >= nbSoftLines() )
			return 0;
	
		return m_pSoftLines[line].width;
	}
	
	//____ getSoftLineWidthPart() _________________________________________________
	
	int LegacyTextField::getSoftLineWidthPart( int _line, int startCol, int nCol ) const
	{
		if( _line < 0 || _line >= nbSoftLines() || !m_pBaseProp->font() )
			return 0;
	
		const LegacyTextLine * pLine = getSoftLine(_line);
	
		if( pLine->nChars - startCol < nCol )
			nCol = pLine->nChars-startCol;
	
	
		const Char * pString = m_buffer.chars() + pLine->ofs;
	
		Pen pen;
		uint16_t hProp = 0xFFFF;
	
		TextAttr	attr;
	
		for( int i = 0 ; i < nCol ; i++ )
		{
			if( pString->isEndOfLine() )
				break;
	
			if( pString->propHandle() != hProp )
			{
				getCharAttr( attr, pLine->ofs + i );
				pen.setAttributes( attr );
			}
			pen.setChar( pString->getGlyph() );
			pen.applyKerning();
			pen.advancePos();
			pString++;
		}
	
		return pen.getPosX();
	
	}
	
	
	//____ getLineWidth() ____________________________________________________________
	
	int LegacyTextField::getLineWidth( int line ) const
	{
		if( line >= lines() )
			return 0;
	
		return m_pHardLines[line].width;
	}
	
	//____ getLineOfsY() __________________________________________________________
	
	int LegacyTextField::getLineOfsY( int line ) const
	{
		if( line >= m_nSoftLines )
			return 0;
	
		int ofs = 0;
		for( int i = 0 ; i < line ; i++ )
			ofs += m_pSoftLines[i].lineSpacing;
	
		return ofs;
	}
	
	//____ unwrappedWidth() _______________________________________________________
	
	int LegacyTextField::unwrappedWidth() const
	{
		int		width = 0;
	
		for( int i = 0 ; i < m_nHardLines ; i++ )
		{
			if( m_pHardLines[i].width > width )
				width = m_pHardLines[i].width;
		}
	
		return width;
	}
	
	//____ unwrappedSize() _______________________________________________________
	
	Size LegacyTextField::unwrappedSize() const
	{
		Size	sz;
	
		for( int i = 0 ; i < m_nHardLines ; i++ )
		{
			if( m_pHardLines[i].width > sz.w )
				sz.w = m_pHardLines[i].width;
	
			sz.h += m_pHardLines[i].lineSpacing;
		}
	
		sz.h += m_pHardLines[m_nHardLines-1].height - m_pHardLines[m_nHardLines-1].lineSpacing;
		return sz;
	}
	
	
	//____ width() ________________________________________________________________
	
	int LegacyTextField::width() const
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
	
	int LegacyTextField::height() const
	{
		int height = 0;
		for( int i = 0 ; i < m_nSoftLines-1 ; i++ )
			height += m_pSoftLines[i].lineSpacing;
	
		height += m_pSoftLines[m_nSoftLines-1].height;
		return height;
	}
	
	//____ heightForWidth() _______________________________________________________
	
	int LegacyTextField::heightForWidth( int width ) const
	{
		if( m_bWrap && width != m_lineWidth )
		{
			const int		maxLines = 16;
	
			LegacyTextLine		lineArray[maxLines];
			LegacyTextLine *	pLines = lineArray;
	
			int nSoftLines = _countWriteSoftLines( width, m_buffer.chars(), pLines, maxLines );
	
			// If the TextLines didn't fit in our little array we need to allocate a bigger one
			// and regenerate
	
			if( nSoftLines > maxLines )
			{
				pLines = new LegacyTextLine[nSoftLines];
				_countWriteSoftLines( width, m_buffer.chars(), pLines, nSoftLines );
			}
	
			// Generate line-info, so height-member is filled in.
	
			for( int i = 0 ; i < nSoftLines ; i++ )
				_refreshLineInfo(&pLines[i]);
	
			// Calculate height
	
			int height = 0;
			for( int i = 0 ; i < nSoftLines-1 ; i++ )
				height += pLines[i].lineSpacing;
	
			height += pLines[nSoftLines-1].height;
	
			// Possibly delete allocated area
	
			if( pLines != lineArray )
				delete [] pLines;
	
			return height;
		}
		else
			return height();
	}
	
	//____ softLineHeight() _______________________________________________________
	
	int LegacyTextField::softLineHeight( int line )
	{
		if( line >= nbSoftLines() )
			return 0;
	
		return m_pSoftLines[line].height;
	}
	
	int LegacyTextField::softLineSpacing( int line )
	{
		if( line >= nbSoftLines() )
			return 0;
	
		return m_pSoftLines[line].lineSpacing;
	}
	
	
	void LegacyTextField::setLinkProperties( const Textprop_p& pProp )
	{
		if( m_pLinkProp != pProp )
		{
			m_pLinkProp = pProp;
			_regenSoftLines();		//TODO: Optimize: Only do if we have links.
			_refreshAllLines();
			_onResize();
		}
	}
	
	void LegacyTextField::clearLinkProperties()
	{
		if( m_pLinkProp )
		{
			m_pLinkProp = 0;
			_regenSoftLines();		//TODO: Optimize: Only do if we have links.
			_refreshAllLines();
			_onResize();
		}
	}
	
	void LegacyTextField::setSelectionProperties( const Textprop_p& pProp )
	{
		if( m_pSelectionProp != pProp )
		{
			m_pSelectionProp = pProp;
			_regenSoftLines();		//TODO: Optimize: Only do if we have selected text.
			_refreshAllLines();
			_onResize();
		}
	}
	
	void LegacyTextField::clearSelectionProperties()
	{
		if( m_pSelectionProp )
		{
			m_pSelectionProp = 0;
			_regenSoftLines();		//TODO: Optimize: Only do if we have selected text.
			_refreshAllLines();
			_onResize();
		}
	}
	
	
	void LegacyTextField::setSelectionBgColor(Color color )
	{
		Textprop	prop = * m_pSelectionProp;
		prop.setBgColor(color);
		m_pSelectionProp = prop.reg();
		_onResize();
	}
	
	
	void LegacyTextField::setProperties( const Textprop_p& pProp )
	{
		m_pBaseProp = pProp;
		_regenSoftLines();
		_refreshAllLines();
		_onResize();
	}
	
	
	void LegacyTextField::clearProperties()
	{
		if( m_pBaseProp )
		{
			m_pBaseProp = 0;
			_regenSoftLines();
			_refreshAllLines();
			_onResize();
		}
	}
	
	//____ getTextUTF8() __________________________________________________________
	/**
		Counts the number of bytes needed to represent the text in UTF-format,
		reserves a memory area just large enough, generates a zero-terminated string
		and returns a pointer to it.
	
		@returns	Pointer to the string in UTF8-format. Caller needs to delete[]
					the string afterwards.
	*/
	
	char *	LegacyTextField::getTextUTF8() const
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
	
	int	LegacyTextField::getTextUTF8( char * pDest, int maxBytes ) const
	{
		return TextTool::getTextUTF8(getText(), pDest, maxBytes);
	}
	
	//____ getTextSizeUTF8() ______________________________________________________
	
	int	LegacyTextField::getTextSizeUTF8() const
	{
		return TextTool::getTextSizeUTF8(getText());
	}
	
	//____ getTextFormattedUTF8() _________________________________________________
	
	char * LegacyTextField::getTextFormattedUTF8() const
	{
		int bytes = getTextSizeFormattedUTF8();
		char * pDest = new char[bytes+1];
		getTextFormattedUTF8(pDest, bytes+1);
		return pDest;
	}
	
	//____ getTextFormattedUTF8() __________________________________________________
	
	int LegacyTextField::getTextFormattedUTF8( char * pDest, int maxBytes ) const
	{
		return TextTool::getTextFormattedUTF8( getText(), pDest, maxBytes );
	}
	
	//____ getTextSizeFormattedUTF8() ______________________________________________
	
	int LegacyTextField::getTextSizeFormattedUTF8() const
	{
		return TextTool::getTextSizeFormattedUTF8(getText(), INT_MAX);
	}
	
	
	char * LegacyTextField::getSelectedTextFormattedUTF8() const
	{
		if(m_selStartLine == m_selEndLine && m_selStartCol == m_selEndCol)
			return 0;
	
		const Char* pTextStart = getLineText(m_selStartLine) + m_selStartCol;
		const Char* pTextEnd = getLineText(m_selEndLine) + m_selEndCol;
		int nChars = pTextEnd - pTextStart;
		int nBytes = TextTool::getTextSizeFormattedUTF8(pTextStart, nChars);
		char * pDest = new char[nBytes + 1];
		TextTool::getTextFormattedUTF8(pTextStart, pDest, nBytes + 1);
		return pDest;
	}
	
	char * LegacyTextField::getSelectedTextUTF8() const
	{
		if(m_selStartLine == m_selEndLine && m_selStartCol == m_selEndCol)
			return 0;
	
		const Char* pTextStart = getLineText(m_selStartLine) + m_selStartCol;
		const Char* pTextEnd = getLineText(m_selEndLine) + m_selEndCol;
		int nChars = pTextEnd - pTextStart;
		int nBytes = TextTool::getTextSizeUTF8(pTextStart, nChars);
		char * pDest = new char[nBytes + 1];
		TextTool::getTextUTF8(pTextStart, pDest, nBytes + 1);
		return pDest;
	}
	
	
	//____ compareTo() ____________________________________________________________
	/*
	int LegacyTextField::compareTo( const LegacyTextField * _pOther, bool bCheckCase ) const
	{
	
		int	diff = 0;
		uint32_t	line = 0;
	
		Char * pMine = getLineText( 0 );
		Char * pOther = _pOther->getLineText( 0 );
	
		if( bCheckCase )
		{
			// default, case sensitive comparison
			while( pMine && pOther )
			{
				int	ofs = -1;
	
				do
				{
					ofs++;
					diff = pMine[ofs].getGlyph() - pOther[ofs].getGlyph();
					if( diff != 0 )
						return diff;
	
				} while( pMine[ofs].getGlyph() != 0 ); // Both lines must be terminated for a termination to occur but not create a diff, so we only have to compare against one...
	
	
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
					diff = towlower( pMine[ofs].getGlyph() ) - towlower( pOther[ofs].getGlyph() );	// this is the only difference from above
					if( diff != 0 )
						return diff;
	
				} while( pMine[ofs].getGlyph() != 0 ); // Both lines must be terminated for a termination to occur but not create a diff, so we only have to compare against one...
	
	
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
	
	void LegacyTextField::setValue( double value, const ValueFormat_p& pFormat )
	{
		Char	str[s_parseBufLen];
		Char * pStr = _parseValue( value, pFormat.rawPtr(), str );
		set( pStr );
	}
	
	//____ setScaledValue() _______________________________________________________
	
	void LegacyTextField::setScaledValue( int64_t value, uint32_t scale, const ValueFormat_p& pFormat )
	{
		Char	str[s_parseBufLen];
		Char * pStr = _parseScaledValue( value, scale, pFormat.rawPtr(), str );
		set( pStr );
	}
	
	
	//____ _parseValue() ___________________________________________________________
	
	Char * LegacyTextField::_parseValue( double value, const ValueFormat * pF, Char tempstring[s_parseBufLen] )
	{
	
		// Write period and decimal part
	
		// length of buffer - decimals - suffix - end color - null terminator
		int fractionOffset = s_parseBufLen - 16 - 4 - 1 - 1;
	
		Char * p = tempstring + fractionOffset;
		if( pF->decimals || pF->bForcePeriod )
		{
			if( 0 == pF->noDecimalThreshold || (int)value < pF->noDecimalThreshold )
			{
				double decPart = value - (int) value ;
	
				if( pF->bForceDecimals || decPart != 0. )
				{
					p->setGlyph(pF->period);
					p++;
	
					for( int i = pF->decimals; i > 0 ; i-- )
					{
						decPart *= 10;
						p->setGlyph((uint16_t)decPart + 0x30);
						p++;
						decPart = decPart - (int) decPart;
					}
				}
			}
		}
	
		// Add suffix
	
		const Char * pSuffix = pF->suffix.chars();
	
		for( int i = 0 ; i < pF->suffix.length() && i < 4 ; i++ )
			* p++ = pSuffix[i];
	
		// Terminate string
	
		p->setGlyph(0);
		p++;
	
		// Write integer part to temp area (backwards)
	
		uint16_t	temp2[32];
	
		uint32_t	intPart = (uint32_t) value;
		int			n = 0;
	
		while( intPart > 0 )
		{
			temp2[n++] = (intPart % 10) + 0x30;
			intPart /= 10;
		}
	
		// Copy integer part acknowledge grouping
	
		p = tempstring + fractionOffset;
		if( pF->grouping == 0 )
		{
			for( int i = 0 ; i < n ; i++ )
			{
				p--;
				p->setGlyph(temp2[i]);
			}
		}
		else
		{
			for( int i = 0 ; i < n ; i++ )
			{
				if( i != 0 && (i % pF->grouping) == 0 )
				{
					p--;
					p->setGlyph(pF->separator);
				}
	
				p--;
				p->setGlyph(temp2[i]);
			}
		}
	
		// Possibly fill out with zeroes, acknowledge grouping
	
		if( n < pF->integers )
		{
			for( int i = n ; i < pF->integers ; i++ )
			{
				if( i != 0 && (i % pF->grouping) == 0 )
				{
					p--;
					p->setGlyph(pF->separator);
				}
	
				p--;
				p->setGlyph(0x30);
			}
	
		}
	
		// Possibly put a prefix at the start
	
		const Char * pPrefix = pF->prefix.chars();
	
		for( int i = pF->prefix.length()-1 ; i >= 0 ; i-- )
			* --p = pPrefix[i];
	
		// Possibly put a plus or minus sign before prefix.
	
		if( value < 0 )
		{
			p--;
			p->setGlyph('-');
		}
		else if( pF->bPlus )
		{
			p--;
			p->setGlyph('+');
		}
	
		// Set character attributes
	
		if( pF->bSetTextprop )
			TextTool::setProperties( pF->pTextProperties, tempstring, s_parseBufLen );
	
		return p;
	}
	
	//____ _parseScaledValue() _____________________________________________________
	
	Char * LegacyTextField::_parseScaledValue( int64_t value, uint32_t scale, const ValueFormat * pF, Char tempstring[s_parseBufLen] )
	{
	
		int64_t absVal = value >= 0 ? value : -value;
	
		int64_t intPart = absVal / scale;
		int64_t decPart = absVal % scale;
	
		// Write period and decimal part
	
		// length of buffer - decimals - suffix - end color - null terminator
		int fractionOffset = s_parseBufLen - 16 - 4 - 1 - 1;
	
		Char * p = tempstring + fractionOffset;
		if( pF->decimals || pF->bForcePeriod )
		{
			if( 0 == pF->noDecimalThreshold || (int)absVal < pF->noDecimalThreshold )
			{
				if( pF->bForceDecimals || decPart != 0 )
				{
					p++->setGlyph(pF->period);
	
					for( int i = pF->decimals; i > 0 ; i-- )
					{
						decPart *= 10;
						p++->setGlyph(((uint16_t)(decPart/scale)) + 0x30);
						decPart = decPart % scale;
					}
				}
			}
		}
	
		const Char * pSuffix = pF->suffix.chars();
	
		for( int i = 0 ; i < pF->suffix.length() && i < 4 ; i++ )
			* p++ = pSuffix[i];
	
		// Terminate string
	
		p++->setGlyph(0);						// Just to be sure, might already have been terminated as part of suffix.
	
		// Write integer part to temp area (backwards)
	
		uint16_t	temp2[32];
	
		int			n = 0;
	
		while( intPart > 0 )
		{
			temp2[n++] = (uint16_t) ((intPart % 10) + 0x30);
			intPart /= 10;
		}
	
		// Copy integer part acknowledge grouping
	
		p = tempstring + fractionOffset;
		if( pF->grouping == 0 )
		{
			for( int i = 0 ; i < n ; i++ )
				(--p)->setGlyph(temp2[i]);
		}
		else
		{
			for( int i = 0 ; i < n ; i++ )
			{
				if( i != 0 && (i % pF->grouping) == 0 )
					(--p)->setGlyph(pF->separator);
	
				(--p)->setGlyph(temp2[i]);
			}
		}
	
		// Possibly fill out with zeroes, acknowledge grouping
	
		if( n < pF->integers )
		{
			for( int i = n ; i < pF->integers ; i++ )
			{
				if( i != 0 && (i % pF->grouping) == 0 )
					(--p)->setGlyph(pF->separator);
	
				(--p)->setGlyph(0x30);
			}
	
		}
	
		// Possibly put a prefix at the start
	
		const Char * pPrefix = pF->prefix.chars();
	
		for( int i = pF->prefix.length()-1 ; i >= 0 ; i-- )
			* --p = pPrefix[i];
	
		// Possibly put a plus or minus sign at the very start.
	
		if( value < 0 || (value == 0 && pF->bZeroIsNegative) )
			(--p)->setGlyph((short) '-');
		else if( pF->bPlus )
			(--p)->setGlyph((short) '+');
	
		// Set character attributes
	
		if( pF->bSetTextprop )
			TextTool::setProperties(pF->pTextProperties, tempstring, s_parseBufLen );
	
		return p;
	}
	
	
	//____ length() ________________________________________________________________
	/**
		Returns number of characters in text, including any linebreak, excluding
		the zero terminating the string.
	
		(note: This behaviour has changed with rewrite of textsystem, did previously
		not include the linebreaks).
	
	*/
	
	int LegacyTextField::length() const
	{
		return m_buffer.length();
	}
	
	//____ refresh() ______________________________________________________________
	
	void LegacyTextField::refresh()
	{
		_regenHardLines();
		_regenSoftLines();
		_refreshAllLines();
	
		if( m_pCursor )
			m_pCursor->gotoHardPos(m_pCursor->line(), m_pCursor->column());
	
		_onResize();
	}
	
	//____ addChar() ______________________________________________________________
	
	int LegacyTextField::addChar( const Char& character )
	{
		if( m_buffer.length() == m_maxChars )
			return 0;
	
		int nAdded = m_buffer.pushBack( character );
		_regenHardLines();
		_regenSoftLines();
		_refreshAllLines();
	
		return nAdded;
	}
	
	
	//____ append() ______________________________________________________________
	
	int LegacyTextField::append( const CharSeq& seq )
	{
		int nAdded;
		if( (int) seq.length() > m_maxChars - (int) m_buffer.length() )
			nAdded = m_buffer.pushBack( CharSeq( seq, 0, m_maxChars - m_buffer.length() ) );
		else
			nAdded = m_buffer.pushBack( seq );
	
		_regenHardLines();
		_regenSoftLines();
		_refreshAllLines();
	
		_onResize();
	
		return nAdded;
	 }
	
	//____ insert() ___________________________________________________________
	
	int LegacyTextField::insert( int ofs, const CharSeq& seq )
	{
		int nInserted;
		if( (int) seq.length() > m_maxChars - (int) m_buffer.length() )
			nInserted = m_buffer.insert( ofs, CharSeq( seq, 0, m_maxChars - m_buffer.length() ) );
		else
			nInserted = m_buffer.insert( ofs, seq );
	
		_regenHardLines();
		_regenSoftLines();
		_refreshAllLines();
	
		_onResize();
		return nInserted;
	}
	
	//____ replace() __________________________________________________________
	
	int LegacyTextField::replace( int ofs, int nDelete, const CharSeq& seq )
	{
		int nInserted;
		if( (int) seq.length() > m_maxChars - (int) m_buffer.length() - nDelete )
			nInserted = m_buffer.replace( ofs, nDelete, CharSeq( seq, 0, m_maxChars - m_buffer.length() - nDelete ) );
		else
			nInserted = m_buffer.replace( ofs, nDelete, seq );
	
		_regenHardLines();
		_regenSoftLines();
		_refreshAllLines();
	
		_onResize();
		return nInserted;
	}
	
	//____ delete() ___________________________________________________________
	
	int LegacyTextField::remove( int ofs, int nChars )
	{
		int nDeleted = m_buffer.remove( ofs, nChars );
		_regenHardLines();
		_regenSoftLines();
		_refreshAllLines();
		_onResize();
		return nDeleted;
	}
	
	
	//____ replaceChar() __________________________________________________________
	
	int LegacyTextField::replaceChar( int ofs, const Char& character )
	{
		int nReplaced = m_buffer.replace( ofs, character );
		_regenHardLines();
		_regenSoftLines();
		_refreshAllLines();
		_onResize();
		return nReplaced;
	}
	
	
	//____ insertChar() ___________________________________________________________
	
	int LegacyTextField::insertChar( int ofs, const Char& character )
	{
		if( m_buffer.length() == m_maxChars )
			return 0;
	
		int nInserted = m_buffer.insert( ofs, character );
		_regenHardLines();
		_regenSoftLines();
		_refreshAllLines();
		_onResize();
		return nInserted;
	}
	
	//____ deleteChar() ___________________________________________________________
	
	int LegacyTextField::deleteChar( int ofs )
	{
		int nDeleted = m_buffer.remove( ofs, 1 );
		_regenHardLines();
		_regenSoftLines();
		_refreshAllLines();
		return nDeleted;
	}
	
	//____ deleteSelected() ___________________________________________________
	
	void LegacyTextField::deleteSelected()
	{
		int line = m_selStartLine;
		int column = m_selStartCol;
	
		int start = lineColToOffset( m_selStartLine, m_selStartCol );
		int end = lineColToOffset( m_selEndLine, m_selEndCol );
	
	
		remove( start, end-start );
		if(m_pCursor)
		{
			m_pCursor->setSelectionMode(false);
			m_pCursor->gotoHardPos(line, column);
		}
	
		_onResize();
	}
	
	//____ lines() ______________________________________________________________
	
	int LegacyTextField::lines() const
	{
		return m_nHardLines;
	}
	
	//____ nbSoftLines() __________________________________________________________
	
	int LegacyTextField::nbSoftLines() const
	{
		return m_nSoftLines;
	}
	
	int LegacyTextField::getSoftLineSelectionWidth(int line) const
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
	
	const LegacyTextLine * LegacyTextField::getLines() const
	{
		return m_pHardLines;
	}
	
	//____ getSoftLines() _________________________________________________________
	
	const LegacyTextLine * LegacyTextField::getSoftLines() const
	{
		return m_pSoftLines;
	}
	
	//____ getLine() ______________________________________________________________
	
	LegacyTextLine * LegacyTextField::getLine( int line ) const
	{
		if( line < 0 || line >= m_nHardLines )
			return 0;
	
		return m_pHardLines + line;
	}
	
	//____ getSoftLine() __________________________________________________________
	
	LegacyTextLine * LegacyTextField::getSoftLine( int line ) const
	{
		if( line < 0 || line >= m_nSoftLines )
			return 0;
	
		return m_pSoftLines + line;
	}
	
	//____ getLineText() __________________________________________________________
	
	const Char * LegacyTextField::getLineText( int line ) const
	{
		if( line < 0 || line >= m_nHardLines )
			return 0;
	
		return m_buffer.chars() + m_pHardLines[line].ofs;
	}
	
	//____ getSoftLineText() ______________________________________________________
	
	const Char * LegacyTextField::getSoftLineText( int line ) const
	{
		if( line < 0 || line >= m_nSoftLines )
			return 0;
	
		return m_buffer.chars() + m_pSoftLines[line].ofs;
	}
	
	//____ setWrap() ______________________________________________________________
	
	void LegacyTextField::setWrap( bool bWrap )
	{
		if( m_bWrap != bWrap )
		{
			m_bWrap = bWrap;
			_regenSoftLines();
			_refreshAllLines();
			_onResize();
		}
	}
	
	//____ setAutoEllipsis() ______________________________________________________
	
	void LegacyTextField::setAutoEllipsis( bool bAutoEllipsis )
	{
		if( m_bAutoEllipsis != bAutoEllipsis )
		{
			m_bAutoEllipsis = bAutoEllipsis;
			_regenSoftLines();
			_refreshAllLines();
			_onResize();
		}
	}
	
	//____ setMaxChars() __________________________________________________________
	
	bool LegacyTextField::setMaxChars( int max )
	{
		if( max <= 0 )
			return false;
	
		if( max != m_maxChars )
		{
			m_maxChars = max;
			if( m_maxChars > (int) m_buffer.length() )
				remove(m_maxChars, INT_MAX);
		}
		return true;
	}
	
	//____ setEditMode() __________________________________________________________
	
	void LegacyTextField::setEditMode(TextEditMode mode)
	{
		if( mode == m_editMode )
			return;
	
		m_editMode = mode;
	
		if( isEditable() )
		{
			m_pCursor = new CaretInstance(*this);
		}
		else
		{
			delete m_pCursor;
			m_pCursor = 0;
		}
	}
	
	
	//____ setLineWidth() _________________________________________________________
	
	void LegacyTextField::setLineWidth( int width )
	{
		if( width != m_lineWidth )
		{
			m_lineWidth = width;
	
			if( m_bWrap )
			{
				_regenSoftLines();
				_refreshAllLines();
			}
		}
	}
	
	//____ posSoft2Hard() _________________________________________________________
	
	void LegacyTextField::posSoft2Hard( int &line, int &col ) const
	{
		// Get a character pointer from soft line/col
	
		if( line < 0 )
			line = 0;
	
		if( line >= m_nSoftLines )
			line = m_nSoftLines - 1;
	
		LegacyTextLine * pLine = &m_pSoftLines[line];
	
		const Char * pOfs = m_buffer.chars() + pLine->ofs;
	
		if( col > pLine->nChars )
			pOfs += pLine->nChars;
		else if( col > 0 )
			pOfs += col;
	
		// Convert character pointer to hard line/col
	
		int ln = 0;
		while( pOfs > m_buffer.chars() + m_pHardLines[ln].ofs + m_pHardLines[ln].nChars )
			ln++;
	
		line = ln;
		col = pOfs - (m_buffer.chars() + m_pHardLines[ln].ofs);
		return;
	}
	
	//____ posHard2Soft() _________________________________________________________
	
	void LegacyTextField::posHard2Soft( int &line, int &col ) const
	{
		// Get a character pointer from hard line/col
	
		if( line >= m_nHardLines )
			line = m_nHardLines - 1;
	
		LegacyTextLine * pLine = &m_pHardLines[line];
	
		const Char * pOfs = m_buffer.chars() + pLine->ofs;
		if( col > pLine->nChars )
			pOfs += pLine->nChars;
		else
			pOfs += col;
	
		// Convert character pointer to soft line/col
	
		int ln = line;							// Corresponding soft line needs to be same or higher...
		while( pOfs > m_buffer.chars() + m_pSoftLines[ln].ofs + m_pSoftLines[ln].nChars )
			ln++;
	
		line = ln;
		col = pOfs - (m_buffer.chars() + m_pSoftLines[ln].ofs);
		return;
	}
	
	//____ _countWriteSoftLines() __________________________________________________
	
	int LegacyTextField::_countWriteSoftLines( int maxWidth, const Char * pStart, LegacyTextLine * pWriteLines, int maxWrite ) const
	{
		const Char *	p = pStart;
		int			nSoftLines = 0;
		uint16_t		hProp = 0xFFFF;					// Force immediate update of textprop.
		bool		bEndOfText = false;
	
		Pen		pen;
		TextAttr	attr;
	
		maxWidth -= _cursorMaxWidth();
	
		while( !bEndOfText )
		{
	
			const Char * 	pLineStart = p;
			const Char *	pbp = 0;				// BreakPoint-pointer.
			bool			bBreakSkips = false;	// Set if the character on the breakpoint should be skipped.
			bool			bBreakAfterPrev = false;// Set if we can break here due to a WG_BREAK_BEFORE on previous character.
	
			while( true )
			{
	
				// Update if textproperties have changed.
	
				if( p->propHandle() != hProp )
				{
					getCharAttr(attr, p - m_buffer.chars());
					pen.setAttributes(attr);
					hProp = p->propHandle();
				}
	
				// Break if this is end of line or end of text
	
				if( p->getGlyph() == '\n' )
				{
					bBreakSkips = true;
					break;
				}
				else if( p->getGlyph() == 0 )
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
	
				BreakRules breakStatus = TextTool::isBreakAllowed( p->glyph, attr.breakLevel );
				switch( breakStatus )
				{
				case WG_BREAK_BEFORE:
					pbp = p;
					bBreakSkips = false;
					break;
	
				case WG_BREAK_ON:
	
					if( p->getGlyph() == WG_HYPHEN_BREAK_PERMITTED )
					{
						// Check so a hyphen will fit on the line as well, otherwise we can't break here.
						// We don't take kerning into account here, not so important.
	
						Glyph_p pHyphen = pen.getGlyphset()->getGlyph( '-', pen.getSize() );
						if( !pHyphen || (pen.getPosX() + pHyphen->advance()) > maxWidth )
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
	
				pen.setChar( p->getGlyph() );
				pen.applyKerning();
	
	
				// Check if we need to put a softbreak.
	
	//			uint32_t len = pen.getBlitPosX() + pen.getGlyph()->rect.w; // No advance on last character of line, just bearingX + width
				int len = pen.getPosX() + pen.getGlyph()->advance();
				if( len > maxWidth )
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
					pen.advancePos();
	
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
			pen.setPosX(0);			// Reset position
			pen.flushChar();		// Flush current character so it won't affect kerning for first character of next line.
		}
		return nSoftLines;
	}
	
	//____ _regenSoftLines() _______________________________________________________
	
	void LegacyTextField::_regenSoftLines()
	{
		bool	bHasSoftLineArray = (m_pSoftLines != m_pHardLines)?true:false;
	
		// If we don't wrap, we have no softlines...
	
		if( !m_bWrap || m_buffer.length() == 0 )
		{
			if( bHasSoftLineArray )
				delete [] m_pSoftLines;
	
			m_pSoftLines = m_pHardLines;
			m_nSoftLines = m_nHardLines;
			return;
		}
	
		int nSoftLines;
	
		if( bHasSoftLineArray )
			nSoftLines = _countWriteSoftLines( m_lineWidth, m_buffer.chars(), m_pSoftLines, m_nSoftLines );
		else
			nSoftLines = _countWriteSoftLines( m_lineWidth, m_buffer.chars(), 0, 0 );
	
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
	
			m_pSoftLines = new LegacyTextLine[nSoftLines];
			m_nSoftLines = nSoftLines;
		}
	
		// Fill in the softlines-array.
	
		_countWriteSoftLines( m_lineWidth, m_buffer.chars(), m_pSoftLines, m_nSoftLines );
	}
	
	//____ _refreshAllLines() _________________________________________________________
	
	void LegacyTextField::_refreshAllLines()
	{
		for( int i = 0 ; i < m_nHardLines ; i++ )
			_refreshLineInfo( &m_pHardLines[i] );
	
		if( m_pSoftLines != m_pHardLines )
		{
			for( int i = 0 ; i < m_nSoftLines ; i++ )
				_refreshLineInfo( &m_pSoftLines[i] );
		}
	}
	
	
	//____ _refreshLineInfo() ____________________________________________________
	
	// Refreshes the width, height, baseline and linespacing parameters of a
	// LegacyTextLine.
	
	
	void LegacyTextField::_refreshLineInfo( LegacyTextLine * pLine ) const
	{
		int maxAscend = 0;
		int maxDescend = 0;
		int maxLineDescend = 0;
	
		const Char * pChars = m_buffer.chars() + pLine->ofs;
		int		nChars = pLine->nChars;
		uint16_t	hProp = 0xFFFF;
	
		Pen pen;
	
		// We must have at least one character, even if it is just CR/LF or EndOfString.
	
		if( nChars == 0 )
		{
			TextAttr	attr;
			getBaseAttr( attr );
			pen.setAttributes( attr );
	
			if( pen.getGlyphset() )
			{
				int height		= pen.getLineHeight();
				int baseline	= pen.getBaseline();
				int spacing		= pen.getLineSpacing();
	
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
				if( pChars[i].propHandle() != hProp )
				{
					TextAttr	attr;
					getCharAttr( attr, pLine->ofs + i );
					pen.setAttributes( attr );
	
					hProp = pChars[i].propHandle();
	
					if( !pen.getGlyphset() )
						break;									// Bail out instead of crashing.
	
					int height		= pen.getLineHeight();
					int baseline	= pen.getBaseline();
					int spacing		= pen.getLineSpacing();
	
					if( baseline > maxAscend )
						maxAscend = baseline;
	
					if( height - baseline > maxDescend )
						maxDescend = height - baseline;
	
					if( spacing - baseline > maxLineDescend )
						maxLineDescend = spacing - baseline;
				}
	
				pen.setChar( pChars[i].getGlyph() );
				pen.applyKerning();
				pen.advancePos();
			}
		}
	
		// Fill in line struct.
	
		pLine->width		= pen.getPosX() + _cursorMaxWidth();
		pLine->height		= maxAscend + maxDescend;
		pLine->lineSpacing	= (int) ((maxAscend + maxLineDescend)*m_lineSpaceAdj);
		pLine->baseline		= maxAscend;
	}
	
	//____ _cursorMaxWidth() ______________________________________________________
	// Space that needs to be reserved for cursor on each line.
	
	int LegacyTextField::_cursorMaxWidth() const
	{
		Caret_p p = m_pCursorStyle?m_pCursorStyle:Base::getDefaultCursor();
	
		if( p && m_editMode == WG_TEXT_EDITABLE )
		{
			int lenInsert = p->advance(Caret::INS);
			int lenOvr = p->advance(Caret::OVR);
			int lenEOL = p->bearingX(Caret::EOL) + p->width(Caret::EOL);
	
			return WgMax( lenEOL, WgMax(lenInsert,lenOvr) );
		}
		else
			return 0;
	}
	
	
	//____ cursorGotoCoord() ________________________________________________________
	
	void LegacyTextField::cursorGotoCoord( const Coord& coord, const Rect& container )
	{
		if( !m_pCursor )
			return;
	
		TextPos pos = coordToPos( coord, container, true );
		gotoSoftPos(pos.line,pos.col);
	}
	
	//____ cursorGotoLine() _________________________________________________________
	
	void LegacyTextField::cursorGotoLine( int line, const Rect& container )
	{
		if( !m_pCursor )
			return;
	
		m_pCursor->gotoSoftLine(line, container );
	}
	
	//____ cursorGoUp() _________________________________________________________
	
	void LegacyTextField::cursorGoUp( int nbLines, const Rect& container )
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
	
	//____ cursorGoDown() _________________________________________________________
	
	void LegacyTextField::cursorGoDown( int nbLines, const Rect& container )
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
	
	
	//____ lineStartY() ______________________________________________________________
	
	int LegacyTextField::lineStartY( int line, const Rect& container ) const
	{
		int		ofs = 0;
	
		if( m_alignment != WG_NORTHWEST && m_alignment != WG_NORTH && m_alignment != WG_NORTHEAST )
		{
			ofs = WgUtil::origoToRect( m_alignment, container.size(), Size(0,height() )).y;
			if( ofs < 0 )
				ofs = 0;
		}
		if( line > m_nSoftLines )
			line = m_nSoftLines;
	
		for( int i = 0 ; i < line ; i++ )
			ofs += m_pSoftLines[i].lineSpacing;
	
		return ofs + container.y;
	}
	
	//____ lineStartX() ______________________________________________________________
	
	int LegacyTextField::lineStartX( int line, const Rect& container ) const
	{
		int		ofs = 0;
	
		if( m_alignment != WG_NORTHWEST && m_alignment != WG_WEST && m_alignment != WG_SOUTHWEST )
		{
			ofs = WgUtil::origoToRect( m_alignment, container.size(), Size(getSoftLineWidth(line),0 )).x;
			if( ofs < 0 )
				ofs = 0;
		}
	
		return ofs + container.x;
	}
	
	//____ coordToLine() __________________________________________________________
	
	int LegacyTextField::coordToLine( const Coord& coord, const Rect& container, bool bCursorMode ) const
	{
		int y = coord.y - lineStartY( 0, container );
	
		LegacyTextLine * pLine = m_pSoftLines;
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
	
	//____ coordToColumn() ___________________________________________________________
	
	int LegacyTextField::coordToColumn( int line, const Coord& coord, const Rect& container, bool bCursorMode ) const
	{
		//TODO: Take cursor and selection into account!!!
	
		if( line < 0 || line >= m_nSoftLines )
			return -1;
	
		LegacyTextLine * pLine = &m_pSoftLines[line];
	
		int xStart = lineStartX( line, container );
	
		if( coord.x < xStart )
			return bCursorMode?0:-1;
	
		if( coord.x >= xStart + pLine->width )
			return bCursorMode?pLine->nChars:-1;
	
		const Char * pChars = m_buffer.chars() + pLine->ofs;
		uint16_t	hProp = 0xFFFF;
		Pen pen;
		pen.setOrigo( Coord(xStart,0) );
		pen.setPosX(xStart);
	
		for( int i = 0 ; i < pLine->nChars ; i++ )
		{
			if( pChars[i].propHandle()!= hProp )
			{
				TextAttr	attr;
				getCharAttr( attr, pLine->ofs + i );
				pen.setAttributes( attr );
	
				hProp = pChars[i].propHandle();
			}
	/*
			if( cursColumn == 0 )
				pen.advancePosCursor( * pCursor );
			cursColumn--;
	*/
			int charStartX = pen.getPosX();
			pen.setChar( pChars[i].getGlyph() );
			pen.applyKerning();
			pen.advancePos();
			int charEndX = pen.getPosX();
	
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
	
	//_____ coordToPos() __________________________________________________________
	
	TextPos LegacyTextField::coordToPos( const Coord& coord, const Rect& container, bool bCursorMode ) const
	{
		int line = coordToLine( coord, container, bCursorMode );
		if( line == -1 )
			return TextPos(-1,-1);
	
		int col = coordToColumn( line, coord, container, bCursorMode );
		if( col == -1 )
			return TextPos(-1,-1);
	
		return TextPos(line,col);
	}
	
	//____ coordToOfs() ___________________________________________________________
	
	int LegacyTextField::coordToOfs( const Coord& coord, const Rect& container, bool bCursorMode ) const
	{
		TextPos pos = coordToPos( coord, container, bCursorMode );
	
		if( pos.line == -1 || pos.col == -1 )
			return -1;
		else
			return posToOfs( pos );
	}
	
	
	//____ coordToLink() __________________________________________________________
	
	TextLink_p LegacyTextField::coordToLink( const Coord& coord, const Rect& container ) const
	{
		int ofs = coordToOfs( coord, container );
		if( ofs >= 0 )
			return getCharLink( ofs );
		else
			return 0;
	}
	
	//____ ofsToCoordY() _________________________________________________________
	
	int LegacyTextField::ofsToCoordY( int ofs, const Rect& container ) const
	{
		int y = lineStartY( ofsToPos(ofs).line, container );
	
		for( int i = 0 ; i < m_nSoftLines ; i++ )
		{
			if( ofs < m_pSoftLines[i].ofs + m_pSoftLines[i].nChars )
				break;
	
			y += m_pSoftLines[i].lineSpacing;
		}
		return y;
	}
	
	//____ ofsToCoordX() _________________________________________________________
	
	int LegacyTextField::ofsToCoordX( int ofs, const Rect& container ) const
	{
		return posToCoordX( ofsToPos(ofs), container );
	}
	
	//____ ofsToCoord() ___________________________________________________________
	
	Coord LegacyTextField::ofsToCoord( int ofs, const Rect& container ) const
	{
		return posToCoord( ofsToPos(ofs), container );
	}
	
	//____ ofsToPos() _____________________________________________________________
	
	TextPos LegacyTextField::ofsToPos( int ofs ) const
	{
		if( ofs < 0 )
			return TextPos(0,0);
	
		if( ofs >= (int) m_buffer.length() )
			return TextPos(m_nSoftLines-1, m_pSoftLines[m_nSoftLines-1].nChars);
	
		int ln = 0;
		while( m_pSoftLines[ln].ofs + m_pSoftLines[ln].nChars < ofs )
			ln++;
	
		return TextPos(ln, ofs - m_pSoftLines[ln].ofs);
	}
	
	//____ posToCoordX() __________________________________________________________
	
	int LegacyTextField::posToCoordX( const TextPos& _pos, const Rect& container ) const
	{
		//TODO: Take cursor and selection into account!!!
	
		TextPos	pos = clampPos(_pos);
	
		LegacyTextLine * pLine = &m_pSoftLines[pos.line];
	
		const Char * pChars = m_buffer.chars() + pLine->ofs;
		uint16_t	hProp = 0xFFFF;
		Pen pen;
		
		int startX = lineStartX( pos.line, container );
		pen.setOrigo( Coord(startX,0) );
		pen.setPosX(startX);
	
		for( int i = 0 ; i < pos.col ; i++ )
		{
			if( pChars[i].propHandle()!= hProp )
			{
				TextAttr	attr;
				getCharAttr( attr, pLine->ofs + i );
				pen.setAttributes( attr );
	
				hProp = pChars[i].propHandle();
			}
	/*
			if( cursColumn == 0 )
				pen.advancePosCursor( * pCursor );
			cursColumn--;
	*/
	
			pen.setChar( pChars[i].getGlyph() );
			pen.applyKerning();
			pen.advancePos();
		}
	
		return pen.getPosX();
	}
	
	//____ posToCoordY() __________________________________________________________
	
	int LegacyTextField::posToCoordY( const TextPos& pos, const Rect& container ) const
	{
		return lineStartY( pos.line, container );
	}
	
	//____ posToCoord() ___________________________________________________________
	
	Coord LegacyTextField::posToCoord( const TextPos& pos, const Rect& container ) const
	{
		return Coord( posToCoordX(pos, container), posToCoordY(pos, container) );
	}
	
	//____ posToOfs() _____________________________________________________________
	
	int LegacyTextField::posToOfs( const TextPos& _pos ) const
	{
		TextPos pos = clampPos(_pos);
		return m_pSoftLines[pos.line].ofs + pos.col;
	}
	
	//____ clampPos() _____________________________________________________________
	
	TextPos LegacyTextField::clampPos( TextPos pos ) const
	{
		if( pos.line < 0 )
			return TextPos(0,0);
	
		if( pos.line >= m_nSoftLines )
			return TextPos(m_nSoftLines-1, m_pSoftLines[m_nSoftLines-1].nChars);
	
		if( pos.col < 0 )
			return TextPos(pos.line,0);
	
		if( pos.col > m_pSoftLines[pos.line].nChars )
			return TextPos(pos.line, m_pSoftLines[pos.line].nChars );
	
		return pos;
	}
	
	//____ FocusRange() ___________________________________________________________
	
	Coord LegacyTextField::focusWindowOnRange( const Size& canvas, const Rect& _window, Range range ) const
	{
		if( _window == canvas )
			return _window.pos();
	
		// Move view so that range stays visible with:
		// 1. At least one character displayed before and (if possible) after the range.
		// 2. At least one line is displayed after the range (if possible) if text is multiline.
		// 3. If range doesn't fit in view, beginning of view is prioritized
	
		// Get rectangle we want to focus
	
		TextPos	posBeg = ofsToPos(range.begin());
		TextPos	posEnd = ofsToPos(range.end());
	
		posBeg.col--;							// We want at least one empty character before the range.
		posEnd.col++;							// We want at least one empty character after the range.
	
		Coord	rangeBegin = posToCoord(posBeg,canvas);
		Coord	rangeEnd = posToCoord(posEnd,canvas);
	
		rangeEnd.y += m_pSoftLines[posEnd.line].height;
	
		if( posEnd.line < m_nSoftLines-1 )
			rangeEnd.y += m_pSoftLines[posEnd.line+1].height;		// We want one empty line after the range if possible.
	
		// We might need to adjust the begin/end coordinates
		// if character range includes end of line 
		// (add space for cursor) or multiple lines
		// (make sure start and end of all lines are included)
	
		for( int i = posBeg.line ; i <= posEnd.line ; i++ )
		{
			// Include beginning of all lines except the first.
	
			int beg = lineStartX( i, canvas );
			if( i != posBeg.line && beg < rangeBegin.x )
				rangeBegin.x = beg;
	
			// Include end of all lines except the last.
			// Include end of last line if input range includes last character of the line.
	
			int end = beg + m_pSoftLines[i].width;
			if( i != posEnd.line || posEnd.col > m_pSoftLines[posEnd.line].nChars )
				if( end > rangeEnd.x )
					rangeEnd.x = end;
		}
	
		Rect	rangeRect( rangeBegin, rangeEnd );
	
		// Adjust window to contain range
	
		Rect window = _window;
	
		if( rangeRect.right() > window.right() )
			window.x = rangeRect.right() - window.w;
	
		if( rangeRect.bottom() > window.bottom() )
			window.x = rangeRect.bottom() - window.h;
	
		if( rangeRect.x < window.x )
			window.x = rangeRect.x;
	
		if( rangeRect.y < window.y )
			window.y = rangeRect.y;
	
		// Adjust window to stay inside canvas
	
		if( window.x < 0 )
			window.x = 0;
	
		if( window.y < 0 ) 
			window.y = 0;
	
		if( window.w > canvas.w )
			window.x = 0;
		else if( window.x + window.w > canvas.w )
			window.x = canvas.w - window.w;
	
		if( window.h > canvas.h )
			window.y = 0;
		else if( window.y + window.h > canvas.h )
			window.y = canvas.h - window.h;
	
		return window.pos();
	
	/*
		// Possibly move viewOfs so that:
		//	1 Cursor remains inside view.
		//  2 At least one character is displayed before the cursor
		//  3 At least one character is displayed after the cursor (if there is one).
	
		if( m_bFocused && m_pText->getFont() )
		{
			Caret * pCursor = TextTool::getCursor( m_pText );
			if( !pCursor )
				return;
	
			int cursCol	= m_pText->column();
	
			TextAttr	attr;
			m_pText->getBaseAttr( attr );
	
			Pen	pen;
			pen.setAttributes( attr );
			pen.setChar(m_pwGlyph);
			pen.advancePos();
	
			int pwAdvance	= pen.getPosX();
			int cursAdvance	= pCursor->advance(m_pText->cursorMode() );
			int cursBearing	= pCursor->bearingX(m_pText->cursorMode() );
			int cursWidth	= pCursor->width(m_pText->cursorMode() );
	
			int cursOfs;		// Cursor offset from beginning of line in pixels.
			int maxOfs;			// Max allowed view offset in pixels.
			int minOfs;			// Min allowed view offset in pixels.
	
			int geoWidth = size().w;
			int	lineWidth = m_pText->getSoftLineWidth( 0 ) + cursBearing+cursWidth;
	
			// Calculate cursOfs
	
			if( m_bPasswordMode )
				cursOfs = cursCol * pwAdvance;
			else
				cursOfs	= m_pText->getSoftLineWidthPart( 0, 0, cursCol );
	
			// Calculate maxOfs
	
			if( cursCol > 0 )
			{
				if( m_bPasswordMode )
					maxOfs = (cursCol-1) * pwAdvance;
				else
					maxOfs = m_pText->getSoftLineWidthPart( 0, 0, cursCol-1 );
	
				if( lineWidth < maxOfs + geoWidth )
					maxOfs = WgMax( lineWidth - geoWidth, 0 );
			}
			else
				maxOfs = cursOfs;
	
	
			// Calculate minOfs
	
			if( cursCol < m_pText->getLine(0)->nChars )
			{
				if( m_bPasswordMode )
					minOfs = (cursCol+1) * pwAdvance + cursAdvance - geoWidth;
				else
					minOfs = m_pText->getSoftLineWidthPart( 0, 0, cursCol+1 ) + cursAdvance - geoWidth;	// Not 100% right, cursor might affect linewidth different from its own width.
			}
			else
				minOfs = cursOfs + cursBearing + cursWidth - geoWidth;
	
			// Check boundaries and update
	
			if( m_viewOfs > maxOfs )
				m_viewOfs = maxOfs;
	
			if( m_viewOfs < minOfs )
				m_viewOfs = minOfs;
	
	
		}
		else
			m_viewOfs = 0;				// Show beginning of line when cursor disappears.
	
	*/		
	}
	
	
	//____ lineColToOffset() ______________________________________________________
	
	int LegacyTextField::lineColToOffset(int line, int col) const
	{
		return m_pHardLines[line].ofs + col;
	}
	
	
	//____ onMsg() _____________________________________________________________
	
	bool LegacyTextField::onMsg( const Msg_p& pMsg, MsgRouter * pMsgRouter, const Rect& container )
	{
		bool bRefresh = false;
	
		switch( pMsg->type() )
		{
			case WG_MSG_MOUSE_ENTER:
			case WG_MSG_MOUSE_MOVE:
			{
	/*
				Coord pointerOfs = pMsg->pointerPos();
				
				TextLink_p pLink = coordToLink( pointerOfs, container );
				if( m_pMarkedLink && pLink != m_pMarkedLink )
				{
					pMsgRouter->post( new LinkMouseLeaveMsg( m_pMarkedLink ));
					m_pMarkedLink = 0;
					bRefresh = true;
				}
	
				if( pLink )
				{
					if( pLink != m_pMarkedLink )
					{
						pMsgRouter->post( new LinkMouseEnterMsg( pLink ));
	
						m_pMarkedLink = pLink;
						m_markedLinkState = WG_STATE_HOVERED;
						bRefresh = true;
					}
				}
				break;
	*/
			}
	
	
			case WG_MSG_MOUSE_LEAVE:
			{
				if( m_pMarkedLink )
				{
					pMsgRouter->post( new LinkMouseLeaveMsg( m_pMarkedLink ));
					m_pMarkedLink = 0;
					bRefresh = true;
				}
				break;
			}
	
			case WG_MSG_MOUSE_PRESS:
			{
				if( m_pMarkedLink )
				{
					pMsgRouter->post( new LinkMousePressMsg( m_pMarkedLink, MouseButtonMsg::cast(pMsg)->button() ));
					m_markedLinkState = WG_STATE_PRESSED;
					bRefresh = true;
				}
				break;
			}
	
			case WG_MSG_MOUSE_REPEAT:
			{
				if( m_pMarkedLink )
				{
					pMsgRouter->post( new LinkMouseRepeatMsg( m_pMarkedLink, MouseButtonMsg::cast(pMsg)->button() ));
				}
				break;
			}
	
			case WG_MSG_MOUSE_RELEASE:
			{
				if( m_pMarkedLink )
				{
					pMsgRouter->post( new LinkMouseReleaseMsg( m_pMarkedLink, MouseButtonMsg::cast(pMsg)->button() ));
	
					if( m_markedLinkState == WG_STATE_PRESSED )
					{
						pMsgRouter->post( new LinkMouseClickMsg( m_pMarkedLink, MouseButtonMsg::cast(pMsg)->button() ));
						pMsgRouter->post( new LinkSelectMsg( m_pMarkedLink ));				
					}
					m_markedLinkState = WG_STATE_HOVERED;
					bRefresh = true;
				}
				break;
			}
	
			case WG_MSG_MOUSE_DOUBLE_CLICK:
				if( m_pMarkedLink )
					pMsgRouter->post( new LinkMouseDoubleClickMsg( m_pMarkedLink, MouseButtonMsg::cast(pMsg)->button() ));
				break;
	
			default:
				break;
		}
	
		//TODO: Optimize, we don't always need a complete refresh, sometimes just a render.
	
		if( bRefresh )
			refresh();
	
		return bRefresh;
	}
	
	
	//____ onAction() _____________________________________________________________
	/*
	bool LegacyTextField::onAction( WgInput::UserAction action, int button_key, const Rect& container, const Coord& pointerOfs )
	{
		bool bRefresh = false;
		TextLinkHandler * pHandler;
	
		if( m_pMarkedLink )
		{
			pHandler = m_pMarkedLink->Handler();
			if( !pHandler )
				pHandler = Base::GetDefaultTextLinkHandler();
		}
	
		switch( action )
		{
			case WgInput::POINTER_ENTER:
			case WgInput::POINTER_OVER:
			{
				TextLink_p pLink = coordToLink( pointerOfs, container );
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
							pHandler = Base::GetDefaultTextLinkHandler();
	
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
	
			case WgInput::BUTTON_DOUBLE_CLICK:
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
	*/
	
	//____ getBaseAttr() __________________________________________________________
	
	void LegacyTextField::getBaseAttr( TextAttr& attr ) const
	{
		attr.clear();
		TextTool::addPropAttributes( attr, Base::getDefaultTextprop(), m_state );
	//	TextTool::setAttrColor( attr, m_pBgBlockColors, m_state );
		TextTool::addPropAttributes( attr, m_pBaseProp, m_state );
	}
	
	//____ getCharAttr() __________________________________________________________
	
	bool LegacyTextField::getCharAttr( TextAttr& attr, int charOfs ) const
	{
		if( charOfs >= (int) m_buffer.length() )
			return false;
	
		attr.clear();
		TextTool::addPropAttributes( attr, Base::getDefaultTextprop(), m_state );
	//	TextTool::setAttrColor( attr, m_pBgBlockColors, m_state );
		TextTool::addPropAttributes( attr, m_pBaseProp, m_state );
	
		// Add selection properties if character is selected
	
		if( m_selStartLine != m_selEndLine || m_selStartCol != m_selEndCol )
		{
			int selStart = lineColToOffset( m_selStartLine, m_selStartCol );
			int selEnd = lineColToOffset( m_selEndLine, m_selEndCol );
	
			if( charOfs >= selStart && charOfs < selEnd )
			{
				if( m_pSelectionProp )
					TextTool::addPropAttributes( attr, m_pSelectionProp, m_state );
				else
					TextTool::addPropAttributes( attr, Base::getDefaultSelectionProp(), m_state );
			}
		}
	
		// Add link properties if character is part of a link
	
		Textprop_p pCharProp = m_buffer.chars()[charOfs].getProperties();
	
		TextLink_p pLink = pCharProp->link();
		if( !pLink )
			pLink = m_pBaseProp->link();
	
		if( pLink )
		{
			Textprop_p pProp = m_pLinkProp?m_pLinkProp:Base::getDefaultLinkProp();
	
			State	state;
	
			if( pLink == m_pMarkedLink )
			{
				state = m_markedLinkState;
			}
			else
			{
				if( m_state == WG_STATE_DISABLED )
					state = WG_STATE_DISABLED;
				else if( pLink->hasBeenAccessed() )
					state = WG_STATE_SELECTED;
				else
					state = WG_STATE_NORMAL;
			}
	
	
			TextTool::addPropAttributes( attr, pProp, state );
		}
	
		// Add characters own properties
	
		TextTool::addPropAttributes( attr, pCharProp, m_state );
	
		return true;
	}
	
	bool LegacyTextField::isCharUnderlined( int charOfs ) const
	{
		//TODO: Optimize
		TextAttr	attr;
		getCharAttr(attr,charOfs);
		return attr.bUnderlined;
	}
	
	Color LegacyTextField::getCharColor( int charOfs ) const
	{
		//TODO: Optimize
		TextAttr	attr;
		getCharAttr(attr,charOfs);
		return attr.color;
	}
	
	Color LegacyTextField::getCharBgColor( int charOfs ) const
	{
		//TODO: Optimize
		TextAttr	attr;
		getCharAttr(attr,charOfs);
		return attr.bgColor;
	}
	
	FontAlt LegacyTextField::getCharStyle( int charOfs ) const
	{
		//TODO: Optimize
		TextAttr	attr;
		getCharAttr(attr,charOfs);
		return attr.style;
	}
	
	int LegacyTextField::getCharSize( int charOfs ) const
	{
		//TODO: Optimize
		TextAttr	attr;
		getCharAttr(attr,charOfs);
		return attr.size;
	}
	
	Font_p LegacyTextField::getCharFont( int charOfs ) const
	{
		//TODO: Optimize
		TextAttr	attr;
		getCharAttr(attr,charOfs);
		return attr.pFont;
	}
	
	int LegacyTextField::getCharBreakLevel( int charOfs ) const
	{
		//TODO: Optimize
		TextAttr	attr;
		getCharAttr(attr,charOfs);
		return attr.breakLevel;
	}
	
	TextLink_p LegacyTextField::getCharLink( int charOfs ) const
	{
		//TODO: Optimize
		TextAttr	attr;
		getCharAttr(attr,charOfs);
		return attr.pLink;
	}
	
	bool LegacyTextField::isCharLink( int charOfs ) const
	{
		//TODO: Optimize
		TextAttr	attr;
		getCharAttr(attr,charOfs);
		return attr.pLink?true:false;
	}

} // namespace wg
