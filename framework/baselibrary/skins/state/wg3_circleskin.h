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
#ifndef WG3_CIRCLESKIN_DOT_H
#define WG3_CIRCLESKIN_DOT_H
#pragma once

#include <wg3_stateskin.h>
#include <wg3_color.h>

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

		//____ Blueprint ______________________________________________________

		struct StateData
		{
			HiColor			color = HiColor::Undefined;
			Coord			contentShift;

			HiColor			outlineColor = HiColor::Undefined;
			pts				outlineThickness = -1;

			float			size = -1;
			pts				thickness = -1;
		};

		struct StateBP
		{
			State			state = StateEnum::Normal;
			StateData		data;
		};

		struct Blueprint
		{
			BlendMode		blendMode = BlendMode::Undefined;

			HiColor			color = HiColor::White;

			int				layer = -1;
			int				markAlpha = 1;

			HiColor			outlineColor = HiColor::Black;
			pts				outlineThickness = 0;
			Border			overflow;
			Border			padding;
			float			size = 1;

			StateBP			states[StateEnum_Nb];

			pts				thickness = 1;
		};

		//.____ Creation __________________________________________

		static CircleSkin_p	create( const Blueprint& blueprint );

		//.____ Identification __________________________________________

		const TypeInfo& typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Internal ____________________________________________________


		bool		_isOpaque(State state) const override;
		bool		_isOpaque(const RectSPX& rect, const SizeSPX& canvasSize, int scale, State state) const override;

		bool		_markTest(const CoordSPX& ofs, const RectSPX& canvas, int scale, State state,
								float value = 1.f, float value2 = -1.f) const override;

		void		_render(GfxDevice* pDevice, const RectSPX& canvas, int scale, State state,
							float value = 1.f, float value2 = -1.f, int animPos = 0,
							float* pStateFractions = nullptr) const override;

		RectSPX		_dirtyRect(	const RectSPX& canvas, int scale, State newState, State oldState, float newValue = 1.f, float oldValue = 1.f,
								float newValue2 = -1.f, float oldValue2 = -1.f, int newAnimPos = 0, int oldAnimPos = 0,
								float* pNewStateFractions = nullptr, float* pOldStateFractions = nullptr) const override;

	private:
		CircleSkin( const Blueprint& blueprint );
		~CircleSkin() {};

		void	_updateUnsetStates();

		BlendMode	m_blendMode = BlendMode::Blend;

		Bitmask<uint32_t>	m_stateColorMask = 1;
		Bitmask<uint32_t>	m_stateSizeMask = 1;
		Bitmask<uint32_t>	m_stateThicknessMask = 1;
		Bitmask<uint32_t>	m_stateOutlineThicknessMask = 1;
		Bitmask<uint32_t>	m_stateOutlineColorMask = 1;


		struct StateInfo
		{
			HiColor		color;
			float		size;
			pts			thickness;
			pts			outlineThickness;
			HiColor		outlineColor;
		};


		StateInfo		m_stateInfo[StateEnum_Nb];
	};


} // namespace wg
#endif //WG3_CIRCLESKIN_DOT_H

