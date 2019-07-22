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

#ifndef WG_QPIX_DOT_H
#define	WG_QPIX_DOT_H
#pragma once

#include <wg_base.h>
//#include <wg_context.h>

namespace wg
{
	class QPix
	{
		friend QPix fromPixel(int pixel);
		friend QPix fromPixel(float pixel);

		friend int toPixel(QPix p);
		friend int roundToPixel(QPix);

	public:
		//.____ Creation __________________________________________

		QPix() : value(0) {}
		QPix(int point) : value(point * Base::pixelQuartersPerPoint()) {}
		QPix(float point) : value(int(point * Base::pixelQuartersPerPoint())) {}

		//.____ Operators ___________________________________________

		operator int() const { return value / Base::pixelQuartersPerPoint(); }
		operator float() const { return value / (float)Base::pixelQuartersPerPoint(); }
		operator double() const { return value / (double)Base::pixelQuartersPerPoint(); }

		inline bool operator== (const QPix& k) const { return (value == k.value); }
		inline bool operator!= (const QPix& k) const { return (value != k.value); }
		inline bool operator< (const QPix& k) const { return (value < k.value); }
		inline bool operator> (const QPix& k) const { return (value > k.value); }
		inline bool operator<= (const QPix& k) const { return (value <= k.value); }
		inline bool operator>= (const QPix& k) const { return (value >= k.value); }

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

		inline QPix& operator++ () { value += Base::pixelQuartersPerPoint(); return *this; }
		inline QPix& operator-- () { value -= Base::pixelQuartersPerPoint(); return *this; }

		inline QPix& operator+= (const QPix& k) { value += k.value; return *this; }
		inline QPix& operator-= (const QPix& k) { value -= k.value; return *this; }
		inline QPix& operator*= (const QPix& k) { value = (value * k.value) >> 2; return *this; }
		inline QPix& operator/= (const QPix& k) { value = (value << 2) / k.value; return *this; }

		inline QPix& operator+= (int k) { value += k * Base::pixelQuartersPerPoint(); return *this; }
		inline QPix& operator-= (int k) { value -= k * Base::pixelQuartersPerPoint(); return *this; }
		inline QPix& operator*= (int x) { value *= x; return *this; }
		inline QPix& operator/= (int x) { value /= x; return *this; }

		inline QPix& operator+= (float k) { value += (int)(k * Base::pixelQuartersPerPoint()); return *this; }
		inline QPix& operator-= (float k) { value -= (int)(k * Base::pixelQuartersPerPoint()); return *this; }
		inline QPix& operator*= (float x) { value = int(value * x); return *this; }
		inline QPix& operator/= (float x) { value = int(value / x); return *this; }

		inline QPix& operator+= (double k) { value += (int)(k * Base::pixelQuartersPerPoint()); return *this; }
		inline QPix& operator-= (double k) { value -= (int)(k * Base::pixelQuartersPerPoint()); return *this; }
		inline QPix& operator*= (double x) { value = int(value * x); return *this; }
		inline QPix& operator/= (double x) { value = int(value / x); return *this; }


		friend int operator+= (int num, const QPix& point) { return num + point.value / Base::pixelQuartersPerPoint(); }
		friend int operator-= (int num, const QPix& point) { return num - point.value / Base::pixelQuartersPerPoint(); }
		friend int operator*= (int num, const QPix& point) { return (num * point.value) / Base::pixelQuartersPerPoint(); }
		friend int operator/= (int num, const QPix& point) { return num * Base::pixelQuartersPerPoint() / point.value; }

		friend float operator+= (float num, const QPix& point) { return num + float(point.value) / Base::pixelQuartersPerPoint(); }
		friend float operator-= (float num, const QPix& point) { return num - float(point.value) / Base::pixelQuartersPerPoint(); }
		friend float operator*= (float num, const QPix& point) { return (num * point.value) / Base::pixelQuartersPerPoint(); }
		friend float operator/= (float num, const QPix& point) { return num * Base::pixelQuartersPerPoint() / point.value; }

		friend double operator+= (double num, const QPix& point) { return num + double(point.value) / Base::pixelQuartersPerPoint(); }
		friend double operator-= (double num, const QPix& point) { return num - double(point.value) / Base::pixelQuartersPerPoint(); }
		friend double operator*= (double num, const QPix& point) { return (num * point.value) / Base::pixelQuartersPerPoint(); }
		friend double operator/= (double num, const QPix& point) { return num * Base::pixelQuartersPerPoint() / point.value; }



		friend QPix operator+ (const QPix& first, const QPix& second) { QPix res; res.value = first.value + second.value; return res; }
		friend QPix operator- (const QPix& first, const QPix& second) { QPix res; res.value = first.value - second.value; return res; }
		friend QPix operator* (const QPix& first, const QPix& second) { QPix res; res.value = (first.value * second.value) >> 2; return res; }
		friend QPix operator/ (const QPix& first, const QPix& second) { QPix res; res.value = (first.value << 2) / second.value; return res; }

