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
#include <wg_widget.h>

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

void WgEventLogger::IgnoreEvent( WgEventType event )
{
	m_eventFilter[event] = false;
}

//____ LogEvent _______________________________________________________________

void WgEventLogger::LogEvent( WgEventType event )
{
	m_eventFilter[event] = true;
}

//____ IgnorePointerEvents ____________________________________________________________

void WgEventLogger::IgnorePointerEvents()
{
	m_eventFilter[WG_EVENT_MOUSE_ENTER] = false;
	m_eventFilter[WG_EVENT_MOUSE_MOVE] = false;
	m_eventFilter[WG_EVENT_MOUSE_POSITION] = false;
	m_eventFilter[WG_EVENT_MOUSE_LEAVE] = false;
}

//____ LogPointerEvents _______________________________________________________________

void WgEventLogger::LogPointerEvents()
{
	m_eventFilter[WG_EVENT_MOUSE_ENTER] = true;
	m_eventFilter[WG_EVENT_MOUSE_MOVE] = true;
	m_eventFilter[WG_EVENT_MOUSE_POSITION] = true;
	m_eventFilter[WG_EVENT_MOUSE_LEAVE] = true;
}


//____ IgnoreMouseButtonEvents ____________________________________________________________

void WgEventLogger::IgnoreMouseButtonEvents()
{
	m_eventFilter[WG_EVENT_MOUSE_PRESS] = false;
	m_eventFilter[WG_EVENT_MOUSE_REPEAT] = false;
	m_eventFilter[WG_EVENT_MOUSE_DRAG] = false;
	m_eventFilter[WG_EVENT_MOUSE_RELEASE] = false;
	m_eventFilter[WG_EVENT_MOUSE_CLICK] = false;
	m_eventFilter[WG_EVENT_MOUSE_DOUBLE_CLICK] = false;
}

//____ LogMouseButtonEvents _______________________________________________________________

void WgEventLogger::LogMouseButtonEvents()
{
	m_eventFilter[WG_EVENT_MOUSE_PRESS] = true;
	m_eventFilter[WG_EVENT_MOUSE_REPEAT] = true;
	m_eventFilter[WG_EVENT_MOUSE_DRAG] = true;
	m_eventFilter[WG_EVENT_MOUSE_RELEASE] = true;
	m_eventFilter[WG_EVENT_MOUSE_CLICK] = true;
	m_eventFilter[WG_EVENT_MOUSE_DOUBLE_CLICK] = true;
}

//____ IgnoreKeyboardEvents ____________________________________________________________

void WgEventLogger::IgnoreKeyboardEvents()
{
	m_eventFilter[WG_EVENT_KEY_PRESS] = false;
	m_eventFilter[WG_EVENT_KEY_REPEAT] = false;
	m_eventFilter[WG_EVENT_KEY_RELEASE] = false;
	m_eventFilter[WG_EVENT_CHARACTER] = false;
}

//____ LogKeyboardEvents _______________________________________________________________

void WgEventLogger::LogKeyboardEvents()
{
	m_eventFilter[WG_EVENT_KEY_PRESS] = true;
	m_eventFilter[WG_EVENT_KEY_REPEAT] = true;
	m_eventFilter[WG_EVENT_KEY_RELEASE] = true;
	m_eventFilter[WG_EVENT_CHARACTER] = true;
}

//____ LogMouseEvents() _______________________________________________________

void WgEventLogger::LogMouseEvents()
{
	LogPointerEvents();
	LogMouseButtonEvents();
	LogEvent( WG_EVENT_WHEEL_ROLL );
}

//____ IgnoreMouseEvents() ____________________________________________________

void WgEventLogger::IgnoreMouseEvents()
{
	IgnorePointerEvents();
	IgnoreMouseButtonEvents();
	IgnoreEvent( WG_EVENT_WHEEL_ROLL );
}


//____ LogInputEvents() _______________________________________________________

void WgEventLogger::LogInputEvents()
{
	LogMouseEvents();
	LogKeyboardEvents();
}

//____ IgnoreInputEvents() ____________________________________________________

