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

using std::string;

const char WgMsgLogger::CLASSNAME[] = {"MsgLogger"};

//____ Constructor ____________________________________________________________

WgMsgLogger::WgMsgLogger( std::ostream& stream ) : m_out(stream)
{
	logAllMsgs();
	ignoreMsg( WG_MSG_TICK );
}

//____ Destructor _____________________________________________________________

WgMsgLogger::~WgMsgLogger()
{
}

//____ isInstanceOf() _________________________________________________________

bool WgMsgLogger::isInstanceOf( const char * pClassName ) const
{
	if( pClassName==CLASSNAME )
		return true;

	return WgReceiver::isInstanceOf(pClassName);
}

//____ className() ____________________________________________________________

const char * WgMsgLogger::className( void ) const
{
	return CLASSNAME;
}

//____ cast() _________________________________________________________________

WgMsgLogger_p WgMsgLogger::cast( const WgObject_p& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgMsgLogger_p( static_cast<WgMsgLogger*>(pObject.rawPtr()) );

	return 0;
}


//____ IgnoreMsg ____________________________________________________________

void WgMsgLogger::ignoreMsg( WgMsgType type )
{
	m_msgFilter[type] = false;
}

//____ LogMsg _______________________________________________________________

void WgMsgLogger::logMsg( WgMsgType type )
{
	m_msgFilter[type] = true;
}

//____ IgnorePointerMsgs ____________________________________________________________

void WgMsgLogger::ignorePointerMsgs()
{
	m_msgFilter[WG_MSG_MOUSE_ENTER] = false;
	m_msgFilter[WG_MSG_MOUSE_MOVE] = false;
	m_msgFilter[WG_MSG_MOUSE_POSITION] = false;
	m_msgFilter[WG_MSG_MOUSE_LEAVE] = false;
}

//____ LogPointerMsgs _______________________________________________________________

void WgMsgLogger::logPointerMsgs()
{
	m_msgFilter[WG_MSG_MOUSE_ENTER] = true;
	m_msgFilter[WG_MSG_MOUSE_MOVE] = true;
	m_msgFilter[WG_MSG_MOUSE_POSITION] = true;
	m_msgFilter[WG_MSG_MOUSE_LEAVE] = true;
}


//____ IgnoreMouseButtonMsgs ____________________________________________________________

void WgMsgLogger::ignoreMouseButtonMsgs()
{
	m_msgFilter[WG_MSG_MOUSE_PRESS] = false;
	m_msgFilter[WG_MSG_MOUSE_REPEAT] = false;
	m_msgFilter[WG_MSG_MOUSE_DRAG] = false;
	m_msgFilter[WG_MSG_MOUSE_RELEASE] = false;
	m_msgFilter[WG_MSG_MOUSE_CLICK] = false;
	m_msgFilter[WG_MSG_MOUSE_DOUBLE_CLICK] = false;
}

//____ LogMouseButtonMsgs _______________________________________________________________

void WgMsgLogger::logMouseButtonMsgs()
{
	m_msgFilter[WG_MSG_MOUSE_PRESS] = true;
	m_msgFilter[WG_MSG_MOUSE_REPEAT] = true;
	m_msgFilter[WG_MSG_MOUSE_DRAG] = true;
	m_msgFilter[WG_MSG_MOUSE_RELEASE] = true;
	m_msgFilter[WG_MSG_MOUSE_CLICK] = true;
	m_msgFilter[WG_MSG_MOUSE_DOUBLE_CLICK] = true;
}

//____ IgnoreKeyboardMsgs ____________________________________________________________

void WgMsgLogger::ignoreKeyboardMsgs()
{
	m_msgFilter[WG_MSG_KEY_PRESS] = false;
	m_msgFilter[WG_MSG_KEY_REPEAT] = false;
	m_msgFilter[WG_MSG_KEY_RELEASE] = false;
	m_msgFilter[WG_MSG_CHARACTER] = false;
}

//____ LogKeyboardMsgs _______________________________________________________________

void WgMsgLogger::logKeyboardMsgs()
{
	m_msgFilter[WG_MSG_KEY_PRESS] = true;
	m_msgFilter[WG_MSG_KEY_REPEAT] = true;
	m_msgFilter[WG_MSG_KEY_RELEASE] = true;
	m_msgFilter[WG_MSG_CHARACTER] = true;
}

//____ logMouseMsgs() _______________________________________________________

void WgMsgLogger::logMouseMsgs()
{
	logPointerMsgs();
	logMouseButtonMsgs();
	logMsg( WG_MSG_WHEEL_ROLL );
}

//____ ignoreMouseMsgs() ____________________________________________________

void WgMsgLogger::ignoreMouseMsgs()
{
	ignorePointerMsgs();
	ignoreMouseButtonMsgs();
	ignoreMsg( WG_MSG_WHEEL_ROLL );
}


//____ logInputMsgs() _______________________________________________________

void WgMsgLogger::logInputMsgs()
{
	logMouseMsgs();
	logKeyboardMsgs();
}

//____ ignoreInputMsgs() ____________________________________________________

