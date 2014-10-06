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

//____ IsInstanceOf() _________________________________________________________

bool WgSizeCapsule::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgCapsule::IsInstanceOf(pClassName);
}

//____ ClassName() ____________________________________________________________

const char * WgSizeCapsule::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ Cast() _________________________________________________________________

WgSizeCapsulePtr WgSizeCapsule::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgSizeCapsulePtr( static_cast<WgSizeCapsule*>(pObject.GetRealPtr()) );

	return 0;
}


//____ SetPreferredSize() _____________________________________________________

void WgSizeCapsule::SetPreferredSize( WgSize size )
{
	if( size != m_preferred )
	{
		m_preferred = size;
		_requestResize();
	}
}

//____ SetSizes() _____________________________________________________________

void WgSizeCapsule::SetSizes( WgSize min, WgSize preferred, WgSize max )
{
	m_min = min;
	m_preferred = preferred;
	m_max = max;
	_requestResize();
}

//____ SetMinSize() ___________________________________________________________

void WgSizeCapsule::SetMinSize( WgSize size )
{
	if( size != m_min )
	{
		m_min = size;
		_requestResize();
	}
}

//____ SetMaxSize() ___________________________________________________________

void WgSizeCapsule::SetMaxSize( WgSize size )
{
	if( size != m_max )
	{
		m_max = size;
		_requestResize();
	}
}

//____ PreferredSize() ________________________________________________________

WgSize WgSizeCapsule::PreferredSize() const 
{
	if( m_hook._widget() )
	{
		WgSize pref = m_hook._widget()->PreferredSize();

		if( m_preferred.w != 0 )
		{
			pref.w = m_preferred.w;
			if( m_pScaler )
				pref.w = (int) (pref.w * m_pScaler->ScaleX());
		}
		if( m_preferred.h != 0 )
		{
			pref.h = m_preferred.h;
			if( m_pScaler )
				pref.h = (int) (pref.h * m_pScaler->ScaleY());
		}

		// Constrain against min/max, taking MatchingWidth/MatchingHeight into account.
		//TODO: Check so we don't have any corner cases that breaks the constraints and
		// and that priorities between preferred height/width are reasonable.

		WgSize min = MinSize();
		WgSize max = MaxSize();

		if( pref.w < min.w )
		{
			pref.w = min.w;
			pref.h = MatchingHeight(pref.w);
		}

		if( pref.h < min.h )
		{
			pref.h = min.h;
			pref.w = MatchingWidth(pref.h);
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
				pref.h = MatchingHeight(pref.w);
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
				pref.w = MatchingWidth(pref.h);
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
			pref.w = (int) (pref.w * m_pScaler->ScaleX());
			pref.h = (int) (pref.h * m_pScaler->ScaleY());
		}
		return pref;
	}
}

//____ MinSize() ______________________________________________________________

WgSize WgSizeCapsule::MinSize() const 
{

	if( m_hook._widget() )
		return WgSize::Max(m_min,m_hook._widget()->MinSize());
	else
		return m_min;
}

//____ MaxSize() ______________________________________________________________

WgSize WgSizeCapsule::MaxSize() const 
{
	if( m_hook._widget() )
		return WgSize::Min(m_max,m_hook._widget()->MaxSize());
	else
		return m_max; 
}

//____ MatchingHeight() _______________________________________________________

int WgSizeCapsule::MatchingHeight( int width ) const
{
	if( m_preferred.h != 0 )
	{
		int h = m_preferred.h;
		
		if( m_pScaler )
			h = (int) (h * m_pScaler->ScaleY() );

		if( m_hook._widget() )
		{
			int max = m_hook._widget()->MaxSize().h;
			int min = m_hook._widget()->MinSize().h;
			WG_LIMIT( h, min, max );
		}
		return h;
	}
	else if( m_hook._widget() )
	{
		int h = m_hook._widget()->MatchingHeight(width);
		WG_LIMIT( h, m_min.h, m_max.h );
		return h;
	}
	else
		return m_min.h;
}

//____ MatchingWidth() _______________________________________________________

int WgSizeCapsule::MatchingWidth( int height ) const
{
	if( m_preferred.w != 0 )
	{
		int w = m_preferred.w;		

		if( m_pScaler )
			w = (int) (w * m_pScaler->ScaleX() );

		if( m_hook._widget() )
		{
			int max = m_hook._widget()->MaxSize().w;
			int min = m_hook._widget()->MinSize().w;
			WG_LIMIT( w, min, max );
		}
		return w;
	}
	else if( m_hook._widget() )
	{
		int w = m_hook._widget()->MatchingWidth(height);
		WG_LIMIT( w, m_min.w, m_max.w );
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
