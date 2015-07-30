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

#include <wg_msg.h>
#include <wg_key.h>
#include <wg_widget.h>
#include <wg_button.h>
#include <wg_animplayer.h>
#include <wg_valuedisplay.h>
#include <wg_valueeditor.h>
#include <wg_scrollbar.h>
#include <wg_textdisplay.h>
#include <wg_lineeditor.h>
#include <wg_menu.h>
#include <wg_legacytextfield.h>
//#include <wg_tablepanel.h>


//____ WgMsg ______________________________________________________________

const char WgMsg::CLASSNAME[] = {"Msg"};

bool WgMsg::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgObject::isInstanceOf(pClassName);
}

const char * WgMsg::className( void ) const
{ 
	return CLASSNAME; 
}

WgMsgPtr WgMsg::cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgMsgPtr( static_cast<WgMsg*>(pObject.rawPtr()) );

	return 0;
}

void WgMsg::setCopyTo( const WgReceiverPtr& pReceiver )
{
	m_pCopyTo = pReceiver;
}

void WgMsg::setRepost( const WgObjectPtr& pSource, const WgReceiverPtr& pCopyTo )
{
	m_pRepostSource = pSource;	
	m_pRepostCopyTo = pCopyTo;
}

void WgMsg::swallow()
{
	m_pRepostSource = 0;
	m_pRepostCopyTo = 0;
}

bool WgMsg::doRepost()
{
	if( m_pRepostSource )
	{
		m_pSource = m_pRepostSource;
		m_pCopyTo = m_pRepostCopyTo;
		
		m_pRepostSource = 0;
		m_pRepostCopyTo = 0;
		
		return true;
	}
	return false;
}



bool WgMsg::isMouseMsg() const
{
	if( m_type == WG_MSG_WHEEL_ROLL ||
		m_type == WG_MSG_MOUSE_ENTER ||
		m_type == WG_MSG_MOUSE_LEAVE ||	
		m_type == WG_MSG_MOUSE_MOVE ||	
		m_type == WG_MSG_MOUSE_POSITION ||
		isMouseButtonMsg() )
		return true;
	else
		return false;
			
}
	
bool WgMsg::isMouseButtonMsg() const
{
	if( m_type == WG_MSG_MOUSE_CLICK ||
		m_type == WG_MSG_MOUSE_DOUBLE_CLICK ||
		m_type == WG_MSG_MOUSE_DRAG ||
		m_type == WG_MSG_MOUSE_PRESS ||
		m_type == WG_MSG_MOUSE_RELEASE ||
		m_type == WG_MSG_MOUSE_REPEAT )
		return true;
	else
		return false;
}
	
bool WgMsg::isKeyMsg() const
{
	if( m_type == WG_MSG_KEY_PRESS ||
		m_type == WG_MSG_KEY_RELEASE ||
		m_type == WG_MSG_KEY_REPEAT )
		return true;
	else
		return false;
}

//____ WgKeyMsg ____________________________________________________________

const char WgKeyMsg::CLASSNAME[] = {"KeyMsg"};

bool WgKeyMsg::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgMsg::isInstanceOf(pClassName);
}

const char * WgKeyMsg::className( void ) const
{ 
	return CLASSNAME; 
}

WgKeyMsgPtr WgKeyMsg::cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgKeyMsgPtr( static_cast<WgKeyMsg*>(pObject.rawPtr()) );

	return 0;
}

bool WgKeyMsg::isCursorKey() const
{
	if( m_translatedKeyCode == WG_KEY_UP || m_translatedKeyCode == WG_KEY_DOWN ||
		m_translatedKeyCode == WG_KEY_LEFT || m_translatedKeyCode == WG_KEY_RIGHT )
		return true;
	else
		return false;
}
	
bool WgKeyMsg::isMovementKey() const
{
	if( m_translatedKeyCode == WG_KEY_PAGE_UP || m_translatedKeyCode == WG_KEY_PAGE_DOWN ||
		m_translatedKeyCode == WG_KEY_HOME || m_translatedKeyCode == WG_KEY_END ||
		isCursorKey() )
		return true;
	else
		return false;
}
	
//____ WgMouseButtonMsg _____________________________________________________

const char WgMouseButtonMsg::CLASSNAME[] = {"MouseButtonMsg"};

bool WgMouseButtonMsg::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgMsg::isInstanceOf(pClassName);
}

const char * WgMouseButtonMsg::className( void ) const
{ 
	return CLASSNAME; 
}

WgMouseButtonMsgPtr WgMouseButtonMsg::cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgMouseButtonMsgPtr( static_cast<WgMouseButtonMsg*>(pObject.rawPtr()) );

	return 0;
}


//____ WgFocusGainedMsg ________________________________________________________

const char WgFocusGainedMsg::CLASSNAME[] = {"FocusGainedMsg"};

WgFocusGainedMsg::WgFocusGainedMsg()
{
	m_type = WG_MSG_FOCUS_GAINED;
}

bool WgFocusGainedMsg::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgMsg::isInstanceOf(pClassName);
}

const char * WgFocusGainedMsg::className( void ) const
{ 
	return CLASSNAME; 
}

WgFocusGainedMsgPtr WgFocusGainedMsg::cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgFocusGainedMsgPtr( static_cast<WgFocusGainedMsg*>(pObject.rawPtr()) );

	return 0;
}


//____ WgFocusLostMsg __________________________________________________________

const char WgFocusLostMsg::CLASSNAME[] = {"FocusLostMsg"};

WgFocusLostMsg::WgFocusLostMsg()
{
	m_type = WG_MSG_FOCUS_LOST;
}

bool WgFocusLostMsg::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgMsg::isInstanceOf(pClassName);
}

const char * WgFocusLostMsg::className( void ) const
{ 
	return CLASSNAME; 
}

