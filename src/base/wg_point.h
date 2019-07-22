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
		//.____ Creation __________________________________________

		Point() : value(0) {}
		Point(int point) : value(point * Base::pixelQuartersPerPoint() ) {}
		Point(float point) : value(int(point * Base::pixelQuartersPerPoint())) {}

		//.____ Operators ___________________________________________

		operator int() const { return value / Base::pixelQuartersPerPoint(); }
		operator float() const { return value / (float)Base::pixelQuartersPerPoint(); }
		operator double() const { return value / (double)Base::pixelQuartersPerPoint(); }

		inline bool operator== (const Point& k) const { return (value == k.value); }
		inline bool operator!= (const Point& k) const { return (value != k.value); }
		inline bool operator< (const Point& k) const { return (value < k.value); }
		inline bool operator> (const Point& k) const { return (value > k.value); }
		inline bool operator<= (const Point& k) const { return (value <= k.value); }
		inline bool operator>= (const Point& k) const { return (value >= k.value); }

		inline bool operator== (int k) const { return (value == k * Base::pixelQuartersPerPoint()); }
		inline bool operator!= (int k) const { return (value != k * Base::pixelQuartersPerPoint()); }
		inline bool operator< (int k) const { return (value < k * Base::pixelQuartersPerPoint()); }
		inline bool operator> (int k) const { return (value > k * Base::pixelQuartersPerPoint()); }
		inline bool operator<= (int k) const { return (value <= k * Base::pixelQuartersPerPoint()); }
		inline bool operator>= (int k) const { return (value >= k * Base::pixelQuartersPerPoint()); }

		inline bool operator== (float k) const { return (value == int(k * Base::pixelQuartersPerPoint())); }
		inline bool operator!= (float k) const { return (value != int(k * Base::pixelQuartersPerPoint())); }
		inline bool operator< (float k) const { return (value < int(k * Base::pixelQuartersPerPoint())); }
		inline bool operator> (float k) const { return (value > int(k * Base::pixelQuartersPerPoint())); }
		inline bool operator<= (float k) const { return (value <= int(k * Base::pixelQuartersPerPoint())); }
		inline bool operator>= (float k) const { return (value >= int(k * Base::pixelQuartersPerPoint())); }

		inline bool operator== (double k) const { return (value == int(k * Base::pixelQuartersPerPoint())); }
		inline bool operator!= (double k) const { return (value != int(k * Base::pixelQuartersPerPoint())); }
		inline bool operator< (double k) const { return (value < int(k * Base::pixelQuartersPerPoint())); }
		inline bool operator> (double k) const { return (value > int(k * Base::pixelQuartersPerPoint())); }
		inline bool operator<= (double k) const { return (value <= int(k * Base::pixelQuartersPerPoint())); }
		inline bool operator>= (double k) const { return (value >= int(k * Base::pixelQuartersPerPoint())); }

		inline Point& operator++ () { value += Base::pixelQuartersPerPoint(); return *this; }
		inline Point& operator-- () { value -= Base::pixelQuartersPerPoint(); return *this; }

		inline Point& operator+= (const Point& k) { value += k.value; return *this; }
		inline Point& operator-= (const Point& k) { value -= k.value; return *this; }
		inline Point& operator*= (const Point& k) { value = (value * k.value) >> 2; return *this; }
		inline Point& operator/= (const Point& k) { value = (value << 2) / k.value; return *this; }

		inline Point& operator+= (int k) { value += k * Base::pixelQuartersPerPoint(); return *this; }
		inline Point& operator-= (int k) { value -= k * Base::pixelQuartersPerPoint(); return *this; }
		inline Point& operator*= (int x) { value *= x; return *this; }
		inline Point& operator/= (int x) { value /= x; return *this; }

		inline Point& operator+= (float k) { value += (int)(k * Base::pixelQuartersPerPoint()); return *this; }
		inline Point& operator-= (float k) { value -= (int)(k * Base::pixelQuartersPerPoint()); return *this; }
		inline Point& operator*= (float x) { value = int(value * x); return *this; }
		inline Point& operator/= (float x) { value = int(value / x); return *this; }

		inline Point& operator+= (double k) { value += (int)(k * Base::pixelQuartersPerPoint()); return *this; }
		inline Point& operator-= (double k) { value -= (int)(k * Base::pixelQuartersPerPoint()); return *this; }
		inline Point& operator*= (double x) { value = int(value * x); return *this; }
		inline Point& operator/= (double x) { value = int(value / x); return *this; }


		friend int operator+= (int num, const Point& point) { return num + point.value / Base::pixelQuartersPerPoint(); }
		friend int operator-= (int num, const Point& point) { return num - point.value / Base::pixelQuartersPerPoint(); }
		friend int operator*= (int num, const Point& point) { return (num * point.value) / Base::pixelQuartersPerPoint(); }
		friend int operator/= (int num, const Point& point) { return num * Base::pixelQuartersPerPoint() / point.value; }

		friend float operator+= (float num, const Point& point) { return num + float(point.value) / Base::pixelQuartersPerPoint(); }
		friend float operator-= (float num, const Point& point) { return num - float(point.value) / Base::pixelQuartersPerPoint(); }
		friend float operator*= (float num, const Point& point) { return (num * point.value) / Base::pixelQuartersPerPoint(); }
		friend float operator/= (float num, const Point& point) { return num * Base::pixelQuartersPerPoint() / point.value; }

		friend double operator+= (double num, const Point& point) { return num + double(point.value) / Base::pixelQuartersPerPoint(); }
		friend double operator-= (double num, const Point& point) { return num - double(point.value) / Base::pixelQuartersPerPoint(); }
		friend double operator*= (double num, const Point& point) { return (num * point.value) / Base::pixelQuartersPerPoint(); }
		friend double operator/= (double num, const Point& point) { return num * Base::pixelQuartersPerPoint() / point.value; }



		friend Point operator+ (const Point& first, const Point& second) { Point res; res.value = first.value + second.value; return res; }
		friend Point operator- (const Point& first, const Point& second) { Point res; res.value = first.value - second.value; return res; }
		friend Point operator* (const Point& first, const Point& second) { Point res; res.value = (first.value * second.value) >> 2; return res; }
		friend Point operator/ (const Point& first, const Point& second) { Point res; res.value = (first.value << 2) / second.value; return res; }

		friend Point operator+ (int num, const Point& point) { Point res; res.value = num * Base::pixelQuartersPerPoint() + point.value; return res; }
		friend Point operator- (int num, const Point& point) { Point res; res.value = num * Base::pixelQuartersPerPoint() - point.value; return res; }
		friend Point operator* (int num, const Point& point) { Point res; res.value = num * point.value; return res; }
		friend Point operator/ (int num, const Point& point) { Point res; res.value = (num * Base::pixelQuartersPerPoint() << 2) / point.value; return res; }

		friend Point operator+ (float num, const Point& point) { Point res; res.value = int(num * Base::pixelQuartersPerPoint()) + point.value; return res; }
		friend Point operator- (float num, const Point& point) { Point res; res.value = int(num * Base::pixelQuartersPerPoint()) - point.value; return res; }
		friend Point operator* (float num, const Point& point) { Point res; res.value = int(num * point.value); return res; }
		friend Point operator/ (float num, const Point& point) { Point res; res.value = int(num * Base::pixelQuartersPerPoint() * 4) / point.value; return res; }

		friend Point operator+ (double num, const Point& point) { Point res; res.value = int(num * Base::pixelQuartersPerPoint()) + point.value; return res; }
		friend Point operator- (double num, const Point& point) { Point res; res.value = int(num * Base::pixelQuartersPerPoint()) - point.value; return res; }
		friend Point operator* (double num, const Point& point) { Point res; res.value = int(num * point.value); return res; }
		friend Point operator/ (double num, const Point& point) { Point res; res.value = int(num * Base::pixelQuartersPerPoint() * 4) / point.value; return res; }

		friend Point operator+ (const Point& point, int num) { Point res; res.value = point.value + num * Base::pixelQuartersPerPoint(); return res; }
		friend Point operator- (const Point& point, int num) { Point res; res.value = point.value - num * Base::pixelQuartersPerPoint(); return res; }
		friend Point operator* (const Point& point, int num) { Point res; res.value = point.value * num; return res; }
		friend Point operator/ (const Point& point, int num) { Point res; res.value = point.value / num; return res; }

		friend Point operator+ (const Point& point, float num) { Point res; res.value = point.value + int(num * Base::pixelQuartersPerPoint()); return res; }
		friend Point operator- (const Point& point, float num) { Point res; res.value = point.value - int(num * Base::pixelQuartersPerPoint()); return res; }
		friend Point operator* (const Point& point, float num) { Point res; res.value = int(point.value * num); return res; }
		friend Point operator/ (const Point& point, float num) { Point res; res.value = int(point.value / num); return res; }

		friend Point operator+ (const Point& point, double num) { Point res; res.value = point.value + int(num * Base::pixelQuartersPerPoint()); return res; }
		friend Point operator- (const Point& point, double num) { Point res; res.value = point.value - int(num * Base::pixelQuartersPerPoint()); return res; }
		friend Point operator* (const Point& point, double num) { Point res; res.value = int(point.value * num); return res; }
		friend Point operator/ (const Point& point, double num) { Point res; res.value = int(point.value / num); return res; }


		friend float operator/(Point& first, const Point& second) { return first.value / float(second.value); }


		//		friend int operator+= (Point& first, int second) { return second + (first.value >> 2); }

		//.____ Misc ______________________________________________

		inline int rawValue() const { return value; }

		inline void setRaw(int rawValue) { value = rawValue; }
		inline void setPixel(int pixelValue) { value = pixelValue << 2; }
		inline void setPoint(int pointValue) { value = pointValue * Base::pixelQuartersPerPoint(); }

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
