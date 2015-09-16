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

#include <cstdio>

#include <wg_msglogger.h>
#include <wg_widget.h>

namespace wg 
{
	
	using std::string;
	
	const char MsgLogger::CLASSNAME[] = {"MsgLogger"};
	
	//____ Constructor ____________________________________________________________
	
	MsgLogger::MsgLogger( std::ostream& stream ) : m_out(stream)
	{
		logAllMsgs();
		ignoreMsg( WG_MSG_TICK );
	}
	
	//____ Destructor _____________________________________________________________
	
	MsgLogger::~MsgLogger()
	{
	}
	
	//____ isInstanceOf() _________________________________________________________
	
	bool MsgLogger::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;
	
		return Receiver::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * MsgLogger::className( void ) const
	{
		return CLASSNAME;
	}
	
	//____ cast() _________________________________________________________________
	
	MsgLogger_p MsgLogger::cast( const Object_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return MsgLogger_p( static_cast<MsgLogger*>(pObject.rawPtr()) );
	
		return 0;
	}
	
	
	//____ IgnoreMsg ____________________________________________________________
	
	void MsgLogger::ignoreMsg( MsgType type )
	{
		m_msgFilter[type] = false;
	}
	
	//____ LogMsg _______________________________________________________________
	
	void MsgLogger::logMsg( MsgType type )
	{
		m_msgFilter[type] = true;
	}
	
	//____ IgnorePointerMsgs ____________________________________________________________
	
	void MsgLogger::ignorePointerMsgs()
	{
		m_msgFilter[WG_MSG_MOUSE_ENTER] = false;
		m_msgFilter[WG_MSG_MOUSE_MOVE] = false;
		m_msgFilter[WG_MSG_MOUSE_LEAVE] = false;
	}
	
	//____ LogPointerMsgs _______________________________________________________________
	
	void MsgLogger::logPointerMsgs()
	{
		m_msgFilter[WG_MSG_MOUSE_ENTER] = true;
		m_msgFilter[WG_MSG_MOUSE_MOVE] = true;
		m_msgFilter[WG_MSG_MOUSE_LEAVE] = true;
	}
	
	
	//____ IgnoreMouseButtonMsgs ____________________________________________________________
	
	void MsgLogger::ignoreMouseButtonMsgs()
	{
		m_msgFilter[WG_MSG_MOUSE_PRESS] = false;
		m_msgFilter[WG_MSG_MOUSE_REPEAT] = false;
		m_msgFilter[WG_MSG_MOUSE_DRAG] = false;
		m_msgFilter[WG_MSG_MOUSE_RELEASE] = false;
		m_msgFilter[WG_MSG_MOUSE_CLICK] = false;
		m_msgFilter[WG_MSG_MOUSE_DOUBLE_CLICK] = false;
	}
	
	//____ LogMouseButtonMsgs _______________________________________________________________
	
	void MsgLogger::logMouseButtonMsgs()
	{
		m_msgFilter[WG_MSG_MOUSE_PRESS] = true;
		m_msgFilter[WG_MSG_MOUSE_REPEAT] = true;
		m_msgFilter[WG_MSG_MOUSE_DRAG] = true;
		m_msgFilter[WG_MSG_MOUSE_RELEASE] = true;
		m_msgFilter[WG_MSG_MOUSE_CLICK] = true;
		m_msgFilter[WG_MSG_MOUSE_DOUBLE_CLICK] = true;
	}
	
	//____ IgnoreKeyboardMsgs ____________________________________________________________
	
	void MsgLogger::ignoreKeyboardMsgs()
	{
		m_msgFilter[WG_MSG_KEY_PRESS] = false;
		m_msgFilter[WG_MSG_KEY_REPEAT] = false;
		m_msgFilter[WG_MSG_KEY_RELEASE] = false;
		m_msgFilter[WG_MSG_TEXT_INPUT] = false;
	}
	
	//____ LogKeyboardMsgs _______________________________________________________________
	
	void MsgLogger::logKeyboardMsgs()
	{
		m_msgFilter[WG_MSG_KEY_PRESS] = true;
		m_msgFilter[WG_MSG_KEY_REPEAT] = true;
		m_msgFilter[WG_MSG_KEY_RELEASE] = true;
		m_msgFilter[WG_MSG_TEXT_INPUT] = true;
	}
	
	//____ logMouseMsgs() _______________________________________________________
	
	void MsgLogger::logMouseMsgs()
	{
		logPointerMsgs();
		logMouseButtonMsgs();
		logMsg( WG_MSG_WHEEL_ROLL );
	}
	
	//____ ignoreMouseMsgs() ____________________________________________________
	