void WgEventLogger::IgnoreInputEvents()
{
	IgnoreMouseEvents();
	IgnoreKeyboardEvents();
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

void WgEventLogger::OnEvent( const WgEventPtr& _pEvent )
{
	if( m_eventFilter[_pEvent->Type()] == false )
		return;

	string	timestamp;
	string	widget;
	string	modkeys;
	string	pointerPos;

	char	params[256]; params[0] = 0;			// Event specific parameters


	timestamp = _formatTimestamp( _pEvent->Timestamp() );

	switch( _pEvent->Type() )
	{
		case WG_EVENT_DUMMY:
			break;
		case WG_EVENT_TICK:
			sprintf( params, " millisec=%d", WgTickEvent::Cast(_pEvent)->Millisec() );
			break;
		case WG_EVENT_POINTER_CHANGE:
			sprintf( params, " style=%s", _formatPointerStyle( WgPointerChangeEvent::Cast(_pEvent)).c_str() );
			break;
		case WG_EVENT_MOUSE_ENTER:
			break;
		case WG_EVENT_MOUSE_MOVE:
			break;
		case WG_EVENT_MOUSE_POSITION:
			break;
		case WG_EVENT_MOUSE_LEAVE:
			break;
		case WG_EVENT_MOUSE_PRESS:
			sprintf( params, " button=%s", _formatMouseButton(WgMousePressEvent::Cast(_pEvent)->Button()).c_str() );
			break;
		case WG_EVENT_MOUSE_REPEAT:
			sprintf( params, " button=%s", _formatMouseButton(WgMouseRepeatEvent::Cast(_pEvent)->Button()).c_str() );
			break;
		case WG_EVENT_MOUSE_DRAG:
		{
			WgMouseDragEventPtr pEvent = WgMouseDragEvent::Cast(_pEvent);

			WgCoord	now		= pEvent->CurrPos();
			WgCoord 	prev	= pEvent->PrevPos();
			WgCoord	start	= pEvent->StartPos();

			WgCoord	dragNow	= pEvent->DraggedNow();
			WgCoord	dragTotal=pEvent->DraggedTotal();

			sprintf( params, " button=%s position(start=%d,%d prev=%d,%d now=%d,%d) dragged(now=%d,%d total=%d,%d)",
					_formatMouseButton(pEvent->Button()).c_str(), start.x, start.y, prev.x, prev.y, now.x, now.y, dragNow.x, dragNow.y, dragTotal.x, dragTotal.y );
			break;
		}
		case WG_EVENT_MOUSE_RELEASE:
		{
			WgMouseReleaseEventPtr pEvent = WgMouseReleaseEvent::Cast(_pEvent);

			const static char outside[] = "outside";
			const static char inside[] = "inside";
			const char * pPress = outside;
			const char * pRelease = outside;

			if( pEvent->PressInside() )
				pPress = inside;

			if( pEvent->ReleaseInside() )
				pRelease = inside;

			sprintf( params, " button=%s press=%s release=%s", _formatMouseButton(pEvent->Button()).c_str(), pPress, pRelease );
			break;
		}
		case WG_EVENT_MOUSE_CLICK:
			sprintf( params, " button=%s", _formatMouseButton(WgMouseClickEvent::Cast(_pEvent)->Button()).c_str() );
			break;
		case WG_EVENT_MOUSE_DOUBLE_CLICK:
			sprintf( params, " button=%s", _formatMouseButton(WgMouseDoubleClickEvent::Cast(_pEvent)->Button()).c_str() );
			break;

		case WG_EVENT_KEY_PRESS:
		{
			WgKeyPressEventPtr pEvent = WgKeyPressEvent::Cast(_pEvent);
			sprintf( params, "wg_keycode=%d native_keycode=%d", pEvent->TranslatedKeyCode(), pEvent->NativeKeyCode() );
			break;
		}
		case WG_EVENT_KEY_REPEAT:
		{
			WgKeyRepeatEventPtr pEvent = WgKeyRepeatEvent::Cast(_pEvent);
			sprintf( params, "wg_keycode=%d native_keycode=%d", pEvent->TranslatedKeyCode(), pEvent->NativeKeyCode() );
			break;
		}
		case WG_EVENT_KEY_RELEASE:
		{
			WgKeyReleaseEventPtr pEvent = WgKeyReleaseEvent::Cast(_pEvent);
			sprintf( params, "wg_keycode=%d native_keycode=%d", pEvent->TranslatedKeyCode(), pEvent->NativeKeyCode() );
			break;
		}
		case WG_EVENT_CHARACTER:
			sprintf( params, " char=%X", WgCharacterEvent::Cast(_pEvent)->Char() );
			break;
		case WG_EVENT_WHEEL_ROLL:
		{
			WgWheelRollEventPtr pEvent = WgWheelRollEvent::Cast(_pEvent);
			sprintf( params, "wheel=%d distance=%d", pEvent->Wheel(), pEvent->Distance() );
			break;
		}

		case WG_EVENT_SELECT:
			break;

		case WG_EVENT_TOGGLE:
			sprintf( params, "set=%d", (int) (WgToggleEvent::Cast(_pEvent)->IsSet()) );
			break;

		case WG_EVENT_VALUE_UPDATE:
		{
			WgValueUpdateEventPtr pEvent = WgValueUpdateEvent::Cast(_pEvent);
			sprintf( params, "value=%ld fraction=%f", pEvent->Value(), pEvent->Fraction() );
			break;
		}

		case WG_EVENT_RANGE_UPDATE:
		{
			WgRangeUpdateEventPtr pEvent = WgRangeUpdateEvent::Cast(_pEvent);
			sprintf( params, "pos=%f length=%f", pEvent->Offset(), pEvent->Length() );
			break;
		}

		case WG_EVENT_TEXT_EDIT:
		{
			WgTextEditEventPtr pEvent = WgTextEditEvent::Cast(_pEvent);
			break;
		}

		case WG_EVENT_ITEM_TOGGLE:
		{
			WgItemToggleEventPtr pEvent = WgItemToggleEvent::Cast(_pEvent);
			sprintf( params, "set=%s index=%d id=%d object=%d", pEvent->IsSet()?"true":"false", pEvent->ItemIndex(), pEvent->ItemId() );
			break;
		}
		case WG_EVENT_ITEM_MOUSE_PRESS:
		{
			WgItemMousePressEventPtr pEvent = WgItemMousePressEvent::Cast(_pEvent);
			sprintf( params, "index=%d id=% mouseButton=%s", pEvent->ItemIndex(), pEvent->ItemId(), _formatMouseButton(pEvent->Button()).c_str() );
			break;
		}
				
		case WG_EVENT_ITEMS_SELECT:
		{
			WgItemsSelectEventPtr pEvent = WgItemsSelectEvent::Cast(_pEvent);
			sprintf( params, "nbItems=%d", pEvent->NbItems() );
			break;
		}

		case WG_EVENT_ITEMS_UNSELECT:
		{
			WgItemsUnselectEventPtr pEvent = WgItemsUnselectEvent::Cast(_pEvent);
			sprintf( params, "nbItems=%d", pEvent->NbItems() );
			break;
		}

		case WG_EVENT_POPUP_CLOSED:
		{
			WgPopupClosedEventPtr pEvent = WgPopupClosedEvent::Cast(_pEvent);
			sprintf( params, "popup=%d", pEvent->Popup() );
			break;
		}

		case WG_EVENT_MODAL_MOVE_OUTSIDE:
			break;
		case WG_EVENT_MODAL_BLOCKED_PRESS:
			sprintf( params, " button=%s", _formatMouseButton(WgModalBlockedPressEvent::Cast(_pEvent)->Button()).c_str() );
			break;
		case WG_EVENT_MODAL_BLOCKED_RELEASE:
			sprintf( params, " button=%s", _formatMouseButton(WgModalBlockedReleaseEvent::Cast(_pEvent)->Button()).c_str() );
			break;

		default:
			break;
	};

	widget = _formatWidget( _pEvent );
	modkeys = _formatModkeys( _pEvent );
	pointerPos = _formatPointerPos( _pEvent );

	m_out << timestamp << " - " << _pEvent->ClassName() << " - " << widget << pointerPos << modkeys << params;
	m_out << std::endl;
}

//____ _formatTimestamp() ______________________________________________________

string WgEventLogger::_formatTimestamp( int64_t ms ) const
{
	char	temp[20];

	int milli	= (int) (ms % 1000);
	int sec		= (int) ((ms/1000) % 60);
	int min		= (int) ((ms/60000) % 60);
	int hour	= (int) (ms/3600000);

	sprintf( temp, "%d:%02d:%02d.%03d", hour, min, sec, milli );
	return string( temp );
}

//____ _formatWidget() __________________________________________________________

string WgEventLogger::_formatWidget( const WgEventPtr& _pEvent ) const
{
	std::string	out;

	if( _pEvent->IsFromWidget() )
	{
		char	temp[64];
		WgWidget * pWidget = _pEvent->Widget();

		static const char def_type[] = "deleted";
		const char * pType = def_type;

		if( pWidget )
			pType = pWidget->ClassName();

		sprintf( temp, " widget=%p (%s)", pWidget, pType );
		out = temp;
	}

	return out;
}

//____ _formatModkeys() __________________________________________________________

string WgEventLogger::_formatModkeys( const WgEventPtr& _pEvent ) const
{
	WgModifierKeys keys = _pEvent->ModKeys();

	string	out;

	if( keys != WG_MODKEY_NONE )
	{
		char	temp[64];
		sprintf( temp, " modkeys=%X", keys );
		out = temp;
	}

	return out;
}

//____ _formatPointerPos() _____________________________________________________

string WgEventLogger::_formatPointerPos( const WgEventPtr& _pEvent ) const
{
	WgCoord localPos = _pEvent->PointerPos();
	WgCoord globalPos = _pEvent->PointerScreenPos();


	char	temp[64];

	if( globalPos == localPos )
		sprintf( temp, " pointer=%d,%d", localPos.x, localPos.y );
	else
		sprintf( temp, " pointer=%d,%d (%d,%d)", localPos.x, localPos.y, globalPos.x, globalPos.y );

	return string(temp);
}

//____ _formatPointerStyle() _____________________________________________________

string WgEventLogger::_formatPointerStyle( const WgPointerChangeEventPtr& _pEvent ) const
{
	switch( _pEvent->Style() )
	{
		case WG_POINTER_ARROW:
			return "WG_POINTER_ARROW";
		case WG_POINTER_HOURGLASS:
			return "WG_POINTER_HOURGLASS";
		case WG_POINTER_HAND:
			return "WG_POINTER_HAND";
		case WG_POINTER_CROSSHAIR:
			return "WG_POINTER_CROSSHAIR";
		case WG_POINTER_HELP:
			return "WG_POINTER_HELP";
		case WG_POINTER_IBEAM:
			return "WG_POINTER_IBEAM";
		case WG_POINTER_STOP:
			return "WG_POINTER_STOP";
		case WG_POINTER_UP_ARROW:
			return "WG_POINTER_UP_ARROW";
		case WG_POINTER_SIZE_ALL:
			return "WG_POINTER_SIZE_ALL";
		case WG_POINTER_SIZE_NE_SW:
			return "WG_POINTER_SIZE_NE_SW";
		case WG_POINTER_SIZE_NW_SE:
			return "WG_POINTER_SIZE_NW_SE";
		case WG_POINTER_SIZE_N_S:
			return "WG_POINTER_SIZE_N_S";
		case WG_POINTER_SIZE_W_E:
			return "WG_POINTER_SIZE_W_E";
		default:
		{
			char	temp[64];
			sprintf( temp, "%d (unkown enum)", _pEvent->Style() );
			return string(temp);
		}
	}
}

//____ _formatMouseButton() ____________________________________________________

std::string WgEventLogger::_formatMouseButton( WgMouseButton button ) const
{
	switch( button )
	{
		case WG_BUTTON_LEFT:
			return "LEFT";
		case WG_BUTTON_MIDDLE:
			return "MIDDLE";
		case WG_BUTTON_RIGHT:
			return "RIGHT";
		case WG_BUTTON_X1:
			return "X1";
		case WG_BUTTON_X2:
			return "X2";
		default:
		{
			char	temp[64];
			sprintf( temp, "%d (unkown enum)", button );
			return string(temp);
		}
	}
}

