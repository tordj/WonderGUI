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
#ifndef WG3_FILLMETERSKIN_DOT_H
#define WG3_FILLMETERSKIN_DOT_H
#pragma once

#include <wg3_skin.h>
#include <wg3_color.h>

namespace wg
{

	class FillMeterSkin;

	typedef	StrongPtr<FillMeterSkin>	FillMeterSkin_p;
	typedef	WeakPtr<FillMeterSkin>		FillMeterSkin_wp;


	class FillMeterSkin : public Skin
	{
	public:
		//.____ Creation __________________________________________

		static FillMeterSkin_p create();
		static FillMeterSkin_p create(Direction direction, Color fillColorEmpty, Color fillColorFull, Color backColor = Color::Transparent, const BorderI& gfxPadding = BorderI(), const BorderI& contentPadding = BorderI(), bool bFillStartOutside = false);

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Geometry _________________________________________________

		void	setPreferredSize(const SizeI& preferred);
		Size	preferredSize() const override;

		//.____ Appearance ____________________________________________________

		void	setDirection(Direction dir);
		Direction direction() const { return m_direction; }

		void	setGfxPadding(BorderI padding);
		Border	gfxPadding() const { return m_barPadding; }

		void	setBackColor(Color back);
		Color	backColor() const { return m_backColor; }

		void	setFillColors(Color empty, Color full);
		void	setFillColorEmpty(Color empty);
		Color	fillColorEmpty() const { return m_barColorEmpty; }
		void	setFillColorFull(Color full);
		Color	fillColorFull() const { return m_barColorFull; }

		void	setFillStartOutside(bool bStartOutside);
		bool	isFillStartOutside() const { return m_bBarStartOutside; }

		//.____ Misc ____________________________________________________

		bool	markTest(	const Coord& ofs, const Rect& canvas, State state, int opacityTreshold, 
							float fraction = 1.f, float fraction2 = -1.f) const override;

		void 	render(	GfxDevice * pDevice, const Rect& canvas, State state, float fraction = 1.f, float fraction2 = -1.f) const override;

		Rect	fractionChangeRect(	const Rect& canvas, State state, float oldFraction, float newFraction, 
									float oldFraction2 = -1.f, float newFraction2 = -1.f) const override;

	private:
		FillMeterSkin();
		FillMeterSkin(	Direction direction, Color barColorEmpty, Color barColorFull, Color backColor, 
						const BorderI& barPadding, const BorderI& contentPadding, bool bBarStartOutside );
		~FillMeterSkin() {};

		Rect		_barFillArea(const Rect& canvas, float fraction, float fraction2) const;
		void		_updateOpacity();
		Rect		_fractionChangeRect(const Rect& canvas, State state, float oldFraction, float newFraction) const;


		Direction	m_direction;
		BorderI		m_barPadding;
		bool		m_bBarStartOutside;
		Color		m_barColorEmpty;
		Color		m_barColorFull;
		Color		m_backColor;
		Size		m_preferredSize;
	};


} // namespace wg
#endif //WG3_FILLMETERSKIN_DOT_H