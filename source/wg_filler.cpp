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



#include <wg_filler.h>
#include <wg_gfxdevice.h>

static const char	c_widgetType[] = {"Filler"};

//____ Constructor ____________________________________________________________

WgFiller::WgFiller()
{
	m_preferredSize = WgSize(-1,-1);
}

//____ Destructor _____________________________________________________________

WgFiller::~WgFiller()
{
}

//____ Type() _________________________________________________________________

const char * WgFiller::Type( void ) const
{
	return GetClass();
}

//____ GetClass() ____________________________________________________________

const char * WgFiller::GetClass()
{
	return c_widgetType;
}

//____ SetPreferredSize() _______________________________________________________

void WgFiller::SetPreferredSize( const WgSize& size )
{
	if( size != m_preferredSize )
	{
		m_preferredSize = size;
		_requestResize();
	}
}

//____ PreferredSize() __________________________________________________________

WgSize WgFiller::PreferredSize() const
{
	if( m_preferredSize.w >= 0 && m_preferredSize.h >= 0 )
		return m_preferredSize;
	else
		return WgWidget::PreferredSize();
}


//____ _onCloneContent() _______________________________________________________

void WgFiller::_onCloneContent( const WgWidget * _pOrg )
{
	WgFiller * pOrg = (WgFiller*) _pOrg;

	m_preferredSize = pOrg->m_preferredSize;
}
