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

class	WgSurface;

//____ WgGlyph ________________________________________________________________

struct	WgGlyph
{
	WgGlyph();
	WgGlyph(Sint32 _rectX, Sint32 _rectY, Sint32 _rectW, Sint32 _rectH, Uint8 _advance, Sint8 _bearingX, Sint8 _bearingY, Uint16 _kerningIndex, WgSurface* _pSurface);

	WgSurface *		pSurf;
	WgRect			rect;
	Uint8			advance;		// spacing to next glyph
	Sint8			bearingX;		// x offset when rendering the glyph (negated offset to glyph origo)
	Sint8			bearingY;		// y offset when rendering the glyph (negated offset to glyph origo)
	Uint32			kerningIndex;	// index into kerning table (WgBitmapGlyphs) or glyph_index (WgVectorGlyphs)
};

//____ WgUnderline ____________________________________________________________

struct WgUnderline
{
	WgUnderline() { pSurf = 0; rect = WgRect(0,0,0,0); bearingX = 0; bearingY = 0; leftBorder = 0; rightBorder = 0; }

	WgSurface * pSurf;
	WgRect		rect;
	Sint8		bearingX;
	Sint8		bearingY;
	Uint8		leftBorder;
	Uint8		rightBorder;
};

//____ WgGlyphSet _____________________________________________________________

class WgGlyphSet
{
public:
	WgGlyphSet() {}
	virtual ~WgGlyphSet() {}

	enum Type
	{
		VECTOR = 0,
		BITMAP,
	};


	virtual	Type			GetType() const = 0;

	virtual int				GetKerning( const WgGlyph* pLeftGlyph, const WgGlyph* pRightGlyph, int size ) = 0;
	virtual const WgGlyph *	GetGlyph( Uint16 chr, int size ) = 0;
	virtual bool			HasGlyph( Uint16 chr ) = 0;

	virtual int				GetHeight( int size ) = 0;
	virtual int				GetLineSpacing( int size ) = 0;
	virtual int				GetBaseline( int size ) = 0;	// Offset in pixels to baseline.
	virtual int				GetNbGlyphs() = 0;
	virtual bool			HasGlyphs() = 0;
	virtual bool			IsMonospace() = 0;
	virtual int				GetWhitespaceAdvance( int size ) = 0;
	virtual int				GetMaxGlyphAdvance( int size ) = 0;


protected:

};




#endif // WG_GLYPHSET_DOT_H
