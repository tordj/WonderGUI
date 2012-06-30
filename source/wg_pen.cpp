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
#include <wg_text.h>
#include <wg_font.h>
#include <wg_cursorinstance.h>
#include <wg_gfxanim.h>
#include <wg_blockset.h>
#include <wg_textmanager.h>
#include <wg_base.h>

WgPen::WgPen()
{
	_init();
}

WgPen::WgPen( WgGfxDevice * pDevice, const WgCoord& origo, const WgRect& clip )
{
	_init();

	m_pDevice	= pDevice;
	m_origo		= origo;
	m_pos		= origo;
	SetClipRect( clip );
}

//____ _init() _________________________________________________________________

void WgPen::_init()
{
	m_pDevice = 0;
	m_pTextNode = 0;

	m_pFont = 0;
	m_pGlyphs = 0;

	m_size = 0;
	m_wantedSize = 0;
	m_style = WG_STYLE_NORMAL;

	m_pGlyph = &m_dummyGlyph;
	m_pPrevGlyph = &m_dummyGlyph;
	m_color = 0xFFFFFFFF,

	m_bShowSpace = true;
	m_bShowCRLF = true;

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



//____ _onAttrChanged() __________________________________________________________

void WgPen::_onAttrChanged()
{
	if( !m_pFont )
	{
		m_pGlyphs = 0;
		return;
	}

	if( m_pTextNode )
		m_size = (int) m_pTextNode->GetSize( m_pFont, m_style, m_wantedSize );
	else
		m_size = m_wantedSize;

	m_pGlyphs = m_pFont->GetGlyphset( m_style, m_size );
}

//____ SetAttributes() ________________________________________________________

bool WgPen::SetAttributes( const WgTextAttr& attr )
{
	if( attr.size < 0 && attr.size > WG_MAX_FONTSIZE )
		return false;

	m_pFont			= attr.pFont;
	m_wantedSize	= attr.size;
	m_style			= attr.style;
	m_color			= attr.color;
	_onAttrChanged();
	return true;
}

//____ SetSize() ______________________________________________________________

bool WgPen::SetSize( int size )
{
	if( size < 0 && size > WG_MAX_FONTSIZE )
		return false;

	m_wantedSize = size;
	_onAttrChanged();
	return true;
}

//____ SetFont() ______________________________________________________________

void WgPen::SetFont( WgFont * pFont )
{
	m_pFont = pFont;
	_onAttrChanged();
}

//____ SetStyle() _____________________________________________________________

void WgPen::SetStyle( WgFontStyle style )
{
	m_style = style;
	_onAttrChanged();
}

//____ SetColor() _____________________________________________________________

void WgPen::SetColor( WgColor color )
{
	m_color = color;
//	_onAttrChanged();
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
			if(m_pGlyphs)
				m_dummyGlyph.SetAdvance( m_pGlyphs->GetWhitespaceAdvance( m_size ) );
			else
				m_dummyGlyph.SetAdvance(0);
			m_pGlyph = &m_dummyGlyph;
			return false;
		}

		if( chr == '\n' && !m_bShowCRLF )
		{
			m_dummyGlyph.SetAdvance(0);
			m_pGlyph = &m_dummyGlyph;
			return false;
		}

		if( chr == 0 )
		{
			m_dummyGlyph.SetAdvance(0);
			m_pGlyph = &m_dummyGlyph;
			return false;
		}

		if( chr == '\t' )
		{
			int newPos = m_origo.x + ((m_pos.x - m_origo.x + m_tabWidth) / m_tabWidth) * m_tabWidth;
			m_dummyGlyph.SetAdvance(newPos - m_pos.x);
			m_pGlyph = &m_dummyGlyph;
			return false;
		}
	}

	if( chr == WG_BREAK_PERMITTED || chr == WG_HYPHEN_BREAK_PERMITTED )
	{
		m_dummyGlyph.SetAdvance(0);
		m_pGlyph = &m_dummyGlyph;
		return false;
	}

	// Get the glyph from our Glyphset.

	if( !m_pGlyphs )
	{
		m_pGlyph = &m_dummyGlyph;
		return false;
	}

	// First we try to get the glyph from our Glyphset.

	WgGlyphPtr p = m_pGlyphs->GetGlyph( chr, m_size );
	if( !p )
	{
		// If not in glyphset we get the closest match in size/style from Font.

		p = m_pFont->GetGlyph( chr, m_style, m_size );
		if( !p )
		{
			// The glyph doesn't exist in this font, try to get the unicode
			// WHITE_BOX as a replacement glyph.

			p = m_pFont->GetGlyph( 0xFFFD, m_style, m_size );
			if( !p )
			{
				// We don't have the white box, try with the most suitable
				// common ascii character instead...

				p = m_pFont->GetGlyph( '*', m_style, m_size );
				if( !p )
				{
					// Total failure, nothing to render...

					m_pGlyph = &m_dummyGlyph;
					m_dummyGlyph.SetAdvance(0);
					return false;
				}
			}
		}
	}

	if( m_pPrevGlyph->Glyphset() != p->Glyphset() )
		m_pPrevGlyph = &m_dummyGlyph; // We can't do kerning between glyphs from different glyphsets.

	m_pGlyph = p;
	return true;
}

