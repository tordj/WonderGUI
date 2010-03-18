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

#include <wg_glyphset.h>

class	WgSurface;


//____ WgBitmapGlyphs _____________________________________________________________

class WgBitmapGlyphs : public WgGlyphSet
{
public:
	WgBitmapGlyphs( WgSurface * pSurf, char * pGlyphSpec, bool binaryFile=false );
	~WgBitmapGlyphs();

	inline Type	GetType() const { return BITMAP; }

	void					insertGlyphs( WgSurface * pSurf, char* pGlyphSpec, bool binaryFile=false );
	void					copyGlyphs( WgBitmapGlyphs* pOtherGlyphSet );


	const WgGlyph *			GetGlyph( Uint16 chr, int size = 0 );						// Size is just a dummy for BitmapGlyphs...
	bool					HasGlyph( Uint16 chr );
	int						GetKerning( const WgGlyph* pLeftGlyph, const WgGlyph* pRightGlyph, int size );

	inline int				GetHeight( int size ) { return m_height; }
	inline int				GetLineSpacing( int size ) { return m_height; }
	inline int				GetBaseline( int size ) { return m_baseline; }
	inline int				GetNbGlyphs() { return m_nGlyphs; }
	inline bool				HasGlyphs() { return m_nGlyphs?true:false; }
	inline bool				IsMonospace() { return m_bMonospace; }
	inline int				GetWhitespaceAdvance( int size ) { return m_spaceSpacing; }
	inline int				GetMaxGlyphAdvance( int size ) { return m_maxSpacing; }



private:

	WgGlyph * 	m_glyphTab[256];

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