WgFocusLostMsgPtr WgFocusLostMsg::cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgFocusLostMsgPtr( static_cast<WgFocusLostMsg*>(pObject.rawPtr()) );

	return 0;
}


//____ WgMouseEnterMsg _______________________________________________________

const char WgMouseEnterMsg::CLASSNAME[] = {"MouseEnterMsg"};

WgMouseEnterMsg::WgMouseEnterMsg( const WgCoord& pos )
{
	m_type = WG_MSG_MOUSE_ENTER;

	m_pointerPos = pos;
}

WgMouseEnterMsg::WgMouseEnterMsg( WgWidget * pWidget )
{
	m_type = WG_MSG_MOUSE_ENTER;
	m_pSource	= pWidget;
	m_pCopyTo 	= pWidget;
}

bool WgMouseEnterMsg::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgMsg::isInstanceOf(pClassName);
}

const char * WgMouseEnterMsg::className( void ) const
{ 
	return CLASSNAME; 
}

WgMouseEnterMsgPtr WgMouseEnterMsg::cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgMouseEnterMsgPtr( static_cast<WgMouseEnterMsg*>(pObject.rawPtr()) );

	return 0;
}


//____ WgMouseLeaveMsg ________________________________________________________

const char WgMouseLeaveMsg::CLASSNAME[] = {"MouseLeaveMsg"};

WgMouseLeaveMsg::WgMouseLeaveMsg()
{
	m_type = WG_MSG_MOUSE_LEAVE;
}

WgMouseLeaveMsg::WgMouseLeaveMsg( WgWidget * pWidget )
{
	m_type = WG_MSG_MOUSE_LEAVE;
	m_pSource		= pWidget;
	m_pCopyTo 		= pWidget;
}

bool WgMouseLeaveMsg::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgMsg::isInstanceOf(pClassName);
}

const char * WgMouseLeaveMsg::className( void ) const
{ 
	return CLASSNAME; 
}

WgMouseLeaveMsgPtr WgMouseLeaveMsg::cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgMouseLeaveMsgPtr( static_cast<WgMouseLeaveMsg*>(pObject.rawPtr()) );

	return 0;
}

//____ WgMouseMoveMsg __________________________________________________________

const char WgMouseMoveMsg::CLASSNAME[] = {"MouseMoveMsg"};

WgMouseMoveMsg::WgMouseMoveMsg( const WgCoord& pos )
{
	m_type = WG_MSG_MOUSE_MOVE;

	m_pointerPos = pos;
}

WgMouseMoveMsg::WgMouseMoveMsg( WgWidget * pWidget )
{
	m_type = WG_MSG_MOUSE_MOVE;
	m_pSource		= pWidget;
	m_pCopyTo 		= pWidget;
}

bool WgMouseMoveMsg::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgMsg::isInstanceOf(pClassName);
}

const char * WgMouseMoveMsg::className( void ) const
{ 
	return CLASSNAME; 
}

WgMouseMoveMsgPtr WgMouseMoveMsg::cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgMouseMoveMsgPtr( static_cast<WgMouseMoveMsg*>(pObject.rawPtr()) );

	return 0;
}


//____ WgMousePressMsg ______________________________________________________

const char WgMousePressMsg::CLASSNAME[] = {"MousePressMsg"};

WgMousePressMsg::WgMousePressMsg( WgMouseButton button ) : WgMouseButtonMsg(button)
{
	m_type = WG_MSG_MOUSE_PRESS;
}

WgMousePressMsg::WgMousePressMsg( WgMouseButton button, WgWidget * pWidget ) : WgMouseButtonMsg(button)
{
	m_type			= WG_MSG_MOUSE_PRESS;
	m_pSource		= pWidget;
	m_pCopyTo 		= pWidget;
}

bool WgMousePressMsg::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgMouseButtonMsg::isInstanceOf(pClassName);
}

const char * WgMousePressMsg::className( void ) const
{ 
	return CLASSNAME; 
}

WgMousePressMsgPtr WgMousePressMsg::cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgMousePressMsgPtr( static_cast<WgMousePressMsg*>(pObject.rawPtr()) );

	return 0;
}

//____ WgMouseRepeatMsg ______________________________________________________

const char WgMouseRepeatMsg::CLASSNAME[] = {"MouseRepeatMsg"};

WgMouseRepeatMsg::WgMouseRepeatMsg( WgMouseButton button ) : WgMouseButtonMsg(button)
{
	m_type = WG_MSG_MOUSE_REPEAT;
}

WgMouseRepeatMsg::WgMouseRepeatMsg( WgMouseButton button, WgWidget * pWidget ) : WgMouseButtonMsg(button)
{
	m_type			= WG_MSG_MOUSE_REPEAT;
	m_pSource		= pWidget;
	m_pCopyTo 		= pWidget;
}

bool WgMouseRepeatMsg::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgMouseButtonMsg::isInstanceOf(pClassName);
}

const char * WgMouseRepeatMsg::className( void ) const
{ 
	return CLASSNAME; 
}

WgMouseRepeatMsgPtr WgMouseRepeatMsg::cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgMouseRepeatMsgPtr( static_cast<WgMouseRepeatMsg*>(pObject.rawPtr()) );

	return 0;
}


//____ WgMouseReleaseMsg _____________________________________________________

const char WgMouseReleaseMsg::CLASSNAME[] = {"MouseReleaseMsg"};

WgMouseReleaseMsg::WgMouseReleaseMsg( WgMouseButton button ) : WgMouseButtonMsg(button)
{
	m_type = WG_MSG_MOUSE_RELEASE;

	m_bReleaseInside = true;			// Always assumed to have had the press inside our window.
	m_bReleaseInside = true;			// Always assumed to be inside our window now.
}

