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
			inline WgCord			PointerPos() const { return m_pointerLocalPos; }
			inline WgCord			PointerScreenPos() const { return m_pointerScreenPos; }

		protected:
			Event() : m_type(WG_EVENT_DUMMY), m_modKeys(WG_MODKEY_NONE), m_timestamp(0), m_bIsForGizmo(false) {}

			struct Param
			{
				union
				{
					bool	boolean;
					int		integer;
					float	real;
					void *	pointer;

					struct
					{
						short	short1;
						short	short2;
					};
				};
			};

			WgEventType		m_type;				// Type of event
			WgModifierKeys	m_modKeys;			// Modifier keys pressed when event posted.
			int64_t			m_timestamp;		// Timestamp of posting this event
			bool			m_bIsForGizmo;		// Set if this event is for a specific Gizmo (m_pGizmo set at creation, even if weak pointer now is null).
			WgGizmoWeakPtr	m_pGizmo;			// Gizmo to receive this event.
			WgCord			m_pointerLocalPos;	// Gizmo-relative position of pointer. Same as m_pointerScreenPos if Gizmo not set.
			WgCord			m_pointerScreenPos;	// Screen position of pointer.
			Param			m_param[4];
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
		PointerEnter( const WgCord& pos );
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
		PointerMove( const WgCord& pos );
	};

	class ButtonPress : public Event
	{
		friend class ::WgEventHandler;
	protected:
		ButtonPress() {}								// So we can make members in WgEventHandler
		ButtonPress( int button, WgGizmo * pGizmo );
	public:
		ButtonPress( int button );

		int				Button() const;
	};

	class ButtonRelease : public Event
	{
		friend class ::WgEventHandler;
	protected:
		ButtonRelease() {}						// So we can make members in WgEventHandler
		ButtonRelease( int button, WgGizmo * pGizmo, bool bPressInside, bool bReleaseInside );
	public:
		ButtonRelease( int button );

		int				Button() const;
		bool			PressInside() const;
		bool			ReleaseInside() const;
	};

	class KeyPress : public Event
	{
	public:
		KeyPress( int native_keycode );

		int				NativeKeyCode() const;
		int				TranslatedKeyCode() const;
	};

	class KeyRelease : public Event
	{
	public:
		KeyRelease( int native_keycode );

		int				NativeKeyCode() const;
		int				TranslatedKeyCode() const;
	};

	class Character : public Event
	{
	public:
		Character( unsigned short character );

		unsigned short	Char() const;
	};

	class WheelRoll : public Event
	{
	public:
		WheelRoll( int wheel, int distance );

		int				Wheel() const;
		int				Distance() const;
	};

	class Tick : public Event
	{
	public:
		Tick( int ms );

		int				Millisec() const;
	};


	//___ Internally posted events ____________________________________________

	class PointerPlaced : public Event
	{
		friend class ::WgEventHandler;
	protected:
		PointerPlaced();
	};

	class ButtonDrag : public Event
	{
		friend class ::WgEventHandler;
	protected:
		ButtonDrag( int button, const WgCord& orgPos, const WgCord& prevPos, const WgCord& currPos );
		ButtonDrag( int button, WgGizmo * pGizmo, const WgCord& orgPos, const WgCord& prevPos, const WgCord& currPos );
	public:
		int				Button() const;
		WgCord			DraggedSinceStart() const;
		WgCord			DraggedSinceLast() const;
		WgCord			StartPos() const;
		WgCord			PrevPos() const;
		WgCord			CurrPos() const;
	};

	class ButtonRepeat : public Event
	{
		friend class ::WgEventHandler;
	protected:
		ButtonRepeat() {}								// So we can make members in WgEventHandler
		ButtonRepeat( int button, WgGizmo * pGizmo );
	public:
		ButtonRepeat( int button );

		int				Button() const;
	};

	class ButtonClick : public Event
	{
		friend class ::WgEventHandler;
	protected:
		ButtonClick( int button );
		ButtonClick( int button, WgGizmo * pGizmo );
	public:
		int				Button() const;
	};

	class ButtonDoubleClick : public Event
	{
		friend class ::WgEventHandler;
	protected:
		ButtonDoubleClick( int button );
		ButtonDoubleClick( int button, WgGizmo * pGizmo );
	public:
		int				Button() const;
	};

	class KeyRepeat : public Event
	{
		friend class ::WgEventHandler;
	protected:
		KeyRepeat( int native_keycode );
	public:
		int				NativeKeyCode() const;
		int				TranslatedKeyCode() const;
	};

}



#endif //WG_EVENT_DOT_H
