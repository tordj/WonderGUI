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

#include <wg_widgetholder.h>
#include <wg_widget.h>


//____ IsContainer() ______________________________________________________________

bool WgWidgetHolder::IsContainer() const
{
	return false;
}

//____ IsRoot() _______________________________________________________________

bool WgWidgetHolder::IsRoot() const
{
	return false;
}

//____ _firstWidget() __________________________________________________________

WgWidget * WgWidgetHolder::_firstWidget() const 
{ 
	WgHook * p = FirstHook(); 
	if( p ) 
		return p->_widget(); 
	else 
		return 0;
}

//____ _lastWidget() ___________________________________________________________

WgWidget * WgWidgetHolder::_lastWidget() const 
{ 
	WgHook * p = LastHook(); 
	if( p ) 
		return p->_widget(); 
	else 
		return 0; 
}