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

#include <wg_event.h>
#include <wg_key.h>
#include <wg_widget.h>
#include <wg_button.h>
#include <wg_checkbox.h>
#include <wg_animplayer.h>
#include <wg_tablist.h>
#include <wg_valuedisplay.h>
#include <wg_valueeditor.h>
#include <wg_scrollbar.h>
#include <wg_textdisplay.h>
#include <wg_lineeditor.h>
#include <wg_menu.h>
#include <wg_text.h>
//#include <wg_tablepanel.h>


//____ WgEvent ______________________________________________________________

const char WgEvent::CLASSNAME[] = {"Event"};

bool WgEvent::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgObject::IsInstanceOf(pClassName);
}

const char * WgEvent::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgEventPtr WgEvent::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgEventPtr( static_cast<WgEvent*>(pObject.GetRealPtr()) );

	return 0;
}

WgWidget * WgEvent::Widget() const
{
	return m_pWidget.GetRealPtr();
}


bool WgEvent::IsMouseEvent() const
{
	if( m_type == WG_EVENT_WHEEL_ROLL ||
		m_type == WG_EVENT_MOUSE_ENTER ||
		m_type == WG_EVENT_MOUSE_LEAVE ||	
		m_type == WG_EVENT_MOUSE_MOVE ||	
		m_type == WG_EVENT_MOUSE_POSITION ||
		IsMouseButtonEvent() )
		return true;
	else
		return false;
			
}
	
bool WgEvent::IsMouseButtonEvent() const
{
	if( m_type == WG_EVENT_MOUSE_CLICK ||
		m_type == WG_EVENT_MOUSE_DOUBLE_CLICK ||
		m_type == WG_EVENT_MOUSE_DRAG ||
		m_type == WG_EVENT_MOUSE_PRESS ||
		m_type == WG_EVENT_MOUSE_RELEASE ||
		m_type == WG_EVENT_MOUSE_REPEAT )
		return true;
	else
		return false;
}
	
bool WgEvent::IsKeyEvent() const
{
	if( m_type == WG_EVENT_KEY_PRESS ||
		m_type == WG_EVENT_KEY_RELEASE ||
		m_type == WG_EVENT_KEY_REPEAT )
		return true;
	else
		return false;
}

//____ WgKeyEvent ____________________________________________________________

const char WgKeyEvent::CLASSNAME[] = {"KeyEvent"};

bool WgKeyEvent::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgEvent::IsInstanceOf(pClassName);
}

const char * WgKeyEvent::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgKeyEventPtr WgKeyEvent::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgKeyEventPtr( static_cast<WgKeyEvent*>(pObject.GetRealPtr()) );

	return 0;
}

bool WgKeyEvent::IsCursorKey() const
{
	if( m_translatedKeyCode == WG_KEY_UP || m_translatedKeyCode == WG_KEY_DOWN ||
		m_translatedKeyCode == WG_KEY_LEFT || m_translatedKeyCode == WG_KEY_RIGHT )
		return true;
	else
		return false;
}
	
bool WgKeyEvent::IsMovementKey() const
{
	if( m_translatedKeyCode == WG_KEY_PAGE_UP || m_translatedKeyCode == WG_KEY_PAGE_DOWN ||
		m_translatedKeyCode == WG_KEY_HOME || m_translatedKeyCode == WG_KEY_END ||
		IsCursorKey() )
		return true;
	else
		return false;
}
	
//____ WgMouseButtonEvent _____________________________________________________

const char WgMouseButtonEvent::CLASSNAME[] = {"MouseButtonEvent"};

bool WgMouseButtonEvent::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgEvent::IsInstanceOf(pClassName);
}

const char * WgMouseButtonEvent::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgMouseButtonEventPtr WgMouseButtonEvent::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgMouseButtonEventPtr( static_cast<WgMouseButtonEvent*>(pObject.GetRealPtr()) );

	return 0;
}


//____ WgFocusGainedEvent ________________________________________________________

const char WgFocusGainedEvent::CLASSNAME[] = {"FocusGainedEvent"};

WgFocusGainedEvent::WgFocusGainedEvent()
{
	m_type = WG_EVENT_FOCUS_GAINED;
}

bool WgFocusGainedEvent::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgEvent::IsInstanceOf(pClassName);
}

const char * WgFocusGainedEvent::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgFocusGainedEventPtr WgFocusGainedEvent::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgFocusGainedEventPtr( static_cast<WgFocusGainedEvent*>(pObject.GetRealPtr()) );

	return 0;
}


//____ WgFocusLostEvent __________________________________________________________

const char WgFocusLostEvent::CLASSNAME[] = {"FocusLostEvent"};

WgFocusLostEvent::WgFocusLostEvent()
{
	m_type = WG_EVENT_FOCUS_LOST;
}

bool WgFocusLostEvent::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgEvent::IsInstanceOf(pClassName);
}

const char * WgFocusLostEvent::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgFocusLostEventPtr WgFocusLostEvent::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgFocusLostEventPtr( static_cast<WgFocusLostEvent*>(pObject.GetRealPtr()) );

	return 0;
}


//____ WgMouseEnterEvent _______________________________________________________

const char WgMouseEnterEvent::CLASSNAME[] = {"MouseEnterEvent"};

WgMouseEnterEvent::WgMouseEnterEvent( const WgCoord& pos )
{
	m_type = WG_EVENT_MOUSE_ENTER;

	m_pointerLocalPos = pos;
	m_pointerScreenPos = pos;
}

