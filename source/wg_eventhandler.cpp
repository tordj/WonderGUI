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

#include <assert.h>
#include <wg_event.h>
#include <wg_eventfilter.h>
#include <wg_eventhandler.h>
#include <wg_root.h>
#include <wg_gizmo_container.h>

//____ Constructor ____________________________________________________________

WgEventHandler::WgEventHandler( int64_t startTime, WgRoot * pRoot )
{
	m_pRoot					= pRoot;
	m_time					= startTime;
	m_modKeys				= WG_MODKEY_NONE;

	m_doubleClickTimeTreshold		= 250;
	m_doubleClickDistanceTreshold	= 2;

	m_buttonRepeatDelay		= 300;
	m_buttonRepeatRate		= 200;

	m_keyRepeatDelay		= 300;
	m_keyRepeatRate			= 150;

	m_bIsProcessing			= false;
	m_bWindowFocus			= true;

	for( int i = 0 ; i <= WG_MAX_BUTTONS ; i++ )
	{
		m_bButtonPressed[i] = 0;
		m_pLatestPressEvents[i] = 0;
		m_pLatestReleaseEvents[i] = 0;
	}
}

//____ Destructor _____________________________________________________________

WgEventHandler::~WgEventHandler()
{
}

//____ MapKey() _______________________________________________________________

void WgEventHandler::MapKey( WgKey translated_keycode, int native_keycode )
{
	m_keycodeMap[native_keycode] = translated_keycode;
}


//____ UnmapKey() _____________________________________________________________

void WgEventHandler::UnmapKey( WgKey translated_keycode )
{
	std::map<int,WgKey>::iterator it = m_keycodeMap.begin();

	while( it != m_keycodeMap.end() )
	{
		if( it->second == translated_keycode )
		{
			std::map<int,WgKey>::iterator it2 = it++;
			m_keycodeMap.erase(it2);
		}
		else
			++it;
	}
}

//____ ClearKeyMap() __________________________________________________________

void WgEventHandler::ClearKeyMap()
{
	m_keycodeMap.clear();
}

//____ SetButtonRepeat() ______________________________________________________

bool WgEventHandler::SetButtonRepeat( int delay, int rate )
{
	if( delay <= 0 || rate <= 0 )
		return false;

	m_buttonRepeatDelay	= delay;
	m_buttonRepeatRate	= rate;
	return true;
}

//____ SetKeyRepeat() _________________________________________________________

bool WgEventHandler::SetKeyRepeat( int delay, int rate )
{
	if( delay <= 0 || rate <= 0 )
		return false;

	m_keyRepeatDelay	= delay;
	m_keyRepeatRate		= rate;
	return true;
}

//____ SetFocusGroup() ________________________________________________________

bool WgEventHandler::SetFocusGroup( WgGizmoContainer * pFocusGroup )
{
	// Sanity checks

	if( pFocusGroup )
	{
		if( pFocusGroup->CastToGizmo() == m_keyFocusGroup.GetRealPtr() )
			return true;									// Not an error, but we don't need to do anything

		if( !pFocusGroup->IsFocusGroup() )
			return false;									// Container is not a focus group

		if( !pFocusGroup->CastToGizmo()->Hook() || pFocusGroup->CastToGizmo()->Hook()->Root() != m_pRoot )
			return false;									// pFocusGroup is not a child of our root.
	}

	// Set new focus gizmo as specified by group

	WgGizmoWeakPtr pNewFocusGizmo;

	if( pFocusGroup )
		if( m_focusGroupMap.find(pFocusGroup->CastToGizmo()) != m_focusGroupMap.end() )
			pNewFocusGizmo = m_focusGroupMap[pFocusGroup->CastToGizmo()];

	if( m_keyFocusGizmo )
		m_keyFocusGizmo->_onLostInputFocus();

	if( pNewFocusGizmo )
		pNewFocusGizmo->_onGotInputFocus();

	// Set members and exit

	m_keyFocusGizmo = pNewFocusGizmo;
	m_keyFocusGroup = pFocusGroup->CastToGizmo();

	return true;
}

//____ SetKeyboardFocus() _____________________________________________________

bool WgEventHandler::SetKeyboardFocus( WgGizmo * pFocus )
{
	// Return if Gizmo is not child of our root.

	if( pFocus && (!pFocus->Hook() || pFocus->Hook()->Root() != m_pRoot) )
		return false;

	// Handle old focus.

	WgGizmo * pOldFocus = m_keyFocusGizmo.GetRealPtr();

	if( pFocus == pOldFocus )
		return true;

	if( pOldFocus )
		pOldFocus->_onLostInputFocus();

	// Handle new focus, possibly switching focus group.

	if( pFocus )
	{
		// Check what focus group (if any) this Gizmo belongs to.

		WgGizmoContainer * p = pFocus->ParentX();
		while( p && !p->IsFocusGroup() )
			p = p->CastToGizmo()->ParentX();

		if( p )
			m_keyFocusGroup = p->CastToGizmo();
		else
			m_keyFocusGroup = 0;

		// Activate focus

		pFocus->_onGotInputFocus();
	}

	// Set members and exit.

	m_keyFocusGizmo = pFocus;
	m_focusGroupMap[m_keyFocusGroup] = pFocus;

	return true;
}

//____ FocusGroup() ___________________________________________________________

WgGizmoContainer * WgEventHandler::FocusGroup() const
{
	WgGizmo * pGizmo = m_keyFocusGroup.GetRealPtr();
	if( pGizmo )
		return pGizmo->CastToContainer();

	return 0;
}

//____ IsButtonPressed() _________________________________________________________

bool WgEventHandler::IsButtonPressed( int button )
{
	if( button >= 1 && button <= WG_MAX_BUTTONS )
		return m_bButtonPressed[button];

	return false;
}

//____ IsKeyPressed() ____________________________________________________________

bool WgEventHandler::IsKeyPressed( int native_keycode )
{
	for( unsigned int i = 0 ; i < m_keysDown.size() ; i++ )
		if( native_keycode == m_keysDown[i]->pEvent->NativeKeyCode() )
			return true;

	return false;
}



