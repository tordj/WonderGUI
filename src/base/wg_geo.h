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

namespace wg 
{
	class Size;
	class Rect;
	class RectF;
	
	//____ Class: Coord8 _________________________________________________________
	
	class	Coord8
	{
	public:
		//.____ Creation __________________________________________

		Coord8() : x(0), y(0) {}
		Coord8( int8_t x, int8_t y ) : x(x), y(y) {}

		//.____ Operators ___________________________________________

		inline Coord8 operator=(const Coord8& k)	{ x = k.x; y = k.y; return *this; }
	
		inline bool operator==(const Coord8& k) const	{ if( x == k.x && y == k.y ) return true; return false; }
		inline bool operator!=(const Coord8& k) const	{ if( x != k.x || y != k.y ) return true; return false; }
	
		//.____ Properties __________________________________________

		int8_t		x, y;
	};
	
	//____ Class: Coord16 ________________________________________________________
	
	class	Coord16
	{
	public:
		//.____ Creation __________________________________________

		Coord16() : x(0), y(0) {}
		Coord16( int16_t x, int16_t y ) : x(x), y(y) {}

		//.____ Operators ___________________________________________

		inline Coord16 operator=(const Coord16& k)	{ x = k.x; y = k.y; return *this; }
	
		inline bool operator==(const Coord16& k) const	{ if( x == k.x && y == k.y ) return true; return false; }
		inline bool operator!=(const Coord16& k) const	{ if( x != k.x || y != k.y ) return true; return false; }

		//.____ Properties __________________________________________

		int16_t	x, y;
	};
	
	
	//____ Class: Coord ________________________________________________________
	/**
		@brief Simple class holding the X and Y values of a 2D coordinate.
	
		Specifies a 2D coordinate through class members x and y, where x is the
		horizontal distance from origo moving from left to right and y is
		the vertical distance moving downwards.
	
		Coord specifies the position in discrete pixels (integer precision).
	**/
	class	Coord
	{
	public:
		//.____ Creation __________________________________________

		Coord() : x(0), y(0) {}
		Coord( int x, int y ) : x(x), y(y) {}
		Coord( const Rect& rect );
		Coord( Coord8 coord8 ) : x(coord8.x), y(coord8.y) {}
		Coord( Coord16 coord16 ) : x(coord16.x), y(coord16.y) {}

		//.____ Misc ______________________________________________

		inline Size toSize();
		inline void clear() { x = 0; y = 0; }			///< Sets X and Y to 0.

		//.____ Operators ___________________________________________

		inline Coord operator=(const Coord& k)	{ x = k.x; y = k.y; return *this; }
		inline Coord operator=(const Rect& r);
	
		inline bool operator==(const Coord& k) const	{ if( x == k.x && y == k.y ) return true; return false; }
		inline bool operator!=(const Coord& k) const	{ if( x != k.x || y != k.y ) return true; return false; }
	
		inline void operator+=(const Coord& k)			{ x += k.x; y += k.y; }
		inline void operator-=(const Coord& k)			{ x -= k.x; y -= k.y; }
		inline Coord operator+(const Coord& k) const	{ Coord res; res.x = x + k.x; res.y = y + k.y; return res; }
		inline Coord operator-(const Coord& k) const	{ Coord res; res.x = x - k.x; res.y = y - k.y; return res; }
	
		inline void operator*=(double v)				{ x = (int) (x*v); y = (int) (y*v); }
		inline void operator/=(double v)				{ x = (int) (x/v); y = (int) (y/v); }
		inline Coord operator*(double v) const	{ Coord res; res.x = (int) (x*v); res.y = (int) (y*v); return res; }
		inline Coord operator/(double v) const	{ Coord res; res.x = (int) (x/v); res.y = (int) (y/v); return res; }
	
		inline void operator*=(float v)				{ x = (int) (x*v); y = (int) (y*v); }
		inline void operator/=(float v)				{ x = (int) (x/v); y = (int) (y/v); }
		inline Coord operator*(float v) const	{ Coord res; res.x = (int) (x*v); res.y = (int) (y*v); return res; }
		inline Coord operator/(float v) const	{ Coord res; res.x = (int) (x/v); res.y = (int) (y/v); return res; }
	
