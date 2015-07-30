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
#include <wg_receiver.h>

const char WgReceiver::CLASSNAME[] = {"Receiver"};



//____ isInstanceOf() _________________________________________________________

bool WgReceiver::isInstanceOf( const char * pClassName ) const
{
	if( pClassName==CLASSNAME )
		return true;

	return WgObject::isInstanceOf(pClassName);
}

//____ className() ____________________________________________________________

const char * WgReceiver::className( void ) const
{
	return CLASSNAME;
}

//____ cast() _________________________________________________________________

WgReceiver_p WgReceiver::cast( const WgObject_p& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgReceiver_p( static_cast<WgReceiver*>(pObject.rawPtr()) );

	return 0;
}

//____ _onRouteAdded() _________________________________________________________

void  WgReceiver::_onRouteAdded()
{	
}

//____ _onRouteRemoved() _______________________________________________________

void  WgReceiver::_onRouteRemoved()
{
}
