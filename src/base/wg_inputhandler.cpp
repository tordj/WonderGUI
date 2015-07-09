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

const char WgInputHandler::CLASSNAME[] = {"InputHandler"};

//____ Constructor ____________________________________________________________

WgInputHandler::WgInputHandler()
{
	m_tickRoute = WgBase::MsgRouter()->AddRoute( WG_MSG_TICK, this );		
}

//____ Destructor _____________________________________________________________

WgInputHandler::~WgInputHandler()
{
	WgBase::MsgRouter()->DeleteRoute(m_tickRoute);
}

//____ IsInstanceOf() _________________________________________________________

bool WgInputHandler::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgObject::IsInstanceOf(pClassName);
}

//____ ClassName() ____________________________________________________________

const char * WgInputHandler::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ Cast() _________________________________________________________________

WgInputHandlerPtr WgInputHandler::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgInputHandlerPtr( static_cast<WgInputHandler*>(pObject.RawPtr()) );

	return 0;
}

//____ SetPointer() ___________________________________________________________


void WgInputHandler::SetPointer( const WgRootPanelPtr& pRoot, WgCoord pos )
{
	WgCoord	prevPointerPos = m_pointerPos;
	
	m_pointerPos = pos;
	m_pMarkedRoot = 0;

	WgWidgetPtr pNowMarked = 0;
	WgWidgetPtr pWidgetTarget = 0;

	if( pRoot && pRoot->Geo().Contains( pos ) )
	{
		m_pMarkedRoot = pRoot.RawPtr();
		pWidgetTarget = pRoot->FindWidget( m_pointerPos, WG_SEARCH_ACTION_TARGET );
	}

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
	
	if( pFirstAlreadyMarked )
		WgBase::MsgRouter()->Post( new WgMouseMoveMsg( pFirstAlreadyMarked ) );

	// Copy content of pNowMarked to m_pMarkedWidget

	m_pMarkedWidget = pNowMarked.RawPtr();

	// Post events for button drag

	for( int i = 0 ; i <= WG_MAX_BUTTONS ; i++ )
	{
		if( m_bButtonPressed[i] )
			WgBase::MsgRouter()->Post( new WgMouseDragMsg( (WgMouseButton) i, m_pLatestPressMsgs[i]->PointerPos(), prevPointerPos, m_pointerPos ) );
	}
	
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
		WgBase::MsgRouter()->Post( new WgPointerChangeMsg( newStyle ) );
		m_pointerStyle = newStyle;
	}	
}

//____ _updateEnteredWidgets() _________________________________________________

WgWidget * WgInputHandler::_updateEnteredWidgets( WgWidget * pMarkedWidget )
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
			WgBase::MsgRouter()->Post( new WgMouseEnterMsg( pWidget ) );		
	}

	// Send MouseLeave to those that were left.

	for( size_t i = 0 ; i < m_vEnteredWidgets.size() ; i++ )
		if(m_vEnteredWidgets[i] )
			WgBase::MsgRouter()->Post( new WgMouseLeaveMsg( m_vEnteredWidgets[i].RawPtr()) );
	
	// Replace the old list with a new one.
	
	m_vEnteredWidgets.clear();
	for( WgWidget * pWidget = pMarkedWidget ; pWidget != 0 ; pWidget = pWidget->_parent() )
		m_vEnteredWidgets.push_back( pWidget );

	// Return first already marked, calling function might need it.

	return pFirstAlreadyMarked;
}

//____ _widgetPosInList() ________________________________________________________

int WgInputHandler::_widgetPosInList( const WgWidget * pWidget, const std::vector<WgWidgetWeakPtr>& list )
{
	for( size_t i = 0 ; i < list.size() ; i++ )
		if( list[i].RawPtr() == pWidget )
			return i;

	return -1;
}



//____ SetButton() _____________________________________________________________

void WgInputHandler::SetButton( WgMouseButton button, bool bPressed )
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

void WgInputHandler::_processMouseButtonPress( WgMouseButton button )
{

	// Post BUTTON_PRESS events for marked widgets and remember which one we have posted it for

	WgWidget * pWidget = m_pMarkedWidget.RawPtr();

	WgMousePressMsg * pMsg = new WgMousePressMsg( button, pWidget );
	WgBase::MsgRouter()->Post( pMsg );

	// Handle possible double-click

	int doubleClickTimeTreshold = WgBase::DoubleClickTimeTreshold();
	int doubleClickDistanceTreshold = WgBase::DoubleClickDistanceTreshold();


	if( m_pLatestPressMsgs[button] && m_pLatestPressMsgs[button]->Timestamp() + doubleClickTimeTreshold > m_timeStamp )
	{
		WgCoord distance = m_pointerPos - m_pLatestPressMsgs[button]->PointerPos();

		if( distance.x <= doubleClickDistanceTreshold &&
			distance.x >= -doubleClickDistanceTreshold &&
			distance.y <= doubleClickDistanceTreshold &&
			distance.y >= -doubleClickDistanceTreshold )
			{
				if( pWidget && pWidget ==  m_latestPressWidgets[button].RawPtr() )
					WgBase::MsgRouter()->Post( new WgMouseDoubleClickMsg(button, pWidget) );
				else
					WgBase::MsgRouter()->Post( new WgMouseDoubleClickMsg(button) );
			}
	}

	// Save info for the future

	m_latestPressWidgets[button] = pWidget;
	m_pLatestPressMsgs[button] = pMsg;
}



//____ _processMouseButtonRelease() _________________________________________________

void WgInputHandler::_processMouseButtonRelease( WgMouseButton button )
{
	// Post BUTTON_RELEASE events for widget that was pressed

	WgWidget * pWidget = m_latestPressWidgets[button].RawPtr();
	bool bIsInside = pWidget ? pWidget->GlobalGeo().Contains( m_pointerPos ) : false;

	WgMouseReleaseMsg * pMsg = new WgMouseReleaseMsg( button, pWidget, true, bIsInside );
	WgBase::MsgRouter()->Post( pMsg );

	// Post click event.

	if( m_bButtonPressed[button] )
	{
		if( bIsInside )
			WgBase::MsgRouter()->Post( new WgMouseClickMsg( button, pWidget ) );
		else
			WgBase::MsgRouter()->Post( new WgMouseClickMsg( button ) );
	}

	// Save info for the future

	m_pLatestReleaseMsgs[button] = pMsg;
}


//____ SetFocused() ____________________________________________________________

void WgInputHandler::SetFocused( const WgRootPanelPtr& pRoot )
{
	
}


//____ SetKey() ________________________________________________________________

void WgInputHandler::SetKey( short nativeKeyCode, bool bPressed )
{
	
}

//____ SetWheelRoll() __________________________________________________________

void WgInputHandler::SetWheelRoll( int wheel, int steps )
{
	
}

//____ OnMsg() _________________________________________________________________

void WgInputHandler::OnMsg( const WgMsgPtr& pMsg )
{
	
}
