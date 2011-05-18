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
	return WgEventFilter( WG_EVENT_BUTTON_PRESS, 0, _filterButtonEvents, -1 );
}

WgEventFilter	WgEventFilter::ButtonPress( int button)
{
	return WgEventFilter( WG_EVENT_BUTTON_PRESS, 0, _filterButtonEvents, button );
}

WgEventFilter	WgEventFilter::ButtonPress( WgGizmo * pGizmo )
{
	return WgEventFilter( WG_EVENT_BUTTON_PRESS, pGizmo, _filterButtonEvents, -1 );
}

WgEventFilter	WgEventFilter::ButtonPress( WgGizmo * pGizmo, int button )
{
	return WgEventFilter( WG_EVENT_BUTTON_PRESS, pGizmo, _filterButtonEvents, button );
}

//____ ButtonRelease() ________________________________________________________

WgEventFilter	WgEventFilter::ButtonRelease()
{
	return WgEventFilter( WG_EVENT_BUTTON_RELEASE, 0, _filterButtonEvents, -1 );
}

WgEventFilter	WgEventFilter::ButtonRelease( int button)
{
	return WgEventFilter( WG_EVENT_BUTTON_RELEASE, 0, _filterButtonEvents, button );
}

WgEventFilter	WgEventFilter::ButtonRelease( WgGizmo * pGizmo )
{
	return WgEventFilter( WG_EVENT_BUTTON_RELEASE, pGizmo, _filterButtonEvents, -1 );
}

WgEventFilter	WgEventFilter::ButtonRelease( WgGizmo * pGizmo, int button )
{
	return WgEventFilter( WG_EVENT_BUTTON_RELEASE, pGizmo, _filterButtonEvents, button );
}

//____ ButtonDrag() ___________________________________________________________

WgEventFilter	WgEventFilter::ButtonDrag()
{
	return WgEventFilter( WG_EVENT_BUTTON_DRAG, 0, _filterButtonEvents, -1 );
}

WgEventFilter	WgEventFilter::ButtonDrag( int button)
{
	return WgEventFilter( WG_EVENT_BUTTON_DRAG, 0, _filterButtonEvents, button );
}

WgEventFilter	WgEventFilter::ButtonDrag( WgGizmo * pGizmo )
{
	return WgEventFilter( WG_EVENT_BUTTON_DRAG, pGizmo, _filterButtonEvents, -1 );
}

WgEventFilter	WgEventFilter::ButtonDrag( WgGizmo * pGizmo, int button )
{
	return WgEventFilter( WG_EVENT_BUTTON_DRAG, pGizmo, _filterButtonEvents, button );
}

//____ ButtonRepeat() ________________________________________________________

WgEventFilter	WgEventFilter::ButtonRepeat()
{
	return WgEventFilter( WG_EVENT_BUTTON_REPEAT, 0, _filterButtonEvents, -1 );
}

WgEventFilter	WgEventFilter::ButtonRepeat( int button)
{
	return WgEventFilter( WG_EVENT_BUTTON_REPEAT, 0, _filterButtonEvents, button );
}

WgEventFilter	WgEventFilter::ButtonRepeat( WgGizmo * pGizmo )
{
	return WgEventFilter( WG_EVENT_BUTTON_REPEAT, pGizmo, _filterButtonEvents, -1 );
}

WgEventFilter	WgEventFilter::ButtonRepeat( WgGizmo * pGizmo, int button )
{
	return WgEventFilter( WG_EVENT_BUTTON_REPEAT, pGizmo, _filterButtonEvents, button );
}

//____ ButtonClick() ________________________________________________________

WgEventFilter	WgEventFilter::ButtonClick()
{
	return WgEventFilter( WG_EVENT_BUTTON_CLICK, 0, _filterButtonEvents, -1 );
}

WgEventFilter	WgEventFilter::ButtonClick( int button)
{
	return WgEventFilter( WG_EVENT_BUTTON_CLICK, 0, _filterButtonEvents, button );
}

WgEventFilter	WgEventFilter::ButtonClick( WgGizmo * pGizmo )
{
	return WgEventFilter( WG_EVENT_BUTTON_CLICK, pGizmo, _filterButtonEvents, -1 );
}

WgEventFilter	WgEventFilter::ButtonClick( WgGizmo * pGizmo, int button )
{
	return WgEventFilter( WG_EVENT_BUTTON_CLICK, pGizmo, _filterButtonEvents, button );
}

//____ ButtonDoubleClick() ________________________________________________________

WgEventFilter	WgEventFilter::ButtonDoubleClick()
{
	return WgEventFilter( WG_EVENT_BUTTON_CLICK, 0, _filterButtonEvents, -1 );
}

WgEventFilter	WgEventFilter::ButtonDoubleClick( int button)
{
	return WgEventFilter( WG_EVENT_BUTTON_CLICK, 0, _filterButtonEvents, button );
}