WgMouseReleaseMsg::WgMouseReleaseMsg( WgMouseButton button, WgWidget * pWidget, bool bPressInside, bool bReleaseInside ) : WgMouseButtonMsg(button)
{
	m_type			= WG_MSG_MOUSE_RELEASE;
	m_pSource		= pWidget;
	m_pCopyTo 		= pWidget;

	m_bPressInside = bPressInside;
	m_bReleaseInside = bReleaseInside;
}

bool WgMouseReleaseMsg::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgMouseButtonMsg::isInstanceOf(pClassName);
}

const char * WgMouseReleaseMsg::className( void ) const
{ 
	return CLASSNAME; 
}

WgMouseReleaseMsgPtr WgMouseReleaseMsg::cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgMouseReleaseMsgPtr( static_cast<WgMouseReleaseMsg*>(pObject.rawPtr()) );

	return 0;
}

bool WgMouseReleaseMsg::pressInside() const
{
	return m_bPressInside;
}

bool WgMouseReleaseMsg::releaseInside() const
{
	return m_bReleaseInside;
}



//____ WgMouseClickMsg _____________________________________________________

const char WgMouseClickMsg::CLASSNAME[] = {"MouseClickMsg"};

WgMouseClickMsg::WgMouseClickMsg( WgMouseButton button ) : WgMouseButtonMsg(button)
{
	m_type = WG_MSG_MOUSE_CLICK;
}

WgMouseClickMsg::WgMouseClickMsg( WgMouseButton button, WgWidget * pWidget ) : WgMouseButtonMsg(button)
{
	m_type = WG_MSG_MOUSE_CLICK;
	m_pSource		= pWidget;
	m_pCopyTo 		= pWidget;
}

bool WgMouseClickMsg::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgMouseButtonMsg::isInstanceOf(pClassName);
}

const char * WgMouseClickMsg::className( void ) const
{ 
	return CLASSNAME; 
}

WgMouseClickMsgPtr WgMouseClickMsg::cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgMouseClickMsgPtr( static_cast<WgMouseClickMsg*>(pObject.rawPtr()) );

	return 0;
}

//____ WgMouseDoubleClickMsg _____________________________________________________

const char WgMouseDoubleClickMsg::CLASSNAME[] = {"MouseDoubleClickMsg"};

WgMouseDoubleClickMsg::WgMouseDoubleClickMsg( WgMouseButton button ) : WgMouseButtonMsg(button)
{
	m_type = WG_MSG_MOUSE_DOUBLE_CLICK;
}

WgMouseDoubleClickMsg::WgMouseDoubleClickMsg( WgMouseButton button, WgWidget * pWidget ) : WgMouseButtonMsg(button)
{
	m_type = WG_MSG_MOUSE_DOUBLE_CLICK;
	m_pSource		= pWidget;
	m_pCopyTo		= pWidget;
}

bool WgMouseDoubleClickMsg::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgMouseButtonMsg::isInstanceOf(pClassName);
}

const char * WgMouseDoubleClickMsg::className( void ) const
{ 
	return CLASSNAME; 
}

WgMouseDoubleClickMsgPtr WgMouseDoubleClickMsg::cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgMouseDoubleClickMsgPtr( static_cast<WgMouseDoubleClickMsg*>(pObject.rawPtr()) );

	return 0;
}

//____ WgKeyPressMsg ___________________________________________________________

const char WgKeyPressMsg::CLASSNAME[] = {"KeyPressMsg"};

WgKeyPressMsg::WgKeyPressMsg( int native_keycode ) : WgKeyMsg(native_keycode)
{
	m_type 			= WG_MSG_KEY_PRESS;
}

WgKeyPressMsg::WgKeyPressMsg( int native_keycode, WgWidget * pWidget ) : WgKeyMsg(native_keycode)
{
	m_type 			= WG_MSG_KEY_PRESS;
	m_pSource		= pWidget;
	m_pCopyTo		= pWidget;
}

bool WgKeyPressMsg::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgKeyMsg::isInstanceOf(pClassName);
}

const char * WgKeyPressMsg::className( void ) const
{ 
	return CLASSNAME; 
}

WgKeyPressMsgPtr WgKeyPressMsg::cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgKeyPressMsgPtr( static_cast<WgKeyPressMsg*>(pObject.rawPtr()) );

	return 0;
}

//____ WgKeyRepeatMsg ___________________________________________________________

const char WgKeyRepeatMsg::CLASSNAME[] = {"KeyRepeatMsg"};

WgKeyRepeatMsg::WgKeyRepeatMsg( int native_keycode ) : WgKeyMsg(native_keycode)
{
	m_type 			= WG_MSG_KEY_REPEAT;
}

WgKeyRepeatMsg::WgKeyRepeatMsg( int native_keycode, WgWidget * pWidget ) : WgKeyMsg(native_keycode)
{
	m_type 			= WG_MSG_KEY_REPEAT;
	m_pSource		= pWidget;
	m_pCopyTo 		= pWidget;
}

bool WgKeyRepeatMsg::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgKeyMsg::isInstanceOf(pClassName);
}

const char * WgKeyRepeatMsg::className( void ) const
{ 
	return CLASSNAME; 
}

WgKeyRepeatMsgPtr WgKeyRepeatMsg::cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgKeyRepeatMsgPtr( static_cast<WgKeyRepeatMsg*>(pObject.rawPtr()) );

	return 0;
}

//____ WgKeyReleaseMsg ________________________________________________________

const char WgKeyReleaseMsg::CLASSNAME[] = {"KeyReleaseMsg"};

WgKeyReleaseMsg::WgKeyReleaseMsg( int native_keycode ) : WgKeyMsg(native_keycode)
{
	m_type 		= WG_MSG_KEY_RELEASE;
}

WgKeyReleaseMsg::WgKeyReleaseMsg( int native_keycode, WgWidget * pWidget ) : WgKeyMsg(native_keycode)
{
	m_type 			= WG_MSG_KEY_RELEASE;
	m_pSource		= pWidget;
	m_pCopyTo		= pWidget;
}

