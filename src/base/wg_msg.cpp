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

bool WgMsg::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgObject::IsInstanceOf(pClassName);
}

const char * WgMsg::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgMsgPtr WgMsg::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgMsgPtr( static_cast<WgMsg*>(pObject.RawPtr()) );

	return 0;
}

void WgMsg::SetCopyTo( const WgReceiverPtr& pReceiver )
{
	m_pCopyTo = pReceiver;
}

void WgMsg::SetRepost( const WgObjectPtr& pSource, const WgReceiverPtr& pCopyTo )
{
	m_pRepostSource = pSource;	
	m_pRepostCopyTo = pCopyTo;
}

void WgMsg::Swallow()
{
	m_pRepostSource = 0;
	m_pRepostCopyTo = 0;
}

bool WgMsg::DoRepost()
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



bool WgMsg::IsMouseMsg() const
{
	if( m_type == WG_MSG_WHEEL_ROLL ||
		m_type == WG_MSG_MOUSE_ENTER ||
		m_type == WG_MSG_MOUSE_LEAVE ||	
		m_type == WG_MSG_MOUSE_MOVE ||	
		m_type == WG_MSG_MOUSE_POSITION ||
		IsMouseButtonMsg() )
		return true;
	else
		return false;
			
}
	
bool WgMsg::IsMouseButtonMsg() const
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
	
bool WgMsg::IsKeyMsg() const
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

bool WgKeyMsg::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgMsg::IsInstanceOf(pClassName);
}

const char * WgKeyMsg::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgKeyMsgPtr WgKeyMsg::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgKeyMsgPtr( static_cast<WgKeyMsg*>(pObject.RawPtr()) );

	return 0;
}

bool WgKeyMsg::IsCursorKey() const
{
	if( m_translatedKeyCode == WG_KEY_UP || m_translatedKeyCode == WG_KEY_DOWN ||
		m_translatedKeyCode == WG_KEY_LEFT || m_translatedKeyCode == WG_KEY_RIGHT )
		return true;
	else
		return false;
}
	
bool WgKeyMsg::IsMovementKey() const
{
	if( m_translatedKeyCode == WG_KEY_PAGE_UP || m_translatedKeyCode == WG_KEY_PAGE_DOWN ||
		m_translatedKeyCode == WG_KEY_HOME || m_translatedKeyCode == WG_KEY_END ||
		IsCursorKey() )
		return true;
	else
		return false;
}
	
//____ WgMouseButtonMsg _____________________________________________________

const char WgMouseButtonMsg::CLASSNAME[] = {"MouseButtonMsg"};

bool WgMouseButtonMsg::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgMsg::IsInstanceOf(pClassName);
}

const char * WgMouseButtonMsg::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgMouseButtonMsgPtr WgMouseButtonMsg::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgMouseButtonMsgPtr( static_cast<WgMouseButtonMsg*>(pObject.RawPtr()) );

	return 0;
}


//____ WgFocusGainedMsg ________________________________________________________

const char WgFocusGainedMsg::CLASSNAME[] = {"FocusGainedMsg"};

WgFocusGainedMsg::WgFocusGainedMsg()
{
	m_type = WG_MSG_FOCUS_GAINED;
}

bool WgFocusGainedMsg::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgMsg::IsInstanceOf(pClassName);
}

const char * WgFocusGainedMsg::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgFocusGainedMsgPtr WgFocusGainedMsg::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgFocusGainedMsgPtr( static_cast<WgFocusGainedMsg*>(pObject.RawPtr()) );

	return 0;
}


//____ WgFocusLostMsg __________________________________________________________

const char WgFocusLostMsg::CLASSNAME[] = {"FocusLostMsg"};

WgFocusLostMsg::WgFocusLostMsg()
{
	m_type = WG_MSG_FOCUS_LOST;
}

bool WgFocusLostMsg::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgMsg::IsInstanceOf(pClassName);
}

const char * WgFocusLostMsg::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgFocusLostMsgPtr WgFocusLostMsg::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgFocusLostMsgPtr( static_cast<WgFocusLostMsg*>(pObject.RawPtr()) );

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

bool WgMouseEnterMsg::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgMsg::IsInstanceOf(pClassName);
}

const char * WgMouseEnterMsg::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgMouseEnterMsgPtr WgMouseEnterMsg::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgMouseEnterMsgPtr( static_cast<WgMouseEnterMsg*>(pObject.RawPtr()) );

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