	void MsgLogger::ignoreMouseMsgs()
	{
		ignorePointerMsgs();
		ignoreMouseButtonMsgs();
		ignoreMsg( WG_MSG_WHEEL_ROLL );
	}
	
	
	//____ logInputMsgs() _______________________________________________________
	
	void MsgLogger::logInputMsgs()
	{
		logMouseMsgs();
		logKeyboardMsgs();
	}
	
	//____ ignoreInputMsgs() ____________________________________________________
	
	void MsgLogger::ignoreInputMsgs()
	{
		ignoreMouseMsgs();
		ignoreKeyboardMsgs();
	}
	
	
	
	
	//____ IgnoreAllMsgs ________________________________________________________
	
	void MsgLogger::ignoreAllMsgs()
	{
		for( int i = 0 ; i < WG_MSG_MAX ; i++ )
			m_msgFilter[i] = false;
	}
	
	//____ LogAllMsgs ___________________________________________________________
	void MsgLogger::logAllMsgs()
	{
		for( int i = 0 ; i < WG_MSG_MAX ; i++ )
			m_msgFilter[i] = true;
	}
	
	
	
	//____ onMsg() ______________________________________________________________
	
	void MsgLogger::onMsg( const Msg_p& _pMsg )
	{
		if( m_msgFilter[_pMsg->type()] == false )
			return;
	
		string	source;
		string	copyTo;
	
		char	params[256]; params[0] = 0;			// Msg specific parameters
	
		
		switch( _pMsg->type() )
		{
			case WG_MSG_DUMMY:
				break;
			case WG_MSG_TICK:
				sprintf( params, " millisec=%d", TickMsg::cast(_pMsg)->timediff() );
				break;
			case WG_MSG_POINTER_CHANGE:
				sprintf( params, " style=%s", _formatPointerStyle( PointerChangeMsg::cast(_pMsg)).c_str() );
				break;
			case WG_MSG_MOUSE_ENTER:
				break;
			case WG_MSG_MOUSE_MOVE:
				break;
			case WG_MSG_MOUSE_LEAVE:
				break;
			case WG_MSG_MOUSE_PRESS:
				sprintf( params, " button=%s", _formatMouseButton(MousePressMsg::cast(_pMsg)->button()).c_str() );
				break;
			case WG_MSG_MOUSE_REPEAT:
				sprintf( params, " button=%s", _formatMouseButton(MouseRepeatMsg::cast(_pMsg)->button()).c_str() );
				break;
			case WG_MSG_MOUSE_DRAG:
			{
				MouseDragMsg_p pMsg = MouseDragMsg::cast(_pMsg);
	
				Coord	now		= pMsg->currPos();
				Coord 	prev	= pMsg->prevPos();
				Coord	start	= pMsg->startPos();
	
				Coord	dragNow	= pMsg->draggedNow();
				Coord	dragTotal=pMsg->draggedTotal();
	
				sprintf( params, " button=%s position(start=%d,%d prev=%d,%d now=%d,%d) dragged(now=%d,%d total=%d,%d)",
						_formatMouseButton(pMsg->button()).c_str(), start.x, start.y, prev.x, prev.y, now.x, now.y, dragNow.x, dragNow.y, dragTotal.x, dragTotal.y );
				break;
			}
			case WG_MSG_MOUSE_RELEASE:
			{
				MouseReleaseMsg_p pMsg = MouseReleaseMsg::cast(_pMsg);
	
				const static char outside[] = "outside";
				const static char inside[] = "inside";
				const char * pPress = outside;
				const char * pRelease = outside;
		
				if( pMsg->releaseInside() )
					pRelease = inside;
	
				sprintf( params, " button=%s release=%s", _formatMouseButton(pMsg->button()).c_str(), pRelease );
				break;
			}
			case WG_MSG_MOUSE_CLICK:
				sprintf( params, " button=%s", _formatMouseButton(MouseClickMsg::cast(_pMsg)->button()).c_str() );
				break;
			case WG_MSG_MOUSE_DOUBLE_CLICK:
				sprintf( params, " button=%s", _formatMouseButton(MouseDoubleClickMsg::cast(_pMsg)->button()).c_str() );
				break;
	
			case WG_MSG_KEY_PRESS:
			{
				KeyPressMsg_p pMsg = KeyPressMsg::cast(_pMsg);
				sprintf( params, "wg_keycode=%d native_keycode=%d", pMsg->translatedKeyCode(), pMsg->nativeKeyCode() );
				break;
			}
			case WG_MSG_KEY_REPEAT:
			{
				KeyRepeatMsg_p pMsg = KeyRepeatMsg::cast(_pMsg);
				sprintf( params, "wg_keycode=%d native_keycode=%d", pMsg->translatedKeyCode(), pMsg->nativeKeyCode() );
				break;
			}
			case WG_MSG_KEY_RELEASE:
			{
				KeyReleaseMsg_p pMsg = KeyReleaseMsg::cast(_pMsg);
				sprintf( params, "wg_keycode=%d native_keycode=%d", pMsg->translatedKeyCode(), pMsg->nativeKeyCode() );
				break;
			}
			case WG_MSG_TEXT_INPUT:
				sprintf( params, " chars=%s", TextInputMsg::cast(_pMsg)->text().chars() );
				break;
			case WG_MSG_WHEEL_ROLL:
			{
				WheelRollMsg_p pMsg = WheelRollMsg::cast(_pMsg);
				sprintf( params, "wheel=%d distance=(%d,%d)", pMsg->wheel(), pMsg->distance().x, pMsg->distance().y );
				break;
			}
	
			case WG_MSG_SELECT:
				break;
	
			case WG_MSG_TOGGLE:
				sprintf( params, "set=%d", (int) (ToggleMsg::cast(_pMsg)->isSet()) );
				break;
	
			case WG_MSG_VALUE_UPDATE:
			{
				ValueUpdateMsg_p pMsg = ValueUpdateMsg::cast(_pMsg);
				sprintf( params, "value=%ld fraction=%f", pMsg->value(), pMsg->fraction() );
				break;
			}
	
			case WG_MSG_RANGE_UPDATE:
			{
				RangeUpdateMsg_p pMsg = RangeUpdateMsg::cast(_pMsg);
				sprintf( params, "pos=%f length=%f", pMsg->offset(), pMsg->length() );
				break;
			}
	
			case WG_MSG_TEXT_EDIT:
			{
				TextEditMsg_p pMsg = TextEditMsg::cast(_pMsg);
				break;
			}
	
			case WG_MSG_ITEM_TOGGLE:
			{
				ItemToggleMsg_p pMsg = ItemToggleMsg::cast(_pMsg);
				sprintf( params, "set=%s id=%d object=%d", pMsg->isSet()?"true":"false", pMsg->itemId() );
				break;
			}
			case WG_MSG_ITEM_MOUSE_PRESS:
			{
				ItemMousePressMsg_p pMsg = ItemMousePressMsg::cast(_pMsg);
				sprintf( params, "id=% mouseButton=%s", pMsg->itemId(), _formatMouseButton(pMsg->button()).c_str() );
				break;
			}
					
			case WG_MSG_ITEMS_SELECT:
			{
				ItemsSelectMsg_p pMsg = ItemsSelectMsg::cast(_pMsg);
				sprintf( params, "nbItems=%d", pMsg->nbItems() );
				break;
			}
	
			case WG_MSG_ITEMS_UNSELECT:
			{
				ItemsUnselectMsg_p pMsg = ItemsUnselectMsg::cast(_pMsg);
				sprintf( params, "nbItems=%d", pMsg->nbItems() );
				break;
			}
	
			case WG_MSG_MODAL_MOVE_OUTSIDE:
				break;
			case WG_MSG_MODAL_BLOCKED_PRESS:
				sprintf( params, " button=%s", _formatMouseButton(ModalBlockedPressMsg::cast(_pMsg)->button()).c_str() );
				break;
			case WG_MSG_MODAL_BLOCKED_RELEASE:
				sprintf( params, " button=%s", _formatMouseButton(ModalBlockedReleaseMsg::cast(_pMsg)->button()).c_str() );
				break;
	
			default:
				break;
		};
	
		source = _formatSource( _pMsg );
		copyTo = _formatCopyTo( _pMsg );

		string	timestamp;
		string	modkeys;
		string	pointerPos;

		if( _pMsg->isInstanceOf( InputMsg::CLASSNAME ) )
		{
			InputMsg_p p = InputMsg::cast(_pMsg);
			timestamp = _formatTimestamp( p->timestamp() );
			modkeys = _formatModkeys( p );
			pointerPos = _formatPointerPos( p );
		}
		
		m_out << " - " << _pMsg->className() << " - " << source << copyTo << pointerPos << modkeys << params;
		m_out << std::endl;
	}
	
