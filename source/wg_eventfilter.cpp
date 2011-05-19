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

#include <assert.h>
#include <wg_key.h>
#include <wg_event.h>
#include <wg_eventfilter.h>
#include <wg_gizmo.h>

//____ PointerEnter() _________________________________________________________

WgEventFilter	WgEventFilter::PointerEnter()
{
	return WgEventFilter( WG_EVENT_POINTER_ENTER, 0, _filterPointerEvents );
}

WgEventFilter	WgEventFilter::PointerEnter( WgGizmo * pGizmo )
{
	return WgEventFilter( WG_EVENT_POINTER_ENTER, pGizmo, _filterPointerEvents );
}

//____ PointerExit() _________________________________________________________

WgEventFilter	WgEventFilter::PointerExit()
{
	return WgEventFilter( WG_EVENT_POINTER_EXIT, 0, _filterPointerEvents );
}

WgEventFilter	WgEventFilter::PointerExit( WgGizmo * pGizmo )
{
	return WgEventFilter( WG_EVENT_POINTER_EXIT, pGizmo, _filterPointerEvents );
}

//____ PointerMove() _________________________________________________________

WgEventFilter	WgEventFilter::PointerMove()
{
	return WgEventFilter( WG_EVENT_POINTER_MOVE, 0, _filterPointerEvents );
}

WgEventFilter	WgEventFilter::PointerMove( WgGizmo * pGizmo )
{
	return WgEventFilter( WG_EVENT_POINTER_MOVE, pGizmo, _filterPointerEvents );
}

//____ ButtonPress() _________________________________________________________

WgEventFilter	WgEventFilter::ButtonPress()
{
	return WgEventFilter( WG_EVENT_BUTTON_PRESS, 0, _filterButtonEvents, 0 );
}

WgEventFilter	WgEventFilter::ButtonPress( int button)
{
	return WgEventFilter( WG_EVENT_BUTTON_PRESS, 0, _filterButtonEvents, button );
}

WgEventFilter	WgEventFilter::ButtonPress( WgGizmo * pGizmo )
{
	return WgEventFilter( WG_EVENT_BUTTON_PRESS, pGizmo, _filterButtonEvents, 0 );
}

WgEventFilter	WgEventFilter::ButtonPress( WgGizmo * pGizmo, int button )
{
	return WgEventFilter( WG_EVENT_BUTTON_PRESS, pGizmo, _filterButtonEvents, button );
}

//____ ButtonRelease() ________________________________________________________

WgEventFilter	WgEventFilter::ButtonRelease()
{
	return WgEventFilter( WG_EVENT_BUTTON_RELEASE, 0, _filterButtonEvents, 0 );
}

WgEventFilter	WgEventFilter::ButtonRelease( int button)
{
	return WgEventFilter( WG_EVENT_BUTTON_RELEASE, 0, _filterButtonEvents, button );
}

WgEventFilter	WgEventFilter::ButtonRelease( WgGizmo * pGizmo )
{
	return WgEventFilter( WG_EVENT_BUTTON_RELEASE, pGizmo, _filterButtonEvents, 0 );
}

WgEventFilter	WgEventFilter::ButtonRelease( WgGizmo * pGizmo, int button )
{
	return WgEventFilter( WG_EVENT_BUTTON_RELEASE, pGizmo, _filterButtonEvents, button );
}

//____ ButtonDrag() ___________________________________________________________

WgEventFilter	WgEventFilter::ButtonDrag()
{
	return WgEventFilter( WG_EVENT_BUTTON_DRAG, 0, _filterButtonEvents, 0 );
}

WgEventFilter	WgEventFilter::ButtonDrag( int button)
{
	return WgEventFilter( WG_EVENT_BUTTON_DRAG, 0, _filterButtonEvents, button );
}

WgEventFilter	WgEventFilter::ButtonDrag( WgGizmo * pGizmo )
{
	return WgEventFilter( WG_EVENT_BUTTON_DRAG, pGizmo, _filterButtonEvents, 0 );
}

WgEventFilter	WgEventFilter::ButtonDrag( WgGizmo * pGizmo, int button )
{
	return WgEventFilter( WG_EVENT_BUTTON_DRAG, pGizmo, _filterButtonEvents, button );
}

//____ ButtonRepeat() ________________________________________________________

WgEventFilter	WgEventFilter::ButtonRepeat()
{
	return WgEventFilter( WG_EVENT_BUTTON_REPEAT, 0, _filterButtonEvents, 0 );
}

WgEventFilter	WgEventFilter::ButtonRepeat( int button)
{
	return WgEventFilter( WG_EVENT_BUTTON_REPEAT, 0, _filterButtonEvents, button );
}