bool WgKeyReleaseMsg::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgKeyMsg::isInstanceOf(pClassName);
}

const char * WgKeyReleaseMsg::className( void ) const
{ 
	return CLASSNAME; 
}

WgKeyReleaseMsgPtr WgKeyReleaseMsg::cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgKeyReleaseMsgPtr( static_cast<WgKeyReleaseMsg*>(pObject.rawPtr()) );

	return 0;
}

//____ WgCharacterMsg __________________________________________________________

const char WgCharacterMsg::CLASSNAME[] = {"CharacterMsg"};

WgCharacterMsg::WgCharacterMsg( unsigned short character )
{
	m_type = WG_MSG_CHARACTER;
	m_char = character;
}

WgCharacterMsg::WgCharacterMsg( unsigned short character, WgWidget * pWidget )
{
	m_type			= WG_MSG_CHARACTER;
	m_char			= character;
	m_pSource		= pWidget;
	m_pCopyTo		= pWidget;
}

unsigned short WgCharacterMsg::character() const
{
	return m_char;
}

bool WgCharacterMsg::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgMsg::isInstanceOf(pClassName);
}

const char * WgCharacterMsg::className( void ) const
{ 
	return CLASSNAME; 
}

WgCharacterMsgPtr WgCharacterMsg::cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgCharacterMsgPtr( static_cast<WgCharacterMsg*>(pObject.rawPtr()) );

	return 0;
}


//____ WgWheelRollMsg __________________________________________________________

const char WgWheelRollMsg::CLASSNAME[] = {"WheelRollMsg"};

WgWheelRollMsg::WgWheelRollMsg( int wheel, int distance )
{
	m_type = WG_MSG_WHEEL_ROLL;
	m_wheel = wheel;
	m_distance = distance;
}

WgWheelRollMsg::WgWheelRollMsg( int wheel, int distance, WgWidget * pWidget )
{
	m_type			= WG_MSG_WHEEL_ROLL;
	m_wheel			= wheel;
	m_distance		= distance;
	m_pSource		= pWidget;
	m_pCopyTo		= pWidget;
}

int WgWheelRollMsg::wheel() const
{
	return m_wheel;
}

int WgWheelRollMsg::distance() const
{
	return m_distance;
}

bool WgWheelRollMsg::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgMsg::isInstanceOf(pClassName);
}

const char * WgWheelRollMsg::className( void ) const
{ 
	return CLASSNAME; 
}

WgWheelRollMsgPtr WgWheelRollMsg::cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgWheelRollMsgPtr( static_cast<WgWheelRollMsg*>(pObject.rawPtr()) );

	return 0;
}

//____ WgTickMsg _______________________________________________________________

const char WgTickMsg::CLASSNAME[] = {"TickMsg"};

WgTickMsg::WgTickMsg( int ms )
{
	m_type = WG_MSG_TICK;
	m_millisec = ms;
}

WgTickMsg::WgTickMsg( int ms, WgReceiver * pReceiver )
{
	m_type 			= WG_MSG_TICK;
	m_millisec 		= ms;
	m_pCopyTo 		= pReceiver;
}
	
int WgTickMsg::millisec() const
{
	return m_millisec;
}

bool WgTickMsg::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgMsg::isInstanceOf(pClassName);
}

const char * WgTickMsg::className( void ) const
{ 
	return CLASSNAME; 
}

WgTickMsgPtr WgTickMsg::cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgTickMsgPtr( static_cast<WgTickMsg*>(pObject.rawPtr()) );

	return 0;
}

//____ WgPointerChangeMsg _______________________________________________________________

const char WgPointerChangeMsg::CLASSNAME[] = {"PointerChangeMsg"};

WgPointerChangeMsg::WgPointerChangeMsg( WgPointerStyle style )
{
	m_type = WG_MSG_POINTER_CHANGE;
	m_style = style;
}
	
WgPointerStyle WgPointerChangeMsg::style() const
{
	return m_style;
}

bool WgPointerChangeMsg::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgMsg::isInstanceOf(pClassName);
}

const char * WgPointerChangeMsg::className( void ) const
{ 
	return CLASSNAME; 
}

WgPointerChangeMsgPtr WgPointerChangeMsg::cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgPointerChangeMsgPtr( static_cast<WgPointerChangeMsg*>(pObject.rawPtr()) );

	return 0;
}

//____ WgSelectMsg _________________________________________________________

const char WgSelectMsg::CLASSNAME[] = {"SelectMsg"};

WgSelectMsg::WgSelectMsg( WgObject * pSource )
{
	m_type 			= WG_MSG_SELECT;
	m_pSource 		= pSource;
}

bool WgSelectMsg::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgMsg::isInstanceOf(pClassName);
}

const char * WgSelectMsg::className( void ) const
{ 
	return CLASSNAME; 
}

WgSelectMsgPtr WgSelectMsg::cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgSelectMsgPtr( static_cast<WgSelectMsg*>(pObject.rawPtr()) );

	return 0;
}

//____ WgToggleMsg ______________________________________________

const char WgToggleMsg::CLASSNAME[] = {"ToggleMsg"};

WgToggleMsg::WgToggleMsg( WgObject * pSource, bool bSet )
{
	m_type = WG_MSG_TOGGLE;
	m_pSource = pSource;
	m_bSet = bSet;
}

bool WgToggleMsg::isSet() const
{
	return m_bSet;
}

bool WgToggleMsg::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgMsg::isInstanceOf(pClassName);
}

const char * WgToggleMsg::className( void ) const
{ 
	return CLASSNAME; 
}

WgToggleMsgPtr WgToggleMsg::cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgToggleMsgPtr( static_cast<WgToggleMsg*>(pObject.rawPtr()) );

	return 0;
}

