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

#include <wg_event.h>

namespace WgEvent
{

	PointerMoved::PointerMoved( const WgCord& pos )
	{
		WgCord	Pos() const { return WgCord( m_param[0].integer, m_param[1].integer ); }
	}

	class ButtonPressed : public EventBase
	{
	public:
		ButtonPressed( int button );

		int		Button() const { return m_param[0].integer; }
	}

	class ButtonReleased : public EventBase
	{
	public:
		ButtonReleased( int button );

		int		Button() const { return m_param[0].integer; }
	}

	class KeyPressed : public EventBase
	{
	public:
		KeyPressed( int native_charcode );

		int	NativeKeyCode() const { return m_param[0].integer; }
		int	TranslatedKeyCode() const { return m_param[1].integer; }
	}

	class KeyReleased : public EventBase
	{
	public:
		KeyReleased( int native_charcode );

		int	NativeKeyCode() const { return m_param[0].integer; }
		int	TranslatedKeyCode() const { return m_param[1].integer; }
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

		int	Wheel() const { return m_param[0].integer; }
		int	Distance() const { return m_param[1].integer; }
	}

	class TimePassed : public EventBase
	{
	public:
		TimePassed( int ms );
