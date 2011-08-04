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

	for( int i = 0 ; i <= WG_MAX_BUTTONS ; i++ )
		m_pLatestButtonEvents[i] = 0;
}

//____ Destructor _____________________________________________________________

WgEventHandler::~WgEventHandler()
{
}

//____ AddCallback() __________________________________________________________

void WgEventHandler::AddCallback( void(*fp)(const WgEvent::Event& _event) )
{
	Callback * p = new FunctionCallback( WgEventFilter(), fp );
	m_globalCallbacks.PushBack( p );
}

void WgEventHandler::AddCallback( void(*fp)(const WgEvent::Event& _event, void * pParam), void * pParam )
{
	Callback * p = new FunctionCallbackParam( WgEventFilter(), fp, pParam );
	m_globalCallbacks.PushBack( p );
}

void WgEventHandler::AddCallback( void(*fp)(const WgEvent::Event& _event, WgGizmo * pDest), WgGizmo * pDest )
{
	Callback * p = new GizmoCallback( WgEventFilter(), fp, pDest );
	m_globalCallbacks.PushBack( p );
}

void WgEventHandler::AddCallback( WgEventListener * pListener )
{
	Callback * p = new ListenerCallback( WgEventFilter(), pListener );
	m_globalCallbacks.PushBack( p );
}

void WgEventHandler::AddCallback( const WgEventFilter& filter, void(*fp)(const WgEvent::Event& _event) )
{
	Callback * p = new FunctionCallback( filter, fp );
	_addCallback( filter, p );
}

void WgEventHandler::AddCallback( const WgEventFilter& filter, void(*fp)(const WgEvent::Event& _event, void * pParam), void * pParam )
{
	Callback * p = new FunctionCallbackParam( filter, fp, pParam );
	_addCallback( filter, p );
}

void WgEventHandler::AddCallback( const WgEventFilter& filter, void(*fp)(const WgEvent::Event& _event, WgGizmo * pDest), WgGizmo * pDest )
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

int WgEventHandler::DeleteCallbacksTo( void(*fp)( const WgEvent::Event& _event) )
{
	return _deleteCallbacksTo( reinterpret_cast<void*>(fp) );
}

int WgEventHandler::DeleteCallbacksTo( void(*fp)( const WgEvent::Event& _event, void * pParam) )
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







int WgEventHandler::DeleteCallback( const WgEventFilter& filter,const  WgGizmo * pGizmo )
{
	return _deleteCallback( filter, pGizmo );
}

int WgEventHandler::DeleteCallback( const WgEventFilter& filter, const WgEventListener * pListener )
{
	return _deleteCallback( filter, pListener );
}

int WgEventHandler::DeleteCallback( const WgEventFilter& filter, void(*fp)( const WgEvent::Event& _event) )
{
	return _deleteCallback( filter, (const void *) fp );
}

int WgEventHandler::DeleteCallback( const WgEventFilter& filter, void(*fp)( const WgEvent::Event& _event, void * pParam) )
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

bool WgEventHandler::QueueEvent( const WgEvent::Event& _event )
{
	if( m_bIsProcessing )
	{
		// Events that are posted as a direct result of another event being processed
		// are inserted before the next event in the queue.
		// If two or more events are posted by the same event being processed,
		// they need to be queued in the order of posting.

		m_insertPos = m_eventQueue.insert( m_insertPos, _event );
		m_insertPos++;
	}
	else
	{
		// Events being posted outside processing loop are simply added to the
		// queue.

		m_eventQueue.push_back( _event );
	}

	return true;
}

//____ ProcessEvents() ________________________________________________________

void WgEventHandler::ProcessEvents()
{
	m_bIsProcessing = true;

	while( !m_eventQueue.empty() )
	{
		WgEvent::Event& ev = m_eventQueue.front();

		m_insertPos = m_eventQueue.begin()+1;	// Insert position set to right after current event.

		_finalizeEvent( ev );

		if( ev.IsForGizmo() )
		{
			WgGizmo * pGizmo = ev.Gizmo();
			if( pGizmo )
				pGizmo->_onEvent( ev, this );
		}
		else
		{
			_processGeneralEvent( ev );
		}
		_processEventCallbacks( ev );

		m_eventQueue.pop_front();
	}

	m_bIsProcessing = false;
}

//____ _processEventCallbacks() ________________________________________________

