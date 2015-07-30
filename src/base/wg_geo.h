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
//=============================================================================

#ifndef	WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

class WgRect;

//____ Class: WgCoord8 _________________________________________________________

class	WgCoord8
{
public:
	WgCoord8() : x(0), y(0) {}
	WgCoord8( Sint8 x, Sint8 y ) : x(x), y(y) {}

	inline WgCoord8 operator=(const WgCoord8& k)	{ x = k.x; y = k.y; return *this; }

	inline bool operator==(const WgCoord8& k) const	{ if( x == k.x && y == k.y ) return true; return false; }
	inline bool operator!=(const WgCoord8& k) const	{ if( x != k.x || y != k.y ) return true; return false; }

	Sint8		x, y;
};

//____ Class: WgCoord16 ________________________________________________________

class	WgCoord16
{
public:
	WgCoord16() : x(0), y(0) {}
	WgCoord16( Sint16 x, Sint16 y ) : x(x), y(y) {}

	inline WgCoord16 operator=(const WgCoord16& k)	{ x = k.x; y = k.y; return *this; }

	inline bool operator==(const WgCoord16& k) const	{ if( x == k.x && y == k.y ) return true; return false; }
	inline bool operator!=(const WgCoord16& k) const	{ if( x != k.x || y != k.y ) return true; return false; }

	Sint16	x, y;
};

//____ Class: WgUCord16 _______________________________________________________

class	WgUCord16
{
public:
	WgUCord16() : x(0), y(0) {}
	WgUCord16( Uint16 _x, Uint16 _y ) {x=_x;y=_y;};

	inline WgUCord16 operator=(const WgUCord16& k)	{ x = k.x; y = k.y; return *this; }

	inline bool operator==(const WgUCord16& k) const	{ if( x == k.x && y == k.y ) return true; return false; }
	inline bool operator!=(const WgUCord16& k) const	{ if( x != k.x || y != k.y ) return true; return false; }

	Uint16	x, y;
};

//____ Class: WgCoord ________________________________________________________
/**
	@brief Simple class holding the X and Y values of a 2D coordinate.

	Specifies a 2D coordinate through class members x and y, where x is the
	horizontal distance from origo moving from left to right and y is
	the vertical distance moving downwards.

	WgCoord specifies the position in discrete pixels (integer precision).
**/
class	WgCoord
{
public:
	WgCoord() : x(0), y(0) {}
	WgCoord( int x, int y ) : x(x), y(y) {}
	WgCoord( const WgRect& rect );
	WgCoord( WgCoord8 coord8 ) : x(coord8.x), y(coord8.y) {}
	WgCoord( WgCoord16 coord16 ) : x(coord16.x), y(coord16.y) {}

	inline WgCoord operator=(const WgCoord& k)	{ x = k.x; y = k.y; return *this; }
	inline WgCoord operator=(const WgRect& r);

	inline bool operator==(const WgCoord& k) const	{ if( x == k.x && y == k.y ) return true; return false; }
	inline bool operator!=(const WgCoord& k) const	{ if( x != k.x || y != k.y ) return true; return false; }

	inline void operator+=(const WgCoord& k)			{ x += k.x; y += k.y; }
	inline void operator-=(const WgCoord& k)			{ x -= k.x; y -= k.y; }
	inline WgCoord operator+(const WgCoord& k) const	{ WgCoord res; res.x = x + k.x; res.y = y + k.y; return res; }
	inline WgCoord operator-(const WgCoord& k) const	{ WgCoord res; res.x = x - k.x; res.y = y - k.y; return res; }

	inline void operator*=(double v)				{ x = (int) (x*v); y = (int) (y*v); }
	inline void operator/=(double v)				{ x = (int) (x/v); y = (int) (y/v); }
	inline WgCoord operator*(double v) const	{ WgCoord res; res.x = (int) (x*v); res.y = (int) (y*v); return res; }
	inline WgCoord operator/(double v) const	{ WgCoord res; res.x = (int) (x/v); res.y = (int) (y/v); return res; }

