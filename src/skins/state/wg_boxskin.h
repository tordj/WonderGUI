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
		static BoxSkin_p 	create(BorderI frame, HiColor fillColor, HiColor frameColor );
		static BoxSkin_p	create(BorderI frame, std::initializer_list< std::tuple<State,HiColor,HiColor> > stateColors );

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;


		//.____ Appearance _________________________________________________

		void		setBlendMode(BlendMode mode);
		BlendMode	blendMode() const { return m_blendMode; }

		void		setFrame( BorderI frame );

		void						setColors(HiColor fill, HiColor frame);
		void						setColors(State state, HiColor fill, HiColor frame);
		void						setColors(std::initializer_list< std::tuple<State, HiColor, HiColor> > stateColors);
		std::tuple<HiColor, HiColor>	colors(State state) const;

		//.____ Geometry _________________________________________________

		Size		minSize() const override;
		Size		preferredSize() const override;
		Size		sizeForContent(const Size& contentSize) const override;

		//.____ Misc ____________________________________________________

		bool		isOpaque( State state ) const override;
		bool		isOpaque(const Rect& rect, const Size& canvasSize, State state) const override;

		bool		markTest(	const Coord& ofs, const Rect& canvas, State state, int opacityTreshold, 
								float value = 1.f, float value2 = -1.f) const override;

		void		render(	GfxDevice * pDevice, const Rect& canvas, State state, 
							float value = 1.f, float value2 = -1.f, int animPos = 0,
							float* pStateFractions = nullptr) const override;

		Rect	dirtyRect(	const Rect& canvas, State newState, State oldState, float newValue = 1.f, float oldValue = 1.f,
							float newValue2 = -1.f, float oldValue2 = -1.f, int newAnimPos = 0, int oldAnimPos = 0,
							float* pNewStateFractions = nullptr, float* pOldStateFractions = nullptr) const override;


	private:
		BoxSkin();
		BoxSkin(BorderI frame, HiColor fillColor, HiColor frameColor  );
		~BoxSkin() {};

		void	_updateOpaqueFlag();
		void	_updateUnsetColors();

		BorderI		m_frame;							// In points
		BlendMode	m_blendMode = BlendMode::Blend;

		Bitmask<uint32_t>	m_stateColorMask = 1;

		HiColor		m_fillColor[StateEnum_Nb];
		HiColor		m_frameColor[StateEnum_Nb];
	};


} // namespace wg
#endif //WG_BOXSKIN_DOT_H


