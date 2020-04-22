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


#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <wg_font.h>
#include <wg_texttool.h>

namespace wg
{

	const TypeInfo Font::TYPEINFO = { "Font", &Object::TYPEINFO };

	//____ typeInfo() _________________________________________________________

	const TypeInfo& Font::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ isMonochrome() ______________________________________________________

	bool Font::isMonochrome()
	{
		return true;
	}

    //____ setBackupFont() ______________________________________________________

    bool Font::setBackupFont(Font * pFont)
    {
        //TODO: Error handling, make sure we don't get circular recursion.

        m_pBackupFont = pFont;
        return true;
    }

	//____ Glyph::Glyph() _______________________________________________________

	Glyph::Glyph()
	{
		m_pFont = 0;
		m_advance = 0;
		m_kerningIndex = 0;
	}

	//____ Glyph::Glyph() _______________________________________________________

	Glyph::Glyph( int advance, int kerningIndex, Font * pFont )
	{
		m_pFont = pFont;
		m_advance = advance;
		m_kerningIndex = kerningIndex;
	}



} // namespace wg
