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

#include <cstdint>


namespace wg
{
	class QPix
	{
		friend class Base;

	public:
		//.____ Creation __________________________________________

		QPix() : raw(0) {}
		QPix(int point) : raw(point * QPix::pixelQuartersPerPoint()) {}
		QPix(float point) : raw(int(point * QPix::pixelQuartersPerPoint())) {}

		//.____ Misc ______________________________________________

		inline void setPixel(int pixelValue) { raw = pixelValue << 2; }
		inline void setPoint(int pointValue) { raw = pointValue * QPix::pixelQuartersPerPoint(); }

		static inline QPix fromPixel(int pixel) { QPix p; p.raw = pixel << 2; return p; }
		static inline QPix fromPixel(float pixel) { QPix p; p.raw = (int)(pixel * 4); return p; }
		static inline QPix fromPixel(double pixel) { QPix p; p.raw = (int)(pixel * 4); return p; }

		static inline QPix fromRaw(int rawValue) { QPix p; p.raw = rawValue; return p; }

		inline int roundToPixel() { return (raw * QPix::pixelQuartersPerPoint() + 2) >> 2; }
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

		inline static int		pixelQuartersPerPoint() { return s_pixelQuartersPerPoint; }
		inline static float		scale() { return s_scale; }

		//.____ Operators ___________________________________________

		operator int() const { return raw / QPix::pixelQuartersPerPoint(); }
		operator float() const { return raw / (float)QPix::pixelQuartersPerPoint(); }
		operator double() const { return raw / (double)QPix::pixelQuartersPerPoint(); }

		inline bool operator== (const QPix& k) const { return (raw == k.raw); }
		inline bool operator!= (const QPix& k) const { return (raw != k.raw); }
		inline bool operator< (const QPix& k) const { return (raw < k.raw); }
		inline bool operator> (const QPix& k) const { return (raw > k.raw); }
		inline bool operator<= (const QPix& k) const { return (raw <= k.raw); }
		inline bool operator>= (const QPix& k) const { return (raw >= k.raw); }

		inline bool operator== (int k) const { return (raw == k * QPix::pixelQuartersPerPoint()); }
		inline bool operator!= (int k) const { return (raw != k * QPix::pixelQuartersPerPoint()); }
		inline bool operator< (int k) const { return (raw < k * QPix::pixelQuartersPerPoint()); }
		inline bool operator> (int k) const { return (raw > k * QPix::pixelQuartersPerPoint()); }
		inline bool operator<= (int k) const { return (raw <= k * QPix::pixelQuartersPerPoint()); }
		inline bool operator>= (int k) const { return (raw >= k * QPix::pixelQuartersPerPoint()); }

		inline bool operator== (float k) const { return (raw == int(k * QPix::pixelQuartersPerPoint())); }
		inline bool operator!= (float k) const { return (raw != int(k * QPix::pixelQuartersPerPoint())); }
		inline bool operator< (float k) const { return (raw < int(k * QPix::pixelQuartersPerPoint())); }
		inline bool operator> (float k) const { return (raw > int(k * QPix::pixelQuartersPerPoint())); }
		inline bool operator<= (float k) const { return (raw <= int(k * QPix::pixelQuartersPerPoint())); }
		inline bool operator>= (float k) const { return (raw >= int(k * QPix::pixelQuartersPerPoint())); }

		inline bool operator== (double k) const { return (raw == int(k * QPix::pixelQuartersPerPoint())); }
		inline bool operator!= (double k) const { return (raw != int(k * QPix::pixelQuartersPerPoint())); }
		inline bool operator< (double k) const { return (raw < int(k * QPix::pixelQuartersPerPoint())); }
		inline bool operator> (double k) const { return (raw > int(k * QPix::pixelQuartersPerPoint())); }
		inline bool operator<= (double k) const { return (raw <= int(k * QPix::pixelQuartersPerPoint())); }
		inline bool operator>= (double k) const { return (raw >= int(k * QPix::pixelQuartersPerPoint())); }

