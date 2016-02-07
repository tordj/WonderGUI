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

namespace wg 
{
	
	class	Glyphset;
	
	
	
	
	//____ GlyphBitmap _____________________________________________________________
	
	struct GlyphBitmap
	{
		Surface_p pSurface;
		Rect		rect;
		int8_t		bearingX;		// x offset when rendering the glyph (negated offset to glyph origo)
		int8_t		bearingY;		// y offset when rendering the glyph (negated offset to glyph origo)
	};
	
	
	//____ Glyph ________________________________________________________________
	
	class Glyph
	{
	friend class Font;
	
	public:
		virtual const GlyphBitmap * getBitmap() = 0;
	
		inline int		advance() { return m_advance; }
		inline int		kerningIndex() { return m_kerningIndex; }
		inline Glyphset *	glyphset() { return m_pGlyphset; }
	
	protected:
		Glyph();
		Glyph( int advance, int _kerningIndex, Glyphset * pGlyphset );
		virtual ~Glyph() {}
	
		Glyphset *	m_pGlyphset;	// glyphset that this glyph belongs to
		int				m_advance;		// spacing to next glyph
		int				m_kerningIndex;	// index into kerning table (BitmapGlyphs) or glyph_index (VectorGlyphs)
	};
	
	typedef Glyph*	Glyph_p;
	
	
	//____ Underline ____________________________________________________________
	
	struct Underline
	{
		Underline() { pSurf = 0; rect = Rect(0,0,0,0); bearingX = 0; bearingY = 0; leftBorder = 0; rightBorder = 0; }
	
		Surface_p pSurf;
		Rect		rect;
		int8_t		bearingX;
		int8_t		bearingY;
		uint8_t		leftBorder;
		uint8_t		rightBorder;
	};
	
	class Glyphset;
	typedef	StrongPtr<Glyphset,Object_p>		Glyphset_p;
	typedef	WeakPtr<Glyphset,Object_wp>	Glyphset_wp;
	
	//____ Glyphset _____________________________________________________________
	
	class Glyphset : public Object
	{
	public:
		bool					isInstanceOf( const char * pClassName ) const;
		const char *			className( void ) const;
		static const char		CLASSNAME[];
		static Glyphset_p	cast( const Object_p& pObject );
	
		enum Type
		{
			VECTOR = 0,
			BITMAP,
		};
	
	
		virtual	Type			getType() const = 0;
	
		virtual int				getKerning( Glyph_p pLeftGlyph, Glyph_p pRightGlyph, int size ) = 0;
		virtual Glyph_p		getGlyph( uint16_t chr, int size ) = 0;
		virtual bool			hasGlyph( uint16_t chr ) = 0;
	
		virtual int				getHeight( int size ) = 0;
		virtual int				getLineSpacing( int size ) = 0;
		virtual int				getBaseline( int size ) = 0;	// Offset in pixels to baseline.
		virtual int				getNbGlyphs() = 0;
		virtual bool			hasGlyphs() = 0;
		virtual bool			isMonospace() = 0;
		virtual int				getWhitespaceAdvance( int size ) = 0;
		virtual int				getMaxGlyphAdvance( int size ) = 0;
	
	
	protected:
		Glyphset() {}
		virtual ~Glyphset() {}
	};
	
	
	
	

} // namespace wg
#endif // WG_GLYPHSET_DOT_H
