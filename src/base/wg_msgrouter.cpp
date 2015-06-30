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
#include <wg_msg.h>
#include <wg_msgfilter.h>
#include <wg_msgrouter.h>
#include <wg_base.h>
#include <wg_rootpanel.h>
#include <wg_panel.h>

const char WgMsgRouter::CLASSNAME[] = {"MsgRouter"};


//____ Constructor ____________________________________________________________

WgMsgRouter::WgMsgRouter()
{
	m_pRoot					= 0;
	m_time					= 0;
	m_modKeys				= WG_MODKEY_NONE;
	m_pointerStyle			= WG_POINTER_DEFAULT;

	m_bIsProcessing			= false;
	m_bWindowFocus			= true;

	for( int i = 0 ; i <= WG_MAX_BUTTONS ; i++ )
	{
		m_bButtonPressed[i] = 0;
		m_pLatestPressMsgs[i] = 0;
		m_pLatestReleaseMsgs[i] = 0;
	}

	m_routeCounter = 1;				// We start on 1
}

//____ Destructor _____________________________________________________________

WgMsgRouter::~WgMsgRouter()
{
}

//____ IsInstanceOf() _________________________________________________________

bool WgMsgRouter::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgObject::IsInstanceOf(pClassName);
}

//____ ClassName() ____________________________________________________________

const char * WgMsgRouter::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ Cast() _________________________________________________________________

WgMsgRouterPtr WgMsgRouter::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgMsgRouterPtr( static_cast<WgMsgRouter*>(pObject.RawPtr()) );

	return 0;
}


void WgMsgRouter::SetRoot( const WgRootPanelPtr& pRoot )
{
	m_pRoot = pRoot.RawPtr();
}


//____ SetFocusGroup() ________________________________________________________

bool WgMsgRouter::SetFocusGroup( const WgPanelPtr& pFocusGroup )
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
		if( m_focusGroupMap.find(pFocusGroup.RawPtr()) != m_focusGroupMap.end() )
			pNewFocusWidget = m_focusGroupMap[pFocusGroup.RawPtr()];

	if( m_keyFocusWidget )
		_setWidgetFocused( m_keyFocusWidget.RawPtr(), false );

	if( pNewFocusWidget )
		_setWidgetFocused( pNewFocusWidget.RawPtr(), true );

	// Set members and exit

	m_keyFocusWidget = pNewFocusWidget;
	m_keyFocusGroup = pFocusGroup.RawPtr();

	return true;
}


//____ SetKeyboardFocus() _____________________________________________________

bool WgMsgRouter::SetKeyboardFocus( const WgWidgetPtr& pFocus )
{
	// Return if Widget is not child of our root.

	if( pFocus && (!pFocus->Hook() || pFocus->Hook()->Root() != m_pRoot) )
		return false;

	// Handle old focus.

	WgWidget * pOldFocus = m_keyFocusWidget.RawPtr();

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

		_setWidgetFocused( pFocus.RawPtr(), true );
	}

	// Set members and exit.

	m_keyFocusWidget = pFocus.RawPtr();
	m_focusGroupMap[m_keyFocusGroup] = pFocus.RawPtr();

	return true;
}

//____ IsMouseButtonPressed() _________________________________________________________

bool WgMsgRouter::IsMouseButtonPressed( int button ) const
{
	if( button >= 1 && button <= WG_MAX_BUTTONS )
		return m_bButtonPressed[button];

	return false;
}

//____ IsAnyMouseButtonPressed() ________________________________________________________

bool WgMsgRouter::IsAnyMouseButtonPressed() const
{
	for( int i = 0 ; i < WG_MAX_BUTTONS ; i++ )
		if( m_bButtonPressed[i] )
			return true;

	return false;
}


//____ IsKeyPressed() ____________________________________________________________

bool WgMsgRouter::IsKeyPressed( int native_keycode ) const
{
	for( unsigned int i = 0 ; i < m_keysDown.size() ; i++ )
		if( native_keycode == m_keysDown[i]->pMsg->NativeKeyCode() )
			return true;

	return false;
}

//____ BroadcastTo() ___________________________________________________________

bool  WgMsgRouter::BroadcastTo( const WgReceiverPtr& pReceiver )
{
	Route * p = new Route( WgMsgFilter(), pReceiver.RawPtr() );
	m_broadcasts.PushBack( p );
	return true;
}

bool  WgMsgRouter::BroadcastTo( const WgMsgFilter& filter, const WgReceiverPtr& pReceiver )
{
	Route * p = new Route( filter, pReceiver.RawPtr() );
	m_broadcasts.PushBack( p );
	return true;
}

