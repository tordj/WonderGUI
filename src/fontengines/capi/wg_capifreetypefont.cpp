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


#include <wg_capifreetypefont.h>
#include <wg_capisurface.h>
#include <wg_c_object.h>
#include <wg_c_font.h>

namespace wg
{

	const TypeInfo	CAPIFreeTypeFont::TYPEINFO = { "CAPIFreeTypeFont", &CAPIFont::TYPEINFO };


	//____ constructor ____________________________________________________________

	CAPIFreeTypeFont::CAPIFreeTypeFont( wg_obj cFont ) : Font(nullptr), m_cFont(cFont)
	{
		wg_retain(cFont);
	}

	//____ Destructor _____________________________________________________________

	CAPIFreeTypeFont::~CAPIFreeTypeFont()
	{
		wg_release(m_cFont);
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& CAPIFreeTypeFont::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ getGlyphWithBitmap() ______________________________________________

	void CAPIFont::getGlyphWithBitmap(uint16_t ch, Glyph& glyph)
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

			
			int ofs = wg_getSurfaceIdentity(cGlyph.surface);
/*
			wg_obj baggage = wg_getSurfaceBaggage(cGlyph.surface);

			CAPISurface_p	pSurface;

			if (baggage == 0)
			{
				pSurface = CAPISurface::create(cGlyph.surface);
				wg_setSurfaceBaggage(cGlyph.surface, static_cast<Object*>(pSurface.rawPtr()));
			}
			else
				pSurface = 



			glyph.pSurface	= cGlyph->pSurface;
*/
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
