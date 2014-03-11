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

#include <wg_iwidgetholder.h>
#include <wg_widget.h>


const char WgIWidgetHolder::CLASSNAME[] = {"IWidgetHolder"};


//____ IsInstanceOf() _________________________________________________________

bool WgIWidgetHolder::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgInterface::IsInstanceOf(pClassName);
}

//____ ClassName() ____________________________________________________________

const char * WgIWidgetHolder::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ Cast() _________________________________________________________________

WgIWidgetHolderPtr WgIWidgetHolder::Cast( const WgInterfacePtr& pInterface )
{
	if( pInterface && pInterface->IsInstanceOf(CLASSNAME) )
		return WgIWidgetHolderPtr( pInterface.GetRealObjectPtr(), static_cast<WgIWidgetHolder*>( pInterface.GetRealPtr()) );

	return 0;
}

//____ IsContainer() ______________________________________________________________

bool WgIWidgetHolder::IsContainer() const
{
	return false;
}

//____ IsRoot() _______________________________________________________________

bool WgIWidgetHolder::IsRoot() const
{
	return false;
}

//____ _firstWidget() __________________________________________________________

WgWidget * WgIWidgetHolder::_firstWidget() const 
{ 
	WgHook * p = _firstHook(); 
	if( p ) 
		return p->_widget(); 
	else 
		return 0;
}

//____ _lastWidget() ___________________________________________________________

WgWidget * WgIWidgetHolder::_lastWidget() const 
{ 
	WgHook * p = _lastHook(); 
	if( p ) 
		return p->_widget(); 
	else 
		return 0; 
}