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

#ifndef	WG_GEO_DOT_H
#define	WG_GEO_DOT_H
#pragma once
//=============================================================================

#include <wg_types.h>

#include <utility>
#include <cstdlib>

namespace wg
{

	template<class T> class CoordT;
	template<class T> class SizeT;
	template<class T> class RectT;

	typedef	CoordT<int>		Coord;
	typedef CoordT<float>	CoordF;

	typedef	SizeT<int>		Size;
	typedef SizeT<float>	SizeF;

	typedef	RectT<int>		Rect;
	typedef RectT<float>	RectF;


	/**
		@brief Simple class holding the X and Y values of a 2D coordinate.

		Specifies a 2D coordinate through class members x and y, where x is the
		horizontal distance from origo moving from left to right and y is
		the vertical distance moving downwards.

		Coord specifies the position in discrete pixels (integer precision).
	**/
	template<class Type> class CoordT
	{
	public:
		//.____ Creation __________________________________________

		CoordT() : x(0), y(0) {}
		CoordT(Type x, Type y) : x(x), y(y) {}
		CoordT(const RectT<Type>& rect);

		//.____ Misc ______________________________________________

		inline SizeT<Type> toSize();
		inline void clear() { x = 0; y = 0; }			///< Sets X and Y to 0.

		//.____ Operators ___________________________________________

		inline CoordT<Type> operator=(const CoordT<Type>& k) { x = k.x; y = k.y; return *this; }
		inline CoordT<Type> operator=(const RectT<Type>& r);

		inline bool operator==(const CoordT<Type>& k) const { if (x == k.x && y == k.y) return true; return false; }
		inline bool operator!=(const CoordT<Type>& k) const { if (x != k.x || y != k.y) return true; return false; }

		inline void operator+=(const CoordT<Type>& k) { x += k.x; y += k.y; }
		inline void operator-=(const CoordT<Type>& k) { x -= k.x; y -= k.y; }
		inline CoordT<Type> operator+(const CoordT<Type>& k) const { CoordT<Type> res; res.x = x + k.x; res.y = y + k.y; return res; }
		inline CoordT<Type> operator-(const CoordT<Type>& k) const { CoordT<Type> res; res.x = x - k.x; res.y = y - k.y; return res; }

		inline void operator*=(double v) { x = (Type)(x*v); y = (Type)(y*v); }
		inline void operator/=(double v) { x = (Type)(x / v); y = (Type)(y / v); }
		inline CoordT<Type> operator*(double v) const { CoordT<Type> res; res.x = (Type)(x*v); res.y = (Type)(y*v); return res; }
		inline CoordT<Type> operator/(double v) const { CoordT<Type> res; res.x = (Type)(x / v); res.y = (Type)(y / v); return res; }

		inline void operator*=(float v) { x = (Type)(x*v); y = (Type)(y*v); }
		inline void operator/=(float v) { x = (Type)(x / v); y = (Type)(y / v); }
		inline CoordT<Type> operator*(float v) const { CoordT<Type> res; res.x = (Type)(x*v); res.y = (Type)(y*v); return res; }
		inline CoordT<Type> operator/(float v) const { CoordT<Type> res; res.x = (Type)(x / v); res.y = (Type)(y / v); return res; }

		inline void operator*=(int v) { x = (Type)(x*v); y = (Type)(x*v); }
		inline void operator/=(int v) { x = (Type)(x / v); y = (Type)(y / v); }
		inline CoordT<Type> operator*(int v) const { CoordT<Type> res; res.x = (Type)(x*v); res.y = (Type)(y*v); return res; }
		inline CoordT<Type> operator/(int v) const { CoordT<Type> res; res.x = (Type)(x / v); res.y = (Type)(y / v); return res; }

		//.____ Properties __________________________________________

		Type	x, y;
	};

	//_____________________________________________________________________________
	template<typename Type>
	inline CoordT<Type>::CoordT(const RectT<Type>& rect)
	{
		x = rect.x;
		y = rect.y;
	}

	//_____________________________________________________________________________
	template<typename Type>
	inline CoordT<Type> CoordT<Type>::operator=(const RectT<Type>& r)
	{
		x = r.x;
		y = r.y;
		return *this;
	}

	//_____________________________________________________________________________
	template<typename Type>
	inline SizeT<Type> CoordT<Type>::toSize()
	{
		return SizeT<Type>(x, y);
	}



	//____ Border ______________________________________________________________
	/**
		@brief Specifies the thickness of the border of a rectangular area, measured in pixels.

		Specifies the thickness of the border of a rectangular area, measured in pixels.

		The thickness of the four sides of the border can be set individually.
		The sides of the border always comes in the order: top, right, bottom, left. I.e.
		you start with the top border and go clockwise.

		Borders are in most cases internal to the rectangular areas geometry, so the combined thickness of left and right
		sides should not surpass the rectangles width and the combined thickness of top and bottom sides should not surpass
		the rectangles height.

		Negative values are in most cases not accepted and result in unspecified behavior.
	**/