//____ AddCallback() __________________________________________________________

void WgEventHandler::AddCallback( void(*fp)(const WgEvent::Event * pEvent) )
{
	Callback * p = new FunctionCallback( WgEventFilter(), fp );
	m_globalCallbacks.PushBack( p );
}

void WgEventHandler::AddCallback( void(*fp)(const WgEvent::Event * pEvent, void * pParam), void * pParam )
{
	Callback * p = new FunctionCallbackParam( WgEventFilter(), fp, pParam );
	m_globalCallbacks.PushBack( p );
}

void WgEventHandler::AddCallback( void(*fp)(const WgEvent::Event * pEvent, WgGizmo * pDest), WgGizmo * pDest )
{
	Callback * p = new GizmoCallback( WgEventFilter(), fp, pDest );
	m_globalCallbacks.PushBack( p );
}

void WgEventHandler::AddCallback( WgEventListener * pListener )
{
	Callback * p = new ListenerCallback( WgEventFilter(), pListener );
	m_globalCallbacks.PushBack( p );
}

void WgEventHandler::AddCallback( const WgEventFilter& filter, void(*fp)(const WgEvent::Event * pEvent) )
{
	Callback * p = new FunctionCallback( filter, fp );
	_addCallback( filter, p );
}

void WgEventHandler::AddCallback( const WgEventFilter& filter, void(*fp)(const WgEvent::Event * pEvent, void * pParam), void * pParam )
{
	Callback * p = new FunctionCallbackParam( filter, fp, pParam );
	_addCallback( filter, p );
}

void WgEventHandler::AddCallback( const WgEventFilter& filter, void(*fp)(const WgEvent::Event * pEvent, WgGizmo * pDest), WgGizmo * pDest )
{
	Callback * p = new GizmoCallback( filter, fp, pDest );
	_addCallback( filter, p );
}

void WgEventHandler::AddCallback( const WgEventFilter& filter, WgEventListener * pListener )
{
	Callback * p = new ListenerCallback( filter, pListener );
	_addCallback( filter, p );
}

//____ DeleteCallbacksTo() _______________________________________________________

int WgEventHandler::DeleteCallbacksTo( const WgGizmo * pGizmo )
{
	// Delete all callbacks with the specified Gizmo as recipient.

	return _deleteCallbacksTo(pGizmo);
}

int WgEventHandler::DeleteCallbacksTo( void(*fp)( const WgEvent::Event * pEvent) )
{
	return _deleteCallbacksTo( reinterpret_cast<void*>(fp) );
}

int WgEventHandler::DeleteCallbacksTo( void(*fp)( const WgEvent::Event * pEvent, void * pParam) )
{
	return _deleteCallbacksTo( reinterpret_cast<void*>(fp) );
}

int WgEventHandler::DeleteCallbacksTo( const WgEventListener * pListener )
{
	return _deleteCallbacksTo(pListener);
}

//____ DeleteCallbacksOn() _______________________________________________________

int WgEventHandler::DeleteCallbacksOn( const WgGizmo * pGizmo )
{
	std::map<WgGizmoWeakPtr,WgChain<Callback> >::iterator it = m_gizmoCallbacks.find(WgGizmoWeakPtr( const_cast<WgGizmo*>(pGizmo) ));

	if( it == m_gizmoCallbacks.end() )
		return 0;

	int nDeleted = it->second.Size();
	m_gizmoCallbacks.erase(it);
	return nDeleted;
}

int WgEventHandler::DeleteCallbacksOn( WgEventType type )
{
	// Delete global callbacks on this event type

	int nDeleted = _deleteCallbacksOnType( type, &m_globalCallbacks );

	// Delete Gizmo-specific callbacks on this event type

	std::map<WgGizmoWeakPtr,WgChain<Callback> >::iterator it = m_gizmoCallbacks.begin();
	while( it != m_gizmoCallbacks.end() )
	{
		nDeleted += _deleteCallbacksOnType( type, &it->second );

		if( it->second.Size() == 0 )
			m_gizmoCallbacks.erase(it++);
		else
			++it;
	}
	return nDeleted;
}




int WgEventHandler::DeleteCallbacksOn( const WgGizmo * pGizmo, WgEventType type )
{
	std::map<WgGizmoWeakPtr,WgChain<Callback> >::iterator it = m_gizmoCallbacks.find( WgGizmoWeakPtr(const_cast<WgGizmo*>(pGizmo)));

	if( it == m_gizmoCallbacks.end() )
		return 0;

	int nDeleted = 0;
	Callback * p = it->second.First();
	while( p )
	{
		if( p->Filter().EventType() == type )
		{
			Callback * pNext = p->Next();
			delete p;
			nDeleted++;
			p = pNext;
		}
		else
			p = p->Next();
	}

	if( it->second.Size() == 0 )
		m_gizmoCallbacks.erase(it);

	return nDeleted;
}





//____ DeleteCallbacks() ______________________________________________________

int WgEventHandler::DeleteCallback( const WgEventFilter& filter,const  WgGizmo * pGizmo )
{
	return _deleteCallback( filter, pGizmo );
}

int WgEventHandler::DeleteCallback( const WgEventFilter& filter, const WgEventListener * pListener )
{
	return _deleteCallback( filter, pListener );
}

int WgEventHandler::DeleteCallback( const WgEventFilter& filter, void(*fp)( const WgEvent::Event * pEvent) )
{
	return _deleteCallback( filter, (const void *) fp );
}

int WgEventHandler::DeleteCallback( const WgEventFilter& filter, void(*fp)( const WgEvent::Event * pEvent, void * pParam) )
{
	return _deleteCallback( filter, (const void *) fp );
}



//____ DeleteAllCallbacks() ___________________________________________________

int WgEventHandler::DeleteAllCallbacks()
{
	m_globalCallbacks.Clear();
	m_gizmoCallbacks.clear();
	return false;
}

//____ DeleteDeadCallbacks() __________________________________________________

