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

#include <wg_standardprinter.h>
#include <wg_printablefield.h>
#include <wg_textstyle.h>
#include <wg_gfxdevice.h>
#include <wg_char.h>
#include <malloc.h>

namespace wg 
{
	
	const char StandardPrinter::CLASSNAME[] = {"StandardPrinter"};
	
	//____ Constructor _____________________________________________________________
	
	StandardPrinter::StandardPrinter() : m_alignment(Origo::NorthWest)
	{
	}
	
	//____ Destructor ______________________________________________________________
	
	StandardPrinter::~StandardPrinter()
	{
	}
	
	
	//____ isInstanceOf() _________________________________________________________
	
	bool StandardPrinter::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return Printer::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * StandardPrinter::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	StandardPrinter_p StandardPrinter::cast( const Object_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return StandardPrinter_p( static_cast<StandardPrinter*>(pObject.rawPtr()) );
	
		return 0;
	}
	
	//____ addField() _________________________________________________________
	
	void StandardPrinter::addField( PrintableField * pField )
	{
		CharBuffer * pBuffer = _charBuffer(pField);
		int nLines = _countLines( pBuffer );
	
		_setFieldDataBlock(pField,0);					// Make sure pointer is null for the realloc call.
		void * pBlock = _reallocBlock(pField,nLines);
		
		_updateLineInfo( _header(pBlock), _lineInfo(pBlock), pBuffer, _baseStyle(pField), _state(pField) );
		_updatePreferredSize( _header(pBlock), _lineInfo(pBlock) );	
	}
	
	//____ removeField() _________________________________________________________
	
	void StandardPrinter::removeField( PrintableField * pField )
	{
		free( _fieldDataBlock(pField) );
		_setFieldDataBlock(pField, 0);
	}
	
	//____ setAlignment() __________________________________________________________
	
	void StandardPrinter::setAlignment( Origo alignment )
	{
		if( alignment != m_alignment )
		{
			m_alignment = alignment;
			
			//TODO: Make all fields dirty
		}
	}
	
	
	int StandardPrinter::charAtPos( const PrintableField * pField, Coord pos ) const
	{
		//TODO: Implement!
		return 0;
	}
	
	//_____ charPos() ______________________________________________________
	
	Coord StandardPrinter::charPos( const PrintableField * pField, int charOfs ) const
	{
		int line = charLine(pField, charOfs);
		
		int ofsX = _charPosX(pField, charOfs);
		int ofsY = _linePosY(_fieldDataBlock(pField), line, pField->size().h );
		
		const LineInfo * pLine = _lineInfo( _fieldDataBlock(pField) ) + line;		
		ofsY += pLine->base;
		
		return Coord(ofsX,ofsY);
	}
	
	//____ charRect() ________________________________________________________
	
	Rect StandardPrinter::charRect( const PrintableField * pField, int charOfs ) const
	{
		const void * pBlock = _fieldDataBlock(pField);
		const BlockHeader * pHeader = _header(pBlock);
		const LineInfo * pLineInfo = _lineInfo(pBlock);
		
		// Find correct line and determine yOfs
		
		int yOfs = _textPosY( pHeader, pField->size().h );		
		while( pLineInfo->length <= charOfs )
		{
			yOfs += pLineInfo->spacing;
			charOfs-= pLineInfo->length;
			pLineInfo++;
		}
		
		// Determine xOfs by parsing line until character
		
		int xOfs = _linePosX( pLineInfo, pField->size().w );
		
		TextAttr		baseAttr;
		_baseStyle(pField)->exportAttr( _state(pField), &baseAttr );

		const Char * pFirst = _charBuffer(pField)->chars() + pLineInfo->offset;
		const Char * pLast = pFirst + charOfs;
		
		xOfs += _charDistance( pFirst, pLast, baseAttr, _state(pField) );

		// Get cell width

		int width = 0;
		
		TextAttr	attr = baseAttr;

		if( pLast->styleHandle() != 0 )
			pLast->stylePtr()->addToAttr( _state(pField), &attr );
		
		Font * pFont = attr.pFont.rawPtr();
		pFont->setSize(attr.size);

		Glyph_p pGlyph = pFont->getGlyph(pLast->getGlyph());
		if( pGlyph )
			width = pGlyph->advance();				// Do not advance for last, just apply kerning.
		else if( pLast->getGlyph() == 32 )
			width = pFont->whitespaceAdvance();

		return Rect(xOfs,yOfs,width,pLineInfo->height);
	}