WgMouseEnterEvent::WgMouseEnterEvent( WgWidget * pWidget )
{
	m_type = WG_EVENT_MOUSE_ENTER;
	m_bIsForWidget	= true;
	m_pWidget 		= pWidget;
}

bool WgMouseEnterEvent::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgEvent::IsInstanceOf(pClassName);
}

const char * WgMouseEnterEvent::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgMouseEnterEventPtr WgMouseEnterEvent::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgMouseEnterEventPtr( static_cast<WgMouseEnterEvent*>(pObject.GetRealPtr()) );

	return 0;
}


//____ WgMouseLeaveEvent ________________________________________________________

const char WgMouseLeaveEvent::CLASSNAME[] = {"MouseLeaveEvent"};

WgMouseLeaveEvent::WgMouseLeaveEvent()
{
	m_type = WG_EVENT_MOUSE_LEAVE;
}

WgMouseLeaveEvent::WgMouseLeaveEvent( WgWidget * pWidget )
{
	m_type = WG_EVENT_MOUSE_LEAVE;
	m_bIsForWidget	= true;
	m_pWidget 		= pWidget;
}

bool WgMouseLeaveEvent::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgEvent::IsInstanceOf(pClassName);
}

const char * WgMouseLeaveEvent::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgMouseLeaveEventPtr WgMouseLeaveEvent::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgMouseLeaveEventPtr( static_cast<WgMouseLeaveEvent*>(pObject.GetRealPtr()) );

	return 0;
}

//____ WgMouseMoveEvent __________________________________________________________

const char WgMouseMoveEvent::CLASSNAME[] = {"MouseMoveEvent"};

WgMouseMoveEvent::WgMouseMoveEvent( const WgCoord& pos )
{
	m_type = WG_EVENT_MOUSE_MOVE;

	m_pointerLocalPos = pos;
	m_pointerScreenPos = pos;
}

WgMouseMoveEvent::WgMouseMoveEvent( WgWidget * pWidget )
{
	m_type = WG_EVENT_MOUSE_MOVE;
	m_bIsForWidget	= true;
	m_pWidget 		= pWidget;
}

bool WgMouseMoveEvent::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgEvent::IsInstanceOf(pClassName);
}

const char * WgMouseMoveEvent::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgMouseMoveEventPtr WgMouseMoveEvent::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgMouseMoveEventPtr( static_cast<WgMouseMoveEvent*>(pObject.GetRealPtr()) );

	return 0;
}


//____ WgMousePressEvent ______________________________________________________

const char WgMousePressEvent::CLASSNAME[] = {"MousePressEvent"};

WgMousePressEvent::WgMousePressEvent( int button ) : WgMouseButtonEvent(button)
{
	m_type = WG_EVENT_MOUSE_PRESS;
}

WgMousePressEvent::WgMousePressEvent( int button, WgWidget * pWidget ) : WgMouseButtonEvent(button)
{
	m_type			= WG_EVENT_MOUSE_PRESS;
	m_bIsForWidget	= true;
	m_pWidget 		= pWidget;
}

bool WgMousePressEvent::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgMouseButtonEvent::IsInstanceOf(pClassName);
}

const char * WgMousePressEvent::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgMousePressEventPtr WgMousePressEvent::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgMousePressEventPtr( static_cast<WgMousePressEvent*>(pObject.GetRealPtr()) );

	return 0;
}

//____ WgMouseRepeatEvent ______________________________________________________

const char WgMouseRepeatEvent::CLASSNAME[] = {"MouseRepeatEvent"};

WgMouseRepeatEvent::WgMouseRepeatEvent( int button ) : WgMouseButtonEvent(button)
{
	m_type = WG_EVENT_MOUSE_REPEAT;
}

WgMouseRepeatEvent::WgMouseRepeatEvent( int button, WgWidget * pWidget ) : WgMouseButtonEvent(button)
{
	m_type			= WG_EVENT_MOUSE_REPEAT;
	m_bIsForWidget	= true;
	m_pWidget 		= pWidget;
}

bool WgMouseRepeatEvent::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgMouseButtonEvent::IsInstanceOf(pClassName);
}

const char * WgMouseRepeatEvent::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgMouseRepeatEventPtr WgMouseRepeatEvent::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgMouseRepeatEventPtr( static_cast<WgMouseRepeatEvent*>(pObject.GetRealPtr()) );

	return 0;
}


//____ WgMouseReleaseEvent _____________________________________________________

const char WgMouseReleaseEvent::CLASSNAME[] = {"MouseReleaseEvent"};

WgMouseReleaseEvent::WgMouseReleaseEvent( int button ) : WgMouseButtonEvent(button)
{
	m_type = WG_EVENT_MOUSE_RELEASE;

	m_bReleaseInside = true;			// Always assumed to have had the press inside our window.
	m_bReleaseInside = true;			// Always assumed to be inside our window now.
}

WgMouseReleaseEvent::WgMouseReleaseEvent( int button, WgWidget * pWidget, bool bPressInside, bool bReleaseInside ) : WgMouseButtonEvent(button)
{
	m_type			= WG_EVENT_MOUSE_RELEASE;
	m_bIsForWidget	= true;
	m_pWidget 		= pWidget;

	m_bPressInside = bPressInside;
	m_bReleaseInside = bReleaseInside;
}

bool WgMouseReleaseEvent::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgMouseButtonEvent::IsInstanceOf(pClassName);
}

const char * WgMouseReleaseEvent::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgMouseReleaseEventPtr WgMouseReleaseEvent::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgMouseReleaseEventPtr( static_cast<WgMouseReleaseEvent*>(pObject.GetRealPtr()) );

	return 0;
}

