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

#ifndef	WG_BITMAPGLYPHS_DOT_H
#define	WG_BITMAPGLYPHS_DOT_H

#ifndef WG_GEO_DOT_H
#	include <wg_geo.h>
#endif

#ifndef WG_GLYPHSET_DOT_H
#	include <wg_glyphset.h>
#endif

#ifndef WG_SURFACE_DOT_H
#	include <wg_surface.h>
#endif

//____ WgBitmapGlyphs _____________________________________________________________

class WgBitmapGlyphs : public WgGlyphset
{
public:
	WgBitmapGlyphs( const WgSurfacePtr& pSurf, char * pGlyphSpec, bool binaryFile=false );
	~WgBitmapGlyphs();

	inline Type	GetType() const { return BITMAP; }

	void					InsertGlyphs( const WgSurfacePtr& pSurf, char* pGlyphSpec, bool binaryFile=false );
	void					CopyGlyphs( WgBitmapGlyphs* pOtherGlyphset );


	WgGlyphPtr				GetGlyph( Uint16 chr, int size = 0 );						// Size is just a dummy for BitmapGlyphs...
	bool					HasGlyph( Uint16 chr );
	int						GetKerning( WgGlyphPtr pLeftGlyph, WgGlyphPtr pRightGlyph, int size );

	inline int				GetHeight( int size ) { return m_height; }
	inline int				GetLineSpacing( int size ) { return m_height; }
	inline int				GetBaseline( int size ) { return m_baseline; }
	inline int				GetNbGlyphs() { return m_nGlyphs; }
	inline bool				HasGlyphs() { return m_nGlyphs?true:false; }
	inline bool				IsMonospace() { return m_bMonospace; }
	inline int				GetWhitespaceAdvance( int size ) { return m_spaceSpacing; }
	inline int				GetMaxGlyphAdvance( int size ) { return m_maxSpacing; }

private:

	class Glyph : public WgGlyph
	{
	public:
		Glyph();
		Glyph( int advance, Sint8 bearingX, Sint8 bearingY, Uint32 kerningIndex, WgGlyphset * pGlyphset, const WgSurfacePtr& pSurf, const WgRect& rect );

		const WgGlyphBitmap * GetBitmap() { return &m_src; }

		void SetAdvance( short advance ) { m_advance = advance; }

		WgGlyphBitmap	m_src;
	};


	Glyph * 	m_glyphTab[256];

	int			m_nKerningGlyphs;
	Sint8*		m_pKerningTable;

	bool		m_bMonospace;
	float		m_avgSpacing;
	int			m_maxSpacing;
	Uint32		m_spaceSpacing;
	Uint32		m_nGlyphs;
	Uint32		m_height;
	int			m_baseline;
};


#endif // WG_BITMAPGLYPHS_DOT_H
