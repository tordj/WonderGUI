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
#include <wg_caret2.h>
#include <wg_gfxdevice.h>

const char WgCaret2::CLASSNAME[] = {"Caret"};


//____ Constructor _____________________________________________________________

WgCaret2::WgCaret2()
{
	m_glyph = 0;
	m_glyphSize = 0;
	m_glyphAdvance = 0;
	m_glyphAscend = 0;
	m_glyphDescend = 0;
	m_mode = WG_CARET_EOL;
	m_ticks = 0;
	m_cycleLength = 1000;
	m_bNeedToRender = false;
}

//____ IsInstanceOf() _________________________________________________________

bool WgCaret2::IsInstanceOf( const char * pClassName ) const
{
	if( pClassName==CLASSNAME )
		return true;

	return WgObject::IsInstanceOf(pClassName);
}

//____ ClassName() ____________________________________________________________

const char * WgCaret2::ClassName( void ) const
{
	return CLASSNAME;
}

//____ Cast() _________________________________________________________________

WgCaret2Ptr WgCaret2::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgCaret2Ptr( static_cast<WgCaret2*>(pObject.RawPtr()) );

	return 0;
}

//____ SetGlyph() ______________________________________________________________

void WgCaret2::SetGlyph( Uint16 glyph, int size, int advance, int ascend, int descend )
{
	m_glyph = glyph;
	m_glyphSize = size;
	m_glyphAdvance = advance;
	m_glyphAscend = ascend;
	m_glyphDescend = descend;

	m_ticks = 0;
	m_bNeedToRender = true;
}

//____ SetMode() _______________________________________________________________

void WgCaret2::SetMode( WgCaretMode mode )
{
	if( mode != m_mode )
	{
		m_mode = mode;
		m_ticks = 0;
		m_bNeedToRender = true;
	}
}

//____ EOLWidth() ______________________________________________________________

int WgCaret2::EOLWidth( int size, int advance, int ascend, int descend ) const
{
	return WgMax( 1, size / 8);
}

//____ Tick() __________________________________________________________________

void WgCaret2::Tick( int ms )
{
	int halfCycle = m_cycleLength / 2;
	int oldBlink = m_ticks / halfCycle;

	m_ticks = (m_ticks + ms) % m_cycleLength;

	int newBlink = m_ticks / halfCycle;
	if( newBlink != oldBlink )
		m_bNeedToRender = true;
}

//____ DirtyRect() _____________________________________________________________

WgRect WgCaret2::DirtyRect( WgCoord pos ) const
{
	switch( m_mode )
	{
		case WG_CARET_INSERT:
			return WgRect( pos.x, pos.y - m_glyphAscend, WgMax(1, m_glyphSize/8), m_glyphAscend + m_glyphDescend );
		case WG_CARET_OVERWRITE:
			return WgRect( pos.x, pos.y - m_glyphAscend, m_glyphAdvance, m_glyphAscend + m_glyphDescend );
		case WG_CARET_EOL:
			return WgRect( pos.x, pos.y - m_glyphAscend, WgMax(1, m_glyphSize/8), m_glyphAscend + m_glyphDescend );
	}
}

//____ Render() ________________________________________________________________

void WgCaret2::Render( WgGfxDevice * pDevice, WgCoord pos, const WgRect& clip )
{
	if( m_ticks < m_cycleLength / 2 )
	{
		WgRect r = DirtyRect(pos);
		WgBlendMode oldMode = pDevice->GetBlendMode();
		pDevice->SetBlendMode(WG_BLENDMODE_INVERT);
		pDevice->Fill( WgRect(r,clip), WgColor::black );
		pDevice->SetBlendMode(oldMode);
	}
}

