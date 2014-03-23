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

//____ MousePress() _________________________________________________________

WgEventFilter	WgEventFilter::MousePress()
{
	return WgEventFilter( WG_EVENT_MOUSE_PRESS, 0, _filterMouseButtonEvents, 0 );
}

WgEventFilter	WgEventFilter::MousePress( int button)
{
	return WgEventFilter( WG_EVENT_MOUSE_PRESS, 0, _filterMouseButtonEvents, button );
}

WgEventFilter	WgEventFilter::MousePress( const WgWidgetPtr& pWidget )
{
	return WgEventFilter( WG_EVENT_MOUSE_PRESS, pWidget.GetRealPtr(), _filterMouseButtonEvents, 0 );
}

WgEventFilter	WgEventFilter::MousePress( const WgWidgetPtr& pWidget, int button )
{
	return WgEventFilter( WG_EVENT_MOUSE_PRESS, pWidget.GetRealPtr(), _filterMouseButtonEvents, button );
}

//____ MouseRelease() ________________________________________________________

WgEventFilter	WgEventFilter::MouseRelease()
{
	return WgEventFilter( WG_EVENT_MOUSE_RELEASE, 0, _filterMouseButtonEvents, 0 );
}

WgEventFilter	WgEventFilter::MouseRelease( int button)
{
	return WgEventFilter( WG_EVENT_MOUSE_RELEASE, 0, _filterMouseButtonEvents, button );
}

WgEventFilter	WgEventFilter::MouseRelease( const WgWidgetPtr& pWidget )
{
	return WgEventFilter( WG_EVENT_MOUSE_RELEASE, pWidget.GetRealPtr(), _filterMouseButtonEvents, 0 );
}

WgEventFilter	WgEventFilter::MouseRelease( const WgWidgetPtr& pWidget, int button )
{
	return WgEventFilter( WG_EVENT_MOUSE_RELEASE, pWidget.GetRealPtr(), _filterMouseButtonEvents, button );
}

//____ MouseDrag() ___________________________________________________________

WgEventFilter	WgEventFilter::MouseDrag()
{
	return WgEventFilter( WG_EVENT_MOUSE_DRAG, 0, _filterMouseButtonEvents, 0 );
}

WgEventFilter	WgEventFilter::MouseDrag( int button)
{
	return WgEventFilter( WG_EVENT_MOUSE_DRAG, 0, _filterMouseButtonEvents, button );
}

WgEventFilter	WgEventFilter::MouseDrag( const WgWidgetPtr& pWidget )
{
	return WgEventFilter( WG_EVENT_MOUSE_DRAG, pWidget.GetRealPtr(), _filterMouseButtonEvents, 0 );
}

WgEventFilter	WgEventFilter::MouseDrag( const WgWidgetPtr& pWidget, int button )
{
	return WgEventFilter( WG_EVENT_MOUSE_DRAG, pWidget.GetRealPtr(), _filterMouseButtonEvents, button );
}

//____ MouseRepeat() ________________________________________________________

WgEventFilter	WgEventFilter::MouseRepeat()
{
	return WgEventFilter( WG_EVENT_MOUSE_REPEAT, 0, _filterMouseButtonEvents, 0 );
}

WgEventFilter	WgEventFilter::MouseRepeat( int button)
{
	return WgEventFilter( WG_EVENT_MOUSE_REPEAT, 0, _filterMouseButtonEvents, button );
}

WgEventFilter	WgEventFilter::MouseRepeat( const WgWidgetPtr& pWidget )
{
	return WgEventFilter( WG_EVENT_MOUSE_REPEAT, pWidget.GetRealPtr(), _filterMouseButtonEvents, 0 );
}

WgEventFilter	WgEventFilter::MouseRepeat( const WgWidgetPtr& pWidget, int button )
{
	return WgEventFilter( WG_EVENT_MOUSE_REPEAT, pWidget.GetRealPtr(), _filterMouseButtonEvents, button );
}

//____ MouseClick() ________________________________________________________