int WgEventHandler::DeleteDeadCallbacks()
{
	int nDeleted = 0;

	// Delete any dead global callbacks

	Callback * p = m_globalCallbacks.First();
	while( p )
	{
		if( p->IsAlive() )
			p = p->Next();
		else
		{
			Callback * pNext = p->Next();
			delete p;
			nDeleted++;
			p = pNext;
		}
	}

	// Delete any dead Gizmo-specific callbacks.
	// These can be dead by either sender or receiver having been deleted.

	std::map<WgGizmoWeakPtr,WgChain<Callback> >::iterator it = m_gizmoCallbacks.begin();

	while( it != m_gizmoCallbacks.end() )
	{
		if( !it->first )
		{
			nDeleted += it->second.Size();
			m_gizmoCallbacks.erase(it++);		// Sender is dead, delete whole branch of callbacks.
		}
		else
		{
			Callback * p = it->second.First();
			while( p )
			{
				if( p->IsAlive() )
					p = p->Next();
				else
				{
					Callback * pNext = p->Next();
					delete p;					// Receiver is dead, delete callback.
					nDeleted++;
					p = pNext;
				}
			}
			++it;
		}
	}

	return nDeleted;
}


//____ _addCallback() _________________________________________________________

void WgEventHandler::_addCallback( const WgEventFilter& filter, Callback * pCallback )
{
	if( filter.FiltersGizmo() )
	{
		WgGizmoWeakPtr pGizmo = filter.Gizmo();

		WgChain<Callback>*	pChain = &m_gizmoCallbacks[pGizmo];

		pChain->PushBack(pCallback);
	}
	else
		m_globalCallbacks.PushBack( pCallback );
}


//____ _deleteCallbacksTo() __________________________________________________

int WgEventHandler::_deleteCallbacksTo( const void * pReceiver )
{
	int nDeleted = 0;

	// Delete any global callbacks for this receiver.

	Callback * p = m_globalCallbacks.First();
	while( p )
	{
		if( p->Receiver() != pReceiver )
			p = p->Next();
		else
		{
			Callback * pNext = p->Next();
			delete p;
			nDeleted++;
			p = pNext;
		}
	}

	// Delete any Gizmo-specific callbacks for this receiver.

	std::map<WgGizmoWeakPtr,WgChain<Callback> >::iterator it = m_gizmoCallbacks.begin();

	while( it != m_gizmoCallbacks.end() )
	{
		Callback * p = it->second.First();
		while( p )
		{
			if( p->Receiver() != pReceiver )
				p = p->Next();
			else
			{
				Callback * pNext = p->Next();
				delete p;					// Receiver is dead, delete callback.
				nDeleted++;
				p = pNext;
			}
		}
		++it;
	}

	return nDeleted;
}

//____ _deleteCallbacksOnType() _______________________________________________

int WgEventHandler::_deleteCallbacksOnType( WgEventType type, WgChain<Callback> * pChain )
{
	int nDeleted = 0;
	Callback * p = pChain->First();

	while( p )
	{

		if( p->Filter().EventType() == type )
		{
			Callback * pNext = p->Next();
			delete p;
			nDeleted++;
			p = pNext;
		}
		else
			p = p->Next();
	}
	return nDeleted;
}

//____ deleteCallback() ______________________________________________________

int WgEventHandler::_deleteCallback( const WgEventFilter& filter, const void * pReceiver )
{
	// Deletes all callbacks created with exactly the same filter settings and receiver.

	int nDeleted = 0;
	WgChain<Callback>*	pChain;

	if( filter.FiltersGizmo() )
	{
		std::map<WgGizmoWeakPtr,WgChain<Callback> >::iterator it = m_gizmoCallbacks.find(WgGizmoWeakPtr(filter.Gizmo()));

		if( it == m_gizmoCallbacks.end() )
			return 0;					// No callbacks for Gizmo

		pChain = &it->second;
	}
	else
	{
		pChain = &m_globalCallbacks;
	}

	// We now have the right chain and need to check all the callbacks.

	Callback * p = pChain->First();
	while( p )
	{
		if( filter.EventType() == p->Filter().EventType() && pReceiver == p->Receiver() )
		{
			Callback * pNext = p->Next();
			delete p;
			nDeleted++;
			p = pNext;
		}
		else
			p = p->Next();
	}

	if( pChain->Size() == 0 && pChain != &m_globalCallbacks )
	{
		std::map<WgGizmoWeakPtr,WgChain<Callback> >::iterator it = m_gizmoCallbacks.find(WgGizmoWeakPtr(filter.Gizmo()));
		m_gizmoCallbacks.erase(it);

	}

	return nDeleted;
}


//____ QueueEvent() ___________________________________________________________

bool WgEventHandler::QueueEvent( WgEvent::Event * pEvent )
{
	if( m_bIsProcessing )
	{
		// Events that are posted as a direct result of another event being processed
		// are inserted before the next event in the queue.
		// If two or more events are posted by the same event being processed,
		// they need to be queued in the order of posting.

		m_insertPos = m_eventQueue.insert( m_insertPos, pEvent );
		m_insertPos++;
	}
	else
	{
		// Events being posted outside processing loop are simply added to the
		// queue.

		m_eventQueue.push_back( pEvent );
	}

	return true;
}

//____ ProcessEvents() ________________________________________________________

void WgEventHandler::ProcessEvents()
{
	m_bIsProcessing = true;

	// First thing: we make sure that we know what Gizmos pointer is inside, in case that has changed.

	m_insertPos = m_eventQueue.begin();	// Insert any POINTER_ENTER/EXIT right at beginning.
	_updateMarkedGizmos(false);

	// Process all the events

	while( !m_eventQueue.empty() )
	{
		WgEvent::Event * pEvent = m_eventQueue.front();

		m_insertPos = m_eventQueue.begin()+1;	// Insert position set to right after current event.

		_finalizeEvent( pEvent );

		if( pEvent->IsForGizmo() )
		{
			WgGizmo * pGizmo = pEvent->Gizmo();
			if( pGizmo )
				pGizmo->_onEvent( pEvent, this );
		}
		else
		{
			_processGeneralEvent( pEvent );
		}
		_processEventCallbacks( pEvent );

		m_eventQueue.pop_front();

		// Delete event object unless its a BUTTON_PRESS, BUTTON_RELEASE or KEY_PRESS event for NO SPECIFIC GIZMO,
		// which are kept in m_pLatestPressEvents, m_pLatestReleaseEvents and m_keysDown respectively.

		if( pEvent->IsForGizmo() || (pEvent->Type() !=  WG_EVENT_BUTTON_PRESS &&
			pEvent->Type() != WG_EVENT_BUTTON_RELEASE && pEvent->Type() != WG_EVENT_KEY_PRESS) )
			delete pEvent;
	}

	m_bIsProcessing = false;
}