//____ WgValueChangeMsg ___________________________________________

const char WgValueUpdateMsg::CLASSNAME[] = {"ValueUpdateMsg"};

WgValueUpdateMsg::WgValueUpdateMsg( WgObject * pSource, int64_t value, double fraction, bool bFinal )
{
	m_type = WG_MSG_VALUE_UPDATE;
	m_pSource = pSource;
	m_value = value;
	m_fraction = fraction;
	m_bFinal = bFinal;
}

int64_t WgValueUpdateMsg::value() const
{
	return m_value;
}

double WgValueUpdateMsg::fraction() const
{
	return m_fraction;
}

bool WgValueUpdateMsg::isFinal() const
{
	return m_bFinal;
}

bool WgValueUpdateMsg::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgMsg::isInstanceOf(pClassName);
}

const char * WgValueUpdateMsg::className( void ) const
{ 
	return CLASSNAME; 
}

WgValueUpdateMsgPtr WgValueUpdateMsg::cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgValueUpdateMsgPtr( static_cast<WgValueUpdateMsg*>(pObject.rawPtr()) );

	return 0;
}

//____ WgRangeUpdateMsg ___________________________________________

const char WgRangeUpdateMsg::CLASSNAME[] = {"RangeUpdateMsg"};

WgRangeUpdateMsg::WgRangeUpdateMsg( WgObject * pSource, int intOfs, int intLength, double fracOfs, double fracLength, bool bFinal )
{
	m_type = WG_MSG_RANGE_UPDATE;
	m_pSource 	= pSource;
	m_ofs		= intOfs;
	m_length	= intLength;
	m_fracOfs	= fracOfs;
	m_fracLength= fracLength;
	m_bFinal	= bFinal;
}

bool WgRangeUpdateMsg::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgMsg::isInstanceOf(pClassName);
}

const char * WgRangeUpdateMsg::className( void ) const
{ 
	return CLASSNAME; 
}

WgRangeUpdateMsgPtr WgRangeUpdateMsg::cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgRangeUpdateMsgPtr( static_cast<WgRangeUpdateMsg*>(pObject.rawPtr()) );

	return 0;
}

int WgRangeUpdateMsg::offset() const
{
	return m_ofs;
}

int WgRangeUpdateMsg::length() const
{
	return m_length;
}

double WgRangeUpdateMsg::fracOffset() const
{
	return m_fracOfs;
}

double WgRangeUpdateMsg::fracLength() const
{
	return m_fracLength;
}

bool WgRangeUpdateMsg::isFinal() const
{
	return m_bFinal;
}


//____ WgTextEditMsg ________________________________________________________

const char WgTextEditMsg::CLASSNAME[] = {"TextEditMsg"};

WgEditTextPtr WgTextEditMsg::text() const
{
	return m_pText;
}

WgTextEditMsg::WgTextEditMsg( const WgEditTextPtr& pText, bool bFinal )
{
	m_type 		= WG_MSG_TEXT_EDIT;
	m_pSource 	= pText->holder();
	m_pText 	= pText;
	m_bFinal	= bFinal;
}

bool WgTextEditMsg::isFinal() const
{
	return m_bFinal;
}

bool WgTextEditMsg::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgMsg::isInstanceOf(pClassName);
}

const char * WgTextEditMsg::className( void ) const
{ 
	return CLASSNAME; 
}

WgTextEditMsgPtr WgTextEditMsg::cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgTextEditMsgPtr( static_cast<WgTextEditMsg*>(pObject.rawPtr()) );

	return 0;
}

//____ WgItemMsg ________________________________________________________

const char WgItemMsg::CLASSNAME[] = {"ItemMsg"};

WgItemMsg::WgItemMsg( WgObject * pSource, int itemId, const WgObjectPtr& pItemObject ) : m_item(itemId,pItemObject)
{
	m_pSource 	= pSource;
}

bool WgItemMsg::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgMsg::isInstanceOf(pClassName);
}

const char * WgItemMsg::className( void ) const
{ 
	return CLASSNAME; 
}

WgItemMsgPtr WgItemMsg::cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgItemMsgPtr( static_cast<WgItemMsg*>(pObject.rawPtr()) );

	return 0;
}

int WgItemMsg::itemId() const 
{ 
	return m_item.id; 
}

WgObjectPtr WgItemMsg::itemObject() const 
{ 
	return m_item.pObject; 
}

//____ WgItemToggleMsg ________________________________________________________

const char WgItemToggleMsg::CLASSNAME[] = {"ItemToggleMsg"};

WgItemToggleMsg::WgItemToggleMsg( WgObject * pSource, int itemId, const WgObjectPtr& pItemObject, bool bSet ) : WgItemMsg(pSource,itemId,pItemObject)
{
	m_type = WG_MSG_ITEM_TOGGLE;
	m_bSet = bSet;
}

bool WgItemToggleMsg::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgItemMsg::isInstanceOf(pClassName);
}

const char * WgItemToggleMsg::className( void ) const
{ 
	return CLASSNAME; 
}

WgItemToggleMsgPtr WgItemToggleMsg::cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgItemToggleMsgPtr( static_cast<WgItemToggleMsg*>(pObject.rawPtr()) );

	return 0;
}

bool WgItemToggleMsg::isSet() const
{
	return m_bSet;
}

//____ WgItemMousePressMsg ________________________________________________________

const char WgItemMousePressMsg::CLASSNAME[] = {"ItemMousePressMsg"};

WgItemMousePressMsg::WgItemMousePressMsg( WgObject * pSource, int itemId, const WgObjectPtr& pItemObject, WgMouseButton button ) : WgItemMsg(pSource,itemId,pItemObject)
{
	m_type = WG_MSG_ITEM_MOUSE_PRESS;
	m_button = button;
}

