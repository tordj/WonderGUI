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
typedef	WgStrongPtr<WgMsgFunc,WgReceiver_p>		WgMsgFunc_p;
typedef	WgWeakPtr<WgMsgFunc,WgReceiver_wp>	WgMsgFunc_wp;

class WgMsgFunc : public WgReceiver
{
public:
	static WgMsgFunc_p	create( void(*fp)( const WgMsg_p& pMsg) ) { return new WgMsgFunc( fp ); }
	static WgMsgFunc_p	create( void(*fp)( const WgMsg_p& pMsg, int param), int param ) { return new WgMsgFunc( fp, param ); }
	static WgMsgFunc_p	create( void(*fp)( const WgMsg_p& pMsg, void * pParam), void * pParam ) { return new WgMsgFunc( fp, pParam ); }
	static WgMsgFunc_p	create( void(*fp)( const WgMsg_p& pMsg, const WgObject_p& pParam), const WgObject_p& pParam ) { return new WgMsgFunc( fp, pParam ); }

	bool						isInstanceOf( const char * pClassName ) const;
	const char *				className( void ) const;
	static const char			CLASSNAME[];
	static WgMsgFunc_p	cast( const WgObject_p& pObject );

	void onMsg( const WgMsg_p& pMsg );

protected:
	WgMsgFunc( void(*fp)( const WgMsg_p& pMsg) );
	WgMsgFunc( void(*fp)( const WgMsg_p& pMsg, int param), int param );
	WgMsgFunc( void(*fp)( const WgMsg_p& pMsg, void * pParam), void * pParam );
	WgMsgFunc( void(*fp)( const WgMsg_p& pMsg, const WgObject_p& pParam), const WgObject_p& pParam );

	void	_onRouteAdded();
	void	_onRouteRemoved();

	int			m_callbackType;		// 0 = no params, 1 = int param, 2 = void * param, 3 = object.
	void *		m_pCallback;
	int			m_param;
	void *		m_pParam;
	WgObject_p	m_pParamObj;


};



#endif //WG_MSGFUNC_DOT_H
