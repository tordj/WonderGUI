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

#include <wg_packpanel.h>

WgPackHook::WgPackHook()
{
	m_weight = 1.f;
}

WgPackPanel * WgPackHook::Parent() const 
{ 
	return static_cast<WgPackPanel*>(_parent()); 
}


bool WgPackHook::SetWeight( float weight )
{
	if( weight < 0 )
		return false;

	if( weight != m_weight )
	{
		m_weight = weight;
		Parent()->_reallocateSpace();
	}
	return true;
}



void WgPackPanel::SetSizeBroker( WgSizeBroker& broker )
{
	m_contractionBroker = broker;
	m_expansionBroker = broker;
	_reallocateSpace();
}

void WgPackPanel::SetContractionBroker( WgSizeBroker& broker )
{
	m_contractionBroker = broker;
	_reallocateSpace();
}

void WgPackPanel::SetExpansionBroker( WgSizeBroker& broker )
{
	m_expansionBroker = broker;
	_reallocateSpace();
}

void WgPackPanel::_reallocateSpace()
{
}