//____ EndBroadcast() __________________________________________________________

bool  WgMsgRouter::EndBroadcast( const WgReceiverPtr& _pReceiver )
{
	WgReceiver * pReceiver = _pReceiver.RawPtr();
	Route * p = m_broadcasts.First();
	while( p )
	{
		if( p->Receiver() == pReceiver )
		{
			delete p;
			return true;
		}
		
		p = p->Next();
	}	
	return false;
}


//____ AddRoute() __________________________________________________________

WgRouteId WgMsgRouter::AddRoute( const WgObjectPtr& pSource, const WgReceiverPtr& pReceiver )
{
	Route * p = new Route( WgMsgFilter(), pReceiver.RawPtr() );
	return _addRoute( pSource, p );
}

WgRouteId WgMsgRouter::AddRoute( const WgMsgFilter& filter, const WgObjectPtr& pSource, const WgReceiverPtr& pReceiver )
{
	Route * p = new Route( filter, pReceiver.RawPtr() );
	return _addRoute( pSource, p );
}

WgRouteId WgMsgRouter::AddRoute( WgMsgType msgType, const WgReceiverPtr& pReceiver )
{
	Route * p = new Route( WgMsgFilter(), pReceiver.RawPtr() );
	return _addRoute( msgType, p );	
}

WgRouteId WgMsgRouter::AddRoute( const WgMsgFilter& filter, WgMsgType msgType, const WgReceiverPtr& pReceiver )
{
	Route * p = new Route( filter, pReceiver.RawPtr() );
	return _addRoute( msgType, p );	
}



//____ DeleteRoutesTo() _______________________________________________________

int WgMsgRouter::DeleteRoutesTo( const WgReceiverPtr& _pReceiver )
{
	WgReceiver * pReceiver = _pReceiver.RawPtr();
	int nDeleted = 0;

	// Delete from source routes

	for( auto it = m_sourceRoutes.begin() ; it != m_sourceRoutes.end() ; it++ )
	{
		Route * p = it->second.First();
		while( p )
		{
			if( p->Receiver() == pReceiver )
			{
				Route * pNext = p->Next();
				delete p;					// Receiver is dead, delete route.
				nDeleted++;
				p = pNext;
			}
			else
				p = p->Next();

		}
	}

	// Delete from type routes

	for( auto it = m_typeRoutes.begin() ; it != m_typeRoutes.end() ; it++ )
	{
		Route * p = it->second.First();
		while( p )
		{
			if( p->Receiver() == pReceiver )
			{
				Route * pNext = p->Next();
				delete p;					// Receiver is dead, delete route.
				nDeleted++;
				p = pNext;
			}
			else
				p = p->Next();
		}
	}

	return nDeleted;
}

//____ DeleteRoutesFrom() _______________________________________________________

int WgMsgRouter::DeleteRoutesFrom( const WgObjectPtr& pSource )
{
	auto it = m_sourceRoutes.find(WgObjectWeakPtr(pSource.RawPtr()) );

	if( it == m_sourceRoutes.end() )
		return 0;

	int nDeleted = it->second.Size();
	m_sourceRoutes.erase(it);
	return nDeleted;
}

int WgMsgRouter::DeleteRoutesFrom( WgMsgType msgType )
{
	auto it = m_typeRoutes.find(msgType);

	if( it == m_typeRoutes.end() )
		return 0;

	int nDeleted = it->second.Size();
	m_typeRoutes.erase(it);
	return nDeleted;
}


//____ DeleteRoute() ______________________________________________________

bool WgMsgRouter::DeleteRoute( WgRouteId id )
{
	for( auto it = m_typeRoutes.begin() ; it != m_typeRoutes.end() ; ++it )
	{
		Route * p = it->second.First();
		while( p )
		{
			if( p->m_handle == id )
			{
				delete p;
				return true;
			}
			p = p->Next();
		}
	}

	for( auto it = m_sourceRoutes.begin() ; it != m_sourceRoutes.end() ; ++it )
	{
		Route * p = it->second.First();
		while( p )
		{
			if( p->m_handle == id )
			{
				delete p;
				return true;
			}
			p = p->Next();
		}
	}

	return false;
}

//____ ClearRoutes() _______________________________________________________

int WgMsgRouter::ClearRoutes()
{
	m_sourceRoutes.clear();
	m_typeRoutes.clear();
	return false;
}

//____ GarbageCollectRoutes() __________________________________________________