//____ _processEventCallbacks() ________________________________________________

void WgEventHandler::_processEventCallbacks( WgEvent::Event * pEvent )
{
	// Call all global callbacks

	Callback * pCallback = m_globalCallbacks.First();

	while( pCallback )
	{
		pCallback->ProcessEvent( pEvent );
		pCallback = pCallback->Next();
	}

	// Call all Gizmo-specific callbacks

	WgChain<Callback> * pChain = 0;

	if( pEvent->IsForGizmo() && pEvent->Gizmo() )
		pChain = &m_gizmoCallbacks[pEvent->GizmoWeakPtr()];
	else
		return;	// Event was for a Gizmo that now has disappeared.


	pCallback = pChain->First();

	while( pCallback )
	{
		pCallback->ProcessEvent( pEvent );
		pCallback = pCallback->Next();
	}
}


//____ _finalizeEvent() ________________________________________________________

void WgEventHandler::_finalizeEvent( WgEvent::Event * pEvent )
{
	// Fill in missing information in the event-class.

	pEvent->m_timestamp	= m_time;
	pEvent->m_modKeys	= m_modKeys;

	// Only global POINTER_ENTER & POINTER_MOVE events have these members
	// set, the rest needs to have them filled in.

	if( pEvent->IsForGizmo() || (pEvent->Type() != WG_EVENT_POINTER_MOVE && pEvent->Type() != WG_EVENT_POINTER_ENTER) )
	{
		pEvent->m_pointerScreenPos = m_pointerPos;
		pEvent->m_pointerLocalPos = m_pointerPos;

		if( pEvent->Gizmo() )
			pEvent->m_pointerLocalPos -= pEvent->Gizmo()->ScreenPos();
	}

	// Event specific finalizations

	switch( pEvent->Type() )
	{

		// Key events need translation of keycodes.

		case WG_EVENT_KEY_PRESS:
		case WG_EVENT_KEY_RELEASE:
		case WG_EVENT_KEY_REPEAT:
		{
			WgEvent::KeyEvent* p = static_cast<WgEvent::KeyEvent*>(pEvent);

			std::map<int,WgKey>::iterator it = m_keycodeMap.find(p->m_nativeKeyCode);
			if( it != m_keycodeMap.end() )
				p->m_translatedKeyCode = it->second;
		}
		break;


		default:
			break;
	}



}

//____ _processGeneralEvent() _________________________________________________

void WgEventHandler::_processGeneralEvent( WgEvent::Event * pEvent )
{

	switch( pEvent->Type() )
	{
		case WG_EVENT_FOCUS_GAINED:
			_processFocusGained( (WgEvent::FocusGained*) pEvent );
			break;

		case WG_EVENT_FOCUS_LOST:
			_processFocusLost( (WgEvent::FocusLost*) pEvent );
			break;

		case WG_EVENT_POINTER_ENTER:
			_processPointerEnter( (WgEvent::PointerEnter*) pEvent );
			break;

		case WG_EVENT_POINTER_MOVE:
			_processPointerMove( (WgEvent::PointerMove*) pEvent );
			break;

		case WG_EVENT_POINTER_PLACED:
			_processPointerPlaced( (WgEvent::PointerPlaced*) pEvent );
			break;

		case WG_EVENT_POINTER_EXIT:
			_processPointerExit( (WgEvent::PointerExit*) pEvent );
			break;

		case WG_EVENT_BUTTON_PRESS:
			_processButtonPress( (WgEvent::ButtonPress*) pEvent );
			break;

		case WG_EVENT_BUTTON_REPEAT:
			_processButtonRepeat( (WgEvent::ButtonRepeat*) pEvent );
			break;

		case WG_EVENT_BUTTON_DRAG:
			_processButtonDrag( (WgEvent::ButtonDrag*) pEvent );
			break;

		case WG_EVENT_BUTTON_RELEASE:
			_processButtonRelease( (WgEvent::ButtonRelease*) pEvent );
			break;

		case WG_EVENT_BUTTON_CLICK:
			_processButtonClick( (WgEvent::ButtonClick*) pEvent );
			break;

		case WG_EVENT_BUTTON_DOUBLECLICK:
			_processButtonDoubleClick( (WgEvent::ButtonDoubleClick*) pEvent );
			break;

		case WG_EVENT_KEY_PRESS:
			_processKeyPress( (WgEvent::KeyPress*) pEvent );
			break;

		case WG_EVENT_KEY_REPEAT:
			_processKeyRepeat( (WgEvent::KeyRepeat*) pEvent );
			break;

		case WG_EVENT_KEY_RELEASE:
			_processKeyRelease( (WgEvent::KeyRelease*) pEvent );
			break;

		case WG_EVENT_CHARACTER:
			_processCharacter( (WgEvent::Character*) pEvent );
			break;

		case WG_EVENT_WHEEL_ROLL:
			_processWheelRoll( (WgEvent::WheelRoll*) pEvent );
			break;

		case WG_EVENT_TICK:
			_processTick( (WgEvent::Tick*) pEvent );
			break;

		case WG_EVENT_DUMMY:
			break;

		case WG_EVENT_MAX:
			assert(false);								// NOT ALLOWED!
			break;
	}
}

//____ _processTick() ______________________________________________________

