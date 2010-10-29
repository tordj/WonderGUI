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

#include	<wg_dirtyrect.h>
#include	<wg_mempool.h>

WgMemPool *	WgDirtyRectObj::g_pMemPool		= 0;
Sint32		WgDirtyRectObj::g_nDirtyRectObj	= 0;

void WgDirtyRectObj::Init()
{
	if( g_pMemPool == 0 )
		g_pMemPool = new WgMemPool(1024, sizeof(WgDirtyRect) );
}

void WgDirtyRectObj::Exit()
{
	delete g_pMemPool;
	g_pMemPool = 0;
}

//____ WgDirtyRectObj::WgDirtyRectObj() ___________________________________________

WgDirtyRectObj::WgDirtyRectObj( void )
{
	g_nDirtyRectObj++;
	pRectList = 0;
}

//____ WgDirtyRectObj::~WgDirtyRectObj() __________________________________________

WgDirtyRectObj::~WgDirtyRectObj( void )
{
	g_nDirtyRectObj--;

	Clear();
}

//____ WgDirtyRectObj::Clip() ___________________________________________________

void WgDirtyRectObj::Clip( const WgRect * _pClip )
{
	WgDirtyRect	* pRect = pRectList;

	while( pRect )
	{
		if( pRect->x < _pClip->x || pRect->y < _pClip->y ||
				pRect->x + pRect->w > _pClip->x + _pClip->w ||
				pRect->y + pRect->h > _pClip->y + _pClip->h )
		{

			if( !pRect->intersection( *pRect, *_pClip ) )
			{
				// Delete the rectangle

	 			WgDirtyRect	* pTmp = pRect;
	 			pRect = pRect->pNext;

				if( pRect )
					pRect->pPrev = pTmp->pPrev;

				if( pTmp->pPrev )
					pTmp->pPrev->pNext = pRect;
				else
					pRectList = pRect;
				g_pMemPool->freeEntry(pTmp);
			}
			else
				pRect = pRect->pNext;
		}
		else
			pRect = pRect->pNext;
	}
}


//____ WgDirtyRectObj::Add() ____________________________________________________

void WgDirtyRectObj::Add( const WgRect& rect )
{
	WgDirtyRect * pRect = (WgDirtyRect*) g_pMemPool->allocEntry();

	*((WgRect*) pRect) = rect;
	Addrect( pRect, pRectList );
}

void WgDirtyRectObj::Add( const int _x, const int _y, const int _w, const int _h )
{
	WgDirtyRect * pRect = (WgDirtyRect*) g_pMemPool->allocEntry();
	pRect->x = _x;
	pRect->y = _y;
	pRect->w = _w;
	pRect->h = _h;
	Addrect( pRect, pRectList );
}


//____ WgDirtyRectObj::Addrect() ________________________________________________

void	WgDirtyRectObj::Addrect( WgDirtyRect * pRect, WgDirtyRect * pOffset )
{
	WgDirtyRect * pReturn;


	// Optimization stuff

	while( pOffset != 0 )
	{
		pReturn =	ClipRectangles( pRect, pOffset );
		if( pReturn != 0 )
		{
			if( pReturn == pRect )
			{        																				// Totally covered by pOffset.
				g_pMemPool->freeEntry(pRect);
				return;
			}

			if( pReturn == pOffset )
			{                																// Delete pOffset.
				if( pOffset->pPrev == 0 )
					pRectList = pOffset->pNext;
				else
					pOffset->pPrev->pNext = pOffset->pNext;

				if( pOffset->pNext != 0 )
					pOffset->pNext->pPrev = pOffset->pPrev;

				WgDirtyRect * pOffset2 = pOffset->pNext;
				g_pMemPool->freeEntry(pOffset);
				pOffset = pOffset2;
			}
			else
			{
				Addrect( pReturn, pOffset->pNext );
				pOffset = pOffset->pNext;
			}


		}
		else
			pOffset = pOffset->pNext;
	}


	// Add to the list

	pRect->pNext = pRectList;
	pRect->pPrev = 0;

	if( pRectList != 0 )
		pRectList->pPrev = pRect;
	pRectList = pRect;
}


//____ WgDirtyRectObj::Transfer() _______________________________________________

void WgDirtyRectObj::Transfer( WgDirtyRectObj * pDest )
{
	WgDirtyRect * pRect = pRectList;
	WgDirtyRect	* pRect2;

	while( pRect != 0 )
	{
		pRect2 = pRect->pNext;
		pDest->Addrect( pRect, pDest->pRectList );
		pRect = pRect2;
  }
	pRectList = 0;
}


//____ WgDirtyRectObj::OneForAll() ______________________________________________