int WgMsgRouter::GarbageCollectRoutes()
{
	int nDeleted = 0;

	// Delete any dead global callbacks

	Route * p = m_broadcasts.First();
	while( p )
	{
		if( p->IsAlive() )
			p = p->Next();
		else
		{
			Route * pNext = p->Next();
			delete p;
			nDeleted++;
			p = pNext;
		}
	}

	// Delete any dead source routes.
	// These can be dead by either source or receiver having been deleted.

	{
		auto it = m_sourceRoutes.begin();

		while( it != m_sourceRoutes.end() )
		{
			if( !it->first )
			{
				nDeleted += it->second.Size();
				it = m_sourceRoutes.erase(it);		// Sender is dead, delete whole branch of callbacks.
			}
			else
			{
				Route * p = it->second.First();
				while( p )
				{
					if( p->IsAlive() )
						p = p->Next();
					else
					{
						Route * pNext = p->Next();
						delete p;					// Receiver is dead, delete callback.
						nDeleted++;
						p = pNext;
					}
				}
				++it;
			}
		}
	}
	// Delete any dead type routes.
	// These can be dead by receiver having been deleted.

	for( auto it = m_typeRoutes.begin(); it != m_typeRoutes.end() ; it++ )
	{
		Route * p = it->second.First();
		while( p )
		{
			if( p->IsAlive() )
				p = p->Next();
			else
			{
				Route * pNext = p->Next();
				delete p;					// Receiver is dead, delete callback.
				nDeleted++;
				p = pNext;
			}
		}
	}

	return nDeleted;
}

//____ _addRoute() _________________________________________________________

WgRouteId WgMsgRouter::_addRoute( const WgObjectPtr& pSource, Route * pRoute )
{
	if( !pSource )
		return 0;

	WgChain<Route>& chain = m_sourceRoutes[pSource.RawPtr()];
	chain.PushBack(pRoute);
	pRoute->m_handle = m_routeCounter++;
	return pRoute->m_handle;
}

//____ _addRoute() _________________________________________________________

WgRouteId WgMsgRouter::_addRoute( WgMsgType type, Route * pRoute )
{
	if( type == WG_MSG_DUMMY || type >= WG_MSG_MAX )
		return 0;

	WgChain<Route>& chain = m_typeRoutes[type];
	chain.PushBack(pRoute);
	pRoute->m_handle = m_routeCounter++;
	return pRoute->m_handle;
}


//____ Post() ___________________________________________________________

bool WgMsgRouter::Post( const WgMsgPtr& pMsg )
{
	if( m_bIsProcessing )
	{
		// Msgs that are posted as a direct result of another event being processed
		// are inserted before the next event in the queue.
		// If two or more events are posted by the same event being processed,
		// they need to be queued in the order of posting.

		m_insertPos = m_msgQueue.insert( m_insertPos, pMsg );
		m_insertPos++;
	}
	else
	{
		// Msgs being posted outside processing loop are simply added to the
		// queue.

		m_msgQueue.push_back( pMsg );
	}

	return true;
}

//____ Dispatch() ________________________________________________________

void WgMsgRouter::Dispatch()
{
	m_bIsProcessing = true;

	// First thing: we make sure that we know what Widgets pointer is inside, in case that has changed.

	m_insertPos = m_msgQueue.begin();	// Insert any POINTER_ENTER/EXIT right at beginning.
	_updateMarkedWidget(false);

	// Process all the events in the queue

	_dispatchQueued();

	m_bIsProcessing = false;
}


//____ _dispatchQueued() ___________________________________________________

void WgMsgRouter::_dispatchQueued()
{
	while( !m_msgQueue.empty() )
	{
		WgMsgPtr& pMsg = m_msgQueue.front();
		m_insertPos = m_msgQueue.begin()+1;	// Insert position set to right after current event.

		_finalizeMsg( pMsg );

		do
		{
			if( pMsg->HasCopyTo()  )
				pMsg->m_pCopyTo->OnMsg( pMsg );
			
			if( pMsg->HasSource() )
				_dispatchToSourceRoutes( pMsg );
			else
				_processGeneralMsg( pMsg );

			_dispatchToTypeRoutes( pMsg );
			_broadcast( pMsg );			
		}
		while( pMsg->DoRepost() );

		m_msgQueue.pop_front();
	}

	m_insertPos = m_msgQueue.begin();		// Insert position set right to start.
}


//____ _broadcast() ________________________________________________

void WgMsgRouter::_broadcast( const WgMsgPtr& pMsg )
{
	Route * pRoute = m_broadcasts.First();

	while( pRoute )
	{
		pRoute->Dispatch( pMsg );
		pRoute = pRoute->Next();
	}
}


