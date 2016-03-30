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
#include <wg_caret.h>
#include <wg_gfxdevice.h>

namespace wg 
{
	
	const char Caret::CLASSNAME[] = {"Caret"};
	
	
	//____ Constructor _____________________________________________________________
	
	Caret::Caret()
	{
		m_mode = CaretMode::Eol;
		m_ticks = 0;
		m_cycleLength = 1000;
		m_bNeedToRender = false;
	}
	
	//____ isInstanceOf() _________________________________________________________
	
	bool Caret::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;
	
		return Object::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * Caret::className( void ) const
	{
		return CLASSNAME;
	}
	
	//____ cast() _________________________________________________________________
	
	Caret_p Caret::cast( const Object_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return Caret_p( static_cast<Caret*>(pObject.rawPtr()) );
	
		return 0;
	}
	
	//____ setCellMetrics() ______________________________________________________________
	
	void Caret::setCellMetrics( const Rect& cell )
	{
		m_cellMetrics = cell;
		m_ticks = 0;
		m_bNeedToRender = true;
	}
	
	//____ setMode() _______________________________________________________________
	
	void Caret::setMode( CaretMode mode )
	{
		if( mode != m_mode )
		{
			m_mode = mode;
			m_ticks = 0;
			m_bNeedToRender = true;
		}
	}
	
	//____ eolWidth() ______________________________________________________________
	
	int Caret::eolWidth( const Rect& eolCellMetrics ) const
	{
		return wg::max( 1, eolCellMetrics.h / 8);
	}
	
	//____ tick() __________________________________________________________________
	
	void Caret::tick( int ms )
	{
		int halfCycle = m_cycleLength / 2;
		int oldBlink = m_ticks / halfCycle;
	
		m_ticks = (m_ticks + ms) % m_cycleLength;
	
		int newBlink = m_ticks / halfCycle;
		if( newBlink != oldBlink )
			m_bNeedToRender = true;
	}
	
	//____ dirtyRect() _____________________________________________________________
	
	Rect Caret::dirtyRect( Coord pos ) const
	{
		switch( m_mode )
		{
			case CaretMode::Overwrite:
				return m_cellMetrics + pos;
			case CaretMode::Eol:
				return Rect( m_cellMetrics.x + pos.x, m_cellMetrics.y + pos.y, wg::max(1, m_cellMetrics.h / 8), m_cellMetrics.h );
			default: // CaretMode::Insert:
				return Rect( m_cellMetrics.x + pos.x, m_cellMetrics.y + pos.y, wg::max(1, m_cellMetrics.h / 8), m_cellMetrics.h );
		}
	}
	
	//____ render() ________________________________________________________________
	
	void Caret::render( GfxDevice * pDevice, Coord pos, const Rect& clip )
	{
		if( m_ticks < m_cycleLength / 2 )
		{
			Rect r = dirtyRect(pos);
			BlendMode oldMode = pDevice->getBlendMode();
			pDevice->setBlendMode(BlendMode::Invert);
			pDevice->fill( Rect(r,clip), Color::Black );
			pDevice->setBlendMode(oldMode);
		}
	}
	

} // namespace wg
