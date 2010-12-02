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



class WgSurface;
class WgSurfaceFactory;

struct	FT_FaceRec_;
typedef struct FT_FaceRec_*	FT_Face;
typedef struct  FT_Bitmap_ FT_Bitmap;


class WgVectorGlyphs : public WgGlyphSet
{
public:

	enum RenderMode
	{
		MONOCHROME,
		CRISP_EDGES,
		BEST_SHAPES
	};


	WgVectorGlyphs( char* pTTF_File, int bytes, int faceIndex );
	~WgVectorGlyphs();

	inline Type	GetType() const { return VECTOR; }

	int			GetKerning( const WgGlyph* pLeftGlyph, const WgGlyph* pRightGlyph, int size );
	WgGlyph *	GetGlyph( Uint16 chr, int size );
	bool		HasGlyph( Uint16 chr );

	int			GetHeight( int size );
	int			GetLineSpacing( int size );
	int			GetBaseline( int size );
	int			GetNbGlyphs();
	bool		HasGlyphs();
	bool		IsMonospace();
	int			GetWhitespaceAdvance( int size );
	int			GetMaxGlyphAdvance( int size );

	inline void	SetSizeOffset( int offset ) { m_sizeOffset = offset; }
	inline int	GetSizeOffset() const { return m_sizeOffset; }
	inline bool SetRenderMode( RenderMode mode ) { return SetRenderMode( mode, 0, 0xFFFF ); }
	inline bool SetRenderMode( RenderMode mode, int size ) { return SetRenderMode(mode,size,size); }
	bool		SetRenderMode( RenderMode mode, int startSize, int endSize );
	inline RenderMode	GetRenderMode( int size ) const { if( size >= 0 && size <= WG_MAX_FONTSIZE ) return m_renderMode[size]; else return MONOCHROME; }

	static void	SetSurfaceFactory( WgSurfaceFactory * pFactory );
	static void	ClearCache();

private:
	const static int	MIN_GLYPH_PIXEL_SIZE = 12;		
	const static int	MAX_GLYPH_PIXEL_SIZE = WG_MAX_FONTSIZE*2;
	const static int	GLYPH_PIXEL_SIZE_QUANTIZATION = 4;
	const static int	GLYPH_SLOT_SIZES = ((MAX_GLYPH_PIXEL_SIZE-MIN_GLYPH_PIXEL_SIZE)/GLYPH_PIXEL_SIZE_QUANTIZATION)+1;

	class CacheSurf : public WgLink
	{
	public:
		CacheSurf( WgSurface * _pSurf ) { pSurf = _pSurf; access = 0; }
		~CacheSurf();

		LINK_METHODS( CacheSurf );		

		Uint32			access;			// Timestamp of last access.
		WgSurface *		pSurf;
	};


	class CacheSlot : public WgLink
	{
	public:
		CacheSlot( CacheSurf * _pSurf, const WgRect& _rect ) { access = 0; pSurf = _pSurf; rect = _rect; pOwner = 0; size = 0; character = 0; }

		LINK_METHODS( CacheSlot );		

		Uint32			access;			// Timestamp of last access.

		CacheSurf *		pSurf;
		WgRect			rect;

		WgGlyph			glyph;
		WgVectorGlyphs *pOwner;			// Font object that owns the character.
		Uint16			size;			// size of character in points.
		Uint16			character;		// Unicode for character.
	};


	void				CopyA8ToRGBA8( const Uint8 * pSrc, int src_width, int src_height, int src_pitch, Uint8 * pDest, int dest_width, int dest_height, int dest_pitch );
	void				CopyA1ToRGBA8( const Uint8 * pSrc, int src_width, int src_height, int src_pitch, Uint8 * pDest, int dest_width, int dest_height, int dest_pitch );


	bool				SetCharSize( int size );

	inline void			SlotLost( const CacheSlot * pSlot ) { RemoveSlotFromIndex( pSlot ); }	// Callback, called BEFORE content of slot is lost.
	void				CopyBitmap( FT_Bitmap * pBitmap, CacheSlot * pSlot );

	void				AddSlotToIndex( Uint16 ch, int size, CacheSlot * pSlot );
	void				RemoveSlotFromIndex( const CacheSlot * pSlot );
	inline CacheSlot *	FindSlotInIndex( Uint16 glyph, int size ) const;

	inline void			TouchSlot( CacheSlot * pSlot );


	FT_Face				m_ftFace;
	char*				m_pData;
	int					m_ftCharSize;
	CacheSlot ***		m_cachedGlyphsIndex[WG_MAX_FONTSIZE+1];
	Uint32				m_accessCounter;
	int					m_renderFlags;
	RenderMode			m_renderMode[WG_MAX_FONTSIZE+1];
	int					m_sizeOffset;								// value to add to specified size (for GetGlyph(), GetKerning() etc) before getting glyph data.
	int					m_whitespaceAdvance[WG_MAX_FONTSIZE+1];

	//____ Static stuff __________________________________________________________


	static CacheSlot *	GetCacheSlot( int width, int height );
	static int			AddCacheSlots( WgChain<CacheSlot> * pChain, const WgSize& slotSize, int minSlots );
	static int			MaxSlotsInSurface( const WgSize& surf, const WgSize& slot );
	static WgSize		CalcTextureSize( const WgSize& slotSize, int nSlots );


	static WgChain<CacheSlot>	s_cacheSlots[GLYPH_SLOT_SIZES];
	static WgChain<CacheSurf>	s_cacheSurfaces;
	static WgSurfaceFactory *	s_pSurfaceFactory;

	//____



};

//____ FindSlotInIndex() _______________________________________________________

WgVectorGlyphs::CacheSlot * WgVectorGlyphs::FindSlotInIndex( Uint16 ch, int size ) const
{
	if( m_cachedGlyphsIndex[size] != 0 && m_cachedGlyphsIndex[size][ch>>8] != 0 && m_cachedGlyphsIndex[size][ch>>8][ch&0xFF] )
		return m_cachedGlyphsIndex[size][ch>>8][ch&0xFF];

	return 0;
}

//____ TouchSlot() _________________________________________________________

void WgVectorGlyphs::TouchSlot( CacheSlot * pSlot )
{
	pSlot->MoveFirst();								// Move slot to the top

	pSlot->access = m_accessCounter;				// Increase access counter.
	pSlot->pSurf->access = m_accessCounter++;		// We don't sort the surfaces, probably faster to just compare access when
													// we need to destroy one?
}

#endif //WG_USE_FREETYPE
#endif //WG_VECTORGLYPHS_DOT_H
