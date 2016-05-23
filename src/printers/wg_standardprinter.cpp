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
	
	
	int StandardPrinter::coordToChar( const PrintableField * pField, Coord pos )
	{
		//TODO: Implement!
		return 0;
	}
	
	Coord StandardPrinter::charToCoord( const PrintableField * pField, int charOfs )
	{
		//TODO: Implement!
		return Coord();
	}
	
	//____ charToRect() ________________________________________________________
	
	Rect StandardPrinter::charToRect( const PrintableField * pField, int charOfs )
	{
		const void * pBlock = _fieldDataBlock(pField);
		const BlockHeader * pHeader = _header(pBlock);
		const LineInfo * pLineInfo = _lineInfo(pBlock);
		
		
		
		// Find correct line and determine yOfs
		
		int yOfs = _textOfsY( pHeader, pField->size().h );		
		while( pLineInfo->length <= charOfs )
		{
			yOfs += pLineInfo->spacing;
			charOfs-= pLineInfo->length;
			pLineInfo++;
		}
		
		// Determine xOfs by parsing line until character
		
		int xOfs = _lineOfsX( pLineInfo, pField->size().w );
		
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

	//____ lineOfChar() ________________________________________________________

	int StandardPrinter::lineOfChar( const PrintableField * pField, int charOfs )
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

	int StandardPrinter::lineBegin( const PrintableField * pField, int lineNb )
	{
		const void * pBlock = _fieldDataBlock(pField);
		const BlockHeader * pHeader = _header(pBlock);
		const LineInfo * pLineInfo = _lineInfo(pBlock);

		if( lineNb < 0 || lineNb >= pHeader->nbLines )
			return -1;
			
		return pLineInfo[lineNb].offset;
	}
	
	//____ lineEnd() ___________________________________________________________
	
	int StandardPrinter::lineEnd( const PrintableField * pField, int lineNb )
	{
		const void * pBlock = _fieldDataBlock(pField);
		const BlockHeader * pHeader = _header(pBlock);
		const LineInfo * pLineInfo = _lineInfo(pBlock);

		if( lineNb < 0 || lineNb >= pHeader->nbLines )
			return -1;
			
		return pLineInfo[lineNb].offset + pLineInfo[lineNb].length;		
	}


	//____ _coordToCaretPos() _____________________________________________________
	
	int StandardPrinter::coordToCaretPos( PrintableField * pField, Coord pos )
	{
		//TODO: Implement!
		return 0;
	}

	//____ _charDistance() _____________________________________________________

	// Returns distance in pixels between beginning of first and beginning of last char.
	// Chars should be on the same line (or pLast could be first char on next line)

	int StandardPrinter::_charDistance( const Char * pFirst, const Char * pLast, const TextAttr& baseAttr, State state )
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
		lineStart.y += _textOfsY( pHeader, canvas.h );
	
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
				lineStart.x = canvas.x + _lineOfsX( pLineInfo, canvas.w );
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
		// Do nothing?
	}
	
	void StandardPrinter::onStateChanged( PrintableField * pField, State newState, State oldState )
	{
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
	
	int StandardPrinter::moveCaret( PrintableField * pField, int caretOfs, int wantedPixelOfs, int verticalSteps, int horizontalSteps, ModifierKeys modif )
	{
		//TODO: Implement!
		return 0;
	}
	
	Rect StandardPrinter::rectForRange( const PrintableField * pField, int ofs, int length ) const
	{
		//TODO: Implement!
		return Rect();
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
		return _header(_fieldDataBlock(pField))->preferredSize.h;
	}
	
	//____ _countLines() ___________________________________________________________
	
	int StandardPrinter::_countLines( const CharBuffer * pBuffer )
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
	
	//____ _lineOfsX() _______________________________________________________________
	
	int StandardPrinter::_lineOfsX( const LineInfo * pLine, int fieldWidth ) const
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
	
	//____ _textOfsY() _____________________________________________________________
	
	int	StandardPrinter::_textOfsY( const BlockHeader * pHeader, int fieldHeight ) const
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

} // namespace wg
