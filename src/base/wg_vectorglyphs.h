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


#ifndef WG_VECTORGLYPHS_DOT_H
#define WG_VECTORGLYPHS_DOT_H

#ifndef WG_USERDEFINES_DOT_H
#	include <wg_userdefines.h>
#endif

#ifdef WG_USE_FREETYPE

#ifndef WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

#ifndef WG_CHAIN_DOT_H
#	include <wg_chain.h>
#endif


#ifndef WG_GLYPHSET_DOT_H
#	include <wg_glyphset.h>
#endif

#ifndef WG_SURFACEFACTORY_DOT_H
#	include <wg_surfacefactory.h>
#endif

struct	FT_FaceRec_;
typedef struct FT_FaceRec_*	FT_Face;
typedef struct  FT_Bitmap_ FT_Bitmap;

namespace wg 
{
	class VectorGlyphs;
	typedef	WgStrongPtr<VectorGlyphs,Glyphset_p>		VectorGlyphs_p;
	typedef	WgWeakPtr<VectorGlyphs,Glyphset_wp>	VectorGlyphs_wp;

	class VectorGlyphs : public Glyphset
	{
	public:
		static VectorGlyphs_p	create( char* pTTF_File, int bytes, int faceIndex ) { return VectorGlyphs_p(new VectorGlyphs(pTTF_File,bytes,faceIndex)); }

		bool		isInstanceOf( const char * pClassName ) const;
		const char *className( void ) const;
		static const char	CLASSNAME[];
		static VectorGlyphs_p	cast( const Object_p& pObject );

		enum RenderMode
		{
			MONOCHROME,
			CRISP_EDGES,
			BEST_SHAPES
		};



		inline Type	getType() const { return VECTOR; }

		int			getKerning( Glyph_p pLeftGlyph, Glyph_p pRightGlyph, int size );
		Glyph_p	getGlyph( Uint16 chr, int size );
		bool		hasGlyph( Uint16 chr );

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
		VectorGlyphs( char* pTTF_File, int bytes, int faceIndex );
		~VectorGlyphs();

		const static int	MIN_GLYPH_PIXEL_SIZE = 12;		
		const static int	MAX_GLYPH_PIXEL_SIZE = WG_MAX_FONTSIZE*2;
		const static int	GLYPH_PIXEL_SIZE_QUANTIZATION = 4;
		const static int	GLYPH_SLOT_SIZES = ((MAX_GLYPH_PIXEL_SIZE-MIN_GLYPH_PIXEL_SIZE)/GLYPH_PIXEL_SIZE_QUANTIZATION)+1;

		class CacheSlot;

		class MyGlyph : public Glyph
		{
		public:
			MyGlyph();
			MyGlyph( Uint16 character, Uint16 size, int advance, Uint32 kerningIndex, Glyphset * pGlyphset );
			~MyGlyph();
			const GlyphBitmap * getBitmap();

			void	slotLost() { m_pSlot = 0; }
			bool	isInitialized() { return m_pGlyphset?true:false; }

			CacheSlot * m_pSlot;
			Uint16		m_size;			// size of character in points.
			Uint16		m_character;	// Unicode for character.
		};

		class CacheSurf : public Link
		{
		public:
			CacheSurf( const Surface_p& _pSurf ) { pSurf = _pSurf; access = 0; }
			~CacheSurf();

			LINK_METHODS( CacheSurf );		

			Uint32			access;			// Timestamp of last access.
			Surface_p	pSurf;
		};


		class CacheSlot : public Link
		{
		public:
			CacheSlot( CacheSurf * _pSurf, const Rect& _rect ) { access = 0; pSurf = _pSurf; rect = _rect; bitmap.pSurface = pSurf->pSurf, pGlyph = 0; }

			LINK_METHODS( CacheSlot );		

			Uint32			access;			// Timestamp of last access.

			GlyphBitmap	bitmap;
			MyGlyph *			pGlyph;

			CacheSurf *		pSurf;
			Rect			rect;				// Rect for the slot - not the glyph itself as in GlyphBitmap which might be smaller.
		};


		void				_copyA8ToRGBA8( const Uint8 * pSrc, int src_width, int src_height, int src_pitch, Uint8 * pDest, int dest_width, int dest_height, int dest_pitch );
		void				_copyA1ToRGBA8( const Uint8 * pSrc, int src_width, int src_height, int src_pitch, Uint8 * pDest, int dest_width, int dest_height, int dest_pitch );


		bool				_setCharSize( int size );

		CacheSlot *			_generateBitmap( MyGlyph * pGlyph );
		void				_copyBitmap( FT_Bitmap * pBitmap, CacheSlot * pSlot );

		MyGlyph *				_addGlyph( Uint16 ch, int size, int advance, Uint32 kerningIndex );
		inline MyGlyph *		_findGlyph( Uint16 glyph, int size ) const;

		inline void			_touchSlot( CacheSlot * pSlot );


		FT_Face				m_ftFace;
		char*				m_pData;
		int					m_ftCharSize;
		MyGlyph **			m_cachedGlyphsIndex[WG_MAX_FONTSIZE+1];
		Uint32				m_accessCounter;
		int					m_renderFlags;
		RenderMode			m_renderMode[WG_MAX_FONTSIZE+1];
		int					m_sizeOffset;								// value to add to specified size (for getGlyph(), getKerning() etc) before getting glyph data.
		int					m_whitespaceAdvance[WG_MAX_FONTSIZE+1];

		//____ Static stuff __________________________________________________________


		static CacheSlot *	getCacheSlot( int width, int height );
		static int			addCacheSlots( Chain<CacheSlot> * pChain, const Size& slotSize, int minSlots );
		static int			maxSlotsInSurface( const Size& surf, const Size& slot );
		static Size		calcTextureSize( const Size& slotSize, int nSlots );


		static Chain<CacheSlot>	s_cacheSlots[GLYPH_SLOT_SIZES];
		static Chain<CacheSurf>	s_cacheSurfaces;
		static SurfaceFactory_p	s_pSurfaceFactory;

		//____



	};

	//____ _findGlyphInIndex() _______________________________________________________

	VectorGlyphs::MyGlyph * VectorGlyphs::_findGlyph( Uint16 ch, int size ) const
	{
		if( m_cachedGlyphsIndex[size] != 0 && m_cachedGlyphsIndex[size][ch>>8] != 0 && m_cachedGlyphsIndex[size][ch>>8][ch&0xFF].isInitialized() )
			return &m_cachedGlyphsIndex[size][ch>>8][ch&0xFF];

		return 0;
	}

	//____ _touchSlot() _________________________________________________________

	void VectorGlyphs::_touchSlot( CacheSlot * pSlot )
	{
		pSlot->moveFirst();								// Move slot to the top

		pSlot->access = m_accessCounter;				// Increase access counter.
		pSlot->pSurf->access = m_accessCounter++;		// We don't sort the surfaces, probably faster to just compare access when
														// we need to destroy one?
	}

} // namespace wg

#endif //WG_USE_FREETYPE
#endif //WG_VECTORGLYPHS_DOT_H
