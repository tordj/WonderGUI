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
#include <wg_cabi_bitmapfont.h>
#include <wg_cabi_surface.h>

#include <cassert>

namespace wg
{

	const TypeInfo	CABIBitmapFont::TYPEINFO = { "CABIBitmapFont", &CABIFont::TYPEINFO };


	//____ constructor ____________________________________________________________

	CABIBitmapFont::CABIBitmapFont( wg_obj cBitmapFont ) : CABIFont(cBitmapFont)
	{
		wg_obj surface = CABI::bitmapFont->getBitmapFontSurface(cBitmapFont);
		
		// CABIBitmapFont doesn't support backup fonts.
		assert( CABI::font->getBackupFont(cBitmapFont) == 0 );
		
		m_pSurface = CABISurface::create(surface);
	}

	//____ Destructor _____________________________________________________________

	CABIBitmapFont::~CABIBitmapFont()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& CABIBitmapFont::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ getGlyphWithBitmap() ______________________________________________

	void CABIBitmapFont::getGlyphWithBitmap(uint16_t ch, Glyph& glyph)
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

			glyph.pSurface 	= m_pSurface;
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
