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
#include <wg_msg.h>
#include <wg_msgfilter.h>
#include <wg_widget.h>
#include <wg_scrollbar.h>
#include <wg_animplayer.h>
#include <wg_valueeditor.h>
#include <wg_menu.h>


//____ Tick() _________________________________________________________________

WgMsgFilter	WgMsgFilter::Tick()
{
	return WgMsgFilter( WG_MSG_TICK, _filterType );
}

//____ PointerChange() _________________________________________________________________

WgMsgFilter	WgMsgFilter::PointerChange()
{
	return WgMsgFilter( WG_MSG_POINTER_CHANGE, _filterType );
}

//____ FocusGained() __________________________________________________________

WgMsgFilter	WgMsgFilter::FocusGained()
{
	return WgMsgFilter( WG_MSG_FOCUS_GAINED, _filterType );
}

//____ FocusGained() __________________________________________________________

WgMsgFilter	WgMsgFilter::FocusLost()
{
	return WgMsgFilter( WG_MSG_FOCUS_LOST, _filterType );
}

//____ MouseEnter() _________________________________________________________

WgMsgFilter	WgMsgFilter::MouseEnter()
{
	return WgMsgFilter( WG_MSG_MOUSE_ENTER, _filterType );
}

//____ MouseLeave() _________________________________________________________

WgMsgFilter	WgMsgFilter::MouseLeave()
{
	return WgMsgFilter( WG_MSG_MOUSE_LEAVE, _filterType );
}

//____ MouseMove() _________________________________________________________

WgMsgFilter	WgMsgFilter::MouseMove()
{
	return WgMsgFilter( WG_MSG_MOUSE_MOVE, _filterType );
}

//____ MousePress() _________________________________________________________

WgMsgFilter	WgMsgFilter::MousePress()
{
	return WgMsgFilter( WG_MSG_MOUSE_PRESS, _filterMouseButtonMsgs, 0 );
}

WgMsgFilter	WgMsgFilter::MousePress( WgMouseButton button)
{
	return WgMsgFilter( WG_MSG_MOUSE_PRESS, _filterMouseButtonMsgs, button );
}

//____ MouseRelease() ________________________________________________________

WgMsgFilter	WgMsgFilter::MouseRelease()
{
	return WgMsgFilter( WG_MSG_MOUSE_RELEASE, _filterMouseButtonMsgs, 0 );
}

WgMsgFilter	WgMsgFilter::MouseRelease( WgMouseButton button)
{
	return WgMsgFilter( WG_MSG_MOUSE_RELEASE, _filterMouseButtonMsgs, button );
}

//____ MouseDrag() ___________________________________________________________

WgMsgFilter	WgMsgFilter::MouseDrag()
{
	return WgMsgFilter( WG_MSG_MOUSE_DRAG, _filterMouseButtonMsgs, 0 );
}

WgMsgFilter	WgMsgFilter::MouseDrag( WgMouseButton button)
{
	return WgMsgFilter( WG_MSG_MOUSE_DRAG, _filterMouseButtonMsgs, button );
}

//____ MouseRepeat() ________________________________________________________

WgMsgFilter	WgMsgFilter::MouseRepeat()
{
	return WgMsgFilter( WG_MSG_MOUSE_REPEAT, _filterMouseButtonMsgs, 0 );
}

WgMsgFilter	WgMsgFilter::MouseRepeat( WgMouseButton button)
{
	return WgMsgFilter( WG_MSG_MOUSE_REPEAT, _filterMouseButtonMsgs, button );
}

//____ MouseClick() ________________________________________________________

WgMsgFilter	WgMsgFilter::MouseClick()
{
	return WgMsgFilter( WG_MSG_MOUSE_CLICK, _filterMouseButtonMsgs, 0 );
}

WgMsgFilter	WgMsgFilter::MouseClick( WgMouseButton button)
{
	return WgMsgFilter( WG_MSG_MOUSE_CLICK, _filterMouseButtonMsgs, button );
}

//____ MouseDoubleClick() ________________________________________________________

WgMsgFilter	WgMsgFilter::MouseDoubleClick()
{
	return WgMsgFilter( WG_MSG_MOUSE_CLICK, _filterMouseButtonMsgs, 0 );
}

WgMsgFilter	WgMsgFilter::MouseDoubleClick( WgMouseButton button)
{
	return WgMsgFilter( WG_MSG_MOUSE_CLICK, _filterMouseButtonMsgs, button );
}

//____ KeyPress() _____________________________________________________________

WgMsgFilter	WgMsgFilter::KeyPress()
{
	return WgMsgFilter( WG_MSG_KEY_PRESS, _filterKeyMsgs, WG_KEY_UNMAPPED );
}

WgMsgFilter	WgMsgFilter::KeyPress( WgKey keycode )
{
	return WgMsgFilter( WG_MSG_KEY_PRESS, _filterKeyMsgs, keycode );
}

//____ KeyRelease() ___________________________________________________________

