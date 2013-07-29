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

#ifndef WG_WIDGET_DOT_H
#	include <wg_widget.h>
#endif

namespace WgEvent
{
	class Event;
};



class WgEventFilter;

typedef	bool(*WgFilterFuncPtr)(const WgEvent::Event *, const WgEventFilter& );


class WgEventFilter
{
	friend class WgEventHandler;
	
public:
	WgEventFilter() : m_eventType(WG_EVENT_DUMMY), m_pFilterFunc(_filterDummy) {}	//Not meant to be public, but has to for compatibility with older compilers.
	
	static WgEventFilter	Tick();
	static WgEventFilter	PointerChange();

	static WgEventFilter	FocusGained();
	static WgEventFilter	FocusLost();

	static WgEventFilter	MouseEnter();
	static WgEventFilter	MouseEnter( const WgWidgetPtr& pWidget );

	static WgEventFilter	MouseLeave();
	static WgEventFilter	MouseLeave( const WgWidgetPtr& pWidget );

	static WgEventFilter	MouseMove();
	static WgEventFilter	MouseMove( const WgWidgetPtr& pWidget );

	static WgEventFilter	MouseButtonPress();
	static WgEventFilter	MouseButtonPress( int button);
	static WgEventFilter	MouseButtonPress( const WgWidgetPtr& pWidget );
	static WgEventFilter	MouseButtonPress( const WgWidgetPtr& pWidget, int button );

	static WgEventFilter	MouseButtonRelease();
	static WgEventFilter	MouseButtonRelease( int button);
	static WgEventFilter	MouseButtonRelease( const WgWidgetPtr& pWidget );
	static WgEventFilter	MouseButtonRelease( const WgWidgetPtr& pWidget, int button );

	static WgEventFilter	MouseButtonDrag();
	static WgEventFilter	MouseButtonDrag( int button);
	static WgEventFilter	MouseButtonDrag( const WgWidgetPtr& pWidget );
	static WgEventFilter	MouseButtonDrag( const WgWidgetPtr& pWidget, int button );

	static WgEventFilter	MouseButtonRepeat();
	static WgEventFilter	MouseButtonRepeat( int button);
	static WgEventFilter	MouseButtonRepeat( const WgWidgetPtr& pWidget );
	static WgEventFilter	MouseButtonRepeat( const WgWidgetPtr& pWidget, int button );

	static WgEventFilter	MouseButtonClick();
	static WgEventFilter	MouseButtonClick( int button);
	static WgEventFilter	MouseButtonClick( const WgWidgetPtr& pWidget );
	static WgEventFilter	MouseButtonClick( const WgWidgetPtr& pWidget, int button );

	static WgEventFilter	MouseButtonDoubleClick();
	static WgEventFilter	MouseButtonDoubleClick( int button);
	static WgEventFilter	MouseButtonDoubleClick( const WgWidgetPtr& pWidget );
	static WgEventFilter	MouseButtonDoubleClick( const WgWidgetPtr& pWidget, int button );

	static WgEventFilter	KeyPress();
	static WgEventFilter	KeyPress( const WgWidgetPtr& pWidget );
	static WgEventFilter	KeyPress( WgKey translated_keycode );
	static WgEventFilter	KeyPress( const WgWidgetPtr& pWidget, WgKey keycode );

	static WgEventFilter	KeyRelease();
	static WgEventFilter	KeyRelease( const WgWidgetPtr& pWidget );
	static WgEventFilter	KeyRelease( WgKey translated_keycode );
	static WgEventFilter	KeyRelease( const WgWidgetPtr& pWidget, WgKey keycode );

	static WgEventFilter	KeyRepeat();
	static WgEventFilter	KeyRepeat( const WgWidgetPtr& pWidget );
	static WgEventFilter	KeyRepeat( WgKey keycode );
	static WgEventFilter	KeyRepeat( const WgWidgetPtr& pWidget, WgKey keycode );

	static WgEventFilter	KeyPressNative( int native_keycode );
	static WgEventFilter	KeyPressNative( const WgWidgetPtr& pWidget, int native_keycode );

	static WgEventFilter	KeyReleaseNative( int native_keycode );
	static WgEventFilter	KeyReleaseNative( const WgWidgetPtr& pWidget, int native_keycode );

	static WgEventFilter	KeyRepeatNative( int native_keycode );
	static WgEventFilter	KeyRepeatNative( const WgWidgetPtr& pWidget, int native_keycode );

	static WgEventFilter	Character();
	static WgEventFilter	Character( unsigned short character);
	static WgEventFilter	Character( const WgWidgetPtr& pWidget );
	static WgEventFilter	Character( const WgWidgetPtr& pWidget, unsigned short character );

	static WgEventFilter	MouseWheelRoll();
	static WgEventFilter	MouseWheelRoll( int wheel );
	static WgEventFilter	MouseWheelRoll( const WgWidgetPtr& pWidget );
	static WgEventFilter	MouseWheelRoll( const WgWidgetPtr& pWidget, int wheel );

	static WgEventFilter	ButtonPress();
	static WgEventFilter	ButtonPress( const WgWidgetPtr& pWidget );

