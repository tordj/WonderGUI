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
#ifndef TRANSITIONS_DOT_H
#define TRANSITIONS_DOT_H
#pragma once

#include <wg_gfxtypes.h>
#include <wg_color.h>
#include <wg_geo.h>
#include <wg_object.h>

namespace wg
{

	//____ Transition _________________________________________________________

	class Transition;
	typedef	StrongPtr<Transition>	Transition_p;
	typedef	WeakPtr<Transition>		Transition_wp;

	class Transition : public Object
	{
	public:

		//.____ Identification _________________________________________________

		const TypeInfo& typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

	protected:
		Transition() {}

		int		_normalize(TransitionCurve curve, int durationMicrosec , int timestampMicrosec);

	};

	//____ ColorTransition ____________________________________________________

	class ColorTransition;
	typedef	StrongPtr<ColorTransition>	ColorTransition_p;
	typedef	WeakPtr<ColorTransition>	ColorTransition_wp;

	class ColorTransition : public Transition
	{
	public:

		//.____ Blueprint _____________________________________________________

		struct Blueprint
		{
			TransitionCurve		curve = TransitionCurve::Linear;
			int					duration = 250*1000;					// Microseconds!
			Finalizer_p			finalizer = nullptr;
			HiColor				midPointColor;
			int					midPointDelay = 0;
			TransitionCurve		midPointCurve = TransitionCurve::Linear;
			int					midPointOffset = 0;
		};

		//.____ Creation ______________________________________________________

		static ColorTransition_p	create(const Blueprint& blueprint);

		//.____ Identification _________________________________________________

		const TypeInfo& typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Misc ___________________________________________________________

		int		duration() const { return m_duration; }

		HiColor	snapshot(int timestampMicroSec, HiColor startColor, HiColor endColor );

	protected:
		ColorTransition( const Blueprint& blueprint );

		TransitionCurve	m_curve;		// Curve for transition before midpoint or for whole curve if no midpoint.
		TransitionCurve	m_curve2;		// Curve for transition after midpoint.
		int				m_duration;		// Microseconds
		int				m_midPointBegin;
		int				m_midPointEnd;

		HiColor			m_midPoint;
	};


	//____ ArrayTransition ____________________________________________________

	class ArrayTransition;
	typedef	StrongPtr<ArrayTransition>	ArrayTransition_p;
	typedef	WeakPtr<ArrayTransition>	ArrayTransition_wp;

	class ArrayTransition : public Transition
	{
	public:

		//.____ Blueprint _____________________________________________________

		struct Blueprint
		{
			TransitionCurve		curve = TransitionCurve::Linear;
			int					duration = 250 * 1000;					// Microseconds!
			Finalizer_p			finalizer = nullptr;
		};


		//.____ Creation ______________________________________________________

		static ArrayTransition_p	create( int duration, TransitionCurve curve = TransitionCurve::Linear);
		static ArrayTransition_p	create( const Blueprint& blueprint);

		//.____ Identification _________________________________________________

		const TypeInfo& typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Misc ___________________________________________________________

		int		duration() const { return m_duration; }

		void	snapshot(int timestampMicroSec, int nValues, const int * pStartValues, const int * pEndValues, int * pOutput);
		void	snapshot(int timestampMicroSec, int nValues, const float * pStartValues, const float * pEndValues, float * pOutput);

	protected:
		ArrayTransition(int duration, TransitionCurve curve);

		template<class BP>
		ArrayTransition(const Blueprint& bp) :
			m_duration(bp.duration),
			m_curve(bp.curve)
		{
			if (bp.finalizer)
				setFinalizer(bp.finalizer);
		}

		TransitionCurve	m_curve;
		int				m_duration;		// Microseconds


	};



}

#endif //TRANSITIONS_DOT_H
