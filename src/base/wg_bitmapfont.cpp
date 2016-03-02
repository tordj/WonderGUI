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
	
	BitmapFont::BitmapFont( const Surface_p& pSurf, char * pGlyphSpec, bool binaryFile )
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
		insertGlyphs(pSurf, pGlyphSpec, binaryFile);
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
	
	//____ copyGlyphs() ___________________________________________________________
	
	void BitmapFont::copyGlyphs( BitmapFont* pOtherFont )
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
					m_glyphTab[tabIndex] = new MyGlyph[256];
					for( int i = 0 ; i < 256 ; i++ )
					{
						m_glyphTab[tabIndex][i] = MyGlyph();
					}
				}
	
				// Copy all the glyphs of this tab
				for( int i = 0 ; i < 256 ; i++ )
				{
					if(!m_glyphTab[tabIndex][i].m_src.pSurface)
					{
						m_glyphTab[tabIndex][i] = pOtherFont->m_glyphTab[tabIndex][i];
	
						m_avgSpacing += m_glyphTab[tabIndex][i].advance();
	
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
	
	inline Glyph_p BitmapFont::getGlyph( uint16_t chr, int size )
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
	
	
	//____ getKerning() _________________________________________________________
	
	
	inline int BitmapFont::getKerning( Glyph_p pLeftGlyph, Glyph_p pRightGlyph, int size )
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
	
	void BitmapFont::insertGlyphs( const Surface_p& pSurf, char* pGlyphSpec, bool binaryFile )
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
				uint32_t blockSize;
				uint32_t sectionCount;
				uint16_t spaceSpacing;
				uint16_t height;
				uint16_t ascend;
	
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
				uint32_t blockSize;
				struct charInfo
				{
					uint16_t character;
					uint16_t x;
					uint16_t y;
					uint16_t width;
					uint16_t height;
					int16_t advance;
					int16_t xoffset;
					int16_t yoffset;
	
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
				uint32_t blockSize;
				struct kerningPair
				{
					uint16_t  first;
					uint16_t  second;
					int16_t amount;
	
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
				m_glyphTab[0] = new MyGlyph[256];
				for( int i = 0 ; i < 256 ; i++ )
				{
					m_glyphTab[0][i] = MyGlyph();
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
	
							uint16_t c = info.character;
	
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
							m_glyphTab[tab][c] = MyGlyph( (uint8_t)info.advance, (int8_t)info.xoffset, (int8_t)info.yoffset, m_nGlyphs, this, pSurf, Rect( info.x, info.y, info.width, info.height) );
	
							if(firstInsert)
							{
								// This is done every iteration, but it doesn't matter really
								m_glyphTab[0][32].setAdvance( m_spaceSpacing );
								m_glyphTab[0][0xA0].setAdvance( m_spaceSpacing );	// NO_BREAK_SPACE
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
	
						while(innerIndex + (int)sizeof(kerningPairsBlock::kerningPair) <= currentIndex)
						{
							kerningPairsBlock::kerningPair info = kerningPairsBlock::kerningPair::create(&pGlyphSpec[innerIndex]);
							innerIndex += sizeof(kerningPairsBlock::kerningPair);
	
							uint16_t cLeft = info.first;
							uint16_t cRight = info.second;
	
							int indexLeft = getGlyph( cLeft )->kerningIndex();
							int indexRight = getGlyph( cRight )->kerningIndex();
	
							m_pKerningTable[ (indexLeft * m_nGlyphs) + indexRight ] = (int8_t)info.amount;
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
	
			pGlyphSpec = TextTool::nextLine( pGlyphSpec );
			nRead = sscanf( pGlyphSpec, "%s %d %d %d %d %d %d %d", chr, &x, &y, &w, &h, &advance, &bearingX, &bearingY );
			pGlyphSpec = TextTool::nextLine( pGlyphSpec );
			firstSpacing = advance;
	
	
			// We always have the page where whitespace is...
			// Only do this the first glyph insertion
	
			if(firstInsert)
			{
				if( (int) m_spaceSpacing != firstSpacing )
					m_bMonospace = false;
	
				m_glyphTab[0] = new MyGlyph[256];
				for( int i = 0 ; i < 256 ; i++ )
				{
					m_glyphTab[0][i] = MyGlyph();
				}
				m_glyphTab[0][32].setAdvance( m_spaceSpacing );
				m_glyphTab[0][0xA0].setAdvance( m_spaceSpacing );	// NO_BREAK_SPACE
			}
	
	
			while( nRead >= 6 && nRead <= 8 ) // bearingX & bearingY are optional.
			{
				// Fix bearing that (incorrectly) starts from top in fontfiles.
	
	//			if( nRead < 8 )
					bearingY -= ascend;
	
				//
	
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
		}
	
		// Divide back into average
		m_avgSpacing /= m_nGlyphs;
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
