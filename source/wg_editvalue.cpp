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

#include <wg_editvalue.h>

const char WgEditValue::CLASSNAME[] = {"EditValue"};


//____ IsInstanceOf() _________________________________________________________

bool WgEditValue::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgModValue::IsInstanceOf(pClassName);
}

//____ ClassName() ____________________________________________________________

const char * WgEditValue::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ Cast() _________________________________________________________________

WgEditValuePtr WgEditValue::Cast( const WgInterfacePtr& pInterface )
{
	if( pInterface && pInterface->IsInstanceOf(CLASSNAME) )
		return WgEditValuePtr( pInterface.GetRealObjectPtr(), static_cast<WgEditValue*>(pInterface.GetRealPtr()) );

	return 0;
}

