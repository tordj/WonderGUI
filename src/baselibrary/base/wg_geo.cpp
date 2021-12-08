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
#include <algorithm>
#include <cstdlib>

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
	Rect::Rect(const Rect& r1, const Rect& r2)
	{
		intersection(r1, r2);
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

	Rect::Rect(const Coord& p1, const Coord& p2)
	{
		if (p1.x < p2.x)
		{
			x = p1.x;
			w = p2.x - p1.x;
		}
		else
		{
			x = p2.x;
			w = p1.x - p2.x;
		}

		if (p1.y < p2.y)
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

	//_____________________________________________________________________________
	/**
	 * Check if given coordinate is within rectangle.
	 *
	 * @param coord Coordinate to check.
	 *
	 * @return True if coordinate is within the bounds of the rectangle.
	 **/
	bool Rect::contains(const Coord& coord) const
	{
		//		int a = coord.x - x;
		//		int b = coord.y - y;
		//		int c = (x + w) - (coord.x);
		//		int d = (y + h) - (coord.y);

		//		return (a | b | c | d) >= 0;

		if (coord.x >= x && coord.x < x + w && coord.y >= y && coord.y < y + h)
			return true;
		return false;
	}

	//_____________________________________________________________________________
	/**
	 * Check if parameter rectangle is fully within our rectangle.
	 *
	 * @param rect Rectangle to check.
	 *
	 * @return True if the parameter rectangle is fully within the bounds of our rectangle.
	 **/
	bool Rect::contains(const Rect& rect) const
	{
		//		int a = rect.x - x;
		//		int b = rect.y - y;
		//		int c = (x + w) - (rect.x + rect.w);
		//		int d = (y + h) - (rect.y + rect.h);

		//		return (a | b | c | d) >= 0;

		if (rect.x >= x && rect.y >= y && rect.x + rect.w <= x + w && rect.y + rect.h <= y + h)
			return true;
		return false;
	}

	//_____________________________________________________________________________
	/**
	 * Limit given coordinate to stay within rectangle.
	 *
	 * @param coord Coordinate to limit to the bounds of the rectangle.
	 *
	 * @return Coordinate limited to the rectangles geometry.
	 **/
	Coord Rect::limit(const Coord& coord) const
	{
		Coord out = coord;
		if (out.x < x)
			out.x = x;
		if (out.y < y)
			out.y = y;
		if (out.x > x + w)
			out.x = x + w;
		if (out.y > y + h)
			out.y = y + h;

		return out;
	}


	//_____________________________________________________________________________
	/**
	 * Check for intersection (partial or full overlap) with specified rectangle.
	 *
	 * @param rect Rectangle to check against.
	 *
	 * @return True if rectangles intersects.
	 **/

	bool Rect::intersectsWith(const Rect& rect) const
	{
		if (left() >= rect.right()) return false;
		if (right() <= rect.left()) return false;
		if (top() >= rect.bottom()) return false;
		if (bottom() <= rect.top()) return false;

		return true;
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

	bool Rect::intersection(const Rect& r1, const Rect& r2)
	{
		MU		x1, y1;						// So we can use ourself as inparameter.
		MU		x2, y2;

		if (r1.x > r2.x)
			x1 = r1.x;
		else
			x1 = r2.x;

		if (r1.y > r2.y)
			y1 = r1.y;
		else
			y1 = r2.y;

		if (r1.x + r1.w < r2.x + r2.w)
			x2 = r1.x + r1.w;
		else
			x2 = r2.x + r2.w;

		if (r1.y + r1.h < r2.y + r2.h)
			y2 = r1.y + r1.h;
		else
			y2 = r2.y + r2.h;

		x = x1;
		y = y1;
		w = x2 - x;
		h = y2 - y;

		if (w <= 0 || h <= 0)
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

	Rect Rect::getUnion(const Rect& r1, const Rect& r2)
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

	void Rect::shrink(const Border &border)
	{
		x += border.left;
		y += border.top;
		w -= border.width();
		h -= border.height();

		if (w < 0)
			w = 0;

		if (h < 0)
			h = 0;
	}

	//____ grow() _____________________________________________________________
	/**
	 * @brief Grow the rectangle by the specified border.
	 *
	 * Grow the rectangle by the specified border.
	 *
	 * @param border	BorderI by which to grow the rectangle.
	 *
	 * Width and height of rectangle is increased by the thickness of the borders.
	 * Top and left borders also affects the position of the rectangle.
	 **/
	void Rect::grow(const Border &border)
	{
		x -= border.left;
		y -= border.top;
		w += border.width();
		h += border.height();
	}


	//____ growToContain() ____________________________________________________
	/**
	 * @brief	Grow the reactangle to contain the specified coordinate.
	 *
	 * Grow the reactangle to contain the specified coordinate.
	 *
	 * @param	coord	Coordinate the rectangle is grown to contain.
	 *
	 * Position of rectangle is affected if coordinate is above or left of coordinate.
	 **/

	void Rect::growToContain(const Coord& coord)
	{
		if (coord.x < x)
		{
			w += x - coord.x;
			x = coord.x;
		}
		else if (coord.x > x + w)
		{
			w = coord.x - x;
		}

		if (coord.y < y)
		{
			h += y - coord.y;
			y = coord.y;
		}
		else if (coord.y > y + h)
		{
			h = coord.y - y;
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

	void Rect::growToContain(const Rect& r)
	{
		if (r.x < x)
		{
			w += x - r.x;
			x = r.x;
		}

		if (r.y < y)
		{
			h += y - r.y;
			y = r.y;
		}

		if (r.x + r.w > x + w)
		{
			w = r.x + r.w - x;
		}

		if (r.y + r.h > y + h)
		{
			h = r.y + r.h - y;
		}
	}



	//____ intersectsWithOrContains() _______________________________________________________

	bool Rect::intersectsWithOrContains(Coord p1, Coord p2, int precision) const
	{
		// NOTE: This needs to be kept in sync with ClipLine() so they always return same result for same input.

		MU x1 = x;
		MU x2 = x + w;

		// We go from left to right when working with the X coordinates

		if (p2.x < p1.x)
			std::swap(p1, p2);

		// Verify that x-coordinates are within range

		if (p1.x >= x2 || p2.x <= x1)
			return false;

		// Calculate angles now before we destroy any values

		int angleX = ((p2.y.qpix - p1.y.qpix) << precision) / (1 + p2.x.qpix - p1.x.qpix);		// Change in Y for each increase of X.


		 // Clip line so no coord is outside rectangles x-dimensions

		if (p1.x < x1)
			p1.y.qpix = p1.y.qpix + ((x1.qpix - p1.x.qpix)*angleX >> precision);

		if (p2.x > x2)
			p2.y.qpix = p2.y.qpix - ((p2.x.qpix - x2.qpix)*angleX >> precision);

		// We go from top to bottom when working with the Y coordinates

		if (p2.y < p1.y)
			std::swap(p1.y, p2.y);

		// Verify that clipped line intersects in y-dimension.

		if (p1.y >= y + h || p2.y <= y)
			return false;

		return true;
	}

	//____ clipLine() _____________________________________________________________

	bool Rect::clipLine(Coord* p1, Coord* p2, int precision) const
	{
		// NOTE: This needs to be kept in sync with IntersectsOrContains() so they always return same result for same input.

		MU x1 = x;
		MU x2 = x + w;
		MU y1 = y;
		MU y2 = y + h;

		// We go from left to right when working with the X coordinates

		if (p2->x < p1->x)
			std::swap(p1, p2);

		// Verify that x-coordinates are within range

		if (p1->x >= x2 || p2->x <= x1)
			return false;

		// Calculate both angles now before we destroy precision

		int angleX = ((p2->y.qpix - p1->y.qpix) << precision) / (1 + p2->x.qpix - p1->x.qpix);		// Change in Y for each increase of X.
		int angleY = ((p2->x.qpix - p1->x.qpix) << precision) / (1 + abs(int(p2->y.qpix) - int(p1->y.qpix)));		// Change in X for each increase of Y.

		// Clip line so no coord is outside rectangles x-dimensions

		if (p1->x < x1)
		{
			p1->y.qpix = p1->y.qpix + ((x1.qpix - p1->x.qpix)*angleX >> precision);
			p1->x = x2;
		}

		if (p2->x > x2)
		{
			p2->y.qpix = p2->y.qpix - ((p2->x.qpix - x2.qpix)*angleX >> precision);
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
			p1->x.qpix = p1->x.qpix + ((y1.qpix - p1->y.qpix)*angleY >> precision);
			p1->y = y1;
		}

		if (p2->y > y2)
		{
			p2->x.qpix = p2->x.qpix - ((p2->y.qpix - y2.qpix)*angleY >> precision);
			p2->y = y2;
		}

		return true;
	}

	//_____________________________________________________________________________

	Coord Rect::distance(Coord coord) const
	{
		coord.x -= x;
		if (coord.x >= 0)
		{
			coord.x -= w;
			if (coord.x < 0)
				coord.x = 0;
		}

		coord.y -= y;
		if (coord.y >= 0)
		{
			coord.y -= h;
			if (coord.y < 0)
				coord.y = 0;
		}
		return coord;
	}





} // namespace wg