bool WgDirtyRectObj::OneForAll( WgRect * wpRect )
{
	if( pRectList == 0 )
		return	false;											// No rectangle returned.
	else
	{
		WgDirtyRect * pRect = pRectList;

		// Just fill in the first so we have something to start with.

		wpRect->x = pRect->x;
		wpRect->y = pRect->y;
		wpRect->w = pRect->x + pRect->w;			// Temporarily use w as x2 for performance reasons
		wpRect->h = pRect->y + pRect->h;			// " -             h as y2 " -
		pRect = pRect->pNext;

		// Go through the rest

		while( pRect )
		{
			if( pRect->x < wpRect->x )
				wpRect->x = pRect->x;
			if( pRect->y < wpRect->y )
				wpRect->y = pRect->y;
			if( pRect->x + pRect->w > wpRect->w )
				wpRect->w = pRect->x + pRect->w;
			if( pRect->y + pRect->h > wpRect->h )
				wpRect->h = pRect->y + pRect->h;
			pRect = pRect->pNext;
		}

		// x2, y2 -> w, h

		wpRect->w -= wpRect->x;
		wpRect->h -= wpRect->y;

		return	true;													// We did return a rectangle.
	}
}

//____ WgDirtyRectObj::Sub() ____________________________________________________

void WgDirtyRectObj::Sub( const int _x, const int _y, const int _w, const int _h )
{
	WgDirtyRect * pRect = pRectList;
//	WgDirtyRect	** wppRect = &pRectList;			// WritePointer to Pointer to rectangle...

	while( pRect != 0 )
	{
		// Do we have any kind of coverage?

		if( pRect->x + pRect->w > _x && pRect->y + pRect->h > _y && pRect->x < _x + _w
						 && pRect->y < _y + _h )
		{
   		// Only partially covered by our subtract rectangle?

   		if( _x > pRect->x || _y > pRect->y || _x + _w < pRect->x + pRect->w
   				|| _y + _h < pRect->y + pRect->h )
			{
				// Handles partial coverage. Up to 4 new rectangles are created for
				// the areas that aren't covered.

				if( pRect->y < _y )													// Top part
				{
					WgDirtyRect * pNew = (WgDirtyRect*) g_pMemPool->allocEntry();

					pNew->x = pRect->x;
					pNew->y = pRect->y;
					pNew->w = pRect->w;
					pNew->h = _y - pRect->y;

					pRectList->pPrev = pNew;									// This IS safe since we are here.
					pNew->pNext = pRectList;
					pNew->pPrev = 0;
					pRectList = pNew;
				}

				if( pRect->x < _x )													// Left part
				{
					WgDirtyRect * pNew = (WgDirtyRect*) g_pMemPool->allocEntry();

					pNew->x = pRect->x;
					pNew->w = _x - pRect->x;

					if( pRect->y > _y )
						pNew->y = pRect->y;
					else
						pNew->y = _y;

					if( pRect->y + pRect->h < _y + _h )
						pNew->h = pRect->y + pRect->h - pNew->y;
					else
						pNew->h = _y + _h - pNew->y;

					pRectList->pPrev = pNew;									// This IS safe since we are here.
					pNew->pNext = pRectList;
					pNew->pPrev = 0;
					pRectList = pNew;
				}

				if( pRect->x + pRect->w > _x + _w )					// Right part
				{
					WgDirtyRect * pNew = (WgDirtyRect*) g_pMemPool->allocEntry();

					pNew->x = _x + _w;
					pNew->w = pRect->x + pRect->w - ( _x + _w );

					if( pRect->y > _y )
						pNew->y = pRect->y;
					else
						pNew->y = _y;

					if( pRect->y + pRect->h < _y + _h )
						pNew->h = pRect->y + pRect->h - pNew->y;
					else
						pNew->h = _y + _h - pNew->y;

					pRectList->pPrev = pNew;									// This IS safe since we are here.
					pNew->pNext = pRectList;
					pNew->pPrev = 0;
					pRectList = pNew;
				}

				if( pRect->y + pRect->h > _y + _h )					// Bottom part
				{
					WgDirtyRect * pNew = (WgDirtyRect*) g_pMemPool->allocEntry();

					pNew->x = pRect->x;
					pNew->y = _y + _h;
					pNew->w = pRect->w;
					pNew->h = pRect->y + pRect->h - ( _y + _h );

					pRectList->pPrev = pNew;									// This IS safe since we are here.
					pNew->pNext = pRectList;
					pNew->pPrev = 0;
					pRectList = pNew;
				}
			}

			// Delete the old rectangle

 			WgDirtyRect	* pTmp = pRect;
 			pRect = pRect->pNext;

			if( pRect )
				pRect->pPrev = pTmp->pPrev;

			if( pTmp->pPrev )
				pTmp->pPrev->pNext = pRect;
			else
				pRectList = pRect;
			g_pMemPool->freeEntry(pTmp);
		}
		else
		{
			// No coverage at all, just go to next one.

			pRect = pRect->pNext;
		}
	}
}


