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

		static PieMeterSkin_p create();
		static PieMeterSkin_p create(	float startAngle, float minLength, float maxLength, Color minColor, Color maxColor, Color emptyColor = Color::Transparent,
										float hubSize = 0.f, Color hubColor = Color::Transparent, 
										Color backColor = Color::Transparent, const BorderI& gfxPadding = BorderI(), 
										const BorderI& contentPadding = BorderI(), bool bStaticSections = true, bool bRectangular = false);

		//.____ Identification __________________________________________

		const TypeInfo&			typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Appearance ____________________________________________________

		bool	setSlices(std::initializer_list<Slice> slices);

		void	setRange(float min, float max);
		void	setStartAngle(float start);
		float	startAngle() const { return m_rangeStart; }
		void	setMinLength(float min);
		float	minLength() const { return m_minRange;  }
		void	setMaxLength(float max);
		float	maxLength() const { return m_maxRange; }

		void	setRectangular(bool bRectangular);
		bool	isRectangular() const { return m_bRectangular;  }

		void	setStaticSections(bool bStatic);
		bool	isStaticSections() const { return m_bStaticSections; }

		void	setGfxPadding(BorderI padding);
		Border	gfxPadding() const { return m_gfxPadding; }

		void	setEmptyColor(Color empty);
		Color	emptyColor() const { return m_emptyColor; }

		void	setHub(float size, Color color);
		void	setHubSize(float hubSize);
		float	hubSize() const { return m_hubSize; }
		void	setHubColor(Color hubColor);
		Color	hubColor() const { return m_hubColor; }

		void	setBackColor(Color back);
		Color	backColor() const { return m_backColor; }

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

		//.____ Misc ____________________________________________________

		bool	isOpaque() const override;
		bool	isOpaque(State state) const override;
		bool	isOpaque(const Rect& rect, const Size& canvasSize, State state) const override;

		bool	isStateIdentical(State state, State comparedTo, float fraction = 1.f) const override;

		bool	markTest(const Coord& ofs, const Rect& canvas, State state, int opacityTreshold, float fraction = 1.f) const override;
		void 	render(GfxDevice * pDevice, const Rect& canvas, State state, float fraction = 1.f) const override;

		Rect	fractionChangeRect(const Rect& canvas, State state, float oldFraction, float newFraction) const override;

	private:
		PieMeterSkin();
		PieMeterSkin(float start, float min, float max, Color minColor, Color maxColor, Color emptyColor, float hubSize, Color hubColor, Color backColor, const BorderI& piePadding, const BorderI& contentPadding, bool bStaticSections, bool bSquare);
		~PieMeterSkin() {};

		void		_updateOpacity();

		SizeI		m_preferredSize;

		float		m_rangeStart = 0.55f;
		float		m_minRange = 0.01f;
		float		m_maxRange = 0.9f;
		float		m_hubSize = 0.75f;
		bool		m_bRectangular = false;
		bool		m_bStaticSections = true;
		bool		m_bOpaque;

		BorderI		m_contentPadding;
		BorderI		m_gfxPadding;

		Color		m_hubColor = Color::Transparent;
		Color		m_backColor = Color::Transparent;
		Color		m_emptyColor = Color::DarkBlue;

		const static int c_maxSlices = 6;
		Slice		m_slices[c_maxSlices];
		int			m_nSlices;
	};


} // namespace wg
#endif //WG_PIEMETERSKIN_DOT_H