void WgEventHandler::_processTick( WgEvent::Tick * pEvent )
{
	// Check if we need to post BUTTON_REPEAT

	for( int button = 0 ; button <= WG_MAX_BUTTONS ; button++ )
	{
		if( m_bButtonPressed[button] )
		{
			int msSinceRepeatStart = (int) (m_time - m_pLatestPressEvents[button]->Timestamp() - m_buttonRepeatDelay );

			// First BUTTON_REPEAT event posted separately.

			if( msSinceRepeatStart < 0 && msSinceRepeatStart + pEvent->Millisec() >= 0 )
				QueueEvent( new WgEvent::ButtonRepeat(button) );

			// Calculate ms since last BUTTON_REPEAT event

			int msToProcess;
			if( msSinceRepeatStart < 0 )
				msToProcess = msSinceRepeatStart + pEvent->Millisec();
			else
				msToProcess = (msSinceRepeatStart % m_buttonRepeatRate) + pEvent->Millisec();

			// Post the amount of BUTTON_REPEAT that should be posted.

			while( msToProcess >= m_buttonRepeatRate )
			{
				QueueEvent( new WgEvent::ButtonRepeat(button) );
				msToProcess -= m_buttonRepeatRate;
			}
		}
	}

	// Check if we need to post KEY_REPEAT

	for( unsigned int i = 0 ; i < m_keysDown.size() ; i++ )
	{
		KeyDownInfo * pInfo = m_keysDown[i];
		int timePassed = (int) (pInfo->pEvent->Timestamp() - pEvent->Timestamp());

		int fraction = 0;

		if( timePassed < m_keyRepeatDelay )
			fraction = (timePassed - m_keyRepeatDelay) + m_keyRepeatRate;
		else
			fraction = (timePassed - m_keyRepeatDelay)%m_keyRepeatRate;

		fraction += pEvent->Millisec();

		while( fraction >= m_keyRepeatRate )
		{
			QueueEvent( new WgEvent::KeyRepeat( pInfo->pEvent->NativeKeyCode() ) );
			fraction -= m_keyRepeatRate;
		}
	}


	// Increase time counter

	m_time += pEvent->Millisec();
}


//____ _processFocusGained() __________________________________________________

void WgEventHandler::_processFocusGained( WgEvent::FocusGained * pEvent )
{
	if( !m_bWindowFocus && m_keyFocusGizmo )
		m_keyFocusGizmo.GetRealPtr()->_onGotInputFocus();

	m_bWindowFocus = true;
}

//____ _processFocusLost() ____________________________________________________

void WgEventHandler::_processFocusLost( WgEvent::FocusLost * pEvent )
{
	if( m_bWindowFocus && m_keyFocusGizmo )
		m_keyFocusGizmo.GetRealPtr()->_onLostInputFocus();

	m_bWindowFocus = false;
}


//____ _processPointerEnter() __________________________________________________

void WgEventHandler::_processPointerEnter( WgEvent::PointerEnter * pEvent )
{
	// Post events for button drag

	for( int i = 0 ; i <= WG_MAX_BUTTONS ; i++ )
	{
		if( m_bButtonPressed[i] )
			QueueEvent( new WgEvent::ButtonDrag( i, m_pLatestPressEvents[i]->PointerPos(), m_pointerPos, pEvent->PointerPos() ) );
	}

	// Post event for finalizing position once button drag is taken care of.

	QueueEvent( new WgEvent::PointerPlaced() );

	// Update pointer position

	m_pointerPos = pEvent->PointerPos();
}

//____ _processPointerExit() ___________________________________________________

void WgEventHandler::_processPointerExit( WgEvent::PointerExit * pEvent )
{
	// Post POINTER_EXIT events for all gizmos we had marked

	for( size_t i = 0 ; i < m_vMarkedGizmos.size() ; i++ )
	{
		WgGizmo * pGizmo = m_vMarkedGizmos[i].GetRealPtr();

		if( pGizmo )
			QueueEvent( new WgEvent::PointerExit( pGizmo ) );
	}

	m_vMarkedGizmos.clear();
	m_vModalGizmos.clear();
}


//____ _processPointerMove() ___________________________________________________

void WgEventHandler::_processPointerMove( WgEvent::PointerMove * pEvent )
{
	// Post events for button drag

	for( int i = 0 ; i <= WG_MAX_BUTTONS ; i++ )
	{
		if( m_bButtonPressed[i] )
			QueueEvent( new WgEvent::ButtonDrag( i, m_pLatestPressEvents[i]->PointerPos(), m_pointerPos, pEvent->PointerPos() ) );
	}

	// Post event for finalizing move once button drag is taken care of.

	QueueEvent( new WgEvent::PointerPlaced() );

	// Update pointer position

	m_pointerPos = pEvent->PointerPos();
}

//____ _processPointerPlaced() _________________________________________________

void WgEventHandler::_processPointerPlaced( WgEvent::PointerPlaced * pEvent )
{
	_updateMarkedGizmos(true);

}

//____ _updateMarkedGizmos() _______________________________________________

