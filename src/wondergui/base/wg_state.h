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

#include <wg_types.h>

namespace wg
{


	class State
	{
	public:

		//.____ Creation _______________________________________________________

		State() { m_state = 0; }
		State(int index) { m_state = s_indexToStateTable[index]; }

		//.____ State __________________________________________________________

		inline bool	setDisabled(bool bDisabled) { if (!bDisabled) m_state &= ~Disabled.m_state; else m_state = (m_state & Selected.m_state) | Disabled.m_state; return true; }
		inline bool	setSelected(bool bSelected) { if (bSelected) m_state |= Selected.m_state; else m_state &= ~Selected.m_state; return true; }
		inline bool	setFocused(bool bFocused) { if (m_state & Disabled.m_state) return false; if (bFocused) m_state |= Focused.m_state; else m_state &= ~Focused.m_state; return true; }
		inline bool	setHovered(bool bHovered) { if (m_state & Disabled.m_state) return false; if (bHovered) m_state |= Hovered.m_state; else m_state &= ~(Pressed.m_state | Targeted.m_state); return true; }
		inline bool	setPressed(bool bPressed) { if (m_state & Disabled.m_state) return false; if (bPressed) m_state |= Pressed.m_state; else m_state &= ~(Pressed.m_state - Hovered.m_state); return true; }
		inline bool	setTargeted(bool bTargeted) { if (m_state & Disabled.m_state) return false; if (bTargeted) m_state |= Targeted.m_state; else m_state &= ~Targeted.m_state; return true; }


		inline bool	isDisabled() const { return (m_state & Disabled.m_state) == Disabled.m_state; }
		inline bool	isSelected() const { return (m_state & Selected.m_state) == Selected.m_state; }
		inline bool	isFocused() const { return (m_state & Focused.m_state) == Focused.m_state; }
		inline bool	isHovered() const { return (m_state & Hovered.m_state) == Hovered.m_state; }
		inline bool	isPressed() const { return (m_state & Pressed.m_state) == Pressed.m_state; }
		inline bool	isTargeted() const { return (m_state & Targeted.m_state) == Targeted.m_state; }

		//._____ Operators _____________________________________________________

		inline bool operator==(State state) const { return m_state == state.m_state; }
		inline bool operator!=(State state) const { return m_state != state.m_state; }

		inline operator int() const { return s_stateToIndexTable[m_state]; }

		inline State operator+(State state) const
		{
			int s = m_state | state.m_state;
			if (s & Disabled.m_state)
				s &= DisabledSelected.m_state;

			State st;
			st.m_state = s;
			return st;
		}
		
		inline State operator-(State _state) const
		{
			int state = _state.m_state;
			int hovered = Hovered.m_state;
			int hoverDependant = (Pressed.m_state | Targeted.m_state) & ~hovered;

			if ((state & Pressed.m_state) == Pressed.m_state)
				state &= ~hovered;				// Special case: Don't remove hovered just because we remove pressed.
			int s = (m_state & ~state);
			if ((s & hovered) == 0)
				s &= ~hoverDependant;			// If we remove hovered we can't keep a state dependant on it.

			State st;
			st.m_state = s;
			return st;
		}

		inline State& operator+=(State state)
		{
			m_state |= state.m_state;
			if (m_state & Disabled.m_state)
				m_state &= DisabledSelected.m_state;
			return *this;
		}
		
		inline State& operator-=(State _state)
		{
			int state = _state.m_state;
			int hovered = Hovered.m_state;
			int hoverDependant = (Pressed.m_state | Targeted.m_state) & ~hovered;

			if ((state & Pressed.m_state) == Pressed.m_state)
				state &= ~hovered;				// Special case: Don't remove hovered just because we remove pressed.
			m_state &= ~state;
			if ((m_state & hovered) == 0)
				m_state &= ~hoverDependant;			// If we remove hovered we can't keep a state dependant on it.
			return *this;
		}

		//.____ Misc ________________________________________________

		inline uint8_t mask() { return m_state; }

		static const int	IndexAmount = 18;			// Number of states
		static const int	MaxValue = 40;				// Highest value for m_state


		//.____ Definitions _________________________________________

		const static State Default;						///< Element is neither hovered, pressed, selected or focused.
		const static State Focused;						///< Element has keyboard focus
		const static State Hovered;						///< Mouse pointer is hovering over element.
		const static State HoveredFocused;
		const static State Pressed;						///< Mouse button (usually left one) is pressed on element.
		const static State PressedFocused;
		const static State Selected;					///< Element is in a selected state, like a selected checkbox or item in a list.
		const static State SelectedFocused;
		const static State SelectedHovered;
		const static State SelectedHoveredFocused;
		const static State SelectedPressed;
		const static State SelectedPressedFocused;
		const static State Targeted;
		const static State TargetedFocused;
		const static State TargetedSelected;
		const static State TargetedSelectedFocused;
		const static State Disabled;					///< Element is disabled and can't be focused or pressed.
		const static State DisabledSelected;


	private:

		const static int		s_stateToIndexTable[MaxValue + 1];
		const static uint8_t	s_indexToStateTable[IndexAmount];

		uint8_t		m_state;
	};


}

#endif	//WG_STATE_DOT_H
