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

#include <wg_printablefield.h>

namespace wg 
{
	
	//____ Constructor _____________________________________________________________
	
	PrintableField::PrintableField( PrintableHolder * pHolder ) : Field( pHolder )
	{
		_printer()->addField(this);
	}
	
	//____ Destructor ______________________________________________________________
	
	PrintableField::~PrintableField()
	{
		_printer()->removeField(this);
	}
	
	//____ setStyle() ______________________________________________________________
	
	void PrintableField::setStyle( const TextStyle_p& pStyle )
	{
		TextStyle_p pOld = m_pStyle;			// Keep ref count until onStyleChanged has been called.
		m_pStyle = pStyle;
		_printer()->onStyleChanged(this, pStyle.rawPtr(), pOld.rawPtr());
	}
	
	//____ clearStyle() ____________________________________________________________
	
	void PrintableField::clearStyle()
	{
		TextStyle_p pOld = m_pStyle;			// Keep ref count until onStyleChanged has been called.
		m_pStyle = 0;
		_printer()->onStyleChanged(this, nullptr, pOld.rawPtr());
	}
	
	//____ setPrinter() __________________________________________________________
	
	void PrintableField::setPrinter( const Printer_p& pPrinter )
	{
		if( pPrinter == m_pPrinter )
			return;
			
		_printer()->removeField(this);
		m_pPrinter = pPrinter;
		_printer()->addField(this);
	}
	
	//____ clearPrinter() ________________________________________________________
	
	void PrintableField::clearPrinter()
	{
		if( !m_pPrinter )
			return;
			
		_printer()->removeField(this);
		m_pPrinter = 0;
		_printer()->addField(this);
	}
	
	//____ setState() ______________________________________________________________
	
	void PrintableField::setState( State state )
	{
		if( state == m_state )
			return;
	
		State old = m_state;
		m_state = state;
		_printer()->onStateChanged( this, state, old );
	}
	
	//____ preferredSize() _________________________________________________________
	
	Size PrintableField::preferredSize() const
	{
		return _printer()->preferredSize(this);
	}
	
	//____ matchingWidth() _________________________________________________________
	
	int PrintableField::matchingWidth( int height ) const
	{
		return _printer()->matchingWidth(this, height);
	}
	
	//____ matchingHeight() ________________________________________________________
	
	int PrintableField::matchingHeight( int width ) const
	{
		return _printer()->matchingHeight(this, width);
	}
	
	//____ charAtPos() ___________________________________________________________
	
	int PrintableField::charAtPos( Coord pos ) const
	{
		return _printer()->charAtPos(this,pos);
	}
	
	//____ charRect() ____________________________________________________________
	
	Rect PrintableField::charRect( int charOfs ) const
	{
		return _printer()->charRect(this, charOfs);
	}
	
	//____ charLine() ____________________________________________________________
	
	int PrintableField::charLine( int charOfs ) const
	{
		return _printer()->charLine(this, charOfs);
	}
	
	
	//____ onRefresh() _____________________________________________________________
	
	void PrintableField::onRefresh()
	{
		_printer()->onRefresh(this);
	}
	
	//____ onNewSize() _____________________________________________________________
	
	void PrintableField::onNewSize( const Size& size )
	{
		if( size == m_size )
			return;
	
		Size oldSize = m_size;
		m_size = size;
		_printer()->onFieldResized(this,size, oldSize);
	}
	
	//_____ onRender() _____________________________________________________________
	
	void  PrintableField::onRender( GfxDevice * pDevice, const Rect& _canvas, const Rect& _clip )
	{
		_printer()->renderField(this, pDevice, _canvas, _clip);
	}
	
	//____ rectForRange() __________________________________________________________
	
	Rect  PrintableField::rectForRange( int ofs, int length ) const
	{
		return _printer()->rectForRange(this, ofs, length);
	}
	
	//____ tooltip() _______________________________________________________________
	
	String PrintableField::tooltip() const
	{
		return _printer()->tooltip(this);
	}
	
	//____ getString() ___________________________________________________________________
	
	String PrintableField::getString() const
	{
		return String(&m_charBuffer);
	}
	
	//____ selectionBegin() ________________________________________________________
	
	int PrintableField::selectionBegin() const
	{
		return 0;
	}
	
	//____ selectionEnd() __________________________________________________________
	
	int PrintableField::selectionEnd() const
	{
		return 0;
	}

} // namespace wg
