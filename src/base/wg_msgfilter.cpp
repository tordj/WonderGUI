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


//____ tick() _________________________________________________________________

WgMsgFilter	WgMsgFilter::tick()
{
	return WgMsgFilter( WG_MSG_TICK, _filterType );
}

//____ pointerChange() _________________________________________________________________

WgMsgFilter	WgMsgFilter::pointerChange()
{
	return WgMsgFilter( WG_MSG_POINTER_CHANGE, _filterType );
}

//____ focusGained() __________________________________________________________

WgMsgFilter	WgMsgFilter::focusGained()
{
	return WgMsgFilter( WG_MSG_FOCUS_GAINED, _filterType );
}

//____ focusGained() __________________________________________________________

WgMsgFilter	WgMsgFilter::focusLost()
{
	return WgMsgFilter( WG_MSG_FOCUS_LOST, _filterType );
}

//____ mouseEnter() _________________________________________________________

WgMsgFilter	WgMsgFilter::mouseEnter()
{
	return WgMsgFilter( WG_MSG_MOUSE_ENTER, _filterType );
}

//____ mouseLeave() _________________________________________________________

WgMsgFilter	WgMsgFilter::mouseLeave()
{
	return WgMsgFilter( WG_MSG_MOUSE_LEAVE, _filterType );
}

//____ mouseMove() _________________________________________________________

WgMsgFilter	WgMsgFilter::mouseMove()
{
	return WgMsgFilter( WG_MSG_MOUSE_MOVE, _filterType );
}

//____ mousePress() _________________________________________________________

WgMsgFilter	WgMsgFilter::mousePress()
{
	return WgMsgFilter( WG_MSG_MOUSE_PRESS, _filterMouseButtonMsgs, 0 );
}

WgMsgFilter	WgMsgFilter::mousePress( WgMouseButton button)
{
	return WgMsgFilter( WG_MSG_MOUSE_PRESS, _filterMouseButtonMsgs, button );
}

//____ mouseRelease() ________________________________________________________

WgMsgFilter	WgMsgFilter::mouseRelease()
{
	return WgMsgFilter( WG_MSG_MOUSE_RELEASE, _filterMouseButtonMsgs, 0 );
}

WgMsgFilter	WgMsgFilter::mouseRelease( WgMouseButton button)
{
	return WgMsgFilter( WG_MSG_MOUSE_RELEASE, _filterMouseButtonMsgs, button );
}

//____ mouseDrag() ___________________________________________________________

WgMsgFilter	WgMsgFilter::mouseDrag()
{
	return WgMsgFilter( WG_MSG_MOUSE_DRAG, _filterMouseButtonMsgs, 0 );
}

WgMsgFilter	WgMsgFilter::mouseDrag( WgMouseButton button)
{
	return WgMsgFilter( WG_MSG_MOUSE_DRAG, _filterMouseButtonMsgs, button );
}

//____ mouseRepeat() ________________________________________________________

WgMsgFilter	WgMsgFilter::mouseRepeat()
{
	return WgMsgFilter( WG_MSG_MOUSE_REPEAT, _filterMouseButtonMsgs, 0 );
}

WgMsgFilter	WgMsgFilter::mouseRepeat( WgMouseButton button)
{
	return WgMsgFilter( WG_MSG_MOUSE_REPEAT, _filterMouseButtonMsgs, button );
}

//____ mouseClick() ________________________________________________________

WgMsgFilter	WgMsgFilter::mouseClick()
{
	return WgMsgFilter( WG_MSG_MOUSE_CLICK, _filterMouseButtonMsgs, 0 );
}

WgMsgFilter	WgMsgFilter::mouseClick( WgMouseButton button)
{
	return WgMsgFilter( WG_MSG_MOUSE_CLICK, _filterMouseButtonMsgs, button );
}

//____ mouseDoubleClick() ________________________________________________________

WgMsgFilter	WgMsgFilter::mouseDoubleClick()
{
	return WgMsgFilter( WG_MSG_MOUSE_CLICK, _filterMouseButtonMsgs, 0 );
}

WgMsgFilter	WgMsgFilter::mouseDoubleClick( WgMouseButton button)
{
	return WgMsgFilter( WG_MSG_MOUSE_CLICK, _filterMouseButtonMsgs, button );
}

//____ keyPress() _____________________________________________________________

WgMsgFilter	WgMsgFilter::keyPress()
{
	return WgMsgFilter( WG_MSG_KEY_PRESS, _filterKeyMsgs, WG_KEY_UNMAPPED );
}