void WgEventHandler::_updateMarkedGizmos(bool bPostPointerMoveEvents)
{
	std::vector<WgGizmo*>	vNowMarked;

	WgGizmo * pGizmoPointed = m_pRoot->FindGizmo( m_pointerPos, WG_SEARCH_MARKPOLICY );
	WgGizmo * pGizmoTarget = m_pRoot->FindGizmo( m_pointerPos, WG_SEARCH_ACTION_TARGET );


	// Collect gizmos we now are inside (if we aren't outside our modal gizmo).

	if( pGizmoPointed == pGizmoTarget )
	{
		WgGizmo * pGizmo = pGizmoPointed;
		while( pGizmo )
		{
			vNowMarked.push_back(pGizmo);

			WgGizmoContainer * p = pGizmo->Hook()->Parent();

			if( p )
				pGizmo = p->CastToGizmo();
			else
				pGizmo = 0;
		}
	}

	// Post POINTER_EXIT events for gizmos no longer marked

	for( size_t i = 0 ; i < m_vMarkedGizmos.size() ; i++ )
	{
		WgGizmo * pGizmo = m_vMarkedGizmos[i].GetRealPtr();

		size_t j = 0;
		while( j < vNowMarked.size() )
		{
			if( pGizmo == vNowMarked[j] )
				break;
			j++;
		}

		if( j == vNowMarked.size() )
			QueueEvent( new WgEvent::PointerExit( pGizmo ) );
	}

	// Post POINTER_ENTER events for new marked gizmos
	// and POINTER_MOVE events for those already marked

	for( size_t i = 0 ; i < vNowMarked.size() ; i++ )
	{
		WgGizmo * pGizmo = vNowMarked[i];

		size_t j = 0;
		while( j < m_vMarkedGizmos.size() )
		{
			if( pGizmo == m_vMarkedGizmos[j].GetRealPtr() )
				break;
			j++;
		}

		if( j == m_vMarkedGizmos.size() )
			QueueEvent( new WgEvent::PointerEnter( pGizmo ) );
		else if( bPostPointerMoveEvents )
			QueueEvent( new WgEvent::PointerMove( pGizmo ) );
	}

	// Copy content of vNowMarked to m_vMarkedGizmos

	m_vMarkedGizmos.clear();
	for( size_t i = 0 ; i < vNowMarked.size() ; i++ )
		m_vMarkedGizmos.push_back( vNowMarked[i] );



	// **** HANDLING OF MODAL GIZMOS ****
	// This is a bit easier and more straight forward since:
	// a) We don't have pointer enter/exit events, just move.
	// b) Press/release outside do not neeed to be matched. We can get release outside
	//    even if press was inside and have a press outside that gets an release inside.
	//	  The modal gizmo still gets the corresponding event if it knows what to listen for.


	// Update m_vModalGizmos and queue outside modal events.

	m_vModalGizmos.clear();
	if( pGizmoTarget != pGizmoPointed )
	{
		WgGizmo * pGizmo = pGizmoTarget;
		while( pGizmo )
		{
			m_vModalGizmos.push_back(pGizmo);
			if( bPostPointerMoveEvents )
				QueueEvent( new WgEvent::PointerMoveOutsideModal(pGizmo) );

			WgGizmoContainer * p = pGizmo->Hook()->Parent();

			if( p )
				pGizmo = p->CastToGizmo();
			else
				pGizmo = 0;
		}
	}
}

//____ _processKeyPress() ______________________________________________________

void WgEventHandler::_processKeyPress( WgEvent::KeyPress * pEvent )
{
	// Sanity checks

	if( !m_keyFocusGizmo )
		return;

	// Fill in the info-structure.

	KeyDownInfo * pInfo = new KeyDownInfo();
	pInfo->pEvent = pEvent;

	// Post KEY_PRESS events for gizmos and remember which ones we have posted it for

	WgGizmo * pGizmo = m_keyFocusGizmo.GetRealPtr();
	while( pGizmo )
	{
		QueueEvent( new WgEvent::KeyPress( pEvent->NativeKeyCode(), pGizmo ) );
		pInfo->vGizmos.push_back(WgGizmoWeakPtr(pGizmo));
		pGizmo = pGizmo->ParentX()->CastToGizmo();
	}

	// Push the info-structure onto m_keysDown.

	m_keysDown.push_back( pInfo );

	// Update modkeys

	switch( pEvent->TranslatedKeyCode() )
	{
		case WG_KEY_SHIFT:
			m_modKeys = (WgModifierKeys) (m_modKeys | WG_MODKEY_SHIFT);
			break;
		case WG_KEY_CONTROL:
			m_modKeys = (WgModifierKeys) (m_modKeys | WG_MODKEY_CTRL);
			break;
		case WG_KEY_ALT:
			m_modKeys = (WgModifierKeys) (m_modKeys | WG_MODKEY_ALT);
			break;
		case WG_KEY_SUPER:
			m_modKeys = (WgModifierKeys) (m_modKeys | WG_MODKEY_SUPER);
			break;
		default:
			break;
	}
}

//____ _processKeyRepeat() ____________________________________________________

void WgEventHandler::_processKeyRepeat( WgEvent::KeyRepeat * pEvent )
{
	// Find right KeyDownInfo structure

	KeyDownInfo * pInfo = 0;

	for( unsigned int i = 0 ; i < m_keysDown.size() ; i++ )
	{
		if( pEvent->NativeKeyCode() == m_keysDown[i]->pEvent->NativeKeyCode() )
		{
			pInfo = m_keysDown[i];
			break;
		}
	}

	assert( pInfo != 0 );		// KEY_RELEASE without a matching KEY_PRESS preceding.
	if( pInfo == 0 )
		return;

	// Post KEY_REPEAT events for gizmos

	for( unsigned int i = 0 ; i < pInfo->vGizmos.size() ; i++ )
	{
		if( pInfo->vGizmos[i] )
			QueueEvent( new WgEvent::KeyRepeat( pEvent->NativeKeyCode(), pInfo->vGizmos[i].GetRealPtr() ));
	}

}

//____ _processKeyRelease() ___________________________________________________

void WgEventHandler::_processKeyRelease( WgEvent::KeyRelease * pEvent )
{
	// Find right KeyDownInfo structure and remove it from m_keysDown.

	KeyDownInfo * pInfo = 0;

	std::vector<KeyDownInfo*>::iterator it = m_keysDown.begin();
	while( it != m_keysDown.end() )
	{
		if( pEvent->NativeKeyCode() == (*it)->pEvent->NativeKeyCode() )
		{
			pInfo = *it;
			m_keysDown.erase(it);
			break;
		}
		it++;
	}

	assert( pInfo != 0 );			// KEY_RELEASE without a matching KEY_PRESS preceding.
	if( !pInfo )
		return;

	// Post KEY_RELEASE events for gizmos

	for( unsigned int i = 0 ; i < pInfo->vGizmos.size() ; i++ )
	{
		if( pInfo->vGizmos[i] )
			QueueEvent( new WgEvent::KeyRelease( pEvent->NativeKeyCode(), pInfo->vGizmos[i].GetRealPtr() ));
	}

	// Delete the KeyPress-message and KeyDownInfo-structure

	delete pInfo->pEvent;
	delete pInfo;

	// Update modkeys

	switch( pEvent->TranslatedKeyCode() )
	{
		case WG_KEY_SHIFT:
			m_modKeys = (WgModifierKeys) (m_modKeys & ~WG_MODKEY_SHIFT);
			break;
		case WG_KEY_CONTROL:
			m_modKeys = (WgModifierKeys) (m_modKeys & ~WG_MODKEY_CTRL);
			break;
		case WG_KEY_ALT:
			m_modKeys = (WgModifierKeys) (m_modKeys & ~WG_MODKEY_ALT);
			break;
		case WG_KEY_SUPER:
			m_modKeys = (WgModifierKeys) (m_modKeys & ~WG_MODKEY_SUPER);
			break;
		default:
			break;
	}
}

