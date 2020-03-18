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

#include <wg_extendedskin.h>
#include <wg_color.h>

#include <initializer_list>
#include <utility>

namespace wg
{

	class ColorSkin;
	typedef	StrongPtr<ColorSkin>	ColorSkin_p;

	class ColorSkin : public ExtendedSkin
	{
	public:
		//.____ Creation __________________________________________

		static ColorSkin_p	create();
		static ColorSkin_p 	create(Color color,  BorderI contentPadding = BorderI() );
		static ColorSkin_p	create(std::initializer_list< std::tuple<State,Color> > stateColors, BorderI contentPadding = BorderI() );

		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const override;
		const char *		className( void ) const override;
		static const char	CLASSNAME[];


		//.____ Appearance _________________________________________________

		void		setBlendMode(BlendMode mode);
		BlendMode	blendMode() const { return m_blendMode; }


		void		setColor(Color fill);
		void		setColor(State state, Color fill);
		void		setColor(std::initializer_list< std::tuple<State, Color> > stateColors);
		Color		color(State state) const;


		//.____ Misc ____________________________________________________


		bool		isOpaque() const override;
		bool		isOpaque( State state ) const override;

		bool		isStateIdentical( State state, State comparedTo ) const override;

		//.____ Internal ______________________________________________________

		bool		_isOpaque(const RectI& rect, const SizeI& canvasSize, State state) const override;

		bool		_markTest(const CoordI& ofs, const RectI& canvas, State state, int opacityTreshold) const override;

		void		_render(GfxDevice * pDevice, const RectI& _canvas, State state) const override;


	private:
		ColorSkin();
		ColorSkin(Color color);
		~ColorSkin() {};

		void	_updateOpaqueFlag();
		void	_updateUnsetColors();

		bool		m_bOpaque;
		BlendMode	m_blendMode = BlendMode::Blend;

		Bitmask<uint32_t>	m_stateColorMask = 1;

		Color		m_color[StateEnum_Nb];
	};


} // namespace wg
#endif //WG_COLORSKIN_DOT_H


