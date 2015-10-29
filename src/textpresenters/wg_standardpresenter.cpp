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

#include <wg_standardpresenter.h>
#include <wg_presentablefield.h>
#include <wg_textstyle.h>
#include <wg_gfxdevice.h>
#include <wg_char.h>
#include <malloc.h>

namespace wg 
{
	
	const char StandardPresenter::CLASSNAME[] = {"StandardPresenter"};
	
	//____ Constructor _____________________________________________________________
	
	StandardPresenter::StandardPresenter() : m_alignment(Origo::NorthWest)
	{
	}
	
	//____ Destructor ______________________________________________________________
	
	StandardPresenter::~StandardPresenter()
	{
	}
	
	
	//____ isInstanceOf() _________________________________________________________
	
	bool StandardPresenter::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return TextPresenter::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * StandardPresenter::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	StandardPresenter_p StandardPresenter::cast( const Object_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return StandardPresenter_p( static_cast<StandardPresenter*>(pObject.rawPtr()) );
	
		return 0;
	}
	
	//____ addField() _________________________________________________________
	
	void StandardPresenter::addField( PresentableField * pField )
	{
		CharBuffer * pBuffer = _charBuffer(pField);
		int nLines = _countLines( pBuffer );
	
		_setFieldDataBlock(pField,0);					// Make sure pointer is null for the realloc call.
		void * pBlock = _reallocBlock(pField,nLines);
		
		_updateLineInfo( _header(pBlock), _lineInfo(pBlock), pBuffer, _baseStyle(pField), _state(pField) );
		_updatePreferredSize( _header(pBlock), _lineInfo(pBlock) );	
	}
	
	//____ removeField() _________________________________________________________
	
	void StandardPresenter::removeField( PresentableField * pField )
	{
		free( _fieldDataBlock(pField) );
		_setFieldDataBlock(pField, 0);
	}
	
	//____ setAlignment() __________________________________________________________
	
	void StandardPresenter::setAlignment( Origo alignment )
	{
		if( alignment != m_alignment )
		{
			m_alignment = alignment;
			
			//TODO: Make all fields dirty
		}
	}
	
	
	int StandardPresenter::coordToChar( const PresentableField * pField, Coord pos )
	{
		//TODO: Implement!
		return 0;
	}
	
	Rect StandardPresenter::charToRect( const PresentableField * pField, int charOfs )
	{
		//TODO: Implement!
		return Rect();
	}
	
	int StandardPresenter::coordToCaretPos( PresentableField * pField, Coord pos )
	{
		//TODO: Implement!
		return 0;
	}
	
