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

//____ MouseEnter() _________________________________________________________

WgEventFilter	WgEventFilter::MouseEnter()
{
	return WgEventFilter( WG_EVENT_MOUSE_ENTER, 0, _filterPointerEvents );
}

WgEventFilter	WgEventFilter::MouseEnter( WgGizmo * pGizmo )
{
	return WgEventFilter( WG_EVENT_MOUSE_ENTER, pGizmo, _filterPointerEvents );
}

//____ MouseLeave() _________________________________________________________

WgEventFilter	WgEventFilter::MouseLeave()
{
	return WgEventFilter( WG_EVENT_MOUSE_LEAVE, 0, _filterPointerEvents );
}

WgEventFilter	WgEventFilter::MouseLeave( WgGizmo * pGizmo )
{
	return WgEventFilter( WG_EVENT_MOUSE_LEAVE, pGizmo, _filterPointerEvents );
}

//____ MouseMove() _________________________________________________________

WgEventFilter	WgEventFilter::MouseMove()
{
	return WgEventFilter( WG_EVENT_MOUSE_MOVE, 0, _filterPointerEvents );
}

WgEventFilter	WgEventFilter::MouseMove( WgGizmo * pGizmo )
{
	return WgEventFilter( WG_EVENT_MOUSE_MOVE, pGizmo, _filterPointerEvents );
}

//____ MouseButtonPress() _________________________________________________________

WgEventFilter	WgEventFilter::MouseButtonPress()
{
	return WgEventFilter( WG_EVENT_MOUSEBUTTON_PRESS, 0, _filterMouseButtonEvents, 0 );
}

WgEventFilter	WgEventFilter::MouseButtonPress( int button)
{
	return WgEventFilter( WG_EVENT_MOUSEBUTTON_PRESS, 0, _filterMouseButtonEvents, button );
}

WgEventFilter	WgEventFilter::MouseButtonPress( WgGizmo * pGizmo )
{
	return WgEventFilter( WG_EVENT_MOUSEBUTTON_PRESS, pGizmo, _filterMouseButtonEvents, 0 );
}

WgEventFilter	WgEventFilter::MouseButtonPress( WgGizmo * pGizmo, int button )
{
	return WgEventFilter( WG_EVENT_MOUSEBUTTON_PRESS, pGizmo, _filterMouseButtonEvents, button );
}

//____ MouseButtonRelease() ________________________________________________________

WgEventFilter	WgEventFilter::MouseButtonRelease()
{
	return WgEventFilter( WG_EVENT_MOUSEBUTTON_RELEASE, 0, _filterMouseButtonEvents, 0 );
}

WgEventFilter	WgEventFilter::MouseButtonRelease( int button)
{
	return WgEventFilter( WG_EVENT_MOUSEBUTTON_RELEASE, 0, _filterMouseButtonEvents, button );
}

WgEventFilter	WgEventFilter::MouseButtonRelease( WgGizmo * pGizmo )
{
	return WgEventFilter( WG_EVENT_MOUSEBUTTON_RELEASE, pGizmo, _filterMouseButtonEvents, 0 );
}

WgEventFilter	WgEventFilter::MouseButtonRelease( WgGizmo * pGizmo, int button )
{
	return WgEventFilter( WG_EVENT_MOUSEBUTTON_RELEASE, pGizmo, _filterMouseButtonEvents, button );
}

//____ MouseButtonDrag() ___________________________________________________________

WgEventFilter	WgEventFilter::MouseButtonDrag()
{
	return WgEventFilter( WG_EVENT_MOUSEBUTTON_DRAG, 0, _filterMouseButtonEvents, 0 );
}

WgEventFilter	WgEventFilter::MouseButtonDrag( int button)
{
	return WgEventFilter( WG_EVENT_MOUSEBUTTON_DRAG, 0, _filterMouseButtonEvents, button );
}

