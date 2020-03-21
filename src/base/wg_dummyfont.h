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

#ifndef	WG_DUMMYFONT_DOT_H
#define	WG_DUMMYFONT_DOT_H
#pragma once

#include <wg_geo.h>
#include <wg_font.h>



namespace wg
{
	class DummyFont;
	typedef	StrongPtr<DummyFont>	DummyFont_p;
	typedef	WeakPtr<DummyFont>		DummyFont_wp;

	class DummyFont : public Font
	{
	public:
		//.____ Creation __________________________________________

		static DummyFont_p	create() { return DummyFont_p(new DummyFont()); }

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Rendering ____________________________________________

		bool			setSize( int size ) override;
		int				size() override;

		int				kerning( Glyph_p pLeftGlyph, Glyph_p pRightGlyph ) override;
		Glyph_p			getGlyph( uint16_t chr ) override;

		int				lineGap() override;
		int				whitespaceAdvance() override;
		int				maxAdvance() override;
		int				maxAscend() override;
		int				maxDescend() override;

		//.____ Misc ___________________________________________________________

		int				nbGlyphs() override;
		bool			hasGlyphs() override;
		bool			isMonospace() override;
		bool			isMonochrome() override;
		bool			hasGlyph( uint16_t chr ) override;




	protected:
		DummyFont();
		~DummyFont();

		class DummyGlyph : public Glyph
		{
		public:
			DummyGlyph( Font * pFont );

			const GlyphBitmap * getBitmap() override { return &m_src; }
			GlyphBitmap	m_src;
		};

		DummyGlyph	m_glyph;

	};

}

#endif // WG_DUMMYFONT_DOT_H
