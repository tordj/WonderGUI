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

#ifndef WG_EVENT_DOT_H
#define WG_EVENT_DOT_H

#include <string>

#ifndef WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

#ifndef WG_USERDEFINES_DOT_H
#	include <wg_userdefines.h>
#endif

#ifndef WG_GEO_DOT_H
#	include <wg_geo.h>
#endif

#ifndef WG_POINTERS_DOT_H
#	include <wg_pointers.h>
#endif


class WgEventHandler;
class WgWidget;
class WgButton;
class WgCheckBox;
class WgRadioButton;
class WgAnimPlayer;
class WgTablist;
class WgValueDisplay;
class WgValueEditor;
class WgScrollbar;
class WgMenu;
class WgPopupLayer;
class WgModalLayer;

class WgTextField;
class WgEditTextField;
class WgModifTextField;


typedef	WgCompStrongPtr<WgTextField,WgComponentPtr>		WgTextFieldPtr;
typedef	WgCompWeakPtr<WgTextField,WgComponentWeakPtr>	WgTextFieldWeakPtr;

typedef	WgCompStrongPtr<WgModifTextField,WgTextFieldPtr>	WgModifTextFieldPtr;
typedef	WgCompWeakPtr<WgModifTextField,WgTextFieldWeakPtr>	WgModifTextFieldWeakPtr;

typedef	WgCompStrongPtr<WgEditTextField,WgModifTextFieldPtr>	WgEditTextFieldPtr;
typedef	WgCompWeakPtr<WgEditTextField,WgModifTextFieldWeakPtr>	WgEditTextFieldWeakPtr;

typedef WgWeakPtr<WgWidget,WgObjectWeakPtr> WgWidgetWeakPtr;





class WgEvent;
typedef	WgStrongPtr<WgEvent,WgObjectPtr>		WgEventPtr;
typedef	WgWeakPtr<WgEvent,WgObjectWeakPtr>	WgEventWeakPtr;

class WgFocusGainedEvent;
typedef	WgStrongPtr<WgFocusGainedEvent,WgEventPtr>		WgFocusGainedEventPtr;
typedef	WgWeakPtr<WgFocusGainedEvent,WgEventWeakPtr>	WgFocusGainedEventWeakPtr;

class WgFocusLostEvent;
typedef	WgStrongPtr<WgFocusLostEvent,WgEventPtr>		WgFocusLostEventPtr;
typedef	WgWeakPtr<WgFocusLostEvent,WgEventWeakPtr>	WgFocusLostEventWeakPtr;

class WgMouseEnterEvent;
typedef	WgStrongPtr<WgMouseEnterEvent,WgEventPtr>		WgMouseEnterEventPtr;
typedef	WgWeakPtr<WgMouseEnterEvent,WgEventWeakPtr>	WgMouseEnterEventWeakPtr;

class WgMouseLeaveEvent;
typedef	WgStrongPtr<WgMouseLeaveEvent,WgEventPtr>		WgMouseLeaveEventPtr;
typedef	WgWeakPtr<WgMouseLeaveEvent,WgEventWeakPtr>	WgMouseLeaveEventWeakPtr;

class WgMouseMoveEvent;
typedef	WgStrongPtr<WgMouseMoveEvent,WgEventPtr>		WgMouseMoveEventPtr;
typedef	WgWeakPtr<WgMouseMoveEvent,WgEventWeakPtr>	WgMouseMoveEventWeakPtr;

class WgMousePositionEvent;
typedef	WgStrongPtr<WgMousePositionEvent,WgEventPtr>		WgMousePositionEventPtr;
typedef	WgWeakPtr<WgMousePositionEvent,WgEventWeakPtr>	WgMousePositionEventWeakPtr;

class WgMouseButtonEvent;
typedef	WgStrongPtr<WgMouseButtonEvent,WgEventPtr>		WgMouseButtonEventPtr;
typedef	WgWeakPtr<WgMouseButtonEvent,WgEventWeakPtr>	WgMouseButtonEventWeakPtr;

class WgMousePressEvent;
typedef	WgStrongPtr<WgMousePressEvent,WgMouseButtonEventPtr>		WgMousePressEventPtr;
typedef	WgWeakPtr<WgMousePressEvent,WgMouseButtonEventWeakPtr>	WgMousePressEventWeakPtr;

class WgMouseReleaseEvent;
typedef	WgStrongPtr<WgMouseReleaseEvent,WgMouseButtonEventPtr>		WgMouseReleaseEventPtr;
typedef	WgWeakPtr<WgMouseReleaseEvent,WgMouseButtonEventWeakPtr>	WgMouseReleaseEventWeakPtr;