		inline QPix& operator++ () { raw += QPix::pixelQuartersPerPoint(); return *this; }
		inline QPix& operator-- () { raw -= QPix::pixelQuartersPerPoint(); return *this; }

		inline QPix& operator+= (const QPix& k) { raw += k.raw; return *this; }
		inline QPix& operator-= (const QPix& k) { raw -= k.raw; return *this; }
		inline QPix& operator*= (const QPix& k) { raw = (raw * k.raw) >> 2; return *this; }
		inline QPix& operator/= (const QPix& k) { raw = (raw << 2) / k.raw; return *this; }

		inline QPix& operator+= (int k) { raw += k * QPix::pixelQuartersPerPoint(); return *this; }
		inline QPix& operator-= (int k) { raw -= k * QPix::pixelQuartersPerPoint(); return *this; }
		inline QPix& operator*= (int x) { raw *= x; return *this; }
		inline QPix& operator/= (int x) { raw /= x; return *this; }

		inline QPix& operator+= (float k) { raw += (int)(k * QPix::pixelQuartersPerPoint()); return *this; }
		inline QPix& operator-= (float k) { raw -= (int)(k * QPix::pixelQuartersPerPoint()); return *this; }
		inline QPix& operator*= (float x) { raw = int(raw * x); return *this; }
		inline QPix& operator/= (float x) { raw = int(raw / x); return *this; }

		inline QPix& operator+= (double k) { raw += (int)(k * QPix::pixelQuartersPerPoint()); return *this; }
		inline QPix& operator-= (double k) { raw -= (int)(k * QPix::pixelQuartersPerPoint()); return *this; }
		inline QPix& operator*= (double x) { raw = int(raw * x); return *this; }
		inline QPix& operator/= (double x) { raw = int(raw / x); return *this; }


		friend int& operator+= (int& num, const QPix& point) { num += point.raw / QPix::pixelQuartersPerPoint(); return num; }
		friend int& operator-= (int& num, const QPix& point) { num -= point.raw / QPix::pixelQuartersPerPoint(); return num; }
		friend int& operator*= (int& num, const QPix& point) { num = (num * point.raw) / QPix::pixelQuartersPerPoint(); return num; }
		friend int& operator/= (int& num, const QPix& point) { num = num * QPix::pixelQuartersPerPoint() / point.raw; return num; }

		friend float& operator+= (float& num, const QPix& point) { num += float(point.raw) / QPix::pixelQuartersPerPoint(); return num; }
		friend float& operator-= (float& num, const QPix& point) { num -= float(point.raw) / QPix::pixelQuartersPerPoint(); return num; }
		friend float& operator*= (float& num, const QPix& point) { num = (num * point.raw) / QPix::pixelQuartersPerPoint(); return num; }
		friend float& operator/= (float& num, const QPix& point) { num = num * QPix::pixelQuartersPerPoint() / point.raw; return num; }

		friend double& operator+= (double& num, const QPix& point) { num += double(point.raw) / QPix::pixelQuartersPerPoint(); return num; }
		friend double& operator-= (double& num, const QPix& point) { num -= double(point.raw) / QPix::pixelQuartersPerPoint(); return num; }
		friend double& operator*= (double& num, const QPix& point) { num = (num * point.raw) / QPix::pixelQuartersPerPoint(); return num; }
		friend double& operator/= (double& num, const QPix& point) { num = num * QPix::pixelQuartersPerPoint() / point.raw; return num; }



		friend QPix operator+ (const QPix& first, const QPix& second) { QPix res; res.raw = first.raw + second.raw; return res; }
		friend QPix operator- (const QPix& first, const QPix& second) { QPix res; res.raw = first.raw - second.raw; return res; }
		friend QPix operator* (const QPix& first, const QPix& second) { QPix res; res.raw = (first.raw * second.raw) >> 2; return res; }
		friend QPix operator/ (const QPix& first, const QPix& second) { QPix res; res.raw = (first.raw << 2) / second.raw; return res; }