		inline void operator*=(int v)				{ x *= v; y *= v; }
		inline void operator/=(int v)				{ x /= v; y /= v; }
		inline Coord operator*(int v) const	{ Coord res; res.x = x * v; res.y = y * v; return res; }
		inline Coord operator/(int v) const	{ Coord res; res.x = x / v; res.y = y / v; return res; }

		//.____ Properties __________________________________________
	
		int	x, y;
	};
	
	//____ Class: CoordF _______________________________________________________
	
	class	CoordF
	{
	public:
		//.____ Creation __________________________________________

		CoordF() : x(0.f), y(0.f) {}
		CoordF( float _x, float _y ) {x=_x;y=_y;};
	
		//.____ Operators ___________________________________________

		inline CoordF operator=(const CoordF& k)	{ x = k.x; y = k.y; return *this; }
	
		inline bool operator==(const CoordF& k) const	{ if( x == k.x && y == k.y ) return true; return false; }
		inline bool operator!=(const CoordF& k) const	{ if( x != k.x || y != k.y ) return true; return false; }
	
		//.____ Properties __________________________________________

		float	x, y;
	};
	
	
	class Size;
	
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
		inline void		set( short top, short left, short bottom, short right ) { this->top = top; this->bottom = bottom; this->left = left; this->right = right; }
	
		inline Size		size() const;
		inline int		width() const { return ((int)left)+right; }
		inline int		height() const { return ((int)top)+bottom; }
		inline void		clear()			{ left = 0; right = 0; top = 0; bottom = 0; }		///< @brief Sets the thickness of all sides to 0.
		inline bool		isEmpty() const { return (left | top | right | bottom) == 0; }
	
		//.____ Operators ___________________________________________

		bool			operator==(const Border& borders) const { return left == borders.left &&
																			right == borders.right &&
																			top == borders.top &&
																			bottom == borders.bottom; }
		bool			operator!=(const Border& borders) const { return !(*this == borders); }
	
		//.____ Properties __________________________________________

