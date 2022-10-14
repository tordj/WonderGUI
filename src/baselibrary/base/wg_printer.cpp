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
#include <wg_printer.h>

namespace wg
{

	const TypeInfo Printer::TYPEINFO = { "Printer", &Object::TYPEINFO };

	const static int g_placementMulX[Placement_size] = { 0, 1, 2, 2, 2, 1, 0, 0, 1 };
	const static int g_placementMulY[Placement_size] = { 0, 0, 0, 1, 2, 2, 2, 1, 1 };


	//____ Create _________________________________________________________________

	Printer_p Printer::create()
	{
		Printer * pPrinter = new Printer();
		return Printer_p(pPrinter);
	}

	//____ constructor ____________________________________________________________

	Printer::Printer()
	{
	}


	//____ typeInfo() _________________________________________________________

	const TypeInfo& Printer::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setGfxDevice() ______________________________________________________

	void Printer::setGfxDevice(GfxDevice * pDevice)
	{
		m_pGfxDevice = pDevice;
	}

	//____ setFont() _____________________________________________________________

	void Printer::setFont(Font * pFont)
	{
		m_pFont = pFont;
	}

	//____ setCursorOrigo() ____________________________________________________________

	void Printer::setCursorOrigo( CoordSPX origo )
	{
		m_cursorOrigo = origo;
	}

	//____ setLineWidth() ________________________________________________________

	void Printer::setLineWidth( spx width )
	{
		m_lineWidth = width;
	}

	//____ resetCursor() _________________________________________________________

	void Printer::resetCursor()
	{
		m_cursorPos = m_cursorOrigo;
	}

	//____ setCursor() ___________________________________________________________

	void Printer::setCursor( CoordSPX pos )
	{
		m_cursorPos = pos;
	}

	//____ crlf() ________________________________________________________________

	void Printer::crlf()
	{
		if( m_pFont )
		{
			m_cursorPos.x = m_cursorOrigo.x;
			m_cursorPos.y += m_pFont->maxAscend() + m_pFont->maxDescend() + m_pFont->lineGap();
		}
	}

	void Printer::crlf( spx maxFontSizeThisLine, spx maxFontSizeNextLine )
	{
		if( m_pFont )
		{
			m_cursorPos.x = m_cursorOrigo.x;
			spx oldSize = m_pFont->size();
			m_pFont->setSize(maxFontSizeThisLine);
			m_cursorPos.y += m_pFont->maxDescend() + m_pFont->lineGap();
			m_pFont->setSize(maxFontSizeNextLine);
			m_cursorPos.y += m_pFont->maxAscend();
			m_pFont->setSize(oldSize);
			
		}
	}

	//____ printAligned() __________________________________________________

	void Printer::printAligned( Placement xAlignment, const char * pText )
	{
		m_cursorPos = _printAligned(m_cursorPos, m_lineWidth, xAlignment, pText);
	}

	//____ print() ________________________________________________________________

	void Printer::print( const char * pText )
	{
		m_cursorPos = _print(m_cursorPos, pText, nullptr, m_cursorOrigo.x );
	}

	//____ lineHeight() __________________________________________________________

	spx Printer::lineHeight()
	{
		if( m_pFont )
			return m_pFont->maxAscend() + m_pFont->maxDescend() + m_pFont->lineGap();
		else
			return 0;
	}

	//____ textSize() _____________________________________________________________

	SizeSPX Printer::textSize( const char * pText )
	{
		return _textSize(pText, nullptr);
	}

	//____ printAt() ______________________________________________________________

	CoordSPX Printer::printAt( CoordSPX pos, const char * pText )
	{
		return _print(pos, pText, nullptr, pos.x );
	}

	//____ printInBox() ____________________________________________________________

	void Printer::printInBox( const RectSPX& box, Placement alignment, const char * pText )
	{
		SizeSPX wholeTextSize = _textSize(pText, nullptr);
		
		int yMul = g_placementMulY[int(alignment)];

		CoordSPX pos = box.pos();
		pos.y += (box.h - wholeTextSize.h) * yMul / 2;
		pos.y += m_pFont->maxAscend();
		
		_printAligned(pos, box.w, alignment, pText);
	}

	//____ _print() ______________________________________________________________

