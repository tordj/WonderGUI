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

namespace wg 
{
	
	const char MsgRouter::CLASSNAME[] = {"MsgRouter"};
	
	
	//____ Constructor ____________________________________________________________
	
	MsgRouter::MsgRouter()
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
	
	MsgRouter::~MsgRouter()
	{
	}
	
	//____ isInstanceOf() _________________________________________________________
	
	bool MsgRouter::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return Object::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * MsgRouter::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	MsgRouter_p MsgRouter::cast( const Object_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return MsgRouter_p( static_cast<MsgRouter*>(pObject.rawPtr()) );
	
		return 0;
	}
	
	
	void MsgRouter::setRoot( const RootPanel_p& pRoot )
	{
		m_pRoot = pRoot.rawPtr();
	}
	
	
	//____ setFocusGroup() ________________________________________________________
	
	bool MsgRouter::setFocusGroup( const Panel_p& pFocusGroup )
	{
		// Sanity checks
	
		if( pFocusGroup )
		{
			if( pFocusGroup == m_keyFocusGroup )
				return true;									// Not an error, but we don't need to do anything
	
			if( !pFocusGroup->isFocusGroup() )
				return false;									// Panel is not a focus group
	
			if( !pFocusGroup->hook() || pFocusGroup->hook()->root() != m_pRoot )
				return false;									// pFocusGroup is not a child of our root.
		}
	
		// Set new focus widget as specified by group
	
		Widget_wp pNewFocusWidget;
	
		if( pFocusGroup )
			if( m_focusGroupMap.find(pFocusGroup.rawPtr()) != m_focusGroupMap.end() )
				pNewFocusWidget = m_focusGroupMap[pFocusGroup.rawPtr()];
	
		if( m_keyFocusWidget )
			_setWidgetFocused( m_keyFocusWidget.rawPtr(), false );
	
		if( pNewFocusWidget )
			_setWidgetFocused( pNewFocusWidget.rawPtr(), true );
	
		// Set members and exit
	
		m_keyFocusWidget = pNewFocusWidget;
		m_keyFocusGroup = pFocusGroup.rawPtr();
	
		return true;
	}
	
	
	//____ setKeyboardFocus() _____________________________________________________
	
	bool MsgRouter::setKeyboardFocus( const Widget_p& pFocus )
	{
		// Return if Widget is not child of our root.
	
		if( pFocus && (!pFocus->hook() || pFocus->hook()->root() != m_pRoot) )
			return false;
	
		// Handle old focus.
	
		Widget * pOldFocus = m_keyFocusWidget.rawPtr();
	
		if( pFocus == pOldFocus )
			return true;
	
		if( pOldFocus )
			_setWidgetFocused( pOldFocus, false );
	
		// Handle new focus, possibly switching focus group.
	
		if( pFocus )
		{
			// Check what focus group (if any) this Widget belongs to.
	
			m_keyFocusGroup = 0;
			Container * p = pFocus->_parent();
			while( p )
			{
				if( p->_isPanel() && static_cast<Panel*>(p)->isFocusGroup() )
				{
					m_keyFocusGroup = static_cast<Panel*>(p);
					break;
				}
	
				p = p->_parent();
			}
	
			// Activate focus
	
			_setWidgetFocused( pFocus.rawPtr(), true );
		}
	
		// Set members and exit.
	
		m_keyFocusWidget = pFocus.rawPtr();
		m_focusGroupMap[m_keyFocusGroup] = pFocus.rawPtr();
	
		return true;
	}
	
	//____ isMouseButtonPressed() _________________________________________________________
	
	bool MsgRouter::isMouseButtonPressed( int button ) const
	{
		if( button >= 1 && button <= WG_MAX_BUTTONS )
			return m_bButtonPressed[button];
	
		return false;
	}
	
	//____ isAnyMouseButtonPressed() ________________________________________________________
	
	bool MsgRouter::isAnyMouseButtonPressed() const
	{
		for( int i = 0 ; i < WG_MAX_BUTTONS ; i++ )
			if( m_bButtonPressed[i] )
				return true;
	
		return false;
	}
	
	
	//____ isKeyPressed() ____________________________________________________________
	
	bool MsgRouter::isKeyPressed( int native_keycode ) const
	{
		for( unsigned int i = 0 ; i < m_keysDown.size() ; i++ )
			if( native_keycode == m_keysDown[i]->pMsg->nativeKeyCode() )
				return true;
	
		return false;
	}
	
	//____ broadcastTo() ___________________________________________________________
	