class WgMouseRepeatEvent;
typedef	WgStrongPtr<WgMouseRepeatEvent,WgMouseButtonEventPtr>		WgMouseRepeatEventPtr;
typedef	WgWeakPtr<WgMouseRepeatEvent,WgMouseButtonEventWeakPtr>	WgMouseRepeatEventWeakPtr;

class WgMouseClickEvent;
typedef	WgStrongPtr<WgMouseClickEvent,WgMouseButtonEventPtr>		WgMouseClickEventPtr;
typedef	WgWeakPtr<WgMouseClickEvent,WgMouseButtonEventWeakPtr>	WgMouseClickEventWeakPtr;

class WgMouseDoubleClickEvent;
typedef	WgStrongPtr<WgMouseDoubleClickEvent,WgMouseButtonEventPtr>		WgMouseDoubleClickEventPtr;
typedef	WgWeakPtr<WgMouseDoubleClickEvent,WgMouseButtonEventWeakPtr>	WgMouseDoubleClickEventWeakPtr;

class WgMouseDragEvent;
typedef	WgStrongPtr<WgMouseDragEvent,WgMouseButtonEventPtr>		WgMouseDragEventPtr;
typedef	WgWeakPtr<WgMouseDragEvent,WgMouseButtonEventWeakPtr>	WgMouseDragEventWeakPtr;

class WgKeyEvent;
typedef	WgStrongPtr<WgKeyEvent,WgEventPtr>		WgKeyEventPtr;
typedef	WgWeakPtr<WgKeyEvent,WgEventWeakPtr>	WgKeyEventWeakPtr;

class WgKeyPressEvent;
typedef	WgStrongPtr<WgKeyPressEvent,WgKeyEventPtr>		WgKeyPressEventPtr;
typedef	WgWeakPtr<WgKeyPressEvent,WgKeyEventWeakPtr>	WgKeyPressEventWeakPtr;

class WgKeyReleaseEvent;
typedef	WgStrongPtr<WgKeyReleaseEvent,WgKeyEventPtr>		WgKeyReleaseEventPtr;
typedef	WgWeakPtr<WgKeyReleaseEvent,WgKeyEventWeakPtr>	WgKeyReleaseEventWeakPtr;

class WgKeyRepeatEvent;
typedef	WgStrongPtr<WgKeyRepeatEvent,WgKeyEventPtr>		WgKeyRepeatEventPtr;
typedef	WgWeakPtr<WgKeyRepeatEvent,WgKeyEventWeakPtr>	WgKeyRepeatEventWeakPtr;

class WgCharacterEvent;
typedef	WgStrongPtr<WgCharacterEvent,WgEventPtr>		WgCharacterEventPtr;
typedef	WgWeakPtr<WgCharacterEvent,WgEventWeakPtr>	WgCharacterEventWeakPtr;

class WgWheelRollEvent;
typedef	WgStrongPtr<WgWheelRollEvent,WgEventPtr>		WgWheelRollEventPtr;
typedef	WgWeakPtr<WgWheelRollEvent,WgEventWeakPtr>	WgWheelRollEventWeakPtr;

class WgTickEvent;
typedef	WgStrongPtr<WgTickEvent,WgEventPtr>		WgTickEventPtr;
typedef	WgWeakPtr<WgTickEvent,WgEventWeakPtr>	WgTickEventWeakPtr;

class WgPointerChangeEvent;
typedef	WgStrongPtr<WgPointerChangeEvent,WgEventPtr>		WgPointerChangeEventPtr;
typedef	WgWeakPtr<WgPointerChangeEvent,WgEventWeakPtr>	WgPointerChangeEventWeakPtr;

class WgSelectEvent;
typedef	WgStrongPtr<WgSelectEvent,WgEventPtr>		WgSelectEventPtr;
typedef	WgWeakPtr<WgSelectEvent,WgEventWeakPtr>	WgSelectEventWeakPtr;

class WgToggleEvent;
typedef	WgStrongPtr<WgToggleEvent,WgEventPtr>		WgToggleEventPtr;
typedef	WgWeakPtr<WgToggleEvent,WgEventWeakPtr>	WgToggleEventWeakPtr;

class WgValueUpdateEvent;
typedef	WgStrongPtr<WgValueUpdateEvent,WgEventPtr>		WgValueUpdateEventPtr;
typedef	WgWeakPtr<WgValueUpdateEvent,WgEventWeakPtr>	WgValueUpdateEventWeakPtr;

class WgRangeUpdateEvent;
typedef	WgStrongPtr<WgRangeUpdateEvent,WgEventPtr>		WgRangeUpdateEventPtr;
typedef	WgWeakPtr<WgRangeUpdateEvent,WgEventWeakPtr>	WgRangeUpdateEventWeakPtr;

