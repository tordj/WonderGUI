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

#ifndef	WG_BITMAPFONT_DOT_H
#define	WG_BITMAPFONT_DOT_H
#pragma once

#include <wg_geo.h>
#include <wg_font.h>
#include <wg_surface.h>

namespace wg 
{
	
	class BitmapFont;
	typedef	StrongPtr<BitmapFont>	BitmapFont_p;
	typedef	WeakPtr<BitmapFont>		BitmapFont_wp;
	
	//____ BitmapFont _____________________________________________________________
	
	class BitmapFont : public Font
	{
	public:
		//.____ Creation __________________________________________
		
		static BitmapFont_p	create( Surface * pSurf, char * pGlyphSpec ) { return BitmapFont_p(new BitmapFont(pSurf,pGlyphSpec)); }

		//.____ Identification __________________________________________
	
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static BitmapFont_p	cast( Object * pObject );
	
		//.____ Rendering ______________________________________________________

		inline bool			setSize( int size ) { return (size == m_size);};						// Size is just a dummy for BitmapFont...
		inline int			size() { return m_size; }
		
		Glyph_p				getGlyph( uint16_t chr );
		int					kerning( Glyph_p pLeftGlyph, Glyph_p pRightGlyph );

		inline int			lineGap() { return m_lineGap; }
		inline int			whitespaceAdvance() { return m_spaceAdvance; }
		inline int			maxAdvance() { return m_maxAdvance; }
		inline int 			maxAscend() { return m_maxAscend; }
		inline int 			maxDescend() { return m_maxDescend; }

		//.____ Misc ___________________________________________________________

		void				insertGlyphs( Surface * pSurf, char* pGlyphSpec );

		inline int			nbGlyphs() { return m_nGlyphs; }
		inline bool			hasGlyphs() { return m_nGlyphs?true:false; }
		bool				hasGlyph( uint16_t chr );
		inline bool			isMonospace() { return m_bMonospace; }
		inline bool			isMonochrome() { return m_bMonochrome; }
	
	protected:
		BitmapFont( Surface * pSurf, char * pGlyphSpec );
		~BitmapFont();
	
		class MyGlyph : public Glyph
		{
		public:
			MyGlyph();
			MyGlyph( int advance, int8_t bearingX, int8_t bearingY, uint32_t kerningIndex, Font * pFont, Surface * pSurf, const Rect& rect );
	
			const GlyphBitmap * getBitmap() { return &m_src; }
	
			void setAdvance( short advance ) { m_advance = advance; }
	
			GlyphBitmap	m_src;
		};
	
	
		MyGlyph * 	m_glyphTab[256];
	
		int			m_nKerningGlyphs;
		int8_t*		m_pKerningTable;
	
		int			m_nGlyphs;
		bool		m_bMonospace;
		bool		m_bMonochrome;
		int			m_lineGap;
		float		m_avgAdvance;
		int			m_spaceAdvance;
		int			m_maxAdvance;
		int			m_maxAscend;
		int			m_maxDescend;
		int			m_size;						// Fontsize for this font.
	};
	
	

} // namespace wg
#endif // WG_BITMAPFONT_DOT_H
