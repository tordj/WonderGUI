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

#ifndef	WG_FONT_DOT_H
#define	WG_FONT_DOT_H
#pragma once

#include <wg_geo.h>
#include <wg_pointers.h>
#include <wg_surface.h>

namespace wg 
{
	
	class	Font;
	
	
	
	
	//____ GlyphBitmap _____________________________________________________________
	
	struct GlyphBitmap
	{
		Surface_p 	pSurface;
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
		inline Font *	glyphset() { return m_pFont; }
	
	protected:
		Glyph();
		Glyph( int advance, int _kerningIndex, Font * pFont );
		virtual ~Glyph() {}
	
		Font *			m_pFont;		// glyphset that this glyph belongs to
		int				m_advance;		// spacing to next glyph
		int				m_kerningIndex;	// index into kerning table (BitmapFont) or glyph_index (VectorFont)
	};
	
	typedef Glyph*	Glyph_p;
	
	class Font;
	typedef	StrongPtr<Font,Object_p>	Font_p;
	typedef	WeakPtr<Font,Object_wp>		Font_wp;
	
	//____ Font _____________________________________________________________
	
	class Font : public Object
	{
	public:
		//.____ Identification __________________________________________

		bool					isInstanceOf( const char * pClassName ) const;
		const char *			className( void ) const;
		static const char		CLASSNAME[];
		static Font_p	cast( const Object_p& pObject );

		//.____ Rendering ____________________________________________
	
		virtual bool			setSize( int size ) = 0;
		virtual int				size() = 0;
		virtual int				kerning( Glyph_p pLeftGlyph, Glyph_p pRightGlyph ) = 0;
		virtual Glyph_p			getGlyph( uint16_t chr ) = 0;

		virtual int				lineGap() = 0;
		virtual int				whitespaceAdvance() = 0;
		virtual int				maxAdvance() = 0;
		virtual int				maxAscend() = 0;
		virtual int				maxDescend() = 0;

		//.____ Misc ___________________________________________________________
	
		virtual int				nbGlyphs() = 0;
		virtual bool			hasGlyphs() = 0;
		virtual bool			hasGlyph( uint16_t chr ) = 0;
		virtual bool			isMonospace() = 0;
		virtual bool			isMonochrome();




	
	
	protected:
		Font() {}
		virtual ~Font() {}
	};
	
	
	
	

} // namespace wg
#endif // WG_FONT_DOT_H