class WgTextEditEvent;
typedef	WgStrongPtr<WgTextEditEvent,WgEventPtr>		WgTextEditEventPtr;
typedef	WgWeakPtr<WgTextEditEvent,WgEventWeakPtr>	WgTextEditEventWeakPtr;

class WgItemEvent;
typedef	WgStrongPtr<WgItemEvent,WgEventPtr>		WgItemEventPtr;
typedef	WgWeakPtr<WgItemEvent,WgEventWeakPtr>	WgItemEventWeakPtr;

class WgItemListEvent;
typedef	WgStrongPtr<WgItemListEvent,WgEventPtr>		WgItemListEventPtr;
typedef	WgWeakPtr<WgItemListEvent,WgEventWeakPtr>	WgItemListEventWeakPtr;

class WgItemsSelectEvent;
typedef	WgStrongPtr<WgItemsSelectEvent,WgItemListEventPtr>		WgItemsSelectEventPtr;
typedef	WgWeakPtr<WgItemsSelectEvent,WgItemListEventWeakPtr>	WgItemsSelectEventWeakPtr;

class WgItemsUnselectEvent;
typedef	WgStrongPtr<WgItemsUnselectEvent,WgItemListEventPtr>		WgItemsUnselectEventPtr;
typedef	WgWeakPtr<WgItemsUnselectEvent,WgItemListEventWeakPtr>	WgItemsUnselectEventWeakPtr;

class WgItemToggleEvent;
typedef	WgStrongPtr<WgItemToggleEvent,WgItemEventPtr>		WgItemToggleEventPtr;
typedef	WgWeakPtr<WgItemToggleEvent,WgItemEventWeakPtr>	WgItemToggleEventWeakPtr;

class WgItemMousePressEvent;
typedef	WgStrongPtr<WgItemMousePressEvent,WgItemEventPtr>		WgItemMousePressEventPtr;
typedef	WgWeakPtr<WgItemMousePressEvent,WgItemEventWeakPtr>	WgItemMousePressEventWeakPtr;

class WgPopupClosedEvent;
typedef	WgStrongPtr<WgPopupClosedEvent,WgEventPtr>		WgPopupClosedEventPtr;
typedef	WgWeakPtr<WgPopupClosedEvent,WgEventWeakPtr>	WgPopupClosedEventWeakPtr;

class WgModalMoveOutsideEvent;
typedef	WgStrongPtr<WgModalMoveOutsideEvent,WgEventPtr>		WgModalMoveOutsideEventPtr;
typedef	WgWeakPtr<WgModalMoveOutsideEvent,WgEventWeakPtr>	WgModalMoveOutsideEventWeakPtr;

class WgModalBlockedPressEvent;
typedef	WgStrongPtr<WgModalBlockedPressEvent,WgEventPtr>		WgModalBlockedPressEventPtr;
typedef	WgWeakPtr<WgModalBlockedPressEvent,WgEventWeakPtr>	WgModalBlockedPressEventWeakPtr;

class WgModalBlockedReleaseEvent;
typedef	WgStrongPtr<WgModalBlockedReleaseEvent,WgEventPtr>		WgModalBlockedReleaseEventPtr;
typedef	WgWeakPtr<WgModalBlockedReleaseEvent,WgEventWeakPtr>	WgModalBlockedReleaseEventWeakPtr;

//

class WgLinkEvent;
typedef	WgStrongPtr<WgLinkEvent,WgEventPtr>		WgLinkEventPtr;
typedef	WgWeakPtr<WgLinkEvent,WgEventWeakPtr>	WgLinkEventWeakPtr;

class WgLinkSelectEvent;
typedef	WgStrongPtr<WgLinkSelectEvent,WgLinkEventPtr>		WgLinkSelectEventPtr;
typedef	WgWeakPtr<WgLinkSelectEvent,WgLinkEventWeakPtr>		WgLinkSelectEventWeakPtr;

class WgLinkMouseEnterEvent;
typedef	WgStrongPtr<WgLinkMouseEnterEvent,WgLinkEventPtr>		WgLinkMouseEnterEventPtr;
typedef	WgWeakPtr<WgLinkMouseEnterEvent,WgLinkEventWeakPtr>		WgLinkMouseEnterEventWeakPtr;

class WgLinkMouseLeaveEvent;
typedef	WgStrongPtr<WgLinkMouseLeaveEvent,WgLinkEventPtr>		WgLinkMouseLeaveEventPtr;
typedef	WgWeakPtr<WgLinkMouseLeaveEvent,WgLinkEventWeakPtr>		WgLinkMouseLeaveEventWeakPtr;

