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

#include <wg_spanitem.h>

WgSpanItem::WgSpanItem( WgSpanHolder * pHolder ) : WgItem( pHolder )
{
	m_min = MIN;
	m_max = MAX;
	m_begin = 0;
	m_length = 1;
}


bool WgSpanItem::SetMin( int min )
{
	if( min < MIN || min > MAX )
		return false;

	if( m_min != min )
	{
		m_min = min;
		if( m_max < m_min )
			m_max = MAX;

		if( m_begin < min )
		{
			m_begin = min;
			if( m_length < m_max - min )
				m_length = m_max - min;
		}

		_onModified();
	}
	return true;
}

bool WgSpanItem::SetMax( int max )
{
	if( max < MIN || max > MAX )
		return false;

	if( m_max != max )
	{
		m_max = max;
		if( m_max < m_min )
			m_min = MIN;

		if( m_begin + m_length > m_max )
		{
			if( m_length > m_max - m_min )
				m_length = m_max - m_min;

			m_begin = m_max - m_length;
		}

		_onModified();
	}
	return true;
}

bool WgSpanItem::SetRange( int min, int max )
{
	if( min > max || min < MIN || max > MAX )
		return false;

	if( m_min != min || m_max != max )
	{
		m_min = min;
		m_max = max;

		if( m_length > m_max - m_min )
			m_length = m_max - m_min;

		if( m_begin < m_min )
			m_begin = m_min;

		if( m_begin + m_length > m_max )
			m_begin = m_max - m_length;

		_onModified();
	}
	return true;
}

void WgSpanItem::SetSpan( int begin, int length )
{
	if( length > m_max - m_min )
		length = m_max - m_min;

	if( begin + length > m_max )
		begin = m_max - length;

	if( begin < m_min )
		begin = m_min;

	if( m_begin != begin || m_length != length )
	{
		m_begin = begin;
		m_length = length;
		_onModified();
	}
}

void WgSpanItem::SetBegin( int begin )
{
	if( begin < m_min )
		begin = m_min;
	else if( begin + m_length > m_max )
		begin = m_max - m_length;

	if( m_begin != begin )
	{
		m_begin = begin;
		_onModified();
	}
}

void WgSpanItem::SetLength( int length )
{
	if( length > m_max - m_min )
		length = m_max - m_min;

	if( m_begin + length > m_max )
		m_begin = m_max - length;

	if( m_length != length )
	{
		m_length = length;
		_onModified();
	}
}

void WgSpanItem::SetRelativeSpan( float begin, float length )
{
	int range = m_max - m_min;
	SetSpan( m_min + (int) (begin*range), (int) length*range );
}

void WgSpanItem::SetRelativePos( float pos )
{
	int range = m_max - m_min;
	SetBegin( m_min + (int) ((pos*range*2)- m_length)/2 );
}

void WgSpanItem::SetRelativeBegin( float begin )
{
	int range = m_max - m_min;
	SetBegin( m_min + (int) (begin*range) );
}

void WgSpanItem::SetRelativeLength( float length )
{	int range = m_max - m_min;
	SetLength( (int) length*range );
}

bool WgSpanItem::StepForward()
{
	int beg = m_begin;
	SetBegin( beg + _stepSize() );
	return beg != m_begin;
}

bool WgSpanItem::StepBackward()
{	int beg = m_begin;
	SetBegin( beg - _stepSize() );
	return beg != m_begin;
}

bool WgSpanItem::SkipForward()
{
	int beg = m_begin;
	SetBegin( beg + _skipSize() );
	return beg != m_begin;
}

bool WgSpanItem::SkipBackward()
{
	int beg = m_begin;
	SetBegin( beg - _skipSize() );
	return beg != m_begin;
}
