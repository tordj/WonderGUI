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
#include <wg_bitmapglyphs.h>
#include <wg_texttool.h>

// TEMP PROFILING INCLUDES
//#include "Utilities/EProfiler.h"
//#include "Util/Util.hpp"



//____ Constructor ____________________________________________________________

WgBitmapGlyphs::WgBitmapGlyphs( WgSurface * pSurf, char * pGlyphSpec, bool binaryFile )
{
	m_nKerningGlyphs= 0;
	m_pKerningTable = 0;

	m_bMonospace	= true;
	m_avgSpacing	= 0;
	m_maxSpacing	= 0;
	m_spaceSpacing	= 0;
	m_nGlyphs		= 0;
	m_height		= 0;
	m_baseline		= 0;


	for( int i = 0 ; i < 256 ; i++ )
		m_glyphTab[i] = 0;

	// Insert the glyphs
	InsertGlyphs(pSurf, pGlyphSpec, binaryFile);
/*
	// Create an underline specification from the '_' character as default.
	// It should be possible to specify something different in the spec file later on...

	const WgGlyph* pUnder = GetGlyph('_', 0);

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

WgBitmapGlyphs::~WgBitmapGlyphs()
{
	for( int i = 0 ; i < 256 ; i++ )
	{
		if( m_glyphTab[i] )
			delete [] m_glyphTab[i];
	}

	if( m_pKerningTable )
		delete [] m_pKerningTable;
}


//____ CopyGlyphs() ___________________________________________________________

void WgBitmapGlyphs::CopyGlyphs( WgBitmapGlyphs* pOtherFont )
{
	// Multiply average spacing by glyph count so that we can continue to add widths..
	m_avgSpacing *= m_nGlyphs;

	for(int tabIndex=0; tabIndex < 256; tabIndex++)
	{
		// See if the other font has glyphs defined on this tab
		if(pOtherFont->m_glyphTab[tabIndex] != NULL)
		{
			if(m_glyphTab[tabIndex] == NULL)
			{
				// Allocate memory for this tab, and reset it
				m_glyphTab[tabIndex] = new Glyph[256];
				for( int i = 0 ; i < 256 ; i++ )
				{
					m_glyphTab[tabIndex][i] = Glyph();
				}
			}

			// Copy all the glyphs of this tab
			for( int i = 0 ; i < 256 ; i++ )
			{
				if(m_glyphTab[tabIndex][i].m_src.pSurface == 0)
				{
					m_glyphTab[tabIndex][i] = pOtherFont->m_glyphTab[tabIndex][i];

					m_avgSpacing += m_glyphTab[tabIndex][i].Advance();

					// See if we have a new max height
					if(m_glyphTab[tabIndex][i].m_src.rect.h > (int)m_height)
						m_height = m_glyphTab[tabIndex][i].m_src.rect.h;

					m_nGlyphs++;
				}
			}
		}
	}

	// Divide back into average
	m_avgSpacing /= m_nGlyphs;
}


//____ HasGlyph() _________________________________________________

inline bool WgBitmapGlyphs::HasGlyph( Uint16 chr )
{
	Glyph * pGlyph = m_glyphTab[chr >> 8];

	if( pGlyph )
	{
		pGlyph += (chr & 0xFF);
		if( pGlyph->m_src.pSurface != 0 )
			return true;
	}

	return false;
}


//____ GetGlyph() _________________________________________________________

inline WgGlyphPtr WgBitmapGlyphs::GetGlyph( Uint16 chr, int size )
{
	Glyph * pGlyph = m_glyphTab[chr >> 8];

	if( pGlyph )
	{
		pGlyph += (chr & 0xFF);
		if( pGlyph->m_src.pSurface != 0 )
			return pGlyph;
	}


	return 0;
}


//____ GetKerning() _________________________________________________________


inline int WgBitmapGlyphs::GetKerning( WgGlyphPtr pLeftGlyph, WgGlyphPtr pRightGlyph, int size )
{
	if( !m_pKerningTable )
		return 0;

	if( !pLeftGlyph || !pRightGlyph )
		return 0;

	int indexLeft = pLeftGlyph->KerningIndex();
	int indexRight = pRightGlyph->KerningIndex();

	if( indexLeft >= m_nKerningGlyphs || indexRight >= m_nKerningGlyphs )
		return 0;

	return m_pKerningTable[ (indexLeft * m_nKerningGlyphs) + indexRight ];
}


//____ InsertGlyphs() _________________________________________________________

void WgBitmapGlyphs::InsertGlyphs( WgSurface * pSurf, char* pGlyphSpec, bool binaryFile )
{
	// Multiply average spacing by glyph count so that we can continue to add widths..
	m_avgSpacing *= m_nGlyphs;

	bool firstInsert = (0 == m_nGlyphs);

	if(binaryFile)
	{
		// New style bfnt font spec

		//////////////////////////////////////////////////////////////////////////
		// Structs for the binary data
		#pragma pack(1)
		struct headerBlock
		{
			Uint32 blockSize;
			Uint32 sectionCount;
			Uint16 spaceSpacing;
			Uint16 height;
			Uint16 ascend;

			static headerBlock create(char* pData)
			{
				headerBlock b;
				memcpy(&b, pData, sizeof(headerBlock));
				return b;
			}
		};
		#pragma pack(1)
		struct charsBlock
		{
			Uint32 blockSize;
			struct charInfo
			{
				Uint16 character;
				Uint16 x;
				Uint16 y;
				Uint16 width;
				Uint16 height;
				Sint16 advance;
				Sint16 xoffset;
				Sint16 yoffset;

				static charInfo create(char* pData)
				{
					charInfo info;
					memcpy(&info, pData, sizeof(info));
					return info;
				}
			};

			static charsBlock create(char* pData)
			{
				charsBlock b;
				memcpy(&b, pData, sizeof(b));
				return b;
			}
		};
		#pragma pack(1)
		struct kerningPairsBlock
		{
			Uint32 blockSize;
			struct kerningPair
			{
				Uint16  first;
				Uint16  second;
				Sint16 amount;

				static kerningPair create(char* pData)
				{
					kerningPair info;
					memcpy(&info, pData, sizeof(info));
					return info;
				}
			};

			static kerningPairsBlock create(char* pData)
			{
				kerningPairsBlock b;
				memcpy(&b, pData, sizeof(b));
				return b;
			}
		};
		//////////////////////////////////////////////////////////////////////////

		enum eBlockTypes
		{
			HEADER = 1,
			CHARACTERS = 2,
			KERNINGPAIRS = 3
		};

		if(firstInsert)
		{
			m_glyphTab[0] = new Glyph[256];
			for( int i = 0 ; i < 256 ; i++ )
			{
				m_glyphTab[0][i] = Glyph();
			}
		}

		// Make sure that the binary fontspec starts with the appropriate signature
		if(strncmp("WGFN", pGlyphSpec, 4) != 0)
			return;

		int currentIndex= (int) strlen("WGFN");
		int currentSection=0;
		int maxSections=INT_MAX;
		while(currentSection++ < maxSections)
		{
			// This stuff is a bit ugly.. but it works
			switch(pGlyphSpec[currentIndex++])
			{
			case HEADER:
				{
					headerBlock pBlock = headerBlock::create(&pGlyphSpec[currentIndex]);
					currentIndex += pBlock.blockSize;
					maxSections = pBlock.sectionCount;

					if(firstInsert)
					{
						// Use the header info from this fontspec
						m_spaceSpacing = pBlock.spaceSpacing;
						m_height = pBlock.height;
					}
				}
				break;
			case CHARACTERS:
				{
					charsBlock pBlock = charsBlock::create(&pGlyphSpec[currentIndex]);
					int innerIndex = currentIndex + sizeof(pBlock);
					currentIndex += pBlock.blockSize;

					while(innerIndex + (int)sizeof(charsBlock::charInfo) <= currentIndex)
					{
						charsBlock::charInfo info = charsBlock::charInfo::create(&pGlyphSpec[innerIndex]);
						innerIndex += sizeof(charsBlock::charInfo);

						Uint16 c = info.character;

						int tab = c >> 8;
						if( m_glyphTab[tab] == 0 )
						{
							m_glyphTab[tab] = new Glyph[256];
							for( int i = 0 ; i < 256 ; i++ )
							{
								m_glyphTab[tab][i] = Glyph();
							}
						}

						c &= 0xff;
						m_glyphTab[tab][c] = Glyph( (Uint8)info.advance, (Sint8)info.xoffset, (Sint8)info.yoffset, m_nGlyphs, this, pSurf, WgRect( info.x, info.y, info.width, info.height) );

						if(firstInsert)
						{
							// This is done every iteration, but it doesn't matter really
							m_glyphTab[0][32].SetAdvance( m_spaceSpacing );
							m_glyphTab[0][0xA0].SetAdvance( m_spaceSpacing );	// NO_BREAK_SPACE
						}

						if( info.height > (int) m_height )
							m_height = info.height;

						if( info.advance > m_maxSpacing )
							m_maxSpacing = info.advance;

						m_avgSpacing += info.advance;
						m_nGlyphs++;
					}
				}
				break;
			case KERNINGPAIRS:
				{
					kerningPairsBlock pBlock = kerningPairsBlock::create(&pGlyphSpec[currentIndex]);
					int innerIndex = currentIndex + sizeof(pBlock);
					currentIndex += pBlock.blockSize;

					Sint8* pNewKerningTable = new Sint8[ m_nGlyphs * m_nGlyphs ];
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

					while(innerIndex + (int)sizeof(kerningPairsBlock::kerningPair) <= currentIndex)
					{
						kerningPairsBlock::kerningPair info = kerningPairsBlock::kerningPair::create(&pGlyphSpec[innerIndex]);
						innerIndex += sizeof(kerningPairsBlock::kerningPair);

						Uint16 cLeft = info.first;
						Uint16 cRight = info.second;

						int indexLeft = GetGlyph( cLeft )->KerningIndex();
						int indexRight = GetGlyph( cRight )->KerningIndex();

						m_pKerningTable[ (indexLeft * m_nGlyphs) + indexRight ] = (Sint8)info.amount;
					}
				}
				break;
			}
		}
	}
	else
	{
		// Old-style ascii fnt spec

		// Fill in m_glyphTab
		char chr[16];
		int		x, y, w, h, advance, bearingX, bearingY;
		int		firstSpacing;
		int		ascend;			// Ascend, i.e. distance from the baseline to the topmost point of any glyph.
		int		nRead = 0;

		// Only interpret header stuff for the first font
		if(firstInsert)
		{
			nRead = sscanf( pGlyphSpec, "%d %d %d", &m_spaceSpacing, &m_height, &ascend );
		}
		else
		{
			int bogus;
			nRead = sscanf( pGlyphSpec, "%d %d %d", &bogus, &bogus, &ascend );
		}

		if( nRead >= 3 && ascend > m_baseline )
			m_baseline = ascend;


		bearingX = 0; // Clear bearingX since it is optional in the font.
		bearingY = 0; // Clear bearingY since it is optional in the font.

		pGlyphSpec = WgTextTool::nextLine( pGlyphSpec );
		nRead = sscanf( pGlyphSpec, "%s %d %d %d %d %d %d %d", chr, &x, &y, &w, &h, &advance, &bearingX, &bearingY );
		pGlyphSpec = WgTextTool::nextLine( pGlyphSpec );
		firstSpacing = advance;


		// We always have the page where whitespace is...
		// Only do this the first glyph insertion

		if(firstInsert)
		{
			if( (int) m_spaceSpacing != firstSpacing )
				m_bMonospace = false;

			m_glyphTab[0] = new Glyph[256];
			for( int i = 0 ; i < 256 ; i++ )
			{
				m_glyphTab[0][i] = Glyph();
			}
			m_glyphTab[0][32].SetAdvance( m_spaceSpacing );
			m_glyphTab[0][0xA0].SetAdvance( m_spaceSpacing );	// NO_BREAK_SPACE
		}


		while( nRead >= 6 && nRead <= 8 ) // bearingX & bearingY are optional.
		{
			// Fix bearing that (incorrectly) starts from top in fontfiles.

			bearingY -= ascend;

			//

			const char * pChr = chr;
			Uint16 c = WgTextTool::parseChar( pChr );

			int tab = c >> 8;
			if( m_glyphTab[tab] == 0 )
			{
				m_glyphTab[tab] = new Glyph[256];
				for( int i = 0 ; i < 256 ; i++ )
				{
					m_glyphTab[tab][i] = Glyph();
				}
			}

			c &= 0xff;

			if(m_glyphTab[tab][c].m_src.pSurface == NULL)
			{
				m_glyphTab[tab][c] = Glyph( advance, bearingX, bearingY, m_nGlyphs, this, pSurf, WgRect( x, y, w, h ) );

				if( advance != firstSpacing )
					m_bMonospace = false;

				if( h > (int) m_height )
					m_height = h;

				if( advance > m_maxSpacing )
					m_maxSpacing = advance;

				m_avgSpacing += advance;
				m_nGlyphs++;
			}


			bearingX = 0; // Clear berings since they are optional in the font.
			bearingY = 0;
			nRead = sscanf( pGlyphSpec, "%s %d %d %d %d %d %d %d", chr, &x, &y, &w, &h, &advance, &bearingX, &bearingY );
			pGlyphSpec = WgTextTool::nextLine( pGlyphSpec );
		}

		// I guess monospace means no kerning.
		if( !m_bMonospace )
		{
			// Read kerning information if there is any.
			char* pKerningStart = strstr( pGlyphSpec, "[kerning]" );
			if( pKerningStart )
			{
				char* pKerningSpec = WgTextTool::nextLine( pKerningStart );

				Sint8* pNewKerningTable = new Sint8[ m_nGlyphs * m_nGlyphs ];
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
				pKerningSpec = WgTextTool::nextLine( pKerningSpec );
				while( nRead == 3 )
				{
					const char* pChrLeft = chrLeft;
					const char* pChrRight = chrRight;
					Uint16 cLeft = WgTextTool::parseChar( pChrLeft );
					Uint16 cRight = WgTextTool::parseChar( pChrRight );

					int indexLeft = GetGlyph( cLeft )->KerningIndex();
					int indexRight = GetGlyph( cRight )->KerningIndex();

					m_pKerningTable[ (indexLeft * m_nGlyphs) + indexRight ] = kern;

					nRead = sscanf( pKerningSpec, "%s %s %d", chrLeft, chrRight, &kern );
					pKerningSpec = WgTextTool::nextLine( pKerningSpec );
				}
			}
		}
	}

	// Divide back into average
	m_avgSpacing /= m_nGlyphs;
}

//____ WgBitmapGlyphs::Glyph constructor ______________________________________

WgBitmapGlyphs::Glyph::Glyph()
{
	m_src.pSurface = 0;
}

WgBitmapGlyphs::Glyph::Glyph( int advance, Sint8 bearingX, Sint8 bearingY, Uint32 kerningIndex, WgGlyphset * pGlyphset, WgSurface * pSurf, const WgRect& rect )
: WgGlyph( advance, kerningIndex, pGlyphset )
{
		m_src.pSurface	= pSurf;
		m_src.rect		= rect;
		m_src.bearingX	= bearingX;
		m_src.bearingY	= bearingY;
}