bool WgMouseLeaveMsg::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgMsg::IsInstanceOf(pClassName);
}

const char * WgMouseLeaveMsg::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgMouseLeaveMsgPtr WgMouseLeaveMsg::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgMouseLeaveMsgPtr( static_cast<WgMouseLeaveMsg*>(pObject.RawPtr()) );

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

bool WgMouseMoveMsg::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgMsg::IsInstanceOf(pClassName);
}

const char * WgMouseMoveMsg::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgMouseMoveMsgPtr WgMouseMoveMsg::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgMouseMoveMsgPtr( static_cast<WgMouseMoveMsg*>(pObject.RawPtr()) );

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

bool WgMousePressMsg::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgMouseButtonMsg::IsInstanceOf(pClassName);
}

const char * WgMousePressMsg::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgMousePressMsgPtr WgMousePressMsg::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgMousePressMsgPtr( static_cast<WgMousePressMsg*>(pObject.RawPtr()) );

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

bool WgMouseRepeatMsg::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgMouseButtonMsg::IsInstanceOf(pClassName);
}

const char * WgMouseRepeatMsg::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgMouseRepeatMsgPtr WgMouseRepeatMsg::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgMouseRepeatMsgPtr( static_cast<WgMouseRepeatMsg*>(pObject.RawPtr()) );

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

bool WgMouseReleaseMsg::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgMouseButtonMsg::IsInstanceOf(pClassName);
}

const char * WgMouseReleaseMsg::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgMouseReleaseMsgPtr WgMouseReleaseMsg::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgMouseReleaseMsgPtr( static_cast<WgMouseReleaseMsg*>(pObject.RawPtr()) );

	return 0;
}

bool WgMouseReleaseMsg::PressInside() const
{
	return m_bPressInside;
}

bool WgMouseReleaseMsg::ReleaseInside() const
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

bool WgMouseClickMsg::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgMouseButtonMsg::IsInstanceOf(pClassName);
}

const char * WgMouseClickMsg::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgMouseClickMsgPtr WgMouseClickMsg::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgMouseClickMsgPtr( static_cast<WgMouseClickMsg*>(pObject.RawPtr()) );

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

bool WgMouseDoubleClickMsg::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgMouseButtonMsg::IsInstanceOf(pClassName);
}

const char * WgMouseDoubleClickMsg::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgMouseDoubleClickMsgPtr WgMouseDoubleClickMsg::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgMouseDoubleClickMsgPtr( static_cast<WgMouseDoubleClickMsg*>(pObject.RawPtr()) );

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

bool WgKeyPressMsg::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgKeyMsg::IsInstanceOf(pClassName);
}

const char * WgKeyPressMsg::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgKeyPressMsgPtr WgKeyPressMsg::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgKeyPressMsgPtr( static_cast<WgKeyPressMsg*>(pObject.RawPtr()) );

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

bool WgKeyRepeatMsg::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgKeyMsg::IsInstanceOf(pClassName);
}

const char * WgKeyRepeatMsg::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgKeyRepeatMsgPtr WgKeyRepeatMsg::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgKeyRepeatMsgPtr( static_cast<WgKeyRepeatMsg*>(pObject.RawPtr()) );

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

bool WgKeyReleaseMsg::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgKeyMsg::IsInstanceOf(pClassName);
}

const char * WgKeyReleaseMsg::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgKeyReleaseMsgPtr WgKeyReleaseMsg::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgKeyReleaseMsgPtr( static_cast<WgKeyReleaseMsg*>(pObject.RawPtr()) );

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

unsigned short WgCharacterMsg::Char() const
{
	return m_char;
}

bool WgCharacterMsg::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgMsg::IsInstanceOf(pClassName);
}

const char * WgCharacterMsg::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgCharacterMsgPtr WgCharacterMsg::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgCharacterMsgPtr( static_cast<WgCharacterMsg*>(pObject.RawPtr()) );

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

int WgWheelRollMsg::Wheel() const
{
	return m_wheel;
}

int WgWheelRollMsg::Distance() const
{
	return m_distance;
}

bool WgWheelRollMsg::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgMsg::IsInstanceOf(pClassName);
}

const char * WgWheelRollMsg::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgWheelRollMsgPtr WgWheelRollMsg::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgWheelRollMsgPtr( static_cast<WgWheelRollMsg*>(pObject.RawPtr()) );

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
	
int WgTickMsg::Millisec() const
{
	return m_millisec;
}

