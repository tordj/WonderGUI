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

namespace wg 
{
	
	
	//____ tick() _________________________________________________________________
	
	MsgFilter	MsgFilter::tick()
	{
		return MsgFilter( WG_MSG_TICK, _filterType );
	}
	
	//____ pointerChange() _________________________________________________________________
	
	MsgFilter	MsgFilter::pointerChange()
	{
		return MsgFilter( WG_MSG_POINTER_CHANGE, _filterType );
	}
	
	//____ focusGained() __________________________________________________________
	
	MsgFilter	MsgFilter::focusGained()
	{
		return MsgFilter( WG_MSG_FOCUS_GAINED, _filterType );
	}
	
	//____ focusGained() __________________________________________________________
	
	MsgFilter	MsgFilter::focusLost()
	{
		return MsgFilter( WG_MSG_FOCUS_LOST, _filterType );
	}
	
	//____ mouseEnter() _________________________________________________________
	
	MsgFilter	MsgFilter::mouseEnter()
	{
		return MsgFilter( WG_MSG_MOUSE_ENTER, _filterType );
	}
	
	//____ mouseLeave() _________________________________________________________
	
	MsgFilter	MsgFilter::mouseLeave()
	{
		return MsgFilter( WG_MSG_MOUSE_LEAVE, _filterType );
	}
	
	//____ mouseMove() _________________________________________________________
	
	MsgFilter	MsgFilter::mouseMove()
	{
		return MsgFilter( WG_MSG_MOUSE_MOVE, _filterType );
	}
	
	//____ mousePress() _________________________________________________________
	
	MsgFilter	MsgFilter::mousePress()
	{
		return MsgFilter( WG_MSG_MOUSE_PRESS, _filterMouseButtonMsgs, 0 );
	}
	
	MsgFilter	MsgFilter::mousePress( WgMouseButton button)
	{
		return MsgFilter( WG_MSG_MOUSE_PRESS, _filterMouseButtonMsgs, button );
	}
	
	//____ mouseRelease() ________________________________________________________
	
	MsgFilter	MsgFilter::mouseRelease()
	{
		return MsgFilter( WG_MSG_MOUSE_RELEASE, _filterMouseButtonMsgs, 0 );
	}
	
	MsgFilter	MsgFilter::mouseRelease( WgMouseButton button)
	{
		return MsgFilter( WG_MSG_MOUSE_RELEASE, _filterMouseButtonMsgs, button );
	}
	
	//____ mouseDrag() ___________________________________________________________
	
	MsgFilter	MsgFilter::mouseDrag()
	{
		return MsgFilter( WG_MSG_MOUSE_DRAG, _filterMouseButtonMsgs, 0 );
	}
	
	MsgFilter	MsgFilter::mouseDrag( WgMouseButton button)
	{
		return MsgFilter( WG_MSG_MOUSE_DRAG, _filterMouseButtonMsgs, button );
	}
	
	//____ mouseRepeat() ________________________________________________________
	
	MsgFilter	MsgFilter::mouseRepeat()
	{
		return MsgFilter( WG_MSG_MOUSE_REPEAT, _filterMouseButtonMsgs, 0 );
	}
	
	MsgFilter	MsgFilter::mouseRepeat( WgMouseButton button)
	{
		return MsgFilter( WG_MSG_MOUSE_REPEAT, _filterMouseButtonMsgs, button );
	}
	
	//____ mouseClick() ________________________________________________________
	
	MsgFilter	MsgFilter::mouseClick()
	{
		return MsgFilter( WG_MSG_MOUSE_CLICK, _filterMouseButtonMsgs, 0 );
	}
	
	MsgFilter	MsgFilter::mouseClick( WgMouseButton button)
	{
		return MsgFilter( WG_MSG_MOUSE_CLICK, _filterMouseButtonMsgs, button );
	}
	
	//____ mouseDoubleClick() ________________________________________________________
	
	MsgFilter	MsgFilter::mouseDoubleClick()
	{
		return MsgFilter( WG_MSG_MOUSE_CLICK, _filterMouseButtonMsgs, 0 );
	}
	
	MsgFilter	MsgFilter::mouseDoubleClick( WgMouseButton button)
	{
		return MsgFilter( WG_MSG_MOUSE_CLICK, _filterMouseButtonMsgs, button );
	}
	
	//____ keyPress() _____________________________________________________________
	
	MsgFilter	MsgFilter::keyPress()
	{
		return MsgFilter( WG_MSG_KEY_PRESS, _filterKeyMsgs, WG_KEY_UNMAPPED );
	}
	
	MsgFilter	MsgFilter::keyPress( WgKey keycode )
	{
		return MsgFilter( WG_MSG_KEY_PRESS, _filterKeyMsgs, keycode );
	}
	
