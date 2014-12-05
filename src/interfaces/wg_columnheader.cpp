
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

#include <wg_columnheader.h>
#include <wg_columnheaderfield.h>

const char WgColumnHeader::CLASSNAME[] = {"ColumnHeader"};


//____ Constructor() ___________________________________________________________


WgColumnHeader::WgColumnHeader(WgColumnHeaderField* pField) : m_pField(pField), icon(&pField->icon), arrow(&pField->arrow), label(&pField->label)
{
}

//____ IsInstanceOf() _________________________________________________________

bool WgColumnHeader::IsInstanceOf( const char * pClassName ) const
{
	if( pClassName==CLASSNAME )
		return true;

	return WgInterface::IsInstanceOf(pClassName);
}

//____ ClassName() ____________________________________________________________

const char * WgColumnHeader::ClassName( void ) const
{
	return CLASSNAME;
}

//____ Cast() _________________________________________________________________

WgColumnHeaderPtr WgColumnHeader::Cast( const WgInterfacePtr& pInterface )
{
	if( pInterface && pInterface->IsInstanceOf(CLASSNAME) )
		return WgColumnHeaderPtr( pInterface.GetRealObjectPtr(), static_cast<WgColumnHeader*>( pInterface.RawPtr()) );

	return 0;
}

//____ _object() _______________________________________________________________

WgObject * WgColumnHeader::_object() const
{
	return m_pField->_object();
}