bool WgMouseReleaseEvent::PressInside() const
{
	return m_bPressInside;
}

bool WgMouseReleaseEvent::ReleaseInside() const
{
	return m_bReleaseInside;
}



//____ WgMouseClickEvent _____________________________________________________

const char WgMouseClickEvent::CLASSNAME[] = {"MouseClickEvent"};

WgMouseClickEvent::WgMouseClickEvent( int button ) : WgMouseButtonEvent(button)
{
	m_type = WG_EVENT_MOUSE_CLICK;
}

WgMouseClickEvent::WgMouseClickEvent( int button, WgWidget * pWidget ) : WgMouseButtonEvent(button)
{
	m_type = WG_EVENT_MOUSE_CLICK;
	m_bIsForWidget	= true;
	m_pWidget 		= pWidget;
}

bool WgMouseClickEvent::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgMouseButtonEvent::IsInstanceOf(pClassName);
}

const char * WgMouseClickEvent::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgMouseClickEventPtr WgMouseClickEvent::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgMouseClickEventPtr( static_cast<WgMouseClickEvent*>(pObject.GetRealPtr()) );

	return 0;
}

//____ WgMouseDoubleClickEvent _____________________________________________________

const char WgMouseDoubleClickEvent::CLASSNAME[] = {"MouseDoubleClickEvent"};

WgMouseDoubleClickEvent::WgMouseDoubleClickEvent( int button ) : WgMouseButtonEvent(button)
{
	m_type = WG_EVENT_MOUSE_DOUBLE_CLICK;
}

WgMouseDoubleClickEvent::WgMouseDoubleClickEvent( int button, WgWidget * pWidget ) : WgMouseButtonEvent(button)
{
	m_type = WG_EVENT_MOUSE_DOUBLE_CLICK;
	m_bIsForWidget	= true;
	m_pWidget 		= pWidget;
}

bool WgMouseDoubleClickEvent::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgMouseButtonEvent::IsInstanceOf(pClassName);
}

const char * WgMouseDoubleClickEvent::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgMouseDoubleClickEventPtr WgMouseDoubleClickEvent::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgMouseDoubleClickEventPtr( static_cast<WgMouseDoubleClickEvent*>(pObject.GetRealPtr()) );

	return 0;
}

//____ WgKeyPressEvent ___________________________________________________________

const char WgKeyPressEvent::CLASSNAME[] = {"KeyPressEvent"};

WgKeyPressEvent::WgKeyPressEvent( int native_keycode ) : WgKeyEvent(native_keycode)
{
	m_type 			= WG_EVENT_KEY_PRESS;
}

WgKeyPressEvent::WgKeyPressEvent( int native_keycode, WgWidget * pWidget ) : WgKeyEvent(native_keycode)
{
	m_type 			= WG_EVENT_KEY_PRESS;
	m_bIsForWidget	= true;
	m_pWidget		= pWidget;
}

bool WgKeyPressEvent::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgKeyEvent::IsInstanceOf(pClassName);
}

const char * WgKeyPressEvent::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgKeyPressEventPtr WgKeyPressEvent::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgKeyPressEventPtr( static_cast<WgKeyPressEvent*>(pObject.GetRealPtr()) );

	return 0;
}

//____ WgKeyRepeatEvent ___________________________________________________________

const char WgKeyRepeatEvent::CLASSNAME[] = {"KeyRepeatEvent"};

WgKeyRepeatEvent::WgKeyRepeatEvent( int native_keycode ) : WgKeyEvent(native_keycode)
{
	m_type 			= WG_EVENT_KEY_REPEAT;
}

WgKeyRepeatEvent::WgKeyRepeatEvent( int native_keycode, WgWidget * pWidget ) : WgKeyEvent(native_keycode)
{
	m_type 			= WG_EVENT_KEY_REPEAT;
	m_bIsForWidget	= true;
	m_pWidget 		= pWidget;
}

bool WgKeyRepeatEvent::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgKeyEvent::IsInstanceOf(pClassName);
}

const char * WgKeyRepeatEvent::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgKeyRepeatEventPtr WgKeyRepeatEvent::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgKeyRepeatEventPtr( static_cast<WgKeyRepeatEvent*>(pObject.GetRealPtr()) );

	return 0;
}

//____ WgKeyReleaseEvent ________________________________________________________

const char WgKeyReleaseEvent::CLASSNAME[] = {"KeyReleaseEvent"};

WgKeyReleaseEvent::WgKeyReleaseEvent( int native_keycode ) : WgKeyEvent(native_keycode)
{
	m_type 		= WG_EVENT_KEY_RELEASE;
}

WgKeyReleaseEvent::WgKeyReleaseEvent( int native_keycode, WgWidget * pWidget ) : WgKeyEvent(native_keycode)
{
	m_type 			= WG_EVENT_KEY_RELEASE;
	m_bIsForWidget	= true;
	m_pWidget		= pWidget;
}

bool WgKeyReleaseEvent::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgKeyEvent::IsInstanceOf(pClassName);
}

const char * WgKeyReleaseEvent::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgKeyReleaseEventPtr WgKeyReleaseEvent::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgKeyReleaseEventPtr( static_cast<WgKeyReleaseEvent*>(pObject.GetRealPtr()) );

	return 0;
}

//____ WgCharacterEvent __________________________________________________________

const char WgCharacterEvent::CLASSNAME[] = {"CharacterEvent"};

