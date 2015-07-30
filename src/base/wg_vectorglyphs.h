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

class WgVectorGlyphs;
typedef	WgStrongPtr<WgVectorGlyphs,WgGlyphsetPtr>		WgVectorGlyphsPtr;
typedef	WgWeakPtr<WgVectorGlyphs,WgGlyphsetWeakPtr>	WgVectorGlyphsWeakPtr;

class WgVectorGlyphs : public WgGlyphset
{
public:
	static WgVectorGlyphsPtr	create( char* pTTF_File, int bytes, int faceIndex ) { return WgVectorGlyphsPtr(new WgVectorGlyphs(pTTF_File,bytes,faceIndex)); }

	bool		isInstanceOf( const char * pClassName ) const;
	const char *className( void ) const;
	static const char	CLASSNAME[];
	static WgVectorGlyphsPtr	cast( const WgObjectPtr& pObject );

	enum RenderMode
	{
		MONOCHROME,
		CRISP_EDGES,
		BEST_SHAPES
	};



	inline Type	getType() const { return VECTOR; }

	int			getKerning( WgGlyphPtr pLeftGlyph, WgGlyphPtr pRightGlyph, int size );
	WgGlyphPtr	getGlyph( Uint16 chr, int size );
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

	static void	setSurfaceFactory( const WgSurfaceFactoryPtr& pFactory );
	static void	clearCache();

private:
	WgVectorGlyphs( char* pTTF_File, int bytes, int faceIndex );
	~WgVectorGlyphs();

	const static int	MIN_GLYPH_PIXEL_SIZE = 12;		
	const static int	MAX_GLYPH_PIXEL_SIZE = WG_MAX_FONTSIZE*2;
	const static int	GLYPH_PIXEL_SIZE_QUANTIZATION = 4;
	const static int	GLYPH_SLOT_SIZES = ((MAX_GLYPH_PIXEL_SIZE-MIN_GLYPH_PIXEL_SIZE)/GLYPH_PIXEL_SIZE_QUANTIZATION)+1;

	class CacheSlot;

	class Glyph : public WgGlyph
	{
	public:
		Glyph();
		Glyph( Uint16 character, Uint16 size, int advance, Uint32 kerningIndex, WgGlyphset * pGlyphset );
		~Glyph();
		const WgGlyphBitmap * getBitmap();

		void	slotLost() { m_pSlot = 0; }
		bool	isInitialized() { return m_pGlyphset?true:false; }

		CacheSlot * m_pSlot;
		Uint16		m_size;			// size of character in points.
		Uint16		m_character;	// Unicode for character.
	};

	class CacheSurf : public WgLink
	{
	public:
		CacheSurf( const WgSurfacePtr& _pSurf ) { pSurf = _pSurf; access = 0; }
		~CacheSurf();

		LINK_METHODS( CacheSurf );		

		Uint32			access;			// Timestamp of last access.
		WgSurfacePtr	pSurf;
	};


	class CacheSlot : public WgLink
	{
	public:
		CacheSlot( CacheSurf * _pSurf, const WgRect& _rect ) { access = 0; pSurf = _pSurf; rect = _rect; bitmap.pSurface = pSurf->pSurf, pGlyph = 0; }

		LINK_METHODS( CacheSlot );		

		Uint32			access;			// Timestamp of last access.

		WgGlyphBitmap	bitmap;
		Glyph *			pGlyph;

		CacheSurf *		pSurf;
		WgRect			rect;				// Rect for the slot - not the glyph itself as in WgGlyphBitmap which might be smaller.
	};


	void				_copyA8ToRGBA8( const Uint8 * pSrc, int src_width, int src_height, int src_pitch, Uint8 * pDest, int dest_width, int dest_height, int dest_pitch );
	void				_copyA1ToRGBA8( const Uint8 * pSrc, int src_width, int src_height, int src_pitch, Uint8 * pDest, int dest_width, int dest_height, int dest_pitch );


	bool				_setCharSize( int size );

	CacheSlot *			_generateBitmap( Glyph * pGlyph );
	void				_copyBitmap( FT_Bitmap * pBitmap, CacheSlot * pSlot );

	Glyph *				_addGlyph( Uint16 ch, int size, int advance, Uint32 kerningIndex );
	inline Glyph *		_findGlyph( Uint16 glyph, int size ) const;

	inline void			_touchSlot( CacheSlot * pSlot );


	FT_Face				m_ftFace;
	char*				m_pData;
	int					m_ftCharSize;
	Glyph **			m_cachedGlyphsIndex[WG_MAX_FONTSIZE+1];
	Uint32				m_accessCounter;
	int					m_renderFlags;
	RenderMode			m_renderMode[WG_MAX_FONTSIZE+1];
	int					m_sizeOffset;								// value to add to specified size (for getGlyph(), getKerning() etc) before getting glyph data.
	int					m_whitespaceAdvance[WG_MAX_FONTSIZE+1];

	//____ Static stuff __________________________________________________________


	static CacheSlot *	getCacheSlot( int width, int height );
	static int			addCacheSlots( WgChain<CacheSlot> * pChain, const WgSize& slotSize, int minSlots );
	static int			maxSlotsInSurface( const WgSize& surf, const WgSize& slot );
	static WgSize		calcTextureSize( const WgSize& slotSize, int nSlots );


	static WgChain<CacheSlot>	s_cacheSlots[GLYPH_SLOT_SIZES];
	static WgChain<CacheSurf>	s_cacheSurfaces;
	static WgSurfaceFactoryPtr	s_pSurfaceFactory;

	//____



};

//____ _findGlyphInIndex() _______________________________________________________

WgVectorGlyphs::Glyph * WgVectorGlyphs::_findGlyph( Uint16 ch, int size ) const
{
	if( m_cachedGlyphsIndex[size] != 0 && m_cachedGlyphsIndex[size][ch>>8] != 0 && m_cachedGlyphsIndex[size][ch>>8][ch&0xFF].isInitialized() )
		return &m_cachedGlyphsIndex[size][ch>>8][ch&0xFF];

	return 0;
}

//____ _touchSlot() _________________________________________________________

void WgVectorGlyphs::_touchSlot( CacheSlot * pSlot )
{
	pSlot->moveFirst();								// Move slot to the top

	pSlot->access = m_accessCounter;				// Increase access counter.
	pSlot->pSurf->access = m_accessCounter++;		// We don't sort the surfaces, probably faster to just compare access when
													// we need to destroy one?
}

#endif //WG_USE_FREETYPE
#endif //WG_VECTORGLYPHS_DOT_H