WgEventFilter	WgEventFilter::ButtonDoubleClick( WgGizmo * pGizmo )
{
	return WgEventFilter( WG_EVENT_BUTTON_CLICK, pGizmo, _filterButtonEvents, -1 );
}

WgEventFilter	WgEventFilter::ButtonDoubleClick( WgGizmo * pGizmo, int button )
{
	return WgEventFilter( WG_EVENT_BUTTON_CLICK, pGizmo, _filterButtonEvents, button );
}

//____ KeyPress() _____________________________________________________________

WgEventFilter	WgEventFilter::KeyPress()
{
}

WgEventFilter	WgEventFilter::KeyPress( WgGizmo * pGizmo )
{
}

WgEventFilter	WgEventFilter::KeyPress( WgKey translated_keycode )
{
}

WgEventFilter	WgEventFilter::KeyPress( WgGizmo * pGizmo, WgKey keycode )
{
}

//____ KeyRelease() ___________________________________________________________

WgEventFilter	WgEventFilter::KeyRelease()
{
}

WgEventFilter	WgEventFilter::KeyRelease( WgGizmo * pGizmo )
{
}

WgEventFilter	WgEventFilter::KeyRelease( WgKey translated_keycode )
{
}

WgEventFilter	WgEventFilter::KeyRelease( WgGizmo * pGizmo, WgKey keycode )
{
}

//____ KeyRepeat() ____________________________________________________________

WgEventFilter	WgEventFilter::KeyRepeat()
{
}

WgEventFilter	WgEventFilter::KeyRepeat( WgGizmo * pGizmo )
{
}

WgEventFilter	WgEventFilter::KeyRepeat( WgKey translated_keycode )
{
}

WgEventFilter	WgEventFilter::KeyRepeat( WgGizmo * pGizmo, WgKey keycode )
{
}

//____ KeyPressNative() _______________________________________________________

WgEventFilter	WgEventFilter::KeyPressNative( int native_keycode )
{
}

WgEventFilter	WgEventFilter::KeyPressNative( WgGizmo * pGizmo, int native_keycode )
{
}

//____ KeyReleaseNative() _____________________________________________________

WgEventFilter	WgEventFilter::KeyReleaseNative( int native_keycode )
{
}

WgEventFilter	WgEventFilter::KeyReleaseNative( WgGizmo * pGizmo, int native_keycode )
{
}

//____ KeyRepeatNative() ______________________________________________________

WgEventFilter	WgEventFilter::KeyRepeatNative( int native_keycode )
{
}

WgEventFilter	WgEventFilter::KeyRepeatNative( WgGizmo * pGizmo, int native_keycode )
{
}

//____ Character() ____________________________________________________________

WgEventFilter	WgEventFilter::Character()
{
}

WgEventFilter	WgEventFilter::Character( unsigned short character)
{
}

WgEventFilter	WgEventFilter::Character( WgGizmo * pGizmo )
{
}

WgEventFilter	WgEventFilter::Character( WgGizmo * pGizmo, unsigned short character )
{
}

//____ WheelRoll() ____________________________________________________________

WgEventFilter	WgEventFilter::WheelRoll()
{
}

WgEventFilter	WgEventFilter::WheelRoll( int wheel )
{
}

WgEventFilter	WgEventFilter::WheelRoll( WgGizmo * pGizmo )
{
}

WgEventFilter	WgEventFilter::WheelRoll( int wheel, WgGizmo * pGizmo )
{
}

//____ Tick() _________________________________________________________________

WgEventFilter	WgEventFilter::Tick()
{
}


bool WgEventFilter::_filterDummy( const WgEvent::Event& _event, const WgEventFilter& filter )
{
	return true;
}


bool WgEventFilter::_filterPointerEvents( const WgEvent::Event& _event, const WgEventFilter& filter )
{
	if( _event.Id() == filter.EventType() && (!filter.Gizmo() || filter.Gizmo() == _event.Gizmo()) )
		return true;

	return false;
}

bool WgEventFilter::_filterButtonEvents( const WgEvent::Event& _event, const WgEventFilter& filter )
{
	if( _event.Id() == filter.EventType() && (!filter.Gizmo() || filter.Gizmo() == _event.Gizmo()) )&&
		(filter.m_data1 == -1 || filter.m_data1 == static_cast<WgEvent::Bu )
		return true;

	return false;
}}

bool WgEventFilter::_filterKeyEvents( const WgEvent::Event& _event, const WgEventFilter& filter )
{
}

bool WgEventFilter::_filterNativeKeyEvents( const WgEvent::Event& _event, const WgEventFilter& filter )
{
}

bool WgEventFilter::_filterCharacterEvents( const WgEvent::Event& _event, const WgEventFilter& filter )
{
}

bool WgEventFilter::_filterWheelRollEvents( const WgEvent::Event& _event, const WgEventFilter& filter )
{
}

bool WgEventFilter::_filterTickEvents( const WgEvent::Event& _event, const WgEventFilter& filter )
{
}


