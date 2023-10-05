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
#ifndef WG_BOXSKIN_DOT_H
#define WG_BOXSKIN_DOT_H
#pragma once

#include <wg_stateskin.h>
#include <wg_color.h>

#include <vector>

namespace wg
{

	class BoxSkin;
	typedef	StrongPtr<BoxSkin>	BoxSkin_p;
	typedef	WeakPtr<BoxSkin>	BoxSkin_wp;

	class BoxSkin : public StateSkin
	{
	public:

		//____ Blueprint ______________________________________________________

		struct StateData
		{
			HiColor			color = HiColor::Undefined;
			Coord			contentShift;
			HiColor			outlineColor = HiColor::Undefined;
		};

		struct StateBP
		{
			StateBP() {}
			StateBP( State state, StateData data ) : state(state), data(data) {}
			StateBP( State state, HiColor color ) : state(state) { data.color = color; }
			StateBP( State state, Coord contentShift ) : state(state)	{ data.contentShift = contentShift; }

			StateBP( State state, HiColor color, HiColor outlineColor ) : state(state)
			{
				data.color = color;
				data.outlineColor = outlineColor;
			}
			
			State			state = State::Normal;
			StateData		data;
		};

		struct Blueprint
		{
			BlendMode		blendMode = BlendMode::Undefined;
			HiColor			color = HiColor::White;
			int				layer = -1;
			Border			margin;
			int				markAlpha = 1;
			Border			outline;
			HiColor			outlineColor = HiColor::Black;
			Border			overflow;
			Border			padding;

			std::vector<StateBP>	states;
		};

		//.____ Creation __________________________________________

		static BoxSkin_p	create( const Blueprint& blueprint );
		static BoxSkin_p 	create(Border frame, HiColor fillColor, HiColor frameColor, Border contentPadding = Border() );

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Internal ________________________________________________________

		SizeSPX		_minSize(int scale) const override;
		SizeSPX		_defaultSize(int scale) const override;
		SizeSPX		_sizeForContent(const SizeSPX& contentSize, int scale) const override;

		bool		_isOpaque(State state) const override;
		bool		_isOpaque(const RectSPX& rect, const SizeSPX& canvasSize, int scale, State state) const override;

		bool		_markTest(const CoordSPX& ofs, const RectSPX& canvas, int scale, State state,
			float value = 1.f, float value2 = -1.f, int alphaOverride = -1 ) const override;

		void		_render(GfxDevice* pDevice, const RectSPX& canvas, int scale, State state,
			float value = 1.f, float value2 = -1.f, int animPos = 0,
			float* pStateFractions = nullptr) const override;

		RectSPX		_dirtyRect(const RectSPX& canvas, int scale, State newState, State oldState,
			float newValue = 1.f, float oldValue = 1.f,
			float newValue2 = -1.f, float oldValue2 = -1.f, int newAnimPos = 0, int oldAnimPos = 0,
			float* pNewStateFractions = nullptr, float* pOldStateFractions = nullptr) const override;

	private:
		BoxSkin( const Blueprint& blueprint );
		~BoxSkin() {};

		void	_updateOpaqueFlag();
		void	_updateUnsetColors();

		Border		m_outline;
		BlendMode	m_blendMode = BlendMode::Undefined;

		Bitmask<uint32_t>	m_stateColorMask = 1;
		Bitmask<uint32_t>	m_stateOutlineColorMask = 1;

		HiColor		m_fillColor[State::IndexAmount];
		HiColor		m_outlineColor[State::IndexAmount];
	};


} // namespace wg
#endif //WG_BOXSKIN_DOT_H


