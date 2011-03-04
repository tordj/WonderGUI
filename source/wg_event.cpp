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
	//____ PointerMoved __________________________________________________________

	PointerMoved::PointerMoved( const WgCord& pos ) 
	{ 
		m_id = WG_EVENT_POINTER_MOVED; 
		m_param[0].integer = pos.x;	
		m_param[1].integer = pos.y;	
	}

	WgCord PointerMoved::Pos() const 
	{ 
		return WgCord( m_param[0].integer, m_param[1].integer ); 
	}
	
	//____ ButtonPressed ______________________________________________________

	ButtonPressed::ButtonPressed( int button ) 
	{ 
		m_id = WG_EVENT_BUTTON_PRESSED; 
		m_param[0].integer = button; 
	}

	int ButtonPressed::Button() const 
	{ 
		return m_param[0].integer; 
	}
	
	WgCord ButtonPressed::PointerPos() const 
	{ 
		return WgCord( m_param[1].integer, m_param[2].integer ); 
	}
	
	//____ ButtonReleased _____________________________________________________

	ButtonReleased::ButtonReleased( int button ) 
	{ 
		m_id = WG_EVENT_BUTTON_RELEASED; 
		m_param[0].integer = button; 
	}

	int ButtonReleased::Button() const 
	{ 
		return m_param[0].integer; 
	}

	WgCord ButtonReleased::PointerPos() const 
	{ 
		return WgCord( m_param[1].integer, m_param[2].integer ); 
	}
	
	//____ KeyPressed _________________________________________________________

	KeyPressed::KeyPressed( int native_charcode ) 
	{ 
		m_id = WG_EVENT_KEY_PRESSED; 
		m_param[0].integer = native_charcode; 
	}

	int KeyPressed::NativeKeyCode() const 
	{ 
		return m_param[0].integer; 
	}
	
	int KeyPressed::TranslatedKeyCode() const 
	{ 
		return m_param[1].integer; 
	}

	WgCord KeyPressed::PointerPos() const 
	{ 
		return WgCord( m_param[2].integer, m_param[3].integer );
	}

	//____ KeyReleased ________________________________________________________

	KeyReleased::KeyReleased( int native_charcode ) 
	{ 
		m_id = WG_EVENT_KEY_RELEASED; 
		m_param[0].integer = native_keycode; 
	}

	int KeyReleased::NativeKeyCode() const 
	{ 
		return m_param[0].integer; 
	}
	
	int KeyReleased::TranslatedKeyCode() const 
	{ 
		return m_param[1].integer; 
	}

	WgCord KeyReleased::PointerPos() const 
	{ 
		return WgCord( m_param[2].integer, m_param[3].integer );
	}


	//____ Character __________________________________________________________

	Character::Character( unsigned short character ) 
	{ 
		m_id = WG_EVENT_CHARACTER; 
		m_param[0].integer = character; 
	}

	unsigned short Character::Character() const 
	{ 
		return (unsigned short) m_param[0].integer; 
	}

	//____ WheelRolled ________________________________________________________

	WheelRolled::WheelRolled( int wheel, int distance ) 
	{ 
		m_id = WG_EVENT_WHEEL_ROLLED; 
		m_param[0].integer = wheel; 
		m_param[1].integer = distance; 
	}

	int WheelRolled::Wheel() const 
	{ 
		return m_param[0].integer; 
	}
	
	int WheelRolled::Distance() const 
	{ 
		return m_param[1].integer; 
	}
	
	WgCord WheelRolled::PointerPos() const 
	{ 
		return WgCord( m_param[2].integer, m_param[3].integer );
	}

	//____ TimePassed _________________________________________________________

	TimePassed::TimePassed( int ms ) 
	{ 
		m_id = WG_EVENT_TIME_PASSED; 
		m_param[0].integer = ms; 
	}

	int TimePassed::Millisec() const 
	{ 
		return m_param[0].integer; 
	}

	//____ EndPointerMoved _______________________________________________________

	EndPointerMoved::EndPointerMoved( const WgCord& pos ) 
	{ 
		m_id = WG_EVENT_END_POINTER_MOVED; 
		m_param[0].integer = pos.x;	
		m_param[1].integer = pos.y;	
	}

	WgCord EndPointerMoved::Pos() const 
	{ 
		return WgCord( m_param[0].integer, m_param[1].integer ); 
	}

	//____ ButtonDrag _________________________________________________________

	ButtonDrag::ButtonDrag( int button, const WgCord& orgPos, const WgCord& prevPos, const WgCord& currPos ) 
	{ 

		m_id = WG_EVENT_BUTTON_DRAG; 
		m_param[0].integer = button;

		m_param[1].short1 = orgPos.x;
		m_param[1].short2 = orgPos.y;

		m_param[2].short1 = currPos.x - orgPos.x;
		m_param[2].short2 = currPos.y - orgPos.y;

		m_param[3].short1 = currPos.x - prevPos.x;
		m_param[3].short2 = currPos.y - prevPos.y;
	}


	WgCord ButtonDrag::DragSinceStart() const
	{
		return WgCord( m_param[2].short1, m_param[2].short2 ) );
	}

	WgCord ButtonDrag::DragSinceLast() const
	{
		return WgCord( m_param[3].short1, m_param[3].short2 ) );
	}

	WgCord ButtonDrag::PointerPos() const;
	{
		return WgCord( m_param[1].short1 + m_param[2].short1, m_param[1].short2 + m_param[2].short2 ) );
	}

	WgCord ButtonDrag::StartPos() const
	{
		return WgCord( m_param[1].short1, m_param[1].short2 ) );
	}

	WgCord ButtonDrag::PrevPos() const
	{
		return WgCord( m_param[1].short1 + m_param[2].short1 - m_param[3].short1, m_param[1].short2 + m_param[2].short2 - m_param[3].short2 ) );
	}

};