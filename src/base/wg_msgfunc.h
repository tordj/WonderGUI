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

#ifndef WG_RECEIVER_DOT_H
#	include <wg_receiver.h>
#endif

class WgMsgFunc;
typedef	WgStrongPtr<WgMsgFunc,WgReceiverPtr>		WgMsgFuncPtr;
typedef	WgWeakPtr<WgMsgFunc,WgReceiverWeakPtr>	WgMsgFuncWeakPtr;

class WgMsgFunc : public WgReceiver
{
public:
	static WgMsgFuncPtr	create( void(*fp)( const WgMsgPtr& pMsg) ) { return new WgMsgFunc( fp ); }
	static WgMsgFuncPtr	create( void(*fp)( const WgMsgPtr& pMsg, int param), int param ) { return new WgMsgFunc( fp, param ); }
	static WgMsgFuncPtr	create( void(*fp)( const WgMsgPtr& pMsg, void * pParam), void * pParam ) { return new WgMsgFunc( fp, pParam ); }
	static WgMsgFuncPtr	create( void(*fp)( const WgMsgPtr& pMsg, const WgObjectPtr& pParam), const WgObjectPtr& pParam ) { return new WgMsgFunc( fp, pParam ); }

	bool						isInstanceOf( const char * pClassName ) const;
	const char *				className( void ) const;
	static const char			CLASSNAME[];
	static WgMsgFuncPtr	cast( const WgObjectPtr& pObject );

	void onMsg( const WgMsgPtr& pMsg );

protected:
	WgMsgFunc( void(*fp)( const WgMsgPtr& pMsg) );
	WgMsgFunc( void(*fp)( const WgMsgPtr& pMsg, int param), int param );
	WgMsgFunc( void(*fp)( const WgMsgPtr& pMsg, void * pParam), void * pParam );
	WgMsgFunc( void(*fp)( const WgMsgPtr& pMsg, const WgObjectPtr& pParam), const WgObjectPtr& pParam );

	void	_onRouteAdded();
	void	_onRouteRemoved();

	int			m_callbackType;		// 0 = no params, 1 = int param, 2 = void * param, 3 = object.
	void *		m_pCallback;
	int			m_param;
	void *		m_pParam;
	WgObjectPtr	m_pParamObj;


};



#endif //WG_MSGFUNC_DOT_H
