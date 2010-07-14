#ifndef	WG_RECT_DOT_H
#define	WG_RECT_DOT_H
//=============================================================================

#include "wg_types.h"

//#include <d3d9.h>

//____ Class WgRect ___________________________________________________________

class WgRect
{
public:
	WgRect() {};
	WgRect( Sint32 x, Sint32 y, Sint32 w, Sint32 h );
	WgRect( const WgRect& r );
	WgRect( const WgRect& r1, const WgRect& r2 );

	inline void operator=( const WgRect& );
//	inline void operator=( const RECT& );
//	inline operator RECT() const;
	
	bool Union( const WgRect& r1, const WgRect& r2 );

	inline bool Contains( Sint32 x, Sint32 y ) const;

	Sint32	x, y, w, h;
};

//____ Contains() _____________________________________________________________

inline bool WgRect::Contains( Sint32 _x, Sint32 _y ) const
{
	if( _x >= x && _x < x + w && _y >= y && _y < y + h )
		return true;
	return false;
}


//____ operators ______________________________________________________________

inline void WgRect::operator=( const WgRect& r2 )
{
	x = r2.x;
	y = r2.y;
	w = r2.w;
	h = r2.h;
}

/*
inline void WgRect::operator=( const RECT& r2)
{
	x = r2.left;
	y = r2.top;
	w = r2.right;
	h = r2.bottom;
}

inline WgRect::operator RECT() const
{
	RECT r;

	r.left   = x;
	r.top    = y;
	r.right  = w;
	r.bottom = h;

	return	r;
}
*/

inline bool operator==( const WgRect& r1, const WgRect& r2 )
{
	if( r1.x == r2.x && r1.y == r2.y && r1.w == r2.w && r1.h == r2.h )
		return true;
	return false;
}

inline bool operator!=( const WgRect& r1, const WgRect& r2 )
{
	if( r1.x == r2.x && r1.y == r2.y && r1.w == r2.w && r1.h == r2.h )
		return false;
	return true;
}


//=======================================================================================
#endif	//WG_RECT_DOT_H
