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
#ifndef WG3_TILESKIN_DOT_H
#define WG3_TILESKIN_DOT_H
#pragma once

#include <wg3_stateskin.h>
#include <wg3_surface.h>
#include <wg3_gradient.h>

namespace wg
{


	class TileSkin;
	typedef	StrongPtr<TileSkin>	TileSkin_p;
	typedef	WeakPtr<TileSkin>		TileSkin_wp;


	class TileSkin : public StateSkin
	{
		//TODO: Add sanity-checking to all Set-methods.
		//TODO: Optimize rendering based on invisibleSections and opaqueSections!

	public:

		//____ Blueprint ______________________________________________________

		struct StateData
		{
			HiColor			color = HiColor::Undefined;
			Coord			contentShift;
			Surface_p		surface;
		};

		struct StateBP
		{
			State			state = StateEnum::Normal;
			StateData		data;
		};

		struct Blueprint
		{
			BlendMode		blendMode = BlendMode::Undefined;
			HiColor			color = HiColor::Undefined;
			Gradient		gradient;
			int				layer = -1;
			int				markAlpha = 1;
			Border			overflow;
			Border			padding;

			StateBP			states[StateEnum_Nb];
			Surface_p		surface;
		};

		//.____ Creation __________________________________________

		static TileSkin_p	create(Surface * pSurface, HiColor color = HiColor::Undefined, int layer = -1 );
		static TileSkin_p	create(const Blueprint& blueprint);

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Internal _________________________________________________

		SizeSPX	_preferredSize(int scale) const override;

		bool	_isOpaque( State state ) const override;
		bool	_isOpaque(const RectSPX& rect, const SizeSPX& canvasSize, int scale, State state) const override;

		bool	_markTest(	const CoordSPX& ofs, const RectSPX& canvas, int scale, State state, 
							float value = 1.f, float value2 = -1.f) const override;

		void	_render(	GfxDevice * pDevice, const RectSPX& canvas, int scale, State state, 
							float value = 1.f, float value2 = -1.f, int animPos = 0,
							float* pStateFractions = nullptr) const override;

		RectSPX	_dirtyRect(	const RectSPX& canvas, int scale, State newState, State oldState, float newValue = 1.f, float oldValue = 1.f,
							float newValue2 = -1.f, float oldValue2 = -1.f, int newAnimPos = 0, int oldAnimPos = 0,
							float* pNewStateFractions = nullptr, float* pOldStateFractions = nullptr) const override;

	private:

		TileSkin(const Blueprint& blueprint);
		~TileSkin() {};

		void		_updateOpaqueFlags();
		void		_updateUnsetStateSurfaces();
		void		_updateUnsetStateColors();

		BlendMode	m_blendMode;

		Bitmask<uint32_t>	m_stateSurfaceMask = 1;
		Bitmask<uint32_t>	m_stateColorMask = 1;

		Surface_p	m_stateSurfaces[StateEnum_Nb];
		HiColor		m_stateColors[StateEnum_Nb];
		bool		m_bStateOpaque[StateEnum_Nb];
		Gradient	m_gradient;
	};


} // namespace wg
#endif //WG3_TILESKIN_DOT_H