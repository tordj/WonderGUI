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

#ifndef WG_GEO_DOT_H
#	include <wg_geo.h>
#endif

#ifndef WG_POINTERS_DOT_H
#	include <wg_pointers.h>
#endif

#ifndef WG_SURFACE_DOT_H
#	include <wg_surface.h>
#endif

class	WgGlyphset;




//____ WgGlyphBitmap _____________________________________________________________

struct WgGlyphBitmap
{
	WgSurface_p pSurface;
	WgRect		rect;
	Sint8		bearingX;		// x offset when rendering the glyph (negated offset to glyph origo)
	Sint8		bearingY;		// y offset when rendering the glyph (negated offset to glyph origo)
};


//____ WgGlyph ________________________________________________________________

class WgGlyph
{
friend class WgFont;

public:
	virtual const WgGlyphBitmap * getBitmap() = 0;

	inline int		advance() { return m_advance; }
	inline int		kerningIndex() { return m_kerningIndex; }
	inline WgGlyphset *	glyphset() { return m_pGlyphset; }

protected:
	WgGlyph();
	WgGlyph( int advance, int _kerningIndex, WgGlyphset * pGlyphset );
	virtual ~WgGlyph() {}

	WgGlyphset *	m_pGlyphset;	// glyphset that this glyph belongs to
	int				m_advance;		// spacing to next glyph
	int				m_kerningIndex;	// index into kerning table (WgBitmapGlyphs) or glyph_index (WgVectorGlyphs)
};

typedef WgGlyph*	WgGlyph_p;


//____ WgUnderline ____________________________________________________________

struct WgUnderline
{
	WgUnderline() { pSurf = 0; rect = WgRect(0,0,0,0); bearingX = 0; bearingY = 0; leftBorder = 0; rightBorder = 0; }

	WgSurface_p pSurf;
	WgRect		rect;
	Sint8		bearingX;
	Sint8		bearingY;
	Uint8		leftBorder;
	Uint8		rightBorder;
};

class WgGlyphset;
typedef	WgStrongPtr<WgGlyphset,WgObject_p>		WgGlyphset_p;
typedef	WgWeakPtr<WgGlyphset,WgObject_wp>	WgGlyphset_wp;

//____ WgGlyphset _____________________________________________________________

class WgGlyphset : public WgObject
{
public:
	bool					isInstanceOf( const char * pClassName ) const;
	const char *			className( void ) const;
	static const char		CLASSNAME[];
	static WgGlyphset_p	cast( const WgObject_p& pObject );

	enum Type
	{
		VECTOR = 0,
		BITMAP,
	};


	virtual	Type			getType() const = 0;

	virtual int				getKerning( WgGlyph_p pLeftGlyph, WgGlyph_p pRightGlyph, int size ) = 0;
	virtual WgGlyph_p		getGlyph( Uint16 chr, int size ) = 0;
	virtual bool			hasGlyph( Uint16 chr ) = 0;

	virtual int				getHeight( int size ) = 0;
	virtual int				getLineSpacing( int size ) = 0;
	virtual int				getBaseline( int size ) = 0;	// Offset in pixels to baseline.
	virtual int				getNbGlyphs() = 0;
	virtual bool			hasGlyphs() = 0;
	virtual bool			isMonospace() = 0;
	virtual int				getWhitespaceAdvance( int size ) = 0;
	virtual int				getMaxGlyphAdvance( int size ) = 0;


protected:
	WgGlyphset() {}
	virtual ~WgGlyphset() {}
};




#endif // WG_GLYPHSET_DOT_H
