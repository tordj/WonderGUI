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

#include <wg_modvalue.h>

const char WgModValue::CLASSNAME[] = {"ModValue"};


//____ isInstanceOf() _________________________________________________________

bool WgModValue::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgValue::isInstanceOf(pClassName);
}

//____ className() ____________________________________________________________

const char * WgModValue::className( void ) const
{ 
	return CLASSNAME; 
}

//____ cast() _________________________________________________________________

WgModValuePtr WgModValue::cast( const WgInterfacePtr& pInterface )
{
	if( pInterface && pInterface->isInstanceOf(CLASSNAME) )
		return WgModValuePtr( pInterface.getRealObjectPtr(), static_cast<WgModValue*>( pInterface.rawPtr()) );

	return 0;
}

//____ clear() _________________________________________________________________

void WgModValue::clear() 
{ 
	bool bModified = _field()->value() != 0;
	_field()->clear();

	if( bModified )
		_field()->onValueModified(); 
}

//____ set() ___________________________________________________________________

bool WgModValue::set( Sint64 value, int scale ) 
{ 
	if( _field()->set(value,scale) )
	{
		_field()->onValueModified();
		return true;
	} 
	else
		return false;

}

//____ setRange() ______________________________________________________________

bool WgModValue::setRange( Sint64 min, Sint64 max ) 
{ 
	Sint64 val = _field()->value();
	
	bool retVal = _field()->setRange(min,max); 
	if( val != _field()->value() )
		_field()->onValueModified();
	
	return retVal;
}

