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

#include <wg_stdtextmapper.h>
#include <wg_textstyle.h>
#include <wg_gfxdevice.h>
#include <wg_char.h>
#include <wg_msgrouter.h>
#include <wg_base.h>

#include <stdlib.h>
#include <algorithm>

namespace wg
{
	using namespace Util;

	const TypeInfo StdTextMapper::TYPEINFO = { "StdTextMapper", &TextMapper::TYPEINFO };

	//____ constructor _____________________________________________________________

	StdTextMapper::StdTextMapper() : m_alignment(Origo::NorthWest), m_bLineWrap(false), m_selectionBackColor(Color::White), m_selectionBackRenderMode(BlendMode::Invert),
		m_selectionCharColor(Color::White), m_selectionCharBlend(BlendMode::Invert), m_pFocusedText(nullptr), m_tickRouteId(0)
	{
	}

	//____ Destructor ______________________________________________________________

	StdTextMapper::~StdTextMapper()
	{
		if( m_tickRouteId )
		{
			Base::msgRouter()->deleteRoute( m_tickRouteId );
			m_tickRouteId = 0;
		}
	}


	//____ typeInfo() _________________________________________________________

	const TypeInfo& StdTextMapper::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ addText() _________________________________________________________

	void StdTextMapper::addText( Text * pText )
	{
		const Char * pChars = _chars(pText);
		int nLines = _countLines( pText, pChars );

		_setTextDataBlock(pText,0);					// Make sure pointer is null for the realloc call.
		void * pBlock = _reallocBlock(pText,nLines);

		_updateLineInfo( pText, pBlock, pChars );
	}

	//____ removeText() _________________________________________________________

	void StdTextMapper::removeText( Text * pText )
	{
		free( _dataBlock(pText) );
		_setTextDataBlock(pText, 0);

		if( pText == m_pFocusedText )
		{
			m_pFocusedText = 0;
			Base::msgRouter()->deleteRoute( m_tickRouteId );
			m_tickRouteId = 0;
		}
	}

	//____ setAlignment() __________________________________________________________

	void StdTextMapper::setAlignment( Origo alignment )
	{
		if( alignment != m_alignment )
		{
			m_alignment = alignment;

			//TODO: Make all Texts dirty
		}
	}

	//____ setLineWrap() __________________________________________________________

	void StdTextMapper::setLineWrap(bool bWrap)
	{
		if (bWrap != m_bLineWrap)
		{
			m_bLineWrap = bWrap;

			//TODO: Make all Texts dirty
		}
	}


	//____ setCaret() __________________________________________________________

	void StdTextMapper::setCaret( Caret * pCaret )
	{
		if( m_pCaret.rawPtr() != pCaret )
		{
			m_pCaret = pCaret;

			//TODO: Force update of field sizes etc if needed.
		}
	}

	//____ setSelectionBackColor() _________________________________________________

	void StdTextMapper::setSelectionBack(Color color, BlendMode renderMode )
	{
		m_selectionBackColor = color;
		m_selectionBackRenderMode = renderMode;
	}

	//____ setSelectionCharColor() ____________________________________________

	void StdTextMapper::setSelectionCharColor(Color color, BlendMode blend)
	{
		m_selectionCharColor = color;
		m_selectionCharBlend = blend;
	}

	//____ charAtPos() _________________________________________________________

	int StdTextMapper::charAtPos( const Text * pText, Coord pos ) const
	{
		int line = _lineAtPosY(pText, pos.y, SelectMode::Marked );
		if( line == -1 )
			return -1;

		return  _charAtPosX(pText, line, pos.x, SelectMode::Marked );
	}

	//_____ charPos() ______________________________________________________

	Coord StdTextMapper::charPos( const Text * pText, int charOfs ) const
	{
		int line = charLine(pText, charOfs);

		MU ofsX = _charPosX(pText, charOfs);
		MU ofsY = _linePosY(_dataBlock(pText), line, _size(pText).h );

		const LineInfo * pLine = _lineInfo( _dataBlock(pText) ) + line;
		ofsY += pLine->base;

		return Coord(ofsX,ofsY);
	}

	//____ charRect() ________________________________________________________

	Rect StdTextMapper::charRect( const Text * pText, int charOfs ) const
	{
		const void * pBlock = _dataBlock(pText);
		const BlockHeader * pHeader = _header(pBlock);
		const LineInfo * pLineInfo = _lineInfo(pBlock);

		// Find correct line and determine yOfs

		MU yOfs = _textPosY( pHeader, _size(pText).h );
		while( pLineInfo->length <= charOfs )
		{
			yOfs += pLineInfo->spacing;
			charOfs-= pLineInfo->length;
			pLineInfo++;
		}

		// Determine xOfs by parsing line until character

		MU xOfs = _linePosX( pLineInfo, _size(pText).w );

		TextAttr		baseAttr;
		_baseStyle(pText)->exportAttr( _state(pText), &baseAttr );

		const Char * pFirst = _chars(pText) + pLineInfo->offset;
		const Char * pLast = pFirst + charOfs;

		xOfs += _charDistance( pFirst, pLast, baseAttr, _state(pText) );

		// Get cell width

		MU width = 0;

		TextAttr	attr = baseAttr;

		if( pLast->styleHandle() != 0 )
			pLast->stylePtr()->addToAttr( _state(pText), &attr );

		Font * pFont = attr.pFont.rawPtr();
		pFont->setSize(attr.size);

		Glyph_p pGlyph = _getGlyph( pFont, pLast->code() );
		if( pGlyph )
			width = pGlyph->advance();				// Do not advance for last, just apply kerning.
		else if( pLast->code() == 32 )
			width = pFont->whitespaceAdvance();

		return Rect(xOfs,yOfs,width,pLineInfo->height);
	}

	//____ charLine() ________________________________________________________

	int StdTextMapper::charLine( const Text * pText, int charOfs ) const
	{
		if( charOfs < 0 )
			return -1;

		const void * pBlock = _dataBlock(pText);
		const BlockHeader * pHeader = _header(pBlock);
		const LineInfo * pLineInfo = _lineInfo(pBlock);

		for( int i = 0 ; i < pHeader->nbLines ; i++ )
		{
			if( charOfs < pLineInfo[i].offset + pLineInfo[i].length )
				return i;
		}

		return -1;
	}

	//____ lineBegin() ________________________________________________________

	int StdTextMapper::lineBegin( const Text * pText, int lineNb ) const
	{
		const void * pBlock = _dataBlock(pText);
		const BlockHeader * pHeader = _header(pBlock);
		const LineInfo * pLineInfo = _lineInfo(pBlock);

		if( lineNb < 0 || lineNb >= pHeader->nbLines )
			return -1;

		return pLineInfo[lineNb].offset;
	}

	//____ lineEnd() ___________________________________________________________

	int StdTextMapper::lineEnd( const Text * pText, int lineNb ) const
	{
		const void * pBlock = _dataBlock(pText);
		const BlockHeader * pHeader = _header(pBlock);
		const LineInfo * pLineInfo = _lineInfo(pBlock);

		if( lineNb < 0 || lineNb >= pHeader->nbLines )
			return -1;

		return pLineInfo[lineNb].offset + pLineInfo[lineNb].length;
	}

