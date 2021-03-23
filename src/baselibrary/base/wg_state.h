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

#ifndef	WG_STATE_DOT_H
#define WG_STATE_DOT_H
#pragma once

namespace wg
{


	//___ StateBits ____________________________________________________

	enum class StateBits
	{
		Focused = 0,
		Hovered = 1,
		Pressed = 2,
		Selected = 3,
		Targeted = 4,
		Disabled = 5
	};

	static const int	StateBits_Nb = 6;
	static const int	StateBits_MaxValue = 5;


	static const int	State_Nb	= 18;			// Number of states
	static const int	State_MaxValue	= 40;		// Highest value for StateEnum

	class State
	{
	public:

		//.____ Creation _______________________________________________________

		State() { m_state = 0; }
		State( State state ) { m_state = state.m_state; }

		//.____ State __________________________________________________________

		inline bool	setEnabled(bool bEnabled) { if(bEnabled) m_state &= ~ ((uint8_t)StateEnum::Disabled); else m_state = (m_state & ((uint8_t)StateEnum::Selected)) | ((uint8_t)StateEnum::Disabled); return true; }
		inline bool	setSelected(bool bSelected) { if(bSelected) m_state |= ((uint8_t)StateEnum::Selected); else m_state &= ~((uint8_t)StateEnum::Selected); return true; }
		inline bool	setFocused(bool bFocused) { if( m_state & ((uint8_t)StateEnum::Disabled) ) return false; if(bFocused) m_state |= ((uint8_t)StateEnum::Focused); else m_state &= ~((uint8_t)StateEnum::Focused); return true; }
		inline bool	setHovered(bool bHovered) { if( m_state & ((uint8_t)StateEnum::Disabled) ) return false; if(bHovered) m_state |= ((uint8_t)StateEnum::Hovered); else m_state &= ~(uint8_t(StateEnum::Pressed)|uint8_t(StateEnum::Targeted)); return true; }
		inline bool	setPressed(bool bPressed) { if( m_state & ((uint8_t)StateEnum::Disabled) ) return false; if(bPressed) m_state |= ((uint8_t)StateEnum::Pressed); else m_state &= ~(((uint8_t)StateEnum::Pressed) - ((uint8_t)StateEnum::Hovered)); return true; }
		inline bool	setTargeted(bool bTargeted) { if (m_state & ((uint8_t)StateEnum::Disabled)) return false; if (bTargeted) m_state |= ((uint8_t)StateEnum::Targeted); else m_state &= ~((uint8_t)StateEnum::Targeted); return true; }


		inline bool	isEnabled() const { return (m_state & ((uint8_t)StateEnum::Disabled)) == ((uint8_t)StateEnum::Normal); }
		inline bool	isSelected() const { return (m_state & ((uint8_t)StateEnum::Selected)) == ((uint8_t)StateEnum::Selected); }
		inline bool	isFocused() const { return (m_state & ((uint8_t)StateEnum::Focused)) == ((uint8_t)StateEnum::Focused); }
		inline bool	isHovered() const { return (m_state & ((uint8_t)StateEnum::Hovered)) == ((uint8_t)StateEnum::Hovered); }
		inline bool	isPressed() const { return (m_state & ((uint8_t)StateEnum::Pressed)) == ((uint8_t)StateEnum::Pressed); }
		inline bool	isTargeted() const { return (m_state & ((uint8_t)StateEnum::Targeted)) == ((uint8_t)StateEnum::Targeted); }

		//._____ Operators _____________________________________________________

		inline bool operator==(State state) const { return m_state == state.m_state; }
		inline bool operator!=(State state) const { return m_state != state.m_state; }

		inline State operator=(State state) { m_state = state.m_state); return *this; }

		inline State operator+(State state) const { uint8_t s = m_state | state.m_state; if (s & State::Disabled.m_state) s &= (int) StateEnum::DisabledSelected; return (StateEnum) s; }
		inline State operator-(State _state) const
		{
			int state = (int)_state;
			int hovered = int(StateEnum::Hovered);
			int hoverDependant = (int(StateEnum::Pressed) | int(StateEnum::Targeted)) & ~hovered;

			if ((state & int(StateEnum::Pressed)) == int(StateEnum::Pressed))
				state &= ~hovered;				// Special case: Don't remove hovered just because we remove pressed.
			int s = (m_state & ~state);
			if ((s & hovered) == 0)
				s &= ~hoverDependant;			// If we remove hovered we can't keep a state dependant on it.
			return (StateEnum)s;
		}

		inline State& operator+=(StateEnum state) { m_state |= (uint8_t) state; if (int(m_state) & int(StateEnum::Disabled)) m_state &= int(StateEnum::DisabledSelected); return *this; }
		inline State& operator-=(StateEnum _state)
		{
			int state = (int)_state;
			int hovered = int(StateEnum::Hovered);
			int hoverDependant = (int(StateEnum::Pressed) | int(StateEnum::Targeted)) & ~hovered;

			if ((state & int(StateEnum::Pressed)) == int(StateEnum::Pressed))
				state &= ~hovered;				// Special case: Don't remove hovered just because we remove pressed.
			m_state &= ~state;
			if ((m_state & hovered) == 0)
				m_state &= ~hoverDependant;			// If we remove hovered we can't keep a state dependant on it.
			return *this;
		}

		//.____ Definitions _________________________________________

		const static state Normal					= 0,			///< Element is neither hovered, pressed, selected or focused.
		const static state Focused					= 1,			///< Element has keyboard focus
		const static state Hovered					= 4,			///< Mouse pointer is hovering over element.
		const static state HoveredFocused			= 4+1,
		const static state Pressed					= 4+2,			///< Mouse button (usually left one) is pressed on element.
		const static state PressedFocused			= 4+2+1,
		const static state Selected					= 8,			///< Element is in a selected state, like a selected checkbox or item in a list.
		const static state SelectedFocused			= 8+1,
		const static state SelectedHovered			= 8+4,
		const static state SelectedHoveredFocused	= 8+4+1,
		const static state SelectedPressed			= 8+4+2,
		const static state SelectedPressedFocused	= 8+4+2+1,
		const static state Targeted					= 16+4,
		const static state TargetedFocused			= 16+4+1,
		const static state TargetedSelected			= 16+8+4,
		const static state TargetedSelectedFocused 	= 16+8+4+1,
		const static state Disabled					= 32,			///< Element is disabled and can't be focused or pressed.
		const static state DisabledSelected			= 32+8,



		inline uint8_t mask() { return m_state; }

	private:
		uint8_t		m_state;
		
		const static uint8_t	s_comboFilter[64];
		
	};
}

#endif	//WG_STATE_DOT_H