bool WgTickMsg::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgMsg::IsInstanceOf(pClassName);
}

const char * WgTickMsg::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgTickMsgPtr WgTickMsg::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgTickMsgPtr( static_cast<WgTickMsg*>(pObject.RawPtr()) );

	return 0;
}

//____ WgPointerChangeMsg _______________________________________________________________

const char WgPointerChangeMsg::CLASSNAME[] = {"PointerChangeMsg"};

WgPointerChangeMsg::WgPointerChangeMsg( WgPointerStyle style )
{
	m_type = WG_MSG_POINTER_CHANGE;
	m_style = style;
}
	
WgPointerStyle WgPointerChangeMsg::Style() const
{
	return m_style;
}

bool WgPointerChangeMsg::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgMsg::IsInstanceOf(pClassName);
}

const char * WgPointerChangeMsg::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgPointerChangeMsgPtr WgPointerChangeMsg::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgPointerChangeMsgPtr( static_cast<WgPointerChangeMsg*>(pObject.RawPtr()) );

	return 0;
}

//____ WgSelectMsg _________________________________________________________

const char WgSelectMsg::CLASSNAME[] = {"SelectMsg"};

WgSelectMsg::WgSelectMsg( WgObject * pSource )
{
	m_type 			= WG_MSG_SELECT;
	m_pSource 		= pSource;
}

bool WgSelectMsg::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgMsg::IsInstanceOf(pClassName);
}

const char * WgSelectMsg::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgSelectMsgPtr WgSelectMsg::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgSelectMsgPtr( static_cast<WgSelectMsg*>(pObject.RawPtr()) );

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

bool WgToggleMsg::IsSet() const
{
	return m_bSet;
}

bool WgToggleMsg::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgMsg::IsInstanceOf(pClassName);
}

const char * WgToggleMsg::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgToggleMsgPtr WgToggleMsg::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgToggleMsgPtr( static_cast<WgToggleMsg*>(pObject.RawPtr()) );

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

int64_t WgValueUpdateMsg::Value() const
{
	return m_value;
}

double WgValueUpdateMsg::Fraction() const
{
	return m_fraction;
}

bool WgValueUpdateMsg::IsFinal() const
{
	return m_bFinal;
}

bool WgValueUpdateMsg::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgMsg::IsInstanceOf(pClassName);
}

const char * WgValueUpdateMsg::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgValueUpdateMsgPtr WgValueUpdateMsg::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgValueUpdateMsgPtr( static_cast<WgValueUpdateMsg*>(pObject.RawPtr()) );

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

bool WgRangeUpdateMsg::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgMsg::IsInstanceOf(pClassName);
}

const char * WgRangeUpdateMsg::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgRangeUpdateMsgPtr WgRangeUpdateMsg::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgRangeUpdateMsgPtr( static_cast<WgRangeUpdateMsg*>(pObject.RawPtr()) );

	return 0;
}

int WgRangeUpdateMsg::Offset() const
{
	return m_ofs;
}

int WgRangeUpdateMsg::Length() const
{
	return m_length;
}

double WgRangeUpdateMsg::FracOffset() const
{
	return m_fracOfs;
}

double WgRangeUpdateMsg::FracLength() const
{
	return m_fracLength;
}

bool WgRangeUpdateMsg::IsFinal() const
{
	return m_bFinal;
}


//____ WgTextEditMsg ________________________________________________________

const char WgTextEditMsg::CLASSNAME[] = {"TextEditMsg"};

WgEditTextPtr WgTextEditMsg::Text() const
{
	return m_pText;
}

WgTextEditMsg::WgTextEditMsg( const WgEditTextPtr& pText, bool bFinal )
{
	m_type 		= WG_MSG_TEXT_EDIT;
	m_pSource 	= pText->Holder();
	m_pText 	= pText;
	m_bFinal	= bFinal;
}

bool WgTextEditMsg::IsFinal() const
{
	return m_bFinal;
}

bool WgTextEditMsg::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgMsg::IsInstanceOf(pClassName);
}

const char * WgTextEditMsg::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgTextEditMsgPtr WgTextEditMsg::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgTextEditMsgPtr( static_cast<WgTextEditMsg*>(pObject.RawPtr()) );

	return 0;
}

//____ WgItemMsg ________________________________________________________

const char WgItemMsg::CLASSNAME[] = {"ItemMsg"};

WgItemMsg::WgItemMsg( WgObject * pSource, int itemId, const WgObjectPtr& pItemObject ) : m_item(itemId,pItemObject)
{
	m_pSource 	= pSource;
}

