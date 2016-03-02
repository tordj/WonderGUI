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


#ifndef WG_VECTORFONT_DOT_H
#define WG_VECTORFONT_DOT_H

#ifndef WG_USERDEFINES_DOT_H
#	include <wg_userdefines.h>
#endif

#ifdef USE_FREETYPE

#ifndef WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

#ifndef WG_CHAIN_DOT_H
#	include <wg_chain.h>
#endif


#ifndef WG_FONT_DOT_H
#	include <wg_font.h>
#endif

#ifndef WG_SURFACEFACTORY_DOT_H
#	include <wg_surfacefactory.h>
#endif

struct	FT_FaceRec_;
typedef struct FT_FaceRec_*	FT_Face;
typedef struct  FT_Bitmap_ FT_Bitmap;

namespace wg 
{
	class VectorFont;
	typedef	StrongPtr<VectorFont,Font_p>		VectorFont_p;
	typedef	WeakPtr<VectorFont,Font_wp>	VectorFont_wp;

	class VectorFont : public Font
	{
	public:
		static VectorFont_p	create( char* pTTF_File, int bytes, int faceIndex ) { return VectorFont_p(new VectorFont(pTTF_File,bytes,faceIndex)); }

		bool		isInstanceOf( const char * pClassName ) const;
		const char *className( void ) const;
		static const char	CLASSNAME[];
		static VectorFont_p	cast( const Object_p& pObject );

		enum RenderMode
		{
			MONOCHROME,
			CRISP_EDGES,
			BEST_SHAPES
		};



		inline Type	getType() const { return VECTOR; }

		int			getKerning( Glyph_p pLeftGlyph, Glyph_p pRightGlyph, int size );
		Glyph_p	getGlyph( uint16_t chr, int size );
		bool		hasGlyph( uint16_t chr );

		int			getHeight( int size );
		int			getLineSpacing( int size );
		int			getBaseline( int size );
		int			getNbGlyphs();
		bool		hasGlyphs();
		bool		isMonospace();
		int			getWhitespaceAdvance( int size );
		int			getMaxGlyphAdvance( int size );

		inline void	setSizeOffset( int offset ) { m_sizeOffset = offset; }
		inline int	getSizeOffset() const { return m_sizeOffset; }
		inline bool setRenderMode( RenderMode mode ) { return setRenderMode( mode, 0, 0xFFFF ); }
		inline bool setRenderMode( RenderMode mode, int size ) { return setRenderMode(mode,size,size); }
		bool		setRenderMode( RenderMode mode, int startSize, int endSize );
		inline RenderMode	getRenderMode( int size ) const { if( size >= 0 && size <= WG_MAX_FONTSIZE ) return m_renderMode[size]; else return MONOCHROME; }

		static void	setSurfaceFactory( const SurfaceFactory_p& pFactory );
		static void	clearCache();

	private:
		VectorFont( char* pTTF_File, int bytes, int faceIndex );
		~VectorFont();

		const static int	MIN_GLYPH_PIXEL_SIZE = 12;		
		const static int	MAX_GLYPH_PIXEL_SIZE = WG_MAX_FONTSIZE*2;
		const static int	GLYPH_PIXEL_SIZE_QUANTIZATION = 4;
		const static int	GLYPH_SLOT_SIZES = ((MAX_GLYPH_PIXEL_SIZE-MIN_GLYPH_PIXEL_SIZE)/GLYPH_PIXEL_SIZE_QUANTIZATION)+1;

		class CacheSlot;

		class MyGlyph : public Glyph
		{
		public:
			MyGlyph();
			MyGlyph( uint16_t character, uint16_t size, int advance, uint32_t kerningIndex, Font * pFont );
			~MyGlyph();
			const GlyphBitmap * getBitmap();

			void	slotLost() { m_pSlot = 0; }
			bool	isInitialized() { return m_pFont?true:false; }

			CacheSlot * m_pSlot;
			uint16_t		m_size;			// size of character in points.
			uint16_t		m_character;	// Unicode for character.
		};

