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
#ifndef WG_FILLMETERSKIN_DOT_H
#define WG_FILLMETERSKIN_DOT_H
#pragma once

#include <wg_skin.h>
#include <wg_color.h>

namespace wg
{

	class FillMeterSkin;

	typedef	StrongPtr<FillMeterSkin>	FillMeterSkin_p;


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

		Size	minSize() const override;

		void	setPreferredSize(const SizeI& preferred);
		Size	preferredSize() const override;

		void	setContentPadding(const BorderI& padding);
		Border	contentPadding(State state) const override;
		Size	contentPaddingSize() const override;
		Coord	contentOfs(State state) const override;
		Rect	contentRect(const Rect& canvas, State state) const override;

		Size	sizeForContent(const Size& contentSize) const override;

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

		bool	isOpaque() const override;
		bool	isOpaque(State state) const override;
		bool	isOpaque(const Rect& rect, const Size& canvasSize, State state) const override;

		bool	isStateIdentical(State state, State comparedTo, float fraction = 1.f) const override;

		bool	markTest(const Coord& ofs, const Rect& canvas, State state, int opacityTreshold, float fraction = 1.f) const override;
		void 	render(GfxDevice * pDevice, const Rect& canvas, State state, float fraction = 1.f) const override;

		Rect	fractionChangeRect(const Rect& canvas, State state, float oldFraction, float newFraction) const override;

	private:
		FillMeterSkin();
		FillMeterSkin(Direction direction, Color barColorEmpty, Color barColorFull, Color backColor, const BorderI& barPadding, const BorderI& contentPadding, bool bBarStartOutside);
		~FillMeterSkin() {};

		Rect		_barFillArea(const Rect& canvas, float fraction) const;


		Direction	m_direction;
		BorderI		m_contentPadding;
		BorderI		m_barPadding;
		bool		m_bBarStartOutside;
		Color		m_barColorEmpty;
		Color		m_barColorFull;
		Color		m_backColor;
		Size		m_preferredSize;
	};


} // namespace wg
#endif //WG_FILLMETERSKIN_DOT_H