class WgLinkMouseButtonEvent;
typedef	WgStrongPtr<WgLinkMouseButtonEvent,WgLinkEventPtr>		WgLinkMouseButtonEventPtr;
typedef	WgWeakPtr<WgLinkMouseButtonEvent,WgLinkEventWeakPtr>		WgLinkMouseButtonEventWeakPtr;

class WgLinkMousePressEvent;
typedef	WgStrongPtr<WgLinkMousePressEvent,WgLinkMouseButtonEventPtr>		WgLinkMousePressEventPtr;
typedef	WgWeakPtr<WgLinkMousePressEvent,WgLinkMouseButtonEventWeakPtr>		WgLinkMousePressEventWeakPtr;

class WgLinkMouseRepeatEvent;
typedef	WgStrongPtr<WgLinkMouseRepeatEvent,WgLinkMouseButtonEventPtr>		WgLinkMouseRepeatEventPtr;
typedef	WgWeakPtr<WgLinkMouseRepeatEvent,WgLinkMouseButtonEventWeakPtr>		WgLinkMouseRepeatEventWeakPtr;

class WgLinkMouseReleaseEvent;
typedef	WgStrongPtr<WgLinkMouseReleaseEvent,WgLinkMouseButtonEventPtr>		WgLinkMouseReleaseEventPtr;
typedef	WgWeakPtr<WgLinkMouseReleaseEvent,WgLinkMouseButtonEventWeakPtr>		WgLinkMouseReleaseEventWeakPtr;

class WgLinkMouseClickEvent;
typedef	WgStrongPtr<WgLinkMouseClickEvent,WgLinkMouseButtonEventPtr>		WgLinkMouseClickEventPtr;
typedef	WgWeakPtr<WgLinkMouseClickEvent,WgLinkMouseButtonEventWeakPtr>		WgLinkMouseClickEventWeakPtr;

class WgLinkMouseDoubleClickEvent;
typedef	WgStrongPtr<WgLinkMouseDoubleClickEvent,WgLinkMouseButtonEventPtr>		WgLinkMouseDoubleClickEventPtr;
typedef	WgWeakPtr<WgLinkMouseDoubleClickEvent,WgLinkMouseButtonEventWeakPtr>		WgLinkMouseDoubleClickEventWeakPtr;

//____ WgEvent _________________________________________________________________
/**
 * @brief Base class for all events.
 */


class WgEvent : public WgObject
{
	friend class WgEventHandler;
	friend class WgWidget;

	public:

		bool				IsInstanceOf( const char * pClassName ) const;
		const char *		ClassName( void ) const;
		static const char	CLASSNAME[];
		static WgEventPtr	Cast( const WgObjectPtr& pObject );

		WgEventType			Type() const { return m_type; }
		int64_t				Timestamp() const { return m_timestamp; }
		bool				IsFromWidget() const { return m_bIsForWidget; }
		WgWidget *			Widget() const;									// Inlining this would demand include of wg_widget.h.
		WgWidgetWeakPtr		WidgetWeakPtr() const;
		WgModifierKeys		ModKeys() const { return m_modKeys; }
		WgCoord				PointerPos() const { return m_pointerLocalPos; }
		WgCoord				PointerGlobalPos() const { return m_pointerScreenPos; }
			
		bool				IsMouseEvent() const;
		bool				IsMouseButtonEvent() const;
		bool				IsKeyEvent() const;

	protected:
		WgEvent();
		virtual ~WgEvent() {}
			
		WgEventType			m_type;				// Type of event
		WgModifierKeys		m_modKeys;			// Modifier keys pressed when event posted.
		int64_t				m_timestamp;		// Timestamp of posting this event
		bool				m_bIsForWidget;		// Set if this event is for a specific Widget.
		WgWidgetWeakPtr		m_pWidget;			// Widget to receive this event.
		WgCoord				m_pointerLocalPos;	// Widget-relative position of pointer. Same as m_pointerScreenPos if Widget not set.
		WgCoord				m_pointerScreenPos;	// Screen position of pointer.
};

//____ WgMouseButtonEvent ______________________________________________________
/**
 * @brief Base class for all mouse button events.
 * 
 */

class WgMouseButtonEvent : public WgEvent
{
	friend class WgEventHandler;
public:
	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgMouseButtonEventPtr	Cast( const WgObjectPtr& pObject );

	WgMouseButton		Button() const { return m_button; }
protected:
	WgMouseButtonEvent(WgMouseButton button) : m_button(button) {}

	WgMouseButton		m_button;
};

//____ WgKeyEvent ______________________________________________________________
/**
 * @brief Base class for all keyboard events.
 */

class WgKeyEvent : public WgEvent
{
	friend class WgEventHandler;
public:
	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgKeyEventPtr	Cast( const WgObjectPtr& pObject );

