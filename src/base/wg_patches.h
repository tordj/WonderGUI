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
#ifndef WG_PATCHES_DOT_H
#define WG_PATCHES_DOT_H

#include <limits.h>

#ifndef WG_GEO_DOT_H
#	include <wg_geo.h>
#endif

class WgPatches
{
public:
	
	WgPatches();
	WgPatches( int startCapacity );
	WgPatches( WgRect * pArray, int capacity );
	~WgPatches();

	bool			SetCapacity( int capacity );
	bool			SetArray( WgRect * pArray, int capacity );

	inline void		Add( const WgRect& rect ) { if( rect.w > 0 && rect.h > 0 ) _add( rect, 0 ); }						// Adds the area
	void			Add( const WgPatches * pSource, int ofs = 0, int len = INT_MAX );
	
	void			Sub( const WgRect& rect );											// Subtracts the area
	void			Sub( const WgPatches * pSource, int ofs = 0, int len = INT_MAX );

	inline void		Push( const WgRect& rect );											// Adds the rect (no optimizations, overlap may occur).
	int				Push( const WgPatches * pSource, int ofs = 0, int len = INT_MAX );
	inline WgRect	Pop();																// Pops last rect from the patches.

	void			Delete( int ofs );													// Deletes specific rect from the patches.
	int				Delete( int ofs, int len );											// Deletes range of rects from the patches.

	inline void		Clear() { m_size = 0; }
	void			Clip( const WgRect& clip );
	WgRect			Union() const;

	int				Repair();															// Fixes any overlap that might have resulted from Push()
	int				Optimize();															// Combines small patches into larger ones where possible

	const WgRect *	Begin() const { return m_pFirst; }
	const WgRect *	End() const { return m_pFirst + m_size; }
	int				Size() const { return m_size; }
	int				Capacity() const { return m_capacity; }
	bool			IsEmpty() const { return (m_size == 0); }

private:
	const static int	c_defaultCapacity = 64;

	void		_add( const WgRect& rect, int startOffset );
	void		_expandMem( int spaceNeeded );

	WgRect * 	m_pFirst;
	int			m_size;
	int			m_capacity;
	bool		m_bOwnsArray;
};



void WgPatches::Push( const WgRect& rect ) 
{ 
	if(m_size==m_capacity)
		_expandMem(1);
	m_pFirst[m_size++]=rect;
}

WgRect WgPatches::Pop() 
{ 
	if( m_size>0 )
		return m_pFirst[--m_size];
	else
		return WgRect(); 
}


#endif //WG_PATCHES_DOT_H
