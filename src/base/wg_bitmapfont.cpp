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


#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <wg_bitmapfont.h>
#include <wg_texttool.h>

namespace wg 
{
	
	
	const char BitmapFont::CLASSNAME[] = {"BitmapFont"};
	
	
	//____ Constructor ____________________________________________________________
	
	BitmapFont::BitmapFont( const Surface_p& pSurf, char * pGlyphSpec )
	{
		m_nKerningGlyphs= 0;
		m_pKerningTable = 0;
	
		m_nGlyphs		= 0;
		m_bMonospace	= true;
		m_avgAdvance	= 0;
		m_maxAdvance	= 0;
		m_spaceAdvance	= 0;
		m_lineGap		= 0;
		m_maxAscend		= 0;
		m_maxDescend	= 0;
		m_size			= 0;
	
	
		for( int i = 0 ; i < 256 ; i++ )
			m_glyphTab[i] = 0;
	
		// Insert the glyphs
		insertGlyphs(pSurf, pGlyphSpec);
	/*
		// Create an underline specification from the '_' character as default.
		// It should be possible to specify something different in the spec file later on...
	
		const Glyph* pUnder = getGlyph('_', 0);
	
		m_underline.pSurf = pUnder->pSurf;
		m_underline.rect = pUnder->rect;
		m_underline.bearingX = pUnder->bearingX;
		m_underline.bearingY = pUnder->bearingY;
	
		if( pUnder->rect.w > 2 )
		{
			m_underline.leftBorder = 1;
			m_underline.rightBorder = 1;
		}
		else
		{
			m_underline.leftBorder = 0;
			m_underline.rightBorder = 0;
		}
	*/
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
	
	//____ isInstanceOf() _________________________________________________________
	
	bool BitmapFont::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return Font::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * BitmapFont::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	BitmapFont_p BitmapFont::cast( const Object_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return BitmapFont_p( static_cast<BitmapFont*>(pObject.rawPtr()) );
	
		return 0;
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
	
	
	inline int BitmapFont::kerning( Glyph_p pLeftGlyph, Glyph_p pRightGlyph )
	{
		if( !m_pKerningTable )
			return 0;
	
		if( !pLeftGlyph || !pRightGlyph )
			return 0;
	
		int indexLeft = pLeftGlyph->kerningIndex();
		int indexRight = pRightGlyph->kerningIndex();
	
		if( indexLeft >= m_nKerningGlyphs || indexRight >= m_nKerningGlyphs )
			return 0;
	
		return m_pKerningTable[ (indexLeft * m_nKerningGlyphs) + indexRight ];
	}
	
	
	//____ insertGlyphs() _________________________________________________________
	
	void BitmapFont::insertGlyphs( const Surface_p& pSurf, char* pGlyphSpec )
	{
		// Multiply average spacing by glyph count so that we can continue to add widths..
		m_avgAdvance *= m_nGlyphs;
	
		bool firstInsert = (0 == m_nGlyphs);
	
		// Old-style ascii fnt spec

		// Fill in m_glyphTab
		char chr[16];
		int		x, y, w, h, advance, bearingX, bearingY;
		int		firstAdvance;
		int		nRead = 0;

		// Only interpret header stuff for the first font
		if(firstInsert)
		{
			nRead = sscanf( pGlyphSpec, "%d %d %d", &m_size, &m_lineGap, &m_spaceAdvance );
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
				m_glyphTab[tab][c] = MyGlyph( advance, bearingX, bearingY, m_nGlyphs, this, pSurf, Rect( x, y, w, h ) );

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

				int8_t* pNewKerningTable = new int8_t[ m_nGlyphs * m_nGlyphs ];
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
	}
	
	//____ BitmapFont::MyGlyph constructor ______________________________________
	
	BitmapFont::MyGlyph::MyGlyph()
	{
		m_src.pSurface = 0;
	}
	
	BitmapFont::MyGlyph::MyGlyph( int advance, int8_t bearingX, int8_t bearingY, uint32_t kerningIndex, Font * pFont, const Surface_p& pSurf, const Rect& rect )
	: Glyph( advance, kerningIndex, pFont )
	{
			m_src.pSurface	= pSurf;
			m_src.rect		= rect;
			m_src.bearingX	= bearingX;
			m_src.bearingY	= bearingY;
	}

} // namespace wg