	int		NativeKeyCode() const { return m_nativeKeyCode; }
	int		TranslatedKeyCode() const { return m_translatedKeyCode; }
	bool	IsCursorKey() const;
	bool	IsMovementKey() const;
protected:
	WgKeyEvent( int nativeKeyCode ) : m_nativeKeyCode(nativeKeyCode), m_translatedKeyCode(0) {}

	int		m_nativeKeyCode;
	int		m_translatedKeyCode;
};

//____ WgFocusGainedEvent ______________________________________________________

class WgFocusGainedEvent : public WgEvent
{
	friend class WgEventHandler;
public:
	static WgFocusGainedEventPtr	Create() { return new WgFocusGainedEvent(); }

	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgFocusGainedEventPtr	Cast( const WgObjectPtr& pObject );
protected:
	WgFocusGainedEvent();
};

//____ WgFocusLostEvent ________________________________________________________

class WgFocusLostEvent : public WgEvent
{
	friend class WgEventHandler;
public:
	static WgFocusLostEventPtr	Create() { return new WgFocusLostEvent(); }

	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgFocusLostEventPtr	Cast( const WgObjectPtr& pObject );
protected:
	WgFocusLostEvent();
};

//____ WgMouseEnterEvent _______________________________________________________

class WgMouseEnterEvent : public WgEvent
{
	friend class WgEventHandler;
public:
	static WgMouseEnterEventPtr			Create( const WgCoord& pos ) { return new WgMouseEnterEvent(pos); }

	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgMouseEnterEventPtr	Cast( const WgObjectPtr& pObject );
protected:
	WgMouseEnterEvent( const WgCoord& pos );
	WgMouseEnterEvent( WgWidget * pWidget );
};

//____ WgMouseLeaveEvent _______________________________________________________

class WgMouseLeaveEvent : public WgEvent
{
	friend class WgEventHandler;
public:
	static WgMouseLeaveEventPtr			Create() { return new WgMouseLeaveEvent(); }

	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgMouseLeaveEventPtr	Cast( const WgObjectPtr& pObject );
protected:
	WgMouseLeaveEvent();
	WgMouseLeaveEvent( WgWidget * pWidget );
};

//____ WgMouseMoveEvent ________________________________________________________

class WgMouseMoveEvent : public WgEvent
{
	friend class WgEventHandler;
public:
	static WgMouseMoveEventPtr			Create( const WgCoord& pos ) { return new WgMouseMoveEvent(pos); }

	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgMouseMoveEventPtr	Cast( const WgObjectPtr& pObject );
protected:
	WgMouseMoveEvent( WgWidget * pWidget );
	WgMouseMoveEvent( const WgCoord& pos );
};

//____ WgMousePressEvent _______________________________________________________

class WgMousePressEvent : public WgMouseButtonEvent
{
	friend class WgEventHandler;
public:
	static WgMousePressEventPtr			Create( WgMouseButton button ) { return new WgMousePressEvent(button); }

	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgMousePressEventPtr	Cast( const WgObjectPtr& pObject );
protected:
	WgMousePressEvent( WgMouseButton button, WgWidget * pWidget );
	WgMousePressEvent( WgMouseButton button );
};

//____ WgMouseReleaseEvent _____________________________________________________

class WgMouseReleaseEvent : public WgMouseButtonEvent
{
	friend class WgEventHandler;
public:
	static WgMouseReleaseEventPtr		Create( WgMouseButton button ) { return new WgMouseReleaseEvent(button); }

	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgMouseReleaseEventPtr	Cast( const WgObjectPtr& pObject );

	bool			PressInside() const;
	bool			ReleaseInside() const;

protected:
	WgMouseReleaseEvent( WgMouseButton button );
	WgMouseReleaseEvent( WgMouseButton button, WgWidget * pWidget, bool bPressInside, bool bReleaseInside );

	bool			m_bPressInside;
	bool			m_bReleaseInside;
};

//____ WgKeyPressEvent _________________________________________________________

class WgKeyPressEvent : public WgKeyEvent
{
	friend class WgEventHandler;
public:
	static WgKeyPressEventPtr			Create( int native_keycode ) { return new WgKeyPressEvent(native_keycode); }

	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgKeyPressEventPtr	Cast( const WgObjectPtr& pObject );
protected:
	WgKeyPressEvent( int native_keycode );
	WgKeyPressEvent( int native_keycode, WgWidget * pWidget );
};

//____ WgKeyReleaseEvent _______________________________________________________

class WgKeyReleaseEvent : public WgKeyEvent
{
	friend class WgEventHandler;
public:
	static WgKeyReleaseEventPtr			Create( int native_keycode ) { return new WgKeyReleaseEvent(native_keycode); }

	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgKeyReleaseEventPtr	Cast( const WgObjectPtr& pObject );
protected:
	WgKeyReleaseEvent( int native_keycode );
	WgKeyReleaseEvent( int native_keycode, WgWidget * pWidget );
};

