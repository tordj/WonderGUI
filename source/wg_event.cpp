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

namespace WgEvent
{
	//____ PointerMove __________________________________________________________

	PointerMove::PointerMove( const WgCord& pos )
	{
		m_id = WG_EVENT_POINTER_MOVE;
		m_param[0].integer = pos.x;
		m_param[1].integer = pos.y;
	}

	WgCord PointerMove::Pos() const
	{
		return WgCord( m_param[0].integer, m_param[1].integer );
	}

	//____ ButtonPress ______________________________________________________

	ButtonPress::ButtonPress( int button )
	{
		m_id = WG_EVENT_BUTTON_PRESS;
		m_param[0].integer = button;
	}

	int ButtonPress::Button() const
	{
		return m_param[0].integer;
	}

	WgCord ButtonPress::PointerPos() const
	{
		return WgCord( m_param[1].integer, m_param[2].integer );
	}

	//____ ButtonRelease _____________________________________________________

	ButtonRelease::ButtonRelease( int button )
	{
		m_id = WG_EVENT_BUTTON_RELEASE;
		m_param[0].integer = button;
	}

	int ButtonRelease::Button() const
	{
		return m_param[0].integer;
	}

	WgCord ButtonRelease::PointerPos() const
	{
		return WgCord( m_param[1].integer, m_param[2].integer );
	}


	//____ ButtonClick _____________________________________________________

	ButtonClick::ButtonClick( int button )
	{
		m_id = WG_EVENT_BUTTON_CLICK;
		m_param[0].integer = button;
	}

	int ButtonClick::Button() const
	{
		return m_param[0].integer;
	}

	WgCord ButtonClick::PointerPos() const
	{
		return WgCord( m_param[1].integer, m_param[2].integer );
	}

	//____ ButtonDoubleClick _____________________________________________________

	ButtonDoubleClick::ButtonDoubleClick( int button )
	{
		m_id = WG_EVENT_BUTTON_DOUBLECLICK;
		m_param[0].integer = button;
	}

	int ButtonDoubleClick::Button() const
	{
		return m_param[0].integer;
	}

	WgCord ButtonDoubleClick::PointerPos() const
	{
		return WgCord( m_param[1].integer, m_param[2].integer );
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

	WgCord KeyPress::PointerPos() const
	{
		return WgCord( m_param[2].integer, m_param[3].integer );
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

	WgCord KeyRelease::PointerPos() const
	{
		return WgCord( m_param[2].integer, m_param[3].integer );
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

	WgCord WheelRoll::PointerPos() const
	{
		return WgCord( m_param[2].integer, m_param[3].integer );
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

	//____ EndPointerMove _______________________________________________________

	EndPointerMove::EndPointerMove( const WgCord& pos )
	{
		m_id = WG_EVENT_END_POINTER_MOVE;
		m_param[0].integer = pos.x;
		m_param[1].integer = pos.y;
	}

	WgCord EndPointerMove::Pos() const
	{
		return WgCord( m_param[0].integer, m_param[1].integer );
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


	WgCord ButtonDrag::DraggedSinceStart() const
	{
		return WgCord( m_param[2].short1, m_param[2].short2 );
	}

	WgCord ButtonDrag::DraggedSinceLast() const
	{
		return WgCord( m_param[3].short1, m_param[3].short2 );
	}

	WgCord ButtonDrag::PointerPos() const
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

	//____ GizmoPress() _______________________________________________________

	GizmoPress::GizmoPress( WgGizmo * pGizmo, int button, const WgCord& screenPos, const WgCord& pos )
	{
		m_id		= WG_EVENT_GIZMO_PRESS;
		m_pGizmo 	= pGizmo;

		m_param[0].integer = button;

		m_param[1].short1 = screenPos.x;
		m_param[1].short2 = screenPos.y;

		m_param[2].short1 = pos.x;
		m_param[2].short2 = pos.y;
	}

	int GizmoPress::Button() const
	{
		return m_param[0].integer;
	}

	WgCord GizmoPress::PointerOfs() const
	{
		return WgCord( m_param[2].short1, m_param[2].short2 );
	}

	WgCord GizmoPress::PointerScreenPos() const
	{
		return WgCord( m_param[1].short1, m_param[1].short2 );
	}

	//____ GizmoRelease() _______________________________________________________

	GizmoRelease::GizmoRelease( WgGizmo * pGizmo, int button, const WgCord& screenPos, const WgCord& ofs, bool bWasPressed, bool bIsOutside )
	{
		m_id		= WG_EVENT_GIZMO_PRESS;
		m_pGizmo 	= pGizmo;

		m_param[0].integer = button;

		m_param[1].short1 = screenPos.x;
		m_param[1].short2 = screenPos.y;

		m_param[2].short1 = ofs.x;
		m_param[2].short2 = ofs.y;

		m_param[3].byte1 = (char) bWasPressed;
		m_param[3].byte2 = (char) bIsOutside;
	}

	int GizmoRelease::Button() const
	{
		return m_param[0].integer;
	}

	WgCord GizmoRelease::PointerOfs() const
	{
		return WgCord( m_param[2].short1, m_param[2].short2 );
	}

	WgCord GizmoRelease::PointerScreenPos() const
	{
		return WgCord( m_param[1].short1, m_param[1].short2 );
	}

	bool GizmoRelease::WasPressed() const
	{
		return (bool) m_param[3].byte1;
	}

	bool GizmoRelease::IsOutside() const
	{
		return (bool) m_param[3].byte2;
	}

};
