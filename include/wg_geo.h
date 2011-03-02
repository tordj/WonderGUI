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


//____ Class: WgCord32 ________________________________________________________

class	WgCord32
{
public:
	WgCord32() : x(0), y(0) {}
	WgCord32( int x, int y ) : x(x), y(y) {}
	WgCord32( const WgRect& rect );

	inline WgCord32 operator=(const WgCord32& k)	{ x = k.x; y = k.y; return *this; }
	inline WgCord32 operator=(const WgRect& r);

	inline bool operator==(const WgCord32& k) const	{ if( x == k.x && y == k.y ) return true; return false; }
	inline bool operator!=(const WgCord32& k) const	{ if( x != k.x || y != k.y ) return true; return false; }

	inline void operator+=(const WgCord32& k)			{ x += k.x; y += k.y; }
	inline void operator-=(const WgCord32& k)			{ x -= k.x; y -= k.y; }
	inline WgCord32 operator+(const WgCord32& k) const	{ WgCord32 res; res.x = x + k.x; res.y = y + k.y; return res; }
	inline WgCord32 operator-(const WgCord32& k) const	{ WgCord32 res; res.x = x - k.x; res.y = y - k.y; return res; }

	inline void operator*=(double v)				{ x = (int) (x*v); y = (int) (y*v); }
	inline void operator/=(double v)				{ x = (int) (x/v); y = (int) (y/v); }
	inline WgCord32 operator*(double v) const	{ WgCord32 res; res.x = (int) (x*v); res.y = (int) (y*v); return res; }
	inline WgCord32 operator/(double v) const	{ WgCord32 res; res.x = (int) (x/v); res.y = (int) (y/v); return res; }

	inline void operator*=(float v)				{ x = (int) (x*v); y = (int) (y*v); }
	inline void operator/=(float v)				{ x = (int) (x/v); y = (int) (y/v); }
	inline WgCord32 operator*(float v) const	{ WgCord32 res; res.x = (int) (x*v); res.y = (int) (y*v); return res; }
	inline WgCord32 operator/(float v) const	{ WgCord32 res; res.x = (int) (x/v); res.y = (int) (y/v); return res; }

	inline void operator*=(int v)				{ x *= v; y *= v; }
	inline void operator/=(int v)				{ x /= v; y /= v; }
	inline WgCord32 operator*(int v) const	{ WgCord32 res; res.x = x * v; res.y = y * v; return res; }
	inline WgCord32 operator/(int v) const	{ WgCord32 res; res.x = x / v; res.y = y / v; return res; }


	int	x, y;
};

typedef WgCord32	WgCord;

//____ Class: WgCord16 ________________________________________________________

class	WgCord16
{
public:
	WgCord16() : x(0), y(0) {}
	WgCord16( Sint16 _x, Sint16 _y ) {x=_x;y=_y;};

	inline WgCord16 operator=(const WgCord16& k)	{ x = k.x; y = k.y; return *this; }

	inline bool operator==(const WgCord16& k) const	{ if( x == k.x && y == k.y ) return true; return false; }
	inline bool operator!=(const WgCord16& k) const	{ if( x != k.x || y != k.y ) return true; return false; }

	Sint16	x, y;
};

//____ Class: WgCord8 _________________________________________________________

class	WgCord8
{
public:
	WgCord8() : x(0), y(0) {}
	WgCord8( Sint8 _x, Sint8 _y ) {x=_x;y=_y;};

	inline WgCord8 operator=(const WgCord8& k)	{ x = k.x; y = k.y; return *this; }

	inline bool operator==(const WgCord8& k) const	{ if( x == k.x && y == k.y ) return true; return false; }
	inline bool operator!=(const WgCord8& k) const	{ if( x != k.x || y != k.y ) return true; return false; }

	Sint8		x, y;
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


//____ Class: WgSize ________________________________________________________

class	WgSize
{
public:
	WgSize() : w(0), h(0) {}
	WgSize( int _w, int _h ) {w=_w;h=_h;}
	WgSize( const WgRect& rect );
	WgSize( const WgCord& c1, const WgCord& c2 ) { w = c2.x - c1.x; h = c2.y - c1.y; }
	inline void constrainTo( const WgMinMax2D& minmax );


	inline WgSize operator=(const WgSize& k)	{ w = k.w; h = k.h; return *this; }
	inline WgSize operator=(const WgRect& k);