//____ _processCharacter() ____________________________________________________

void WgEventHandler::_processCharacter( WgEvent::Character * pEvent )
{
	WgGizmo * pGizmo = m_keyFocusGizmo.GetRealPtr();

	if( pGizmo )
		QueueEvent( new WgEvent::Character( pEvent->Char(), pGizmo ) );
}

//____ _processWheelRoll() ____________________________________________________

void WgEventHandler::_processWheelRoll( WgEvent::WheelRoll * pEvent )
{
	_updateMarkedGizmos(false);

	for( size_t i = 0 ; i < m_vMarkedGizmos.size() ; i++ )
	{
		WgGizmo * pGizmo = m_vMarkedGizmos[i].GetRealPtr();

		if( pGizmo )
			QueueEvent( new WgEvent::WheelRoll( pEvent->Wheel(), pEvent->Distance(), pGizmo ) );
	}
}

//____ _processButtonPress() ___________________________________________________

void WgEventHandler::_processButtonPress( WgEvent::ButtonPress * pEvent )
{
	_updateMarkedGizmos(false);

	int button = pEvent->Button();

	// Update m_previousPressGizmos

	m_previousPressGizmos[button].clear();

	for( size_t i = 0 ; i < m_latestPressGizmos[button].size() ; i++ )
	{
		WgGizmo * pGizmo = m_latestPressGizmos[button][i].GetRealPtr();

		if( pGizmo )
			m_previousPressGizmos[button].push_back(pGizmo);
	}

	// Post BUTTON_PRESS events for marked gizmos and remember which one we have posted it for

	m_latestPressGizmos[button].clear();

	for( size_t i = 0 ; i < m_vMarkedGizmos.size() ; i++ )
	{
		WgGizmo * pGizmo = m_vMarkedGizmos[i].GetRealPtr();

		if( pGizmo )
		{
			QueueEvent( new WgEvent::ButtonPress( button, pGizmo ) );
			m_latestPressGizmos[button].push_back(pGizmo);
		}
	}

	// Handle possible double-click

	if( m_pLatestPressEvents[button] && m_pLatestPressEvents[button]->Timestamp() + m_doubleClickTimeTreshold > pEvent->Timestamp() )
	{
		WgCord distance = pEvent->PointerPos() - m_pLatestPressEvents[button]->PointerPos();

		if( distance.x <= m_doubleClickDistanceTreshold &&
			distance.x >= -m_doubleClickDistanceTreshold &&
			distance.y <= m_doubleClickDistanceTreshold &&
			distance.y >= -m_doubleClickDistanceTreshold )
			QueueEvent( new WgEvent::ButtonDoubleClick(button) );
	}

	// Save info for the future

	delete m_pLatestPressEvents[button];		// Delete previous saved event.
	m_pLatestPressEvents[button] = pEvent;

	m_bButtonPressed[button] = true;

	// If m_vModalGizmos has content then we have the press outside a modal
	// gizmo and should inform.

	for( size_t i = 0 ; i < m_vModalGizmos.size() ; i++ )
	{
		WgGizmo * pGizmo = m_vModalGizmos[i].GetRealPtr();

		if( pGizmo )
			QueueEvent( new WgEvent::ButtonPressOutsideModal( button, pGizmo ) );
	}


}


//____ _processButtonRepeat() __________________________________________________

void WgEventHandler::_processButtonRepeat( WgEvent::ButtonRepeat * pEvent )
{
	_updateMarkedGizmos(false);

	int button = pEvent->Button();

	// Post BUTTON_REPEAT events for all widgets that received the press and we
	// still are inside.

	for( size_t i = 0 ; i < m_latestPressGizmos[button].size() ; i++ )
	{
		WgGizmo * pGizmo = m_latestPressGizmos[button][i].GetRealPtr();
		if( pGizmo && _isGizmoInList( pGizmo, m_vMarkedGizmos ) )
			QueueEvent( new WgEvent::ButtonRepeat(button, pGizmo) );
	}
}



//____ _processButtonRelease() _________________________________________________

void WgEventHandler::_processButtonRelease( WgEvent::ButtonRelease * pEvent )
{
	_updateMarkedGizmos(false);

	int button = pEvent->Button();

	// Post BUTTON_RELEASE events for all gizmos that were pressed

	for( size_t i = 0 ; i < m_latestPressGizmos[button].size() ; i++ )
	{
		WgGizmo * pGizmo = m_latestPressGizmos[button][i].GetRealPtr();
		if( pGizmo )
		{
			bool bIsInside = pGizmo->ScreenGeo().contains(pEvent->PointerPos());
			QueueEvent( new WgEvent::ButtonRelease( button, pGizmo, true, bIsInside ) );
		}
	}

	// Post BUTTON_RELEASE events for all gizmos that were NOT pressed

	for( size_t i = 0 ; i < m_vMarkedGizmos.size() ; i++ )
	{
		WgGizmo * pGizmo = m_vMarkedGizmos[i].GetRealPtr();
		if( pGizmo )
		{
			if( !_isGizmoInList( pGizmo, m_latestPressGizmos[button] ) )
			{
				bool bIsInside = pGizmo->ScreenGeo().contains(pEvent->PointerPos());
				QueueEvent( new WgEvent::ButtonRelease( button, pGizmo, false, bIsInside ) );
			}
		}
	}

	// As long as the button was pressed inside our window we have a click
	// on this level.

	if( m_bButtonPressed[button] )
		QueueEvent( new WgEvent::ButtonClick( button ) );

	// Save info for the future

	delete m_pLatestReleaseEvents[button];		// Delete previous saved event.
	m_pLatestReleaseEvents[button] = pEvent;
	m_bButtonPressed[button] = false;

	// If m_vModalGizmos has content then we have the release outside a modal
	// gizmo and should inform.

	for( size_t i = 0 ; i < m_vModalGizmos.size() ; i++ )
	{
		WgGizmo * pGizmo = m_vModalGizmos[i].GetRealPtr();

		if( pGizmo )
			QueueEvent( new WgEvent::ButtonReleaseOutsideModal( button, pGizmo ) );
	}


}