WgMsgFilter	WgMsgFilter::KeyRelease()
{
	return WgMsgFilter( WG_MSG_KEY_RELEASE, _filterKeyMsgs, WG_KEY_UNMAPPED );
}

WgMsgFilter	WgMsgFilter::KeyRelease( WgKey keycode )
{
	return WgMsgFilter( WG_MSG_KEY_RELEASE, _filterKeyMsgs, keycode );
}

//____ KeyRepeat() ____________________________________________________________

WgMsgFilter	WgMsgFilter::KeyRepeat()
{
	return WgMsgFilter( WG_MSG_KEY_REPEAT, _filterKeyMsgs, WG_KEY_UNMAPPED );
}

WgMsgFilter	WgMsgFilter::KeyRepeat( WgKey keycode )
{
	return WgMsgFilter( WG_MSG_KEY_REPEAT, _filterKeyMsgs, keycode );
}

//____ KeyPressNative() _______________________________________________________

WgMsgFilter	WgMsgFilter::KeyPressNative( int native_keycode )
{
	return WgMsgFilter( WG_MSG_KEY_PRESS, _filterNativeKeyMsgs, native_keycode );
}

//____ KeyReleaseNative() _____________________________________________________

WgMsgFilter	WgMsgFilter::KeyReleaseNative( int native_keycode )
{
	return WgMsgFilter( WG_MSG_KEY_RELEASE, _filterNativeKeyMsgs, native_keycode );
}

//____ KeyRepeatNative() ______________________________________________________

WgMsgFilter	WgMsgFilter::KeyRepeatNative( int native_keycode )
{
	return WgMsgFilter( WG_MSG_KEY_REPEAT, _filterNativeKeyMsgs, native_keycode );
}

//____ Character() ____________________________________________________________

WgMsgFilter	WgMsgFilter::Character()
{
	return WgMsgFilter( WG_MSG_CHARACTER, _filterCharacterMsgs, 0 );
}

WgMsgFilter	WgMsgFilter::Character( unsigned short character)
{
	return WgMsgFilter( WG_MSG_CHARACTER, _filterCharacterMsgs, character );
}

//____ WheelRoll() ____________________________________________________________

WgMsgFilter	WgMsgFilter::WheelRoll()
{
	return WgMsgFilter( WG_MSG_WHEEL_ROLL, _filterWheelRollMsgs, 0 );
}

WgMsgFilter	WgMsgFilter::WheelRoll( int wheel )
{
	return WgMsgFilter( WG_MSG_WHEEL_ROLL, _filterWheelRollMsgs, wheel );
}

//____ Select() __________________________________________________________

WgMsgFilter	WgMsgFilter::Select()
{
	return WgMsgFilter( WG_MSG_SELECT, _filterType );
}

//____ Toggle() __________________________________________________________

WgMsgFilter	WgMsgFilter::Toggle()
{
	return WgMsgFilter( WG_MSG_TOGGLE, _filterType );
}

//____ ValueUpdate() _______________________________________________________

WgMsgFilter	WgMsgFilter::ValueUpdate()
{
	return WgMsgFilter( WG_MSG_VALUE_UPDATE, _filterType );
}

//____ RangeUpdate() __________________________________________________________

WgMsgFilter	WgMsgFilter::RangeUpdate()
{
	return WgMsgFilter( WG_MSG_RANGE_UPDATE, _filterType );
}

//____ TextEdit() _______________________________________________________

WgMsgFilter	WgMsgFilter::TextEdit()
{
	return WgMsgFilter( WG_MSG_TEXT_EDIT, _filterType );
}

//____ ItemToggle() ___________________________________________________________

WgMsgFilter WgMsgFilter::ItemToggle()
{
	return WgMsgFilter( WG_MSG_ITEM_TOGGLE, _filterType );
}

WgMsgFilter WgMsgFilter::ItemToggle( int id )
{
	return WgMsgFilter( WG_MSG_ITEM_TOGGLE, _filterItemToggleMsgs, id, 0 );
}

//____ ItemMousePress() _______________________________________________________

WgMsgFilter WgMsgFilter::ItemMousePress()
{
	return WgMsgFilter( WG_MSG_ITEM_MOUSE_PRESS, _filterType );
}

WgMsgFilter WgMsgFilter::ItemMousePress( WgMouseButton button )
{
	return WgMsgFilter( WG_MSG_ITEM_MOUSE_PRESS, _filterItemMousePressMsgs, -1, button );
}

WgMsgFilter WgMsgFilter::ItemMousePress( WgMouseButton button, int id )
{
	return WgMsgFilter( WG_MSG_ITEM_MOUSE_PRESS, _filterItemMousePressMsgs, id, button );
}

//____ ItemsSelect() __________________________________________________________

WgMsgFilter WgMsgFilter::ItemsSelect()
{
	return WgMsgFilter( WG_MSG_ITEMS_SELECT, _filterType );
}

//____ ItemsUnselect() ________________________________________________________

