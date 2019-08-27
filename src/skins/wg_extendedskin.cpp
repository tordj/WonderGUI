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

#include <wg_extendedskin.h>
#include <wg_util.h>

namespace wg
{

	using namespace Util;

	const char ExtendedSkin::CLASSNAME[] = {"ExtendedSkin"};


	//____ isInstanceOf() _________________________________________________________

	bool ExtendedSkin::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return Skin::isInstanceOf(pClassName);
	}

	//____ className() ____________________________________________________________

	const char * ExtendedSkin::className( void ) const
	{
		return CLASSNAME;
	}

	//____ cast() _________________________________________________________________

	ExtendedSkin_p ExtendedSkin::cast( Object * pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return ExtendedSkin_p( static_cast<ExtendedSkin*>(pObject) );

		return 0;
	}


	//____ setContentPadding() ____________________________________________________

	void ExtendedSkin::setContentPadding( BorderI padding )
	{
		m_contentPadding = padding;
	}

	Size ExtendedSkin::contentPaddingAdapted() const
	{
		return Util::rawToQpix(_contentPadding());
	}


	//____ clearContentShift() ________________________________________________

	void ExtendedSkin::clearContentShift()
	{
		m_contentShiftStateMask = 1;			// Mode normal is set by default

		for( int i = 0 ; i < StateEnum_Nb ; i++ )
			m_contentShift[i] = { 0,0 };
	}

	//____ setContentShift() ______________________________________________________

	void ExtendedSkin::setContentShift(State state, CoordI shift)
	{
		int index = _stateToIndex(state);
		m_contentShift[index] = shift;
		m_contentShiftStateMask.setBit(index);

		_refreshUnsetStates();
	}

	//____ setContentShift() _____________________________________________________

	void ExtendedSkin::setContentShift(std::initializer_list< std::pair<State, CoordI> > stateShifts)
	{
		for (auto& shift : stateShifts)
		{
			int index = _stateToIndex(shift.first);
			m_contentShift[index] = shift.second;
			m_contentShiftStateMask.setBit(index);
		}

		_refreshUnsetStates();
	}

	//____ contentShift() ________________________________________________

	CoordI ExtendedSkin::contentShift(State state) const
	{
		int index = _stateToIndex(state);
		if (m_contentShiftStateMask.bit(index))
			return m_contentShift[index];

		return CoordI();
	}

	//____ contentShiftAdapted() ________________________________________________________

	Coord ExtendedSkin::contentShiftAdapted(State state) const
	{
		int index = _stateToIndex(state);
		return pixelAligned(Coord(m_contentShift[index]));
	}

	//____ isStateIdentical() ______________________________________________________

	bool ExtendedSkin::isStateIdentical(State state, State comparedTo) const
	{
		return (m_contentShift[_stateToIndex(state)] == m_contentShift[_stateToIndex(comparedTo)]);
	}

	//____ _minSize() ______________________________________________________________

	SizeI ExtendedSkin::_minSize() const
	{
		return pointsToRawAligned(m_contentPadding);
	}

	//____ _preferredSize() ______________________________________________________________

	SizeI ExtendedSkin::_preferredSize() const
	{
		return pointsToRawAligned(m_contentPadding);
	}

	//____ _sizeForContent() _______________________________________________________

	SizeI ExtendedSkin::_sizeForContent( const SizeI contentSize ) const
	{
		return contentSize + pointsToRawAligned(m_contentPadding);
	}

	//____ _contentPadding() _______________________________________________________

	SizeI ExtendedSkin::_contentPadding() const
	{
		return pointsToRawAligned(m_contentPadding);
	}


	//____ _contentRect() __________________________________________________________

	RectI ExtendedSkin::_contentRect( const RectI& canvas, State state ) const
	{
		return (canvas - pointsToRawAligned(m_contentPadding)) + pointsToRawAligned(m_contentShift[_stateToIndex(state)]);
	}

	//____ _contentofs() __________________________________________________________

	CoordI ExtendedSkin::_contentOfs( State state ) const
	{
		return pointsToRawAligned(CoordI( m_contentPadding.left, m_contentPadding.top)) + pointsToRawAligned(m_contentShift[_stateToIndex(state)]);
	}

	//____ _refreshUnsetStates() _________________________________________________

	void ExtendedSkin::_refreshUnsetStates()
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
