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

	//____ setContentPadding() ____________________________________________________

	void StateSkin::setContentPadding( const BorderI& padding )
	{
		m_contentPadding = padding;
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

	//____ isStateIdentical() ______________________________________________________

	bool StateSkin::isStateIdentical(State state, State comparedTo, float fraction) const
	{
		return (m_contentShift[_stateToIndex(state)] == m_contentShift[_stateToIndex(comparedTo)]);
	}

	//____ minSize() ______________________________________________________________

	Size StateSkin::minSize() const
	{
		return Size(Border(m_contentPadding).aligned());
	}

	//____ _preferredSize() ______________________________________________________________

	Size StateSkin::preferredSize() const
	{
		return Size(Border(m_contentPadding).aligned());
	}

	//____ sizeForContent() _______________________________________________________

	Size StateSkin::sizeForContent( const Size& contentSize ) const
	{
		return contentSize + Size(Border(m_contentPadding).aligned());
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

	//____ contentPaddingSize() _______________________________________________________

	Size StateSkin::contentPaddingSize() const
	{
		return Size(Border(m_contentPadding).aligned());
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