	class Border
	{
	public:
		//.____ Creation __________________________________________

		Border() : top(0), right(0), bottom(0), left(0) {}
		Border( short topBottom, short leftRight ) : top(topBottom), right(leftRight), bottom(topBottom), left(leftRight) {}
		Border( short top, short leftRight, short bottom ) : top(top), right(leftRight), bottom(bottom), left(leftRight) {}
		Border( short top, short right, short bottom, short left ) : top(top), right(right), bottom(bottom), left(left) {}
		Border( short all ) : top(all), right(all), bottom(all), left(all) {}

		//.____ Misc ______________________________________________

		inline void		set( short all ) { left = right = top = bottom = all; }
		inline void		set( short topBottom, short leftRight ) { top = bottom = topBottom; left = right = leftRight; }
		inline void		set( short top, short leftRight, short bottom ) { this->top = top; this->bottom = bottom; this->left = this->right = leftRight; }
		inline void		set( short top, short right, short bottom, short left ) { this->top = top; this->bottom = bottom; this->left = left; this->right = right; }

		inline Size		size() const;
		inline int		width() const { return ((int)left)+right; }
		inline int		height() const { return ((int)top)+bottom; }
		inline void		clear()			{ left = 0; right = 0; top = 0; bottom = 0; }		///< @brief Sets the thickness of all sides to 0.
		inline bool		isEmpty() const { return (left | top | right | bottom) == 0; }

		inline Border	scale(int scale) const { return Border( top*scale / 4096,right*scale / 4096,bottom*scale / 4096,left*scale / 4096); } // Only for WG2 compatibility!

		//.____ Operators ___________________________________________

		bool			operator==(const Border& borders) const { return left == borders.left &&
																			right == borders.right &&
																			top == borders.top &&
																			bottom == borders.bottom; }
		bool			operator!=(const Border& borders) const { return !(*this == borders); }

		//.____ Properties __________________________________________

		short		top, right, bottom, left;
	};


	//____ SizeT<T> ________________________________________________________

	/**
		@brief Specifies the size of a rectangular area, measured in pixels.

		Specifies the size of a rectangular area, measured in pixels.
		Members are integer precision and can be negative.
	**/

	template<class Type> class SizeT
	{
	public:
		//.____ Creation __________________________________________

		SizeT() : w(0), h(0) {}
		SizeT( Type width, Type height ) : w(width), h(height) {}
		SizeT( const SizeT<Type>& size ) : w(size.w), h(size.h) {}
		SizeT( const RectT<Type>& rect );
		SizeT( const CoordT<Type>& c1, const CoordT<Type>& c2 ) { w = c2.x - c1.x; h = c2.y - c1.y; }

		//.____ Misc ______________________________________________

		inline void limit( const SizeT<Type>& min, const SizeT<Type>& max );
		inline void clear()		{ w = 0; h = 0; }

		inline CoordT<Type> toCoord() { return CoordT<Type>(w,h); }

		//.____ Operators ___________________________________________

		inline SizeT<Type> operator=(const SizeT<Type>& k)	{ w = k.w; h = k.h; return *this; }
		inline SizeT<Type> operator=(const RectT<Type>& k);

		inline bool operator==(const SizeT<Type>& k) const	{ if( w == k.w && h == k.h ) return true; return false; }
		inline bool operator!=(const SizeT<Type>& k) const	{ if( w != k.w || h != k.h ) return true; return false; }

		inline void operator+=(const SizeT<Type>& k)				{ w += k.w; h += k.h; }
		inline void operator-=(const SizeT<Type>& k)				{ w -= k.w; h -= k.h; }
		inline SizeT<Type> operator+(const SizeT<Type>& k) const	{ SizeT<Type> res; res.w = w + k.w; res.h = h + k.h; return res; }
		inline SizeT<Type> operator-(const SizeT<Type>& k) const	{ SizeT<Type> res; res.w = w - k.w; res.h = h - k.h; return res; }

		inline void operator+=(const Border& k)			{ w += k.left + k.right; h += k.top + k.bottom; }
		inline void operator-=(const Border& k)			{ w -= k.left + k.right; h -= k.top + k.bottom; }
		inline SizeT<Type> operator+(const Border& k) const	{ SizeT<Type> res; res.w = w + k.left + k.right; res.h = h + k.top + k.bottom; return res; }
		inline SizeT<Type> operator-(const Border& k) const	{ SizeT<Type> res; res.w = w - k.left - k.right; res.h = h - k.top - k.bottom; return res; return res; }

		inline void operator*=(double x)				{ w = (Type) (w*x); h = (Type) (h*x); }
		inline void operator/=(double x)				{ w = (Type) (w/x); h = (Type) (h/x); }
		inline SizeT<Type> operator*(double x) const	{ SizeT<Type> res; res.w = (Type) (w*x); res.h = (Type) (h*x); return res; }
		inline SizeT<Type> operator/(double x) const	{ SizeT<Type> res; res.w = (Type) (w/x); res.h = (Type) (h/x); return res; }

		inline void operator*=(float x)				{ w = (Type) (w*x); h = (Type) (h*x); }
		inline void operator/=(float x)				{ w = (Type) (w/x); h = (Type) (h/x); }
		inline SizeT<Type> operator*(float x) const	{ SizeT<Type> res; res.w = (Type) (w*x); res.h = (Type) (h*x); return res; }
		inline SizeT<Type> operator/(float x) const	{ SizeT<Type> res; res.w = (Type) (w/x); res.h = (Type) (h/x); return res; }

		inline void operator*=(int x)				{ w *= x; h *= x; }
		inline void operator/=(int x)				{ w /= x; h /= x; }
		inline SizeT<Type> operator*(int x) const	{ SizeT<Type> res; res.w = w * x; res.h = h * x; return res; }
		inline SizeT<Type> operator/(int x) const	{ SizeT<Type> res; res.w = w / x; res.h = h / x; return res; }

		static inline SizeT<Type> min( SizeT<Type> sz1, SizeT<Type> sz2 ) { return SizeT<Type>( sz1.w<sz2.w?sz1.w:sz2.w, sz1.h<sz2.h?sz1.h:sz2.h ); }
		static inline SizeT<Type> max( SizeT<Type> sz1, SizeT<Type> sz2 ) { return SizeT<Type>( sz1.w>sz2.w?sz1.w:sz2.w, sz1.h>sz2.h?sz1.h:sz2.h ); }

		//.____ Properties __________________________________________

		Type	w;		///< Width of the rectangular area in pixels.
		Type	h;		///< Height of the rectangular area in pixels.

	};

