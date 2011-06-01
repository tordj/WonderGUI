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


#include <memory.h>
#include <assert.h>
#include <wg_input.h>
#include <wg_widget.h>
#include <wg_gizmo.h>
#include <wg_pointerspy.h>
#include <wdg_root.h>



//____ WgInput() ______________________________________________________________

WgInput::WgInput()
{
	m_pRootWidget			= 0;
	m_pFocusedWidget		= 0;
	m_pFocusedGizmo			= 0;

	m_time					= 0;
	m_doubleClickTreshold	= 250;
	m_modifierKeys			= WG_MODKEY_NONE;

	m_buttonRepeatDelay		= 300;
	m_buttonRepeatRate		= 200;

	m_keyRepeatDelay		= 300;
	m_keyRepeatRate			= 150;

	m_pointerX				= -1;
	m_pointerY				= -1;

	for( int i = 0 ; i < WG_MAX_BUTTONS ; i++ )
	{
		m_bButtonDown[i] 		= false;
		m_pressed[i].x			= 0;
		m_pressed[i].y			= 0;
		m_pressed[i].timestamp	= 0;
		m_pressed[i].nWidgets	= 0;

		m_released[i].x 		= 0;
		m_released[i].y 		= 0;
		m_released[i].timestamp = 0;
		m_released[i].nWidgets	= 0;

		m_repeatTime[i] 		= 0;
	}

	m_position.x 			= 0;
	m_position.y			= 0;
	m_position.timestamp 	= 0;
	m_position.nWidgets 	= 0;

	m_currentPosition.x 		= 0;
	m_currentPosition.y 		= 0;
	m_currentPosition.timestamp	= 0;
	m_currentPosition.nWidgets 	= 0;

	m_keys.nKeysDown = 0;

	m_hoverStartTime = m_time;

	m_pPointerSpies = 0;
	m_nPointerSpies = 0;

	for( int i = 0 ; i < WG_KEY_ARRAYSIZE ; i++ )
		m_keycodeMap[i] = 0;
}

//____ ~WgInput() _____________________________________________________________

WgInput::~WgInput()
{
	if( m_pPointerSpies )
		delete [] m_pPointerSpies;
}

//____ map_keycode() __________________________________________________________

void WgInput::map_keycode( WgKey wg_keycode, Uint16 native_keycode )
{
	assert( wg_keycode < WG_KEY_ARRAYSIZE );
	if( wg_keycode > WG_KEY_ARRAYSIZE )
		return;

	m_keycodeMap[wg_keycode] = native_keycode;
}

//____ translate_keycode() ____________________________________________________

WgKey WgInput::translate_keycode( Uint16 native_keycode )
{
	for( int i = 0 ; i < WG_KEY_ARRAYSIZE ; i++ )
		if( m_keycodeMap[i] == native_keycode )
			return (WgKey) i;

	return WG_KEY_UNMAPPED;
}

//____ addPointerSpy() ________________________________________________________

void WgInput::addPointerSpy( WgPointerSpy * pSpy )
{
	for( Uint32 i = 0; i < m_nPointerSpies; ++i )
	{
		if( m_pPointerSpies[ i ] == pSpy )
			return;
	}

	WgPointerSpy** pNewArray = new WgPointerSpy*[ m_nPointerSpies + 1 ];
	if( m_pPointerSpies )
	{
		int oldSize = m_nPointerSpies * sizeof( *m_pPointerSpies );
		memcpy( pNewArray, m_pPointerSpies, oldSize );
		delete [] m_pPointerSpies;
		m_pPointerSpies = 0;
	}
	pNewArray[ m_nPointerSpies ] = pSpy;
	m_nPointerSpies++;
	m_pPointerSpies = pNewArray;
}

//____ removePointerSpy() _____________________________________________________
bool WgInput::removePointerSpy( WgPointerSpy * pSpy )
{
	Uint32 i = 0;
	for( i = 0; i < m_nPointerSpies; ++i )
	{
		if( m_pPointerSpies[ i ] == pSpy )
			break;
	}
	if( i >= m_nPointerSpies )
		return false;

	WgPointerSpy** pNewArray = 0;
	if( m_nPointerSpies > 1 )
	{
		pNewArray = new WgPointerSpy*[ m_nPointerSpies - 1 ];
		int iNewPos = 0;
		for( Uint32 i = 0; i < m_nPointerSpies; ++i )
		{
			if( m_pPointerSpies[ i ] != pSpy )
				pNewArray[ iNewPos++ ] = m_pPointerSpies[ i ];
		}
	}

	m_nPointerSpies--;
	delete m_pPointerSpies;
	m_pPointerSpies = pNewArray;

	return true;
}


//____ begin_events() _________________________________________________________

void WgInput::begin_events( Uint32 milliseconds )
{
	m_time = milliseconds;
}


//____ flush_event_queue() ___________________________________________________________