	inline void operator*=(float v)				{ x = (int) (x*v); y = (int) (y*v); }
	inline void operator/=(float v)				{ x = (int) (x/v); y = (int) (y/v); }
	inline WgCoord operator*(float v) const	{ WgCoord res; res.x = (int) (x*v); res.y = (int) (y*v); return res; }
	inline WgCoord operator/(float v) const	{ WgCoord res; res.x = (int) (x/v); res.y = (int) (y/v); return res; }

	inline void operator*=(int v)				{ x *= v; y *= v; }
	inline void operator/=(int v)				{ x /= v; y /= v; }
	inline WgCoord operator*(int v) const	{ WgCoord res; res.x = x * v; res.y = y * v; return res; }
	inline WgCoord operator/(int v) const	{ WgCoord res; res.x = x / v; res.y = y / v; return res; }

	inline void clear()						{ x = 0; y = 0; }			///< Sets X and Y to 0.

	int	x, y;
};



class WgSize;

//____ WgBorder ______________________________________________________________
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

class WgBorder
{
public:
	WgBorder() : top(0), right(0), bottom(0), left(0) {}
	WgBorder( short topBottom, short leftRight ) : top(topBottom), right(leftRight), bottom(topBottom), left(leftRight) {}
	WgBorder( short top, short leftRight, short bottom ) : top(top), right(leftRight), bottom(bottom), left(leftRight) {}
	WgBorder( short top, short right, short bottom, short left ) : top(top), right(right), bottom(bottom), left(left) {}
	WgBorder( short all ) : top(all), right(all), bottom(all), left(all) {}

	inline void		set( short all ) { left = right = top = bottom = all; }
	inline void		set( short topBottom, short leftRight ) { top = bottom = topBottom; left = right = leftRight; }
	inline void		set( short top, short leftRight, short bottom ) { this->top = top; this->bottom = bottom; this->left = this->right = leftRight; }
	inline void		set( short top, short left, short bottom, short right ) { this->top = top; this->bottom = bottom; this->left = left; this->right = right; }

	inline WgSize		size() const;
	inline int			width() const { return ((int)left)+right; }
	inline int			height() const { return ((int)top)+bottom; }
	inline void		clear()			{ left = 0; right = 0; top = 0; bottom = 0; }		///< @brief Sets the thickness of all sides to 0.
	inline bool		isEmpty() const { return (left | top | right | bottom) == 0; }

	bool			operator==(const WgBorder& borders) const { return left == borders.left &&
																		right == borders.right &&
																		top == borders.top &&
																		bottom == borders.bottom; }
	bool			operator!=(const WgBorder& borders) const { return !(*this == borders); }

	short		top, right, bottom, left;
};

//____ Class: WgSize ________________________________________________________

/**
	@brief Specifies the size of a rectangular area, measured in pixels.
	 
	Specifies the size of a rectangular area, measured in pixels.
	Members are integer precision and can be negative.
**/

class	WgSize
{
public:
	WgSize() : w(0), h(0) {}
	WgSize( int width, int height ) : w(width), h(height) {}
	WgSize( const WgSize& size ) : w(size.w), h(size.h) {}
	WgSize( const WgRect& rect );
	WgSize( const WgCoord& c1, const WgCoord& c2 ) { w = c2.x - c1.x; h = c2.y - c1.y; }
	inline void limit( const WgSize& min, const WgSize& max );
	inline void clear()		{ w = 0; h = 0; }


	inline WgSize operator=(const WgSize& k)	{ w = k.w; h = k.h; return *this; }
	inline WgSize operator=(const WgRect& k);

	inline bool operator==(const WgSize& k) const	{ if( w == k.w && h == k.h ) return true; return false; }
	inline bool operator!=(const WgSize& k) const	{ if( w != k.w || h != k.h ) return true; return false; }

