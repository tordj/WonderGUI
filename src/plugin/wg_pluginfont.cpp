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


#include <wg_pluginbase.h>
#include <wg_pluginfont.h>
#include <wg_pluginsurface.h>

namespace wg
{

	const TypeInfo	PluginFont::TYPEINFO = { "PluginFont", &Font::TYPEINFO };


	//____ constructor ____________________________________________________________

	PluginFont::PluginFont( wg_obj cFont ) : Font(nullptr), m_cFont(cFont)
	{
		PluginBase::object->retain(cFont);
	}

	//____ Destructor _____________________________________________________________

	PluginFont::~PluginFont()
	{
		PluginBase::object->release(m_cFont);
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& PluginFont::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setSize() __________________________________________________________

	bool PluginFont::setSize( spx size )
	{
		return PluginBase::font->setFontSize(m_cFont, size);
	}
	
	//____ size() _____________________________________________________________

	spx PluginFont::size()
	{
		return PluginBase::font->fontSize(m_cFont);
	}

	//____ kerning() ___________________________________________________________

	spx PluginFont::kerning( Glyph& leftGlyph, Glyph& rightGlyph )
	{
		wg_glyph	left, right;

		left.advance = leftGlyph.advance;
		left.kerningIndex = leftGlyph.kerningIndex;
		left.fontRef = leftGlyph.fontRef;

		right.advance = rightGlyph.advance;
		right.kerningIndex = rightGlyph.kerningIndex;
		right.fontRef = rightGlyph.fontRef;

		return PluginBase::font->getKerning(m_cFont, &left, &right);
	}

	//____ whitespaceAdvance() _________________________________________________

	spx PluginFont::whitespaceAdvance()
	{
		return PluginBase::font->whitespaceAdvance(m_cFont);
	}

	//____ lineGap() ____________________________________________________________

	spx PluginFont::lineGap()
	{
		return PluginBase::font->lineGap(m_cFont);
	}


	//____ maxAscend() ____________________________________________________________

	spx PluginFont::maxAscend()
	{
		return PluginBase::font->maxAscend(m_cFont);
	}

	//____ maxDescend() ____________________________________________________________

	spx PluginFont::maxDescend()
	{
		return PluginBase::font->maxDescend(m_cFont);
	}


	//____ nbGlyphs() __________________________________________________________

	int PluginFont::nbGlyphs()
	{
		return PluginBase::font->nbGlyphs(m_cFont);
	}

	//____ hasGlyphs() ____________________________________________________________

	bool PluginFont::hasGlyphs()
	{
		return PluginBase::font->hasGlyphs(m_cFont);
	}

	//____ isMonospace() __________________________________________________________

	bool PluginFont::isMonospace()
	{
		return PluginBase::font->isMonospace(m_cFont);
	}

	//____ maxAdvance() ___________________________________________________

	spx PluginFont::maxAdvance()
	{
		return PluginBase::font->maxAdvance(m_cFont);
	}


	//____ hasGlyph() _____________________________________________________________

	bool PluginFont::hasGlyph( uint16_t ch )
	{
		return PluginBase::font->hasGlyph(m_cFont,ch);
	}

	//____ getGlyphWithoutBitmap() _____________________________________________________________

	void PluginFont::getGlyphWithoutBitmap(uint16_t ch, Glyph& glyph)
	{
		wg_glyph	cGlyph;

		PluginBase::font->getGlyphWithoutBitmap(m_cFont, ch, &cGlyph);

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