void WgInput::flush_event_queue()
{
	for(size_t i = 0; i < m_eventQueue.size(); i++)
	{
		switch(m_eventQueue[i].event)
		{
		case WG_BUTTON_PRESS: button_press_(m_eventQueue[i]); break;
		case WG_BUTTON_RELEASE: button_release_(m_eventQueue[i]); break;
		case WG_KEY_PRESS: key_press_(m_eventQueue[i]); break;
		case WG_KEY_RELEASE: key_release_(m_eventQueue[i]); break;
		case WG_CHAR: character_(m_eventQueue[i]); break;
		case WG_WHEEL_ROLL: wheel_roll_(m_eventQueue[i]); break;
		}
	}
	
	m_eventQueue.clear();
	m_eventQueue.reserve(16);
}

//____ end_events() ___________________________________________________________

void WgInput::end_events()
{
	flush_event_queue();

	// Generate new position data, wait with the widget stack...
	m_currentPosition.x 			= m_pointerX;
	m_currentPosition.y 			= m_pointerY;
	m_currentPosition.timestamp	= m_time;
	m_currentPosition.modifier	= m_modifierKeys;
	m_currentPosition.nWidgets 	= 0;


	// Send button_down to those we have an active press inside. We want to do this
	// before we set up the widget stack since these typically might move widgets
	// and we don't want sporadic enter/exit actions.

	for( int btn = 0 ; btn < WG_MAX_BUTTONS ; btn++ )
	{
		if( m_bButtonDown[btn] == true )
		{
			WgActionDetails * p = &m_pressed[btn];

			for( Uint32 wdg = 0 ; wdg < p->nWidgets ; wdg++ )
			{
				if( p->aWidgets[wdg] )
					p->aWidgets[wdg]->ActionRespond( BUTTON_DOWN, btn+1, m_currentPosition, *this );
				else if( p->aGizmos[wdg] )
					p->aGizmos[wdg]->_onAction( BUTTON_DOWN, btn+1, m_currentPosition, *this );
			}
			// reset hover time if a button is down
			m_hoverStartTime = m_time;
		}
	}

	// Generate the widget stack now when we believe that widget positions are up-to-date

	bool cp = false;
	WgWidget* pOldTop = 0;
start:
	WgWidget * pBlockingModal = 0;
	WgWidget * pWidget = m_pRootWidget->FindOccupant( m_currentPosition.x, m_currentPosition.y, true, 0, &pBlockingModal );
	WgWidget * topMark = pWidget;

	// Viktor change:
	// a widget should be sent to the pointerspy even if it's disabled
	// (we want tooltips for disabled widgets...).
	// so we need to store a separate pointer to the widget that's considered
	// the top marked if we disregard enabled/disabled state.
	WgWidget* pTopMarkSpy = pWidget;

	while( pWidget )
	{
		// Is this widget blocking this action source for its children?

		if( pWidget->m_actionFilterBlock & WgWidget::POINTER )
		{
			m_currentPosition.nWidgets = 0;
			pTopMarkSpy = 0;
		}

		// Is this widget copying this action source from its children?

		if( pWidget == topMark || (pWidget->m_actionFilterCopy & WgWidget::POINTER) )
		{
			if( pWidget->IsEnabled() )
				m_currentPosition.aWidgets[m_currentPosition.nWidgets++] = pWidget;

			if( pTopMarkSpy == 0 )
				pTopMarkSpy = pWidget;
		}
		pWidget = pWidget->m_pParent;
	}

	// Possibly send button repeats.

	for( int btn = 0 ; btn < WG_MAX_BUTTONS ; btn++ )
	{
		if( m_bButtonDown[btn] == true )
		{
			for( Uint32 wdg = 0 ; wdg < m_pressed[btn].nWidgets ; wdg++ )
			{
				WgWidget * pWdg = m_pressed[btn].aWidgets[wdg];

				if( widgetInStack( pWdg, m_currentPosition ) )
				{
					if( m_repeatTime[btn] == 0 )
					{
						if( m_time - m_pressed[btn].timestamp > m_buttonRepeatDelay )
						{
							m_repeatTime[btn] = m_pressed[btn].timestamp + m_buttonRepeatDelay;
							pWdg->ActionRespond( BUTTON_REPEAT, btn+1, m_currentPosition, *this );
						}
					}

					if( m_repeatTime[btn] != 0 )
					{
						while( m_time - m_repeatTime[btn] > m_buttonRepeatRate )
						{
							m_repeatTime[btn] += m_buttonRepeatRate;
							pWdg->ActionRespond( BUTTON_REPEAT, btn+1, m_currentPosition, *this );
						}
					}
				}
			}
		}
	}

	// Send any pointer exit actions

	for( Uint32 wdg = 0 ; wdg < m_position.nWidgets ; wdg++ )
	{
		if( !widgetInStack( m_position.aWidgets[wdg], m_currentPosition ) )
		{
			m_position.aWidgets[wdg]->ActionRespond( POINTER_EXIT, 0, m_currentPosition, *this );
		}
	}

	// Send any pointer enter actions to new ones and position to everyone in our stack.

	if( pBlockingModal )
	{
		pBlockingModal->ActionRespond( POINTER_OUTSIDE_MODAL, 0, m_currentPosition, *this );
	}
	else
	{
		for( Uint32 wdg = 0 ; wdg < m_currentPosition.nWidgets ; wdg++ )
		{
			WgWidget * pWdg = m_currentPosition.aWidgets[wdg];

			if( !widgetInStack( pWdg, m_position ) )
			{
				pWdg->ActionRespond( POINTER_ENTER, 0, m_currentPosition, *this );
			}
			pWdg->ActionRespond( POINTER_OVER, 0, m_currentPosition, *this );
		}
	}


	// Handle KEY_DOWN and KEY_REPEAT actions.

	if( m_keys.nKeysDown != 0 )
	{
		WgActionDetails	myKeyAction;

		myKeyAction.timestamp	= m_time;
		myKeyAction.modifier	= m_modifierKeys;
		myKeyAction.nWidgets	= 0;

		// Update widget stack

	  WgWidget * pWidget = m_pFocusedWidget;

	  while( pWidget )
	  {
	    // Is this widget blocking this action source for its children?

	    if( pWidget->m_actionFilterBlock & WgWidget::KEYBOARD )
	      myKeyAction.nWidgets = 0;

	    // Is this widget copying this action source from its children?

	    if( pWidget == m_pFocusedWidget || (pWidget->m_actionFilterCopy & WgWidget::KEYBOARD) )
		{
			if( pWidget->IsEnabled() )
				myKeyAction.aWidgets[myKeyAction.nWidgets++] = pWidget;
		}
	    pWidget = pWidget->m_pParent;
	  }

		// Send any KEY_DOWN and KEY_REPEAT actions to our widgets

		for( Uint32 key = 0 ; key < m_keys.nKeysDown ; key++ )
		{
			myKeyAction.keycode 		= m_keys.aKeysDown[key].keycode;
			myKeyAction.native_keycode 	= m_keys.aKeysDown[key].native_keycode;
			myKeyAction.character		= m_keys.aKeysDown[key].character;

			int keycode = myKeyAction.keycode;
			if( keycode == WG_KEY_UNMAPPED )
				keycode = myKeyAction.native_keycode;

			// Send KEY_DOWN

			for( Uint32 wdg = 0 ; wdg < myKeyAction.nWidgets ; wdg++ )
			{
				myKeyAction.aWidgets[wdg]->ActionRespond( KEY_DOWN, keycode, myKeyAction, *this );

				// Possibly send KEY_REPEAT and CHARACTER

				if( m_keyRepeatTime[key] == 0 )
				{
					if( m_time - m_keys.aKeysDown[key].timestamp > m_keyRepeatDelay )
					{
						m_keyRepeatTime[key] = m_keys.aKeysDown[key].timestamp + m_keyRepeatDelay;
						myKeyAction.aWidgets[wdg]->ActionRespond( KEY_REPEAT, keycode, myKeyAction, *this );
						if( myKeyAction.character != 0 )
							myKeyAction.aWidgets[wdg]->ActionRespond( CHARACTER, myKeyAction.character, myKeyAction, *this );
					}
				}

				if( m_keyRepeatTime[key] != 0 )
				{
					while( m_time - m_keyRepeatTime[key] > m_keyRepeatRate )
					{
						m_keyRepeatTime[key] += m_keyRepeatRate;
						myKeyAction.aWidgets[wdg]->ActionRespond( KEY_REPEAT, keycode, myKeyAction, *this );
						if( myKeyAction.character != 0 )
							myKeyAction.aWidgets[wdg]->ActionRespond( CHARACTER, myKeyAction.character, myKeyAction, *this );
					}
				}
			}
		}
	}

	// HACK : tord - M.I.G
	if( topMark )
	{
		if( cp )
		{
			pOldTop->m_pointerMask = WgWidget::POINTER_SOFT_OPAQUE;
		}
		else if( WgWidget::POINTER_SOFT_OPAQUE == topMark->m_pointerMask )
		{
			topMark->m_pointerMask = WgWidget::POINTER_TRANSPARENT;
			pOldTop = topMark;
			cp = true;
			goto start;		// fint
		}
	}

	// if the topmark has changed, reset hover time
	{
		WgWidget* pOldTopWidget = 0;
		if( m_position.nWidgets > 0 )
            pOldTopWidget = m_position.aWidgets[0];

		WgWidget* pNewTopWidget = 0;
		if( m_currentPosition.nWidgets > 0 )
            pNewTopWidget = m_currentPosition.aWidgets[0];

		if( pOldTopWidget != pNewTopWidget )
			m_hoverStartTime = m_time;
	}

	// Update member structures.

	m_position = m_currentPosition;
	m_currentPosition.nWidgets 	= 0;

	// Update PointerSpy

	if( m_pPointerSpies )
	{
		// use the top
		/*WgWidget * pWdg = 0;
		if( m_position.nWidgets > 0 )
			pWdg = m_position.aWidgets[0];*/

		WgWidget* pPressed = 0;
		if( m_bButtonDown[ 0 ] == true )
		{
			const WgActionDetails * p = &m_pressed[0];
			if( p->nWidgets > 0 )
				pPressed = p->aWidgets[0];
		}
		
		for( Uint32 i = 0; i < m_nPointerSpies; ++i )
			m_pPointerSpies[ i ]->PointerPosition( m_position.x, m_position.y, pTopMarkSpy, m_time - m_hoverStartTime, pPressed );
	}

}



