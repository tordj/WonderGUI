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
#ifndef WG_STATICTILESKIN_DOT_H
#define WG_STATICTILESKIN_DOT_H
#pragma once

#include <wg_skin.h>
#include <wg_surface.h>
#include <wg_gradient.h>

namespace wg
{
	class StaticTileSkin;

	typedef	StrongPtr<StaticTileSkin>	StaticTileSkin_p;
	typedef	WeakPtr<StaticTileSkin>	StaticTileSkin_wp;


	class StaticTileSkin : public Skin
	{
	public:
	
		//____ Blueprint ______________________________________________________

		struct Blueprint
		{
			BlendMode	blendMode = BlendMode::Undefined;
			HiColor		color = HiColor::Undefined;
			Gradient	gradient;
			int			layer = -1;
			int			markAlpha = 1;
			Border		overflow;
			Border		padding;
			Surface_p	surface;
		};

		
		//.____ Creation __________________________________________


		static StaticTileSkin_p create( Surface * pSurface );
		static StaticTileSkin_p create( const Blueprint& blueprint );

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Internal ____________________________________________________

		SizeSPX		_defaultSize(int scale) const override;

		bool		_markTest(	const CoordSPX& ofs, const RectSPX& canvas, int scale, State state,
								float value = 1.f, float value2 = -1.f, int alphaOverride = -1 ) const override;

		void		_render(	GfxDevice * pDevice, const RectSPX& canvas, int scale, State state, 
							float value = 1.f, float value2 = -1.f, int animPos = 0, 
							float* pStateFractions = nullptr) const override;

	private:
		StaticTileSkin(const Blueprint& blueprint);
		~StaticTileSkin() {};

		void		_updateOpacityFlag();

		Surface_p		m_pSurface;
		BlendMode		m_blendMode;
		HiColor			m_color;
		Gradient		m_gradient;
		bool			m_bGradient;
	};


} // namespace wg
#endif //WG_STATICTILESKIN_DOT_H