WgEventFilter	WgEventFilter::MouseClick()
{
	return WgEventFilter( WG_EVENT_MOUSE_CLICK, 0, _filterMouseButtonEvents, 0 );
}

WgEventFilter	WgEventFilter::MouseClick( int button)
{
	return WgEventFilter( WG_EVENT_MOUSE_CLICK, 0, _filterMouseButtonEvents, button );
}

WgEventFilter	WgEventFilter::MouseClick( const WgWidgetPtr& pWidget )
{
	return WgEventFilter( WG_EVENT_MOUSE_CLICK, pWidget.GetRealPtr(), _filterMouseButtonEvents, 0 );
}

WgEventFilter	WgEventFilter::MouseClick(const WgWidgetPtr& pWidget, int button )
{
	return WgEventFilter( WG_EVENT_MOUSE_CLICK, pWidget.GetRealPtr(), _filterMouseButtonEvents, button );
}

//____ MouseDoubleClick() ________________________________________________________

WgEventFilter	WgEventFilter::MouseDoubleClick()
{
	return WgEventFilter( WG_EVENT_MOUSE_CLICK, 0, _filterMouseButtonEvents, 0 );
}

WgEventFilter	WgEventFilter::MouseDoubleClick( int button)
{
	return WgEventFilter( WG_EVENT_MOUSE_CLICK, 0, _filterMouseButtonEvents, button );
}

WgEventFilter	WgEventFilter::MouseDoubleClick( const WgWidgetPtr& pWidget )
{
	return WgEventFilter( WG_EVENT_MOUSE_CLICK, pWidget.GetRealPtr(), _filterMouseButtonEvents, 0 );
}

