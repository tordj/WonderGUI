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
#ifndef WG_PIEMETERSKIN_DOT_H
#define WG_PIEMETERSKIN_DOT_H
#pragma once

#include <wg_skin.h>
#include <wg_color.h>

namespace wg
{

	class PieMeterSkin;

	typedef	StrongPtr<PieMeterSkin>	PieMeterSkin_p;


	class PieMeterSkin : public Skin
	{
	public:

		struct Slice
		{
			float	size;
			Color	minColor;
			Color	maxColor;
		};

		//.____ Creation __________________________________________

		static PieMeterSkin_p create(float start, float min, float max, float thickness, Color minColor, Color maxColor, Color hubColor = Color::Transparent, Color backColor = Color::Transparent, const BorderI& piePadding = BorderI(), const BorderI& contentPadding = BorderI(), bool bFixedSections = true, bool bRounded = true);

		//.____ Identification __________________________________________

		const TypeInfo&			typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Geometry _________________________________________________

		Size	minSize() const override;

		void	setPreferredSize(const SizeI& preferred);
		Size	preferredSize() const override;

		Size	sizeForContent(const Size& contentSize) const override;
	
		void	setContentPadding(const BorderI& padding);
		Border	contentPadding(State state) const override;
		Size	contentPaddingSize() const override;
		Coord	contentOfs(State state) const override;
		Rect	contentRect(const Rect& canvas, State state) const override;

		//.____ Control __________________________________________________

		bool	setSlices(std::initializer_list<Slice> slices);

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
		PieMeterSkin(float start, float min, float max, float thickness, Color minColor, Color maxColor, Color hubColor, Color backColor, const BorderI& piePadding, const BorderI& contentPadding, bool bFixedSections, bool bRounded);
		~PieMeterSkin() {};

		SizeI		m_preferredSize;

		float		m_rangeStart = 0.f;
		float		m_minRange = 0.01f;
		float		m_maxRange = 1.f;
		float		m_thickness = 1.f;
		bool		m_bRounded = true;
		bool		m_bFixedSections = true;
		bool		m_bOpaque;

		BorderI		m_contentPadding;
		BorderI		m_piePadding;

		Color		m_hubColor;
		Color		m_backColor;

		const static int c_maxSlices = 6;
		Slice		m_slices[c_maxSlices];
		int			m_nSlices;
	};


} // namespace wg
#endif //WG_PIEMETERSKIN_DOT_H
