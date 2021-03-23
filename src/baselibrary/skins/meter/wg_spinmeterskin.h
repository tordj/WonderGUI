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
#ifndef WG_SPINMETERSKIN_DOT_H
#define WG_SPINMETERSKIN_DOT_H
#pragma once

#include <wg_skin.h>
#include <wg_color.h>
#include <wg_gradient.h>
#include <wg_surface.h>

namespace wg
{

	class SpinMeterSkin;

	typedef	StrongPtr<SpinMeterSkin>	SpinMeterSkin_p;
	typedef	WeakPtr<SpinMeterSkin>		SpinMeterSkin_wp;


	class SpinMeterSkin : public Skin
	{
	public:

		//.____ Creation __________________________________________

		static SpinMeterSkin_p create(	Surface * pSurface, Size preferredSize, CoordF srcCenter = CoordF(0.5f,0.5f), 
										CoordF dstCenter = CoordF(0.5f,0.5f), float fromDegrees = 0.f, float toDegrees = 360.f, 
										float zoom = 1.f, const BorderI& gfxPadding = BorderI(), const BorderI& contentPadding = BorderI() );

		//.____ Identification __________________________________________

		const TypeInfo&			typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Geometry _________________________________________________

		Size	preferredSize() const override;

		//.____ Appearance _________________________________________________

		void		setBlendMode(BlendMode mode);
		BlendMode	blendMode() const { return m_blendMode; }

		void		setColor(HiColor tintColor);
		HiColor		color() const { return m_color; }

		void		setGradient(const Gradient& gradient);
		Gradient	gradient() const { return m_gradient; }

		//.____ Misc ____________________________________________________

		bool	markTest(	const Coord& ofs, const Rect& canvas, State state, int opacityTreshold, 
							float value = 1.f, float value2 = -1.f) const override;

		void 	render(	GfxDevice * pDevice, const Rect& canvas, State state, 
						float value = 1.f, float value2 = -1.f, int animPos = 0,
						float* pStateFractions = nullptr) const override;

		Rect	dirtyRect(	const Rect& canvas, State newState, State oldState, float newValue = 1.f, float oldValue = 1.f,
							float newValue2 = -1.f, float oldValue2 = -1.f, int newAnimPos = 0, int oldAnimPos = 0,
							float* pNewStateFractions = nullptr, float* pOldStateFractions = nullptr) const override;

	private:
		SpinMeterSkin(	Surface * pSurface, Size preferredSize, CoordF srcCenter = CoordF(0.5f, 0.5f),
						CoordF dstCenter = CoordF(0.5f, 0.5f), float fromDegrees = 0.f, float toDegrees = 360.f,
						float zoom = 1.f, const BorderI& gfxPadding = BorderI(), const BorderI& contentPadding = BorderI());
		~SpinMeterSkin() {};

		void		_updateOpacityFlag();

		Surface_p	m_pSurface;
		Size		m_preferredSize;
		CoordF		m_srcCenter;
		CoordF		m_dstCenter;
		float		m_fromDegrees;
		float		m_toDegrees;
		float		m_zoom;
		BorderI		m_gfxPadding;

		BlendMode		m_blendMode = BlendMode::Blend;
		HiColor			m_color = Color::White;
		Gradient		m_gradient;
		bool			m_bGradient = false;
	};


} // namespace wg
#endif //WG_SPINMETERSKIN_DOT_H
