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

//____ Constructor ____________________________________________________________

WgEventHandler::WgEventHandler( Int64 startTime )
{
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

bool WgEventHandler::QueueEvent( const WgEvent::EventBase& _event )
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
		WgEvent::EventBase ev = m_eventQueue.front();
		m_eventQueue.pop_front();

		CompleteEvent( ev );
		ProcessEventInternal( ev );
//		ProcessEventCallbacks( ev );
	}

	m_bIsProcessing = false;
}


//____ CompleteEvent() ________________________________________________________

void WgEventHandler::CompleteEvent( WgEvent::EventBase& _event )
{
	// Fill in missing information in the event-class.

	_event.m_timestamp	= m_time;
	_event.m_modKeys	= m_modKeys;

	switch( _event.m_id )
	{

		case WG_EVENT_BUTTON_PRESSED:
		case WG_EVENT_BUTTON_RELEASED:
			m_param[1].integer = m_pointerPos.x;
			m_param[2].integer = m_pointerPos.y;
			break;

		case WG_EVENT_KEY_PRESSED:
		case WG_EVENT_KEY_RELEASED:
		case WG_EVENT_WHEEL_ROLLED:
			m_param[2].integer = m_pointerPos.x;
			m_param[3].integer = m_pointerPos.y;
			break;

		case WG_EVENT_POINTER_MOVED:
		case WG_EVENT_KEY_PRESSED:
		case WG_EVENT_KEY_RELEASED:
		case WG_EVENT_CHARACTER:
		case WG_EVENT_TIME_PASSED:
			break;
	}

}


//____ ProcessEventInternal() _________________________________________________

void WgEventHandler::ProcessEventInternal( WgEvent::EventBase& _event )
{

	switch( _event.m_id )
	{
		case WG_EVENT_POINTER_MOVED:
			ProcessPointerMoved( (WgEvent::PointerMoved*) &_event);
			break;
		case WG_EVENT_BUTTON_PRESSED:
		case WG_EVENT_BUTTON_RELEASED:
		case WG_EVENT_KEY_PRESSED:
		case WG_EVENT_KEY_RELEASED:
		case WG_EVENT_CHARACTER:
		case WG_EVENT_WHEEL_ROLLED:
			break;

		case WG_EVENT_END_POINTER_MOVED:
		case WG_EVENT_BUTTON_DRAG:
			 break;

		case WG_EVENT_TIME_PASSED:
			m_time += m_param[0].integer;
			break;
	}

}

//____ ProcessPointerMoved() __________________________________________________

void WgEventHandler::ProcessPointerMoved( WgEvent::PointerMoved * pEvent )
{
	// Update pointer position

	m_pointerPos = pEvent->Pos();

	// Post events for button drag


	// Post event for finalizing move once button drag is taken care of.

	QueueEvent( WgEvent::EndPointerMoved( m_pointerPos ) );

}