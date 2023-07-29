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

#include <wg_transitions.h>

namespace wg
{
	const TypeInfo Transition::TYPEINFO = { "Transition", &Object::TYPEINFO };
	const TypeInfo ColorTransition::TYPEINFO = { "ColorTransition", &Transition::TYPEINFO };
	const TypeInfo ArrayTransition::TYPEINFO = { "ArrayTransition", &Transition::TYPEINFO };


	//____ typeInfo() ____________________________________________________________

	const TypeInfo& Transition::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ _normalize() _______________________________________________________

	int	Transition::_normalize(TransitionCurve type, int duration, int timestamp)
	{
		// 8.24 binal format.

		// Methods adapted from Creak's answer on StackOverflow.
		// https://stackoverflow.com/questions/13462001/ease-in-and-ease-out-animation-formula


		int64_t normTS = int((int64_t(timestamp) << 24) / duration);

		switch (type)
		{
			default:
			case TransitionCurve::Linear:
				return (int)normTS;

			case TransitionCurve::EaseIn:
				return int((normTS * normTS) >> 24);

			case TransitionCurve::EaseOut:
				return int((normTS * ((2 << 24) - normTS)) >> 24);

			case TransitionCurve::EaseInOut:
			{
				int64_t halfpoint = 1 << 23;

				if (normTS <= halfpoint)
					return int((2 * (normTS * normTS)) >> 24);

				normTS -= halfpoint;
				return int((2 * normTS * ((1 << 24) - normTS)) >> 24) + halfpoint;
			}

			case TransitionCurve::Bezier:
				return int((((normTS * normTS) >> 24) * ((3 << 24) - (((2 << 24) * normTS)>>24))) >> 24);

			case TransitionCurve::Parametric:
			{
				int64_t sqt = (normTS * normTS) >> 24;

				return (sqt << 24) / (2 * (sqt - normTS) + (1 << 24));
			}
		}

	}

	//____ ColorTransition::create() __________________________________________

	ColorTransition_p ColorTransition::create(const Blueprint& blueprint)
	{
		return ColorTransition_p(new ColorTransition(blueprint));
	}

	//____ ColorTransition::constructor ________________________________________

	ColorTransition::ColorTransition(const Blueprint& bp) :

		m_duration(bp.duration),
		m_midPoint(bp.midPointColor)
	{
		

		if (m_midPoint.isValid() )
		{
			int begin = m_duration / 2 + bp.midPointOffset - bp.midPointDelay/2;
			int end = begin + bp.midPointDelay;
			
			limit(begin, 1, m_duration - 1);
			limit(end, begin, m_duration - 1);

			m_midPointBegin = begin;
			m_midPointEnd = end;

			const static TransitionCurve curves[4] = { TransitionCurve::Linear, TransitionCurve::EaseIn, TransitionCurve::EaseOut, TransitionCurve::EaseInOut };

			int easeOne = 0;

			if (bp.curve == TransitionCurve::EaseIn || bp.curve == TransitionCurve::EaseInOut)
				easeOne = 1;

			if (bp.midPointCurve == TransitionCurve::EaseInOut || bp.midPointCurve == TransitionCurve::EaseIn)
				easeOne += 2;

			m_curve = curves[easeOne];

			int easeTwo = 0;

			if (bp.curve == TransitionCurve::EaseOut || bp.curve == TransitionCurve::EaseInOut)
				easeTwo = 2;

			if (bp.midPointCurve == TransitionCurve::EaseInOut || bp.midPointCurve == TransitionCurve::EaseOut)
				easeTwo += 2;

			m_curve2 = curves[easeTwo];
 		}
		else
		{
			m_curve = bp.curve;
			m_curve2 = TransitionCurve::Linear;
			m_midPointBegin = -1;
			m_midPointEnd = -1;
		}

	}

	//____ typeInfo() ____________________________________________________________

	const TypeInfo& ColorTransition::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ ColorTransition::snapshot() ________________________________________

	HiColor	ColorTransition::snapshot(int timestamp, HiColor startColor, HiColor endColor)
	{
		limit(timestamp, 0, m_duration);

		if (m_midPointBegin > 0)
		{
			if (timestamp < m_midPointBegin)
			{
				int progress = _normalize(m_curve, m_midPointBegin, timestamp);
				return HiColor::mix(startColor, m_midPoint, progress >> (24-12));
			}
			else if (timestamp >= m_midPointEnd)
			{
				int progress = _normalize(m_curve2, m_duration - m_midPointEnd, timestamp - m_midPointEnd);
				return HiColor::mix(m_midPoint, endColor, progress >> (24-12));
			}
			else
				return m_midPoint;
		}
		else
		{
			int progress = _normalize(m_curve, m_duration, timestamp);
			return HiColor::mix(startColor, endColor, progress >> (24-12));
		}
	}



	//____ ArrayTransition::create() __________________________________________

	ArrayTransition_p ArrayTransition::create(int duration, TransitionCurve curve)
	{
		return ArrayTransition_p(new ArrayTransition(duration, curve));
	}

	//____ ArrayTransition::constructor ________________________________________

	ArrayTransition::ArrayTransition(int duration, TransitionCurve curve) :
		m_duration(duration),
		m_curve(curve)
	{
	}

	//____ typeInfo() ____________________________________________________________

	const TypeInfo& ArrayTransition::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ ArrayTransition::snapshot() ________________________________________

	void ArrayTransition::snapshot(int timestamp, int nValues, const int* pStartValues, const int* pEndValues, int* pOutput)
	{
		limit(timestamp, 0, m_duration);

		int64_t progress = _normalize(m_curve, m_duration, timestamp);

		for (int i = 0; i < nValues; i++)
		{
			int start = pStartValues[i];
			int end = pEndValues[i];

			pOutput[i] = start + int(((end - start) * progress) >> 24);
		}
	}

	//____ ArrayTransition::snapshot() ________________________________________

	void ArrayTransition::snapshot(int timestamp, int nValues, const float* pStartValues, const float* pEndValues, float* pOutput)
	{
		limit(timestamp, 0, m_duration);

		float progress = float(_normalize(m_curve, m_duration, timestamp)) / (65536.f*256.f);

		for (int i = 0; i < nValues; i++)
		{
			float start = pStartValues[i];
			float end = pEndValues[i];

			pOutput[i] = start + (end - start) * progress;
		}
	}
}