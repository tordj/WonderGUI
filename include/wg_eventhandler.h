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

#ifndef WG_EVENTHANDLER_DOT_H
#define WG_EVENTHANDLER_DOT_H

#include <vector>

#ifndef WG_EVENT_DOT_H
#	include <wg_event.h>
#endif

class WgEventHandler
{
public:
	WgEventHandler( Int64 startTime );
	~WgEventHandler();

	bool QueueEvent( const WgEvent::EventBase& _event );

	void ProcessEvents();

	
/*
	bool AddCallback( WgEventId eventId, WgGizmo * pEmitter, void(*fp)(void*), void * pObj );
	bool AddCallback( const WgSignal::Signal_bool& signal, WgEmitter * pEmitter, void(*fp)(void*,bool), void * pObj );
	bool AddCallback( const WgSignal::Signal_int& signal, WgEmitter * pEmitter, void(*fp)(void*,int), void * pObj );
	bool AddCallback( const WgSignal::Signal_float& signal, WgEmitter * pEmitter, void(*fp)(void*,float), void * pObj );
*/


private:


	std::vector<WgEvent>	m_eventQueue;

	Int64					m_time;

};




#endif //WG_EVENTHANDLER_DOT_H