	//____ wordBegin() _________________________________________________________

	int StdTextMapper::wordBegin( const Text * pText, int charOfs ) const
	{
		//TODO: Implement!
		return charOfs;
	}

	//____ wordEnd() ___________________________________________________________

	int StdTextMapper::wordEnd( const Text * pText, int charOfs ) const
	{
		//TODO: Implement!
		return charOfs+1;
	}


	//____ _charDistance() _____________________________________________________

	// Returns distance in points between beginning of first and beginning of last char.
	// Chars should be on the same line (or pLast could be first char on next line)

	MU StdTextMapper::_charDistance( const Char * pFirst, const Char * pLast, const TextAttr& baseAttr, State state ) const
	{
		TextAttr		attr;
		Font_p 			pFont;
		TextStyle_h		hStyle = 0xFFFF;

		Glyph_p	pGlyph;
		Glyph_p	pPrevGlyph =  0;
		const Char * pChar = pFirst;

		MU distance = 0;

		while( pChar <= pLast )
		{
			// TODO: Include handling of special characters
			// TODO: Support char textcolor and background color and effects.

			if( pChar->styleHandle() != hStyle )
			{
				int oldFontSize = attr.size;
				attr = baseAttr;

				if( pChar->styleHandle() != 0 )
					pChar->stylePtr()->addToAttr( state, &attr );

				if( pFont != attr.pFont || attr.size != oldFontSize )
				{
					pFont = attr.pFont;
					pFont->setSize(attr.size);
					pPrevGlyph = 0;								// No kerning against across different fonts or characters of different size.
				}
			}

			pGlyph = _getGlyph( pFont.rawPtr(), pChar->code() );

			if( pGlyph )
			{
				if( pPrevGlyph )
					distance += pFont->kerning(pPrevGlyph, pGlyph);

				if( pChar != pLast )
					distance += pGlyph->advance();				// Do not advance for last, just apply kerning.
			}
			else if( pChar->code() == 32 && pChar != pLast )
				distance += pFont->whitespaceAdvance();

			pPrevGlyph = pGlyph;
			pChar++;
		}

		return distance;
	}

	//____ receive() ___________________________________________________________

	void StdTextMapper::receive( Msg * pMsg )
	{
		if( pMsg->type() == MsgType::Tick && m_pFocusedText )
		{
			if( _caretVisible(m_pFocusedText) )
			{				
				Caret * pCaret = m_pCaret ? m_pCaret : Base::defaultCaret();
				if( pCaret )
				{
					int ms = static_cast<TickMsg*>(pMsg)->timediff();

					bool bDirty = pCaret->tick( ms );
					if( bDirty )
					{
						int caretOfs = _caretOfs(m_pFocusedText);
						_setTextDirty( m_pFocusedText, pCaret->dirtyRect(charRect(m_pFocusedText, caretOfs)) );
					}
				}
			}
		}
	}

	//____ _render()___________________________________________________________

	void StdTextMapper::render( Text * pText, GfxDevice * pDevice, const Rect& canvas )
	{

		void * pBlock = _dataBlock(pText);
		BlockHeader * pHeader = _header(pBlock);
		LineInfo * pLineInfo = _lineInfo(pBlock);
		const Char * pCharArray = _chars(pText);

		Coord lineStart = canvas.pos();
		lineStart.y += _textPosY( pHeader, canvas.h );

		TextAttr		baseAttr;
		_baseStyle(pText)->exportAttr( _state(pText), &baseAttr );

		TextAttr		attr;
		Font_p 			pFont;
		TextStyle_h		hStyle = 0xFFFF;

		Color	baseTint = pDevice->tintColor();
		Color	localTint = Color::White;

		BlendMode renderMode = pDevice->blendMode();

		// Limit our cliplist if needed

		ClipPopData popData;
		if (pHeader->textSize.w > canvas.w || pHeader->textSize.h > canvas.h)
			popData = limitClipList(pDevice, canvas);


		// Render back colors

		_renderBack( pText, pDevice, canvas );


		int selBeg, selEnd;
		std::tie(selBeg,selEnd) = _selection(pText);

		// Get selection start and end

		const Char * pSelBeg = nullptr;
		const Char * pSelEnd = nullptr;

		bool bInSelection = false;

		if( selBeg != selEnd )
		{
			if( m_selectionBackRenderMode != BlendMode::Ignore )
			{
				if( m_selectionBackRenderMode != BlendMode::Undefined )
					pDevice->setBlendMode( m_selectionBackRenderMode );
				_renderBackSection( pText, pDevice, canvas, selBeg, selEnd, m_selectionBackColor );
				pDevice->setBlendMode( renderMode );
			}

			pSelBeg = pCharArray + selBeg;
			pSelEnd = pCharArray + selEnd;
		}


		//

		const Rect& clip = Rect::fromPX(pDevice->clipBounds());

		for( int i = 0 ; i < pHeader->nbLines ; i++ )
		{
			if( lineStart.y < clip.y + clip.h && lineStart.y + pLineInfo->height > clip.y )
			{
				lineStart.x = canvas.x + _linePosX( pLineInfo, canvas.w );
				const Char * pChar = pCharArray + pLineInfo->offset;

				Glyph_p	pGlyph;
				Glyph_p	pPrevGlyph =  0;

				Coord pos = lineStart;
				pos.y += pLineInfo->base;

				bool bRecalcColor = false;

				for( int x = 0 ; x < pLineInfo->length ; x++ )
				{
					// TODO: Include handling of special characters
					// TODO: Support char textcolor and background color and effects.

					if( pChar->styleHandle() != hStyle )
					{
						MU oldFontSize = attr.size;
						Color oldBgColor = attr.bgColor;

						attr = baseAttr;

						if( pChar->styleHandle() != 0 )
							pChar->stylePtr()->addToAttr( _state(pText), &attr );

						if( pFont != attr.pFont || MU(attr.size) != oldFontSize )
						{

							if( !pFont || (pFont->isMonochrome() != attr.pFont->isMonochrome()) )
								bRecalcColor = true;		// Font tint-color is changed.

							pFont = attr.pFont;
							pFont->setSize(attr.size);
							pPrevGlyph = 0;								// No kerning against across different fonts or character of different size.
						}

						if( attr.color != localTint )
						{
							localTint = attr.color;
							bRecalcColor = true;
						}
					}

					//

					if( pChar == pSelBeg )
					{
						bInSelection = true;
						bRecalcColor = true;
					}
					else if( pChar == pSelEnd )
					{
						bInSelection = false;
						bRecalcColor = true;
					}

					if( bRecalcColor )
					{
						if (bInSelection)
							pDevice->setTintColor(baseTint * Color::blend(localTint, m_selectionCharColor, m_selectionCharBlend));
						else
							pDevice->setTintColor( baseTint * localTint );

						bRecalcColor = false;
					}

					//

					pGlyph = _getGlyph( pFont.rawPtr(), pChar->code());

					if( pGlyph )
					{
						if( pPrevGlyph )
							pos.x += pFont->kerning(pPrevGlyph, pGlyph);

						const GlyphBitmap * pBitmap = pGlyph->getBitmap();
						pDevice->setBlitSource(pBitmap->pSurface);
						pDevice->blit( Coord(pos.x + pBitmap->bearingX, pos.y + pBitmap->bearingY).px(), pBitmap->rect  );

						pos.x += pGlyph->advance();
					}
					else if( pChar->code() == 32 )
						pos.x += pFont->whitespaceAdvance();

					pPrevGlyph = pGlyph;
					pChar++;
				}
			}

			lineStart.y += pLineInfo->spacing;
			pLineInfo++;
		}

		pDevice->setTintColor( baseTint );


		// Render caret (if there is any)

		if( _caretVisible(pText) )
		{			
			Caret * pCaret = m_pCaret ? m_pCaret : Base::defaultCaret();

			if( pCaret )
			{
				pCaret->render( pDevice, charRect(pText, _caretOfs(pText)) + canvas.pos() );
			}
		}
		// Pop any changes to cliplist.

		popClipList(pDevice, popData);
	}