//____ pointer_setPos() _______________________________________________________

void WgInput::pointer_setPos( Sint32 x, Sint32 y )
{
	m_pointerX = x;
	m_pointerY = y;
}

//____ pointer_move() _________________________________________________________

void WgInput::pointer_move( Sint32 x, Sint32 y )
{
	m_pointerX += x;
	m_pointerY += y;
}

//____ button_press() _________________________________________________________

void WgInput::button_press_( WgInputEventData ed )
{
	Uint8 button = ed.data0;

	if( button < 1 || button > WG_MAX_BUTTONS )
		return;

	WgActionDetails	myAction;

	myAction.x 			= ed.pointerX;
	myAction.y 			= ed.pointerY;
	myAction.modifier	= ed.modifier;
	myAction.timestamp	= ed.timestamp;
	myAction.nWidgets 	= 0;

	// Update the widget stack


	WgWidget * pBlockingModal = 0;
	WgWidget * pWidget = m_pRootWidget->FindOccupant( myAction.x, myAction.y, true, 0, &pBlockingModal );
	WgWidget * topMark = pWidget;
	Sint32	blockFilter = (1 << button) + WgWidget::POINTER;

	if( pBlockingModal )
	{
		pBlockingModal->ActionRespond( BUTTON_PRESS_OUTSIDE_MODAL, button, myAction, *this );
		return;
	}


	while( pWidget )
	{
		// Is this widget blocking this action source for its children?

		if( pWidget->m_actionFilterBlock & blockFilter )	// Blocking any of POINTER or our BUTTON blocks the action.
			myAction.nWidgets = 0;

		// Is this widget copying this action source from its children?

		if( pWidget == topMark || ((pWidget->m_actionFilterCopy & blockFilter) == blockFilter) ) // Must copy both POINTER and BUTTON actions to get a copy.
		{
			if( pWidget->IsEnabled() )										// Only enabled widgets gets mouseclicks.
				myAction.aWidgets[myAction.nWidgets++] = pWidget;
		}
		pWidget = pWidget->m_pParent;
	}

	// Step through our widgets and process them one by one

	for( Uint32 i = 0 ; i < myAction.nWidgets ; i++ )
	{
		WgWidget * pWidget = myAction.aWidgets[i];

		// Possibly send action mouse_enter and add it to m_position if it isn't
		// there already since we always wants mouse_enter to proceed button_press.

		if( !widgetInStack( pWidget, m_position ) )
		{
			pWidget->ActionRespond( POINTER_ENTER, button, myAction, *this );
			m_position.aWidgets[m_position.nWidgets++] = pWidget;
		}

		// Send action "pressed"

		pWidget->ActionRespond( BUTTON_PRESS, button, myAction, *this );

		// possibly send double-click if widget was involved in both last press
		// and last release and time passed has been within treshold

		if( widgetInStack( pWidget, m_pressed[button-1] ) &&
				widgetInStack( pWidget, m_released[button-1] ) )
		{
			// Send double-click if everything was within treshold

			if( myAction.timestamp - m_pressed[button-1].timestamp <= m_doubleClickTreshold )
			{
				pWidget->ActionRespond( BUTTON_DOUBLECLICK, button, myAction, *this );
			}
		}

	}




	// Update member structures

	button--;

	m_bButtonDown[button]	= true;
	m_repeatTime[button] 	= 0;
	m_pressed[button] 		= myAction;
}


