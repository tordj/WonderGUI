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
#include <wg_gizmo.h>

namespace WgEvent
{
	//____ Event ______________________________________________________________

	WgGizmo * Event::Gizmo() const
	{
		return m_pGizmo.GetRealPtr();
	}

	//____ FocusGained ________________________________________________________

	FocusGained::FocusGained()
	{
		m_type = WG_EVENT_FOCUS_GAINED;
	}

	//____ FocusLost __________________________________________________________

	FocusLost::FocusLost()
	{
		m_type = WG_EVENT_FOCUS_LOST;
	}

	//____ PointerEnter _______________________________________________________

	PointerEnter::PointerEnter( const WgCord& pos )
	{
		m_type = WG_EVENT_POINTER_ENTER;

		m_pointerLocalPos = pos;
		m_pointerScreenPos = pos;
	}

	PointerEnter::PointerEnter( WgGizmo * pGizmo )
	{
		m_type = WG_EVENT_POINTER_ENTER;
		m_bIsForGizmo	= true;
		m_pGizmo 		= pGizmo;
	}

	//____ PointerExit ________________________________________________________

	PointerExit::PointerExit()
	{
		m_type = WG_EVENT_POINTER_EXIT;
	}

	PointerExit::PointerExit( WgGizmo * pGizmo )
	{
		m_type = WG_EVENT_POINTER_EXIT;
		m_bIsForGizmo	= true;
		m_pGizmo 		= pGizmo;
	}

	//____ PointerMove __________________________________________________________

	PointerMove::PointerMove( const WgCord& pos )
	{
		m_type = WG_EVENT_POINTER_MOVE;

		m_pointerLocalPos = pos;
		m_pointerScreenPos = pos;
	}

	PointerMove::PointerMove( WgGizmo * pGizmo )
	{
		m_type = WG_EVENT_POINTER_MOVE;
		m_bIsForGizmo	= true;
		m_pGizmo 		= pGizmo;
	}

	//____ ButtonPress ______________________________________________________

	ButtonPress::ButtonPress( int button ) : ButtonEvent(button)
	{
		m_type = WG_EVENT_BUTTON_PRESS;
	}

	ButtonPress::ButtonPress( int button, WgGizmo * pGizmo ) : ButtonEvent(button)
	{
		m_type			= WG_EVENT_BUTTON_PRESS;
		m_bIsForGizmo	= true;
		m_pGizmo 		= pGizmo;
	}

	//____ ButtonRepeat ______________________________________________________

	ButtonRepeat::ButtonRepeat( int button ) : ButtonEvent(button)
	{
		m_type = WG_EVENT_BUTTON_REPEAT;
	}

	ButtonRepeat::ButtonRepeat( int button, WgGizmo * pGizmo ) : ButtonEvent(button)
	{
		m_type			= WG_EVENT_BUTTON_REPEAT;
		m_bIsForGizmo	= true;
		m_pGizmo 		= pGizmo;
	}

	//____ ButtonRelease _____________________________________________________

	ButtonRelease::ButtonRelease( int button ) : ButtonEvent(button)
	{
		m_type = WG_EVENT_BUTTON_RELEASE;

		m_bPressInside = true;			// Always assumed to have had the press inside our window.
		m_bReleaseInside = true;			// Always assumed to be inside our window now.
	}

	ButtonRelease::ButtonRelease( int button, WgGizmo * pGizmo, bool bPressInside, bool bReleaseInside ) : ButtonEvent(button)
	{
		m_type			= WG_EVENT_BUTTON_RELEASE;
		m_bIsForGizmo	= true;
		m_pGizmo 		= pGizmo;

		m_bPressInside = bPressInside;
		m_bReleaseInside = bReleaseInside;
	}

	bool ButtonRelease::PressInside() const
	{
		return m_bPressInside;
	}

	bool ButtonRelease::ReleaseInside() const
	{
		return m_bReleaseInside;
	}



	//____ ButtonClick _____________________________________________________

	ButtonClick::ButtonClick( int button ) : ButtonEvent(button)
	{
		m_type = WG_EVENT_BUTTON_CLICK;
	}

	ButtonClick::ButtonClick( int button, WgGizmo * pGizmo ) : ButtonEvent(button)
	{
		m_type = WG_EVENT_BUTTON_CLICK;
		m_bIsForGizmo	= true;
		m_pGizmo 		= pGizmo;
	}


	//____ ButtonDoubleClick _____________________________________________________

	ButtonDoubleClick::ButtonDoubleClick( int button ) : ButtonEvent(button)
	{
		m_type = WG_EVENT_BUTTON_DOUBLECLICK;
	}