	//____ _formatTimestamp() ______________________________________________________
	
	string MsgLogger::_formatTimestamp( int64_t ms ) const
	{
		char	temp[20];
	
		int milli	= (int) (ms % 1000);
		int sec		= (int) ((ms/1000) % 60);
		int min		= (int) ((ms/60000) % 60);
		int hour	= (int) (ms/3600000);
	
		sprintf( temp, "%d:%02d:%02d.%03d", hour, min, sec, milli );
		return string( temp );
	}
	
	//____ _formatSource() __________________________________________________________
	
	string MsgLogger::_formatSource( const Msg_p& _pMsg ) const
	{
		std::string	out;
	
		if( _pMsg->sourceRawPtr() )
		{
			char	temp[64];
			Object * pObject = _pMsg->sourceRawPtr();
	
			static const char def_type[] = "deleted";
			const char * pType = def_type;
	
			if( pObject )
				pType = pObject->className();
	
			sprintf( temp, " source=%p (%s)", pObject, pType );
			out = temp;
		}
	
		return out;
	}
	
	//____ _formatCopyTo() __________________________________________________________
	
	string MsgLogger::_formatCopyTo( const Msg_p& _pMsg ) const
	{
		std::string	out;
	
		if( _pMsg->hasCopyTo() )
		{
			char	temp[64];
			Receiver * pCopyTo = _pMsg->getCopyTo().rawPtr();
	
			const char * pType = pCopyTo->className();
	
			sprintf( temp, " copyTo=%p (%s)", pCopyTo, pType );
			out = temp;
		}
	
		return out;
	}
	
	
	//____ _formatModkeys() __________________________________________________________
	