//____ button_release() _______________________________________________________

void WgInput::button_release_( WgInputEventData ed )
{
	Uint8 button = ed.data0;

	if( button < 1 || button > WG_MAX_BUTTONS )
		return;

	if( !m_bButtonDown[button-1] )
		return;

	WgActionDetails	myAction;

	myAction.x 			= ed.pointerX;
	myAction.y 			= ed.pointerY;
	myAction.timestamp	= ed.timestamp;
	myAction.modifier	= ed.modifier;
	myAction.nWidgets 	= 0;

	// Update the widget stack

	WgWidget * pBlockingModal = 0;
	WgWidget * pWidget = m_pRootWidget->FindOccupant( myAction.x, myAction.y, true, 0, &pBlockingModal );
	WgWidget * topMark = pWidget;
	Sint32	blockFilter = (1 << button) + WgWidget::POINTER;


	if( pBlockingModal )
	{
		pBlockingModal->ActionRespond( BUTTON_RELEASE_OUTSIDE_MODAL, button, myAction, *this );
	}
	else
	{
		while( pWidget )
		{
			// Is this widget blocking this action source for its children?

			if( pWidget->m_actionFilterBlock & blockFilter )	// Blocking any of POINTER or our BUTTON blocks the action.
				myAction.nWidgets = 0;

			// Is this widget copying this action source from its children?

			if( pWidget == topMark || ((pWidget->m_actionFilterCopy & blockFilter) == blockFilter) ) // Must copy both POINTER and BUTTON actions to get a copy.
			{
				if( pWidget->IsEnabled() )
					myAction.aWidgets[myAction.nWidgets++] = pWidget;
			}
			pWidget = pWidget->m_pParent;
		}


		// Step through our widgets and process them one by one

		for( Uint32 i = 0 ; i < myAction.nWidgets ; i++ )
		{
			WgWidget * pWidget = myAction.aWidgets[i];

			// Possibly send action mouse_enter and add it to m_position if it isn't
			// there already since we always wants mouse_enter to proceed button_release.

			if( !widgetInStack( pWidget, m_position ) )
			{
				pWidget->ActionRespond( POINTER_ENTER, button, myAction, *this );
				m_position.aWidgets[m_position.nWidgets++] = pWidget;
			}

			// Send action "release"

			pWidget->ActionRespond( BUTTON_RELEASE, button, myAction, *this );

			// possibly send click if button was pressed upon this widget

			if( widgetInStack( pWidget, m_pressed[button-1] ) )
			{
				pWidget->ActionRespond( BUTTON_CLICK, button, myAction, *this );
			}

		}
	}


	// Send BUTTON_RELEASE_OUTSIDE to those who first accepted the press, but aren't in focus anymore...

	for( unsigned int i = 0 ; i < m_pressed[button-1].nWidgets ; i++ )
	{
		WgWidget * pWidget = m_pressed[button-1].aWidgets[i];

		if( !widgetInStack( pWidget, myAction ) )
			pWidget->ActionRespond( BUTTON_RELEASE_OUTSIDE, button, myAction, *this );
	}


	// Update member structures

	button--;

	m_bButtonDown[button]	= false;
	m_released[button] 		= myAction;
}

