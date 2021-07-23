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
			if( pGlyph->m_src.pSurface )
				return true;
		}

		return false;
	}


	//____ getGlyph() _________________________________________________________

	inline Glyph_p BitmapFont::getGlyph( uint16_t chr )
	{
		MyGlyph * pGlyph = m_glyphTab[chr >> 8];

		if( pGlyph )
		{
			pGlyph += (chr & 0xFF);
			if( pGlyph->m_src.pSurface )
				return pGlyph;
		}

		return 0;
	}


	//____ kerning() _________________________________________________________


	inline spx BitmapFont::kerning( Glyph_p pLeftGlyph, Glyph_p pRightGlyph )
	{
		if( !m_pKerningTable )
			return 0;

		if( !pLeftGlyph || !pRightGlyph )
			return 0;

		int indexLeft = pLeftGlyph->kerningIndex();
		int indexRight = pRightGlyph->kerningIndex();

		if( indexLeft >= m_nKerningGlyphs || indexRight >= m_nKerningGlyphs )
			return 0;

		return m_pKerningTable[ (indexLeft * m_nKerningGlyphs) + indexRight ]*64;
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

					if (!m_glyphTab[tab][ch].m_src.pSurface)
					{
						m_glyphTab[tab][ch] = MyGlyph(r.w, 0, -baseline, m_nGlyphs, this, pSurf, r*64);

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
		
/*

		// Multiply average spacing by glyph count so that we can continue to add widths..
		m_avgAdvance *= m_nGlyphs;

		bool firstInsert = (0 == m_nGlyphs);

		// Old-style ascii fnt spec

		// Fill in m_glyphTab
		char chr[16];
		int		x, y, w, h, advance, bearingX, bearingY;
		int		firstAdvance;
		int		nRead = 0;
		int		monochrome = 1;

		// Only interpret header stuff for the first font
		if(firstInsert)
		{
            int size;
			nRead = sscanf( pGlyphSpec, "size: %d linegap: %d space: %d monochrome: %d", &size, &m_lineGap, &m_spaceAdvance, &monochrome );

			m_bMonochrome = (monochrome>0);
            m_size = size;                  // Implicit conversion to MU-format.

		}
		else
		{
			int size, lineGap, spaceAdvance;

			nRead = sscanf( pGlyphSpec, "%d %d %d",  &size, &lineGap, &spaceAdvance );

			if( lineGap > m_lineGap ) m_lineGap = lineGap;
			if( spaceAdvance > m_spaceAdvance ) m_spaceAdvance = spaceAdvance;
		}

		if( nRead < 3 )
			return;						// Incorrect header



		bearingX = 0; // Clear bearingX since it is optional in the font.
		bearingY = 0; // Clear bearingY since it is optional in the font.

		pGlyphSpec = TextTool::nextLine( pGlyphSpec );
		nRead = sscanf( pGlyphSpec, "%s %d %d %d %d %d %d %d", chr, &x, &y, &w, &h, &advance, &bearingX, &bearingY );
		pGlyphSpec = TextTool::nextLine( pGlyphSpec );
		firstAdvance = advance;

		// We always have the page where whitespace is...
		// Only do this the first glyph insertion

		if(firstInsert)
		{
			if( (int) m_spaceAdvance != firstAdvance )
				m_bMonospace = false;

			m_glyphTab[0] = new MyGlyph[256];
			for( int i = 0 ; i < 256 ; i++ )
			{
				m_glyphTab[0][i] = MyGlyph();
			}
			m_glyphTab[0][32].setAdvance( m_spaceAdvance );
			m_glyphTab[0][0xA0].setAdvance( m_spaceAdvance );	// NO_BREAK_SPACE
		}


		while( nRead >= 6 && nRead <= 8 ) // bearingX & bearingY are optional.
		{
			// Fix bearing that (incorrectly) starts from top in fontfiles.


			const char * pChr = chr;
			uint16_t c = TextTool::parseChar( pChr );

			int tab = c >> 8;
			if( m_glyphTab[tab] == 0 )
			{
				m_glyphTab[tab] = new MyGlyph[256];
				for( int i = 0 ; i < 256 ; i++ )
				{
					m_glyphTab[tab][i] = MyGlyph();
				}
			}

			c &= 0xff;

			if(!m_glyphTab[tab][c].m_src.pSurface)
			{
				m_glyphTab[tab][c] = MyGlyph( advance, bearingX, bearingY, m_nGlyphs, this, pSurf, RectI( x, y, w, h ) );

				if( advance != firstAdvance )
					m_bMonospace = false;

				if( m_maxAscend < 0 - bearingY )
					m_maxAscend = -bearingY;

				if( m_maxDescend < h + bearingY )
					m_maxDescend = h + bearingY;

				if( advance > m_maxAdvance )
					m_maxAdvance = advance;

				m_avgAdvance += advance;
				m_nGlyphs++;
			}


			bearingX = 0; // Clear berings since they are optional in the font.
			bearingY = 0;
			nRead = sscanf( pGlyphSpec, "%s %d %d %d %d %d %d %d", chr, &x, &y, &w, &h, &advance, &bearingX, &bearingY );
			pGlyphSpec = TextTool::nextLine( pGlyphSpec );
		}

		// I guess monospace means no kerning.
		if( !m_bMonospace )
		{
			// Read kerning information if there is any.
			char* pKerningStart = strstr( pGlyphSpec, "[kerning]" );
			if( pKerningStart )
			{
				char* pKerningSpec = TextTool::nextLine( pKerningStart );

				int16_t* pNewKerningTable = new int16_t[ m_nGlyphs * m_nGlyphs ];
				int oldSize = sizeof( *m_pKerningTable ) * m_nKerningGlyphs * m_nKerningGlyphs;
				int newSize = sizeof( *m_pKerningTable ) * m_nGlyphs * m_nGlyphs;
				if( m_pKerningTable )
				{
					memcpy( pNewKerningTable, m_pKerningTable, oldSize );
					delete [] m_pKerningTable;
					m_pKerningTable = 0;
				}
				memset( &pNewKerningTable[ oldSize ], 0, newSize - oldSize );

				m_pKerningTable = pNewKerningTable;

				m_nKerningGlyphs = m_nGlyphs;

				char chrLeft[8];
				char chrRight[8];
				int kern = 0;

				nRead = sscanf( pKerningSpec, "%s %s %d", chrLeft, chrRight, &kern );
				pKerningSpec = TextTool::nextLine( pKerningSpec );
				while( nRead == 3 )
				{
					const char* pChrLeft = chrLeft;
					const char* pChrRight = chrRight;
					uint16_t cLeft = TextTool::parseChar( pChrLeft );
					uint16_t cRight = TextTool::parseChar( pChrRight );

					int indexLeft = getGlyph( cLeft )->kerningIndex();
					int indexRight = getGlyph( cRight )->kerningIndex();

					m_pKerningTable[ (indexLeft * m_nGlyphs) + indexRight ] = kern;

					nRead = sscanf( pKerningSpec, "%s %s %d", chrLeft, chrRight, &kern );
					pKerningSpec = TextTool::nextLine( pKerningSpec );
				}
			}
		}

		// Divide back into average
		m_avgAdvance /= m_nGlyphs;
*/
	}

	//____ BitmapFont::MyGlyph constructor ______________________________________

	BitmapFont::MyGlyph::MyGlyph()
	{
		m_src.pSurface = 0;
	}

	BitmapFont::MyGlyph::MyGlyph( int advance, int16_t bearingX, int16_t bearingY, uint32_t kerningIndex, Font * pFont, Surface * pSurf, const RectSPX& rect )
	: Glyph( advance*64, kerningIndex, pFont )
	{
			m_src.pSurface	= pSurf;
			m_src.rect		= rect;
			m_src.bearingX	= bearingX*64;
			m_src.bearingY  = bearingY*64;
	}

} // namespace wg
