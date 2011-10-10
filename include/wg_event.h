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

#ifndef WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

#ifndef WG_USERDEFINES_DOT_H
#	include <wg_userdefines.h>
#endif

#ifndef WG_GEO_DOT_H
#	include <wg_geo.h>
#endif

#ifndef WG_SMARTPTR_DOT_H
#	include <wg_smartptr.h>
#endif



class WgEventHandler;
class WgGizmo;

typedef class WgWeakPtr<WgGizmo> WgGizmoWeakPtr;

namespace WgEvent
{
	class Event
	{
		friend class ::WgEventHandler;

		public:
			inline WgEventType		Type() const { return m_type; }
			inline int64_t			Timestamp() const { return m_timestamp; }
					WgGizmo *		Gizmo() const;									// Inlining this would demand include of wg_gizmo.h.
			inline WgGizmoWeakPtr	GizmoWeakPtr() const { return m_pGizmo; }
			inline bool				IsForGizmo() const { return m_bIsForGizmo; }
			inline WgModifierKeys	ModKeys() const { return m_modKeys; }
			inline WgCoord			PointerPos() const { return m_pointerLocalPos; }
			inline WgCoord			PointerScreenPos() const { return m_pointerScreenPos; }

		protected:
			Event() : m_type(WG_EVENT_DUMMY), m_modKeys(WG_MODKEY_NONE), m_timestamp(0), m_bIsForGizmo(false) {}
			virtual ~Event() {}

			WgEventType		m_type;				// Type of event
			WgModifierKeys	m_modKeys;			// Modifier keys pressed when event posted.
			int64_t			m_timestamp;		// Timestamp of posting this event
			bool			m_bIsForGizmo;		// Set if this event is for a specific Gizmo (m_pGizmo set at creation, even if weak pointer now is null).
			WgGizmoWeakPtr	m_pGizmo;			// Gizmo to receive this event.
			WgCoord			m_pointerLocalPos;	// Gizmo-relative position of pointer. Same as m_pointerScreenPos if Gizmo not set.
			WgCoord			m_pointerScreenPos;	// Screen position of pointer.
	};

	class ButtonEvent : public Event
	{
		friend class ::WgEventHandler;
	public:
		inline int		Button() const { return m_button; }
	protected:
		ButtonEvent(int button) : m_button(button) {}

		int		m_button;
	};

	class KeyEvent : public Event
	{
		friend class ::WgEventHandler;
	public:
		inline int		NativeKeyCode() const { return m_nativeKeyCode; }
		inline int		TranslatedKeyCode() const { return m_translatedKeyCode; }
	protected:
		KeyEvent( int nativeKeyCode ) : m_nativeKeyCode(nativeKeyCode), m_translatedKeyCode(0) {}

		int		m_nativeKeyCode;
		int		m_translatedKeyCode;
	};

	class FocusGained : public Event
	{
		friend class ::WgEventHandler;
	public:
		FocusGained();
	};

	class FocusLost : public Event
	{
		friend class ::WgEventHandler;
	public:
		FocusLost();
	};

	class PointerEnter : public Event
	{
		friend class ::WgEventHandler;
	public:
		PointerEnter( const WgCoord& pos );
	protected:
		PointerEnter( WgGizmo * pGizmo );
	};

	class PointerExit : public Event
	{
		friend class ::WgEventHandler;
	public:
		PointerExit();
	protected:
		PointerExit( WgGizmo * pGizmo );
	};


	class PointerMove : public Event
	{
		friend class ::WgEventHandler;
	protected:
		PointerMove( WgGizmo * pGizmo );
	public:
		PointerMove( const WgCoord& pos );
	};

	class ButtonPress : public ButtonEvent
	{
		friend class ::WgEventHandler;
	protected:
		ButtonPress( int button, WgGizmo * pGizmo );
	public:
		ButtonPress( int button );
	};

	class ButtonRelease : public ButtonEvent
	{
		friend class ::WgEventHandler;
	public:
		ButtonRelease( int button );

		bool			PressInside() const;
		bool			ReleaseInside() const;

	protected:
		ButtonRelease( int button, WgGizmo * pGizmo, bool bPressInside, bool bReleaseInside );

		bool			m_bPressInside;
		bool			m_bReleaseInside;
	};

	class KeyPress : public KeyEvent
	{
		friend class ::WgEventHandler;
	public:
		KeyPress( int native_keycode );
	protected:
		KeyPress( int native_keycode, WgGizmo * pGizmo );
	};

	class KeyRelease : public KeyEvent
	{
		friend class ::WgEventHandler;
	public:
		KeyRelease( int native_keycode );
	protected:
		KeyRelease( int native_keycode, WgGizmo * pGizmo );
	};

	class Character : public Event
	{
		friend class ::WgEventHandler;
	public:
		Character( unsigned short character );

		unsigned short	Char() const;
	protected:
		Character( unsigned short character, WgGizmo * pGizmo );
	protected:
		unsigned short	m_char;
	};

	class WheelRoll : public Event
	{
		friend class ::WgEventHandler;
	public:
		WheelRoll( int wheel, int distance );

		int			Wheel() const;
		int			Distance() const;
	protected:
		WheelRoll( int wheel, int distance, WgGizmo * pGizmo );

		int			m_wheel;
		int			m_distance;
	};

	class Tick : public Event
	{
		friend class ::WgEventHandler;
	public:
		Tick( int ms );

		int				Millisec() const;
	protected:
		Tick( int ms, WgGizmo * pGizmo );

		int			m_millisec;
	};


	//___ Internally posted events ____________________________________________

	class PointerPlaced : public Event
	{
		friend class ::WgEventHandler;
	protected:
		PointerPlaced();
	};

	class ButtonDrag : public ButtonEvent
	{
		friend class ::WgEventHandler;
	protected:
		ButtonDrag( int button, const WgCoord& startPos, const WgCoord& prevPos, const WgCoord& currPos );
		ButtonDrag( int button, WgGizmo * pGizmo, const WgCoord& orgPos, const WgCoord& prevPos, const WgCoord& currPos );
	public:
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

	class ButtonRepeat : public ButtonEvent
	{
		friend class ::WgEventHandler;
	protected:
		ButtonRepeat( int button, WgGizmo * pGizmo );
	public:
		ButtonRepeat( int button );
	};

	class ButtonClick : public ButtonEvent
	{
		friend class ::WgEventHandler;
	protected:
		ButtonClick( int button );
		ButtonClick( int button, WgGizmo * pGizmo );
	};

	class ButtonDoubleClick : public ButtonEvent
	{
		friend class ::WgEventHandler;
	protected:
		ButtonDoubleClick( int button );
		ButtonDoubleClick( int button, WgGizmo * pGizmo );
	};

	class KeyRepeat : public KeyEvent
	{
		friend class ::WgEventHandler;
	protected:
		KeyRepeat( int native_keycode );
		KeyRepeat( int native_keycode, WgGizmo * pGizmo );
	};


	class PointerMoveOutsideModal : public Event
	{
		friend class ::WgEventHandler;
	protected:
		PointerMoveOutsideModal( WgGizmo * pGizmo );
	};

	class ButtonPressOutsideModal : public ButtonEvent
	{
		friend class ::WgEventHandler;
	protected:
		ButtonPressOutsideModal( int button, WgGizmo * pModalGizmo );
	};

	class ButtonReleaseOutsideModal : public ButtonEvent
	{
		friend class ::WgEventHandler;
	protected:
		ButtonReleaseOutsideModal( int button, WgGizmo * pModalGizmo );

	};

}



#endif //WG_EVENT_DOT_H