	inline bool operator==(const WgSize& k) const	{ if( w == k.w && h == k.h ) return true; return false; }
	inline bool operator!=(const WgSize& k) const	{ if( w != k.w || h != k.h ) return true; return false; }

	inline void operator+=(const WgSize& k)				{ w += k.w; h += k.h; }
	inline void operator-=(const WgSize& k)				{ w -= k.w; h -= k.h; }
	inline WgSize operator+(const WgSize& k) const	{ WgSize res; res.w = w + k.w; res.h = h + k.h; return res; }
	inline WgSize operator-(const WgSize& k) const	{ WgSize res; res.w = w - k.w; res.h = h - k.h; return res; }

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
	static inline WgSize max( WgSize sz1, WgSize sz2 ) { return WgSize( sz1.w<sz2.w?sz1.w:sz2.w, sz1.h<sz2.h?sz1.h:sz2.h ); }

	int	w, h;
};



//____ WgBorders ______________________________________________________________

class WgBorders
{
public:
	WgBorders() : left(0), right(0), top(0), bottom(0) {}
	WgBorders( Uint8 _left, Uint8 _right, Uint8 _top, Uint8 _bottom ) : left(_left), right(_right), top(_top), bottom(_bottom) {}
	WgBorders( Uint8 _all ) : left(_all), right(_all), top(_all), bottom(_all) {}

	inline void		set( Uint8 _all ) { left = right = top = bottom = _all; }

	inline WgSize	size() const { return WgSize( ((int)left)+right, ((int)top)+bottom ); }
	inline Uint32	width() const { return ((Uint32)left)+right; }
	inline Uint32	height() const { return ((Uint32)top)+bottom; }

	bool			operator==(const WgBorders& borders) const { return left == borders.left &&
																		right == borders.right &&
																		top == borders.top &&
																		bottom == borders.bottom; }
	bool			operator!=(const WgBorders& borders) const { return !(*this == borders); }


	Uint8	left, right;
	Uint8	top, bottom;
};


//____ WgRect ___________________________________________________________

class WgRect
{
public:
	WgRect() : x(0), y(0), w(0), h(0) {}
	WgRect( int x, int y, int w, int h ) : x(x), y(y), w(w), h(h) {}
	WgRect( const WgRect& r ) : x(r.x), y(r.y), w(r.w), h(r.h) {}
	WgRect( const WgRect& r1, const WgRect& r2 );
	WgRect( const WgCord& p1, const WgCord& p2 );
	WgRect( const WgCord& p, const WgSize& sz ) : x(p.x), y(p.y), w(sz.w), h(sz.h) {}
	WgRect( const WgCord& p, int w, int h ) : x(p.x), y(p.y), w(w), h(h) {}
	WgRect( int x, int y, const WgSize& sz ) : x(x), y(y), w(sz.w), h(sz.h) {}
	WgRect( const WgCord& p ) : x(p.x), y(p.y), w(0), h(0) {}
	WgRect( const WgSize& sz ) : x(0), y(0), w(sz.w), h(sz.h) {}

	inline void operator=( const WgRect& );
	inline void operator=( const WgSize& sz) { x=0; y=0; w=sz.w; h=sz.h; }
	inline void operator=( const WgCord& c) { x=c.x; y=c.y; w=0; h=0; }
	inline bool operator==( const WgRect& rect) { return x == rect.x && y == rect.y && w == rect.w && h == rect.h; }
	inline bool operator!=( const WgRect& rect) { return !(*this == rect); }

	inline void operator+=(const WgSize& k)				{ w += k.w; h += k.h; }
	inline void operator-=(const WgSize& k)				{ w -= k.w; h -= k.h; }
	inline WgRect operator+(const WgSize& k) const	{ WgRect res; res.x = x; res.y = y; res.w = w + k.w; res.h = h + k.h; return res; }
	inline WgRect operator-(const WgSize& k) const	{ WgRect res; res.x = x; res.y = y; res.w = w - k.w; res.h = h - k.h; return res; }



	inline void operator+=(const WgCord& k)				{ x += k.x; y += k.y; }
	inline void operator-=(const WgCord& k)				{ x -= k.x; y -= k.y; }
	inline WgRect operator+(const WgCord& k) const	{ WgRect res; res.x = x + k.x; res.y = y + k.y; res.w = w; res.h = h; return res; }
	inline WgRect operator-(const WgCord& k) const	{ WgRect res; res.x = x - k.x; res.y = y - k.y; res.w = w; res.h = h; return res; }

