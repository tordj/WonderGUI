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

static const char	c_widgetType[] = {"SizeCapsule"};

//____ Constructor ____________________________________________________________

WgSizeCapsule::WgSizeCapsule() : m_max(INT_MAX,INT_MAX)
{
}

//____ Destructor _____________________________________________________________

WgSizeCapsule::~WgSizeCapsule()
{
}

//____ Type() _________________________________________________________________

const char * WgSizeCapsule::Type( void ) const
{
	return GetClass();
}

//____ GetClass() ____________________________________________________________

const char * WgSizeCapsule::GetClass()
{
	return c_widgetType;
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
	if( m_hook.Widget() )
	{
		WgSize pref = m_hook.Widget()->PreferredSize();

		if( m_preferred.w != 0 )
			pref.w = m_preferred.w;

		if( m_preferred.h != 0 )
			pref.h = m_preferred.h;

		pref.ConstrainTo( MinSize(), MaxSize() );
		return pref;
	}
	else
		return m_preferred;
}

//____ MinSize() ______________________________________________________________

WgSize WgSizeCapsule::MinSize() const 
{

	if( m_hook.Widget() )
		return WgSize::Max(m_min,m_hook.Widget()->MinSize());
	else
		return m_min;
}

//____ MaxSize() ______________________________________________________________

WgSize WgSizeCapsule::MaxSize() const 
{
	if( m_hook.Widget() )
		return WgSize::Min(m_max,m_hook.Widget()->MaxSize());
	else
		return m_max; 
}

//____ HeightForWidth() _______________________________________________________

int WgSizeCapsule::HeightForWidth( int width ) const
{
	if( m_preferred.h != 0 )
	{
		int h = m_preferred.h;
		
		if( m_hook.Widget() )
		{
			int max = m_hook.Widget()->MaxSize().h;
			int min = m_hook.Widget()->MinSize().h;
			WG_LIMIT( h, min, max );
		}
		return h;
	}
	else if( m_hook.Widget() )
	{
		int h = m_hook.Widget()->HeightForWidth(width);
		WG_LIMIT( h, m_min.h, m_max.h );
		return h;
	}
	else
		return m_min.h;
}

//____ WidthForHeight() _______________________________________________________

int WgSizeCapsule::WidthForHeight( int height ) const
{
	if( m_preferred.w != 0 )
	{
		int w = m_preferred.w;		
		if( m_hook.Widget() )
		{
			int max = m_hook.Widget()->MaxSize().w;
			int min = m_hook.Widget()->MinSize().w;
			WG_LIMIT( w, min, max );
		}
		return w;
	}
	else if( m_hook.Widget() )
	{
		int w = m_hook.Widget()->WidthForHeight(height);
		WG_LIMIT( w, m_min.w, m_max.w );
		return w;
	}
	else
		return m_min.w;
}