//____ wheel_roll() ___________________________________________________________

void WgInput::wheel_roll_( WgInputEventData ed )
{
	Uint8 wheel = ed.data0;

	if( wheel < 1 || wheel > 3)
		return;


	WgActionDetails	myAction;

	myAction.rolldistance = ed.data1;
	myAction.modifier	= ed.modifier;
	myAction.timestamp	= ed.timestamp;
	myAction.nWidgets 	= 0;

	// Update the widget stack


	WgWidget * pBlockingModal = 0;
	WgWidget * pWidget = m_pRootWidget->FindOccupant( ed.pointerX, ed.pointerY, true, 0, &pBlockingModal );
	WgWidget * topMark = pWidget;
	Sint32	blockFilter = (1 << (wheel-1)) + WgWidget::WHEEL1;

	if( pBlockingModal )
	{
		pBlockingModal->ActionRespond( WHEEL_ROLL_OUTSIDE_MODAL, wheel, myAction, *this );
		return;
	}

	while( pWidget )
	{
		// Is this widget blocking this action source for its children?

		if( pWidget->m_actionFilterBlock & blockFilter )	// Blocking any of POINTER or our BUTTON blocks the action.
			myAction.nWidgets = 0;

		// Is this widget copying this action source from its children?

		if( pWidget == topMark || ((pWidget->m_actionFilterCopy & blockFilter) == blockFilter) ) // Must copy both POINTER and BUTTON actions to get a copy.
		{
			if( pWidget->IsEnabled() )
				myAction.aWidgets[myAction.nWidgets++] = pWidget;
		}
		pWidget = pWidget->m_pParent;
	}

	// Step through our widgets and process them one by one

	for( Uint32 i = 0 ; i < myAction.nWidgets ; i++ )
	{
		WgWidget * pWidget = myAction.aWidgets[i];

		// Possibly send action mouse_enter and add it to m_position if it isn't
		// there already since we always wants mouse_enter to proceed wheel_roll.

		if( !widgetInStack( pWidget, m_position ) )
		{
			pWidget->ActionRespond( POINTER_ENTER, 0, myAction, *this );
			m_position.aWidgets[m_position.nWidgets++] = pWidget;
		}

		// Send action "wheel_roll"

		pWidget->ActionRespond( WHEEL_ROLL, wheel, myAction, *this );
	}
}

//____ character() ____________________________________________________________