	//____ _renderBack()___________________________________________________________

	void StdTextMapper::_renderBack( Text * pText, GfxDevice * pDevice, const Rect& canvas )
	{
		const Char * pCharArray = _chars(pText);
		const Char * pBeg = pCharArray;
		const Char * pChar;

		TextStyle_h hStyle = 0xFFFF;

		Color		color = Color::Transparent;

		for( pChar = pCharArray ; !pChar->isEndOfText() ; pChar++ )
		{
			if( pChar->styleHandle() != hStyle )
			{
				State state = _state(pText);

				Color newColor;

				TextStyle_p p = pChar->stylePtr();
				if( p && p->isBgColorDefined(state) )
					newColor = p->bgColor(state);
				else
					newColor = _baseStyle(pText)->bgColor(state);

				if( newColor != color )
				{
					if( color.a != 0 )
						_renderBackSection( pText, pDevice, canvas, int(pBeg - pCharArray), int(pChar - pCharArray), color );
					color = newColor;
					pBeg = pChar;
				}

				hStyle = pChar->styleHandle();
			}
		}

		if( color.a != 0 )
			_renderBackSection( pText, pDevice, canvas, int(pBeg - pCharArray), int(pChar - pCharArray), color );
	}

	//____ _renderBackSection() ________________________________________________

	void StdTextMapper::_renderBackSection( Text * pText, GfxDevice * pDevice, const Rect& canvas,
											int begChar, int endChar, Color color )
	{

		Coord begPos = charPos( pText, begChar );
		LineInfo * pBegLine = _lineInfo( _dataBlock(pText) ) + charLine( pText, begChar );

		Coord endPos = charPos( pText, endChar );
		LineInfo * pEndLine = _lineInfo( _dataBlock(pText) ) + charLine( pText, endChar );


		if( pBegLine == pEndLine )
		{
			Rect area;
			area.x = canvas.x + begPos.x;
			area.y = canvas.y + begPos.y - pBegLine->base;
			area.w = endPos.x - begPos.x;
			area.h = pBegLine->height;

			pDevice->fill( area.px(), color );
		}
		else
		{
			LineInfo * pLine = pBegLine;

			Rect area;
			area.x = canvas.x + begPos.x;
			area.y = canvas.y + begPos.y - pLine->base;
			area.w = pLine->width - (begPos.x - _linePosX( pLine, canvas.w));
			area.h = pLine->height;

			pDevice->fill( area.px(), color );

			area.y += pLine->spacing;
			pLine++;

			while( pLine != pEndLine )
			{
				area.x = canvas.x + _linePosX( pLine, canvas.w);
				area.w = pLine->width;
				area.h = pLine->height;

				pDevice->fill( area.px(), color );

				area.y += pLine->spacing;
				pLine++;
			}


			area.x = canvas.x + _linePosX( pLine, canvas.w);
			area.w = endPos.x - area.x;
			area.h = pLine->height;

			pDevice->fill( area.px(), color );
		}

	}

	//____ caretMove() ________________________________________________________

	void StdTextMapper::caretMove( Text * pText, int newOfs, int oldOfs )
	{
		Caret * pCaret = m_pCaret ? m_pCaret : Base::defaultCaret();
		if( _caretVisible(pText) && pCaret )
		{
			bool bDirty = pCaret->restartCycle();

			if( bDirty || oldOfs != newOfs )
			{
				_setTextDirty( pText, pCaret->dirtyRect( charRect(pText, newOfs) ));

				if( oldOfs != newOfs )
					_setTextDirty( pText, pCaret->dirtyRect( charRect(pText, oldOfs)) );
			}
		}
	}

	//____ selectionChange() __________________________________________________

	void StdTextMapper::selectionChange( Text * pText, int selectOfs, int caretOfs, int oldSelectOfs, int oldCaretOfs )
	{
		int oldBeg = std::min(oldSelectOfs,oldCaretOfs);
		int oldEnd = std::max(oldSelectOfs,oldCaretOfs);

		int newBeg = std::min(selectOfs,caretOfs);
		int newEnd = std::max(selectOfs,caretOfs);

		// Two cases here, either new and old selection overlaps or they are completely disjointed
		
		if( (newBeg < oldBeg && newEnd > oldBeg) || (newBeg > oldBeg && oldEnd > newBeg) )
		{
			//  Selection is overlapping, we just need to modify it at the edges
			
			if( oldBeg != newBeg )
			{
				int beg = std::min(oldBeg,newBeg);
				int len = std::max(oldBeg,newBeg) - beg;

				_setTextDirty( pText, rectForRange( pText, beg, len ) );
			}

			if( oldEnd != newEnd )
			{
				int beg = std::min(oldEnd,newEnd);
				int len = std::max(oldEnd,newEnd) - beg;

				_setTextDirty( pText, rectForRange( pText, beg, len ) );
			}			
		}
		else
		{
			// Selection is not overlapping, we have two dirty areas
			
			if( oldEnd != oldBeg )
				_setTextDirty( pText, rectForRange( pText, oldBeg, oldEnd - oldBeg ));
			if( newEnd != oldEnd > 0 )
				_setTextDirty( pText, rectForRange( pText, newBeg, newEnd - newBeg ));
		}
		
		// Update/redraw the caret
		
		caretMove( pText, caretOfs, oldCaretOfs );		
	}

	//____ onTextModified() ____________________________________________________

	void StdTextMapper::onTextModified( Text * pText, int ofs, int charsRemoved, int charsAdded )
	{
		onRefresh(pText);
	}

	//____ onResized() ___________________________________________________________

	void StdTextMapper::onResized( Text * pText, Size newSize, Size oldSize )
	{
		if (m_bLineWrap)
			onRefresh(pText);


		///TODO: Implement!
	}

	//____ onStateChanged() ______________________________________________________

	void StdTextMapper::onStateChanged( Text * pText, State newState, State oldState )
	{
		// TODO: Support for more than one input device, focusing different (or same) texts.

		if( newState.isFocused() != oldState.isFocused() )
		{
			if( newState.isFocused() )
			{
				m_pFocusedText = pText;
				if( !m_tickRouteId )
					m_tickRouteId = Base::msgRouter()->addRoute( MsgType::Tick, this );
			}
			else
			{
				m_pFocusedText = 0;
				if( m_tickRouteId )
				{
					Base::msgRouter()->deleteRoute( m_tickRouteId );
					m_tickRouteId = 0;
				}
			}
		}
	}

