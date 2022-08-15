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

#include <wg_cabi.h>
#include <wg_cabi_freetypefont.h>
#include <wg_cabi_surface.h>

namespace wg
{

	const TypeInfo	CABIFreeTypeFont::TYPEINFO = { "CABIFreeTypeFont", &CABIFont::TYPEINFO };


	//____ create() ___________________________________________________________

	CABIFreeTypeFont_p CABIFreeTypeFont::create(wg_obj cFreeTypeFont, CABIBitmapCache* pCache) 
	{ 
		return CABIFreeTypeFont_p(new CABIFreeTypeFont(cFreeTypeFont, pCache)); 
	}

	//____ constructor ____________________________________________________________

	CABIFreeTypeFont::CABIFreeTypeFont( wg_obj cFreeTypeFont, CABIBitmapCache * pCache ) : CABIFont(cFreeTypeFont)
	{
		m_pCache = pCache;
	}

	//____ destructor _____________________________________________________________

	CABIFreeTypeFont::~CABIFreeTypeFont()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& CABIFreeTypeFont::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ getGlyphWithBitmap() ______________________________________________

	void CABIFreeTypeFont::getGlyphWithBitmap(uint16_t ch, Glyph& glyph)
	{
		wg_glyph	cGlyph;

		CABI::font->getGlyphWithBitmap(m_cFont, ch, &cGlyph);

		if (cGlyph.advance != 0)
		{
			glyph.advance = cGlyph.advance;
			glyph.fontRef = cGlyph.fontRef;
			glyph.kerningIndex = cGlyph.kerningIndex;

			glyph.rect		= * (RectSPX*) &cGlyph.rect;
			glyph.bearingX	= cGlyph.bearingX;
			glyph.bearingY	= cGlyph.bearingY;

			glyph.pSurface = m_pCache->wrapperSurface(cGlyph.surface);
		}
		else
		{
			glyph.advance = 0;
			glyph.fontRef = 0;
			glyph.kerningIndex = 0;
			glyph.pSurface = nullptr;
		}

		return;
	}

} // namespace wg