WgEventFilter	WgEventFilter::ButtonRepeat( WgGizmo * pGizmo )
{
	return WgEventFilter( WG_EVENT_BUTTON_REPEAT, pGizmo, _filterButtonEvents, 0 );
}

WgEventFilter	WgEventFilter::ButtonRepeat( WgGizmo * pGizmo, int button )
{
	return WgEventFilter( WG_EVENT_BUTTON_REPEAT, pGizmo, _filterButtonEvents, button );
}

//____ ButtonClick() ________________________________________________________

WgEventFilter	WgEventFilter::ButtonClick()
{
	return WgEventFilter( WG_EVENT_BUTTON_CLICK, 0, _filterButtonEvents, 0 );
}

WgEventFilter	WgEventFilter::ButtonClick( int button)
{
	return WgEventFilter( WG_EVENT_BUTTON_CLICK, 0, _filterButtonEvents, button );
}

WgEventFilter	WgEventFilter::ButtonClick( WgGizmo * pGizmo )
{
	return WgEventFilter( WG_EVENT_BUTTON_CLICK, pGizmo, _filterButtonEvents, 0 );
}

WgEventFilter	WgEventFilter::ButtonClick( WgGizmo * pGizmo, int button )
{
	return WgEventFilter( WG_EVENT_BUTTON_CLICK, pGizmo, _filterButtonEvents, button );
}

//____ ButtonDoubleClick() ________________________________________________________

WgEventFilter	WgEventFilter::ButtonDoubleClick()
{
	return WgEventFilter( WG_EVENT_BUTTON_CLICK, 0, _filterButtonEvents, 0 );
}

WgEventFilter	WgEventFilter::ButtonDoubleClick( int button)
{
	return WgEventFilter( WG_EVENT_BUTTON_CLICK, 0, _filterButtonEvents, button );
}

WgEventFilter	WgEventFilter::ButtonDoubleClick( WgGizmo * pGizmo )
{
	return WgEventFilter( WG_EVENT_BUTTON_CLICK, pGizmo, _filterButtonEvents, 0 );
}

WgEventFilter	WgEventFilter::ButtonDoubleClick( WgGizmo * pGizmo, int button )
{
	return WgEventFilter( WG_EVENT_BUTTON_CLICK, pGizmo, _filterButtonEvents, button );
}

//____ KeyPress() _____________________________________________________________

WgEventFilter	WgEventFilter::KeyPress()
{
	return WgEventFilter( WG_EVENT_KEY_PRESS, 0, _filterKeyEvents, WG_KEY_UNMAPPED );
}

WgEventFilter	WgEventFilter::KeyPress( WgGizmo * pGizmo )
{
	return WgEventFilter( WG_EVENT_KEY_PRESS, pGizmo, _filterKeyEvents, WG_KEY_UNMAPPED );
}

WgEventFilter	WgEventFilter::KeyPress( WgKey keycode )
{
	return WgEventFilter( WG_EVENT_KEY_PRESS, 0, _filterKeyEvents, keycode );
}

WgEventFilter	WgEventFilter::KeyPress( WgGizmo * pGizmo, WgKey keycode )
{
	return WgEventFilter( WG_EVENT_KEY_PRESS, pGizmo, _filterKeyEvents, keycode );
}

//____ KeyRelease() ___________________________________________________________

WgEventFilter	WgEventFilter::KeyRelease()
{
	return WgEventFilter( WG_EVENT_KEY_RELEASE, 0, _filterKeyEvents, WG_KEY_UNMAPPED );
}

WgEventFilter	WgEventFilter::KeyRelease( WgGizmo * pGizmo )
{
	return WgEventFilter( WG_EVENT_KEY_RELEASE, pGizmo, _filterKeyEvents, WG_KEY_UNMAPPED );
}

WgEventFilter	WgEventFilter::KeyRelease( WgKey keycode )
{
	return WgEventFilter( WG_EVENT_KEY_RELEASE, 0, _filterKeyEvents, keycode );
}

WgEventFilter	WgEventFilter::KeyRelease( WgGizmo * pGizmo, WgKey keycode )
{
	return WgEventFilter( WG_EVENT_KEY_RELEASE, pGizmo, _filterKeyEvents, keycode );
}

//____ KeyRepeat() ____________________________________________________________

WgEventFilter	WgEventFilter::KeyRepeat()
{
	return WgEventFilter( WG_EVENT_KEY_REPEAT, 0, _filterKeyEvents, WG_KEY_UNMAPPED );
}