bool WgItemMousePressMsg::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgItemMsg::isInstanceOf(pClassName);
}

const char * WgItemMousePressMsg::className( void ) const
{ 
	return CLASSNAME; 
}

WgItemMousePressMsgPtr WgItemMousePressMsg::cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgItemMousePressMsgPtr( static_cast<WgItemMousePressMsg*>(pObject.rawPtr()) );

	return 0;
}

WgMouseButton WgItemMousePressMsg::button() const
{
	return m_button;
}

//____ WgItemListMsg ________________________________________________________

const char WgItemListMsg::CLASSNAME[] = {"ItemListMsg"};

WgItemListMsg::WgItemListMsg( WgObject * pSource, int nbItems, WgItemInfo * pItems )
{
	m_pSource 	= pSource;
	m_nbItems	= nbItems;
	m_pItems	= pItems;
}

WgItemListMsg::~WgItemListMsg()
{
	delete [] m_pItems;
}

bool WgItemListMsg::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgMsg::isInstanceOf(pClassName);
}

const char * WgItemListMsg::className( void ) const
{ 
	return CLASSNAME; 
}

WgItemListMsgPtr WgItemListMsg::cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgItemListMsgPtr( static_cast<WgItemListMsg*>(pObject.rawPtr()) );

	return 0;
}

int WgItemListMsg::nbItems() const
{
	return m_nbItems;
}

const WgItemInfo * WgItemListMsg::items() const
{
	return m_pItems;
}

//____ WgItemsSelectMsg ________________________________________________________

const char WgItemsSelectMsg::CLASSNAME[] = {"ItemsSelectMsg"};

WgItemsSelectMsg::WgItemsSelectMsg( WgObject * pSource, int nbItems, WgItemInfo * pItems ) : WgItemListMsg(pSource,nbItems,pItems)
{
	m_type = WG_MSG_ITEMS_SELECT;
}

bool WgItemsSelectMsg::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgItemListMsg::isInstanceOf(pClassName);
}

const char * WgItemsSelectMsg::className( void ) const
{ 
	return CLASSNAME; 
}

WgItemsSelectMsgPtr WgItemsSelectMsg::cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgItemsSelectMsgPtr( static_cast<WgItemsSelectMsg*>(pObject.rawPtr()) );

	return 0;
}

//____ WgItemsUnselectMsg ________________________________________________________

const char WgItemsUnselectMsg::CLASSNAME[] = {"ItemsUnselectMsg"};

WgItemsUnselectMsg::WgItemsUnselectMsg( WgObject * pSource, int nbItems, WgItemInfo * pItems ) : WgItemListMsg(pSource,nbItems,pItems)
{
	m_type = WG_MSG_ITEMS_UNSELECT;
}

bool WgItemsUnselectMsg::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgItemListMsg::isInstanceOf(pClassName);
}

const char * WgItemsUnselectMsg::className( void ) const
{ 
	return CLASSNAME; 
}

WgItemsUnselectMsgPtr WgItemsUnselectMsg::cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgItemsUnselectMsgPtr( static_cast<WgItemsUnselectMsg*>(pObject.rawPtr()) );

	return 0;
}

//____ WgPopupClosedMsg ____________________________________________

const char WgPopupClosedMsg::CLASSNAME[] = {"PopupClosedMsg"};

WgPopupClosedMsg::WgPopupClosedMsg( WgWidget * pPopup, const WgWidgetWeakPtr& pCaller )
{
	m_type			= WG_MSG_POPUP_CLOSED;
	m_pCopyTo		= pCaller.rawPtr();
	m_pSource		= pPopup;
}

bool WgPopupClosedMsg::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgMsg::isInstanceOf(pClassName);
}

const char * WgPopupClosedMsg::className( void ) const
{ 
	return CLASSNAME; 
}

WgPopupClosedMsgPtr WgPopupClosedMsg::cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgPopupClosedMsgPtr( static_cast<WgPopupClosedMsg*>(pObject.rawPtr()) );

	return 0;
}

//____ WgModalMoveOutsideMsg ___________________________________________________

const char WgModalMoveOutsideMsg::CLASSNAME[] = {"ModalMoveOutsideMsg"};

WgModalMoveOutsideMsg::WgModalMoveOutsideMsg( WgWidget * pSource )
{
	m_type = WG_MSG_MODAL_MOVE_OUTSIDE;
	m_pSource		= pSource;
}

bool WgModalMoveOutsideMsg::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgMsg::isInstanceOf(pClassName);
}

const char * WgModalMoveOutsideMsg::className( void ) const
{ 
	return CLASSNAME; 
}

WgModalMoveOutsideMsgPtr WgModalMoveOutsideMsg::cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgModalMoveOutsideMsgPtr( static_cast<WgModalMoveOutsideMsg*>(pObject.rawPtr()) );

	return 0;
}


//____ WgModalBlockedPressMsg _______________________________________________

const char WgModalBlockedPressMsg::CLASSNAME[] = {"ModalBlockedPressMsg"};

WgModalBlockedPressMsg::WgModalBlockedPressMsg( WgMouseButton button, WgWidget * pSource ) : WgMouseButtonMsg(button)
{
	m_type			= WG_MSG_MODAL_BLOCKED_PRESS;
	m_pSource 		= pSource;
}

bool WgModalBlockedPressMsg::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgMsg::isInstanceOf(pClassName);
}

const char * WgModalBlockedPressMsg::className( void ) const
{ 
	return CLASSNAME; 
}

WgModalBlockedPressMsgPtr WgModalBlockedPressMsg::cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgModalBlockedPressMsgPtr( static_cast<WgModalBlockedPressMsg*>(pObject.rawPtr()) );

	return 0;
}

//____ WgModalBlockedReleaseMsg _____________________________________________

const char WgModalBlockedReleaseMsg::CLASSNAME[] = {"ModalBlockedReleaseMsg"};