	inline void operator+=(const WgSize& k)				{ w += k.w; h += k.h; }
	inline void operator-=(const WgSize& k)				{ w -= k.w; h -= k.h; }
	inline WgSize operator+(const WgSize& k) const	{ WgSize res; res.w = w + k.w; res.h = h + k.h; return res; }
	inline WgSize operator-(const WgSize& k) const	{ WgSize res; res.w = w - k.w; res.h = h - k.h; return res; }

	inline void operator+=(const WgBorder& k)			{ w += k.left + k.right; h += k.top + k.bottom; }
	inline void operator-=(const WgBorder& k)			{ w -= k.left + k.right; h -= k.top + k.bottom; }
	inline WgSize operator+(const WgBorder& k) const	{ WgSize res; res.w = w + k.left + k.right; res.h = h + k.top + k.bottom; return res; }
	inline WgSize operator-(const WgBorder& k) const	{ WgSize res; res.w = w - k.left - k.right; res.h = h - k.top - k.bottom; return res; return res; }

	inline void operator*=(double x)				{ w = (int) (w*x); h = (int) (h*x); }
	inline void operator/=(double x)				{ w = (int) (w/x); h = (int) (h/x); }
	inline WgSize operator*(double x) const	{ WgSize res; res.w = (int) (w*x); res.h = (int) (h*x); return res; }
	inline WgSize operator/(double x) const	{ WgSize res; res.w = (int) (w/x); res.h = (int) (h/x); return res; }

	inline void operator*=(float x)				{ w = (int) (w*x); h = (int) (h*x); }
	inline void operator/=(float x)				{ w = (int) (w/x); h = (int) (h/x); }
	inline WgSize operator*(float x) const	{ WgSize res; res.w = (int) (w*x); res.h = (int) (h*x); return res; }
	inline WgSize operator/(float x) const	{ WgSize res; res.w = (int) (w/x); res.h = (int) (h/x); return res; }

	inline void operator*=(int x)				{ w *= x; h *= x; }
	inline void operator/=(int x)				{ w /= x; h /= x; }
	inline WgSize operator*(int x) const	{ WgSize res; res.w = w * x; res.h = h * x; return res; }
	inline WgSize operator/(int x) const	{ WgSize res; res.w = w / x; res.h = h / x; return res; }

	static inline WgSize min( WgSize sz1, WgSize sz2 ) { return WgSize( sz1.w<sz2.w?sz1.w:sz2.w, sz1.h<sz2.h?sz1.h:sz2.h ); }
	static inline WgSize max( WgSize sz1, WgSize sz2 ) { return WgSize( sz1.w>sz2.w?sz1.w:sz2.w, sz1.h>sz2.h?sz1.h:sz2.h ); }

	int	w;		///< Width of the rectangular area in pixels.
	int h;		///< Height of the rectangular area in pixels.

};


//____ WgRect ___________________________________________________________

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
	but various WonderGUI methods accepting a WgRect as parameter might not be able to handle it.
	
**/

