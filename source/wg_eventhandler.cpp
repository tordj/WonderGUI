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
#include <wg_rootpanel.h>
#include <wg_panel.h>

const char WgEventHandler::CLASSNAME[] = {"EventHandler"};


//____ Constructor ____________________________________________________________

WgEventHandler::WgEventHandler( WgRootPanel * pRoot )
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

	m_handleCounter = 1;				// We start on 1
}

//____ Destructor _____________________________________________________________

WgEventHandler::~WgEventHandler()
{
}

//____ IsInstanceOf() _________________________________________________________

bool WgEventHandler::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgObject::IsInstanceOf(pClassName);
}

//____ ClassName() ____________________________________________________________

const char * WgEventHandler::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ Cast() _________________________________________________________________

WgEventHandlerPtr WgEventHandler::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgEventHandlerPtr( static_cast<WgEventHandler*>(pObject.GetRealPtr()) );

	return 0;
}


//____ SetFocusGroup() ________________________________________________________

bool WgEventHandler::SetFocusGroup( const WgPanelPtr& pFocusGroup )
{
	// Sanity checks

	if( pFocusGroup )
	{
		if( pFocusGroup == m_keyFocusGroup )
			return true;									// Not an error, but we don't need to do anything

		if( !pFocusGroup->IsFocusGroup() )
			return false;									// Panel is not a focus group

		if( !pFocusGroup->Hook() || pFocusGroup->Hook()->Root() != m_pRoot )
			return false;									// pFocusGroup is not a child of our root.
	}

	// Set new focus widget as specified by group

	WgWidgetWeakPtr pNewFocusWidget;

	if( pFocusGroup )
		if( m_focusGroupMap.find(pFocusGroup.GetRealPtr()) != m_focusGroupMap.end() )
			pNewFocusWidget = m_focusGroupMap[pFocusGroup.GetRealPtr()];

	if( m_keyFocusWidget )
		_setWidgetFocused( m_keyFocusWidget.GetRealPtr(), false );

	if( pNewFocusWidget )
		_setWidgetFocused( pNewFocusWidget.GetRealPtr(), true );

	// Set members and exit

	m_keyFocusWidget = pNewFocusWidget;
	m_keyFocusGroup = pFocusGroup.GetRealPtr();

	return true;
}


//____ SetKeyboardFocus() _____________________________________________________

bool WgEventHandler::SetKeyboardFocus( const WgWidgetPtr& pFocus )
{
	// Return if Widget is not child of our root.

	if( pFocus && (!pFocus->Hook() || pFocus->Hook()->Root() != m_pRoot) )
		return false;

	// Handle old focus.

	WgWidget * pOldFocus = m_keyFocusWidget.GetRealPtr();

	if( pFocus == pOldFocus )
		return true;

	if( pOldFocus )
		_setWidgetFocused( pOldFocus, false );

	// Handle new focus, possibly switching focus group.

	if( pFocus )
	{
		// Check what focus group (if any) this Widget belongs to.

		m_keyFocusGroup = 0;
		WgContainer * p = pFocus->_parent();
		while( p )
		{
			if( p->_isPanel() && static_cast<WgPanel*>(p)->IsFocusGroup() )
			{
				m_keyFocusGroup = static_cast<WgPanel*>(p);
				break;
			}

			p = p->_parent();
		}

		// Activate focus

		_setWidgetFocused( pFocus.GetRealPtr(), true );
	}

	// Set members and exit.

	m_keyFocusWidget = pFocus.GetRealPtr();
	m_focusGroupMap[m_keyFocusGroup] = pFocus.GetRealPtr();

	return true;
}

//____ IsMouseButtonPressed() _________________________________________________________

bool WgEventHandler::IsMouseButtonPressed( int button ) const
{
	if( button >= 1 && button <= WG_MAX_BUTTONS )
		return m_bButtonPressed[button];

	return false;
}

//____ IsAnyMouseButtonPressed() ________________________________________________________

bool WgEventHandler::IsAnyMouseButtonPressed() const
{
	for( int i = 0 ; i < WG_MAX_BUTTONS ; i++ )
		if( m_bButtonPressed[i] )
			return true;

	return false;
}


//____ IsKeyPressed() ____________________________________________________________

bool WgEventHandler::IsKeyPressed( int native_keycode ) const
{
	for( unsigned int i = 0 ; i < m_keysDown.size() ; i++ )
		if( native_keycode == m_keysDown[i]->pEvent->NativeKeyCode() )
			return true;

	return false;
}



//____ AddCallback() __________________________________________________________

WgCallbackHandle WgEventHandler::AddCallback( void(*fp)(const WgEventPtr& pEvent) )
{
	Callback * p = new NoParamCallback( WgEventFilter(), fp );
	return _addCallback( p );
}

