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

namespace wg 
{
	
	const char MsgFunc::CLASSNAME[] = {"MsgFunc"};
	
	
	//____ Constructors ____________________________________________________________
	
	MsgFunc::MsgFunc( void(*fp)( const Msg_p& pMsg) )
	{
		m_callbackType = 0;
		m_pCallback = (void*) fp;	
	}
	
	MsgFunc::MsgFunc( void(*fp)( const Msg_p& pMsg, int param), int param )
	{
		m_callbackType = 1;
		m_pCallback = (void*) fp;
		m_param = param;
	}
	
	MsgFunc::MsgFunc( void(*fp)( const Msg_p& pMsg, void * pParam), void * pParam )
	{
		m_callbackType = 2;
		m_pCallback = (void*) fp;
		m_pParam = pParam;
	}
	
	MsgFunc::MsgFunc( void(*fp)( const Msg_p& pMsg, const Object_p& pParam), const Object_p& pParam )
	{
		m_callbackType = 3;
		m_pCallback = (void*) fp;
		m_pParamObj = pParam;
		
	}
	
	
	//____ isInstanceOf() _________________________________________________________
	
	bool MsgFunc::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;
	
		return Receiver::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * MsgFunc::className( void ) const
	{
		return CLASSNAME;
	}
	
	//____ cast() _________________________________________________________________
	
	MsgFunc_p MsgFunc::cast( const Object_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return MsgFunc_p( static_cast<MsgFunc*>(pObject.rawPtr()) );
	
		return 0;
	}
	
	//____ onMsg() _______________________________________________________________
	
	void MsgFunc::onMsg( const Msg_p& pMsg )
	{
		switch( m_callbackType )
		{
			case 0:
				((void(*)( const Msg_p&))m_pCallback)(pMsg);
				break;
			case 1:
				((void(*)( const Msg_p&, int ))m_pCallback)(pMsg,m_param);
				break;
			case 2:
				((void(*)( const Msg_p&, void* ))m_pCallback)(pMsg,m_pParam);
				break;
			case 3:
				((void(*)( const Msg_p&, const Object_p& ))m_pCallback)(pMsg,m_pParamObj);
				break;
			default:
				break;
		}
	}
	
	//____ _onRouteAdded() _________________________________________________________
	
	void  MsgFunc::_onRouteAdded()
	{	
		_incRefCount();
	}
	
	//____ _onRouteRemoved() _______________________________________________________
	
	void  MsgFunc::_onRouteRemoved()
	{
		_decRefCount();
	}

} // namespace wg