WgEventFilter	WgEventFilter::MouseDoubleClick( const WgWidgetPtr& pWidget, int button )
{
	return WgEventFilter( WG_EVENT_MOUSE_CLICK, pWidget.GetRealPtr(), _filterMouseButtonEvents, button );
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

//____ WheelRoll() ____________________________________________________________

WgEventFilter	WgEventFilter::WheelRoll()
{
	return WgEventFilter( WG_EVENT_WHEEL_ROLL, 0, _filterWheelRollEvents, 0 );
}

WgEventFilter	WgEventFilter::WheelRoll( int wheel )
{
	return WgEventFilter( WG_EVENT_WHEEL_ROLL, 0, _filterWheelRollEvents, wheel );
}

WgEventFilter	WgEventFilter::WheelRoll( const WgWidgetPtr& pWidget )
{
	return WgEventFilter( WG_EVENT_WHEEL_ROLL, pWidget.GetRealPtr(), _filterWheelRollEvents, 0 );
}

WgEventFilter	WgEventFilter::WheelRoll( const WgWidgetPtr& pWidget, int wheel )
{
	return WgEventFilter( WG_EVENT_WHEEL_ROLL, pWidget.GetRealPtr(), _filterWheelRollEvents, wheel );
}

//____ Select() __________________________________________________________

WgEventFilter	WgEventFilter::Select()
{
	return WgEventFilter( WG_EVENT_SELECT, 0, _filterType );
}

WgEventFilter	WgEventFilter::Select( const WgWidgetPtr& pWidget )
{
	return WgEventFilter( WG_EVENT_SELECT, pWidget.GetRealPtr(), _filterTypeWidget );
}

//____ Toggle() __________________________________________________________

WgEventFilter	WgEventFilter::Toggle()
{
	return WgEventFilter( WG_EVENT_TOGGLE, 0, _filterType );
}

WgEventFilter	WgEventFilter::Toggle( const WgWidgetPtr& pWidget )
{
	return WgEventFilter( WG_EVENT_TOGGLE, pWidget.GetRealPtr(), _filterTypeWidget );
}

//____ ValueUpdate() _______________________________________________________

WgEventFilter	WgEventFilter::ValueUpdate()
{
	return WgEventFilter( WG_EVENT_VALUE_UPDATE, 0, _filterType );
}

WgEventFilter	WgEventFilter::ValueUpdate( const WgWidgetPtr& pWidget )
{
	return WgEventFilter( WG_EVENT_VALUE_UPDATE, pWidget.GetRealPtr(), _filterTypeWidget );
}

//____ RangeUpdate() __________________________________________________________

WgEventFilter	WgEventFilter::RangeUpdate()
{
	return WgEventFilter( WG_EVENT_RANGE_UPDATE, 0, _filterType );
}

WgEventFilter	WgEventFilter::RangeUpdate( const WgWidgetPtr& pWidget )
{
	return WgEventFilter( WG_EVENT_RANGE_UPDATE, pWidget.GetRealPtr(), _filterTypeWidget );
}

//____ TextEdit() _______________________________________________________

WgEventFilter	WgEventFilter::TextEdit()
{
	return WgEventFilter( WG_EVENT_TEXT_EDIT, 0, _filterType );
}

WgEventFilter	WgEventFilter::TextEdit( const WgWidgetPtr& pWidget )
{
	return WgEventFilter( WG_EVENT_TEXT_EDIT, pWidget.GetRealPtr(), _filterTypeWidget );
}

//____ ItemToggle() ___________________________________________________________

WgEventFilter WgEventFilter::ItemToggle()
{
	return WgEventFilter( WG_EVENT_ITEM_TOGGLE, 0, _filterType );
}

WgEventFilter WgEventFilter::ItemToggle( const WgWidgetPtr& pWidget )
{
	return WgEventFilter( WG_EVENT_ITEM_TOGGLE, pWidget.GetRealPtr(), _filterTypeWidget );
}

WgEventFilter WgEventFilter::ItemToggle( const WgWidgetPtr& pWidget, int id )
{
	return WgEventFilter( WG_EVENT_ITEM_TOGGLE, pWidget.GetRealPtr(), _filterItemToggleEvents, id, 0 );
}

//____ ItemMousePress() _______________________________________________________

WgEventFilter WgEventFilter::ItemMousePress()
{
	return WgEventFilter( WG_EVENT_ITEM_MOUSE_PRESS, 0, _filterType );
}

WgEventFilter WgEventFilter::ItemMousePress( int button )
{
	return WgEventFilter( WG_EVENT_ITEM_MOUSE_PRESS, 0, _filterItemMousePressEvents, -1, button );
}

WgEventFilter WgEventFilter::ItemMousePress( const WgWidgetPtr& pWidget )
{
	return WgEventFilter( WG_EVENT_ITEM_MOUSE_PRESS, pWidget.GetRealPtr(), _filterTypeWidget );
}

WgEventFilter WgEventFilter::ItemMousePress( const WgWidgetPtr& pWidget, int button )
{
	return WgEventFilter( WG_EVENT_ITEM_MOUSE_PRESS, pWidget.GetRealPtr(), _filterItemMousePressEvents, -1, button );
}

WgEventFilter WgEventFilter::ItemMousePress( const WgWidgetPtr& pWidget, int button, int id )
{
	return WgEventFilter( WG_EVENT_ITEM_MOUSE_PRESS, pWidget.GetRealPtr(), _filterItemMousePressEvents, id, button );
}

//____ ItemsSelect() __________________________________________________________

WgEventFilter WgEventFilter::ItemsSelect()
{
	return WgEventFilter( WG_EVENT_ITEMS_SELECT, 0, _filterType );
}

WgEventFilter WgEventFilter::ItemsSelect( const WgWidgetPtr& pWidget )
{
	return WgEventFilter( WG_EVENT_ITEMS_SELECT, pWidget.GetRealPtr(), _filterTypeWidget );
}


//____ ItemsUnselect() ________________________________________________________

WgEventFilter WgEventFilter::ItemsUnselect()
{
	return WgEventFilter( WG_EVENT_ITEMS_UNSELECT, 0, _filterType );
}

WgEventFilter WgEventFilter::ItemsUnselect( const WgWidgetPtr& pWidget )
{
	return WgEventFilter( WG_EVENT_ITEMS_UNSELECT, pWidget.GetRealPtr(), _filterTypeWidget );
}



/*
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
*/

//_____________________________________________________________________________

bool WgEventFilter::_filterDummy( const WgEventPtr& pEvent, const WgEventFilter& filter )
{
	return true;
}

bool WgEventFilter::_filterType( const WgEventPtr& pEvent, const WgEventFilter& filter )
{
	if( pEvent->Type() == filter.EventType() )
		return true;

	return false;
}

bool WgEventFilter::_filterTypeWidget( const WgEventPtr& pEvent, const WgEventFilter& filter )
{
	if( pEvent->Type() == filter.EventType() && (!filter.Widget() || filter.Widget() == pEvent->Widget()) )
		return true;

	return false;
}

bool WgEventFilter::_filterMouseButtonEvents( const WgEventPtr& pEvent, const WgEventFilter& filter )
{
	if( pEvent->Type() == filter.EventType() && (!filter.Widget() || filter.Widget() == pEvent->Widget()) )
	{
		if( filter.m_data1 == 0 )
			return true;
		else
		{
			WgMouseButtonEventPtr p = WgMouseButtonEvent::Cast(pEvent);

			if( p->Button() == filter.m_data1 )
				return true;
		}
	}
	return false;
}

bool WgEventFilter::_filterKeyEvents( const WgEventPtr& pEvent, const WgEventFilter& filter )
{
	if( pEvent->Type() == filter.EventType() && (!filter.Widget() || filter.Widget() == pEvent->Widget()) )
	{
		if( filter.m_data1 == 0 )
			return true;
		else
		{
			WgKeyEventPtr p = WgKeyEvent::Cast(pEvent);

			if( p->TranslatedKeyCode() == filter.m_data1 )
				return true;
		}
	}
	return false;
}

bool WgEventFilter::_filterNativeKeyEvents( const WgEventPtr& pEvent, const WgEventFilter& filter )
{
	if( pEvent->Type() == filter.EventType() && (!filter.Widget() || filter.Widget() == pEvent->Widget()) )
	{
		WgKeyEventPtr p = WgKeyEvent::Cast(pEvent);

		if( p->NativeKeyCode() == filter.m_data1 )
			return true;
	}
	return false;
}

bool WgEventFilter::_filterCharacterEvents( const WgEventPtr& pEvent, const WgEventFilter& filter )
{
	if( pEvent->Type() == filter.EventType() && (!filter.Widget() || filter.Widget() == pEvent->Widget()) )
	{
		int chr = WgCharacterEvent::Cast(pEvent)->Char();

		if( chr == filter.m_data1 )
			return true;
	}
	return false;
}

bool WgEventFilter::_filterWheelRollEvents( const WgEventPtr& pEvent, const WgEventFilter& filter )
{
	if( pEvent->Type() == filter.EventType() && (!filter.Widget() || filter.Widget() == pEvent->Widget()) )
	{
		int chr = WgWheelRollEvent::Cast(pEvent)->Wheel();

		if( chr == filter.m_data1 )
			return true;
	}
	return false;
}

bool WgEventFilter::_filterItemToggleEvents( const WgEventPtr& pEvent, const WgEventFilter& filter )
{
	if( pEvent->Type() == filter.EventType() && (!filter.Widget() || filter.Widget() == pEvent->Widget()) )
	{
		int itemId = WgItemToggleEvent::Cast(pEvent)->ItemId();

		if( itemId == filter.m_data1 )
			return true;
	}
	return false;
}


bool WgEventFilter::_filterItemMousePressEvents( const WgEventPtr& pEvent, const WgEventFilter& filter )
{
	if( pEvent->Type() == filter.EventType() && (!filter.Widget() || filter.Widget() == pEvent->Widget()) )
	{
		WgItemMousePressEventPtr pEvent = WgItemMousePressEvent::Cast(pEvent);

		if( (filter.m_data1 == -1 || pEvent->ItemId() == filter.m_data1) ||
			(filter.m_data2 == -1 || pEvent->Button() == filter.m_data2) )
			return true;
	}
	return false;
}



/*
bool WgEventFilter::_filterMenuitemEvent( const WgEventPtr& pEvent, const WgEventFilter& filter )
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
*/