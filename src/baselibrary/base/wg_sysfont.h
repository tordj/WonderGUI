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

#ifndef	WG_SYSFONT_DOT_H
#define	WG_SYSFONT_DOT_H
#pragma once

#include <wg_geo.h>
#include <wg_font.h>
#include <wg_surface.h>

namespace wg
{

	class SysFont;
	typedef	StrongPtr<SysFont>	SysFont_p;
	typedef	WeakPtr<SysFont>	SysFont_wp;

	//____ SysFont _____________________________________________________________

	class SysFont : public Font
	{
	public:
		//.____ Creation __________________________________________

		static SysFont_p	create(Surface* pSurf, char* pGlyphSpec) { return SysFont_p(new SysFont(pSurf, pGlyphSpec)); }

		//.____ Identification __________________________________________

		const TypeInfo& typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Rendering ______________________________________________________

		inline bool			setSize(spx size) override { return (size == 16); };						// SizeI is just a dummy for SysFont...
		inline spx			size() override { return 16; }

		Glyph_p				getGlyph(uint16_t chr) override;
		spx					kerning(Glyph_p pLeftGlyph, Glyph_p pRightGlyph) override { return 0; }

		inline spx			lineGap() override { return 2*64; }
		inline spx			whitespaceAdvance() override { return 14*64; }
		inline spx			maxAdvance() override { return 14*64; }
		inline spx 			maxAscend() override { return 12*64; }
		inline spx 			maxDescend() override { return 2*64; }

		//.____ Misc ___________________________________________________________


		inline int			nbGlyphs() override { return 32*3; }
		inline bool			hasGlyphs() override { return true; }
		bool				hasGlyph(uint16_t chr) override;
		inline bool			isMonospace() override { return true; }
		inline bool			isMonochrome() override { return true; }

	protected:
		SysFont(Surface* pSurf, char* pGlyphSpec);
		~SysFont();

		class MyGlyph : public Glyph
		{
		public:
			MyGlyph();
			MyGlyph(int advance, int16_t bearingX, int16_t bearingY, uint32_t kerningIndex, Font* pFont, Surface* pSurf, const RectSPX& rect);

			const GlyphBitmap* getBitmap() override { return &m_src; }

			void setAdvance(short advance) { m_advance = advance; }

			GlyphBitmap	m_src;
		};

	};



} // namespace wg
#endif // WG_SYSFONT_DOT_H