WgModalBlockedReleaseMsg::WgModalBlockedReleaseMsg( WgMouseButton button, WgWidget * pSource ) : WgMouseButtonMsg(button)
{
	m_type			= WG_MSG_MODAL_BLOCKED_RELEASE;
	m_pSource 		= pSource;
}

bool WgModalBlockedReleaseMsg::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgMsg::isInstanceOf(pClassName);
}

const char * WgModalBlockedReleaseMsg::className( void ) const
{ 
	return CLASSNAME; 
}

WgModalBlockedReleaseMsgPtr WgModalBlockedReleaseMsg::cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgModalBlockedReleaseMsgPtr( static_cast<WgModalBlockedReleaseMsg*>(pObject.rawPtr()) );

	return 0;
}

//____ WgLinkMsg __________________________________________________

const char WgLinkMsg::CLASSNAME[] = {"LinkMsg"};

bool WgLinkMsg::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgMsg::isInstanceOf(pClassName);
}

const char * WgLinkMsg::className( void ) const
{ 
	return CLASSNAME; 
}

WgLinkMsgPtr WgLinkMsg::cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgLinkMsgPtr( static_cast<WgLinkMsg*>(pObject.rawPtr()) );

	return 0;
}

//____ WgLinkSelectMsg __________________________________________________

const char WgLinkSelectMsg::CLASSNAME[] = {"LinkSelectMsg"};

WgLinkSelectMsg::WgLinkSelectMsg( const WgTextLinkPtr& pLink )
{
	m_type			= WG_MSG_LINK_SELECT;
	m_pSource		= pLink;
}

bool WgLinkSelectMsg::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgLinkMsg::isInstanceOf(pClassName);
}

const char * WgLinkSelectMsg::className( void ) const
{ 
	return CLASSNAME; 
}

WgLinkSelectMsgPtr WgLinkSelectMsg::cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgLinkSelectMsgPtr( static_cast<WgLinkSelectMsg*>(pObject.rawPtr()) );

	return 0;
}

//____ WgLinkMouseEnterMsg __________________________________________________

const char WgLinkMouseEnterMsg::CLASSNAME[] = {"LinkMouseEnterMsg"};

WgLinkMouseEnterMsg::WgLinkMouseEnterMsg( const WgTextLinkPtr& pLink )
{
	m_type			= WG_MSG_LINK_MOUSE_ENTER;
	m_pSource		= pLink;
}

bool WgLinkMouseEnterMsg::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgLinkMsg::isInstanceOf(pClassName);
}

const char * WgLinkMouseEnterMsg::className( void ) const
{ 
	return CLASSNAME; 
}

WgLinkMouseEnterMsgPtr WgLinkMouseEnterMsg::cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgLinkMouseEnterMsgPtr( static_cast<WgLinkMouseEnterMsg*>(pObject.rawPtr()) );

	return 0;
}

//____ WgLinkMouseLeaveMsg __________________________________________________

const char WgLinkMouseLeaveMsg::CLASSNAME[] = {"LinkMouseLeaveMsg"};

WgLinkMouseLeaveMsg::WgLinkMouseLeaveMsg( const WgTextLinkPtr& pLink )
{
	m_type			= WG_MSG_LINK_MOUSE_LEAVE;
	m_pSource		= pLink;
}

bool WgLinkMouseLeaveMsg::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgLinkMsg::isInstanceOf(pClassName);
}

const char * WgLinkMouseLeaveMsg::className( void ) const
{ 
	return CLASSNAME; 
}

WgLinkMouseLeaveMsgPtr WgLinkMouseLeaveMsg::cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgLinkMouseLeaveMsgPtr( static_cast<WgLinkMouseLeaveMsg*>(pObject.rawPtr()) );

	return 0;
}

//____ WgLinkMouseButtonMsg __________________________________________________

const char WgLinkMouseButtonMsg::CLASSNAME[] = {"LinkMouseButtonMsg"};

WgMouseButton WgLinkMouseButtonMsg::button() const
{
	return m_button;
}

bool WgLinkMouseButtonMsg::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgLinkMsg::isInstanceOf(pClassName);
}

const char * WgLinkMouseButtonMsg::className( void ) const
{ 
	return CLASSNAME; 
}

WgLinkMouseButtonMsgPtr WgLinkMouseButtonMsg::cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgLinkMouseButtonMsgPtr( static_cast<WgLinkMouseButtonMsg*>(pObject.rawPtr()) );

	return 0;
}

//____ WgLinkMousePressMsg __________________________________________________

const char WgLinkMousePressMsg::CLASSNAME[] = {"LinkMousePressMsg"};

WgLinkMousePressMsg::WgLinkMousePressMsg( const WgTextLinkPtr& pLink, WgMouseButton button )
{
	m_type			= WG_MSG_LINK_MOUSE_PRESS;
	m_pSource		= pLink;
	m_button		= button;
}

bool WgLinkMousePressMsg::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgLinkMouseButtonMsg::isInstanceOf(pClassName);
}

const char * WgLinkMousePressMsg::className( void ) const
{ 
	return CLASSNAME; 
}

WgLinkMousePressMsgPtr WgLinkMousePressMsg::cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgLinkMousePressMsgPtr( static_cast<WgLinkMousePressMsg*>(pObject.rawPtr()) );

	return 0;
}

//____ WgLinkMouseRepeatMsg __________________________________________________

const char WgLinkMouseRepeatMsg::CLASSNAME[] = {"LinkMouseRepeatMsg"};

WgLinkMouseRepeatMsg::WgLinkMouseRepeatMsg( const WgTextLinkPtr& pLink, WgMouseButton button )
{
	m_type			= WG_MSG_LINK_MOUSE_REPEAT;
	m_pSource		= pLink;
	m_button		= button;
}

