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

	inline void Clear()						{ x = 0; y = 0; }			///< Sets X and Y to 0.

	int	x, y;
};



class WgSize;

//____ WgBorder ______________________________________________________________
/**
	@brief Specifies the thickness of the border of a rectangular area, measured in pixels.
	
	The thickness of the four sides of the border can be set individually.
	The sides of the border always comes in the order: top, right, bottom, left. I.e.
	you start with the top border and go clockwise.
	
	Border are in most cases internal to the rectangular areas geometry, so the combined thickness of left and right
	sides may not surpass the rectangles width and the combined thickness of top and bottom sides may not surpass
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

	inline void		Set( short all ) { left = right = top = bottom = all; }
	inline void		Set( short topBottom, short leftRight ) { top = bottom = topBottom; left = right = leftRight; }
	inline void		Set( short top, short leftRight, short bottom ) { this->top = top; this->bottom = bottom; this->left = this->right = leftRight; }
	inline void		Set( short top, short left, short bottom, short right ) { this->top = top; this->bottom = bottom; this->left = left; this->right = right; }

	inline WgSize		Size() const;
	inline int			Width() const { return ((int)left)+right; }
	inline int			Height() const { return ((int)top)+bottom; }
	inline void		Clear()			{ left = 0; right = 0; top = 0; bottom = 0; }		///< @brief Sets the thickness of all sides to 0.
	inline bool		IsEmpty() const { return (left | top | right | bottom) == 0; }

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
	inline void Limit( const WgSize& min, const WgSize& max );
	inline void Clear()		{ w = 0; h = 0; }


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

	static inline WgSize Min( WgSize sz1, WgSize sz2 ) { return WgSize( sz1.w<sz2.w?sz1.w:sz2.w, sz1.h<sz2.h?sz1.h:sz2.h ); }
	static inline WgSize Max( WgSize sz1, WgSize sz2 ) { return WgSize( sz1.w>sz2.w?sz1.w:sz2.w, sz1.h>sz2.h?sz1.h:sz2.h ); }

	int	w;		///< Width of the rectangular area in pixels.
	int h;		///< Height of the rectangular area in pixels.

};


//____ WgRect ___________________________________________________________

/**
	@brief Specifies a rectangular area of a 2D surface through position and size.
 
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
	WgRect() : x(0), y(0), w(0), h(0) {}		///< @brief Initializes the rectangle with x, y, w and h set to 0.
	WgRect( int x, int y, int w, int h ) : x(x), y(y), w(w), h(h) {}	///< @brief Initializes a rectangle with the given values.
	WgRect( const WgRect& r ) : x(r.x), y(r.y), w(r.w), h(r.h) {}		///< @brief Initializes a copy of specified rectangle.	
	WgRect( const WgRect& r1, const WgRect& r2 );
	WgRect( const WgCoord& p1, const WgCoord& p2 );
	WgRect( const WgCoord& p, const WgSize& sz ) : x(p.x), y(p.y), w(sz.w), h(sz.h) {} 	///< @brief Initializes a rectangle with specified position and size.
																									///< @param p 	Coordinate containing position for rectangle.
																									///< @param sz	Coordinate containing size for rectangle.
	WgRect( const WgCoord& p, int w, int h ) : x(p.x), y(p.y), w(w), h(h) {}				///< @brief Initializes a rectangle with specified position and size.
																									///< @param p		Coordinate containing position for rectangle.
																									///< @param w		Width for rectangle.
																									///< @param h		Height for rectangle.
	WgRect( int x, int y, const WgSize& sz ) : x(x), y(y), w(sz.w), h(sz.h) {}				///< @brief Initializes a rectangle with specified position and size.
																									///< @param x		Horizontal position for rectangle.
																									///< @param y		Vertical position for rectangle.
																									///< @param sz	Width and height for rectangle. 
	WgRect( const WgCoord& p ) : x(p.x), y(p.y), w(0), h(0) {}								///< @brief Initializes a rectangle to specified position, with width and height set to 0.
																									///< @param p		Coordinate containing position for rectangle.
	WgRect( const WgSize& sz ) : x(0), y(0), w(sz.w), h(sz.h) {}								///< @brief Initializes a rectangle to specified size, with an x- and y-postion of 0.
																									///< @param sz	Width and height for rectangle.
	inline void operator=( const WgRect& );													///< @brief Normal assignment operator.
	inline void operator=( const WgSize& sz) { x=0; y=0; w=sz.w; h=sz.h; }					///< @brief Sets rectangle to specified size. Position is cleared.
	inline void operator=( const WgCoord& c) { x=c.x; y=c.y; w=0; h=0; }					///< @brief Sets rectangle to specified position. Size is cleared.
	inline bool operator==( const WgRect& rect) const { return x == rect.x && y == rect.y && w == rect.w && h == rect.h; }
	inline bool operator!=( const WgRect& rect) const { return !(*this == rect); }

	inline void operator+=(const WgSize& k)				{ w += k.w; h += k.h; }			///< @brief Increase size of rectangle.
	inline void operator-=(const WgSize& k)				{ w -= k.w; h -= k.h; }			///< @brief Decrease size of rectangle.
	inline WgRect operator+(const WgSize& k) const	{ WgRect res; res.x = x; res.y = y; res.w = w + k.w; res.h = h + k.h; return res; } ///< @brief Returns rectangle with size increased by WgSize.
	inline WgRect operator-(const WgSize& k) const	{ WgRect res; res.x = x; res.y = y; res.w = w - k.w; res.h = h - k.h; return res; } ///< @brief Returns rectangle with size decreased by WgSize.



	inline void operator+=(const WgCoord& k)				{ x += k.x; y += k.y; }			///< @brief Increase position of rectangle.
	inline void operator-=(const WgCoord& k)				{ x -= k.x; y -= k.y; }			///< @brief Decrease position of rectangle.
	inline WgRect operator+(const WgCoord& k) const	{ WgRect res; res.x = x + k.x; res.y = y + k.y; res.w = w; res.h = h; return res; } ///< @brief Returns rectangle with position increased by WgCoord.
	inline WgRect operator-(const WgCoord& k) const	{ WgRect res; res.x = x - k.x; res.y = y - k.y; res.w = w; res.h = h; return res; } ///< @brief Returns rectangle with position decreased by WgCoord.

	inline void operator+=(const WgBorder& k);
	inline void operator-=(const WgBorder& k);
	inline WgRect operator+(const WgBorder& k) const;
	inline WgRect operator-(const WgBorder& k) const;
	
	inline void SetPos( const WgCoord& p ) { x = p.x; y = p.y; }				///< @brief Sets the position (members x and y) of the rectangle.
	inline void SetSize( const WgSize& sz ) { w = sz.w; h = sz.h; }			///< @brief Sets the size (members w and h) of the rectangle.

	void Shrink( const WgBorder &borders );
	void Shrink( int top, int right, int bottom, int left );

	void Grow( const WgBorder &borders );
	void Grow( int top, int right, int bottom, int left );

	bool Intersection( const WgRect& r1, const WgRect& r2 );
	static WgRect Union( const WgRect& r1, const WgRect& r2 );

	void GrowToContain( int x, int y );
	void GrowToContain( const WgRect& rect );
	void GrowToContain( const WgCoord& coord );
	
	inline bool Contains( int x, int y ) const;								///< @brief Check if given coordinate is within rectangle.
																					///< @param x Horizontal position of coordinate of to check.
																					///< @param y Vertical position of coordinate of to check.
																					///< @return True if coordinate is within the bounds of the rectangle.
	inline bool Contains( const WgRect& rect ) const;							///< @brief Check if parameter rectangle is fully within our rectangle.
																					///< @param rect Rectangle to check.
																					///< @return True if the parameter rectangle is fully within the bounds of our rectangle.
	inline bool	Contains( const WgCoord& coord ) const;					///< @brief Check if given coordinate is within rectangle
																					///< @param coord Coordinate to check
																					///< @return True if coordinate is within the bounds of the rectangle.
	inline WgCoord Limit( const WgCoord& coord ) const;						///< @brief Limit given coordinate to stay within rectangle.
																					///< @param coord Coordinate to limit to the bounds of the rectangle.
																					///< @return Coordinate limited to the rectangles geometry.

	inline bool IntersectsWith( const WgRect& rect ) const;					///< @brief Checks for intersection (partial or full overlap) with specified rectangle.
																					///< @param rect Rectangle to check against.
																					///< @return True if rectangles intersects.

	inline int Width() const		{ return w; }									///< @brief Returns the width of the rectangle (member variable w).
	inline int Height() const	{ return h; }									///< @brief Returns the height of the rectangle (member variable h).
	inline WgSize Size() const		{ return WgSize(w,h); }					///< @brief Returns the width and height of the rectangle as a WgSize object.
	inline WgCoord Pos() const		{ return WgCoord(x,y); }					///< @brief Returns the X- and Y-coordinates as a WgCoord object.

	inline int Left() const		{ return x; }									///< @brief Returns the X coordinate of the left border. This is equal to member variable x.
	inline int Top() const		{ return y; }									///< @brief Returns the Y cordinate of the top border. This is equal to member variable y.
	inline int Right() const		{ return x + w; }								///< @brief Returns the X coordinate of the right border. This is equal to member variables x + w.
	inline int Bottom() const	{ return y + h; }								///< @brief Returns the Y coordinate of the bottom border. This is equal to member variables y + h.

	inline bool	IsEmpty() const { return (w==0||h==0)?true:false; }		///< @brief Returns true if rectangle has no area (width or height is zero).
	inline void Clear()			{ x = 0; y = 0; w = 0; h = 0; }				///< @brief Sets all values to zero.

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

	bool Intersection( const WgRectF& r1, const WgRectF& r2 );
		
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
inline WgSize WgBorder::Size() const
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
inline void WgSize::Limit( const WgSize& min, const WgSize& max )
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

//_____________________________________________________________________________
inline bool WgRect::Contains( int _x, int _y ) const
{
	if( _x >= x && _x < x + w && _y >= y && _y < y + h )
		return true;
	return false;
}

//_____________________________________________________________________________
inline bool WgRect::Contains( const WgCoord& _coord ) const
{
	if( _coord.x >= x && _coord.x < x + w && _coord.y >= y && _coord.y < y + h )
		return true;
	return false;
}

//_____________________________________________________________________________
inline bool WgRect::Contains( const WgRect& _rect ) const
{
	if( _rect.x >= x && _rect.y >= y && _rect.x + _rect.w <= x + w &&  _rect.y + _rect.h <= y + h )
		return true;
	return false;
}

//_____________________________________________________________________________
inline WgCoord WgRect::Limit( const WgCoord& _coord ) const
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
inline bool WgRect::IntersectsWith( const WgRect& _rect ) const
{
	if(Left() >= _rect.Right()) return false;
	if(Right() <= _rect.Left()) return false;
	if(Top() >= _rect.Bottom()) return false;
	if(Bottom() <= _rect.Top()) return false;

	return true;
}

//_____________________________________________________________________________
inline void WgRect::operator=( const WgRect& r2 )
{
	x = r2.x;
	y = r2.y;
	w = r2.w;
	h = r2.h;
}

//_____________________________________________________________________________
/**
 * @brief Grows the rectangle by the specified border.
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
 * @brief Shrinks the rectangle by the specified border.
 * 
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




//=======================================================================================
#endif	//WG_GEO_DOT_H
