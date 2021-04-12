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
#ifndef WG_STATICGRADIENTSKIN_DOT_H
#define WG_STATICGRADIENTSKIN_DOT_H
#pragma once

#include <wg_skin.h>
#include <wg_gradient.h>

namespace wg
{

	class StaticGradientSkin;

	typedef	StrongPtr<StaticGradientSkin>	StaticGradientSkin_p;
	typedef	WeakPtr<StaticGradientSkin>	StaticGradientSkin_wp;


	class StaticGradientSkin : public Skin
	{
	public:
		//.____ Creation __________________________________________

		static StaticGradientSkin_p create( const Gradient& gradient );

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Appearance _________________________________________________

		void		setBlendMode(BlendMode mode);
		BlendMode	blendMode() const { return m_blendMode; }

		//.____ Internal ____________________________________________________

		bool		_markTest(	const CoordSPX& ofs, const RectSPX& canvas, int scale, State state, int opacityTreshold, 
								float value = 1.f, float value2 = -1.f) const override;

		void		_render(	GfxDevice * pDevice, const RectSPX& canvas, int scale, State state, 
							float value = 1.f, float value2 = -1.f, int animPos = 0, float* pStateFractions = nullptr) const override;

	private:
		StaticGradientSkin( const Gradient& gradient );
		~StaticGradientSkin() {};

		Gradient	m_gradient;
		BlendMode	m_blendMode = BlendMode::Undefined;

	};


} // namespace wg
#endif //WG_STATICGRADIENTSKIN_DOT_H
