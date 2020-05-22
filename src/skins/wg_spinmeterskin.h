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
#include <wg_surface.h>

namespace wg
{

	class SpinMeterSkin;

	typedef	StrongPtr<SpinMeterSkin>	SpinMeterSkin_p;


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

		Size	minSize() const override;
		Size	preferredSize() const override;

		Size	sizeForContent(const Size& contentSize) const override;
	
		void	setContentPadding(const BorderI& padding);
		Border	contentPadding(State state) const override;
		Size	contentPaddingSize() const override;
		Coord	contentOfs(State state) const override;
		Rect	contentRect(const Rect& canvas, State state) const override;

		//.____ Misc ____________________________________________________

		bool	isOpaque() const override;
		bool	isOpaque(State state) const override;
		bool	isOpaque(const Rect& rect, const Size& canvasSize, State state) const override;

		bool	isStateIdentical(State state, State comparedTo, float fraction = 1.f) const override;

		bool	markTest(const Coord& ofs, const Rect& canvas, State state, int opacityTreshold, float fraction = 1.f) const override;
		void 	render(GfxDevice * pDevice, const Rect& canvas, State state, float fraction = 1.f) const override;

		Rect	fractionChangeRect(const Rect& canvas, State state, float oldFraction, float newFraction) const override;

	private:
		SpinMeterSkin(	Surface * pSurface, Size preferredSize, CoordF srcCenter = CoordF(0.5f, 0.5f),
						CoordF dstCenter = CoordF(0.5f, 0.5f), float fromDegrees = 0.f, float toDegrees = 360.f,
						float zoom = 1.f, const BorderI& gfxPadding = BorderI(), const BorderI& contentPadding = BorderI());
		~SpinMeterSkin() {};

		Surface_p	m_pSurface;
		Size		m_preferredSize;
		CoordF		m_srcCenter;
		CoordF		m_dstCenter;
		float		m_fromDegrees;
		float		m_toDegrees;
		float		m_zoom;
		BorderI		m_gfxPadding;
		BorderI		m_contentPadding;

		bool		m_bOpaque;
	};


} // namespace wg
#endif //WG_PIEMETERSKIN_DOT_H