WgMsgFilter WgMsgFilter::ItemsUnselect()
{
	return WgMsgFilter( WG_MSG_ITEMS_UNSELECT, _filterType );
}

/*
//____ MenuitemSelect() __________________________________________________________

WgMsgFilter	WgMsgFilter::MenuitemSelect()
{
	return WgMsgFilter( WG_MSG_MENUITEM_SELECT, 0, _filterMenuitemMsg );
}

WgMsgFilter	WgMsgFilter::MenuitemSelect( int itemId )
{
	return WgMsgFilter( WG_MSG_MENUITEM_SELECT, 0, _filterMenuitemMsg, itemId );
}

WgMsgFilter	WgMsgFilter::MenuitemSelect( const WgWidgetPtr& pWidget )
{
	return WgMsgFilter( WG_MSG_MENUITEM_SELECT, pWidget.RawPtr(), _filterMenuitemMsg );
}

WgMsgFilter	WgMsgFilter::MenuitemSelect( const WgWidgetPtr& pWidget, int itemId )
{
	return WgMsgFilter( WG_MSG_MENUITEM_SELECT, pWidget.RawPtr(), _filterMenuitemMsg, itemId );
}
*/

//_____________________________________________________________________________

bool WgMsgFilter::_filterDummy( const WgMsgPtr& pMsg, const WgMsgFilter& filter )
{
	return true;
}

bool WgMsgFilter::_filterType( const WgMsgPtr& pMsg, const WgMsgFilter& filter )
{
	if( pMsg->Type() == filter.MsgType() )
		return true;

	return false;
}

bool WgMsgFilter::_filterMouseButtonMsgs( const WgMsgPtr& pMsg, const WgMsgFilter& filter )
{
	if( pMsg->Type() == filter.MsgType() )
	{
		if( filter.m_data1 == 0 )
			return true;
		else
		{
			WgMouseButtonMsgPtr p = WgMouseButtonMsg::Cast(pMsg);

			if( p->Button() == filter.m_data1 )
				return true;
		}
	}
	return false;
}

bool WgMsgFilter::_filterKeyMsgs( const WgMsgPtr& pMsg, const WgMsgFilter& filter )
{
	if( pMsg->Type() == filter.MsgType() )
	{
		if( filter.m_data1 == 0 )
			return true;
		else
		{
			WgKeyMsgPtr p = WgKeyMsg::Cast(pMsg);

			if( p->TranslatedKeyCode() == filter.m_data1 )
				return true;
		}
	}
	return false;
}

bool WgMsgFilter::_filterNativeKeyMsgs( const WgMsgPtr& pMsg, const WgMsgFilter& filter )
{
	if( pMsg->Type() == filter.MsgType() )
	{
		WgKeyMsgPtr p = WgKeyMsg::Cast(pMsg);

		if( p->NativeKeyCode() == filter.m_data1 )
			return true;
	}
	return false;
}

bool WgMsgFilter::_filterCharacterMsgs( const WgMsgPtr& pMsg, const WgMsgFilter& filter )
{
	if( pMsg->Type() == filter.MsgType() )
	{
		int chr = WgCharacterMsg::Cast(pMsg)->Char();

		if( chr == filter.m_data1 )
			return true;
	}
	return false;
}

bool WgMsgFilter::_filterWheelRollMsgs( const WgMsgPtr& pMsg, const WgMsgFilter& filter )
{
	if( pMsg->Type() == filter.MsgType() )
	{
		int chr = WgWheelRollMsg::Cast(pMsg)->Wheel();

		if( chr == filter.m_data1 )
			return true;
	}
	return false;
}

bool WgMsgFilter::_filterItemToggleMsgs( const WgMsgPtr& pMsg, const WgMsgFilter& filter )
{
	if( pMsg->Type() == filter.MsgType() )
	{
		int itemId = WgItemToggleMsg::Cast(pMsg)->ItemId();

		if( itemId == filter.m_data1 )
			return true;
	}
	return false;
}


bool WgMsgFilter::_filterItemMousePressMsgs( const WgMsgPtr& pMsg, const WgMsgFilter& filter )
{
	if( pMsg->Type() == filter.MsgType() )
	{
		WgItemMousePressMsgPtr pMsg = WgItemMousePressMsg::Cast(pMsg);

		if( (filter.m_data1 == -1 || pMsg->ItemId() == filter.m_data1) ||
			(filter.m_data2 == -1 || pMsg->Button() == filter.m_data2) )
			return true;
	}
	return false;
}



/*
bool WgMsgFilter::_filterMenuitemMsg( const WgMsgPtr& pMsg, const WgMsgFilter& filter )
{
	if( pMsg->Type() == filter.MsgType() )
	{
		if( filter.m_data1 == 0 )
			return true;
		else
		{
			const WgMsg::MenuitemMsg * p = static_cast<const WgMsg::MenuitemMsg*>(pMsg);

			if( p->ItemId() == filter.m_data1 )
				return true;
		}
	}
	return false;
}
*/
