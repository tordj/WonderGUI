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
#include <wg_printablefield.h>

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
	
	String Printer::tooltip( const PrintableField * pField ) const
	{
		return String();
	}
	
	//____ _charBuffer() ___________________________________________________________
	
	CharBuffer * Printer::_charBuffer( PrintableField * pField ) const
	{
		return &(pField->m_charBuffer);
	}
	
	const CharBuffer * Printer::_charBuffer( const PrintableField * pField ) const
	{
		return &(pField->m_charBuffer);
	}
	
	//____ _fieldDataBlock() _______________________________________________________
	
	void * Printer::_fieldDataBlock( PrintableField * pField) const
	{
		return pField->m_pPrinterData;
	}
	
	const void * Printer::_fieldDataBlock( const PrintableField * pField) const
	{
		return pField->m_pPrinterData;
	}
	
	
	//____ _fieldDataInt() _________________________________________________________
	
	int Printer::_fieldDataInt( PrintableField * pField ) const
	{
		return pField->m_printerData;
	}
	
	//____ _baseStyle() _________________________________________________________
	
	TextStyle * Printer::_baseStyle( PrintableField * pField ) const
	{
		TextStyle * pStyle = pField->m_pStyle.rawPtr();
		if( pStyle == 0 )
			pStyle = Base::defaultStyle().rawPtr();
		
		return pStyle;
	}
	
	//____ _state() _________________________________________________________
	
	State Printer::_state( PrintableField * pField ) const
	{
		return pField->m_state;
	}
	
	//____ _setFieldDataBlock() ____________________________________________________
	
	void  Printer::_setFieldDataBlock( PrintableField * pField, void * pBlock )
	{
		pField->m_pPrinterData = pBlock;
	}
	
	//____ _setFieldDataInt() ______________________________________________________
	
	void  Printer::_setFieldDataInt( PrintableField * pField, int data )
	{
		pField->m_printerData = data;
	}

	//____ _setFieldDirty() ____________________________________________________

	void  Printer::_setFieldDirty( PrintableField * pField )
	{
		pField->_onDirty();
	}
	
	void  Printer::_setFieldDirty( PrintableField * pField, const Rect& rect )
	{
		pField->_onDirty( rect );
	}

	//____ _requestFieldResize() _______________________________________________

	void Printer::_requestFieldResize( PrintableField * pField )
	{
		pField->_onResize();
	}
	

} // namespace wg
