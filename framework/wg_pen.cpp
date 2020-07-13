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
#include <wg_text.h>
#include <wg3_font.h>
#include <wg_cursorinstance.h>
#include <wg_gfxanim.h>
#include <wg_blockset.h>
#include <wg_base.h>

WgPen::WgPen()
{
	_init();
}

WgPen::WgPen( wg::GfxDevice * pDevice, const WgCoord& origo )
{
	_init();

	m_pDevice	= pDevice;
	m_origo		= origo;
	m_pos		= origo;
}

//____ _init() _________________________________________________________________

void WgPen::_init()
{
	m_pDevice = 0;

	m_pFont = 0;

	m_size = 0;
	m_wantedSize = 0;

	m_pGlyph = &m_dummyGlyph;
	m_pPrevGlyph = &m_dummyGlyph;
	m_color = 0xFFFFFFFF;

	m_bShowSpace = false;
	m_bShowCRLF = false;

	m_tabWidth = 80;

	m_scale = WG_SCALE_BASE;
}


//____ SetScale() ______________________________________________________________

void WgPen::SetScale( int scale )
{
	m_scale = scale;
	_onAttrChanged();
}


//____ _onAttrChanged() __________________________________________________________

void WgPen::_onAttrChanged()
{
	if( !m_pFont )
	{
		return;
	}

	m_size = m_wantedSize * m_scale >> WG_SCALE_BINALS;
	m_pFont->setSize(wg::MU::fromPX(m_size));
}

//____ SetAttributes() ________________________________________________________

bool WgPen::SetAttributes( const wg::TextAttr& attr )
{
	if( attr.size < 0 && attr.size > WG_MAX_FONTSIZE )
		return false;

	if( attr.pFont != m_pFont || attr.size != m_wantedSize )
		m_pPrevGlyph = &m_dummyGlyph;                           // Can't do kerning with character of different font or size.

	m_pFont			= attr.pFont;
	m_wantedSize	= attr.size;
	m_color			= attr.color;
	_onAttrChanged();
	return true;
}

//____ SetSize() ______________________________________________________________

bool WgPen::SetSize( int size )
{
	if( size < 0 && size > WG_MAX_FONTSIZE )
		return false;

	if( size != m_wantedSize )
	{
		m_pPrevGlyph = &m_dummyGlyph;                           // Can't do kerning with character of different font or size.
		m_wantedSize = size;
		_onAttrChanged();
	}
	return true;
}

//____ SetFont() ______________________________________________________________

void WgPen::SetFont( wg::Font * pFont )
{
	if( pFont != m_pFont )
	{
		m_pPrevGlyph = &m_dummyGlyph;                           // Can't do kerning with character of different font or size.
		m_pFont = pFont;
		_onAttrChanged();
	}
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
			if(m_pFont)
				m_dummyGlyph.SetAdvance( m_pFont->whitespaceAdvance() );
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

	if( chr == int(WgExtChar::BreakPermitted) || chr == int(WgExtChar::HyphenBreakPermitted) )
	{
		m_dummyGlyph.SetAdvance(0);
		m_pGlyph = &m_dummyGlyph;
		return false;
	}

	// Get the glyph from our Glyphset.

	if( !m_pFont )
	{
		m_pGlyph = &m_dummyGlyph;
		return false;
	}

	// First we try to get the glyph from our Glyphset.

	wg::Glyph_p p = m_pFont->getGlyph( chr );
	if( !p )
	{
		// The glyph doesn't exist in this font, try to get the unicode
		// WHITE_BOX as a replacement glyph.

		p = m_pFont->getGlyph( 0xFFFD );
		if( !p )
		{
			// We don't have the white box, try with the most suitable
			// common ascii character instead...

			p = m_pFont->getGlyph( '*' );
			if( !p )
			{
				// Total failure, nothing to render...

				m_pGlyph = &m_dummyGlyph;
				m_dummyGlyph.SetAdvance(0);
				return false;
			}
		}
	}

	m_pGlyph = p;
	return true;
}

//____ BlitChar() _____________________________________________________________

void WgPen::BlitChar() const
{
	const wg::GlyphBitmap * pSrc = m_pGlyph->getBitmap();

	if( pSrc )
	{
		int x = m_pos.x + pSrc->bearingX;
		int y = m_pos.y + pSrc->bearingY;

		m_pDevice->setBlitSource(pSrc->pSurface);            //TODO: Optimize!!!!

		m_pDevice->blit( WgCoord(x,y), pSrc->rect);
	}
}

//____ BlitCursor() ___________________________________________________________

bool WgPen::BlitCursor( const WgCursorInstance& instance ) const
{
	WgCursor * pCursor = _getCursor(instance.m_pText);
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
			tintColor = m_pDevice->tintColor();
			m_pDevice->setTintColor( tintColor * instance.text()->getColor() );
			break;
		case WgCursor::INVERT_BG:
			blendMode = m_pDevice->blendMode();
			m_pDevice->setBlendMode(WgBlendMode::Invert);
			break;
	}

	//

	WgGfxDevice::BlitBlock(m_pDevice, block, WgRect(m_pos + bearing, size));

	// Restore tintcolor/blendmode.

	switch( pCursor->GetBlitMode() )
	{
		case WgCursor::NORMAL:
			break;
		case WgCursor::TINTED:
			m_pDevice->setTintColor( tintColor );
			break;
		case WgCursor::INVERT_BG:
			m_pDevice->setBlendMode( blendMode );
			break;
	}

	return true;
}



//____ AdvancePosCursor() _____________________________________________________

void WgPen::AdvancePosCursor( const WgCursorInstance& instance )
{
	WgCursor * pCursor = _getCursor( instance.m_pText );
	if( pCursor == 0 )
		return;

	WgCursor::Mode mode = instance.cursorMode();

	WgGfxAnim * pAnim	= pCursor->Anim( mode );
	if( !pAnim )
		return;

	int advance = pCursor->Advance(mode);

	if( pAnim->BlockFlags() & WG_SCALE_CENTER )
	{
		float	scaleValue = (pCursor->SizeRatio(mode) * GetLineSpacing())/pAnim->Size(WG_SCALE_BASE).h;
		advance = (int) (advance * scaleValue);
	}

	m_pos.x += advance;
}

//____ _getCursor() _____________________________________________________

WgCursor * WgPen::_getCursor(const WgText * pText) const
{
	WgCursor * p = pText->getCursorStyle();
	if(p)
		return p;
	return WgBase::GetDefaultCursor();
}

