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

#include <wg_list.h>

const char WgList::CLASSNAME[] = {"List"};
const char WgListHook::CLASSNAME[] = {"ListHook"};


//____ WgListHook::IsInstanceOf() __________________________________________

bool WgListHook::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgHook::IsInstanceOf(pClassName);
}

//____ WgListHook::ClassName() _____________________________________________

const char * WgListHook::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ WgListHook::Cast() __________________________________________________

WgListHookPtr WgListHook::Cast( const WgHookPtr& pHook )
{
	if( pHook && pHook->IsInstanceOf(CLASSNAME) )
		return WgListHookPtr( static_cast<WgListHook*>(pHook.GetRealPtr()) );

	return 0;
}