class WgRect
{
public:
	WgRect() : x(0), y(0), w(0), h(0) {}								///< @brief Create rectangle with x, y, w and h set to 0.
																		///<
																		///< Create rectangle with x, y, w and h set to 0.
	WgRect( int x, int y, int w, int h ) : x(x), y(y), w(w), h(h) {}	///< @brief Create rectangle with the given values.
																		///<
																		///< Create rectangle with the given values.
	WgRect( const WgRect& r ) : x(r.x), y(r.y), w(r.w), h(r.h) {}		///< @brief Create a copy of specified rectangle.
																		///<
																		///< Create a copy of specified rectangle.
	WgRect( const WgRect& r1, const WgRect& r2 );						///< @brief Create rectangle from intersection of specified rectangles.
	WgRect( const WgCoord& p1, const WgCoord& p2 );						///< @brief	Create rectangle to cover the area between the specified coordinates.
	WgRect( const WgCoord& p, const WgSize& sz ) : x(p.x), y(p.y), w(sz.w), h(sz.h) {} 	///< @brief Create rectangle of specified position and size.
																						///<
																						///< Create rectangle of specified position and size.
																						///< @param p 	Coordinate containing position for rectangle.
																						///< @param sz	Coordinate containing size for rectangle.
	WgRect( const WgCoord& p, int w, int h ) : x(p.x), y(p.y), w(w), h(h) {}			///< @brief Create rectangle of specified position and size.
																						///<
																						///< Create rectangle of specified position and size.
																						///< @param p		Coordinate containing position for rectangle.
																						///< @param w		Width for rectangle.
																						///< @param h		Height for rectangle.
	WgRect( int x, int y, const WgSize& sz ) : x(x), y(y), w(sz.w), h(sz.h) {}			///< @brief Create rectangle of specified position and size.
																						///<
																						///< Create rectangle of specified position and size.
																						///< @param x		Horizontal position for rectangle.
																						///< @param y		Vertical position for rectangle.
																						///< @param sz	Width and height for rectangle.
	WgRect( const WgCoord& p ) : x(p.x), y(p.y), w(0), h(0) {}			///< @brief Create rectangle of specified position and size of (0,0).
																		///<
																		///< Create rectangle of specified position and size of (0,0).
																		///< @param p		Coordinate containing position for rectangle.
	WgRect( const WgSize& sz ) : x(0), y(0), w(sz.w), h(sz.h) {}		///< @brief Create rectangle of specified size and position (0,0).
																		///<
																		///< Create rectangle of specified size and position (0,0).
																		///< @param sz	Width and height for rectangle.
	inline void operator=( const WgRect& );					///< @brief Normal assignment operator.
	inline void operator=( const WgSize& sz);				///< @brief Set rectangle to specified size. Position is cleared.
	inline void operator=( const WgCoord& c);				///< @brief Set rectangle to specified position. Size is cleared.
	inline bool operator==( const WgRect& rect) const;
	inline bool operator!=( const WgRect& rect) const;

	inline void operator+=(const WgSize& k);				///< @brief Increase size of rectangle.
	inline void operator-=(const WgSize& k);				///< @brief Decrease size of rectangle.
	inline WgRect operator+(const WgSize& k) const; 		///< @brief Returns rectangle with size increased by WgSize.
	inline WgRect operator-(const WgSize& k) const; 		///< @brief Returns rectangle with size decreased by WgSize.



	inline void operator+=(const WgCoord& k); 				///< @brief Increase position of rectangle.
	inline void operator-=(const WgCoord& k);				///< @brief Decrease position of rectangle.
	inline WgRect operator+(const WgCoord& k) const; 		///< @brief Returns rectangle with position increased by WgCoord.
	inline WgRect operator-(const WgCoord& k) const; 		///< @brief Returns rectangle with position decreased by WgCoord.

	inline void operator+=(const WgBorder& k);				///< @brief Grow rectangle by the specified border.
	inline void operator-=(const WgBorder& k);
	inline WgRect operator+(const WgBorder& k) const;
	inline WgRect operator-(const WgBorder& k) const;
	
	inline void setPos( const WgCoord& p );					///< @brief Set position of rectangle.
	inline void setSize( const WgSize& sz );				///< @brief Set size of rectangle.

	void shrink( const WgBorder &borders );
	void shrink( int top, int right, int bottom, int left );

	void grow( const WgBorder &borders );
	void grow( int top, int right, int bottom, int left );

	bool intersection( const WgRect& r1, const WgRect& r2 );
	static WgRect getUnion( const WgRect& r1, const WgRect& r2 );

	void growToContain( int x, int y );
	void growToContain( const WgRect& rect );
	void growToContain( const WgCoord& coord );
	
	inline bool contains( int x, int y ) const;				///< @brief Check if coordinate is within rectangle.
	inline bool	contains( const WgCoord& coord ) const;		///< @brief Check if coordinate is within rectangle.
	inline bool contains( const WgRect& rect ) const;		///< @brief Check if rectangle is fully within our rectangle.