	//____ charLine() ________________________________________________________

	int StandardPrinter::charLine( const PrintableField * pField, int charOfs ) const
	{
		if( charOfs < 0 )
			return -1;
		
		const void * pBlock = _fieldDataBlock(pField);
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

	int StandardPrinter::lineBegin( const PrintableField * pField, int lineNb ) const
	{
		const void * pBlock = _fieldDataBlock(pField);
		const BlockHeader * pHeader = _header(pBlock);
		const LineInfo * pLineInfo = _lineInfo(pBlock);

		if( lineNb < 0 || lineNb >= pHeader->nbLines )
			return -1;
			
		return pLineInfo[lineNb].offset;
	}
	
	//____ lineEnd() ___________________________________________________________
	
	int StandardPrinter::lineEnd( const PrintableField * pField, int lineNb ) const
	{
		const void * pBlock = _fieldDataBlock(pField);
		const BlockHeader * pHeader = _header(pBlock);
		const LineInfo * pLineInfo = _lineInfo(pBlock);

		if( lineNb < 0 || lineNb >= pHeader->nbLines )
			return -1;
			
		return pLineInfo[lineNb].offset + pLineInfo[lineNb].length;		
	}

	//____ _charDistance() _____________________________________________________

	// Returns distance in pixels between beginning of first and beginning of last char.
	// Chars should be on the same line (or pLast could be first char on next line)

	int StandardPrinter::_charDistance( const Char * pFirst, const Char * pLast, const TextAttr& baseAttr, State state ) const
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
		
			pGlyph = pFont->getGlyph(pChar->getGlyph());

			if( pGlyph )
			{
				if( pPrevGlyph )
					distance += pFont->kerning(pPrevGlyph, pGlyph);

				if( pChar != pLast )
					distance += pGlyph->advance();				// Do not advance for last, just apply kerning.
			}
			else if( pChar->getGlyph() == 32 && pChar != pLast )
				distance += pFont->whitespaceAdvance();
				
			pPrevGlyph = pGlyph;
			pChar++;
		}

