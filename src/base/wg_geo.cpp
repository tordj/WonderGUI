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

#include <utility>

namespace wg 
{
	
	//____ Rect() _______________________________________________________________
	/**
	 * Create rectangle from intersection of specified rectangles.
	 *
	 * @param r1	First intersecting rectangle.
	 * @param r2	Second intersecting rectangle.
	 * 
	 * If specified rectangles don't intersect, width and height will be set to zero.
	 * 
	 **/
	Rect::Rect( const Rect& r1, const Rect& r2 )
	{
		intersection( r1, r2 );
	}
	
	/**
	 * Create rectangle to cover the area between the specified coordinates.
	 *
	 * @param p1	Coordinate specifying any corner of the rectangle.
	 * @param p2	Coordinate specifying the opposite corner of the rectangle, 
	 * 				diagonally across the rectangle from p1.
	 * 
	 * The order of the parameters are irrelevant.
	 * 
	 **/
	
	Rect::Rect( const Coord& p1, const Coord& p2 )
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
	
	
	//____ intersection() ________________________________________________________________
	/**
	 * @brief Set rectangle to intersection of specified rectangles.
	 *
	 * Set rectangle to intersection of specified rectangles.
	 *
	 * @param r1	First intersecting rectangle.
	 * @param r2	Second intersecting rectangle.
	 * 
	 * If specified rectangles don't intersect, width and height will be set to zero.
	 * 
	 * @return	True if the specified rectangles intersected.
	 **/
	 
	bool Rect::intersection( const Rect& _r1, const Rect& _r2 )
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
	
	//____ getUnion() ________________________________________________________________
	/**
	 * @brief Get the union of specified rectangles.
	 *
	 * Get the union of specified rectangles.
	 *
	 * @param r1	First rectangle.
	 * @param r2	Second rectangle.
	 * 
	 * The union of the rectangles is the smallest rectangle that contains both.
	 * 
	 * @return	A rectangle that is the union of the specified rectangles.
	 **/
	
	//TODO: Inconsistency between how Union and Intersection methods work. Both should be members of static.
	
	Rect Rect::getUnion( const Rect& r1, const Rect& r2 )
	{
		Rect out;
		out.x = r1.x < r2.x ? r1.x : r2.x;
		out.y = r1.y < r2.y ? r1.y : r2.y;
		out.w = (r1.x + r1.w > r2.x + r2.w ? r1.x + r1.w : r2.x + r2.w) - out.x;
		out.h = (r1.y + r1.h > r2.y + r2.h ? r1.y + r1.h : r2.y + r2.h) - out.y;
		return out;
	}
	
	
	//____ shrink() _____________________________________________________________
	/**
	 * @brief Shrink the rectangle by the specified borders.
	 *
	 * Shrink the rectangle by the specified borders.
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
	
	void Rect::shrink( int top, int right, int bottom, int left )
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
	 * @brief Shrink the rectangle by the specified border.
	 *
	 * Shrink the rectangle by the specified border.
	 *
	 * @param border	Border by which to shrink the rectangle.
	 * 
	 * Width and height of rectangle is shrunk by the thickness of the borders.
	 * Top and left borders also affects the position of the rectangle.
	 * 
	 * The rectangle is constrained to a minimum width and height of 0.
	 **/
	
	void Rect::shrink(const Border &_borders)
	{
		x += _borders.left;
		y += _borders.top;
		w -= _borders.width();
		h -= _borders.height();
	
		if( w < 0 )
			w = 0;
	
		if( h < 0 )
			h = 0;
	}
	
	//____ grow() _____________________________________________________________
	/**
	 * @brief Grow the rectangle by the specified borders.
	 *
	 * Grow the rectangle by the specified borders.
	 *
	 * @param top	Width in pixels of top border.
	 * @param right Width in pixels of right border.
	 * @param bottom Width in pixels of bottom border.
	 * @param left Width in pixels of left border.
	 * 
	 * Width and height of rectangle is increased by the thickness of the borders.
	 * Top and left borders also affects the position of the rectangle.
	 **/
	
	void Rect::grow(int top, int right, int bottom, int left)
	{
		x -= left;
		y -= top;
		w += right + left;
		h += bottom + top;
	}
	
	/**
	 * @brief Grow the rectangle by the specified border.
	 *
	 * Grow the rectangle by the specified border.
	 *
	 * @param border	Border by which to grow the rectangle.
	 * 
	 * Width and height of rectangle is increased by the thickness of the borders.
	 * Top and left borders also affects the position of the rectangle.
	 **/
	void Rect::grow(const Border &_borders)
	{
		x -= _borders.left;
		y -= _borders.top;
		w += _borders.width();
		h += _borders.height();
	}
	
