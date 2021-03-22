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

#include <wg_mu.h>
#include <wg_freetypefont.h>
#include <wg_surface.h>
#include <wg_surfacefactory.h>
#include <wg_base.h>
#include <wg_context.h>
#include <assert.h>


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
		setSize( 10 );
		
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

	bool FreeTypeFont::setSize( MU size )
	{
		if( size == m_size )
			return true;

		if (size < 0)
			return false;

		// Sanity check


		FT_Error err = FT_Set_Char_Size( m_ftFace, size.px()*64, 0, 0,0 );
//		FT_Error err = FT_Set_Pixel_Sizes( m_ftFace, 0, size );
		if( err )
		{
			m_size = 0;
			return false;
		}

		if (size >= m_nCachedFontSizes)
			_growCachedFontSizes(int(size) + 1);

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

	MU FreeTypeFont::kerning( Glyph_p pLeftGlyph, Glyph_p pRightGlyph )
	{
		if( !pLeftGlyph || !pRightGlyph || pLeftGlyph->_font() != this || pRightGlyph->_font() != this )
			return 0;

		// Get kerning info

		FT_Vector	delta;
		FT_Get_Kerning( m_ftFace, pLeftGlyph->kerningIndex(), pRightGlyph->kerningIndex(), FT_KERNING_DEFAULT, &delta );

		return MU::fromPX(int(delta.x >> 6));
	}

	//____ whitespaceAdvance() _________________________________________________

	MU FreeTypeFont::whitespaceAdvance()
	{
        int pxSize = m_size.px();
        
		if (m_pCachedFontSizes[pxSize] == nullptr)
		{
			m_pCachedFontSizes[pxSize] = new CachedFontSize();
		}


		if( !m_pCachedFontSizes[pxSize]->whitespaceAdvance.qpix )
		{
			FT_Error err;

			// Load whitespace glyph

			err = FT_Load_Char( m_ftFace, ' ', FT_LOAD_RENDER );
			if( err )
				return 0;

			// Get and return advance
			m_pCachedFontSizes[pxSize]->whitespaceAdvance = MU::fromPX(int(m_ftFace->glyph->advance.x >> 6));
		}

        return m_pCachedFontSizes[pxSize]->whitespaceAdvance;
	}

	//____ lineGap() ____________________________________________________________

	MU FreeTypeFont::lineGap()
	{
		return MU::fromPX(int(m_ftFace->size->metrics.height - m_ftFace->size->metrics.ascender + m_ftFace->size->metrics.descender) >> 6);
	}


	//____ maxAscend() ____________________________________________________________

	MU FreeTypeFont::maxAscend()
	{
		return MU::fromPX(int(m_ftFace->size->metrics.ascender) >> 6);
	}

	//____ maxDescend() ____________________________________________________________

	MU FreeTypeFont::maxDescend()
	{
		return MU::fromPX(-int(m_ftFace->size->metrics.descender) >> 6);
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

	MU FreeTypeFont::maxAdvance()
	{
		return MU::fromPX(int(m_ftFace->size->metrics.max_advance >> 6));
	}


	//____ hasGlyph() _____________________________________________________________

	bool FreeTypeFont::hasGlyph( uint16_t ch )
	{
		int index = FT_Get_Char_Index( m_ftFace, ch );
		if( index == 0 )
			return false;

		return true;
	}

	//____ getGlyph() _____________________________________________________________

	Glyph_p FreeTypeFont::getGlyph( uint16_t ch )
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

                if( m_pBackupFont )
                {
                    if( m_pBackupFont->size() == m_size )
                        return m_pBackupFont->getGlyph(ch);
                    else
                    {
                        MU sz = m_pBackupFont->size();
                        m_pBackupFont->setSize(m_size);
                        Glyph_p pGlyph = m_pBackupFont->getGlyph(ch);
                        m_pBackupFont->setSize(sz);
                        return pGlyph;
                    }
                }

                return 0;            // We got index for missing glyph.
            }

			err = FT_Load_Glyph( m_ftFace, char_index, m_renderFlags );
			if( err )
				return 0;

			// Get some details about the glyph

			MU advance = MU::fromPX(int(m_ftFace->glyph->advance.x >> 6));

			// Get a MyGlyph object and fill in details

			pGlyph = _addGlyph( ch, m_size, advance, char_index );
		}

		return pGlyph;
	}




	//____ _generateBitmap() _______________________________________________________

	void FreeTypeFont::_generateBitmap( MyGlyph * pGlyph )
	{
		FT_Error err;

        //

        bool bDifferentSize = pGlyph->m_size != m_size;

        if( bDifferentSize )
             FT_Set_Char_Size( m_ftFace, pGlyph->m_size.px()*64, 0, 0,0 );

		// Load MyGlyph

		err = FT_Load_Glyph( m_ftFace, pGlyph->kerningIndex(), FT_LOAD_RENDER | FT_LOAD_COLOR | m_renderFlags );
		if( err )
        {
			//TODO: Error handling!
			
            if( bDifferentSize )
                FT_Set_Char_Size( m_ftFace, m_size.px()*64, 0, 0,0 );
            return;
        }

		// Get some details about the glyph

		int width = m_ftFace->glyph->bitmap.width;
		int height = m_ftFace->glyph->bitmap.rows;

		// Get a cache slot

		_getCacheSlot( width, height, &pGlyph->bitmap );

		pGlyph->bitmap.bearingX = MU::fromPX(m_ftFace->glyph->bitmap_left);
		pGlyph->bitmap.bearingY = MU::fromPX(-m_ftFace->glyph->bitmap_top);


		_copyBitmap( &m_ftFace->glyph->bitmap, &pGlyph->bitmap );	// Copy our glyph bitmap to the slot

        if( bDifferentSize )
            FT_Set_Char_Size( m_ftFace, m_size.px()*64, 0, 0,0 );
	}


	//____ _copyBitmap() ____________________________________________________________

	// Currently only supports 32-bit RGBA surfaces!

	void FreeTypeFont::_copyBitmap( FT_Bitmap * pFTBitmap, GlyphBitmap * pGlyphBitmap )
	{
		Surface_p pSurf = pGlyphBitmap->pSurface;

		auto pixbuf = pSurf->allocPixelBuffer(pGlyphBitmap->rect);

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

	FreeTypeFont::MyGlyph * FreeTypeFont::_addGlyph( uint16_t ch, MU size, MU advance, uint32_t kerningIndex )
	{
		int szOfs = size.px();

		if (m_pCachedFontSizes[szOfs] == nullptr)
			m_pCachedFontSizes[szOfs] = new CachedFontSize();

		if (m_pCachedFontSizes[szOfs]->page[ch >> 7] == nullptr)
			m_pCachedFontSizes[szOfs]->page[ch >> 7] = new MyGlyph[128];

		assert( !m_pCachedFontSizes[szOfs]->page[ch>>7][ch & 0x7F].isInitialized() );

		m_pCachedFontSizes[szOfs]->page[ch >> 7][ch & 0x7F] = MyGlyph( ch, size, advance, kerningIndex, this );

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
							Surface* pGlyphSurf = pGlyphs[glyph].bitmap.pSurface;
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

	void FreeTypeFont::_getCacheSlot( int width, int height, GlyphBitmap * pBitmap )
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
		
		pBitmap->pSurface = pCacheSurf->pSurface;
		pBitmap->rect.x = pCacheSurf->used;
		pBitmap->rect.y = 0;
		pBitmap->rect.w = width;
		pBitmap->rect.h = height;
		
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

			SizeI pixels = toRemove.pSurface->size();
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


	FreeTypeFont::MyGlyph::MyGlyph()
	: Glyph( 0, 0, nullptr )
	{
		m_size = 0;
		m_character = 0;
	}


	FreeTypeFont::MyGlyph::MyGlyph( uint16_t character, MU size, MU advance, uint32_t kerningIndex, Font * pFont )
	: Glyph( advance, kerningIndex, pFont )
	{
		m_size = size;
		m_character = character;
	}


	const GlyphBitmap * FreeTypeFont::MyGlyph::getBitmap()
	{
		if( bitmap.pSurface == nullptr )
		{
			((FreeTypeFont*)m_pFont)->_generateBitmap( this );
		}

		return &bitmap;
	}

} // namespace wg
