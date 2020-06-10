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
#ifndef WG_COLORSKIN_DOT_H
#define WG_COLORSKIN_DOT_H
#pragma once

#include <wg_stateskin.h>
#include <wg_color.h>

#include <initializer_list>
#include <utility>

namespace wg
{

	class ColorStateSkin;
	typedef	StrongPtr<ColorStateSkin>	ColorStateSkin_p;
	typedef	WeakPtr<ColorStateSkin>		ColorStateSkin_wp;

	class ColorStateSkin : public StateSkin
	{
	public:
		//.____ Creation __________________________________________

		static ColorStateSkin_p	create();
		static ColorStateSkin_p create(Color color,  BorderI contentPadding = BorderI() );
		static ColorStateSkin_p	create(std::initializer_list< std::tuple<State,Color> > stateColors, BorderI contentPadding = BorderI() );

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;


		//.____ Appearance _________________________________________________

		void		setBlendMode(BlendMode mode);
		BlendMode	blendMode() const { return m_blendMode; }


		void		setColor(Color fill);
		void		setColor(State state, Color fill);
		void		setColor(std::initializer_list< std::tuple<State, Color> > stateColors);
		Color		color(State state) const;


		//.____ Misc ____________________________________________________


		bool		isOpaque( State state ) const override;
		bool		isOpaque(const Rect& rect, const Size& canvasSize, State state) const override;

		bool		isStateIdentical( State state, State comparedTo, float fraction = 1.f ) const override;

		bool		markTest(const Coord& ofs, const Rect& canvas, State state, int opacityTreshold, float fraction = 1.f ) const override;

		void		render(GfxDevice * pDevice, const Rect& _canvas, State state, float fraction = 1.f ) const override;


	private:
		ColorStateSkin();
		ColorStateSkin(Color color);
		~ColorStateSkin() {};

		void	_updateOpaqueFlag();
		void	_updateUnsetColors();

		BlendMode	m_blendMode = BlendMode::Blend;

		Bitmask<uint32_t>	m_stateColorMask = 1;

		Color		m_color[StateEnum_Nb];
	};


} // namespace wg
#endif //WG_COLORSKIN_DOT_H