	bool  MsgRouter::broadcastTo( const Receiver_p& pReceiver )
	{
		Route * p = new Route( MsgFilter(), pReceiver.rawPtr() );
		m_broadcasts.pushBack( p );
		return true;
	}
	
	bool  MsgRouter::broadcastTo( const MsgFilter& filter, const Receiver_p& pReceiver )
	{
		Route * p = new Route( filter, pReceiver.rawPtr() );
		m_broadcasts.pushBack( p );
		return true;
	}
	
	//____ endBroadcast() __________________________________________________________
	
	bool  MsgRouter::endBroadcast( const Receiver_p& _pReceiver )
	{
		Receiver * pReceiver = _pReceiver.rawPtr();
		Route * p = m_broadcasts.first();
		while( p )
		{
			if( p->receiver() == pReceiver )
			{
				delete p;
				return true;
			}
			
			p = p->next();
		}	
		return false;
	}
	
	
	//____ addRoute() __________________________________________________________
	
	WgRouteId MsgRouter::addRoute( const Object_p& pSource, const Receiver_p& pReceiver )
	{
		Route * p = new Route( MsgFilter(), pReceiver.rawPtr() );
		return _addRoute( pSource, p );
	}
	
	WgRouteId MsgRouter::addRoute( const MsgFilter& filter, const Object_p& pSource, const Receiver_p& pReceiver )
	{
		Route * p = new Route( filter, pReceiver.rawPtr() );
		return _addRoute( pSource, p );
	}
	
	WgRouteId MsgRouter::addRoute( MsgType msgType, const Receiver_p& pReceiver )
	{
		Route * p = new Route( MsgFilter(), pReceiver.rawPtr() );
		return _addRoute( msgType, p );	
	}
	
	WgRouteId MsgRouter::addRoute( MsgType msgType, Receiver * pReceiver )
	{
		Route * p = new Route( MsgFilter(), pReceiver );
		return _addRoute( msgType, p );	
	}
	
	WgRouteId MsgRouter::addRoute( const MsgFilter& filter, MsgType msgType, const Receiver_p& pReceiver )
	{
		Route * p = new Route( filter, pReceiver.rawPtr() );
		return _addRoute( msgType, p );	
	}
	
	
	
	//____ deleteRoutesTo() _______________________________________________________
	
	int MsgRouter::deleteRoutesTo( const Receiver_p& _pReceiver )
	{
		Receiver * pReceiver = _pReceiver.rawPtr();
		int nDeleted = 0;
	
		// Delete from source routes
	
		for( auto it = m_sourceRoutes.begin() ; it != m_sourceRoutes.end() ; it++ )
		{
			Route * p = it->second.first();
			while( p )
			{
				if( p->receiver() == pReceiver )
				{
					Route * pNext = p->next();
					delete p;					// Receiver is dead, delete route.
					nDeleted++;
					p = pNext;
				}
				else
					p = p->next();
	
			}
		}
	
		// Delete from type routes
	
		for( auto it = m_typeRoutes.begin() ; it != m_typeRoutes.end() ; it++ )
		{
			Route * p = it->second.first();
			while( p )
			{
				if( p->receiver() == pReceiver )
				{
					Route * pNext = p->next();
					delete p;					// Receiver is dead, delete route.
					nDeleted++;
					p = pNext;
				}
				else
					p = p->next();
			}
		}
	
		return nDeleted;
	}
	
	//____ deleteRoutesFrom() _______________________________________________________
	
	int MsgRouter::deleteRoutesFrom( const Object_p& pSource )
	{
		auto it = m_sourceRoutes.find(Object_wp(pSource.rawPtr()) );
	
		if( it == m_sourceRoutes.end() )
			return 0;
	
		int nDeleted = it->second.size();
		m_sourceRoutes.erase(it);
		return nDeleted;
	}
	
	int MsgRouter::deleteRoutesFrom( MsgType msgType )
	{
		auto it = m_typeRoutes.find(msgType);
	
		if( it == m_typeRoutes.end() )
			return 0;
	
		int nDeleted = it->second.size();
		m_typeRoutes.erase(it);
		return nDeleted;
	}
	
	
	//____ deleteRoute() ______________________________________________________
	
	bool MsgRouter::deleteRoute( WgRouteId id )
	{
		for( auto it = m_typeRoutes.begin() ; it != m_typeRoutes.end() ; ++it )
		{
			Route * p = it->second.first();
			while( p )
			{
				if( p->m_handle == id )
				{
					delete p;
					return true;
				}
				p = p->next();
			}
		}
	
		for( auto it = m_sourceRoutes.begin() ; it != m_sourceRoutes.end() ; ++it )
		{
			Route * p = it->second.first();
			while( p )
			{
				if( p->m_handle == id )
				{
					delete p;
					return true;
				}
				p = p->next();
			}
		}
	
		return false;
	}
	
