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
/*
	TODO: Support kerning.
	TODO: Support multiple font-files and surfaces.
	TODO: Support fallback-font.
	TODO: Set advance on whitespace glyph from whitespace-advance.
	TODO: Support charGap, just like lineGap, add onto all glyphs advance.
	TODO: Support multiple sizes?
	TODO: Support stretch-blitting?
*/

#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <wg_bitmapfont.h>
#include <wg_texttool.h>
#include <wg_gridwalker.h>

namespace wg
{
	const TypeInfo	BitmapFont::TYPEINFO = { "BitmapFont", &Font::TYPEINFO };

	//____ constructor ____________________________________________________________

	BitmapFont::BitmapFont(Surface* pSurf, char* pGlyphSpec)
	{
		m_nKerningGlyphs= 0;
		m_pKerningTable = 0;

		m_nGlyphs		= 0;
		m_bMonospace	= true;
		m_bMonochrome	= true;
		m_maxAdvance	= 0;
		m_spaceAdvance	= 0;
		m_lineGap		= 0;
		m_maxAscend		= 0;
		m_maxDescend	= 0;
		m_size			= 0;

		for( int i = 0 ; i < 256 ; i++ )
			m_glyphTab[i] = nullptr;

		_insertGlyphs(pSurf, pGlyphSpec);
	}


	//____ Destructor _____________________________________________________________