void WgEventHandler::_processEventCallbacks( WgEvent::Event& _event )
{
	// Call all global callbacks

	Callback * pCallback = m_globalCallbacks.First();

	while( pCallback )
	{
		pCallback->ProcessEvent( _event );
		pCallback = pCallback->Next();
	}

	// Call all Gizmo-specific callbacks

	WgChain<Callback> * pChain = 0;

	if( _event.IsForGizmo() && _event.Gizmo() )
		pChain = &m_gizmoCallbacks[_event.GizmoWeakPtr()];
	else
		return;	// Event was for a Gizmo that now has disappeared.


	pCallback = pChain->First();

	while( pCallback )
	{
		pCallback->ProcessEvent( _event );
		pCallback = pCallback->Next();
	}
}


//____ _finalizeEvent() ________________________________________________________

void WgEventHandler::_finalizeEvent( WgEvent::Event& _event )
{
	// Fill in missing information in the event-class.

	_event.m_timestamp	= m_time;
	_event.m_modKeys	= m_modKeys;

	// Only global POINTER_ENTER & POINTER_MOVE events have these members
	// set, the rest needs to have them filled in.

	if( _event.IsForGizmo() || (_event.Type() != WG_EVENT_POINTER_MOVE && _event.Type() != WG_EVENT_POINTER_ENTER) )
	{
		_event.m_pointerScreenPos = m_pointerPos;
		_event.m_pointerLocalPos = m_pointerPos;

		if( _event.Gizmo() )
			_event.m_pointerLocalPos -= _event.Gizmo()->ScreenPos();
	}
}

//____ _processGeneralEvent() _________________________________________________

void WgEventHandler::_processGeneralEvent( WgEvent::Event& _event )
{

	switch( _event.Type() )
	{
		case WG_EVENT_POINTER_ENTER:
			_processPointerEnter( (WgEvent::PointerEnter*) &_event );
			break;

		case WG_EVENT_POINTER_MOVE:
			_processPointerMove( (WgEvent::PointerMove*) &_event );
			break;
		case WG_EVENT_POINTER_PLACED:
			_processPointerPlaced( (WgEvent::PointerPlaced*) &_event );
			break;

		case WG_EVENT_POINTER_EXIT:
			_processPointerExit( (WgEvent::PointerExit*) &_event );
			break;

		case WG_EVENT_BUTTON_PRESS:
			_processButtonPress( (WgEvent::ButtonPress*) &_event );
			break;

		case WG_EVENT_BUTTON_REPEAT:
			_processButtonRepeat( (WgEvent::ButtonRepeat*) &_event );
			break;

		case WG_EVENT_BUTTON_DRAG:
			_processButtonDrag( (WgEvent::ButtonDrag*) &_event );
			break;

		case WG_EVENT_BUTTON_RELEASE:
			_processButtonRelease( (WgEvent::ButtonRelease*) &_event );
			break;

		case WG_EVENT_BUTTON_CLICK:
			_processButtonClick( (WgEvent::ButtonClick*) &_event );
			break;
		case WG_EVENT_BUTTON_DOUBLECLICK:
			_processButtonDoubleClick( (WgEvent::ButtonDoubleClick*) &_event );
			break;

		case WG_EVENT_KEY_PRESS:
		case WG_EVENT_KEY_REPEAT:
		case WG_EVENT_KEY_RELEASE:
		case WG_EVENT_CHARACTER:
		case WG_EVENT_WHEEL_ROLL:
			break;

		case WG_EVENT_TICK:
			_processTick( (WgEvent::Tick*) &_event );
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
		if( m_pLatestButtonEvents[button] == &m_latestPress[button] )
		{
			int msSinceRepeatStart = (int) (m_time - m_latestPress[button].Timestamp() - m_buttonRepeatDelay );

			// First BUTTON_REPEAT event posted separately.

			if( msSinceRepeatStart < 0 && msSinceRepeatStart + pEvent->Millisec() >= 0 )
				QueueEvent( WgEvent::ButtonRepeat(button) );

			// Calculate ms since last BUTTON_REPEAT event

			int msToProcess;
			if( msSinceRepeatStart < 0 )
				msToProcess = msSinceRepeatStart + pEvent->Millisec();
			else
				msToProcess = (msSinceRepeatStart % m_buttonRepeatRate) + pEvent->Millisec();

			// Post the amount of BUTTON_REPEAT that should be posted.

			while( msToProcess >= m_buttonRepeatRate )
			{
				QueueEvent( WgEvent::ButtonRepeat(button) );
				msToProcess -= m_buttonRepeatRate;
			}
		}
	}


	// Increase time counter

	m_time += pEvent->Millisec();
}

//____ _processPointerEnter() __________________________________________________

