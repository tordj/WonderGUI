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

#ifndef WG_EVENTFILTER_DOT_H
#define WG_EVENTFILTER_DOT_H

#ifndef WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

#ifndef WG_SMARTPTR_DOT_H
#	include <wg_smartptr.h>
#endif

#ifndef WG_KEY_DOT_H
#	include <wg_key.h>
#endif

namespace WgEvent
{
	class Event;
};

class WgGizmo;
class WgEventFilter;

typedef class WgWeakPtr<WgGizmo> WgGizmoWeakPtr;

typedef	bool(*WgFilterFuncPtr)(const WgEvent::Event *, const WgEventFilter& );


class WgEventFilter
{
	friend class WgEventHandler;
public:

	static WgEventFilter	Tick();
	static WgEventFilter	PointerChange();

	static WgEventFilter	FocusGained();
	static WgEventFilter	FocusLost();

	static WgEventFilter	MouseEnter();
	static WgEventFilter	MouseEnter( WgGizmo * pGizmo );

	static WgEventFilter	MouseLeave();
	static WgEventFilter	MouseLeave( WgGizmo * pGizmo );

	static WgEventFilter	MouseMove();
	static WgEventFilter	MouseMove( WgGizmo * pGizmo );

	static WgEventFilter	MouseButtonPress();
	static WgEventFilter	MouseButtonPress( int button);
	static WgEventFilter	MouseButtonPress( WgGizmo * pGizmo );
	static WgEventFilter	MouseButtonPress( WgGizmo * pGizmo, int button );

	static WgEventFilter	MouseButtonRelease();
	static WgEventFilter	MouseButtonRelease( int button);
	static WgEventFilter	MouseButtonRelease( WgGizmo * pGizmo );
	static WgEventFilter	MouseButtonRelease( WgGizmo * pGizmo, int button );

	static WgEventFilter	MouseButtonDrag();
	static WgEventFilter	MouseButtonDrag( int button);
	static WgEventFilter	MouseButtonDrag( WgGizmo * pGizmo );
	static WgEventFilter	MouseButtonDrag( WgGizmo * pGizmo, int button );

	static WgEventFilter	MouseButtonRepeat();
	static WgEventFilter	MouseButtonRepeat( int button);
	static WgEventFilter	MouseButtonRepeat( WgGizmo * pGizmo );
	static WgEventFilter	MouseButtonRepeat( WgGizmo * pGizmo, int button );

	static WgEventFilter	MouseButtonClick();
	static WgEventFilter	MouseButtonClick( int button);
	static WgEventFilter	MouseButtonClick( WgGizmo * pGizmo );
	static WgEventFilter	MouseButtonClick( WgGizmo * pGizmo, int button );

	static WgEventFilter	MouseButtonDoubleClick();
	static WgEventFilter	MouseButtonDoubleClick( int button);
	static WgEventFilter	MouseButtonDoubleClick( WgGizmo * pGizmo );
	static WgEventFilter	MouseButtonDoubleClick( WgGizmo * pGizmo, int button );

	static WgEventFilter	KeyPress();
	static WgEventFilter	KeyPress( WgGizmo * pGizmo );
	static WgEventFilter	KeyPress( WgKey translated_keycode );
	static WgEventFilter	KeyPress( WgGizmo * pGizmo, WgKey keycode );

	static WgEventFilter	KeyRelease();
	static WgEventFilter	KeyRelease( WgGizmo * pGizmo );
	static WgEventFilter	KeyRelease( WgKey translated_keycode );
	static WgEventFilter	KeyRelease( WgGizmo * pGizmo, WgKey keycode );

	static WgEventFilter	KeyRepeat();
	static WgEventFilter	KeyRepeat( WgGizmo * pGizmo );
	static WgEventFilter	KeyRepeat( WgKey keycode );
	static WgEventFilter	KeyRepeat( WgGizmo * pGizmo, WgKey keycode );

