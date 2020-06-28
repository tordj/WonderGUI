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

#include <initializer_list>
#include <utility>

namespace wg
{

	class BoxSkin;
	typedef	StrongPtr<BoxSkin>	BoxSkin_p;
	typedef	WeakPtr<BoxSkin>	BoxSkin_wp;

	class BoxSkin : public StateSkin
	{
	public:
		//.____ Creation __________________________________________

		static BoxSkin_p	create();
		static BoxSkin_p 	create(BorderI frame, Color fillColor, Color frameColor );
		static BoxSkin_p	create(BorderI frame, std::initializer_list< std::tuple<State,Color,Color> > stateColors );

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;


		//.____ Appearance _________________________________________________

		void		setBlendMode(BlendMode mode);
		BlendMode	blendMode() const { return m_blendMode; }

		void		setFrame( BorderI frame );

		void						setColors(Color fill, Color frame);
		void						setColors(State state, Color fill, Color frame);
		void						setColors(std::initializer_list< std::tuple<State, Color, Color> > stateColors);
		std::tuple<Color, Color>	colors(State state) const;

		//.____ Geometry _________________________________________________

		Size		minSize() const override;
		Size		preferredSize() const override;
		Size		sizeForContent(const Size& contentSize) const override;

		//.____ Misc ____________________________________________________

		bool		isOpaque( State state ) const override;
		bool		isOpaque(const Rect& rect, const Size& canvasSize, State state) const override;

		bool		isStateIdentical( State state, State comparedTo, float fraction = 1.f, float fraction2 = -1.f) const override;

		bool		markTest(	const Coord& ofs, const Rect& canvas, State state, int opacityTreshold, 
								float fraction = 1.f, float fraction2 = -1.f) const override;

		void		render(	GfxDevice * pDevice, const Rect& canvas, State state, 
							float fraction = 1.f, float fraction2 = -1.f) const override;



	private:
		BoxSkin();
		BoxSkin(BorderI frame, Color fillColor, Color frameColor  );
		~BoxSkin() {};

		void	_updateOpaqueFlag();
		void	_updateUnsetColors();

		BorderI		m_frame;							// In points
		BlendMode	m_blendMode = BlendMode::Blend;

		Bitmask<uint32_t>	m_stateColorMask = 1;

		Color		m_fillColor[StateEnum_Nb];
		Color		m_frameColor[StateEnum_Nb];
	};


} // namespace wg
#endif //WG_BOXSKIN_DOT_H