		friend QPix operator+ (int num, const QPix& point) { QPix res; res.value = num * Base::pixelQuartersPerPoint() + point.value; return res; }
		friend QPix operator- (int num, const QPix& point) { QPix res; res.value = num * Base::pixelQuartersPerPoint() - point.value; return res; }
		friend QPix operator* (int num, const QPix& point) { QPix res; res.value = num * point.value; return res; }
		friend QPix operator/ (int num, const QPix& point) { QPix res; res.value = (num * Base::pixelQuartersPerPoint() << 2) / point.value; return res; }

		friend QPix operator+ (float num, const QPix& point) { QPix res; res.value = int(num * Base::pixelQuartersPerPoint()) + point.value; return res; }
		friend QPix operator- (float num, const QPix& point) { QPix res; res.value = int(num * Base::pixelQuartersPerPoint()) - point.value; return res; }
		friend QPix operator* (float num, const QPix& point) { QPix res; res.value = int(num * point.value); return res; }
		friend QPix operator/ (float num, const QPix& point) { QPix res; res.value = int(num * Base::pixelQuartersPerPoint() * 4) / point.value; return res; }

		friend QPix operator+ (double num, const QPix& point) { QPix res; res.value = int(num * Base::pixelQuartersPerPoint()) + point.value; return res; }
		friend QPix operator- (double num, const QPix& point) { QPix res; res.value = int(num * Base::pixelQuartersPerPoint()) - point.value; return res; }
		friend QPix operator* (double num, const QPix& point) { QPix res; res.value = int(num * point.value); return res; }
		friend QPix operator/ (double num, const QPix& point) { QPix res; res.value = int(num * Base::pixelQuartersPerPoint() * 4) / point.value; return res; }

		friend QPix operator+ (const QPix& point, int num) { QPix res; res.value = point.value + num * Base::pixelQuartersPerPoint(); return res; }
		friend QPix operator- (const QPix& point, int num) { QPix res; res.value = point.value - num * Base::pixelQuartersPerPoint(); return res; }
		friend QPix operator* (const QPix& point, int num) { QPix res; res.value = point.value * num; return res; }
		friend QPix operator/ (const QPix& point, int num) { QPix res; res.value = point.value / num; return res; }

		friend QPix operator+ (const QPix& point, float num) { QPix res; res.value = point.value + int(num * Base::pixelQuartersPerPoint()); return res; }
		friend QPix operator- (const QPix& point, float num) { QPix res; res.value = point.value - int(num * Base::pixelQuartersPerPoint()); return res; }
		friend QPix operator* (const QPix& point, float num) { QPix res; res.value = int(point.value * num); return res; }
		friend QPix operator/ (const QPix& point, float num) { QPix res; res.value = int(point.value / num); return res; }

		friend QPix operator+ (const QPix& point, double num) { QPix res; res.value = point.value + int(num * Base::pixelQuartersPerPoint()); return res; }
		friend QPix operator- (const QPix& point, double num) { QPix res; res.value = point.value - int(num * Base::pixelQuartersPerPoint()); return res; }
		friend QPix operator* (const QPix& point, double num) { QPix res; res.value = int(point.value * num); return res; }
		friend QPix operator/ (const QPix& point, double num) { QPix res; res.value = int(point.value / num); return res; }


		friend float operator/(QPix& first, const QPix& second) { return first.value / float(second.value); }


		//		friend int operator+= (QPix& first, int second) { return second + (first.value >> 2); }

		//.____ Misc ______________________________________________

		inline int rawValue() const { return value; }

		inline void setRaw(int rawValue) { value = rawValue; }
		inline void setPixel(int pixelValue) { value = pixelValue << 2; }
		inline void setPoint(int pointValue) { value = pointValue * Base::pixelQuartersPerPoint(); }

		inline void	pixelAlign() { value = (value + 2) & 0xFFFFFFFC; }

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

	inline QPix fromPixel(int pixel)
	{
		QPix p;
		p.value = pixel << 2;
		return p;
	}

	inline QPix fromPixel(float pixel)
	{
		QPix p;
		p.value = (int)(pixel * 4);
		return p;
	}


	inline int toPixel(QPix p)
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


	inline int roundToPixel(QPix p)
	{
		return (p.value + 2) >> 2;
	}

	inline int roundToPixel(int point)
	{
		return (point * Base::pixelQuartersPerPoint() + 2) >> 2;
	}

	inline int roundToPixel(float point)
	{
		return (int)(point * Base::scale());
	}


} // namespace wg
#endif //WG_QPIX_DOT_H
