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
#include <wg_gizmo_button.h>
#include <wg_gizmo_checkbox.h>
#include <wg_gizmo_radiobutton.h>

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

	//____ MouseEnter _______________________________________________________

	MouseEnter::MouseEnter( const WgCoord& pos )
	{
		m_type = WG_EVENT_MOUSE_ENTER;

		m_pointerLocalPos = pos;
		m_pointerScreenPos = pos;
	}

	MouseEnter::MouseEnter( WgGizmo * pGizmo )
	{
		m_type = WG_EVENT_MOUSE_ENTER;
		m_bIsForGizmo	= true;
		m_pGizmo 		= pGizmo;
	}

	//____ MouseLeave ________________________________________________________

	MouseLeave::MouseLeave()
	{
		m_type = WG_EVENT_MOUSE_LEAVE;
	}

	MouseLeave::MouseLeave( WgGizmo * pGizmo )
	{
		m_type = WG_EVENT_MOUSE_LEAVE;
		m_bIsForGizmo	= true;
		m_pGizmo 		= pGizmo;
	}

	//____ MouseMove __________________________________________________________

	MouseMove::MouseMove( const WgCoord& pos )
	{
		m_type = WG_EVENT_MOUSE_MOVE;

		m_pointerLocalPos = pos;
		m_pointerScreenPos = pos;
	}

	MouseMove::MouseMove( WgGizmo * pGizmo )
	{
		m_type = WG_EVENT_MOUSE_MOVE;
		m_bIsForGizmo	= true;
		m_pGizmo 		= pGizmo;
	}

	//____ MouseButtonPress ______________________________________________________

	MouseButtonPress::MouseButtonPress( int button ) : MouseButtonEvent(button)
	{
		m_type = WG_EVENT_MOUSEBUTTON_PRESS;
	}

	MouseButtonPress::MouseButtonPress( int button, WgGizmo * pGizmo ) : MouseButtonEvent(button)
	{
		m_type			= WG_EVENT_MOUSEBUTTON_PRESS;
		m_bIsForGizmo	= true;
		m_pGizmo 		= pGizmo;
	}

	//____ MouseButtonRepeat ______________________________________________________

	MouseButtonRepeat::MouseButtonRepeat( int button ) : MouseButtonEvent(button)
	{
		m_type = WG_EVENT_MOUSEBUTTON_REPEAT;
	}

	MouseButtonRepeat::MouseButtonRepeat( int button, WgGizmo * pGizmo ) : MouseButtonEvent(button)
	{
		m_type			= WG_EVENT_MOUSEBUTTON_REPEAT;
		m_bIsForGizmo	= true;
		m_pGizmo 		= pGizmo;
	}

	//____ MouseButtonRelease _____________________________________________________

	MouseButtonRelease::MouseButtonRelease( int button ) : MouseButtonEvent(button)
	{
		m_type = WG_EVENT_MOUSEBUTTON_RELEASE;

		m_bPressInside = true;			// Always assumed to have had the press inside our window.
		m_bReleaseInside = true;			// Always assumed to be inside our window now.
	}

	MouseButtonRelease::MouseButtonRelease( int button, WgGizmo * pGizmo, bool bPressInside, bool bReleaseInside ) : MouseButtonEvent(button)
	{
		m_type			= WG_EVENT_MOUSEBUTTON_RELEASE;
		m_bIsForGizmo	= true;
		m_pGizmo 		= pGizmo;

		m_bPressInside = bPressInside;
		m_bReleaseInside = bReleaseInside;
	}

	bool MouseButtonRelease::PressInside() const
	{
		return m_bPressInside;
	}

	bool MouseButtonRelease::ReleaseInside() const
	{
		return m_bReleaseInside;
	}



	//____ MouseButtonClick _____________________________________________________

	MouseButtonClick::MouseButtonClick( int button ) : MouseButtonEvent(button)
	{
		m_type = WG_EVENT_MOUSEBUTTON_CLICK;
	}

	MouseButtonClick::MouseButtonClick( int button, WgGizmo * pGizmo ) : MouseButtonEvent(button)
	{
		m_type = WG_EVENT_MOUSEBUTTON_CLICK;
		m_bIsForGizmo	= true;
		m_pGizmo 		= pGizmo;
	}


	//____ MouseButtonDoubleClick _____________________________________________________

	MouseButtonDoubleClick::MouseButtonDoubleClick( int button ) : MouseButtonEvent(button)
	{
		m_type = WG_EVENT_MOUSEBUTTON_DOUBLECLICK;
	}

	MouseButtonDoubleClick::MouseButtonDoubleClick( int button, WgGizmo * pGizmo ) : MouseButtonEvent(button)
	{
		m_type = WG_EVENT_MOUSEBUTTON_DOUBLECLICK;
		m_bIsForGizmo	= true;
		m_pGizmo 		= pGizmo;
	}

	//____ MouseMoveOutsideModal ___________________________________________________

	MouseMoveOutsideModal::MouseMoveOutsideModal( WgGizmo * pGizmo )
	{
		m_type = WG_EVENT_MOUSE_MOVE_OUTSIDE_MODAL;
		m_bIsForGizmo	= true;
		m_pGizmo		= pGizmo;
	}

	//____ MouseButtonPressOutsideModal ______________________________________________________

	MouseButtonPressOutsideModal::MouseButtonPressOutsideModal( int button, WgGizmo * pGizmo ) : MouseButtonEvent(button)
	{
		m_type			= WG_EVENT_MOUSEBUTTON_PRESS_OUTSIDE_MODAL;
		m_bIsForGizmo	= true;
		m_pGizmo 		= pGizmo;
	}

	//____ MouseButtonReleaseOutsideModal _____________________________________________________

	MouseButtonReleaseOutsideModal::MouseButtonReleaseOutsideModal( int button, WgGizmo * pGizmo ) : MouseButtonEvent(button)
	{
		m_type			= WG_EVENT_MOUSEBUTTON_RELEASE_OUTSIDE_MODAL;
		m_bIsForGizmo	= true;
		m_pGizmo 		= pGizmo;
	}

	//____ KeyPress ___________________________________________________________

	KeyPress::KeyPress( int native_keycode ) : KeyEvent(native_keycode)
	{
		m_type 			= WG_EVENT_KEY_PRESS;
	}

	KeyPress::KeyPress( int native_keycode, WgGizmo * pGizmo ) : KeyEvent(native_keycode)
	{
		m_type 			= WG_EVENT_KEY_PRESS;
		m_bIsForGizmo	= true;
		m_pGizmo		= pGizmo;
	}

	//____ KeyRepeat ___________________________________________________________

	KeyRepeat::KeyRepeat( int native_keycode ) : KeyEvent(native_keycode)
	{
		m_type 			= WG_EVENT_KEY_REPEAT;
	}

	KeyRepeat::KeyRepeat( int native_keycode, WgGizmo * pGizmo ) : KeyEvent(native_keycode)
	{
		m_type 			= WG_EVENT_KEY_REPEAT;
		m_bIsForGizmo	= true;
		m_pGizmo 		= pGizmo;
	}

	//____ KeyRelease ________________________________________________________

	KeyRelease::KeyRelease( int native_keycode ) : KeyEvent(native_keycode)
	{
		m_type 		= WG_EVENT_KEY_RELEASE;
	}

	KeyRelease::KeyRelease( int native_keycode, WgGizmo * pGizmo ) : KeyEvent(native_keycode)
	{
		m_type 			= WG_EVENT_KEY_RELEASE;
		m_bIsForGizmo	= true;
		m_pGizmo		= pGizmo;
	}

	//____ Character __________________________________________________________

	Character::Character( unsigned short character )
	{
		m_type = WG_EVENT_CHARACTER;
		m_char = character;
	}

	Character::Character( unsigned short character, WgGizmo * pGizmo )
	{
		m_type			= WG_EVENT_CHARACTER;
		m_char			= character;
		m_bIsForGizmo	= true;
		m_pGizmo		= pGizmo;
	}

	unsigned short Character::Char() const
	{
		return m_char;
	}

	//____ MouseWheelRoll __________________________________________________________

	MouseWheelRoll::MouseWheelRoll( int wheel, int distance )
	{
		m_type = WG_EVENT_MOUSEWHEEL_ROLL;
		m_wheel = wheel;
		m_distance = distance;
	}

	MouseWheelRoll::MouseWheelRoll( int wheel, int distance, WgGizmo * pGizmo )
	{
		m_type			= WG_EVENT_MOUSEWHEEL_ROLL;
		m_wheel			= wheel;
		m_distance		= distance;
		m_bIsForGizmo	= true;
		m_pGizmo		= pGizmo;
	}


	int MouseWheelRoll::Wheel() const
	{
		return m_wheel;
	}

	int MouseWheelRoll::Distance() const
	{
		return m_distance;
	}

	//____ Tick _______________________________________________________________

	Tick::Tick( int ms )
	{
		m_type = WG_EVENT_TICK;
		m_millisec = ms;
	}

	Tick::Tick( int ms, WgGizmo * pGizmo )
	{
		m_type 			= WG_EVENT_TICK;
		m_millisec 		= ms;
		m_bIsForGizmo	= true;
		m_pGizmo 		= pGizmo;
	}


	int Tick::Millisec() const
	{
		return m_millisec;
	}

	//____ ButtonPress _________________________________________________________

	ButtonPress::ButtonPress( WgGizmoButton * pGizmo )
	{
		m_type 			= WG_EVENT_BUTTON_PRESS;
		m_pGizmo 		= pGizmo;
	}

	WgGizmoButton * ButtonPress::Button() const
	{
		WgGizmo * pGizmo = m_pGizmo.GetRealPtr();
		if( pGizmo )
			return static_cast<WgGizmoButton*>(pGizmo);
		else
			return 0;
	}


	//____ Checkbox event methods ______________________________________________

	WgGizmoCheckbox * CheckboxEvent::Checkbox() const
	{
		WgGizmo * pGizmo = m_pGizmo.GetRealPtr();
		if( pGizmo )
			return static_cast<WgGizmoCheckbox*>(pGizmo);
		else
			return 0;
	}
		
	CheckboxCheck::CheckboxCheck( WgGizmoCheckbox * pGizmo )
	{
		m_type = WG_EVENT_CHECKBOX_CHECK;
		m_pGizmo = pGizmo;
	}

	CheckboxUncheck::CheckboxUncheck( WgGizmoCheckbox * pGizmo )
	{
		m_type = WG_EVENT_CHECKBOX_UNCHECK;
		m_pGizmo = pGizmo;
	}

	CheckboxToggle::CheckboxToggle( WgGizmoCheckbox * pGizmo, bool bChecked )
	{
		m_type = WG_EVENT_CHECKBOX_TOGGLE;
		m_pGizmo = pGizmo;
		m_bChecked = bChecked;
	}

	bool CheckboxToggle::IsChecked() const
	{
		return m_bChecked;
	}

	//____ WgGizmoRadiobutton events ___________________________________________

	WgGizmoRadiobutton * RadiobuttonEvent::Radiobutton() const
	{
		WgGizmo * pGizmo = m_pGizmo.GetRealPtr();
		if( pGizmo )
			return static_cast<WgGizmoRadiobutton*>(pGizmo);
		else
			return 0;
	}
		
	RadiobuttonSelect::RadiobuttonSelect( WgGizmoRadiobutton * pGizmo )
	{
		m_type = WG_EVENT_RADIOBUTTON_SELECT;
		m_pGizmo = pGizmo;
	}

	RadiobuttonUnselect::RadiobuttonUnselect( WgGizmoRadiobutton * pGizmo )
	{
		m_type = WG_EVENT_RADIOBUTTON_UNSELECT;
		m_pGizmo = pGizmo;
	}

	RadiobuttonToggle::RadiobuttonToggle( WgGizmoRadiobutton * pGizmo, bool bSelected )
	{
		m_type = WG_EVENT_RADIOBUTTON_TOGGLE;
		m_pGizmo = pGizmo;
		m_bSelected = bSelected;
	}

	bool RadiobuttonToggle::IsSelected() const
	{
		return m_bSelected;
	}


	//____ MousePositioned _______________________________________________________

	MousePositioned::MousePositioned()
	{
		m_type = WG_EVENT_MOUSE_POSITIONED;
	}

	//____ MouseButtonDrag _________________________________________________________

	MouseButtonDrag::MouseButtonDrag( int button, const WgCoord& startPos, const WgCoord& prevPos, const WgCoord& currPos ) : MouseButtonEvent(button)
	{

		m_type = WG_EVENT_MOUSEBUTTON_DRAG;

		m_startPos = startPos;
		m_prevPos = prevPos;
		m_currPos = currPos;
	}

	MouseButtonDrag::MouseButtonDrag( int button, WgGizmo * pGizmo, const WgCoord& startPos, const WgCoord& prevPos, const WgCoord& currPos ) : MouseButtonEvent(button)
	{
		m_type = WG_EVENT_MOUSEBUTTON_DRAG;
		m_bIsForGizmo		= true;
		m_pGizmo 			= pGizmo;

		m_startPos = startPos;
		m_prevPos = prevPos;
		m_currPos = currPos;
	}

	WgCoord MouseButtonDrag::DraggedTotal() const
	{
		return m_currPos - m_startPos;
	}

	WgCoord MouseButtonDrag::DraggedNow() const
	{
		return m_currPos - m_prevPos;
	}

	WgCoord MouseButtonDrag::CurrPos() const
	{
		return m_currPos;
	}

	WgCoord MouseButtonDrag::StartPos() const
	{
		return m_startPos;
	}

	WgCoord MouseButtonDrag::PrevPos() const
	{
		return m_prevPos;
	}

};