void WgEventHandler::_processPointerEnter( WgEvent::PointerEnter * pEvent )
{
	// Post events for button drag

	for( int i = 0 ; i <= WG_MAX_BUTTONS ; i++ )
	{
		if( m_pLatestButtonEvents[i] && m_pLatestButtonEvents[i]->Type() == WG_EVENT_BUTTON_PRESS )
			QueueEvent( WgEvent::ButtonDrag( i, m_latestPress[i].PointerPos(), m_pointerPos, pEvent->PointerPos() ) );
	}

	// Post event for finalizing position once button drag is taken care of.

	QueueEvent( WgEvent::PointerPlaced() );

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
			QueueEvent( WgEvent::PointerExit( pGizmo ) );
	}

	m_vMarkedGizmos.clear();
}


//____ _processPointerMove() ___________________________________________________

void WgEventHandler::_processPointerMove( WgEvent::PointerMove * pEvent )
{
	// Post events for button drag

	for( int i = 0 ; i <= WG_MAX_BUTTONS ; i++ )
	{
		if( m_pLatestButtonEvents[i] && m_pLatestButtonEvents[i]->Type() == WG_EVENT_BUTTON_PRESS )
			QueueEvent( WgEvent::ButtonDrag( i, m_latestPress[i].PointerPos(), m_pointerPos, pEvent->PointerPos() ) );
	}

	// Post event for finalizing move once button drag is taken care of.

	QueueEvent( WgEvent::PointerPlaced() );

	// Update pointer position

	m_pointerPos = pEvent->PointerPos();
}

//____ _processPointerPlaced() _________________________________________________

void WgEventHandler::_processPointerPlaced( WgEvent::PointerPlaced * pEvent )
{
	std::vector<WgGizmo*>	vNowMarked;

	// Collect widgets we now are inside

	WgGizmo * pGizmo = m_pRoot->FindGizmo( m_pointerPos, WG_SEARCH_ACTION_TARGET );

	while( pGizmo )
	{
		vNowMarked.push_back(pGizmo);

		WgGizmoContainer * p = pGizmo->Hook()->Parent();

		if( p )
			pGizmo = p->CastToGizmo();
		else
			pGizmo = 0;
	}

	// Post POINTER_EXIT events for gizmos no longer marked

	for( size_t i = 0 ; i < m_vMarkedGizmos.size() ; i++ )
	{
		pGizmo = m_vMarkedGizmos[i].GetRealPtr();

		size_t j = 0;
		while( j < vNowMarked.size() )
		{
			if( pGizmo == vNowMarked[j] )
				break;
			j++;
		}

		if( j == vNowMarked.size() )
			QueueEvent( WgEvent::PointerExit( pGizmo ) );
	}

	// Post POINTER_ENTER events for new marked gizmos
	// and POINTER_MOVE events for those already marked

	for( size_t i = 0 ; i < vNowMarked.size() ; i++ )
	{
		pGizmo = vNowMarked[i];

		size_t j = 0;
		while( j < m_vMarkedGizmos.size() )
		{
			if( pGizmo == m_vMarkedGizmos[j].GetRealPtr() )
				break;
			j++;
		}

		if( j == vNowMarked.size() )
			QueueEvent( WgEvent::PointerEnter( pGizmo ) );
		else
			QueueEvent( WgEvent::PointerMove( pGizmo ) );
	}

	// Copy content of vNowMarked to m_vMarkedGizmos

	m_vMarkedGizmos.clear();
	for( size_t i = 0 ; i < vNowMarked.size() ; i++ )
		m_vMarkedGizmos.push_back( vNowMarked[i] );
}

//____ _processButtonPress() ___________________________________________________

void WgEventHandler::_processButtonPress( WgEvent::ButtonPress * pEvent )
{
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
			QueueEvent( WgEvent::ButtonPress( button, pGizmo ) );
			m_latestPressGizmos[button].push_back(pGizmo);
		}
	}

	// Handle possible double-click

	if( m_latestPress[button].Timestamp() + m_doubleClickTimeTreshold > pEvent->Timestamp() )
	{
		WgCord distance = pEvent->PointerPos() - m_latestPress[button].PointerPos();

		if( distance.x <= m_doubleClickDistanceTreshold &&
			distance.x >= -m_doubleClickDistanceTreshold &&
			distance.y <= m_doubleClickDistanceTreshold &&
			distance.y >= -m_doubleClickDistanceTreshold )
			QueueEvent( WgEvent::ButtonDoubleClick(button) );
	}

	// Save info for the future

	m_latestPress[button] = *pEvent;
	m_pLatestButtonEvents[button] = &m_latestPress[button];

}


//____ _processButtonRepeat() __________________________________________________

