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
	typedef	WeakPtr<FillMeterSkin>		FillMeterSkin_wp;


	class FillMeterSkin : public Skin
	{
	public:
		//.____ Creation __________________________________________

		static FillMeterSkin_p create();
		static FillMeterSkin_p create(Direction direction, HiColor fillColorEmpty, HiColor fillColorFull, HiColor backColor = Color::Transparent, const Border& gfxPadding = Border(), const Border& contentPadding = Border(), pts minFillLength = 0);

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Geometry _________________________________________________

		void	setPreferredSize(const Size& preferred);

		//.____ Appearance ____________________________________________________

		void		setBlendMode(BlendMode mode);
		BlendMode	blendMode() const { return m_blendMode; }

		void	setDirection(Direction dir);
		Direction direction() const { return m_direction; }

		void	setGfxPadding(Border padding);
		Border	gfxPadding() const { return m_barPadding; }

		void	setBackColor(HiColor back);
		HiColor	backColor() const { return m_backColor; }

		void	setFillColors(HiColor empty, HiColor full);
		void	setFillColorEmpty(HiColor empty);
		HiColor	fillColorEmpty() const { return m_barColorEmpty; }
		void	setFillColorFull(HiColor full);
		HiColor	fillColorFull() const { return m_barColorFull; }

		void	setMinFillLength(int minFillLength);
		int     minFillLength() const { return m_minFillLength; }

        void    setCenteredBarOrigin(bool bCenter);
        bool    centeredBarOrigin() const { return m_bCenteredBarOrigin; }
        
		//.____ Internal ____________________________________________________

		SizeSPX	_preferredSize(int scale) const override;

		bool	_markTest(	const CoordSPX& ofs, const RectSPX& canvas, int sale, State state, int opacityTreshold, 
							float value = 1.f, float value2 = -1.f) const override;

		void 	_render(	GfxDevice * pDevice, const RectSPX& canvas, int scale, State state, float value = 1.f, 
							float value2 = -1.f, int animPos = 0, float* pStateFractions = nullptr) const override;

		RectSPX	_dirtyRect(	const RectSPX& canvas, int scale, State newState, State oldState, float newValue = 1.f, float oldValue = 1.f,
							float newValue2 = -1.f, float oldValue2 = -1.f, int newAnimPos = 0, int oldAnimPos = 0,
							float* pNewStateFractions = nullptr, float* pOldStateFractions = nullptr) const override;

	private:
		FillMeterSkin();
		FillMeterSkin(	Direction direction, HiColor barColorEmpty, HiColor barColorFull, HiColor backColor, 
						const Border& barPadding, const Border& contentPadding, pts minFillLength );
		~FillMeterSkin() {};

		RectSPX		_barFillArea(const RectSPX& canvas, int scale, float value, float value2) const;
		void		_updateOpacity();
		RectSPX		_valueChangeRect(const RectSPX& canvas, int scale, State state, float oldFraction, float newFraction) const;

		BlendMode	m_blendMode = BlendMode::Undefined;
		Direction	m_direction;
		Border		m_barPadding;
        pts		    m_minFillLength;
		HiColor		m_barColorEmpty;
		HiColor		m_barColorFull;
		HiColor		m_backColor;
		Size		m_preferredSize;
        bool        m_bCenteredBarOrigin = false;
	};


} // namespace wg
#endif //WG_FILLMETERSKIN_DOT_H