	string MsgLogger::_formatModkeys( const InputMsg_p& _pMsg ) const
	{
		WgModifierKeys keys = _pMsg->modKeys();
	
		string	out;
	
		if( keys != WG_MODKEY_NONE )
		{
			char	temp[64];
			sprintf( temp, " modkeys=%X", keys );
			out = temp;
		}
	
		return out;
	}
	
	//____ _formatPointerPos() _____________________________________________________
	
	string MsgLogger::_formatPointerPos( const InputMsg_p& _pMsg ) const
	{
		Coord globalPos = _pMsg->pointerPos();
	
	
		char	temp[64];
	
		sprintf( temp, " pointer=%d,%d", globalPos.x, globalPos.y );
	
		return string(temp);
	}
	
	//____ _formatPointerStyle() _____________________________________________________
	
	string MsgLogger::_formatPointerStyle( const PointerChangeMsg_p& _pMsg ) const
	{
		switch( _pMsg->style() )
		{
			case WG_POINTER_ARROW:
				return "WG_POINTER_ARROW";
			case WG_POINTER_HOURGLASS:
				return "WG_POINTER_HOURGLASS";
			case WG_POINTER_HAND:
				return "WG_POINTER_HAND";
			case WG_POINTER_CROSSHAIR:
				return "WG_POINTER_CROSSHAIR";
			case WG_POINTER_HELP:
				return "WG_POINTER_HELP";
			case WG_POINTER_IBEAM:
				return "WG_POINTER_IBEAM";
			case WG_POINTER_STOP:
				return "WG_POINTER_STOP";
			case WG_POINTER_UP_ARROW:
				return "WG_POINTER_UP_ARROW";
			case WG_POINTER_SIZE_ALL:
				return "WG_POINTER_SIZE_ALL";
			case WG_POINTER_SIZE_NE_SW:
				return "WG_POINTER_SIZE_NE_SW";
			case WG_POINTER_SIZE_NW_SE:
				return "WG_POINTER_SIZE_NW_SE";
			case WG_POINTER_SIZE_N_S:
				return "WG_POINTER_SIZE_N_S";
			case WG_POINTER_SIZE_W_E:
				return "WG_POINTER_SIZE_W_E";
			default:
			{
				char	temp[64];
				sprintf( temp, "%d (unkown enum)", _pMsg->style() );
				return string(temp);
			}
		}
	}
	
	//____ _formatMouseButton() ____________________________________________________
	
	std::string MsgLogger::_formatMouseButton( WgMouseButton button ) const
	{
		switch( button )
		{
			case WG_BUTTON_LEFT:
				return "LEFT";
			case WG_BUTTON_MIDDLE:
				return "MIDDLE";
			case WG_BUTTON_RIGHT:
				return "RIGHT";
			case WG_BUTTON_X1:
				return "X1";
			case WG_BUTTON_X2:
				return "X2";
			default:
			{
				char	temp[64];
				sprintf( temp, "%d (unkown enum)", button );
				return string(temp);
			}
		}
	}
	
	//____ _onRouteAdded() _________________________________________________________
	
	void  MsgLogger::_onRouteAdded()
	{	
		_incRefCount();
	}
	
	//____ _onRouteRemoved() _______________________________________________________
	
	void  MsgLogger::_onRouteRemoved()
	{
		_decRefCount();
	}

} // namespace wg
