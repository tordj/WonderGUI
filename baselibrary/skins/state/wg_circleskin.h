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
#ifndef WG_CIRCLESKIN_DOT_H
#define WG_CIRCLESKIN_DOT_H
#pragma once

#include <wg_stateskin.h>
#include <wg_color.h>

#include <initializer_list>
#include <utility>

namespace wg
{

	class CircleSkin;
	typedef	StrongPtr<CircleSkin>	CircleSkin_p;
	typedef	WeakPtr<CircleSkin>		CircleSkin_wp;

	class CircleSkin : public StateSkin
	{
	public:
		//.____ Creation __________________________________________

		static CircleSkin_p	create();
		static CircleSkin_p create(HiColor color, float size = 1.f, float thickness = 1.f, float outlineThickness = 0.f, HiColor outlineColor = Color::Black, BorderI contentPadding = BorderI());

		//.____ Identification __________________________________________

		const TypeInfo& typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;


		//.____ Appearance _________________________________________________

		void		setBlendMode(BlendMode mode);
		BlendMode	blendMode() const { return m_blendMode; }

		void		setAppearance(State state, HiColor color, float size = 1.f, float thickness = 1.f, float outlineThickness = 0.f, HiColor outlineColor = Color::Black);

		//.____ Misc ____________________________________________________


		bool		isOpaque(State state) const override;
		bool		isOpaque(const Rect& rect, const Size& canvasSize, State state) const override;

		bool		markTest(const Coord& ofs, const Rect& canvas, State state, int opacityTreshold,
			float value = 1.f, float value2 = -1.f) const override;

		void		render(GfxDevice* pDevice, const Rect& canvas, State state,
			float value = 1.f, float value2 = -1.f, int animPos = 0,
			float* pStateFractions = nullptr) const override;

		Rect	dirtyRect(const Rect& canvas, State newState, State oldState, float newValue = 1.f, float oldValue = 1.f,
			float newValue2 = -1.f, float oldValue2 = -1.f, int newAnimPos = 0, int oldAnimPos = 0,
			float* pNewStateFractions = nullptr, float* pOldStateFractions = nullptr) const override;

	private:
		CircleSkin();
		CircleSkin(HiColor color, float size, float thickness, float outlineThickness, HiColor outlineColor, BorderI contentPadding);
		~CircleSkin() {};

		void	_updateUnsetStates();

		BlendMode	m_blendMode = BlendMode::Blend;

		Bitmask<uint32_t>	m_stateInfoMask = 1;

		struct StateInfo
		{
			HiColor		color;
			float		size;
			float		thickness;
			float		outlineThickness;
			HiColor		outlineColor;
		};


		StateInfo		m_stateInfo[StateEnum_Nb];
	};


} // namespace wg
#endif //WG_CIRCLESKIN_DOT_H