WgCharacterEvent::WgCharacterEvent( unsigned short character )
{
	m_type = WG_EVENT_CHARACTER;
	m_char = character;
}

WgCharacterEvent::WgCharacterEvent( unsigned short character, WgWidget * pWidget )
{
	m_type			= WG_EVENT_CHARACTER;
	m_char			= character;
	m_bIsForWidget	= true;
	m_pWidget		= pWidget;
}

unsigned short WgCharacterEvent::Char() const
{
	return m_char;
}

bool WgCharacterEvent::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgEvent::IsInstanceOf(pClassName);
}

const char * WgCharacterEvent::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgCharacterEventPtr WgCharacterEvent::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgCharacterEventPtr( static_cast<WgCharacterEvent*>(pObject.GetRealPtr()) );

	return 0;
}


//____ WgWheelRollEvent __________________________________________________________

const char WgWheelRollEvent::CLASSNAME[] = {"WheelRollEvent"};

WgWheelRollEvent::WgWheelRollEvent( int wheel, int distance )
{
	m_type = WG_EVENT_WHEEL_ROLL;
	m_wheel = wheel;
	m_distance = distance;
}

WgWheelRollEvent::WgWheelRollEvent( int wheel, int distance, WgWidget * pWidget )
{
	m_type			= WG_EVENT_WHEEL_ROLL;
	m_wheel			= wheel;
	m_distance		= distance;
	m_bIsForWidget	= true;
	m_pWidget		= pWidget;
}

int WgWheelRollEvent::Wheel() const
{
	return m_wheel;
}

int WgWheelRollEvent::Distance() const
{
	return m_distance;
}

bool WgWheelRollEvent::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgEvent::IsInstanceOf(pClassName);
}

const char * WgWheelRollEvent::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgWheelRollEventPtr WgWheelRollEvent::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgWheelRollEventPtr( static_cast<WgWheelRollEvent*>(pObject.GetRealPtr()) );

	return 0;
}

//____ WgTickEvent _______________________________________________________________

const char WgTickEvent::CLASSNAME[] = {"TickEvent"};

WgTickEvent::WgTickEvent( int ms )
{
	m_type = WG_EVENT_TICK;
	m_millisec = ms;
}

WgTickEvent::WgTickEvent( int ms, WgWidget * pWidget )
{
	m_type 			= WG_EVENT_TICK;
	m_millisec 		= ms;
	m_bIsForWidget	= true;
	m_pWidget 		= pWidget;
}
	
int WgTickEvent::Millisec() const
{
	return m_millisec;
}

bool WgTickEvent::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgEvent::IsInstanceOf(pClassName);
}

const char * WgTickEvent::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgTickEventPtr WgTickEvent::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgTickEventPtr( static_cast<WgTickEvent*>(pObject.GetRealPtr()) );

	return 0;
}

//____ WgPointerChangeEvent _______________________________________________________________

const char WgPointerChangeEvent::CLASSNAME[] = {"PointerChangeEvent"};

WgPointerChangeEvent::WgPointerChangeEvent( WgPointerStyle style )
{
	m_type = WG_EVENT_POINTER_CHANGE;
	m_style = style;
}
	
WgPointerStyle WgPointerChangeEvent::Style() const
{
	return m_style;
}

bool WgPointerChangeEvent::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgEvent::IsInstanceOf(pClassName);
}

const char * WgPointerChangeEvent::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgPointerChangeEventPtr WgPointerChangeEvent::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgPointerChangeEventPtr( static_cast<WgPointerChangeEvent*>(pObject.GetRealPtr()) );

	return 0;
}

//____ WgSelectEvent _________________________________________________________

const char WgSelectEvent::CLASSNAME[] = {"SelectEvent"};

WgSelectEvent::WgSelectEvent( WgWidget * pWidget )
{
	m_type 			= WG_EVENT_SELECT;
	m_pWidget 		= pWidget;
}

bool WgSelectEvent::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgEvent::IsInstanceOf(pClassName);
}

const char * WgSelectEvent::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgSelectEventPtr WgSelectEvent::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgSelectEventPtr( static_cast<WgSelectEvent*>(pObject.GetRealPtr()) );

	return 0;
}

//____ WgToggleEvent ______________________________________________

const char WgToggleEvent::CLASSNAME[] = {"ToggleEvent"};

WgToggleEvent::WgToggleEvent( WgWidget * pWidget, bool bSet )
{
	m_type = WG_EVENT_TOGGLE;
	m_pWidget = pWidget;
	m_bSet = bSet;
}

bool WgToggleEvent::IsSet() const
{
	return m_bSet;
}

bool WgToggleEvent::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgEvent::IsInstanceOf(pClassName);
}

const char * WgToggleEvent::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgToggleEventPtr WgToggleEvent::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgToggleEventPtr( static_cast<WgToggleEvent*>(pObject.GetRealPtr()) );

	return 0;
}

//____ WgValueChangeEvent ___________________________________________

const char WgValueUpdateEvent::CLASSNAME[] = {"ValueUpdateEvent"};

WgValueUpdateEvent::WgValueUpdateEvent( WgWidget * pWidget, int64_t value, double fraction, bool bFinal )
{
	m_type = WG_EVENT_VALUE_UPDATE;
	m_pWidget = pWidget;
	m_value = value;
	m_fraction = fraction;
	m_bFinal = bFinal;
}

int64_t WgValueUpdateEvent::Value() const
{
	return m_value;
}

double WgValueUpdateEvent::Fraction() const
{
	return m_fraction;
}

