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
#ifndef WG_SPINANIMSKIN_DOT_H
#define WG_SPINANIMSKIN_DOT_H
#pragma once

#include <wg_animskin.h>
#include <wg_color.h>
#include <wg_gradient.h>
#include <wg_surface.h>

namespace wg
{

	class SpinAnimSkin;

	typedef	StrongPtr<SpinAnimSkin>	SpinAnimSkin_p;
	typedef	WeakPtr<SpinAnimSkin>		SpinAnimSkin_wp;


	class SpinAnimSkin : public AnimSkin
	{
	public:

		//.____ Creation __________________________________________

		static SpinAnimSkin_p create(	Surface * pSurface, Size preferredSize, int cycleDuration, CoordF srcCenter = CoordF(0.5f,0.5f), 
										CoordF dstCenter = CoordF(0.5f,0.5f), float fromDegrees = 0.f, float toDegrees = 360.f, 
										float zoom = 1.f, const Border& gfxPadding = Border(), const Border& contentPadding = Border() );

		//.____ Identification __________________________________________

		const TypeInfo&			typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Control __________________________________________________

		void	setCycleDuration(int millisec);
		int		cycleDuration() const { return m_cycleDuration; }								/// Returns duration of one cycle of animation.



		//.____ Appearance _________________________________________________

		void		setBlendMode(BlendMode mode);
		BlendMode	blendMode() const { return m_blendMode; }

		void		setColor(HiColor tintColor);
		HiColor		color() const { return m_color; }

		void		setGradient(const Gradient& gradient);
		Gradient	gradient() const { return m_gradient; }

		//.____ Internal ________________________________________________________

		SizeSPX	_preferredSize(int scale) const override;

		bool	
			_markTest(const CoordSPX& ofs, const RectSPX& canvas, int scale, State state, int opacityTreshold,
			float value = 1.f, float value2 = -1.f) const override;

		void 	_render(GfxDevice* pDevice, const RectSPX& canvas, int scale, State state,
			float value = 1.f, float value2 = -1.f, int animPos = 0, float* pStateFractions = nullptr) const override;

		RectSPX	_dirtyRect(const RectSPX& canvas, int scale, State newState, State oldState, float newValue = 1.f, float oldValue = 1.f,
			float newValue2 = -1.f, float oldValue2 = -1.f, int newAnimPos = 0, int oldAnimPos = 0,
			float* pNewStateFractions = nullptr, float* pOldStateFractions = nullptr) const override;

		int		_animationLength(State state) const override;

	private:
		SpinAnimSkin(	Surface * pSurface, Size preferredSize, int cycleDuration, CoordF srcCenter = CoordF(0.5f, 0.5f),
						CoordF dstCenter = CoordF(0.5f, 0.5f), float fromDegrees = 0.f, float toDegrees = 360.f,
						float zoom = 1.f, const Border& gfxPadding = Border(), const Border& contentPadding = Border());
		~SpinAnimSkin();

		void		_updateOpacityFlag();

		int			m_animationCycles[StateBits_Nb];

		int			m_cycleDuration;			// In millisec

		Surface_p	m_pSurface;
		Size		m_preferredSize;
		CoordF		m_srcCenter;
		CoordF		m_dstCenter;
		float		m_fromDegrees;
		float		m_toDegrees;
		float		m_zoom;
		Border		m_gfxPadding;

		BlendMode		m_blendMode = BlendMode::Undefined;
		HiColor			m_color = Color::White;
		Gradient		m_gradient;
		bool			m_bGradient = false;
	};


} // namespace wg
#endif //WG_SPINANIMSKIN_DOT_H