//____ WgCharacterEvent ________________________________________________________

class WgCharacterEvent : public WgEvent
{
	friend class WgEventHandler;
public:
	static WgCharacterEventPtr			Create( unsigned short character ) { return new WgCharacterEvent(character); }

	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgCharacterEventPtr	Cast( const WgObjectPtr& pObject );

	unsigned short	Char() const;
protected:
	WgCharacterEvent( unsigned short character );
	WgCharacterEvent( unsigned short character, WgWidget * pWidget );
protected:
	unsigned short	m_char;
};

//____ WgWheelRollEvent ________________________________________________________

class WgWheelRollEvent : public WgEvent
{
	friend class WgEventHandler;
public:
	static WgWheelRollEventPtr			Create( int wheel, int distance ) { return new WgWheelRollEvent(wheel,distance); }

	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgWheelRollEventPtr	Cast( const WgObjectPtr& pObject );

	int			Wheel() const;
	int			Distance() const;
protected:
	WgWheelRollEvent( int wheel, int distance );
	WgWheelRollEvent( int wheel, int distance, WgWidget * pWidget );

	int			m_wheel;
	int			m_distance;
};

//____ WgTickEvent _____________________________________________________________

class WgTickEvent : public WgEvent
{
	friend class WgEventHandler;
public:
	static WgTickEventPtr				Create( int ms ) { return new WgTickEvent(ms); }

	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgTickEventPtr	Cast( const WgObjectPtr& pObject );

	int				Millisec() const;
protected:
	WgTickEvent( int ms );
	WgTickEvent( int ms, WgWidget * pWidget );

	int			m_millisec;
};

//____ WgPointerChangeEvent ____________________________________________________

class WgPointerChangeEvent : public WgEvent
{
	friend class WgEventHandler;
public:
	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgPointerChangeEventPtr	Cast( const WgObjectPtr& pObject );

	WgPointerStyle	Style() const;
		
protected:
	WgPointerChangeEvent( WgPointerStyle style );
		
	WgPointerStyle	m_style;
};


//____ WgSelectEvent ___________________________________________________________

class WgSelectEvent : public WgEvent
{
public:
	WgSelectEvent( WgWidget * pWidget );

	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgSelectEventPtr	Cast( const WgObjectPtr& pObject );
};

//____ WgToggleEvent ___________________________________________________________

class WgToggleEvent : public WgEvent
{
public:
	WgToggleEvent( WgWidget * pWidget, bool bSet );

	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgToggleEventPtr	Cast( const WgObjectPtr& pObject );

	bool		IsSet() const;

private:
	bool	m_bSet;
};

//____ WgValueUpdateEvent ______________________________________________________

class WgValueUpdateEvent : public WgEvent
{
public:
	WgValueUpdateEvent( WgWidget * pWidget, int64_t value, double fraction, bool bFinal );

	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgValueUpdateEventPtr	Cast( const WgObjectPtr& pObject );

	int64_t		Value() const;
	double		Fraction() const;
	bool		IsFinal() const;

protected:
	int64_t		m_value;
	double		m_fraction;
	bool		m_bFinal;
};

//____ WgRangeUpdateEvent ______________________________________________________

class WgRangeUpdateEvent : public WgEvent
{
public:
	WgRangeUpdateEvent( WgWidget * pWidget, int intOfs, int intLength, double fracOfs, double fracLength, bool bFinal ); 

	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgRangeUpdateEventPtr	Cast( const WgObjectPtr& pObject );

	int			Offset() const;
	int			Length() const;
	double		FracOffset() const;
	double		FracLength() const;
	bool		IsFinal() const;

	int			m_ofs;
	int			m_length;
	double		m_fracOfs;
	double		m_fracLength;
	bool		m_bFinal;
};

//____ WgTextEditEvent _________________________________________________________

class WgTextEditEvent : public WgEvent
{
public:
	WgTextEditEvent( const WgEditTextFieldPtr& pText, bool bFinal );

	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgTextEditEventPtr	Cast( const WgObjectPtr& pObject );

	WgEditTextFieldPtr	Text() const;
	bool				IsFinal() const;

protected:
	WgEditTextFieldPtr	m_pText;
	bool				m_bFinal;
};


//____

class WgItemInfo
{
public:
	WgItemInfo(): id(-1) {}
	WgItemInfo( int _id, const WgObjectPtr& _pObject ) : id(_id), pObject(_pObject) {}

	int				id;
	WgObjectPtr		pObject;
};

//____ WgItemEvent ____________________________________________________________

class WgItemEvent : public WgEvent
{
public:
	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgItemEventPtr	Cast( const WgObjectPtr& pObject );