bool WgValueUpdateEvent::IsFinal() const
{
	return m_bFinal;
}

bool WgValueUpdateEvent::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgEvent::IsInstanceOf(pClassName);
}

const char * WgValueUpdateEvent::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgValueUpdateEventPtr WgValueUpdateEvent::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgValueUpdateEventPtr( static_cast<WgValueUpdateEvent*>(pObject.GetRealPtr()) );

	return 0;
}

//____ WgRangeUpdateEvent ___________________________________________

const char WgRangeUpdateEvent::CLASSNAME[] = {"RangeUpdateEvent"};

WgRangeUpdateEvent::WgRangeUpdateEvent( WgWidget * pWidget, int intOfs, int intLength, double fracOfs, double fracLength, bool bFinal )
{
	m_type = WG_EVENT_RANGE_UPDATE;
	m_pWidget = pWidget;
	m_ofs		= intOfs;
	m_length	= intLength;
	m_fracOfs	= fracOfs;
	m_fracLength= fracLength;
	m_bFinal	= bFinal;
}

bool WgRangeUpdateEvent::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgEvent::IsInstanceOf(pClassName);
}

const char * WgRangeUpdateEvent::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgRangeUpdateEventPtr WgRangeUpdateEvent::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgRangeUpdateEventPtr( static_cast<WgRangeUpdateEvent*>(pObject.GetRealPtr()) );

	return 0;
}

int WgRangeUpdateEvent::Offset() const
{
	return m_ofs;
}

int WgRangeUpdateEvent::Length() const
{
	return m_length;
}

double WgRangeUpdateEvent::FracOffset() const
{
	return m_fracOfs;
}

double WgRangeUpdateEvent::FracLength() const
{
	return m_fracLength;
}

bool WgRangeUpdateEvent::IsFinal() const
{
	return m_bFinal;
}


//____ WgTextEditEvent ________________________________________________________

const char WgTextEditEvent::CLASSNAME[] = {"TextEditEvent"};

WgIEditTextPtr WgTextEditEvent::Text() const
{
	return m_pText;
}

WgTextEditEvent::WgTextEditEvent( WgWidget * pWidget, WgText * pText, bool bFinal )
{
	m_type 		= WG_EVENT_TEXT_EDIT;
	m_pWidget 	= pWidget;
	m_pText 	= WgIEditTextPtr(pWidget, pText);
	m_bFinal	= bFinal;
}

bool WgTextEditEvent::IsFinal() const
{
	return m_bFinal;
}

bool WgTextEditEvent::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgEvent::IsInstanceOf(pClassName);
}

const char * WgTextEditEvent::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgTextEditEventPtr WgTextEditEvent::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgTextEditEventPtr( static_cast<WgTextEditEvent*>(pObject.GetRealPtr()) );

	return 0;
}

//____ WgItemEvent ________________________________________________________

const char WgItemEvent::CLASSNAME[] = {"ItemEvent"};

WgItemEvent::WgItemEvent( WgWidget * pWidget, int itemIndex, int itemId, const WgObjectPtr& pItemObject ) : m_item(itemIndex,itemId,pItemObject)
{
	m_pWidget 	= pWidget;
}

bool WgItemEvent::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgEvent::IsInstanceOf(pClassName);
}

const char * WgItemEvent::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgItemEventPtr WgItemEvent::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgItemEventPtr( static_cast<WgItemEvent*>(pObject.GetRealPtr()) );

	return 0;
}

int WgItemEvent::ItemIndex() const 
{ 
	return m_item.index; 
}

int WgItemEvent::ItemId() const 
{ 
	return m_item.id; 
}

WgObjectPtr WgItemEvent::ItemObject() const 
{ 
	return m_item.pObject; 
}

//____ WgItemToggleEvent ________________________________________________________

const char WgItemToggleEvent::CLASSNAME[] = {"ItemToggleEvent"};

WgItemToggleEvent::WgItemToggleEvent( WgWidget * pWidget, int itemIndex, int itemId, const WgObjectPtr& pItemObject, bool bSet ) : WgItemEvent(pWidget,itemIndex,itemId,pItemObject)
{
	m_type = WG_EVENT_ITEM_TOGGLE;
	m_bSet = bSet;
}

bool WgItemToggleEvent::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgItemEvent::IsInstanceOf(pClassName);
}

const char * WgItemToggleEvent::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgItemToggleEventPtr WgItemToggleEvent::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgItemToggleEventPtr( static_cast<WgItemToggleEvent*>(pObject.GetRealPtr()) );

	return 0;
}

bool WgItemToggleEvent::IsSet() const
{
	return m_bSet;
}

//____ WgItemMousePressEvent ________________________________________________________

const char WgItemMousePressEvent::CLASSNAME[] = {"ItemMousePressEvent"};

WgItemMousePressEvent::WgItemMousePressEvent( WgWidget * pWidget, int itemIndex, int itemId, const WgObjectPtr& pItemObject, int button ) : WgItemEvent(pWidget,itemIndex,itemId,pItemObject)
{
	m_type = WG_EVENT_ITEM_MOUSE_PRESS;
	m_button = button;
}

bool WgItemMousePressEvent::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgItemEvent::IsInstanceOf(pClassName);
}

const char * WgItemMousePressEvent::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgItemMousePressEventPtr WgItemMousePressEvent::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgItemMousePressEventPtr( static_cast<WgItemMousePressEvent*>(pObject.GetRealPtr()) );

	return 0;
}

int WgItemMousePressEvent::Button() const
{
	return m_button;
}

