#include <wg_rect.h>

//#include <mss.h>					// SHOULD ALWAYS BE THE LAST INCLUDE!!!

//____ WgRect::WgRect() _______________________________________________________

WgRect::WgRect( Sint32 _x, Sint32 _y, Sint32 _w, Sint32 _h )
{
	x = _x;
	y = _y;
	w = _w;
	h = _h;
}

WgRect::WgRect( const WgRect& r )
{
	x = r.x;
	y = r.y;
	w = r.w;
	h = r.h;
}

WgRect::WgRect( const WgRect& r1, const WgRect& r2 )
{
	Union( r1, r2 );
}



//____ WgRect::Union() ________________________________________________________

bool WgRect::Union( const WgRect& _r1, const WgRect& _r2 )
{
	Sint32		x1, y1;						// So we can use ourself as inparameter.
	Sint32		x2, y2;

	if( _r1.x > _r2.x )
		x1 = _r1.x;
	else
		x1 = _r2.x;

	if( _r1.y > _r2.y )
		y1 = _r1.y;
	else
		y1 = _r2.y;

	if( _r1.x + _r1.w < _r2.x + _r2.w )
		x2 = _r1.x + _r1.w;
	else
		x2 = _r2.x + _r2.w;

	if( _r1.y + _r1.h < _r2.y + _r2.h )
		y2 = _r1.y + _r1.h;
	else
		y2 = _r2.y + _r2.h;

	x = x1;
	y = y1;
	w = x2 - x;
	h = y2 - y;

	if( w <= 0 || h <= 0 )
		return	false;

	return	true;	
}

