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


#include <wg_caretinstance.h>

#ifndef WG_LEGACYTEXTFIELD_DOT_H
#	include <wg_legacytextfield.h>
#endif

#ifndef WG_TEXTTOOL_DOT_H
#	include <wg_texttool.h>
#endif

#ifndef WG_FONT_DOT_H
#	include <wg_font.h>
#endif

#include <wg_widget.h>

namespace wg 
{
	
	//____ Constructor() __________________________________________________________
	
	CaretInstance::CaretInstance( LegacyTextField& text )
	{
		m_pText 		= &text;
		m_bHidden		= true;
		m_bInsert		= true;
		m_bSelectMode	= false;
		m_wantedOfsX	= -1;
	
		m_time 			= 0;
	
		m_line 			= text.lines()-1;
		m_column 		= text.getLine(m_line)->nChars;
		m_selStartLine	= m_line;
		m_selStartColumn= m_column;
	}
	
	//____ incTime() _______________________________________________________________
	
	bool CaretInstance::incTime( int ms )
	{
		m_time += ms;
		return true; //TODO: Should only return true if cursor needs to be re-rendered.
	}
	
	//____ hide() _________________________________________________________________
	
	void CaretInstance::hide()
	{
		m_bHidden = true;
	}
	
	//____ show() _________________________________________________________________
	
	void CaretInstance::show()
	{
		m_bHidden = false;
	}
	
	//____ isHidden() _____________________________________________________________
	
	bool CaretInstance::isHidden() const
	{
		return m_bHidden;
	}
	
	
	
	//____ insertMode() ____________________________________________________________
	
	void CaretInstance::insertMode( bool bInsert )
	{
		m_bInsert = bInsert;
	}
	
	
	//____ gotoSoftLine() _________________________________________________________
	
	void CaretInstance::gotoSoftLine( int line, const Rect& container )
	{
		// First we need to update m_wantedOfsX...
	
		if( m_wantedOfsX == -1 )
		{
			int	ln = m_line;
			int	col = m_column;
			m_pText->posHard2Soft( ln, col );
	
			m_wantedOfsX = m_pText->getSoftLineWidthPart( ln, 0, col );
		}
	
		// Set our line and convert to hard
	
		int ln = line;
		int col = m_pText->coordToColumn( line, Coord( m_wantedOfsX+container.x, 0), container, true );
	
	
		m_pText->posSoft2Hard( ln, col );
		_updateLocation(ln, col);
	}
	
	
	//____ gotoColumn() ____________________________________________________________
	
	void CaretInstance::gotoColumn( int col )
	{
		const int minLine = 0;
		const int maxLine = m_pText->lines()-1;
	
		int line = m_line;
	
		if( line > maxLine )
			line = maxLine;
	
		int maxCol = m_pText->getLine(line)->nChars;
	
		// Check whether to hop down/up on lines
		if(col < 0)
		{
			line = line - 1;
	
			// Have we reached the top?
			if(line < minLine)
			{
				// Yes
				line = minLine;
				col = 0;
			}
			else
			{
				// No, set cursor column to end of line
				col = m_pText->getLine(line)->nChars;
			}
		}
		else if( col > (int)maxCol )
		{
			line = line + 1;
	
			// Have we reached the bottom?
			if(line > maxLine)
			{
				// Yes, set cursor on end of line
				line = maxLine;
				col = m_pText->getLine(line)->nChars;
			}
			else
			{
				// No, set cursor column to 0
				col = 0;
			}
		}
	
		_updateLocation(line, col);
	
		m_wantedOfsX = -1;
	}
	
	//____ gotoPrevWord() _________________________________________________________
	
	void CaretInstance::gotoPrevWord()
	{
		gotoBeginningOfWord();
	
		if(m_line == 0 && m_column == 0)
			return;
	
		int line = m_line;
		int col = m_column;
	
		if(line > m_pText->lines() - 1)
			line = m_pText->lines() - 1;
	
		if(col > m_pText->getLine(line)->nChars)
			col = m_pText->getLine(line)->nChars;
	
		const Char* pText = m_pText->getLineText(line) + col - 1;
	
		while( !isspace(pText->getGlyph()) && !ispunct(pText->getGlyph()) )
		{
			if(col == 0)
			{
				if(line == 0)
					break;
				line--;
				col = m_pText->getLine(line)->nChars;
			}
			else
			{
				col--;
				if(line == 0 && col == 0)
					break;
			}
			pText--;
		}
	
		_updateLocation(line, col);
		m_wantedOfsX = -1;
	}
	
	//_____________________________________________________________________________
	void CaretInstance::gotoBeginningOfWord()
	{
		if(m_line == 0 && m_column == 0)
			return;
	
		int line = m_line;
		int col = m_column;
	
		if(line > m_pText->lines() - 1)
			line = m_pText->lines() - 1;
	
		if(col > m_pText->getLine(line)->nChars)
			col = m_pText->getLine(line)->nChars;
	
		const Char* pText = m_pText->getLineText(line) + col - 1;
	
		while( isspace(pText->getGlyph()) || ispunct(pText->getGlyph()) )
		{
			if(col == 0)
			{
				if(line == 0)
					break;
				line--;
				col = m_pText->getLine(line)->nChars;
			}
			else
			{
				col--;
				if(line == 0 && col == 0)
					break;
			}
			pText--;
		}
	
		_updateLocation(line, col);
		m_wantedOfsX = -1;
	}
	