	static WgEventFilter	KeyPressNative( int native_keycode );
	static WgEventFilter	KeyPressNative( WgGizmo * pGizmo, int native_keycode );

	static WgEventFilter	KeyReleaseNative( int native_keycode );
	static WgEventFilter	KeyReleaseNative( WgGizmo * pGizmo, int native_keycode );

	static WgEventFilter	KeyRepeatNative( int native_keycode );
	static WgEventFilter	KeyRepeatNative( WgGizmo * pGizmo, int native_keycode );

	static WgEventFilter	Character();
	static WgEventFilter	Character( unsigned short character);
	static WgEventFilter	Character( WgGizmo * pGizmo );
	static WgEventFilter	Character( WgGizmo * pGizmo, unsigned short character );

	static WgEventFilter	MouseWheelRoll();
	static WgEventFilter	MouseWheelRoll( int wheel );
	static WgEventFilter	MouseWheelRoll( WgGizmo * pGizmo );
	static WgEventFilter	MouseWheelRoll( WgGizmo * pGizmo, int wheel );

	static WgEventFilter	ButtonPress();
	static WgEventFilter	ButtonPress( WgGizmo * pGizmo );

	static WgEventFilter	CheckboxCheck();
	static WgEventFilter	CheckboxCheck( WgGizmoCheckbox * pGizmo );

	static WgEventFilter	CheckboxUncheck();
	static WgEventFilter	CheckboxUncheck( WgGizmoCheckbox * pGizmo );

	static WgEventFilter	CheckboxToggle();
	static WgEventFilter	CheckboxToggle( WgGizmoCheckbox * pGizmo );

	static WgEventFilter	RadiobuttonSelect();
	static WgEventFilter	RadiobuttonSelect( WgGizmoRadiobutton * pGizmo );

	static WgEventFilter	RadiobuttonUnselect();
	static WgEventFilter	RadiobuttonUnselect( WgGizmoRadiobutton * pGizmo );

	static WgEventFilter	RadiobuttonToggle();
	static WgEventFilter	RadiobuttonToggle( WgGizmoRadiobutton * pGizmo );

	static WgEventFilter	AnimationUpdate();
	static WgEventFilter	AnimationUpdate( WgGizmoAnimation * pGizmo );

	static WgEventFilter	TabSelect();
	static WgEventFilter	TabSelect( WgGizmoTablist * pGizmo );
	static WgEventFilter	TabSelect( WgGizmoTablist * pGizmo, int id );

	static WgEventFilter	TabPress();
	static WgEventFilter	TabPress( int button );
	static WgEventFilter	TabPress( WgGizmoTablist * pGizmo );
	static WgEventFilter	TabPress( WgGizmoTablist * pGizmo, int button );
	static WgEventFilter	TabPress( WgGizmoTablist * pGizmo, int button, int id );

	static WgEventFilter	EditvalueModify();
	static WgEventFilter	EditvalueModify( WgGizmoEditvalue * pGizmo );

	static WgEventFilter	EditvalueSet();
	static WgEventFilter	EditvalueSet( WgGizmoEditvalue * pGizmo );

	static WgEventFilter	SliderMove();
	static WgEventFilter	SliderMove( WgGizmoSlider * pGizmo );

	static WgEventFilter	SliderStepFwd();
	static WgEventFilter	SliderStepFwd( WgGizmoSlider * pGizmo );

	static WgEventFilter	SliderStepBwd();
	static WgEventFilter	SliderStepBwd( WgGizmoSlider * pGizmo );

	static WgEventFilter	SliderJumpFwd();
	static WgEventFilter	SliderJumpFwd( WgGizmoSlider * pGizmo );

	static WgEventFilter	SliderJumpBwd();
	static WgEventFilter	SliderJumpBwd( WgGizmoSlider * pGizmo );

	static WgEventFilter	SliderWheelRoll();
	static WgEventFilter	SliderWheelRoll( WgGizmoSlider * pGizmo );