	//_____________________________________________________________________________
	template<typename Type>
	inline SizeT<Type>::SizeT(const RectT<Type>& rect)
	{
		w = rect.w;
		h = rect.h;
	}

	//_____________________________________________________________________________
	template<typename Type>
	inline SizeT<Type> SizeT<Type>::operator=(const RectT<Type>& k)
	{
		w = k.w;
		h = k.h;
		return *this;
	}

	//_____________________________________________________________________________
	template<typename Type>
	inline void SizeT<Type>::limit(const SizeT<Type>& min, const SizeT<Type>& max)
	{
		if (w < min.w)
			w = min.w;
		if (h < min.h)
			h = min.h;

		if (w > max.w)
			w = max.w;
		if (h > max.h)
			h = max.h;
	}


	//____ Rect<T> ___________________________________________________________

	/**
		@brief Specifies a rectangular area of a 2D surface through position and size.

		Specifies a rectangular area of a 2D surface through position and size.

		Position is specified through the members x and y, containing the horizontal
		and vertical offset from an origo position while members w and h specifies
		the width and height.

		Origo is normally the top-left corner of a parent rectangle, screen surface or
		bitmap and position normally specifies the position of the top-left corner of the rectangle, although
		this might differ depending on context.

		Increasing x and y will always move the rectangle to the right and down respectively.

		Members are not checked or truncated so width and height are allowed to contain negative values
		but various WonderGUI methods accepting a Rect as parameter might not be able to handle it.

	**/

