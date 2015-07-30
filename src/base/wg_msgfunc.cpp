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

#include <wg_msgfunc.h>

const char WgMsgFunc::CLASSNAME[] = {"MsgFunc"};


//____ Constructors ____________________________________________________________

WgMsgFunc::WgMsgFunc( void(*fp)( const WgMsg_p& pMsg) )
{
	m_callbackType = 0;
	m_pCallback = (void*) fp;	
}

WgMsgFunc::WgMsgFunc( void(*fp)( const WgMsg_p& pMsg, int param), int param )
{
	m_callbackType = 1;
	m_pCallback = (void*) fp;
	m_param = param;
}

WgMsgFunc::WgMsgFunc( void(*fp)( const WgMsg_p& pMsg, void * pParam), void * pParam )
{
	m_callbackType = 2;
	m_pCallback = (void*) fp;
	m_pParam = pParam;
}

WgMsgFunc::WgMsgFunc( void(*fp)( const WgMsg_p& pMsg, const WgObject_p& pParam), const WgObject_p& pParam )
{
	m_callbackType = 3;
	m_pCallback = (void*) fp;
	m_pParamObj = pParam;
	
}


//____ isInstanceOf() _________________________________________________________

bool WgMsgFunc::isInstanceOf( const char * pClassName ) const
{
	if( pClassName==CLASSNAME )
		return true;

	return WgReceiver::isInstanceOf(pClassName);
}

//____ className() ____________________________________________________________

const char * WgMsgFunc::className( void ) const
{
	return CLASSNAME;
}

//____ cast() _________________________________________________________________

WgMsgFunc_p WgMsgFunc::cast( const WgObject_p& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgMsgFunc_p( static_cast<WgMsgFunc*>(pObject.rawPtr()) );

	return 0;
}

//____ onMsg() _______________________________________________________________

void WgMsgFunc::onMsg( const WgMsg_p& pMsg )
{
	switch( m_callbackType )
	{
		case 0:
			((void(*)( const WgMsg_p&))m_pCallback)(pMsg);
			break;
		case 1:
			((void(*)( const WgMsg_p&, int ))m_pCallback)(pMsg,m_param);
			break;
		case 2:
			((void(*)( const WgMsg_p&, void* ))m_pCallback)(pMsg,m_pParam);
			break;
		case 3:
			((void(*)( const WgMsg_p&, const WgObject_p& ))m_pCallback)(pMsg,m_pParamObj);
			break;
		default:
			break;
	}
}

//____ _onRouteAdded() _________________________________________________________

void  WgMsgFunc::_onRouteAdded()
{	
	_incRefCount();
}

//____ _onRouteRemoved() _______________________________________________________

void  WgMsgFunc::_onRouteRemoved()
{
	_decRefCount();
}
