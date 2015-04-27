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

const char WgStandardPresenter::CLASSNAME[] = {"StandardPresenter"};

//____ Constructor _____________________________________________________________

WgStandardPresenter::WgStandardPresenter() : m_alignment(WG_NORTHWEST)
{
}

//____ Destructor ______________________________________________________________

WgStandardPresenter::~WgStandardPresenter()
{
}


//____ IsInstanceOf() _________________________________________________________

bool WgStandardPresenter::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgTextPresenter::IsInstanceOf(pClassName);
}

//____ ClassName() ____________________________________________________________

const char * WgStandardPresenter::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ Cast() _________________________________________________________________

WgStandardPresenterPtr WgStandardPresenter::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgStandardPresenterPtr( static_cast<WgStandardPresenter*>(pObject.RawPtr()) );

	return 0;
}

//____ AddField() _________________________________________________________

void WgStandardPresenter::AddField( WgPresentableField * pField )
{
	WgCharBuffer * pBuffer = _charBuffer(pField);
	int nLines = _countLines( pBuffer );

	_setFieldDataBlock(pField,0);					// Make sure pointer is null for the realloc call.
	void * pBlock = _reallocBlock(pField,nLines);
	
	_updateLineInfo( _header(pBlock), _lineInfo(pBlock), pBuffer, _baseStyle(pField), _state(pField) );
	_updatePreferredSize( _header(pBlock), _lineInfo(pBlock) );	
}

//____ RemoveField() _________________________________________________________

void WgStandardPresenter::RemoveField( WgPresentableField * pField )
{
	free( _fieldDataBlock(pField) );
	_setFieldDataBlock(pField, 0);
}

//____ SetAlignment() __________________________________________________________

void WgStandardPresenter::SetAlignment( WgOrigo alignment )
{
	if( alignment != m_alignment )
	{
		m_alignment = alignment;
		
		//TODO: Make all fields dirty
	}
}


int WgStandardPresenter::CoordToChar( const WgPresentableField * pField, WgCoord pos )
{
}

WgRect WgStandardPresenter::CharToRect( const WgPresentableField * pField, int charOfs )
{
}

int WgStandardPresenter::CoordToCaretPos( WgPresentableField * pField, WgCoord pos )
{
}

void WgStandardPresenter::RenderField( WgPresentableField * pField, WgGfxDevice * pDevice, WgEditState * pEditState, const WgRect& canvas, const WgRect& clip )
{
	void * pBlock = _fieldDataBlock(pField);
	BlockHeader * pHeader = _header(pBlock);
	LineInfo * pLineInfo = _lineInfo(pBlock);
	const WgChar * pCharArray = _charBuffer(pField)->Chars();
	
	WgCoord lineStart = canvas.Pos();
	lineStart.y += _textOfsY( pHeader, canvas.h );

	WgTextAttr2		baseAttr;
	_baseStyle(pField)->ExportAttr( _state(pField), &baseAttr );

	WgTextAttr2		attr = baseAttr;
	WgFontPtr pFont = attr.pFont;
	WgGlyphsetPtr pGlyphSet = pFont->GetGlyphset(attr.fontStyle, attr.size);
	
	for( int i = 0 ; i < pHeader->nbLines ; i++ )
	{
		if( lineStart.y < clip.y + clip.h && lineStart.y + pLineInfo->height > clip.y )
		{		
			lineStart.x = canvas.x + _lineOfsX( pLineInfo, canvas.w );
			const WgChar * pChars = pCharArray + pLineInfo->offset;

			WgGlyphPtr	pGlyph;
			WgGlyphPtr	pPrevGlyph;
			
			WgCoord pos = lineStart;
			pos.y += pLineInfo->base;
			
			for( int x = 0 ; x < pLineInfo->length ; x++ )
			{
				// TODO: Include handling of special characters
				// TODO: Support char-style changes.
			
				pGlyph = pGlyphSet->GetGlyph(pChars->Glyph(),attr.size);

				if( pGlyph )
				{
					const WgGlyphBitmap * pBitmap = pGlyph->GetBitmap();
					pDevice->ClipBlit( clip, pBitmap->pSurface, pBitmap->rect, pos.x + pBitmap->bearingX, pos.y + pBitmap->bearingY  );

					pos.x += pGlyph->Advance();
					if( pPrevGlyph )
						pos.x += pGlyphSet->GetKerning(pPrevGlyph, pGlyph, attr.size);
				}
				else if( pChars->Glyph() == 32 )
					pos.x += pGlyphSet->GetWhitespaceAdvance( attr.size );
					
				pPrevGlyph = pGlyph;
				pChars++;
			}			
		}
		
		lineStart.y += pLineInfo->spacing;
		pLineInfo++;
	}
	
}


void WgStandardPresenter::OnTextModified( WgPresentableField * pField, int ofs, int charsRemoved, int charsAdded )
{
	OnRefresh(pField);
}

void WgStandardPresenter::OnFieldResize( WgPresentableField * pField, WgSize newSize )
{
	// Do nothing?
}

void WgStandardPresenter::OnStateChange( WgPresentableField * pField, WgState newState, WgState oldState )
{
}

void WgStandardPresenter::OnStyleChange( WgPresentableField * pField )
{
}