//____ WgItemListEvent ________________________________________________________

const char WgItemListEvent::CLASSNAME[] = {"ItemListEvent"};

WgItemListEvent::WgItemListEvent( WgWidget * pWidget, int nbItems, WgItemInfo * pItems )
{
	m_pWidget 	= pWidget;
	m_nbItems	= nbItems;
	m_pItems	= pItems;
}

WgItemListEvent::~WgItemListEvent()
{
	delete [] m_pItems;
}

bool WgItemListEvent::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgEvent::IsInstanceOf(pClassName);
}

const char * WgItemListEvent::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgItemListEventPtr WgItemListEvent::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgItemListEventPtr( static_cast<WgItemListEvent*>(pObject.GetRealPtr()) );

	return 0;
}

int WgItemListEvent::NbItems() const
{
	return m_nbItems;
}

const WgItemInfo * WgItemListEvent::Items() const
{
	return m_pItems;
}

//____ WgItemsSelectEvent ________________________________________________________

const char WgItemsSelectEvent::CLASSNAME[] = {"ItemsSelectEvent"};

WgItemsSelectEvent::WgItemsSelectEvent( WgWidget * pWidget, int nbItems, WgItemInfo * pItems ) : WgItemListEvent(pWidget,nbItems,pItems)
{
	m_type = WG_EVENT_ITEMS_SELECT;
}

bool WgItemsSelectEvent::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgItemListEvent::IsInstanceOf(pClassName);
}

const char * WgItemsSelectEvent::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgItemsSelectEventPtr WgItemsSelectEvent::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgItemsSelectEventPtr( static_cast<WgItemsSelectEvent*>(pObject.GetRealPtr()) );

	return 0;
}

//____ WgItemsUnselectEvent ________________________________________________________

const char WgItemsUnselectEvent::CLASSNAME[] = {"ItemsUnselectEvent"};

WgItemsUnselectEvent::WgItemsUnselectEvent( WgWidget * pWidget, int nbItems, WgItemInfo * pItems ) : WgItemListEvent(pWidget,nbItems,pItems)
{
	m_type = WG_EVENT_ITEMS_UNSELECT;
}

bool WgItemsUnselectEvent::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgItemListEvent::IsInstanceOf(pClassName);
}

const char * WgItemsUnselectEvent::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgItemsUnselectEventPtr WgItemsUnselectEvent::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgItemsUnselectEventPtr( static_cast<WgItemsUnselectEvent*>(pObject.GetRealPtr()) );

	return 0;
}

//____ WgPopupClosedEvent ____________________________________________

const char WgPopupClosedEvent::CLASSNAME[] = {"PopupClosedEvent"};

WgPopupClosedEvent::WgPopupClosedEvent( WgWidget * pPopup, const WgWidgetWeakPtr& pCaller )
{
	m_type		= WG_EVENT_POPUP_CLOSED;
	m_pWidget	= pCaller;
	m_bIsForWidget	= true;
	m_pPopup		= pPopup;
}

bool WgPopupClosedEvent::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgEvent::IsInstanceOf(pClassName);
}

const char * WgPopupClosedEvent::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgPopupClosedEventPtr WgPopupClosedEvent::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgPopupClosedEventPtr( static_cast<WgPopupClosedEvent*>(pObject.GetRealPtr()) );

	return 0;
}

WgWidget * WgPopupClosedEvent::Popup() const
{
	return m_pPopup.GetRealPtr();
}

WgWidget * WgPopupClosedEvent::Caller() const
{
	return m_pWidget.GetRealPtr();
}

//____ WgModalMoveOutsideEvent ___________________________________________________

const char WgModalMoveOutsideEvent::CLASSNAME[] = {"ModalMoveOutsideEvent"};

WgModalMoveOutsideEvent::WgModalMoveOutsideEvent( WgWidget * pWidget )
{
	m_type = WG_EVENT_MODAL_MOVE_OUTSIDE;
	m_bIsForWidget	= true;
	m_pWidget		= pWidget;
}

bool WgModalMoveOutsideEvent::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgEvent::IsInstanceOf(pClassName);
}

const char * WgModalMoveOutsideEvent::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgModalMoveOutsideEventPtr WgModalMoveOutsideEvent::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgModalMoveOutsideEventPtr( static_cast<WgModalMoveOutsideEvent*>(pObject.GetRealPtr()) );

	return 0;
}


//____ WgModalBlockedPressEvent _______________________________________________

const char WgModalBlockedPressEvent::CLASSNAME[] = {"ModalBlockedPressEvent"};

WgModalBlockedPressEvent::WgModalBlockedPressEvent( int button, WgWidget * pWidget ) : WgMouseButtonEvent(button)
{
	m_type			= WG_EVENT_MODAL_BLOCKED_PRESS;
	m_bIsForWidget	= true;
	m_pWidget 		= pWidget;
}

bool WgModalBlockedPressEvent::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgEvent::IsInstanceOf(pClassName);
}

const char * WgModalBlockedPressEvent::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgModalBlockedPressEventPtr WgModalBlockedPressEvent::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgModalBlockedPressEventPtr( static_cast<WgModalBlockedPressEvent*>(pObject.GetRealPtr()) );

	return 0;
}

//____ WgModalBlockedReleaseEvent _____________________________________________

const char WgModalBlockedReleaseEvent::CLASSNAME[] = {"ModalBlockedReleaseEvent"};

