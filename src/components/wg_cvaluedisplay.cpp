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

#include <wg_widget.h>
#include <wg_cvaluedisplay.h>

#include <limits>

namespace wg
{

	//____ constructor ________________________________________________________

	CValueDisplay::CValueDisplay(GeoComponent::Holder * pHolder) : CStaticValueDisplay(pHolder), m_minValue(std::numeric_limits<int64_t>::min()), m_maxValue(std::numeric_limits<int64_t>::max())
	{
	}

	//____ clear() _________________________________________________________________

	void CValueDisplay::clear()
	{
		bool bModified = value() != 0;
		_clear();

		if (bModified)
			onValueModified();
	}

	//____ set() ___________________________________________________________________

	bool CValueDisplay::set(int64_t value, int scale)
	{
		if (_set(value, scale))
		{
			onValueModified();
			return true;
		}
		else
			return false;
	}

	//____ setRange() ______________________________________________________________

	bool CValueDisplay::setRange(int64_t min, int64_t max)
	{
		int64_t val = value();

		bool retVal = _setRange(min, max);
		if (val != value())
			onValueModified();

		return retVal;
	}

	//____ _clear() _________________________________________________________________

	void CValueDisplay::_clear()
	{
		CStaticValueDisplay::clear();
		m_minValue = std::numeric_limits<int64_t>::min();
		m_maxValue = std::numeric_limits<int64_t>::max();
	}

	//____ _set() ___________________________________________________________________

	bool CValueDisplay::_set( int64_t value, int scale )
	{
		if( value > m_maxValue )
			value = m_maxValue;
		else if( value < m_minValue )
			value = m_minValue;
		return CStaticValueDisplay::set(value,scale);
	}

	//____ _setRange() ______________________________________________________________

	bool CValueDisplay::_setRange( int64_t min, int64_t max )
	{
		if( min > max )
			return false;

		m_minValue = min;
		m_maxValue = max;

		int64_t val = m_value;
		limit( val, min, max );
		m_value = val;
		if( val != m_value )
			CStaticValueDisplay::set(val, m_scale);
		return true;
	}

} // namespace wg
