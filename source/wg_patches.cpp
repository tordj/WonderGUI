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

#include <wg_patches.h>

//____ Constructor _____________________________________________________________

WgPatches::WgPatches()
{
	m_pFirst	= new WgRect[c_defaultCapacity];
	m_size		= 0;
	m_capacity	= c_defaultCapacity;
	m_bOwnsArray = true;
}

WgPatches::WgPatches( int startCapacity )
{
	m_pFirst	= new WgRect[startCapacity];
	m_size		= 0;
	m_capacity	= startCapacity;
	m_bOwnsArray = true;
}

WgPatches::WgPatches( WgRect * pArray, int capacity )
{
	m_pFirst 	= pArray;
	m_size		= 0;
	m_capacity	= capacity;
	m_bOwnsArray = false;
}

//____ Destructor ______________________________________________________________

WgPatches::~WgPatches()
{
	if( m_bOwnsArray )
		delete [] m_pFirst;
}

//____ SetCapacity _____________________________________________________________

bool WgPatches::SetCapacity( int capacity )
{
	if( capacity < m_size )
		return false;
	
	WgRect * pNew = new WgRect[capacity];
	memcpy( pNew, m_pFirst, sizeof(WgRect)*m_size );
	if( m_bOwnsArray )
	{
		delete [] m_pFirst;
		m_bOwnsArray = false;
	}
	m_pFirst = pNew;
	m_capacity = capacity;
	return true;
}



void WgPatches::Add( const WgRect& rect )
{
}

void WgPatches::Add( const WgPatches * pSource, int ofs, int len )
{
}

void WgPatches::Sub( const WgRect& rect )
{
}

void WgPatches::Sub( const WgPatches * pSource, int ofs, int len )
{
}

//____ Push() __________________________________________________________________

int WgPatches::Push( const WgPatches * pSource, int ofs, int len )
{
	if( ofs > pSource->m_size )
		return 0;
		
	if( len > pSource->m_size - ofs )
		len = pSource->m_size - ofs;
	
	if( m_capacity - m_size < len )
		_expand(len);

	memcpy( m_pFirst + m_size, pSource->m_pFirst + ofs, len );
	m_size += len;
	return len;
}

//____ Clip() __________________________________________________________________

void WgPatches::Clip( const WgRect& clip )
{
	WgRect * pRect = m_pFirst;

	for( WgRect * pRect = m_pFirst ; pRect < m_pFirst + m_size ; pRect++ )
	{

		if( pRect->x < clip.x || pRect->y < clip.y ||
			pRect->x + pRect->w > clip.x + clip.w ||
			pRect->y + pRect->h > clip.y + clip.h )
		{
			if( !pRect->Intersection( *pRect, clip ) )
				*pRect-- = m_pFirst[--m_size];				// Delete the rectangle
		}
	}
}

//____ Union() _________________________________________________________________

WgRect WgPatches::Union() const
{
	if( m_size == 0 )
		return WgRect();

	int x1 = m_pFirst->x;
	int x2 = m_pFirst->x + m_pFirst->w;
	int y1 = m_pFirst->y;
	int y2 = m_pFirst->y + m_pFirst->h;
		
	for( int i = 1 ; i < m_size ; i++ )
	{
		if( m_pFirst[i].x < x1 )
			x1 = m_pFirst[i].x;
		if( m_pFirst[i].y < y1 )
			y1 = m_pFirst[i].y;
		if( m_pFirst[i].x + m_pFirst[i].w > x2 )
			x2 = m_pFirst[i].x + m_pFirst[i].w;
		if( m_pFirst[i].y + m_pFirst[i].h > y2 )
			y2 = m_pFirst[i].y + m_pFirst[i].h;
	}
	
	return WgRect(x1,y1,x2-x1,y2-y1);
}

//____ Repair() ________________________________________________________________

int WgPatches::Repair()
{
}

//____ Optimize() ______________________________________________________________

int WgPatches::Optimize()
{
}

//____ _expand() _______________________________________________________________

void WgPatches::_expand( int spaceNeeded )
{
	int capacity = m_capacity;
	
	while( capacity - m_size < spaceNeeded )
		capacity += capacity;

	SetCapacity(capacity);
}
