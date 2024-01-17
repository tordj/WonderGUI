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

		State() { m_bitmask = 0; }
		State(int index) { m_bitmask = s_indexToBitmask[index]; }

		//.____ State __________________________________________________________

		inline bool	setDisabled(bool bDisabled) 
		{ 
			if (!bDisabled) 
				m_isDisabled = 0; 
			else
			{
				auto sel = m_isSelected;
				m_bitmask = 0;
				m_isSelected = sel;
				m_isDisabled = 1;
			}
			return true;
		}

		inline bool	setSelected(bool bSelected) 
		{ 
			m_isSelected = bSelected;
			return true;
		}

		inline bool	setFocused(bool bFocused) 
		{ 
			if (m_isDisabled) 
				return false; 

			m_isFocused = bFocused;
			return true; 
		}

		inline bool	setHovered(bool bHovered) 
		{ 
			if (m_isDisabled)
				return false;

			if (!bHovered)
			{
				m_isPressed = 0;
				m_isTargeted = 0;
			}

			m_isHovered = bHovered;
			return true;
		}

		inline bool	setPressed(bool bPressed) 
		{ 
			if (m_isDisabled)
				return false;

			if (bPressed)
				m_isHovered = 1;

			m_isPressed = bPressed;
			return true; 
		}

		inline bool	setTargeted(bool bTargeted) 
		{ 
			if (m_isDisabled)
				return false;

			m_isTargeted = bTargeted;
			return true; 
		}


		inline bool	isDisabled() const { return m_isDisabled; }
		inline bool	isSelected() const { return m_isSelected; }
		inline bool	isFocused() const { return m_isFocused; }
		inline bool	isHovered() const { return m_isHovered; }
		inline bool	isPressed() const { return m_isPressed; }
		inline bool	isTargeted() const { return m_isTargeted; }

		//._____ Operators _____________________________________________________

		inline bool operator==(State state) const { return m_bitmask == state.m_bitmask; }
		inline bool operator!=(State state) const { return m_bitmask != state.m_bitmask; }

		inline operator int() const { return s_bitmaskToIndex[m_bitmask]; }

		inline State operator+(State state) const
		{
			State s;
			s.m_bitmask = m_bitmask | state.m_bitmask;

			if ( s.m_isDisabled)
			{
				auto sel = s.m_isSelected;
				s.m_bitmask = 0;
				s.m_isSelected = sel;
				s.m_isDisabled = 1;
			}
			return s;
		}
		
		inline State operator-(State state) const
		{
			State s;
			if (state.m_isPressed)
				state.m_isHovered = 0;				// Special case: Don't remove hovered just because we remove pressed.
			s.m_bitmask = m_bitmask & ~state.m_bitmask;
			if (!s.m_isHovered)
			{
				// If we remove hovered we can't keep a state dependant on it.
				s.m_isPressed = 0;
				s.m_isTargeted = 0;
			}
			return s;
		}

		inline State& operator+=(State state)
		{
			m_bitmask |= state.m_bitmask;
			if (m_isDisabled)
			{
				auto sel = m_isSelected;
				m_bitmask = 0;
				m_isSelected = sel;
				m_isDisabled = 1;
			}
			return *this;
		}
		
		inline State& operator-=(State state)
		{
			if (state.m_isPressed)
				state.m_isHovered = 0;				// Special case: Don't remove hovered just because we remove pressed.
			m_bitmask &= ~state.m_bitmask;
			if (!m_isHovered)
			{
				// If we remove hovered we can't keep a state dependent on it.
				m_isPressed = 0;
				m_isTargeted = 0;
			}
			return *this;
		}

		//.____ Misc ________________________________________________

		inline uint8_t bitmask() const { return m_bitmask; }


		//.____ Definitions _________________________________________

		static const State Default;
		static const State Focused;
		static const State Hovered;
		static const State HoveredFocused;
		static const State Pressed;
		static const State PressedFocused;
		static const State Selected;
		static const State SelectedFocused;
		static const State SelectedHovered;
		static const State SelectedHoveredFocused;
		static const State SelectedPressed;
		static const State SelectedPressedFocused;
		static const State Targeted;
		static const State TargetedFocused;
		static const State TargetedSelected;
		static const State TargetedSelectedFocused;
		static const State Disabled;
		static const State DisabledSelected;

		static const int	IndexAmount = 18;			// Number of states
		static const int	MaxValue = 40;				// Highest value for m_bitmask

	private:

		const static int		s_bitmaskToIndex[MaxValue + 1];
		const static uint8_t	s_indexToBitmask[IndexAmount];

		union 
		{
			struct
			{
				uint8_t	m_isFocused : 1;
				uint8_t m_isPressed : 1;
				uint8_t	m_isHovered : 1;
				uint8_t m_isSelected : 1;
				uint8_t m_isTargeted : 1;
				uint8_t m_isDisabled : 1;

			};
				uint8_t	m_bitmask;
		};

	};


}

#endif	//WG_STATE_DOT_H
