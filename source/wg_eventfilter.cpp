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
#include <wg_animplayer.h>
#include <wg_valueeditor.h>
#include <wg_menu.h>


//____ Tick() _________________________________________________________________

WgEventFilter	WgEventFilter::Tick()
{
	return WgEventFilter( WG_EVENT_TICK, _filterType );
}

//____ PointerChange() _________________________________________________________________

WgEventFilter	WgEventFilter::PointerChange()
{
	return WgEventFilter( WG_EVENT_POINTER_CHANGE, _filterType );
}

//____ FocusGained() __________________________________________________________

WgEventFilter	WgEventFilter::FocusGained()
{
	return WgEventFilter( WG_EVENT_FOCUS_GAINED, _filterType );
}

//____ FocusGained() __________________________________________________________

WgEventFilter	WgEventFilter::FocusLost()
{
	return WgEventFilter( WG_EVENT_FOCUS_LOST, _filterType );
}

//____ MouseEnter() _________________________________________________________

WgEventFilter	WgEventFilter::MouseEnter()
{
	return WgEventFilter( WG_EVENT_MOUSE_ENTER, _filterType );
}

//____ MouseLeave() _________________________________________________________

WgEventFilter	WgEventFilter::MouseLeave()
{
	return WgEventFilter( WG_EVENT_MOUSE_LEAVE, _filterType );
}

//____ MouseMove() _________________________________________________________

WgEventFilter	WgEventFilter::MouseMove()
{
	return WgEventFilter( WG_EVENT_MOUSE_MOVE, _filterType );
}

//____ MousePress() _________________________________________________________

WgEventFilter	WgEventFilter::MousePress()
{
	return WgEventFilter( WG_EVENT_MOUSE_PRESS, _filterMouseButtonEvents, 0 );
}

WgEventFilter	WgEventFilter::MousePress( WgMouseButton button)
{
	return WgEventFilter( WG_EVENT_MOUSE_PRESS, _filterMouseButtonEvents, button );
}

//____ MouseRelease() ________________________________________________________

WgEventFilter	WgEventFilter::MouseRelease()
{
	return WgEventFilter( WG_EVENT_MOUSE_RELEASE, _filterMouseButtonEvents, 0 );
}

WgEventFilter	WgEventFilter::MouseRelease( WgMouseButton button)
{
	return WgEventFilter( WG_EVENT_MOUSE_RELEASE, _filterMouseButtonEvents, button );
}

//____ MouseDrag() ___________________________________________________________

WgEventFilter	WgEventFilter::MouseDrag()
{
	return WgEventFilter( WG_EVENT_MOUSE_DRAG, _filterMouseButtonEvents, 0 );
}

WgEventFilter	WgEventFilter::MouseDrag( WgMouseButton button)
{
	return WgEventFilter( WG_EVENT_MOUSE_DRAG, _filterMouseButtonEvents, button );
}

//____ MouseRepeat() ________________________________________________________

WgEventFilter	WgEventFilter::MouseRepeat()
{
	return WgEventFilter( WG_EVENT_MOUSE_REPEAT, _filterMouseButtonEvents, 0 );
}

WgEventFilter	WgEventFilter::MouseRepeat( WgMouseButton button)
{
	return WgEventFilter( WG_EVENT_MOUSE_REPEAT, _filterMouseButtonEvents, button );
}

//____ MouseClick() ________________________________________________________

WgEventFilter	WgEventFilter::MouseClick()
{
	return WgEventFilter( WG_EVENT_MOUSE_CLICK, _filterMouseButtonEvents, 0 );
}

WgEventFilter	WgEventFilter::MouseClick( WgMouseButton button)
{
	return WgEventFilter( WG_EVENT_MOUSE_CLICK, _filterMouseButtonEvents, button );
}

//____ MouseDoubleClick() ________________________________________________________

WgEventFilter	WgEventFilter::MouseDoubleClick()
{
	return WgEventFilter( WG_EVENT_MOUSE_CLICK, _filterMouseButtonEvents, 0 );
}

WgEventFilter	WgEventFilter::MouseDoubleClick( WgMouseButton button)
{
	return WgEventFilter( WG_EVENT_MOUSE_CLICK, _filterMouseButtonEvents, button );
}

//____ KeyPress() _____________________________________________________________

WgEventFilter	WgEventFilter::KeyPress()
{
	return WgEventFilter( WG_EVENT_KEY_PRESS, _filterKeyEvents, WG_KEY_UNMAPPED );
}

WgEventFilter	WgEventFilter::KeyPress( WgKey keycode )
{
	return WgEventFilter( WG_EVENT_KEY_PRESS, _filterKeyEvents, keycode );
}

//____ KeyRelease() ___________________________________________________________

WgEventFilter	WgEventFilter::KeyRelease()
{
	return WgEventFilter( WG_EVENT_KEY_RELEASE, _filterKeyEvents, WG_KEY_UNMAPPED );
}

WgEventFilter	WgEventFilter::KeyRelease( WgKey keycode )
{
	return WgEventFilter( WG_EVENT_KEY_RELEASE, _filterKeyEvents, keycode );
}

//____ KeyRepeat() ____________________________________________________________

WgEventFilter	WgEventFilter::KeyRepeat()
{
	return WgEventFilter( WG_EVENT_KEY_REPEAT, _filterKeyEvents, WG_KEY_UNMAPPED );
}

WgEventFilter	WgEventFilter::KeyRepeat( WgKey keycode )
{
	return WgEventFilter( WG_EVENT_KEY_REPEAT, _filterKeyEvents, keycode );
}

//____ KeyPressNative() _______________________________________________________

