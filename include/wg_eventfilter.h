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


class WgEventFilter;

typedef	bool(*WgFilterFuncPtr)(const WgEventPtr&, const WgEventFilter& );


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

	static WgEventFilter	WheelRoll();
	static WgEventFilter	WheelRoll( int wheel );
	static WgEventFilter	WheelRoll( const WgWidgetPtr& pWidget );
	static WgEventFilter	WheelRoll( const WgWidgetPtr& pWidget, int wheel );

	static WgEventFilter	Select();
	static WgEventFilter	Select( const WgWidgetPtr& pWidget );

	static WgEventFilter	Toggle();
	static WgEventFilter	Toggle( const WgWidgetPtr& pWidget );

	static WgEventFilter	ValueUpdate();
	static WgEventFilter	ValueUpdate( const WgWidgetPtr& pWidget );

	static WgEventFilter	RangeUpdate();
	static WgEventFilter	RangeUpdate( const WgWidgetPtr& pWidget );

	static WgEventFilter	TextEdit();
	static WgEventFilter	TextEdit( const WgWidgetPtr& pWidget );

	static WgEventFilter	ItemToggle();
	static WgEventFilter	ItemToggle( const WgWidgetPtr& pWidget );
	static WgEventFilter	ItemToggle( const WgWidgetPtr& pWidget, int id );

	static WgEventFilter	ItemMousePress();
	static WgEventFilter	ItemMousePress( int button );
	static WgEventFilter	ItemMousePress( const WgWidgetPtr& pWidget );
	static WgEventFilter	ItemMousePress( const WgWidgetPtr& pWidget, int button );
	static WgEventFilter	ItemMousePress( const WgWidgetPtr& pWidget, int button, int id );

	static WgEventFilter	ItemsSelect();
	static WgEventFilter	ItemsSelect( const WgWidgetPtr& pWidget );

	static WgEventFilter	ItemsUnselect();
	static WgEventFilter	ItemsUnselect( const WgWidgetPtr& pWidget );


	//TODO: Add missing filters!



	inline bool				FilterEvent( const WgEventPtr& pEvent ) const { return m_pFilterFunc( pEvent, *this ); }
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


	static bool _filterDummy( const WgEventPtr& pEvent, const WgEventFilter& filter );
	static bool _filterType( const WgEventPtr& pEvent, const WgEventFilter& filter );
	static bool _filterTypeWidget( const WgEventPtr& pEvent, const WgEventFilter& filter );

	static bool _filterPointerEvents( const WgEventPtr& pEvent, const WgEventFilter& filter );
	static bool _filterMouseButtonEvents( const WgEventPtr& pEvent, const WgEventFilter& filter );
	static bool _filterKeyEvents( const WgEventPtr& pEvent, const WgEventFilter& filter );
	static bool _filterNativeKeyEvents( const WgEventPtr& pEvent, const WgEventFilter& filter );
	static bool _filterCharacterEvents( const WgEventPtr& pEvent, const WgEventFilter& filter );
	static bool _filterWheelRollEvents( const WgEventPtr& pEvent, const WgEventFilter& filter );
	static bool _filterItemToggleEvents( const WgEventPtr& pEvent, const WgEventFilter& filter );
	static bool _filterItemMousePressEvents( const WgEventPtr& pEvent, const WgEventFilter& filter );
};


#endif // WG_EVENTFILTER_DOT_H
