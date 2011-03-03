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


enum	WgEventId
{
	WG_EVENT_POINTER_MOVED,
	WG_EVENT_BUTTON_PRESSED,
	WG_EVENT_BUTTON_RELEASED,
	WG_EVENT_KEY_PRESSED,
	WG_EVENT_KEY_RELEASED,
	WG_EVENT_CHARACTER,
	WG_EVENT_WHEEL_ROLLED,
};


namespace WgEvent
{
	class EventBase
	{
		friend class WgEventHandler;

		public:

			inline WgEventId	Id() { return m_id; }
			inline Int64		Timestamp() { return m_timestamp; }
			inline WgGizmo *	Gizmo() { return m_pGizmo; }

		protected:
			EventBase() { m_timestamp = 0; m_pGizmo = 0; };

			struct Param
			{
				union
				{
					bool	boolean;
					int		integer;
					float	real;
					void *	pointer;
				};
			}

			WgEventId	m_id;				// Id of the event
			Int64		m_timestamp;		// Timestamp of posting this event
			WgGizmo *	m_pGizmo;			// Gizmo posting the event
			Param		m_param[4];
	};

	class PointerMoved : public EventBase
	{
	public:
		PointerMoved( const WgCord& pos );

		WgCord			Pos() const { return WgCord( m_param[0].integer, m_param[1].integer ); }
		WgModifierKeys	Modkeys() const { return (WgModifierKeys) m_param[2].integer; }
	}

	class ButtonPressed : public EventBase
	{
	public:
		ButtonPressed( int button );

		int				Button() const { return m_param[0].integer; }
		WgCord			Pos() const { return WgCord( m_param[1].integer, m_param[2].integer ); }
		WgModifierKeys	Modkeys() const { return (WgModifierKeys) m_param[3].integer; }
	}

	class ButtonReleased : public EventBase
	{
	public:
		ButtonReleased( int button );

		int				Button() const { return m_param[0].integer; }
		WgCord			Pos() const { return WgCord( m_param[1].integer, m_param[2].integer ); }
		WgModifierKeys	Modkeys() const { return (WgModifierKeys) m_param[3].integer; }
	}

	class KeyPressed : public EventBase
	{
	public:
		KeyPressed( int native_charcode );

		int				NativeKeyCode() const { return m_param[0].integer; }
		int				TranslatedKeyCode() const { return m_param[1].integer; }
		WgModifierKeys	Modkeys() const { return (WgModifierKeys) m_param[2].integer; }
	}

	class KeyReleased : public EventBase
	{
	public:
		KeyReleased( int native_charcode );

		int				NativeKeyCode() const { return m_param[0].integer; }
		int				TranslatedKeyCode() const { return m_param[1].integer; }
		WgModifierKeys	Modkeys() const { return (WgModifierKeys) m_param[2].integer; }
	}

	class Character : public EventBase
	{
	public:
		Character( unsigned short character );

		unsigned short	Character() const { return (unsigned short) m_param[0].integer; }
	}

	class WheelRolled : public EventBase
	{
	public:
		WheelRolled( int wheel, int distance );

		int				Wheel() const { return m_param[0].integer; }
		int				Distance() const { return m_param[1].integer; }
		WgModifierKeys	Modkeys() const { return (WgModifierKeys) m_param[2].integer; }
	}

	class TimePassed : public EventBase
	{
	public:
		TimePassed( int ms );

		int				Millisec() const { return m_param[0].integer; }
	}

}



#endif //WG_EVENT_DOT_H