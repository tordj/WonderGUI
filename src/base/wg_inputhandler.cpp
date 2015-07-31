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
		m_tickRoute = Base::msgRouter()->addRoute( WG_MSG_TICK, this );		
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
	
	//____ setPointer() ___________________________________________________________
	
	
	void InputHandler::setPointer( const RootPanel_p& pRoot, Coord pos )
	{
		Coord	prevPointerPos = m_pointerPos;
		
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
		
		if( pFirstAlreadyMarked )
			Base::msgRouter()->post( new MouseMoveMsg( pFirstAlreadyMarked ) );
	
		// Copy content of pNowMarked to m_pMarkedWidget
	
		m_pMarkedWidget = pNowMarked.rawPtr();
	
		// Post events for button drag
	
		for( int i = 0 ; i <= WG_MAX_BUTTONS ; i++ )
		{
			if( m_bButtonPressed[i] )
				Base::msgRouter()->post( new MouseDragMsg( (WgMouseButton) i, m_pLatestPressMsgs[i]->pointerPos(), prevPointerPos, m_pointerPos ) );
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
	
	Widget * InputHandler::_updateEnteredWidgets( Widget * pMarkedWidget )
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
				Base::msgRouter()->post( new MouseEnterMsg( pWidget ) );		
		}
	
		// Send MouseLeave to those that were left.
	
		for( size_t i = 0 ; i < m_vEnteredWidgets.size() ; i++ )
			if(m_vEnteredWidgets[i] )
				Base::msgRouter()->post( new MouseLeaveMsg( m_vEnteredWidgets[i].rawPtr()) );
		
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
	
	void InputHandler::setButton( WgMouseButton button, bool bPressed )
	{
		// Sanity checks
		
		if( m_bButtonPressed[button] == bPressed )
			return;
			
		//
		
		if( bPressed )
			_processMouseButtonPress( button );
		else
			_processMouseButtonRelease( button );
		
		m_bButtonPressed[button] = bPressed;
	}
	
	
	
	//____ _processMouseButtonPress() ___________________________________________________
	
	void InputHandler::_processMouseButtonPress( WgMouseButton button )
	{
	
		// Post BUTTON_PRESS events for marked widgets and remember which one we have posted it for
	
		Widget * pWidget = m_pMarkedWidget.rawPtr();
	
		MousePressMsg * pMsg = new MousePressMsg( button, pWidget );
		Base::msgRouter()->post( pMsg );
	
		// Handle possible double-click
	
		int doubleClickTimeTreshold = Base::doubleClickTimeTreshold();
		int doubleClickDistanceTreshold = Base::doubleClickDistanceTreshold();
	
	
		if( m_pLatestPressMsgs[button] && m_pLatestPressMsgs[button]->timestamp() + doubleClickTimeTreshold > m_timeStamp )
		{
			Coord distance = m_pointerPos - m_pLatestPressMsgs[button]->pointerPos();
	
			if( distance.x <= doubleClickDistanceTreshold &&
				distance.x >= -doubleClickDistanceTreshold &&
				distance.y <= doubleClickDistanceTreshold &&
				distance.y >= -doubleClickDistanceTreshold )
				{
					if( pWidget && pWidget ==  m_latestPressWidgets[button].rawPtr() )
						Base::msgRouter()->post( new MouseDoubleClickMsg(button, pWidget) );
					else
						Base::msgRouter()->post( new MouseDoubleClickMsg(button) );
				}
		}
	
		// Save info for the future
	
		m_latestPressWidgets[button] = pWidget;
		m_pLatestPressMsgs[button] = pMsg;
	}
	
	
	
	//____ _processMouseButtonRelease() _________________________________________________
	
	void InputHandler::_processMouseButtonRelease( WgMouseButton button )
	{
		// Post BUTTON_RELEASE events for widget that was pressed
	
		Widget * pWidget = m_latestPressWidgets[button].rawPtr();
		bool bIsInside = pWidget ? pWidget->globalGeo().contains( m_pointerPos ) : false;
	
		MouseReleaseMsg * pMsg = new MouseReleaseMsg( button, pWidget, true, bIsInside );
		Base::msgRouter()->post( pMsg );
	
		// Post click event.
	
		if( m_bButtonPressed[button] )
		{
			if( bIsInside )
				Base::msgRouter()->post( new MouseClickMsg( button, pWidget ) );
			else
				Base::msgRouter()->post( new MouseClickMsg( button ) );
		}
	
		// Save info for the future
	
		m_pLatestReleaseMsgs[button] = pMsg;
	}
	
	
	//____ setFocused() ____________________________________________________________
	
	void InputHandler::setFocused( const RootPanel_p& pRoot )
	{
		
	}
	
	
	//____ setKey() ________________________________________________________________
	
	void InputHandler::setKey( short nativeKeyCode, bool bPressed )
	{
		
	}
	
	//____ setWheelRoll() __________________________________________________________
	
	void InputHandler::setWheelRoll( int wheel, int steps )
	{
		
	}
	
	//____ onMsg() _________________________________________________________________
	
	void InputHandler::onMsg( const Msg_p& pMsg )
	{
		
	}

} // namespace wg
