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

#ifndef WG_MSGFUNC_DOT_H
#define WG_MSGFUNC_DOT_H
#pragma once

#include <wg_receiver.h>

namespace wg 
{
	
	class MsgFunc;
	typedef	StrongPtr<MsgFunc,Receiver_p>		MsgFunc_p;
	typedef	WeakPtr<MsgFunc,Receiver_wp>	MsgFunc_wp;
	
	class MsgFunc : public Receiver
	{
	public:
		static MsgFunc_p	create( void(*fp)( const Msg_p& pMsg) ) { return new MsgFunc( fp ); }
		static MsgFunc_p	create( void(*fp)( const Msg_p& pMsg, int param), int param ) { return new MsgFunc( fp, param ); }
		static MsgFunc_p	create( void(*fp)( const Msg_p& pMsg, void * pParam), void * pParam ) { return new MsgFunc( fp, pParam ); }
		static MsgFunc_p	create( void(*fp)( const Msg_p& pMsg, const Object_p& pParam), const Object_p& pParam ) { return new MsgFunc( fp, pParam ); }
	
		bool						isInstanceOf( const char * pClassName ) const;
		const char *				className( void ) const;
		static const char			CLASSNAME[];
		static MsgFunc_p	cast( const Object_p& pObject );
	
		void receive( const Msg_p& pMsg );
	
	protected:
		MsgFunc( void(*fp)( const Msg_p& pMsg) );
		MsgFunc( void(*fp)( const Msg_p& pMsg, int param), int param );
		MsgFunc( void(*fp)( const Msg_p& pMsg, void * pParam), void * pParam );
		MsgFunc( void(*fp)( const Msg_p& pMsg, const Object_p& pParam), const Object_p& pParam );
	
		void	_onRouteAdded();
		void	_onRouteRemoved();
	
		int			m_callbackType;		// 0 = no params, 1 = int param, 2 = void * param, 3 = object.
		void *		m_pCallback;
		int			m_param;
		void *		m_pParam;
		Object_p	m_pParamObj;
	
	
	};
	
	
	

} // namespace wg
#endif //WG_MSGFUNC_DOT_H
