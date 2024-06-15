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
#ifndef WG_GRADIENTSKIN_DOT_H
#define WG_GRADIENTSKIN_DOT_H
#pragma once

#include <wg_stateskin.h>
#include <wg_gradient.h>

#include <vector>

namespace wg
{

	class GradientSkin;

	typedef	StrongPtr<GradientSkin>	GradientSkin_p;
	typedef	WeakPtr<GradientSkin>	GradientSkin_wp;


	class GradientSkin : public StateSkin
	{
	public:

		//____ Blueprint ______________________________________________________

		struct StateData
		{
			Coord			contentShift;
			Gradient		gradient;
		};

		struct StateBP
		{

			StateBP() {}
			StateBP(State state) : state(state) {}
			StateBP(State state, StateData data) : state(state), data(data) {}
			StateBP(State state, Gradient gradient) : state(state) { data.gradient = gradient; }
			StateBP(State state, Coord contentShift) : state(state) { data.contentShift = contentShift; }
			StateBP(State state, Gradient gradient, Coord contentShift) : state(state) { data.gradient = gradient; data.contentShift = contentShift; }

			State			state = State::Default;
			StateData		data;
		};


		struct Blueprint
		{
			BlendMode	blendMode = BlendMode::Blend;
			Finalizer_p	finalizer = nullptr;
			Gradient	gradient = HiColor::White;
			int			layer = -1;
			Border		margin;
			int			markAlpha = 1;
			Border		overflow;
			Border		padding;

			std::vector<StateBP>	states;
		};

		//.____ Creation __________________________________________

		static GradientSkin_p create(const Gradient& gradient);
		static GradientSkin_p create( const Blueprint& blueprint );

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Internal ____________________________________________________

		bool		_markTest(	const CoordSPX& ofs, const RectSPX& canvas, int scale, State state, 
								float value = 1.f, float value2 = -1.f, int alphaOverride = -1 ) const override;

		void		_render(	GfxDevice * pDevice, const RectSPX& canvas, int scale, State state, 
								float value = 1.f, float value2 = -1.f, int animPos = 0, float* pStateFractions = nullptr) const override;

		RectSPX		_dirtyRect( const RectSPX& canvas, int scale, State newState, State oldState, float newValue = 1.f, float oldValue = 1.f,
								float newValue2 = -1.f, float oldValue2 = -1.f, int newAnimPos = 0, int oldAnimPos = 0,
								float* pNewStateFractions = nullptr, float* pOldStateFractions = nullptr) const override;

		RectSPX		_coverage(const RectSPX& geo, int scale, State state) const override;

	private:
		GradientSkin( const Blueprint& blueprint );
		~GradientSkin() {};

		void	_updateUnsetGradients(Bitmask<uint32_t> stateMask);

		Gradient	m_gradient[State::IndexAmount];
		BlendMode	m_blendMode;
		bool		m_bOpaque;



	};


} // namespace wg
#endif //WG_GRADIENTSKIN_DOT_H
