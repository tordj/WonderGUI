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

#include <deque>
#include <map>
#include <vector>

#ifndef WG_EVENT_DOT_H
#	include <wg_event.h>
#endif

#ifndef WG_KEY_DOT_H
#	include <wg_key.h>
#endif

class WgRoot;

class WgEventHandler
{
public:
	WgEventHandler( int64_t startTime, WgRoot * pRoot );
	~WgEventHandler();

	bool QueueEvent( const WgEvent::Event& _event );

	void ProcessEvents();


/*
	bool AddCallback( WgEventId eventId, WgGizmo * pEmitter, void(*fp)(void*), void * pObj );
	bool AddCallback( const WgSignal::Signal_bool& signal, WgEmitter * pEmitter, void(*fp)(void*,bool), void * pObj );
	bool AddCallback( const WgSignal::Signal_int& signal, WgEmitter * pEmitter, void(*fp)(void*,int), void * pObj );
	bool AddCallback( const WgSignal::Signal_float& signal, WgEmitter * pEmitter, void(*fp)(void*,float), void * pObj );
*/


private:

	void	CompleteEvent( WgEvent::Event& _event );
	void	ProcessEventInternal( WgEvent::Event& _event );

	void	ProcessTimePass( WgEvent::TimePass * pEvent );
	void	ProcessPointerMove( WgEvent::PointerMove * pEvent );
	void	ProcessEndPointerMove( WgEvent::EndPointerMove * pEvent );
	void	ProcessButtonPress( WgEvent::ButtonPress * pEvent );
	void	ProcessButtonRelease( WgEvent::ButtonRelease * pEvent );
	void	ProcessButtonDrag( WgEvent::ButtonDrag * pEvent );

	//

	WgRoot *		m_pRoot;

	std::deque<WgEvent::Event>				m_eventQueue;
	bool									m_bIsProcessing;	// Set when we are inside ProcessEvents().
	std::deque<WgEvent::Event>::iterator	m_insertPos;		// Position where we insert events being queued when processing.

	int64_t		m_time;
	WgCord			m_pointerPos;
	WgModifierKeys	m_modKeys;

	// Settings for keyboard/pointer input

	int				m_doubleClickTimeTreshold;		// Maximum millseconds between first and second press to count as a doubleclick.
	int				m_doubleClickDistanceTreshold;	// Maximum distance the pointer may move between first and second press to count as a doubleclick.

	int				m_buttonRepeatDelay;
	int				m_buttonRepeatRate;

	int				m_keyRepeatDelay;
	int				m_keyRepeatRate;


	std::map<int,WgKey>	m_keycodeMap;		// Maps native keycodes to WgKey.

	// Current mouse state

	std::vector<WgGizmoWeakPtr>	m_vMarkedWidgets;	// Widgets the pointer currently is "inside".

	// Current button states

	WgEvent::Event *		m_pLatestButtonEvents[WG_MAX_BUTTONS];	// Pointer at event object in either m_latestPress or m_latestRelease.

	WgEvent::ButtonPress	m_latestPress[WG_MAX_BUTTONS];			// Saved info for the last time each button was pressed.
	WgEvent::ButtonRelease	m_latestRelease[WG_MAX_BUTTONS];		// Saved info for the last time each button was released.

	std::vector<WgGizmoWeakPtr>	m_latestPressWidgets[WG_MAX_BUTTONS];	// List of widgets who received the latest press, for each button.

	// Current keyboard state



};




#endif //WG_EVENTHANDLER_DOT_H
