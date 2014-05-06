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

#include <wg_iicon.h>

const char WgIIcon::CLASSNAME[] = {"IIcon"};


//____ IsInstanceOf() _________________________________________________________

bool WgIIcon::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgInterface::IsInstanceOf(pClassName);
}

//____ ClassName() ____________________________________________________________

const char * WgIIcon::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ Cast() _________________________________________________________________

WgIIconPtr WgIIcon::Cast( const WgInterfacePtr& pInterface )
{
	if( pInterface && pInterface->IsInstanceOf(CLASSNAME) )
		return WgIIconPtr( pInterface.GetRealObjectPtr(), static_cast<WgIIcon*>( pInterface.GetRealPtr()) );

	return 0;
}