	//____ onStyleChanged() ______________________________________________________

	void StdTextMapper::onStyleChanged( Text * pText, TextStyle * pNewStyle, TextStyle * pOldStyle )
	{
		//TODO: Optimize: only update line info if textsize possibly affected.

		void * pBlock = _dataBlock(pText);

		_updateLineInfo( pText, pBlock, _chars(pText) );

		_setTextDirty(pText);
	}

	//____ onCharStyleChanged() __________________________________________________

	void StdTextMapper::onCharStyleChanged( Text * pText, int ofs, int len )
	{
		void * pBlock = _dataBlock(pText);

		_updateLineInfo( pText, pBlock, _chars(pText) );

		_setTextDirty(pText);
	}

	//____ onRefresh() ___________________________________________________________

	void StdTextMapper::onRefresh( Text * pText )
	{
		const Char * pChars = _chars(pText);
		int nLines = _countLines( pText, pChars );

		void * pBlock = _dataBlock(pText);
		if( !pBlock || _header(pBlock)->nbLines != nLines )
			pBlock = _reallocBlock(pText,nLines);

		_updateLineInfo( pText, pBlock, pChars );
		_setTextDirty(pText);
	}

	//___ rectForRange() _________________________________________________________

	Rect StdTextMapper::rectForRange( const Text * pText, int ofs, int length ) const
	{
		int begChar = ofs;
		int endChar = ofs + length;

		Size canvas = _size(pText);


		Coord begPos = charPos(pText, begChar);
		const LineInfo * pBegLine = _lineInfo(_dataBlock(pText)) + charLine(pText, begChar);

		Coord endPos = charPos(pText, endChar);
		const LineInfo * pEndLine = _lineInfo(_dataBlock(pText)) + charLine(pText, endChar);

		MU x1, x2, y1, y2;

		if (pBegLine == pEndLine)
		{
			x1 = begPos.x;
			y1 = begPos.y - pBegLine->base;
			x2 = endPos.x;
			y2 = y1 + pBegLine->height;
		}
		else
		{
			const LineInfo * pLine = pBegLine;

			x1 = begPos.x;
			y1 = begPos.y - pLine->base;
			x2 = pLine->width + _linePosX(pLine, canvas.w);
			y2 = y1 + pLine->height;

			pLine++;

			while (pLine != pEndLine)
			{
				MU nx1 = _linePosX(pLine, canvas.w);
				MU nx2 = nx1 + pLine->width;

				if (nx1 < x1)
					x1 = nx1;
				if (nx2 > x2)
					x2 = nx2;

				y2 += pLine->spacing;
				pLine++;
			}

			MU nx1 = _linePosX(pLine, canvas.w);
			MU nx2 = endPos.x;

			if (nx1 < x1)
				x1 = nx1;
			if (nx2 > x2)
				x2 = nx2;

			y2 += pLine->height;
		}

		return Rect( x1, y1, x2-x1, y2-y1);
	}

	//____ rectForCaret() ______________________________________________________

	// Includes left/right margin where applicable.

	Rect StdTextMapper::rectForCaret( const Text * pText ) const
	{
		Caret * pCaret = m_pCaret ? m_pCaret : Base::defaultCaret();
		if( !_caretVisible(pText) || !pCaret )
			return Rect();

		return pCaret->dirtyRect( charRect(pText, _caretOfs(pText) ) );
	}


	//____ textDirection() ____________________________________________________

	Direction StdTextMapper::textDirection( Text * pText, int charOfs ) const
	{
		return Direction::Right;
	}

	//____ caretToPos() _____________________________________________________

	int StdTextMapper::caretToPos( Text * pText, Coord pos, MU& wantedLineOfs ) const
	{
		wantedLineOfs = -1;

		int line = _lineAtPosY(pText, pos.y, SelectMode::Closest );
		return  _charAtPosX(pText, line, pos.x, SelectMode::ClosestBegin );
	}

	//____ caretUp() ___________________________________________________________

	int StdTextMapper::caretUp( Text * pText, int charOfs, MU& wantedLineOfs ) const
	{
		int line = charLine(pText, charOfs );

		if( line > 0 )
		{
			if( wantedLineOfs == -1 )
				wantedLineOfs = _charPosX( pText, charOfs );

			charOfs = _charAtPosX(pText, line-1, wantedLineOfs, SelectMode::ClosestBegin );
		}
		return charOfs;
	}

	//____ caretDown() _________________________________________________________

	int StdTextMapper::caretDown( Text * pText, int charOfs, MU& wantedLineOfs ) const
	{
		int line = charLine(pText, charOfs );

		if( line >= 0 && line < _header(_dataBlock(pText))->nbLines-1 )
		{
			if( wantedLineOfs == -1 )
				wantedLineOfs = _charPosX( pText, charOfs );

			charOfs = _charAtPosX(pText, line+1, wantedLineOfs, SelectMode::ClosestBegin );
		}
		return charOfs;
	}

	//____ caretLeft() _________________________________________________________

	int StdTextMapper::caretLeft( Text * pText, int charOfs, MU& wantedLineOfs ) const
	{
		if( charOfs > 0 )
			charOfs--;

		wantedLineOfs = -1;
		return charOfs;
	}

	//____ caretRight() ________________________________________________________

	int StdTextMapper::caretRight( Text * pText, int charOfs, MU& wantedLineOfs ) const
	{
		if( charOfs < _length(pText) )
			charOfs++;

		wantedLineOfs = -1;
		return charOfs;
	}

	//____ caretHome() ________________________________________________________

	int StdTextMapper::caretHome( Text * pText, int charOfs, MU& wantedLineOfs ) const
	{
		int line = charLine( pText, charOfs );

		if( line >= 0 )
		{
			const LineInfo * pLine = _lineInfo( _dataBlock(pText) ) + line;
			charOfs = pLine->offset;
		}
		wantedLineOfs = -1;
		return charOfs;
	}

	//____ caretEnd() ________________________________________________________

	int StdTextMapper::caretEnd( Text * pText, int charOfs, MU& wantedLineOfs ) const
	{
		int line = charLine( pText, charOfs );

		if( line >= 0 )
		{
			const LineInfo * pLine = _lineInfo( _dataBlock(pText) ) + line;
			charOfs = pLine->offset+ pLine->length-1;
		}
		wantedLineOfs = -1;
		return charOfs;
	}

	//____ caretPrevWord() _____________________________________________________

	int StdTextMapper::caretPrevWord( Text * pText, int charOfs ) const
	{
		//TODO: Implement!
		return charOfs;
	}

	//____ caretNextWord() _____________________________________________________

	int StdTextMapper::caretNextWord( Text * pText, int charOfs ) const
	{
		//TODO: Implement!
		return charOfs;
	}


	//____ tooltip() _______________________________________________________________

	String StdTextMapper::tooltip( const Text * pText ) const
	{
		//TODO: Return the text if it overflows the mapped area?

		return String();
	}


	//____ preferredSize() _________________________________________________________

