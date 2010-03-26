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
#include <wg_cursorinstance.h>
#include <wg_gfxanim.h>
#include <wg_blockset.h>


WgPen::WgPen()
{ 
	Init();
}

WgPen::WgPen( WgGfxDevice * pDevice, const WgCord& origo, const WgRect& clip )
{
	Init();

	m_pDevice	= pDevice;
	m_origo		= origo;
	m_pos		= origo;
	SetClipRect( clip );
}

//____ Init() _________________________________________________________________

void WgPen::Init()
{
	m_pDevice = 0;

	m_pFont = 0; 
	m_pGlyphs = 0; 

	m_size = 0; 
	m_style = WG_STYLE_NORMAL; 

	m_pGlyph = &m_dummyGlyph; 
	m_pPrevGlyph = &m_dummyGlyph; 
	m_color = 0xFFFFFFFF, 

	m_dummyGlyph.advance = 0;
	m_dummyGlyph.bearingX = 0;
	m_dummyGlyph.bearingY = 0;
	m_dummyGlyph.kerningIndex = 0;
	m_dummyGlyph.pSurf = 0;

	m_tabWidth = 80; 

	m_bClip = false;
}

//____ SetClipRect() __________________________________________________________

void WgPen::SetClipRect( const WgRect& clip ) 
{ 
	m_clipRect = clip; 
	if( m_clipRect.x == 0 && m_clipRect.y == 0 && m_clipRect.w == 0 && m_clipRect.h == 0 )
		m_bClip = false;
	else
		m_bClip = true;
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
			x -= (x - m_origo.x) % m_tabWidth;
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


//____ BlitCursor() _______________________________________________________

bool WgPen::BlitCursor( const WgCursorInstance& instance ) const
{
	WgCursor * pCursor = m_pFont->GetCursor();
	if( pCursor == 0 )
		return false;

	WgGfxAnim * pAnim	= pCursor->anim( instance.mode() );
	if( pAnim == 0 )
		return false;

	WgGfxFrame * pFrame = pAnim->getFrame( instance.time(), 0 );

	float	scaleValue = (pCursor->sizeRatio() * GetLineSpacing())/pAnim->height();
	
	WgSize	size = WgSize( pAnim->width(), pAnim->height() );
	WgCord  bearing = pCursor->bearing( instance.mode() );

	int blockFlags = 0;

	switch( pCursor->scaleMode() )
	{
		case WgCursor::FIXED_SIZE:
			break;

		case WgCursor::TILE_VERTICALLY:
			blockFlags |= WG_TILE_ALL;
		case WgCursor::STRETCH_VERTICALLY:
			size.h		= (int) (size.h * scaleValue);
			bearing.y	= (int) (bearing.y * scaleValue);
			break;

		case WgCursor::TILE_SCALED:
			blockFlags |= WG_TILE_ALL;
		case WgCursor::STRETCH_SCALED:
			size		*= scaleValue;
			bearing		*= scaleValue;
			break;
	}

	WgBlock block( pFrame->pSurf, WgRect( pFrame->ofs.x, pFrame->ofs.y, pAnim->width(), pAnim->height()), * pCursor->stretchBorders(), 0, blockFlags );

	if( m_bClip )
		m_pDevice->ClipBlitBlock( m_clipRect, block, WgRect(m_pos + bearing, size) );
	else
		m_pDevice->BlitBlock( block, WgRect(m_pos + bearing, size) );

	return true;
}



//____ AdvancePosCursor() _____________________________________________________

void WgPen::AdvancePosCursor( const WgCursorInstance& instance )
{
	WgCursor * pCursor = m_pFont->GetCursor();
	if( !pCursor )
		return;

	WgGfxAnim * pAnim	= pCursor->anim( instance.mode() );
	if( !pAnim )
		return;

	int advance = pCursor->advance(instance.mode());

	if( pCursor->scaleMode() == WgCursor::TILE_SCALED || pCursor->scaleMode() == WgCursor::STRETCH_SCALED )
	{
		float	scaleValue = (pCursor->sizeRatio() * GetLineSpacing())/pAnim->height();
		advance = (int) (advance * scaleValue);
	}

	m_pos.x += advance;
}
