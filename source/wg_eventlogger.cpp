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
	m_eventFilter[WG_EVENT_MOUSEBUTTON_PRESS] = false;
	m_eventFilter[WG_EVENT_MOUSEBUTTON_REPEAT] = false;
	m_eventFilter[WG_EVENT_MOUSEBUTTON_DRAG] = false;
	m_eventFilter[WG_EVENT_MOUSEBUTTON_RELEASE] = false;
	m_eventFilter[WG_EVENT_MOUSEBUTTON_CLICK] = false;
	m_eventFilter[WG_EVENT_MOUSEBUTTON_DOUBLECLICK] = false;
}

//____ LogMouseButtonEvents _______________________________________________________________

void WgEventLogger::LogMouseButtonEvents()
{
	m_eventFilter[WG_EVENT_MOUSEBUTTON_PRESS] = true;
	m_eventFilter[WG_EVENT_MOUSEBUTTON_REPEAT] = true;
	m_eventFilter[WG_EVENT_MOUSEBUTTON_DRAG] = true;
	m_eventFilter[WG_EVENT_MOUSEBUTTON_RELEASE] = true;
	m_eventFilter[WG_EVENT_MOUSEBUTTON_CLICK] = true;
	m_eventFilter[WG_EVENT_MOUSEBUTTON_DOUBLECLICK] = true;
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
	LogEvent( WG_EVENT_MOUSEWHEEL_ROLL );
}

//____ IgnoreMouseEvents() ____________________________________________________