	void StandardPresenter::renderField( PresentableField * pField, GfxDevice * pDevice, const Rect& canvas, const Rect& clip )
	{
		void * pBlock = _fieldDataBlock(pField);
		BlockHeader * pHeader = _header(pBlock);
		LineInfo * pLineInfo = _lineInfo(pBlock);
		const Char * pCharArray = _charBuffer(pField)->chars();
		
		Coord lineStart = canvas.pos();
		lineStart.y += _textOfsY( pHeader, canvas.h );
	
		TextAttr2		baseAttr;
		_baseStyle(pField)->exportAttr( _state(pField), &baseAttr );
	
		TextAttr2		attr = baseAttr;
		Font_p pFont = attr.pFont;
		Glyphset_p pGlyphSet = pFont->getGlyphset( FontAlt::Normal, attr.size);
		
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
					// TODO: Support char-style changes.
				
					pGlyph = pGlyphSet->getGlyph(pChars->getGlyph(),attr.size);
	
					if( pGlyph )
					{
						const GlyphBitmap * pBitmap = pGlyph->getBitmap();
						pDevice->clipBlit( clip, pBitmap->pSurface, pBitmap->rect, pos.x + pBitmap->bearingX, pos.y + pBitmap->bearingY  );
	
						pos.x += pGlyph->advance();
						if( pPrevGlyph )
							pos.x += pGlyphSet->getKerning(pPrevGlyph, pGlyph, attr.size);
					}
					else if( pChars->getGlyph() == 32 )
						pos.x += pGlyphSet->getWhitespaceAdvance( attr.size );
						
					pPrevGlyph = pGlyph;
					pChars++;
				}			
			}
			
			lineStart.y += pLineInfo->spacing;
			pLineInfo++;
		}
		
	}
	
	
	void StandardPresenter::onTextModified( PresentableField * pField, int ofs, int charsRemoved, int charsAdded )
	{
		onRefresh(pField);
	}
	
	void StandardPresenter::onFieldResize( PresentableField * pField, Size newSize )
	{
		// Do nothing?
	}
	
	void StandardPresenter::onStateChange( PresentableField * pField, State newState, State oldState )
	{
	}
	
	void StandardPresenter::onStyleChange( PresentableField * pField )
	{
	}
	
	
	
	void StandardPresenter::onRefresh( PresentableField * pField )
	{
		CharBuffer * pBuffer = _charBuffer(pField);
		int nLines = _countLines( pBuffer );
	
		void * pBlock = _fieldDataBlock(pField);
		if( !pBlock || _header(pBlock)->nbLines != nLines )
			pBlock = _reallocBlock(pField,nLines);
		
		_updateLineInfo( _header(pBlock), _lineInfo(pBlock), pBuffer, _baseStyle(pField), _state(pField) );
		_updatePreferredSize( _header(pBlock), _lineInfo(pBlock) );
	}
	
	int StandardPresenter::moveCaret( PresentableField * pField, int caretOfs, int wantedPixelOfs, int verticalSteps, int horizontalSteps, WgModifierKeys modif )
	{
		//TODO: Implement!
		return 0;
	}
	
	Rect StandardPresenter::rectForRange( const PresentableField * pField, int ofs, int length ) const
	{
		//TODO: Implement!
		return Rect();
	}
	
	//____ tooltip() _______________________________________________________________
	
	String StandardPresenter::tooltip( const PresentableField * pField ) const
	{
		//TODO: Return the text if it overflows the field.
		
		return String();
	}
	
	
	//____ preferredSize() _________________________________________________________
	
	Size StandardPresenter::preferredSize( const PresentableField * pField ) const
	{
		return _header(_fieldDataBlock(pField))->preferredSize;
	}
	
	//____ matchingWidth() _________________________________________________________
	
	int StandardPresenter::matchingWidth( const PresentableField * pField, int height ) const
	{
		return _header(_fieldDataBlock(pField))->preferredSize.w;
	}
	
	//____ matchingHeight() ________________________________________________________
	
	int StandardPresenter::matchingHeight( const PresentableField * pField, int width ) const
	{
		return _header(_fieldDataBlock(pField))->preferredSize.h;
	}
	
	//____ _countLines() ___________________________________________________________
	
	int StandardPresenter::_countLines( const CharBuffer * pBuffer )
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
	
	void * StandardPresenter::_reallocBlock( PresentableField* pField, int nLines )
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
	
	void StandardPresenter::_updateLineInfo( BlockHeader * pHeader, LineInfo * pLines, const CharBuffer * pBuffer, const TextStyle * pBaseStyle,
												State state )
	{
		const Char * pChars = pBuffer->chars();
	
		TextAttr2		baseAttr;
		pBaseStyle->exportAttr( state, &baseAttr );
	
	
		TextAttr2		attr = baseAttr;
		Font_p pFont = attr.pFont;
		Glyphset_p pGlyphSet = pFont->getGlyphset(FontAlt::Normal, attr.size);
	
		
		
		while( true )
		{
			pLines->offset = pChars - pBuffer->chars();
	
			int	width = 0;
			int height = pGlyphSet->getHeight(attr.size);
			int spacing = pGlyphSet->getLineSpacing(attr.size);
			int base = pGlyphSet->getBaseline(attr.size);
	
			Glyph_p	pGlyph;
			Glyph_p	pPrevGlyph;
	
			while( !pChars->isEndOfLine() )
			{
				// TODO: Include handling of special characters
				// TODO: Change loop, needs to include EOL character in line.
				// TODO: Support char-style changes.
				
				pGlyph = pGlyphSet->getGlyph(pChars->getGlyph(),attr.size);
	
				if( pGlyph )
				{
					width += pGlyph->advance();
					if( pPrevGlyph )
						width += pGlyphSet->getKerning(pPrevGlyph, pGlyph, attr.size);
				}
				else if( pChars->getGlyph() == 32 )
					width += pGlyphSet->getWhitespaceAdvance(attr.size);
	
				pPrevGlyph = pGlyph;
				pChars++;
			}
			pLines->length = pChars - (pBuffer->chars() + pLines->offset) +1; 		// +1 to include line terminator.
				
			pLines->width = width;
			pLines->height = height;
			pLines->base = base;
			pLines->spacing = spacing;
			pLines++;			
				
			if( pChars->isEndOfText() )
				break;
	
			pChars++;		
		}
	}
		
	//____ _updatePreferredSize() __________________________________________________	
		
	bool StandardPresenter::_updatePreferredSize( BlockHeader * pHeader, LineInfo * pLines )
	{	
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
	
	int StandardPresenter::_lineOfsX( LineInfo * pLine, int fieldWidth ) const
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
	
	int	StandardPresenter::_textOfsY( BlockHeader * pHeader, int fieldHeight ) const
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