//____ _processButtonDrag() ____________________________________________________

void WgEventHandler::_processButtonDrag( WgEvent::ButtonDrag * pEvent )
{
	int button = pEvent->Button();

	// Post POINTER_DRAG events for all gizmos that are pressed

	for( size_t i = 0 ; i < m_latestPressGizmos[button].size() ; i++ )
	{
		WgGizmo * pGizmo = m_latestPressGizmos[button][i].GetRealPtr();

		if( pGizmo )
		{
			WgCord	ofs = pGizmo->ScreenPos();
			QueueEvent( new WgEvent::ButtonDrag( button, pGizmo, pEvent->StartPos() - ofs, pEvent->PrevPos() - ofs, pEvent->CurrPos() - ofs ) );
		}
	}

}

//____ _processButtonClick() _________________________________________________

void WgEventHandler::_processButtonClick( WgEvent::ButtonClick * pEvent )
{
	int button = pEvent->Button();

	// Post BUTTON_CLICK events for all widgets that received the press and we
	// still are inside.

	for( size_t i = 0 ; i < m_latestPressGizmos[button].size() ; i++ )
	{
		WgGizmo * pGizmo = m_latestPressGizmos[button][i].GetRealPtr();
		if( pGizmo && _isGizmoInList( pGizmo, m_vMarkedGizmos ) )
			QueueEvent( new WgEvent::ButtonClick(button, pGizmo) );
	}
}

//____ _processButtonDoubleClick() _________________________________________________

void WgEventHandler::_processButtonDoubleClick( WgEvent::ButtonDoubleClick * pEvent )
{
	int button = pEvent->Button();

	// Post BUTTON_DOUBLECLICK events for all widgets that received both this and previous press.

	for( size_t i = 0 ; i < m_latestPressGizmos[button].size() ; i++ )
	{
		WgGizmo * pGizmo = m_latestPressGizmos[button][i].GetRealPtr();
		if( pGizmo && _isGizmoInList( pGizmo, m_previousPressGizmos[button] ) )
			QueueEvent( new WgEvent::ButtonDoubleClick(button, pGizmo) );
	}
}

//____ _isGizmoInList() ________________________________________________________

bool WgEventHandler::_isGizmoInList( const WgGizmo * pGizmo, const std::vector<WgGizmoWeakPtr>& list )
{
	for( size_t i = 0 ; i < list.size() ; i++ )
		if( list[i].GetRealPtr() == pGizmo )
			return true;

	return false;
}





WgEventHandler::GizmoCallback::GizmoCallback( const WgEventFilter& filter, void(*fp)(const WgEvent::Event * pEvent, WgGizmo * pDest), WgGizmo * pDest )
{
	m_filter = filter;
	m_pFunction = fp;
	m_pGizmo = pDest;
}

void WgEventHandler::GizmoCallback::ProcessEvent( const WgEvent::Event * pEvent )
{
	WgGizmo * p = m_pGizmo.GetRealPtr();
	if( p && m_filter.FilterEvent(pEvent) )
		m_pFunction(pEvent,p);
}

bool WgEventHandler::GizmoCallback::IsAlive() const
{
	return m_pGizmo?true:false;
}

void * WgEventHandler::GizmoCallback::Receiver() const
{
	return m_pGizmo.GetRealPtr();
}


WgEventHandler::FunctionCallback::FunctionCallback( const WgEventFilter& filter, void(*fp)(const WgEvent::Event * pEvent) )
{
	m_filter = filter;
	m_pFunction = fp;
}

void WgEventHandler::FunctionCallback::ProcessEvent( const WgEvent::Event * pEvent )
{
	if( m_pFunction && m_filter.FilterEvent(pEvent) )
		m_pFunction(pEvent);
}

bool WgEventHandler::FunctionCallback::IsAlive() const
{
	return true;
}

void * WgEventHandler::FunctionCallback::Receiver() const
{
	return reinterpret_cast<void*>(m_pFunction);
}

WgEventHandler::FunctionCallbackParam::FunctionCallbackParam( const WgEventFilter& filter, void(*fp)(const WgEvent::Event * pEvent, void * pDest), void * pParam )
{
	m_filter = filter;
	m_pFunction = fp;
	m_pParam 	= pParam;
}

void WgEventHandler::FunctionCallbackParam::ProcessEvent( const WgEvent::Event * pEvent )
{
	if( m_pFunction && m_filter.FilterEvent(pEvent) )
		m_pFunction(pEvent,m_pParam);
}

bool WgEventHandler::FunctionCallbackParam::IsAlive() const
{
	return true;
}

void * WgEventHandler::FunctionCallbackParam::Receiver() const
{
	return reinterpret_cast<void*>(m_pFunction);
}


WgEventHandler::ListenerCallback::ListenerCallback( const WgEventFilter& filter, WgEventListener * pListener )
{
	m_filter = filter;
	m_pListener = pListener;
}

void WgEventHandler::ListenerCallback::ProcessEvent( const WgEvent::Event * pEvent )
{
	if( m_filter.FilterEvent(pEvent) )
		m_pListener->OnEvent( pEvent );
}

bool WgEventHandler::ListenerCallback::IsAlive() const
{
	return true;
}

void * WgEventHandler::ListenerCallback::Receiver() const
{
	return m_pListener;
}