void WgEventLogger::IgnoreMouseEvents()
{
	IgnorePointerEvents();
	IgnoreMouseButtonEvents();
	IgnoreEvent( WG_EVENT_MOUSEWHEEL_ROLL );
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

void WgEventLogger::OnEvent( const WgEvent::Event * _pEvent )
{
	if( m_eventFilter[_pEvent->Type()] == false )
		return;

	string	timestamp;
	string	id;
	string	gizmo;
	string	modkeys;
	string	pointerPos;

	char	params[256]; params[0] = 0;			// Event specific parameters


	timestamp = _formatTimestamp( _pEvent->Timestamp() );

	switch( _pEvent->Type() )
	{
		case WG_EVENT_DUMMY:
			id = "Dummy";
			break;
		case WG_EVENT_TICK:
			id = "Tick";
			sprintf( params, " millisec=%d", ((const WgEvent::Tick*)_pEvent)->Millisec() );
			break;
		case WG_EVENT_MOUSE_ENTER:
			id = "MouseEnter";
			break;
		case WG_EVENT_MOUSE_MOVE:
			id = "MouseMove";
			break;
		case WG_EVENT_MOUSE_POSITION:
			id = "MousePosition";
			break;
		case WG_EVENT_MOUSE_LEAVE:
			id = "MouseLeave";
			break;
		case WG_EVENT_MOUSEBUTTON_PRESS:
			id = "MouseButtonPress";
			sprintf( params, " button=%d", ((const WgEvent::MouseButtonPress*)_pEvent)->Button() );
			break;
		case WG_EVENT_MOUSEBUTTON_REPEAT:
			id = "MouseButtonRepeat";
			sprintf( params, " button=%d", ((const WgEvent::MouseButtonRepeat*)_pEvent)->Button() );
			break;
		case WG_EVENT_MOUSEBUTTON_DRAG:
		{
			id = "MouseButtonDrag";

			const WgEvent::MouseButtonDrag * pEvent = static_cast<const WgEvent::MouseButtonDrag*>(_pEvent);

			WgCoord	now		= pEvent->CurrPos();
			WgCoord 	prev	= pEvent->PrevPos();
			WgCoord	start	= pEvent->StartPos();

			WgCoord	dragNow	= pEvent->DraggedNow();
			WgCoord	dragTotal=pEvent->DraggedTotal();

			sprintf( params, " button=%d position(start=%d,%d prev=%d,%d now=%d,%d) dragged(now=%d,%d total=%d,%d)",
					pEvent->Button(), start.x, start.y, prev.x, prev.y, now.x, now.y, dragNow.x, dragNow.y, dragTotal.x, dragTotal.y );
			break;
		}
		case WG_EVENT_MOUSEBUTTON_RELEASE:
		{
			id = "MouseButtonRelease";

			const WgEvent::MouseButtonRelease * pEvent = static_cast<const WgEvent::MouseButtonRelease*>(_pEvent);

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
		case WG_EVENT_MOUSEBUTTON_CLICK:
			id = "MouseButtonClick";
			sprintf( params, " button=%d", static_cast<const WgEvent::MouseButtonClick*>(_pEvent)->Button() );
			break;
		case WG_EVENT_MOUSEBUTTON_DOUBLECLICK:
			id = "MouseButtonDoubleClick";
			sprintf( params, " button=%d", static_cast<const WgEvent::MouseButtonDoubleClick*>(_pEvent)->Button() );
			break;

		case WG_EVENT_KEY_PRESS:
		{
			id = "KeyPress";
			const WgEvent::KeyPress * pEvent = static_cast<const WgEvent::KeyPress*>(_pEvent);
			sprintf( params, "wg_keycode=%d native_keycode=%d", pEvent->TranslatedKeyCode(), pEvent->NativeKeyCode() );
			break;
		}
		case WG_EVENT_KEY_REPEAT:
		{
			id = "KeyRepeat";
			const WgEvent::KeyRepeat * pEvent = static_cast<const WgEvent::KeyRepeat*>(_pEvent);
			sprintf( params, "wg_keycode=%d native_keycode=%d", pEvent->TranslatedKeyCode(), pEvent->NativeKeyCode() );
			break;
		}
		case WG_EVENT_KEY_RELEASE:
		{
			id = "KeyRelease";
			const WgEvent::KeyRelease * pEvent = static_cast<const WgEvent::KeyRelease*>(_pEvent);
			sprintf( params, "wg_keycode=%d native_keycode=%d", pEvent->TranslatedKeyCode(), pEvent->NativeKeyCode() );
			break;
		}
		case WG_EVENT_CHARACTER:
			id = "Character";
			sprintf( params, " char=%X", static_cast<const WgEvent::Character*>(_pEvent)->Char() );
			break;
		case WG_EVENT_MOUSEWHEEL_ROLL:
		{
			id = "MouseWheelRoll";
			const WgEvent::MouseWheelRoll * pEvent = static_cast<const WgEvent::MouseWheelRoll*>(_pEvent);
			sprintf( params, "wheel=%d distance=%d", pEvent->Wheel(), pEvent->Distance() );
			break;
		}

		case WG_EVENT_BUTTON_PRESS:
			id = "ButtonPress";
			break;

		case WG_EVENT_CHECKBOX_CHECK:
			id = "CheckboxCheck";
			break;

		case WG_EVENT_CHECKBOX_UNCHECK:
			id = "CheckboxUncheck";
			break;

		case WG_EVENT_CHECKBOX_TOGGLE:
			id = "CheckboxToggle";
			sprintf( params, "checked=%d", (int) (static_cast<const WgEvent::CheckboxToggle*>(_pEvent)->IsChecked()) );
			break;

		case WG_EVENT_RADIOBUTTON_SELECT:
			id = "RadiobuttonSelect";
			break;

		case WG_EVENT_RADIOBUTTON_UNSELECT:
			id = "RadiobuttonUnselect";
			break;

		case WG_EVENT_RADIOBUTTON_TOGGLE:
			id = "RadiobuttonToggle";
			sprintf( params, "selected=%d", (int) (static_cast<const WgEvent::RadiobuttonToggle*>(_pEvent)->IsSelected()) );
			break;

		default:
			id = "Unknown Event";
			break;

		case WG_EVENT_ANIMATION_UPDATE:
		{
			id = "AnimationUpdate";
			const WgEvent::AnimationUpdate * pEvent = static_cast<const WgEvent::AnimationUpdate*>(_pEvent);
			sprintf( params, "frame=%d fraction=%f", pEvent->Frame(), pEvent->Fraction() );
			break;
		}

		case WG_EVENT_TAB_SELECT:
			id = "TabSelect";
			sprintf( params, "tabId=%d", static_cast<const WgEvent::TabSelect*>(_pEvent)->TabId() );
			break;

		case WG_EVENT_TAB_PRESS:
		{
			id = "TabPress";
			const WgEvent::TabPress * pEvent = static_cast<const WgEvent::TabPress*>(_pEvent);
			sprintf( params, "tabId=%d mouseButton=%d", pEvent->TabId(), pEvent->MouseButton() );
			break;
		}

		case WG_EVENT_EDITVALUE_MODIFY:
		{
			id = "EditvalueModify";
			const WgEvent::EditvalueEvent * pEvent = static_cast<const WgEvent::EditvalueEvent*>(_pEvent);
			sprintf( params, "value=%ld fraction=%f", pEvent->Value(), pEvent->Fraction() );
			break;
		}

		case WG_EVENT_EDITVALUE_SET:
		{
			id = "EditvalueSet";
			const WgEvent::EditvalueEvent * pEvent = static_cast<const WgEvent::EditvalueEvent*>(_pEvent);
			sprintf( params, "value=%ld fraction=%f", pEvent->Value(), pEvent->Fraction() );
			break;
		}

		case WG_EVENT_SLIDER_MOVE:
		{
			id = "SliderMove";
			const WgEvent::SliderEvent * pEvent = static_cast<const WgEvent::SliderEvent*>(_pEvent);
			sprintf( params, "pos=%f length=%f", pEvent->Pos(), pEvent->Length() );
			break;
		}
				
		case WG_EVENT_SLIDER_STEP_FWD:
		{
			id = "SliderStepFwd";
			const WgEvent::SliderEvent * pEvent = static_cast<const WgEvent::SliderEvent*>(_pEvent);
			sprintf( params, "pos=%f length=%f", pEvent->Pos(), pEvent->Length() );
			break;
		}

		case WG_EVENT_SLIDER_STEP_BWD:
		{
			id = "SliderStepBwd";
			const WgEvent::SliderEvent * pEvent = static_cast<const WgEvent::SliderEvent*>(_pEvent);
			sprintf( params, "pos=%f length=%f", pEvent->Pos(), pEvent->Length() );
			break;
		}

		case WG_EVENT_SLIDER_JUMP_FWD:
		{
			id = "SliderJumpFwd";
			const WgEvent::SliderEvent * pEvent = static_cast<const WgEvent::SliderEvent*>(_pEvent);
			sprintf( params, "pos=%f length=%f", pEvent->Pos(), pEvent->Length() );
			break;
		}

		case WG_EVENT_SLIDER_JUMP_BWD:
		{
			id = "SliderJumpBwd";
			const WgEvent::SliderEvent * pEvent = static_cast<const WgEvent::SliderEvent*>(_pEvent);
			sprintf( params, "pos=%f length=%f", pEvent->Pos(), pEvent->Length() );
			break;
		}

		case WG_EVENT_SLIDER_WHEEL_ROLL:
		{
			id = "SliderWheelRolled";
			const WgEvent::SliderWheelRolled * pEvent = static_cast<const WgEvent::SliderWheelRolled*>(_pEvent);
			sprintf( params, "distance=%d pos=%f length=%f", pEvent->Distance(), pEvent->Pos(), pEvent->Length() );
			break;
		}

		case WG_EVENT_TEXT_MODIFY:
		{
			id = "TextModify";
			const WgEvent::TextEvent * pEvent = static_cast<const WgEvent::TextEvent*>(_pEvent);
			break;
		}

		case WG_EVENT_TEXT_SET:
		{
			id = "TextSet";
			const WgEvent::TextEvent * pEvent = static_cast<const WgEvent::TextEvent*>(_pEvent);
			break;
		}

		case WG_EVENT_MENUITEM_SELECTED:
		{
			id = "MenuItemSelected";
			const WgEvent::MenuItemEvent * pEvent = static_cast<const WgEvent::MenuItemEvent*>(_pEvent);
			sprintf( params, "itemId=%d", pEvent->ItemId() );
			break;
		}

		case WG_EVENT_MENUITEM_CHECKED:
		{
			id = "MenuItemChecked";
			const WgEvent::MenuItemEvent * pEvent = static_cast<const WgEvent::MenuItemEvent*>(_pEvent);
			sprintf( params, "itemId=%d", pEvent->ItemId() );
			break;
		}

		case WG_EVENT_MENUITEM_UNCHECKED:
		{
			id = "MenuItemUnchecked";
			const WgEvent::MenuItemEvent * pEvent = static_cast<const WgEvent::MenuItemEvent*>(_pEvent);
			sprintf( params, "itemId=%d", pEvent->ItemId() );
			break;
		}

		case WG_EVENT_MODAL_MOVE_OUTSIDE:
			id = "ModalMoveOutside";
			break;
		case WG_EVENT_MODAL_BLOCKED_PRESS:
			id = "ModalBlockedPress";
			sprintf( params, " button=%d", ((const WgEvent::ModalBlockedPress*)_pEvent)->Button() );
			break;
		case WG_EVENT_MODAL_BLOCKED_RELEASE:
			id = "ModalBlockedRelease";
			sprintf( params, " button=%d", ((const WgEvent::ModalBlockedRelease*)_pEvent)->Button() );
			break;


	};

	gizmo = _formatGizmo( _pEvent );
	modkeys = _formatModkeys( _pEvent );
	pointerPos = _formatPointerPos( _pEvent );

	m_out << timestamp << " - " << id << " - " << gizmo << pointerPos << modkeys << params;
	m_out << std::endl;
}

//____ _formatTimestamp() ______________________________________________________

string WgEventLogger::_formatTimestamp( int64_t ms )
{
	char	temp[20];

	int milli	= (int) (ms % 1000);
	int sec		= (int) ((ms/1000) % 60);
	int min		= (int) ((ms/60000) % 60);
	int hour	= (int) (ms/3600000);

	sprintf( temp, "%d:%02d:%02d.%03d", hour, min, sec, milli );
	return string( temp );
}

//____ _formatGizmo() __________________________________________________________

string WgEventLogger::_formatGizmo( const WgEvent::Event * _pEvent )
{
	std::string	out;

	if( _pEvent->IsForGizmo() || _pEvent->Gizmo() )
	{
		char	temp[64];
		WgGizmo * pGizmo = _pEvent->Gizmo();

		static const char def_type[] = "deleted";
		const char * pType = def_type;

		if( pGizmo )
			pType = pGizmo->Type();

		sprintf( temp, " gizmo=%p (%s)", pGizmo, pType );
		out = temp;
	}

	return out;
}

//____ _formatModkeys() __________________________________________________________

string WgEventLogger::_formatModkeys( const WgEvent::Event * _pEvent )
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

string WgEventLogger::_formatPointerPos( const WgEvent::Event * _pEvent )
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

