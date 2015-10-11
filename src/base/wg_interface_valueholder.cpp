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


#include <wg_interface_valueholder.h>
#include <wg_string.h>

#include <math.h>

namespace wg 
{
	//#include <stdint.h>
	
	//____ Constructor ____________________________________________________________
	
	_Interface_ValueHolder::_Interface_ValueHolder()
	{
		m_rangeMin						= LLONG_MIN >> 2;
		m_rangeMax						= LLONG_MAX >> 2;
		m_value							= 0;
		m_stepSize						= 1;
		m_unitSize						= 1;
		m_nValueDigits					= 0;
		m_modulator						= 1;
	}
	
	//____ setRange() _____________________________________________________________
	
	bool _Interface_ValueHolder::setRange( int64_t min, int64_t max )
	{
		if( min > max )
			return false;
	
		if( min == m_rangeMin && max == m_rangeMax )
			return true;
	
		m_rangeMin = min;
		m_rangeMax = max;
	
		if( m_value < m_rangeMin )
		{
			m_value = m_rangeMin;
			_valueModified();
		}
		else if( m_value > m_rangeMax )
		{
			m_value = m_rangeMax;
			_valueModified();
		}
	
		_rangeModified();
		_updateScrollbar( fractionalValue(), 0.f );
		return true;
	}
	
	//____ setFractionalRounding() ________________________________________________
	
	bool _Interface_ValueHolder::setFractionalRounding( int nValueDigits, int lastDigitModulo )
	{
		if( nValueDigits < 0 || nValueDigits > 12 || (lastDigitModulo != 1 && lastDigitModulo != 2 && lastDigitModulo != 5 ) )
			return false;
	
		m_nValueDigits	= nValueDigits;
		m_modulator		= lastDigitModulo;
		return true;
	}
	
	
	//____ setUnitSize() __________________________________________________________
	
	bool _Interface_ValueHolder::setUnitSize( int unitsize )
	{
		if( unitsize <= 0 )
			return false;
	
		m_unitSize = unitsize;
		return true;
	}
	
	//____ setValue() _____________________________________________________________
	
	bool _Interface_ValueHolder::setValue( int64_t value )
	{
		bool b = _setValue(value);
		_updateScrollbar( fractionalValue(), 0.f );
		return b;
	}
	
	//____ setFractionalValue() ___________________________________________________
	
	bool _Interface_ValueHolder::setFractionalValue( float fraction )
	{
		// First we check if fraction is identical to what we might have sent out
		// ourselves. In that case we ignore it even if it would result in a slightly
		// different value due to rounding. This prevents signal-looping and making
		// it impossible to step up/down at certain values if we are connected to a scrollbar.
	
		float currFracValue = (m_value - m_rangeMin) / (float)(m_rangeMax - m_rangeMin);
		if( fraction > currFracValue-0.000001 && fraction < currFracValue+0.000001 )
			return true;
	
		bool b = _setFractionalValue(fraction);
		_updateScrollbar( fractionalValue(), 0.f );
		return b;
	}
	
	//____ fractionalValue() ______________________________________________________
	
	float _Interface_ValueHolder::fractionalValue()
	{
		if( m_rangeMax == m_rangeMin )
			return 0.0f;
	
		return( (m_value - m_rangeMin) / (float)(m_rangeMax - m_rangeMin) );
	}
	
	//____ setStepSize() __________________________________________________________
	
	bool _Interface_ValueHolder::setStepSize( int size )
	{
		if( size <= 0 )
			return false;
	
		m_stepSize = size;
		return true;
	}
	
	//____ incValue() _____________________________________________________________
	
	bool _Interface_ValueHolder::incValue()
	{
		return setValue( m_value + m_stepSize );
	}
	
	bool _Interface_ValueHolder::incValue( int increment )
	{
		return setValue( m_value + increment );
	}
	
	//____ decValue() _____________________________________________________________
	
	bool _Interface_ValueHolder::decValue()
	{
		return setValue( m_value - m_stepSize );
	}
	
	bool _Interface_ValueHolder::decValue( int decrement )
	{
		return setValue( m_value - decrement );
	}
	
	//____ _stepFwd() ______________________________________________________________
	
	float _Interface_ValueHolder::_stepFwd()
	{
		_setValue( m_value + m_stepSize );
		return fractionalValue();
	}
	
	//____ _stepBwd() ______________________________________________________________
	
	float _Interface_ValueHolder::_stepBwd()
	{
		_setValue( m_value - m_stepSize );
		return fractionalValue();
	}
	
	//____ _jumpFwd() ______________________________________________________________
	
	float _Interface_ValueHolder::_jumpFwd()
	{
		_setValue( m_value + 10*m_stepSize );
		return fractionalValue();
	}
	
	//____ _jumpBwd() ______________________________________________________________
	
	float _Interface_ValueHolder::_jumpBwd()
	{
		_setValue( m_value - 10*m_stepSize );
		return fractionalValue();
	}
	
	//____ _wheelRolled() __________________________________________________________
	
	float _Interface_ValueHolder::_wheelRolled( int distance )
	{
		_setValue( m_value + distance*m_stepSize );
		return fractionalValue();
	}
	
	//____ _setPosition() __________________________________________________________
	
	float _Interface_ValueHolder::_setPosition( float fraction )
	{
		_setFractionalValue(fraction);
		return fractionalValue();
	}
	
	//____ _getHandlePosition() ____________________________________________________
	
	float _Interface_ValueHolder::_getHandlePosition()
	{
		return fractionalValue();
	}
	
	//____ _getHandleSize() ________________________________________________________
	
	float _Interface_ValueHolder::_getHandleSize()
	{
		return 0.f;
	}
	
	
	//____ _setFractionalValue() ___________________________________________________
	
	bool _Interface_ValueHolder::_setFractionalValue( float fraction )
	{
		// Calculate new value and set it.
	
		int64_t value = (int64_t)(fraction * (m_rangeMax - m_rangeMin) + m_rangeMin);
	
		if( m_nValueDigits != 0 && value != m_rangeMin && value != m_rangeMax )
		{
			int ceiling = (int) pow( 10.f, (float) m_nValueDigits );
	
			int mulVal = 1;
			while( value > ceiling )
			{
				value /=10;
				mulVal *= 10;
			}
	
			if( mulVal > 1 || value > ceiling/10 )
				value -= value%m_modulator;					// Modulate lowest value-digit.
	
			value *= mulVal;
		}
	
		return _setValue( value );
	}
	
	//____ _setValue() _____________________________________________________________
	
	bool _Interface_ValueHolder::_setValue( int64_t value )
	{
		bool	retVal = true;
	
		if( value < m_rangeMin )
		{
			value = m_rangeMin;
			retVal = false;
		}
	
		if( value > m_rangeMax )
		{
			value = m_rangeMax;
			retVal = false;
		}
	
		value -= value % m_unitSize;
	
		if( value != m_value )
		{
			m_value = value;
			_valueModified();
		}
	
		return retVal;
	}
	
	
	//____ _onCloneContent() _______________________________________________________
	
	void _Interface_ValueHolder::_onCloneContent( const _Interface_ValueHolder * pOrg )
	{
		m_rangeMin 	= pOrg->m_rangeMin;
		m_rangeMax 	= pOrg->m_rangeMax;
		m_value		= pOrg->m_value;
		m_stepSize	= pOrg->m_stepSize;
		m_unitSize	= pOrg->m_unitSize;
	}
	
	

} // namespace wg
