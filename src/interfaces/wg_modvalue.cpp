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

namespace wg 
{
	
	const char ModValue::CLASSNAME[] = {"ModValue"};
	
	
	//____ isInstanceOf() _________________________________________________________
	
	bool ModValue::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return Value::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * ModValue::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	ModValue_p ModValue::cast( const Interface_p& pInterface )
	{
		if( pInterface && pInterface->isInstanceOf(CLASSNAME) )
			return ModValue_p( pInterface.getRealObjectPtr(), static_cast<ModValue*>( pInterface.rawPtr()) );
	
		return 0;
	}
	
	//____ clear() _________________________________________________________________
	
	void ModValue::clear() 
	{ 
		bool bModified = _item()->value() != 0;
		_item()->clear();
	
		if( bModified )
			_item()->onValueModified(); 
	}
	
	//____ set() ___________________________________________________________________
	
	bool ModValue::set( int64_t value, int scale ) 
	{ 
		if( _item()->set(value,scale) )
		{
			_item()->onValueModified();
			return true;
		} 
		else
			return false;
	
	}
	
	//____ setRange() ______________________________________________________________
	
	bool ModValue::setRange( int64_t min, int64_t max ) 
	{ 
		int64_t val = _item()->value();
		
		bool retVal = _item()->setRange(min,max); 
		if( val != _item()->value() )
			_item()->onValueModified();
		
		return retVal;
	}
	

} // namespace wg