void WgMsgLogger::ignoreInputMsgs()
{
	ignoreMouseMsgs();
	ignoreKeyboardMsgs();
}




//____ IgnoreAllMsgs ________________________________________________________

void WgMsgLogger::ignoreAllMsgs()
{
	for( int i = 0 ; i < WG_MSG_MAX ; i++ )
		m_msgFilter[i] = false;
}

//____ LogAllMsgs ___________________________________________________________
void WgMsgLogger::logAllMsgs()
{
	for( int i = 0 ; i < WG_MSG_MAX ; i++ )
		m_msgFilter[i] = true;
}



//____ onMsg() ______________________________________________________________

void WgMsgLogger::onMsg( const WgMsg_p& _pMsg )
{
	if( m_msgFilter[_pMsg->type()] == false )
		return;

	string	timestamp;
	string	source;
	string	copyTo;
	string	modkeys;
	string	pointerPos;

	char	params[256]; params[0] = 0;			// Msg specific parameters


	timestamp = _formatTimestamp( _pMsg->timestamp() );

	switch( _pMsg->type() )
	{
		case WG_MSG_DUMMY:
			break;
		case WG_MSG_TICK:
			sprintf( params, " millisec=%d", WgTickMsg::cast(_pMsg)->millisec() );
			break;
		case WG_MSG_POINTER_CHANGE:
			sprintf( params, " style=%s", _formatPointerStyle( WgPointerChangeMsg::cast(_pMsg)).c_str() );
			break;
		case WG_MSG_MOUSE_ENTER:
			break;
		case WG_MSG_MOUSE_MOVE:
			break;
		case WG_MSG_MOUSE_POSITION:
			break;
		case WG_MSG_MOUSE_LEAVE:
			break;
		case WG_MSG_MOUSE_PRESS:
			sprintf( params, " button=%s", _formatMouseButton(WgMousePressMsg::cast(_pMsg)->button()).c_str() );
			break;
		case WG_MSG_MOUSE_REPEAT:
			sprintf( params, " button=%s", _formatMouseButton(WgMouseRepeatMsg::cast(_pMsg)->button()).c_str() );
			break;
		case WG_MSG_MOUSE_DRAG:
		{
			WgMouseDragMsg_p pMsg = WgMouseDragMsg::cast(_pMsg);

			WgCoord	now		= pMsg->currPos();
			WgCoord 	prev	= pMsg->prevPos();
			WgCoord	start	= pMsg->startPos();

			WgCoord	dragNow	= pMsg->draggedNow();
			WgCoord	dragTotal=pMsg->draggedTotal();

			sprintf( params, " button=%s position(start=%d,%d prev=%d,%d now=%d,%d) dragged(now=%d,%d total=%d,%d)",
					_formatMouseButton(pMsg->button()).c_str(), start.x, start.y, prev.x, prev.y, now.x, now.y, dragNow.x, dragNow.y, dragTotal.x, dragTotal.y );
			break;
		}
		case WG_MSG_MOUSE_RELEASE:
		{
			WgMouseReleaseMsg_p pMsg = WgMouseReleaseMsg::cast(_pMsg);

			const static char outside[] = "outside";
			const static char inside[] = "inside";
			const char * pPress = outside;
			const char * pRelease = outside;

			if( pMsg->pressInside() )
				pPress = inside;

			if( pMsg->releaseInside() )
				pRelease = inside;

			sprintf( params, " button=%s press=%s release=%s", _formatMouseButton(pMsg->button()).c_str(), pPress, pRelease );
			break;
		}
		case WG_MSG_MOUSE_CLICK:
			sprintf( params, " button=%s", _formatMouseButton(WgMouseClickMsg::cast(_pMsg)->button()).c_str() );
			break;
		case WG_MSG_MOUSE_DOUBLE_CLICK:
			sprintf( params, " button=%s", _formatMouseButton(WgMouseDoubleClickMsg::cast(_pMsg)->button()).c_str() );
			break;

		case WG_MSG_KEY_PRESS:
		{
			WgKeyPressMsg_p pMsg = WgKeyPressMsg::cast(_pMsg);
			sprintf( params, "wg_keycode=%d native_keycode=%d", pMsg->translatedKeyCode(), pMsg->nativeKeyCode() );
			break;
		}
		case WG_MSG_KEY_REPEAT:
		{
			WgKeyRepeatMsg_p pMsg = WgKeyRepeatMsg::cast(_pMsg);
			sprintf( params, "wg_keycode=%d native_keycode=%d", pMsg->translatedKeyCode(), pMsg->nativeKeyCode() );
			break;
		}
		case WG_MSG_KEY_RELEASE:
		{
			WgKeyReleaseMsg_p pMsg = WgKeyReleaseMsg::cast(_pMsg);
			sprintf( params, "wg_keycode=%d native_keycode=%d", pMsg->translatedKeyCode(), pMsg->nativeKeyCode() );
			break;
		}
		case WG_MSG_CHARACTER:
			sprintf( params, " char=%X", WgCharacterMsg::cast(_pMsg)->character() );
			break;
		case WG_MSG_WHEEL_ROLL:
		{
			WgWheelRollMsg_p pMsg = WgWheelRollMsg::cast(_pMsg);
			sprintf( params, "wheel=%d distance=%d", pMsg->wheel(), pMsg->distance() );
			break;
		}

		case WG_MSG_SELECT:
			break;

		case WG_MSG_TOGGLE:
			sprintf( params, "set=%d", (int) (WgToggleMsg::cast(_pMsg)->isSet()) );
			break;

		case WG_MSG_VALUE_UPDATE:
		{
			WgValueUpdateMsg_p pMsg = WgValueUpdateMsg::cast(_pMsg);
			sprintf( params, "value=%ld fraction=%f", pMsg->value(), pMsg->fraction() );
			break;
		}

		case WG_MSG_RANGE_UPDATE:
		{
			WgRangeUpdateMsg_p pMsg = WgRangeUpdateMsg::cast(_pMsg);
			sprintf( params, "pos=%f length=%f", pMsg->offset(), pMsg->length() );
			break;
		}

		case WG_MSG_TEXT_EDIT:
		{
			WgTextEditMsg_p pMsg = WgTextEditMsg::cast(_pMsg);
			break;
		}

		case WG_MSG_ITEM_TOGGLE:
		{
			WgItemToggleMsg_p pMsg = WgItemToggleMsg::cast(_pMsg);
			sprintf( params, "set=%s id=%d object=%d", pMsg->isSet()?"true":"false", pMsg->itemId() );
			break;
		}
		case WG_MSG_ITEM_MOUSE_PRESS:
		{
			WgItemMousePressMsg_p pMsg = WgItemMousePressMsg::cast(_pMsg);
			sprintf( params, "id=% mouseButton=%s", pMsg->itemId(), _formatMouseButton(pMsg->button()).c_str() );
			break;
		}
				
		case WG_MSG_ITEMS_SELECT:
		{
			WgItemsSelectMsg_p pMsg = WgItemsSelectMsg::cast(_pMsg);
			sprintf( params, "nbItems=%d", pMsg->nbItems() );
			break;
		}

		case WG_MSG_ITEMS_UNSELECT:
		{
			WgItemsUnselectMsg_p pMsg = WgItemsUnselectMsg::cast(_pMsg);
			sprintf( params, "nbItems=%d", pMsg->nbItems() );
			break;
		}

		case WG_MSG_MODAL_MOVE_OUTSIDE:
			break;
		case WG_MSG_MODAL_BLOCKED_PRESS:
			sprintf( params, " button=%s", _formatMouseButton(WgModalBlockedPressMsg::cast(_pMsg)->button()).c_str() );
			break;
		case WG_MSG_MODAL_BLOCKED_RELEASE:
			sprintf( params, " button=%s", _formatMouseButton(WgModalBlockedReleaseMsg::cast(_pMsg)->button()).c_str() );
			break;

		default:
			break;
	};

	source = _formatSource( _pMsg );
	copyTo = _formatCopyTo( _pMsg );
	modkeys = _formatModkeys( _pMsg );
	pointerPos = _formatPointerPos( _pMsg );

	m_out << timestamp << " - " << _pMsg->className() << " - " << source << copyTo << pointerPos << modkeys << params;
	m_out << std::endl;
}

