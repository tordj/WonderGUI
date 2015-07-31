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

#include <wg_interface.h>
#include <wg_pointers.h>

namespace wg 
{
	
	const char Interface::CLASSNAME[] = {"Interface"};
	
	bool Interface::isInstanceOf( const char * pClassName ) const
	{ 
		return (pClassName==CLASSNAME); 
	}
	
	const char * Interface::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	
	Interface_p Interface::cast( const Interface_p& pInterface )
	{
		return pInterface;
	}
	
	
	Interface_p	Interface::ptr()
	{
		return Interface_p(_object(),this);
	}
	
	Object_p Interface::holder() 
	{ 
		return Object_p( _object() ); 
	}
	

} // namespace wg
