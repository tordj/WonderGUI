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

#include <wg_boxlayout.h>

WgBoxLayoutHook::WgBoxLayoutHook()
{
	m_maxLength = INT_MAX;
	m_minLength = 0;
	m_weight = 1.f;
}

WgBoxLayout * WgBoxLayoutHook::Parent() const 
{ 
	return static_cast<WgBoxLayout*>(_parent()); 
}


bool WgBoxLayoutHook::SetWeight( float weight )
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



void WgBoxLayout::SetGeoBroker( WgGeoBroker& broker )
{
	m_contractionBroker = broker;
	m_expansionBroker = broker;
	_reallocateSpace();
}

void WgBoxLayout::SetContractionBroker( WgGeoBroker& broker )
{
	m_contractionBroker = broker;
	_reallocateSpace();
}

void WgBoxLayout::SetExpansionBroker( WgGeoBroker& broker )
{
	m_expansionBroker = broker;
	_reallocateSpace();
}

void WgBoxLayout::_reallocateSpace()
{
}
