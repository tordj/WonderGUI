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
#include <wg_gradient.h>

namespace wg
{

	class FillMeterSkin;

	typedef	StrongPtr<FillMeterSkin>	FillMeterSkin_p;
	typedef	WeakPtr<FillMeterSkin>		FillMeterSkin_wp;


	class FillMeterSkin : public Skin
	{
	public:

		//____ Blueprint ______________________________________________________

		struct Blueprint
		{
			HiColor		backColor = HiColor::Transparent;
			BlendMode	blendMode = BlendMode::Undefined;
			HiColor		color = Color::Blue;
			Direction	direction = Direction::Right;
			Border		gfxPadding;
			Gradient	gradient;

			int			layer = -1;
			Border		margin;
			int			markAlpha = 1;

			HiColor		maxColor = HiColor::Undefined;		// Overrides barColor when both min/max are set
			HiColor		minColor = HiColor::Undefined;

			Border		overflow;
			Border		padding;
			Size		defaultSize;						// Mandatory
			bool		startFromCenter = false;
			pts			startLength = 0;
		};

		//.____ Creation __________________________________________

		static FillMeterSkin_p create( const Blueprint& blueprint );

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;
        
		//.____ Internal ____________________________________________________

		SizeSPX	_defaultSize(int scale) const override;

		bool	_markTest(	const CoordSPX& ofs, const RectSPX& canvas, int sale, State state, 
							float value = 1.f, float value2 = -1.f, int alphaOverride = -1 ) const override;

		void 	_render(	GfxDevice * pDevice, const RectSPX& canvas, int scale, State state, float value = 1.f, 
							float value2 = -1.f, int animPos = 0, float* pStateFractions = nullptr) const override;

		RectSPX	_dirtyRect(	const RectSPX& canvas, int scale, State newState, State oldState, float newValue = 1.f, float oldValue = 1.f,
							float newValue2 = -1.f, float oldValue2 = -1.f, int newAnimPos = 0, int oldAnimPos = 0,
							float* pNewStateFractions = nullptr, float* pOldStateFractions = nullptr) const override;

	private:
		FillMeterSkin();
		FillMeterSkin(	const Blueprint& blueprint );
		~FillMeterSkin() {};

		RectSPX		_barFillArea(const RectSPX& canvas, int scale, float value, float value2) const;
		void		_updateOpacity();
		RectSPX		_valueChangeRect(const RectSPX& canvas, int scale, State state, float oldFraction, float newFraction) const;

		BlendMode	m_blendMode = BlendMode::Undefined;
		Direction	m_direction;
		Border		m_gfxPadding;
        pts		    m_minBarLength;
		HiColor		m_barColorEmpty;
		HiColor		m_barColorFull;
		HiColor		m_backColor;
		Size		m_defaultSize;
        bool        m_bCenteredBarOrigin = false;

		Gradient	m_gradient;
		bool		m_bGradient = false;
	};


} // namespace wg
#endif //WG_FILLMETERSKIN_DOT_H