	inline void operator+=(const WgBorders& k)			{ x -= k.left; y-= k.top; w += k.left + k.right; h += k.top + k.bottom; }
	inline void operator-=(const WgBorders& k)			{ x += k.left; y+= k.top; w -= k.left + k.right; h -= k.top + k.bottom; }
	inline WgRect operator+(const WgBorders& k) const	{ WgRect res; res.x = x - k.left; res.y = y- k.top; res.w = w + k.left + k.right; res.h = h + k.top + k.bottom; return res; }
	inline WgRect operator-(const WgBorders& k) const	{ WgRect res; res.x = x + k.left; res.y = y+ k.top; res.w = w - (k.left + k.right); res.h = h - (k.top + k.bottom); return res; }

	void setPos( const WgCord& p ) { x = p.x; y = p.y; }
	void setSize( const WgSize& sz ) { w = sz.w; h = sz.h; }

	void shrink(const WgBorders &borders);
	void shrink(int left, int top, int right, int bottom);

	void grow(const WgBorders &borders);
	void grow(int left, int top, int right, int bottom);


	bool intersection( const WgRect& r1, const WgRect& r2 );

	void growToContain( int x, int y );
	void growToContain( const WgRect& _rect );
	void growToContain( const WgCord& _coord );

	inline bool contains( int x, int y ) const;
	inline bool contains( const WgRect& _rect ) const;
	inline bool	contains( const WgCord& _coord ) const;

	inline bool intersectsWith( const WgRect& _rect ) const;

	inline int width() const		{ return w; }
	inline int height() const	{ return h; }
	inline WgSize size() const		{ return WgSize(w,h); }
	inline WgCord pos() const		{ return WgCord(x,y); }

	inline int left() const		{ return x; }
	inline int top() const		{ return y; }
	inline int right() const		{ return x + w; }
	inline int bottom() const	{ return y + h; }

	int	x, y, w, h;
};



//_____________________________________________________________________________
inline WgCord32::WgCord32( const WgRect& rect )
{
	x = rect.x;
	y = rect.y;
}

//_____________________________________________________________________________
inline WgCord32 WgCord32::operator=(const WgRect& r)
{
	x = r.x;
	y = r.y;
	return *this;
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
inline void WgSize::constrainTo( const WgMinMax2D& c )
{
	if( w < c.minW )
		w = c.minW;
	if( h < c.minH )
		h = c.minH;

	if( w > c.maxW )
		w = c.maxW;
	if( h > c.maxH )
		h = c.maxH;
}

//_____________________________________________________________________________
inline bool WgRect::contains( int _x, int _y ) const
{
	if( _x >= x && _x < x + w && _y >= y && _y < y + h )
		return true;
	return false;
}

//_____________________________________________________________________________
inline bool WgRect::contains( const WgCord& _coord ) const
{
	if( _coord.x >= x && _coord.x < x + w && _coord.y >= y && _coord.y < y + h )
		return true;
	return false;
}

//_____________________________________________________________________________
inline bool WgRect::contains( const WgRect& _rect ) const
{
	if( _rect.x >= x && _rect.y >= y && _rect.x + _rect.w <= x + w &&  _rect.y + _rect.h <= y + h )
		return true;
	return false;
}

//_____________________________________________________________________________
inline bool WgRect::intersectsWith( const WgRect& _rect ) const
{
	if(left() >= _rect.right()) return false;
	if(right() <= _rect.left()) return false;
	if(top() >= _rect.bottom()) return false;
	if(bottom() <= _rect.top()) return false;

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
inline bool operator==( const WgRect& r1, const WgRect& r2 )
{
	if( r1.x == r2.x && r1.y == r2.y && r1.w == r2.w && r1.h == r2.h )
		return true;
	return false;
}

//_____________________________________________________________________________
inline bool operator!=( const WgRect& r1, const WgRect& r2 )
{
	if( r1.x == r2.x && r1.y == r2.y && r1.w == r2.w && r1.h == r2.h )
		return false;
	return true;
}


//=======================================================================================
#endif	//WG_GEO_DOT_H
