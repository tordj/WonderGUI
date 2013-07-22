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

const char WgFiller::CLASSNAME[] = {"Filler"};

//____ Constructor ____________________________________________________________

WgFiller::WgFiller()
{
	m_preferredSize = WgSize(-1,-1);
}

//____ Destructor _____________________________________________________________

WgFiller::~WgFiller()
{
}

//____ IsInstanceOf() _________________________________________________________

bool WgFiller::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgWidget::IsInstanceOf(pClassName);
}

//____ ClassName() ____________________________________________________________

const char * WgFiller::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ Cast() _________________________________________________________________

WgFillerPtr WgFiller::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgFillerPtr( static_cast<WgFiller*>(pObject.GetRealPtr()) );

	return 0;
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
