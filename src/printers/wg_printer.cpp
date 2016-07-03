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

#include <wg_printer.h>
#include <wg_printableitem.h>

namespace wg 
{
	
	const char Printer::CLASSNAME[] = {"Printer"};
	
	
	
	//____ isInstanceOf() _________________________________________________________
	
	bool Printer::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return Object::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * Printer::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	Printer_p Printer::cast( const Object_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return Printer_p( static_cast<Printer*>(pObject.rawPtr()) );
	
		return 0;
	}
	
	//____ tooltip() _______________________________________________________________
	
	String Printer::tooltip( const PrintableItem * pItem ) const
	{
		return String();
	}
		
	//____ _charBuffer() ___________________________________________________________
	
	CharBuffer * Printer::_charBuffer( PrintableItem * pItem ) const
	{
		return &(pItem->m_charBuffer);
	}
	
	const CharBuffer * Printer::_charBuffer( const PrintableItem * pItem ) const
	{
		return &(pItem->m_charBuffer);
	}
	
	//____ _itemDataBlock() _______________________________________________________
	
	void * Printer::_itemDataBlock( PrintableItem * pItem) const
	{
		return pItem->m_pPrinterData;
	}
	
	const void * Printer::_itemDataBlock( const PrintableItem * pItem) const
	{
		return pItem->m_pPrinterData;
	}
	
	
	//____ _itemDataInt() _________________________________________________________
	
	int Printer::_itemDataInt( const PrintableItem * pItem ) const
	{
		return pItem->m_printerData;
	}
	
	//____ _baseStyle() _________________________________________________________
	
	const TextStyle * Printer::_baseStyle( const PrintableItem * pItem ) const
	{
		TextStyle * pStyle = pItem->m_pStyle.rawPtr();
		if( pStyle == 0 )
			pStyle = Base::defaultStyle().rawPtr();
		
		return pStyle;
	}
	
	//____ _state() _________________________________________________________
	
	State Printer::_state( const PrintableItem * pItem ) const
	{
		return pItem->m_state;
	}
	
	//____ _setItemDataBlock() ____________________________________________________
	
	void  Printer::_setItemDataBlock( PrintableItem * pItem, void * pBlock )
	{
		pItem->m_pPrinterData = pBlock;
	}
	
	//____ _setItemDataInt() ______________________________________________________
	
	void  Printer::_setItemDataInt( PrintableItem * pItem, int data )
	{
		pItem->m_printerData = data;
	}

	//____ _setItemDirty() ____________________________________________________

	void  Printer::_setItemDirty( PrintableItem * pItem )
	{
		pItem->_requestRender();
	}
	
	void  Printer::_setItemDirty( PrintableItem * pItem, const Rect& rect )
	{
		pItem->_requestRender( rect );
	}

	//____ _requestItemResize() _______________________________________________

	void Printer::_requestItemResize( PrintableItem * pItem )
	{
		pItem->_requestResize();
	}
	

} // namespace wg
