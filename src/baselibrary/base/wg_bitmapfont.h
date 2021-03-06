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

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Rendering ______________________________________________________

		inline bool			setSize( MU size ) override { return (size == m_size);};						// SizeI is just a dummy for BitmapFont...
		inline MU			size() override { return m_size; }

		Glyph_p				getGlyph( uint16_t chr ) override;
		MU					kerning( Glyph_p pLeftGlyph, Glyph_p pRightGlyph ) override;

		inline MU			lineGap() override { return m_lineGap; }
		inline MU			whitespaceAdvance() override { return m_spaceAdvance; }
		inline MU			maxAdvance() override { return m_maxAdvance; }
		inline MU 			maxAscend() override { return m_maxAscend; }
		inline MU 			maxDescend() override { return m_maxDescend; }

		//.____ Misc ___________________________________________________________

		void				insertGlyphs( Surface * pSurf, char* pGlyphSpec );

		inline int			nbGlyphs() override { return m_nGlyphs; }
		inline bool			hasGlyphs() override { return m_nGlyphs?true:false; }
		bool				hasGlyph( uint16_t chr ) override;
		inline bool			isMonospace() override { return m_bMonospace; }
		inline bool			isMonochrome() override { return m_bMonochrome; }

	protected:
		BitmapFont( Surface * pSurf, char * pGlyphSpec );
		~BitmapFont();

		class MyGlyph : public Glyph
		{
		public:
			MyGlyph();
			MyGlyph( int advance, int16_t bearingX, int16_t bearingY, uint32_t kerningIndex, Font * pFont, Surface * pSurf, const RectI& rect );

			const GlyphBitmap * getBitmap() override { return &m_src; }

			void setAdvance( short advance ) { m_advance = advance; }

			GlyphBitmap	m_src;
		};


		MyGlyph * 	m_glyphTab[256];

		int			m_nKerningGlyphs;
		int16_t*	m_pKerningTable;

		int			m_nGlyphs;
		bool		m_bMonospace;
		bool		m_bMonochrome;
		int			m_lineGap;
		float		m_avgAdvance;
		int			m_spaceAdvance;
		int			m_maxAdvance;
		int			m_maxAscend;
		int			m_maxDescend;
		MU			m_size;						// Fontsize for this font.
	};



} // namespace wg
#endif // WG_BITMAPFONT_DOT_H