		friend QPix operator+ (int num, const QPix& point) { QPix res; res.raw = num * QPix::pixelQuartersPerPoint() + point.raw; return res; }
		friend QPix operator- (int num, const QPix& point) { QPix res; res.raw = num * QPix::pixelQuartersPerPoint() - point.raw; return res; }
		friend QPix operator* (int num, const QPix& point) { QPix res; res.raw = num * point.raw; return res; }
		friend QPix operator/ (int num, const QPix& point) { QPix res; res.raw = (num * QPix::pixelQuartersPerPoint() << 2) / point.raw; return res; }

		friend QPix operator+ (float num, const QPix& point) { QPix res; res.raw = int(num * QPix::pixelQuartersPerPoint()) + point.raw; return res; }
		friend QPix operator- (float num, const QPix& point) { QPix res; res.raw = int(num * QPix::pixelQuartersPerPoint()) - point.raw; return res; }
		friend QPix operator* (float num, const QPix& point) { QPix res; res.raw = int(num * point.raw); return res; }
		friend QPix operator/ (float num, const QPix& point) { QPix res; res.raw = int(num * QPix::pixelQuartersPerPoint() * 4) / point.raw; return res; }

		friend QPix operator+ (double num, const QPix& point) { QPix res; res.raw = int(num * QPix::pixelQuartersPerPoint()) + point.raw; return res; }
		friend QPix operator- (double num, const QPix& point) { QPix res; res.raw = int(num * QPix::pixelQuartersPerPoint()) - point.raw; return res; }
		friend QPix operator* (double num, const QPix& point) { QPix res; res.raw = int(num * point.raw); return res; }
		friend QPix operator/ (double num, const QPix& point) { QPix res; res.raw = int(num * QPix::pixelQuartersPerPoint() * 4) / point.raw; return res; }

		friend QPix operator+ (const QPix& point, int num) { QPix res; res.raw = point.raw + num * QPix::pixelQuartersPerPoint(); return res; }
		friend QPix operator- (const QPix& point, int num) { QPix res; res.raw = point.raw - num * QPix::pixelQuartersPerPoint(); return res; }
		friend QPix operator* (const QPix& point, int num) { QPix res; res.raw = point.raw * num; return res; }
		friend QPix operator/ (const QPix& point, int num) { QPix res; res.raw = point.raw / num; return res; }

		friend QPix operator+ (const QPix& point, float num) { QPix res; res.raw = point.raw + int(num * QPix::pixelQuartersPerPoint()); return res; }
		friend QPix operator- (const QPix& point, float num) { QPix res; res.raw = point.raw - int(num * QPix::pixelQuartersPerPoint()); return res; }
		friend QPix operator* (const QPix& point, float num) { QPix res; res.raw = int(point.raw * num); return res; }
		friend QPix operator/ (const QPix& point, float num) { QPix res; res.raw = int(point.raw / num); return res; }

		friend QPix operator+ (const QPix& point, double num) { QPix res; res.raw = point.raw + int(num * QPix::pixelQuartersPerPoint()); return res; }
		friend QPix operator- (const QPix& point, double num) { QPix res; res.raw = point.raw - int(num * QPix::pixelQuartersPerPoint()); return res; }
		friend QPix operator* (const QPix& point, double num) { QPix res; res.raw = int(point.raw * num); return res; }
		friend QPix operator/ (const QPix& point, double num) { QPix res; res.raw = int(point.raw / num); return res; }


		friend float operator/(QPix& first, const QPix& second) { return first.raw / float(second.raw); }

		//		friend int operator+= (QPix& first, int second) { return second + (first.raw >> 2); }


	protected:

		static int		s_pixelQuartersPerPoint;
		static float	s_scale;						// Valid values are 1.0, 1.25, 1.5, 1.75, 2, 2.25, 2.5, 3.0, 3.5, 4.0, 4.5, 5.0, 6.0, 7.0, 8.0
	};


} // namespace wg
#endif //WG_QPIX_DOT_H
