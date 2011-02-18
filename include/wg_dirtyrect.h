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

#ifndef	WG_DIRTYRECT_DOT_H
#define	WG_DIRTYRECT_DOT_H

#ifndef	WG_GEO_DOT_H
#	include	<wg_geo.h>
#endif

class WgMemPool;

class	WgDirtyRect:public WgRect
{
	friend class WgDirtyRectObj;

	WgDirtyRect() {}
	~WgDirtyRect() {}
public:
	static void Destroy( WgDirtyRect * pRect );		// Only call if rect is not connected!!!
	
	//	int					x, y, w, h;
	WgDirtyRect		* pPrev, * pNext;
};

class	WgDirtyRectObj
{
public:
	static void Init();
	static void Exit();

	WgDirtyRectObj();
	~WgDirtyRectObj();

	void		Add( const WgRect& rect );
	inline void	Sub( const WgRect& rect ) { Sub( rect.x, rect.y, rect.w, rect.h ); }

	void	Add( const int _x, const int _y, const int _w, const int _h );
	void 	Sub( const int _x, const int _y, const int _w, const int _h );
	void	Transfer( WgDirtyRectObj * _pDest );						// Move dirty rectangles to this container.
	void	ClipTransfer( WgDirtyRectObj * _pDest, const WgRect * _pClip );	// Transfer all that is within clip
																																	// rectangle to pDest.
	void	Clip( const WgRect * _pClip );

	void	Clear( void );
	bool	OneForAll( WgRect * _wpRect );		// Generates the smallest possible rectangle
												// Containing all dirty rectangles (or returns false).

	WgDirtyRect *	Pop();
	inline void		PushExistingRect( WgDirtyRect * pRect ) { Addrect( pRect, pRectList ); }

	WgDirtyRect * 	pRectList;
private:
	WgDirtyRect * 	ClipRectangles( WgDirtyRect * _p1, WgDirtyRect * _p2 );
	void			Addrect( WgDirtyRect * _pRect, WgDirtyRect * _pOffset );

	static	WgMemPool *	g_pMemPool;
	static	Sint32		g_nDirtyRectObj;
};


#endif	// WG_DIRTYRECT_DOT_H