//____ BlitChar() _____________________________________________________________

void WgPen::BlitChar() const
{
	const WgGlyphBitmap * pSrc = m_pGlyph->GetBitmap();

	if( pSrc )
	{
		int x = m_pos.x + pSrc->bearingX;
		int y = m_pos.y + pSrc->bearingY;

		if( m_bClip )
			m_pDevice->ClipBlit( m_clipRect, pSrc->pSurface, pSrc->rect, x, y);
		else
			m_pDevice->Blit( pSrc->pSurface, pSrc->rect, x, y);
	}
}

//____ BlitCursor() ___________________________________________________________

bool WgPen::BlitCursor( const WgCursorInstance& instance ) const
{
	WgCursor * pCursor = WgTextTool::GetCursor(instance.m_pText);
	if( pCursor == 0 )
		return false;

	WgCursor::Mode mode = instance.cursorMode();

	WgGfxAnim * pAnim	= pCursor->Anim( mode );
	if( pAnim == 0 )
		return false;

	WgBlock	block =	pAnim->GetBlock( instance.time(), 0 );

	WgSize	size = block.Size();
	WgCoord  bearing = pCursor->Bearing( mode );

	float	scaleValue = (pCursor->SizeRatio(mode)*GetLineHeight())/ size.h;

	if( block.IsScaled() )
	{
		size		*= scaleValue;
		bearing		*= scaleValue;
		size.w += 1;				// So that scaled size won't be limited by width truncation.
	}
	else if( !block.IsFixedSize() )
	{
		size.h		= (int) (size.h * scaleValue);
		bearing.y	= (int) (bearing.y * scaleValue);
	}

	// Set tintcolor/blendmode. Save original modes so we can restore them.

	WgColor		tintColor;
	WgBlendMode blendMode;
	switch( pCursor->GetBlitMode() )
	{
		case WgCursor::NORMAL:
			break;
		case WgCursor::TINTED:
			tintColor = m_pDevice->GetTintColor();
			m_pDevice->SetTintColor( tintColor * instance.text()->getColor() );
			break;
		case WgCursor::INVERT_BG:
			blendMode = m_pDevice->GetBlendMode();
			m_pDevice->SetBlendMode(WG_BLENDMODE_INVERT);
			break;
	}

	//

	if( m_bClip )
		m_pDevice->ClipBlitBlock( m_clipRect, block, WgRect(m_pos + bearing, size) );
	else
		m_pDevice->BlitBlock( block, WgRect(m_pos + bearing, size) );

	// Restore tintcolor/blendmode.

	switch( pCursor->GetBlitMode() )
	{
		case WgCursor::NORMAL:
			break;
		case WgCursor::TINTED:
			m_pDevice->SetTintColor( tintColor );
			break;
		case WgCursor::INVERT_BG:
			m_pDevice->SetBlendMode( blendMode );
			break;
	}

	return true;
}



//____ AdvancePosCursor() _____________________________________________________

void WgPen::AdvancePosCursor( const WgCursorInstance& instance )
{
	WgCursor * pCursor = WgTextTool::GetCursor( instance.m_pText );
	if( pCursor == 0 )
		return;

	WgCursor::Mode mode = instance.cursorMode();

	WgGfxAnim * pAnim	= pCursor->Anim( mode );
	if( !pAnim )
		return;

	int advance = pCursor->Advance(mode);

	if( pAnim->BlockFlags() & WG_SCALE_CENTER )
	{
		float	scaleValue = (pCursor->SizeRatio(mode) * GetLineSpacing())/pAnim->Size().h;
		advance = (int) (advance * scaleValue);
	}

	m_pos.x += advance;
}