		class CacheSurf : public Link
		{
		public:
			CacheSurf( const Surface_p& _pSurf ) { pSurf = _pSurf; access = 0; }
			~CacheSurf();

			LINK_METHODS( CacheSurf );		

			uint32_t			access;			// Timestamp of last access.
			Surface_p	pSurf;
		};


		class CacheSlot : public Link
		{
		public:
			CacheSlot( CacheSurf * _pSurf, const Rect& _rect ) { access = 0; pSurf = _pSurf; rect = _rect; bitmap.pSurface = pSurf->pSurf, pGlyph = 0; }

			LINK_METHODS( CacheSlot );		

			uint32_t			access;			// Timestamp of last access.

			GlyphBitmap	bitmap;
			MyGlyph *			pGlyph;

			CacheSurf *		pSurf;
			Rect			rect;				// Rect for the slot - not the glyph itself as in GlyphBitmap which might be smaller.
		};


		void				_copyA8ToRGBA8( const uint8_t * pSrc, int src_width, int src_height, int src_pitch, uint8_t * pDest, int dest_width, int dest_height, int dest_pitch );
		void				_copyA1ToRGBA8( const uint8_t * pSrc, int src_width, int src_height, int src_pitch, uint8_t * pDest, int dest_width, int dest_height, int dest_pitch );


		bool				_setCharSize( int size );

		CacheSlot *			_generateBitmap( MyGlyph * pGlyph );
		void				_copyBitmap( FT_Bitmap * pBitmap, CacheSlot * pSlot );

		MyGlyph *			_addGlyph( uint16_t ch, int size, int advance, uint32_t kerningIndex );
		inline MyGlyph *	_findGlyph( uint16_t glyph, int size ) const;

		inline void			_touchSlot( CacheSlot * pSlot );


		FT_Face				m_ftFace;
		char*				m_pData;
		int					m_ftCharSize;
		MyGlyph **			m_cachedGlyphsIndex[WG_MAX_FONTSIZE+1];
		uint32_t			m_accessCounter;
		int					m_renderFlags;
		RenderMode			m_renderMode[WG_MAX_FONTSIZE+1];
		int					m_sizeOffset;								// value to add to specified size (for getGlyph(), getKerning() etc) before getting glyph data.
		int					m_whitespaceAdvance[WG_MAX_FONTSIZE+1];

		//____ Static stuff __________________________________________________________


		static CacheSlot *	getCacheSlot( int width, int height );
		static int			addCacheSlots( Chain<CacheSlot> * pChain, const Size& slotSize, int minSlots );
		static int			maxSlotsInSurface( const Size& surf, const Size& slot );
		static Size			calcTextureSize( const Size& slotSize, int nSlots );


		static Chain<CacheSlot>	s_cacheSlots[GLYPH_SLOT_SIZES];
		static Chain<CacheSurf>	s_cacheSurfaces;
		static SurfaceFactory_p	s_pSurfaceFactory;

		//____



	};

	//____ _findGlyphInIndex() _______________________________________________________

	VectorFont::MyGlyph * VectorFont::_findGlyph( uint16_t ch, int size ) const
	{
		if( m_cachedGlyphsIndex[size] != 0 && m_cachedGlyphsIndex[size][ch>>8] != 0 && m_cachedGlyphsIndex[size][ch>>8][ch&0xFF].isInitialized() )
			return &m_cachedGlyphsIndex[size][ch>>8][ch&0xFF];

		return 0;
	}

	//____ _touchSlot() _________________________________________________________

	void VectorFont::_touchSlot( CacheSlot * pSlot )
	{
		pSlot->moveFirst();								// Move slot to the top

		pSlot->access = m_accessCounter;				// Increase access counter.
		pSlot->pSurf->access = m_accessCounter++;		// We don't sort the surfaces, probably faster to just compare access when
														// we need to destroy one?
	}

} // namespace wg

#endif //USE_FREETYPE
#endif //WG_VECTORFONT_DOT_H
