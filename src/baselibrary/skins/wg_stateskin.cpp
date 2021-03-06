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

#include <wg_stateskin.h>
#include <wg_util.h>

namespace wg
{

	using namespace Util;

	const TypeInfo StateSkin::TYPEINFO = { "StateSkin", &Skin::TYPEINFO };


	//____ typeInfo() _________________________________________________________

	const TypeInfo& StateSkin::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ clearContentShift() ________________________________________________

	void StateSkin::clearContentShift()
	{
		m_contentShiftStateMask = 1;			// Mode normal is set by default

		for( int i = 0 ; i < StateEnum_Nb ; i++ )
			m_contentShift[i] = { 0,0 };

		m_bContentShifting = false;
	}

	//____ setContentShift() ______________________________________________________

	void StateSkin::setContentShift(State state, CoordI shift)
	{
		int index = _stateToIndex(state);
		m_contentShift[index] = shift;
		m_contentShiftStateMask.setBit(index);

		m_bContentShifting = true;				// Making it easy for us, just assume something will be shifting when this method is called.

		_refreshUnsetStates();
	}

	//____ setContentShift() _____________________________________________________

	void StateSkin::setContentShift(std::initializer_list< std::pair<State, CoordI> > stateShifts)
	{
		for (auto& shift : stateShifts)
		{
			int index = _stateToIndex(shift.first);
			m_contentShift[index] = shift.second;
			m_contentShiftStateMask.setBit(index);
		}

		m_bContentShifting = true;				// Making it easy for us, just assume something will be shifting when this method is called.

		_refreshUnsetStates();
	}

	//____ contentShift() ________________________________________________

	CoordI StateSkin::contentShift(State state) const
	{
		int index = _stateToIndex(state);
		if (m_contentShiftStateMask.bit(index))
			return m_contentShift[index];

		return CoordI();
	}

	//____ contentPadding() _______________________________________________________

	Border StateSkin::contentPadding(State state) const
	{
		Border b = Border(m_contentPadding).aligned();
		Coord ofs = Coord(m_contentShift[_stateToIndex(state)]).aligned();

		b.left += ofs.x;
		b.top += ofs.y;

		return b;
	}

	//____ contentRect() __________________________________________________________

	Rect StateSkin::contentRect( const Rect& canvas, State state ) const
	{
		return (canvas - Border(m_contentPadding).aligned()) + Coord(m_contentShift[_stateToIndex(state)]).aligned();
	}

	//____ contentofs() __________________________________________________________

	Coord StateSkin::contentOfs( State state ) const
	{
		return Coord( m_contentPadding.left, m_contentPadding.top).aligned() + Coord(m_contentShift[_stateToIndex(state)]).aligned();
	}

	//____ dirtyRect() ______________________________________________________

	Rect StateSkin::dirtyRect(const Rect& canvas, State newState, State oldState, float newValue, float oldValue,
		float newValue2, float oldValue2, int newAnimPos, int oldAnimPos,
		float* pNewStateFractions, float* pOldStateFractions) const
	{
		if (m_contentShift[_stateToIndex(newState)] == m_contentShift[_stateToIndex(oldState)])
			return Rect();
		else
			return canvas;
	}

	//____ _refreshUnsetStates() _________________________________________________

	void StateSkin::_refreshUnsetStates()
	{
		for (int i = 0; i < StateEnum_Nb; i++)
		{
			if ( !m_contentShiftStateMask.bit(i) )
			{
				int bestAlternative = bestStateIndexMatch(i, m_contentShiftStateMask);
				m_contentShift[i] = m_contentShift[bestAlternative];
			}
		}
	}

} // namespace wg