WgMsgFilter	WgMsgFilter::keyPress( WgKey keycode )
{
	return WgMsgFilter( WG_MSG_KEY_PRESS, _filterKeyMsgs, keycode );
}

//____ keyRelease() ___________________________________________________________

WgMsgFilter	WgMsgFilter::keyRelease()
{
	return WgMsgFilter( WG_MSG_KEY_RELEASE, _filterKeyMsgs, WG_KEY_UNMAPPED );
}

WgMsgFilter	WgMsgFilter::keyRelease( WgKey keycode )
{
	return WgMsgFilter( WG_MSG_KEY_RELEASE, _filterKeyMsgs, keycode );
}

//____ keyRepeat() ____________________________________________________________

WgMsgFilter	WgMsgFilter::keyRepeat()
{
	return WgMsgFilter( WG_MSG_KEY_REPEAT, _filterKeyMsgs, WG_KEY_UNMAPPED );
}

WgMsgFilter	WgMsgFilter::keyRepeat( WgKey keycode )
{
	return WgMsgFilter( WG_MSG_KEY_REPEAT, _filterKeyMsgs, keycode );
}

//____ keyPressNative() _______________________________________________________

WgMsgFilter	WgMsgFilter::keyPressNative( int native_keycode )
{
	return WgMsgFilter( WG_MSG_KEY_PRESS, _filterNativeKeyMsgs, native_keycode );
}

//____ keyReleaseNative() _____________________________________________________

WgMsgFilter	WgMsgFilter::keyReleaseNative( int native_keycode )
{
	return WgMsgFilter( WG_MSG_KEY_RELEASE, _filterNativeKeyMsgs, native_keycode );
}

//____ keyRepeatNative() ______________________________________________________

WgMsgFilter	WgMsgFilter::keyRepeatNative( int native_keycode )
{
	return WgMsgFilter( WG_MSG_KEY_REPEAT, _filterNativeKeyMsgs, native_keycode );
}

//____ character() ____________________________________________________________

WgMsgFilter	WgMsgFilter::character()
{
	return WgMsgFilter( WG_MSG_CHARACTER, _filterCharacterMsgs, 0 );
}

WgMsgFilter	WgMsgFilter::character( unsigned short character)
{
	return WgMsgFilter( WG_MSG_CHARACTER, _filterCharacterMsgs, character );
}

//____ wheelRoll() ____________________________________________________________

WgMsgFilter	WgMsgFilter::wheelRoll()
{
	return WgMsgFilter( WG_MSG_WHEEL_ROLL, _filterWheelRollMsgs, 0 );
}

WgMsgFilter	WgMsgFilter::wheelRoll( int wheel )
{
	return WgMsgFilter( WG_MSG_WHEEL_ROLL, _filterWheelRollMsgs, wheel );
}

//____ select() __________________________________________________________

WgMsgFilter	WgMsgFilter::select()
{
	return WgMsgFilter( WG_MSG_SELECT, _filterType );
}

//____ toggle() __________________________________________________________

WgMsgFilter	WgMsgFilter::toggle()
{
	return WgMsgFilter( WG_MSG_TOGGLE, _filterType );
}

//____ valueUpdate() _______________________________________________________

WgMsgFilter	WgMsgFilter::valueUpdate()
{
	return WgMsgFilter( WG_MSG_VALUE_UPDATE, _filterType );
}

//____ rangeUpdate() __________________________________________________________

WgMsgFilter	WgMsgFilter::rangeUpdate()
{
	return WgMsgFilter( WG_MSG_RANGE_UPDATE, _filterType );
}

//____ textEdit() _______________________________________________________

WgMsgFilter	WgMsgFilter::textEdit()
{
	return WgMsgFilter( WG_MSG_TEXT_EDIT, _filterType );
}

//____ itemToggle() ___________________________________________________________

WgMsgFilter WgMsgFilter::itemToggle()
{
	return WgMsgFilter( WG_MSG_ITEM_TOGGLE, _filterType );
}

WgMsgFilter WgMsgFilter::itemToggle( int id )
{
	return WgMsgFilter( WG_MSG_ITEM_TOGGLE, _filterItemToggleMsgs, id, 0 );
}

//____ itemMousePress() _______________________________________________________

WgMsgFilter WgMsgFilter::itemMousePress()
{
	return WgMsgFilter( WG_MSG_ITEM_MOUSE_PRESS, _filterType );
}

WgMsgFilter WgMsgFilter::itemMousePress( WgMouseButton button )
{
	return WgMsgFilter( WG_MSG_ITEM_MOUSE_PRESS, _filterItemMousePressMsgs, -1, button );
}

