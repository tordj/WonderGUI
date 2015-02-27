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


//____ IsInstanceOf() _________________________________________________________

bool WgModValue::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgValue::IsInstanceOf(pClassName);
}

//____ ClassName() ____________________________________________________________

const char * WgModValue::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ Cast() _________________________________________________________________

WgModValuePtr WgModValue::Cast( const WgInterfacePtr& pInterface )
{
	if( pInterface && pInterface->IsInstanceOf(CLASSNAME) )
		return WgModValuePtr( pInterface.GetRealObjectPtr(), static_cast<WgModValue*>( pInterface.RawPtr()) );

	return 0;
}

//____ Clear() _________________________________________________________________

void WgModValue::Clear() 
{ 
	bool bModified = _field()->Value() != 0;
	_field()->Clear();

	if( bModified )
		_field()->OnValueModified(); 
}

//____ Set() ___________________________________________________________________

bool WgModValue::Set( Sint64 value, int scale ) 
{ 
	if( _field()->Set(value,scale) )
	{
		_field()->OnValueModified();
		return true;
	} 
	else
		return false;

}

//____ SetRange() ______________________________________________________________

bool WgModValue::SetRange( Sint64 min, Sint64 max ) 
{ 
	Sint64 val = _field()->Value();
	
	bool retVal = _field()->SetRange(min,max); 
	if( val != _field()->Value() )
		_field()->OnValueModified();
	
	return retVal;
}