WgEventFilter	WgEventFilter::KeyRepeat( WgGizmo * pGizmo )
{
	return WgEventFilter( WG_EVENT_KEY_REPEAT, pGizmo, _filterKeyEvents, WG_KEY_UNMAPPED );
}

WgEventFilter	WgEventFilter::KeyRepeat( WgKey keycode )
{
	return WgEventFilter( WG_EVENT_KEY_REPEAT, 0, _filterKeyEvents, keycode );
}

WgEventFilter	WgEventFilter::KeyRepeat( WgGizmo * pGizmo, WgKey keycode )
{
	return WgEventFilter( WG_EVENT_KEY_REPEAT, pGizmo, _filterKeyEvents, keycode );
}

//____ KeyPressNative() _______________________________________________________

WgEventFilter	WgEventFilter::KeyPressNative( int native_keycode )
{
	return WgEventFilter( WG_EVENT_KEY_PRESS, 0, _filterNativeKeyEvents, native_keycode );
}

WgEventFilter	WgEventFilter::KeyPressNative( WgGizmo * pGizmo, int native_keycode )
{
	return WgEventFilter( WG_EVENT_KEY_PRESS, pGizmo, _filterNativeKeyEvents, native_keycode );
}

//____ KeyReleaseNative() _____________________________________________________

WgEventFilter	WgEventFilter::KeyReleaseNative( int native_keycode )
{
	return WgEventFilter( WG_EVENT_KEY_RELEASE, 0, _filterNativeKeyEvents, native_keycode );
}

WgEventFilter	WgEventFilter::KeyReleaseNative( WgGizmo * pGizmo, int native_keycode )
{
	return WgEventFilter( WG_EVENT_KEY_RELEASE, pGizmo, _filterNativeKeyEvents, native_keycode );
}

//____ KeyRepeatNative() ______________________________________________________

WgEventFilter	WgEventFilter::KeyRepeatNative( int native_keycode )
{
	return WgEventFilter( WG_EVENT_KEY_REPEAT, 0, _filterNativeKeyEvents, native_keycode );
}

WgEventFilter	WgEventFilter::KeyRepeatNative( WgGizmo * pGizmo, int native_keycode )
{
	return WgEventFilter( WG_EVENT_KEY_REPEAT, pGizmo, _filterNativeKeyEvents, native_keycode );
}

//____ Character() ____________________________________________________________

WgEventFilter	WgEventFilter::Character()
{
	return WgEventFilter( WG_EVENT_CHARACTER, 0, _filterCharacterEvents, 0 );
}

WgEventFilter	WgEventFilter::Character( unsigned short character)
{
	return WgEventFilter( WG_EVENT_CHARACTER, 0, _filterCharacterEvents, character );
}

WgEventFilter	WgEventFilter::Character( WgGizmo * pGizmo )
{
	return WgEventFilter( WG_EVENT_CHARACTER, pGizmo, _filterCharacterEvents, 0 );
}

WgEventFilter	WgEventFilter::Character( WgGizmo * pGizmo, unsigned short character )
{
	return WgEventFilter( WG_EVENT_CHARACTER, pGizmo, _filterCharacterEvents, character );
}

//____ WheelRoll() ____________________________________________________________

WgEventFilter	WgEventFilter::WheelRoll()
{
	return WgEventFilter( WG_EVENT_WHEEL_ROLL, 0, _filterWheelRollEvents, 0 );
}

WgEventFilter	WgEventFilter::WheelRoll( int wheel )
{
	return WgEventFilter( WG_EVENT_WHEEL_ROLL, 0, _filterWheelRollEvents, wheel );
}

WgEventFilter	WgEventFilter::WheelRoll( WgGizmo * pGizmo )
{
	return WgEventFilter( WG_EVENT_WHEEL_ROLL, pGizmo, _filterWheelRollEvents, 0 );
}

WgEventFilter	WgEventFilter::WheelRoll( int wheel, WgGizmo * pGizmo )
{
	return WgEventFilter( WG_EVENT_WHEEL_ROLL, pGizmo, _filterWheelRollEvents, wheel );
}

//____ Tick() _________________________________________________________________

WgEventFilter	WgEventFilter::Tick()
{
	return WgEventFilter( WG_EVENT_TICK, 0, _filterTickEvents );
}

//_____________________________________________________________________________

bool WgEventFilter::_filterDummy( const WgEvent::Event& _event, const WgEventFilter& filter )
{
	return true;
}


bool WgEventFilter::_filterPointerEvents( const WgEvent::Event& _event, const WgEventFilter& filter )
{
	if( _event.Type() == filter.EventType() && (!filter.Gizmo() || filter.Gizmo() == _event.Gizmo()) )
		return true;

	return false;
}

