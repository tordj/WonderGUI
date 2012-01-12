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

#include <memory.h>
#include <wg_patches.h>

//____ Constructor _____________________________________________________________

WgPatches::WgPatches()
{
	m_pFirst	= 0;
	m_size		= 0;
	m_capacity	= 0;
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

//____ SetCapacity() ___________________________________________________________

bool WgPatches::SetCapacity( int capacity )
{
	if( capacity < m_size )
		return false;
	
	WgRect * pNew = 0;
	if( capacity > 0 )
	{
		pNew = new WgRect[capacity];
		memcpy( pNew, m_pFirst, sizeof(WgRect)*m_size );
	}
	
	if( m_bOwnsArray )
	{
		delete [] m_pFirst;
		m_bOwnsArray = false;
	}
	
	m_pFirst = pNew;
	m_capacity = capacity;
	return true;
}

//____ _add() __________________________________________________________________

void WgPatches::_add( const WgRect& rect, int startOffset )
{
	WgRect newR = rect;
	
	for( int i = startOffset ; i < m_size ; i++ )
	{
		WgRect * pR = m_pFirst + i;

		// Bail out early if no intersection at all.

		if( newR.x >= pR->x + pR->w || newR.x + newR.w <= pR->x ||
			newR.y >= pR->y + pR->h || newR.y + newR.h <= pR->y )
			continue;															// No intersection.
		
		// Check for total coverage

		if( newR.x >= pR->x  &&  newR.x + newR.w <= pR->x + pR->w  &&
		  newR.y >= pR->y  &&  newR.y + newR.h <= pR->y + pR->h  )
			return;                                      						// newR totally covered by pR

		if( newR.x <= pR->x  &&  newR.x + newR.w >= pR->x + pR->w  &&
		  newR.y <= pR->y  &&  newR.y + newR.h >= pR->y + pR->h  )
		{
			Delete( i-- );														// pR totally covered by newR
			continue;
		}

		// In four special cases we rather clip pR than newR...

		if( newR.x <= pR->x && newR.x + newR.w >= pR->x + pR->w )
 		{
 			if( newR.y <= pR->y && newR.y + newR.h > pR->y )
 			{
 				int diff = newR.y + newR.h - pR->y;
 				pR->y += diff;
 				pR->h -= diff;
				continue;
 			}

 			if( newR.y < pR->y + pR->h && newR.y + newR.h >= pR->y + pR->h )
 			{
 				int diff = pR->y + pR->h - newR.y;
 				pR->h -= diff;
				continue;
 			}

 		}
 		else if( newR.y <= pR->y && newR.y + newR.h >= pR->y + pR->h )
 		{
 			if( newR.x <= pR->x && newR.x + newR.w > pR->x )
 			{
 				int diff = newR.x + newR.w - pR->x;
 				pR->x += diff;
 				pR->w -= diff;
				continue;
 			}

 			if( newR.x < pR->x + pR->w && newR.x + newR.w >= pR->x + pR->w )
			{
				int diff = pR->x + pR->w - newR.x;
 				pR->w -= diff;
				continue;
 			}
 		}
		
		// Clip newR against pR.

		WgRect	xR;
		bool	bExtraRect = false;

		// Cut off upper part

		if( newR.y < pR->y )
		{
			bExtraRect = true;
			xR.x = newR.x;
			xR.w = newR.w;
			xR.y = newR.y;
			xR.h = pR->y - newR.y;

			newR.h -= xR.h;
			newR.y += xR.h;
		}

		// Take care of middle part(s)

		if( newR.x < pR->x )
		{
			if( !bExtraRect )
			{
				bExtraRect = true;
				xR.x = newR.x;
				xR.y = newR.y;
				xR.w = pR->x - newR.x;
				if( newR.y + newR.h < pR->y + pR->h )
					xR.h = newR.h;
				else
					xR.h = pR->y + pR->h - xR.y;
			}
			else
			{
				newR.w = pR->x - newR.x;
				_add( xR, i+1 );
				continue;
			}
		}

		if( newR.x + newR.w > pR->x + pR->w )
		{
			if( !bExtraRect )
			{
				bExtraRect = true;
				xR.x = pR->x + pR->w;
				xR.y = newR.y;
				xR.w = newR.x + newR.w - (pR->x + pR->w);
				if( newR.y + newR.h < pR->y + pR->h )
					xR.h = newR.h;
				else
					xR.h = pR->y + pR->h - xR.y;
			}
			else
			{
				newR.w = newR.x + newR.w - (pR->x + pR->w);
				newR.x = pR->x + pR->w;
				_add( xR, i+1 );
				continue;
			}
		}

		// Take care of lower part

		if( newR.y + newR.h > pR->y + pR->h )
		{
			newR.h = newR.y + newR.h - (pR->y + pR->h);
			newR.y = pR->y + pR->h;
			_add( xR, i+1 );
			continue;
		}

		// If we got here, we certainly have xR and just trash in newR...

		newR = xR;
	}
	
	// If we haven't returned yet we have a patch left to add.
	
	Push( newR );	
}

//____ Add() ___________________________________________________________________

void WgPatches::Add( const WgPatches * pSource, int ofs, int len )
{
	if( ofs > pSource->m_size )
		return;
		
	if( len > pSource->m_size - ofs )
		len = pSource->m_size - ofs;

	for( int i = 0 ; i < len ; i++ )
		_add( pSource->m_pFirst[ofs++], 0 );
}

//____ Sub() ___________________________________________________________________

void WgPatches::Sub( const WgPatches * pSource, int ofs, int len )
{
	if( ofs > pSource->m_size )
		return;
		
	if( len > pSource->m_size - ofs )
		len = pSource->m_size - ofs;

	for( int i = 0 ; i < len ; i++ )
		Sub( pSource->m_pFirst[ofs++] );	
}

void WgPatches::Sub( const WgRect& rect )
{
	if( rect.w == 0 || rect.h == 0 )
		return;

	for( int i = 0 ; i < m_size ; i++ )
	{
		WgRect * pR = m_pFirst + i;
				
		if( !(pR->x + pR->w > rect.x && pR->y + pR->h > rect.y && 
			pR->x < rect.x + rect.w && pR->y < rect.y + rect.h) )
			continue;												// No overlap at all.
			
   		// Only partially covered by our subtract rectangle?

   		if( rect.x > pR->x || rect.y > pR->y || rect.x + rect.w < pR->x + pR->w
   				|| rect.y + rect.h < pR->y + pR->h )
		{
			// Handles partial coverage. Up to 4 new rectangles are created for
			// the areas that aren't covered.

			if( pR->y < rect.y )													// Top part
			{
				Push( WgRect( pR->x, pR->y, pR->w, rect.y - pR->y) );				// Not optimal, will cause unnecessary processing later.
			}

			if( pR->x < rect.x )													// Left part
			{
				WgRect newR;

				newR.x = pR->x;
				newR.w = rect.x - pR->x;

				if( pR->y > rect.y )
					newR.y = pR->y;
				else
					newR.y = rect.y;

				if( pR->y + pR->h < rect.y + rect.h )
					newR.h = pR->y + pR->h - newR.y;
				else
					newR.h = rect.y + rect.h - newR.y;
					
					Push( newR );													// Not optimal, will cause unnecessary processing later.
			}

			if( pR->x + pR->w > rect.x + rect.w )					// Right part
			{
				WgRect newR;

				newR.x = rect.x + rect.w;
				newR.w = pR->x + pR->w - ( rect.x + rect.w );

				if( pR->y > rect.y )
					newR.y = pR->y;
				else
					newR.y = rect.y;

				if( pR->y + pR->h < rect.y + rect.h )
					newR.h = pR->y + pR->h - newR.y;
				else
					newR.h = rect.y + rect.h - newR.y;

					Push( newR );													// Not optimal, will cause unnecessary processing later.
			}

			if( pR->y + pR->h > rect.y + rect.h )					// Bottom part
			{
				Push( WgRect( pR->x, 
							  rect.y + rect.h, 
							  pR->w, 
							  pR->y + pR->h - ( rect.y + rect.h )) );				// Not optimal, will cause unnecessary processing later.
			}
		}

		// Delete the old rectangle

		Delete( i-- );
	}
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

//____ Delete() ________________________________________________________________

void  WgPatches::Delete( int ofs )
{
	if( ofs >= m_size )
		return;
		
	m_pFirst[ofs] = m_pFirst[--m_size];
}

int WgPatches::Delete( int ofs, int len )
{
	if( ofs > m_size )
		return 0;
		
	if( len > m_size - ofs )
		len = m_size - ofs;


	int src = m_size - len;
	if( src < ofs + len )
		src = ofs + len;

	while( src < m_size )
		m_pFirst[ofs++] = m_pFirst[src++];

	m_size -= len;
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
	//TODO: Implement
}

//____ Optimize() ______________________________________________________________

int WgPatches::Optimize()
{
	//TODO: Implement
}

//____ _expand() _______________________________________________________________

void WgPatches::_expand( int spaceNeeded )
{
	int capacity = m_capacity==0?c_defaultCapacity:m_capacity;
	
	
	while( capacity - m_size < spaceNeeded )
		capacity += capacity;

	SetCapacity(capacity);
}
