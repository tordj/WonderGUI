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
#include <wg_slider.h>
#include <wg_checkbox.h>
#include <wg_radiobutton.h>
#include <wg_animplayer.h>
#include <wg_tablist.h>
#include <wg_valueeditor.h>
#include <wg_menu.h>




//____ Tick() _________________________________________________________________

WgEventFilter	WgEventFilter::Tick()
{
	return WgEventFilter( WG_EVENT_TICK, 0, _filterType );
}

//____ PointerChange() _________________________________________________________________

WgEventFilter	WgEventFilter::PointerChange()
{
	return WgEventFilter( WG_EVENT_POINTER_CHANGE, 0, _filterType );
}

//____ FocusGained() __________________________________________________________

WgEventFilter	WgEventFilter::FocusGained()
{
	return WgEventFilter( WG_EVENT_FOCUS_GAINED, 0, _filterType );
}

//____ FocusGained() __________________________________________________________

WgEventFilter	WgEventFilter::FocusLost()
{
	return WgEventFilter( WG_EVENT_FOCUS_LOST, 0, _filterType );
}

//____ MouseEnter() _________________________________________________________

WgEventFilter	WgEventFilter::MouseEnter()
{
	return WgEventFilter( WG_EVENT_MOUSE_ENTER, 0, _filterTypeGizmo );
}

WgEventFilter	WgEventFilter::MouseEnter( WgGizmo * pGizmo )
{
	return WgEventFilter( WG_EVENT_MOUSE_ENTER, pGizmo, _filterTypeGizmo );
}

//____ MouseLeave() _________________________________________________________

WgEventFilter	WgEventFilter::MouseLeave()
{
	return WgEventFilter( WG_EVENT_MOUSE_LEAVE, 0, _filterTypeGizmo );
}

WgEventFilter	WgEventFilter::MouseLeave( WgGizmo * pGizmo )
{
	return WgEventFilter( WG_EVENT_MOUSE_LEAVE, pGizmo, _filterTypeGizmo );
}

//____ MouseMove() _________________________________________________________

WgEventFilter	WgEventFilter::MouseMove()
{
	return WgEventFilter( WG_EVENT_MOUSE_MOVE, 0, _filterTypeGizmo );
}