//____ WgDirtyRectObj::ClipTransfer() ___________________________________________

void WgDirtyRectObj::ClipTransfer( WgDirtyRectObj * _pDest, const WgRect * _pClip )
{
	WgDirtyRect * pRect = pRectList;


	while( pRect != 0 )
	{
		// Do we have any kind of coverage?

		if( pRect->x + pRect->w > _pClip->x && pRect->y + pRect->h > _pClip->y &&
				pRect->x < _pClip->x + _pClip->w && pRect->y < _pClip->y + _pClip->h )
		{
   		// Only partially covered by our clip rectangle?

   		if( _pClip->x > pRect->x || _pClip->y > pRect->y || _pClip->x + _pClip->w < pRect->x + pRect->w
   				|| _pClip->y + _pClip->h < pRect->y + pRect->h )
			{
				// Handles partial coverage. Up to 4 new rectangles are created for
				// the areas that aren't covered.

				if( pRect->y < _pClip->y )													// Top part
				{
					WgDirtyRect * pNew = (WgDirtyRect*) g_pMemPool->allocEntry();

					pNew->x = pRect->x;
					pNew->y = pRect->y;
					pNew->w = pRect->w;
					pNew->h = _pClip->y - pRect->y;

					pRectList->pPrev = pNew;									// This IS safe since we are here.
					pNew->pNext = pRectList;
					pNew->pPrev = 0;
					pRectList = pNew;
				}

				if( pRect->x < _pClip->x )													// Left part
				{
					WgDirtyRect * pNew = (WgDirtyRect*) g_pMemPool->allocEntry();

					pNew->x = pRect->x;
					pNew->w = _pClip->x - pRect->x;

					if( pRect->y > _pClip->y )
						pNew->y = pRect->y;
					else
						pNew->y = _pClip->y;

					if( pRect->y + pRect->h < _pClip->y + _pClip->h )
						pNew->h = pRect->y + pRect->h - pNew->y;
					else
						pNew->h = _pClip->y + _pClip->h - pNew->y;

					pRectList->pPrev = pNew;									// This IS safe since we are here.
					pNew->pNext = pRectList;
					pNew->pPrev = 0;
					pRectList = pNew;
				}

				if( pRect->x + pRect->w > _pClip->x + _pClip->w )					// Right part
				{
					WgDirtyRect * pNew = (WgDirtyRect*) g_pMemPool->allocEntry();

					pNew->x = _pClip->x + _pClip->w;
					pNew->w = pRect->x + pRect->w - ( _pClip->x + _pClip->w );

					if( pRect->y > _pClip->y )
						pNew->y = pRect->y;
					else
						pNew->y = _pClip->y;

					if( pRect->y + pRect->h < _pClip->y + _pClip->h )
						pNew->h = pRect->y + pRect->h - pNew->y;
					else
						pNew->h = _pClip->y + _pClip->h - pNew->y;

					pRectList->pPrev = pNew;									// This IS safe since we are here.
					pNew->pNext = pRectList;
					pNew->pPrev = 0;
					pRectList = pNew;
				}

				if( pRect->y + pRect->h > _pClip->y + _pClip->h )					// Bottom part
				{
					WgDirtyRect * pNew = (WgDirtyRect*) g_pMemPool->allocEntry();

					pNew->x = pRect->x;
					pNew->y = _pClip->y + _pClip->h;
					pNew->w = pRect->w;
					pNew->h = pRect->y + pRect->h - ( _pClip->y + _pClip->h );

					pRectList->pPrev = pNew;									// This IS safe since we are here.
					pNew->pNext = pRectList;
					pNew->pPrev = 0;
					pRectList = pNew;
				}

				// Finally, clip the dirty rectangle itself.

				pRect->intersection( *pRect, *_pClip );
			}
			else																// Fully covered by our clip rectangle.
			{
				// Disconnect and add the rectangle to our destination

	 			WgDirtyRect	* pTmp = pRect;
	 			pRect = pRect->pNext;

				if( pRect )
					pRect->pPrev = pTmp->pPrev;

				if( pTmp->pPrev )
					pTmp->pPrev->pNext = pRect;
				else
					pRectList = pRect;

				_pDest->Addrect( pTmp, _pDest->pRectList );
			}
		}
		else
		{
			// No coverage at all, just go to next one.

			pRect = pRect->pNext;
		}
	}
}

//____ WgDirtyRectObj::Clear() __________________________________________________

void WgDirtyRectObj::Clear( void )
{
	WgDirtyRect * pRect = pRectList, * pRect2;

	while( pRect != 0 )
	{
		pRect2 = pRect->pNext;
		g_pMemPool->freeEntry(pRect);
		pRect = pRect2;
	}

	pRectList = 0;
}


