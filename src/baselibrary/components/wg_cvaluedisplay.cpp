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
	const TypeInfo CValueDisplay::TYPEINFO = { "CValueDisplay", &CStaticValueDisplay::TYPEINFO };

	//____ constructor ________________________________________________________

	CValueDisplay::CValueDisplay(GeoComponent::Holder * pHolder) : CStaticValueDisplay(pHolder), m_minValue(std::numeric_limits<double>::lowest()), m_maxValue(std::numeric_limits<double>::max())
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& CValueDisplay::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ clear() _________________________________________________________________

	void CValueDisplay::clear()
	{
		m_minValue = std::numeric_limits<double>::lowest();
		m_maxValue = std::numeric_limits<double>::max();
		CStaticValueDisplay::_set(0.0);
	}

	//____ set() ___________________________________________________________________

	bool CValueDisplay::set(double value)
	{
		return _set(value);
	}

	//____ setRange() ______________________________________________________________

	bool CValueDisplay::setRange(double min, double max)
	{
		if (min > max)
			return false;

		m_minValue = min;
		m_maxValue = max;

		double val = m_value;
		limit(val, min, max);
		m_value = val;
		if (val != m_value)
			CStaticValueDisplay::_set(val);
		return true;
	}

	//____ _set() ___________________________________________________________________

	bool CValueDisplay::_set( double value )
	{
		if( value > m_maxValue )
			value = m_maxValue;
		else if( value < m_minValue )
			value = m_minValue;
		return CStaticValueDisplay::_set(value);
	}

} // namespace wg