	//____ keyRelease() ___________________________________________________________
	
	MsgFilter	MsgFilter::keyRelease()
	{
		return MsgFilter( WG_MSG_KEY_RELEASE, _filterKeyMsgs, WG_KEY_UNMAPPED );
	}
	
	MsgFilter	MsgFilter::keyRelease( WgKey keycode )
	{
		return MsgFilter( WG_MSG_KEY_RELEASE, _filterKeyMsgs, keycode );
	}
	
	//____ keyRepeat() ____________________________________________________________
	
	MsgFilter	MsgFilter::keyRepeat()
	{
		return MsgFilter( WG_MSG_KEY_REPEAT, _filterKeyMsgs, WG_KEY_UNMAPPED );
	}
	
	MsgFilter	MsgFilter::keyRepeat( WgKey keycode )
	{
		return MsgFilter( WG_MSG_KEY_REPEAT, _filterKeyMsgs, keycode );
	}
	
	//____ keyPressNative() _______________________________________________________
	
	MsgFilter	MsgFilter::keyPressNative( int native_keycode )
	{
		return MsgFilter( WG_MSG_KEY_PRESS, _filterNativeKeyMsgs, native_keycode );
	}
	
	//____ keyReleaseNative() _____________________________________________________
	
	MsgFilter	MsgFilter::keyReleaseNative( int native_keycode )
	{
		return MsgFilter( WG_MSG_KEY_RELEASE, _filterNativeKeyMsgs, native_keycode );
	}
	
	//____ keyRepeatNative() ______________________________________________________
	
	MsgFilter	MsgFilter::keyRepeatNative( int native_keycode )
	{
		return MsgFilter( WG_MSG_KEY_REPEAT, _filterNativeKeyMsgs, native_keycode );
	}
	
	//____ textInput() ____________________________________________________________
	
	MsgFilter	MsgFilter::textInput()
	{
		return MsgFilter( WG_MSG_TEXT_INPUT, _filterType );
	}
	
	//____ wheelRoll() ____________________________________________________________
	
	MsgFilter	MsgFilter::wheelRoll()
	{
		return MsgFilter( WG_MSG_WHEEL_ROLL, _filterWheelRollMsgs, 0 );
	}
	
	MsgFilter	MsgFilter::wheelRoll( int wheel )
	{
		return MsgFilter( WG_MSG_WHEEL_ROLL, _filterWheelRollMsgs, wheel );
	}
	
	//____ select() __________________________________________________________
	
	MsgFilter	MsgFilter::select()
	{
		return MsgFilter( WG_MSG_SELECT, _filterType );
	}
	
	//____ toggle() __________________________________________________________
	
	MsgFilter	MsgFilter::toggle()
	{
		return MsgFilter( WG_MSG_TOGGLE, _filterType );
	}
	
	//____ valueUpdate() _______________________________________________________
	
	MsgFilter	MsgFilter::valueUpdate()
	{
		return MsgFilter( WG_MSG_VALUE_UPDATE, _filterType );
	}
	
	//____ rangeUpdate() __________________________________________________________
	
	MsgFilter	MsgFilter::rangeUpdate()
	{
		return MsgFilter( WG_MSG_RANGE_UPDATE, _filterType );
	}
	
	//____ textEdit() _______________________________________________________
	
	MsgFilter	MsgFilter::textEdit()
	{
		return MsgFilter( WG_MSG_TEXT_EDIT, _filterType );
	}
	
	//____ itemToggle() ___________________________________________________________
	
	MsgFilter MsgFilter::itemToggle()
	{
		return MsgFilter( WG_MSG_ITEM_TOGGLE, _filterType );
	}
	
	MsgFilter MsgFilter::itemToggle( int id )
	{
		return MsgFilter( WG_MSG_ITEM_TOGGLE, _filterItemToggleMsgs, id, 0 );
	}
	
	//____ itemMousePress() _______________________________________________________
	
	MsgFilter MsgFilter::itemMousePress()
	{
		return MsgFilter( WG_MSG_ITEM_MOUSE_PRESS, _filterType );
	}
	
	MsgFilter MsgFilter::itemMousePress( WgMouseButton button )
	{
		return MsgFilter( WG_MSG_ITEM_MOUSE_PRESS, _filterItemMousePressMsgs, -1, button );
	}
	
	MsgFilter MsgFilter::itemMousePress( WgMouseButton button, int id )
	{
		return MsgFilter( WG_MSG_ITEM_MOUSE_PRESS, _filterItemMousePressMsgs, id, button );
	}
	
	//____ itemsSelect() __________________________________________________________
	
