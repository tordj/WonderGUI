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

namespace wg 
{
	
	const char Caret2::CLASSNAME[] = {"Caret"};
	
	
	//____ Constructor _____________________________________________________________
	
	Caret2::Caret2()
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
	
	bool Caret2::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;
	
		return Object::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * Caret2::className( void ) const
	{
		return CLASSNAME;
	}
	
	//____ cast() _________________________________________________________________
	
	Caret2_p Caret2::cast( const Object_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return Caret2_p( static_cast<Caret2*>(pObject.rawPtr()) );
	
		return 0;
	}
	
	//____ setGlyph() ______________________________________________________________
	
	void Caret2::setGlyph( Uint16 glyph, int size, int advance, int ascend, int descend )
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
	
	void Caret2::setMode( CaretMode mode )
	{
		if( mode != m_mode )
		{
			m_mode = mode;
			m_ticks = 0;
			m_bNeedToRender = true;
		}
	}
	
	//____ eolWidth() ______________________________________________________________
	
	int Caret2::eolWidth( int size, int advance, int ascend, int descend ) const
	{
		return WgMax( 1, size / 8);
	}
	
	//____ tick() __________________________________________________________________
	
	void Caret2::tick( int ms )
	{
		int halfCycle = m_cycleLength / 2;
		int oldBlink = m_ticks / halfCycle;
	
		m_ticks = (m_ticks + ms) % m_cycleLength;
	
		int newBlink = m_ticks / halfCycle;
		if( newBlink != oldBlink )
			m_bNeedToRender = true;
	}
	
	//____ dirtyRect() _____________________________________________________________
	
	Rect Caret2::dirtyRect( Coord pos ) const
	{
		switch( m_mode )
		{
			case WG_CARET_INSERT:
				return Rect( pos.x, pos.y - m_glyphAscend, WgMax(1, m_glyphSize/8), m_glyphAscend + m_glyphDescend );
			case WG_CARET_OVERWRITE:
				return Rect( pos.x, pos.y - m_glyphAscend, m_glyphAdvance, m_glyphAscend + m_glyphDescend );
			case WG_CARET_EOL:
				return Rect( pos.x, pos.y - m_glyphAscend, WgMax(1, m_glyphSize/8), m_glyphAscend + m_glyphDescend );
		}
	}
	
	//____ render() ________________________________________________________________
	
	void Caret2::render( GfxDevice * pDevice, Coord pos, const Rect& clip )
	{
		if( m_ticks < m_cycleLength / 2 )
		{
			Rect r = dirtyRect(pos);
			WgBlendMode oldMode = pDevice->getBlendMode();
			pDevice->setBlendMode(WG_BLENDMODE_INVERT);
			pDevice->fill( Rect(r,clip), Color::black );
			pDevice->setBlendMode(oldMode);
		}
	}
	

} // namespace wg
