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
#ifndef WG_FILLBARSKIN_DOT_H
#define WG_FILLBARSKIN_DOT_H
#pragma once

#include <wg_skin.h>
#include <wg_color.h>

namespace wg
{

	class FillBarSkin;

	typedef	StrongPtr<FillBarSkin>	FillBarSkin_p;


	class FillBarSkin : public Skin
	{
	public:
		//.____ Creation __________________________________________

		static FillBarSkin_p create(Direction direction, Color barColor, Color backColor, const BorderI& barPadding = BorderI(), const BorderI& contentPadding = BorderI(), bool bBarStartOutside = false );

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Geometry _________________________________________________

		Size	minSize() const override;
		Size	preferredSize() const override;

		Size	sizeForContent(const Size& contentSize) const override;
		Border	contentPadding() const override;
		Size	contentPaddingSize() const override;
		Coord	contentOfs(State state) const override;
		Rect	contentRect(const Rect& canvas, State state) const override;

		//.____ Misc ____________________________________________________

		bool	isOpaque() const override;
		bool	isOpaque(State state) const override;
		bool	isOpaque(const Rect& rect, const Size& canvasSize, State state) const override;

		bool	isStateIdentical(State state, State comparedTo, float fraction = 1.f) const override;

		bool	markTest(const Coord& ofs, const Rect& canvas, State state, int opacityTreshold, float fraction = 1.f) const override;
		void 	render(GfxDevice * pDevice, const Rect& canvas, State state, float fraction = 1.f) const override;

		bool	ignoresFraction() const override;
		Rect	fractionChangeRect(const Rect& canvas, State state, float oldFraction, float newFraction) const override;

	private:
		FillBarSkin(Direction direction, Color barColor, Color backColor, const BorderI& barPadding, const BorderI& contentPadding, bool bBarStartOutside);
		~FillBarSkin() {};

		Rect		_barFillArea(const Rect& canvas, float fraction) const;


		Direction	m_direction;
		BorderI		m_contentPadding;
		BorderI		m_barPadding;
		bool		m_bBarStartOutside = false;
		Color		m_barColor;
		Color		m_backColor;
	};


} // namespace wg
#endif //WG_STATICCOLORSKIN_DOT_H
