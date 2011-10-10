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

//#include <wg_textprop.h>
#include <wg_textpropmanager.h>
#include <wg_util.h>
#include <wg_color.h>
#include <wg_char.h>
#include <wg_font.h>

#include <memory.h>

Sint16				WgTextPropManager::g_propIndex[256];

WgTextPropHolder	WgTextPropManager::g_nullProp;


WgTextPropHolder *	WgTextPropManager::g_pPropBuffer = &g_nullProp;
Uint32				WgTextPropManager::g_nPropUsed  = 1;
Uint32				WgTextPropManager::g_nPropTotal = 1;
Sint16				WgTextPropManager::g_firstFreeProp = -1;
bool				WgTextPropManager::g_bMergeSimilar = true;

WgTextPropManager	dummy;		// Used to bootstrap the system...


//____ Constructor ____________________________________________________________

WgTextPropManager::WgTextPropManager()
{
	// Initialize nullProp

	g_nullProp.m_id				= 0;
	g_nullProp.m_indexOfs		= 0;			// To be set with CalculateChecksum after Prop finalized...
	g_nullProp.m_refCnt			= 1;			// Never delete this one...
	g_nullProp.m_next			= -1;
	g_nullProp.m_prev			= -1;

	g_nullProp.m_indexOfs = g_nullProp.m_prop.CalculateChecksum();

	// Initialize the index table

//	g_propIndex[0] = 0;
	for( int i = 0 ; i < 256 ; i++ )
		g_propIndex[i] = -1;

	// Insert nullProp into the index table

	g_propIndex[g_nullProp.m_indexOfs] = 0;
}

//____ RegisterProp() _________________________________________________________

Uint16 WgTextPropManager::RegisterProp( const WgTextProp& prop )
{
	prop.AssertIntegrity();

	Uint8 ofs = prop.CalculateChecksum();

	// Check s_propIndex for indentical property, increase ref and return if found.

	Sint16 h = g_propIndex[ofs];

	while( h >= 0 )
	{
		WgTextPropHolder * p = &g_pPropBuffer[h];

		if( g_bMergeSimilar && prop.CompareTo( &p->m_prop ) )
			return p->m_id;

		h = (Sint16) p->m_next;
	}

	// Hämta en ledig WgTextProp, fyll i data, länka och returnera id.

	if( g_firstFreeProp < 0 )
		IncreaseBuffer();

	Uint16 id = g_firstFreeProp;
	WgTextPropHolder * p = &g_pPropBuffer[id];


	g_firstFreeProp = p->m_next;
	if( g_firstFreeProp >= 0 )
		g_pPropBuffer[g_firstFreeProp].m_prev = -1;

	p->m_prev = -1;
	p->m_next = g_propIndex[ofs];
	g_propIndex[ofs] = id;

	if( p->m_next >= 0 )
		g_pPropBuffer[p->m_next].m_prev = id;

	p->m_id 			= id;
	p->m_indexOfs		= ofs;
	p->m_refCnt 		= 0;

	p->m_prop			= prop;

	g_nPropUsed++;
	return id;
}


//____ IncreaseBuffer() ________________________________________________________

void WgTextPropManager::IncreaseBuffer()
{
	if( g_nPropTotal == 32768 )
		return;

	// Determine size of new buffer

	int newSize;

	if( g_nPropTotal == 1 )
		newSize = NB_START_PROPS;
	else
		newSize = g_nPropTotal*2;

	if( newSize > 32768 )
		newSize = 32768;

	// Allocate new buffer and make binary copy from old before destroying it.
	// Binary copy and constructing/deleting as char to avoid overhead and
	// problems with smartpointers that otherwise would increase/decrease
	// references etc.

	char * pNewBuffer = new char[sizeof(WgTextPropHolder)*newSize];

	memcpy( pNewBuffer, g_pPropBuffer, sizeof(WgTextPropHolder)*g_nPropTotal );
	memset( pNewBuffer+sizeof(WgTextPropHolder)*g_nPropTotal, 0, (newSize - g_nPropTotal)*sizeof(WgTextPropHolder) );

	if( g_pPropBuffer != &g_nullProp )
		delete [] (char*)g_pPropBuffer;

	// Construct the s_firstFreeProp chain.

	WgTextPropHolder * p = (WgTextPropHolder*) pNewBuffer;

	for( int i = g_nPropTotal ; i < newSize ; i++ )
	{
		p[i].m_next = i+1;
		p[i].m_prev = i-1;
	}
	p[g_nPropTotal].m_prev 	= -1;	// First has no previous...
	p[newSize-1].m_next 	= -1;	// Last has no next...

	g_firstFreeProp = g_nPropTotal;

	// Set the global data.

	g_pPropBuffer = (WgTextPropHolder*) pNewBuffer;
	g_nPropTotal = newSize;
}

//____ FreeProp() ______________________________________________

void WgTextPropManager::FreeProp( Uint16 hProp )
{
	WgTextPropHolder * p = &g_pPropBuffer[hProp];

	// Remove prop from its g_propIndex chain.

	if( p->m_prev != -1 )
		g_pPropBuffer[p->m_prev].m_next = p->m_next;
	else
		g_propIndex[p->m_indexOfs] = p->m_next;

	if( p->m_next != -1 )
		g_pPropBuffer[p->m_next].m_prev = p->m_prev;


	// Insert prop into g_firstFreeProp chain.

	p->m_next = g_firstFreeProp;
	p->m_prev = -1;

	if( g_firstFreeProp != -1 )
		g_pPropBuffer[g_firstFreeProp].m_prev = hProp;
	g_firstFreeProp = hProp;

	// Dereference smartpointers

	if( p->m_prop.m_pLink )
		p->m_prop.m_pLink = 0;

	// Decrease counter, also delete buffer if we are back to only the dummy...

	g_nPropUsed--;

	if( g_nPropUsed == 1 )
	{
		memcpy( &g_nullProp, g_pPropBuffer, sizeof(WgTextPropHolder) );	// Copy content of prop 0 to nullProp.

		delete [] (char*) g_pPropBuffer;	// Typecast to avoid dereferencing Normal delete, so smartpointers gets dereferenced.

		g_pPropBuffer 		= &g_nullProp;
		g_nPropTotal 		= 1;
		g_firstFreeProp 	= -1;
	}
}