	inline WgCoord limit( const WgCoord& coord ) const;		///< @brief Limit given coordinate to stay within rectangle.

	inline bool intersectsWith( const WgRect& rect ) const;	///< @brief Check for intersection (partial or full overlap) with specified rectangle.

	inline int width() const;								///< @brief Get the width of the rectangle.
	inline int height() const;								///< @brief Get the height of the rectangle.
	inline WgSize size() const;								///< @brief Get the size of the rectangle.
	inline WgCoord pos() const;								///< @brief Get the position of the rectangle.

	inline int left() const;								///< @brief Get X coordinate of left border.
	inline int top() const;									///< @brief Get Y cordinate of top border.
	inline int right() const;								///< @brief Get X coordinate of right border.
	inline int bottom() const;								///< @brief Get Y coordinate of bottom border.

	inline bool	isEmpty() const;							///< @brief Check if rectangle has no area.
	inline void clear();									///< @brief Sets all values to zero.

	int	x;		///< Start position in pixels along the X axis.
	int y;		///< Start position in pixels along the Y axis.
	int w;		///< Width, i.e. length of rectangle along the X axis. 
	int h;		///< Height, i.e. length of rectangle along the Y axis.
};

//____ WgRectF ________________________________________________________________

class WgRectF
{
public:
	WgRectF() : x(0.f), y(0.f), w(0.f), h(0.f) {}
	WgRectF( float x, float y, float w, float h ) : x(x), y(y), w(w), h(h) {}	
	WgRectF( const WgRect& r ) : x((float)r.x), y((float)r.y), w((float)r.w), h((float)r.h) {}
	WgRectF( const WgRectF& r ) : x(r.x), y(r.y), w(r.w), h(r.h) {}
	WgRectF( const WgRectF& r1, const WgRectF& r2 );

	bool intersection( const WgRectF& r1, const WgRectF& r2 );
		
	float x, y, w, h;
};


//_____________________________________________________________________________
inline WgCoord::WgCoord( const WgRect& rect )
{
	x = rect.x;
	y = rect.y;
}

//_____________________________________________________________________________
inline WgCoord WgCoord::operator=(const WgRect& r)
{
	x = r.x;
	y = r.y;
	return *this;
}

//_____________________________________________________________________________
inline WgSize WgBorder::size() const
{
	return WgSize( ((int)left)+right, ((int)top)+bottom );
}

//_____________________________________________________________________________
inline WgSize::WgSize( const WgRect& rect )
{
	w = rect.w;
	h = rect.h;
}

//_____________________________________________________________________________
inline WgSize WgSize::operator=(const WgRect& k)
{
	w = k.w;
	h = k.h;
	return *this;
}