WgCallbackHandle WgEventHandler::AddCallback( void(*fp)(const WgEventPtr& pEvent, int param), int param )
{
	Callback * p = new IntParamCallback( WgEventFilter(), fp, param );
	return _addCallback( p );
}

WgCallbackHandle WgEventHandler::AddCallback( void(*fp)(const WgEventPtr& pEvent, void * pParam), void * pParam )
{
	Callback * p = new VoidPtrParamCallback( WgEventFilter(), fp, pParam );
	return _addCallback( p );
}

WgCallbackHandle WgEventHandler::AddCallback( void(*fp)(const WgEventPtr& pEvent, const WgObjectPtr& pParam), const WgObjectPtr& pParam )
{
	Callback * p = new ObjectParamCallback( WgEventFilter(), fp, pParam.GetRealPtr() );
	return _addCallback( p );
}

WgCallbackHandle WgEventHandler::AddCallback( WgEventListener * pListener )
{
	Callback * p = new ListenerCallback( WgEventFilter(), pListener );
	return _addCallback( p );
}

WgCallbackHandle WgEventHandler::AddCallback( const WgEventFilter& filter, void(*fp)(const WgEventPtr& pEvent) )
{
	Callback * p = new NoParamCallback( filter, fp );
	return _addCallback( filter, p );
}

WgCallbackHandle WgEventHandler::AddCallback( const WgEventFilter& filter, void(*fp)(const WgEventPtr& pEvent, int param), int param )
{
	Callback * p = new IntParamCallback( filter, fp, param );
	return _addCallback( filter, p );
}

WgCallbackHandle WgEventHandler::AddCallback( const WgEventFilter& filter, void(*fp)(const WgEventPtr& pEvent, void * pParam), void * pParam )
{
	Callback * p = new VoidPtrParamCallback( filter, fp, pParam );
	return _addCallback( filter, p );
}

WgCallbackHandle WgEventHandler::AddCallback( const WgEventFilter& filter, void(*fp)(const WgEventPtr& pEvent, const WgObjectPtr& pParam), const WgObjectPtr& pParam )
{
	Callback * p = new ObjectParamCallback( filter, fp, pParam );
	return _addCallback( filter, p );
}

WgCallbackHandle WgEventHandler::AddCallback( const WgEventFilter& filter, WgEventListener * pListener )
{
	Callback * p = new ListenerCallback( filter, pListener );
	return _addCallback( filter, p );
}

//____ DeleteCallbacksTo() _______________________________________________________

int WgEventHandler::DeleteCallbacksTo( void(*fp)( const WgEventPtr& pEvent) )
{
	return _deleteCallbacksTo( reinterpret_cast<void*>(fp) );
}

int WgEventHandler::DeleteCallbacksTo( void(*fp)( const WgEventPtr& pEvent, void * pParam) )
{
	return _deleteCallbacksTo( reinterpret_cast<void*>(fp) );
}

int WgEventHandler::DeleteCallbacksTo( const WgEventListener * pListener )
{
	return _deleteCallbacksTo(pListener);
}

int WgEventHandler::DeleteCallbacksTo( void(*fp)( const WgEventPtr& pEvent, const WgObjectPtr& pParam) )
{
	return _deleteCallbacksTo( reinterpret_cast<void*>(fp) );
}

//____ DeleteCallbacksOn() _______________________________________________________

int WgEventHandler::DeleteCallbacksOn( const WgWidgetPtr& pWidget )
{
	std::map<WgWidgetWeakPtr,WgChain<Callback> >::iterator it = m_widgetCallbacks.find(WgWidgetWeakPtr(pWidget.GetRealPtr()) );

	if( it == m_widgetCallbacks.end() )
		return 0;

	int nDeleted = it->second.Size();
	m_widgetCallbacks.erase(it);
	return nDeleted;
}

int WgEventHandler::DeleteCallbacksOn( WgEventType type )
{
	// Delete global callbacks on this event type

	int nDeleted = _deleteCallbacksOnType( type, &m_globalCallbacks );

	// Delete Widget-specific callbacks on this event type

	std::map<WgWidgetWeakPtr,WgChain<Callback> >::iterator it = m_widgetCallbacks.begin();
	while( it != m_widgetCallbacks.end() )
	{
		nDeleted += _deleteCallbacksOnType( type, &it->second );

		if( it->second.Size() == 0 )
			m_widgetCallbacks.erase(it++);
		else
			++it;
	}
	return nDeleted;
}


