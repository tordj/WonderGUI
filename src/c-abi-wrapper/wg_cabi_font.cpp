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


#include <wg_cabi_font.h>
#include <wg_cabi_surface.h>
#include <wg_c_object.h>
#include <wg_c_font.h>

namespace wg
{

	const TypeInfo	CABIFont::TYPEINFO = { "CABIFont", &Font::TYPEINFO };


	//____ constructor ____________________________________________________________

	CABIFont::CABIFont( wg_obj cFont ) : Font(nullptr), m_cFont(cFont)
	{
		wg_retain(cFont);
	}

	//____ Destructor _____________________________________________________________

	CABIFont::~CABIFont()
	{
		wg_release(m_cFont);
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& CABIFont::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setSize() __________________________________________________________

	bool CABIFont::setSize( spx size )
	{
		return wg_setFontSize(m_cFont, size);
	}
	
	//____ size() _____________________________________________________________

	spx CABIFont::size()
	{
		return wg_fontSize(m_cFont);
	}

	//____ kerning() ___________________________________________________________

	spx CABIFont::kerning( Glyph& leftGlyph, Glyph& rightGlyph )
	{
		wg_glyph	left, right;

		left.advance = leftGlyph.advance;
		left.kerningIndex = leftGlyph.kerningIndex;
		left.fontRef = leftGlyph.fontRef;

		right.advance = rightGlyph.advance;
		right.kerningIndex = rightGlyph.kerningIndex;
		right.fontRef = rightGlyph.fontRef;

		return wg_getKerning(m_cFont, &left, &right);
	}

	//____ whitespaceAdvance() _________________________________________________

	spx CABIFont::whitespaceAdvance()
	{
		return wg_whitespaceAdvance(m_cFont);
	}

	//____ lineGap() ____________________________________________________________

	spx CABIFont::lineGap()
	{
		return wg_lineGap(m_cFont);
	}


	//____ maxAscend() ____________________________________________________________

	spx CABIFont::maxAscend()
	{
		return wg_maxAscend(m_cFont);
	}

	//____ maxDescend() ____________________________________________________________

	spx CABIFont::maxDescend()
	{
		return wg_maxDescend(m_cFont);
	}


	//____ nbGlyphs() __________________________________________________________

	int CABIFont::nbGlyphs()
	{
		return wg_nbGlyphs(m_cFont);
	}

	//____ hasGlyphs() ____________________________________________________________

	bool CABIFont::hasGlyphs()
	{
		return wg_hasGlyphs(m_cFont);
	}

	//____ isMonospace() __________________________________________________________

	bool CABIFont::isMonospace()
	{
		return wg_isMonospace(m_cFont);
	}

	//____ maxAdvance() ___________________________________________________

	spx CABIFont::maxAdvance()
	{
		return wg_maxAdvance(m_cFont);
	}


	//____ hasGlyph() _____________________________________________________________

	bool CABIFont::hasGlyph( uint16_t ch )
	{
		return wg_hasGlyph(m_cFont,ch);
	}

	//____ getGlyphWithoutBitmap() _____________________________________________________________

	void CABIFont::getGlyphWithoutBitmap(uint16_t ch, Glyph& glyph)
	{
		wg_glyph	cGlyph;

		wg_getGlyphWithoutBitmap(m_cFont, ch, &cGlyph);

		if (cGlyph.advance != 0)
		{
			glyph.advance		= cGlyph.advance;
			glyph.fontRef		= cGlyph.fontRef;
			glyph.kerningIndex	= cGlyph.kerningIndex;

		}
		else
		{
			glyph.advance = 0;
			glyph.fontRef = 0;
			glyph.kerningIndex = 0;
		}

		return;
	}

} // namespace wg