	Size StdTextMapper::preferredSize( const Text * pText ) const
	{
		return _header(_dataBlock(pText))->preferredSize;
	}

	//____ matchingWidth() _________________________________________________________

	MU StdTextMapper::matchingWidth( const Text * pText, MU height ) const
	{
		return	_header(_dataBlock(pText))->preferredSize.w;
	}

	//____ matchingHeight() ________________________________________________________

	MU StdTextMapper::matchingHeight( const Text * pText, MU width ) const
	{
		return _calcMatchingHeight(_chars(pText), _baseStyle(pText), _state(pText), width);
	}

	//____ _countLines() ___________________________________________________________

	int StdTextMapper::_countLines( Text * pText, const Char * pChars ) const
	{
		if (m_bLineWrap)
			return _countWrapLines(pChars, _baseStyle(pText), _state(pText), _size(pText).w);
		else
			return _countFixedLines(pChars);
	}

	//____ _countFixedLines() ___________________________________________________________

	int StdTextMapper::_countFixedLines(const Char * pChars) const
	{
		int lines = 0;
		while (true)
		{
			if (pChars->isEndOfLine())
			{
				lines++;
				if (pChars->isEndOfText())
					return lines;
			}
			pChars++;
		}
	}


	//____ _countWrapLines() ________________________________________________

	int StdTextMapper::_countWrapLines(const Char * pChars, const TextStyle * pBaseStyle, State state, MU maxLineWidth) const
	{
		TextAttr		baseAttr;
		pBaseStyle->exportAttr(state, &baseAttr);


		TextAttr		attr;
		Font_p 			pFont;

		TextStyle_h		hCharStyle = 0xFFFF;			// Force change on first character.

		Glyph_p	pGlyph = nullptr;
		Glyph_p	pPrevGlyph = nullptr;

		MU spaceAdv = 0;
		MU width = 0;
		MU potentialWidth = 0;
		MU eolCaretWidth = 0;
		//

		const Char * pBreakpoint = nullptr;
		MU bpWidth = 0;

		int nLines = 0;												//

		while (true)
		{

			if (pChars->styleHandle() != hCharStyle)
			{

				MU oldFontSize = attr.size;

				attr = baseAttr;
				if (pChars->styleHandle() != 0)
					pChars->stylePtr()->addToAttr(state, &attr);

				if (pFont != attr.pFont || MU(attr.size) != oldFontSize)
				{
					pFont = attr.pFont;
					pFont->setSize(attr.size);
					pPrevGlyph = 0;								// No kerning against across different fonts or fontsizes.
				}

				spaceAdv = pFont->whitespaceAdvance();

				Caret * pCaret = m_pCaret ? m_pCaret : Base::defaultCaret();
				if (pCaret)
				{
					Size eolCellSize(pGlyph ? pGlyph->advance() : MU(), pFont->maxAscend() + pFont->maxDescend());
					eolCaretWidth = pCaret->eolWidth(eolCellSize);
				}
				else
					eolCaretWidth = 0;

			}

			// TODO: Include handling of special characters
			// TODO: Support sub/superscript.

			pGlyph = _getGlyph(pFont.rawPtr(), pChars->code());

			if (pGlyph)
			{
				if (pPrevGlyph)
					width += pFont->kerning(pPrevGlyph, pGlyph);

				potentialWidth += pGlyph->advance();
				width = potentialWidth;
			}
			else if (pChars->code() == 32)
				potentialWidth += spaceAdv;

			pPrevGlyph = pGlyph;

			// Handle end of line

			if (pChars->isEndOfLine())
			{
				nLines++;

				//

				if (pChars->isEndOfText())
					break;

				// Prepare for next line

				pChars++;			// Line terminator belongs to previous line.

				width = 0;
				potentialWidth = 0;
				pBreakpoint = nullptr;
				pPrevGlyph = nullptr;
			}
			else
			{
				if (width > maxLineWidth - eolCaretWidth && pBreakpoint != nullptr)
				{
					bpWidth += eolCaretWidth;

					nLines++;

					// Prepare for next line

					pChars = pBreakpoint;

					width = 0;
					potentialWidth = 0;
					pBreakpoint = nullptr;
					pPrevGlyph = nullptr;

				}
				else
				{
					int code = pChars->code();
					if (code == 32)
					{
						pBreakpoint = pChars + 1;
						bpWidth = width;
					}

					pChars++;
				}
			}
		}
		return nLines;
	}

	//____ _calcMatchingHeight() ________________________________________________

	MU StdTextMapper::_calcMatchingHeight(const Char * pChars, const TextStyle * pBaseStyle, State state, MU maxLineWidth) const
	{
		Caret * pCaret = m_pCaret ? m_pCaret : Base::defaultCaret();

		TextAttr		baseAttr;
		pBaseStyle->exportAttr(state, &baseAttr);

		MU totalHeight = 0;

		TextAttr		attr;
		Font_p 			pFont;

		TextStyle_h		hCharStyle = 0xFFFF;			// Force change on first character.

		Glyph_p	pGlyph = nullptr;
		Glyph_p	pPrevGlyph = nullptr;

		MU maxAscend = 0;
		MU maxDescend = 0;
		MU maxDescendGap = 0;							// Including the line gap.
		MU spaceAdv = 0;
		MU width = 0;
		MU potentialWidth = 0;
		MU eolCaretWidth = 0;
		//

		const Char * pBreakpoint = nullptr;
		MU bpWidth = 0;
		MU bpMaxAscend = 0;
		MU bpMaxDescend = 0;
		MU bpMaxDescendGap = 0;							// Including the line gap.

															//
		while (true)
		{

			if (pChars->styleHandle() != hCharStyle)
			{

				MU oldFontSize = attr.size;

				attr = baseAttr;
				if (pChars->styleHandle() != 0)
					pChars->stylePtr()->addToAttr(state, &attr);

				if (pFont != attr.pFont || MU(attr.size) != oldFontSize)
				{
					pFont = attr.pFont;
					pFont->setSize(attr.size);
					pPrevGlyph = 0;								// No kerning against across different fonts or fontsizes.
				}

				MU ascend = pFont->maxAscend();
				if (ascend > maxAscend)
					maxAscend = ascend;

				MU descend = pFont->maxDescend();
				if (descend > maxDescend)
					maxDescend = descend;

				MU descendGap = descend + pFont->lineGap();
				if (descendGap > maxDescendGap)
					maxDescendGap = descendGap;

				spaceAdv = pFont->whitespaceAdvance();

				if (pCaret)
				{
					Size eolCellSize(pGlyph ? pGlyph->advance() : MU(), pFont->maxAscend() + pFont->maxDescend());
					eolCaretWidth = pCaret->eolWidth(eolCellSize);
				}
				else
					eolCaretWidth = 0;

				hCharStyle = pChars->styleHandle();
			}

			// TODO: Include handling of special characters
			// TODO: Support sub/superscript.

			pGlyph = _getGlyph(pFont.rawPtr(), pChars->code());

			if (pGlyph)
			{
				if (pPrevGlyph)
					width += pFont->kerning(pPrevGlyph, pGlyph);

				potentialWidth += pGlyph->advance();
				width = potentialWidth;
			}
			else if (pChars->code() == 32)
				potentialWidth += spaceAdv;

			pPrevGlyph = pGlyph;

			// Handle end of line

			if (pChars->isEndOfLine())
			{
				// Make sure we have space for eol caret

				if (pCaret)
				{
					Size eolCellSize(pGlyph ? pGlyph->advance() : MU(), pFont->maxAscend() + pFont->maxDescend());
					MU w = pCaret->eolWidth(eolCellSize);
					if (w > eolCellSize.w)
						width += w - eolCellSize.w;
				}

				// Update totalHeight

				totalHeight += pChars->isEndOfText() ? maxAscend + maxDescend : maxAscend + maxDescendGap;

				if (pChars->isEndOfText())
					break;

				// Prepare for next line

				pChars++;			// Line terminator belongs to previous line.

				width = 0;
				potentialWidth = 0;
				pBreakpoint = nullptr;
				pPrevGlyph = nullptr;

				if (pChars->styleHandle() == hCharStyle)
				{
					// Still need to reset these, don't include data from styles only on previous line.

					maxAscend = pFont->maxAscend();
					maxDescend = pFont->maxDescend();
					maxDescendGap = maxDescend + pFont->lineGap();
				}
				else
				{
					maxAscend = 0;
					maxDescend = 0;
					maxDescendGap = 0;
				}
			}
			else
			{
				if (width > maxLineWidth - eolCaretWidth && pBreakpoint != nullptr)
				{
					bpWidth += eolCaretWidth;

					// Update totalHeight

					totalHeight += pChars->isEndOfText() ? bpMaxAscend + bpMaxDescend : bpMaxAscend + bpMaxDescendGap;

					// Prepare for next line

					pChars = pBreakpoint;

					width = 0;
					potentialWidth = 0;
					pBreakpoint = nullptr;
					pPrevGlyph = nullptr;

					if (pChars->styleHandle() == hCharStyle)
					{
						// Still need to reset these, don't include data from styles only on previous line.

						maxAscend = pFont->maxAscend();
						maxDescend = pFont->maxDescend();
						maxDescendGap = maxDescend + pFont->lineGap();
					}
					else
					{
						maxAscend = 0;
						maxDescend = 0;
						maxDescendGap = 0;
					}
				}
				else
				{
					int code = pChars->code();
					if (code == 32)
					{
						pBreakpoint = pChars + 1;
						bpWidth = width;
						bpMaxAscend = maxAscend;
						bpMaxDescend = maxDescend;
						bpMaxDescendGap = maxDescendGap;							// Including the line gap.
					}

					pChars++;
				}
			}
		}
		return totalHeight;
	}



