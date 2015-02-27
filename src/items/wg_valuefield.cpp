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
#include <wg_valuefield.h>


//____ Constructor _____________________________________________________________

WgValueField::WgValueField(WgValueHolder * pHolder) : WgPresentableField(pHolder)
{
	m_value = 0;
	m_scale = 1;
}

//____ SetFormatter() __________________________________________________________

void WgValueField::SetFormatter( const WgValueFormatterPtr& pFormatter )
{
	if( m_pFormatter != pFormatter )
	{
		m_pFormatter = pFormatter;
		_regenText();
	}
}

//____ ClearFormatter() ________________________________________________________

void WgValueField::ClearFormatter()
{
	if( m_pFormatter )
	{
		m_pFormatter = 0;
		_regenText();
	}
}

//____ Clear() _________________________________________________________________

void WgValueField::Clear()
{
	if( m_value != 0 )
	{
		m_value = 0;
		m_scale = 1;
		_regenText();
	}
}

//____ Set() ___________________________________________________________________

bool WgValueField::Set( Sint64 value, int scale )
{
	if( m_value != value && m_scale != scale )
	{
		if( scale <= 0 )
			return false;
		
		m_value = value;
		m_scale = scale;
		_regenText();
	}
	return true;
}
/*
void WgValueField::Set( float value )
{
	value *= m_scale;
	Sint64 intVal = (Sint64) value;
	if( intVal != m_value )
	{
		m_value = intVal;
		_regenText();
	}
}

void WgValueField::Set( double value )
{
	value *= m_scale;
	Sint64 intVal = (Sint64) value;
	if( intVal != value )
	{
		m_value = intVal;
		_regenText();
	}
}
*/

//____ OnRefresh() _____________________________________________________________

void WgValueField::OnRefresh()
{	
	_regenText();
	WgPresentableField::OnRefresh();
}

//____ _regenText() ____________________________________________________________

void WgValueField::_regenText()
{
	WgValueFormatter * pFormatter = m_pFormatter ? m_pFormatter.RawPtr() : 0; //WgBase::DefaultValueFormatter();
	if( m_pFormatter )
		m_charBuffer = pFormatter->Format(m_value, m_scale);
	_onDirty();
}