	int				ItemId() const;
	WgObjectPtr		ItemObject() const;

protected:
	WgItemEvent( WgWidget * pWidget, int itemId, const WgObjectPtr& pItemObject );

	WgItemInfo		m_item;
};

//____ WgItemToggleEvent ______________________________________________________

class WgItemToggleEvent : public WgItemEvent
{
public:
	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgItemToggleEventPtr	Cast( const WgObjectPtr& pObject );

	bool		IsSet() const;

//protected:
	WgItemToggleEvent( WgWidget * pWidget, int itemId, const WgObjectPtr& pItemObject, bool bSet );

	bool		m_bSet;
};

//____ WgItemMousePressEvent ______________________________________________________

class WgItemMousePressEvent : public WgItemEvent
{
public:
	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgItemMousePressEventPtr	Cast( const WgObjectPtr& pObject );

	WgMouseButton		Button() const;
//protected:
	WgItemMousePressEvent( WgWidget * pWidget, int itemId, const WgObjectPtr& pItemObject, WgMouseButton button );

	WgMouseButton		m_button;
};


//____ WgItemListEvent ___________________________________________________________

class WgItemListEvent : public WgEvent
{
public:
	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgItemListEventPtr	Cast( const WgObjectPtr& pObject );

	int					NbItems() const;
	const WgItemInfo *	Items() const;

protected:
	WgItemListEvent( WgWidget * pWidget, int nbItems, WgItemInfo * pItems );
	virtual ~WgItemListEvent();
	int				m_nbItems;
	WgItemInfo *	m_pItems;
};


//____ WgItemsSelectEvent _____________________________________________________

class WgItemsSelectEvent : public WgItemListEvent
{
public:
	WgItemsSelectEvent( WgWidget * pWidget, int nbItems, WgItemInfo * pItems );

	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgItemsSelectEventPtr	Cast( const WgObjectPtr& pObject );
};

//____ WgItemsUnselectEvent _____________________________________________________

class WgItemsUnselectEvent : public WgItemListEvent
{
public:
	WgItemsUnselectEvent( WgWidget * pWidget, int nbItems, WgItemInfo * pItems );

	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgItemsUnselectEventPtr	Cast( const WgObjectPtr& pObject );
};

//____ WgPopupClosedEvent ______________________________________________________

class WgPopupClosedEvent : public WgEvent
{
	friend class WgPopupLayer;
public:
	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgPopupClosedEventPtr	Cast( const WgObjectPtr& pObject );

	WgWidget *		Popup() const;									// Inlining this would demand include of wg_widget.h.
	WgWidgetWeakPtr	PopupWeakPtr() const;							// Caller is the same as m_pWidget, since m_pWidget should receive
	// the event.

	WgWidget *		Caller() const;									// Inlining this would demand include of wg_widget.h.
	WgWidgetWeakPtr	CallerWeakPtr() const;

protected:
	WgPopupClosedEvent( WgWidget * pPopup, const WgWidgetWeakPtr& pCaller );

	WgWidgetWeakPtr m_pPopup;
};

//____ WgWidgetModalLayer events _________________________________________________

class WgModalMoveOutsideEvent : public WgEvent
{
	friend class WgModalLayer;
public:
	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgModalMoveOutsideEventPtr	Cast( const WgObjectPtr& pObject );

protected:
	WgModalMoveOutsideEvent( WgWidget * pWidget );
};

class WgModalBlockedPressEvent : public WgMouseButtonEvent
{
	friend class WgModalLayer;
public:
	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgModalBlockedPressEventPtr	Cast( const WgObjectPtr& pObject );

protected:
	WgModalBlockedPressEvent( WgMouseButton button, WgWidget * pModalWidget );
};

class WgModalBlockedReleaseEvent : public WgMouseButtonEvent
{
	friend class WgModalLayer;
public:
	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgModalBlockedReleaseEventPtr	Cast( const WgObjectPtr& pObject );

protected:
	WgModalBlockedReleaseEvent( WgMouseButton button, WgWidget * pModalWidget );
};

//____ Link events _________________________________________________________

class WgLinkEvent : public WgEvent
{
public:
	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgLinkEventPtr	Cast( const WgObjectPtr& pObject );

	std::string		Link() const;
protected:
	std::string		m_link;
};

class WgLinkSelectEvent : public WgLinkEvent
{
public:
	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgLinkSelectEventPtr	Cast( const WgObjectPtr& pObject );

	WgLinkSelectEvent( WgWidget * pWidget, std::string link );
};


class WgLinkMouseEnterEvent : public WgLinkEvent
{
public:
	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgLinkMouseEnterEventPtr	Cast( const WgObjectPtr& pObject );

