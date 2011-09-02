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

#ifndef	WG_RECTCHAIN_DOT_H
#define	WG_RECTCHAIN_DOT_H

#ifndef	WG_GEO_DOT_H
#	include	<wg_geo.h>
#endif

class WgMemPool;

class	WgRectLink:public WgRect
{
	friend class WgRectChain;

	WgRectLink() {}
	~WgRectLink() {}
public:

	//	int					x, y, w, h;
	WgRectLink		* pPrev, * pNext;
};

class	WgRectChain
{
public:
	static void Init();
	static void Exit();

	WgRectChain();
	~WgRectChain();

	static void Destroy( WgRectLink * pRect );		// Only for destroying dirty rects not belonging to any DirtyRectObj!

	void		Add( const WgRect& rect );
	inline void	Sub( const WgRect& rect ) { Sub( rect.x, rect.y, rect.w, rect.h ); }

	void	Add( const int _x, const int _y, const int _w, const int _h );
	void 	Sub( const int _x, const int _y, const int _w, const int _h );
	void	Transfer( WgRectChain * _pDest );						// Move rectangles to this container.
	void	ClipTransfer( WgRectChain * _pDest, const WgRect * _pClip );	// Transfer all that is within clip
																																	// rectangle to pDest.
	void	Clip( const WgRect * _pClip );

	void	Clear( void );
	bool	OneForAll( WgRect * _wpRect );		// Generates the smallest possible rectangle
												// Containing all rectangles (or returns false).

	WgRectLink *	Pop();
	inline void		PushExistingRect( WgRectLink * pRect ) { Addrect( pRect, pRectList ); }

	WgRectLink * 	pRectList;
private:
	WgRectLink * 	ClipRectangles( WgRectLink * _p1, WgRectLink * _p2 );
	void			Addrect( WgRectLink * _pRect, WgRectLink * _pOffset );

	static	WgMemPool *	g_pMemPool;
	static	int		g_nRectChains;
};


#endif	// WG_RECTCHAIN_DOT_H
