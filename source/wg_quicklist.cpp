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

#include <wg_quicklist.h>

const char WgQuickList::CLASSNAME[] = {"QuickList"};
const char WgQuickListHook::CLASSNAME[] = {"QuickListHook"};


//____ WgQuickListHook::IsInstanceOf() __________________________________________

bool WgQuickListHook::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgListHook::IsInstanceOf(pClassName);
}

//____ WgQuickListHook::ClassName() _____________________________________________

const char * WgQuickListHook::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ WgQuickListHook::Cast() __________________________________________________

WgQuickListHookPtr WgQuickListHook::Cast( const WgHookPtr& pHook )
{
	if( pHook && pHook->IsInstanceOf(CLASSNAME) )
		return WgQuickListHookPtr( static_cast<WgQuickListHook*>(pHook.GetRealPtr()) );

	return 0;
}