WgEventFilter	WgEventFilter::MouseButtonDrag( WgGizmo * pGizmo )
{
	return WgEventFilter( WG_EVENT_MOUSEBUTTON_DRAG, pGizmo, _filterMouseButtonEvents, 0 );
}

WgEventFilter	WgEventFilter::MouseButtonDrag( WgGizmo * pGizmo, int button )
{
	return WgEventFilter( WG_EVENT_MOUSEBUTTON_DRAG, pGizmo, _filterMouseButtonEvents, button );
}

//____ MouseButtonRepeat() ________________________________________________________

WgEventFilter	WgEventFilter::MouseButtonRepeat()
{
	return WgEventFilter( WG_EVENT_MOUSEBUTTON_REPEAT, 0, _filterMouseButtonEvents, 0 );
}

WgEventFilter	WgEventFilter::MouseButtonRepeat( int button)
{
	return WgEventFilter( WG_EVENT_MOUSEBUTTON_REPEAT, 0, _filterMouseButtonEvents, button );
}

WgEventFilter	WgEventFilter::MouseButtonRepeat( WgGizmo * pGizmo )
{
	return WgEventFilter( WG_EVENT_MOUSEBUTTON_REPEAT, pGizmo, _filterMouseButtonEvents, 0 );
}

WgEventFilter	WgEventFilter::MouseButtonRepeat( WgGizmo * pGizmo, int button )
{
	return WgEventFilter( WG_EVENT_MOUSEBUTTON_REPEAT, pGizmo, _filterMouseButtonEvents, button );
}

//____ MouseButtonClick() ________________________________________________________

WgEventFilter	WgEventFilter::MouseButtonClick()
{
	return WgEventFilter( WG_EVENT_MOUSEBUTTON_CLICK, 0, _filterMouseButtonEvents, 0 );
}

WgEventFilter	WgEventFilter::MouseButtonClick( int button)
{
	return WgEventFilter( WG_EVENT_MOUSEBUTTON_CLICK, 0, _filterMouseButtonEvents, button );
}

WgEventFilter	WgEventFilter::MouseButtonClick( WgGizmo * pGizmo )
{
	return WgEventFilter( WG_EVENT_MOUSEBUTTON_CLICK, pGizmo, _filterMouseButtonEvents, 0 );
}

WgEventFilter	WgEventFilter::MouseButtonClick( WgGizmo * pGizmo, int button )
{
	return WgEventFilter( WG_EVENT_MOUSEBUTTON_CLICK, pGizmo, _filterMouseButtonEvents, button );
}

//____ MouseButtonDoubleClick() ________________________________________________________

WgEventFilter	WgEventFilter::MouseButtonDoubleClick()
{
	return WgEventFilter( WG_EVENT_MOUSEBUTTON_CLICK, 0, _filterMouseButtonEvents, 0 );
}

WgEventFilter	WgEventFilter::MouseButtonDoubleClick( int button)
{
	return WgEventFilter( WG_EVENT_MOUSEBUTTON_CLICK, 0, _filterMouseButtonEvents, button );
}

WgEventFilter	WgEventFilter::MouseButtonDoubleClick( WgGizmo * pGizmo )
{
	return WgEventFilter( WG_EVENT_MOUSEBUTTON_CLICK, pGizmo, _filterMouseButtonEvents, 0 );
}

