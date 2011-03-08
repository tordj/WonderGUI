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
	WG_EVENT_POINTER_MOVED,
	WG_EVENT_BUTTON_PRESSED,
	WG_EVENT_BUTTON_RELEASED,
	WG_EVENT_KEY_PRESSED,
	WG_EVENT_KEY_RELEASED,
	WG_EVENT_CHARACTER,
	WG_EVENT_WHEEL_ROLLED,
	WG_EVENT_TIME_PASSED,

	WG_EVENT_END_POINTER_MOVED,
	WG_EVENT_BUTTON_DRAGGED,

	WG_EVENT_POINTER_ENTERED_GIZMO,
	WG_EVENT_POINTER_LEFT_GIZMO
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
				};
			};

			WgEventId		m_id;				// Id of the event
			WgModifierKeys	m_modKeys;			// Modifier keys pressed when event posted.
			int64_t		m_timestamp;		// Timestamp of posting this event
			WgGizmoWeakPtr	m_pGizmo;			// Gizmo posting the event
			Param			m_param[4];
	};

	class PointerMoved : public Event
	{
	public:
		PointerMoved( const WgCord& pos );

		WgCord			Pos() const;
	};

	class ButtonPressed : public Event
	{
	public:
		ButtonPressed( int button );

		int				Button() const;
		WgCord			PointerPos() const;
	};

	class ButtonReleased : public Event
	{
	public:
		ButtonReleased( int button );

		int				Button() const;
		WgCord			PointerPos() const;
	};

	class KeyPressed : public Event
	{
	public:
		KeyPressed( int native_keycode );

		int				NativeKeyCode() const;
		int				TranslatedKeyCode() const;
		WgCord			PointerPos() const;
	};

	class KeyReleased : public Event
	{
	public:
		KeyReleased( int native_keycode );

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

	class WheelRolled : public Event
	{
	public:
		WheelRolled( int wheel, int distance );

		int				Wheel() const;
		int				Distance() const;
		WgCord			PointerPos() const;
	};

	class TimePassed : public Event
	{
	public:
		TimePassed( int ms );

		int				Millisec() const;
	};


	//___ Internally posted events ____________________________________________

	class EndPointerMoved : public Event
	{
		friend class ::WgEventHandler;
	protected:
		EndPointerMoved( const WgCord& pos );
	public:
		WgCord			Pos() const;
	};

	class ButtonDragged : public Event
	{
		friend class ::WgEventHandler;
	protected:
		ButtonDragged( int button, const WgCord& orgPos, const WgCord& prevPos, const WgCord& currPos );
	public:
		int				Button() const;
		WgCord			DraggedSinceStart() const;
		WgCord			DraggedSinceLast() const;
		WgCord			PointerPos() const;
		WgCord			StartPos() const;
		WgCord			PrevPos() const;
	};

	class PointerEnteredGizmo : public Event
	{
		friend class ::WgEventHandler;
	protected:
		PointerEnteredGizmo( WgGizmo * pGizmo ) { m_pGizmo = pGizmo; }
	};

	class PointerLeftGizmo : public Event
	{
		friend class ::WgEventHandler;
	protected:
		PointerLeftGizmo( WgGizmo * pGizmo ) { m_pGizmo = pGizmo; }
	};

}



#endif //WG_EVENT_DOT_H
