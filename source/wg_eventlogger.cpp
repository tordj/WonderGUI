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

#include <cstdio>

#include <wg_eventlogger.h>
#include <wg_gizmo.h>

using std::string;

//____ Constructor ____________________________________________________________

WgEventLogger::WgEventLogger( std::ostream& stream ) : m_out(stream)
{
	LogAllEvents();
	IgnoreEvent( WG_EVENT_TICK );
}

//____ Destructor _____________________________________________________________

WgEventLogger::~WgEventLogger()
{
}

//____ IgnoreEvent ____________________________________________________________

void WgEventLogger::IgnoreEvent( WgEventId event )
{
	m_eventFilter[event] = false;
}

//____ LogEvent _______________________________________________________________

void WgEventLogger::LogEvent( WgEventId event )
{
	m_eventFilter[event] = true;
}

//____ IgnoreAllEvents ________________________________________________________

void WgEventLogger::IgnoreAllEvents()
{
	for( int i = 0 ; i < WG_EVENT_MAX ; i++ )
		m_eventFilter[i] = false;
}

//____ LogAllEvents ___________________________________________________________
void WgEventLogger::LogAllEvents()
{
	for( int i = 0 ; i < WG_EVENT_MAX ; i++ )
		m_eventFilter[i] = true;
}



//____ OnEvent() ______________________________________________________________

void WgEventLogger::OnEvent( const WgEvent::Event& _event )
{
	if( m_eventFilter[_event.Id()] == false )
		return;

	string	timestamp;
	string	id;
	string	gizmo;
	string	modkeys;
	string	pointerPos;

	char	params[256]; params[0] = 0;			// Event specific parameters


	timestamp = FormatTimestamp( _event.Timestamp() );

	switch( _event.Id() )
	{
		case WG_EVENT_DUMMY:
			id = "Dummy";
			break;
		case WG_EVENT_TICK:
			id = "Tick";
			sprintf( params, " millisec=%d", ((const WgEvent::Tick&)_event).Millisec() );
			break;
		case WG_EVENT_POINTER_ENTER:
			id = "PointerEnter";
			break;
		case WG_EVENT_POINTER_MOVE:
			id = "PointerMove";
			break;
		case WG_EVENT_POINTER_PLACED:
			id = "PointerPlaced";
			break;
		case WG_EVENT_POINTER_EXIT:
			id = "PointerExit";
			break;
		case WG_EVENT_BUTTON_PRESS:
			id = "ButtonPress";
			sprintf( params, " button=%d", ((const WgEvent::ButtonPress&)_event).Button() );
			break;
		case WG_EVENT_BUTTON_REPEAT:
			id = "ButtonRepeat";
			sprintf( params, " button=%d", ((const WgEvent::ButtonRepeat&)_event).Button() );
			break;
		case WG_EVENT_BUTTON_DRAG:
		{
			id = "ButtonDrag";

			const WgEvent::ButtonDrag * pEvent = static_cast<const WgEvent::ButtonDrag*>(&_event);

			WgCord	now		= pEvent->CurrPos();
			WgCord 	prev	= pEvent->PrevPos();
			WgCord	start	= pEvent->StartPos();

			WgCord	dragNow	= pEvent->DraggedSinceLast();
			WgCord	dragTotal=pEvent->DraggedSinceStart();

			sprintf( params, " button=%d position(start=%d,%d prev=%d,%d now=%d,%d) dragged(now=%d,%d total=%d,%d)",
					pEvent->Button(), start.x, start.y, prev.x, prev.y, now.x, now.y, dragNow.x, dragNow.y, dragTotal.x, dragTotal.y );
			break;
		}
		case WG_EVENT_BUTTON_RELEASE:
		{
			id = "ButtonRelease";

			const WgEvent::ButtonRelease * pEvent = static_cast<const WgEvent::ButtonRelease*>(&_event);

			const static char outside[] = "outside";
			const static char inside[] = "inside";
			const char * pPress = outside;
			const char * pRelease = outside;

			if( pEvent->PressInside() )
				pPress = inside;

			if( pEvent->ReleaseInside() )
				pRelease = inside;

			sprintf( params, " button=%d press=%s release=%s", pEvent->Button(), pPress, pRelease );
			break;
		}
		case WG_EVENT_BUTTON_CLICK:
			id = "ButtonClick";
			sprintf( params, " button=%d", ((const WgEvent::ButtonClick&)_event).Button() );
			break;
		case WG_EVENT_BUTTON_DOUBLECLICK:
			id = "ButtonDoubleClick";
			sprintf( params, " button=%d", ((const WgEvent::ButtonDoubleClick&)_event).Button() );
			break;
		case WG_EVENT_KEY_PRESS:
			id = "KeyPress";
			break;
		case WG_EVENT_KEY_REPEAT:
			id = "KeyRepeat";
			break;
		case WG_EVENT_KEY_RELEASE:
			id = "KeyRelease";
			break;
		case WG_EVENT_CHARACTER:
			id = "Character";
			break;
		case WG_EVENT_WHEEL_ROLL:
			id = "WheelRoll";
			break;

		default:
			id = "Unknown Event";
			break;


	};

	gizmo = FormatGizmo( _event );

	modkeys = FormatModkeys( _event );
	pointerPos = FormatPointerPos( _event );




	m_out << timestamp << " - " << id << " - " << gizmo << pointerPos << modkeys << params;



	 m_out << std::endl;
}

//____ FormatTimestamp() ______________________________________________________

string WgEventLogger::FormatTimestamp( int64_t ms )
{
	char	temp[20];

	int milli	= (int) (ms % 1000);
	int sec		= (int) ((ms/1000) % 60);
	int min		= (int) ((ms/60000) % 60);
	int hour	= (int) (ms/3600000);

	sprintf( temp, "%d:%02d:%02d.%03d", hour, min, sec, milli );
	return string( temp );
}

//____ FormatGizmo() __________________________________________________________

string WgEventLogger::FormatGizmo( const WgEvent::Event& _event )
{
	std::string	out;

	if( _event.IsForGizmo() )
	{
		char	temp[64];
		WgGizmo * pGizmo = _event.Gizmo();

		static const char def_type[] = "deleted";
		const char * pType = def_type;

		if( pGizmo )
			pType = pGizmo->Type();

		sprintf( temp, " gizmo=%p (%s)", pGizmo, pType );
		out = temp;
	}

	return out;
}

//____ FormatModkeys() __________________________________________________________

string WgEventLogger::FormatModkeys( const WgEvent::Event& _event )
{
	WgModifierKeys keys = _event.ModKeys();

	string	out;

	if( keys != WG_MODKEY_NONE )
	{
		char	temp[64];
		sprintf( temp, " modkeys=%X", keys );
		out = temp;
	}

	return out;
}

//____ FormatPointerPos() _____________________________________________________

string WgEventLogger::FormatPointerPos( const WgEvent::Event& _event )
{
	WgCord localPos = _event.PointerPos();
	WgCord globalPos = _event.PointerScreenPos();


	char	temp[64];

	if( globalPos == localPos )
		sprintf( temp, " pointer=%d,%d", localPos.x, localPos.y );
	else
		sprintf( temp, " pointer=%d,%d (%d,%d)", localPos.x, localPos.y, globalPos.x, globalPos.y );

	return string(temp);
}

