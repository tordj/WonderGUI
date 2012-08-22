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
#include <wg_base.h>
#include <wg_root.h>
#include <wg_gizmo_container.h>

//____ Constructor ____________________________________________________________

WgEventHandler::WgEventHandler( WgRoot * pRoot )
{
	m_pRoot					= pRoot;
	m_time					= 0;
	m_modKeys				= WG_MODKEY_NONE;
	m_pointerStyle			= WG_POINTER_DEFAULT;

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

//____ SetFocusGroup() ________________________________________________________

bool WgEventHandler::SetFocusGroup( WgGizmoContainer * pFocusGroup )
{
	// Sanity checks

	if( pFocusGroup )
	{
		if( pFocusGroup == m_keyFocusGroup.GetRealPtr() )
			return true;									// Not an error, but we don't need to do anything

		if( !pFocusGroup->IsFocusGroup() )
			return false;									// Container is not a focus group

		if( !pFocusGroup->Hook() || pFocusGroup->Hook()->Root() != m_pRoot )
			return false;									// pFocusGroup is not a child of our root.
	}

	// Set new focus gizmo as specified by group

	WgGizmoWeakPtr pNewFocusGizmo;

	if( pFocusGroup )
		if( m_focusGroupMap.find(pFocusGroup) != m_focusGroupMap.end() )
			pNewFocusGizmo = m_focusGroupMap[pFocusGroup];

	if( m_keyFocusGizmo )
		m_keyFocusGizmo->_onLostInputFocus();

	if( pNewFocusGizmo )
		pNewFocusGizmo->_onGotInputFocus();

	// Set members and exit

	m_keyFocusGizmo = pNewFocusGizmo;
	m_keyFocusGroup = pFocusGroup;

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

		WgGizmoContainer * p = pFocus->Parent()->CastToContainer();
		while( p && !p->IsFocusGroup() )
			p = p->Parent()->CastToContainer();

		if( p )
			m_keyFocusGroup = p;
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

//____ IsMouseButtonPressed() _________________________________________________________

bool WgEventHandler::IsMouseButtonPressed( int button )
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

		WgChain<Callback>& chain = m_gizmoCallbacks[pGizmo];

		chain.PushBack(pCallback);
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

//____ ForwardEvent() __________________________________________________________

bool WgEventHandler::ForwardEvent( const WgEvent::Event * _pEvent )
{
	if( !_pEvent )
		return false;
	
	WgGizmo * p = _pEvent->Gizmo();
	if( p && p->Parent() )
		return ForwardEvent( _pEvent, p->Parent()->CastToGizmo() );
	else
		return false;				
}

bool WgEventHandler::ForwardEvent( const WgEvent::Event * _pEvent, WgGizmo * pRecipient )
{
	if( !_pEvent || !pRecipient )
		return false;

	WgEvent::Event * pEvent = 0;

	switch( _pEvent->Type() ) 
	{
		case WG_EVENT_MOUSEBUTTON_PRESS:
			pEvent = new WgEvent::MouseButtonPress(0);
			pEvent->_cloneContentFrom( static_cast<const WgEvent::MouseButtonPress*>(_pEvent) );
			break;
		case WG_EVENT_MOUSEBUTTON_REPEAT:
			pEvent =  new WgEvent::MouseButtonRepeat(0);
			pEvent->_cloneContentFrom( static_cast<const WgEvent::MouseButtonRepeat*>(_pEvent) );
			break;
		case WG_EVENT_MOUSEBUTTON_DRAG:
			pEvent =  new WgEvent::MouseButtonDrag(0,WgCoord(),WgCoord(),WgCoord());
			pEvent->_cloneContentFrom( static_cast<const WgEvent::MouseButtonDrag*>(_pEvent) );
			break;
		case WG_EVENT_MOUSEBUTTON_RELEASE:
			pEvent =  new WgEvent::MouseButtonRelease(0);
			pEvent->_cloneContentFrom( static_cast<const WgEvent::MouseButtonRelease*>(_pEvent) );
			break;
		case WG_EVENT_MOUSEBUTTON_CLICK:
			pEvent =  new WgEvent::MouseButtonClick(0);
			pEvent->_cloneContentFrom( static_cast<const WgEvent::MouseButtonClick*>(_pEvent) );
			break;
		case WG_EVENT_MOUSEBUTTON_DOUBLE_CLICK:
			pEvent =  new WgEvent::MouseButtonDoubleClick(0);
			pEvent->_cloneContentFrom( static_cast<const WgEvent::MouseButtonDoubleClick*>(_pEvent) );
			break;
		case WG_EVENT_KEY_PRESS:
			pEvent =  new WgEvent::KeyPress(0);
			pEvent->_cloneContentFrom( static_cast<const WgEvent::KeyPress*>(_pEvent) );
			break;
		case WG_EVENT_KEY_REPEAT:
			pEvent =  new WgEvent::KeyRepeat(0);
			pEvent->_cloneContentFrom( static_cast<const WgEvent::KeyRepeat*>(_pEvent) );		
			break;
		case WG_EVENT_KEY_RELEASE:
			pEvent =  new WgEvent::KeyRelease(0);
			pEvent->_cloneContentFrom( static_cast<const WgEvent::KeyRelease*>(_pEvent) );		
			break;
		case WG_EVENT_CHARACTER:
			pEvent =  new WgEvent::Character(0);
			pEvent->_cloneContentFrom( static_cast<const WgEvent::Character*>(_pEvent) );		
			break;
		case WG_EVENT_MOUSEWHEEL_ROLL:
			pEvent =  new WgEvent::MouseWheelRoll(0,0);
			pEvent->_cloneContentFrom( static_cast<const WgEvent::MouseWheelRoll*>(_pEvent) );
			break;
		
		default:
			return false;
	}

	pEvent->m_pGizmo = pRecipient;
	pEvent->m_pForwardedFrom = _pEvent->m_pGizmo;
	QueueEvent( pEvent );
	return true;
}


//____ ProcessEvents() ________________________________________________________

void WgEventHandler::ProcessEvents()
{
	int64_t	time = m_time;

	m_bIsProcessing = true;

	// First thing: we make sure that we know what Gizmos pointer is inside, in case that has changed.

	m_insertPos = m_eventQueue.begin();	// Insert any POINTER_ENTER/EXIT right at beginning.
	_updateMarkedGizmo(false);

	// Process all the events in the queue

	_processEventQueue();

	// Post Gizmo-specific tick events now we know how much time has passed

	_postTickEvents( (int) (m_time-time) );

	// Process Gizmo-specific tick events (and any events they might trigger)

	_processEventQueue();

	m_bIsProcessing = false;
}

//____ _processEventQueue() ___________________________________________________

void WgEventHandler::_processEventQueue()
{
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

		if( pEvent->IsForGizmo() || (pEvent->Type() !=  WG_EVENT_MOUSEBUTTON_PRESS &&
			pEvent->Type() != WG_EVENT_MOUSEBUTTON_RELEASE && pEvent->Type() != WG_EVENT_KEY_PRESS) )
			delete pEvent;
	}
}

//____ _postTickEvents() ______________________________________________________

void WgEventHandler::_postTickEvents( int ticks )
{
	std::vector<WgGizmoWeakPtr>::iterator it = m_vTickGizmos.begin();

	while( it != m_vTickGizmos.end() )
	{
		WgGizmo * pGizmo = (*it).GetRealPtr();

		if( pGizmo && pGizmo->Hook() && pGizmo->Hook()->Root() == m_pRoot && pGizmo->m_bReceiveTick )
		{
			QueueEvent( new WgEvent::Tick( ticks, pGizmo ) );
			++it;
		}
		else
			it = m_vTickGizmos.erase(it);
	}
}

//____ _addTickReceiver() _____________________________________________________

void WgEventHandler::_addTickReceiver( WgGizmo * pGizmo )
{
	if( pGizmo && !_isGizmoInList( pGizmo, m_vTickGizmos ) )
		m_vTickGizmos.push_back( WgGizmoWeakPtr(pGizmo) );
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

	if( pEvent->Gizmo() )
	{
		std::map<WgGizmoWeakPtr,WgChain<Callback> >::iterator it;

		it = m_gizmoCallbacks.find(pEvent->GizmoWeakPtr());
		if( it != m_gizmoCallbacks.end() )
			pChain = &(it->second);
	}

	if( !pChain )
		return;

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

	if( pEvent->IsForGizmo() || (pEvent->Type() != WG_EVENT_MOUSE_MOVE && pEvent->Type() != WG_EVENT_MOUSE_ENTER) )
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
			p->m_translatedKeyCode = WgBase::TranslateKey(p->m_nativeKeyCode);
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

		case WG_EVENT_MOUSE_ENTER:
			_processMouseEnter( (WgEvent::MouseEnter*) pEvent );
			break;

		case WG_EVENT_MOUSE_MOVE:
			_processMouseMove( (WgEvent::MouseMove*) pEvent );
			break;

		case WG_EVENT_MOUSE_POSITION:
			_processMousePosition( (WgEvent::MousePosition*) pEvent );
			break;

		case WG_EVENT_MOUSE_LEAVE:
			_processMouseLeave( (WgEvent::MouseLeave*) pEvent );
			break;

		case WG_EVENT_MOUSEBUTTON_PRESS:
			_processMouseButtonPress( (WgEvent::MouseButtonPress*) pEvent );
			break;

		case WG_EVENT_MOUSEBUTTON_REPEAT:
			_processMouseButtonRepeat( (WgEvent::MouseButtonRepeat*) pEvent );
			break;

		case WG_EVENT_MOUSEBUTTON_DRAG:
			_processMouseButtonDrag( (WgEvent::MouseButtonDrag*) pEvent );
			break;

		case WG_EVENT_MOUSEBUTTON_RELEASE:
			_processMouseButtonRelease( (WgEvent::MouseButtonRelease*) pEvent );
			break;

		case WG_EVENT_MOUSEBUTTON_CLICK:
			_processMouseButtonClick( (WgEvent::MouseButtonClick*) pEvent );
			break;

		case WG_EVENT_MOUSEBUTTON_DOUBLE_CLICK:
			_processMouseButtonDoubleClick( (WgEvent::MouseButtonDoubleClick*) pEvent );
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

		case WG_EVENT_MOUSEWHEEL_ROLL:
			_processMouseWheelRoll( (WgEvent::MouseWheelRoll*) pEvent );
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
			int buttonDelay = WgBase::MouseButtonRepeatDelay();
			int buttonRate = WgBase::MouseButtonRepeatRate();

			int msSinceRepeatStart = (int) (m_time - m_pLatestPressEvents[button]->Timestamp() - buttonDelay );

			// First BUTTON_REPEAT event posted separately.

			if( msSinceRepeatStart < 0 && msSinceRepeatStart + pEvent->Millisec() >= 0 )
				QueueEvent( new WgEvent::MouseButtonRepeat(button) );

			// Calculate ms since last BUTTON_REPEAT event

			int msToProcess;
			if( msSinceRepeatStart < 0 )
				msToProcess = msSinceRepeatStart + pEvent->Millisec();
			else
				msToProcess = (msSinceRepeatStart % buttonRate) + pEvent->Millisec();

			// Post the amount of BUTTON_REPEAT that should be posted.

			while( msToProcess >= buttonRate )
			{
				QueueEvent( new WgEvent::MouseButtonRepeat(button) );
				msToProcess -= buttonRate;
			}
		}
	}

	// Check if we need to post KEY_REPEAT

	int keyDelay 	= WgBase::KeyRepeatDelay();
	int keyRate 	= WgBase::KeyRepeatRate();

	for( unsigned int i = 0 ; i < m_keysDown.size() ; i++ )
	{
		KeyDownInfo * pInfo = m_keysDown[i];
		int timePassed = (int) (pEvent->Timestamp() - pInfo->pEvent->Timestamp());

		int fraction = timePassed - keyDelay;

		if( fraction < 0 )
			fraction += keyRate;
		else
			fraction %= keyRate;

		fraction += pEvent->Millisec();

		while( fraction >= keyRate )
		{
			QueueEvent( new WgEvent::KeyRepeat( pInfo->pEvent->NativeKeyCode() ) );
			fraction -= keyRate;
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


//____ _processMouseEnter() __________________________________________________

void WgEventHandler::_processMouseEnter( WgEvent::MouseEnter * pEvent )
{
	// Post events for button drag

	for( int i = 0 ; i <= WG_MAX_BUTTONS ; i++ )
	{
		if( m_bButtonPressed[i] )
			QueueEvent( new WgEvent::MouseButtonDrag( i, m_pLatestPressEvents[i]->PointerPos(), m_pointerPos, pEvent->PointerPos() ) );
	}

	// Post event for finalizing position once button drag is taken care of.

	QueueEvent( new WgEvent::MousePosition() );

	// Update pointer position

	m_pointerPos = pEvent->PointerPos();
}

//____ _processMouseLeave() ___________________________________________________

void WgEventHandler::_processMouseLeave( WgEvent::MouseLeave * pEvent )
{
	// Post POINTER_EXIT event to marked gizmo

	WgGizmo * pGizmo = m_pMarkedGizmo.GetRealPtr();

	if( pGizmo )
		QueueEvent( new WgEvent::MouseLeave( pGizmo ) );

	m_pMarkedGizmo = 0;
}


//____ _processMouseMove() ___________________________________________________

void WgEventHandler::_processMouseMove( WgEvent::MouseMove * pEvent )
{
	// Post events for button drag

	for( int i = 0 ; i <= WG_MAX_BUTTONS ; i++ )
	{
		if( m_bButtonPressed[i] )
			QueueEvent( new WgEvent::MouseButtonDrag( i, m_pLatestPressEvents[i]->PointerPos(), m_pointerPos, pEvent->PointerPos() ) );
	}

	// Post event for finalizing move once button drag is taken care of.

	QueueEvent( new WgEvent::MousePosition() );

	// Update pointer position

	m_pointerPos = pEvent->PointerPos();
}

//____ _processMousePosition() _________________________________________________

void WgEventHandler::_processMousePosition( WgEvent::MousePosition * pEvent )
{
	_updateMarkedGizmo(true);

}

//____ _updateMarkedGizmo() _______________________________________________

void WgEventHandler::_updateMarkedGizmo(bool bPostMouseMoveEvents)
{
	WgGizmo*  pNowMarked = 0;

	WgGizmo * pGizmoTarget = m_pRoot->FindGizmo( m_pointerPos, WG_SEARCH_ACTION_TARGET );

	// Figure out which button of currently pressed has been pressed the longest.
	// Mouse is only allowed to mark Gizmos that were marked on press of that button.

	int button = 0;								// Button that has been pressed for longest, 0 = no button pressed
	for( int i = 1 ; i <= WG_MAX_BUTTONS ; i++ )
	{
		if( m_bButtonPressed[i] && (button == 0 || m_pLatestPressEvents[i]->Timestamp() < m_pLatestPressEvents[button]->Timestamp()) )
			button = i;
	}

	// We are only marking the Gizmo if no mouse button is pressed or the first pressed button
	// was pressed on it.

	WgGizmo * pGizmo = pGizmoTarget;

	if( button == 0 || pGizmoTarget == m_latestPressGizmos[button].GetRealPtr() )
		pNowMarked = pGizmoTarget;

	// Post POINTER_EXIT events for gizmos no longer marked,
	// Post POINTER_ENTER events for new marked gizmos
	// and POINTER_MOVE events for those already marked

	if( pNowMarked != m_pMarkedGizmo.GetRealPtr() )
	{
		if( m_pMarkedGizmo )
			QueueEvent( new WgEvent::MouseLeave( m_pMarkedGizmo.GetRealPtr() ) );
	
		if( pNowMarked )
			QueueEvent( new WgEvent::MouseEnter( pNowMarked ) );
	}
	else if( bPostMouseMoveEvents && pNowMarked )
		QueueEvent( new WgEvent::MouseMove( pNowMarked ) );

	// Copy content of pNowMarked to m_pMarkedGizmo

	m_pMarkedGizmo = pNowMarked;
	
	// Update PointerStyle
	
	WgPointerStyle newStyle = pNowMarked? pNowMarked->GetPointerStyle():WG_POINTER_DEFAULT;
	
	if( newStyle != m_pointerStyle )
	{
		QueueEvent( new WgEvent::PointerChange( newStyle ) );
		m_pointerStyle = newStyle;
	}	
	
}


//____ _processKeyPress() ______________________________________________________

void WgEventHandler::_processKeyPress( WgEvent::KeyPress * pEvent )
{
	// Fill in the info-structure.

	KeyDownInfo * pInfo = new KeyDownInfo();
	pInfo->pEvent = pEvent;

	// Post KEY_PRESS events for gizmos and remember which ones we have posted it for

	WgGizmo * pGizmo = m_keyFocusGizmo.GetRealPtr();
	QueueEvent( new WgEvent::KeyPress( pEvent->NativeKeyCode(), pGizmo ) );
	pInfo->pGizmo = pGizmo;

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

	// Post KEY_REPEAT event for gizmo

		if( pInfo->pGizmo )
			QueueEvent( new WgEvent::KeyRepeat( pEvent->NativeKeyCode(), pInfo->pGizmo.GetRealPtr() ));
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

//	assert( pInfo != 0 );			// KEY_RELEASE without a matching KEY_PRESS preceding.
	if( !pInfo )
		return;

	// Post KEY_RELEASE event for gizmo

	if( pInfo->pGizmo )
		QueueEvent( new WgEvent::KeyRelease( pEvent->NativeKeyCode(), pInfo->pGizmo.GetRealPtr() ));

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

//____ _processMouseWheelRoll() ____________________________________________________

void WgEventHandler::_processMouseWheelRoll( WgEvent::MouseWheelRoll * pEvent )
{
	_updateMarkedGizmo(false);

	WgGizmo * pGizmo = m_pMarkedGizmo.GetRealPtr();

	if( pGizmo )
		QueueEvent( new WgEvent::MouseWheelRoll( pEvent->Wheel(), pEvent->Distance(), pGizmo ) );
}

//____ _processMouseButtonPress() ___________________________________________________

void WgEventHandler::_processMouseButtonPress( WgEvent::MouseButtonPress * pEvent )
{
	_updateMarkedGizmo(false);

	int button = pEvent->Button();

	// Update m_previousPressGizmos

	m_previousPressGizmos[button] = 0;

	WgGizmo * pGizmo = m_latestPressGizmos[button].GetRealPtr();
	if( pGizmo )
		m_previousPressGizmos[button] = pGizmo;

	// Post BUTTON_PRESS events for marked gizmos and remember which one we have posted it for

	m_latestPressGizmos[button] = 0;

	pGizmo = m_pMarkedGizmo.GetRealPtr();
	if( pGizmo )
	{
		QueueEvent( new WgEvent::MouseButtonPress( button, pGizmo ) );
		m_latestPressGizmos[button] = pGizmo;
	}

	// Handle possible double-click

	int doubleClickTimeTreshold = WgBase::DoubleClickTimeTreshold();
	int doubleClickDistanceTreshold = WgBase::DoubleClickDistanceTreshold();


	if( m_pLatestPressEvents[button] && m_pLatestPressEvents[button]->Timestamp() + doubleClickTimeTreshold > pEvent->Timestamp() )
	{
		WgCoord distance = pEvent->PointerPos() - m_pLatestPressEvents[button]->PointerPos();

		if( distance.x <= doubleClickDistanceTreshold &&
			distance.x >= -doubleClickDistanceTreshold &&
			distance.y <= doubleClickDistanceTreshold &&
			distance.y >= -doubleClickDistanceTreshold )
			QueueEvent( new WgEvent::MouseButtonDoubleClick(button) );
	}

	// Save info for the future

	delete m_pLatestPressEvents[button];		// Delete previous saved event.
	m_pLatestPressEvents[button] = pEvent;

	m_bButtonPressed[button] = true;
}


//____ _processMouseButtonRepeat() __________________________________________________

void WgEventHandler::_processMouseButtonRepeat( WgEvent::MouseButtonRepeat * pEvent )
{
	_updateMarkedGizmo(false);

	int button = pEvent->Button();

	// Post BUTTON_REPEAT event for gizmo that received the press if we are still inside.

	WgGizmo * pGizmo = m_latestPressGizmos[button].GetRealPtr();
	if( pGizmo && pGizmo == m_pMarkedGizmo.GetRealPtr() )
		QueueEvent( new WgEvent::MouseButtonRepeat(button, pGizmo) );
}



//____ _processMouseButtonRelease() _________________________________________________

void WgEventHandler::_processMouseButtonRelease( WgEvent::MouseButtonRelease * pEvent )
{
	_updateMarkedGizmo(false);

	int button = pEvent->Button();

	// Post BUTTON_RELEASE events for gizmo that was pressed

	WgGizmo * pGizmo = m_latestPressGizmos[button].GetRealPtr();
	if( pGizmo )
	{
		bool bIsInside = pGizmo->ScreenGeo().Contains(pEvent->PointerPos());
		QueueEvent( new WgEvent::MouseButtonRelease( button, pGizmo, true, bIsInside ) );
	}

	// Post BUTTON_RELEASE events for marked gizmo that was NOT pressed

	pGizmo = m_pMarkedGizmo.GetRealPtr();
	if( pGizmo )
	{
		if( pGizmo != m_latestPressGizmos[button].GetRealPtr() )
		{
			bool bIsInside = pGizmo->ScreenGeo().Contains(pEvent->PointerPos());
			QueueEvent( new WgEvent::MouseButtonRelease( button, pGizmo, false, bIsInside ) );
		}
	}

	// As long as the button was pressed inside our window we have a click
	// on this level.

	if( m_bButtonPressed[button] )
		QueueEvent( new WgEvent::MouseButtonClick( button ) );

	// Save info for the future

	delete m_pLatestReleaseEvents[button];		// Delete previous saved event.
	m_pLatestReleaseEvents[button] = pEvent;
	m_bButtonPressed[button] = false;
}

//____ _processMouseButtonDrag() ____________________________________________________

void WgEventHandler::_processMouseButtonDrag( WgEvent::MouseButtonDrag * pEvent )
{
	int button = pEvent->Button();

	// Post POINTER_DRAG event for pressed gizmo

	WgGizmo * pGizmo = m_latestPressGizmos[button].GetRealPtr();

	if( pGizmo )
	{
		WgCoord	ofs = pGizmo->ScreenPos();
		QueueEvent( new WgEvent::MouseButtonDrag( button, pGizmo, pEvent->StartPos() - ofs, pEvent->PrevPos() - ofs, pEvent->CurrPos() - ofs ) );
	}

}

//____ _processMouseButtonClick() _________________________________________________

void WgEventHandler::_processMouseButtonClick( WgEvent::MouseButtonClick * pEvent )
{
	int button = pEvent->Button();

	// Post BUTTON_CLICK events for gizmo that received the press if we
	// still are inside.

	WgGizmo * pGizmo = m_latestPressGizmos[button].GetRealPtr();
	if( pGizmo && pGizmo == m_pMarkedGizmo.GetRealPtr() )
		QueueEvent( new WgEvent::MouseButtonClick(button, pGizmo) );
}

//____ _processMouseButtonDoubleClick() _________________________________________________

void WgEventHandler::_processMouseButtonDoubleClick( WgEvent::MouseButtonDoubleClick * pEvent )
{
	int button = pEvent->Button();

	// Post BUTTON_DOUBLE_CLICK event if gizom received both this and previous press.

	WgGizmo * pGizmo = m_latestPressGizmos[button].GetRealPtr();
	if( pGizmo && pGizmo ==  m_previousPressGizmos[button].GetRealPtr() )
		QueueEvent( new WgEvent::MouseButtonDoubleClick(button, pGizmo) );
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

