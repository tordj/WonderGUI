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

		static SpinAnimSkin_p create(	Surface * pSurface, Size preferredSize, CoordF srcCenter = CoordF(0.5f,0.5f), 
										CoordF dstCenter = CoordF(0.5f,0.5f), float fromDegrees = 0.f, float toDegrees = 360.f, 
										float zoom = 1.f, const BorderI& gfxPadding = BorderI(), const BorderI& contentPadding = BorderI() );

		//.____ Identification __________________________________________

		const TypeInfo&			typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Control __________________________________________________

		void	setCycleDuration(int millisec);
		int		cycleDuration() const { return m_cycleDuration; }								/// Returns duration of one cycle of animation.

		//.____ Geometry _________________________________________________

		Size	preferredSize() const override;

		//.____ Misc ____________________________________________________

		bool	markTest(	const Coord& ofs, const Rect& canvas, State state, int opacityTreshold, 
							float fraction = 1.f, float fraction2 = -1.f) const override;

		void 	render(	GfxDevice * pDevice, const Rect& canvas, State state, 
						float fraction = 1.f, float fraction2 = -1.f) const override;

		Rect	fractionChangeRect(	const Rect& canvas, State state, float oldFraction, float newFraction,
									float oldFraction2 = -1.f, float newFraction2 = -1.f) const override;

	private:
		SpinAnimSkin(	Surface * pSurface, Size preferredSize, CoordF srcCenter = CoordF(0.5f, 0.5f),
						CoordF dstCenter = CoordF(0.5f, 0.5f), float fromDegrees = 0.f, float toDegrees = 360.f,
						float zoom = 1.f, const BorderI& gfxPadding = BorderI(), const BorderI& contentPadding = BorderI());
		~SpinAnimSkin();


		int			m_cycleDuration;			// In millisec
		int			m_cycleProgress;			// In millisec

		Surface_p	m_pSurface;
		Size		m_preferredSize;
		CoordF		m_srcCenter;
		CoordF		m_dstCenter;
		float		m_fromDegrees;
		float		m_toDegrees;
		float		m_zoom;
		BorderI		m_gfxPadding;

	};


} // namespace wg
#endif //WG_SPINANIMSKIN_DOT_H
