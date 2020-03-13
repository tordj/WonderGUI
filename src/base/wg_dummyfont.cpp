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


	const char DummyFont::CLASSNAME[] = {"DummyFont"};

	//____ Constructor ____________________________________________________________

	DummyFont::DummyFont() : m_glyph(this)
	{
	}


	//____ Destructor _____________________________________________________________

	DummyFont::~DummyFont()
	{
	}

	//____ isInstanceOf() _________________________________________________________

	bool DummyFont::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return Font::isInstanceOf(pClassName);
	}

	//____ className() ____________________________________________________________

	const char * DummyFont::className( void ) const
	{
		return CLASSNAME;
	}

	int DummyFont::nbGlyphs()
	{
		return 0;
	}

	bool DummyFont::hasGlyphs()
	{
		return false;
	}

	bool DummyFont::isMonospace()
	{
		return true;
	}

	bool DummyFont::isMonochrome()
	{
		return true;
	}

	bool DummyFont::hasGlyph( uint16_t chr )
	{
		return false;
	}

	bool DummyFont::setSize( int size )
	{
		return false;
	}

	int DummyFont::size()
	{
		return 1;
	}

	int DummyFont::kerning( Glyph_p pLeftGlyph, Glyph_p pRightGlyph )
	{
		return 0;
	}

	Glyph_p DummyFont::getGlyph( uint16_t chr )
	{
		return &m_glyph;
	}

	int DummyFont::lineGap()
	{
		return 1;
	}

	int DummyFont::whitespaceAdvance()
	{
		return 1;
	}

	int DummyFont::maxAdvance()
	{
		return 1;
	}

	int DummyFont::maxAscend()
	{
		return 1;
	}

	int DummyFont::maxDescend()
	{
		return 0;
	}

	DummyFont::DummyGlyph::DummyGlyph( Font * pFont )
	: Glyph( 1, 0, pFont )
	{
			m_src.pSurface	= 0;
			m_src.bearingX	= 0;
			m_src.bearingY	= 0;
	}
}