WgModalBlockedReleaseEvent::WgModalBlockedReleaseEvent( int button, WgWidget * pWidget ) : WgMouseButtonEvent(button)
{
	m_type			= WG_EVENT_MODAL_BLOCKED_RELEASE;
	m_bIsForWidget	= true;
	m_pWidget 		= pWidget;
}

bool WgModalBlockedReleaseEvent::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgEvent::IsInstanceOf(pClassName);
}

const char * WgModalBlockedReleaseEvent::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgModalBlockedReleaseEventPtr WgModalBlockedReleaseEvent::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgModalBlockedReleaseEventPtr( static_cast<WgModalBlockedReleaseEvent*>(pObject.GetRealPtr()) );

	return 0;
}

//____ WgLinkEvent __________________________________________________

const char WgLinkEvent::CLASSNAME[] = {"LinkEvent"};

std::string WgLinkEvent::Link() const
{
	return m_link;
}

bool WgLinkEvent::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgEvent::IsInstanceOf(pClassName);
}

const char * WgLinkEvent::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgLinkEventPtr WgLinkEvent::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgLinkEventPtr( static_cast<WgLinkEvent*>(pObject.GetRealPtr()) );

	return 0;
}

//____ WgLinkSelectEvent __________________________________________________

const char WgLinkSelectEvent::CLASSNAME[] = {"LinkSelectEvent"};

WgLinkSelectEvent::WgLinkSelectEvent( WgWidget * pWidget, std::string link )
{
	m_type			= WG_EVENT_LINK_SELECT;
	m_pWidget		= pWidget;
	m_link			= link;
}

bool WgLinkSelectEvent::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgLinkEvent::IsInstanceOf(pClassName);
}

const char * WgLinkSelectEvent::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgLinkSelectEventPtr WgLinkSelectEvent::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgLinkSelectEventPtr( static_cast<WgLinkSelectEvent*>(pObject.GetRealPtr()) );

	return 0;
}

//____ WgLinkMouseEnterEvent __________________________________________________

const char WgLinkMouseEnterEvent::CLASSNAME[] = {"LinkMouseEnterEvent"};

WgLinkMouseEnterEvent::WgLinkMouseEnterEvent( WgWidget * pWidget, std::string link )
{
	m_type			= WG_EVENT_LINK_MOUSE_ENTER;
	m_pWidget		= pWidget;
	m_link			= link;
}

bool WgLinkMouseEnterEvent::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgLinkEvent::IsInstanceOf(pClassName);
}

const char * WgLinkMouseEnterEvent::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgLinkMouseEnterEventPtr WgLinkMouseEnterEvent::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgLinkMouseEnterEventPtr( static_cast<WgLinkMouseEnterEvent*>(pObject.GetRealPtr()) );

	return 0;
}

//____ WgLinkMouseLeaveEvent __________________________________________________

const char WgLinkMouseLeaveEvent::CLASSNAME[] = {"LinkMouseLeaveEvent"};

WgLinkMouseLeaveEvent::WgLinkMouseLeaveEvent( WgWidget * pWidget, std::string link )
{
	m_type			= WG_EVENT_LINK_MOUSE_LEAVE;
	m_pWidget		= pWidget;
	m_link			= link;
}

bool WgLinkMouseLeaveEvent::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgLinkEvent::IsInstanceOf(pClassName);
}

const char * WgLinkMouseLeaveEvent::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgLinkMouseLeaveEventPtr WgLinkMouseLeaveEvent::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgLinkMouseLeaveEventPtr( static_cast<WgLinkMouseLeaveEvent*>(pObject.GetRealPtr()) );

	return 0;
}

//____ WgLinkMouseButtonEvent __________________________________________________

const char WgLinkMouseButtonEvent::CLASSNAME[] = {"LinkMouseButtonEvent"};

int WgLinkMouseButtonEvent::Button() const
{
	return m_button;
}

bool WgLinkMouseButtonEvent::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgLinkEvent::IsInstanceOf(pClassName);
}

const char * WgLinkMouseButtonEvent::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgLinkMouseButtonEventPtr WgLinkMouseButtonEvent::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgLinkMouseButtonEventPtr( static_cast<WgLinkMouseButtonEvent*>(pObject.GetRealPtr()) );

	return 0;
}

//____ WgLinkMousePressEvent __________________________________________________

const char WgLinkMousePressEvent::CLASSNAME[] = {"LinkMousePressEvent"};

WgLinkMousePressEvent::WgLinkMousePressEvent( WgWidget * pWidget, std::string link, int button )
{
	m_type			= WG_EVENT_LINK_MOUSE_PRESS;
	m_pWidget		= pWidget;
	m_link			= link;
	m_button		= button;
}

bool WgLinkMousePressEvent::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgLinkMouseButtonEvent::IsInstanceOf(pClassName);
}

const char * WgLinkMousePressEvent::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgLinkMousePressEventPtr WgLinkMousePressEvent::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgLinkMousePressEventPtr( static_cast<WgLinkMousePressEvent*>(pObject.GetRealPtr()) );

	return 0;
}

//____ WgLinkMouseRepeatEvent __________________________________________________

const char WgLinkMouseRepeatEvent::CLASSNAME[] = {"LinkMouseRepeatEvent"};

WgLinkMouseRepeatEvent::WgLinkMouseRepeatEvent( WgWidget * pWidget, std::string link, int button )
{
	m_type			= WG_EVENT_LINK_MOUSE_REPEAT;
	m_pWidget		= pWidget;
	m_link			= link;
	m_button		= button;
}

bool WgLinkMouseRepeatEvent::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgLinkMouseButtonEvent::IsInstanceOf(pClassName);
}