//____ _dispatchToTypeRoutes() __________________________________________________

void WgMsgRouter::_dispatchToTypeRoutes( const WgMsgPtr& pMsg )
{
	auto it = m_typeRoutes.find(pMsg->Type());
	if( it != m_typeRoutes.end() )
	{
		Route * pRoute = it->second.First();

		while( pRoute )
		{
			pRoute->Dispatch( pMsg );
			pRoute = pRoute->Next();
		}
	}	
}

//____ _dispatchToSourceRoutes() ________________________________________________

void WgMsgRouter::_dispatchToSourceRoutes( const WgMsgPtr& pMsg )
{
	WgObject * pSource = pMsg->SourceRawPtr();

	if( pSource )
	{
		auto it = m_sourceRoutes.find(WgObjectWeakPtr(pSource));
		if( it != m_sourceRoutes.end() )
		{
			Route * pRoute = it->second.First();
			while( pRoute )
			{
				pRoute->Dispatch( pMsg );
				pRoute = pRoute->Next();
			}
		}
	}
}


//____ _finalizeMsg() ________________________________________________________

void WgMsgRouter::_finalizeMsg( const WgMsgPtr& pMsg )
{
	// Fill in missing information in the event-class.

	pMsg->m_timestamp	= m_time;
	pMsg->m_modKeys	= m_modKeys;

	// Only global POINTER_ENTER & POINTER_MOVE events have these members
	// set, the rest needs to have them filled in.

	if( pMsg->HasCopyTo() || (pMsg->Type() != WG_MSG_MOUSE_MOVE && pMsg->Type() != WG_MSG_MOUSE_ENTER) )
	{
		pMsg->m_pointerPos = m_pointerPos;
	}

	// Msg specific finalizations

	switch( pMsg->Type() )
	{

		// Key events need translation of keycodes.

		case WG_MSG_KEY_PRESS:
		case WG_MSG_KEY_RELEASE:
		case WG_MSG_KEY_REPEAT:
		{
			WgKeyMsg* p = static_cast<WgKeyMsg*>(pMsg.RawPtr());
			p->m_translatedKeyCode = WgBase::TranslateKey(p->m_nativeKeyCode);
		}
		break;


		default:
			break;
	}



}

//____ _processGeneralMsg() _________________________________________________

void WgMsgRouter::_processGeneralMsg( const WgMsgPtr& _pMsg )
{
	WgMsg * pMsg = _pMsg.RawPtr();

	switch( pMsg->Type() )
	{
		case WG_MSG_FOCUS_GAINED:
			_processFocusGained( (WgFocusGainedMsg*) pMsg );
			break;

		case WG_MSG_FOCUS_LOST:
			_processFocusLost( (WgFocusLostMsg*) pMsg );
			break;

		case WG_MSG_MOUSE_ENTER:
			_processMouseEnter( (WgMouseEnterMsg*) pMsg );
			break;

		case WG_MSG_MOUSE_MOVE:
			_processMouseMove( (WgMouseMoveMsg*) pMsg );
			break;

		case WG_MSG_MOUSE_POSITION:
			_processMousePosition( (WgMousePositionMsg*) pMsg );
			break;

		case WG_MSG_MOUSE_LEAVE:
			_processMouseLeave( (WgMouseLeaveMsg*) pMsg );
			break;

		case WG_MSG_MOUSE_PRESS:
			_processMouseButtonPress( (WgMousePressMsg*) pMsg );
			break;

		case WG_MSG_MOUSE_REPEAT:
			_processMouseButtonRepeat( (WgMouseRepeatMsg*) pMsg );
			break;

		case WG_MSG_MOUSE_DRAG:
			_processMouseButtonDrag( (WgMouseDragMsg*) pMsg );
			break;

		case WG_MSG_MOUSE_RELEASE:
			_processMouseButtonRelease( (WgMouseReleaseMsg*) pMsg );
			break;

		case WG_MSG_MOUSE_CLICK:
			_processMouseButtonClick( (WgMouseClickMsg*) pMsg );
			break;

		case WG_MSG_MOUSE_DOUBLE_CLICK:
			_processMouseButtonDoubleClick( (WgMouseDoubleClickMsg*) pMsg );
			break;

		case WG_MSG_KEY_PRESS:
			_processKeyPress( (WgKeyPressMsg*) pMsg );
			break;

		case WG_MSG_KEY_REPEAT:
			_processKeyRepeat( (WgKeyRepeatMsg*) pMsg );
			break;

		case WG_MSG_KEY_RELEASE:
			_processKeyRelease( (WgKeyReleaseMsg*) pMsg );
			break;

		case WG_MSG_CHARACTER:
			_processCharacter( (WgCharacterMsg*) pMsg );
			break;

		case WG_MSG_WHEEL_ROLL:
			_processMouseWheelRoll( (WgWheelRollMsg*) pMsg );
			break;

		case WG_MSG_TICK:
			_processTick( (WgTickMsg*) pMsg );
			break;

		case WG_MSG_DUMMY:
			break;

		case WG_MSG_MAX:
			assert(false);								// NOT ALLOWED!
			break;
	}
}