		return distance;
	}

	
	//____ _renderField()___________________________________________________________
	
	void StandardPrinter::renderField( PrintableField * pField, GfxDevice * pDevice, const Rect& canvas, const Rect& clip )
	{		
		void * pBlock = _fieldDataBlock(pField);
		BlockHeader * pHeader = _header(pBlock);
		LineInfo * pLineInfo = _lineInfo(pBlock);
		const Char * pCharArray = _charBuffer(pField)->chars();
		
		Coord lineStart = canvas.pos();
		lineStart.y += _textPosY( pHeader, canvas.h );
	
		TextAttr		baseAttr;
		_baseStyle(pField)->exportAttr( _state(pField), &baseAttr );
	
		TextAttr		attr;
		Font_p 			pFont;
		TextStyle_h		hStyle = 0xFFFF;

		Color	baseTint = pDevice->getTintColor();
		Color	localTint = Color::White;
		
		for( int i = 0 ; i < pHeader->nbLines ; i++ )
		{
			if( lineStart.y < clip.y + clip.h && lineStart.y + pLineInfo->height > clip.y )
			{		
				lineStart.x = canvas.x + _linePosX( pLineInfo, canvas.w );
				const Char * pChars = pCharArray + pLineInfo->offset;
	
				Glyph_p	pGlyph;
				Glyph_p	pPrevGlyph =  0;
				
				Coord pos = lineStart;
				pos.y += pLineInfo->base;
				
				for( int x = 0 ; x < pLineInfo->length ; x++ )
				{
					// TODO: Include handling of special characters
					// TODO: Support char textcolor and background color and effects.
				
					if( pChars->styleHandle() != hStyle )
					{
						int oldFontSize = attr.size;
						attr = baseAttr;

						if( pChars->styleHandle() != 0 )
							pChars->stylePtr()->addToAttr( _state(pField), &attr );
						
						if( pFont != attr.pFont || attr.size != oldFontSize )
						{
							pFont = attr.pFont;
							pFont->setSize(attr.size);
							pPrevGlyph = 0;								// No kerning against across different fonts or character of different size.
						}

						if( attr.color != localTint )
						{
							localTint = attr.color;
							pDevice->setTintColor( baseTint * localTint );
						}
					}
				
					pGlyph = pFont->getGlyph(pChars->getGlyph());
	
					if( pGlyph )
					{
						if( pPrevGlyph )
							pos.x += pFont->kerning(pPrevGlyph, pGlyph);

						const GlyphBitmap * pBitmap = pGlyph->getBitmap();
						pDevice->clipBlit( clip, pBitmap->pSurface, pBitmap->rect, pos.x + pBitmap->bearingX, pos.y + pBitmap->bearingY  );
	
						pos.x += pGlyph->advance();
					}
					else if( pChars->getGlyph() == 32 )
						pos.x += pFont->whitespaceAdvance();
						
					pPrevGlyph = pGlyph;
					pChars++;
				}			
			}
			
			lineStart.y += pLineInfo->spacing;
			pLineInfo++;
		}
		
		if( localTint != Color::White )
			pDevice->setTintColor( baseTint );
	}
	
	
	void StandardPrinter::onTextModified( PrintableField * pField, int ofs, int charsRemoved, int charsAdded )
	{
		onRefresh(pField);
	}
	
	void StandardPrinter::onFieldResized( PrintableField * pField, Size newSize, Size oldSize )
	{
		///TODO: Implement!
	}
	
	void StandardPrinter::onStateChanged( PrintableField * pField, State newState, State oldState )
	{
		//TODO: Implement!
	}
	
	void StandardPrinter::onStyleChanged( PrintableField * pField, TextStyle * pNewStyle, TextStyle * pOldStyle )
	{
		State state = _state(pField);
		void * pBlock = _fieldDataBlock(pField);
		
		_updateLineInfo( _header(pBlock), _lineInfo(pBlock), _charBuffer(pField), _baseStyle(pField), _state(pField) );
		_updatePreferredSize( _header(pBlock), _lineInfo(pBlock) );

		_setFieldDirty(pField);
	}
	

	void StandardPrinter::onCharStyleChanged( PrintableField * pField, int ofs, int len )
	{
		State state = _state(pField);
		void * pBlock = _fieldDataBlock(pField);
		
		_updateLineInfo( _header(pBlock), _lineInfo(pBlock), _charBuffer(pField), _baseStyle(pField), _state(pField) );
		_updatePreferredSize( _header(pBlock), _lineInfo(pBlock) );

		_setFieldDirty(pField);
	}

	
	void StandardPrinter::onRefresh( PrintableField * pField )
	{
		CharBuffer * pBuffer = _charBuffer(pField);
		int nLines = _countLines( pBuffer );
	
		void * pBlock = _fieldDataBlock(pField);
		if( !pBlock || _header(pBlock)->nbLines != nLines )
			pBlock = _reallocBlock(pField,nLines);
		
		_updateLineInfo( _header(pBlock), _lineInfo(pBlock), pBuffer, _baseStyle(pField), _state(pField) );
		_updatePreferredSize( _header(pBlock), _lineInfo(pBlock) );
	}
	
	Rect StandardPrinter::rectForRange( const PrintableField * pField, int ofs, int length ) const
	{
		//TODO: Implement!
		return Rect();
	}
	
	//____ textDirection() ____________________________________________________

	Direction StandardPrinter::textDirection( PrintableField * pField, int charOfs ) const
	{
		return Direction::Right;
	}

	//____ caretToPos() _____________________________________________________
	
	int StandardPrinter::caretToPos( PrintableField * pField, Coord pos, int& wantedLineOfs ) const
	{
		wantedLineOfs = -1;

		int line = _lineAtPosY(pField, pos.y, SelectMode::Closest );
		return  _charAtPosX(pField, line, pos.x, SelectMode::ClosestBegin );		
	}

	//____ caretUp() ___________________________________________________________
	
	int StandardPrinter::caretUp( PrintableField * pField, int charOfs, int& wantedLineOfs ) const
	{
		int line = charLine(pField, charOfs );

		if( line > 0 )
			charOfs = _charAtPosX(pField, line-1, wantedLineOfs, SelectMode::ClosestBegin );

		return charOfs;
	}

	//____ caretDown() _________________________________________________________
	
	int StandardPrinter::caretDown( PrintableField * pField, int charOfs, int& wantedLineOfs ) const
	{
		int line = charLine(pField, charOfs );

		if( line >= 0 && line < _header(_fieldDataBlock(pField))->nbLines-1 )
			charOfs = _charAtPosX(pField, line+1, wantedLineOfs, SelectMode::ClosestBegin );

		return charOfs;
	}

	//____ caretLeft() _________________________________________________________
	
	int StandardPrinter::caretLeft( PrintableField * pField, int charOfs, int& wantedLineOfs ) const
	{
		if( charOfs > 0 )
			charOfs--;
			
		wantedLineOfs = -1;
		return charOfs;
	}
	
	//____ caretRight() ________________________________________________________
	
	int StandardPrinter::caretRight( PrintableField * pField, int charOfs, int& wantedLineOfs ) const
	{
		if( charOfs < _charBuffer(pField)->length() )
			charOfs++;
			
		wantedLineOfs = -1;
		return charOfs;
	}
	
	//____ caretHome() ________________________________________________________

	int StandardPrinter::caretHome( PrintableField * pField, int charOfs, int& wantedLineOfs ) const
	{
		int line = charLine( pField, charOfs );

		if( line >= 0 )
		{
			const LineInfo * pLine = _lineInfo( _fieldDataBlock(pField) ) + line;
			charOfs = pLine->offset;
		}
		wantedLineOfs = -1;
		return charOfs;		
	}

	//____ caretEnd() ________________________________________________________
	
	int StandardPrinter::caretEnd( PrintableField * pField, int charOfs, int& wantedLineOfs ) const
	{
		int line = charLine( pField, charOfs );

		if( line >= 0 )
		{
			const LineInfo * pLine = _lineInfo( _fieldDataBlock(pField) ) + line;
			charOfs = pLine->offset+ pLine->length-1;
		}
		wantedLineOfs = -1;
		return charOfs;		
	}

	//____ tooltip() _______________________________________________________________
	
	String StandardPrinter::tooltip( const PrintableField * pField ) const
	{
		//TODO: Return the text if it overflows the field.
		
		return String();
	}
	
	
	//____ preferredSize() _________________________________________________________
	
	Size StandardPrinter::preferredSize( const PrintableField * pField ) const
	{
		return _header(_fieldDataBlock(pField))->preferredSize;
	}
	
	//____ matchingWidth() _________________________________________________________
	
	int StandardPrinter::matchingWidth( const PrintableField * pField, int height ) const
	{
		return _header(_fieldDataBlock(pField))->preferredSize.w;
	}
	
	//____ matchingHeight() ________________________________________________________
	
	int StandardPrinter::matchingHeight( const PrintableField * pField, int width ) const
	{
		//TODO: Implement correct calculation!
		
		return _header(_fieldDataBlock(pField))->preferredSize.h;
	}
	
	//____ _countLines() ___________________________________________________________
	
	int StandardPrinter::_countLines( const CharBuffer * pBuffer ) const
	{
		const Char * pChars = pBuffer->chars();
		int lines = 0;
		while( true )
		{
			if( pChars->isEndOfLine() )
			{	
				lines++;
				if( pChars->isEndOfText() )
					return lines;
			}	
			pChars++;
		}
	}
	
	//____ _reallocBlock() _________________________________________________________
	
	void * StandardPrinter::_reallocBlock( PrintableField* pField, int nLines )
	{
		void * pBlock = _fieldDataBlock(pField);
		if( pBlock )
			free( pBlock );
			
		pBlock = malloc( sizeof(BlockHeader) + sizeof(LineInfo)*nLines);
		_setFieldDataBlock(pField, pBlock);
		((BlockHeader *)pBlock)->nbLines = nLines;
		
		return pBlock;
	}
	
	
	//____ _updateLineInfo() _______________________________________________________
	
	void StandardPrinter::_updateLineInfo( BlockHeader * pHeader, LineInfo * pLines, const CharBuffer * pBuffer, const TextStyle * pBaseStyle,
												State state )
	{
		const Char * pChars = pBuffer->chars();
	
		TextAttr		baseAttr;
		pBaseStyle->exportAttr( state, &baseAttr );
	
	
		TextAttr		attr;
		Font_p 			pFont;
	
		TextStyle_h		hCharStyle = 0xFFFF;			// Force change on first character.
				
		Glyph_p	pGlyph;
		Glyph_p	pPrevGlyph;

		int maxAscend = 0;
		int maxDescend = 0;
		int maxDescendGap = 0;							// Including the line gap.
		int spaceAdv = 0;
		int width = 0;
		
		pLines->offset = pChars - pBuffer->chars();

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
			}


			// TODO: Include handling of special characters
			// TODO: Support sub/superscript.
												
			pGlyph = pFont->getGlyph(pChars->getGlyph());

			if( pGlyph )
			{
				if( pPrevGlyph )
					width += pFont->kerning(pPrevGlyph, pGlyph);

				width += pGlyph->advance();
			}
			else if( pChars->getGlyph() == 32 )
				width += spaceAdv;

			pPrevGlyph = pGlyph;
	
			//
	
			if( pChars->isEndOfLine() )
			{
				// Finish this line
				
				pLines->length = pChars - (pBuffer->chars() + pLines->offset) +1; 		// +1 to include line terminator.
					
				pLines->width = width;
				pLines->height = maxAscend + maxDescend;
				pLines->base = maxAscend;
				pLines->spacing = maxAscend + maxDescendGap;
				pLines++;			
					
				if( pChars->isEndOfText() )
					break;

				// Prepare for next line

				pChars++;			// Line terminator belongs to previous line.

				pLines->offset = pChars - pBuffer->chars();
				width = 0;
				
				if( pChars->styleHandle() != hCharStyle )
				{
					maxAscend = 0;
					maxDescend = 0;
					maxDescendGap = 0;
				}
			}
			else
				pChars++;	
		}
	}
		
	//____ _updatePreferredSize() __________________________________________________	
		
	bool StandardPrinter::_updatePreferredSize( BlockHeader * pHeader, LineInfo * pLines )
	{
		//TODO: Call onResize(), not just return value.
	
		Size size;
		
		int i;
		for( i = 0 ; i < pHeader->nbLines-1 ; i++ )
		{
			if( pLines[i].width > size.w )
				size.w = pLines[i].width;
			size.h += pLines[i].spacing;
		}
		
		if( pLines[i].width > size.w )
			size.w = pLines[i].width;
		size.h += pLines[i].height;
	
		if( size != pHeader->preferredSize )
		{
			pHeader->preferredSize = size;
			return true;
		}
	
		return false;
	}
	
	//____ _linePosX() _______________________________________________________________
	
	int StandardPrinter::_linePosX( const LineInfo * pLine, int fieldWidth ) const
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
				return (fieldWidth - pLine->width) / 2;
			case Origo::NorthEast:
			case Origo::East:
			case Origo::SouthEast:
				return fieldWidth - pLine->width;
		}	
	}

	//____ _linePosY() _______________________________________________________________
	
	int StandardPrinter::_linePosY( const void * pBlock, int line, int fieldHeight ) const
	{
		int ofsY = _textPosY( _header(pBlock), fieldHeight );
	
		const LineInfo * pL = _lineInfo(pBlock);
		for( int i = 0 ; i < line ; i++ )
			ofsY += pL[i].spacing;
			
		return ofsY;
	}
	
	//____ _textPosY() _____________________________________________________________
	
	int	StandardPrinter::_textPosY( const BlockHeader * pHeader, int fieldHeight ) const
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
				return (fieldHeight - pHeader->preferredSize.h) / 2;
			case Origo::SouthWest:
			case Origo::South:
			case Origo::SouthEast:
				return fieldHeight - pHeader->preferredSize.h;
		}	
	}

	//____ _charPosX() _________________________________________________________

	int StandardPrinter::_charPosX( const PrintableField * pField, int charOfs ) const
	{
		const LineInfo * pLine = _lineInfo( _fieldDataBlock(pField) ) + charLine(pField, charOfs);		
		const Char * pBufferStart = _charBuffer(pField)->chars();
		
		TextAttr attr;
		_baseStyle(pField)->exportAttr( _state(pField), &attr );
		
		return _linePosX( pLine, pField->size().w ) + _charDistance( pBufferStart + pLine->offset, pBufferStart + charOfs, attr, _state(pField) );
	}

	//____ _lineAtPosY() _______________________________________________________

	int StandardPrinter::_lineAtPosY( PrintableField * pField, int posY, SelectMode mode ) const
	{
		void * pBlock = _fieldDataBlock(pField);
		const BlockHeader * pHead = _header(pBlock);
		int linePosY = _textPosY( pHead, pField->size().h );
		const LineInfo * pLine = _lineInfo( _fieldDataBlock(pField) );		
		
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

	int StandardPrinter::_charAtPosX( PrintableField * pField, int line, int posX, SelectMode mode ) const
	{
		const LineInfo * pLine = _lineInfo( _fieldDataBlock(pField) ) + line;		


		int distance = _linePosX( pLine, pField->size().w );

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

		const Char * pLineBegin = _charBuffer(pField)->chars() + pLine->offset;
		State state = _state(pField);
		
		TextAttr baseAttr;
		_baseStyle(pField)->exportAttr( state, &baseAttr );
		

		TextAttr		attr;
		Font_p 			pFont;
		TextStyle_h		hStyle = 0xFFFF;

		Glyph_p	pGlyph;
		Glyph_p	pPrevGlyph =  0;
		
		const Char * pChar = pLineBegin;

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
		
			pGlyph = pFont->getGlyph(pChar->getGlyph());
			int pCharBeg = distance;

			if( pGlyph )
			{
				if( pPrevGlyph )
					distance += pFont->kerning(pPrevGlyph, pGlyph);

				distance += pGlyph->advance();
			}
			else if( pChar->getGlyph() == 32 )
				distance += pFont->whitespaceAdvance();
				
			// Check if we have passed our mark.
			
			if( distance >= posX )
			{
				switch( mode )
				{
					case SelectMode::ClosestBegin:
					{
						if( posX - pCharBeg < distance - posX )
							return pChar - pLineBegin;
						else
							return pChar+1 - pLineBegin;
					}
					case SelectMode::ClosestEnd:
					{
						if( posX - pCharBeg < distance - posX )
							return pChar == pLineBegin ? 0 : pChar-1 - pLineBegin;
						else
							return pChar - pLineBegin;
					}
					break;
					case SelectMode::Closest:
					case SelectMode::Marked:
					{
						return pChar - pLineBegin;
					}
				}
			}

			pPrevGlyph = pGlyph;
			pChar++;
		}

		return -1;			// We should never get here...
	}


} // namespace wg