void WgInput::character_( WgInputEventData ed )
{
	// attach this character to the ActionDetails of keycode_for_repeat if valid.

	Uint16 character = ed.data0;
	Uint16 native_keycode_for_repeat = ed.data1;

	if( native_keycode_for_repeat != 0 )
	{
		for( unsigned int i = 0 ; i < m_keys.nKeysDown ; i++ )
		{
			if( m_keys.aKeysDown[i].native_keycode == native_keycode_for_repeat )
			{
				m_keys.aKeysDown[i].character = character;
				break;
			}
		}
	}

	// Call Action-respond on all affected widgets.

	WgActionDetails		myAction;

	myAction.timestamp		= ed.timestamp;
	myAction.modifier		= ed.modifier;
	myAction.native_keycode	= native_keycode_for_repeat;
	myAction.keycode		= translate_keycode( native_keycode_for_repeat );
	myAction.character		= character;
	myAction.nWidgets		= 0;

	// Update the widget stack

 	WgWidget * pWidget = m_pFocusedWidget;

	while( pWidget )
	{
    	// Is this widget blocking this action source for its children?

		if( pWidget->m_actionFilterBlock & WgWidget::KEYBOARD )
			myAction.nWidgets = 0;

		// Is this widget copying this action source from its children?

		if( pWidget == m_pFocusedWidget || (pWidget->m_actionFilterCopy & WgWidget::KEYBOARD) )
		{
			if( pWidget->IsEnabled() )
				myAction.aWidgets[myAction.nWidgets++] = pWidget;
		}
		pWidget = pWidget->m_pParent;
	}

	// Step through our widgets and process them one by one

	for( Uint32 i = 0 ; i < myAction.nWidgets ; i++ )
	{
		myAction.aWidgets[i]->ActionRespond( CHARACTER, character, myAction, *this );
	}
}


//____ key_press() ____________________________________________________________

void WgInput::key_press_( WgInputEventData ed )
{
	Uint16 native_keycode = ed.data0;
	Uint16 character = ed.data1;

	// Check if this key already is pressed and in that case ignore.

	for( unsigned int i = 0 ; i < m_keys.nKeysDown ; i++ )
	{
		if( m_keys.aKeysDown[i].native_keycode == native_keycode )
			return;
	}


	//

	WgKey keycode = translate_keycode( native_keycode );


	WgActionDetails		myAction;

	myAction.timestamp	= ed.timestamp;
	myAction.modifier	= ed.modifier;
	myAction.native_keycode	= native_keycode;
	myAction.keycode	= keycode;
	myAction.character	= character;
	myAction.nWidgets	= 0;

	// Update the widget stack

 	WgWidget * pWidget = m_pFocusedWidget;

	while( pWidget )
	{
    	// Is this widget blocking this action source for its children?

		if( pWidget->m_actionFilterBlock & WgWidget::KEYBOARD )
			myAction.nWidgets = 0;

		// Is this widget copying this action source from its children?

		if( pWidget == m_pFocusedWidget || (pWidget->m_actionFilterCopy & WgWidget::KEYBOARD) )
		{
			if( pWidget->IsEnabled() )
				myAction.aWidgets[myAction.nWidgets++] = pWidget;
		}
		pWidget = pWidget->m_pParent;
	}

	// Step through our widgets and process them one by one

	for( Uint32 i = 0 ; i < myAction.nWidgets ; i++ )
	{
		if(keycode == WG_KEY_UNMAPPED)
			myAction.aWidgets[i]->ActionRespond( KEY_PRESS, native_keycode, myAction, *this );
		else
			myAction.aWidgets[i]->ActionRespond( KEY_PRESS, keycode, myAction, *this );

		if( character != 0 )
			myAction.aWidgets[i]->ActionRespond( CHARACTER, character, myAction, *this );
	}

	// Update member structures

	m_keys.aKeysDown[m_keys.nKeysDown].timestamp 		= ed.timestamp;
	m_keys.aKeysDown[m_keys.nKeysDown].modifier			= ed.modifier;
	m_keys.aKeysDown[m_keys.nKeysDown].keycode 			= keycode;
	m_keys.aKeysDown[m_keys.nKeysDown].native_keycode	= native_keycode;
	m_keys.aKeysDown[m_keys.nKeysDown].character		= character;

	m_keyRepeatTime[m_keys.nKeysDown] = 0;
	m_keys.nKeysDown++;
}

//____ key_release() __________________________________________________________

void WgInput::key_release_( WgInputEventData ed )
{
	Uint16 native_keycode = ed.data0;

	// Find which entry in the stack this corresponds to.

	Uint32 key;
	for( key = 0 ; key < m_keys.nKeysDown ; key++ )
		if( m_keys.aKeysDown[key].native_keycode == native_keycode )
			break;

	if( key == m_keys.nKeysDown )
		return;

	WgKey keycode = translate_keycode( native_keycode );

	// Set up a local-only WgActionDetails for our ActionRespond()-calls. Don't save it.

	WgActionDetails		myAction;

	myAction.timestamp		= ed.timestamp;
	myAction.modifier		= ed.modifier;
	myAction.keycode		= keycode;
	myAction.native_keycode = native_keycode;
	myAction.character		= m_keys.aKeysDown[key].character;
	myAction.nWidgets		= 0;

	// Update widget stack

  WgWidget * pWidget = m_pFocusedWidget;

  while( pWidget )
  {
    // Is this widget blocking this action source for its children?

    if( pWidget->m_actionFilterBlock & WgWidget::KEYBOARD )
      myAction.nWidgets = 0;

    // Is this widget copying this action source from its children?

    if( pWidget == m_pFocusedWidget || (pWidget->m_actionFilterCopy & WgWidget::KEYBOARD) )
	{
		if( pWidget->IsEnabled() )
			myAction.aWidgets[myAction.nWidgets++] = pWidget;
	}
    pWidget = pWidget->m_pParent;
  }


	// Step through our widgets and process them one by one

	for( Uint32 i = 0 ; i < myAction.nWidgets ; i++ )
	{
		if(keycode == WG_KEY_UNMAPPED)
			myAction.aWidgets[i]->ActionRespond( KEY_RELEASE, native_keycode, myAction, *this );
		else
			myAction.aWidgets[i]->ActionRespond( KEY_RELEASE, keycode, myAction, *this );
	}

	// Update member structures, that is: get rid of our pressed

	m_keys.nKeysDown--;

	if( key != m_keys.nKeysDown )
	{
		m_keys.aKeysDown[key] = m_keys.aKeysDown[m_keys.nKeysDown];
		m_keyRepeatTime[key] = m_keyRepeatTime[m_keys.nKeysDown];
	}
}

