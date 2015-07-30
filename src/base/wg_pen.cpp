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
#include <wg_legacytextfield.h>
#include <wg_font.h>
#include <wg_caretinstance.h>
#include <wg_gfxanim.h>
#include <wg_base.h>
#include <wg_widget.h>

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
	setClipRect( clip );
}

//____ _init() _________________________________________________________________

void WgPen::_init()
{
	m_pDevice = 0;

	m_pFont = 0;

	m_size = 0;
	m_wantedSize = 0;
	m_style = WG_FONT_NORMAL;

	m_pGlyph = &m_dummyGlyph;
	m_pPrevGlyph = &m_dummyGlyph;
	m_color = 0xFFFFFFFF,

	m_bShowSpace = false;
	m_bShowCRLF = false;

	m_tabWidth = 80;

	m_bClip = false;
}

//____ setClipRect() __________________________________________________________

void WgPen::setClipRect( const WgRect& clip )
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

	m_size = m_wantedSize;
	m_pGlyphs = m_pFont->getGlyphset( m_style, m_size );
}

//____ setAttributes() ________________________________________________________

bool WgPen::setAttributes( const WgTextAttr& attr )
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

//____ setSize() ______________________________________________________________

bool WgPen::setSize( int size )
{
	if( size < 0 && size > WG_MAX_FONTSIZE )
		return false;

	m_wantedSize = size;
	_onAttrChanged();
	return true;
}

//____ setFont() ______________________________________________________________

void WgPen::setFont( const WgFont_p& pFont )
{
	m_pFont = pFont;
	_onAttrChanged();
}

//____ setStyle() _____________________________________________________________

void WgPen::setStyle( WgFontAlt style )
{
	m_style = style;
	_onAttrChanged();
}

//____ setColor() _____________________________________________________________

void WgPen::setColor( WgColor color )
{
	m_color = color;
//	_onAttrChanged();
}


//____ setChar() _____________________________________________________________

