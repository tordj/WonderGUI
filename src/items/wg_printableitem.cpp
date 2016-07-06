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

#include <wg_printableitem.h>

namespace wg 
{
	
	//____ Constructor _____________________________________________________________
	
	PrintableItem::PrintableItem( Widget * pWidget ) : Item( pWidget )
	{
		_printer()->addItem(this);
	}
	
	//____ Destructor ______________________________________________________________
	
	PrintableItem::~PrintableItem()
	{
		_printer()->removeItem(this);
	}
	
	//____ setStyle() ______________________________________________________________
	
	void PrintableItem::setStyle( const TextStyle_p& pStyle )
	{
		TextStyle_p pOld = m_pStyle;			// Keep ref count until onStyleChanged has been called.
		m_pStyle = pStyle;
		_printer()->onStyleChanged(this, pStyle.rawPtr(), pOld.rawPtr());
	}
	
	//____ clearStyle() ____________________________________________________________
	
	void PrintableItem::clearStyle()
	{
		TextStyle_p pOld = m_pStyle;			// Keep ref count until onStyleChanged has been called.
		m_pStyle = 0;
		_printer()->onStyleChanged(this, nullptr, pOld.rawPtr());
	}
	
	//____ setPrinter() __________________________________________________________
	
	void PrintableItem::setPrinter( const Printer_p& pPrinter )
	{
		if( pPrinter == m_pPrinter )
			return;
			
		_printer()->removeItem(this);
		m_pPrinter = pPrinter;
		_printer()->addItem(this);
	}
	
	//____ clearPrinter() ________________________________________________________
	
	void PrintableItem::clearPrinter()
	{
		if( !m_pPrinter )
			return;
			
		_printer()->removeItem(this);
		m_pPrinter = 0;
		_printer()->addItem(this);
	}
	
	//____ setState() ______________________________________________________________
	
	void PrintableItem::setState( State state )
	{
		if( state == m_state )
			return;
	
		State old = m_state;
		m_state = state;
		_printer()->onStateChanged( this, state, old );
	}
	
	//____ preferredSize() _________________________________________________________
	
	Size PrintableItem::preferredSize() const
	{
		return _printer()->preferredSize(this);
	}
	
	//____ matchingWidth() _________________________________________________________
	
	int PrintableItem::matchingWidth( int height ) const
	{
		return _printer()->matchingWidth(this, height);
	}
	
	//____ matchingHeight() ________________________________________________________
	
	int PrintableItem::matchingHeight( int width ) const
	{
		return _printer()->matchingHeight(this, width);
	}
	
	//____ charAtPos() ___________________________________________________________
	
	int PrintableItem::charAtPos( Coord pos ) const
	{
		return _printer()->charAtPos(this,pos);
	}
	
	//____ charRect() ____________________________________________________________
	
	Rect PrintableItem::charRect( int charOfs ) const
	{
		return _printer()->charRect(this, charOfs);
	}
	
	//____ charLine() ____________________________________________________________
	
	int PrintableItem::charLine( int charOfs ) const
	{
		return _printer()->charLine(this, charOfs);
	}
	
	
	//____ onRefresh() _____________________________________________________________
	
	void PrintableItem::onRefresh()
	{
		_printer()->onRefresh(this);
	}
	
	//____ onNewSize() _____________________________________________________________
	
	void PrintableItem::onNewSize( const Size& size )
	{
		if( size == m_size )
			return;
	
		Size oldSize = m_size;
		m_size = size;
		_printer()->requestResized(this,size, oldSize);
	}
	
	//_____ onRender() _____________________________________________________________
	
	void  PrintableItem::onRender( GfxDevice * pDevice, const Rect& _canvas, const Rect& _clip )
	{
		_printer()->renderItem(this, pDevice, _canvas, _clip);
	}
	
	//____ rectForRange() __________________________________________________________
	
	Rect  PrintableItem::rectForRange( int ofs, int length ) const
	{
		return _printer()->rectForRange(this, ofs, length);
	}
	
	//____ tooltip() _______________________________________________________________
	
	String PrintableItem::tooltip() const
	{
		return _printer()->tooltip(this);
	}
	
	//____ getString() ___________________________________________________________________
	
	String PrintableItem::getString() const
	{
		return String(&m_charBuffer);
	}
	
	//____ selectionBegin() ________________________________________________________
	
	int PrintableItem::selectionBegin() const
	{
		return 0;
	}
	
	//____ selectionEnd() __________________________________________________________
	
	int PrintableItem::selectionEnd() const
	{
		return 0;
	}

	//____ _editState() ________________________________________________________

	const EditState * PrintableItem::_editState() const
	{
		return nullptr;
	}



} // namespace wg
