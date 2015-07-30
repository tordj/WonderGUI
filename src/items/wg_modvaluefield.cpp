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
#include <wg_modvaluefield.h>

//____ clear() _________________________________________________________________

void WgModValueField::clear()
{
	WgValueField::clear();
	m_minValue = INT64_MIN;
	m_maxValue = INT64_MAX;
}

//____ set() ___________________________________________________________________

bool WgModValueField::set( Sint64 value, int scale )
{
	if( value > m_maxValue )
		value = m_maxValue;
	else if( value < m_minValue )
		value = m_minValue;
	return WgValueField::set(value,scale);
}

//____ setRange() ______________________________________________________________

bool WgModValueField::setRange( Sint64 min, Sint64 max )
{
	if( min > max )
		return false;

	m_minValue = min;
	m_maxValue = max;
	
	Sint64 val = m_value;
	LIMIT( val, min, max );
	m_value = val;
	if( val != m_value )
		WgValueField::set(val, m_scale);
	return true;
}	