const char * WgLinkMouseRepeatEvent::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgLinkMouseRepeatEventPtr WgLinkMouseRepeatEvent::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgLinkMouseRepeatEventPtr( static_cast<WgLinkMouseRepeatEvent*>(pObject.GetRealPtr()) );

	return 0;
}

//____ WgLinkMouseReleaseEvent __________________________________________________

const char WgLinkMouseReleaseEvent::CLASSNAME[] = {"LinkMouseReleaseEvent"};

WgLinkMouseReleaseEvent::WgLinkMouseReleaseEvent( WgWidget * pWidget, std::string link, int button )
{
	m_type			= WG_EVENT_LINK_MOUSE_RELEASE;
	m_pWidget		= pWidget;
	m_link			= link;
	m_button		= button;
}

bool WgLinkMouseReleaseEvent::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgLinkMouseButtonEvent::IsInstanceOf(pClassName);
}

const char * WgLinkMouseReleaseEvent::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgLinkMouseReleaseEventPtr WgLinkMouseReleaseEvent::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgLinkMouseReleaseEventPtr( static_cast<WgLinkMouseReleaseEvent*>(pObject.GetRealPtr()) );

	return 0;
}

//____ WgLinkMouseClickEvent __________________________________________________

const char WgLinkMouseClickEvent::CLASSNAME[] = {"LinkMouseClickEvent"};

WgLinkMouseClickEvent::WgLinkMouseClickEvent( WgWidget * pWidget, std::string link, int button )
{
	m_type			= WG_EVENT_LINK_MOUSE_CLICK;
	m_pWidget		= pWidget;
	m_link			= link;
	m_button		= button;
}

bool WgLinkMouseClickEvent::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgLinkMouseButtonEvent::IsInstanceOf(pClassName);
}

const char * WgLinkMouseClickEvent::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgLinkMouseClickEventPtr WgLinkMouseClickEvent::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgLinkMouseClickEventPtr( static_cast<WgLinkMouseClickEvent*>(pObject.GetRealPtr()) );

	return 0;
}

//____ WgLinkMouseDoubleClickEvent __________________________________________________

const char WgLinkMouseDoubleClickEvent::CLASSNAME[] = {"LinkMouseDoubleClickEvent"};

WgLinkMouseDoubleClickEvent::WgLinkMouseDoubleClickEvent( WgWidget * pWidget, std::string link, int button )
{
	m_type			= WG_EVENT_LINK_MOUSE_DOUBLE_CLICK;
	m_pWidget		= pWidget;
	m_link			= link;
	m_button		= button;
}

bool WgLinkMouseDoubleClickEvent::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgLinkMouseButtonEvent::IsInstanceOf(pClassName);
}

const char * WgLinkMouseDoubleClickEvent::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgLinkMouseDoubleClickEventPtr WgLinkMouseDoubleClickEvent::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgLinkMouseDoubleClickEventPtr( static_cast<WgLinkMouseDoubleClickEvent*>(pObject.GetRealPtr()) );

	return 0;
}


//____ WgMousePositionEvent _______________________________________________________

const char WgMousePositionEvent::CLASSNAME[] = {"MousePositionEvent"};

WgMousePositionEvent::WgMousePositionEvent()
{
	m_type = WG_EVENT_MOUSE_POSITION;
}

bool WgMousePositionEvent::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgEvent::IsInstanceOf(pClassName);
}

const char * WgMousePositionEvent::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgMousePositionEventPtr WgMousePositionEvent::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgMousePositionEventPtr( static_cast<WgMousePositionEvent*>(pObject.GetRealPtr()) );

	return 0;
}

//____ WgMouseDragEvent _________________________________________________________

const char WgMouseDragEvent::CLASSNAME[] = {"MouseDragEvent"};

WgMouseDragEvent::WgMouseDragEvent( int button, const WgCoord& startPos, const WgCoord& prevPos, const WgCoord& currPos ) : WgMouseButtonEvent(button)
{
	m_type = WG_EVENT_MOUSE_DRAG;

	m_startPos = startPos;
	m_prevPos = prevPos;
	m_currPos = currPos;
}

WgMouseDragEvent::WgMouseDragEvent( int button, WgWidget * pWidget, const WgCoord& startPos, const WgCoord& prevPos, const WgCoord& currPos ) : WgMouseButtonEvent(button)
{
	m_type = WG_EVENT_MOUSE_DRAG;
	m_bIsForWidget		= true;
	m_pWidget 			= pWidget;

	m_startPos = startPos;
	m_prevPos = prevPos;
	m_currPos = currPos;
}

bool WgMouseDragEvent::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgMouseButtonEvent::IsInstanceOf(pClassName);
}

const char * WgMouseDragEvent::ClassName( void ) const
{ 
	return CLASSNAME; 
}

WgMouseDragEventPtr WgMouseDragEvent::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgMouseDragEventPtr( static_cast<WgMouseDragEvent*>(pObject.GetRealPtr()) );

	return 0;
}

WgCoord WgMouseDragEvent::DraggedTotal() const
{
	return m_currPos - m_startPos;
}

WgCoord WgMouseDragEvent::DraggedNow() const
{
	return m_currPos - m_prevPos;
}

WgCoord WgMouseDragEvent::CurrPos() const
{
	return m_currPos;
}

WgCoord WgMouseDragEvent::StartPos() const
{
	return m_startPos;
}

WgCoord WgMouseDragEvent::PrevPos() const
{
	return m_prevPos;
}

