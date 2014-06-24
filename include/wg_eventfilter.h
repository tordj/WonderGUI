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

#ifndef WG_POINTERS_DOT_H
#	include <wg_pointers.h>
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

	static WgEventFilter	MouseLeave();

	static WgEventFilter	MouseMove();

	static WgEventFilter	MousePress();
	static WgEventFilter	MousePress( WgMouseButton button);

	static WgEventFilter	MouseRelease();
	static WgEventFilter	MouseRelease( WgMouseButton button);

	static WgEventFilter	MouseDrag();
	static WgEventFilter	MouseDrag( WgMouseButton button);

	static WgEventFilter	MouseRepeat();
	static WgEventFilter	MouseRepeat( WgMouseButton button);

	static WgEventFilter	MouseClick();
	static WgEventFilter	MouseClick( WgMouseButton button);

	static WgEventFilter	MouseDoubleClick();
	static WgEventFilter	MouseDoubleClick( WgMouseButton button);

	static WgEventFilter	KeyPress();
	static WgEventFilter	KeyPress( WgKey translated_keycode );

	static WgEventFilter	KeyRelease();
	static WgEventFilter	KeyRelease( WgKey translated_keycode );

	static WgEventFilter	KeyRepeat();
	static WgEventFilter	KeyRepeat( WgKey keycode );

	static WgEventFilter	KeyPressNative( int native_keycode );

	static WgEventFilter	KeyReleaseNative( int native_keycode );

	static WgEventFilter	KeyRepeatNative( int native_keycode );

	static WgEventFilter	Character();
	static WgEventFilter	Character( unsigned short character);

	static WgEventFilter	WheelRoll();
	static WgEventFilter	WheelRoll( int wheel );

	static WgEventFilter	Select();

	static WgEventFilter	Toggle();

	static WgEventFilter	ValueUpdate();

	static WgEventFilter	RangeUpdate();

	static WgEventFilter	TextEdit();

	static WgEventFilter	ItemToggle();
	static WgEventFilter	ItemToggle( int id );

	static WgEventFilter	ItemMousePress();
	static WgEventFilter	ItemMousePress( WgMouseButton button );
	static WgEventFilter	ItemMousePress( WgMouseButton button, int id );

	static WgEventFilter	ItemsSelect();

	static WgEventFilter	ItemsUnselect();

	//TODO: Add missing filters!


	inline bool				FilterEvent( const WgEventPtr& pEvent ) const { return m_pFilterFunc( pEvent, *this ); }
	inline WgEventType 		EventType() const { return m_eventType; }
private:
	WgEventFilter( WgEventType eventType, WgFilterFuncPtr pFunc, int data1 = 0, int data2 = 0 )
				: m_eventType(eventType), m_pFilterFunc(pFunc), m_data1(data1), m_data2(data2) 
	{
	}

	WgEventType			m_eventType;		// Only event type filter accepts, if a specific.
	WgFilterFuncPtr 	m_pFilterFunc;		// Pointer at filter function.
	int					m_data1;			// Additional data for filter function.
	int					m_data2;			// Additional data for filter function.


	static bool _filterDummy( const WgEventPtr& pEvent, const WgEventFilter& filter );
	static bool _filterType( const WgEventPtr& pEvent, const WgEventFilter& filter );

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
