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
#include <wg_inputhandler.h>
#include <wg_base.h>
#include <wg_rootpanel.h>

namespace wg 
{
	
	const char InputHandler::CLASSNAME[] = {"InputHandler"};
	
	//____ Constructor ____________________________________________________________
	
	InputHandler::InputHandler()
	{
		m_tickRoute 	= Base::msgRouter()->addRoute( WG_MSG_TICK, this );	
		m_timeStamp 	= 0;
		m_pointerStyle 	= WG_POINTER_DEFAULT;
		m_modKeys 		= WG_MODKEY_NONE;

		m_doubleClickTimeTreshold 		= 250;
		m_doubleClickDistanceTreshold 	= 2;
	
		m_buttonRepeatDelay 	= 300;
		m_buttonRepeatRate 		= 200;
	
		m_keyRepeatDelay 		= 300;
		m_keyRepeatRate 		= 150;


		
		for( int i = 0 ; i < WG_MAX_BUTTONS+1 ; i++ )
		{
			m_bButtonPressed[i] = false;
			m_latestPressTimestamps[i] = 0;
		}
	}
	
	//____ Destructor _____________________________________________________________
	
	InputHandler::~InputHandler()
	{
		Base::msgRouter()->deleteRoute(m_tickRoute);
	}
	
	//____ isInstanceOf() _________________________________________________________
	
	bool InputHandler::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return Object::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * InputHandler::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	InputHandler_p InputHandler::cast( const Object_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return InputHandler_p( static_cast<InputHandler*>(pObject.rawPtr()) );
	
		return 0;
	}
	
	//____ setDoubleClickTresholds() _______________________________________________
	
	bool InputHandler::setDoubleClickTresholds( int time, int distance )
	{
		if( time <= 0 || distance <= 0 )
			return false;
	
		m_doubleClickTimeTreshold		= time;
		m_doubleClickDistanceTreshold	= distance;
		return true;
	}
	
	//____ setButtonRepeat() ______________________________________________________
	
	bool InputHandler::setButtonRepeat( int delay, int rate )
	{
		if( delay <= 0 || rate <= 0 )
			return false;
	
		m_buttonRepeatDelay	= delay;
		m_buttonRepeatRate	= rate;
		return true;
	}
	
	//____ setKeyRepeat() _________________________________________________________
	
	bool InputHandler::setKeyRepeat( int delay, int rate )
	{
		if( delay <= 0 || rate <= 0 )
			return false;
	
		m_keyRepeatDelay	= delay;
		m_keyRepeatRate		= rate;
		return true;
	}
	
	//____ mapKey() _______________________________________________________________
	
	void InputHandler::mapKey( WgKey translated_keycode, int native_keycode )
	{
		m_keycodeMap[native_keycode] = translated_keycode;
	}
	
	
	//____ unmapKey() _____________________________________________________________
	
	void InputHandler::unmapKey( WgKey translated_keycode )
	{
		auto it = m_keycodeMap.begin();
	
		while( it != m_keycodeMap.end() )
		{
			if( it->second == translated_keycode )
			{
				auto it2 = it++;
				m_keycodeMap.erase(it2);
			}
			else
				++it;
		}
	}
	
	//____ clearKeyMap() __________________________________________________________
	
	void InputHandler::clearKeyMap()
	{
		m_keycodeMap.clear();
	}
	
	//____ translateKey() __________________________________________________________
	
	WgKey InputHandler::translateKey( int native_keycode )
	{
		auto it = m_keycodeMap.find(native_keycode);
		if( it != m_keycodeMap.end() )
			return  it->second;
		else
			return WG_KEY_UNMAPPED;
	}
	
