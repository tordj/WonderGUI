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

#include <wg_eventcatcher.h>

const char WgEventCatcher::CLASSNAME[] = {"EventCatcher"};


//____ Constructors ____________________________________________________________

WgEventCatcher::WgEventCatcher( void(*fp)( const WgEventPtr& pEvent) )
{
	m_callbackType = 0;
	m_pCallback = (void*) fp;	
}

WgEventCatcher::WgEventCatcher( void(*fp)( const WgEventPtr& pEvent, int param), int param )
{
	m_callbackType = 1;
	m_pCallback = (void*) fp;
	m_param = param;
}

WgEventCatcher::WgEventCatcher( void(*fp)( const WgEventPtr& pEvent, void * pParam), void * pParam )
{
	m_callbackType = 2;
	m_pCallback = (void*) fp;
	m_pParam = pParam;
}

WgEventCatcher::WgEventCatcher( void(*fp)( const WgEventPtr& pEvent, const WgObjectPtr& pParam), const WgObjectPtr& pParam )
{
	m_callbackType = 3;
	m_pCallback = (void*) fp;
	m_pParamObj = pParam;
	
}


//____ IsInstanceOf() _________________________________________________________

bool WgEventCatcher::IsInstanceOf( const char * pClassName ) const
{
	if( pClassName==CLASSNAME )
		return true;

	return WgEventListener::IsInstanceOf(pClassName);
}

//____ ClassName() ____________________________________________________________

const char * WgEventCatcher::ClassName( void ) const
{
	return CLASSNAME;
}

//____ Cast() _________________________________________________________________

WgEventCatcherPtr WgEventCatcher::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgEventCatcherPtr( static_cast<WgEventCatcher*>(pObject.RawPtr()) );

	return 0;
}

//____ OnEvent() _______________________________________________________________

void WgEventCatcher::OnEvent( const WgEventPtr& pEvent )
{
	switch( m_callbackType )
	{
		case 0:
			((void(*)( const WgEventPtr&))m_pCallback)(pEvent);
			break;
		case 1:
			((void(*)( const WgEventPtr&, int ))m_pCallback)(pEvent,m_param);
			break;
		case 2:
			((void(*)( const WgEventPtr&, void* ))m_pCallback)(pEvent,m_pParam);
			break;
		case 3:
			((void(*)( const WgEventPtr&, const WgObjectPtr& ))m_pCallback)(pEvent,m_pParamObj);
			break;
		default:
			break;
	}
}

//____ _onRouteAdded() _________________________________________________________

void  WgEventCatcher::_onRouteAdded()
{	
	_incRefCount();
}

//____ _onRouteRemoved() _______________________________________________________

void  WgEventCatcher::_onRouteRemoved()
{
	_decRefCount();
}
