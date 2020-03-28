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
	class MU
	{
		friend class Base;

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
		static inline MU fromQpix(int qpixValue) { MU p; p.qpix = qpixValue; return p; }

		inline int roundToPixel() { return (qpix * MU::qpixPerPoint() + 2) >> 2; }
		inline void	align() { qpix = (qpix + 2) & 0xFFFFFFFC; }
		inline int qpixAligned() { return (qpix + 2) & 0xFFFFFFFC; }


		inline static int		qpixPerPoint() { return s_qpixPerPoint; }
		inline static float		scale() { return s_scale; }

		//.____ Operators ___________________________________________

		operator int() const { return qpix / MU::qpixPerPoint(); }
		operator float() const { return qpix / (float)MU::qpixPerPoint(); }
		operator double() const { return qpix / (double)MU::qpixPerPoint(); }

		inline MU operator-() { MU res; res.qpix = -qpix; return res; }

		inline bool operator== (const MU& k) const { return (qpix == k.qpix); }
		inline bool operator!= (const MU& k) const { return (qpix != k.qpix); }
		inline bool operator< (const MU& k) const { return (qpix < k.qpix); }
		inline bool operator> (const MU& k) const { return (qpix > k.qpix); }
		inline bool operator<= (const MU& k) const { return (qpix <= k.qpix); }
		inline bool operator>= (const MU& k) const { return (qpix >= k.qpix); }

		inline MU& operator++ () { qpix += MU::qpixPerPoint(); return *this; }
		inline MU& operator-- () { qpix -= MU::qpixPerPoint(); return *this; }

		inline MU& operator+= (const MU& k) { qpix += k.qpix; return *this; }
		inline MU& operator-= (const MU& k) { qpix -= k.qpix; return *this; }
		inline MU& operator*= (const MU& k) { qpix = (qpix * k.qpix) >> 2; return *this; }
		inline MU& operator/= (const MU& k) { qpix = (qpix << 2) / k.qpix; return *this; }


		template<typename Type>
		inline bool operator== (Type k) const { return (qpix == k * MU::qpixPerPoint()); }

		template<typename Type>
		inline bool operator!= (Type k) const { return (qpix != k * MU::qpixPerPoint()); }

		template<typename Type>
		inline bool operator< (Type k) const { return (qpix < k * MU::qpixPerPoint()); }

		template<typename Type>
		inline bool operator> (Type k) const { return (qpix > k * MU::qpixPerPoint()); }

		template<typename Type>
		inline bool operator<= (Type k) const { return (qpix <= k * MU::qpixPerPoint()); }

		template<typename Type>
		inline bool operator>= (Type k) const { return (qpix >= k * MU::qpixPerPoint()); }

		template<typename Type>
		inline MU& operator+= (Type k) { qpix += k * MU::qpixPerPoint(); return *this; }

		template<typename Type>
		inline MU& operator-= (Type k) { qpix -= k * MU::qpixPerPoint(); return *this; }

		template<typename Type>
		inline MU& operator*= (Type x) { qpix *= x; return *this; }

		template<typename Type>
		inline MU& operator/= (Type x) { qpix /= x; return *this; }


		//		friend int operator+= (MU& first, int second) { return second + (first.qpix >> 2); }

		//.____ Content _______________________________________________________

		int32_t qpix;

	protected:

		static int		s_qpixPerPoint;
		static float	s_scale;						// Valid values are 1.0, 1.25, 1.5, 1.75, 2, 2.25, 2.5, 3.0, 3.5, 4.0, 4.5, 5.0, 6.0, 7.0, 8.0
	};

	MU abs(MU in);

	inline MU operator+ (const MU& first, const MU& second) { MU res; res.qpix = first.qpix + second.qpix; return res; }
	inline MU operator- (const MU& first, const MU& second) { MU res; res.qpix = first.qpix - second.qpix; return res; }
	inline MU operator* (const MU& first, const MU& second) { MU res; res.qpix = (first.qpix * second.qpix) >> 2; return res; }
	inline MU operator/ (const MU& first, const MU& second) { MU res; res.qpix = (first.qpix << 2) / second.qpix; return res; }

	inline float operator/(MU& first, const MU& second) { return first.qpix / float(second.qpix); }

	template<typename T>
	T& operator+= (T& num, const MU& point) { num += T(point.qpix) / MU::qpixPerPoint(); return num; }

	template<typename T>
	T& operator-= (T& num, const MU& point) { num -= T(point.qpix) / MU::qpixPerPoint(); return num; }

	template<typename T>
	T& operator*= (T& num, const MU& point) { num = (num * point.qpix) / MU::qpixPerPoint(); return num; }

	template<typename T>
	T& operator/= (float& num, const MU& point) { num = num * MU::qpixPerPoint() / point.qpix; return num; }

	template<typename T>
	MU operator+ (T num, const MU& point) { MU res; res.qpix = num * MU::qpixPerPoint() + point.qpix; return res; }

	template<typename T>
	MU operator- (T num, const MU& point) { MU res; res.qpix = num * MU::qpixPerPoint() - point.qpix; return res; }

	template<typename T>
	MU operator* (T num, const MU& point) { MU res; res.qpix = num * point.qpix; return res; }

	template<typename T>
	MU operator/ (T num, const MU& point) { MU res; res.qpix = (num * MU::qpixPerPoint() << 2) / point.qpix; return res; }

	template<typename T>
	MU operator+ (const MU& point, T num) { MU res; res.qpix = point.qpix + num * MU::qpixPerPoint(); return res; }

	template<typename T>
	MU operator- (const MU& point, T num) { MU res; res.qpix = point.qpix - num * MU::qpixPerPoint(); return res; }

	template<typename T>
	MU operator* (const MU& point, T num) { MU res; res.qpix = point.qpix * num; return res; }

	template<typename T>
	MU operator/ (const MU& point, T num) { MU res; res.qpix = point.qpix / num; return res; }

} // namespace wg
#endif //WG_QPIX_DOT_H
