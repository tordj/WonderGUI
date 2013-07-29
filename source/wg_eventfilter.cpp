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
#include <wg_widget.h>
#include <wg_scrollbar.h>
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
	return WgEventFilter( WG_EVENT_MOUSE_ENTER, 0, _filterTypeWidget );
}

WgEventFilter	WgEventFilter::MouseEnter( const WgWidgetPtr& pWidget )
{
	return WgEventFilter( WG_EVENT_MOUSE_ENTER, pWidget.GetRealPtr(), _filterTypeWidget );
}

//____ MouseLeave() _________________________________________________________

WgEventFilter	WgEventFilter::MouseLeave()
{
	return WgEventFilter( WG_EVENT_MOUSE_LEAVE, 0, _filterTypeWidget );
}

WgEventFilter	WgEventFilter::MouseLeave( const WgWidgetPtr& pWidget )
{
	return WgEventFilter( WG_EVENT_MOUSE_LEAVE, pWidget.GetRealPtr(), _filterTypeWidget );
}

//____ MouseMove() _________________________________________________________

WgEventFilter	WgEventFilter::MouseMove()
{
	return WgEventFilter( WG_EVENT_MOUSE_MOVE, 0, _filterTypeWidget );
}

WgEventFilter	WgEventFilter::MouseMove( const WgWidgetPtr& pWidget )
{
	return WgEventFilter( WG_EVENT_MOUSE_MOVE, pWidget.GetRealPtr(), _filterTypeWidget );
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

WgEventFilter	WgEventFilter::MouseButtonPress( const WgWidgetPtr& pWidget )
{
	return WgEventFilter( WG_EVENT_MOUSEBUTTON_PRESS, pWidget.GetRealPtr(), _filterMouseButtonEvents, 0 );
}

WgEventFilter	WgEventFilter::MouseButtonPress( const WgWidgetPtr& pWidget, int button )
{
	return WgEventFilter( WG_EVENT_MOUSEBUTTON_PRESS, pWidget.GetRealPtr(), _filterMouseButtonEvents, button );
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

WgEventFilter	WgEventFilter::MouseButtonRelease( const WgWidgetPtr& pWidget )
{
	return WgEventFilter( WG_EVENT_MOUSEBUTTON_RELEASE, pWidget.GetRealPtr(), _filterMouseButtonEvents, 0 );
}

WgEventFilter	WgEventFilter::MouseButtonRelease( const WgWidgetPtr& pWidget, int button )
{
	return WgEventFilter( WG_EVENT_MOUSEBUTTON_RELEASE, pWidget.GetRealPtr(), _filterMouseButtonEvents, button );
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

WgEventFilter	WgEventFilter::MouseButtonDrag( const WgWidgetPtr& pWidget )
{
	return WgEventFilter( WG_EVENT_MOUSEBUTTON_DRAG, pWidget.GetRealPtr(), _filterMouseButtonEvents, 0 );
}

WgEventFilter	WgEventFilter::MouseButtonDrag( const WgWidgetPtr& pWidget, int button )
{
	return WgEventFilter( WG_EVENT_MOUSEBUTTON_DRAG, pWidget.GetRealPtr(), _filterMouseButtonEvents, button );
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

WgEventFilter	WgEventFilter::MouseButtonRepeat( const WgWidgetPtr& pWidget )
{
	return WgEventFilter( WG_EVENT_MOUSEBUTTON_REPEAT, pWidget.GetRealPtr(), _filterMouseButtonEvents, 0 );
}

WgEventFilter	WgEventFilter::MouseButtonRepeat( const WgWidgetPtr& pWidget, int button )
{
	return WgEventFilter( WG_EVENT_MOUSEBUTTON_REPEAT, pWidget.GetRealPtr(), _filterMouseButtonEvents, button );
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

WgEventFilter	WgEventFilter::MouseButtonClick( const WgWidgetPtr& pWidget )
{
	return WgEventFilter( WG_EVENT_MOUSEBUTTON_CLICK, pWidget.GetRealPtr(), _filterMouseButtonEvents, 0 );
}

WgEventFilter	WgEventFilter::MouseButtonClick(const WgWidgetPtr& pWidget, int button )
{
	return WgEventFilter( WG_EVENT_MOUSEBUTTON_CLICK, pWidget.GetRealPtr(), _filterMouseButtonEvents, button );
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

WgEventFilter	WgEventFilter::MouseButtonDoubleClick( const WgWidgetPtr& pWidget )
{
	return WgEventFilter( WG_EVENT_MOUSEBUTTON_CLICK, pWidget.GetRealPtr(), _filterMouseButtonEvents, 0 );
}

WgEventFilter	WgEventFilter::MouseButtonDoubleClick( const WgWidgetPtr& pWidget, int button )
{
	return WgEventFilter( WG_EVENT_MOUSEBUTTON_CLICK, pWidget.GetRealPtr(), _filterMouseButtonEvents, button );
}

//____ KeyPress() _____________________________________________________________

WgEventFilter	WgEventFilter::KeyPress()
{
	return WgEventFilter( WG_EVENT_KEY_PRESS, 0, _filterKeyEvents, WG_KEY_UNMAPPED );
}

WgEventFilter	WgEventFilter::KeyPress( const WgWidgetPtr& pWidget )
{
	return WgEventFilter( WG_EVENT_KEY_PRESS, pWidget.GetRealPtr(), _filterKeyEvents, WG_KEY_UNMAPPED );
}

WgEventFilter	WgEventFilter::KeyPress( WgKey keycode )
{
	return WgEventFilter( WG_EVENT_KEY_PRESS, 0, _filterKeyEvents, keycode );
}

WgEventFilter	WgEventFilter::KeyPress( const WgWidgetPtr& pWidget, WgKey keycode )
{
	return WgEventFilter( WG_EVENT_KEY_PRESS, pWidget.GetRealPtr(), _filterKeyEvents, keycode );
}

//____ KeyRelease() ___________________________________________________________

WgEventFilter	WgEventFilter::KeyRelease()
{
	return WgEventFilter( WG_EVENT_KEY_RELEASE, 0, _filterKeyEvents, WG_KEY_UNMAPPED );
}

WgEventFilter	WgEventFilter::KeyRelease( const WgWidgetPtr& pWidget )
{
	return WgEventFilter( WG_EVENT_KEY_RELEASE, pWidget.GetRealPtr(), _filterKeyEvents, WG_KEY_UNMAPPED );
}

WgEventFilter	WgEventFilter::KeyRelease( WgKey keycode )
{
	return WgEventFilter( WG_EVENT_KEY_RELEASE, 0, _filterKeyEvents, keycode );
}

WgEventFilter	WgEventFilter::KeyRelease( const WgWidgetPtr& pWidget, WgKey keycode )
{
	return WgEventFilter( WG_EVENT_KEY_RELEASE, pWidget.GetRealPtr(), _filterKeyEvents, keycode );
}

//____ KeyRepeat() ____________________________________________________________

WgEventFilter	WgEventFilter::KeyRepeat()
{
	return WgEventFilter( WG_EVENT_KEY_REPEAT, 0, _filterKeyEvents, WG_KEY_UNMAPPED );
}

WgEventFilter	WgEventFilter::KeyRepeat( const WgWidgetPtr& pWidget )
{
	return WgEventFilter( WG_EVENT_KEY_REPEAT, pWidget.GetRealPtr(), _filterKeyEvents, WG_KEY_UNMAPPED );
}

WgEventFilter	WgEventFilter::KeyRepeat( WgKey keycode )
{
	return WgEventFilter( WG_EVENT_KEY_REPEAT, 0, _filterKeyEvents, keycode );
}

WgEventFilter	WgEventFilter::KeyRepeat( const WgWidgetPtr& pWidget, WgKey keycode )
{
	return WgEventFilter( WG_EVENT_KEY_REPEAT, pWidget.GetRealPtr(), _filterKeyEvents, keycode );
}

//____ KeyPressNative() _______________________________________________________

WgEventFilter	WgEventFilter::KeyPressNative( int native_keycode )
{
	return WgEventFilter( WG_EVENT_KEY_PRESS, 0, _filterNativeKeyEvents, native_keycode );
}

WgEventFilter	WgEventFilter::KeyPressNative( const WgWidgetPtr& pWidget, int native_keycode )
{
	return WgEventFilter( WG_EVENT_KEY_PRESS, pWidget.GetRealPtr(), _filterNativeKeyEvents, native_keycode );
}

//____ KeyReleaseNative() _____________________________________________________

WgEventFilter	WgEventFilter::KeyReleaseNative( int native_keycode )
{
	return WgEventFilter( WG_EVENT_KEY_RELEASE, 0, _filterNativeKeyEvents, native_keycode );
}

WgEventFilter	WgEventFilter::KeyReleaseNative( const WgWidgetPtr& pWidget, int native_keycode )
{
	return WgEventFilter( WG_EVENT_KEY_RELEASE, pWidget.GetRealPtr(), _filterNativeKeyEvents, native_keycode );
}

//____ KeyRepeatNative() ______________________________________________________

WgEventFilter	WgEventFilter::KeyRepeatNative( int native_keycode )
{
	return WgEventFilter( WG_EVENT_KEY_REPEAT, 0, _filterNativeKeyEvents, native_keycode );
}

WgEventFilter	WgEventFilter::KeyRepeatNative( const WgWidgetPtr& pWidget, int native_keycode )
{
	return WgEventFilter( WG_EVENT_KEY_REPEAT, pWidget.GetRealPtr(), _filterNativeKeyEvents, native_keycode );
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

WgEventFilter	WgEventFilter::Character( const WgWidgetPtr& pWidget )
{
	return WgEventFilter( WG_EVENT_CHARACTER, pWidget.GetRealPtr(), _filterCharacterEvents, 0 );
}

WgEventFilter	WgEventFilter::Character( const WgWidgetPtr& pWidget, unsigned short character )
{
	return WgEventFilter( WG_EVENT_CHARACTER, pWidget.GetRealPtr(), _filterCharacterEvents, character );
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

WgEventFilter	WgEventFilter::MouseWheelRoll( const WgWidgetPtr& pWidget )
{
	return WgEventFilter( WG_EVENT_MOUSEWHEEL_ROLL, pWidget.GetRealPtr(), _filterMouseWheelRollEvents, 0 );
}

WgEventFilter	WgEventFilter::MouseWheelRoll( const WgWidgetPtr& pWidget, int wheel )
{
	return WgEventFilter( WG_EVENT_MOUSEWHEEL_ROLL, pWidget.GetRealPtr(), _filterMouseWheelRollEvents, wheel );
}

//____ ButtonPress() __________________________________________________________

WgEventFilter	WgEventFilter::ButtonPress()
{
	return WgEventFilter( WG_EVENT_BUTTON_PRESS, 0, _filterTypeWidget );
}

WgEventFilter	WgEventFilter::ButtonPress( const WgWidgetPtr& pWidget )
{
	return WgEventFilter( WG_EVENT_BUTTON_PRESS, pWidget.GetRealPtr(), _filterTypeWidget );
}

//____ CheckboxCheck() __________________________________________________________

WgEventFilter	WgEventFilter::CheckboxCheck()
{
	return WgEventFilter( WG_EVENT_CHECKBOX_CHECK, 0, _filterTypeWidget );
}

WgEventFilter	WgEventFilter::CheckboxCheck( const WgWidgetPtr& pWidget )
{
	return WgEventFilter( WG_EVENT_CHECKBOX_CHECK, pWidget.GetRealPtr(), _filterTypeWidget );
}

//____ CheckboxUncheck() __________________________________________________________

WgEventFilter	WgEventFilter::CheckboxUncheck()
{
	return WgEventFilter( WG_EVENT_CHECKBOX_UNCHECK, 0, _filterTypeWidget );
}

WgEventFilter	WgEventFilter::CheckboxUncheck( const WgWidgetPtr& pWidget )
{
	return WgEventFilter( WG_EVENT_CHECKBOX_UNCHECK, pWidget.GetRealPtr(), _filterTypeWidget );
}


//____ CheckboxToggle() __________________________________________________________

WgEventFilter	WgEventFilter::CheckboxToggle()
{
	return WgEventFilter( WG_EVENT_CHECKBOX_TOGGLE, 0, _filterTypeWidget );
}

WgEventFilter	WgEventFilter::CheckboxToggle( const WgWidgetPtr& pWidget )
{
	return WgEventFilter( WG_EVENT_CHECKBOX_TOGGLE, pWidget.GetRealPtr(), _filterTypeWidget );
}

//____ AnimationUpdate() _______________________________________________________

WgEventFilter	WgEventFilter::AnimationUpdate()
{
	return WgEventFilter( WG_EVENT_ANIMATION_UPDATE, 0, _filterTypeWidget );
}

WgEventFilter	WgEventFilter::AnimationUpdate( const WgWidgetPtr& pWidget )
{
	return WgEventFilter( WG_EVENT_ANIMATION_UPDATE, pWidget.GetRealPtr(), _filterTypeWidget );
}

//____ TabSelect() _____________________________________________________________

WgEventFilter	WgEventFilter::TabSelect()
{
	return WgEventFilter( WG_EVENT_TAB_SELECT, 0, _filterTabSelect );
}

WgEventFilter	WgEventFilter::TabSelect( const WgWidgetPtr& pWidget )
{
	return WgEventFilter( WG_EVENT_TAB_SELECT, pWidget.GetRealPtr(), _filterTabSelect );
}

WgEventFilter	WgEventFilter::TabSelect( const WgWidgetPtr& pWidget, int id )
{
	return WgEventFilter( WG_EVENT_TAB_SELECT, pWidget.GetRealPtr(), _filterTabSelect, id );
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

WgEventFilter	WgEventFilter::TabPress( const WgWidgetPtr& pWidget )
{
	return WgEventFilter( WG_EVENT_TAB_PRESS, pWidget.GetRealPtr(), _filterTabPress );
}

WgEventFilter	WgEventFilter::TabPress( const WgWidgetPtr& pWidget, int button )
{
	return WgEventFilter( WG_EVENT_TAB_PRESS, pWidget.GetRealPtr(), _filterTabPress, button );
}

WgEventFilter	WgEventFilter::TabPress( const WgWidgetPtr& pWidget, int button, int id )
{
	return WgEventFilter( WG_EVENT_TAB_PRESS, pWidget.GetRealPtr(), _filterTabPress, button, id );
}

//____ EditvalueModify() _______________________________________________________

WgEventFilter	WgEventFilter::EditvalueModify()
{
	return WgEventFilter( WG_EVENT_EDITVALUE_MODIFY, 0, _filterTypeWidget );
}

WgEventFilter	WgEventFilter::EditvalueModify( const WgWidgetPtr& pWidget )
{
	return WgEventFilter( WG_EVENT_EDITVALUE_MODIFY, pWidget.GetRealPtr(), _filterTypeWidget );
}

//____ EditvalueSet() _______________________________________________________

WgEventFilter	WgEventFilter::EditvalueSet()
{
	return WgEventFilter( WG_EVENT_EDITVALUE_SET, 0, _filterTypeWidget );
}

WgEventFilter	WgEventFilter::EditvalueSet( const WgWidgetPtr& pWidget )
{
	return WgEventFilter( WG_EVENT_EDITVALUE_SET, pWidget.GetRealPtr(), _filterTypeWidget );
}


//____ ScrollbarMove() __________________________________________________________

WgEventFilter	WgEventFilter::ScrollbarMove()
{
	return WgEventFilter( WG_EVENT_SCROLLBAR_MOVE, 0, _filterTypeWidget );
}

WgEventFilter	WgEventFilter::ScrollbarMove( const WgWidgetPtr& pWidget )
{
	return WgEventFilter( WG_EVENT_SCROLLBAR_MOVE, pWidget.GetRealPtr(), _filterTypeWidget );
}

//____ ScrollbarStepFwd() __________________________________________________________

WgEventFilter	WgEventFilter::ScrollbarStepFwd()
{
	return WgEventFilter( WG_EVENT_SCROLLBAR_STEP_FWD, 0, _filterTypeWidget );
}

WgEventFilter	WgEventFilter::ScrollbarStepFwd( const WgWidgetPtr& pWidget )
{
	return WgEventFilter( WG_EVENT_SCROLLBAR_STEP_FWD, pWidget.GetRealPtr(), _filterTypeWidget );
}

//____ ScrollbarStepBwd() __________________________________________________________

WgEventFilter	WgEventFilter::ScrollbarStepBwd()
{
	return WgEventFilter( WG_EVENT_SCROLLBAR_STEP_BWD, 0, _filterTypeWidget );
}

WgEventFilter	WgEventFilter::ScrollbarStepBwd( const WgWidgetPtr& pWidget )
{
	return WgEventFilter( WG_EVENT_SCROLLBAR_STEP_BWD, pWidget.GetRealPtr(), _filterTypeWidget );
}

//____ ScrollbarJumpFwd() __________________________________________________________

WgEventFilter	WgEventFilter::ScrollbarJumpFwd()
{
	return WgEventFilter( WG_EVENT_SCROLLBAR_JUMP_FWD, 0, _filterTypeWidget );
}

WgEventFilter	WgEventFilter::ScrollbarJumpFwd( const WgWidgetPtr& pWidget )
{
	return WgEventFilter( WG_EVENT_SCROLLBAR_JUMP_FWD, pWidget.GetRealPtr(), _filterTypeWidget );
}

//____ ScrollbarJumpBwd() __________________________________________________________

WgEventFilter	WgEventFilter::ScrollbarJumpBwd()
{
	return WgEventFilter( WG_EVENT_SCROLLBAR_JUMP_BWD, 0, _filterTypeWidget );
}

WgEventFilter	WgEventFilter::ScrollbarJumpBwd( const WgWidgetPtr& pWidget )
{
	return WgEventFilter( WG_EVENT_SCROLLBAR_JUMP_BWD, pWidget.GetRealPtr(), _filterTypeWidget );
}

//____ ScrollbarWheelRoll() __________________________________________________________

WgEventFilter	WgEventFilter::ScrollbarWheelRoll()
{
	return WgEventFilter( WG_EVENT_SCROLLBAR_WHEEL_ROLL, 0, _filterTypeWidget );
}

WgEventFilter	WgEventFilter::ScrollbarWheelRoll( const WgWidgetPtr& pWidget )
{
	return WgEventFilter( WG_EVENT_SCROLLBAR_WHEEL_ROLL, pWidget.GetRealPtr(), _filterTypeWidget );
}

//____ TextModify() _______________________________________________________

WgEventFilter	WgEventFilter::TextModify()
{
	return WgEventFilter( WG_EVENT_TEXT_MODIFY, 0, _filterTypeWidget );
}

WgEventFilter	WgEventFilter::TextModify( const WgWidgetPtr& pWidget )
{
	return WgEventFilter( WG_EVENT_TEXT_MODIFY, pWidget.GetRealPtr(), _filterTypeWidget );
}

//____ TextSet() _______________________________________________________

WgEventFilter	WgEventFilter::TextSet()
{
	return WgEventFilter( WG_EVENT_TEXT_SET, 0, _filterTypeWidget );
}

WgEventFilter	WgEventFilter::TextSet( const WgWidgetPtr& pWidget )
{
	return WgEventFilter( WG_EVENT_TEXT_SET, pWidget.GetRealPtr(), _filterTypeWidget );
}

//____ KnobTurn() __________________________________________________________

WgEventFilter	WgEventFilter::KnobTurn()
{
	return WgEventFilter( WG_EVENT_KNOB_TURN, 0, _filterTypeWidget );
}

WgEventFilter	WgEventFilter::KnobTurn( const WgWidgetPtr& pWidget )
{
	return WgEventFilter( WG_EVENT_KNOB_TURN, pWidget.GetRealPtr(), _filterTypeWidget );
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

WgEventFilter	WgEventFilter::MenuitemSelect( const WgWidgetPtr& pWidget )
{
	return WgEventFilter( WG_EVENT_MENUITEM_SELECT, pWidget.GetRealPtr(), _filterMenuitemEvent );
}

WgEventFilter	WgEventFilter::MenuitemSelect( const WgWidgetPtr& pWidget, int itemId )
{
	return WgEventFilter( WG_EVENT_MENUITEM_SELECT, pWidget.GetRealPtr(), _filterMenuitemEvent, itemId );
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

WgEventFilter	WgEventFilter::MenuitemCheck( const WgWidgetPtr& pWidget )
{
	return WgEventFilter( WG_EVENT_MENUITEM_CHECK, pWidget.GetRealPtr(), _filterMenuitemEvent );
}

WgEventFilter	WgEventFilter::MenuitemCheck( const WgWidgetPtr& pWidget, int itemId )
{
	return WgEventFilter( WG_EVENT_MENUITEM_CHECK, pWidget.GetRealPtr(), _filterMenuitemEvent, itemId );
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

WgEventFilter	WgEventFilter::MenuitemUncheck( const WgWidgetPtr& pWidget )
{
	return WgEventFilter( WG_EVENT_MENUITEM_UNCHECK, pWidget.GetRealPtr(), _filterMenuitemEvent );
}

WgEventFilter	WgEventFilter::MenuitemUncheck( const WgWidgetPtr& pWidget, int itemId )
{
	return WgEventFilter( WG_EVENT_MENUITEM_UNCHECK, pWidget.GetRealPtr(), _filterMenuitemEvent, itemId );
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

bool WgEventFilter::_filterTypeWidget( const WgEvent::Event * pEvent, const WgEventFilter& filter )
{
	if( pEvent->Type() == filter.EventType() && (!filter.Widget() || filter.Widget() == pEvent->Widget()) )
		return true;

	return false;
}

bool WgEventFilter::_filterMouseButtonEvents( const WgEvent::Event * pEvent, const WgEventFilter& filter )
{
	if( pEvent->Type() == filter.EventType() && (!filter.Widget() || filter.Widget() == pEvent->Widget()) )
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
	if( pEvent->Type() == filter.EventType() && (!filter.Widget() || filter.Widget() == pEvent->Widget()) )
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
	if( pEvent->Type() == filter.EventType() && (!filter.Widget() || filter.Widget() == pEvent->Widget()) )
	{
		const WgEvent::KeyEvent * p = static_cast<const WgEvent::KeyEvent*>(pEvent);

		if( p->NativeKeyCode() == filter.m_data1 )
			return true;
	}
	return false;
}

bool WgEventFilter::_filterCharacterEvents( const WgEvent::Event * pEvent, const WgEventFilter& filter )
{
	if( pEvent->Type() == filter.EventType() && (!filter.Widget() || filter.Widget() == pEvent->Widget()) )
	{
		int chr = (static_cast<const WgEvent::Character *>(pEvent))->Char();

		if( chr == filter.m_data1 )
			return true;
	}
	return false;
}

bool WgEventFilter::_filterMouseWheelRollEvents( const WgEvent::Event * pEvent, const WgEventFilter& filter )
{
	if( pEvent->Type() == filter.EventType() && (!filter.Widget() || filter.Widget() == pEvent->Widget()) )
	{
		int chr = (static_cast<const WgEvent::MouseWheelRoll *>(pEvent))->Wheel();

		if( chr == filter.m_data1 )
			return true;
	}
	return false;
}

bool WgEventFilter::_filterTabSelect( const WgEvent::Event * pEvent, const WgEventFilter& filter )
{
	if( pEvent->Type() == filter.EventType() && (!filter.Widget() || filter.Widget() == pEvent->Widget()) )
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
	if( pEvent->Type() == filter.EventType() && (!filter.Widget() || filter.Widget() == pEvent->Widget()) )
	{
		const WgEvent::TabPress * p = static_cast<const WgEvent::TabPress*>(pEvent);

		if( (filter.m_data1 == 0 || filter.m_data1 == p->MouseButton()) && (filter.m_data2 == 0 || filter.m_data2 == p->TabId()) )
			return true;
	}
	return false;
}

bool WgEventFilter::_filterMenuitemEvent( const WgEvent::Event * pEvent, const WgEventFilter& filter )
{
	if( pEvent->Type() == filter.EventType() && (!filter.Widget() || filter.Widget() == pEvent->Widget()) )
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