//____ _processTick() ______________________________________________________

void WgMsgRouter::_processTick( WgTickMsg * pMsg )
{
	// Check if we need to post BUTTON_REPEAT

	for( int button = 0 ; button <= WG_MAX_BUTTONS ; button++ )
	{
		if( m_bButtonPressed[button] )
		{
			int buttonDelay = WgBase::MouseButtonRepeatDelay();
			int buttonRate = WgBase::MouseButtonRepeatRate();

			int msSinceRepeatStart = (int) (m_time - m_pLatestPressMsgs[button]->Timestamp() - buttonDelay );

			// First BUTTON_REPEAT event posted separately.

			if( msSinceRepeatStart < 0 && msSinceRepeatStart + pMsg->Millisec() >= 0 )
				Post( new WgMouseRepeatMsg((WgMouseButton)button) );

			// Calculate ms since last BUTTON_REPEAT event

			int msToProcess;
			if( msSinceRepeatStart < 0 )
				msToProcess = msSinceRepeatStart + pMsg->Millisec();
			else
				msToProcess = (msSinceRepeatStart % buttonRate) + pMsg->Millisec();

			// Post the amount of BUTTON_REPEAT that should be posted.

			while( msToProcess >= buttonRate )
			{
				Post( new WgMouseRepeatMsg((WgMouseButton)button) );
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
		int timePassed = (int) (pMsg->Timestamp() - pInfo->pMsg->Timestamp());

		int fraction = timePassed - keyDelay;

		if( fraction < 0 )
			fraction += keyRate;
		else
			fraction %= keyRate;

		fraction += pMsg->Millisec();

		while( fraction >= keyRate )
		{
			Post( new WgKeyRepeatMsg( pInfo->pMsg->NativeKeyCode() ) );
			fraction -= keyRate;
		}
	}


	// Increase time counter

	m_time += pMsg->Millisec();
}


//____ _processFocusGained() __________________________________________________

void WgMsgRouter::_processFocusGained( WgFocusGainedMsg * pMsg )
{
	if( !m_bWindowFocus && m_keyFocusWidget )
		_setWidgetFocused( m_keyFocusWidget.RawPtr(), true );

	m_bWindowFocus = true;
}

//____ _processFocusLost() ____________________________________________________

void WgMsgRouter::_processFocusLost( WgFocusLostMsg * pMsg )
{
	if( m_bWindowFocus && m_keyFocusWidget )
		_setWidgetFocused( m_keyFocusWidget.RawPtr(), false );

	m_bWindowFocus = false;
}


//____ _processMouseEnter() __________________________________________________

void WgMsgRouter::_processMouseEnter( WgMouseEnterMsg * pMsg )
{
	// Post events for button drag

	for( int i = 0 ; i <= WG_MAX_BUTTONS ; i++ )
	{
		if( m_bButtonPressed[i] )
			Post( new WgMouseDragMsg((WgMouseButton)i, m_pLatestPressMsgs[i]->PointerPos(), m_pointerPos, pMsg->PointerPos() ) );
	}

	// Post event for finalizing position once button drag is taken care of.

	Post( new WgMousePositionMsg() );

	// Update pointer position

	m_pointerPos = pMsg->PointerPos();
}

//____ _processMouseLeave() ___________________________________________________

void WgMsgRouter::_processMouseLeave( WgMouseLeaveMsg * pMsg )
{
	// Post POINTER_EXIT event to marked widget

	WgWidget * pWidget = m_pMarkedWidget.RawPtr();

	if( pWidget )
		Post( new WgMouseLeaveMsg( pWidget ) );

	m_pMarkedWidget = 0;
}


//____ _processMouseMove() ___________________________________________________

void WgMsgRouter::_processMouseMove( WgMouseMoveMsg * pMsg )
{
	// Post events for button drag

	for( int i = 0 ; i <= WG_MAX_BUTTONS ; i++ )
	{
		if( m_bButtonPressed[i] )
			Post( new WgMouseDragMsg( (WgMouseButton) i, m_pLatestPressMsgs[i]->PointerPos(), m_pointerPos, pMsg->PointerPos() ) );
	}

	// Post event for finalizing move once button drag is taken care of.

	Post( new WgMousePositionMsg() );

	// Update pointer position

	m_pointerPos = pMsg->PointerPos();
}

//____ _processMousePosition() _________________________________________________

void WgMsgRouter::_processMousePosition( WgMousePositionMsg * pMsg )
{
	_updateMarkedWidget(true);

}

//____ _setWidgetFocused() ____________________________________________________

void WgMsgRouter::_setWidgetFocused( WgWidget * pWidget, bool bFocused )
{
		WgState oldState = pWidget->m_state;
		pWidget->m_state.SetFocused(bFocused);

		if( pWidget->m_state != oldState )
			pWidget->_onStateChanged(oldState);
}


//____ _updateMarkedWidget() _______________________________________________

void WgMsgRouter::_updateMarkedWidget(bool bPostMouseMoveMsgs)
{
	WgWidgetPtr pNowMarked = 0;

	WgWidgetPtr pWidgetTarget = m_pRoot->FindWidget( m_pointerPos, WG_SEARCH_ACTION_TARGET );

	// Figure out which button of currently pressed has been pressed the longest.
	// Mouse is only allowed to mark Widgets that were marked on press of that button.

	int button = 0;								// Button that has been pressed for longest, 0 = no button pressed
	for( int i = 1 ; i <= WG_MAX_BUTTONS ; i++ )
	{
		if( m_bButtonPressed[i] && (button == 0 || m_pLatestPressMsgs[i]->Timestamp() < m_pLatestPressMsgs[button]->Timestamp()) )
			button = i;
	}

	// We are only marking the Widget if no mouse button is pressed or the first pressed button
	// was pressed on it.

	if( button == 0 || pWidgetTarget == m_latestPressWidgets[button].RawPtr() )
		pNowMarked = pWidgetTarget;

	// Post Leave events for widgets no longer marked,
	// Post Enter events for new marked widgets
	// and Move events for those already marked

	WgWidget * pFirstAlreadyMarked = _updateEnteredWidgets( pNowMarked.RawPtr() );
	
	if( bPostMouseMoveMsgs && pFirstAlreadyMarked )
		Post( new WgMouseMoveMsg( pFirstAlreadyMarked ) );

	// Copy content of pNowMarked to m_pMarkedWidget

	m_pMarkedWidget = pNowMarked.RawPtr();
	
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
		Post( new WgPointerChangeMsg( newStyle ) );
		m_pointerStyle = newStyle;
	}	
}

//____ _updateEnteredWidgets() _________________________________________________

WgWidget * WgMsgRouter::_updateEnteredWidgets( WgWidget * pMarkedWidget )
{
	
	// Loop through our new widgets and check if they already
	// were entered. Send MouseEnter to all new widgets and notice the first
	// common ancestor .

	WgWidget * pFirstAlreadyMarked = 0;
	for( WgWidget * pWidget = pMarkedWidget ; pWidget != 0 ; pWidget = pWidget->_parent() )
	{
		int ofs = _widgetPosInList( pWidget, m_vEnteredWidgets );
		if( ofs >= 0 )
		{	
			if( !pFirstAlreadyMarked )
				pFirstAlreadyMarked = pWidget;
			m_vEnteredWidgets[ofs] = 0;			
		}
		else
			Post( new WgMouseEnterMsg( pWidget ) );		
	}

	// Send MouseLeave to those that were left.

	for( size_t i = 0 ; i < m_vEnteredWidgets.size() ; i++ )
		if(m_vEnteredWidgets[i] )
			Post( new WgMouseLeaveMsg( m_vEnteredWidgets[i].RawPtr()) );
	
	// Replace the old list with a new one.
	
	m_vEnteredWidgets.clear();
	for( WgWidget * pWidget = pMarkedWidget ; pWidget != 0 ; pWidget = pWidget->_parent() )
		m_vEnteredWidgets.push_back( pWidget );

	// Return first already marked, calling function might need it.

	return pFirstAlreadyMarked;
}


//____ _processKeyPress() ______________________________________________________

void WgMsgRouter::_processKeyPress( WgKeyPressMsg * pMsg )
{
	// Fill in the info-structure.

	KeyDownInfo * pInfo = new KeyDownInfo();
	pInfo->pMsg = pMsg;

	// Post KEY_PRESS events for widgets and remember which ones we have posted it for

	WgWidget * pWidget = m_keyFocusWidget.RawPtr();
	Post( new WgKeyPressMsg( pMsg->NativeKeyCode(), pWidget ) );
	pInfo->pWidget = pWidget;

	// Push the info-structure onto m_keysDown.

	m_keysDown.push_back( pInfo );

	// Update modkeys

	switch( pMsg->TranslatedKeyCode() )
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

void WgMsgRouter::_processKeyRepeat( WgKeyRepeatMsg * pMsg )
{
	// Find right KeyDownInfo structure

	KeyDownInfo * pInfo = 0;

	for( unsigned int i = 0 ; i < m_keysDown.size() ; i++ )
	{
		if( pMsg->NativeKeyCode() == m_keysDown[i]->pMsg->NativeKeyCode() )
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
			Post( new WgKeyRepeatMsg( pMsg->NativeKeyCode(), pInfo->pWidget.RawPtr() ));
}

//____ _processKeyRelease() ___________________________________________________

void WgMsgRouter::_processKeyRelease( WgKeyReleaseMsg * pMsg )
{
	// Find right KeyDownInfo structure and remove it from m_keysDown.

	KeyDownInfo * pInfo = 0;

	std::vector<KeyDownInfo*>::iterator it = m_keysDown.begin();
	while( it != m_keysDown.end() )
	{
		if( pMsg->NativeKeyCode() == (*it)->pMsg->NativeKeyCode() )
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
		Post( new WgKeyReleaseMsg( pMsg->NativeKeyCode(), pInfo->pWidget.RawPtr() ));

	// Delete the KeyPress-message and KeyDownInfo-structure

	delete pInfo;

	// Update modkeys

	switch( pMsg->TranslatedKeyCode() )
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

void WgMsgRouter::_processCharacter( WgCharacterMsg * pMsg )
{
	WgWidget * pWidget = m_keyFocusWidget.RawPtr();

	if( pWidget )
		Post( new WgCharacterMsg( pMsg->Char(), pWidget ) );
}

//____ _processMouseWheelRoll() ____________________________________________________

void WgMsgRouter::_processMouseWheelRoll( WgWheelRollMsg * pMsg )
{
	_updateMarkedWidget(false);

	WgWidget * pWidget = m_pMarkedWidget.RawPtr();

	if( pWidget )
		Post( new WgWheelRollMsg( pMsg->Wheel(), pMsg->Distance(), pWidget ) );
}

//____ _processMouseButtonPress() ___________________________________________________

void WgMsgRouter::_processMouseButtonPress( WgMousePressMsg * pMsg )
{
	_updateMarkedWidget(false);

	WgMouseButton button = pMsg->Button();

	// Update m_previousPressWidgets

	m_previousPressWidgets[button] = 0;

	WgWidget * pWidget = m_latestPressWidgets[button].RawPtr();
	if( pWidget )
		m_previousPressWidgets[button] = pWidget;

	// Post BUTTON_PRESS events for marked widgets and remember which one we have posted it for

	m_latestPressWidgets[button] = 0;

	pWidget = m_pMarkedWidget.RawPtr();
	if( pWidget )
	{
		Post( new WgMousePressMsg( button, pWidget ) );
		m_latestPressWidgets[button] = pWidget;
	}

	// Handle possible double-click

	int doubleClickTimeTreshold = WgBase::DoubleClickTimeTreshold();
	int doubleClickDistanceTreshold = WgBase::DoubleClickDistanceTreshold();


	if( m_pLatestPressMsgs[button] && m_pLatestPressMsgs[button]->Timestamp() + doubleClickTimeTreshold > pMsg->Timestamp() )
	{
		WgCoord distance = pMsg->PointerPos() - m_pLatestPressMsgs[button]->PointerPos();

		if( distance.x <= doubleClickDistanceTreshold &&
			distance.x >= -doubleClickDistanceTreshold &&
			distance.y <= doubleClickDistanceTreshold &&
			distance.y >= -doubleClickDistanceTreshold )
			Post( new WgMouseDoubleClickMsg(button) );
	}

	// Save info for the future

	m_pLatestPressMsgs[button] = pMsg;

	m_bButtonPressed[button] = true;
}


//____ _processMouseButtonRepeat() __________________________________________________

void WgMsgRouter::_processMouseButtonRepeat( WgMouseRepeatMsg * pMsg )
{
	_updateMarkedWidget(false);

	WgMouseButton button = pMsg->Button();

	// Post BUTTON_REPEAT event for widget that received the press if we are still inside.

	WgWidget * pWidget = m_latestPressWidgets[button].RawPtr();
	if( pWidget && pWidget == m_pMarkedWidget.RawPtr() )
		Post( new WgMouseRepeatMsg(button, pWidget) );
}



//____ _processMouseButtonRelease() _________________________________________________

void WgMsgRouter::_processMouseButtonRelease( WgMouseReleaseMsg * pMsg )
{
	_updateMarkedWidget(false);

	WgMouseButton button = pMsg->Button();

	// Post BUTTON_RELEASE events for widget that was pressed

	WgWidget * pWidget = m_latestPressWidgets[button].RawPtr();
	if( pWidget )
	{
		bool bIsInside = pWidget->GlobalGeo().Contains(pMsg->PointerPos());
		Post( new WgMouseReleaseMsg( button, pWidget, true, bIsInside ) );
	}

	// Post BUTTON_RELEASE events for marked widget that was NOT pressed

	pWidget = m_pMarkedWidget.RawPtr();
	if( pWidget )
	{
		if( pWidget != m_latestPressWidgets[button].RawPtr() )
		{
			bool bIsInside = pWidget->GlobalGeo().Contains(pMsg->PointerPos());
			Post( new WgMouseReleaseMsg( button, pWidget, false, bIsInside ) );
		}
	}

	// As long as the button was pressed inside our window we have a click
	// on this level.

	if( m_bButtonPressed[button] )
		Post( new WgMouseClickMsg( button ) );

	// Save info for the future

	m_pLatestReleaseMsgs[button] = pMsg;
	m_bButtonPressed[button] = false;
}

//____ _processMouseButtonDrag() ____________________________________________________

void WgMsgRouter::_processMouseButtonDrag( WgMouseDragMsg * pMsg )
{
	WgMouseButton button = pMsg->Button();

	// Post POINTER_DRAG event for pressed widget

	WgWidget * pWidget = m_latestPressWidgets[button].RawPtr();

	if( pWidget )
	{
		WgCoord	ofs = pWidget->GlobalPos();
		Post( new WgMouseDragMsg( button, pWidget, pMsg->StartPos() - ofs, pMsg->PrevPos() - ofs, pMsg->CurrPos() - ofs ) );
	}

}

//____ _processMouseButtonClick() _________________________________________________

void WgMsgRouter::_processMouseButtonClick( WgMouseClickMsg * pMsg )
{
	WgMouseButton button = pMsg->Button();

	// Post BUTTON_CLICK events for widget that received the press if we
	// still are inside.

	WgWidget * pWidget = m_latestPressWidgets[button].RawPtr();
	if( pWidget && pWidget == m_pMarkedWidget.RawPtr() )
		Post( new WgMouseClickMsg(button, pWidget) );
}

//____ _processMouseButtonDoubleClick() _________________________________________________

void WgMsgRouter::_processMouseButtonDoubleClick( WgMouseDoubleClickMsg * pMsg )
{
	WgMouseButton button = pMsg->Button();

	// Post BUTTON_DOUBLE_CLICK event if gizom received both this and previous press.

	WgWidget * pWidget = m_latestPressWidgets[button].RawPtr();
	if( pWidget && pWidget ==  m_previousPressWidgets[button].RawPtr() )
		Post( new WgMouseDoubleClickMsg(button, pWidget) );
}

//____ _widgetPosInList() ________________________________________________________

int WgMsgRouter::_widgetPosInList( const WgWidget * pWidget, const std::vector<WgWidgetWeakPtr>& list )
{
	for( size_t i = 0 ; i < list.size() ; i++ )
		if( list[i].RawPtr() == pWidget )
			return i;

	return -1;
}


WgMsgRouter::Route::Route( const WgMsgFilter& filter, WgReceiver * pReceiver )
{
	m_filter = filter;
	m_pReceiver = pReceiver;
	pReceiver->_onRouteAdded();
}

WgMsgRouter::Route::~Route()
{
	if( m_pReceiver )
		m_pReceiver->_onRouteRemoved();
}

void WgMsgRouter::Route::Dispatch( const WgMsgPtr& pMsg )
{
	if( m_filter.FilterMsg(pMsg) )
		m_pReceiver->OnMsg( pMsg );
}

bool WgMsgRouter::Route::IsAlive() const
{
	return true;
}

WgReceiver * WgMsgRouter::Route::Receiver() const
{
	return m_pReceiver.RawPtr();
}