bool WgEventFilter::_filterButtonEvents( const WgEvent::Event& _event, const WgEventFilter& filter )
{
	if( _event.Type() == filter.EventType() && (!filter.Gizmo() || filter.Gizmo() == _event.Gizmo()) )
	{
		if( filter.m_data1 == 0 )
			return true;
		else
		{
			int btn;

			switch(_event.Type() )
			{
				case WG_EVENT_BUTTON_PRESS:
					btn = (static_cast<const WgEvent::ButtonPress&>(_event)).Button();
					break;
				case WG_EVENT_BUTTON_REPEAT:
					btn = (static_cast<const WgEvent::ButtonRepeat&>(_event)).Button();
					break;
				case WG_EVENT_BUTTON_DRAG:
					btn = (static_cast<const WgEvent::ButtonDrag&>(_event)).Button();
					break;
				case WG_EVENT_BUTTON_RELEASE:
					btn = (static_cast<const WgEvent::ButtonRelease&>(_event)).Button();
					break;
				case WG_EVENT_BUTTON_CLICK:
					btn = (static_cast<const WgEvent::ButtonClick&>(_event)).Button();
					break;
				case WG_EVENT_BUTTON_DOUBLECLICK:
					btn = (static_cast<const WgEvent::ButtonDoubleClick&>(_event)).Button();
					break;
				default:
					assert(false);		// We should never get here.
					return false;
			}

			if( btn == filter.m_data1 )
				return true;
		}
	}
	return false;
}

bool WgEventFilter::_filterKeyEvents( const WgEvent::Event& _event, const WgEventFilter& filter )
{
	if( _event.Type() == filter.EventType() && (!filter.Gizmo() || filter.Gizmo() == _event.Gizmo()) )
	{
		if( filter.m_data1 == 0 )
			return true;
		else
		{
			int key;

			switch(_event.Type() )
			{
				case WG_EVENT_KEY_PRESS:
					key = (static_cast<const WgEvent::KeyPress&>(_event)).TranslatedKeyCode();
					break;
				case WG_EVENT_KEY_REPEAT:
					key = (static_cast<const WgEvent::KeyRepeat&>(_event)).TranslatedKeyCode();
					break;
				case WG_EVENT_KEY_RELEASE:
					key = (static_cast<const WgEvent::KeyRelease&>(_event)).TranslatedKeyCode();
					break;
				default:
					assert(false);		// We should never get here.
					return false;
			}

			if( key == filter.m_data1 )
				return true;
		}
	}
	return false;
}

bool WgEventFilter::_filterNativeKeyEvents( const WgEvent::Event& _event, const WgEventFilter& filter )
{
	if( _event.Type() == filter.EventType() && (!filter.Gizmo() || filter.Gizmo() == _event.Gizmo()) )
	{
		int key;

		switch(_event.Type() )
		{
			case WG_EVENT_KEY_PRESS:
				key = (static_cast<const WgEvent::KeyPress&>(_event)).NativeKeyCode();
				break;
			case WG_EVENT_KEY_REPEAT:
				key = (static_cast<const WgEvent::KeyRepeat&>(_event)).NativeKeyCode();
				break;
			case WG_EVENT_KEY_RELEASE:
				key = (static_cast<const WgEvent::KeyRelease&>(_event)).NativeKeyCode();
				break;
			default:
				assert(false);		// We should never get here.
				return false;
		}

		if( key == filter.m_data1 )
			return true;
	}
	return false;
}

bool WgEventFilter::_filterCharacterEvents( const WgEvent::Event& _event, const WgEventFilter& filter )
{
	if( _event.Type() == filter.EventType() && (!filter.Gizmo() || filter.Gizmo() == _event.Gizmo()) )
	{
		int chr = (static_cast<const WgEvent::Character&>(_event)).Char();

		if( chr == filter.m_data1 )
			return true;
	}
	return false;
}

bool WgEventFilter::_filterWheelRollEvents( const WgEvent::Event& _event, const WgEventFilter& filter )
{
	if( _event.Type() == filter.EventType() && (!filter.Gizmo() || filter.Gizmo() == _event.Gizmo()) )
	{
		int chr = (static_cast<const WgEvent::WheelRoll&>(_event)).Wheel();

		if( chr == filter.m_data1 )
			return true;
	}
	return false;
}

bool WgEventFilter::_filterTickEvents( const WgEvent::Event& _event, const WgEventFilter& filter )
{
	if( _event.Type() == filter.EventType() )
		return true;

	return false;
}


