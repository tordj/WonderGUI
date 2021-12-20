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

/* TODO: See if we can be better off using multiple size-objects instead of changing size all the time.
   TODO: Separate glyph bitmap from glyph struct, not render or prio cache slot until bitmap is accessed (probably has more to do with Pen).
*/

#include <array>
#include <algorithm>
#include <assert.h>

#include <wg_freetypefont.h>
#include <wg_surface.h>
#include <wg_surfacefactory.h>
#include <wg_base.h>
#include <wg_context.h>
#include <wg_util.h>


#include <ft2build.h>
#include FT_FREETYPE_H


#define SUPPORT_STEM_DARKENING

#ifdef SUPPORT_STEM_DARKENING
#	include FT_PARAMETER_TAGS_H			// If your compiler fails on this line, you likely have an old freetype library that does not support stem darkening. 
										// Comment out define above to compile anyway. Small fonts will look thin/bright and anti-alias won't be as smooth
										// without stem darkening when gamma correction is enabled.
#endif

namespace wg
{
	using namespace Util;

	const TypeInfo FreeTypeFont::TYPEINFO = { "FreeTypeFont", &Font::TYPEINFO };

	std::vector<FreeTypeFont*>	FreeTypeFont::s_instances;

	FT_Library			FreeTypeFont::s_freeTypeLibrary;

	std::vector<FreeTypeFont::CacheSurf>	FreeTypeFont::s_cacheSurfaces[10];


	uint32_t			FreeTypeFont::s_cacheSurfacesCreated = 0;
	int					FreeTypeFont::s_cacheSize = 0;
	int					FreeTypeFont::s_cacheLimit = 0;

	const uint8_t		FreeTypeFont::s_categoryHeight[9] = { 8, 12, 16, 24, 32, 48, 64, 96, 128 };

	const uint8_t		FreeTypeFont::s_sizeToCategory[129] = { 0,0,0,0,0,0,0,0,0,
																1,1,1,1,
																2,2,2,2,
																3,3,3,3,3,3,3,3,
																4,4,4,4,4,4,4,4,
																5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
																6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
																7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
																8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8 };


	//____ constructor ____________________________________________________________

	FreeTypeFont::FreeTypeFont( Blob_p pFontFile, int faceIndex )
	{
        if( s_instances.empty() )
        {
            FT_Error err = FT_Init_FreeType(&s_freeTypeLibrary);
            if (err != 0)
            {
                //TODO: Error handling!
            }
        }
        s_instances.push_back(this);

		m_pFontFile = pFontFile;
		m_size 			= 0;

//		_growCachedFontSizes(c_maxFontSize);

		FT_Error err = FT_New_Memory_Face(	s_freeTypeLibrary,
											(const FT_Byte *)pFontFile->data(),
											pFontFile->size(),
											faceIndex,
											&m_ftFace );
		if( err )
		{
			//TODO: Error handling...
		}

		setRenderMode( RenderMode::BestShapes );
		setSize( 10*64 );
		
		// Darken the stem if we have gammaCorrection enabled.

#ifdef SUPPORT_STEM_DARKENING
		if (Base::activeContext()->gammaCorrection())
		{
			FT_Parameter         property1;
			FT_Bool              darken_stems = 1;
			property1.tag = FT_PARAM_TAG_STEM_DARKENING;
			property1.data = &darken_stems;

			FT_Parameter         properties[1] = { property1 };

			FT_Face_Properties(m_ftFace, 1, properties);
		}
#endif
	}

	//____ Destructor _____________________________________________________________