	WgLinkMouseEnterEvent( WgWidget * pWidget, std::string link );
};

class WgLinkMouseLeaveEvent : public WgLinkEvent
{
public:
	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgLinkMouseLeaveEventPtr	Cast( const WgObjectPtr& pObject );

	WgLinkMouseLeaveEvent( WgWidget * pWidget, std::string link );
};

class WgLinkMouseButtonEvent : public WgLinkEvent
{
public:
	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgLinkMouseButtonEventPtr	Cast( const WgObjectPtr& pObject );

	WgMouseButton		Button() const;
protected:
	WgMouseButton		m_button;
};

class WgLinkMousePressEvent : public WgLinkMouseButtonEvent
{
public:
	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgLinkMousePressEventPtr	Cast( const WgObjectPtr& pObject );

	WgLinkMousePressEvent( WgWidget * pWidget, std::string link, WgMouseButton button );
};

class WgLinkMouseRepeatEvent : public WgLinkMouseButtonEvent
{
public:
	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgLinkMouseRepeatEventPtr	Cast( const WgObjectPtr& pObject );

	WgLinkMouseRepeatEvent( WgWidget * pWidget, std::string link, WgMouseButton button );
};

class WgLinkMouseReleaseEvent : public WgLinkMouseButtonEvent
{
public:
	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgLinkMouseReleaseEventPtr	Cast( const WgObjectPtr& pObject );

	WgLinkMouseReleaseEvent( WgWidget * pWidget, std::string link, WgMouseButton button );
};

class WgLinkMouseClickEvent : public WgLinkMouseButtonEvent
{
public:
	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgLinkMouseClickEventPtr	Cast( const WgObjectPtr& pObject );

	WgLinkMouseClickEvent( WgWidget * pWidget, std::string link, WgMouseButton button );
};

class WgLinkMouseDoubleClickEvent : public WgLinkMouseButtonEvent
{
public:
	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgLinkMouseDoubleClickEventPtr	Cast( const WgObjectPtr& pObject );

	WgLinkMouseDoubleClickEvent( WgWidget * pWidget, std::string link, WgMouseButton button );
};

//____ Internally posted events ____________________________________________

class WgMousePositionEvent : public WgEvent
{
	friend class WgEventHandler;
public:
	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgMousePositionEventPtr	Cast( const WgObjectPtr& pObject );

protected:
	WgMousePositionEvent();
};

class WgMouseDragEvent : public WgMouseButtonEvent
{
	friend class WgEventHandler;
protected:
	WgMouseDragEvent( WgMouseButton button, const WgCoord& startPos, const WgCoord& prevPos, const WgCoord& currPos );
	WgMouseDragEvent( WgMouseButton button, WgWidget * pWidget, const WgCoord& orgPos, const WgCoord& prevPos, const WgCoord& currPos );
public:
	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgMouseDragEventPtr	Cast( const WgObjectPtr& pObject );

	WgCoord			DraggedTotal() const;
	WgCoord			DraggedNow() const;
	WgCoord			StartPos() const;
	WgCoord			PrevPos() const;
	WgCoord			CurrPos() const;
protected:
	WgCoord			m_startPos;
	WgCoord			m_prevPos;
	WgCoord			m_currPos;
};

class WgMouseRepeatEvent : public WgMouseButtonEvent
{
	friend class WgEventHandler;
protected:
	WgMouseRepeatEvent( WgMouseButton button, WgWidget * pWidget );
	WgMouseRepeatEvent( WgMouseButton button );
public:
	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgMouseRepeatEventPtr	Cast( const WgObjectPtr& pObject );
};

class WgMouseClickEvent : public WgMouseButtonEvent
{
	friend class WgEventHandler;
protected:
	WgMouseClickEvent( WgMouseButton button );
	WgMouseClickEvent( WgMouseButton button, WgWidget * pWidget );
public:
	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgMouseClickEventPtr	Cast( const WgObjectPtr& pObject );
};

class WgMouseDoubleClickEvent : public WgMouseButtonEvent
{
	friend class WgEventHandler;
public:
	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgMouseDoubleClickEventPtr	Cast( const WgObjectPtr& pObject );

protected:
	WgMouseDoubleClickEvent( WgMouseButton button );
	WgMouseDoubleClickEvent( WgMouseButton button, WgWidget * pWidget );
};

class WgKeyRepeatEvent : public WgKeyEvent
{
	friend class WgEventHandler;
public:
	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgKeyRepeatEventPtr	Cast( const WgObjectPtr& pObject );

protected:
	WgKeyRepeatEvent( int native_keycode );
	WgKeyRepeatEvent( int native_keycode, WgWidget * pWidget );
};




#endif //WG_EVENT_DOT_H
