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


//____ isInstanceOf() _________________________________________________________

bool WgEditValue::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgModValue::isInstanceOf(pClassName);
}

//____ className() ____________________________________________________________

const char * WgEditValue::className( void ) const
{ 
	return CLASSNAME; 
}

//____ cast() _________________________________________________________________

WgEditValuePtr WgEditValue::cast( const WgInterfacePtr& pInterface )
{
	if( pInterface && pInterface->isInstanceOf(CLASSNAME) )
		return WgEditValuePtr( pInterface.getRealObjectPtr(), static_cast<WgEditValue*>(pInterface.rawPtr()) );

	return 0;
}

