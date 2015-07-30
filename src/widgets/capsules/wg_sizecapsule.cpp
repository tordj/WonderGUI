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

#include <wg_sizecapsule.h>

const char WgSizeCapsule::CLASSNAME[] = {"SizeCapsule"};

//____ Constructor ____________________________________________________________

WgSizeCapsule::WgSizeCapsule() : m_max(INT_MAX,INT_MAX)
{
}

//____ Destructor _____________________________________________________________

WgSizeCapsule::~WgSizeCapsule()
{
}

//____ isInstanceOf() _________________________________________________________

bool WgSizeCapsule::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgCapsule::isInstanceOf(pClassName);
}

//____ className() ____________________________________________________________

const char * WgSizeCapsule::className( void ) const
{ 
	return CLASSNAME; 
}

//____ cast() _________________________________________________________________

WgSizeCapsule_p WgSizeCapsule::cast( const WgObject_p& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgSizeCapsule_p( static_cast<WgSizeCapsule*>(pObject.rawPtr()) );

	return 0;
}


//____ setPreferredSize() _____________________________________________________

void WgSizeCapsule::setPreferredSize( WgSize size )
{
	if( size != m_preferred )
	{
		m_preferred = size;
		_requestResize();
	}
}

//____ setSizes() _____________________________________________________________

void WgSizeCapsule::setSizes( WgSize min, WgSize preferred, WgSize max )
{
	m_min = min;
	m_preferred = preferred;
	m_max = max;
	_requestResize();
}

//____ setMinSize() ___________________________________________________________

void WgSizeCapsule::setMinSize( WgSize size )
{
	if( size != m_min )
	{
		m_min = size;
		_requestResize();
	}
}

//____ setMaxSize() ___________________________________________________________

void WgSizeCapsule::setMaxSize( WgSize size )
{
	if( size != m_max )
	{
		m_max = size;
		_requestResize();
	}
}

//____ preferredSize() ________________________________________________________

WgSize WgSizeCapsule::preferredSize() const 
{
	if( m_hook._widget() )
	{
		WgSize pref = m_hook._widget()->preferredSize();

		if( m_preferred.w != 0 )
		{
			pref.w = m_preferred.w;
			if( m_pScaler )
				pref.w = (int) (pref.w * m_pScaler->scaleX());
		}
		if( m_preferred.h != 0 )
		{
			pref.h = m_preferred.h;
			if( m_pScaler )
				pref.h = (int) (pref.h * m_pScaler->scaleY());
		}

		// Constrain against min/max, taking MatchingWidth/MatchingHeight into account.
		//TODO: Check so we don't have any corner cases that breaks the constraints and
		// and that priorities between preferred height/width are reasonable.

		WgSize min = minSize();
		WgSize max = maxSize();

		if( pref.w < min.w )
		{
			pref.w = min.w;
			pref.h = matchingHeight(pref.w);
		}

		if( pref.h < min.h )
		{
			pref.h = min.h;
			pref.w = matchingWidth(pref.h);
			if( pref.w < min.w )
				pref.w = min.w;
		}

		if( pref.w > max.w )
		{
			if( pref.h > max.h )
				pref = max;
			else
			{
				pref.w = max.w;
				pref.h = matchingHeight(pref.w);
				if( pref.h > max.h )
					pref.h = max.h;
			}
		}
		else if( pref.h > max.h )
		{
			if( pref.w > max.w )
				pref = max;
			else
			{
				pref.h = max.h;
				pref.w = matchingWidth(pref.h);
				if( pref.w > max.w )
					pref.w = max.w;
			}
		}
		return pref;
	}
	else
	{
		WgSize pref = m_preferred;
		if( m_pScaler )
		{
			pref.w = (int) (pref.w * m_pScaler->scaleX());
			pref.h = (int) (pref.h * m_pScaler->scaleY());
		}
		return pref;
	}
}

//____ minSize() ______________________________________________________________

WgSize WgSizeCapsule::minSize() const 
{

	if( m_hook._widget() )
		return WgSize::max(m_min,m_hook._widget()->minSize());
	else
		return m_min;
}

//____ maxSize() ______________________________________________________________

WgSize WgSizeCapsule::maxSize() const 
{
	if( m_hook._widget() )
		return WgSize::min(m_max,m_hook._widget()->maxSize());
	else
		return m_max; 
}

//____ matchingHeight() _______________________________________________________

int WgSizeCapsule::matchingHeight( int width ) const
{
	if( m_preferred.h != 0 )
	{
		int h = m_preferred.h;
		
		if( m_pScaler )
			h = (int) (h * m_pScaler->scaleY() );

		if( m_hook._widget() )
		{
			int max = m_hook._widget()->maxSize().h;
			int min = m_hook._widget()->minSize().h;
			LIMIT( h, min, max );
		}
		return h;
	}
	else if( m_hook._widget() )
	{
		int h = m_hook._widget()->matchingHeight(width);
		LIMIT( h, m_min.h, m_max.h );
		return h;
	}
	else
		return m_min.h;
}

//____ matchingWidth() _______________________________________________________

int WgSizeCapsule::matchingWidth( int height ) const
{
	if( m_preferred.w != 0 )
	{
		int w = m_preferred.w;		

		if( m_pScaler )
			w = (int) (w * m_pScaler->scaleX() );

		if( m_hook._widget() )
		{
			int max = m_hook._widget()->maxSize().w;
			int min = m_hook._widget()->minSize().w;
			LIMIT( w, min, max );
		}
		return w;
	}
	else if( m_hook._widget() )
	{
		int w = m_hook._widget()->matchingWidth(height);
		LIMIT( w, m_min.w, m_max.w );
		return w;
	}
	else
		return m_min.w;
}

//____ _onScaleChanged() ______________________________________________________

void WgSizeCapsule::_onScaleChanged()
{
	_requestResize();
}