int WgEventHandler::DeleteCallbacksOn( const WgWidgetPtr& pWidget, WgEventType type )
{
	std::map<WgWidgetWeakPtr,WgChain<Callback> >::iterator it = m_widgetCallbacks.find( WgWidgetWeakPtr(pWidget.GetRealPtr()) );

	if( it == m_widgetCallbacks.end() )
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
		m_widgetCallbacks.erase(it);

	return nDeleted;
}



//____ DeleteCallback() ______________________________________________________

bool WgEventHandler::DeleteCallback( WgCallbackHandle handle )
{
	Callback * p = m_globalCallbacks.First();
	while( p )
	{
		if( p->m_handle == handle )
			delete p;
			return true;
			p = p->Next();
	}

	std::map<WgWidgetWeakPtr,WgChain<Callback> >::iterator	it;
	for( it = m_widgetCallbacks.begin() ; it != m_widgetCallbacks.end() ; ++it )
	{
		Callback * p = it->second.First();
		while( p )
		{
			if( p->m_handle == handle )
				delete p;
				return true;
				p = p->Next();
		}
	}

	return false;
}

//____ DeleteAllCallbacks() ___________________________________________________

int WgEventHandler::DeleteAllCallbacks()
{
	m_globalCallbacks.Clear();
	m_widgetCallbacks.clear();
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

	// Delete any dead Widget-specific callbacks.
	// These can be dead by either sender or receiver having been deleted.

	std::map<WgWidgetWeakPtr,WgChain<Callback> >::iterator it = m_widgetCallbacks.begin();

	while( it != m_widgetCallbacks.end() )
	{
		if( !it->first )
		{
			nDeleted += it->second.Size();
			m_widgetCallbacks.erase(it++);		// Sender is dead, delete whole branch of callbacks.
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

WgCallbackHandle WgEventHandler::_addCallback( Callback * pCallback )
{
	m_globalCallbacks.PushBack( pCallback );
	pCallback->m_handle = m_handleCounter++;
	return pCallback->m_handle;
}

WgCallbackHandle WgEventHandler::_addCallback( const WgEventFilter& filter, Callback * pCallback )
{
	if( filter.FiltersWidget() )
	{
		WgWidgetWeakPtr pWidget = filter.Widget();

		WgChain<Callback>& chain = m_widgetCallbacks[pWidget];

		chain.PushBack(pCallback);
	}
	else
		m_globalCallbacks.PushBack( pCallback );

	pCallback->m_handle = m_handleCounter++;
	return pCallback->m_handle;
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

	// Delete any Widget-specific callbacks for this receiver.

	std::map<WgWidgetWeakPtr,WgChain<Callback> >::iterator it = m_widgetCallbacks.begin();

	while( it != m_widgetCallbacks.end() )
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

//____ QueueEvent() ___________________________________________________________

bool WgEventHandler::QueueEvent( const WgEventPtr& pEvent )
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
	int64_t	time = m_time;

	m_bIsProcessing = true;

	// First thing: we make sure that we know what Widgets pointer is inside, in case that has changed.

	m_insertPos = m_eventQueue.begin();	// Insert any POINTER_ENTER/EXIT right at beginning.
	_updateMarkedWidget(false);

	// Process all the events in the queue

	_processEventQueue();

	// Post Widget-specific tick events now we know how much time has passed

	_postTickEvents( (int) (m_time-time) );

	// Process Widget-specific tick events (and any events they might trigger)

	_processEventQueue();

	m_bIsProcessing = false;
}

//____ SwallowEvent() _________________________________________________________

bool WgEventHandler::SwallowEvent( const WgEventPtr& pEvent )
{
	if( pEvent != m_pEventProcessing )
		return false;

	m_pNextEventReceiver = 0;
	return true;
}

//____ ForwardEvent() _________________________________________________________

bool WgEventHandler::ForwardEvent( const WgEventPtr& pEvent, const WgWidgetPtr& pWidget )
{
	if( pEvent != m_pEventProcessing )
		return false;

	m_pNextEventReceiver = pWidget;
	return true;
}


//____ _processEventQueue() ___________________________________________________

void WgEventHandler::_processEventQueue()
{
	while( !m_eventQueue.empty() )
	{
		WgEventPtr& pEvent = m_eventQueue.front();
		m_pEventProcessing = pEvent;
		m_pNextEventReceiver = 0;

		m_insertPos = m_eventQueue.begin()+1;	// Insert position set to right after current event.

		_finalizeEvent( pEvent );

		if( pEvent->IsFromWidget() )
		{
			_processGlobalEventCallbacks( pEvent );

			WgWidget * pWidget = pEvent->Widget();
			while( pWidget )
			{
				if( pEvent->Type() == WG_EVENT_TICK )
					m_pNextEventReceiver = 0;
				else
					m_pNextEventReceiver = pWidget->Parent();

				pWidget->_onEvent( pEvent, this );
				_processWidgetEventCallbacks( pEvent, pWidget );
				pWidget = m_pNextEventReceiver.GetRealPtr();
			}
		}
		else
		{
			_processGeneralEvent( pEvent );
			_processGlobalEventCallbacks( pEvent );
		}

		m_eventQueue.pop_front();
		m_insertPos = m_eventQueue.begin();		// Insert position set right to start.
	}
}

//____ _postTickEvents() ______________________________________________________

void WgEventHandler::_postTickEvents( int ticks )
{
	std::vector<WgWidgetWeakPtr>::iterator it = m_vTickWidgets.begin();

	while( it != m_vTickWidgets.end() )
	{
		WgWidget * pWidget = (*it).GetRealPtr();

		if( pWidget && pWidget->Hook() && pWidget->Hook()->Root() == m_pRoot && pWidget->m_bReceiveTick )
		{
			QueueEvent( new WgTickEvent( ticks, pWidget ) );
			++it;
		}
		else
			it = m_vTickWidgets.erase(it);
	}
}

//____ _addTickReceiver() _____________________________________________________

void WgEventHandler::_addTickReceiver( WgWidget * pWidget )
{
	if( pWidget && !_isWidgetInList( pWidget, m_vTickWidgets ) )
		m_vTickWidgets.push_back( WgWidgetWeakPtr(pWidget) );
}

//____ _processGlobalEventCallbacks() ________________________________________________

void WgEventHandler::_processGlobalEventCallbacks( const WgEventPtr& pEvent )
{
	Callback * pCallback = m_globalCallbacks.First();

	while( pCallback )
	{
		pCallback->ProcessEvent( pEvent );
		pCallback = pCallback->Next();
	}
}


//____ _processWidgetEventCallbacks() ________________________________________________

void WgEventHandler::_processWidgetEventCallbacks( const WgEventPtr& pEvent, WgWidget * pWidget )
{
	Callback * pCallback = 0;
	WgChain<Callback> * pChain = 0;

	if( pEvent->Widget() )
	{
		std::map<WgWidgetWeakPtr,WgChain<Callback> >::iterator it;

		it = m_widgetCallbacks.find(pEvent->WidgetWeakPtr());
		if( it != m_widgetCallbacks.end() )
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

void WgEventHandler::_finalizeEvent( const WgEventPtr& pEvent )
{
	// Fill in missing information in the event-class.

	pEvent->m_timestamp	= m_time;
	pEvent->m_modKeys	= m_modKeys;

	// Only global POINTER_ENTER & POINTER_MOVE events have these members
	// set, the rest needs to have them filled in.

	if( pEvent->IsFromWidget() || (pEvent->Type() != WG_EVENT_MOUSE_MOVE && pEvent->Type() != WG_EVENT_MOUSE_ENTER) )
	{
		pEvent->m_pointerScreenPos = m_pointerPos;
		pEvent->m_pointerLocalPos = m_pointerPos;

		if( pEvent->Widget() )
			pEvent->m_pointerLocalPos -= pEvent->Widget()->ScreenPos();
	}

	// Event specific finalizations

	switch( pEvent->Type() )
	{

		// Key events need translation of keycodes.

		case WG_EVENT_KEY_PRESS:
		case WG_EVENT_KEY_RELEASE:
		case WG_EVENT_KEY_REPEAT:
		{
			WgKeyEvent* p = static_cast<WgKeyEvent*>(pEvent.GetRealPtr());
			p->m_translatedKeyCode = WgBase::TranslateKey(p->m_nativeKeyCode);
		}
		break;


		default:
			break;
	}



}

//____ _processGeneralEvent() _________________________________________________

void WgEventHandler::_processGeneralEvent( const WgEventPtr& _pEvent )
{
	WgEvent * pEvent = _pEvent.GetRealPtr();

	switch( pEvent->Type() )
	{
		case WG_EVENT_FOCUS_GAINED:
			_processFocusGained( (WgFocusGainedEvent*) pEvent );
			break;

		case WG_EVENT_FOCUS_LOST:
			_processFocusLost( (WgFocusLostEvent*) pEvent );
			break;

		case WG_EVENT_MOUSE_ENTER:
			_processMouseEnter( (WgMouseEnterEvent*) pEvent );
			break;

		case WG_EVENT_MOUSE_MOVE:
			_processMouseMove( (WgMouseMoveEvent*) pEvent );
			break;

		case WG_EVENT_MOUSE_POSITION:
			_processMousePosition( (WgMousePositionEvent*) pEvent );
			break;

		case WG_EVENT_MOUSE_LEAVE:
			_processMouseLeave( (WgMouseLeaveEvent*) pEvent );
			break;

		case WG_EVENT_MOUSE_PRESS:
			_processMouseButtonPress( (WgMousePressEvent*) pEvent );
			break;

		case WG_EVENT_MOUSE_REPEAT:
			_processMouseButtonRepeat( (WgMouseRepeatEvent*) pEvent );
			break;

		case WG_EVENT_MOUSE_DRAG:
			_processMouseButtonDrag( (WgMouseDragEvent*) pEvent );
			break;

		case WG_EVENT_MOUSE_RELEASE:
			_processMouseButtonRelease( (WgMouseReleaseEvent*) pEvent );
			break;

		case WG_EVENT_MOUSE_CLICK:
			_processMouseButtonClick( (WgMouseClickEvent*) pEvent );
			break;

		case WG_EVENT_MOUSE_DOUBLE_CLICK:
			_processMouseButtonDoubleClick( (WgMouseDoubleClickEvent*) pEvent );
			break;

		case WG_EVENT_KEY_PRESS:
			_processKeyPress( (WgKeyPressEvent*) pEvent );
			break;

		case WG_EVENT_KEY_REPEAT:
			_processKeyRepeat( (WgKeyRepeatEvent*) pEvent );
			break;

		case WG_EVENT_KEY_RELEASE:
			_processKeyRelease( (WgKeyReleaseEvent*) pEvent );
			break;

		case WG_EVENT_CHARACTER:
			_processCharacter( (WgCharacterEvent*) pEvent );
			break;

		case WG_EVENT_WHEEL_ROLL:
			_processMouseWheelRoll( (WgWheelRollEvent*) pEvent );
			break;

		case WG_EVENT_TICK:
			_processTick( (WgTickEvent*) pEvent );
			break;

		case WG_EVENT_DUMMY:
			break;

		case WG_EVENT_MAX:
			assert(false);								// NOT ALLOWED!
			break;
	}
}

//____ _processTick() ______________________________________________________

void WgEventHandler::_processTick( WgTickEvent * pEvent )
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
				QueueEvent( new WgMouseRepeatEvent((WgMouseButton)button) );

			// Calculate ms since last BUTTON_REPEAT event

			int msToProcess;
			if( msSinceRepeatStart < 0 )
				msToProcess = msSinceRepeatStart + pEvent->Millisec();
			else
				msToProcess = (msSinceRepeatStart % buttonRate) + pEvent->Millisec();

			// Post the amount of BUTTON_REPEAT that should be posted.

			while( msToProcess >= buttonRate )
			{
				QueueEvent( new WgMouseRepeatEvent((WgMouseButton)button) );
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
			QueueEvent( new WgKeyRepeatEvent( pInfo->pEvent->NativeKeyCode() ) );
			fraction -= keyRate;
		}
	}


	// Increase time counter

	m_time += pEvent->Millisec();
}


//____ _processFocusGained() __________________________________________________

void WgEventHandler::_processFocusGained( WgFocusGainedEvent * pEvent )
{
	if( !m_bWindowFocus && m_keyFocusWidget )
		_setWidgetFocused( m_keyFocusWidget.GetRealPtr(), true );

	m_bWindowFocus = true;
}

//____ _processFocusLost() ____________________________________________________

void WgEventHandler::_processFocusLost( WgFocusLostEvent * pEvent )
{
	if( m_bWindowFocus && m_keyFocusWidget )
		_setWidgetFocused( m_keyFocusWidget.GetRealPtr(), false );

	m_bWindowFocus = false;
}


//____ _processMouseEnter() __________________________________________________

void WgEventHandler::_processMouseEnter( WgMouseEnterEvent * pEvent )
{
	// Post events for button drag

	for( int i = 0 ; i <= WG_MAX_BUTTONS ; i++ )
	{
		if( m_bButtonPressed[i] )
			QueueEvent( new WgMouseDragEvent( (WgMouseButton)i, m_pLatestPressEvents[i]->PointerPos(), m_pointerPos, pEvent->PointerPos() ) );
	}

	// Post event for finalizing position once button drag is taken care of.

	QueueEvent( new WgMousePositionEvent() );

	// Update pointer position

	m_pointerPos = pEvent->PointerPos();
}

//____ _processMouseLeave() ___________________________________________________

void WgEventHandler::_processMouseLeave( WgMouseLeaveEvent * pEvent )
{
	// Post POINTER_EXIT event to marked widget

	WgWidget * pWidget = m_pMarkedWidget.GetRealPtr();

	if( pWidget )
		QueueEvent( new WgMouseLeaveEvent( pWidget ) );

	m_pMarkedWidget = 0;
}


//____ _processMouseMove() ___________________________________________________

void WgEventHandler::_processMouseMove( WgMouseMoveEvent * pEvent )
{
	// Post events for button drag

	for( int i = 0 ; i <= WG_MAX_BUTTONS ; i++ )
	{
		if( m_bButtonPressed[i] )
			QueueEvent( new WgMouseDragEvent( (WgMouseButton) i, m_pLatestPressEvents[i]->PointerPos(), m_pointerPos, pEvent->PointerPos() ) );
	}

	// Post event for finalizing move once button drag is taken care of.

	QueueEvent( new WgMousePositionEvent() );

	// Update pointer position

	m_pointerPos = pEvent->PointerPos();
}

//____ _processMousePosition() _________________________________________________

void WgEventHandler::_processMousePosition( WgMousePositionEvent * pEvent )
{
	_updateMarkedWidget(true);

}

//____ _setWidgetFocused() ____________________________________________________

void WgEventHandler::_setWidgetFocused( WgWidget * pWidget, bool bFocused )
{
		WgState oldState = pWidget->m_state;
		pWidget->m_state.SetFocused(bFocused);

		if( pWidget->m_state != oldState )
			pWidget->_onStateChanged(oldState);
}


//____ _updateMarkedWidget() _______________________________________________

void WgEventHandler::_updateMarkedWidget(bool bPostMouseMoveEvents)
{
	WgWidgetPtr pNowMarked = 0;

	WgWidgetPtr pWidgetTarget = m_pRoot->FindWidget( m_pointerPos, WG_SEARCH_ACTION_TARGET );

	// Figure out which button of currently pressed has been pressed the longest.
	// Mouse is only allowed to mark Widgets that were marked on press of that button.

	int button = 0;								// Button that has been pressed for longest, 0 = no button pressed
	for( int i = 1 ; i <= WG_MAX_BUTTONS ; i++ )
	{
		if( m_bButtonPressed[i] && (button == 0 || m_pLatestPressEvents[i]->Timestamp() < m_pLatestPressEvents[button]->Timestamp()) )
			button = i;
	}

	// We are only marking the Widget if no mouse button is pressed or the first pressed button
	// was pressed on it.

	if( button == 0 || pWidgetTarget == m_latestPressWidgets[button].GetRealPtr() )
		pNowMarked = pWidgetTarget;

	// Post POINTER_EXIT events for widgets no longer marked,
	// Post POINTER_ENTER events for new marked widgets
	// and POINTER_MOVE events for those already marked

	if( pNowMarked != m_pMarkedWidget.GetRealPtr() )
	{
		// We send new enter events before leave. Containers are depending on this order to
		// know if they or one of their children still are marked when they receive a leave event.

		if( pNowMarked )
			QueueEvent( new WgMouseEnterEvent( pNowMarked.GetRealPtr() ) );

		if( m_pMarkedWidget )
			QueueEvent( new WgMouseLeaveEvent( m_pMarkedWidget.GetRealPtr() ) );	
	}
	else if( bPostMouseMoveEvents && pNowMarked )
		QueueEvent( new WgMouseMoveEvent( pNowMarked.GetRealPtr() ) );

	// Copy content of pNowMarked to m_pMarkedWidget

	m_pMarkedWidget = pNowMarked.GetRealPtr();
	
	// Update PointerStyle
	
	WgPointerStyle newStyle;
	
	if( pNowMarked && pNowMarked->IsEnabled() )
		newStyle = pNowMarked->PointerStyle();
	else if( button != 0 )
		newStyle = m_pointerStyle;
	else
		newStyle = WG_POINTER_DEFAULT;

	if( newStyle != m_pointerStyle )
	{
		QueueEvent( new WgPointerChangeEvent( newStyle ) );
		m_pointerStyle = newStyle;
	}	
	
}


//____ _processKeyPress() ______________________________________________________

void WgEventHandler::_processKeyPress( WgKeyPressEvent * pEvent )
{
	// Fill in the info-structure.

	KeyDownInfo * pInfo = new KeyDownInfo();
	pInfo->pEvent = pEvent;

	// Post KEY_PRESS events for widgets and remember which ones we have posted it for

	WgWidget * pWidget = m_keyFocusWidget.GetRealPtr();
	QueueEvent( new WgKeyPressEvent( pEvent->NativeKeyCode(), pWidget ) );
	pInfo->pWidget = pWidget;

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

void WgEventHandler::_processKeyRepeat( WgKeyRepeatEvent * pEvent )
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

	// Post KEY_REPEAT event for widget

		if( pInfo->pWidget )
			QueueEvent( new WgKeyRepeatEvent( pEvent->NativeKeyCode(), pInfo->pWidget.GetRealPtr() ));
}

//____ _processKeyRelease() ___________________________________________________

void WgEventHandler::_processKeyRelease( WgKeyReleaseEvent * pEvent )
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

	// Post KEY_RELEASE event for widget

	if( pInfo->pWidget )
		QueueEvent( new WgKeyReleaseEvent( pEvent->NativeKeyCode(), pInfo->pWidget.GetRealPtr() ));

	// Delete the KeyPress-message and KeyDownInfo-structure

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

void WgEventHandler::_processCharacter( WgCharacterEvent * pEvent )
{
	WgWidget * pWidget = m_keyFocusWidget.GetRealPtr();

	if( pWidget )
		QueueEvent( new WgCharacterEvent( pEvent->Char(), pWidget ) );
}

//____ _processMouseWheelRoll() ____________________________________________________

void WgEventHandler::_processMouseWheelRoll( WgWheelRollEvent * pEvent )
{
	_updateMarkedWidget(false);

	WgWidget * pWidget = m_pMarkedWidget.GetRealPtr();

	if( pWidget )
		QueueEvent( new WgWheelRollEvent( pEvent->Wheel(), pEvent->Distance(), pWidget ) );
}

//____ _processMouseButtonPress() ___________________________________________________

void WgEventHandler::_processMouseButtonPress( WgMousePressEvent * pEvent )
{
	_updateMarkedWidget(false);

	WgMouseButton button = pEvent->Button();

	// Update m_previousPressWidgets

	m_previousPressWidgets[button] = 0;

	WgWidget * pWidget = m_latestPressWidgets[button].GetRealPtr();
	if( pWidget )
		m_previousPressWidgets[button] = pWidget;

	// Post BUTTON_PRESS events for marked widgets and remember which one we have posted it for

	m_latestPressWidgets[button] = 0;

	pWidget = m_pMarkedWidget.GetRealPtr();
	if( pWidget )
	{
		QueueEvent( new WgMousePressEvent( button, pWidget ) );
		m_latestPressWidgets[button] = pWidget;
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
			QueueEvent( new WgMouseDoubleClickEvent(button) );
	}

	// Save info for the future

	m_pLatestPressEvents[button] = pEvent;

	m_bButtonPressed[button] = true;
}


//____ _processMouseButtonRepeat() __________________________________________________

void WgEventHandler::_processMouseButtonRepeat( WgMouseRepeatEvent * pEvent )
{
	_updateMarkedWidget(false);

	WgMouseButton button = pEvent->Button();

	// Post BUTTON_REPEAT event for widget that received the press if we are still inside.

	WgWidget * pWidget = m_latestPressWidgets[button].GetRealPtr();
	if( pWidget && pWidget == m_pMarkedWidget.GetRealPtr() )
		QueueEvent( new WgMouseRepeatEvent(button, pWidget) );
}



//____ _processMouseButtonRelease() _________________________________________________

void WgEventHandler::_processMouseButtonRelease( WgMouseReleaseEvent * pEvent )
{
	_updateMarkedWidget(false);

	WgMouseButton button = pEvent->Button();

	// Post BUTTON_RELEASE events for widget that was pressed

	WgWidget * pWidget = m_latestPressWidgets[button].GetRealPtr();
	if( pWidget )
	{
		bool bIsInside = pWidget->ScreenGeo().Contains(pEvent->PointerPos());
		QueueEvent( new WgMouseReleaseEvent( button, pWidget, true, bIsInside ) );
	}

	// Post BUTTON_RELEASE events for marked widget that was NOT pressed

	pWidget = m_pMarkedWidget.GetRealPtr();
	if( pWidget )
	{
		if( pWidget != m_latestPressWidgets[button].GetRealPtr() )
		{
			bool bIsInside = pWidget->ScreenGeo().Contains(pEvent->PointerPos());
			QueueEvent( new WgMouseReleaseEvent( button, pWidget, false, bIsInside ) );
		}
	}

	// As long as the button was pressed inside our window we have a click
	// on this level.

	if( m_bButtonPressed[button] )
		QueueEvent( new WgMouseClickEvent( button ) );

	// Save info for the future

	m_pLatestReleaseEvents[button] = pEvent;
	m_bButtonPressed[button] = false;
}

//____ _processMouseButtonDrag() ____________________________________________________

void WgEventHandler::_processMouseButtonDrag( WgMouseDragEvent * pEvent )
{
	WgMouseButton button = pEvent->Button();

	// Post POINTER_DRAG event for pressed widget

	WgWidget * pWidget = m_latestPressWidgets[button].GetRealPtr();

	if( pWidget )
	{
		WgCoord	ofs = pWidget->ScreenPos();
		QueueEvent( new WgMouseDragEvent( button, pWidget, pEvent->StartPos() - ofs, pEvent->PrevPos() - ofs, pEvent->CurrPos() - ofs ) );
	}

}

//____ _processMouseButtonClick() _________________________________________________

void WgEventHandler::_processMouseButtonClick( WgMouseClickEvent * pEvent )
{
	WgMouseButton button = pEvent->Button();

	// Post BUTTON_CLICK events for widget that received the press if we
	// still are inside.

	WgWidget * pWidget = m_latestPressWidgets[button].GetRealPtr();
	if( pWidget && pWidget == m_pMarkedWidget.GetRealPtr() )
		QueueEvent( new WgMouseClickEvent(button, pWidget) );
}

//____ _processMouseButtonDoubleClick() _________________________________________________

void WgEventHandler::_processMouseButtonDoubleClick( WgMouseDoubleClickEvent * pEvent )
{
	WgMouseButton button = pEvent->Button();

	// Post BUTTON_DOUBLE_CLICK event if gizom received both this and previous press.

	WgWidget * pWidget = m_latestPressWidgets[button].GetRealPtr();
	if( pWidget && pWidget ==  m_previousPressWidgets[button].GetRealPtr() )
		QueueEvent( new WgMouseDoubleClickEvent(button, pWidget) );
}

//____ _isWidgetInList() ________________________________________________________

bool WgEventHandler::_isWidgetInList( const WgWidget * pWidget, const std::vector<WgWidgetWeakPtr>& list )
{
	for( size_t i = 0 ; i < list.size() ; i++ )
		if( list[i].GetRealPtr() == pWidget )
			return true;

	return false;
}





WgEventHandler::ObjectParamCallback::ObjectParamCallback( const WgEventFilter& filter, void(*fp)(const WgEventPtr& pEvent, const WgObjectPtr& pParam), const WgObjectPtr& pParam )
{
	m_filter = filter;
	m_pFunction = fp;
	m_pObject = pParam;
}

void WgEventHandler::ObjectParamCallback::ProcessEvent( const WgEventPtr& pEvent )
{
	WgObject * p = m_pObject.GetRealPtr();
	if( p && m_filter.FilterEvent(pEvent) )
		m_pFunction(pEvent,p);
}

bool WgEventHandler::ObjectParamCallback::IsAlive() const
{
	return m_pObject?true:false;
}

void * WgEventHandler::ObjectParamCallback::Receiver() const
{
	return m_pObject.GetRealPtr();
}


WgEventHandler::NoParamCallback::NoParamCallback( const WgEventFilter& filter, void(*fp)(const WgEventPtr& pEvent) )
{
	m_filter = filter;
	m_pFunction = fp;
}

void WgEventHandler::NoParamCallback::ProcessEvent( const WgEventPtr& pEvent )
{
	if( m_pFunction && m_filter.FilterEvent(pEvent) )
		m_pFunction(pEvent);
}

bool WgEventHandler::NoParamCallback::IsAlive() const
{
	return true;
}

void * WgEventHandler::NoParamCallback::Receiver() const
{
	return reinterpret_cast<void*>(m_pFunction);
}

WgEventHandler::IntParamCallback::IntParamCallback( const WgEventFilter& filter, void(*fp)(const WgEventPtr& pEvent, int param), int param )
{
	m_filter = filter;
	m_pFunction = fp;
	m_param 	= param;
}

void WgEventHandler::IntParamCallback::ProcessEvent( const WgEventPtr& pEvent )
{
	if( m_pFunction && m_filter.FilterEvent(pEvent) )
		m_pFunction(pEvent,m_param);
}

bool WgEventHandler::IntParamCallback::IsAlive() const
{
	return true;
}

void * WgEventHandler::IntParamCallback::Receiver() const
{
	return reinterpret_cast<void*>(m_pFunction);
}


WgEventHandler::VoidPtrParamCallback::VoidPtrParamCallback( const WgEventFilter& filter, void(*fp)(const WgEventPtr& pEvent, void * pParam), void * pParam )
{
	m_filter = filter;
	m_pFunction = fp;
	m_pParam 	= pParam;
}

void WgEventHandler::VoidPtrParamCallback::ProcessEvent( const WgEventPtr& pEvent )
{
	if( m_pFunction && m_filter.FilterEvent(pEvent) )
		m_pFunction(pEvent,m_pParam);
}

bool WgEventHandler::VoidPtrParamCallback::IsAlive() const
{
	return true;
}

void * WgEventHandler::VoidPtrParamCallback::Receiver() const
{
	return reinterpret_cast<void*>(m_pFunction);
}


WgEventHandler::ListenerCallback::ListenerCallback( const WgEventFilter& filter, WgEventListener * pListener )
{
	m_filter = filter;
	m_pListener = pListener;
}

void WgEventHandler::ListenerCallback::ProcessEvent( const WgEventPtr& pEvent )
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