	//____ gotoNextWord() _________________________________________________________
	
	void CaretInstance::gotoNextWord()
	{
		gotoEndOfWord();
	
		int line = m_line;
		int col = m_column;
	
		if(line > m_pText->lines() - 1)
			line = m_pText->lines() - 1;
	
		if(col > m_pText->getLine(line)->nChars)
			col = m_pText->getLine(line)->nChars;
	
		const Char* pText = m_pText->getLineText(line) + col;
	
		while( !pText->isEndOfText() && (isspace(pText->getGlyph()) || ispunct(pText->getGlyph())) )
		{
			if(pText->isEndOfLine())
				line++, col = 0;
			else
				col++;
			pText++;
		}
	
		_updateLocation(line, col);
		m_wantedOfsX = -1;
	}
	
	//_____________________________________________________________________________
	void CaretInstance::gotoEndOfWord()
	{
		int line = m_line;
		int col = m_column;
	
		if(line > m_pText->lines() - 1)
			line = m_pText->lines() - 1;
	
		if(col > m_pText->getLine(line)->nChars)
			col = m_pText->getLine(line)->nChars;
	
		const Char* pText = m_pText->getLineText(line) + col;
	
		while( !pText->isEndOfText() && !isspace(pText->getGlyph()) && !ispunct(pText->getGlyph()) )
		{
			if(pText->isEndOfLine())
				line++, col = 0;
			else
				col++;
			pText++;
		}
	
		_updateLocation(line, col);
		m_wantedOfsX = -1;
	}
	
	//____ gotoHardPos() __________________________________________________________
	
	void CaretInstance::gotoHardPos( int line, int col )
	{
		_gotoPos( line, col );
		m_wantedOfsX = -1;
	}
	
	
	//____ _gotoPos() ______________________________________________________________
	
	void CaretInstance::_gotoPos( int line, int col )
	{
		int maxLine = m_pText->lines()-1;
		if( line > maxLine )
			line = maxLine;
	
		int maxCol = m_pText->getLine(line)->nChars;
		if( col > maxCol )
			col = maxCol;
	
		_updateLocation(line, col);
	}
	
	//____ gotoSoftPos() __________________________________________________________
	
	void CaretInstance::gotoSoftPos( int line, int col )
	{
		m_pText->posSoft2Hard( line, col );
	
		_updateLocation(line, col);
	
		m_wantedOfsX = -1;
	}
	
	//____ getSoftPos() ___________________________________________________________
	
	void CaretInstance::getSoftPos( int &line, int &col ) const
	{
		line	= m_line;
		col		= m_column;
	
		m_pText->posHard2Soft( line, col );
	}
	
	
	
	//____ putChar() ______________________________________________________________
	
	bool CaretInstance::putChar( Uint16 character )
	{
		m_wantedOfsX = -1;
	
		int ret;
	
		int line = m_line;
		int column = m_column;
	
		int ofs = m_pText->lineColToOffset( line, column );
	
		if( m_bInsert )
			ret = m_pText->insertChar( ofs, character );
	
		else
			ret = m_pText->replaceChar( ofs, character );
	
	
		if( character == '\n' )
		{
			line++;
			column = 0;
		}
		else if( ret == 1 )
			column++;
	
	
		_updateLocation(line, column);
	
		return ret==1?true:false;
	}
	
	//____ putText() ______________________________________________________________
	
	int	CaretInstance::putText( const CharSeq& seq )
	{
		m_wantedOfsX = -1;
	
		int nInserted;
		int nLines = m_pText->lines();
	
		int ofs = m_pText->lineColToOffset( m_line, m_column );
	
		if( m_bInsert )
			nInserted = m_pText->insert( ofs, seq );
		else
			nInserted = m_pText->replace( ofs, seq.length(), seq );
	
		nLines = m_pText->lines() - nLines;
	
		_updateLocation(m_line + nLines, m_column + nInserted);
	
		return nInserted;
	}
	
	//______________________________________________________________
	void CaretInstance::unputText( int nChar )
	{
		if(nChar < 0)
			return;
	
		// TODO: FIX!
		for(; nChar; nChar--)
			delPrevChar();
	}
	
	//_____________________________________________________________________
	void CaretInstance::delPrevWord()
	{
		int	ofs1 = m_pText->lineColToOffset( m_line, m_column );
		gotoPrevWord();
		int ofs2 = m_pText->lineColToOffset( m_line, m_column );
		m_pText->remove( ofs2, ofs1-ofs2 );
	}
	
	//_____________________________________________________________________
	void CaretInstance::delNextWord()
	{
		int	ofs1 = m_pText->lineColToOffset( m_line, m_column );
		int line = m_line;
		int column = m_column;
		gotoNextWord();
		int ofs2 = m_pText->lineColToOffset( m_line, m_column );
		m_pText->remove(ofs1, ofs2-ofs1);
		_updateLocation(line, column);
	}
	
