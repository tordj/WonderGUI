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


#include <wg_emitter.h>

#include <string>
//#include <Misc/SendAssert.hpp>

int WgEmitter::s_EmitRef;

//____ WgEmitter() ____________________________________________________________

WgEmitter::WgEmitter()
{
	m_nCallbacks		= 0;
	m_paCallbacks		= 0;
}


//____ ~WgEmitter() ___________________________________________________________

WgEmitter::~WgEmitter()
{
	if(s_EmitRef != 0)
	{
		// SendAssertStackDump("invalid widget removal");
	}

	if( m_paCallbacks )
		delete [] m_paCallbacks;
}


//____ AddCallbackUnsafe() ____________________________________________________

void WgEmitter::AddCallbackUnsafe( WgSignal::Signal signal, void * pCallback, void * pObj )
{
	WgCallback cb;
	cb.pCallback = pCallback;
	cb.signal = signal;
	cb.pObj = pObj;

	// check if it's already in the list
	for( int i = 0 ; i < m_nCallbacks ; i++ )
	{
		if( m_paCallbacks[i] == cb )
			return;
	}

	WgCallback * pNewArray = new WgCallback[m_nCallbacks+1];

	if( m_paCallbacks )
	{
		for( int i = 0 ; i < m_nCallbacks ; i++ )
			pNewArray[i] = m_paCallbacks[i];

		delete [] m_paCallbacks;
	}

	pNewArray[m_nCallbacks] = cb;

	m_paCallbacks = pNewArray;
	m_nCallbacks++;
}

//____ RemoveCallback() _______________________________________________________

int	WgEmitter::RemoveCallback( WgSignal::Signal signal, void * pCallback, void * pObj )
{
	int oldCallbacks = m_nCallbacks;
	
	for( int i = 0 ; i < m_nCallbacks ; i++ )
	{
			if( m_paCallbacks[i].signal == signal && m_paCallbacks[i].pCallback == pCallback 
					&& m_paCallbacks[i].pObj == pObj )
			{
				m_paCallbacks[i] = m_paCallbacks[m_nCallbacks-1];
				m_nCallbacks--;
				i--;																							// Let's try this one again...
			}
	}
	return oldCallbacks - m_nCallbacks;
}

//____ RemoveCallbacks(signal) ________________________________________________

int	WgEmitter::RemoveCallbacks( WgSignal::Signal signal )
{
	int oldCallbacks = m_nCallbacks;
	
	for( int i = 0 ; i < m_nCallbacks ; i++ )
	{
			if( m_paCallbacks[i].signal == signal )
			{
				m_paCallbacks[i] = m_paCallbacks[m_nCallbacks-1];
				m_nCallbacks--;
				i--;																							// Let's try this one again...
			}
	}
	return oldCallbacks - m_nCallbacks;
}

//____ RemoveCallbacks(obj) ___________________________________________________

int	WgEmitter::RemoveCallbacks( void * pObj )
{
	int oldCallbacks = m_nCallbacks;
	
	for( int i = 0 ; i < m_nCallbacks ; i++ )
	{
			if( m_paCallbacks[i].pObj == pObj )
			{
				m_paCallbacks[i] = m_paCallbacks[m_nCallbacks-1];
				m_nCallbacks--;
				i--;																							// Let's try this one again...
			}
	}
	return oldCallbacks - m_nCallbacks;
}

//____ RemoveCallbacks() ______________________________________________________

int WgEmitter::RemoveCallbacks()
{
	int retval = m_nCallbacks;

	m_nCallbacks = 0;

	if( m_paCallbacks )
	{
		delete [] m_paCallbacks;
		m_paCallbacks = 0;
	}

	return retval;
}
