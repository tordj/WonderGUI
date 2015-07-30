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

//____ isInstanceOf() _________________________________________________________

bool WgCaret2::isInstanceOf( const char * pClassName ) const
{
	if( pClassName==CLASSNAME )
		return true;

	return WgObject::isInstanceOf(pClassName);
}

//____ className() ____________________________________________________________

const char * WgCaret2::className( void ) const
{
	return CLASSNAME;
}

//____ cast() _________________________________________________________________

WgCaret2Ptr WgCaret2::cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgCaret2Ptr( static_cast<WgCaret2*>(pObject.rawPtr()) );

	return 0;
}

//____ setGlyph() ______________________________________________________________

void WgCaret2::setGlyph( Uint16 glyph, int size, int advance, int ascend, int descend )
{
	m_glyph = glyph;
	m_glyphSize = size;
	m_glyphAdvance = advance;
	m_glyphAscend = ascend;
	m_glyphDescend = descend;

	m_ticks = 0;
	m_bNeedToRender = true;
}

//____ setMode() _______________________________________________________________

void WgCaret2::setMode( WgCaretMode mode )
{
	if( mode != m_mode )
	{
		m_mode = mode;
		m_ticks = 0;
		m_bNeedToRender = true;
	}
}

//____ eolWidth() ______________________________________________________________

int WgCaret2::eolWidth( int size, int advance, int ascend, int descend ) const
{
	return WgMax( 1, size / 8);
}

//____ tick() __________________________________________________________________

void WgCaret2::tick( int ms )
{
	int halfCycle = m_cycleLength / 2;
	int oldBlink = m_ticks / halfCycle;

	m_ticks = (m_ticks + ms) % m_cycleLength;

	int newBlink = m_ticks / halfCycle;
	if( newBlink != oldBlink )
		m_bNeedToRender = true;
}

//____ dirtyRect() _____________________________________________________________

WgRect WgCaret2::dirtyRect( WgCoord pos ) const
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

//____ render() ________________________________________________________________

void WgCaret2::render( WgGfxDevice * pDevice, WgCoord pos, const WgRect& clip )
{
	if( m_ticks < m_cycleLength / 2 )
	{
		WgRect r = dirtyRect(pos);
		WgBlendMode oldMode = pDevice->getBlendMode();
		pDevice->setBlendMode(WG_BLENDMODE_INVERT);
		pDevice->fill( WgRect(r,clip), WgColor::black );
		pDevice->setBlendMode(oldMode);
	}
}