	FreeTypeFont::~FreeTypeFont()
	{
		for( int size = 0 ; size < m_nCachedFontSizes ; size++ )
		{
			if (m_pCachedFontSizes[size] != nullptr)
				delete m_pCachedFontSizes[size];
		}

		FT_Done_Face( m_ftFace );

		std::remove(s_instances.begin(), s_instances.end(), this);
        if( s_instances.empty() )
        {
            clearCache();
            FT_Done_FreeType(s_freeTypeLibrary);
        }

	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& FreeTypeFont::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setSize() __________________________________________________________

	bool FreeTypeFont::setSize( spx _size )
	{
		spx size = align(_size);

		if( size == m_size )
			return true;

		if (size < 0)
			return false;

		int pxSize = size/64;

		// Sanity check


		FT_Error err = FT_Set_Char_Size( m_ftFace, size, 0, 0,0 );
//		FT_Error err = FT_Set_Pixel_Sizes( m_ftFace, 0, size );
		if( err )
		{
			m_size = 0;
			return false;
		}


		if (pxSize >= m_nCachedFontSizes)
			_growCachedFontSizes(pxSize + 1);

		m_size = size;
		return true;
	}


	//____ _refreshRenderFlags() _______________________________________________

	void FreeTypeFont::_refreshRenderFlags()
	{
		switch( m_renderMode )
		{
			case RenderMode::Monochrome:
				m_renderFlags = FT_LOAD_MONOCHROME | FT_LOAD_TARGET_MONO;
				break;
			case RenderMode::CrispEdges:
				m_renderFlags = FT_LOAD_TARGET_NORMAL;
				break;
			case RenderMode::BestShapes:
				m_renderFlags = FT_LOAD_TARGET_LIGHT;
				break;

			default:
				break;
		}
	}


	//____ setRenderMode() ________________________________________________________

	bool FreeTypeFont::setRenderMode( RenderMode mode )
	{
		m_renderMode = mode;
		_refreshRenderFlags();
		clearCache();
		return true;
	}

	//____ kerning() ___________________________________________________________

	spx FreeTypeFont::kerning( Glyph& leftGlyph, Glyph& rightGlyph )
	{
		if( leftGlyph.advance == 0 || rightGlyph.advance == 0 || leftGlyph.pFont != this || rightGlyph.pFont != this )
			return 0;

		// Get kerning info

		FT_Vector	delta;
		FT_Get_Kerning( m_ftFace, leftGlyph.kerningIndex, rightGlyph.kerningIndex, FT_KERNING_DEFAULT, &delta );

		return delta.x;
	}

	//____ whitespaceAdvance() _________________________________________________

	spx FreeTypeFont::whitespaceAdvance()
	{
        int pxSize = m_size/64;
        
		if (m_pCachedFontSizes[pxSize] == nullptr)
		{
			m_pCachedFontSizes[pxSize] = new CachedFontSize();
		}


		if( !m_pCachedFontSizes[pxSize]->whitespaceAdvance )
		{
			FT_Error err;

			// Load whitespace glyph

			err = FT_Load_Char( m_ftFace, ' ', FT_LOAD_RENDER );
			if( err )
				return 0;

			// Get and return advance
			m_pCachedFontSizes[pxSize]->whitespaceAdvance = align(m_ftFace->glyph->advance.x);
		}

        return m_pCachedFontSizes[pxSize]->whitespaceAdvance;
	}

	//____ lineGap() ____________________________________________________________

	spx FreeTypeFont::lineGap()
	{
		return align(m_ftFace->size->metrics.height - m_ftFace->size->metrics.ascender + m_ftFace->size->metrics.descender);
	}


	//____ maxAscend() ____________________________________________________________

	spx FreeTypeFont::maxAscend()
	{
		return m_ftFace->size->metrics.ascender;
	}

	//____ maxDescend() ____________________________________________________________

	spx FreeTypeFont::maxDescend()
	{
		return -(m_ftFace->size->metrics.descender);
	}


	//____ nbGlyphs() __________________________________________________________

	int FreeTypeFont::nbGlyphs()
	{
		return int(m_ftFace->num_glyphs);
	}

	//____ hasGlyphs() ____________________________________________________________

	bool FreeTypeFont::hasGlyphs()
	{
		return m_ftFace->num_glyphs?true:false;
	}

	//____ isMonospace() __________________________________________________________

	bool FreeTypeFont::isMonospace()
	{
		return FT_IS_FIXED_WIDTH( m_ftFace )>0?true:false;
	}

	//____ maxAdvance() ___________________________________________________

	spx FreeTypeFont::maxAdvance()
	{
		return align(m_ftFace->size->metrics.max_advance);
	}


	//____ hasGlyph() _____________________________________________________________

	bool FreeTypeFont::hasGlyph( uint16_t ch )
	{
		int index = FT_Get_Char_Index( m_ftFace, ch );
		if( index == 0 )
			return false;

		return true;
	}

	//____ getGlyphWithoutBitmap() _____________________________________________________________

	void FreeTypeFont::getGlyphWithoutBitmap(uint16_t ch, Glyph& glyph)
	{
		// Get cached glyph if we have one

		MyGlyph * pGlyph = _findGlyph( ch, m_size );
		if( pGlyph == 0 )
		{
			FT_Error err;

			// Load MyGlyph

			FT_UInt char_index = FT_Get_Char_Index( m_ftFace, ch );
			if( char_index == 0 )
            {
                // Glyph is missing, try to find it in backup font.

				if (!m_pBackupFont)
					goto no_glyph;

				if (m_pBackupFont->size() == m_size)
					m_pBackupFont->getGlyphWithoutBitmap(ch, glyph);
				else
				{
					spx sz = m_pBackupFont->size();
					m_pBackupFont->setSize(m_size);
					m_pBackupFont->getGlyphWithoutBitmap(ch, glyph);
					m_pBackupFont->setSize(sz);
				}
	            return;            	
			}

			err = FT_Load_Glyph( m_ftFace, char_index, m_renderFlags );
			if (err)
				goto no_glyph;

			// Get some details about the glyph

			spx advance = align(m_ftFace->glyph->advance.x);

			// Get a MyGlyph object and fill in details

			pGlyph = _addGlyph( ch, m_size, advance, char_index );
		}

		glyph.advance		= pGlyph->advance;
		glyph.pFont			= pGlyph->pFont;
		glyph.kerningIndex	= pGlyph->kerningIndex;
		return;

	no_glyph:
		glyph.advance = 0;
		glyph.pFont = 0;
		glyph.kerningIndex = 0;
		return;

	}

	//____ getGlyphWithBitmap() ______________________________________________

	void FreeTypeFont::getGlyphWithBitmap(uint16_t ch, Glyph& glyph)
	{
		// Get cached glyph if we have one

		MyGlyph* pGlyph = _findGlyph(ch, m_size);
		if (pGlyph == 0)
		{
			FT_Error err;

			// Load MyGlyph

			FT_UInt char_index = FT_Get_Char_Index(m_ftFace, ch);
			if (char_index == 0)
			{
				// Glyph is missing, try to find it in backup font.

				if (!m_pBackupFont)
					goto no_glyph;

				if (m_pBackupFont->size() == m_size)
					m_pBackupFont->getGlyphWithoutBitmap(ch, glyph);
				else
				{
					spx sz = m_pBackupFont->size();
					m_pBackupFont->setSize(m_size);
					m_pBackupFont->getGlyphWithoutBitmap(ch, glyph);
					m_pBackupFont->setSize(sz);
				}
				return;
			}

			err = FT_Load_Glyph(m_ftFace, char_index, m_renderFlags);
			if (err)
				goto no_glyph;

			// Get some details about the glyph

			spx advance = align(m_ftFace->glyph->advance.x);

			// Get a MyGlyph object and fill in details

			pGlyph = _addGlyph(ch, m_size, advance, char_index);
		}

		glyph.pFont = pGlyph->pFont;
		glyph.advance = pGlyph->advance;
		glyph.kerningIndex = pGlyph->kerningIndex;

		if (pGlyph->pSurface == nullptr)
			_generateBitmap(pGlyph);

		glyph.pSurface = pGlyph->pSurface;
		glyph.rect = pGlyph->rect;
		glyph.bearingX = pGlyph->bearingX;
		glyph.bearingY = pGlyph->bearingY;
		return;

	no_glyph:
		glyph.pFont = 0;
		glyph.advance = 0;
		glyph.kerningIndex = 0;
		glyph.pSurface = nullptr;
		return;
	}



	//____ _generateBitmap() _______________________________________________________

	void FreeTypeFont::_generateBitmap( MyGlyph * pGlyph )
	{
		FT_Error err;

        //

        bool bDifferentSize = pGlyph->size != m_size;

        if( bDifferentSize )
             FT_Set_Char_Size( m_ftFace, align(pGlyph->size), 0, 0,0 );

		// Load MyGlyph

		err = FT_Load_Glyph( m_ftFace, pGlyph->kerningIndex, FT_LOAD_RENDER | FT_LOAD_COLOR | m_renderFlags );
		if( err )
        {
			//TODO: Error handling!
			
            if( bDifferentSize )
                FT_Set_Char_Size( m_ftFace, m_size, 0, 0,0 );
            return;
        }

		// Get some details about the glyph

		int width = m_ftFace->glyph->bitmap.width;
		int height = m_ftFace->glyph->bitmap.rows;

		// Get a cache slot

		_getCacheSlot( width, height, pGlyph );

		pGlyph->bearingX = m_ftFace->glyph->bitmap_left*64;
		pGlyph->bearingY = -m_ftFace->glyph->bitmap_top*64;


		_copyBitmap( &m_ftFace->glyph->bitmap, pGlyph );	// Copy our glyph bitmap to the slot

        if( bDifferentSize )
            FT_Set_Char_Size( m_ftFace, m_size, 0, 0,0 );
	}


	//____ _copyBitmap() ____________________________________________________________

	void FreeTypeFont::_copyBitmap( FT_Bitmap * pFTBitmap, MyGlyph * pGlyph )
	{
		Surface_p pSurf = pGlyph->pSurface;

		auto pixbuf = pSurf->allocPixelBuffer(pGlyph->rect/64);

		assert( pSurf->pixelDescription()->format == PixelFormat::A_8);

		// Copy glyph bitmap into alpha channel of slot, making sure to clear any
		// left over area of slots alpha channel.


		switch( pFTBitmap->pixel_mode )
		{
			case (FT_PIXEL_MODE_MONO):
				_copyA1ToA8( pFTBitmap->buffer, pFTBitmap->width, pFTBitmap->rows, pFTBitmap->pitch, pixbuf.pPixels, pixbuf.pitch );
				break;
			case (FT_PIXEL_MODE_GRAY):
				_copyA8ToA8( pFTBitmap->buffer, pFTBitmap->width, pFTBitmap->rows, pFTBitmap->pitch, pixbuf.pPixels, pixbuf.pitch );
				break;
			case (FT_PIXEL_MODE_BGRA):
				_copyBGRA8ToBGRA8(pFTBitmap->buffer, pFTBitmap->width, pFTBitmap->rows, pFTBitmap->pitch, pixbuf.pPixels, pixbuf.pitch);
				break;
			default:
				assert( false );
		}

		pSurf->pullPixels(pixbuf);
		pSurf->freePixelBuffer(pixbuf);
	}


	//____ _copyA8ToA8() _____________________________________________________

	void FreeTypeFont::_copyA8ToA8( const uint8_t * pSrc, int src_width, int src_height, int src_pitch,
										uint8_t * pDest, int dest_pitch )
	{
		for( int y = 0 ; y < src_height ; y++ )
		{
			for( int x = 0 ; x < src_width ; x++ )
				pDest[x] = pSrc[x];

			pSrc  += src_pitch;
			pDest += dest_pitch;
		}

	}

	//____ _copyA1ToA8() _____________________________________________________

	void FreeTypeFont::_copyA1ToA8( const uint8_t * pSrc, int src_width, int src_height, int src_pitch,
										uint8_t * pDest, int dest_pitch )
	{
		uint8_t lookup[2] = { 0, 255 };

		for( int y = 0 ; y < src_height ; y++ )
		{
			for( int x = 0 ; x < src_width ; x++ )
				pDest[x] = lookup[(((pSrc[x>>3])<<(x&7))&0xFF)>>7];
		
			pSrc  += src_pitch;
			pDest += dest_pitch;
		}
	}

	//____ _copyBGRA8ToBGRA8() _____________________________________________________

	void FreeTypeFont::_copyBGRA8ToBGRA8(const uint8_t* pSrc, int src_width, int src_height, int src_pitch,
		uint8_t* pDest, int dest_pitch)
	{
		for (int y = 0; y < src_height; y++)
		{
			for (int x = 0; x < src_width*4; x++)
				pDest[x] = pSrc[x];

			pSrc += src_pitch;
			pDest += dest_pitch;
		}

	}


	//___ _addGlyph() ________________________________________________________

	FreeTypeFont::MyGlyph * FreeTypeFont::_addGlyph( uint16_t ch, spx size, spx advance, uint32_t kerningIndex )
	{
		int szOfs = size/64;

		if (m_pCachedFontSizes[szOfs] == nullptr)
			m_pCachedFontSizes[szOfs] = new CachedFontSize();

		if (m_pCachedFontSizes[szOfs]->page[ch >> 7] == nullptr)
			m_pCachedFontSizes[szOfs]->page[ch >> 7] = new MyGlyph[128];

		assert( !m_pCachedFontSizes[szOfs]->page[ch>>7][ch & 0x7F].isInitialized() );

		auto pGlyph = &m_pCachedFontSizes[szOfs]->page[ch >> 7][ch & 0x7F]; 
		
		pGlyph->character = ch;
		pGlyph->size = size;
		pGlyph->advance = advance;
		pGlyph->kerningIndex = kerningIndex;
		pGlyph->pFont = this;
		
		return &m_pCachedFontSizes[szOfs]->page[ch >> 7][ch & 0x7F];
	}

	//____ setCacheLimit() ________________________________________________________

	void FreeTypeFont::setCacheLimit(int maxBytes)
	{
		if (maxBytes < 0)
		{
			Base::handleError(ErrorSeverity::SilentFail, ErrorCode::InvalidParam, "You can not set cache to a negative size!", nullptr, TYPEINFO, __func__, __FILE__, __LINE__);
			return;
		}

		s_cacheLimit = maxBytes;

		if (s_cacheLimit > 0 && s_cacheSize > s_cacheLimit)
			_truncateCache(s_cacheLimit);
	}

	//____ clearCache() ___________________________________________________________

	void FreeTypeFont::clearCache()
	{
		// Forget all cache references

		for (auto pFont : s_instances)
			pFont->_forgetAllCacheReferences();

		// Clear cache

		for (int i = 0; i < 10; i++)
			s_cacheSurfaces[i].clear();

		s_cacheSize = 0;

	}

	//____ _forgetAllCacheReferences() _____________________________________________

	void FreeTypeFont::_forgetAllCacheReferences()
	{
		auto pSizes = m_pCachedFontSizes;

		for (int size = 0; size < m_nCachedFontSizes; size++)
		{
			if (m_pCachedFontSizes[size] != nullptr)
			{
				auto pPages = m_pCachedFontSizes[size]->page;

				for (int page = 0; page < 512; page++)
				{
					if (pPages[page] != nullptr)
					{
						auto pGlyphs = pPages[page];
						for( int glyph = 0 ; glyph < 128 ; glyph++ )
							pGlyphs[glyph].bitmapLost();
					}
				}
			}
		}
	}

	//____ _forgetCacheReferences() _____________________________________________

	void FreeTypeFont::_forgetCacheReferences( int nRemovedSurfaces, Surface * pRemovedSurfaces[] )
	{
		auto pSizes = m_pCachedFontSizes;

		for (int size = 0; size < m_nCachedFontSizes; size++)
		{
			if (m_pCachedFontSizes[size] != nullptr)
			{
				auto pPages = m_pCachedFontSizes[size]->page;

				for (int page = 0; page < 512; page++)
				{
					if (pPages[page] != nullptr)
					{
						auto pGlyphs = pPages[page];
						for (int glyph = 0; glyph < 128; glyph++)
						{
							Surface* pGlyphSurf = pGlyphs[glyph].pSurface;
							if (pGlyphSurf)
							{
								for (int i = 0; i < nRemovedSurfaces; i++)
								{
									if (pGlyphSurf == pRemovedSurfaces[i])
									{
										pGlyphs[glyph].bitmapLost();
										break;
									}
								}
							}
						}
					}
				}
			}
		}
	}


	//____ _getCacheSlot() _________________________________________________________

	void FreeTypeFont::_getCacheSlot( int width, int height, MyGlyph * pGlyph )
	{
		int category = height > 128 ? 9 : s_sizeToCategory[height];

		CacheSurf * pCacheSurf;

		if( category == 9  )
			pCacheSurf = _addCacheSurface( category, width, height );
		else if(s_cacheSurfaces[category].empty())
			pCacheSurf = _addCacheSurface(category, 1024, s_categoryHeight[category]);
		else 
		{
			auto& ref = s_cacheSurfaces[category].back();
			
			if( ref.capacity - ref.used >= width )
				pCacheSurf = &ref;
			else
				pCacheSurf = _addCacheSurface( category, 1024, s_categoryHeight[category]);
		}
		
		pGlyph->pSurface = pCacheSurf->pSurface;
		pGlyph->rect.x = pCacheSurf->used*64;
		pGlyph->rect.y = 0;
		pGlyph->rect.w = width*64;
		pGlyph->rect.h = height*64;
		
		pCacheSurf->used += width;
	}

	//____ _addCacheSurface() __________________________________________________

	FreeTypeFont::CacheSurf * FreeTypeFont::_addCacheSurface( int category, int width, int height )
	{
		auto pFactory = Base::activeContext()->surfaceFactory();
		
		Surface_p pSurf;
		
		if( category == 9 )
			pSurf = pFactory->createSurface( {width,height}, PixelFormat::A_8 );
		else
			pSurf = pFactory->createSurface( {width,s_categoryHeight[category]}, PixelFormat::A_8 );
	
		s_cacheSize += width * height;

		s_cacheSurfaces[category].emplace_back(pSurf,width);
		return &s_cacheSurfaces[category].back();
	}

	//____ _truncateCache() ___________________________________________________

	void FreeTypeFont::_truncateCache(int maxSize)
	{
		Surface*	removedSurfaces[32];
		int			nRemovedSurfaces = 0;

		// Remove cache entries

		while (s_cacheSize > maxSize && nRemovedSurfaces < 32 )
		{
			// Find category with oldest cache entry

			uint32_t lowestCreationNb = s_cacheSurfacesCreated;
			int oldestCat = -1;
			for (int cat = 0; cat < 10; cat++)
			{
				if (!s_cacheSurfaces[cat].empty() && s_cacheSurfaces[cat].front().creationNb < lowestCreationNb)
					oldestCat = cat;
			}

			auto& toRemove = s_cacheSurfaces[oldestCat].front();

			// Decreaste s_cacheSize

			SizeI pixels = toRemove.pSurface->pixelSize();
			s_cacheSize -= pixels.w * pixels.h;

			// Remove surface and add to list of removed.

			removedSurfaces[nRemovedSurfaces++] = toRemove.pSurface;
			s_cacheSurfaces[oldestCat].erase(s_cacheSurfaces[oldestCat].begin());
		}

		// Remove references to bitmaps in removed cache surfaces

		for (auto pFont : s_instances)
			pFont->_forgetCacheReferences(nRemovedSurfaces, removedSurfaces);

		// Continue recursively if we need to remove more than we could in this round.

		if (s_cacheSize > maxSize)
			_truncateCache(maxSize);
	}


	//____ _growCachedFontSizes() ________________________________________________

	void FreeTypeFont::_growCachedFontSizes(int newSize)
	{
		auto pNew = new CachedFontSize*[newSize];

		for (int i = 0; i < m_nCachedFontSizes; i++)
			pNew[i] = m_pCachedFontSizes[i];

		for (int i = m_nCachedFontSizes; i < newSize; i++)
			pNew[i] = nullptr;

		delete m_pCachedFontSizes;
		m_pCachedFontSizes = pNew;
		m_nCachedFontSizes = newSize;
	}

/*
	FreeTypeFont::MyGlyph::MyGlyph( uint16_t character, spx size, spx advance, uint32_t kerningIndex, Font * pFont )
	: Glyph( advance, kerningIndex, pFont )
	{
		m_size = size;
		m_character = character;
	}
*/

} // namespace wg
