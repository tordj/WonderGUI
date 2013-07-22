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


//____ IsWidget() ______________________________________________________________

bool WgWidgetHolder::IsWidget() const
{
	return false;
}

//____ IsContainer() ______________________________________________________________

bool WgWidgetHolder::IsContainer() const
{
	return false;
}

//____ IsPanel() ______________________________________________________________

bool WgWidgetHolder::IsPanel() const
{
	return false;
}

//____ IsList() ______________________________________________________________

bool WgWidgetHolder::IsList() const
{
	return false;
}

//____ IsCapsule() ______________________________________________________________

bool WgWidgetHolder::IsCapsule() const
{
	return false;
}

//____ IsLayer() ______________________________________________________________

bool WgWidgetHolder::IsLayer() const
{
	return false;
}

//____ IsRoot() _______________________________________________________________

bool WgWidgetHolder::IsRoot() const
{
	return false;
}

//____ CastToWidget() _______________________________________________________

WgWidget * WgWidgetHolder::CastToWidget()
{
	return 0;
}

const WgWidget * WgWidgetHolder::CastToWidget() const
{
	return 0;
}


//____ CastToContainer() _______________________________________________________

WgContainer * WgWidgetHolder::CastToContainer()
{
	return 0;
}

const WgContainer * WgWidgetHolder::CastToContainer() const
{
	return 0;
}


//____ CastToPanel() _______________________________________________________

WgPanel * WgWidgetHolder::CastToPanel()
{
	return 0;
}

const WgPanel * WgWidgetHolder::CastToPanel() const
{
	return 0;
}

//____ CastToList() _______________________________________________________

WgList * WgWidgetHolder::CastToList()
{
	return 0;
}

const WgList * WgWidgetHolder::CastToList() const
{
	return 0;
}

//____ CastToCapsule() _______________________________________________________

WgCapsule * WgWidgetHolder::CastToCapsule()
{
	return 0;
}

const WgCapsule * WgWidgetHolder::CastToCapsule() const
{
	return 0;
}

//____ CastToLayer() _______________________________________________________

WgLayer * WgWidgetHolder::CastToLayer()
{
	return 0;
}

const WgLayer * WgWidgetHolder::CastToLayer() const
{
	return 0;
}

//____ CastToRoot() ___________________________________________________________

WgRootPanel * WgWidgetHolder::CastToRoot()
{
	return 0;
}

const WgRootPanel * WgWidgetHolder::CastToRoot() const
{
	return 0;
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