	//____ growToContain() _______________________________________________________
	/**
	 * @brief	Grow the reactangle to contain the specified coordinate.
	 *
	 * Grow the reactangle to contain the specified coordinate.
	 *
	 * @param	x	Horizontal position of coordinate.
	 * @param	y	Vertical position of coordinate.
	 * 
	 * Position of rectangle is affected if coordinate is above or left of coordinate.
	 **/
	
	void Rect::growToContain( int _x, int _y )
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
	 * @brief	Grow the reactangle to contain the specified coordinate.
	 *
	 * Grow the reactangle to contain the specified coordinate.
	 *
	 * @param	coord	Coordinate the rectangle is grown to contain.
	 * 
	 * Position of rectangle is affected if coordinate is above or left of coordinate.
	 **/
	
	void Rect::growToContain( const Coord& _coord )
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
	 * @brief	Grow the reactangle to contain the specified rectangle.
	 *
	 * Grow the reactangle to contain the specified rectangle.
	 *
	 * @param	r	Rectangle that shall be contained.
	 * 
	 * Position of our rectangle is affected if parameter rectangle is (partly or fully) above or left of coordinate.
	 **/
	
	void Rect::growToContain( const Rect& r )
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
	


	//____ intersectsWithOrContains() _______________________________________________________

	bool Rect::intersectsWithOrContains(Coord p1, Coord p2, int precision) const
	{
		// NOTE: This needs to be kept in sync with ClipLine() so they always return same result for same input.

		int x1 = x;
		int x2 = x + w;

		// We go from left to right when working with the X coordinates

		if (p2.x < p1.x)
			std::swap(p1, p2);

		// Verify that x-coordinates are within range

		if (p1.x >= x2 || p2.x <= x1)
			return false;

		// Calculate angles now before we destroy any values

		int angleX = ((p2.y - p1.y) << precision) / (1 + p2.x - p1.x);		// Change in Y for each increase of X.

																			// Clip line so no coord is outside rectangles x-dimensions

		if (p1.x < x1)
			p1.y = p1.y + ((x1 - p1.x)*angleX >> precision);

		if (p2.x > x2)
			p2.y = p2.y - ((p2.x - x2)*angleX >> precision);

		// We go from top to bottom when working with the Y coordinates

		if (p2.y < p1.y)
			std::swap(p1.y, p2.y);

		// Verify that clipped line intersects in y-dimension.

		if (p1.y >= y + h || p2.y <= y)
			return false;

		return true;
	}

	//____ clipLine() _____________________________________________________________

	bool Rect::clipLine(Coord * p1, Coord* p2, int precision) const
	{
		// NOTE: This needs to be kept in sync with IntersectsOrContains() so they always return same result for same input.

		int x1 = x;
		int x2 = x + w;
		int y1 = y;
		int y2 = y + h;

		// We go from left to right when working with the X coordinates

		if (p2->x < p1->x)
			std::swap(p1, p2);

		// Verify that x-coordinates are within range

		if (p1->x >= x2 || p2->x <= x1)
			return false;

		// Calculate both angles now before we destroy precision

		int angleX = ((p2->y - p1->y) << precision) / (1 + p2->x - p1->x);		// Change in Y for each increase of X.
		int angleY = ((p2->x - p1->x) << precision) / (1 + abs(p2->y - p1->y));		// Change in X for each increase of Y.

																					// Clip line so no coord is outside rectangles x-dimensions

		if (p1->x < x1)
		{
			p1->y = p1->y + ((x1 - p1->x)*angleX >> precision);
			p1->x = x2;
		}

		if (p2->x > x2)
		{
			p2->y = p2->y - ((p2->x - x2)*angleX >> precision);
			p2->x = x2;
		}

		// We go from top to bottom when working with the Y coordinates

		if (p2->y < p1->y)
		{
			std::swap(p1, p2);
			angleY = -angleY;
		}

		// Verify that clipped line intersects in y-dimension.

		if (p1->y >= y2 || p2->y <= y1)
			return false;

		// Clip line so no coord is outside rectangles y-dimensions

		if (p1->y < y1)
		{
			p1->x = p1->x + ((y1 - p1->y)*angleY >> precision);
			p1->y = y1;
		}

		if (p2->y > y2)
		{
			p2->x = p2->x - ((p2->y - y2)*angleY >> precision);
			p2->y = y2;
		}

		return true;
	}

	//____ intersection() ________________________________________________________________

	RectF::RectF( const RectF& r1, const RectF& r2 )
	{
		intersection( r1, r2 );
	}
		
	bool RectF::intersection( const RectF& _r1, const RectF& _r2 )
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
	

} // namespace wg