bool WgItemMsg::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgMsg::IsInstanceOf(pClassName);
}

const char * WgItemMsg::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgItemMsgPtr WgItemMsg::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgItemMsgPtr( static_cast<WgItemMsg*>(pObject.RawPtr()) );

	return 0;
}

int WgItemMsg::ItemId() const 
{ 
	return m_item.id; 
}

WgObjectPtr WgItemMsg::ItemObject() const 
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

bool WgItemToggleMsg::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgItemMsg::IsInstanceOf(pClassName);
}

const char * WgItemToggleMsg::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgItemToggleMsgPtr WgItemToggleMsg::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgItemToggleMsgPtr( static_cast<WgItemToggleMsg*>(pObject.RawPtr()) );

	return 0;
}

bool WgItemToggleMsg::IsSet() const
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

bool WgItemMousePressMsg::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgItemMsg::IsInstanceOf(pClassName);
}

const char * WgItemMousePressMsg::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgItemMousePressMsgPtr WgItemMousePressMsg::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgItemMousePressMsgPtr( static_cast<WgItemMousePressMsg*>(pObject.RawPtr()) );

	return 0;
}

WgMouseButton WgItemMousePressMsg::Button() const
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

bool WgItemListMsg::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgMsg::IsInstanceOf(pClassName);
}

const char * WgItemListMsg::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgItemListMsgPtr WgItemListMsg::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgItemListMsgPtr( static_cast<WgItemListMsg*>(pObject.RawPtr()) );

	return 0;
}

int WgItemListMsg::NbItems() const
{
	return m_nbItems;
}

const WgItemInfo * WgItemListMsg::Items() const
{
	return m_pItems;
}

//____ WgItemsSelectMsg ________________________________________________________

const char WgItemsSelectMsg::CLASSNAME[] = {"ItemsSelectMsg"};

WgItemsSelectMsg::WgItemsSelectMsg( WgObject * pSource, int nbItems, WgItemInfo * pItems ) : WgItemListMsg(pSource,nbItems,pItems)
{
	m_type = WG_MSG_ITEMS_SELECT;
}

bool WgItemsSelectMsg::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgItemListMsg::IsInstanceOf(pClassName);
}

const char * WgItemsSelectMsg::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgItemsSelectMsgPtr WgItemsSelectMsg::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgItemsSelectMsgPtr( static_cast<WgItemsSelectMsg*>(pObject.RawPtr()) );

	return 0;
}

//____ WgItemsUnselectMsg ________________________________________________________

const char WgItemsUnselectMsg::CLASSNAME[] = {"ItemsUnselectMsg"};

WgItemsUnselectMsg::WgItemsUnselectMsg( WgObject * pSource, int nbItems, WgItemInfo * pItems ) : WgItemListMsg(pSource,nbItems,pItems)
{
	m_type = WG_MSG_ITEMS_UNSELECT;
}

bool WgItemsUnselectMsg::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgItemListMsg::IsInstanceOf(pClassName);
}

const char * WgItemsUnselectMsg::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgItemsUnselectMsgPtr WgItemsUnselectMsg::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgItemsUnselectMsgPtr( static_cast<WgItemsUnselectMsg*>(pObject.RawPtr()) );

	return 0;
}

//____ WgPopupClosedMsg ____________________________________________

const char WgPopupClosedMsg::CLASSNAME[] = {"PopupClosedMsg"};

WgPopupClosedMsg::WgPopupClosedMsg( WgWidget * pPopup, const WgWidgetWeakPtr& pCaller )
{
	m_type			= WG_MSG_POPUP_CLOSED;
	m_pCopyTo		= pCaller.RawPtr();
	m_pSource		= pPopup;
}

bool WgPopupClosedMsg::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgMsg::IsInstanceOf(pClassName);
}

const char * WgPopupClosedMsg::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgPopupClosedMsgPtr WgPopupClosedMsg::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgPopupClosedMsgPtr( static_cast<WgPopupClosedMsg*>(pObject.RawPtr()) );

	return 0;
}

//____ WgModalMoveOutsideMsg ___________________________________________________

const char WgModalMoveOutsideMsg::CLASSNAME[] = {"ModalMoveOutsideMsg"};

WgModalMoveOutsideMsg::WgModalMoveOutsideMsg( WgWidget * pSource )
{
	m_type = WG_MSG_MODAL_MOVE_OUTSIDE;
	m_pSource		= pSource;
}