	//____ clearRoutes() _______________________________________________________
	
	int MsgRouter::clearRoutes()
	{
		m_sourceRoutes.clear();
		m_typeRoutes.clear();
		return false;
	}
	
	//____ garbageCollectRoutes() __________________________________________________
	
	int MsgRouter::garbageCollectRoutes()
	{
		int nDeleted = 0;
	
		// Delete any dead global callbacks
	
		Route * p = m_broadcasts.first();
		while( p )
		{
			if( p->isAlive() )
				p = p->next();
			else
			{
				Route * pNext = p->next();
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
					nDeleted += it->second.size();
					it = m_sourceRoutes.erase(it);		// Sender is dead, delete whole branch of callbacks.
				}
				else
				{
					Route * p = it->second.first();
					while( p )
					{
						if( p->isAlive() )
							p = p->next();
						else
						{
							Route * pNext = p->next();
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
			Route * p = it->second.first();
			while( p )
			{
				if( p->isAlive() )
					p = p->next();
				else
				{
					Route * pNext = p->next();
					delete p;					// Receiver is dead, delete callback.
					nDeleted++;
					p = pNext;
				}
			}
		}
	
		return nDeleted;
	}
	
	//____ _addRoute() _________________________________________________________
	
	WgRouteId MsgRouter::_addRoute( const Object_p& pSource, Route * pRoute )
	{
		if( !pSource )
			return 0;
	
		Chain<Route>& chain = m_sourceRoutes[pSource.rawPtr()];
		chain.pushBack(pRoute);
		pRoute->m_handle = m_routeCounter++;
		return pRoute->m_handle;
	}
	
	//____ _addRoute() _________________________________________________________
	
	WgRouteId MsgRouter::_addRoute( MsgType type, Route * pRoute )
	{
		if( type == WG_MSG_DUMMY || type >= WG_MSG_MAX )
			return 0;
	
		Chain<Route>& chain = m_typeRoutes[type];
		chain.pushBack(pRoute);
		pRoute->m_handle = m_routeCounter++;
		return pRoute->m_handle;
	}
	
	
	//____ post() ___________________________________________________________
	
	bool MsgRouter::post( const Msg_p& pMsg )
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
	
	//____ dispatch() ________________________________________________________
	
	void MsgRouter::dispatch()
	{
		m_bIsProcessing = true;
	
		m_insertPos = m_msgQueue.begin();	// Insert any POINTER_ENTER/EXIT right at beginning.
	
		_dispatchQueued();
	
		m_bIsProcessing = false;
	}
	
	
	//____ _dispatchQueued() ___________________________________________________
	
	void MsgRouter::_dispatchQueued()
	{
		while( !m_msgQueue.empty() )
		{
			Msg_p& pMsg = m_msgQueue.front();
			m_insertPos = m_msgQueue.begin()+1;	// Insert position set to right after current event.
	
			_finalizeMsg( pMsg );
	
			do
			{
				if( pMsg->hasCopyTo()  )
					pMsg->m_pCopyTo->onMsg( pMsg );
				
				if( pMsg->hasSource() )
					_dispatchToSourceRoutes( pMsg );
	
				_dispatchToTypeRoutes( pMsg );
				_broadcast( pMsg );			
			}
			while( pMsg->doRepost() );
	
			m_msgQueue.pop_front();
		}
	
		m_insertPos = m_msgQueue.begin();		// Insert position set right to start.
	}
	
	
	//____ _broadcast() ________________________________________________
	
	void MsgRouter::_broadcast( const Msg_p& pMsg )
	{
		Route * pRoute = m_broadcasts.first();
	
		while( pRoute )
		{
			pRoute->dispatch( pMsg );
			pRoute = pRoute->next();
		}
	}
	
	
	//____ _dispatchToTypeRoutes() __________________________________________________
	
	void MsgRouter::_dispatchToTypeRoutes( const Msg_p& pMsg )
	{
		auto it = m_typeRoutes.find(pMsg->type());
		if( it != m_typeRoutes.end() )
		{
			Route * pRoute = it->second.first();
	
			while( pRoute )
			{
				pRoute->dispatch( pMsg );
				pRoute = pRoute->next();
			}
		}	
	}
	
	//____ _dispatchToSourceRoutes() ________________________________________________
	
	void MsgRouter::_dispatchToSourceRoutes( const Msg_p& pMsg )
	{
		Object * pSource = pMsg->sourceRawPtr();
	
		if( pSource )
		{
			auto it = m_sourceRoutes.find(Object_wp(pSource));
			if( it != m_sourceRoutes.end() )
			{
				Route * pRoute = it->second.first();
				while( pRoute )
				{
					pRoute->dispatch( pMsg );
					pRoute = pRoute->next();
				}
			}
		}
	}
	
	
	//____ _finalizeMsg() ________________________________________________________
	
	void MsgRouter::_finalizeMsg( const Msg_p& pMsg )
	{
		// Fill in missing information in the event-class.
	
		pMsg->m_timestamp	= m_time;
		pMsg->m_modKeys	= m_modKeys;
	
		// Only global POINTER_ENTER & POINTER_MOVE events have these members
		// set, the rest needs to have them filled in.
	
		if( pMsg->hasCopyTo() || (pMsg->type() != WG_MSG_MOUSE_MOVE && pMsg->type() != WG_MSG_MOUSE_ENTER) )
		{
			pMsg->m_pointerPos = m_pointerPos;
		}
	
		// Msg specific finalizations
	
		switch( pMsg->type() )
		{
	
			// Key events need translation of keycodes.
	
			case WG_MSG_KEY_PRESS:
			case WG_MSG_KEY_RELEASE:
			case WG_MSG_KEY_REPEAT:
			{
				KeyMsg* p = static_cast<KeyMsg*>(pMsg.rawPtr());
				p->m_translatedKeyCode = Base::translateKey(p->m_nativeKeyCode);
			}
			break;
	
	
			default:
				break;
		}
	
	
	
	}
	
	//____ _processGeneralMsg() _________________________________________________
	
	void MsgRouter::_processGeneralMsg( const Msg_p& _pMsg )
	{
		Msg * pMsg = _pMsg.rawPtr();
	
		switch( pMsg->type() )
		{
			case WG_MSG_FOCUS_GAINED:
				_processFocusGained( (FocusGainedMsg*) pMsg );
				break;
	
			case WG_MSG_FOCUS_LOST:
				_processFocusLost( (FocusLostMsg*) pMsg );
				break;
	
			case WG_MSG_MOUSE_ENTER:
				_processMouseEnter( (MouseEnterMsg*) pMsg );
				break;
	
			case WG_MSG_MOUSE_MOVE:
				_processMouseMove( (MouseMoveMsg*) pMsg );
				break;
	
			case WG_MSG_MOUSE_POSITION:
				_processMousePosition( (MousePositionMsg*) pMsg );
				break;
	
			case WG_MSG_MOUSE_LEAVE:
				_processMouseLeave( (MouseLeaveMsg*) pMsg );
				break;
	
			case WG_MSG_MOUSE_PRESS:
				_processMouseButtonPress( (MousePressMsg*) pMsg );
				break;
	
			case WG_MSG_MOUSE_REPEAT:
				_processMouseButtonRepeat( (MouseRepeatMsg*) pMsg );
				break;
	
			case WG_MSG_MOUSE_DRAG:
				_processMouseButtonDrag( (MouseDragMsg*) pMsg );
				break;
	
			case WG_MSG_MOUSE_RELEASE:
				_processMouseButtonRelease( (MouseReleaseMsg*) pMsg );
				break;
	
			case WG_MSG_MOUSE_CLICK:
				_processMouseButtonClick( (MouseClickMsg*) pMsg );
				break;
	
			case WG_MSG_MOUSE_DOUBLE_CLICK:
				_processMouseButtonDoubleClick( (MouseDoubleClickMsg*) pMsg );
				break;
	
			case WG_MSG_KEY_PRESS:
				_processKeyPress( (KeyPressMsg*) pMsg );
				break;
	
			case WG_MSG_KEY_REPEAT:
				_processKeyRepeat( (KeyRepeatMsg*) pMsg );
				break;
	
			case WG_MSG_KEY_RELEASE:
				_processKeyRelease( (KeyReleaseMsg*) pMsg );
				break;
	
			case WG_MSG_CHARACTER:
				_processCharacter( (CharacterMsg*) pMsg );
				break;
	
			case WG_MSG_WHEEL_ROLL:
				_processMouseWheelRoll( (WheelRollMsg*) pMsg );
				break;
	
			case WG_MSG_TICK:
				_processTick( (TickMsg*) pMsg );
				break;
	
			case WG_MSG_DUMMY:
				break;
	
			case WG_MSG_MAX:
				assert(false);								// NOT ALLOWED!
				break;
		}
	}
	
	//____ _processTick() ______________________________________________________
	
	void MsgRouter::_processTick( TickMsg * pMsg )
	{
		// Check if we need to post BUTTON_REPEAT
	
		for( int button = 0 ; button <= WG_MAX_BUTTONS ; button++ )
		{
			if( m_bButtonPressed[button] )
			{
				int buttonDelay = Base::mouseButtonRepeatDelay();
				int buttonRate = Base::mouseButtonRepeatRate();
	
				int msSinceRepeatStart = (int) (m_time - m_pLatestPressMsgs[button]->timestamp() - buttonDelay );
	
				// First BUTTON_REPEAT event posted separately.
	
				if( msSinceRepeatStart < 0 && msSinceRepeatStart + pMsg->millisec() >= 0 )
					post( new MouseRepeatMsg((WgMouseButton)button) );
	
				// Calculate ms since last BUTTON_REPEAT event
	
				int msToProcess;
				if( msSinceRepeatStart < 0 )
					msToProcess = msSinceRepeatStart + pMsg->millisec();
				else
					msToProcess = (msSinceRepeatStart % buttonRate) + pMsg->millisec();
	
				// Post the amount of BUTTON_REPEAT that should be posted.
	
				while( msToProcess >= buttonRate )
				{
					post( new MouseRepeatMsg((WgMouseButton)button) );
					msToProcess -= buttonRate;
				}
			}
		}
	
		// Check if we need to post KEY_REPEAT
	
		int keyDelay 	= Base::keyRepeatDelay();
		int keyRate 	= Base::keyRepeatRate();
	
		for( unsigned int i = 0 ; i < m_keysDown.size() ; i++ )
		{
			KeyDownInfo * pInfo = m_keysDown[i];
			int timePassed = (int) (pMsg->timestamp() - pInfo->pMsg->timestamp());
	
			int fraction = timePassed - keyDelay;
	
			if( fraction < 0 )
				fraction += keyRate;
			else
				fraction %= keyRate;
	
			fraction += pMsg->millisec();
	
			while( fraction >= keyRate )
			{
				post( new KeyRepeatMsg( pInfo->pMsg->nativeKeyCode() ) );
				fraction -= keyRate;
			}
		}
	
	
		// Increase time counter
	
		m_time += pMsg->millisec();
	}
	
	
	//____ _processFocusGained() __________________________________________________
	
	void MsgRouter::_processFocusGained( FocusGainedMsg * pMsg )
	{
		if( !m_bWindowFocus && m_keyFocusWidget )
			_setWidgetFocused( m_keyFocusWidget.rawPtr(), true );
	
		m_bWindowFocus = true;
	}
	
	//____ _processFocusLost() ____________________________________________________
	
	void MsgRouter::_processFocusLost( FocusLostMsg * pMsg )
	{
		if( m_bWindowFocus && m_keyFocusWidget )
			_setWidgetFocused( m_keyFocusWidget.rawPtr(), false );
	
		m_bWindowFocus = false;
	}
	
	
	//____ _processMouseEnter() __________________________________________________
	
	void MsgRouter::_processMouseEnter( MouseEnterMsg * pMsg )
	{
		// Post events for button drag
	
		for( int i = 0 ; i <= WG_MAX_BUTTONS ; i++ )
		{
			if( m_bButtonPressed[i] )
				post( new MouseDragMsg((WgMouseButton)i, m_pLatestPressMsgs[i]->pointerPos(), m_pointerPos, pMsg->pointerPos() ) );
		}
	
		// Post event for finalizing position once button drag is taken care of.
	
		post( new MousePositionMsg() );
	
		// Update pointer position
	
		m_pointerPos = pMsg->pointerPos();
	}
	
	//____ _processMouseLeave() ___________________________________________________
	
	void MsgRouter::_processMouseLeave( MouseLeaveMsg * pMsg )
	{
		// Post POINTER_EXIT event to marked widget
	
		Widget * pWidget = m_pMarkedWidget.rawPtr();
	
		if( pWidget )
			post( new MouseLeaveMsg( pWidget ) );
	
		m_pMarkedWidget = 0;
	}
	
	
	//____ _processMouseMove() ___________________________________________________
	
	void MsgRouter::_processMouseMove( MouseMoveMsg * pMsg )
	{
		// Post events for button drag
	
		for( int i = 0 ; i <= WG_MAX_BUTTONS ; i++ )
		{
			if( m_bButtonPressed[i] )
				post( new MouseDragMsg( (WgMouseButton) i, m_pLatestPressMsgs[i]->pointerPos(), m_pointerPos, pMsg->pointerPos() ) );
		}
	
		// Post event for finalizing move once button drag is taken care of.
	
		post( new MousePositionMsg() );
	
		// Update pointer position
	
		m_pointerPos = pMsg->pointerPos();
	}
	
	//____ _processMousePosition() _________________________________________________
	
	void MsgRouter::_processMousePosition( MousePositionMsg * pMsg )
	{
		_updateMarkedWidget(true);
	
	}
	
	//____ _setWidgetFocused() ____________________________________________________
	
	void MsgRouter::_setWidgetFocused( Widget * pWidget, bool bFocused )
	{
			State oldState = pWidget->m_state;
			pWidget->m_state.setFocused(bFocused);
	
			if( pWidget->m_state != oldState )
				pWidget->_onStateChanged(oldState);
	}
	
	
	//____ _updateMarkedWidget() _______________________________________________
	
	void MsgRouter::_updateMarkedWidget(bool bPostMouseMoveMsgs)
	{
		Widget_p pNowMarked = 0;
	
		Widget_p pWidgetTarget = m_pRoot->findWidget( m_pointerPos, WG_SEARCH_ACTION_TARGET );
	
		// Figure out which button of currently pressed has been pressed the longest.
		// Mouse is only allowed to mark Widgets that were marked on press of that button.
	
		int button = 0;								// Button that has been pressed for longest, 0 = no button pressed
		for( int i = 1 ; i <= WG_MAX_BUTTONS ; i++ )
		{
			if( m_bButtonPressed[i] && (button == 0 || m_pLatestPressMsgs[i]->timestamp() < m_pLatestPressMsgs[button]->timestamp()) )
				button = i;
		}
	
		// We are only marking the Widget if no mouse button is pressed or the first pressed button
		// was pressed on it.
	
		if( button == 0 || pWidgetTarget == m_latestPressWidgets[button].rawPtr() )
			pNowMarked = pWidgetTarget;
	
		// Post Leave events for widgets no longer marked,
		// Post Enter events for new marked widgets
		// and Move events for those already marked
	
		Widget * pFirstAlreadyMarked = _updateEnteredWidgets( pNowMarked.rawPtr() );
		
		if( bPostMouseMoveMsgs && pFirstAlreadyMarked )
			post( new MouseMoveMsg( pFirstAlreadyMarked ) );
	
		// Copy content of pNowMarked to m_pMarkedWidget
	
		m_pMarkedWidget = pNowMarked.rawPtr();
		
		// Update PointerStyle
		
		WgPointerStyle newStyle;
		
		if( pNowMarked && pNowMarked->isEnabled() )
			newStyle = pNowMarked->pointerStyle();
		else if( button != 0 )
			newStyle = m_pointerStyle;
		else
			newStyle = WG_POINTER_DEFAULT;
	
		if( newStyle != m_pointerStyle )
		{
			post( new PointerChangeMsg( newStyle ) );
			m_pointerStyle = newStyle;
		}	
	}
	
	//____ _updateEnteredWidgets() _________________________________________________
	
	Widget * MsgRouter::_updateEnteredWidgets( Widget * pMarkedWidget )
	{
		
		// Loop through our new widgets and check if they already
		// were entered. Send MouseEnter to all new widgets and notice the first
		// common ancestor .
	
		Widget * pFirstAlreadyMarked = 0;
		for( Widget * pWidget = pMarkedWidget ; pWidget != 0 ; pWidget = pWidget->_parent() )
		{
			int ofs = _widgetPosInList( pWidget, m_vEnteredWidgets );
			if( ofs >= 0 )
			{	
				if( !pFirstAlreadyMarked )
					pFirstAlreadyMarked = pWidget;
				m_vEnteredWidgets[ofs] = 0;			
			}
			else
				post( new MouseEnterMsg( pWidget ) );		
		}
	
		// Send MouseLeave to those that were left.
	
		for( size_t i = 0 ; i < m_vEnteredWidgets.size() ; i++ )
			if(m_vEnteredWidgets[i] )
				post( new MouseLeaveMsg( m_vEnteredWidgets[i].rawPtr()) );
		
		// Replace the old list with a new one.
		
		m_vEnteredWidgets.clear();
		for( Widget * pWidget = pMarkedWidget ; pWidget != 0 ; pWidget = pWidget->_parent() )
			m_vEnteredWidgets.push_back( pWidget );
	
		// Return first already marked, calling function might need it.
	
		return pFirstAlreadyMarked;
	}
	
	
	//____ _processKeyPress() ______________________________________________________
	
	void MsgRouter::_processKeyPress( KeyPressMsg * pMsg )
	{
		// Fill in the info-structure.
	
		KeyDownInfo * pInfo = new KeyDownInfo();
		pInfo->pMsg = pMsg;
	
		// Post KEY_PRESS events for widgets and remember which ones we have posted it for
	
		Widget * pWidget = m_keyFocusWidget.rawPtr();
		post( new KeyPressMsg( pMsg->nativeKeyCode(), pWidget ) );
		pInfo->pWidget = pWidget;
	
		// Push the info-structure onto m_keysDown.
	
		m_keysDown.push_back( pInfo );
	
		// Update modkeys
	
		switch( pMsg->translatedKeyCode() )
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
	
	void MsgRouter::_processKeyRepeat( KeyRepeatMsg * pMsg )
	{
		// Find right KeyDownInfo structure
	
		KeyDownInfo * pInfo = 0;
	
		for( unsigned int i = 0 ; i < m_keysDown.size() ; i++ )
		{
			if( pMsg->nativeKeyCode() == m_keysDown[i]->pMsg->nativeKeyCode() )
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
				post( new KeyRepeatMsg( pMsg->nativeKeyCode(), pInfo->pWidget.rawPtr() ));
	}
	
	//____ _processKeyRelease() ___________________________________________________
	
	void MsgRouter::_processKeyRelease( KeyReleaseMsg * pMsg )
	{
		// Find right KeyDownInfo structure and remove it from m_keysDown.
	
		KeyDownInfo * pInfo = 0;
	
		std::vector<KeyDownInfo*>::iterator it = m_keysDown.begin();
		while( it != m_keysDown.end() )
		{
			if( pMsg->nativeKeyCode() == (*it)->pMsg->nativeKeyCode() )
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
			post( new KeyReleaseMsg( pMsg->nativeKeyCode(), pInfo->pWidget.rawPtr() ));
	
		// Delete the KeyPress-message and KeyDownInfo-structure
	
		delete pInfo;
	
		// Update modkeys
	
		switch( pMsg->translatedKeyCode() )
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
	
	void MsgRouter::_processCharacter( CharacterMsg * pMsg )
	{
		Widget * pWidget = m_keyFocusWidget.rawPtr();
	
		if( pWidget )
			post( new CharacterMsg( pMsg->character(), pWidget ) );
	}
	
	//____ _processMouseWheelRoll() ____________________________________________________
	
	void MsgRouter::_processMouseWheelRoll( WheelRollMsg * pMsg )
	{
		_updateMarkedWidget(false);
	
		Widget * pWidget = m_pMarkedWidget.rawPtr();
	
		if( pWidget )
			post( new WheelRollMsg( pMsg->wheel(), pMsg->distance(), pWidget ) );
	}
	
	//____ _processMouseButtonPress() ___________________________________________________
	
	void MsgRouter::_processMouseButtonPress( MousePressMsg * pMsg )
	{
		_updateMarkedWidget(false);
	
		WgMouseButton button = pMsg->button();
	
		// Update m_previousPressWidgets
	
		m_previousPressWidgets[button] = 0;
	
		Widget * pWidget = m_latestPressWidgets[button].rawPtr();
		if( pWidget )
			m_previousPressWidgets[button] = pWidget;
	
		// Post BUTTON_PRESS events for marked widgets and remember which one we have posted it for
	
		m_latestPressWidgets[button] = 0;
	
		pWidget = m_pMarkedWidget.rawPtr();
		if( pWidget )
		{
			post( new MousePressMsg( button, pWidget ) );
			m_latestPressWidgets[button] = pWidget;
		}
	
		// Handle possible double-click
	
		int doubleClickTimeTreshold = Base::doubleClickTimeTreshold();
		int doubleClickDistanceTreshold = Base::doubleClickDistanceTreshold();
	
	
		if( m_pLatestPressMsgs[button] && m_pLatestPressMsgs[button]->timestamp() + doubleClickTimeTreshold > pMsg->timestamp() )
		{
			Coord distance = pMsg->pointerPos() - m_pLatestPressMsgs[button]->pointerPos();
	
			if( distance.x <= doubleClickDistanceTreshold &&
				distance.x >= -doubleClickDistanceTreshold &&
				distance.y <= doubleClickDistanceTreshold &&
				distance.y >= -doubleClickDistanceTreshold )
				post( new MouseDoubleClickMsg(button) );
		}
	
		// Save info for the future
	
		m_pLatestPressMsgs[button] = pMsg;
	
		m_bButtonPressed[button] = true;
	}
	
	
	//____ _processMouseButtonRepeat() __________________________________________________
	
	void MsgRouter::_processMouseButtonRepeat( MouseRepeatMsg * pMsg )
	{
		_updateMarkedWidget(false);
	
		WgMouseButton button = pMsg->button();
	
		// Post BUTTON_REPEAT event for widget that received the press if we are still inside.
	
		Widget * pWidget = m_latestPressWidgets[button].rawPtr();
		if( pWidget && pWidget == m_pMarkedWidget.rawPtr() )
			post( new MouseRepeatMsg(button, pWidget) );
	}
	
	
	
	//____ _processMouseButtonRelease() _________________________________________________
	
	void MsgRouter::_processMouseButtonRelease( MouseReleaseMsg * pMsg )
	{
		_updateMarkedWidget(false);
	
		WgMouseButton button = pMsg->button();
	
		// Post BUTTON_RELEASE events for widget that was pressed
	
		Widget * pWidget = m_latestPressWidgets[button].rawPtr();
		if( pWidget )
		{
			bool bIsInside = pWidget->globalGeo().contains(pMsg->pointerPos());
			post( new MouseReleaseMsg( button, pWidget, true, bIsInside ) );
		}
	
		// Post BUTTON_RELEASE events for marked widget that was NOT pressed
	
		pWidget = m_pMarkedWidget.rawPtr();
		if( pWidget )
		{
			if( pWidget != m_latestPressWidgets[button].rawPtr() )
			{
				bool bIsInside = pWidget->globalGeo().contains(pMsg->pointerPos());
				post( new MouseReleaseMsg( button, pWidget, false, bIsInside ) );
			}
		}
	
		// As long as the button was pressed inside our window we have a click
		// on this level.
	
		if( m_bButtonPressed[button] )
			post( new MouseClickMsg( button ) );
	
		// Save info for the future
	
		m_pLatestReleaseMsgs[button] = pMsg;
		m_bButtonPressed[button] = false;
	}
	
	//____ _processMouseButtonDrag() ____________________________________________________
	
	void MsgRouter::_processMouseButtonDrag( MouseDragMsg * pMsg )
	{
		WgMouseButton button = pMsg->button();
	
		// Post POINTER_DRAG event for pressed widget
	
		Widget * pWidget = m_latestPressWidgets[button].rawPtr();
	
		if( pWidget )
		{
			Coord	ofs = pWidget->globalPos();
			post( new MouseDragMsg( button, pWidget, pMsg->startPos() - ofs, pMsg->prevPos() - ofs, pMsg->currPos() - ofs ) );
		}
	
	}
	
	//____ _processMouseButtonClick() _________________________________________________
	
	void MsgRouter::_processMouseButtonClick( MouseClickMsg * pMsg )
	{
		WgMouseButton button = pMsg->button();
	
		// Post BUTTON_CLICK events for widget that received the press if we
		// still are inside.
	
		Widget * pWidget = m_latestPressWidgets[button].rawPtr();
		if( pWidget && pWidget == m_pMarkedWidget.rawPtr() )
			post( new MouseClickMsg(button, pWidget) );
	}
	
	//____ _processMouseButtonDoubleClick() _________________________________________________
	
	void MsgRouter::_processMouseButtonDoubleClick( MouseDoubleClickMsg * pMsg )
	{
		WgMouseButton button = pMsg->button();
	
		// Post BUTTON_DOUBLE_CLICK event if gizom received both this and previous press.
	
		Widget * pWidget = m_latestPressWidgets[button].rawPtr();
		if( pWidget && pWidget ==  m_previousPressWidgets[button].rawPtr() )
			post( new MouseDoubleClickMsg(button, pWidget) );
	}
	
	//____ _widgetPosInList() ________________________________________________________
	
	int MsgRouter::_widgetPosInList( const Widget * pWidget, const std::vector<Widget_wp>& list )
	{
		for( size_t i = 0 ; i < list.size() ; i++ )
			if( list[i].rawPtr() == pWidget )
				return i;
	
		return -1;
	}
	
	
	MsgRouter::Route::Route( const MsgFilter& filter, Receiver * pReceiver )
	{
		m_filter = filter;
		m_pReceiver = pReceiver;
		pReceiver->_onRouteAdded();
	}
	
	MsgRouter::Route::~Route()
	{
		if( m_pReceiver )
			m_pReceiver->_onRouteRemoved();
	}
	
	void MsgRouter::Route::dispatch( const Msg_p& pMsg )
	{
		if( m_filter.filterMsg(pMsg) )
			m_pReceiver->onMsg( pMsg );
	}
	
	bool MsgRouter::Route::isAlive() const
	{
		return true;
	}
	
	Receiver * MsgRouter::Route::receiver() const
	{
		return m_pReceiver.rawPtr();
	}
	

} // namespace wg