	BitmapFont::~BitmapFont()
	{
		for( int i = 0 ; i < 256 ; i++ )
		{
			if( m_glyphTab[i] )
				delete [] m_glyphTab[i];
		}

		if( m_pKerningTable )
			delete [] m_pKerningTable;
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& BitmapFont::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ hasGlyph() _________________________________________________

	inline bool BitmapFont::hasGlyph( uint16_t chr )
	{
		MyGlyph * pGlyph = m_glyphTab[chr >> 8];

		if( pGlyph )
		{
			pGlyph += (chr & 0xFF);
			if( pGlyph->pSurface )
				return true;
		}

		return false;
	}


	//____ getGlyphWithoutBitmap() _________________________________________________________

	void BitmapFont::getGlyphWithoutBitmap(uint16_t chr, Glyph& glyph)
	{
		MyGlyph * pGlyph = m_glyphTab[chr >> 8];

		if( pGlyph )
		{
			pGlyph += (chr & 0xFF);
			if (pGlyph->pSurface)
			{
				glyph.pFont = this;
				glyph.advance = pGlyph->advance;
				glyph.kerningIndex = pGlyph->kerningIndex;
				return;
			}
		}

		glyph.pFont = 0;
		glyph.advance = 0;
		glyph.kerningIndex = 0;
		return;
	}

	//____ getGlyphWithBitmap() _________________________________________________________

	void BitmapFont::getGlyphWithBitmap(uint16_t chr, Glyph& glyph)
	{
		MyGlyph* pGlyph = m_glyphTab[chr >> 8];

		if (pGlyph)
		{
			pGlyph += (chr & 0xFF);
			if (pGlyph->pSurface)
			{
				glyph.pFont			= this;
				glyph.advance		= pGlyph->advance;
				glyph.kerningIndex	= pGlyph->kerningIndex;
				glyph.pSurface		= pGlyph->pSurface;
				glyph.rect			= pGlyph->rect;
				glyph.bearingX		= pGlyph->bearingX;
				glyph.bearingY		= pGlyph->bearingY;
				return;
			}
		}

		glyph.pFont = 0;
		glyph.advance = 0;
		glyph.kerningIndex = 0;
		glyph.pSurface = nullptr;
		return;
	}


	//____ kerning() _________________________________________________________

	inline spx BitmapFont::kerning( Glyph& leftGlyph, Glyph&  rightGlyph )
	{
		if( !m_pKerningTable )
			return 0;

		if( leftGlyph.pFont != this || rightGlyph.pFont != this )
			return 0;

		return m_pKerningTable[ (leftGlyph.kerningIndex * m_nKerningGlyphs) + rightGlyph.kerningIndex ]*64;
	}


	//____ _insertGlyphs() _________________________________________________________

	void BitmapFont::_insertGlyphs( Surface * pSurf, const char* pGlyphSpec )
	{
		// Get section pointers

		const char* pInfoBeg = strstr(pGlyphSpec, "[INFO BEGIN]");
		const char* pInfoEnd = strstr(pGlyphSpec, "[INFO END]");

		const char* pCharmapBeg = strstr(pGlyphSpec, "[CHARMAP BEGIN]");
		const char* pCharmapEnd = strstr(pGlyphSpec, "[CHARMAP END]");

		const char* pKerningBeg = strstr(pGlyphSpec, "[KERNING BEGIN]");
		const char* pKerningEnd = strstr(pGlyphSpec, "[KERNING END]");

		if (!pInfoBeg || !pInfoEnd || !pCharmapBeg || !pCharmapEnd)
		{
			//TODO: Error handling

			return;
		}

		// Advance pointers

		pInfoBeg += 12;
		pCharmapBeg += 15;

		if (pKerningBeg)
			pKerningBeg += 15;

		// Read info

		int	size = 0, whitespace = 0, monochrome = -1, baseline = 0, linegap = 0;

		const char* pSize = strstr(pInfoBeg, "SIZE:");
		const char* pWhitespace = strstr(pInfoBeg, "WHITESPACE:");
		const char* pMonochrome = strstr(pInfoBeg, "MONOCHROME:");
		const char* pBaseline = strstr(pInfoBeg, "BASELINE:");
		const char* pLineGap = strstr(pInfoBeg, "LINEGAP:");

		if (pSize && pSize < pInfoEnd)
			size = strtol(pSize + 5, nullptr, 10);

		if (pWhitespace && pWhitespace < pInfoEnd)
			whitespace = strtol(pWhitespace + 11, nullptr, 10);

		if(pMonochrome && pMonochrome < pInfoEnd)
			monochrome = strtol(pMonochrome + 11, nullptr, 10);

		if (pBaseline && pBaseline < pInfoEnd)
			baseline = strtol(pBaseline + 9, nullptr, 10);

		if (pLineGap && pLineGap < pInfoEnd)
			linegap = strtol(pLineGap + 8, nullptr, 10);


		// Read charmap and surface-grid

		int	maxAdvance = 0;
		int maxDescend = 0;
		int firstAdvance = -1;
		bool bMonospace = true;

		const char* pChar = pCharmapBeg;
		while ((*pChar == 10 || *pChar == 13) && pChar < pCharmapEnd)
			TextTool::readChar(pChar);


		PixelBuffer pixelBuffer = pSurf->allocPixelBuffer();
		pSurf->pushPixels(pixelBuffer);

		GridWalker	gw(pixelBuffer);

		if (!gw.firstLine())
		{
			//TODO: Error handling!
		}

		bool bNewLine = false;
		while (pChar < pCharmapEnd)
		{
			uint16_t ch = TextTool::readChar(pChar);

			if (ch == '3')
			{
				int dummy = 0;
			}

			if (ch == 10 || ch == 13)
				bNewLine = true;
			else
			{
				RectI r;
				if (bNewLine)
				{
					if (gw.nextLine() == false)
					{
						//TODO: Error handling!
					}

					r = gw.firstCell();
					bNewLine = false;
				}
				else
					r = gw.nextCell();

				if (!r.isEmpty())
				{
					int tab = ch / 256;

					if (m_glyphTab[tab] == nullptr)
					{
						m_glyphTab[tab] = new MyGlyph[256];
						for (int i = 0; i < 256; i++)
						{
							m_glyphTab[tab][i] = MyGlyph();
						}
					}
					ch &= 0xff;

					if (!m_glyphTab[tab][ch].pSurface)
					{
						auto * pGlyph = &m_glyphTab[tab][ch];
						
						pGlyph->advance = r.w * 64;
						pGlyph->bearingX = 0;
						pGlyph->bearingY = -baseline*64;
						pGlyph->kerningIndex = 0;
						pGlyph->pSurface = pSurf;
						pGlyph->rect = r * 64;

						if (firstAdvance == -1)
							firstAdvance = r.w;

						if (r.w != firstAdvance)
							bMonospace = false;

						if (r.w > maxAdvance)
							maxAdvance = r.w;

						if (r.h - baseline > maxDescend)
							maxDescend = r.h - baseline;

						m_nGlyphs++;
					}
				}
			}
		}

		pSurf->freePixelBuffer(pixelBuffer);

		// Fill in font data.

		if (size > 0)
			m_size = size * 64;

		if (monochrome >= 0)
			m_bMonochrome = monochrome;

		if (linegap * 64 > m_lineGap)
			m_lineGap = linegap * 64;

		if (whitespace * 64 > m_spaceAdvance)
			m_spaceAdvance = whitespace * 64;

		if (bMonospace == false)
			m_bMonospace = false;

		if (maxAdvance * 64 > m_maxAdvance)
			m_maxAdvance = maxAdvance * 54;

		if (baseline * 64 > m_maxAscend)
			m_maxAscend = baseline * 64;

		if (maxDescend * 64 > m_maxDescend)
			m_maxDescend = maxDescend * 64;
	}

} // namespace wg