bool WgPen::setChar( Uint32 chr )
{
	m_pPrevGlyph = m_pGlyph;

	// Special handling of some characters

	if( chr <= 32 )
	{
		if( chr == ' ' && !m_bShowSpace )
		{
			if(m_pGlyphs)
				m_dummyGlyph.setAdvance( m_pGlyphs->getWhitespaceAdvance( m_size ) );
			else
				m_dummyGlyph.setAdvance(0);
			m_pGlyph = &m_dummyGlyph;
			return false;
		}

		if( chr == '\n' && !m_bShowCRLF )
		{
			m_dummyGlyph.setAdvance(0);
			m_pGlyph = &m_dummyGlyph;
			return false;
		}

		if( chr == 0 )
		{
			m_dummyGlyph.setAdvance(0);
			m_pGlyph = &m_dummyGlyph;
			return false;
		}

		if( chr == '\t' )
		{
			int newPos = m_origo.x + ((m_pos.x - m_origo.x + m_tabWidth) / m_tabWidth) * m_tabWidth;
			m_dummyGlyph.setAdvance(newPos - m_pos.x);
			m_pGlyph = &m_dummyGlyph;
			return false;
		}
	}

	if( chr == WG_BREAK_PERMITTED || chr == WG_HYPHEN_BREAK_PERMITTED )
	{
		m_dummyGlyph.setAdvance(0);
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

	WgGlyph_p p = m_pGlyphs->getGlyph( chr, m_size );
	if( !p )
	{
		// If not in glyphset we get the closest match in size/style from Font.

		p = m_pFont->getGlyph( chr, m_style, m_size );
		if( !p )
		{
			// The glyph doesn't exist in this font, try to get the unicode
			// WHITE_BOX as a replacement glyph.

			p = m_pFont->getGlyph( 0xFFFD, m_style, m_size );
			if( !p )
			{
				// We don't have the white box, try with the most suitable
				// common ascii character instead...

				p = m_pFont->getGlyph( '*', m_style, m_size );
				if( !p )
				{
					// Total failure, nothing to render...

					m_pGlyph = &m_dummyGlyph;
					m_dummyGlyph.setAdvance(0);
					return false;
				}
			}
		}
	}

	if( m_pPrevGlyph->glyphset() != p->glyphset() )
		m_pPrevGlyph = &m_dummyGlyph; // We can't do kerning between glyphs from different glyphsets.

	m_pGlyph = p;
	return true;
}

//____ blitChar() _____________________________________________________________

void WgPen::blitChar() const
{
	const WgGlyphBitmap * pSrc = m_pGlyph->getBitmap();

	if( pSrc )
	{
		int x = m_pos.x + pSrc->bearingX;
		int y = m_pos.y + pSrc->bearingY;

		if( m_bClip )
			m_pDevice->clipBlit( m_clipRect, pSrc->pSurface, pSrc->rect, x, y);
		else
			m_pDevice->blit( pSrc->pSurface, pSrc->rect, x, y);
	}
}

//____ blitCursor() ___________________________________________________________

bool WgPen::blitCursor( const WgCaretInstance& instance ) const
{
	WgCaret_p pCursor = WgTextTool::getCursor(instance.m_pText);
	if( !pCursor )
		return false;

	WgCaret::Mode mode = instance.cursorMode();

	WgGfxAnim_p pAnim	= pCursor->anim( mode );
	if( !pAnim )
		return false;

	WgGfxFrame * pAnimFrame =	pAnim->getFrame( instance.time(), 0 );

	WgSize	size = pAnim->size();
	WgCoord  bearing = pCursor->bearing( mode );

	float	scaleValue = (pCursor->sizeRatio(mode)*getLineHeight())/ size.h;

	if( pCursor->scaleWidth(mode) )
	{
		size		*= scaleValue;
		bearing		*= scaleValue;
		size.w += 1;				// So that scaled size won't be limited by width truncation.
	}
	else
	{
		size.h		= (int) (size.h * scaleValue);
		bearing.y	= (int) (bearing.y * scaleValue);
	}

	// Set tintcolor/blendmode. Save original modes so we can restore them.

	WgColor		tintColor;
	WgBlendMode blendMode;
	switch( pCursor->getBlitMode() )
	{
		case WgCaret::NORMAL:
			break;
		case WgCaret::TINTED:
			tintColor = m_pDevice->getTintColor();
			m_pDevice->setTintColor( tintColor * instance.text()->properties()->color( WG_STATE_NORMAL ) );
			break;
		case WgCaret::INVERT_BG:
			blendMode = m_pDevice->getBlendMode();
			m_pDevice->setBlendMode(WG_BLENDMODE_INVERT);
			break;
	}

	//

	if( m_bClip )
		m_pDevice->clipStretchBlit( m_clipRect, pAnimFrame->pSurf, pAnimFrame->rect, WgRect(m_pos + bearing, size) );
	else
		m_pDevice->stretchBlit( pAnimFrame->pSurf, pAnimFrame->rect, WgRect(m_pos + bearing, size) );

	// Restore tintcolor/blendmode.

	switch( pCursor->getBlitMode() )
	{
		case WgCaret::NORMAL:
			break;
		case WgCaret::TINTED:
			m_pDevice->setTintColor( tintColor );
			break;
		case WgCaret::INVERT_BG:
			m_pDevice->setBlendMode( blendMode );
			break;
	}

	return true;
}



//____ advancePosCursor() _____________________________________________________

void WgPen::advancePosCursor( const WgCaretInstance& instance )
{
	WgCaret_p pCursor = WgTextTool::getCursor( instance.m_pText );
	if( !pCursor )
		return;

	WgCaret::Mode mode = instance.cursorMode();

	WgGfxAnim_p pAnim	= pCursor->anim( mode );
	if( !pAnim )
		return;

	int advance = pCursor->advance(mode);

	if( pCursor->scaleWidth(mode) )
	{
		float	scaleValue = (pCursor->sizeRatio(mode) * getLineSpacing())/pAnim->size().h;
		advance = (int) (advance * scaleValue);
	}

	m_pos.x += advance;
}

