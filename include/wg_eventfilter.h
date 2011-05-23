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

typedef	bool(*WgFilterFuncPtr)(const WgEvent::Event&, const WgEventFilter& );


class WgEventFilter
{
	friend class WgEventHandler;
public:
	static WgEventFilter	PointerEnter();
	static WgEventFilter	PointerEnter( WgGizmo * pGizmo );

	static WgEventFilter	PointerExit();
	static WgEventFilter	PointerExit( WgGizmo * pGizmo );

	static WgEventFilter	PointerMove();
	static WgEventFilter	PointerMove( WgGizmo * pGizmo );

	static WgEventFilter	ButtonPress();
	static WgEventFilter	ButtonPress( int button);
	static WgEventFilter	ButtonPress( WgGizmo * pGizmo );
	static WgEventFilter	ButtonPress( WgGizmo * pGizmo, int button );

	static WgEventFilter	ButtonRelease();
	static WgEventFilter	ButtonRelease( int button);
	static WgEventFilter	ButtonRelease( WgGizmo * pGizmo );
	static WgEventFilter	ButtonRelease( WgGizmo * pGizmo, int button );

	static WgEventFilter	ButtonDrag();
	static WgEventFilter	ButtonDrag( int button);
	static WgEventFilter	ButtonDrag( WgGizmo * pGizmo );
	static WgEventFilter	ButtonDrag( WgGizmo * pGizmo, int button );

	static WgEventFilter	ButtonRepeat();
	static WgEventFilter	ButtonRepeat( int button);
	static WgEventFilter	ButtonRepeat( WgGizmo * pGizmo );
	static WgEventFilter	ButtonRepeat( WgGizmo * pGizmo, int button );

	static WgEventFilter	ButtonClick();
	static WgEventFilter	ButtonClick( int button);
	static WgEventFilter	ButtonClick( WgGizmo * pGizmo );
	static WgEventFilter	ButtonClick( WgGizmo * pGizmo, int button );

	static WgEventFilter	ButtonDoubleClick();
	static WgEventFilter	ButtonDoubleClick( int button);
	static WgEventFilter	ButtonDoubleClick( WgGizmo * pGizmo );
	static WgEventFilter	ButtonDoubleClick( WgGizmo * pGizmo, int button );

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

	static WgEventFilter	WheelRoll();
	static WgEventFilter	WheelRoll( int wheel );
	static WgEventFilter	WheelRoll( WgGizmo * pGizmo );
	static WgEventFilter	WheelRoll( int wheel, WgGizmo * pGizmo );

	static WgEventFilter	Tick();


	inline bool			FilterEvent( const WgEvent::Event& _event ) const { return m_pFilterFunc( _event, *this ); }
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


	static bool _filterDummy( const WgEvent::Event& _event, const WgEventFilter& filter );
	static bool _filterPointerEvents( const WgEvent::Event& _event, const WgEventFilter& filter );
	static bool _filterButtonEvents( const WgEvent::Event& _event, const WgEventFilter& filter );
	static bool _filterKeyEvents( const WgEvent::Event& _event, const WgEventFilter& filter );
	static bool _filterNativeKeyEvents( const WgEvent::Event& _event, const WgEventFilter& filter );
	static bool _filterCharacterEvents( const WgEvent::Event& _event, const WgEventFilter& filter );
	static bool _filterWheelRollEvents( const WgEvent::Event& _event, const WgEventFilter& filter );
	static bool _filterTickEvents( const WgEvent::Event& _event, const WgEventFilter& filter );




};


#endif // WG_EVENTFILTER_DOT_H
