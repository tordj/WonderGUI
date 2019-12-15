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
#include <wg_ctext.h>
#include <wg_textstyle.h>
#include <wg_gfxdevice.h>
#include <wg_char.h>
#include <wg_msgrouter.h>

#include <stdlib.h>
#include <algorithm>

namespace wg
{
	using namespace Util;

	const char StdTextMapper::CLASSNAME[] = {"StdTextMapper"};

	//____ Constructor _____________________________________________________________

	StdTextMapper::StdTextMapper() : m_alignment(Origo::NorthWest), m_bLineWrap(false), m_selectionBackColor(Color::White), m_selectionBackRenderMode(BlendMode::Invert),
		m_selectionCharColor(Color::White), m_selectionCharBlend(BlendMode::Invert), m_pFocusedComponent(nullptr), m_tickRouteId(0)
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


	//____ isInstanceOf() _________________________________________________________

	bool StdTextMapper::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return TextMapper::isInstanceOf(pClassName);
	}

	//____ className() ____________________________________________________________

	const char * StdTextMapper::className( void ) const
	{
		return CLASSNAME;
	}

	//____ cast() _________________________________________________________________

	StdTextMapper_p StdTextMapper::cast( Object * pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return StdTextMapper_p( static_cast<StdTextMapper*>(pObject) );

		return 0;
	}

	//____ addComponent() _________________________________________________________

	void StdTextMapper::addComponent( CText * pText )
	{
		CharBuffer * pBuffer = _charBuffer(pText);
		int nLines = _countLines( pText, pBuffer );

		_setComponentDataBlock(pText,0);					// Make sure pointer is null for the realloc call.
		void * pBlock = _reallocBlock(pText,nLines);

		_updateLineInfo( pText, pBlock, pBuffer );
	}

	//____ removeComponent() _________________________________________________________

	void StdTextMapper::removeComponent( CText * pText )
	{
		free( _dataBlock(pText) );
		_setComponentDataBlock(pText, 0);

		if( pText == m_pFocusedComponent )
		{
			m_pFocusedComponent = 0;
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

			//TODO: Make all components dirty
		}
	}

	//____ setLineWrap() __________________________________________________________

	void StdTextMapper::setLineWrap(bool bWrap)
	{
		if (bWrap != m_bLineWrap)
		{
			m_bLineWrap = bWrap;

			//TODO: Make all components dirty
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

	int StdTextMapper::charAtPos( const CText * pText, CoordI pos ) const
	{
		int line = _lineAtPosY(pText, pos.y, SelectMode::Marked );
		if( line == -1 )
			return -1;

		return  _charAtPosX(pText, line, pos.x, SelectMode::Marked );
	}

	//_____ charPos() ______________________________________________________

	CoordI StdTextMapper::charPos( const CText * pText, int charOfs ) const
	{
		int line = charLine(pText, charOfs);

		int ofsX = _charPosX(pText, charOfs);
		int ofsY = _linePosY(_dataBlock(pText), line, pText->_size().h );

		const LineInfo * pLine = _lineInfo( _dataBlock(pText) ) + line;
		ofsY += pLine->base;

		return CoordI(ofsX,ofsY);
	}

	//____ charRect() ________________________________________________________

	RectI StdTextMapper::charRect( const CText * pText, int charOfs ) const
	{
		const void * pBlock = _dataBlock(pText);
		const BlockHeader * pHeader = _header(pBlock);
		const LineInfo * pLineInfo = _lineInfo(pBlock);

		// Find correct line and determine yOfs

		int yOfs = _textPosY( pHeader, pText->_size().h );
		while( pLineInfo->length <= charOfs )
		{
			yOfs += pLineInfo->spacing;
			charOfs-= pLineInfo->length;
			pLineInfo++;
		}

		// Determine xOfs by parsing line until character

		int xOfs = _linePosX( pLineInfo, pText->_size().w );

		TextAttr		baseAttr;
		_baseStyle(pText)->exportAttr( _state(pText), &baseAttr );

		const Char * pFirst = _charBuffer(pText)->chars() + pLineInfo->offset;
		const Char * pLast = pFirst + charOfs;

		xOfs += _charDistance( pFirst, pLast, baseAttr, _state(pText) );

		// Get cell width

		int width = 0;

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

		return RectI(xOfs,yOfs,width,pLineInfo->height);
	}

	//____ charLine() ________________________________________________________

	int StdTextMapper::charLine( const CText * pText, int charOfs ) const
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

	int StdTextMapper::lineBegin( const CText * pText, int lineNb ) const
	{
		const void * pBlock = _dataBlock(pText);
		const BlockHeader * pHeader = _header(pBlock);
		const LineInfo * pLineInfo = _lineInfo(pBlock);

		if( lineNb < 0 || lineNb >= pHeader->nbLines )
			return -1;

		return pLineInfo[lineNb].offset;
	}

	//____ lineEnd() ___________________________________________________________

	int StdTextMapper::lineEnd( const CText * pText, int lineNb ) const
	{
		const void * pBlock = _dataBlock(pText);
		const BlockHeader * pHeader = _header(pBlock);
		const LineInfo * pLineInfo = _lineInfo(pBlock);

		if( lineNb < 0 || lineNb >= pHeader->nbLines )
			return -1;

		return pLineInfo[lineNb].offset + pLineInfo[lineNb].length;
	}

	//____ wordBegin() _________________________________________________________

	int StdTextMapper::wordBegin( const CText * pText, int charOfs ) const
	{
		//TODO: Implement!
		return charOfs;
	}

	//____ wordEnd() ___________________________________________________________

	int StdTextMapper::wordEnd( const CText * pText, int charOfs ) const
	{
		//TODO: Implement!
		return charOfs+1;
	}


	//____ _charDistance() _____________________________________________________

	// Returns distance in pixels between beginning of first and beginning of last char.
	// Chars should be on the same line (or pLast could be first char on next line)

	int StdTextMapper::_charDistance( const Char * pFirst, const Char * pLast, const TextAttr& baseAttr, State state ) const
	{
		TextAttr		attr;
		Font_p 			pFont;
		TextStyle_h		hStyle = 0xFFFF;

		Glyph_p	pGlyph;
		Glyph_p	pPrevGlyph =  0;
		const Char * pChar = pFirst;

		int distance = 0;

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
		if( pMsg->type() == MsgType::Tick && m_pFocusedComponent )
		{
			const EditState * pEditState = m_pFocusedComponent->_editState();

			Caret * pCaret = m_pCaret ? m_pCaret : Base::defaultCaret();
			if( pEditState && pEditState->bCaret && pCaret )
			{
				int ms = static_cast<TickMsg*>(pMsg)->timediff();

				bool bDirty = pCaret->tick( ms );
				if( bDirty )
				{
					_setComponentDirty( m_pFocusedComponent, pCaret->dirtyRect(charRect(m_pFocusedComponent, pEditState->caretOfs)) );
				}
			}
		}
	}

	//____ _render()___________________________________________________________

	void StdTextMapper::render( CText * pText, GfxDevice * pDevice, const RectI& canvas )
	{

		void * pBlock = _dataBlock(pText);
		BlockHeader * pHeader = _header(pBlock);
		LineInfo * pLineInfo = _lineInfo(pBlock);
		const Char * pCharArray = _charBuffer(pText)->chars();

		CoordI lineStart = canvas.pos();
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
			limitClipList(pDevice, canvas);


		// Render back colors

		_renderBack( pText, pDevice, canvas );


		const EditState * pEditState = _editState( pText );

		// Get selection start and end

		const Char * pSelBeg = nullptr;
		const Char * pSelEnd = nullptr;

		bool bInSelection = false;

		if( pEditState && pEditState->selectOfs != pEditState->caretOfs )
		{
			int selBeg = pEditState->selectOfs;
			int selEnd = pEditState->caretOfs;
			if( selBeg > selEnd )
				std::swap( selBeg, selEnd );


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

		const RectI& clip = pDevice->clipBounds();

		for( int i = 0 ; i < pHeader->nbLines ; i++ )
		{
			if( lineStart.y < clip.y + clip.h && lineStart.y + pLineInfo->height > clip.y )
			{
				lineStart.x = canvas.x + _linePosX( pLineInfo, canvas.w );
				const Char * pChar = pCharArray + pLineInfo->offset;

				Glyph_p	pGlyph;
				Glyph_p	pPrevGlyph =  0;

				CoordI pos = lineStart;
				pos.y += pLineInfo->base;

				bool bRecalcColor = false;

				for( int x = 0 ; x < pLineInfo->length ; x++ )
				{
					// TODO: Include handling of special characters
					// TODO: Support char textcolor and background color and effects.

					if( pChar->styleHandle() != hStyle )
					{
						int oldFontSize = attr.size;
						Color oldBgColor = attr.bgColor;

						attr = baseAttr;

						if( pChar->styleHandle() != 0 )
							pChar->stylePtr()->addToAttr( _state(pText), &attr );

						if( pFont != attr.pFont || attr.size != oldFontSize )
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
						pDevice->blit( CoordI(pos.x + pBitmap->bearingX, pos.y + pBitmap->bearingY), pBitmap->rect  );

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

		Caret * pCaret = m_pCaret ? m_pCaret : Base::defaultCaret();
		if( pEditState && pEditState->bCaret && pCaret )
		{
			pCaret->render( pDevice, charRect(pText, pEditState->caretOfs) + canvas.pos() );
		}

		// Pop any changes to cliplist.

		popClipList(pDevice, popData);
	}




	//____ _renderBack()___________________________________________________________

	void StdTextMapper::_renderBack( CText * pText, GfxDevice * pDevice, const RectI& canvas )
	{
		const Char * pCharArray = _charBuffer(pText)->chars();
		const Char * pBeg = pCharArray;
		const Char * pChar;

		TextStyle_h hStyle = 0xFFFF;

		Color		color = Color::Transparent;

		for( pChar = pCharArray ; !pChar->isEndOfText() ; pChar++ )
		{
			if( pChar->styleHandle() != hStyle )
			{
				Color newColor = _baseStyle(pText)->combBgColor( _state(pText) );

				TextStyle_p p = pChar->stylePtr();
				if( p )
					newColor = Color::blend( newColor, p->combBgColor(_state(pText)), p->bgColorBlendMode(_state(pText)) );

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

	void StdTextMapper::_renderBackSection( CText * pText, GfxDevice * pDevice, const RectI& canvas,
											int begChar, int endChar, Color color )
	{

		CoordI begPos = charPos( pText, begChar );
		LineInfo * pBegLine = _lineInfo( _dataBlock(pText) ) + charLine( pText, begChar );

		CoordI endPos = charPos( pText, endChar );
		LineInfo * pEndLine = _lineInfo( _dataBlock(pText) ) + charLine( pText, endChar );


		if( pBegLine == pEndLine )
		{
			RectI area;
			area.x = canvas.x + begPos.x;
			area.y = canvas.y + begPos.y - pBegLine->base;
			area.w = endPos.x - begPos.x;
			area.h = pBegLine->height;

			pDevice->fill( area, color );
		}
		else
		{
			LineInfo * pLine = pBegLine;

			RectI area;
			area.x = canvas.x + begPos.x;
			area.y = canvas.y + begPos.y - pLine->base;
			area.w = pLine->width - (begPos.x - _linePosX( pLine, canvas.w));
			area.h = pLine->height;

			pDevice->fill( area, color );

			area.y += pLine->spacing;
			pLine++;

			while( pLine != pEndLine )
			{
				area.x = canvas.x + _linePosX( pLine, canvas.w);
				area.w = pLine->width;
				area.h = pLine->height;

				pDevice->fill( area, color );

				area.y += pLine->spacing;
				pLine++;
			}


			area.x = canvas.x + _linePosX( pLine, canvas.w);
			area.w = endPos.x - area.x;
			area.h = pLine->height;

			pDevice->fill( area, color );
		}

	}

	//____ caretMove() ________________________________________________________

	void StdTextMapper::caretMove( CText * pText, int ofs )
	{
		const EditState * pEditState = pText->_editState();

		Caret * pCaret = m_pCaret ? m_pCaret : Base::defaultCaret();
		if( pEditState->bCaret && pCaret )
		{
			bool bDirty = pCaret->restartCycle();

			if( bDirty || pEditState->caretOfs != ofs )
			{
				_setComponentDirty( pText, pCaret->dirtyRect( charRect(pText, ofs) ));

				if( pEditState->caretOfs != ofs )
					_setComponentDirty( pText, pCaret->dirtyRect( charRect(pText, pEditState->caretOfs)) );
			}
		}
	}

	//____ selectionChange() __________________________________________________

	void StdTextMapper::selectionChange( CText * pText, int selectOfs, int caretOfs )
	{
		RectI dirt;

		const EditState * pEditState = pText->_editState();

		if( selectOfs != pEditState->selectOfs )
		{
			int beg = std::min(selectOfs,pEditState->selectOfs);
			int len = std::max(selectOfs,pEditState->selectOfs) - beg;
			dirt = rectForRange( pText, beg, len );
		}

		if( caretOfs != pEditState->caretOfs )
		{
			int beg = std::min(caretOfs,pEditState->caretOfs);
			int len = std::max(caretOfs,pEditState->caretOfs) - beg;

			if( dirt.isEmpty() )
				dirt = rectForRange( pText, beg, len );
			else
				dirt.growToContain(rectForRange( pText, beg, len ) );

			Caret * pCaret = m_pCaret ? m_pCaret : Base::defaultCaret();
			if( pEditState->bCaret && pCaret )
			{
				pCaret->restartCycle();
				dirt.growToContain( pCaret->dirtyRect( charRect(pText, caretOfs) ));
				dirt.growToContain( pCaret->dirtyRect( charRect(pText, pText->_editState()->caretOfs)) );
			}
		}

		_setComponentDirty( pText, dirt );
	}


	//____ onTextModified() ____________________________________________________

	void StdTextMapper::onTextModified( CText * pText, int ofs, int charsRemoved, int charsAdded )
	{
		onRefresh(pText);
	}

	//____ onResized() ___________________________________________________________

	void StdTextMapper::onResized( CText * pText, SizeI newSize, SizeI oldSize )
	{
		if (m_bLineWrap)
			onRefresh(pText);


		///TODO: Implement!
	}

	//____ onStateChanged() ______________________________________________________

	void StdTextMapper::onStateChanged( CText * pText, State newState, State oldState )
	{
		// TODO: Support for more than one input device, focusing different (or same) components.

		if( newState.isFocused() != oldState.isFocused() )
		{
			if( newState.isFocused() )
			{
				m_pFocusedComponent = pText;
				if( !m_tickRouteId )
					m_tickRouteId = Base::msgRouter()->addRoute( MsgType::Tick, this );
			}
			else
			{
				m_pFocusedComponent = 0;
				if( m_tickRouteId )
				{
					Base::msgRouter()->deleteRoute( m_tickRouteId );
					m_tickRouteId = 0;
				}
			}
		}
	}

	//____ onStyleChanged() ______________________________________________________

	void StdTextMapper::onStyleChanged( CText * pText, TextStyle * pNewStyle, TextStyle * pOldStyle )
	{
		//TODO: Optimize: only update line info if textsize possibly affected.

		void * pBlock = _dataBlock(pText);

		_updateLineInfo( pText, pBlock, _charBuffer(pText) );

		_setComponentDirty(pText);
	}

	//____ onCharStyleChanged() __________________________________________________

	void StdTextMapper::onCharStyleChanged( CText * pText, int ofs, int len )
	{
		void * pBlock = _dataBlock(pText);

		_updateLineInfo( pText, pBlock, _charBuffer(pText) );

		_setComponentDirty(pText);
	}

	//____ onRefresh() ___________________________________________________________

	void StdTextMapper::onRefresh( CText * pText )
	{
		CharBuffer * pBuffer = _charBuffer(pText);
		int nLines = _countLines( pText, pBuffer );

		void * pBlock = _dataBlock(pText);
		if( !pBlock || _header(pBlock)->nbLines != nLines )
			pBlock = _reallocBlock(pText,nLines);

		_updateLineInfo( pText, pBlock, pBuffer );
		_setComponentDirty(pText);
	}

	//___ rectForRange() _________________________________________________________

	RectI StdTextMapper::rectForRange( const CText * pText, int ofs, int length ) const
	{
		int begChar = ofs;
		int endChar = ofs + length;

		SizeI canvas = pText->_size();


		CoordI begPos = charPos(pText, begChar);
		const LineInfo * pBegLine = _lineInfo(_dataBlock(pText)) + charLine(pText, begChar);

		CoordI endPos = charPos(pText, endChar);
		const LineInfo * pEndLine = _lineInfo(_dataBlock(pText)) + charLine(pText, endChar);

		int x1, x2, y1, y2;

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
				int nx1 = _linePosX(pLine, canvas.w);
				int nx2 = nx1 + pLine->width;

				if (nx1 < x1)
					x1 = nx1;
				if (nx2 > x2)
					x2 = nx2;

				y2 += pLine->spacing;
				pLine++;
			}

			int nx1 = _linePosX(pLine, canvas.w);
			int nx2 = endPos.x;

			if (nx1 < x1)
				x1 = nx1;
			if (nx2 > x2)
				x2 = nx2;

			y2 += pLine->height;
		}

		return RectI( x1, y1, x2-x1, y2-y1);
	}

	//____ rectForCaret() ______________________________________________________

	// Includes left/right margin where applicable.

	RectI StdTextMapper::rectForCaret( const CText * pText ) const
	{
		Caret * pCaret = m_pCaret ? m_pCaret : Base::defaultCaret();
		if( !pText->_editState()->bCaret || !pCaret )
			return RectI();

		return pCaret->dirtyRect( charRect(pText, pText->_editState()->caretOfs) );
	}


	//____ textDirection() ____________________________________________________

	Direction StdTextMapper::textDirection( CText * pText, int charOfs ) const
	{
		return Direction::Right;
	}

	//____ caretToPos() _____________________________________________________

	int StdTextMapper::caretToPos( CText * pText, CoordI pos, int& wantedLineOfs ) const
	{
		wantedLineOfs = -1;

		int line = _lineAtPosY(pText, pos.y, SelectMode::Closest );
		return  _charAtPosX(pText, line, pos.x, SelectMode::ClosestBegin );
	}

	//____ caretUp() ___________________________________________________________

	int StdTextMapper::caretUp( CText * pText, int charOfs, int& wantedLineOfs ) const
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

	int StdTextMapper::caretDown( CText * pText, int charOfs, int& wantedLineOfs ) const
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

	int StdTextMapper::caretLeft( CText * pText, int charOfs, int& wantedLineOfs ) const
	{
		if( charOfs > 0 )
			charOfs--;

		wantedLineOfs = -1;
		return charOfs;
	}

	//____ caretRight() ________________________________________________________

	int StdTextMapper::caretRight( CText * pText, int charOfs, int& wantedLineOfs ) const
	{
		if( charOfs < _charBuffer(pText)->length() )
			charOfs++;

		wantedLineOfs = -1;
		return charOfs;
	}

	//____ caretHome() ________________________________________________________

	int StdTextMapper::caretHome( CText * pText, int charOfs, int& wantedLineOfs ) const
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

	int StdTextMapper::caretEnd( CText * pText, int charOfs, int& wantedLineOfs ) const
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

	int StdTextMapper::caretPrevWord( CText * pText, int charOfs ) const
	{
		//TODO: Implement!
		return charOfs;
	}

	//____ caretNextWord() _____________________________________________________

	int StdTextMapper::caretNextWord( CText * pText, int charOfs ) const
	{
		//TODO: Implement!
		return charOfs;
	}


	//____ tooltip() _______________________________________________________________

	String StdTextMapper::tooltip( const CText * pText ) const
	{
		//TODO: Return the text if it overflows the component.

		return String();
	}


	//____ preferredSize() _________________________________________________________

	SizeI StdTextMapper::preferredSize( const CText * pText ) const
	{
		return _header(_dataBlock(pText))->preferredSize;
	}

	//____ matchingWidth() _________________________________________________________

	int StdTextMapper::matchingWidth( const CText * pText, int height ) const
	{
		return	_header(_dataBlock(pText))->preferredSize.w;
	}

	//____ matchingHeight() ________________________________________________________

	int StdTextMapper::matchingHeight( const CText * pText, int width ) const
	{
		return _calcMatchingHeight(_charBuffer(pText), _baseStyle(pText), _state(pText), width);
	}

	//____ _countLines() ___________________________________________________________

	int StdTextMapper::_countLines( CText * pText, const CharBuffer * pBuffer ) const
	{
		if (m_bLineWrap)
			return _countWrapLines(pBuffer, _baseStyle(pText), _state(pText), pText->_size().w);
		else
			return _countFixedLines(pBuffer);
	}

	//____ _countFixedLines() ___________________________________________________________

	int StdTextMapper::_countFixedLines(const CharBuffer * pBuffer) const
	{
		const Char * pChars = pBuffer->chars();
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

	int StdTextMapper::_countWrapLines(const CharBuffer * pBuffer, const TextStyle * pBaseStyle, State state, int maxLineWidth) const
	{
		const Char * pChars = pBuffer->chars();

		TextAttr		baseAttr;
		pBaseStyle->exportAttr(state, &baseAttr);


		TextAttr		attr;
		Font_p 			pFont;

		TextStyle_h		hCharStyle = 0xFFFF;			// Force change on first character.

		Glyph_p	pGlyph = nullptr;
		Glyph_p	pPrevGlyph = nullptr;

		int spaceAdv = 0;
		int width = 0;
		int potentialWidth = 0;
		int eolCaretWidth = 0;
		//

		const Char * pBreakpoint = nullptr;
		int bpWidth = 0;

		int nLines = 0;												//

		while (true)
		{

			if (pChars->styleHandle() != hCharStyle)
			{

				int oldFontSize = attr.size;

				attr = baseAttr;
				if (pChars->styleHandle() != 0)
					pChars->stylePtr()->addToAttr(state, &attr);

				if (pFont != attr.pFont || attr.size != oldFontSize)
				{
					pFont = attr.pFont;
					pFont->setSize(attr.size);
					pPrevGlyph = 0;								// No kerning against across different fonts or fontsizes.
				}

				spaceAdv = pFont->whitespaceAdvance();

				Caret * pCaret = m_pCaret ? m_pCaret : Base::defaultCaret();
				if (pCaret)
				{
					SizeI eolCellSize(pGlyph ? pGlyph->advance() : 0, pFont->maxAscend() + pFont->maxDescend());
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

	int StdTextMapper::_calcMatchingHeight(const CharBuffer * pBuffer, const TextStyle * pBaseStyle, State state, int maxLineWidth) const
	{
		Caret * pCaret = m_pCaret ? m_pCaret : Base::defaultCaret();
		const Char * pChars = pBuffer->chars();

		TextAttr		baseAttr;
		pBaseStyle->exportAttr(state, &baseAttr);

		int totalHeight = 0;

		TextAttr		attr;
		Font_p 			pFont;

		TextStyle_h		hCharStyle = 0xFFFF;			// Force change on first character.

		Glyph_p	pGlyph = nullptr;
		Glyph_p	pPrevGlyph = nullptr;

		int maxAscend = 0;
		int maxDescend = 0;
		int maxDescendGap = 0;							// Including the line gap.
		int spaceAdv = 0;
		int width = 0;
		int potentialWidth = 0;
		int eolCaretWidth = 0;
		//

		const Char * pBreakpoint = nullptr;
		int bpWidth = 0;
		int bpMaxAscend = 0;
		int bpMaxDescend = 0;
		int bpMaxDescendGap = 0;							// Including the line gap.

															//
		while (true)
		{

			if (pChars->styleHandle() != hCharStyle)
			{

				int oldFontSize = attr.size;

				attr = baseAttr;
				if (pChars->styleHandle() != 0)
					pChars->stylePtr()->addToAttr(state, &attr);

				if (pFont != attr.pFont || attr.size != oldFontSize)
				{
					pFont = attr.pFont;
					pFont->setSize(attr.size);
					pPrevGlyph = 0;								// No kerning against across different fonts or fontsizes.
				}

				int ascend = pFont->maxAscend();
				if (ascend > maxAscend)
					maxAscend = ascend;

				int descend = pFont->maxDescend();
				if (descend > maxDescend)
					maxDescend = descend;

				int descendGap = descend + pFont->lineGap();
				if (descendGap > maxDescendGap)
					maxDescendGap = descendGap;

				spaceAdv = pFont->whitespaceAdvance();

				if (pCaret)
				{
					SizeI eolCellSize(pGlyph ? pGlyph->advance() : 0, pFont->maxAscend() + pFont->maxDescend());
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
					SizeI eolCellSize(pGlyph ? pGlyph->advance() : 0, pFont->maxAscend() + pFont->maxDescend());
					int w = pCaret->eolWidth(eolCellSize);
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

	void * StdTextMapper::_reallocBlock( CText* pText, int nLines )
	{
		void * pBlock = _dataBlock(pText);
		if( pBlock )
			free( pBlock );

		pBlock = malloc( sizeof(BlockHeader) + sizeof(LineInfo)*nLines);
		_setComponentDataBlock(pText, pBlock);
		((BlockHeader *)pBlock)->nbLines = nLines;

		return pBlock;
	}


	//____ _updateLineInfo() _______________________________________________________

	void StdTextMapper::_updateLineInfo( CText * pText, void * pBlock, const CharBuffer * pBuffer )
	{
		BlockHeader * pHeader = _header(_dataBlock(pText));
		SizeI preferredSize;


		if (m_bLineWrap)
		{
			//TODO: This is slow, calling both _updateFixedLineInfo and _updateWrapLineInfo if line is wrapped, just so we can update preferredSize.

			preferredSize = _updateFixedLineInfo(_header(pBlock), _lineInfo(pBlock), pBuffer, _baseStyle(pText), _state(pText));
			pHeader->textSize = _updateWrapLineInfo(_header(pBlock), _lineInfo(pBlock), pBuffer, _baseStyle(pText), _state(pText), pText->_size().w);
		}
		else
		{
			preferredSize = _updateFixedLineInfo(_header(pBlock), _lineInfo(pBlock), pBuffer, _baseStyle(pText), _state(pText));
			pHeader->textSize = preferredSize;
		}


		if (preferredSize != pHeader->preferredSize)
		{
			pHeader->preferredSize = preferredSize;
			_requestComponentResize(pText);
		}
	}

	//____ _updateWrapLineInfo() ________________________________________________

	SizeI StdTextMapper::_updateWrapLineInfo(BlockHeader * pHeader, LineInfo * pLines, const CharBuffer * pBuffer, const TextStyle * pBaseStyle, State state, int maxLineWidth )
	{
		Caret * pCaret = m_pCaret ? m_pCaret : Base::defaultCaret();
		const Char * pChars = pBuffer->chars();

		TextAttr		baseAttr;
		pBaseStyle->exportAttr(state, &baseAttr);

		SizeI size;

		TextAttr		attr;
		Font_p 			pFont;

		TextStyle_h		hCharStyle = 0xFFFF;			// Force change on first character.

		Glyph_p	pGlyph = nullptr;
		Glyph_p	pPrevGlyph = nullptr;

		int maxAscend = 0;
		int maxDescend = 0;
		int maxDescendGap = 0;							// Including the line gap.
		int spaceAdv = 0;
		int width = 0;
		int potentialWidth = 0;
		int eolCaretWidth = 0;
		//

		const Char * pBreakpoint = nullptr;
		int bpWidth = 0;
		int bpMaxAscend = 0;
		int bpMaxDescend = 0;
		int bpMaxDescendGap = 0;							// Including the line gap.


		pLines->offset = int(pChars - pBuffer->chars());

		while (true)
		{

			if (pChars->styleHandle() != hCharStyle)
			{

				int oldFontSize = attr.size;

				attr = baseAttr;
				if (pChars->styleHandle() != 0)
					pChars->stylePtr()->addToAttr(state, &attr);

				if (pFont != attr.pFont || attr.size != oldFontSize)
				{
					pFont = attr.pFont;
					pFont->setSize(attr.size);
					pPrevGlyph = 0;								// No kerning against across different fonts or fontsizes.
				}

				int ascend = pFont->maxAscend();
				if (ascend > maxAscend)
					maxAscend = ascend;

				int descend = pFont->maxDescend();
				if (descend > maxDescend)
					maxDescend = descend;

				int descendGap = descend + pFont->lineGap();
				if (descendGap > maxDescendGap)
					maxDescendGap = descendGap;

				spaceAdv = pFont->whitespaceAdvance();

				if (pCaret)
				{
					SizeI eolCellSize(pGlyph ? pGlyph->advance() : 0, pFont->maxAscend() + pFont->maxDescend());
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
					SizeI eolCellSize(pGlyph ? pGlyph->advance() : 0, pFont->maxAscend() + pFont->maxDescend());
					int w = pCaret->eolWidth(eolCellSize);
					if (w > eolCellSize.w)
						width += w - eolCellSize.w;
				}

				// Finish this line

				pLines->length = int(pChars - (pBuffer->chars() + pLines->offset)) + 1; 		// +1 to include line terminator.

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

				pLines->offset = int(pChars - pBuffer->chars());
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

					pLines->length = int(pBreakpoint - (pBuffer->chars() + pLines->offset));

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

					pLines->offset = int(pChars - pBuffer->chars());
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

	SizeI StdTextMapper::_updateFixedLineInfo( BlockHeader * pHeader, LineInfo * pLines, const CharBuffer * pBuffer, const TextStyle * pBaseStyle,
												State state )
	{
		Caret * pCaret = m_pCaret ? m_pCaret : Base::defaultCaret();
		const Char * pChars = pBuffer->chars();

		SizeI		size;

		TextAttr		baseAttr;
		pBaseStyle->exportAttr( state, &baseAttr );

		TextAttr		attr;
		Font_p 			pFont;

		TextStyle_h		hCharStyle = 0xFFFF;			// Force change on first character.

		Glyph_p	pGlyph = nullptr;
		Glyph_p	pPrevGlyph = nullptr;

		int maxAscend = 0;
		int maxDescend = 0;
		int maxDescendGap = 0;							// Including the line gap.
		int spaceAdv = 0;
		int width = 0;

		pLines->offset = int(pChars - pBuffer->chars());

		while( true )
		{

			if( pChars->styleHandle() != hCharStyle )
			{

				int oldFontSize = attr.size;

				attr = baseAttr;
				if( pChars->styleHandle() != 0 )
					pChars->stylePtr()->addToAttr( state, &attr );

				if( pFont != attr.pFont || attr.size != oldFontSize )
				{
					pFont = attr.pFont;
					pFont->setSize(attr.size);
					pPrevGlyph = 0;								// No kerning against across different fonts or fontsizes.
				}

				int ascend = pFont->maxAscend();
				if( ascend > maxAscend )
					maxAscend = ascend;

				int descend = pFont->maxDescend();
				if( descend > maxDescend )
					maxDescend = descend;

				int descendGap = descend + pFont->lineGap();
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
					SizeI eolCellSize( pGlyph ? pGlyph->advance() : 0, pFont->maxAscend() + pFont->maxDescend() );
					int w = pCaret->eolWidth( eolCellSize );
					if( w > eolCellSize.w )
						width += w - eolCellSize.w;
				}

				// Finish this line

				pLines->length = int(pChars - (pBuffer->chars() + pLines->offset)) +1; 		// +1 to include line terminator.

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

				pLines->offset = int(pChars - pBuffer->chars());
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

	int StdTextMapper::_linePosX( const LineInfo * pLine, int canvasWidth ) const
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

	int StdTextMapper::_linePosY( const void * pBlock, int line, int canvasHeight ) const
	{
		int ofsY = _textPosY( _header(pBlock), canvasHeight );

		const LineInfo * pL = _lineInfo(pBlock);
		for( int i = 0 ; i < line ; i++ )
			ofsY += pL[i].spacing;

		return ofsY;
	}

	//____ _textPosY() _____________________________________________________________

	int	StdTextMapper::_textPosY( const BlockHeader * pHeader, int canvasHeight ) const
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

	int StdTextMapper::_charPosX( const CText * pText, int charOfs ) const
	{
		const LineInfo * pLine = _lineInfo( _dataBlock(pText) ) + charLine(pText, charOfs);
		const Char * pBufferStart = _charBuffer(pText)->chars();

		TextAttr attr;
		_baseStyle(pText)->exportAttr( _state(pText), &attr );

		return _linePosX( pLine, pText->_size().w ) + _charDistance( pBufferStart + pLine->offset, pBufferStart + charOfs, attr, _state(pText) );
	}

	//____ _lineAtPosY() _______________________________________________________

	int StdTextMapper::_lineAtPosY( const CText * pText, int posY, SelectMode mode ) const
	{
		const void * pBlock = _dataBlock(pText);
		const BlockHeader * pHead = _header(pBlock);
		int linePosY = _textPosY( pHead, pText->_size().h );
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
				int prevBeg = linePosY;
				linePosY += pLine[0].spacing;

				for( int i = 1 ; i < pHead->nbLines ; i++ )
				{
					int beg = linePosY;

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
				int prevEnd = linePosY + pLine[0].height;
				linePosY += pLine[0].spacing;

				for( int i = 1 ; i < pHead->nbLines ; i++ )
				{
					int end = linePosY + pLine[i].height;

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

	int StdTextMapper::_charAtPosX( const CText * pText, int line, int posX, SelectMode mode ) const
	{
		const LineInfo * pLine = _lineInfo( _dataBlock(pText) ) + line;


		int distance = _linePosX( pLine, pText->_size().w );

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

		const Char * pTextBegin = _charBuffer(pText)->chars();
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

			// Forward distance with the glyph

			pGlyph = _getGlyph( pFont.rawPtr(), pChar->code() );
			int pCharBeg = distance;

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
						if( posX - pCharBeg <= distance - posX )
							return int(pChar - pTextBegin);
						else
							return int(pChar+1 - pTextBegin);
					}
					case SelectMode::ClosestEnd:
					{
						if( posX - pCharBeg < distance - posX )
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