//____ WgDirtyRectObj::ClipRectangles() _________________________________________

WgDirtyRect * WgDirtyRectObj::ClipRectangles( WgDirtyRect * p1, WgDirtyRect * p2 )
{
	// This function has a preference for clipping/splitting p1...

/*
		Return values:	0 = No rectangle has been created or completely disappeared.
												Both might have changed in size though...
									<>0 = Om adressen stämmer överens med antingen p1 eller p2 så ska
												den rektangeln tas bort, annars är det adressen till en ny
												rektangel som har skapats (mao, någon har splittrats i två).
*/

	// Check for intersection.

	if( p1->x >= p2->x + p2->w || p1->x + p1->w <= p2->x ||
			p1->y >= p2->y + p2->h || p1->y + p1->h <= p2->y )
		return	0;																							// No intersection.

	// Check for total coverage

	if( p1->x <= p2->x  &&  p1->x + p1->w >= p2->x + p2->w  &&
      p1->y <= p2->y  &&  p1->y + p1->h >= p2->y + p2->h  )
		return p2;																							// p2 totally covered by p1

	if( p1->x >= p2->x  &&  p1->x + p1->w <= p2->x + p2->w  &&
      p1->y >= p2->y  &&  p1->y + p1->h <= p2->y + p2->h  )
		return	p1;                                      				// p1 totally covered by p2


	// In four special cases we rather clip p2 than p1...

 	if( p1->x <= p2->x && p1->x + p1->w >= p2->x + p2->w )
 		{
 			if( p1->y <= p2->y && p1->y + p1->h > p2->y )
 			{
 				int diff = p1->y + p1->h - p2->y;
 				p2->y += diff;
 				p2->h -= diff;
				return	0;
 			}

 			if( p1->y < p2->y + p2->h && p1->y + p1->h >= p2->y + p2->h )
 			{
 				int diff = p2->y + p2->h - p1->y;
 				p2->h -= diff;
				return	0;
 			}

 		}
 		else if( p1->y <= p2->y && p1->y + p1->h >= p2->y + p2->h )
 		{
 			if( p1->x <= p2->x && p1->x + p1->w > p2->x )
 			{
 				int diff = p1->x + p1->w - p2->x;
 				p2->x += diff;
 				p2->w -= diff;
				return	0;
 			}

 			if( p1->x < p2->x + p2->w && p1->x + p1->w >= p2->x + p2->w )
  		{
  			int diff = p2->x + p2->w - p1->x;
 				p2->w -= diff;
				return	0;
 			}
 		}

		// Clip p1 against p2.

		WgDirtyRect * p3 = 0;

		// Cut off upper part

		if( p1->y < p2->y )
		{
			p3 = (WgDirtyRect*) g_pMemPool->allocEntry();
			p3->x = p1->x;
			p3->w = p1->w;
			p3->y = p1->y;
			p3->h = p2->y - p1->y;

			p1->h -= p3->h;
			p1->y += p3->h;
		}

		// Take care of middle part(s)

		if( p1->x < p2->x )
		{
			if( p3 == 0 )
			{
				p3 = (WgDirtyRect*) g_pMemPool->allocEntry();
				p3->x = p1->x;
				p3->y = p1->y;
				p3->w = p2->x - p1->x;
				if( p1->y + p1->h < p2->y + p2->h )
					p3->h = p1->h;
				else
					p3->h = p2->y + p2->h - p3->y;
			}
			else
			{
				p1->w = p2->x - p1->x;
				return	p3;
			}
		}

		if( p1->x + p1->w > p2->x + p2->w )
		{
			if( p3 == 0 )
			{
				p3 = (WgDirtyRect*) g_pMemPool->allocEntry();
				p3->x = p2->x + p2->w;
				p3->y = p1->y;
				p3->w = p1->x + p1->w - (p2->x + p2->w);
				if( p1->y + p1->h < p2->y + p2->h )
					p3->h = p1->h;
				else
					p3->h = p2->y + p2->h - p3->y;
			}
			else
			{
				p1->w = p1->x + p1->w - (p2->x + p2->w);
				p1->x = p2->x + p2->w;
				return	p3;
			}
		}

		// Take care of lower part

		if( p1->y + p1->h > p2->y + p2->h )
		{
			p1->h = p1->y + p1->h - (p2->y + p2->h);
			p1->y = p2->y + p2->h;
			return	p3;
		}

		// If we got here, we certainly have p3 and just trash in p1...

		p1->x = p3->x;
		p1->y = p3->y;
		p1->w = p3->w;
		p1->h = p3->h;
		g_pMemPool->freeEntry(p3);
		return	0;
}