	//____ _reallocBlock() _________________________________________________________

	void * StdTextMapper::_reallocBlock( Text* pText, int nLines )
	{
		void * pBlock = _dataBlock(pText);
		if( pBlock )
			free( pBlock );

		pBlock = malloc( sizeof(BlockHeader) + sizeof(LineInfo)*nLines);
		_setTextDataBlock(pText, pBlock);
		((BlockHeader *)pBlock)->nbLines = nLines;

		return pBlock;
	}


	//____ _updateLineInfo() _______________________________________________________

	void StdTextMapper::_updateLineInfo( Text * pText, void * pBlock, const Char * pChars )
	{
		BlockHeader * pHeader = _header(_dataBlock(pText));
		SizeI preferredSize;


		if (m_bLineWrap)
		{
			//TODO: This is slow, calling both _updateFixedLineInfo and _updateWrapLineInfo if line is wrapped, just so we can update preferredSize.

			preferredSize = _updateFixedLineInfo(_header(pBlock), _lineInfo(pBlock), pChars, _baseStyle(pText), _state(pText));
			pHeader->textSize = _updateWrapLineInfo(_header(pBlock), _lineInfo(pBlock), pChars, _baseStyle(pText), _state(pText), _size(pText).w);
		}
		else
		{
			preferredSize = _updateFixedLineInfo(_header(pBlock), _lineInfo(pBlock), pChars, _baseStyle(pText), _state(pText));
			pHeader->textSize = preferredSize;
		}


		if (preferredSize != pHeader->preferredSize)
		{
			pHeader->preferredSize = preferredSize;
			_requestTextResize(pText);
		}
	}

	//____ _updateWrapLineInfo() ________________________________________________

