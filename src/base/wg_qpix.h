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

	public:
		//.____ Creation __________________________________________

		QPix() : raw(0) {}
		QPix(int point) : raw(point * Base::pixelQuartersPerPoint()) {}
		QPix(float point) : raw(int(point * Base::pixelQuartersPerPoint())) {}

		//.____ Operators ___________________________________________

		operator int() const { return raw / Base::pixelQuartersPerPoint(); }
		operator float() const { return raw / (float)Base::pixelQuartersPerPoint(); }
		operator double() const { return raw / (double)Base::pixelQuartersPerPoint(); }

		inline bool operator== (const QPix& k) const { return (raw == k.raw); }
		inline bool operator!= (const QPix& k) const { return (raw != k.raw); }
		inline bool operator< (const QPix& k) const { return (raw < k.raw); }
		inline bool operator> (const QPix& k) const { return (raw > k.raw); }
		inline bool operator<= (const QPix& k) const { return (raw <= k.raw); }
		inline bool operator>= (const QPix& k) const { return (raw >= k.raw); }

		inline bool operator== (int k) const { return (raw == k * Base::pixelQuartersPerPoint()); }
		inline bool operator!= (int k) const { return (raw != k * Base::pixelQuartersPerPoint()); }
		inline bool operator< (int k) const { return (raw < k * Base::pixelQuartersPerPoint()); }
		inline bool operator> (int k) const { return (raw > k * Base::pixelQuartersPerPoint()); }
		inline bool operator<= (int k) const { return (raw <= k * Base::pixelQuartersPerPoint()); }
		inline bool operator>= (int k) const { return (raw >= k * Base::pixelQuartersPerPoint()); }

		inline bool operator== (float k) const { return (raw == int(k * Base::pixelQuartersPerPoint())); }
		inline bool operator!= (float k) const { return (raw != int(k * Base::pixelQuartersPerPoint())); }
		inline bool operator< (float k) const { return (raw < int(k * Base::pixelQuartersPerPoint())); }
		inline bool operator> (float k) const { return (raw > int(k * Base::pixelQuartersPerPoint())); }
		inline bool operator<= (float k) const { return (raw <= int(k * Base::pixelQuartersPerPoint())); }
		inline bool operator>= (float k) const { return (raw >= int(k * Base::pixelQuartersPerPoint())); }

		inline bool operator== (double k) const { return (raw == int(k * Base::pixelQuartersPerPoint())); }
		inline bool operator!= (double k) const { return (raw != int(k * Base::pixelQuartersPerPoint())); }
		inline bool operator< (double k) const { return (raw < int(k * Base::pixelQuartersPerPoint())); }
		inline bool operator> (double k) const { return (raw > int(k * Base::pixelQuartersPerPoint())); }
		inline bool operator<= (double k) const { return (raw <= int(k * Base::pixelQuartersPerPoint())); }
		inline bool operator>= (double k) const { return (raw >= int(k * Base::pixelQuartersPerPoint())); }

		inline QPix& operator++ () { raw += Base::pixelQuartersPerPoint(); return *this; }
		inline QPix& operator-- () { raw -= Base::pixelQuartersPerPoint(); return *this; }

		inline QPix& operator+= (const QPix& k) { raw += k.raw; return *this; }
		inline QPix& operator-= (const QPix& k) { raw -= k.raw; return *this; }
		inline QPix& operator*= (const QPix& k) { raw = (raw * k.raw) >> 2; return *this; }
		inline QPix& operator/= (const QPix& k) { raw = (raw << 2) / k.raw; return *this; }

		inline QPix& operator+= (int k) { raw += k * Base::pixelQuartersPerPoint(); return *this; }
		inline QPix& operator-= (int k) { raw -= k * Base::pixelQuartersPerPoint(); return *this; }
		inline QPix& operator*= (int x) { raw *= x; return *this; }
		inline QPix& operator/= (int x) { raw /= x; return *this; }

		inline QPix& operator+= (float k) { raw += (int)(k * Base::pixelQuartersPerPoint()); return *this; }
		inline QPix& operator-= (float k) { raw -= (int)(k * Base::pixelQuartersPerPoint()); return *this; }
		inline QPix& operator*= (float x) { raw = int(raw * x); return *this; }
		inline QPix& operator/= (float x) { raw = int(raw / x); return *this; }

		inline QPix& operator+= (double k) { raw += (int)(k * Base::pixelQuartersPerPoint()); return *this; }
		inline QPix& operator-= (double k) { raw -= (int)(k * Base::pixelQuartersPerPoint()); return *this; }
		inline QPix& operator*= (double x) { raw = int(raw * x); return *this; }
		inline QPix& operator/= (double x) { raw = int(raw / x); return *this; }


		friend int& operator+= (int& num, const QPix& point) { num += point.raw / Base::pixelQuartersPerPoint(); return num; }
		friend int& operator-= (int& num, const QPix& point) { num -= point.raw / Base::pixelQuartersPerPoint(); return num;  }
		friend int& operator*= (int& num, const QPix& point) { num = (num * point.raw) / Base::pixelQuartersPerPoint(); return num; }
		friend int& operator/= (int& num, const QPix& point) { num = num * Base::pixelQuartersPerPoint() / point.raw; return num; }

		friend float& operator+= (float& num, const QPix& point) { num += float(point.raw) / Base::pixelQuartersPerPoint(); return num; }
		friend float& operator-= (float& num, const QPix& point) { num -= float(point.raw) / Base::pixelQuartersPerPoint(); return num; }
		friend float& operator*= (float& num, const QPix& point) { num = (num * point.raw) / Base::pixelQuartersPerPoint(); return num; }
		friend float& operator/= (float& num, const QPix& point) { num = num * Base::pixelQuartersPerPoint() / point.raw; return num; }

		friend double& operator+= (double& num, const QPix& point) { num += double(point.raw) / Base::pixelQuartersPerPoint(); return num; }
		friend double& operator-= (double& num, const QPix& point) { num -= double(point.raw) / Base::pixelQuartersPerPoint(); return num; }
		friend double& operator*= (double& num, const QPix& point) { num = (num * point.raw) / Base::pixelQuartersPerPoint(); return num; }
		friend double& operator/= (double& num, const QPix& point) { num = num * Base::pixelQuartersPerPoint() / point.raw; return num; }



		friend QPix operator+ (const QPix& first, const QPix& second) { QPix res; res.raw = first.raw + second.raw; return res; }
		friend QPix operator- (const QPix& first, const QPix& second) { QPix res; res.raw = first.raw - second.raw; return res; }
		friend QPix operator* (const QPix& first, const QPix& second) { QPix res; res.raw = (first.raw * second.raw) >> 2; return res; }
		friend QPix operator/ (const QPix& first, const QPix& second) { QPix res; res.raw = (first.raw << 2) / second.raw; return res; }

		friend QPix operator+ (int num, const QPix& point) { QPix res; res.raw = num * Base::pixelQuartersPerPoint() + point.raw; return res; }
		friend QPix operator- (int num, const QPix& point) { QPix res; res.raw = num * Base::pixelQuartersPerPoint() - point.raw; return res; }
		friend QPix operator* (int num, const QPix& point) { QPix res; res.raw = num * point.raw; return res; }
		friend QPix operator/ (int num, const QPix& point) { QPix res; res.raw = (num * Base::pixelQuartersPerPoint() << 2) / point.raw; return res; }

		friend QPix operator+ (float num, const QPix& point) { QPix res; res.raw = int(num * Base::pixelQuartersPerPoint()) + point.raw; return res; }
		friend QPix operator- (float num, const QPix& point) { QPix res; res.raw = int(num * Base::pixelQuartersPerPoint()) - point.raw; return res; }
		friend QPix operator* (float num, const QPix& point) { QPix res; res.raw = int(num * point.raw); return res; }
		friend QPix operator/ (float num, const QPix& point) { QPix res; res.raw = int(num * Base::pixelQuartersPerPoint() * 4) / point.raw; return res; }

		friend QPix operator+ (double num, const QPix& point) { QPix res; res.raw = int(num * Base::pixelQuartersPerPoint()) + point.raw; return res; }
		friend QPix operator- (double num, const QPix& point) { QPix res; res.raw = int(num * Base::pixelQuartersPerPoint()) - point.raw; return res; }
		friend QPix operator* (double num, const QPix& point) { QPix res; res.raw = int(num * point.raw); return res; }
		friend QPix operator/ (double num, const QPix& point) { QPix res; res.raw = int(num * Base::pixelQuartersPerPoint() * 4) / point.raw; return res; }

		friend QPix operator+ (const QPix& point, int num) { QPix res; res.raw = point.raw + num * Base::pixelQuartersPerPoint(); return res; }
		friend QPix operator- (const QPix& point, int num) { QPix res; res.raw = point.raw - num * Base::pixelQuartersPerPoint(); return res; }
		friend QPix operator* (const QPix& point, int num) { QPix res; res.raw = point.raw * num; return res; }
		friend QPix operator/ (const QPix& point, int num) { QPix res; res.raw = point.raw / num; return res; }

		friend QPix operator+ (const QPix& point, float num) { QPix res; res.raw = point.raw + int(num * Base::pixelQuartersPerPoint()); return res; }
		friend QPix operator- (const QPix& point, float num) { QPix res; res.raw = point.raw - int(num * Base::pixelQuartersPerPoint()); return res; }
		friend QPix operator* (const QPix& point, float num) { QPix res; res.raw = int(point.raw * num); return res; }
		friend QPix operator/ (const QPix& point, float num) { QPix res; res.raw = int(point.raw / num); return res; }

		friend QPix operator+ (const QPix& point, double num) { QPix res; res.raw = point.raw + int(num * Base::pixelQuartersPerPoint()); return res; }
		friend QPix operator- (const QPix& point, double num) { QPix res; res.raw = point.raw - int(num * Base::pixelQuartersPerPoint()); return res; }
		friend QPix operator* (const QPix& point, double num) { QPix res; res.raw = int(point.raw * num); return res; }
		friend QPix operator/ (const QPix& point, double num) { QPix res; res.raw = int(point.raw / num); return res; }


		friend float operator/(QPix& first, const QPix& second) { return first.raw / float(second.raw); }

		//		friend int operator+= (QPix& first, int second) { return second + (first.raw >> 2); }

		//.____ Misc ______________________________________________

		inline void setPixel(int pixelValue) { raw = pixelValue << 2; }
		inline void setPoint(int pointValue) { raw = pointValue * Base::pixelQuartersPerPoint(); }

		static inline QPix fromPixel(int pixel) { QPix p; p.raw = pixel << 2; return p; }
		static inline QPix fromPixel(float pixel) { QPix p; p.raw = (int)(pixel * 4); return p; }
		static inline QPix fromPixel(double pixel) { QPix p; p.raw = (int)(pixel * 4); return p; }

		static inline QPix fromRaw(int rawValue) { QPix p; p.raw = rawValue; return p; }

		inline int roundToPixel() { return (raw * Base::pixelQuartersPerPoint() + 2) >> 2; }
		inline void	pixelAlign() { raw = (raw + 2) & 0xFFFFFFFC; }
		inline int rawAligned() { return (raw + 2) & 0xFFFFFFFC; }

		union
		{
			struct
			{
				uint32_t quarters : 2;
				int32_t pixel : 30;
			};
			int32_t raw;
		};


	};



	inline int toPixel(QPix p)
	{
		return p.raw >> 2;
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
		return (p.raw + 2) >> 2;
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
