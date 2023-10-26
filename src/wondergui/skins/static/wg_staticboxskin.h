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
#ifndef WG_STATICBOXSKIN_DOT_H
#define WG_STATICBOXSKIN_DOT_H
#pragma once

#include <wg_skin.h>
#include <wg_color.h>

namespace wg
{

	class StaticBoxSkin;

	typedef	StrongPtr<StaticBoxSkin>	StaticBoxSkin_p;
	typedef	WeakPtr<StaticBoxSkin>		StaticBoxSkin_wp;


	class StaticBoxSkin : public Skin
	{
	public:

		//____ Blueprint ______________________________________________________

		struct Blueprint
		{
			BlendMode	blendMode = BlendMode::Blend;
			HiColor		color = HiColor::White;
			Finalizer_p	finalizer = nullptr;
			int			layer = -1;
			Border		margin;
			int			markAlpha = 1;
			Border		outline;
			HiColor		outlineColor = HiColor::Black;
			Border		overflow;
			Border		padding;
		};

		//.____ Creation __________________________________________

		static StaticBoxSkin_p create(const Blueprint& blueprint);
		static StaticBoxSkin_p create(Border frame, HiColor fillColor, HiColor frameColor);

		//.____ Identification __________________________________________

		const TypeInfo&			typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Internal ____________________________________________________

		SizeSPX		_minSize(int scale) const override;
		SizeSPX		_defaultSize(int scale) const override;

		bool		_markTest(	const CoordSPX& ofs, const RectSPX& canvas, int scale, State state, 
								float value = 1.f, float value2 = -1.f, int alphaOverride = -1 ) const override;

		void		_render(	GfxDevice * pDevice, const RectSPX& canvas, int scale, State state, 
								float value = 1.f, float value2 = -1.f, int animPos = 0, 
								float* pStateFractions = nullptr) const override;

	private:
		StaticBoxSkin(const Blueprint& blueprint);
		~StaticBoxSkin() {};

		void		_updateOpaqueFlag();

		Border		m_outline;
		BlendMode	m_blendMode;

		HiColor		m_fillColor;
		HiColor		m_outlineColor;
	};


} // namespace wg
#endif //WG_STATICBOXSKIN_DOT_H