	//____ setPointer() ___________________________________________________________
	
	
	void InputHandler::setPointer( const RootPanel_p& pRoot, Coord pos, int64_t timestamp )
	{
		Coord	prevPointerPos = m_pointerPos;
		if( timestamp == 0 )
			timestamp = m_timeStamp;
		
		m_pointerPos = pos;
		m_pMarkedRoot = 0;
	
		Widget_p pNowMarked = 0;
		Widget_p pWidgetTarget = 0;
	
		if( pRoot && pRoot->geo().contains( pos ) )
		{
			m_pMarkedRoot = pRoot.rawPtr();
			pWidgetTarget = pRoot->findWidget( m_pointerPos, WG_SEARCH_ACTION_TARGET );
		}
	
		// Figure out which button of currently pressed has been pressed the longest.
		// Mouse is only allowed to mark Widgets that were marked on press of that button.
	
		int button = 0;								// Button that has been pressed for longest, 0 = no button pressed
		for( int i = 1 ; i <= WG_MAX_BUTTONS ; i++ )
		{
			if( m_bButtonPressed[i] && (button == 0 || m_latestPressTimestamps[i] < m_latestPressTimestamps[button]) )
				button = i;
		}
	
		// We are only marking the Widget if no mouse button is pressed or the first pressed button
		// was pressed on it.
	
		if( button == 0 || pWidgetTarget == m_latestPressWidgets[button].rawPtr() )
			pNowMarked = pWidgetTarget;
	
		// Post Leave events for widgets no longer marked,
		// Post Enter events for new marked widgets
		// and Move events for those already marked
	
		Widget * pFirstAlreadyMarked = _updateEnteredWidgets( pNowMarked.rawPtr(), timestamp );
		
		if( pFirstAlreadyMarked )
			Base::msgRouter()->post( new MouseMoveMsg( pFirstAlreadyMarked, m_modKeys, pos, timestamp ) );
	
		// Copy content of pNowMarked to m_pMarkedWidget
	
		m_pMarkedWidget = pNowMarked.rawPtr();
	
		// Post events for button drag
	
		for( int i = 0 ; i <= WG_MAX_BUTTONS ; i++ )
		{
			if( m_bButtonPressed[i] )
				Base::msgRouter()->post( new MouseDragMsg( (WgMouseButton) i, m_latestPressWidgets[i].rawPtr(), m_latestPressPosition[i], prevPointerPos, m_modKeys, m_pointerPos, timestamp ) );
		}
		
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
			Base::msgRouter()->post( new PointerChangeMsg( newStyle ) );
			m_pointerStyle = newStyle;
		}	
	}
	
	//____ _updateEnteredWidgets() _________________________________________________
	
	Widget * InputHandler::_updateEnteredWidgets( Widget * pMarkedWidget, int64_t timestamp )
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
				Base::msgRouter()->post( new MouseEnterMsg( pWidget, m_modKeys, m_pointerPos, timestamp ) );		
		}
	
		// Send MouseLeave to those that were left.
	
		for( size_t i = 0 ; i < m_vEnteredWidgets.size() ; i++ )
			if(m_vEnteredWidgets[i] )
				Base::msgRouter()->post( new MouseLeaveMsg( m_vEnteredWidgets[i].rawPtr(), m_modKeys, m_pointerPos, timestamp) );
		
		// Replace the old list with a new one.
		
		m_vEnteredWidgets.clear();
		for( Widget * pWidget = pMarkedWidget ; pWidget != 0 ; pWidget = pWidget->_parent() )
			m_vEnteredWidgets.push_back( pWidget );
	
		// Return first already marked, calling function might need it.
	
		return pFirstAlreadyMarked;
	}
	
	//____ _widgetPosInList() ________________________________________________________
	
	int InputHandler::_widgetPosInList( const Widget * pWidget, const std::vector<Widget_wp>& list )
	{
		for( size_t i = 0 ; i < list.size() ; i++ )
			if( list[i].rawPtr() == pWidget )
				return i;
	
		return -1;
	}
	
	
	
	//____ setButton() _____________________________________________________________
	
	void InputHandler::setButton( WgMouseButton button, bool bPressed, int64_t timestamp )
	{
		// Sanity checks
		
		if( m_bButtonPressed[button] == bPressed )
			return;
			
		//
		
		if( bPressed )
			_processButtonPress( button, timestamp );
		else
			_processButtonRelease( button, timestamp );
		
		m_bButtonPressed[button] = bPressed;
	}
	
	
	
	//____ _processButtonPress() ___________________________________________________
	
	void InputHandler::_processButtonPress( WgMouseButton button, int64_t timestamp )
	{			
		if( timestamp == 0 )
			timestamp = m_timeStamp;
			
		// Post BUTTON_PRESS events for marked widgets and remember which one we have posted it for
	
		Widget * pWidget = m_pMarkedWidget.rawPtr();
	
		MousePressMsg * pMsg = new MousePressMsg( button, pWidget, m_modKeys, m_pointerPos, timestamp );
		Base::msgRouter()->post( pMsg );
	
		// Handle possible double-click
	
		bool doubleClick = false;
	
		if( m_latestPressTimestamps[button] + m_doubleClickTimeTreshold > timestamp )
		{
			Coord distance = m_pointerPos - m_latestPressPosition[button];
	
			if( distance.x <= m_doubleClickDistanceTreshold &&
				distance.x >= -m_doubleClickDistanceTreshold &&
				distance.y <= m_doubleClickDistanceTreshold &&
				distance.y >= -m_doubleClickDistanceTreshold )
				{
					if( pWidget && pWidget ==  m_latestPressWidgets[button].rawPtr() )
						Base::msgRouter()->post( new MouseDoubleClickMsg(button, pWidget, m_modKeys, m_pointerPos, timestamp) );
					else
						Base::msgRouter()->post( new MouseDoubleClickMsg(button, 0, m_modKeys, m_pointerPos, timestamp) );
					
					doubleClick = true;
				}				
		}
	
		// Save info for the future
	
		m_latestPressWidgets[button]		= pWidget;
		m_latestPressTimestamps[button] 	= timestamp;
		m_latestPressPosition[button] 		= m_pointerPos;
		m_latestPressDoubleClick[button] 	= doubleClick;
	}
	
	
	
	//____ _processButtonRelease() _________________________________________________
	
	void InputHandler::_processButtonRelease( WgMouseButton button, int64_t timestamp )
	{
		if( timestamp == 0 )
			timestamp = m_timeStamp;
		
		
		// Post BUTTON_RELEASE events for widget that was pressed
	
		Widget * pWidget = m_latestPressWidgets[button].rawPtr();
		bool bIsInside = pWidget ? pWidget->globalGeo().contains( m_pointerPos ) : false;
	
		MouseReleaseMsg * pMsg = new MouseReleaseMsg( button, pWidget, bIsInside, m_modKeys, m_pointerPos, timestamp );
		Base::msgRouter()->post( pMsg );
	
		// Post click event, if press didn't already resulted in a double click.
	
		if( m_bButtonPressed[button] && !m_latestPressDoubleClick[button] )
		{
			if( bIsInside )
				Base::msgRouter()->post( new MouseClickMsg( button, pWidget, m_modKeys, m_pointerPos, timestamp ) );
			else
				Base::msgRouter()->post( new MouseClickMsg( button, 0, m_modKeys, m_pointerPos, timestamp ) );
		}	
	}
	

	//____ isButtonPressed() _________________________________________________________
	
	bool InputHandler::isButtonPressed( WgMouseButton button ) const
	{
		if( button >= 1 && button <= WG_MAX_BUTTONS )
			return m_bButtonPressed[button];
	
		return false;
	}
	
	//____ isAnyButtonPressed() ________________________________________________________
	
	bool InputHandler::isAnyButtonPressed() const
	{
		for( int i = 0 ; i < WG_MAX_BUTTONS ; i++ )
			if( m_bButtonPressed[i] )
				return true;
	
		return false;
	}

	
	//____ setFocusedWindow() ____________________________________________________________
	
	void InputHandler::setFocusedWindow( const RootPanel_p& pRoot )
	{
		if( pRoot == m_pFocusedRoot )
			return;
			
		if( m_pFocusedRoot )
		{
			Widget * p = m_pFocusedRoot->_focusedChild();
			if( p )
				_setUnfocused( p );			
		}
		m_pFocusedRoot = pRoot.rawPtr();
		
		if( pRoot )
		{
			Widget * p = pRoot->_focusedChild();

			if( p )
				_setFocused( p );
			else
				_setFocused( pRoot->widget().rawPtr() );				// Bottom container gets focus per default.			
		}
	}
	
	//____ _focusChanged() _____________________________________________________
	
	bool InputHandler::_focusChanged( Widget * pOldFocused, Widget * pNewFocused )
	{
		if( pOldFocused && pOldFocused->hook()->root() == m_pFocusedRoot )
			_setUnfocused( pOldFocused );
			
		if( pNewFocused && pNewFocused->hook()->root() == m_pFocusedRoot )
			_setFocused( pNewFocused );
	}
	
	//____ _focusedWidget() ____________________________________________________

	Widget * InputHandler::_focusedWidget() const
	{
		if( m_pFocusedRoot )
			return m_pFocusedRoot->_focusedChild();
		
		return 0;
	}
	
	//____ _setFocused() _______________________________________________________
	
	void InputHandler::_setFocused( Widget * pWidget ) 
	{
		FocusGainedMsg * pMsg = new FocusGainedMsg( pWidget );
		Base::msgRouter()->post( pMsg );
	}

	//____ _setUnfocused() _______________________________________________________
	
	void InputHandler::_setUnfocused( Widget * pWidget ) 
	{
		FocusLostMsg * pMsg = new FocusLostMsg( pWidget );
		Base::msgRouter()->post( pMsg );
	}
	
	//____ setKey() ________________________________________________________________
	
	void InputHandler::setKey( short nativeKeyCode, bool bPressed, int64_t timestamp )
	{
		if( timestamp == 0 )
			timestamp = m_timeStamp;
		
		if( bPressed )
			_processKeyPress( nativeKeyCode, timestamp );
		else
			_processKeyRelease( nativeKeyCode, timestamp );
	}
	
	
	//____ _processKeyPress() ______________________________________________________
	
	void InputHandler::_processKeyPress( short nativeKeyCode, int64_t timestamp )
	{
		// Verify that this key isn't already pressed
		
		for( auto i : m_keysDown )
		{
			if( i.nativeKeyCode == nativeKeyCode )
				return;
		}

		//
		
		WgKey translatedKeyCode = translateKey(nativeKeyCode);
		
		// Create and fill in the info-structure.
	
		KeyDownInfo info;
		info.nativeKeyCode = nativeKeyCode;
		info.translatedKeyCode = translatedKeyCode;
		info.pressTimestamp = timestamp;
		info.pWidget = _focusedWidget();

		m_keysDown.push_back( info );

		// Post KEY_PRESS message
	
		Widget * pWidget = _focusedWidget();
		Base::msgRouter()->post( new KeyPressMsg( nativeKeyCode, translatedKeyCode, pWidget, m_modKeys, m_pointerPos, timestamp ) );
		
		// Update modkeys
	
		switch( translatedKeyCode )
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


	//____ _processKeyRelease() ___________________________________________________
	
	void InputHandler::_processKeyRelease( short nativeKeyCode, int64_t timestamp )
	{
		// Find right KeyDownInfo structure and remove it from m_keysDown.
	
		auto it = m_keysDown.begin();
		while( it != m_keysDown.end() )
		{
			if( nativeKeyCode == it->nativeKeyCode )
			{
				m_keysDown.erase(it);
				break;
			}
			it++;
		}
	
		// Post KEY_RELEASE message
	
		WgKey translatedKeyCode = translateKey(nativeKeyCode);

		Widget * pWidget = _focusedWidget();
		Base::msgRouter()->post( new KeyReleaseMsg( nativeKeyCode, translatedKeyCode, pWidget, m_modKeys, m_pointerPos, timestamp ) );
	
		// Update modkeys
	
		switch( translatedKeyCode )
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
	
	
	//____ setWheelRoll() __________________________________________________________
	
	void InputHandler::setWheelRoll( int wheel, Coord distance, int64_t timestamp )
	{
		if( m_pMarkedWidget )
		{
			if( timestamp == 0 )
				timestamp = m_timeStamp;
			Base::msgRouter()->post( new WheelRollMsg( wheel, distance, m_pMarkedWidget.rawPtr(), m_modKeys, m_pointerPos, timestamp ) );
		}
	}

	//____ isKeyPressed() ______________________________________________________
	
	bool InputHandler::isKeyPressed( short nativeKeyCode ) const
	{
		for( auto key : m_keysDown )
			if( key.nativeKeyCode == nativeKeyCode )
				return true;

		return false;	
	}
	
	//____ onMsg() _________________________________________________________________
	
	void InputHandler::onMsg( const Msg_p& pMsg )
	{
		
		if( pMsg->type() == WG_MSG_TICK ) {
					
			int64_t timestamp = TickMsg::cast(pMsg)->timestamp();
	
			_handleButtonRepeats( timestamp );
			_handleKeyRepeats( timestamp );

			m_timeStamp = timestamp;			
		}		
	}

	//____ _handleButtonRepeats() _________________________________________

	void InputHandler::_handleButtonRepeats( int64_t timestamp )
	{
		for( int button = 0 ; button <= WG_MAX_BUTTONS ; button++ )
		{
			if( m_bButtonPressed[button] )
			{	
				int64_t firstRepeat = m_latestPressTimestamps[button] + m_buttonRepeatDelay;
				int64_t repeatPos;			// Next timestamp where a button repeat should occur.

				if( m_timeStamp < firstRepeat )
					repeatPos = firstRepeat;
				else
					repeatPos = firstRepeat + ((m_timeStamp - firstRepeat) / m_buttonRepeatRate) * m_buttonRepeatRate + m_buttonRepeatRate;  

				while( repeatPos <= timestamp )
				{
					Base::msgRouter()->post( new MouseRepeatMsg( (WgMouseButton)button, m_latestPressWidgets[button].rawPtr(), m_modKeys, m_pointerPos, repeatPos ));
					repeatPos += m_keyRepeatRate;
				}
			}
		}		
	}


	//____ _handleKeyRepeats() _________________________________________

	void InputHandler::_handleKeyRepeats( int64_t timestamp )
	{	
		for( auto key : m_keysDown )
		{
			int64_t firstRepeat = key.pressTimestamp + m_keyRepeatDelay;
			int64_t repeatPos;			// Next timestamp where a key repeat should occur.

			if( m_timeStamp < firstRepeat )
				repeatPos = firstRepeat;
			else
				repeatPos = firstRepeat + ((m_timeStamp - firstRepeat) / m_keyRepeatRate) * m_keyRepeatRate + m_keyRepeatRate;  

			while( repeatPos <= timestamp )
			{
				Base::msgRouter()->post( new KeyRepeatMsg( key.nativeKeyCode, key.translatedKeyCode, key.pWidget.rawPtr(), m_modKeys, m_pointerPos, repeatPos ));
				repeatPos += m_keyRepeatRate;
			}
		}	
	}


} // namespace wg
