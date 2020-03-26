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

#ifndef WG_MU_DOT_H
#define	WG_MU_DOT_H
#pragma once

#include <cstdint>

class WgBase;

namespace wg
{
	class MU
	{
		friend class Base;
		friend class ::WgBase;

	public:
		//.____ Creation __________________________________________

		MU() : qpix(0) {}
		MU(int point) : qpix(point * MU::qpixPerPoint()) {}
		MU(float point) : qpix(int(point * MU::qpixPerPoint())) {}

		//.____ Misc ______________________________________________

		inline void setPixel(int pixelValue) { qpix = pixelValue << 2; }
		inline void setPoint(int pointValue) { qpix = pointValue * MU::qpixPerPoint(); }

		static inline MU fromPixel(int pixel) { MU p; p.qpix = pixel << 2; return p; }
		static inline MU fromPixel(float pixel) { MU p; p.qpix = (int)(pixel * 4); return p; }
		static inline MU fromPixel(double pixel) { MU p; p.qpix = (int)(pixel * 4); return p; }

		static inline MU fromRaw(int qpixValue) { MU p; p.qpix = qpixValue; return p; }

		inline int roundToPixel() { return (qpix * MU::qpixPerPoint() + 2) >> 2; }
		inline void	pixelAlign() { qpix = (qpix + 2) & 0xFFFFFFFC; }
		inline int qpixAligned() { return (qpix + 2) & 0xFFFFFFFC; }


		inline static int		qpixPerPoint() { return s_qpixPerPoint; }
		inline static float		scale() { return s_scale; }

		//.____ Operators ___________________________________________

		operator int() const { return qpix / MU::qpixPerPoint(); }
		operator float() const { return qpix / (float)MU::qpixPerPoint(); }
		operator double() const { return qpix / (double)MU::qpixPerPoint(); }

		inline bool operator== (const MU& k) const { return (qpix == k.qpix); }
		inline bool operator!= (const MU& k) const { return (qpix != k.qpix); }
		inline bool operator< (const MU& k) const { return (qpix < k.qpix); }
		inline bool operator> (const MU& k) const { return (qpix > k.qpix); }
		inline bool operator<= (const MU& k) const { return (qpix <= k.qpix); }
		inline bool operator>= (const MU& k) const { return (qpix >= k.qpix); }

		inline bool operator== (int k) const { return (qpix == k * MU::qpixPerPoint()); }
		inline bool operator!= (int k) const { return (qpix != k * MU::qpixPerPoint()); }
		inline bool operator< (int k) const { return (qpix < k * MU::qpixPerPoint()); }
		inline bool operator> (int k) const { return (qpix > k * MU::qpixPerPoint()); }
		inline bool operator<= (int k) const { return (qpix <= k * MU::qpixPerPoint()); }
		inline bool operator>= (int k) const { return (qpix >= k * MU::qpixPerPoint()); }

		inline bool operator== (float k) const { return (qpix == int(k * MU::qpixPerPoint())); }
		inline bool operator!= (float k) const { return (qpix != int(k * MU::qpixPerPoint())); }
		inline bool operator< (float k) const { return (qpix < int(k * MU::qpixPerPoint())); }
		inline bool operator> (float k) const { return (qpix > int(k * MU::qpixPerPoint())); }
		inline bool operator<= (float k) const { return (qpix <= int(k * MU::qpixPerPoint())); }
		inline bool operator>= (float k) const { return (qpix >= int(k * MU::qpixPerPoint())); }

		inline bool operator== (double k) const { return (qpix == int(k * MU::qpixPerPoint())); }
		inline bool operator!= (double k) const { return (qpix != int(k * MU::qpixPerPoint())); }
		inline bool operator< (double k) const { return (qpix < int(k * MU::qpixPerPoint())); }
		inline bool operator> (double k) const { return (qpix > int(k * MU::qpixPerPoint())); }
		inline bool operator<= (double k) const { return (qpix <= int(k * MU::qpixPerPoint())); }
		inline bool operator>= (double k) const { return (qpix >= int(k * MU::qpixPerPoint())); }

		inline MU& operator++ () { qpix += MU::qpixPerPoint(); return *this; }
		inline MU& operator-- () { qpix -= MU::qpixPerPoint(); return *this; }

		inline MU& operator+= (const MU& k) { qpix += k.qpix; return *this; }
		inline MU& operator-= (const MU& k) { qpix -= k.qpix; return *this; }
		inline MU& operator*= (const MU& k) { qpix = (qpix * k.qpix) >> 2; return *this; }
		inline MU& operator/= (const MU& k) { qpix = (qpix << 2) / k.qpix; return *this; }

		inline MU& operator+= (int k) { qpix += k * MU::qpixPerPoint(); return *this; }
		inline MU& operator-= (int k) { qpix -= k * MU::qpixPerPoint(); return *this; }
		inline MU& operator*= (int x) { qpix *= x; return *this; }
		inline MU& operator/= (int x) { qpix /= x; return *this; }

		inline MU& operator+= (float k) { qpix += (int)(k * MU::qpixPerPoint()); return *this; }
		inline MU& operator-= (float k) { qpix -= (int)(k * MU::qpixPerPoint()); return *this; }
		inline MU& operator*= (float x) { qpix = int(qpix * x); return *this; }
		inline MU& operator/= (float x) { qpix = int(qpix / x); return *this; }

		inline MU& operator+= (double k) { qpix += (int)(k * MU::qpixPerPoint()); return *this; }
		inline MU& operator-= (double k) { qpix -= (int)(k * MU::qpixPerPoint()); return *this; }
		inline MU& operator*= (double x) { qpix = int(qpix * x); return *this; }
		inline MU& operator/= (double x) { qpix = int(qpix / x); return *this; }