void WgEventHandler::_processButtonRepeat( WgEvent::ButtonRepeat * pEvent )
{
	int button = pEvent->Button();

	// Post BUTTON_REPEAT events for all widgets that received the press and we
	// still are inside.

	for( size_t i = 0 ; i < m_latestPressGizmos[button].size() ; i++ )
	{
		WgGizmo * pGizmo = m_latestPressGizmos[button][i].GetRealPtr();
		if( pGizmo && _isGizmoInList( pGizmo, m_vMarkedGizmos ) )
			QueueEvent( WgEvent::ButtonRepeat(button, pGizmo) );
	}
}



//____ _processButtonRelease() _________________________________________________

void WgEventHandler::_processButtonRelease( WgEvent::ButtonRelease * pEvent )
{
	int button = pEvent->Button();

	// Post BUTTON_RELEASE events for all gizmos that were pressed

	for( size_t i = 0 ; i < m_latestPressGizmos[button].size() ; i++ )
	{
		WgGizmo * pGizmo = m_latestPressGizmos[button][i].GetRealPtr();
		if( pGizmo )
		{
			bool bIsInside = pGizmo->ScreenGeometry().contains(pEvent->PointerPos());
			QueueEvent( WgEvent::ButtonRelease( button, pGizmo, true, bIsInside ) );
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
				bool bIsInside = pGizmo->ScreenGeometry().contains(pEvent->PointerPos());
				QueueEvent( WgEvent::ButtonRelease( button, pGizmo, false, bIsInside ) );
			}
		}
	}

	// As long as the button was pressed inside our window we have a click
	// on this level.

	if( m_pLatestButtonEvents[button] == &m_latestPress[button] )
		QueueEvent( WgEvent::ButtonClick( button ) );

	// Save info for the future

	m_latestRelease[button] = *pEvent;
	m_pLatestButtonEvents[button] = &m_latestRelease[button];

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
			QueueEvent( WgEvent::ButtonDrag( button, pGizmo, pEvent->StartPos() - ofs, pEvent->PrevPos() - ofs, pEvent->CurrPos() - ofs ) );
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
			QueueEvent( WgEvent::ButtonClick(button, pGizmo) );
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
			QueueEvent( WgEvent::ButtonDoubleClick(button, pGizmo) );
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





WgEventHandler::GizmoCallback::GizmoCallback( const WgEventFilter& filter, void(*fp)(const WgEvent::Event& _event, WgGizmo * pDest), WgGizmo * pDest )
{
	m_filter = filter;
	m_pFunction = fp;
	m_pGizmo = pDest;
}

void WgEventHandler::GizmoCallback::ProcessEvent( const WgEvent::Event& _event )
{
	WgGizmo * p = m_pGizmo.GetRealPtr();
	if( p && m_filter.FilterEvent(_event) )
		m_pFunction(_event,p);
}

bool WgEventHandler::GizmoCallback::IsAlive() const
{
	return m_pGizmo?true:false;
}

void * WgEventHandler::GizmoCallback::Receiver() const
{
	return m_pGizmo.GetRealPtr();
}


WgEventHandler::FunctionCallback::FunctionCallback( const WgEventFilter& filter, void(*fp)(const WgEvent::Event& _event) )
{
	m_filter = filter;
	m_pFunction = fp;
}

void WgEventHandler::FunctionCallback::ProcessEvent( const WgEvent::Event& _event )
{
	if( m_pFunction && m_filter.FilterEvent(_event) )
		m_pFunction(_event);
}

bool WgEventHandler::FunctionCallback::IsAlive() const
{
	return true;
}

void * WgEventHandler::FunctionCallback::Receiver() const
{
	return reinterpret_cast<void*>(m_pFunction);
}

WgEventHandler::FunctionCallbackParam::FunctionCallbackParam( const WgEventFilter& filter, void(*fp)(const WgEvent::Event& _event, void * pDest), void * pParam )
{
	m_filter = filter;
	m_pFunction = fp;
	m_pParam 	= pParam;
}

void WgEventHandler::FunctionCallbackParam::ProcessEvent( const WgEvent::Event& _event )
{
	if( m_pFunction && m_filter.FilterEvent(_event) )
		m_pFunction(_event,m_pParam);
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

void WgEventHandler::ListenerCallback::ProcessEvent( const WgEvent::Event& _event )
{
	if( m_filter.FilterEvent(_event) )
		m_pListener->OnEvent( _event );
}

bool WgEventHandler::ListenerCallback::IsAlive() const
{
	return true;
}

void * WgEventHandler::ListenerCallback::Receiver() const
{
	return m_pListener;
}