WgEventFilter	WgEventFilter::MouseMove( WgGizmo * pGizmo )
{
	return WgEventFilter( WG_EVENT_MOUSE_MOVE, pGizmo, _filterTypeGizmo );
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

WgEventFilter	WgEventFilter::MouseWheelRoll( WgGizmo * pGizmo, int wheel )
{
	return WgEventFilter( WG_EVENT_MOUSEWHEEL_ROLL, pGizmo, _filterMouseWheelRollEvents, wheel );
}

//____ ButtonPress() __________________________________________________________

WgEventFilter	WgEventFilter::ButtonPress()
{
	return WgEventFilter( WG_EVENT_BUTTON_PRESS, 0, _filterTypeGizmo );
}

WgEventFilter	WgEventFilter::ButtonPress( WgGizmo * pGizmo )
{
	return WgEventFilter( WG_EVENT_BUTTON_PRESS, pGizmo, _filterTypeGizmo );
}

//____ CheckboxCheck() __________________________________________________________

WgEventFilter	WgEventFilter::CheckboxCheck()
{
	return WgEventFilter( WG_EVENT_CHECKBOX_CHECK, 0, _filterTypeGizmo );
}

WgEventFilter	WgEventFilter::CheckboxCheck( WgCheckBox * pGizmo )
{
	return WgEventFilter( WG_EVENT_CHECKBOX_CHECK, pGizmo, _filterTypeGizmo );
}

//____ CheckboxUncheck() __________________________________________________________

WgEventFilter	WgEventFilter::CheckboxUncheck()
{
	return WgEventFilter( WG_EVENT_CHECKBOX_UNCHECK, 0, _filterTypeGizmo );
}

WgEventFilter	WgEventFilter::CheckboxUncheck( WgCheckBox * pGizmo )
{
	return WgEventFilter( WG_EVENT_CHECKBOX_UNCHECK, pGizmo, _filterTypeGizmo );
}


//____ CheckboxToggle() __________________________________________________________

WgEventFilter	WgEventFilter::CheckboxToggle()
{
	return WgEventFilter( WG_EVENT_CHECKBOX_TOGGLE, 0, _filterTypeGizmo );
}

WgEventFilter	WgEventFilter::CheckboxToggle( WgCheckBox * pGizmo )
{
	return WgEventFilter( WG_EVENT_CHECKBOX_TOGGLE, pGizmo, _filterTypeGizmo );
}

//____ RadiobuttonSelect() __________________________________________________________

WgEventFilter	WgEventFilter::RadiobuttonSelect()
{
	return WgEventFilter( WG_EVENT_RADIOBUTTON_SELECT, 0, _filterTypeGizmo );
}

WgEventFilter	WgEventFilter::RadiobuttonSelect( WgRadioButton * pGizmo )
{
	return WgEventFilter( WG_EVENT_RADIOBUTTON_SELECT, pGizmo, _filterTypeGizmo );
}

//____ RadiobuttonUnselect() __________________________________________________________

WgEventFilter	WgEventFilter::RadiobuttonUnselect()
{
	return WgEventFilter( WG_EVENT_RADIOBUTTON_UNSELECT, 0, _filterTypeGizmo );
}

WgEventFilter	WgEventFilter::RadiobuttonUnselect( WgRadioButton * pGizmo )
{
	return WgEventFilter( WG_EVENT_RADIOBUTTON_UNSELECT, pGizmo, _filterTypeGizmo );
}


//____ RadiobuttonToggle() __________________________________________________________

WgEventFilter	WgEventFilter::RadiobuttonToggle()
{
	return WgEventFilter( WG_EVENT_RADIOBUTTON_TOGGLE, 0, _filterTypeGizmo );
}

WgEventFilter	WgEventFilter::RadiobuttonToggle( WgRadioButton * pGizmo )
{
	return WgEventFilter( WG_EVENT_RADIOBUTTON_TOGGLE, pGizmo, _filterTypeGizmo );
}

//____ AnimationUpdate() _______________________________________________________

WgEventFilter	WgEventFilter::AnimationUpdate()
{
	return WgEventFilter( WG_EVENT_ANIMATION_UPDATE, 0, _filterTypeGizmo );
}

WgEventFilter	WgEventFilter::AnimationUpdate( WgAnimPlayer * pGizmo )
{
	return WgEventFilter( WG_EVENT_ANIMATION_UPDATE, pGizmo, _filterTypeGizmo );
}

//____ TabSelect() _____________________________________________________________

WgEventFilter	WgEventFilter::TabSelect()
{
	return WgEventFilter( WG_EVENT_TAB_SELECT, 0, _filterTabSelect );
}

WgEventFilter	WgEventFilter::TabSelect( WgTablist * pGizmo )
{
	return WgEventFilter( WG_EVENT_TAB_SELECT, pGizmo, _filterTabSelect );
}

WgEventFilter	WgEventFilter::TabSelect( WgTablist * pGizmo, int id )
{
	return WgEventFilter( WG_EVENT_TAB_SELECT, pGizmo, _filterTabSelect, id );
}

//____ TabPress() _____________________________________________________________

WgEventFilter	WgEventFilter::TabPress()
{
	return WgEventFilter( WG_EVENT_TAB_PRESS, 0, _filterTabPress );
}

WgEventFilter	WgEventFilter::TabPress( int button )
{
	return WgEventFilter( WG_EVENT_TAB_PRESS, 0, _filterTabPress, button );
}

WgEventFilter	WgEventFilter::TabPress( WgTablist * pGizmo )
{
	return WgEventFilter( WG_EVENT_TAB_PRESS, pGizmo, _filterTabPress );
}

WgEventFilter	WgEventFilter::TabPress( WgTablist * pGizmo, int button )
{
	return WgEventFilter( WG_EVENT_TAB_PRESS, pGizmo, _filterTabPress, button );
}

WgEventFilter	WgEventFilter::TabPress( WgTablist * pGizmo, int button, int id )
{
	return WgEventFilter( WG_EVENT_TAB_PRESS, pGizmo, _filterTabPress, button, id );
}

//____ EditvalueModify() _______________________________________________________

WgEventFilter	WgEventFilter::EditvalueModify()
{
	return WgEventFilter( WG_EVENT_EDITVALUE_MODIFY, 0, _filterTypeGizmo );
}

WgEventFilter	WgEventFilter::EditvalueModify( WgValueEditor * pGizmo )
{
	return WgEventFilter( WG_EVENT_EDITVALUE_MODIFY, pGizmo, _filterTypeGizmo );
}

//____ EditvalueSet() _______________________________________________________

WgEventFilter	WgEventFilter::EditvalueSet()
{
	return WgEventFilter( WG_EVENT_EDITVALUE_SET, 0, _filterTypeGizmo );
}

WgEventFilter	WgEventFilter::EditvalueSet( WgValueEditor * pGizmo )
{
	return WgEventFilter( WG_EVENT_EDITVALUE_SET, pGizmo, _filterTypeGizmo );
}


//____ SliderMove() __________________________________________________________

WgEventFilter	WgEventFilter::SliderMove()
{
	return WgEventFilter( WG_EVENT_SLIDER_MOVE, 0, _filterTypeGizmo );
}

WgEventFilter	WgEventFilter::SliderMove( WgGizmoSlider * pGizmo )
{
	return WgEventFilter( WG_EVENT_SLIDER_MOVE, pGizmo, _filterTypeGizmo );
}

//____ SliderStepFwd() __________________________________________________________

WgEventFilter	WgEventFilter::SliderStepFwd()
{
	return WgEventFilter( WG_EVENT_SLIDER_STEP_FWD, 0, _filterTypeGizmo );
}

WgEventFilter	WgEventFilter::SliderStepFwd( WgGizmoSlider * pGizmo )
{
	return WgEventFilter( WG_EVENT_SLIDER_STEP_FWD, pGizmo, _filterTypeGizmo );
}

//____ SliderStepBwd() __________________________________________________________

WgEventFilter	WgEventFilter::SliderStepBwd()
{
	return WgEventFilter( WG_EVENT_SLIDER_STEP_BWD, 0, _filterTypeGizmo );
}

WgEventFilter	WgEventFilter::SliderStepBwd( WgGizmoSlider * pGizmo )
{
	return WgEventFilter( WG_EVENT_SLIDER_STEP_BWD, pGizmo, _filterTypeGizmo );
}

//____ SliderJumpFwd() __________________________________________________________

WgEventFilter	WgEventFilter::SliderJumpFwd()
{
	return WgEventFilter( WG_EVENT_SLIDER_JUMP_FWD, 0, _filterTypeGizmo );
}

WgEventFilter	WgEventFilter::SliderJumpFwd( WgGizmoSlider * pGizmo )
{
	return WgEventFilter( WG_EVENT_SLIDER_JUMP_FWD, pGizmo, _filterTypeGizmo );
}

//____ SliderJumpBwd() __________________________________________________________

WgEventFilter	WgEventFilter::SliderJumpBwd()
{
	return WgEventFilter( WG_EVENT_SLIDER_JUMP_BWD, 0, _filterTypeGizmo );
}

WgEventFilter	WgEventFilter::SliderJumpBwd( WgGizmoSlider * pGizmo )
{
	return WgEventFilter( WG_EVENT_SLIDER_JUMP_BWD, pGizmo, _filterTypeGizmo );
}

//____ SliderWheelRoll() __________________________________________________________

WgEventFilter	WgEventFilter::SliderWheelRoll()
{
	return WgEventFilter( WG_EVENT_SLIDER_WHEEL_ROLL, 0, _filterTypeGizmo );
}

WgEventFilter	WgEventFilter::SliderWheelRoll( WgGizmoSlider * pGizmo )
{
	return WgEventFilter( WG_EVENT_SLIDER_WHEEL_ROLL, pGizmo, _filterTypeGizmo );
}

//____ TextModify() _______________________________________________________

WgEventFilter	WgEventFilter::TextModify()
{
	return WgEventFilter( WG_EVENT_TEXT_MODIFY, 0, _filterTypeGizmo );
}

WgEventFilter	WgEventFilter::TextModify( WgGizmo * pGizmo )
{
	return WgEventFilter( WG_EVENT_TEXT_MODIFY, pGizmo, _filterTypeGizmo );
}

//____ TextSet() _______________________________________________________

WgEventFilter	WgEventFilter::TextSet()
{
	return WgEventFilter( WG_EVENT_TEXT_SET, 0, _filterTypeGizmo );
}

WgEventFilter	WgEventFilter::TextSet( WgGizmo * pGizmo )
{
	return WgEventFilter( WG_EVENT_TEXT_SET, pGizmo, _filterTypeGizmo );
}

//____ KnobTurn() __________________________________________________________

WgEventFilter	WgEventFilter::KnobTurn()
{
	return WgEventFilter( WG_EVENT_KNOB_TURN, 0, _filterTypeGizmo );
}

WgEventFilter	WgEventFilter::KnobTurn( WgGizmo * pGizmo )
{
	return WgEventFilter( WG_EVENT_KNOB_TURN, pGizmo, _filterTypeGizmo );
}

//____ MenuitemSelect() __________________________________________________________

WgEventFilter	WgEventFilter::MenuitemSelect()
{
	return WgEventFilter( WG_EVENT_MENUITEM_SELECT, 0, _filterMenuitemEvent );
}

WgEventFilter	WgEventFilter::MenuitemSelect( int itemId )
{
	return WgEventFilter( WG_EVENT_MENUITEM_SELECT, 0, _filterMenuitemEvent, itemId );
}

WgEventFilter	WgEventFilter::MenuitemSelect( WgMenu * pGizmo )
{
	return WgEventFilter( WG_EVENT_MENUITEM_SELECT, pGizmo, _filterMenuitemEvent );
}

WgEventFilter	WgEventFilter::MenuitemSelect( WgMenu * pGizmo, int itemId )
{
	return WgEventFilter( WG_EVENT_MENUITEM_SELECT, pGizmo, _filterMenuitemEvent, itemId );
}

//____ MenuitemCheck() __________________________________________________________

WgEventFilter	WgEventFilter::MenuitemCheck()
{
	return WgEventFilter( WG_EVENT_MENUITEM_CHECK, 0, _filterMenuitemEvent );
}

WgEventFilter	WgEventFilter::MenuitemCheck( int itemId )
{
	return WgEventFilter( WG_EVENT_MENUITEM_CHECK, 0, _filterMenuitemEvent, itemId );
}

WgEventFilter	WgEventFilter::MenuitemCheck( WgMenu * pGizmo )
{
	return WgEventFilter( WG_EVENT_MENUITEM_CHECK, pGizmo, _filterMenuitemEvent );
}

WgEventFilter	WgEventFilter::MenuitemCheck( WgMenu * pGizmo, int itemId )
{
	return WgEventFilter( WG_EVENT_MENUITEM_CHECK, pGizmo, _filterMenuitemEvent, itemId );
}

//____ MenuitemUncheck() __________________________________________________________

WgEventFilter	WgEventFilter::MenuitemUncheck()
{
	return WgEventFilter( WG_EVENT_MENUITEM_UNCHECK, 0, _filterMenuitemEvent );
}

WgEventFilter	WgEventFilter::MenuitemUncheck( int itemId )
{
	return WgEventFilter( WG_EVENT_MENUITEM_UNCHECK, 0, _filterMenuitemEvent, itemId );
}

WgEventFilter	WgEventFilter::MenuitemUncheck( WgMenu * pGizmo )
{
	return WgEventFilter( WG_EVENT_MENUITEM_UNCHECK, pGizmo, _filterMenuitemEvent );
}

WgEventFilter	WgEventFilter::MenuitemUncheck( WgMenu * pGizmo, int itemId )
{
	return WgEventFilter( WG_EVENT_MENUITEM_UNCHECK, pGizmo, _filterMenuitemEvent, itemId );
}


//_____________________________________________________________________________

bool WgEventFilter::_filterDummy( const WgEvent::Event * pEvent, const WgEventFilter& filter )
{
	return true;
}

bool WgEventFilter::_filterType( const WgEvent::Event * pEvent, const WgEventFilter& filter )
{
	if( pEvent->Type() == filter.EventType() )
		return true;

	return false;
}

bool WgEventFilter::_filterTypeGizmo( const WgEvent::Event * pEvent, const WgEventFilter& filter )
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

bool WgEventFilter::_filterTabSelect( const WgEvent::Event * pEvent, const WgEventFilter& filter )
{
	if( pEvent->Type() == filter.EventType() && (!filter.Gizmo() || filter.Gizmo() == pEvent->Gizmo()) )
	{
		if( filter.m_data1 == 0 )
			return true;
		else
		{
			const WgEvent::TabSelect * p = static_cast<const WgEvent::TabSelect*>(pEvent);

			if( p->TabId() == filter.m_data1 )
				return true;
		}
	}
	return false;
}

bool WgEventFilter::_filterTabPress( const WgEvent::Event * pEvent, const WgEventFilter& filter )
{
	if( pEvent->Type() == filter.EventType() && (!filter.Gizmo() || filter.Gizmo() == pEvent->Gizmo()) )
	{
		const WgEvent::TabPress * p = static_cast<const WgEvent::TabPress*>(pEvent);

		if( (filter.m_data1 == 0 || filter.m_data1 == p->MouseButton()) && (filter.m_data2 == 0 || filter.m_data2 == p->TabId()) )
			return true;
	}
	return false;
}

bool WgEventFilter::_filterMenuitemEvent( const WgEvent::Event * pEvent, const WgEventFilter& filter )
{
	if( pEvent->Type() == filter.EventType() && (!filter.Gizmo() || filter.Gizmo() == pEvent->Gizmo()) )
	{
		if( filter.m_data1 == 0 )
			return true;
		else
		{
			const WgEvent::MenuitemEvent * p = static_cast<const WgEvent::MenuitemEvent*>(pEvent);

			if( p->ItemId() == filter.m_data1 )
				return true;
		}
	}
	return false;
}