	static WgEventFilter	TextModify();
	static WgEventFilter	TextModify( WgGizmo * pGizmo );

	static WgEventFilter	TextSet();
	static WgEventFilter	TextSet( WgGizmo * pGizmo );

	static WgEventFilter	MenuitemSelect();
	static WgEventFilter	MenuitemSelect( int itemId );
	static WgEventFilter	MenuitemSelect( WgGizmoMenu * pGizmo );
	static WgEventFilter	MenuitemSelect( WgGizmoMenu * pGizmo, int itemId );

	static WgEventFilter	MenuitemCheck();
	static WgEventFilter	MenuitemCheck( int itemId );
	static WgEventFilter	MenuitemCheck( WgGizmoMenu * pGizmo );
	static WgEventFilter	MenuitemCheck( WgGizmoMenu * pGizmo, int itemId );

	static WgEventFilter	MenuitemUncheck();
	static WgEventFilter	MenuitemUncheck( int itemId );
	static WgEventFilter	MenuitemUncheck( WgGizmoMenu * pGizmo );
	static WgEventFilter	MenuitemUncheck( WgGizmoMenu * pGizmo, int itemId );


	//TODO: Add missing filters!


	static WgEventFilter	KnobTurn();
	static WgEventFilter	KnobTurn( WgGizmo * pGizmo );


	inline bool			FilterEvent( const WgEvent::Event * pEvent ) const { return m_pFilterFunc( pEvent, *this ); }
	inline WgEventType 	EventType() const { return m_eventType; }
	inline bool			FiltersGizmo() const { return m_pGizmo?true:false; }
	inline WgGizmo *	Gizmo() const { return m_pGizmo; }
private:
	WgEventFilter() : m_eventType(WG_EVENT_DUMMY), m_pFilterFunc(_filterDummy) {}
	WgEventFilter( WgEventType eventType, WgGizmo * pGizmo, WgFilterFuncPtr pFunc, int data1 = 0, int data2 = 0 )
				: m_eventType(eventType), m_pGizmo(pGizmo), m_pFilterFunc(pFunc), m_data1(data1), m_data2(data2) {}


	WgEventType			m_eventType;		// Only event type filter accepts, if a specific.
	WgGizmo *	 		m_pGizmo;			// Only Gizmo filter accepts, if a specific one.
	WgFilterFuncPtr 	m_pFilterFunc;		// Pointer at filter function.
	int					m_data1;			// Additional data for filter function.
	int					m_data2;			// Additional data for filter function.


	static bool _filterDummy( const WgEvent::Event * pEvent, const WgEventFilter& filter );
	static bool _filterType( const WgEvent::Event * pEvent, const WgEventFilter& filter );
	static bool _filterTypeGizmo( const WgEvent::Event * pEvent, const WgEventFilter& filter );

	static bool _filterPointerEvents( const WgEvent::Event * pEvent, const WgEventFilter& filter );
	static bool _filterMouseButtonEvents( const WgEvent::Event * pEvent, const WgEventFilter& filter );
	static bool _filterKeyEvents( const WgEvent::Event * pEvent, const WgEventFilter& filter );
	static bool _filterNativeKeyEvents( const WgEvent::Event * pEvent, const WgEventFilter& filter );
	static bool _filterCharacterEvents( const WgEvent::Event * pEvent, const WgEventFilter& filter );
	static bool _filterMouseWheelRollEvents( const WgEvent::Event * pEvent, const WgEventFilter& filter );
	static bool _filterTabSelect( const WgEvent::Event * pEvent, const WgEventFilter& filter );
	static bool _filterTabPress( const WgEvent::Event * pEvent, const WgEventFilter& filter );
	static bool _filterMenuitemEvent( const WgEvent::Event * pEvent, const WgEventFilter& filter );
};


#endif // WG_EVENTFILTER_DOT_H
