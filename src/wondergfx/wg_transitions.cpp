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


	//____ typeInfo() ____________________________________________________________

	const TypeInfo& Transition::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ _normalize() _______________________________________________________

	int	Transition::_normalize(TransitionCurve type, int duration, int timestamp)
	{
		int normalizedTimestamp = (timestamp << 16) / duration;

		switch (type)
		{
		case TransitionCurve::Linear:
			// Do nothing;
			break;

		case TransitionCurve::EaseIn:
			//TODO: Implement!!!
			break;

		case TransitionCurve::EaseOut:
			//TODO: Implement!!!
			break;

		case TransitionCurve::EaseInOut:
			//TODO: Implement!!!
			break;

		}
		return normalizedTimestamp;
	}

	//____ ColorTransition::create() __________________________________________

	ColorTransition_p ColorTransition::create(const Blueprint& blueprint)
	{
		return ColorTransition_p(new ColorTransition(blueprint));
	}

	//____ ColorTransition::constructor ________________________________________

	ColorTransition::ColorTransition(const Blueprint& bp) :

		m_duration(bp.duration),
		m_from(bp.from),
		m_midPoint(bp.midPoint),
		m_to(bp.to)
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

	HiColor	ColorTransition::snapshot(int timestamp)
	{
		limit(timestamp, 0, m_duration);

		if (m_midPointBegin > 0)
		{
			if (timestamp < m_midPointBegin)
			{
				int progress = _normalize(m_curve, m_midPointBegin, timestamp);
				return HiColor::mix(m_from, m_midPoint, progress >> 4);
			}
			else if (timestamp >= m_midPointEnd)
			{
				int progress = _normalize(m_curve2, m_duration - m_midPointEnd, timestamp - m_midPointEnd);
				return HiColor::mix(m_midPoint, m_to, progress >> 4);
			}
			else
				return m_midPoint;
		}
		else
		{
			int progress = _normalize(m_curve, m_duration, timestamp);
			return HiColor::mix(m_from, m_to, progress >> 4);
		}
	}


}