	template<class Type> class RectT
	{
	public:
		//.____ Creation __________________________________________

		RectT() : x(0), y(0), w(0), h(0) {}								///< @brief Create rectangle with x, y, w and h set to 0.
																			///<
																			///< Create rectangle with x, y, w and h set to 0.
		RectT( Type x, Type y, Type w, Type h ) : x(x), y(y), w(w), h(h) {}	///< @brief Create rectangle with the given values.
																			///<
																			///< Create rectangle with the given values.
		RectT( const RectT<Type>& r ) : x(r.x), y(r.y), w(r.w), h(r.h) {}		///< @brief Create a copy of specified rectangle.
																			///<
																			///< Create a copy of specified rectangle.

		template<typename T>
		RectT(const RectT<T>& r) : x((Type)r.x), y((Type)r.y), w((Type)r.w), h((Type)r.h) {}		///< @brief Create a copy of specified rectangle.


		RectT( const RectT<Type>& r1, const RectT<Type>& r2 );						///< @brief Create rectangle from intersection of specified rectangles.
		RectT( const CoordT<Type>& p1, const CoordT<Type>& p2 );						///< @brief	Create rectangle to cover the area between the specified coordinates.
		RectT( const CoordT<Type>& p, const SizeT<Type>& sz ) : x(p.x), y(p.y), w(sz.w), h(sz.h) {} 	///< @brief Create rectangle of specified position and size.
																							///<
																							///< Create rectangle of specified position and size.
																							///< @param p 	Coordinate containing position for rectangle.
																							///< @param sz	Coordinate containing size for rectangle.
		RectT( const CoordT<Type>& p, int w, int h ) : x(p.x), y(p.y), w(w), h(h) {}			///< @brief Create rectangle of specified position and size.
																							///<
																							///< Create rectangle of specified position and size.
																							///< @param p		Coordinate containing position for rectangle.
																							///< @param w		Width for rectangle.
																							///< @param h		Height for rectangle.
		RectT( Type x, Type y, const SizeT<Type>& sz ) : x(x), y(y), w(sz.w), h(sz.h) {}			///< @brief Create rectangle of specified position and size.
																							///<
																							///< Create rectangle of specified position and size.
																							///< @param x		Horizontal position for rectangle.
																							///< @param y		Vertical position for rectangle.
																							///< @param sz	Width and height for rectangle.
		RectT( const CoordT<Type>& p ) : x(p.x), y(p.y), w(0), h(0) {}			///< @brief Create rectangle of specified position and size of (0,0).
																			///<
																			///< Create rectangle of specified position and size of (0,0).
																			///< @param p		Coordinate containing position for rectangle.
		RectT( const SizeT<Type>& sz ) : x(0), y(0), w(sz.w), h(sz.h) {}		///< @brief Create rectangle of specified size and position (0,0).
																			///<
																			///< Create rectangle of specified size and position (0,0).
																			///< @param sz	Width and height for rectangle.

		//.____ Misc ______________________________________________

		inline void setPos( const CoordT<Type>& p );					///< @brief Set position of rectangle.
		inline void setSize( const SizeT<Type>& sz );					///< @brief Set size of rectangle.

		void shrink( const Border &borders );
		void shrink( Type top, Type right, Type bottom, Type left );

		void grow( const Border &borders );
		void grow( Type top, Type right, Type bottom, Type left );

		bool intersection( const RectT<Type>& r1, const RectT<Type>& r2 );
		static RectT<Type> getUnion( const RectT<Type>& r1, const RectT<Type>& r2 );

		void growToContain( Type x, Type y );
		void growToContain( const RectT<Type>& rect );
		void growToContain( const CoordT<Type>& coord );

		inline bool contains( Type x, Type y ) const;				///< @brief Check if coordinate is within rectangle.
		inline bool	contains( const CoordT<Type>& coord ) const;		///< @brief Check if coordinate is within rectangle.
		inline bool contains( const RectT<Type>& rect ) const;			///< @brief Check if rectangle is fully within our rectangle.

		inline CoordT<Type> limit( const CoordT<Type>& coord ) const;			///< @brief Limit given coordinate to stay within rectangle.
		inline CoordT<Type> center() const { return {x+w/2,y+h/2}; }
		inline RectT<Type> center(SizeT<Type> sz) const { return { x + (w-sz.w) / 2,y + (h-sz.h) / 2, sz }; }

		inline bool intersectsWith( const RectT<Type>& rect ) const;	///< @brief Check for intersection (partial or full overlap) with specified rectangle.

		bool		intersectsWithOrContains(CoordT<Type> p1, CoordT<Type> p2, int precision = 14) const;
		bool		clipLine(CoordT<Type> * p1, CoordT<Type> * p2, int precision = 14) const;


		inline Type width() const;								///< @brief Get the width of the rectangle.
		inline Type height() const;								///< @brief Get the height of the rectangle.
		inline SizeT<Type> size() const;								///< @brief Get the size of the rectangle.
		inline CoordT<Type> pos() const;								///< @brief Get the position of the rectangle.

		inline Type left() const;								///< @brief Get X coordinate of left border.
		inline Type top() const;									///< @brief Get Y cordinate of top border.
		inline Type right() const;								///< @brief Get X coordinate of right border.
		inline Type bottom() const;								///< @brief Get Y coordinate of bottom border.

		inline bool	isEmpty() const;							///< @brief Check if rectangle has no area.
		inline void clear();									///< @brief Sets all values to zero.

		//.____ Operators ___________________________________________

		inline void operator=(const RectT<Type>&);					///< @brief Normal assignment operator.
		inline void operator=(const SizeT<Type>& sz);				///< @brief Set rectangle to specified size. Position is cleared.
		inline void operator=(const CoordT<Type>& c);				///< @brief Set rectangle to specified position. Size is cleared.
		inline bool operator==(const RectT<Type>& rect) const;
		inline bool operator!=(const RectT<Type>& rect) const;

		inline void operator+=(const SizeT<Type>& k);				///< @brief Increase size of rectangle.
		inline void operator-=(const SizeT<Type>& k);				///< @brief Decrease size of rectangle.
		inline RectT<Type> operator+(const SizeT<Type>& k) const; 		///< @brief Returns rectangle with size increased by Size.
		inline RectT<Type> operator-(const SizeT<Type>& k) const; 		///< @brief Returns rectangle with size decreased by Size.



		inline void operator+=(const CoordT<Type>& k); 				///< @brief Increase position of rectangle.
		inline void operator-=(const CoordT<Type>& k);				///< @brief Decrease position of rectangle.
		inline RectT<Type> operator+(const CoordT<Type>& k) const; 		///< @brief Returns rectangle with position increased by Coord.
		inline RectT<Type> operator-(const CoordT<Type>& k) const; 		///< @brief Returns rectangle with position decreased by Coord.

		inline void operator+=(const Border& k);				///< @brief Grow rectangle by the specified border.
		inline void operator-=(const Border& k);
		inline RectT<Type> operator+(const Border& k) const;
		inline RectT<Type> operator-(const Border& k) const;

		//.____ Properties __________________________________________

		Type	x;		///< Start position in pixels along the X axis.
		Type	y;		///< Start position in pixels along the Y axis.
		Type	w;		///< Width, i.e. length of rectangle along the X axis.
		Type	h;		///< Height, i.e. length of rectangle along the Y axis.
	};


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
	template<typename Type>
	RectT<Type>::RectT(const RectT<Type>& r1, const RectT<Type>& r2)
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