WgMsgFilter WgMsgFilter::itemMousePress( WgMouseButton button, int id )
{
	return WgMsgFilter( WG_MSG_ITEM_MOUSE_PRESS, _filterItemMousePressMsgs, id, button );
}

//____ itemsSelect() __________________________________________________________

WgMsgFilter WgMsgFilter::itemsSelect()
{
	return WgMsgFilter( WG_MSG_ITEMS_SELECT, _filterType );
}

//____ itemsUnselect() ________________________________________________________

WgMsgFilter WgMsgFilter::itemsUnselect()
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
	return WgMsgFilter( WG_MSG_MENUITEM_SELECT, pWidget.rawPtr(), _filterMenuitemMsg );
}

WgMsgFilter	WgMsgFilter::MenuitemSelect( const WgWidgetPtr& pWidget, int itemId )
{
	return WgMsgFilter( WG_MSG_MENUITEM_SELECT, pWidget.rawPtr(), _filterMenuitemMsg, itemId );
}
*/

//_____________________________________________________________________________

bool WgMsgFilter::_filterDummy( const WgMsgPtr& pMsg, const WgMsgFilter& filter )
{
	return true;
}

bool WgMsgFilter::_filterType( const WgMsgPtr& pMsg, const WgMsgFilter& filter )
{
	if( pMsg->type() == filter.msgType() )
		return true;

	return false;
}

bool WgMsgFilter::_filterMouseButtonMsgs( const WgMsgPtr& pMsg, const WgMsgFilter& filter )
{
	if( pMsg->type() == filter.msgType() )
	{
		if( filter.m_data1 == 0 )
			return true;
		else
		{
			WgMouseButtonMsgPtr p = WgMouseButtonMsg::cast(pMsg);

			if( p->button() == filter.m_data1 )
				return true;
		}
	}
	return false;
}

bool WgMsgFilter::_filterKeyMsgs( const WgMsgPtr& pMsg, const WgMsgFilter& filter )
{
	if( pMsg->type() == filter.msgType() )
	{
		if( filter.m_data1 == 0 )
			return true;
		else
		{
			WgKeyMsgPtr p = WgKeyMsg::cast(pMsg);

			if( p->translatedKeyCode() == filter.m_data1 )
				return true;
		}
	}
	return false;
}

bool WgMsgFilter::_filterNativeKeyMsgs( const WgMsgPtr& pMsg, const WgMsgFilter& filter )
{
	if( pMsg->type() == filter.msgType() )
	{
		WgKeyMsgPtr p = WgKeyMsg::cast(pMsg);

		if( p->nativeKeyCode() == filter.m_data1 )
			return true;
	}
	return false;
}

bool WgMsgFilter::_filterCharacterMsgs( const WgMsgPtr& pMsg, const WgMsgFilter& filter )
{
	if( pMsg->type() == filter.msgType() )
	{
		int chr = WgCharacterMsg::cast(pMsg)->character();

		if( chr == filter.m_data1 )
			return true;
	}
	return false;
}

bool WgMsgFilter::_filterWheelRollMsgs( const WgMsgPtr& pMsg, const WgMsgFilter& filter )
{
	if( pMsg->type() == filter.msgType() )
	{
		int chr = WgWheelRollMsg::cast(pMsg)->wheel();

		if( chr == filter.m_data1 )
			return true;
	}
	return false;
}

bool WgMsgFilter::_filterItemToggleMsgs( const WgMsgPtr& pMsg, const WgMsgFilter& filter )
{
	if( pMsg->type() == filter.msgType() )
	{
		int itemId = WgItemToggleMsg::cast(pMsg)->itemId();

		if( itemId == filter.m_data1 )
			return true;
	}
	return false;
}


bool WgMsgFilter::_filterItemMousePressMsgs( const WgMsgPtr& pMsg, const WgMsgFilter& filter )
{
	if( pMsg->type() == filter.msgType() )
	{
		WgItemMousePressMsgPtr pMsg = WgItemMousePressMsg::cast(pMsg);

		if( (filter.m_data1 == -1 || pMsg->itemId() == filter.m_data1) ||
			(filter.m_data2 == -1 || pMsg->button() == filter.m_data2) )
			return true;
	}
	return false;
}



/*
bool WgMsgFilter::_filterMenuitemMsg( const WgMsgPtr& pMsg, const WgMsgFilter& filter )
{
	if( pMsg->type() == filter.msgType() )
	{
		if( filter.m_data1 == 0 )
			return true;
		else
		{
			const WgMsg::MenuitemMsg * p = static_cast<const WgMsg::MenuitemMsg*>(pMsg);

			if( p->itemId() == filter.m_data1 )
				return true;
		}
	}
	return false;
}
*/
