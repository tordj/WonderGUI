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


#include <wg_geo.h>

//____ WgRect() _______________________________________________________________
/**
 * @brief Initializes the rectangle to be the intersection of specified rectangles.
 * 
 * @param r1	First intersecting rectangle.
 * @param r2	Second intersecting rectangle.
 * 
 * If specified rectangles don't intersect, width and height will be set to zero.
 * 
 **/
WgRect::WgRect( const WgRect& r1, const WgRect& r2 )
{
	Intersection( r1, r2 );
}

/**
 * @brief	Initializes the rectangle to cover the area between the specified coordinates.
 * 
 * @param p1	Coordinate specifying any corner of the rectangle.
 * @param p2	Coordinate specifying the opposite corner of the rectangle, 
 * 				diagonally across the rectangle from p1.
 * 
 * The order of the parameters are irrelevant.
 * 
 **/

WgRect::WgRect( const WgCoord& p1, const WgCoord& p2 )
{
	if( p1.x < p2.x )
	{
		x = p1.x;
		w = p2.x - p1.x;
	}
	else
	{
		x = p2.x;
		w = p1.x - p2.x;
	}

	if( p1.y < p2.y )
	{
		y = p1.y;
		h = p2.y - p1.y;
	}
	else
	{
		y = p2.y;
		h = p1.y - p2.y;
	}
}


//____ Intersection() ________________________________________________________________
/**
 * @brief Sets the rectangle to be the intersection of specified rectangles.
 * 
 * @param r1	First intersecting rectangle.
 * @param r2	Second intersecting rectangle.
 * 
 * If specified rectangles don't intersect, width and height will be set to zero.
 * 
 * @return	True if the specified rectangles intersected.
 **/
 
bool WgRect::Intersection( const WgRect& _r1, const WgRect& _r2 )
{
	int		x1, y1;						// So we can use ourself as inparameter.
	int		x2, y2;

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
	{
		w = 0;
		h = 0;
		return	false;
	}

	return	true;
}

//____ Union() ________________________________________________________________
/**
 * @brief Returns the union of specified rectangles.
 * 
 * @param r1	First rectangle.
 * @param r2	Second rectangle.
 * 
 * The union of the rectangles is the smallest rectangle that contains both.
 * 
 * @return	A rectangle that is the union of the specified rectangles.
 **/

//TODO: Inconsistency between how Union and Intersection methods work. Both should be members of static.

WgRect WgRect::Union( const WgRect& r1, const WgRect& r2 )
{
	WgRect out;
	out.x = r1.x < r2.x ? r1.x : r2.x;
	out.y = r1.y < r2.y ? r1.y : r2.y;
	out.w = r1.x + r1.w > r2.x + r2.w ? r1.x + r1.w : r2.x + r2.w;
	out.h = r1.y + r1.h > r2.y + r2.h ? r1.y + r1.h : r2.y + r2.h;
	return out;
}


//____ Shrink() _____________________________________________________________
/**
 * @brief Shrinks the rectangle by the specified borders.
 * 
 * @param top	Width in pixels of top border.
 * @param right Width in pixels of right border.
 * @param bottom Width in pixels of bottom border.
 * @param left Width in pixels of left border.
 * 
 * Width and height of rectangle is shrunk by the thickness of the borders.
 * Top and left borders also affects the position of the rectangle.
 * 
 * The rectangle is constrained to a minimum width and height of 0.
 **/

void WgRect::Shrink( int top, int right, int bottom, int left )
{
	x += left;
	y += top;
	w -= right + left;
	h -= bottom + top;

	if( w < 0 )
		w = 0;

	if( h < 0 )
		h = 0;
}

/**
 * @brief Shrinks the rectangle by the specified border.
 * 
 * @param border	Border by which to shrink the rectangle.
 * 
 * Width and height of rectangle is shrunk by the thickness of the borders.
 * Top and left borders also affects the position of the rectangle.
 * 
 * The rectangle is constrained to a minimum width and height of 0.
 **/

