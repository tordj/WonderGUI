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

	const char Font::CLASSNAME[] = {"Font"};

	//____ isInstanceOf() _________________________________________________________

	bool Font::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return Object::isInstanceOf(pClassName);
	}

	//____ className() ____________________________________________________________

	const char * Font::className( void ) const
	{
		return CLASSNAME;
	}

	//____ isMonochrome() ______________________________________________________

	bool Font::isMonochrome()
	{
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
