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

#include <wg_eventhandler.h>

//____ Constructor ____________________________________________________________

WgEventHandler::WgEventHandler( Int64 startTime )
{
	m_time = startTime;
}

//____ Destructor _____________________________________________________________

WgEventHandler::~WgEventHandler()
{
}

//____ QueueEvent() ___________________________________________________________

bool WgEventHandler::QueueEvent( const WgEvent::EventBase& _event )
{
	m_eventQueue.push_back( _event );

	WgEvent& ev = m_eventQueue.back();

}

//____ ProcessEvents() ________________________________________________________

void WgEventHandler::ProcessEvents()
{
}
