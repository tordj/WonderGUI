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

#include <wg_dummyfont.h>

namespace wg
{


	const TypeInfo DummyFont::TYPEINFO = { "DummyFont", &Font::TYPEINFO };

	//____ constructor ____________________________________________________________

	DummyFont::DummyFont() : m_glyph(this)
	{
	}


	//____ Destructor _____________________________________________________________

	DummyFont::~DummyFont()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& DummyFont::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ nbGlyphs() _________________________________________________________

	int DummyFont::nbGlyphs()
	{
		return 0;
	}

	//____ hasGlyphs() ________________________________________________________

	bool DummyFont::hasGlyphs()
	{
		return false;
	}

	//____ isMonospace() ______________________________________________________

	bool DummyFont::isMonospace()
	{
		return true;
	}

	//____ isMonochrome() _____________________________________________________

	bool DummyFont::isMonochrome()
	{
		return true;
	}

	//____ hasGlyph() _________________________________________________________

	bool DummyFont::hasGlyph( uint16_t chr )
	{
		return false;
	}

	//____ setSize() _________________________________________________________

	bool DummyFont::setSize( int size )
	{
		return false;
	}

	//____ size() _____________________________________________________________

	int DummyFont::size()
	{
		return 1;
	}

	//____ kerning() __________________________________________________________

	spx DummyFont::kerning( Glyph_p pLeftGlyph, Glyph_p pRightGlyph )
	{
		return 0;
	}

	//____ getGlyph() _________________________________________________________

	Glyph_p DummyFont::getGlyph( uint16_t chr )
	{
		return &m_glyph;
	}

	//____ lineGap() __________________________________________________________

	spx DummyFont::lineGap()
	{
		return 1;
	}

	//____ whitespaceAdvance() ________________________________________________

	spx DummyFont::whitespaceAdvance()
	{
		return 1;
	}

	//____ maxAdvance() _______________________________________________________

	spx DummyFont::maxAdvance()
	{
		return 1;
	}

	//____ maxAscend() ________________________________________________________

	spx DummyFont::maxAscend()
	{
		return 1;
	}

	//____ maxDescend() _______________________________________________________

	spx DummyFont::maxDescend()
	{
		return 0;
	}

	//____ DummyGlyph::Constructor ____________________________________________

	DummyFont::DummyGlyph::DummyGlyph( Font * pFont )
	: Glyph( 1, 0, pFont )
	{
			m_src.pSurface	= 0;
			m_src.bearingX	= 0;
			m_src.bearingY	= 0;
	}
}
