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

#include <wg_pen.h>
#include <wg_texttool.h>
#include <wg_font.h>


WgPen::WgPen()
{ 
	m_pDevice = 0;

	m_pFont = 0; 
	m_pGlyphs = 0; 

	m_size = 0; 
	m_style = WG_STYLE_NORMAL; 

	m_pGlyph = &m_dummyGlyph; 
	m_pPrevGlyph = &m_dummyGlyph; 
	m_color = 0xFFFFFFFF, 

	m_pos.x = 0;
	m_pos.y = 0;

	m_dummyGlyph.advance = 0;
	m_dummyGlyph.bearingX = 0;
	m_dummyGlyph.bearingY = 0;
	m_dummyGlyph.kerningIndex = 0;
	m_dummyGlyph.pSurf = 0;

	m_tabOrigo = 0; 
	m_tabWidth = 80; 
}

//____ SetTextProp() __________________________________________________________

bool WgPen::SetTextProp( Uint16 hTextProp, Uint16 hCharProp, WgMode mode )
{

	m_pFont	= WgTextTool::GetCombFont( hTextProp, hCharProp );
	if( !m_pFont )
		return false;

	m_color = WgTextTool::GetCombColor( hTextProp, hCharProp, mode );
	m_size	= WgTextTool::GetCombSize( hTextProp, hCharProp, mode );
	m_style	= WgTextTool::GetCombStyle( hTextProp, hCharProp, mode );

	m_bShowSpace = WgTextTool::GetCombCharVisibility( ' ', hTextProp, hCharProp );
	m_bShowCRLF = WgTextTool::GetCombCharVisibility( '\n', hTextProp, hCharProp );

	m_pGlyph = &m_dummyGlyph;
	m_pPrevGlyph = &m_dummyGlyph;


	m_pGlyphs = m_pFont->GetGlyphSet( m_style, m_size );
	if( !m_pGlyphs )
		return false;

	return true;
}


//____ SetChar() _____________________________________________________________

bool WgPen::SetChar( Uint32 chr )
{
	m_pPrevGlyph = m_pGlyph;

	// Special handling of some characters

	if( chr <= 32 )
	{
		if( chr == ' ' && !m_bShowSpace )
		{
			m_dummyGlyph.advance = m_pGlyphs->GetWhitespaceAdvance( m_size );
			m_pGlyph = &m_dummyGlyph;
			return false;
		}
		
		if( chr == '\n' && !m_bShowCRLF )
		{
			m_dummyGlyph.advance = 0;
			m_pGlyph = &m_dummyGlyph;
			return false;		
		}

		if( chr == 0 )
		{
			m_dummyGlyph.advance = 0;
			m_pGlyph = &m_dummyGlyph;
			return false;		
		}

		if( chr == '\t' )
		{
			int x = m_pos.x;
			x += m_tabWidth;
			x -= (x - m_tabOrigo) % m_tabWidth;
			m_dummyGlyph.advance = x;
			return false;
		}
	}

	if( chr == WG_BREAK_PERMITTED || chr == WG_HYPHEN_BREAK_PERMITTED )
	{
		m_dummyGlyph.advance = 0;
		m_pGlyph = &m_dummyGlyph;
		return false;		
	}

	// Get the glyph from our GlyphSet.

	const WgGlyph * p = m_pGlyphs->GetGlyph( chr, m_size );
	if( p )
	{
		m_pGlyph = p;
		return true;
	}
	// If not in glyphset we get the closest match in size/style from Font.

	m_pPrevGlyph = &m_dummyGlyph;		// We can't do kerning against a glyph from a different GlyphSet.

	p = m_pFont->GetGlyph( chr, m_style, m_size );
	if( p )
	{
		m_pGlyph = p;
		return true;
	}

	// The glyph doesn't exist in this font, try to get the unicode
	// WHITE_BOX as a replacement glyph.

	p = m_pFont->GetGlyph( 0xFFFD, m_style, m_size );
	if( p )
	{
		m_pGlyph = p;
		return true;
	}

	// We don't have the white box, try with the most suitable
	// common ascii character instead...

	p = m_pFont->GetGlyph( '*', m_style, m_size );
	if( p )
	{
		m_pGlyph = p;
		return true;
	}

	// Total failure, nothing to render...

	m_pGlyph = &m_dummyGlyph;
	m_dummyGlyph.advance = 0;
	return false;
}



