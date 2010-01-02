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

#include <wg_font.h>
#include <string.h>

WgFont * WgFont::g_pDefaultFont = 0;


//____ Constructor ____________________________________________________________

WgFont::WgFont()
{
	SetDefaultGlyphSet( 0 );

	m_pCursor = 0;
}


WgFont::WgFont( WgGlyphSet * _pNormal, WgCursor * _pCursor )
{
	SetDefaultGlyphSet( _pNormal );

	m_pCursor = _pCursor;
}


//____ Destructor _____________________________________________________________

WgFont::~WgFont()
{
}

//____ SetGlyphSet() __________________________________________________________

bool WgFont::SetGlyphSet( WgFontStyle _style, WgGlyphSet * _pGlyphs )
{
	m_aGlyphSets[_style] = _pGlyphs;

	return true;
}

bool WgFont::SetDefaultGlyphSet( WgGlyphSet * _pDefault )
{
	m_pDefault = _pDefault;
	for( int i = 0 ; i < WG_NB_FONTSTYLES ; i++ )
		m_aGlyphSets[i] = _pDefault;
	return true;
}

//____ SetCursor() ____________________________________________________________

bool WgFont::SetCursor( WgCursor * _pCursor )
{
	m_pCursor = _pCursor;
	return true;
}