	static WgEventFilter	CheckboxCheck();
	static WgEventFilter	CheckboxCheck( const WgWidgetPtr& pWidget );

	static WgEventFilter	CheckboxUncheck();
	static WgEventFilter	CheckboxUncheck( const WgWidgetPtr& pWidget );

	static WgEventFilter	CheckboxToggle();
	static WgEventFilter	CheckboxToggle( const WgWidgetPtr& pWidget );

	static WgEventFilter	AnimationUpdate();
	static WgEventFilter	AnimationUpdate( const WgWidgetPtr& pWidget );

	static WgEventFilter	TabSelect();
	static WgEventFilter	TabSelect( const WgWidgetPtr& pWidget );
	static WgEventFilter	TabSelect( const WgWidgetPtr& pWidget, int id );

	static WgEventFilter	TabPress();
	static WgEventFilter	TabPress( int button );
	static WgEventFilter	TabPress( const WgWidgetPtr& pWidget );
	static WgEventFilter	TabPress( const WgWidgetPtr& pWidget, int button );
	static WgEventFilter	TabPress( const WgWidgetPtr& pWidget, int button, int id );

	static WgEventFilter	EditvalueModify();
	static WgEventFilter	EditvalueModify( const WgWidgetPtr& pWidget );

	static WgEventFilter	EditvalueSet();
	static WgEventFilter	EditvalueSet( const WgWidgetPtr& pWidget );

	static WgEventFilter	ScrollbarMove();
	static WgEventFilter	ScrollbarMove( const WgWidgetPtr& pWidget );

	static WgEventFilter	ScrollbarStepFwd();
	static WgEventFilter	ScrollbarStepFwd( const WgWidgetPtr& pWidget );

	static WgEventFilter	ScrollbarStepBwd();
	static WgEventFilter	ScrollbarStepBwd( const WgWidgetPtr& pWidget );

	static WgEventFilter	ScrollbarJumpFwd();
	static WgEventFilter	ScrollbarJumpFwd( const WgWidgetPtr& pWidget );

	static WgEventFilter	ScrollbarJumpBwd();
	static WgEventFilter	ScrollbarJumpBwd( const WgWidgetPtr& pWidget );

	static WgEventFilter	ScrollbarWheelRoll();
	static WgEventFilter	ScrollbarWheelRoll( const WgWidgetPtr& pWidget );

	static WgEventFilter	TextModify();
	static WgEventFilter	TextModify( const WgWidgetPtr& pWidget );

	static WgEventFilter	TextSet();
	static WgEventFilter	TextSet( const WgWidgetPtr& pWidget );

	static WgEventFilter	MenuitemSelect();
	static WgEventFilter	MenuitemSelect( int itemId );
	static WgEventFilter	MenuitemSelect( const WgWidgetPtr& pWidget );
	static WgEventFilter	MenuitemSelect( const WgWidgetPtr& pWidget, int itemId );

	static WgEventFilter	MenuitemCheck();
	static WgEventFilter	MenuitemCheck( int itemId );
	static WgEventFilter	MenuitemCheck( const WgWidgetPtr& pWidget );
	static WgEventFilter	MenuitemCheck( const WgWidgetPtr& pWidget, int itemId );

	static WgEventFilter	MenuitemUncheck();
	static WgEventFilter	MenuitemUncheck( int itemId );
	static WgEventFilter	MenuitemUncheck( const WgWidgetPtr& pWidget );
	static WgEventFilter	MenuitemUncheck( const WgWidgetPtr& pWidget, int itemId );


	//TODO: Add missing filters!


	static WgEventFilter	KnobTurn();
	static WgEventFilter	KnobTurn( const WgWidgetPtr& pWidget );


	inline bool				FilterEvent( const WgEvent::Event * pEvent ) const { return m_pFilterFunc( pEvent, *this ); }
	inline WgEventType 		EventType() const { return m_eventType; }
	inline bool				FiltersWidget() const { return m_pWidget?true:false; }
	inline WgWidgetWeakPtr	Widget() const { return m_pWidget; }
private:
	WgEventFilter( WgEventType eventType, WgWidget * pWidget, WgFilterFuncPtr pFunc, int data1 = 0, int data2 = 0 )
				: m_eventType(eventType), m_pWidget(pWidget), m_pFilterFunc(pFunc), m_data1(data1), m_data2(data2) 
	{
	}

	WgEventType			m_eventType;		// Only event type filter accepts, if a specific.
	WgWidget *			m_pWidget;			// Only Widget filter accepts, if a specific one.
	WgFilterFuncPtr 	m_pFilterFunc;		// Pointer at filter function.
	int					m_data1;			// Additional data for filter function.
	int					m_data2;			// Additional data for filter function.


	static bool _filterDummy( const WgEvent::Event * pEvent, const WgEventFilter& filter );
	static bool _filterType( const WgEvent::Event * pEvent, const WgEventFilter& filter );
	static bool _filterTypeWidget( const WgEvent::Event * pEvent, const WgEventFilter& filter );

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