//____ _formatTimestamp() ______________________________________________________

string WgMsgLogger::_formatTimestamp( int64_t ms ) const
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

string WgMsgLogger::_formatSource( const WgMsg_p& _pMsg ) const
{
	std::string	out;

	if( _pMsg->sourceRawPtr() )
	{
		char	temp[64];
		WgObject * pObject = _pMsg->sourceRawPtr();

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

string WgMsgLogger::_formatCopyTo( const WgMsg_p& _pMsg ) const
{
	std::string	out;

	if( _pMsg->hasCopyTo() )
	{
		char	temp[64];
		WgReceiver * pCopyTo = _pMsg->getCopyTo().rawPtr();

		const char * pType = pCopyTo->className();

		sprintf( temp, " copyTo=%p (%s)", pCopyTo, pType );
		out = temp;
	}

	return out;
}


//____ _formatModkeys() __________________________________________________________

string WgMsgLogger::_formatModkeys( const WgMsg_p& _pMsg ) const
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

string WgMsgLogger::_formatPointerPos( const WgMsg_p& _pMsg ) const
{
	WgCoord globalPos = _pMsg->pointerPos();


	char	temp[64];

	sprintf( temp, " pointer=%d,%d", globalPos.x, globalPos.y );

	return string(temp);
}

//____ _formatPointerStyle() _____________________________________________________

string WgMsgLogger::_formatPointerStyle( const WgPointerChangeMsg_p& _pMsg ) const
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

std::string WgMsgLogger::_formatMouseButton( WgMouseButton button ) const
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

void  WgMsgLogger::_onRouteAdded()
{	
	_incRefCount();
}

//____ _onRouteRemoved() _______________________________________________________

void  WgMsgLogger::_onRouteRemoved()
{
	_decRefCount();
}
