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

#include <wg_modtext.h>

const char WgModText::CLASSNAME[] = {"ModText"};


//____ isInstanceOf() _________________________________________________________

bool WgModText::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgText::isInstanceOf(pClassName);
}

//____ className() ____________________________________________________________

const char * WgModText::className( void ) const
{ 
	return CLASSNAME; 
}

//____ cast() _________________________________________________________________

WgModTextPtr WgModText::cast( const WgInterfacePtr& pInterface )
{
	if( pInterface && pInterface->isInstanceOf(CLASSNAME) )
		return WgModTextPtr( pInterface.getRealObjectPtr(), static_cast<WgModText*>( pInterface.rawPtr()) );

	return 0;
}