	//____ delPrevChar() __________________________________________________________
	
	bool CaretInstance::delPrevChar()
	{
		m_wantedOfsX = -1;
	
		int column= m_column;
		int line = m_line;
	
		if( column == 0 )
		{
			if( line == 0 )
				return false;
	
			line -= 1;
			column = m_pText->getLine(line)->nChars;
		}
		else
			column--;
	
	
		int ret = m_pText->deleteChar( m_pText->lineColToOffset( m_line, m_column ) -1 );
		_updateLocation(line, column);
		return ret==1?true:false;
	}
	
	//____ delNextChar() __________________________________________________________
	
	bool CaretInstance::delNextChar()
	{
		m_wantedOfsX = -1;
	
		int ofs = m_pText->lineColToOffset( m_line, m_column );
		int nDel = m_pText->deleteChar( ofs );
	
		if( nDel )
			return true;
		else
			return false;
	}
	
	//____ ofsX() __________________________________________________________________
	
	int CaretInstance::ofsX() const
	{
		int	ln = m_line;
		int	col = m_column;
		m_pText->posHard2Soft( ln, col );
	
		return m_pText->getSoftLineWidthPart( ln, 0, col );
	}
	
	//____ ofsY() __________________________________________________________________
	
	int CaretInstance::ofsY() const
	{
		//TODO: Need a better way...
	
		int ofs = 0;
	
		for( int i = 0 ; i < (int) m_line ; i++ )
			ofs += m_pText->softLineSpacing(i);
	
		return ofs;
	}
	
	//____ mode() __________________________________________________________________
	
	Caret::Mode CaretInstance::cursorMode() const
	{
		if( m_line >= m_pText->lines() || m_pText->getLine(m_line)->nChars == m_column )
			return Caret::EOL;
	
		if( m_bInsert )
			return Caret::INS;
	
		return Caret::OVR;
	}
	
	//____ goBOL() ________________________________________________________________
	
	void CaretInstance::goBOL()
	{
		int	ln = m_line;
		int	col = m_column;
		m_pText->posHard2Soft( ln, col );
	
		col = 0;
		m_pText->posSoft2Hard( ln, col );
		gotoHardPos( ln, col );
	
		m_wantedOfsX = -1;
	}
	
	//____ goEOL() ________________________________________________________________
	
	void CaretInstance::goEOL()
	{
		int	ln = m_line;
		int	col = m_column;
		m_pText->posHard2Soft( ln, col );
	
		col = INT_MAX;
	
		m_pText->posSoft2Hard( ln, col );
		gotoHardPos( ln, col );
	
		m_wantedOfsX = -1;
	}
	
	//////////////////////////////////////////////////////////////////////////
	void CaretInstance::_updateLocation(int line, int column)
	{
		m_line = line;
		m_column = column;
	
		if(m_bSelectMode)
		{
			m_pText->selectText(m_selStartLine, m_selStartColumn, line, column);
		}
		else
		{
			clearSelection();
		}
	}
	
	//////////////////////////////////////////////////////////////////////////
	void CaretInstance::setSelectionMode(bool bOn)
	{
		m_bSelectMode = bOn;
	}
	
	bool CaretInstance::hasSelection()
	{
		if(m_selStartLine < 0)
			return false;
	
		return m_selStartLine != m_line || (m_selStartLine == m_line && m_selStartColumn != m_column);
	}
	
	void CaretInstance::delSelection()
	{
		if(!hasSelection())
			return;
	
		m_wantedOfsX = -1;
	
		int line = m_line;
		int column = m_column;
		if(m_selStartLine < (int)m_line || (m_selStartLine == (int)m_line && m_selStartColumn < (int)m_column))
		{
			std::swap(line, m_selStartLine);
			std::swap(column, m_selStartColumn);
		}
	
		int ofs1 = m_pText->lineColToOffset(line, column);
		int ofs2 = m_pText->lineColToOffset(m_selStartLine, m_selStartColumn );
		m_pText->remove(ofs1, ofs2-ofs1);
	
		m_selStartLine = line;
		m_selStartColumn = column;
	
		_updateLocation(line, column);
		clearSelection();
	}
	
	void CaretInstance::clearSelection()
	{
		m_pText->clearSelection();
		m_selStartLine = m_line;
		m_selStartColumn = m_column;
	}
	
	void CaretInstance::selectRange( Range range )
	{
		TextPos beg = m_pText->ofsToPos( range.ofs );
		TextPos end = m_pText->ofsToPos( range.ofs + range.len );
	
		m_pText->clearSelection();
		setSelectionMode(true);
		m_selStartLine = beg.line;
		m_selStartColumn = beg.col;
		gotoHardPos( end.line, end.col );
		setSelectionMode(false);
	}
	
	void CaretInstance::selectAll()
	{
		m_pText->clearSelection();
		setSelectionMode(true);
		m_selStartLine = 0;
		m_selStartColumn = 0;
		gotoHardPos(m_pText->lines(), INT_MAX);
		setSelectionMode(false);
	}

} // namespace wg