void WgRect::Shrink(const WgBorder &_borders)
{
	x += _borders.left;
	y += _borders.top;
	w -= _borders.Width();
	h -= _borders.Height();

	if( w < 0 )
		w = 0;

	if( h < 0 )
		h = 0;
}

//____ Grow() _____________________________________________________________
/**
 * @brief Grows the rectangle by the specified borders.
 * 
 * @param top	Width in pixels of top border.
 * @param right Width in pixels of right border.
 * @param bottom Width in pixels of bottom border.
 * @param left Width in pixels of left border.
 * 
 * Width and height of rectangle is increased by the thickness of the borders.
 * Top and left borders also affects the position of the rectangle.
 **/

void WgRect::Grow(int top, int right, int bottom, int left)
{
	x -= left;
	y -= top;
	w += right + left;
	h += bottom + top;
}

/**
 * @brief Grows the rectangle by the specified border.
 * 
 * @param border	Border by which to grow the rectangle.
 * 
 * Width and height of rectangle is increased by the thickness of the borders.
 * Top and left borders also affects the position of the rectangle.
 **/
void WgRect::Grow(const WgBorder &_borders)
{
	x -= _borders.left;
	y -= _borders.top;
	w += _borders.Width();
	h += _borders.Height();
}

//____ GrowToContain() _______________________________________________________
/**
 * @brief	Grows the reactangle to contain the specified coordinate.
 * 
 * @param	x	Horizontal position of coordinate.
 * @param	y	Vertical position of coordinate.
 * 
 * Position of rectangle is affected if coordinate is above or left of coordinate.
 **/

void WgRect::GrowToContain( int _x, int _y )
{
	if( _x < x )
	{
		w += x - _x;
		x = _x;
	}
	else if( _x > x + w )
	{
		w = _x - x;
	}

	if( _y < y )
	{
		h += y - _y;
		y = _y;
	}
	else if( _y > y + h )
	{
		h = _y - y;
	}
}

/**
 * @brief	Grows the reactangle to contain the specified coordinate.
 * 
 * @param	coord	Coordinate the rectangle is grown to contain.
 * 
 * Position of rectangle is affected if coordinate is above or left of coordinate.
 **/

void WgRect::GrowToContain( const WgCoord& _coord )
{
	if( _coord.x < x )
	{
		w += x - _coord.x;
		x = _coord.x;
	}
	else if( _coord.x > x + w )
	{
		w = _coord.x - x;
	}

	if( _coord.y < y )
	{
		h += y - _coord.y;
		y = _coord.y;
	}
	else if( _coord.y > y + h )
	{
		h = _coord.y - y;
	}
}

/**
 * @brief	Grows the reactangle to contain the specified rectangle.
 * 
 * @param	r	Rectangle that shall be contained.
 * 
 * Position of our rectangle is affected if parameter rectangle is (partly or fully) above or left of coordinate.
 **/

void WgRect::GrowToContain( const WgRect& r )
{
	if( r.x < x )
	{
		w += x - r.x;
		x = r.x;
	}

	if( r.y < y )
	{
		h += y - r.y;
		y = r.y;
	}

	if( r.x + r.w > x + w )
	{
		w = r.x + r.w - x;
	}

	if( r.y + r.h > y + h )
	{
		h = r.y + r.h - y;
	}
}


WgRectF::WgRectF( const WgRectF& r1, const WgRectF& r2 )
{
	Intersection( r1, r2 );
}

//____ Intersection() ________________________________________________________________

bool WgRectF::Intersection( const WgRectF& _r1, const WgRectF& _r2 )
{
	float	x1, y1;						// So we can use ourself as inparameter.
	float	x2, y2;
	
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
	
	if( w <= 0.f || h <= 0.f )
	{
		w = 0.f;
		h = 0.f;
		return	false;
	}
	
	return	true;
}