void WgInput::button_press( Uint8 button )
{
	WgInputEventData ed;
	ed.event = WG_BUTTON_PRESS;
	ed.timestamp = m_time;
	ed.pointerX = m_pointerX;
	ed.pointerY = m_pointerY;
	ed.modifier = m_modifierKeys;
	ed.data0 = button;
	ed.data1 = 0;
	m_eventQueue.push_back(ed);
}

void WgInput::button_release( Uint8 button )
{
	WgInputEventData ed;
	ed.event = WG_BUTTON_RELEASE;
	ed.timestamp = m_time;
	ed.pointerX = m_pointerX;
	ed.pointerY = m_pointerY;
	ed.modifier = m_modifierKeys;
	ed.data0 = button;
	ed.data1 = 0;
	m_eventQueue.push_back(ed);
}

void WgInput::key_press( Uint16 native_keycode, Uint16 character )
{
	WgInputEventData ed;
	ed.event = WG_KEY_PRESS;
	ed.timestamp = m_time;
	ed.pointerX = m_pointerX;
	ed.pointerY = m_pointerY;
	ed.modifier = m_modifierKeys;
	ed.data0 = native_keycode;
	ed.data1 = character;
	m_eventQueue.push_back(ed);
}

void WgInput::key_release( Uint16 native_keycode )
{
	WgInputEventData ed;
	ed.event = WG_KEY_RELEASE;
	ed.timestamp = m_time;
	ed.pointerX = m_pointerX;
	ed.pointerY = m_pointerY;
	ed.modifier = m_modifierKeys;
	ed.data0 = native_keycode;
	ed.data1 = 0;
	m_eventQueue.push_back(ed);
}

void WgInput::character( Uint16 character, Uint16 native_keycode_for_repeat )
{
	WgInputEventData ed;
	ed.event = WG_CHAR;
	ed.timestamp = m_time;
	ed.pointerX = m_pointerX;
	ed.pointerY = m_pointerY;
	ed.modifier = m_modifierKeys;
	ed.data0 = character;
	ed.data1 = native_keycode_for_repeat;
	m_eventQueue.push_back(ed);
}

void WgInput::wheel_roll( Uint8 wheel, Sint32 distance )
{
	WgInputEventData ed;
	ed.event = WG_WHEEL_ROLL;
	ed.timestamp = m_time;
	ed.pointerX = m_pointerX;
	ed.pointerY = m_pointerY;
	ed.modifier = m_modifierKeys;
	ed.data0 = wheel;
	ed.data1 = distance;
	m_eventQueue.push_back(ed);
}

//____ key_release_all() ______________________________________________________

void WgInput::key_release_all()
{
	for(size_t iKey = 0; iKey < m_keys.nKeysDown; iKey++ )
		key_release( m_keys.aKeysDown[iKey].native_keycode );
	m_keys.nKeysDown = 0;
}

//____ button_release_all() ______________________________________________________

void WgInput::button_release_all()
{
	for( int i = 0 ; i < WG_MAX_BUTTONS ; i++ )
		button_release( i );
}


//____ isKeyDown() ____________________________________________________________

bool WgInput::isKeyDown( Uint16 native_keycode ) const
{
	for( Uint32 i = 0 ; i < m_keys.nKeysDown ; i++ )
		if( m_keys.aKeysDown[i].native_keycode == native_keycode )
			return true;

	return false;
}

bool WgInput::isKeyDown( WgKey wg_keycode ) const
{
	Uint16 nativeKey = m_keycodeMap[wg_keycode];
	for( Uint32 i = 0 ; i < m_keys.nKeysDown ; i++ )
		if( m_keys.aKeysDown[i].native_keycode == nativeKey )
			return true;

	return false;
}

//____ isButtonDown() _________________________________________________________

bool	WgInput::isButtonDown( Uint8 button ) const
{
	if( button == 0 || button > WG_MAX_BUTTONS )
		return false;

	return m_bButtonDown[button-1];
}

//____ getPressDetails() ______________________________________________________

const WgActionDetails * WgInput::getPressDetails( Uint8 button ) const
{
	if( button == 0 || button > WG_MAX_BUTTONS )
		return 0;

	return &m_pressed[button-1];
}