bool WgLinkMouseRepeatMsg::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgLinkMouseButtonMsg::isInstanceOf(pClassName);
}

const char * WgLinkMouseRepeatMsg::className( void ) const
{ 
	return CLASSNAME; 
}

WgLinkMouseRepeatMsgPtr WgLinkMouseRepeatMsg::cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgLinkMouseRepeatMsgPtr( static_cast<WgLinkMouseRepeatMsg*>(pObject.rawPtr()) );

	return 0;
}

//____ WgLinkMouseReleaseMsg __________________________________________________

const char WgLinkMouseReleaseMsg::CLASSNAME[] = {"LinkMouseReleaseMsg"};

WgLinkMouseReleaseMsg::WgLinkMouseReleaseMsg( const WgTextLinkPtr& pLink, WgMouseButton button )
{
	m_type			= WG_MSG_LINK_MOUSE_RELEASE;
	m_pSource		= pLink;
	m_button		= button;
}

bool WgLinkMouseReleaseMsg::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgLinkMouseButtonMsg::isInstanceOf(pClassName);
}

const char * WgLinkMouseReleaseMsg::className( void ) const
{ 
	return CLASSNAME; 
}

WgLinkMouseReleaseMsgPtr WgLinkMouseReleaseMsg::cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgLinkMouseReleaseMsgPtr( static_cast<WgLinkMouseReleaseMsg*>(pObject.rawPtr()) );

	return 0;
}

//____ WgLinkMouseClickMsg __________________________________________________

const char WgLinkMouseClickMsg::CLASSNAME[] = {"LinkMouseClickMsg"};

WgLinkMouseClickMsg::WgLinkMouseClickMsg( const WgTextLinkPtr& pLink, WgMouseButton button )
{
	m_type			= WG_MSG_LINK_MOUSE_CLICK;
	m_pSource		= pLink;
	m_button		= button;
}

bool WgLinkMouseClickMsg::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgLinkMouseButtonMsg::isInstanceOf(pClassName);
}

const char * WgLinkMouseClickMsg::className( void ) const
{ 
	return CLASSNAME; 
}

WgLinkMouseClickMsgPtr WgLinkMouseClickMsg::cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgLinkMouseClickMsgPtr( static_cast<WgLinkMouseClickMsg*>(pObject.rawPtr()) );

	return 0;
}

//____ WgLinkMouseDoubleClickMsg __________________________________________________

const char WgLinkMouseDoubleClickMsg::CLASSNAME[] = {"LinkMouseDoubleClickMsg"};

WgLinkMouseDoubleClickMsg::WgLinkMouseDoubleClickMsg( const WgTextLinkPtr& pLink, WgMouseButton button )
{
	m_type			= WG_MSG_LINK_MOUSE_DOUBLE_CLICK;
	m_pSource		= pLink;
	m_button		= button;
}

bool WgLinkMouseDoubleClickMsg::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgLinkMouseButtonMsg::isInstanceOf(pClassName);
}

const char * WgLinkMouseDoubleClickMsg::className( void ) const
{ 
	return CLASSNAME; 
}

WgLinkMouseDoubleClickMsgPtr WgLinkMouseDoubleClickMsg::cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgLinkMouseDoubleClickMsgPtr( static_cast<WgLinkMouseDoubleClickMsg*>(pObject.rawPtr()) );

	return 0;
}


//____ WgMousePositionMsg _______________________________________________________

const char WgMousePositionMsg::CLASSNAME[] = {"MousePositionMsg"};

WgMousePositionMsg::WgMousePositionMsg()
{
	m_type = WG_MSG_MOUSE_POSITION;
}

bool WgMousePositionMsg::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgMsg::isInstanceOf(pClassName);
}

const char * WgMousePositionMsg::className( void ) const
{ 
	return CLASSNAME; 
}

WgMousePositionMsgPtr WgMousePositionMsg::cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgMousePositionMsgPtr( static_cast<WgMousePositionMsg*>(pObject.rawPtr()) );

	return 0;
}

//____ WgMouseDragMsg _________________________________________________________

const char WgMouseDragMsg::CLASSNAME[] = {"MouseDragMsg"};

WgMouseDragMsg::WgMouseDragMsg( WgMouseButton button, const WgCoord& startPos, const WgCoord& prevPos, const WgCoord& currPos ) : WgMouseButtonMsg(button)
{
	m_type = WG_MSG_MOUSE_DRAG;

	m_startPos = startPos;
	m_prevPos = prevPos;
	m_currPos = currPos;
}

WgMouseDragMsg::WgMouseDragMsg( WgMouseButton button, WgWidget * pWidget, const WgCoord& startPos, const WgCoord& prevPos, const WgCoord& currPos ) : WgMouseButtonMsg(button)
{
	m_type 			= WG_MSG_MOUSE_DRAG;
	m_pSource		= pWidget;
	m_pCopyTo	= pWidget;

	m_startPos = startPos;
	m_prevPos = prevPos;
	m_currPos = currPos;
}

bool WgMouseDragMsg::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgMouseButtonMsg::isInstanceOf(pClassName);
}

const char * WgMouseDragMsg::className( void ) const
{ 
	return CLASSNAME; 
}

WgMouseDragMsgPtr WgMouseDragMsg::cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgMouseDragMsgPtr( static_cast<WgMouseDragMsg*>(pObject.rawPtr()) );

	return 0;
}

WgCoord WgMouseDragMsg::draggedTotal() const
{
	return m_currPos - m_startPos;
}

WgCoord WgMouseDragMsg::draggedNow() const
{
	return m_currPos - m_prevPos;
}

WgCoord WgMouseDragMsg::currPos() const
{
	return m_currPos;
}

WgCoord WgMouseDragMsg::startPos() const
{
	return m_startPos;
}

WgCoord WgMouseDragMsg::prevPos() const
{
	return m_prevPos;
}

