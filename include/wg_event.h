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
	WG_EVENT_TIME_PASSED,

	WG_EVENT_END_POINTER_MOVED,
	WG_EVENT_BUTTON_DRAG,
};


namespace WgEvent
{
	class Event
	{
		friend class WgEventHandler;

		public:

			inline WgEventId		Id() const { return m_id; }
			inline Int64			Timestamp() const { return m_timestamp; }
			inline WgGizmo *		Gizmo() const { return m_pGizmo; }
			inline WgModifierKeys	ModKeys() const { return m_modKeys; }  

		protected:
			Event() { m_timestamp = 0; m_pGizmo = 0; };

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
			}

			WgEventId		m_id;				// Id of the event
			WgModifierKeys	m_modKeys;			// Modifier keys pressed when event posted.
			Int64			m_timestamp;		// Timestamp of posting this event
			WgGizmo *		m_pGizmo;			// Gizmo posting the event
			Param			m_param[4];
	};

	class PointerMoved : public Event
	{
	public:
		PointerMoved( const WgCord& pos );

		WgCord			Pos() const;
	}

	class ButtonPressed : public Event
	{
	public:
		ButtonPressed( int button );

		int				Button() const;
		WgCord			PointerPos() const;
	}

	class ButtonReleased : public Event
	{
	public:
		ButtonReleased( int button );

		int				Button() const;
		WgCord			PointerPos() const;
	}

	class KeyPressed : public Event
	{
	public:
		KeyPressed( int native_charcode );

		int				NativeKeyCode() const;
		int				TranslatedKeyCode() const;
		WgCord			PointerPos() const;
	}

	class KeyReleased : public Event
	{
	public:
		KeyReleased( int native_charcode );

		int				NativeKeyCode() const;
		int				TranslatedKeyCode() const;
		WgCord			PointerPos() const;
	}

	class Character : public Event
	{
	public:
		Character( unsigned short character );

		unsigned short	Character() const;
	}

	class WheelRolled : public Event
	{
	public:
		WheelRolled( int wheel, int distance );

		int				Wheel() const;
		int				Distance() const;
		WgCord			PointerPos() const;
	}

	class TimePassed : public Event
	{
	public:
		TimePassed( int ms );

		int				Millisec() const;
	}


	//___ Internally posted events ____________________________________________

	class EndPointerMoved : public Event
	{
		friend class WgEventHandler;
	protected:
		EndPointerMoved( const WgCord& pos );
	public:
		WgCord			Pos() const;
	}

	class ButtonDrag : public Event
	{
		friend class WgEventHandler;
	protected:
		ButtonDrag( int button, const WgCord& orgPos, const WgCord& prevPos, const WgCord& currPos );
	public:
		int				Button() const;
		WgCord			DragSinceStart() const;
		WgCord			DragSinceLast() const;
		WgCord			PointerPos() const;
		WgCord			StartPos() const;
		WgCord			PrevPos() const;
	}


}



#endif //WG_EVENT_DOT_H