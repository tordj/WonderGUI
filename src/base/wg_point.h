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

#ifndef WG_POINT_DOT_H
#define	WG_POINT_DOT_H
#pragma once

#include <wg_base.h>
//#include <wg_context.h>

namespace wg
{
	class Point
	{
		friend Point fromPixel(int pixel);
		friend Point fromPixel(float pixel);

		friend int toPixel(Point p);
		friend int roundToPixel(Point);

	public:

		Point() : value(0) {}
		Point(int point) : value(point * Base::pixelQuartersPerPoint() ) {}
		Point(float point) : value(int(point * Base::pixelQuartersPerPoint())) {}

		operator int() const { return value / Base::pixelQuartersPerPoint(); }
		operator float() const { return value / (float)Base::pixelQuartersPerPoint(); }

		inline bool operator==(const Point& k) const { return (value == k.value); }
		inline bool operator!=(const Point& k) const { return (value != k.value); }

		inline void operator++() { value += Base::pixelQuartersPerPoint(); }
		inline void operator--() { value -= Base::pixelQuartersPerPoint(); }

		inline void operator+=(const Point& k) { value += k.value; }
		inline void operator-=(const Point& k) { value -= k.value; }
		inline Point operator+(const Point& k) const { Point res; res.value = value + k.value; return res; }
		inline Point operator-(const Point& k) const { Point res; res.value = value - k.value; return res; }

		inline void operator+=(int k) { value += k*Base::pixelQuartersPerPoint(); }
		inline void operator-=(int k) { value -= k * Base::pixelQuartersPerPoint(); }
		inline Point operator+(int k) const { return static_cast<Point>(value + k * Base::pixelQuartersPerPoint()); }
		inline Point operator-(int k) const { return static_cast<Point>(value - k * Base::pixelQuartersPerPoint()); }

		inline void operator+=(float k) { value += (int) (k * Base::pixelQuartersPerPoint()); }
		inline void operator-=(float k) { value -= (int)(k * Base::pixelQuartersPerPoint()); }
		inline Point operator+(float k) const { Point res; res.value = (value + (int)(k * Base::pixelQuartersPerPoint())); return res; }
		inline Point operator-(float k) const { Point res; res.value = (value - (int)(k * Base::pixelQuartersPerPoint())); return res; }

		inline void operator+=(double k) { value += (int)(k * Base::pixelQuartersPerPoint()); }
		inline void operator-=(double k) { value -= (int)(k * Base::pixelQuartersPerPoint()); }
		inline Point operator+(double k) const { Point res; res.value = (value + (int)(k * Base::pixelQuartersPerPoint())); return res; }
		inline Point operator-(double k) const { Point res; res.value = (value - (int)(k * Base::pixelQuartersPerPoint())); return res; }

		inline void operator*=(int x) { value *= x; }
		inline void operator/=(int x) { value /= x; }
		inline Point operator*(int x) const { Point res; res.value = value * x; return res; }
		inline Point operator/(int x) const { Point res; res.value = value / x; return res; }

		inline void operator*=(float x) { value = int(value * x); }
		inline void operator/=(float x) { value = int(value / x); }
		inline Point operator*(float x) const { Point res; res.value = int(value * x); return res; }
		inline Point operator/(float x) const { Point res; res.value = int(value / x); return res; }

		inline void operator*=(double x) { value = int(value * x); }
		inline void operator/=(double x) { value = int(value / x); }
		inline Point operator*(double x) const { Point res; res.value = int(value * x); return res; }
		inline Point operator/(double x) const { Point res; res.value = int(value / x); return res; }

		inline float operator/(const Point& k) const { return value/(float) k.value; }

		inline void	pixelAlign() { value = (value + 2) & 0xFFFFFFFC; }

	private:

		union
		{
			struct
			{
				uint32_t quarters : 2;
				int32_t pixel : 30;
			};
			int32_t value;
		};


	};

	inline Point fromPixel(int pixel)
	{
		Point p;
		p.value = pixel << 2;
		return p;
	}

	inline Point fromPixel(float pixel)
	{
		Point p;
		p.value = (int)(pixel*4);
		return p;
	}


	inline int toPixel(Point p)
	{
		return p.value >> 2;
	}

	inline int toPixel(int point) 
	{ 
		return (point * Base::pixelQuartersPerPoint()) >> 2;
	}

	inline float toPixel(float point) 
	{ 
		return (point * Base::scale());
	}


	inline int roundToPixel(Point p)
	{
		return (p.value + 2) >> 2;
	}

	inline int roundToPixel(int point)
	{
		return (point * Base::pixelQuartersPerPoint()+2) >> 2;
	}

	inline int roundToPixel(float point)
	{
		return (int)(point * Base::scale());
	}


} // namespace wg
#endif //WG_POINT_DOT_H