bool WgModalMoveOutsideMsg::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgMsg::IsInstanceOf(pClassName);
}

const char * WgModalMoveOutsideMsg::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgModalMoveOutsideMsgPtr WgModalMoveOutsideMsg::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgModalMoveOutsideMsgPtr( static_cast<WgModalMoveOutsideMsg*>(pObject.RawPtr()) );

	return 0;
}


//____ WgModalBlockedPressMsg _______________________________________________

const char WgModalBlockedPressMsg::CLASSNAME[] = {"ModalBlockedPressMsg"};

WgModalBlockedPressMsg::WgModalBlockedPressMsg( WgMouseButton button, WgWidget * pSource ) : WgMouseButtonMsg(button)
{
	m_type			= WG_MSG_MODAL_BLOCKED_PRESS;
	m_pSource 		= pSource;
}

bool WgModalBlockedPressMsg::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgMsg::IsInstanceOf(pClassName);
}

const char * WgModalBlockedPressMsg::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgModalBlockedPressMsgPtr WgModalBlockedPressMsg::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgModalBlockedPressMsgPtr( static_cast<WgModalBlockedPressMsg*>(pObject.RawPtr()) );

	return 0;
}

//____ WgModalBlockedReleaseMsg _____________________________________________

const char WgModalBlockedReleaseMsg::CLASSNAME[] = {"ModalBlockedReleaseMsg"};

WgModalBlockedReleaseMsg::WgModalBlockedReleaseMsg( WgMouseButton button, WgWidget * pSource ) : WgMouseButtonMsg(button)
{
	m_type			= WG_MSG_MODAL_BLOCKED_RELEASE;
	m_pSource 		= pSource;
}

bool WgModalBlockedReleaseMsg::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgMsg::IsInstanceOf(pClassName);
}

const char * WgModalBlockedReleaseMsg::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgModalBlockedReleaseMsgPtr WgModalBlockedReleaseMsg::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgModalBlockedReleaseMsgPtr( static_cast<WgModalBlockedReleaseMsg*>(pObject.RawPtr()) );

	return 0;
}

//____ WgLinkMsg __________________________________________________

const char WgLinkMsg::CLASSNAME[] = {"LinkMsg"};

bool WgLinkMsg::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgMsg::IsInstanceOf(pClassName);
}

const char * WgLinkMsg::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgLinkMsgPtr WgLinkMsg::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgLinkMsgPtr( static_cast<WgLinkMsg*>(pObject.RawPtr()) );

	return 0;
}

//____ WgLinkSelectMsg __________________________________________________

const char WgLinkSelectMsg::CLASSNAME[] = {"LinkSelectMsg"};

WgLinkSelectMsg::WgLinkSelectMsg( const WgTextLinkPtr& pLink )
{
	m_type			= WG_MSG_LINK_SELECT;
	m_pSource		= pLink;
}

bool WgLinkSelectMsg::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgLinkMsg::IsInstanceOf(pClassName);
}

const char * WgLinkSelectMsg::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgLinkSelectMsgPtr WgLinkSelectMsg::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgLinkSelectMsgPtr( static_cast<WgLinkSelectMsg*>(pObject.RawPtr()) );

	return 0;
}

//____ WgLinkMouseEnterMsg __________________________________________________

const char WgLinkMouseEnterMsg::CLASSNAME[] = {"LinkMouseEnterMsg"};

WgLinkMouseEnterMsg::WgLinkMouseEnterMsg( const WgTextLinkPtr& pLink )
{
	m_type			= WG_MSG_LINK_MOUSE_ENTER;
	m_pSource		= pLink;
}

bool WgLinkMouseEnterMsg::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgLinkMsg::IsInstanceOf(pClassName);
}

const char * WgLinkMouseEnterMsg::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgLinkMouseEnterMsgPtr WgLinkMouseEnterMsg::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgLinkMouseEnterMsgPtr( static_cast<WgLinkMouseEnterMsg*>(pObject.RawPtr()) );

	return 0;
}

//____ WgLinkMouseLeaveMsg __________________________________________________

const char WgLinkMouseLeaveMsg::CLASSNAME[] = {"LinkMouseLeaveMsg"};

WgLinkMouseLeaveMsg::WgLinkMouseLeaveMsg( const WgTextLinkPtr& pLink )
{
	m_type			= WG_MSG_LINK_MOUSE_LEAVE;
	m_pSource		= pLink;
}