	template<typename Type>
	RectT<Type>::RectT(const CoordT<Type>& p1, const CoordT<Type>& p2)
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



	//___________________________________________________________________________
	/**
	 * Check if given coordinate is within rectangle.
	 *
	 * @param x Horizontal position of coordinate to check.
	 * @param y Vertical position of coordinate to check.
	 *
	 * @return True if coordinate is within the bounds of the rectangle.
	 **/
	template<typename Type>
	inline bool RectT<Type>::contains( Type x, Type y ) const
	{
		if( x >= this->x && x < this->x + w && y >= this->y && y < this->y + h )
			return true;
		return false;
	}


	//_____________________________________________________________________________
	/**
	 * Check if given coordinate is within rectangle.
	 *
	 * @param coord Coordinate to check.
	 *
	 * @return True if coordinate is within the bounds of the rectangle.
	 **/
	template<typename Type>
	inline bool RectT<Type>::contains( const CoordT<Type>& coord ) const
	{
//		int a = coord.x - x;
//		int b = coord.y - y;
//		int c = (x + w) - (coord.x);
//		int d = (y + h) - (coord.y);

//		return (a | b | c | d) >= 0;

		if( coord.x >= x && coord.x < x + w && coord.y >= y && coord.y < y + h )
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
	template<typename Type>
	inline bool RectT<Type>::contains( const RectT<Type>& rect ) const
	{
//		int a = rect.x - x;
//		int b = rect.y - y;
//		int c = (x + w) - (rect.x + rect.w);
//		int d = (y + h) - (rect.y + rect.h);

//		return (a | b | c | d) >= 0;

		if( rect.x >= x && rect.y >= y && rect.x + rect.w <= x + w &&  rect.y + rect.h <= y + h )
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
	template<typename Type>
	 inline CoordT<Type> RectT<Type>::limit( const CoordT<Type>& coord ) const
	{
		CoordT<Type> out = coord;
		if( out.x < x )
			out.x = x;
		if( out.y < y )
			out.y = y;
		if( out.x > x + w )
			out.x = x + w;
		if( out.y > y + h )
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
	 template<typename Type>
	 inline bool RectT<Type>::intersectsWith( const RectT<Type>& rect ) const
	{
		if(left() >= rect.right()) return false;
		if(right() <= rect.left()) return false;
		if(top() >= rect.bottom()) return false;
		if(bottom() <= rect.top()) return false;

		return true;
	}

	//_____________________________________________________________________________
	/**
	 * Normal assignment operator.
	 **/
	 template<typename Type>
	 inline void RectT<Type>::operator=( const RectT<Type>& r2 )
	{
		x = r2.x;
		y = r2.y;
		w = r2.w;
		h = r2.h;
	}

	//_____________________________________________________________________________
	/**
	 * Set rectangle to specified size. Position is cleared.
	 **/
	 template<typename Type>
	 inline void RectT<Type>::operator=( const SizeT<Type>& sz)
	{
		x=0;
		y=0;
		w=sz.w;
		h=sz.h;
	}

	//_____________________________________________________________________________
	/**
	 * Set rectangle to specified position. Size is cleared.
	 **/
	 template<typename Type>
	 inline void RectT<Type>::operator=( const CoordT<Type>& c)
	{
		x=c.x;
		y=c.y;
		w=0;
		h=0;
	}

	//_____________________________________________________________________________
	/**
	 * Standard comparison operator
	 **/
	 template<typename Type>
	 inline bool RectT<Type>::operator==( const RectT<Type>& rect) const
	{
		return x == rect.x && y == rect.y && w == rect.w && h == rect.h;
	}

	//_____________________________________________________________________________
	/**
	 * Standard comparison operator
	 **/
	 template<typename Type>
	 inline bool RectT<Type>::operator!=( const RectT<Type>& rect) const
	{
		return !(*this == rect);
	}

	//_____________________________________________________________________________
	/**
	 * Increase size of rectangle.
	 **/
	 template<typename Type>
	 inline void RectT<Type>::operator+=(const SizeT<Type>& k)
	{
		w += k.w;
		h += k.h;
	}

	//_____________________________________________________________________________
	/**
	 * Decrease size of rectangle.
	 **/
	 template<typename Type>
	 inline void RectT<Type>::operator-=(const SizeT<Type>& k)
	{
		w -= k.w;
		h -= k.h;
	}
	//_____________________________________________________________________________
	/**
	 * Get rectangle with size increased by Size.
	 **/

	 template<typename Type>
	 inline RectT<Type> RectT<Type>::operator+(const SizeT<Type>& k) const
	{
		RectT<Type> res;
		res.x = x;
		res.y = y;
		res.w = w + k.w;
		res.h = h + k.h;
		return res;
	}

	//_____________________________________________________________________________
	/**
	 * Get rectangle with size decreased with Size.
	 **/

	 template<typename Type>
	 inline RectT<Type> RectT<Type>::operator-(const SizeT<Type>& k) const
	{
		RectT<Type> res;
		res.x = x;
		res.y = y;
		res.w = w - k.w;
		res.h = h - k.h;
		return res;
	}


	//_____________________________________________________________________________
	/**
	 * Increase position of rectangle.
	 **/

	 template<typename Type>
	 inline void RectT<Type>::operator+=(const CoordT<Type>& k)
	{
		x += k.x;
		y += k.y;
	}

	//_____________________________________________________________________________
	/**
	 * Decrease position of rectangle.
	 **/

	 template<typename Type>
	 inline void RectT<Type>::operator-=(const CoordT<Type>& k)
	{
		x -= k.x;
		y -= k.y;
	}

	//_____________________________________________________________________________
	/**
	 * Get rectangle with position increased with Coord.
	 **/

	 template<typename Type>
	 inline RectT<Type> RectT<Type>::operator+(const CoordT<Type>& k) const
	{
		RectT<Type> res;
		res.x = x + k.x;
		res.y = y + k.y;
		res.w = w;
		res.h = h;
		return res;
	}

	//_____________________________________________________________________________
	/**
	 * Get rectangle with position decreased with Coord.
	 **/
	 template<typename Type>
	 inline RectT<Type> RectT<Type>::operator-(const CoordT<Type>& k) const
	{
		RectT<Type> res;
		res.x = x - k.x;
		res.y = y - k.y;
		res.w = w;
		res.h = h;
		return res;
	}


	//_____________________________________________________________________________
	/**
	 * Grow rectangle by the specified border.
	 *
	 * The position of the rectangle is affected by the top and left borders.
	 **/
	 template<typename Type>
	 inline void RectT<Type>::operator+=(const Border& k)
	{
		x -= k.left;
		y -= k.top;
		w += k.left + k.right;
		h += k.top + k.bottom;
	}

	//_____________________________________________________________________________
	/**
	 * @brief Shrink rectangle by the specified border.
	 *
	 * Shrink rectangle by the specified border.
	 * The position of the rectangle is affected by the top and left borders.
	 **/
	 template<typename Type>
	 inline void RectT<Type>::operator-=(const Border& k)
	{
		x += k.left;
		y += k.top;
		w -= k.left + k.right;
		h -= k.top + k.bottom;
	}


	//_____________________________________________________________________________
	/**
	 * @brief Returns a rectangle grown by the border.
	 *
	 * Returns a rectangle grown by the border.
	 * The position of the rectangle is affected by the top and left borders.
	 **/
	 template<typename Type>
	 inline RectT<Type> RectT<Type>::operator+(const Border& k) const
	{
		RectT<Type> res;
		res.x = x - k.left;
		res.y = y- k.top;
		res.w = w + k.left + k.right;
		res.h = h + k.top + k.bottom;
		return res;
	}

	//_____________________________________________________________________________
	/**
	 * @brief Returns a rectangle shrunk by the border.
	 *
	 * Returns a rectangle shrunk by the border.
	 * The position of the rectangle is affected by the top and left borders.
	 **/
	 template<typename Type>
	 inline RectT<Type> RectT<Type>::operator-(const Border& k) const
	{
		RectT<Type> res;
		res.x = x + k.left;
		res.y = y + k.top;
		res.w = w - (k.left + k.right);
		res.h = h - (k.top + k.bottom);
		return res;
	}


	//_____________________________________________________________________________
	/**
	 * Set position (members x and y) of the rectangle.
	 **/
	 template<typename Type>
	 inline void RectT<Type>::setPos( const CoordT<Type>& p )
	{
		x = p.x;
		y = p.y;
	}

	//_____________________________________________________________________________
	/**
	 * Set the size (members w and h) of the rectangle.
	 **/
	 template<typename Type>
	 inline void RectT<Type>::setSize( const SizeT<Type>& sz )
	{
		w = sz.w;
		h = sz.h;
	}

	//_____________________________________________________________________________
	/**
	 * Get width of the rectangle (member variable w).
	 **/
	 template<typename Type>
	 inline Type RectT<Type>::width() const
	{
		return w;
	}

	//_____________________________________________________________________________
	/**
	 * Get height of the rectangle (member variable h).
	 **/
	 template<typename Type>
	 inline Type RectT<Type>::height() const
	{
		return h;
	}

	//_____________________________________________________________________________
	/**
	 * Get size (width and height) of the rectangle.
	 **/
	 template<typename Type>
	 inline SizeT<Type> RectT<Type>::size() const
	{
		return SizeT<Type>(w,h);
	}

	//_____________________________________________________________________________
	/**
	 * Get position (members x and y) of the rectangle.
	 **/
	 template<typename Type>
	 inline CoordT<Type> RectT<Type>::pos() const
	{
		return CoordT<Type>(x,y);
	}

	//_____________________________________________________________________________
	/**
	 * Get X coordinate of left edge. This is equal to member variable x.
	 **/
	 template<typename Type>
	 inline Type RectT<Type>::left() const
	{
		return x;
	}

	//_____________________________________________________________________________
	/**
	 * Get Y coordinate of top edge. This is equal to member variable y.
	 **/
	 template<typename Type>
	 inline Type RectT<Type>::top() const
	{
		return y;
	}

	//_____________________________________________________________________________
	/**
	 * Get X coordinate of right edge. This is equal to member variables x + w.
	 **/
	 template<typename Type>
	 inline Type RectT<Type>::right() const
	{
		return x + w;
	}

	//_____________________________________________________________________________
	/**
	 * Get Y coordinate of bottom edge. This is equal to member variables y + h.
	 **/
	 template<typename Type>
	 inline Type RectT<Type>::bottom() const
	{
		return y + h;
	}
	//_____________________________________________________________________________
	/**
	 * 	Check if rectangle has no area (width or height is zero).
	 **/
	 template<typename Type>
	 inline bool	RectT<Type>::isEmpty() const
	{
		return (w==0||h==0)?true:false;
	}

	//_____________________________________________________________________________
	/**
	 * Sets all values to zero.
	 **/
	 template<typename Type>
	 inline void RectT<Type>::clear()
	{
		x = 0;
		y = 0;
		w = 0;
		h = 0;
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
	 template<typename Type>
	 bool RectT<Type>::intersection(const RectT<Type>& _r1, const RectT<Type>& _r2)
	 {
		 Type		x1, y1;						// So we can use ourself as inparameter.
		 Type		x2, y2;

		 if (_r1.x > _r2.x)
			 x1 = _r1.x;
		 else
			 x1 = _r2.x;

		 if (_r1.y > _r2.y)
			 y1 = _r1.y;
		 else
			 y1 = _r2.y;

		 if (_r1.x + _r1.w < _r2.x + _r2.w)
			 x2 = _r1.x + _r1.w;
		 else
			 x2 = _r2.x + _r2.w;

		 if (_r1.y + _r1.h < _r2.y + _r2.h)
			 y2 = _r1.y + _r1.h;
		 else
			 y2 = _r2.y + _r2.h;

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

	 template<typename Type>
	 RectT<Type> RectT<Type>::getUnion(const RectT<Type>& r1, const RectT<Type>& r2)
	 {
		 RectT<Type> out;
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

	 template<typename Type>
	 void RectT<Type>::shrink(Type top, Type right, Type bottom, Type left)
	 {
		 x += left;
		 y += top;
		 w -= right + left;
		 h -= bottom + top;

		 if (w < 0)
			 w = 0;

		 if (h < 0)
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

	 template<typename Type>
	 void RectT<Type>::shrink(const Border &_borders)
	 {
		 x += _borders.left;
		 y += _borders.top;
		 w -= _borders.width();
		 h -= _borders.height();

		 if (w < 0)
			 w = 0;

		 if (h < 0)
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

	 template<typename Type>
	 void RectT<Type>::grow(Type top, Type right, Type bottom, Type left)
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
	 template<typename Type>
	 void RectT<Type>::grow(const Border &_borders)
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
	 template<typename Type>
	 void RectT<Type>::growToContain(Type _x, Type _y)
	 {
		 if (_x < x)
		 {
			 w += x - _x;
			 x = _x;
		 }
		 else if (_x > x + w)
		 {
			 w = _x - x;
		 }

		 if (_y < y)
		 {
			 h += y - _y;
			 y = _y;
		 }
		 else if (_y > y + h)
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
	 template<typename Type>
	 void RectT<Type>::growToContain(const CoordT<Type>& _coord)
	 {
		 if (_coord.x < x)
		 {
			 w += x - _coord.x;
			 x = _coord.x;
		 }
		 else if (_coord.x > x + w)
		 {
			 w = _coord.x - x;
		 }

		 if (_coord.y < y)
		 {
			 h += y - _coord.y;
			 y = _coord.y;
		 }
		 else if (_coord.y > y + h)
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

	 template<typename Type>
	 void RectT<Type>::growToContain(const RectT<Type>& r)
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

	 template<typename Type>
	 bool RectT<Type>::intersectsWithOrContains(CoordT<Type> p1, CoordT<Type> p2, int precision) const
	 {
		 // NOTE: This needs to be kept in sync with ClipLine() so they always return same result for same input.

		 Type x1 = x;
		 Type x2 = x + w;

		 // We go from left to right when working with the X coordinates

		 if (p2.x < p1.x)
			 std::swap(p1, p2);

		 // Verify that x-coordinates are within range

		 if (p1.x >= x2 || p2.x <= x1)
			 return false;

		 // Calculate angles now before we destroy any values
		 
		 Type angleX;
		 if( std::is_integral<Type>::value )
			angleX = ((p2.y - p1.y) << precision) / (1 + p2.x - p1.x);		// Change in Y for each increase of X.
		 else
			angleX = (p2.y - p1.y) / (1 + p2.x - p1.x);					// Change in Y for each increase of X.


		 // Clip line so no coord is outside rectangles x-dimensions

		 if (std::is_integral<Type>::value)
		 {
			 if (p1.x < x1)
				 p1.y = p1.y + ((x1 - p1.x)*angleX >> precision);

			 if (p2.x > x2)
				 p2.y = p2.y - ((p2.x - x2)*angleX >> precision);
		 }
		 else
		 {
			 if (p1.x < x1)
				 p1.y = p1.y + (x1 - p1.x)*angleX;

			 if (p2.x > x2)
				 p2.y = p2.y - (p2.x - x2)*angleX;
		 }

		 // We go from top to bottom when working with the Y coordinates

		 if (p2.y < p1.y)
			 std::swap(p1.y, p2.y);

		 // Verify that clipped line intersects in y-dimension.

		 if (p1.y >= y + h || p2.y <= y)
			 return false;

		 return true;
	 }

	 //____ clipLine() _____________________________________________________________

	template<typename Type>
	bool RectT<Type>::clipLine(CoordT<Type>* p1, CoordT<Type>* p2, int precision) const
	{
		// NOTE: This needs to be kept in sync with IntersectsOrContains() so they always return same result for same input.

		Type x1 = x;
		Type x2 = x + w;
		Type y1 = y;
		Type y2 = y + h;

		// We go from left to right when working with the X coordinates

		if (p2->x < p1->x)
			std::swap(p1, p2);

		// Verify that x-coordinates are within range

		if (p1->x >= x2 || p2->x <= x1)
			return false;

		// Calculate both angles now before we destroy precision
		Type angleX, angleY;
		if (std::is_integral<Type>::value)
		{
			angleX = ((p2->y - p1->y) << precision) / (1 + p2->x - p1->x);		// Change in Y for each increase of X.
			angleY = ((p2->x - p1->x) << precision) / (1 + std::abs(p2->y - p1->y));		// Change in X for each increase of Y.
		}
		else
		{
			angleX = (p2->y - p1->y) / (1 + p2->x - p1->x);					// Change in Y for each increase of X.
			angleY = (p2->x - p1->x) / (1 + std::abs(p2->y - p1->y));		// Change in X for each increase of Y.
		}

		// Clip line so no coord is outside rectangles x-dimensions

		if (p1->x < x1)
		{
			if (std::is_integral<Type>::value)
				p1->y = p1->y + ((x1 - p1->x)*angleX >> precision);
			else
				p1->y = p1->y + (x1 - p1->x)*angleX;
			p1->x = x2;
		}

		if (p2->x > x2)
		{
			if (std::is_integral<Type>::value)
				p2->y = p2->y - ((p2->x - x2)*angleX >> precision);
			else
				p2->y = p2->y - (p2->x - x2)*angleX;

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
			 if (std::is_integral<Type>::value)
				 p1->x = p1->x + ((y1 - p1->y)*angleY >> precision);
			 else
				 p1->x = p1->x + (y1 - p1->y)*angleY;

			 p1->y = y1;
		 }

		 if (p2->y > y2)
		 {
			 if (std::is_integral<Type>::value)
				 p2->x = p2->x - ((p2->y - y2)*angleY >> precision);
			 else
				p2->x = p2->x - (p2->y - y2)*angleY;
			 p2->y = y2;
		 }

		 return true;
	 }



	 //_____________________________________________________________________________
	 inline Size Border::size() const
	 {
		 return Size(((int)left) + right, ((int)top) + bottom);
	 }

	//=======================================================================================

} // namespace wg
#endif	//WG_GEO_DOT_H