WgEventFilter	WgEventFilter::MouseButtonDoubleClick( WgGizmo * pGizmo, int button )
{
	return WgEventFilter( WG_EVENT_MOUSEBUTTON_CLICK, pGizmo, _filterMouseButtonEvents, button );
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

//____ MouseWheelRoll() ____________________________________________________________

WgEventFilter	WgEventFilter::MouseWheelRoll()
{
	return WgEventFilter( WG_EVENT_MOUSEWHEEL_ROLL, 0, _filterMouseWheelRollEvents, 0 );
}

WgEventFilter	WgEventFilter::MouseWheelRoll( int wheel )
{
	return WgEventFilter( WG_EVENT_MOUSEWHEEL_ROLL, 0, _filterMouseWheelRollEvents, wheel );
}

WgEventFilter	WgEventFilter::MouseWheelRoll( WgGizmo * pGizmo )
{
	return WgEventFilter( WG_EVENT_MOUSEWHEEL_ROLL, pGizmo, _filterMouseWheelRollEvents, 0 );
}

WgEventFilter	WgEventFilter::MouseWheelRoll( int wheel, WgGizmo * pGizmo )
{
	return WgEventFilter( WG_EVENT_MOUSEWHEEL_ROLL, pGizmo, _filterMouseWheelRollEvents, wheel );
}

//____ Tick() _________________________________________________________________

WgEventFilter	WgEventFilter::Tick()
{
	return WgEventFilter( WG_EVENT_TICK, 0, _filterTickEvents );
}

//_____________________________________________________________________________

bool WgEventFilter::_filterDummy( const WgEvent::Event * pEvent, const WgEventFilter& filter )
{
	return true;
}


bool WgEventFilter::_filterPointerEvents( const WgEvent::Event * pEvent, const WgEventFilter& filter )
{
	if( pEvent->Type() == filter.EventType() && (!filter.Gizmo() || filter.Gizmo() == pEvent->Gizmo()) )
		return true;

	return false;
}

bool WgEventFilter::_filterMouseButtonEvents( const WgEvent::Event * pEvent, const WgEventFilter& filter )
{
	if( pEvent->Type() == filter.EventType() && (!filter.Gizmo() || filter.Gizmo() == pEvent->Gizmo()) )
	{
		if( filter.m_data1 == 0 )
			return true;
		else
		{
			const WgEvent::MouseButtonEvent * p = static_cast<const WgEvent::MouseButtonEvent*>(pEvent);

			if( p->Button() == filter.m_data1 )
				return true;
		}
	}
	return false;
}

bool WgEventFilter::_filterKeyEvents( const WgEvent::Event * pEvent, const WgEventFilter& filter )
{
	if( pEvent->Type() == filter.EventType() && (!filter.Gizmo() || filter.Gizmo() == pEvent->Gizmo()) )
	{
		if( filter.m_data1 == 0 )
			return true;
		else
		{
			const WgEvent::KeyEvent * p = static_cast<const WgEvent::KeyEvent*>(pEvent);

			if( p->TranslatedKeyCode() == filter.m_data1 )
				return true;
		}
	}
	return false;
}

bool WgEventFilter::_filterNativeKeyEvents( const WgEvent::Event * pEvent, const WgEventFilter& filter )
{
	if( pEvent->Type() == filter.EventType() && (!filter.Gizmo() || filter.Gizmo() == pEvent->Gizmo()) )
	{
		const WgEvent::KeyEvent * p = static_cast<const WgEvent::KeyEvent*>(pEvent);

		if( p->NativeKeyCode() == filter.m_data1 )
			return true;
	}
	return false;
}

bool WgEventFilter::_filterCharacterEvents( const WgEvent::Event * pEvent, const WgEventFilter& filter )
{
	if( pEvent->Type() == filter.EventType() && (!filter.Gizmo() || filter.Gizmo() == pEvent->Gizmo()) )
	{
		int chr = (static_cast<const WgEvent::Character *>(pEvent))->Char();

		if( chr == filter.m_data1 )
			return true;
	}
	return false;
}

bool WgEventFilter::_filterMouseWheelRollEvents( const WgEvent::Event * pEvent, const WgEventFilter& filter )
{
	if( pEvent->Type() == filter.EventType() && (!filter.Gizmo() || filter.Gizmo() == pEvent->Gizmo()) )
	{
		int chr = (static_cast<const WgEvent::MouseWheelRoll *>(pEvent))->Wheel();

		if( chr == filter.m_data1 )
			return true;
	}
	return false;
}

bool WgEventFilter::_filterTickEvents( const WgEvent::Event * pEvent, const WgEventFilter& filter )
{
	if( pEvent->Type() == filter.EventType() )
		return true;

	return false;
}