	Size StdTextMapper::_updateWrapLineInfo(BlockHeader * pHeader, LineInfo * pLines, const Char * pChars, const TextStyle * pBaseStyle, State state, MU maxLineWidth )
	{
		Caret * pCaret = m_pCaret ? m_pCaret : Base::defaultCaret();
		const Char * pTextStart = pChars;

		TextAttr		baseAttr;
		pBaseStyle->exportAttr(state, &baseAttr);

		Size			size;

		TextAttr		attr;
		Font_p 			pFont;

		TextStyle_h		hCharStyle = 0xFFFF;			// Force change on first character.

		Glyph_p	pGlyph = nullptr;
		Glyph_p	pPrevGlyph = nullptr;

		MU maxAscend = 0;
		MU maxDescend = 0;
		MU maxDescendGap = 0;							// Including the line gap.
		MU spaceAdv = 0;
		MU width = 0;
		MU potentialWidth = 0;
		MU eolCaretWidth = 0;
		//

		const Char * pBreakpoint = nullptr;
		MU bpWidth = 0;
		MU bpMaxAscend = 0;
		MU bpMaxDescend = 0;
		MU bpMaxDescendGap = 0;							// Including the line gap.


		pLines->offset = int(pChars - pTextStart);

		while (true)
		{

			if (pChars->styleHandle() != hCharStyle)
			{

				MU oldFontSize = attr.size;

				attr = baseAttr;
				if (pChars->styleHandle() != 0)
					pChars->stylePtr()->addToAttr(state, &attr);

				if (pFont != attr.pFont || MU(attr.size) != oldFontSize)
				{
					pFont = attr.pFont;
					pFont->setSize(attr.size);
					pPrevGlyph = 0;								// No kerning against across different fonts or fontsizes.
				}

				MU ascend = pFont->maxAscend();
				if (ascend > maxAscend)
					maxAscend = ascend;

				MU descend = pFont->maxDescend();
				if (descend > maxDescend)
					maxDescend = descend;

				MU descendGap = descend + pFont->lineGap();
				if (descendGap > maxDescendGap)
					maxDescendGap = descendGap;

				spaceAdv = pFont->whitespaceAdvance();

				if (pCaret)
				{
					Size eolCellSize(pGlyph ? pGlyph->advance() : MU(), pFont->maxAscend() + pFont->maxDescend());
					eolCaretWidth = pCaret->eolWidth(eolCellSize);
				}
				else
					eolCaretWidth = 0;

				hCharStyle = pChars->styleHandle();
			}

			// TODO: Include handling of special characters
			// TODO: Support sub/superscript.

			pGlyph = _getGlyph(pFont.rawPtr(), pChars->code());

			if (pGlyph)
			{
				if (pPrevGlyph)
					width += pFont->kerning(pPrevGlyph, pGlyph);

				potentialWidth += pGlyph->advance();
				width = potentialWidth;
			}
			else if (pChars->code() == 32)
				potentialWidth += spaceAdv;

			pPrevGlyph = pGlyph;

			// Handle end of line

			if (pChars->isEndOfLine())
			{
				// Make sure we have space for eol caret

				if (pCaret)
				{
					Size eolCellSize(pGlyph ? pGlyph->advance() : MU(), pFont->maxAscend() + pFont->maxDescend());
					MU w = pCaret->eolWidth(eolCellSize);
					if (w > eolCellSize.w)
						width += w - eolCellSize.w;
				}

				// Finish this line

				pLines->length = int(pChars - (pTextStart + pLines->offset)) + 1; 		// +1 to include line terminator.

				pLines->width = width;
				pLines->height = maxAscend + maxDescend;
				pLines->base = maxAscend;
				pLines->spacing = maxAscend + maxDescendGap;
				pLines++;

				// Update size

				if (width > size.w)
					size.w = width;

				size.h += pChars->isEndOfText() ? maxAscend + maxDescend : maxAscend + maxDescendGap;

				if (pChars->isEndOfText())
					break;

				// Prepare for next line

				pChars++;			// Line terminator belongs to previous line.

				pLines->offset = int(pChars - pTextStart);
				width = 0;
				potentialWidth = 0;
				pBreakpoint = nullptr;
				pPrevGlyph = nullptr;

				if (pChars->styleHandle() == hCharStyle)
				{
					// Still need to reset these, don't include data from styles only on previous line.

					maxAscend = pFont->maxAscend();
					maxDescend = pFont->maxDescend();
					maxDescendGap = maxDescend + pFont->lineGap();
				}
				else
				{
					maxAscend = 0;
					maxDescend = 0;
					maxDescendGap = 0;
				}
			}
			else
			{
				if (width > maxLineWidth - eolCaretWidth && pBreakpoint != nullptr )
				{
					bpWidth += eolCaretWidth;

					// Finish this line

					pLines->length = int(pBreakpoint - (pTextStart + pLines->offset));

					pLines->width = bpWidth;
					pLines->height = bpMaxAscend + bpMaxDescend;
					pLines->base = bpMaxAscend;
					pLines->spacing = bpMaxAscend + bpMaxDescendGap;
					pLines++;

					// Update size

					if (bpWidth > size.w)
						size.w = bpWidth;

					size.h += pChars->isEndOfText() ? bpMaxAscend + bpMaxDescend : bpMaxAscend + bpMaxDescendGap;

					// Prepare for next line

					pChars = pBreakpoint;

					pLines->offset = int(pChars - pTextStart);
					width = 0;
					potentialWidth = 0;
					pBreakpoint = nullptr;
					pPrevGlyph = nullptr;

					if (pChars->styleHandle() == hCharStyle)
					{
						// Still need to reset these, don't include data from styles only on previous line.

						maxAscend = pFont->maxAscend();
						maxDescend = pFont->maxDescend();
						maxDescendGap = maxDescend + pFont->lineGap();
					}
					else
					{
						maxAscend = 0;
						maxDescend = 0;
						maxDescendGap = 0;
					}
				}
				else
				{
					int code = pChars->code();
					if (code == 32)
					{
						pBreakpoint = pChars + 1;
						bpWidth = width;
						bpMaxAscend = maxAscend;
						bpMaxDescend = maxDescend;
						bpMaxDescendGap = maxDescendGap;							// Including the line gap.
					}

					pChars++;
				}
			}
		}
		return size;
	}



	//____ _updateFixedLineInfo() ________________________________________________

	Size StdTextMapper::_updateFixedLineInfo( BlockHeader * pHeader, LineInfo * pLines, const Char * pChars, const TextStyle * pBaseStyle,
												State state )
	{
		Caret * pCaret = m_pCaret ? m_pCaret : Base::defaultCaret();
		const Char * pTextStart = pChars;

		Size		size;

		TextAttr		baseAttr;
		pBaseStyle->exportAttr( state, &baseAttr );

		TextAttr		attr;
		Font_p 			pFont;

		TextStyle_h		hCharStyle = 0xFFFF;			// Force change on first character.

		Glyph_p	pGlyph = nullptr;
		Glyph_p	pPrevGlyph = nullptr;

		MU maxAscend = 0;
		MU maxDescend = 0;
		MU maxDescendGap = 0;							// Including the line gap.
		MU spaceAdv = 0;
		MU width = 0;

		pLines->offset = int(pChars - pTextStart);

		while( true )
		{

			if( pChars->styleHandle() != hCharStyle )
			{

				MU oldFontSize = attr.size;

				attr = baseAttr;
				if( pChars->styleHandle() != 0 )
					pChars->stylePtr()->addToAttr( state, &attr );

				if( pFont != attr.pFont || MU(attr.size) != oldFontSize )
				{
					pFont = attr.pFont;
					pFont->setSize(attr.size);
					pPrevGlyph = 0;								// No kerning against across different fonts or fontsizes.
				}

				MU ascend = pFont->maxAscend();
				if( ascend > maxAscend )
					maxAscend = ascend;

				MU descend = pFont->maxDescend();
				if( descend > maxDescend )
					maxDescend = descend;

				MU descendGap = descend + pFont->lineGap();
				if( descendGap > maxDescendGap )
					maxDescendGap = descendGap;

				spaceAdv = pFont->whitespaceAdvance();

				hCharStyle = pChars->styleHandle();
			}


			// TODO: Include handling of special characters
			// TODO: Support sub/superscript.

			pGlyph = _getGlyph( pFont.rawPtr(), pChars->code() );

			if( pGlyph )
			{
				if( pPrevGlyph )
					width += pFont->kerning(pPrevGlyph, pGlyph);

				width += pGlyph->advance();
			}
			else if( pChars->code() == 32 )
				width += spaceAdv;

			pPrevGlyph = pGlyph;

			// Handle end of line

			if( pChars->isEndOfLine() )
			{
				// Make sure we have space for eol caret

				if( pCaret )
				{
					Size eolCellSize( pGlyph ? pGlyph->advance() : MU(), pFont->maxAscend() + pFont->maxDescend() );
					MU w = pCaret->eolWidth( eolCellSize );
					if( w > eolCellSize.w )
						width += w - eolCellSize.w;
				}

				// Finish this line

				pLines->length = int(pChars - (pTextStart + pLines->offset)) +1; 		// +1 to include line terminator.

				pLines->width = width;
				pLines->height = maxAscend + maxDescend;
				pLines->base = maxAscend;
				pLines->spacing = maxAscend + maxDescendGap;
				pLines++;

				// Update size

				if (width > size.w)
					size.w = width;

				size.h += pChars->isEndOfText() ? maxAscend + maxDescend : maxAscend + maxDescendGap;

				//

				if( pChars->isEndOfText() )
					break;

				// Prepare for next line

				pChars++;			// Line terminator belongs to previous line.

				pLines->offset = int(pChars - pTextStart);
				width = 0;
				pPrevGlyph = nullptr;

				if (pChars->styleHandle() == hCharStyle)
				{
					// Still need to reset these, don't include data from styles only on previous line.

					maxAscend = pFont->maxAscend();
					maxDescend = pFont->maxDescend();
					maxDescendGap = maxDescend + pFont->lineGap();
				}
				else
				{
					maxAscend = 0;
					maxDescend = 0;
					maxDescendGap = 0;
				}
			}
			else
				pChars++;
		}
		return size;
	}


