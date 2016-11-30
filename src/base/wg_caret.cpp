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
		
	//____ setMode() _______________________________________________________________
	
	bool Caret::setMode( CaretMode mode )
	{
		if( mode != m_mode )
		{
			m_mode = mode;
			m_ticks = 0;
			m_bNeedToRender = true;
		}
		return m_bNeedToRender;
	}

	//____ setCycleLength() ____________________________________________________

	bool Caret::setCycleLength( int millisec )
	{
		if( millisec < 2 )
			return false;
			
		m_cycleLength = millisec;
		return true;		
	}

	//____ restartCycle() ______________________________________________________

 	bool Caret::restartCycle()
	{ 
		int oldTicks = m_ticks;
		m_ticks = 0; 

		return _updateNeedToRender( oldTicks, m_ticks );
	}	
	
	
	//____ eolWidth() ______________________________________________________________
	
	int Caret::eolWidth( const Size& eolCell ) const
	{
		return wg::max( 1, eolCell.h / 16);
	}
	
	//____ tick() __________________________________________________________________
	
	bool Caret::tick( int ms )
	{
		int oldTicks = m_ticks;
		m_ticks = (m_ticks + ms) % m_cycleLength;
	
		return _updateNeedToRender( oldTicks, m_ticks );
	}
	
	//____ dirtyRect() _____________________________________________________________
	
	Rect Caret::dirtyRect( Rect cell ) const
	{
		switch( m_mode )
		{
			case CaretMode::Overwrite:
				return cell;
			case CaretMode::Eol:
				return Rect( cell.x, cell.y, wg::max(1, cell.h/ 16), cell.h );
			default: // CaretMode::Insert:
				return Rect( cell.x, cell.y, wg::max(1, cell.h/ 16), cell.h );
		}
	}
	
	//____ render() ________________________________________________________________
	
	void Caret::render( GfxDevice * pDevice, Rect cell, const Rect& clip )
	{
		if( m_ticks < m_cycleLength / 2 )
		{
			Rect r = dirtyRect(cell);
			BlendMode oldMode = pDevice->blendMode();
			pDevice->setBlendMode(BlendMode::Invert);
			pDevice->fill( Rect(r,clip), Color::White );
			pDevice->setBlendMode(oldMode);
		}
		
		m_bNeedToRender = false;
	}

	//____ _updateNeedToRender() _______________________________________________

	bool Caret::_updateNeedToRender( int oldTicks, int newTicks )
	{
		int halfCycle = m_cycleLength / 2;
		int oldBlink = oldTicks / halfCycle;
		
		int newBlink = newTicks / halfCycle;
		if( newBlink != oldBlink )
			m_bNeedToRender = true;
			
		return m_bNeedToRender;
	}
	

} // namespace wg