		short		top, right, bottom, left;
	};
	
	//____ Class: Size ________________________________________________________
	
	/**
		@brief Specifies the size of a rectangular area, measured in pixels.
		 
		Specifies the size of a rectangular area, measured in pixels.
		Members are integer precision and can be negative.
	**/
	
	class	Size
	{
	public:
		//.____ Creation __________________________________________

		Size() : w(0), h(0) {}
		Size( int width, int height ) : w(width), h(height) {}
		Size( const Size& size ) : w(size.w), h(size.h) {}
		Size( const Rect& rect );
		Size( const Coord& c1, const Coord& c2 ) { w = c2.x - c1.x; h = c2.y - c1.y; }

		//.____ Misc ______________________________________________

		inline void limit( const Size& min, const Size& max );
		inline void clear()		{ w = 0; h = 0; }

		inline Coord toCoord() { return Coord(w,h); }
	
		//.____ Operators ___________________________________________

		inline Size operator=(const Size& k)	{ w = k.w; h = k.h; return *this; }
		inline Size operator=(const Rect& k);
	
		inline bool operator==(const Size& k) const	{ if( w == k.w && h == k.h ) return true; return false; }
		inline bool operator!=(const Size& k) const	{ if( w != k.w || h != k.h ) return true; return false; }
	
		inline void operator+=(const Size& k)				{ w += k.w; h += k.h; }
		inline void operator-=(const Size& k)				{ w -= k.w; h -= k.h; }
		inline Size operator+(const Size& k) const	{ Size res; res.w = w + k.w; res.h = h + k.h; return res; }
		inline Size operator-(const Size& k) const	{ Size res; res.w = w - k.w; res.h = h - k.h; return res; }
	
		inline void operator+=(const Border& k)			{ w += k.left + k.right; h += k.top + k.bottom; }
		inline void operator-=(const Border& k)			{ w -= k.left + k.right; h -= k.top + k.bottom; }
		inline Size operator+(const Border& k) const	{ Size res; res.w = w + k.left + k.right; res.h = h + k.top + k.bottom; return res; }
		inline Size operator-(const Border& k) const	{ Size res; res.w = w - k.left - k.right; res.h = h - k.top - k.bottom; return res; return res; }
	
		inline void operator*=(double x)				{ w = (int) (w*x); h = (int) (h*x); }
		inline void operator/=(double x)				{ w = (int) (w/x); h = (int) (h/x); }
		inline Size operator*(double x) const	{ Size res; res.w = (int) (w*x); res.h = (int) (h*x); return res; }
		inline Size operator/(double x) const	{ Size res; res.w = (int) (w/x); res.h = (int) (h/x); return res; }
	
		inline void operator*=(float x)				{ w = (int) (w*x); h = (int) (h*x); }
		inline void operator/=(float x)				{ w = (int) (w/x); h = (int) (h/x); }
		inline Size operator*(float x) const	{ Size res; res.w = (int) (w*x); res.h = (int) (h*x); return res; }
		inline Size operator/(float x) const	{ Size res; res.w = (int) (w/x); res.h = (int) (h/x); return res; }
	
		inline void operator*=(int x)				{ w *= x; h *= x; }
		inline void operator/=(int x)				{ w /= x; h /= x; }
		inline Size operator*(int x) const	{ Size res; res.w = w * x; res.h = h * x; return res; }
		inline Size operator/(int x) const	{ Size res; res.w = w / x; res.h = h / x; return res; }
	
		static inline Size min( Size sz1, Size sz2 ) { return Size( sz1.w<sz2.w?sz1.w:sz2.w, sz1.h<sz2.h?sz1.h:sz2.h ); }
		static inline Size max( Size sz1, Size sz2 ) { return Size( sz1.w>sz2.w?sz1.w:sz2.w, sz1.h>sz2.h?sz1.h:sz2.h ); }
	
		//.____ Properties __________________________________________

		int	w;		///< Width of the rectangular area in pixels.
		int h;		///< Height of the rectangular area in pixels.
	
	};
	

	//____ Class: SizeF ________________________________________________________

	/**
	@brief Specifies the size of a rectangular area.

	Specifies the size of a rectangular area.
	Members are float precision and can be negative.
	**/

	class	SizeF
	{
	public:
		//.____ Creation __________________________________________

		SizeF() : w(0.f), h(0.f) {}
		SizeF(float width, float height) : w(width), h(height) {}
		SizeF(const SizeF& size) : w(size.w), h(size.h) {}
		SizeF(const RectF& rect);
		SizeF(const CoordF& c1, const CoordF& c2) { w = c2.x - c1.x; h = c2.y - c1.y; }

		//.____ Misc ______________________________________________

		inline void limit(const SizeF& min, const SizeF& max);
		inline void clear() { w = 0.f; h = 0.f; }

		inline CoordF toCoord() { return CoordF(w, h); }

		//.____ Operators ___________________________________________

		inline SizeF operator=(const SizeF& k) { w = k.w; h = k.h; return *this; }
		inline SizeF operator=(const RectF& k);

		inline bool operator==(const SizeF& k) const { if (w == k.w && h == k.h) return true; return false; }
		inline bool operator!=(const SizeF& k) const { if (w != k.w || h != k.h) return true; return false; }

		inline void operator+=(const SizeF& k) { w += k.w; h += k.h; }
		inline void operator-=(const SizeF& k) { w -= k.w; h -= k.h; }
		inline SizeF operator+(const SizeF& k) const { SizeF res; res.w = w + k.w; res.h = h + k.h; return res; }
		inline SizeF operator-(const SizeF& k) const { SizeF res; res.w = w - k.w; res.h = h - k.h; return res; }
/*
		inline void operator+=(const Border& k) { w += k.left + k.right; h += k.top + k.bottom; }
		inline void operator-=(const Border& k) { w -= k.left + k.right; h -= k.top + k.bottom; }
		inline Size operator+(const Border& k) const { Size res; res.w = w + k.left + k.right; res.h = h + k.top + k.bottom; return res; }
		inline Size operator-(const Border& k) const { Size res; res.w = w - k.left - k.right; res.h = h - k.top - k.bottom; return res; return res; }
*/
		inline void operator*=(double x) { w = (float)(w*x); h = (float)(h*x); }
		inline void operator/=(double x) { w = (float)(w / x); h = (float)(h / x); }
		inline SizeF operator*(double x) const { SizeF res; res.w = (float)(w*x); res.h = (float)(h*x); return res; }
		inline SizeF operator/(double x) const { SizeF res; res.w = (float)(w / x); res.h = (float)(h / x); return res; }

		inline void operator*=(float x) { w *= x; h *= x; }
		inline void operator/=(float x) { w /= x; h /= x; }
		inline SizeF operator*(float x) const { SizeF res; res.w = w*x; res.h = h*x; return res; }
		inline SizeF operator/(float x) const { SizeF res; res.w = w / x; res.h = h / x; return res; }

		inline void operator*=(int x) { w *= x; h *= x; }
		inline void operator/=(int x) { w /= x; h /= x; }
		inline SizeF operator*(int x) const { SizeF res; res.w = w * x; res.h = h * x; return res; }
		inline SizeF operator/(int x) const { SizeF res; res.w = w / x; res.h = h / x; return res; }

		static inline SizeF min(SizeF sz1, Size sz2) { return SizeF(sz1.w<sz2.w ? sz1.w : sz2.w, sz1.h<sz2.h ? sz1.h : sz2.h); }
		static inline SizeF max(SizeF sz1, Size sz2) { return SizeF(sz1.w>sz2.w ? sz1.w : sz2.w, sz1.h>sz2.h ? sz1.h : sz2.h); }

		//.____ Properties __________________________________________

		float	w;		///< Width of the rectangular area in pixels.
		float	h;		///< Height of the rectangular area in pixels.

	};


	
	//____ Rect ___________________________________________________________
	
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
	
	class Rect
	{
	public:
		//.____ Creation __________________________________________

		Rect() : x(0), y(0), w(0), h(0) {}								///< @brief Create rectangle with x, y, w and h set to 0.
																			///<
																			///< Create rectangle with x, y, w and h set to 0.
		Rect( int x, int y, int w, int h ) : x(x), y(y), w(w), h(h) {}	///< @brief Create rectangle with the given values.
																			///<
																			///< Create rectangle with the given values.
		Rect( const Rect& r ) : x(r.x), y(r.y), w(r.w), h(r.h) {}		///< @brief Create a copy of specified rectangle.
																			///<
																			///< Create a copy of specified rectangle.
		Rect( const Rect& r1, const Rect& r2 );						///< @brief Create rectangle from intersection of specified rectangles.
		Rect( const Coord& p1, const Coord& p2 );						///< @brief	Create rectangle to cover the area between the specified coordinates.
		Rect( const Coord& p, const Size& sz ) : x(p.x), y(p.y), w(sz.w), h(sz.h) {} 	///< @brief Create rectangle of specified position and size.
																							///<
																							///< Create rectangle of specified position and size.
																							///< @param p 	Coordinate containing position for rectangle.
																							///< @param sz	Coordinate containing size for rectangle.
		Rect( const Coord& p, int w, int h ) : x(p.x), y(p.y), w(w), h(h) {}			///< @brief Create rectangle of specified position and size.
																							///<
																							///< Create rectangle of specified position and size.
																							///< @param p		Coordinate containing position for rectangle.
																							///< @param w		Width for rectangle.
																							///< @param h		Height for rectangle.
		Rect( int x, int y, const Size& sz ) : x(x), y(y), w(sz.w), h(sz.h) {}			///< @brief Create rectangle of specified position and size.
																							///<
																							///< Create rectangle of specified position and size.
																							///< @param x		Horizontal position for rectangle.
																							///< @param y		Vertical position for rectangle.
																							///< @param sz	Width and height for rectangle.
		Rect( const Coord& p ) : x(p.x), y(p.y), w(0), h(0) {}			///< @brief Create rectangle of specified position and size of (0,0).
																			///<
																			///< Create rectangle of specified position and size of (0,0).
																			///< @param p		Coordinate containing position for rectangle.
		Rect( const Size& sz ) : x(0), y(0), w(sz.w), h(sz.h) {}		///< @brief Create rectangle of specified size and position (0,0).
																			///<
																			///< Create rectangle of specified size and position (0,0).
																			///< @param sz	Width and height for rectangle.
		
		//.____ Misc ______________________________________________

		inline void setPos( const Coord& p );					///< @brief Set position of rectangle.
		inline void setSize( const Size& sz );					///< @brief Set size of rectangle.
	
		void shrink( const Border &borders );
		void shrink( int top, int right, int bottom, int left );
	
		void grow( const Border &borders );
		void grow( int top, int right, int bottom, int left );
	
		bool intersection( const Rect& r1, const Rect& r2 );
		static Rect getUnion( const Rect& r1, const Rect& r2 );
	
		void growToContain( int x, int y );
		void growToContain( const Rect& rect );
		void growToContain( const Coord& coord );
		
		inline bool contains( int x, int y ) const;				///< @brief Check if coordinate is within rectangle.
		inline bool	contains( const Coord& coord ) const;		///< @brief Check if coordinate is within rectangle.
		inline bool contains( const Rect& rect ) const;			///< @brief Check if rectangle is fully within our rectangle.
	
		inline Coord limit( const Coord& coord ) const;			///< @brief Limit given coordinate to stay within rectangle.
	
		inline bool intersectsWith( const Rect& rect ) const;	///< @brief Check for intersection (partial or full overlap) with specified rectangle.
	
		inline int width() const;								///< @brief Get the width of the rectangle.
		inline int height() const;								///< @brief Get the height of the rectangle.
		inline Size size() const;								///< @brief Get the size of the rectangle.
		inline Coord pos() const;								///< @brief Get the position of the rectangle.
	
		inline int left() const;								///< @brief Get X coordinate of left border.
		inline int top() const;									///< @brief Get Y cordinate of top border.
		inline int right() const;								///< @brief Get X coordinate of right border.
		inline int bottom() const;								///< @brief Get Y coordinate of bottom border.
	
		inline bool	isEmpty() const;							///< @brief Check if rectangle has no area.
		inline void clear();									///< @brief Sets all values to zero.

		//.____ Operators ___________________________________________

		inline void operator=(const Rect&);					///< @brief Normal assignment operator.
		inline void operator=(const Size& sz);				///< @brief Set rectangle to specified size. Position is cleared.
		inline void operator=(const Coord& c);				///< @brief Set rectangle to specified position. Size is cleared.
		inline bool operator==(const Rect& rect) const;
		inline bool operator!=(const Rect& rect) const;

		inline void operator+=(const Size& k);				///< @brief Increase size of rectangle.
		inline void operator-=(const Size& k);				///< @brief Decrease size of rectangle.
		inline Rect operator+(const Size& k) const; 		///< @brief Returns rectangle with size increased by Size.
		inline Rect operator-(const Size& k) const; 		///< @brief Returns rectangle with size decreased by Size.



		inline void operator+=(const Coord& k); 				///< @brief Increase position of rectangle.
		inline void operator-=(const Coord& k);				///< @brief Decrease position of rectangle.
		inline Rect operator+(const Coord& k) const; 		///< @brief Returns rectangle with position increased by Coord.
		inline Rect operator-(const Coord& k) const; 		///< @brief Returns rectangle with position decreased by Coord.

		inline void operator+=(const Border& k);				///< @brief Grow rectangle by the specified border.
		inline void operator-=(const Border& k);
		inline Rect operator+(const Border& k) const;
		inline Rect operator-(const Border& k) const;

		//.____ Properties __________________________________________

		int	x;		///< Start position in pixels along the X axis.
		int y;		///< Start position in pixels along the Y axis.
		int w;		///< Width, i.e. length of rectangle along the X axis. 
		int h;		///< Height, i.e. length of rectangle along the Y axis.
	};
	
	//____ RectF ________________________________________________________________
	
	class RectF
	{
	public:
		//.____ Creation __________________________________________

		RectF() : x(0.f), y(0.f), w(0.f), h(0.f) {}
		RectF( float x, float y, float w, float h ) : x(x), y(y), w(w), h(h) {}	
		RectF( const Rect& r ) : x((float)r.x), y((float)r.y), w((float)r.w), h((float)r.h) {}
		RectF( const RectF& r ) : x(r.x), y(r.y), w(r.w), h(r.h) {}
		RectF( const RectF& r1, const RectF& r2 );
		RectF(const CoordF& pos, const SizeF& size) : x(pos.x), y(pos.y), w(size.w), h(size.h) {}

		//.____ Misc ______________________________________________

		bool intersection( const RectF& r1, const RectF& r2 );
			
		//.____ Properties __________________________________________

		float x, y, w, h;
	};
	
	
	//_____________________________________________________________________________
	inline Coord::Coord( const Rect& rect )
	{
		x = rect.x;
		y = rect.y;
	}
	
	//_____________________________________________________________________________
	inline Coord Coord::operator=(const Rect& r)
	{
		x = r.x;
		y = r.y;
		return *this;
	}
	
	//_____________________________________________________________________________
	inline Size Coord::toSize()
	{ 
		return Size(x,y); 
	}
	
	
	//_____________________________________________________________________________
	inline Size Border::size() const
	{
		return Size( ((int)left)+right, ((int)top)+bottom );
	}
	
	//_____________________________________________________________________________
	inline Size::Size( const Rect& rect )
	{
		w = rect.w;
		h = rect.h;
	}
	
	//_____________________________________________________________________________
	inline Size Size::operator=(const Rect& k)
	{
		w = k.w;
		h = k.h;
		return *this;
	}
	
	//_____________________________________________________________________________
	inline void Size::limit( const Size& min, const Size& max )
	{
		if( w < min.w )
			w = min.w;
		if( h < min.h )
			h = min.h;
	
		if( w > max.w )
			w = max.w;
		if( h > max.h )
			h = max.h;
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
	inline bool Rect::contains( int _x, int _y ) const
	{
		if( _x >= x && _x < x + w && _y >= y && _y < y + h )
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
	inline bool Rect::contains( const Coord& _coord ) const
	{
		if( _coord.x >= x && _coord.x < x + w && _coord.y >= y && _coord.y < y + h )
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
	inline bool Rect::contains( const Rect& _rect ) const
	{
		if( _rect.x >= x && _rect.y >= y && _rect.x + _rect.w <= x + w &&  _rect.y + _rect.h <= y + h )
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
	 inline Coord Rect::limit( const Coord& _coord ) const
	{
		Coord out = _coord;
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
	inline bool Rect::intersectsWith( const Rect& _rect ) const
	{
		if(left() >= _rect.right()) return false;
		if(right() <= _rect.left()) return false;
		if(top() >= _rect.bottom()) return false;
		if(bottom() <= _rect.top()) return false;
	
		return true;
	}
	
	//_____________________________________________________________________________
	/**
	 * Normal assignment operator.
	 **/
	inline void Rect::operator=( const Rect& r2 )
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
	inline void Rect::operator=( const Size& sz)
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
	inline void Rect::operator=( const Coord& c)
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
	inline bool Rect::operator==( const Rect& rect) const
	{
		return x == rect.x && y == rect.y && w == rect.w && h == rect.h;
	}
	
	//_____________________________________________________________________________
	/**
	 * Standard comparison operator
	 **/
	inline bool Rect::operator!=( const Rect& rect) const
	{
		return !(*this == rect);
	}
	
	//_____________________________________________________________________________
	/**
	 * Increase size of rectangle.
	 **/
	inline void Rect::operator+=(const Size& k)
	{
		w += k.w;
		h += k.h;
	}
	
	//_____________________________________________________________________________
	/**
	 * Decrease size of rectangle.
	 **/
	inline void Rect::operator-=(const Size& k)
	{
		w -= k.w;
		h -= k.h;
	}			
	//_____________________________________________________________________________
	/**
	 * Get rectangle with size increased by Size.
	 **/
	
	inline Rect Rect::operator+(const Size& k) const
	{
		Rect res;
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
	
	inline Rect Rect::operator-(const Size& k) const
	{
		Rect res;
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
	
	inline void Rect::operator+=(const Coord& k)
	{
		x += k.x;
		y += k.y;
	}
	
	//_____________________________________________________________________________
	/**
	 * Decrease position of rectangle.
	 **/
	
	inline void Rect::operator-=(const Coord& k)
	{
		x -= k.x;
		y -= k.y;
	}
	
	//_____________________________________________________________________________
	/**
	 * Get rectangle with position increased with Coord.
	 **/
	
	inline Rect Rect::operator+(const Coord& k) const
	{
		Rect res;
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
	inline Rect Rect::operator-(const Coord& k) const
	{
		Rect res;
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
	inline void Rect::operator+=(const Border& k)			
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
	inline void Rect::operator-=(const Border& k)			
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
	inline Rect Rect::operator+(const Border& k) const
	{ 
		Rect res; 
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
	inline Rect Rect::operator-(const Border& k) const	
	{ 
		Rect res; 
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
	inline void Rect::setPos( const Coord& p )
	{
		x = p.x;
		y = p.y;
	}
	
	//_____________________________________________________________________________
	/**
	 * Set the size (members w and h) of the rectangle.
	 **/
	inline void Rect::setSize( const Size& sz )
	{
		w = sz.w;
		h = sz.h;
	}
	
	//_____________________________________________________________________________
	/**
	 * Get width of the rectangle (member variable w).
	 **/
	inline int Rect::width() const
	{
		return w;
	}
	
	//_____________________________________________________________________________
	/**
	 * Get height of the rectangle (member variable h).
	 **/
	inline int Rect::height() const
	{
		return h;
	}
	
	//_____________________________________________________________________________
	/**
	 * Get size (width and height) of the rectangle.
	 **/
	inline Size Rect::size() const
	{
		return Size(w,h);
	}
	
	//_____________________________________________________________________________
	/**
	 * Get position (members x and y) of the rectangle.
	 **/
	inline Coord Rect::pos() const
	{
		return Coord(x,y);
	}
	
	//_____________________________________________________________________________
	/**
	 * Get X coordinate of left edge. This is equal to member variable x.
	 **/
	inline int Rect::left() const
	{
		return x;
	}
	
	//_____________________________________________________________________________
	/**
	 * Get Y coordinate of top edge. This is equal to member variable y.
	 **/
	inline int Rect::top() const
	{
		return y;
	}
	
	//_____________________________________________________________________________
	/**
	 * Get X coordinate of right edge. This is equal to member variables x + w.
	 **/
	inline int Rect::right() const
	{
		return x + w;
	}
	
	//_____________________________________________________________________________
	/**
	 * Get Y coordinate of bottom edge. This is equal to member variables y + h.
	 **/
	inline int Rect::bottom() const
	{
		return y + h;
	}
	//_____________________________________________________________________________
	/**
	 * 	Check if rectangle has no area (width or height is zero).
	 **/
	inline bool	Rect::isEmpty() const
	{
		return (w==0||h==0)?true:false;
	}
	
	//_____________________________________________________________________________
	/**
	 * Sets all values to zero.
	 **/
	inline void Rect::clear()
	{
		x = 0;
		y = 0;
		w = 0;
		h = 0;
	}
	
	
	//=======================================================================================

} // namespace wg
#endif	//WG_GEO_DOT_H
