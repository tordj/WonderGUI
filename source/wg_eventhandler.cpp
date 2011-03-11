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

#include <wg_event.h>
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
}

//____ Destructor _____________________________________________________________

WgEventHandler::~WgEventHandler()
{
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

		m_eventQueue.insert( m_insertPos++, _event );
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
	m_insertPos = m_eventQueue.begin();

	while( !m_eventQueue.empty() )
	{
		WgEvent::Event ev = m_eventQueue.front();
		m_eventQueue.pop_front();

		CompleteEvent( ev );
		ProcessEventInternal( ev );
//		ProcessEventCallbacks( ev );
	}

	m_bIsProcessing = false;
}


//____ CompleteEvent() ________________________________________________________

void WgEventHandler::CompleteEvent( WgEvent::Event& _event )
{
	// Fill in missing information in the event-class.

	_event.m_timestamp	= m_time;
	_event.m_modKeys	= m_modKeys;

	switch( _event.m_id )
	{

		case WG_EVENT_BUTTON_PRESS:
		case WG_EVENT_BUTTON_RELEASE:
		case WG_EVENT_BUTTON_CLICK:
		case WG_EVENT_BUTTON_DOUBLECLICK:
			_event.m_param[1].integer = m_pointerPos.x;
			_event.m_param[2].integer = m_pointerPos.y;
			break;

		case WG_EVENT_KEY_PRESS:
		case WG_EVENT_KEY_RELEASE:
		case WG_EVENT_WHEEL_ROLL:
			_event.m_param[2].integer = m_pointerPos.x;
			_event.m_param[3].integer = m_pointerPos.y;
			break;

		case WG_EVENT_POINTER_MOVE:
		case WG_EVENT_CHARACTER:
		case WG_EVENT_TIME_PASS:
			break;


		default:
			break;
	}

}


//____ ProcessEventInternal() _________________________________________________

void WgEventHandler::ProcessEventInternal( WgEvent::Event& _event )
{

	switch( _event.m_id )
	{
		case WG_EVENT_POINTER_MOVE:
			ProcessPointerMove( (WgEvent::PointerMove*) &_event );
			break;
		case WG_EVENT_BUTTON_PRESS:
			ProcessButtonPress( (WgEvent::ButtonPress*) &_event );
			break;
		case WG_EVENT_BUTTON_RELEASE:
			ProcessButtonRelease( (WgEvent::ButtonRelease*) &_event );
			break;
		case WG_EVENT_KEY_PRESS:
		case WG_EVENT_KEY_RELEAS:
		case WG_EVENT_CHARACTER:
		case WG_EVENT_WHEEL_ROLL:
			break;

		case WG_EVENT_TIME_PASS:
			ProcessTimePass( (WgEvent::TimePass*) &_event );
			break;

		case WG_EVENT_END_POINTER_MOVE:
			ProcessEndPointerMove( (WgEvent::EndPointerMove*) &_event );
			break;
		case WG_EVENT_BUTTON_DRAG:
			ProcessButtonDrag( (WgEvent::ButtonDrag*) &_event );
			break;

		case WG_EVENT_DUMMY:
			break;

		case WG_EVENT_POINTER_ENTER_GIZMO:
			break;

		case WG_EVENT_POINTER_EXIT_GIZMO:
			break;

	}

}

//____ ProcessTimePass() ______________________________________________________

void WgEventHandler::ProcessTimePass( WgEvent::TimePass * pEvent )
{
			m_time += pEvent->Millisec();
}


//____ ProcessPointerMove() ___________________________________________________

void WgEventHandler::ProcessPointerMove( WgEvent::PointerMove * pEvent )
{
	// Post events for button drag

	for( int i = 0 ; i < WG_MAX_BUTTONS ; i++ )
	{
		if( m_latestButtonEvents[i].Id() == WG_EVENT_BUTTON_PRESS )
		{
			WgEvent::ButtonPress * p = static_cast<WgEvent::ButtonPress*> (&m_latestButtonEvents[i]);

			QueueEvent( WgEvent::ButtonDrag( i, p->PointerPos(), m_pointerPos, pEvent->Pos() ) );
		}
	}

	// Post event for finalizing move once button drag is taken care of.

	QueueEvent( WgEvent::EndPointerMove( pEvent->Pos() ) );

	// Update pointer position

	m_pointerPos = pEvent->Pos();
}

//____ ProcessEndPointerMove() _______________________________________________

void WgEventHandler::ProcessEndPointerMove( WgEvent::EndPointerMove * pEvent )
{
	std::vector<WgGizmo*>	vNowInside;

	// Collect widgets we now are inside

	WgGizmo * pGizmo = m_pRoot->FindGizmo( m_pointerPos, WG_SEARCH_ACTION_TARGET );

	while( pGizmo )
	{
		vNowInside.push_back(pGizmo);
		pGizmo = pGizmo->Hook()->Parent()->CastToGizmo();
	}

	// Post PointerLeftGizmo events for those we have left

	for( size_t i = 0 ; i < m_vMarkedWidgets.size() ; i++ )
	{
		pGizmo = m_vMarkedWidgets[i].GetRealPtr();

		size_t j = 0;
		while( j < vNowInside.size() )
		{
			if( pGizmo == vNowInside[j] )
				break;
			j++;
		}

		if( j == vNowInside.size() )
			QueueEvent( WgEvent::PointerExitGizmo( pGizmo ) );
	}

	// Post PointerEnteredGizmo events for the new ones

	for( size_t i = 0 ; i < vNowInside.size() ; i++ )
	{
		pGizmo = vNowInside[i];

		size_t j = 0;
		while( j < m_vMarkedWidgets.size() )
		{
			if( pGizmo == m_vMarkedWidgets[j].GetRealPtr() )
				break;
			j++;
		}

		if( j == vNowInside.size() )
			QueueEvent( WgEvent::PointerEnterGizmo( pGizmo ) );
	}

}

//____ ProcessButtonPress() ___________________________________________________

void WgEventHandler::ProcessButtonPress( WgEvent::ButtonPress * pEvent )
{
	int button = pEvent->Button();

	// Post GIZMO_PRESS events

	for( size_t i = 0 ; i < vNowInside.size() ; i++ )
	{
		WgGizmo * pGizmo = vNowInside[i].GetRealPtr();

		if( pGizmo )
		{
			WgCord localPos = pEvent->PointerPos() - pGizmo->ScreenPos();
			QueueEvent( WgEvent::GizmoPress(pGizmo, button, pEvent->PointerPos(), localPos ) );
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
			QueueEvent( WgEvent::ButtonDoubleClick(button);
	}

	// Save info for the future

	m_latestPress[button] = &pEvent;

}



//____ ProcessButtonRelease() _________________________________________________

void WgEventHandler::ProcessButtonRelease( WgEvent::ButtonRelease * pEvent )
{
	int button = pEvent->Button();

	// Post GIZMO_RELEASE events for all that were pressed

	// Post GIZMO_RELEASE events for all that were NOT pressed


}

//____ ProcessButtonDrag() ____________________________________________________

void WgEventHandler::ProcessButtonDrag( WgEvent::ButtonDrag * pEvent )
{
}