	CoordSPX Printer::_print( CoordSPX pos, const char * pBegin, const char * pEnd, spx origoX )
	{
		if( !m_pGfxDevice || !m_pFont )
			return pos;
				
		Glyph	glyphs[2];

		Glyph * pPrevGlyph = &glyphs[0];
		Glyph * pGlyph = &glyphs[1];
		
		pPrevGlyph->fontRef = nullptr;
		
		const char * p = pBegin;
		while( p != pEnd )
		{
			uint32_t chr = _parseChar(p);

			if( chr <= 32 )
			{
				if( chr == 32 )	// SPACE
					pos.x += m_pFont->whitespaceAdvance();
				else if( chr == 10 )	// LF
				{
					pos.x = origoX;
					pos.y += m_pFont->maxAscend() + m_pFont->maxDescend() + m_pFont->lineGap();
				}
				else if( chr == 9 )	// TAB
				{
					spx tablen = m_pFont->whitespaceAdvance()*8;
					pos.x = pos.x - ((pos.x-origoX) % tablen) + tablen;
				}
				else if( chr == 0 )
				{
					return pos;
				}
					
				pPrevGlyph->fontRef = nullptr;
			}
			 else
			{
				m_pFont->getGlyphWithBitmap(chr, *pGlyph);

				CoordSPX blitPos = pos;
				blitPos.x += pGlyph->bearingX;
				blitPos.y += pGlyph->bearingY;
				
				m_pGfxDevice->setBlitSource(pGlyph->pSurface);
				m_pGfxDevice->blit(blitPos,pGlyph->rect);

				pos.x += pGlyph->advance;

				if( pGlyph->fontRef && pPrevGlyph->fontRef )
					pos.x += m_pFont->kerning(*pPrevGlyph, *pGlyph);
			}
			std::swap(pGlyph,pPrevGlyph);
		}
		return pos;
	}

	//____ _printAligned() __________________________________________________

	CoordSPX Printer::_printAligned( CoordSPX pos, spx lineWidth, Placement xAlign, const char * pBegin )
	{
		int mul = g_placementMulX[int(xAlign)];

		while( * pBegin != 0 )
		{
			auto pEnd = _lineEnd(pBegin);
			
			SizeSPX lineSize = _textSize(pBegin,pEnd);

			CoordSPX printPos = pos;
			printPos.x += (lineWidth - lineSize.w)*mul/2;
			
			_print(printPos, pBegin, pEnd, printPos.x );
			pos.y += lineSize.h;
			pos.y += m_pFont->lineGap();
			
			pBegin = pEnd;

			if( * pBegin == 13 )
				pBegin++;

			if( * pBegin == 10 )
				pBegin++;

			if( * pBegin == 13 )
				pBegin++;
		}

		return pos;
	}


	//____ _parseChar() __________________________________________________________

	uint32_t Printer::_parseChar( const char *& pStr )
	{
		uint32_t g = * ((const unsigned char *&)pStr)++;

		if( g < 128 )
			return g;											// 1 byte character (7 bits data).

		if( (g & 0xE0) == 0xC0 )					// 2 bytes character (11 bits data).
		{
			g &= 0x1F;
			g <<= 6;
			g += ((*pStr++) & 0x3F);
		}
		else if( (g & 0xF0) == 0xE0 )			// 3 bytes character (16 bits data).
		{
			g &= 0x0F;
			g <<= 6;
			g += ((*pStr++) & 0x3F);
			g <<= 6;
			g += ((*pStr++) & 0x3F);
		}
		else if( (g & 0xF8) == 0xF0 )			// 4 bytes character (21 bits data).
		{
			g &= 0x07;
			g <<= 6;
			g += ((*pStr++) & 0x3F);
			g <<= 6;
			g += ((*pStr++) & 0x3F);
			g <<= 6;
			g += ((*pStr++) & 0x3F);
		}
		else
			return 0xFFFFFFFF;					// Broken character (or more than 4 bytes which we don't support)

	}

	//____ _lineEnd() ____________________________________________________________

	const char * Printer::_lineEnd( const char * pPos )
	{
		while( * pPos != 0 && * pPos != 10 )
			pPos++;

		return pPos;
	}

	//____ _textSize() ___________________________________________________________

	SizeSPX Printer::_textSize( const char * pBegin, const char * pEnd )
	{
		if( !m_pGfxDevice || !m_pFont )
			return SizeSPX();
				
		Glyph	glyphs[2];

		Glyph * pPrevGlyph = &glyphs[0];
		Glyph * pGlyph = &glyphs[1];
		
		pPrevGlyph->fontRef = nullptr;
	
		CoordSPX pos;
		pos.y += m_pFont->maxAscend();
		
		spx maxX = 0;
		
		const char * p = pBegin;
		while( p != pEnd && * p != 0 )
		{
			uint32_t chr = _parseChar(p);

			if( chr <= 32 )
			{
				if( chr == 32 )	// SPACE
					pos.x += m_pFont->whitespaceAdvance();
				else if( chr == 10 )	// LF
				{
					if( pos.x > maxX )
						maxX = pos.x;
					pos.x = 0;
					pos.y += m_pFont->maxDescend() + m_pFont->lineGap() + m_pFont->maxAscend();
				}
				else if( chr == 9 )	// TAB
				{
					spx tablen = m_pFont->whitespaceAdvance()*8;
					pos.x = pos.x - (pos.x % tablen) + tablen;
				}
					
				pPrevGlyph->fontRef = nullptr;
			}
			 else
			{
				m_pFont->getGlyphWithoutBitmap(chr, *pGlyph);

				pos.x += pGlyph->advance;
				if( pGlyph->fontRef && pPrevGlyph->fontRef )
					pos.x += m_pFont->kerning(*pPrevGlyph, *pGlyph);
			}
			std::swap(pGlyph,pPrevGlyph);
		}

		pos.y += m_pFont->maxDescend();
		
		if( pos.x > maxX )
			maxX = pos.x;
		
		return SizeSPX(maxX, pos.y);
	}


} // namespace wg
