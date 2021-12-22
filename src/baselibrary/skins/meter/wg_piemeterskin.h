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

		const static int c_maxSlices = 12;

		//____ Blueprint ______________________________________________________

		struct Slice
		{
			float	size = -1.f;
			HiColor	minColor;
			HiColor	maxColor;
		};

		struct Blueprint
		{
			HiColor		backColor = Color::Transparent;

			BlendMode	blendMode = BlendMode::Undefined;

			Border		contentPadding;

			HiColor		emptyColor = Color::DarkBlue;

			Border		gfxPadding;

			HiColor		hubColor = Color::Transparent;
			float		hubSize = 0.75f;

			int			layer = -1;

			float		maxLength = 0.9f;
			float		minLength = 0.01f;

			bool		movingSlices = false;

			Size		preferredSize;

			bool		rectangular = false;

			float		rotation = 0.f;

			Slice		slices[c_maxSlices];










		};

		//.____ Creation __________________________________________

		static PieMeterSkin_p create( const Blueprint& bp );

		//.____ Identification __________________________________________

		const TypeInfo&			typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

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
		PieMeterSkin( const Blueprint& bp );
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

		Slice		m_slices[c_maxSlices];
		int			m_nSlices;
	};


} // namespace wg
#endif //WG_PIEMETERSKIN_DOT_H
