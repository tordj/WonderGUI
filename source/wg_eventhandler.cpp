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

	m_doubleClickTreshold	= 250;

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

		case WG_EVENT_BUTTON_PRESSED:
		case WG_EVENT_BUTTON_RELEASED:
			_event.m_param[1].integer = m_pointerPos.x;
			_event.m_param[2].integer = m_pointerPos.y;
			break;

		case WG_EVENT_KEY_PRESSED:
		case WG_EVENT_KEY_RELEASED:
		case WG_EVENT_WHEEL_ROLLED:
			_event.m_param[2].integer = m_pointerPos.x;
			_event.m_param[3].integer = m_pointerPos.y;
			break;

		case WG_EVENT_POINTER_MOVED:
		case WG_EVENT_CHARACTER:
		case WG_EVENT_TIME_PASSED:
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
		case WG_EVENT_POINTER_MOVED:
			ProcessPointerMoved( (WgEvent::PointerMoved*) &_event );
			break;
		case WG_EVENT_BUTTON_PRESSED:
			ProcessButtonPressed( (WgEvent::ButtonPressed*) &_event );
			break;
		case WG_EVENT_BUTTON_RELEASED:
			ProcessButtonReleased( (WgEvent::ButtonReleased*) &_event );
			break;
		case WG_EVENT_KEY_PRESSED:
		case WG_EVENT_KEY_RELEASED:
		case WG_EVENT_CHARACTER:
		case WG_EVENT_WHEEL_ROLLED:
			break;

		case WG_EVENT_TIME_PASSED:
			ProcessTimePassed( (WgEvent::TimePassed*) &_event );
			break;

		case WG_EVENT_END_POINTER_MOVED:
			ProcessEndPointerMoved( (WgEvent::EndPointerMoved*) &_event );
			break;
		case WG_EVENT_BUTTON_DRAGGED:
			ProcessButtonDragged( (WgEvent::ButtonDragged*) &_event );
			break;

		case WG_EVENT_DUMMY:
			break;

		case WG_EVENT_POINTER_ENTERED_GIZMO:
			break;

		case WG_EVENT_POINTER_LEFT_GIZMO:
			break;

	}

}

//____ ProcessTimePassed() ____________________________________________________

void WgEventHandler::ProcessTimePassed( WgEvent::TimePassed * pEvent )
{
			m_time += pEvent->Millisec();
}


//____ ProcessPointerMoved() __________________________________________________

void WgEventHandler::ProcessPointerMoved( WgEvent::PointerMoved * pEvent )
{
	// Post events for button drag

	for( int i = 0 ; i < WG_MAX_BUTTONS ; i++ )
	{
		if( m_latestButtonEvents[i].Id() == WG_EVENT_BUTTON_PRESSED )
		{
			WgEvent::ButtonPressed * p = static_cast<WgEvent::ButtonPressed*> (&m_latestButtonEvents[i]);

			QueueEvent( WgEvent::ButtonDragged( i, p->PointerPos(), m_pointerPos, pEvent->Pos() ) );
		}
	}

	// Post event for finalizing move once button drag is taken care of.

	QueueEvent( WgEvent::EndPointerMoved( pEvent->Pos() ) );

	// Update pointer position

	m_pointerPos = pEvent->Pos();
}

//____ ProcessEndPointerMoved() _______________________________________________

void WgEventHandler::ProcessEndPointerMoved( WgEvent::EndPointerMoved * pEvent )
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
			QueueEvent( WgEvent::PointerLeftGizmo( pGizmo ) );
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
			QueueEvent( WgEvent::PointerEnteredGizmo( pGizmo ) );
	}

}

//____ ProcessButtonPressed() _________________________________________________

void WgEventHandler::ProcessButtonPressed( WgEvent::ButtonPressed * pEvent )
{
}

//____ ProcessButtonReleased() ________________________________________________

void WgEventHandler::ProcessButtonReleased( WgEvent::ButtonReleased * pEvent )
{
}

//____ ProcessButtonDragged() _________________________________________________

void WgEventHandler::ProcessButtonDragged( WgEvent::ButtonDragged * pEvent )
{
}

