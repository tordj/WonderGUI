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

class WgBitmapGlyphs;
typedef	WgStrongPtr<WgBitmapGlyphs,WgGlyphsetPtr>		WgBitmapGlyphsPtr;
typedef	WgWeakPtr<WgBitmapGlyphs,WgGlyphsetWeakPtr>		WgBitmapGlyphsWeakPtr;

//____ WgBitmapGlyphs _____________________________________________________________

class WgBitmapGlyphs : public WgGlyphset
{
public:
	static WgBitmapGlyphsPtr	create( const WgSurfacePtr& pSurf, char * pGlyphSpec, bool binaryFile=false ) { return WgBitmapGlyphsPtr(new WgBitmapGlyphs(pSurf,pGlyphSpec,binaryFile)); }

	bool						isInstanceOf( const char * pClassName ) const;
	const char *				className( void ) const;
	static const char			CLASSNAME[];
	static WgBitmapGlyphsPtr	cast( const WgObjectPtr& pObject );


	inline Type	getType() const { return BITMAP; }

	void					insertGlyphs( const WgSurfacePtr& pSurf, char* pGlyphSpec, bool binaryFile=false );
	void					copyGlyphs( WgBitmapGlyphs* pOtherGlyphset );


	WgGlyphPtr				getGlyph( Uint16 chr, int size = 0 );						// Size is just a dummy for BitmapGlyphs...
	bool					hasGlyph( Uint16 chr );
	int						getKerning( WgGlyphPtr pLeftGlyph, WgGlyphPtr pRightGlyph, int size );

	inline int				getHeight( int size ) { return m_height; }
	inline int				getLineSpacing( int size ) { return m_height; }
	inline int				getBaseline( int size ) { return m_baseline; }
	inline int				getNbGlyphs() { return m_nGlyphs; }
	inline bool				hasGlyphs() { return m_nGlyphs?true:false; }
	inline bool				isMonospace() { return m_bMonospace; }
	inline int				getWhitespaceAdvance( int size ) { return m_spaceSpacing; }
	inline int				getMaxGlyphAdvance( int size ) { return m_maxSpacing; }

protected:
	WgBitmapGlyphs( const WgSurfacePtr& pSurf, char * pGlyphSpec, bool binaryFile=false );
	~WgBitmapGlyphs();

	class Glyph : public WgGlyph
	{
	public:
		Glyph();
		Glyph( int advance, Sint8 bearingX, Sint8 bearingY, Uint32 kerningIndex, WgGlyphset * pGlyphset, const WgSurfacePtr& pSurf, const WgRect& rect );

		const WgGlyphBitmap * getBitmap() { return &m_src; }

		void setAdvance( short advance ) { m_advance = advance; }

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