	MsgFilter MsgFilter::itemsSelect()
	{
		return MsgFilter( WG_MSG_ITEMS_SELECT, _filterType );
	}
	
	//____ itemsUnselect() ________________________________________________________
	
	MsgFilter MsgFilter::itemsUnselect()
	{
		return MsgFilter( WG_MSG_ITEMS_UNSELECT, _filterType );
	}
	
	/*
	//____ MenuitemSelect() __________________________________________________________
	
	MsgFilter	MsgFilter::MenuitemSelect()
	{
		return MsgFilter( WG_MSG_MENUITEM_SELECT, 0, _filterMenuitemMsg );
	}
	
	MsgFilter	MsgFilter::MenuitemSelect( int itemId )
	{
		return MsgFilter( WG_MSG_MENUITEM_SELECT, 0, _filterMenuitemMsg, itemId );
	}
	
	MsgFilter	MsgFilter::MenuitemSelect( const Widget_p& pWidget )
	{
		return MsgFilter( WG_MSG_MENUITEM_SELECT, pWidget.rawPtr(), _filterMenuitemMsg );
	}
	
	MsgFilter	MsgFilter::MenuitemSelect( const Widget_p& pWidget, int itemId )
	{
		return MsgFilter( WG_MSG_MENUITEM_SELECT, pWidget.rawPtr(), _filterMenuitemMsg, itemId );
	}
	*/
	
	//_____________________________________________________________________________
	
	bool MsgFilter::_filterDummy( const Msg_p& pMsg, const MsgFilter& filter )
	{
		return true;
	}
	
	bool MsgFilter::_filterType( const Msg_p& pMsg, const MsgFilter& filter )
	{
		if( pMsg->type() == filter.msgType() )
			return true;
	
		return false;
	}
	
	bool MsgFilter::_filterMouseButtonMsgs( const Msg_p& pMsg, const MsgFilter& filter )
	{
		if( pMsg->type() == filter.msgType() )
		{
			if( filter.m_data1 == 0 )
				return true;
			else
			{
				MouseButtonMsg_p p = MouseButtonMsg::cast(pMsg);
	
				if( p->button() == filter.m_data1 )
					return true;
			}
		}
		return false;
	}
	
	bool MsgFilter::_filterKeyMsgs( const Msg_p& pMsg, const MsgFilter& filter )
	{
		if( pMsg->type() == filter.msgType() )
		{
			if( filter.m_data1 == 0 )
				return true;
			else
			{
				KeyMsg_p p = KeyMsg::cast(pMsg);
	
				if( p->translatedKeyCode() == filter.m_data1 )
					return true;
			}
		}
		return false;
	}
	
	bool MsgFilter::_filterNativeKeyMsgs( const Msg_p& pMsg, const MsgFilter& filter )
	{
		if( pMsg->type() == filter.msgType() )
		{
			KeyMsg_p p = KeyMsg::cast(pMsg);
	
			if( p->nativeKeyCode() == filter.m_data1 )
				return true;
		}
		return false;
	}
		
	bool MsgFilter::_filterWheelRollMsgs( const Msg_p& pMsg, const MsgFilter& filter )
	{
		if( pMsg->type() == filter.msgType() )
		{
			int chr = WheelRollMsg::cast(pMsg)->wheel();
	
			if( chr == filter.m_data1 )
				return true;
		}
		return false;
	}
	
	bool MsgFilter::_filterItemToggleMsgs( const Msg_p& pMsg, const MsgFilter& filter )
	{
		if( pMsg->type() == filter.msgType() )
		{
			int itemId = ItemToggleMsg::cast(pMsg)->itemId();
	
			if( itemId == filter.m_data1 )
				return true;
		}
		return false;
	}
	
	
	bool MsgFilter::_filterItemMousePressMsgs( const Msg_p& pMsg, const MsgFilter& filter )
	{
		if( pMsg->type() == filter.msgType() )
		{
			ItemMousePressMsg_p pMsg = ItemMousePressMsg::cast(pMsg);
	
			if( (filter.m_data1 == -1 || pMsg->itemId() == filter.m_data1) ||
				(filter.m_data2 == -1 || pMsg->button() == filter.m_data2) )
				return true;
		}
		return false;
	}
	
	
	
	/*
	bool MsgFilter::_filterMenuitemMsg( const Msg_p& pMsg, const MsgFilter& filter )
	{
		if( pMsg->type() == filter.msgType() )
		{
			if( filter.m_data1 == 0 )
				return true;
			else
			{
				const Msg::MenuitemMsg * p = static_cast<const Msg::MenuitemMsg*>(pMsg);
	
				if( p->itemId() == filter.m_data1 )
					return true;
			}
		}
		return false;
	}
	*/

} // namespace wg
