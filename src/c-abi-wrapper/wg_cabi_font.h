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


#ifndef WG_CABIFONT_DOT_H
#define WG_CABIFONT_DOT_H
#pragma once


#include <wg_c_types.h>
#include <wg_font.h>


namespace wg
{
	class	CABIFont;
	typedef	StrongPtr<CABIFont>		CABIFont_p;
	typedef	WeakPtr<CABIFont>	CABIFont_wp;

	class CABIFont : public Font
	{
	public:


		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Rendering ______________________________________________________

		bool		setSize( spx size ) override;
		spx			size() override;

		void		getGlyphWithoutBitmap(uint16_t chr, Glyph& glyph) override;
		spx			kerning(Glyph& leftGlyph, Glyph& rightGlyph) override;

		spx			lineGap() override;
		spx			whitespaceAdvance() override;
		spx			maxAdvance() override;
		spx			maxAscend() override;
		spx			maxDescend() override;

		//.____ Misc ___________________________________________________________

		int			nbGlyphs() override;
		bool		hasGlyphs() override;
		bool		hasGlyph( uint16_t chr ) override;
		bool		isMonospace() override;

		inline wg_obj	cObject() { return m_cFont; }


	protected:
		CABIFont( wg_obj cFont );
		~CABIFont();

		wg_obj		m_cFont;
	};



} // namespace wg

#endif //WG_CABIFONT_DOT_H
