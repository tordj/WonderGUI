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

#ifndef WG_EVENTCATCHER_DOT_H
#define WG_EVENTCATCHER_DOT_H

#ifndef WG_EVENTLISTENER_DOT_H
#	include <wg_eventlistener.h>
#endif

class WgEventCatcher;
typedef	WgStrongPtr<WgEventCatcher,WgEventListenerPtr>		WgEventCatcherPtr;
typedef	WgWeakPtr<WgEventCatcher,WgEventListenerWeakPtr>	WgEventCatcherWeakPtr;

class WgEventCatcher : public WgEventListener
{
public:
	static WgEventCatcherPtr	Create( void(*fp)( const WgEventPtr& pEvent) ) { return new WgEventCatcher( fp ); }
	static WgEventCatcherPtr	Create( void(*fp)( const WgEventPtr& pEvent, int param), int param ) { return new WgEventCatcher( fp, param ); }
	static WgEventCatcherPtr	Create( void(*fp)( const WgEventPtr& pEvent, void * pParam), void * pParam ) { return new WgEventCatcher( fp, pParam ); }
	static WgEventCatcherPtr	Create( void(*fp)( const WgEventPtr& pEvent, const WgObjectPtr& pParam), const WgObjectPtr& pParam ) { return new WgEventCatcher( fp, pParam ); }

	bool						IsInstanceOf( const char * pClassName ) const;
	const char *				ClassName( void ) const;
	static const char			CLASSNAME[];
	static WgEventCatcherPtr	Cast( const WgObjectPtr& pObject );

	void OnEvent( const WgEventPtr& pEvent );

protected:
	WgEventCatcher( void(*fp)( const WgEventPtr& pEvent) );
	WgEventCatcher( void(*fp)( const WgEventPtr& pEvent, int param), int param );
	WgEventCatcher( void(*fp)( const WgEventPtr& pEvent, void * pParam), void * pParam );
	WgEventCatcher( void(*fp)( const WgEventPtr& pEvent, const WgObjectPtr& pParam), const WgObjectPtr& pParam );

	void	_onRouteAdded();
	void	_onRouteRemoved();

	int			m_callbackType;		// 0 = no params, 1 = int param, 2 = void * param, 3 = object.
	void *		m_pCallback;
	int			m_param;
	void *		m_pParam;
	WgObjectPtr	m_pParamObj;


};



#endif //WG_EVENTCATCHER_DOT_H
