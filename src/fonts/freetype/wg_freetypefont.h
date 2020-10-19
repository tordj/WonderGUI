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


#include <wg_types.h>
#include <wg_chain.h>


#include <wg_font.h>
#include <wg_surfacefactory.h>
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

		inline bool setRenderMode( RenderMode mode ) { return setRenderMode( mode, 0, 0xFFFF ); }
		inline bool setRenderMode( RenderMode mode, int size ) { return setRenderMode(mode,size,size); }
		bool		setRenderMode( RenderMode mode, int startSize, int endSize );
		inline RenderMode	renderMode( int size ) const { if( size >= 0 && size <= c_maxFontSize ) return m_renderMode[size]; else return RenderMode::Monochrome; }


	private:
		FreeTypeFont( Blob_p pFontFile, int faceIndex );
		~FreeTypeFont();

		const static int	c_maxFontSize = 256;	// Max size (pixels) for font.
		const static int	c_minGlyphPixelSize = 12;
		const static int	c_maxGlyphPixelSize = c_maxFontSize*2;
		const static int	c_glyphPixelSizeQuantization = 4;
		const static int	c_glyphSlotSizes = ((c_maxGlyphPixelSize-c_minGlyphPixelSize)/c_glyphPixelSizeQuantization)+1;

		class CacheSlot;

		class MyGlyph : public Glyph
		{
		public:
			MyGlyph();
			MyGlyph( uint16_t character, MU size, MU advance, uint32_t kerningIndex, Font * pFont );
			~MyGlyph();
			const GlyphBitmap * getBitmap();

			void	slotLost() { m_pSlot = 0; }
			bool	isInitialized() { return m_pFont?true:false; }

			CacheSlot * m_pSlot;
			MU			m_size;			// size of character.
			uint16_t	m_character;	// Unicode for character.
		};

		class CacheSurf : public Link
		{
		public:
			CacheSurf( Surface * _pSurf ) { pSurf = _pSurf; access = 0; }
			~CacheSurf();

			LINK_METHODS( CacheSurf );

			uint32_t			access;			// Timestamp of last access.
			Surface_p	pSurf;
		};


		class CacheSlot : public Link
		{
		public:
			CacheSlot( CacheSurf * _pSurf, const RectI& _rect ) { access = 0; pSurf = _pSurf; rect = _rect; bitmap.pSurface = pSurf->pSurf; pGlyph = 0; }

			LINK_METHODS( CacheSlot );

			uint32_t			access;			// Timestamp of last access.

			GlyphBitmap	bitmap;
			MyGlyph *			pGlyph;

			CacheSurf *		pSurf;
			RectI			rect;				// RectI for the slot - not the glyph itself as in GlyphBitmap which might be smaller.
		};


		void				_copyA8ToRGBA8( const uint8_t * pSrc, int src_width, int src_height, int src_pitch, uint32_t * pDest, int dest_width, int dest_height, int dest_pitch );
		void				_copyA1ToRGBA8( const uint8_t * pSrc, int src_width, int src_height, int src_pitch, uint32_t * pDest, int dest_width, int dest_height, int dest_pitch );


		bool				_setCharSize( int size );

		CacheSlot *			_generateBitmap( MyGlyph * pGlyph );
		void				_copyBitmap( FT_Bitmap * pBitmap, CacheSlot * pSlot );

		MyGlyph *			_addGlyph( uint16_t ch, MU size, MU advance, uint32_t kerningIndex );
		inline MyGlyph *	_findGlyph( uint16_t glyph, MU size ) const;

		inline void			_touchSlot( CacheSlot * pSlot );
		void				_refreshRenderFlags();


		FT_Face				m_ftFace;
		Blob_p				m_pFontFile;
		char*				m_pData;
		MyGlyph **			m_cachedGlyphsIndex[c_maxFontSize+1];
		uint32_t			m_accessCounter;
		int					m_renderFlags;
		RenderMode			m_renderMode[c_maxFontSize+1];
		MU					m_whitespaceAdvance[c_maxFontSize+1];
		MU					m_size;

		//____ Static stuff __________________________________________________________




		static CacheSlot *	getCacheSlot( int width, int height );
		static int			addCacheSlots( Chain<CacheSlot> * pChain, const SizeI& slotSize, int minSlots );
		static int			maxSlotsInSurface( const SizeI& surf, const SizeI& slot );
		static SizeI			calcTextureSize( const SizeI& slotSize, int nSlots );

		static int			s_instanceCounter;
		static FT_Library	s_freeTypeLibrary;

		static Chain<CacheSlot>	s_cacheSlots[c_glyphSlotSizes];
		static Chain<CacheSurf>	s_cacheSurfaces;
		static SurfaceFactory_p	s_pSurfaceFactory;

		//____



	};

	//____ _findGlyphInIndex() _______________________________________________________

	FreeTypeFont::MyGlyph * FreeTypeFont::_findGlyph( uint16_t ch, MU size ) const
	{
		int sizeOfs = size.px();

		if( m_cachedGlyphsIndex[sizeOfs] != 0 && m_cachedGlyphsIndex[sizeOfs][ch>>8] != 0 && m_cachedGlyphsIndex[sizeOfs][ch>>8][ch&0xFF].isInitialized() )
			return &m_cachedGlyphsIndex[sizeOfs][ch>>8][ch&0xFF];

		return 0;
	}

	//____ _touchSlot() _________________________________________________________

	void FreeTypeFont::_touchSlot( CacheSlot * pSlot )
	{
		pSlot->moveFirst();								// Move slot to the top

		pSlot->access = m_accessCounter;				// Increase access counter.
		pSlot->pSurf->access = m_accessCounter++;		// We don't sort the surfaces, probably faster to just compare access when
														// we need to destroy one?
	}

} // namespace wg

#endif //WG_FREETYPEFONT_DOT_H