	ButtonDoubleClick::ButtonDoubleClick( int button, WgGizmo * pGizmo ) : ButtonEvent(button)
	{
		m_type = WG_EVENT_BUTTON_DOUBLECLICK;
		m_bIsForGizmo	= true;
		m_pGizmo 		= pGizmo;
	}

	//____ PointerMoveOutsideModal ___________________________________________________

	PointerMoveOutsideModal::PointerMoveOutsideModal( WgGizmo * pGizmo )
	{
		m_type = WG_EVENT_POINTER_MOVE_OUTSIDE_MODAL;
		m_bIsForGizmo	= true;
		m_pGizmo		= pGizmo;
	}

	//____ ButtonPressOutsideModal ______________________________________________________

	ButtonPressOutsideModal::ButtonPressOutsideModal( int button, WgGizmo * pGizmo ) : ButtonEvent(button)
	{
		m_type			= WG_EVENT_BUTTON_PRESS_OUTSIDE_MODAL;
		m_bIsForGizmo	= true;
		m_pGizmo 		= pGizmo;
	}

	//____ ButtonReleaseOutsideModal _____________________________________________________

	ButtonReleaseOutsideModal::ButtonReleaseOutsideModal( int button, WgGizmo * pGizmo ) : ButtonEvent(button)
	{
		m_type			= WG_EVENT_BUTTON_RELEASE_OUTSIDE_MODAL;
		m_bIsForGizmo	= true;
		m_pGizmo 		= pGizmo;
	}

	//____ KeyPress ___________________________________________________________

	KeyPress::KeyPress( int native_keycode ) : KeyEvent(native_keycode)
	{
		m_type = WG_EVENT_KEY_PRESS;
	}

	//____ KeyRepeat ___________________________________________________________

	KeyRepeat::KeyRepeat( int native_keycode ) : KeyEvent(native_keycode)
	{
		m_type = WG_EVENT_KEY_REPEAT;
	}

	//____ KeyRelease ________________________________________________________

	KeyRelease::KeyRelease( int native_keycode ) : KeyEvent(native_keycode)
	{
		m_type = WG_EVENT_KEY_RELEASE;
	}

	//____ Character __________________________________________________________

	Character::Character( unsigned short character )
	{
		m_type = WG_EVENT_CHARACTER;
		m_char = character;
	}

	unsigned short Character::Char() const
	{
		return m_char;
	}

	//____ WheelRoll __________________________________________________________

	WheelRoll::WheelRoll( int wheel, int distance )
	{
		m_type = WG_EVENT_WHEEL_ROLL;
		m_wheel = wheel;
		m_distance = distance;
	}

	int WheelRoll::Wheel() const
	{
		return m_wheel;
	}

	int WheelRoll::Distance() const
	{
		return m_distance;
	}

	//____ Tick _______________________________________________________________

	Tick::Tick( int ms )
	{
		m_type = WG_EVENT_TICK;
		m_millisec = ms;
	}

	int Tick::Millisec() const
	{
		return m_millisec;
	}

	//____ PointerPlaced _______________________________________________________

	PointerPlaced::PointerPlaced()
	{
		m_type = WG_EVENT_POINTER_PLACED;
	}

	//____ ButtonDrag _________________________________________________________

	ButtonDrag::ButtonDrag( int button, const WgCord& startPos, const WgCord& prevPos, const WgCord& currPos ) : ButtonEvent(button)
	{

		m_type = WG_EVENT_BUTTON_DRAG;

		m_startPos = startPos;
		m_prevPos = prevPos;
		m_currPos = currPos;
	}

	ButtonDrag::ButtonDrag( int button, WgGizmo * pGizmo, const WgCord& startPos, const WgCord& prevPos, const WgCord& currPos ) : ButtonEvent(button)
	{
		m_type = WG_EVENT_BUTTON_DRAG;
		m_bIsForGizmo		= true;
		m_pGizmo 			= pGizmo;

		m_startPos = startPos;
		m_prevPos = prevPos;
		m_currPos = currPos;
	}

	WgCord ButtonDrag::DraggedTotal() const
	{
		return m_currPos - m_startPos;
	}

	WgCord ButtonDrag::DraggedNow() const
	{
		return m_currPos - m_prevPos;
	}

	WgCord ButtonDrag::CurrPos() const
	{
		return m_currPos;
	}

	WgCord ButtonDrag::StartPos() const
	{
		return m_startPos;
	}

	WgCord ButtonDrag::PrevPos() const
	{
		return m_prevPos;
	}

};
