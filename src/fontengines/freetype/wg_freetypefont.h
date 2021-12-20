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


#ifndef WG_FREETYPEFONT_DOT_H
#define WG_FREETYPEFONT_DOT_H
#pragma once

#include <vector>

#include <wg_types.h>
#include <wg_chain.h>


#include <wg_font.h>
#include <wg_blob.h>


typedef struct FT_LibraryRec_  *FT_Library;

struct	FT_FaceRec_;
typedef struct FT_FaceRec_*	FT_Face;
typedef struct  FT_Bitmap_ FT_Bitmap;

namespace wg
{
	class FreeTypeFont;
	typedef	StrongPtr<FreeTypeFont>		FreeTypeFont_p;
	typedef	WeakPtr<FreeTypeFont>	FreeTypeFont_wp;

	class FreeTypeFont : public Font
	{
	public:

		enum class RenderMode
		{
			Monochrome,
			CrispEdges,
			BestShapes
		};

		//.____ Creation __________________________________________

		static FreeTypeFont_p	create( Blob_p pFontFile, int faceIndex = 0 ) { return FreeTypeFont_p(new FreeTypeFont(pFontFile,faceIndex)); }

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Rendering ______________________________________________________

		bool		setSize( spx size ) override;
		inline spx	size() override { return m_size; }

		void			getGlyphWithoutBitmap(uint16_t chr, Glyph& glyph) override;
		void			getGlyphWithBitmap(uint16_t chr, Glyph& glyph) override;
		spx				kerning(Glyph& leftGlyph, Glyph& rightGlyph) override;

		spx			lineGap() override;
		spx			whitespaceAdvance() override;
		spx			maxAdvance() override;
		spx			maxAscend() override;
		spx			maxDescend() override;

		//.____ Misc ___________________________________________________________

		int			nbGlyphs() override;
		bool		hasGlyphs() override;
		bool		hasGlyph( uint16_t chr ) override;
		bool		isMonospace() override;


		static void setCacheLimit(int maxBytes);
		static int	cacheLimit() { return s_cacheLimit; }

		static int	cacheSize() { return s_cacheSize; }

		static void	clearCache();

		//.____ Appearance ___________________________________________

		inline bool 		setRenderMode( RenderMode mode );
		inline RenderMode	renderMode( int size ) const { return m_renderMode; }


	private:
		FreeTypeFont( Blob_p pFontFile, int faceIndex );
		~FreeTypeFont();

		class MyGlyph
		{
		public:
			inline void	bitmapLost() { pSurface = nullptr; }
			inline bool isInitialized() { return pFont != nullptr; }

			Font*		pFont = nullptr;	// Set to null if glyph not initialized.
			uint16_t	character = 0;			// Unicode for character.
			spx			size = 0;				// size of character.

			spx			advance = 0;
			uint32_t	kerningIndex = 0;		//

			Surface_p 	pSurface;			// Set to null if no bitmap for glyph could be found.
			RectSPX		rect;
			spx			bearingX = 0;			// x offset when rendering the glyph (negated offset to glyph origo)
			spx			bearingY = 0;			// y offset when rendering the glyph (negated offset to glyph origo)
		};

		struct CacheSurf
		{
			CacheSurf( Surface * _pSurface, int _capacity ) : 
				pSurface(_pSurface), 
				capacity(_capacity), 
				used(0)
				{
					creationNb = FreeTypeFont::s_cacheSurfacesCreated++;
				}
				
			
			Surface_p	pSurface;
			int			capacity;
			int			used;
			
			uint32_t	creationNb;
		};

		struct CachedFontSize
		{
			CachedFontSize() 
			{
				whitespaceAdvance = 0;
				for (int i = 0; i < 512; i++)
					page[i] = nullptr;
			}

			~CachedFontSize()
			{
				for (int i = 0; i < 512; i++)
					if( page[i] )
						delete [] page[i];
			}

			spx			whitespaceAdvance;
			MyGlyph*	page[512];
		};

		static void 		_getCacheSlot(int width, int height, MyGlyph* pBitmap);
		static CacheSurf*	_addCacheSurface(int category, int width, int height);
		static void			_truncateCache(int maxSize);


		void				_copyA8ToA8( const uint8_t * pSrc, int src_width, int src_height, int src_pitch, uint8_t * pDest, int dest_pitch );
		void				_copyA1ToA8( const uint8_t * pSrc, int src_width, int src_height, int src_pitch, uint8_t * pDest, int dest_pitch );
		void				_copyBGRA8ToBGRA8(const uint8_t* pSrc, int src_width, int src_height, int src_pitch, uint8_t* pDest, int dest_pitch);

		void				_generateBitmap( MyGlyph * pGlyph );
		void				_copyBitmap( FT_Bitmap * pBitmap, MyGlyph * pSlot );

		MyGlyph *			_addGlyph( uint16_t ch, spx size, spx advance, uint32_t kerningIndex );
		inline MyGlyph *	_findGlyph( uint16_t glyph, spx size ) const;

		void				_refreshRenderFlags();

		void				_growCachedFontSizes(int newSize);
		void				_forgetCacheReferences(int nRemovedSurfaces, Surface* pRemovedSurfaces[]);
		void				_forgetAllCacheReferences();


		FT_Face				m_ftFace;
		Blob_p				m_pFontFile;

		CachedFontSize **	m_pCachedFontSizes = nullptr;				//	[X sizes][512 codepages][128 glyphs/codepage]
		int					m_nCachedFontSizes = 0;

		uint32_t			m_surfCreationNb = 0;

		int					m_renderFlags;
		RenderMode			m_renderMode;
		spx					m_size;

		//____ Static stuff __________________________________________________________

		static FT_Library			s_freeTypeLibrary;

		static	std::vector<FreeTypeFont*>	s_instances;		// List of alive FreeTypeFont-objects.


		static std::vector<CacheSurf>s_cacheSurfaces[10];		// The nine combined surfaces + the tenth vector with all character-specific surfaces.


		static uint32_t				s_cacheSurfacesCreated;		// Increments and provides m_surfCreationNb for each new surface created.
		static int					s_cacheSize;				// Size in bytes of existing cacheSurfaces. Used and unused cache space.
		static int					s_cacheLimit;				// Maximum size of cache in bytes. 0 = no limit.

		const static uint8_t		s_categoryHeight[9];		// Pixel heights for cache surfaces in each category.
		const static uint8_t		s_sizeToCategory[129];		// Table for grouping sizes into categories.
		


	};

	//____ _findGlyphInIndex() _______________________________________________________

	FreeTypeFont::MyGlyph * FreeTypeFont::_findGlyph( uint16_t ch, spx size ) const
	{
		int sizeOfs = (size+32)/64;

		if( m_pCachedFontSizes[sizeOfs] != nullptr && m_pCachedFontSizes[sizeOfs]->page[ch>>7] != nullptr && m_pCachedFontSizes[sizeOfs]->page[ch >> 7][ch&0x7F].isInitialized() )
			return &m_pCachedFontSizes[sizeOfs]->page[ch >> 7][ch & 0x7F];

		return 0;
	}


} // namespace wg

#endif //WG_FREETYPEFONT_DOT_H