void WgStandardPresenter::OnRefresh( WgPresentableField * pField )
{
	WgCharBuffer * pBuffer = _charBuffer(pField);
	int nLines = _countLines( pBuffer );

	void * pBlock = _fieldDataBlock(pField);
	if( !pBlock || _header(pBlock)->nbLines != nLines )
		pBlock = _reallocBlock(pField,nLines);
	
	_updateLineInfo( _header(pBlock), _lineInfo(pBlock), pBuffer, _baseStyle(pField), _state(pField) );
	_updatePreferredSize( _header(pBlock), _lineInfo(pBlock) );
}

int WgStandardPresenter::MoveCaret( WgPresentableField * pField, WgEditState * pEditState, int verticalSteps, int horizontalSteps, WgModifierKeys modif )
{
}

WgRect WgStandardPresenter::RectForRange( const WgPresentableField * pField, int ofs, int length ) const
{
}

//____ Tooltip() _______________________________________________________________

WgString WgStandardPresenter::Tooltip( const WgPresentableField * pField ) const
{
	//TODO: Return the text if it overflows the field.
	
	return WgString();
}


//____ PreferredSize() _________________________________________________________

WgSize WgStandardPresenter::PreferredSize( const WgPresentableField * pField ) const
{
	return _header(_fieldDataBlock(pField))->preferredSize;
}

//____ MatchingWidth() _________________________________________________________

int WgStandardPresenter::MatchingWidth( const WgPresentableField * pField, int height ) const
{
	return _header(_fieldDataBlock(pField))->preferredSize.w;
}

//____ MatchingHeight() ________________________________________________________

int WgStandardPresenter::MatchingHeight( const WgPresentableField * pField, int width ) const
{
	return _header(_fieldDataBlock(pField))->preferredSize.h;
}

//____ _countLines() ___________________________________________________________

int WgStandardPresenter::_countLines( const WgCharBuffer * pBuffer )
{
	const WgChar * pChars = pBuffer->Chars();
	int lines = 0;
	while( true )
	{
		if( pChars->IsEndOfLine() )
		{	
			lines++;
			if( pChars->IsEndOfText() )
				return lines;
		}	
		pChars++;
	}
}

//____ _reallocBlock() _________________________________________________________

void * WgStandardPresenter::_reallocBlock( WgPresentableField* pField, int nLines )
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

void WgStandardPresenter::_updateLineInfo( BlockHeader * pHeader, LineInfo * pLines, const WgCharBuffer * pBuffer, const WgTextStyle * pBaseStyle,
											WgState state )
{
	const WgChar * pChars = pBuffer->Chars();

	WgTextAttr2		baseAttr;
	pBaseStyle->ExportAttr( state, &baseAttr );


	WgTextAttr2		attr = baseAttr;
	WgFontPtr pFont = attr.pFont;
	WgGlyphsetPtr pGlyphSet = pFont->GetGlyphset(attr.fontStyle, attr.size);

	
	
	while( true )
	{
		pLines->offset = pChars - pBuffer->Chars();

		int	width = 0;
		int height = pGlyphSet->GetHeight(attr.size);
		int spacing = pGlyphSet->GetLineSpacing(attr.size);
		int base = pGlyphSet->GetBaseline(attr.size);

		WgGlyphPtr	pGlyph;
		WgGlyphPtr	pPrevGlyph;

		while( !pChars->IsEndOfLine() )
		{
			// TODO: Include handling of special characters
			// TODO: Change loop, needs to include EOL character in line.
			// TODO: Support char-style changes.
			
			pGlyph = pGlyphSet->GetGlyph(pChars->Glyph(),attr.size);

			if( pGlyph )
			{
				width += pGlyph->Advance();
				if( pPrevGlyph )
					width += pGlyphSet->GetKerning(pPrevGlyph, pGlyph, attr.size);
			}
			else if( pChars->Glyph() == 32 )
				width += pGlyphSet->GetWhitespaceAdvance(attr.size);

			pPrevGlyph = pGlyph;
			pChars++;
		}
		pLines->length = pChars - (pBuffer->Chars() + pLines->offset) +1; 		// +1 to include line terminator.
			
		pLines->width = width;
		pLines->height = height;
		pLines->base = base;
		pLines->spacing = spacing;
		pLines++;			
			
		if( pChars->IsEndOfText() )
			break;

		pChars++;		
	}
}
	
//____ _updatePreferredSize() __________________________________________________	
	
bool WgStandardPresenter::_updatePreferredSize( BlockHeader * pHeader, LineInfo * pLines )
{	
	WgSize size;
	
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

int WgStandardPresenter::_lineOfsX( LineInfo * pLine, int fieldWidth ) const
{
	switch( m_alignment )
	{
		case WG_NORTHWEST:
		case WG_WEST:
		case WG_SOUTHWEST:
		default:
			return 0;
		case WG_NORTH:
		case WG_CENTER:
		case WG_SOUTH:
			return (fieldWidth - pLine->width) / 2;
		case WG_NORTHEAST:
		case WG_EAST:
		case WG_SOUTHEAST:
			return fieldWidth - pLine->width;
	}	
}

//____ _textOfsY() _____________________________________________________________

int	WgStandardPresenter::_textOfsY( BlockHeader * pHeader, int fieldHeight ) const
{
	switch( m_alignment )
	{
		case WG_NORTHWEST:
		case WG_NORTH:
		case WG_NORTHEAST:
		default:
			return 0;
		case WG_WEST:
		case WG_CENTER:
		case WG_EAST:
			return (fieldHeight - pHeader->preferredSize.h) / 2;
		case WG_SOUTHWEST:
		case WG_SOUTH:
		case WG_SOUTHEAST:
			return fieldHeight - pHeader->preferredSize.h;
	}	
}