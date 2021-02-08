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

		bool		setSize( MU size ) override;
		inline MU	size() override { return m_size; }

		MU			kerning( Glyph_p pLeftGlyph, Glyph_p pRightGlyph ) override;
		Glyph_p		getGlyph( uint16_t chr ) override;

		MU			lineGap() override;
		MU			whitespaceAdvance() override;
		MU			maxAdvance() override;
		MU			maxAscend() override;
		MU			maxDescend() override;

		//.____ Misc ___________________________________________________________

		int			nbGlyphs() override;
		bool		hasGlyphs() override;
		bool		hasGlyph( uint16_t chr ) override;
		bool		isMonospace() override;

		static void	clearCache();

		//.____ Appearance ___________________________________________

		inline bool 		setRenderMode( RenderMode mode );
		inline RenderMode	renderMode( int size ) const { return m_renderMode; }


	private:
		FreeTypeFont( Blob_p pFontFile, int faceIndex );
		~FreeTypeFont();

		const static int	c_maxFontSize = 1024;	// Max size (pixels) for font.


		class MyGlyph : public Glyph
		{
		public:
			MyGlyph();
			MyGlyph( uint16_t character, MU size, MU advance, uint32_t kerningIndex, Font * pFont );
			const GlyphBitmap * getBitmap();

			void	slotLost() { bitmap.pSurface = nullptr; }
			bool	isInitialized() { return m_pFont?true:false; }

			GlyphBitmap	bitmap;
			MU			m_size;			// size of character.
			uint16_t	m_character;	// Unicode for character.
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

		void				_copyA8ToRGBA8( const uint8_t * pSrc, int src_width, int src_height, int src_pitch, uint32_t * pDest, int dest_width, int dest_height, int dest_pitch );
		void				_copyA1ToRGBA8( const uint8_t * pSrc, int src_width, int src_height, int src_pitch, uint32_t * pDest, int dest_width, int dest_height, int dest_pitch );


		bool				_setCharSize( int size );

		void				_generateBitmap( MyGlyph * pGlyph );
		void				_copyBitmap( FT_Bitmap * pBitmap, GlyphBitmap * pSlot );

		MyGlyph *			_addGlyph( uint16_t ch, MU size, MU advance, uint32_t kerningIndex );
		inline MyGlyph *	_findGlyph( uint16_t glyph, MU size ) const;

		void				_refreshRenderFlags();

		void 				_getCacheSlot( int width, int height, GlyphBitmap * pBitmap );
		CacheSurf * 		_addCacheSurface( int category, int width, int height );


		FT_Face				m_ftFace;
		Blob_p				m_pFontFile;
		MyGlyph **			m_cachedGlyphsIndex[c_maxFontSize+1];
		MU					m_whitespaceAdvance[c_maxFontSize+1];
		uint32_t			m_surfCreationNb = 0;

		int					m_renderFlags;
		RenderMode			m_renderMode;
		MU					m_size;

		//____ Static stuff __________________________________________________________


		static int					s_instanceCounter;
		static FT_Library			s_freeTypeLibrary;

		static std::vector<CacheSurf>s_cacheSurfaces[10];		// The nine combined surfaces + the tenth vector with all character-specific surfaces.

		//____

		static uint32_t				s_cacheSurfacesCreated;

		const static uint8_t		s_categoryHeight[9];
		const static uint8_t		s_sizeToCategory[129];
		
		
	};

	//____ _findGlyphInIndex() _______________________________________________________

	FreeTypeFont::MyGlyph * FreeTypeFont::_findGlyph( uint16_t ch, MU size ) const
	{
		int sizeOfs = size.px();

		if( m_cachedGlyphsIndex[sizeOfs] != 0 && m_cachedGlyphsIndex[sizeOfs][ch>>8] != 0 && m_cachedGlyphsIndex[sizeOfs][ch>>8][ch&0xFF].isInitialized() )
			return &m_cachedGlyphsIndex[sizeOfs][ch>>8][ch&0xFF];

		return 0;
	}


} // namespace wg

#endif //WG_FREETYPEFONT_DOT_H
