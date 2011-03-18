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

	//____ PointerEnter _______________________________________________________

	PointerEnter::PointerEnter( const WgCord& pos )
	{
		m_id = WG_EVENT_POINTER_ENTER;

		m_pointerLocalPos = pos; 
		m_pointerScreenPos = pos; 
	}

	PointerEnter::PointerEnter( WgGizmo * pGizmo )
	{
		m_id = WG_EVENT_POINTER_ENTER;
		m_bIsForGizmo	= true;
		m_pGizmo 		= pGizmo;
	}

	//____ PointerExit ________________________________________________________

	PointerExit::PointerExit()
	{
		m_id = WG_EVENT_POINTER_EXIT;
	}

	PointerExit::PointerExit( WgGizmo * pGizmo )
	{
		m_id = WG_EVENT_POINTER_EXIT;
		m_bIsForGizmo	= true;
		m_pGizmo 		= pGizmo;
	}

	//____ PointerMove __________________________________________________________

	PointerMove::PointerMove( const WgCord& pos )
	{
		m_id = WG_EVENT_POINTER_MOVE;

		m_pointerLocalPos = pos;
		m_pointerScreenPos = pos;
	}

	PointerMove::PointerMove( WgGizmo * pGizmo )
	{
		m_id = WG_EVENT_POINTER_MOVE;
		m_bIsForGizmo	= true;
		m_pGizmo 		= pGizmo;
	}

	//____ ButtonPress ______________________________________________________

	ButtonPress::ButtonPress( int button )
	{
		m_id = WG_EVENT_BUTTON_PRESS;
		m_param[0].integer = button;
	}

	ButtonPress::ButtonPress( int button, WgGizmo * pGizmo )
	{
		m_id			= WG_EVENT_BUTTON_PRESS;
		m_bIsForGizmo	= true;
		m_pGizmo 		= pGizmo;

		m_param[0].integer = button;
	}


	int ButtonPress::Button() const
	{
		return m_param[0].integer;
	}

	//____ ButtonRepeat ______________________________________________________

	ButtonRepeat::ButtonRepeat( int button )
	{
		m_id = WG_EVENT_BUTTON_REPEAT;
		m_param[0].integer = button;
	}

	ButtonRepeat::ButtonRepeat( int button, WgGizmo * pGizmo )
	{
		m_id			= WG_EVENT_BUTTON_REPEAT;
		m_bIsForGizmo	= true;
		m_pGizmo 		= pGizmo;

		m_param[0].integer = button;
	}


	int ButtonRepeat::Button() const
	{
		return m_param[0].integer;
	}


	//____ ButtonRelease _____________________________________________________

	ButtonRelease::ButtonRelease( int button )
	{
		m_id = WG_EVENT_BUTTON_RELEASE;
		m_param[0].integer = button;

		m_param[1].boolean = true;			// Always assumed to have had the press inside our window.
		m_param[2].boolean = true;			// Always assumed to be inside our window now.
	}

	ButtonRelease::ButtonRelease( int button, WgGizmo * pGizmo, bool bPressInside, bool bReleaseInside )
	{
		m_id			= WG_EVENT_BUTTON_RELEASE;
		m_bIsForGizmo	= true;
		m_pGizmo 		= pGizmo;

		m_param[0].integer = button;
		m_param[1].boolean = bPressInside;
		m_param[2].boolean = bReleaseInside;
	}


	int ButtonRelease::Button() const
	{
		return m_param[0].integer;
	}

	bool ButtonRelease::PressInside() const
	{
		return m_param[1].boolean;
	}

	bool ButtonRelease::ReleaseInside() const
	{
		return m_param[2].boolean;
	}



	//____ ButtonClick _____________________________________________________

	ButtonClick::ButtonClick( int button )
	{
		m_id = WG_EVENT_BUTTON_CLICK;
		m_param[0].integer = button;
	}

	ButtonClick::ButtonClick( int button, WgGizmo * pGizmo )
	{
		m_id = WG_EVENT_BUTTON_CLICK;
		m_bIsForGizmo	= true;
		m_pGizmo 		= pGizmo;

		m_param[0].integer = button;
	}


	int ButtonClick::Button() const
	{
		return m_param[0].integer;
	}

	//____ ButtonDoubleClick _____________________________________________________

	ButtonDoubleClick::ButtonDoubleClick( int button )
	{
		m_id = WG_EVENT_BUTTON_DOUBLECLICK;
		m_param[0].integer = button;
	}

	ButtonDoubleClick::ButtonDoubleClick( int button, WgGizmo * pGizmo )
	{
		m_id = WG_EVENT_BUTTON_CLICK;
		m_bIsForGizmo	= true;
		m_pGizmo 		= pGizmo;

		m_param[0].integer = button;
	}

	int ButtonDoubleClick::Button() const
	{
		return m_param[0].integer;
	}


	//____ KeyPress ___________________________________________________________

	KeyPress::KeyPress( int native_keycode )
	{
		m_id = WG_EVENT_KEY_PRESS;
		m_param[0].integer = native_keycode;
	}

	int KeyPress::NativeKeyCode() const
	{
		return m_param[0].integer;
	}

	int KeyPress::TranslatedKeyCode() const
	{
		return m_param[1].integer;
	}

	//____ KeyRelease ________________________________________________________

	KeyRelease::KeyRelease( int native_keycode )
	{
		m_id = WG_EVENT_KEY_RELEASE;
		m_param[0].integer = native_keycode;
	}

	int KeyRelease::NativeKeyCode() const
	{
		return m_param[0].integer;
	}

	int KeyRelease::TranslatedKeyCode() const
	{
		return m_param[1].integer;
	}

	//____ Character __________________________________________________________

	Character::Character( unsigned short character )
	{
		m_id = WG_EVENT_CHARACTER;
		m_param[0].integer = character;
	}

	unsigned short Character::Char() const
	{
		return (unsigned short) m_param[0].integer;
	}

	//____ WheelRoll __________________________________________________________

	WheelRoll::WheelRoll( int wheel, int distance )
	{
		m_id = WG_EVENT_WHEEL_ROLL;
		m_param[0].integer = wheel;
		m_param[1].integer = distance;
	}

	int WheelRoll::Wheel() const
	{
		return m_param[0].integer;
	}

	int WheelRoll::Distance() const
	{
		return m_param[1].integer;
	}

	//____ TimePass ___________________________________________________________

	TimePass::TimePass( int ms )
	{
		m_id = WG_EVENT_TIME_PASS;
		m_param[0].integer = ms;
	}

	int TimePass::Millisec() const
	{
		return m_param[0].integer;
	}

	//____ PointerPlaced _______________________________________________________

	PointerPlaced::PointerPlaced()
	{
		m_id = WG_EVENT_POINTER_PLACED;
	}

	//____ ButtonDrag _________________________________________________________

	ButtonDrag::ButtonDrag( int button, const WgCord& orgPos, const WgCord& prevPos, const WgCord& currPos )
	{

		m_id = WG_EVENT_BUTTON_DRAG;
		m_param[0].integer = button;

		m_param[1].short1 = orgPos.x;
		m_param[1].short2 = orgPos.y;

		m_param[2].short1 = currPos.x - orgPos.x;
		m_param[2].short2 = currPos.y - orgPos.y;

		m_param[3].short1 = currPos.x - prevPos.x;
		m_param[3].short2 = currPos.y - prevPos.y;
	}

	ButtonDrag::ButtonDrag( int button, WgGizmo * pGizmo, const WgCord& orgPos, const WgCord& prevPos, const WgCord& currPos )
	{
		m_id = WG_EVENT_BUTTON_DRAG;
		m_bIsForGizmo		= true;
		m_pGizmo 			= pGizmo;
		m_param[0].integer	= button;

		m_param[1].short1 = orgPos.x;
		m_param[1].short2 = orgPos.y;

		m_param[2].short1 = currPos.x - orgPos.x;
		m_param[2].short2 = currPos.y - orgPos.y;

		m_param[3].short1 = currPos.x - prevPos.x;
		m_param[3].short2 = currPos.y - prevPos.y;
	}



	WgCord ButtonDrag::DraggedSinceStart() const
	{
		return WgCord( m_param[2].short1, m_param[2].short2 );
	}

	WgCord ButtonDrag::DraggedSinceLast() const
	{
		return WgCord( m_param[3].short1, m_param[3].short2 );
	}

	WgCord ButtonDrag::CurrPos() const
	{
		return WgCord( m_param[1].short1 + m_param[2].short1, m_param[1].short2 + m_param[2].short2 );
	}

	WgCord ButtonDrag::StartPos() const
	{
		return WgCord( m_param[1].short1, m_param[1].short2 );
	}

	WgCord ButtonDrag::PrevPos() const
	{
		return WgCord( m_param[1].short1 + m_param[2].short1 - m_param[3].short1, m_param[1].short2 + m_param[2].short2 - m_param[3].short2 );
	}

};
