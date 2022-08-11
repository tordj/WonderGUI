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


#include <wg_capibitmapfont.h>
#include <wg_capisurface.h>
#include <wg_c_object.h>
#include <wg_c_bitmapfont.h>

namespace wg
{

	const TypeInfo	CAPIFont::TYPEINFO = { "CAPIBitmapFont", &CAPIFont::TYPEINFO };


	//____ constructor ____________________________________________________________

	CAPIBitmapFont::CAPIBitmapFont( wg_obj cBitmapFont ) : Font(nullptr), m_cFont(cFont)
	{
		wg_obj surface = wg_getBitmapFontSurface(cBitmapFont);
		
		// CAPIBitmapFont doesn't support backup fonts.
		assert( wg_getBackupFont(cBitmapFont) == 0 );
		
		m_pSurface = CAPISurface::create(surface);
	}

	//____ Destructor _____________________________________________________________

	CAPIBitmapFont::~CAPIBitmapFont()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& CAPIBitmapFont::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ getGlyphWithBitmap() ______________________________________________

	void CAPIBitmapFont::getGlyphWithBitmap(uint16_t ch, Glyph& glyph)
	{
		wg_glyph	cGlyph;

		wg_getGlyphWithBitmap(m_cFont, ch, &cGlyph);

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
