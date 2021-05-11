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
	typedef	WeakPtr<PieMeterSkin>	PieMeterSkin_wp;


	class PieMeterSkin : public Skin
	{
	public:

		struct Slice
		{
			float	size;
			HiColor	minColor;
			HiColor	maxColor;
		};

		//.____ Creation __________________________________________

		static PieMeterSkin_p create();
		static PieMeterSkin_p create(	float startAngle, float minLength, float maxLength, HiColor minColor, HiColor maxColor, HiColor emptyColor = Color::Transparent,
										float hubSize = 0.f, HiColor hubColor = Color::Transparent, 
										HiColor backColor = Color::Transparent, const Border& gfxPadding = Border(), 
										const Border& contentPadding = Border(), bool bStaticSlices = true, bool bRectangular = false);

		//.____ Identification __________________________________________

		const TypeInfo&			typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Appearance ____________________________________________________

		void		setBlendMode(BlendMode mode);
		BlendMode	blendMode() const { return m_blendMode; }

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

		void	setGfxPadding(Border padding);
		Border	gfxPadding() const { return m_gfxPadding; }

		void	setEmptyColor(HiColor empty);
		HiColor	emptyColor() const { return m_emptyColor; }

		void	setHub(float size, HiColor color);
		void	setHubSize(float hubSize);
		float	hubSize() const { return m_hubSize; }
		void	setHubColor(HiColor hubColor);
		HiColor	hubColor() const { return m_hubColor; }

		void	setBackColor(HiColor back);
		HiColor	backColor() const { return m_backColor; }


		//.____ Geometry _________________________________________________

		void	setPreferredSize(const Size& preferred);
		Size	preferredSize() const { return m_preferredSize; }

		//.____ Internal ____________________________________________________

		//TODO: We probably need all kind of size related methods, taking set preferred size and content + padding into account.

		SizeSPX	_preferredSize(int scale) const override;

		bool	_markTest(	const CoordSPX& ofs, const RectSPX& canvas, int scale, State state, int opacityTreshold, 
							float value = 1.f, float value2 = -1.f) const override;

		void 	_render(	GfxDevice * pDevice, const RectSPX& canvas, int scale, State state, 
							float value = 1.f, float value2 = -1.f, int animPos = 0,
							float* pStateFractions = nullptr) const override;

		RectSPX	_dirtyRect(	const RectSPX& canvas, int scale, State newState, State oldState, float newValue = 1.f, float oldValue = 1.f,
							float newValue2 = -1.f, float oldValue2 = -1.f, int newAnimPos = 0, int oldAnimPos = 0,
							float* pNewStateFractions = nullptr, float* pOldStateFractions = nullptr) const override;

	private:
		PieMeterSkin();
		PieMeterSkin(float start, float min, float max, HiColor minColor, HiColor maxColor, HiColor emptyColor, float hubSize, HiColor hubColor, HiColor backColor, const Border& piePadding, const Border& contentPadding, bool bStaticSections, bool bSquare);
		~PieMeterSkin() {};

		void		_updateOpacity();

		Size		m_preferredSize;

		float		m_rangeStart = 0.55f;
		float		m_minRange = 0.01f;
		float		m_maxRange = 0.9f;
		float		m_hubSize = 0.75f;
		bool		m_bRectangular = false;
		bool		m_bStaticSections = true;

		Border		m_gfxPadding;

		BlendMode	m_blendMode = BlendMode::Undefined;
		HiColor		m_hubColor = Color::Transparent;
		HiColor		m_backColor = Color::Transparent;
		HiColor		m_emptyColor = Color::DarkBlue;

		const static int c_maxSlices = 12;
		Slice		m_slices[c_maxSlices];
		int			m_nSlices;
	};


} // namespace wg
#endif //WG_PIEMETERSKIN_DOT_H