//____ getReleaseDetails() ____________________________________________________

const WgActionDetails * WgInput::getReleaseDetails( Uint8 button ) const
{
	if( button == 0 || button > WG_MAX_BUTTONS )
		return 0;

	return &m_released[button-1];
}

//____ getMouseOverDetails() __________________________________________________

const WgActionDetails * WgInput::getMouseOverDetails( void ) const
{
	return &m_position;
}

//____ getKeyDownDetails() ____________________________________________________

const WgKeyDownStack * WgInput::getKeyDownDetails( void ) const
{
	return &m_keys;
}


//____ wasButtonPressInside() _________________________________________________

bool WgInput::wasButtonPressInside( Uint8 button, WgWidget * pWidget ) const
{
	if( button == 0 || button > WG_MAX_BUTTONS )
		return false;

	return widgetInStack( pWidget, m_pressed[button-1] );
}

//____ wasButtonReleaseInside() _______________________________________________

bool WgInput::wasButtonReleaseInside( Uint8 button, WgWidget * pWidget ) const
{
	if( button == 0 || button > WG_MAX_BUTTONS )
		return false;

	return widgetInStack( pWidget, m_released[button-1] );
}

//____ isPointerInside() ______________________________________________________

bool WgInput::isPointerInside( WgWidget * pWidget ) const
{
	return widgetInStack( pWidget, m_position );
}


//____ setFocusedWidget() _____________________________________________________

void	WgInput::setFocusedWidget( WgWidget * pWidget )
{
	if( m_pFocusedWidget )
	{
		WgWidget* pTmp = m_pFocusedWidget;
		m_pFocusedWidget = 0;
		pTmp->LostInputFocus();
	}

	m_pFocusedWidget = pWidget;

	if( m_pFocusedWidget )
		m_pFocusedWidget->GotInputFocus();

}


//____ setRootWidget() ________________________________________________________

void WgInput::setRootWidget( Wdg_Root * pRootWidget )
{
	m_pRootWidget		= pRootWidget;
	m_pFocusedWidget	= pRootWidget;
}

//____ setButtonRepeat() ______________________________________________________

bool WgInput::setButtonRepeat( Uint32 delay, Uint32 rate )
{
	m_buttonRepeatDelay	= delay;
	m_buttonRepeatRate	= rate;
	return true;
}

//____ setKeyRepeat() _________________________________________________________

bool WgInput::setKeyRepeat( Uint32 delay, Uint32 rate )
{
	m_keyRepeatDelay	= delay;
	m_keyRepeatRate		= rate;
	return true;
}

//____ setDoubleClick() _______________________________________________________

bool WgInput::setDoubleClick( Uint32 treshold )
{
	m_doubleClickTreshold = treshold;
	return true;
}


//____ widgetInStack() ________________________________________________________

bool WgInput::widgetInStack( const WgWidget * pWidget, const WgActionDetails& info ) const
{
	for( Uint32 i = 0 ; i < info.nWidgets ; i++ )
	{
		if( info.aWidgets[i] == pWidget )
			return true;
	}
	return false;
}


//____ widgetRemoved() ________________________________________________________
/**
	Informs this Input-object that the specified Widget has been removed and
	should not receive any signals anymore. Last signals are sent out from here...
*/

#include <string>
//#include <Misc/SendAssert.hpp>

void WgInput::widgetRemoved( WgWidget * pWidget )
{
	if( m_currentPosition.removeWidget(pWidget) )
	{
		// This is baad. It means a widget was removed from the end_events() function during a callback
//		SendAssertStackDump("widget removed in end_events");
	}

	if( m_position.removeWidget(pWidget) )
	{
		WgActionDetails		myAction;

		myAction.x 			= m_pointerX;
		myAction.y 			= m_pointerY;
		myAction.timestamp	= m_time;
		myAction.modifier	= m_modifierKeys;
		myAction.nWidgets 	= 0;

		pWidget->ActionRespond( POINTER_EXIT, 0, myAction, *this );
	}

	for( int i = 0 ; i < WG_MAX_BUTTONS ; i++ )
	{
		if( m_pressed[i].removeWidget(pWidget) )
		{
			WgActionDetails		myAction;

			myAction.x 			= m_pointerX;
			myAction.y 			= m_pointerY;
			myAction.timestamp	= m_time;
			myAction.modifier	= m_modifierKeys;
			myAction.nWidgets 	= 0;

			pWidget->ActionRespond( BUTTON_RELEASE_OUTSIDE, i+1, myAction, *this );
		}
		m_released[i].removeWidget(pWidget);
	}
}


//____ WgActionDetails::removeWidget() ________________________________________

bool WgActionDetails::removeWidget( WgWidget * pWidget )
{
	for( Uint32 i = 0 ; i < nWidgets ; i++ )
	{
		if( aWidgets[i] == pWidget )
		{
			while( i < nWidgets-1 )
			{
				aWidgets[i] = aWidgets[i+1];
				i++;
			}
			aWidgets[i] = 0;
			nWidgets--;
			return true;
		}
	}
	return false;
}