//_____________________________________________________________________________
inline void WgSize::limit( const WgSize& min, const WgSize& max )
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
inline bool WgRect::contains( int _x, int _y ) const
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
inline bool WgRect::contains( const WgCoord& _coord ) const
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
inline bool WgRect::contains( const WgRect& _rect ) const
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
 inline WgCoord WgRect::limit( const WgCoord& _coord ) const
{
	WgCoord out = _coord;
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
inline bool WgRect::intersectsWith( const WgRect& _rect ) const
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
inline void WgRect::operator=( const WgRect& r2 )
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
inline void WgRect::operator=( const WgSize& sz)
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
inline void WgRect::operator=( const WgCoord& c)
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
inline bool WgRect::operator==( const WgRect& rect) const
{
	return x == rect.x && y == rect.y && w == rect.w && h == rect.h;
}

//_____________________________________________________________________________
/**
 * Standard comparison operator
 **/
inline bool WgRect::operator!=( const WgRect& rect) const
{
	return !(*this == rect);
}

//_____________________________________________________________________________
/**
 * Increase size of rectangle.
 **/
inline void WgRect::operator+=(const WgSize& k)
{
	w += k.w;
	h += k.h;
}

//_____________________________________________________________________________
/**
 * Decrease size of rectangle.
 **/
inline void WgRect::operator-=(const WgSize& k)
{
	w -= k.w;
	h -= k.h;
}			
//_____________________________________________________________________________
/**
 * Get rectangle with size increased by WgSize.
 **/

inline WgRect WgRect::operator+(const WgSize& k) const
{
	WgRect res;
	res.x = x;
	res.y = y;
	res.w = w + k.w;
	res.h = h + k.h;
	return res;
}

//_____________________________________________________________________________
/**
 * Get rectangle with size decreased with WgSize.
 **/

inline WgRect WgRect::operator-(const WgSize& k) const
{
	WgRect res;
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

inline void WgRect::operator+=(const WgCoord& k)
{
	x += k.x;
	y += k.y;
}

//_____________________________________________________________________________
/**
 * Decrease position of rectangle.
 **/

inline void WgRect::operator-=(const WgCoord& k)
{
	x -= k.x;
	y -= k.y;
}

//_____________________________________________________________________________
/**
 * Get rectangle with position increased with WgCoord.
 **/

inline WgRect WgRect::operator+(const WgCoord& k) const
{
	WgRect res;
	res.x = x + k.x;
	res.y = y + k.y;
	res.w = w;
	res.h = h;
	return res;
}

//_____________________________________________________________________________
/**
 * Get rectangle with position decreased with WgCoord.
 **/
inline WgRect WgRect::operator-(const WgCoord& k) const
{
	WgRect res;
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
inline void WgRect::operator+=(const WgBorder& k)			
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
inline void WgRect::operator-=(const WgBorder& k)			
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
inline WgRect WgRect::operator+(const WgBorder& k) const
{ 
	WgRect res; 
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
inline WgRect WgRect::operator-(const WgBorder& k) const	
{ 
	WgRect res; 
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
inline void WgRect::setPos( const WgCoord& p )
{
	x = p.x;
	y = p.y;
}

//_____________________________________________________________________________
/**
 * Set the size (members w and h) of the rectangle.
 **/
inline void WgRect::setSize( const WgSize& sz )
{
	w = sz.w;
	h = sz.h;
}

//_____________________________________________________________________________
/**
 * Get width of the rectangle (member variable w).
 **/
inline int WgRect::width() const
{
	return w;
}

//_____________________________________________________________________________
/**
 * Get height of the rectangle (member variable h).
 **/
inline int WgRect::height() const
{
	return h;
}

//_____________________________________________________________________________
/**
 * Get size (width and height) of the rectangle.
 **/
inline WgSize WgRect::size() const
{
	return WgSize(w,h);
}

//_____________________________________________________________________________
/**
 * Get position (members x and y) of the rectangle.
 **/
inline WgCoord WgRect::pos() const
{
	return WgCoord(x,y);
}

//_____________________________________________________________________________
/**
 * Get X coordinate of left edge. This is equal to member variable x.
 **/
inline int WgRect::left() const
{
	return x;
}

//_____________________________________________________________________________
/**
 * Get Y coordinate of top edge. This is equal to member variable y.
 **/
inline int WgRect::top() const
{
	return y;
}

//_____________________________________________________________________________
/**
 * Get X coordinate of right edge. This is equal to member variables x + w.
 **/
inline int WgRect::right() const
{
	return x + w;
}

//_____________________________________________________________________________
/**
 * Get Y coordinate of bottom edge. This is equal to member variables y + h.
 **/
inline int WgRect::bottom() const
{
	return y + h;
}
//_____________________________________________________________________________
/**
 * 	Check if rectangle has no area (width or height is zero).
 **/
inline bool	WgRect::isEmpty() const
{
	return (w==0||h==0)?true:false;
}

//_____________________________________________________________________________
/**
 * Sets all values to zero.
 **/
inline void WgRect::clear()
{
	x = 0;
	y = 0;
	w = 0;
	h = 0;
}


//=======================================================================================
#endif	//WG_GEO_DOT_H