		friend int& operator+= (int& num, const MU& point) { num += point.qpix / MU::qpixPerPoint(); return num; }
		friend int& operator-= (int& num, const MU& point) { num -= point.qpix / MU::qpixPerPoint(); return num; }
		friend int& operator*= (int& num, const MU& point) { num = (num * point.qpix) / MU::qpixPerPoint(); return num; }
		friend int& operator/= (int& num, const MU& point) { num = num * MU::qpixPerPoint() / point.qpix; return num; }

		friend float& operator+= (float& num, const MU& point) { num += float(point.qpix) / MU::qpixPerPoint(); return num; }
		friend float& operator-= (float& num, const MU& point) { num -= float(point.qpix) / MU::qpixPerPoint(); return num; }
		friend float& operator*= (float& num, const MU& point) { num = (num * point.qpix) / MU::qpixPerPoint(); return num; }
		friend float& operator/= (float& num, const MU& point) { num = num * MU::qpixPerPoint() / point.qpix; return num; }

		friend double& operator+= (double& num, const MU& point) { num += double(point.qpix) / MU::qpixPerPoint(); return num; }
		friend double& operator-= (double& num, const MU& point) { num -= double(point.qpix) / MU::qpixPerPoint(); return num; }
		friend double& operator*= (double& num, const MU& point) { num = (num * point.qpix) / MU::qpixPerPoint(); return num; }
		friend double& operator/= (double& num, const MU& point) { num = num * MU::qpixPerPoint() / point.qpix; return num; }



		friend MU operator+ (const MU& first, const MU& second) { MU res; res.qpix = first.qpix + second.qpix; return res; }
		friend MU operator- (const MU& first, const MU& second) { MU res; res.qpix = first.qpix - second.qpix; return res; }
		friend MU operator* (const MU& first, const MU& second) { MU res; res.qpix = (first.qpix * second.qpix) >> 2; return res; }
		friend MU operator/ (const MU& first, const MU& second) { MU res; res.qpix = (first.qpix << 2) / second.qpix; return res; }

		friend MU operator+ (int num, const MU& point) { MU res; res.qpix = num * MU::qpixPerPoint() + point.qpix; return res; }
		friend MU operator- (int num, const MU& point) { MU res; res.qpix = num * MU::qpixPerPoint() - point.qpix; return res; }
		friend MU operator* (int num, const MU& point) { MU res; res.qpix = num * point.qpix; return res; }
		friend MU operator/ (int num, const MU& point) { MU res; res.qpix = (num * MU::qpixPerPoint() << 2) / point.qpix; return res; }

		friend MU operator+ (float num, const MU& point) { MU res; res.qpix = int(num * MU::qpixPerPoint()) + point.qpix; return res; }
		friend MU operator- (float num, const MU& point) { MU res; res.qpix = int(num * MU::qpixPerPoint()) - point.qpix; return res; }
		friend MU operator* (float num, const MU& point) { MU res; res.qpix = int(num * point.qpix); return res; }
		friend MU operator/ (float num, const MU& point) { MU res; res.qpix = int(num * MU::qpixPerPoint() * 4) / point.qpix; return res; }

		friend MU operator+ (double num, const MU& point) { MU res; res.qpix = int(num * MU::qpixPerPoint()) + point.qpix; return res; }
		friend MU operator- (double num, const MU& point) { MU res; res.qpix = int(num * MU::qpixPerPoint()) - point.qpix; return res; }
		friend MU operator* (double num, const MU& point) { MU res; res.qpix = int(num * point.qpix); return res; }
		friend MU operator/ (double num, const MU& point) { MU res; res.qpix = int(num * MU::qpixPerPoint() * 4) / point.qpix; return res; }

		friend MU operator+ (const MU& point, int num) { MU res; res.qpix = point.qpix + num * MU::qpixPerPoint(); return res; }
		friend MU operator- (const MU& point, int num) { MU res; res.qpix = point.qpix - num * MU::qpixPerPoint(); return res; }
		friend MU operator* (const MU& point, int num) { MU res; res.qpix = point.qpix * num; return res; }
		friend MU operator/ (const MU& point, int num) { MU res; res.qpix = point.qpix / num; return res; }

		friend MU operator+ (const MU& point, float num) { MU res; res.qpix = point.qpix + int(num * MU::qpixPerPoint()); return res; }
		friend MU operator- (const MU& point, float num) { MU res; res.qpix = point.qpix - int(num * MU::qpixPerPoint()); return res; }
		friend MU operator* (const MU& point, float num) { MU res; res.qpix = int(point.qpix * num); return res; }
		friend MU operator/ (const MU& point, float num) { MU res; res.qpix = int(point.qpix / num); return res; }

		friend MU operator+ (const MU& point, double num) { MU res; res.qpix = point.qpix + int(num * MU::qpixPerPoint()); return res; }
		friend MU operator- (const MU& point, double num) { MU res; res.qpix = point.qpix - int(num * MU::qpixPerPoint()); return res; }
		friend MU operator* (const MU& point, double num) { MU res; res.qpix = int(point.qpix * num); return res; }
		friend MU operator/ (const MU& point, double num) { MU res; res.qpix = int(point.qpix / num); return res; }


		friend float operator/(MU& first, const MU& second) { return first.qpix / float(second.qpix); }

		//		friend int operator+= (MU& first, int second) { return second + (first.qpix >> 2); }

		//.____ Content _______________________________________________________

		int32_t qpix;

	protected:

		static int		s_qpixPerPoint;
		static float	s_scale;						// Valid values are 1.0, 1.25, 1.5, 1.75, 2, 2.25, 2.5, 3.0, 3.5, 4.0, 4.5, 5.0, 6.0, 7.0, 8.0
	};


} // namespace wg
#endif //WG_MU_DOT_H
