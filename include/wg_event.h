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

#ifndef WG_GIZMO_DOT_H
#	include <wg_gizmo.h>
#endif


class WgEventHandler;


enum	WgEventId
{
	WG_EVENT_DUMMY,
	WG_EVENT_POINTER_MOVE,
	WG_EVENT_BUTTON_PRESS,
	WG_EVENT_BUTTON_RELEASE,
	WG_EVENT_KEY_PRESS,
	WG_EVENT_KEY_RELEASE,
	WG_EVENT_CHARACTER,
	WG_EVENT_WHEEL_ROLL,
	WG_EVENT_TIME_PASS,

	WG_EVENT_END_POINTER_MOVE,
	WG_EVENT_BUTTON_DRAG,

	WG_EVENT_POINTER_ENTER_GIZMO,
	WG_EVENT_POINTER_EXIT_GIZMO,

	WG_EVENT_GIZMO_PRESS,
	WG_EVENT_GIZMO_RELEASE,

	WG_EVENT_BUTTON_CLICK,
	WG_EVENT_BUTTON_DOUBLECLICK,

};



namespace WgEvent
{
	class Event
	{
		friend class ::WgEventHandler;

		public:

			inline WgEventId		Id() const { return m_id; }
			inline int64_t		Timestamp() const { return m_timestamp; }
			inline WgGizmo *		Gizmo() const { return m_pGizmo.GetRealPtr(); }
			inline WgModifierKeys	ModKeys() const { return m_modKeys; }

		protected:
			Event() : m_id(WG_EVENT_DUMMY), m_modKeys(WG_MODKEY_NONE), m_timestamp(0), m_pGizmo(0) {}

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

					struct
					{
						char	byte1;
						char	byte2;
						char	byte3;
						char	byte4;
					};
				};
			};

			WgEventId		m_id;				// Id of the event
			WgModifierKeys	m_modKeys;			// Modifier keys pressed when event posted.
			int64_t			m_timestamp;		// Timestamp of posting this event
			WgGizmoWeakPtr	m_pGizmo;			// Gizmo posting the event
			Param			m_param[4];
	};

	class PointerMove : public Event
	{
	public:
		PointerMove( const WgCord& pos );

		WgCord			Pos() const;
	};

	class ButtonPress : public Event
	{
	public:
		ButtonPress( int button );

		int				Button() const;
		WgCord			PointerPos() const;
	};

	class ButtonRelease : public Event
	{
	public:
		ButtonRelease( int button );

		int				Button() const;
		WgCord			PointerPos() const;
	};

	class KeyPress : public Event
	{
	public:
		KeyPress( int native_keycode );

		int				NativeKeyCode() const;
		int				TranslatedKeyCode() const;
		WgCord			PointerPos() const;
	};

	class KeyRelease : public Event
	{
	public:
		KeyRelease( int native_keycode );

		int				NativeKeyCode() const;
		int				TranslatedKeyCode() const;
		WgCord			PointerPos() const;
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
		WgCord			PointerPos() const;
	};

	class TimePass : public Event
	{
	public:
		TimePass( int ms );

		int				Millisec() const;
	};


	//___ Internally posted events ____________________________________________

	class EndPointerMove : public Event
	{
		friend class ::WgEventHandler;
	protected:
		EndPointerMove( const WgCord& pos );
	public:
		WgCord			Pos() const;
	};

	class ButtonDrag : public Event
	{
		friend class ::WgEventHandler;
	protected:
		ButtonDrag( int button, const WgCord& orgPos, const WgCord& prevPos, const WgCord& currPos );
	public:
		int				Button() const;
		WgCord			DraggedSinceStart() const;
		WgCord			DraggedSinceLast() const;
		WgCord			PointerPos() const;
		WgCord			StartPos() const;
		WgCord			PrevPos() const;
	};

	class ButtonClick : public Event
	{
		friend class ::WgEventHandler;
	protected:
		ButtonClick( int button );

		int				Button() const;
		WgCord			PointerPos() const;
	}

	class ButtonDoubleClick : public Event
	{
		friend class ::WgEventHandler;
	protected:
		ButtonDoubleClick( int button );

		int				Button() const;
		WgCord			PointerPos() const;
	}

	class PointerEnterGizmo : public Event
	{
		friend class ::WgEventHandler;
	protected:
		PointerEnterGizmo( WgGizmo * pGizmo ) { m_pGizmo = pGizmo; }
	};

	class PointerExitGizmo : public Event
	{
		friend class ::WgEventHandler;
	protected:
		PointerExitGizmo( WgGizmo * pGizmo ) { m_pGizmo = pGizmo; }
	};

	class GizmoPress : public Event
	{
		friend class ::WgEventHandler;
	protected:
		GizmoPress( WgGizmo * pGizmo, int button, WgCord& screenPos, WgCord& ofs );

		int				Button() const;
		WgCord			PointerOfs() const;
		WgCord			PointerScreenPos() const;
	};

	class GizmoRelease : public Event
	{
		friend class ::WgEventHandler;
	protected:
		GizmoRelease( WgGizmo * pGizmo, int button, WgCord& screenPos, WgCord& ofs, bool bWasPressed, bool bIsOutside );

		int				Button() const;
		WgCord			PointerOfs() const;
		WgCord			PointerScreenPos() const;
		bool			WasPressed() const;
		bool			IsOutside() const;
	};


}



#endif //WG_EVENT_DOT_H