	//____ _linePosX() _______________________________________________________________

	MU StdTextMapper::_linePosX( const LineInfo * pLine, MU canvasWidth ) const
	{
		switch( m_alignment )
		{
			case Origo::NorthWest:
			case Origo::West:
			case Origo::SouthWest:
			default:
				return 0;
			case Origo::North:
			case Origo::Center:
			case Origo::South:
				return (canvasWidth - pLine->width) / 2;
			case Origo::NorthEast:
			case Origo::East:
			case Origo::SouthEast:
				return canvasWidth - pLine->width;
		}
	}

	//____ _linePosY() _______________________________________________________________

	MU StdTextMapper::_linePosY( const void * pBlock, int line, MU canvasHeight ) const
	{
		MU ofsY = _textPosY( _header(pBlock), canvasHeight );

		const LineInfo * pL = _lineInfo(pBlock);
		for( int i = 0 ; i < line ; i++ )
			ofsY += pL[i].spacing;

		return ofsY;
	}

	//____ _textPosY() _____________________________________________________________

	MU	StdTextMapper::_textPosY( const BlockHeader * pHeader, MU canvasHeight ) const
	{
		switch( m_alignment )
		{
			case Origo::NorthWest:
			case Origo::North:
			case Origo::NorthEast:
			default:
				return 0;
			case Origo::West:
			case Origo::Center:
			case Origo::East:
				return (canvasHeight - pHeader->textSize.h) / 2;
			case Origo::SouthWest:
			case Origo::South:
			case Origo::SouthEast:
				return canvasHeight - pHeader->textSize.h;
		}
	}

	//____ _charPosX() _________________________________________________________

	MU StdTextMapper::_charPosX( const Text * pText, int charOfs ) const
	{
		const LineInfo * pLine = _lineInfo( _dataBlock(pText) ) + charLine(pText, charOfs);
		const Char * pChars = _chars(pText);

		TextAttr attr;
		_baseStyle(pText)->exportAttr( _state(pText), &attr );

		return _linePosX( pLine, _size(pText).w ) + _charDistance( pChars + pLine->offset, pChars + charOfs, attr, _state(pText) );
	}

	//____ _lineAtPosY() _______________________________________________________

	int StdTextMapper::_lineAtPosY( const Text * pText, MU posY, SelectMode mode ) const
	{
		const void * pBlock = _dataBlock(pText);
		const BlockHeader * pHead = _header(pBlock);
		MU linePosY = _textPosY( pHead, _size(pText).h );
		const LineInfo * pLine = _lineInfo( _dataBlock(pText) );

		if( posY < linePosY )
		{
			if( mode == SelectMode::Marked )
				return -1;

			return 0;
		}

		switch( mode )
		{
			case SelectMode::ClosestBegin:
			{
				MU prevBeg = linePosY;
				linePosY += pLine[0].spacing;

				for( int i = 1 ; i < pHead->nbLines ; i++ )
				{
					MU beg = linePosY;

					if( posY <= beg )
					{
						if( posY - prevBeg < beg - posY )
							return i-1;
						else
							return i;
					}
					prevBeg = beg;
					linePosY += pLine[i].spacing;
				}
				return pHead->nbLines-1;
			}
			case SelectMode::ClosestEnd:
			{
				MU prevEnd = linePosY + pLine[0].height;
				linePosY += pLine[0].spacing;

				for( int i = 1 ; i < pHead->nbLines ; i++ )
				{
					MU end = linePosY + pLine[i].height;

					if( posY <= end )
					{
						if( posY - prevEnd < end - posY )
							return i-1;
						else
							return i;
					}
					prevEnd = end;
					linePosY += pLine[i].spacing;
				}
				return pHead->nbLines-1;
			}
			default:
			case SelectMode::Closest:
			case SelectMode::Marked:
			{
				for( int i = 0 ; i < pHead->nbLines ; i++ )
				{
					if( posY >= linePosY && posY < linePosY + pLine[i].height )
						return i;

					linePosY += pLine[i].spacing;
				}

				if( mode == SelectMode::Closest )
					return pHead->nbLines-1;

				return -1;
			}
		}
	}

	//____ _charAtPosX() _______________________________________________________

	int StdTextMapper::_charAtPosX( const Text * pText, int line, MU posX, SelectMode mode ) const
	{
		const LineInfo * pLine = _lineInfo( _dataBlock(pText) ) + line;


		MU distance = _linePosX( pLine, _size(pText).w );

		// Handle special case when we are left of line.

		if( posX < distance )
		{
			if( mode == SelectMode::Marked )
				return -1;
			else
				return pLine->offset;
		}

		// Handle special case when we are right of line.

		if( posX >= distance + pLine->width )
		{
			if( mode == SelectMode::Marked )
				return -1;
			else
				return pLine->offset + pLine->length-1;
		}

		// We are somewhere inside the line, lets loop through characters

		const Char * pTextBegin = _chars(pText);
		State state = _state(pText);

		TextAttr baseAttr;
		_baseStyle(pText)->exportAttr( state, &baseAttr );


		TextAttr		attr;
		Font_p 			pFont;
		TextStyle_h		hStyle = 0xFFFF;

		Glyph_p	pGlyph;
		Glyph_p	pPrevGlyph =  0;

		const Char * pChar = pTextBegin + pLine->offset;

		while( true )
		{
			// TODO: Include handling of special characters

			if( pChar->styleHandle() != hStyle )
			{
				MU oldFontSize = attr.size;
				attr = baseAttr;

				if( pChar->styleHandle() != 0 )
					pChar->stylePtr()->addToAttr( state, &attr );

				if( pFont != attr.pFont || MU(attr.size) != oldFontSize )
				{
					pFont = attr.pFont;
					pFont->setSize(attr.size);
					pPrevGlyph = 0;								// No kerning against across different fonts or characters of different size.
				}
			}

			// Forward distance with the glyph

			pGlyph = _getGlyph( pFont.rawPtr(), pChar->code() );
			MU charBeg = distance;

			if( pGlyph )
			{
				if( pPrevGlyph )
					distance += pFont->kerning(pPrevGlyph, pGlyph);

				distance += pGlyph->advance();
			}
			else if( pChar->code() == 32 )
				distance += pFont->whitespaceAdvance();

			// Check if we have passed our mark.

			if( distance >= posX )
			{
				switch( mode )
				{
					case SelectMode::ClosestBegin:
					{
						if( posX - charBeg <= distance - posX )
							return int(pChar - pTextBegin);
						else
							return int(pChar+1 - pTextBegin);
					}
					case SelectMode::ClosestEnd:
					{
						if( posX - charBeg < distance - posX )
							return pChar == pTextBegin ? 0 : int(pChar-1 - pTextBegin);
						else
							return int(pChar - pTextBegin);
					}
					break;
					case SelectMode::Closest:
					case SelectMode::Marked:
					{
						return int(pChar - pTextBegin);
					}
				}
			}

			pPrevGlyph = pGlyph;
			pChar++;
		}

		return -1;			// We should never get here...
	}


} // namespace wg