WgEventFilter	WgEventFilter::KeyPressNative( int native_keycode )
{
	return WgEventFilter( WG_EVENT_KEY_PRESS, _filterNativeKeyEvents, native_keycode );
}

//____ KeyReleaseNative() _____________________________________________________

WgEventFilter	WgEventFilter::KeyReleaseNative( int native_keycode )
{
	return WgEventFilter( WG_EVENT_KEY_RELEASE, _filterNativeKeyEvents, native_keycode );
}

//____ KeyRepeatNative() ______________________________________________________

WgEventFilter	WgEventFilter::KeyRepeatNative( int native_keycode )
{
	return WgEventFilter( WG_EVENT_KEY_REPEAT, _filterNativeKeyEvents, native_keycode );
}

//____ Character() ____________________________________________________________

WgEventFilter	WgEventFilter::Character()
{
	return WgEventFilter( WG_EVENT_CHARACTER, _filterCharacterEvents, 0 );
}

WgEventFilter	WgEventFilter::Character( unsigned short character)
{
	return WgEventFilter( WG_EVENT_CHARACTER, _filterCharacterEvents, character );
}

//____ WheelRoll() ____________________________________________________________

WgEventFilter	WgEventFilter::WheelRoll()
{
	return WgEventFilter( WG_EVENT_WHEEL_ROLL, _filterWheelRollEvents, 0 );
}

WgEventFilter	WgEventFilter::WheelRoll( int wheel )
{
	return WgEventFilter( WG_EVENT_WHEEL_ROLL, _filterWheelRollEvents, wheel );
}

//____ Select() __________________________________________________________

WgEventFilter	WgEventFilter::Select()
{
	return WgEventFilter( WG_EVENT_SELECT, _filterType );
}

//____ Toggle() __________________________________________________________

WgEventFilter	WgEventFilter::Toggle()
{
	return WgEventFilter( WG_EVENT_TOGGLE, _filterType );
}

//____ ValueUpdate() _______________________________________________________

WgEventFilter	WgEventFilter::ValueUpdate()
{
	return WgEventFilter( WG_EVENT_VALUE_UPDATE, _filterType );
}

//____ RangeUpdate() __________________________________________________________

WgEventFilter	WgEventFilter::RangeUpdate()
{
	return WgEventFilter( WG_EVENT_RANGE_UPDATE, _filterType );
}

//____ TextEdit() _______________________________________________________

WgEventFilter	WgEventFilter::TextEdit()
{
	return WgEventFilter( WG_EVENT_TEXT_EDIT, _filterType );
}

//____ ItemToggle() ___________________________________________________________

WgEventFilter WgEventFilter::ItemToggle()
{
	return WgEventFilter( WG_EVENT_ITEM_TOGGLE, _filterType );
}

WgEventFilter WgEventFilter::ItemToggle( int id )
{
	return WgEventFilter( WG_EVENT_ITEM_TOGGLE, _filterItemToggleEvents, id, 0 );
}

//____ ItemMousePress() _______________________________________________________

WgEventFilter WgEventFilter::ItemMousePress()
{
	return WgEventFilter( WG_EVENT_ITEM_MOUSE_PRESS, _filterType );
}

WgEventFilter WgEventFilter::ItemMousePress( WgMouseButton button )
{
	return WgEventFilter( WG_EVENT_ITEM_MOUSE_PRESS, _filterItemMousePressEvents, -1, button );
}

WgEventFilter WgEventFilter::ItemMousePress( WgMouseButton button, int id )
{
	return WgEventFilter( WG_EVENT_ITEM_MOUSE_PRESS, _filterItemMousePressEvents, id, button );
}

//____ ItemsSelect() __________________________________________________________

WgEventFilter WgEventFilter::ItemsSelect()
{
	return WgEventFilter( WG_EVENT_ITEMS_SELECT, _filterType );
}

//____ ItemsUnselect() ________________________________________________________

WgEventFilter WgEventFilter::ItemsUnselect()
{
	return WgEventFilter( WG_EVENT_ITEMS_UNSELECT, _filterType );
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

bool WgEventFilter::_filterMouseButtonEvents( const WgEventPtr& pEvent, const WgEventFilter& filter )
{
	if( pEvent->Type() == filter.EventType() )
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
	if( pEvent->Type() == filter.EventType() )
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
	if( pEvent->Type() == filter.EventType() )
	{
		WgKeyEventPtr p = WgKeyEvent::Cast(pEvent);

		if( p->NativeKeyCode() == filter.m_data1 )
			return true;
	}
	return false;
}

bool WgEventFilter::_filterCharacterEvents( const WgEventPtr& pEvent, const WgEventFilter& filter )
{
	if( pEvent->Type() == filter.EventType() )
	{
		int chr = WgCharacterEvent::Cast(pEvent)->Char();

		if( chr == filter.m_data1 )
			return true;
	}
	return false;
}

bool WgEventFilter::_filterWheelRollEvents( const WgEventPtr& pEvent, const WgEventFilter& filter )
{
	if( pEvent->Type() == filter.EventType() )
	{
		int chr = WgWheelRollEvent::Cast(pEvent)->Wheel();

		if( chr == filter.m_data1 )
			return true;
	}
	return false;
}

bool WgEventFilter::_filterItemToggleEvents( const WgEventPtr& pEvent, const WgEventFilter& filter )
{
	if( pEvent->Type() == filter.EventType() )
	{
		int itemId = WgItemToggleEvent::Cast(pEvent)->ItemId();

		if( itemId == filter.m_data1 )
			return true;
	}
	return false;
}


bool WgEventFilter::_filterItemMousePressEvents( const WgEventPtr& pEvent, const WgEventFilter& filter )
{
	if( pEvent->Type() == filter.EventType() )
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
	if( pEvent->Type() == filter.EventType() )
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