bool WgLinkMouseLeaveMsg::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgLinkMsg::IsInstanceOf(pClassName);
}

const char * WgLinkMouseLeaveMsg::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgLinkMouseLeaveMsgPtr WgLinkMouseLeaveMsg::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgLinkMouseLeaveMsgPtr( static_cast<WgLinkMouseLeaveMsg*>(pObject.RawPtr()) );

	return 0;
}

//____ WgLinkMouseButtonMsg __________________________________________________

const char WgLinkMouseButtonMsg::CLASSNAME[] = {"LinkMouseButtonMsg"};

WgMouseButton WgLinkMouseButtonMsg::Button() const
{
	return m_button;
}

bool WgLinkMouseButtonMsg::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgLinkMsg::IsInstanceOf(pClassName);
}

const char * WgLinkMouseButtonMsg::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgLinkMouseButtonMsgPtr WgLinkMouseButtonMsg::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgLinkMouseButtonMsgPtr( static_cast<WgLinkMouseButtonMsg*>(pObject.RawPtr()) );

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

bool WgLinkMousePressMsg::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgLinkMouseButtonMsg::IsInstanceOf(pClassName);
}

const char * WgLinkMousePressMsg::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgLinkMousePressMsgPtr WgLinkMousePressMsg::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgLinkMousePressMsgPtr( static_cast<WgLinkMousePressMsg*>(pObject.RawPtr()) );

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

bool WgLinkMouseRepeatMsg::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgLinkMouseButtonMsg::IsInstanceOf(pClassName);
}

const char * WgLinkMouseRepeatMsg::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgLinkMouseRepeatMsgPtr WgLinkMouseRepeatMsg::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgLinkMouseRepeatMsgPtr( static_cast<WgLinkMouseRepeatMsg*>(pObject.RawPtr()) );

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

bool WgLinkMouseReleaseMsg::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgLinkMouseButtonMsg::IsInstanceOf(pClassName);
}

const char * WgLinkMouseReleaseMsg::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgLinkMouseReleaseMsgPtr WgLinkMouseReleaseMsg::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgLinkMouseReleaseMsgPtr( static_cast<WgLinkMouseReleaseMsg*>(pObject.RawPtr()) );

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

bool WgLinkMouseClickMsg::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgLinkMouseButtonMsg::IsInstanceOf(pClassName);
}

const char * WgLinkMouseClickMsg::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgLinkMouseClickMsgPtr WgLinkMouseClickMsg::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgLinkMouseClickMsgPtr( static_cast<WgLinkMouseClickMsg*>(pObject.RawPtr()) );

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

bool WgLinkMouseDoubleClickMsg::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgLinkMouseButtonMsg::IsInstanceOf(pClassName);
}

const char * WgLinkMouseDoubleClickMsg::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgLinkMouseDoubleClickMsgPtr WgLinkMouseDoubleClickMsg::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgLinkMouseDoubleClickMsgPtr( static_cast<WgLinkMouseDoubleClickMsg*>(pObject.RawPtr()) );

	return 0;
}


//____ WgMousePositionMsg _______________________________________________________

const char WgMousePositionMsg::CLASSNAME[] = {"MousePositionMsg"};

WgMousePositionMsg::WgMousePositionMsg()
{
	m_type = WG_MSG_MOUSE_POSITION;
}

bool WgMousePositionMsg::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgMsg::IsInstanceOf(pClassName);
}

const char * WgMousePositionMsg::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgMousePositionMsgPtr WgMousePositionMsg::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgMousePositionMsgPtr( static_cast<WgMousePositionMsg*>(pObject.RawPtr()) );

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

bool WgMouseDragMsg::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgMouseButtonMsg::IsInstanceOf(pClassName);
}

const char * WgMouseDragMsg::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgMouseDragMsgPtr WgMouseDragMsg::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgMouseDragMsgPtr( static_cast<WgMouseDragMsg*>(pObject.RawPtr()) );

	return 0;
}

WgCoord WgMouseDragMsg::DraggedTotal() const
{
	return m_currPos - m_startPos;
}

WgCoord WgMouseDragMsg::DraggedNow() const
{
	return m_currPos - m_prevPos;
}

WgCoord WgMouseDragMsg::CurrPos() const
{
	return m_currPos;
}

WgCoord WgMouseDragMsg::StartPos() const
{
	return m_startPos;
}

WgCoord WgMouseDragMsg::PrevPos() const
{
	return m_prevPos;
}

