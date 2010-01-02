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

#ifndef	WG_GLYPHSET_DOT_H
#define	WG_GLYPHSET_DOT_H

#include <wg_geo.h>

class	WgSurface;

//____ WgGlyph ________________________________________________________________

struct	WgGlyph
{
	WgGlyph();
	WgGlyph(Sint32 _rectX, Sint32 _rectY, Sint32 _rectW, Sint32 _rectH, Uint8 _advance, Sint8 _bearingX, Sint8 _bearingY, Uint16 _kerningIndex, WgSurface* _pSurface);

	WgRect			rect;
	Uint8			advance;		// spacing to next glyph
	Sint8			bearingX;		// x offset when rendering the glyph (negated offset to glyph origo)
	Sint8			bearingY;		// y offset when rendering the glyph (negated offset to glyph origo)
	Uint16			kerningIndex;	// index into kerning table
	WgSurface *		pSurf;
};

//____ WgGlyphSet _____________________________________________________________

class WgGlyphSet
{
public:
	WgGlyphSet( WgSurface * pSurf, char * pGlyphSpec, bool binaryFile=false );
	~WgGlyphSet();

	void			insertGlyphs( WgSurface * pSurf, char* pGlyphSpec, bool binaryFile=false );
	void			copyGlyphs( WgGlyphSet* pOtherGlyphSet );

	void			setFallback( const WgGlyphSet * pFallbackGlyphSet );
	const WgGlyphSet * getFallback() const { return m_pFallback; }

	bool			setReplacementGlyph( Uint16 glyph = 0xFFFD );
	Uint16			getReplacementGlyph() const { return m_replacementGlyphChar; }

	inline Uint32	width( Uint16 _glyph ) const { return glyph(_glyph)->rect.w; }
	
	//inline Uint32 spacing( Uint16 _glyph ) const { return glyph(_glyph)->spacing; }

	inline Sint8	kerning( Uint16 leftGlyph, Uint16 rightGlyph ) const;
	inline Sint8	kerning( const WgGlyph* pLeftGlyph, const WgGlyph* pRightGlyph ) const;

	inline Uint8	advance( Uint16 _glyph ) const { return glyph( _glyph )->advance; }
	inline Uint8	advance( const WgGlyph* pGlyph ) const { return pGlyph->advance; }

	inline Sint8	bearingX( Uint16 _glyph ) const { return glyph( _glyph )->bearingX; }
	inline Sint8	bearingX( const WgGlyph* pGlyph ) const { return pGlyph->bearingX; }

	inline Sint8	bearingY( Uint16 _glyph ) const { return glyph( _glyph )->bearingY; }
	inline Sint8	bearingY( const WgGlyph* pGlyph ) const { return pGlyph->bearingY; }

	inline Uint32		height() const { return m_height; }
	inline bool			hasGlyphs() const { if( m_nGlyphs > 0 ) return true; return false; }
	inline bool			isMonospace() const { return m_bMonospace; }
	inline Uint32		nbGlyphs() const { return m_nGlyphs; }
	inline float		avgSpacing() const { return m_avgSpacing; }
	inline Uint32		whitespace() const { return m_spaceSpacing; }

	inline const WgRect&	underlineRect() const { return m_underlineRect; }
	inline Uint32			underlineStretchOfs() const { return m_underlineStretchBeg; }
	inline Uint32			underlineStretchLen() const { return m_underlineStretchLen; }
	inline Sint8			underlineBearingX() const { return m_underlineBearingX; }
	inline Sint8			underlineBearingY() const { return m_underlineBearingY; }

	inline const WgGlyph *	glyph( Uint32 chr ) const;		// Recurses through fallbacks.
	inline bool				hasGlyph( Uint16 chr ) const;	// Do not check fallbacks.

private:
	void		DecideReplacementGlyph();

	bool		m_bMonospace;
	float		m_avgSpacing;
	int			m_maxSpacing;
	Uint32		m_spaceSpacing;
	Uint32		m_nGlyphs;
	Uint32		m_height;
	WgGlyph * 	m_glyphTab[256];
	WgGlyph		m_replacementGlyph;
	Uint16		m_replacementGlyphChar;

	WgRect		m_underlineRect;
	Uint32		m_underlineStretchBeg;
	Uint32		m_underlineStretchLen;
	Sint8		m_underlineBearingX;
	Sint8		m_underlineBearingY;

	int			m_nKerningGlyphs;
	Sint8*		m_pKerningTable;

	const WgGlyphSet*	m_pFallback;
};


//____ WgGlyphSet::hasGlyph() _________________________________________________

inline bool WgGlyphSet::hasGlyph( Uint16 chr ) const
{
	WgGlyph * pGlyph = m_glyphTab[chr >> 8];

	if( pGlyph )
	{
		pGlyph += (chr & 0xFF);
		if( pGlyph->pSurf != 0 )
			return true;
	}

	return false;
}


//____ WgGlyphSet::glyph() _________________________________________________________

inline WgGlyph const *	WgGlyphSet::glyph( Uint32 chr ) const
{
	WgGlyph * pGlyph = m_glyphTab[chr >> 8];

	if( pGlyph )
	{
		pGlyph += (chr & 0xFF);
		if( pGlyph->pSurf != 0 )
			return pGlyph;
	}

	for( const WgGlyphSet * pFallback = m_pFallback ; pFallback != 0 ; pFallback = pFallback->m_pFallback )
	{
		pGlyph = pFallback->m_glyphTab[chr >> 8];
		if( pGlyph )
		{
			pGlyph += (chr & 0xFF);
			if( pGlyph->pSurf != 0 )
				return pGlyph;
		}
	}

	return &m_replacementGlyph;
}


//____ WgGlyphSet::kerning() _________________________________________________________

inline Sint8 WgGlyphSet::kerning( Uint16 leftGlyph, Uint16 rightGlyph ) const
{
	if( leftGlyph == 0 || rightGlyph == 0 )
		return 0;
	
	if( !m_pKerningTable )
		return 0;

	int indexLeft = glyph( leftGlyph )->kerningIndex;
	int indexRight = glyph( rightGlyph )->kerningIndex;

	if( indexLeft >= m_nKerningGlyphs || indexRight >= m_nKerningGlyphs )
		return 0;

	return m_pKerningTable[ (indexLeft * m_nKerningGlyphs) + indexRight ];
}

inline Sint8 WgGlyphSet::kerning( const WgGlyph* pLeftGlyph, const WgGlyph* pRightGlyph ) const
{
	if( pLeftGlyph == 0 || pRightGlyph == 0 )
		return 0;
	
	if( !m_pKerningTable )
		return 0;

	int indexLeft = pLeftGlyph->kerningIndex;
	int indexRight = pRightGlyph->kerningIndex;

	if( indexLeft >= m_nKerningGlyphs || indexRight >= m_nKerningGlyphs )
		return 0;

	return m_pKerningTable[ (indexLeft * m_nKerningGlyphs) + indexRight ];
}


#endif // WG_GLYPHSET_DOT_H
