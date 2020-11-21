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
#ifndef WG_TILESKIN_DOT_H
#define WG_TILESKIN_DOT_H
#pragma once

#include <wg_stateskin.h>
#include <wg_surface.h>
#include <wg_gradient.h>

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

		//.____ Creation __________________________________________

		static TileSkin_p	create();
		static TileSkin_p	create(Surface * pSurface );
		static TileSkin_p	create(std::initializer_list<std::tuple<State,Surface_p>> stateSurfaces);

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;


		//.____ Appearance _________________________________________________

		void	setSurface(Surface * pSurface);
		void	setSurface(State state, Surface * pSurface);
		void	setSurfaces(std::initializer_list<std::tuple<State, Surface_p>> stateSurfaces);

		Surface_p surface(State state = StateEnum::Normal) const;

		void	setColor(Color tint);
		void	setColor(State state, Color tint);
		void	setColor(std::initializer_list< std::tuple<State, Color> > stateTints);
		Color	color(State state) const;

		void	setGradient(const Gradient& gradient);
		Gradient gradient() const { return m_gradient; }

		void			setBlendMode(BlendMode mode);
		BlendMode		blendMode() const { return m_blendMode; }

		//.____ Geometry _________________________________________________

		Size	preferredSize() const override;

		//.____ Misc ____________________________________________________

		bool	isOpaque( State state ) const override;
		bool	isOpaque(const Rect& rect, const Size& canvasSize, State state) const override;

		bool	markTest(	const Coord& ofs, const Rect& canvas, State state, int opacityTreshold, 
							float value = 1.f, float value2 = -1.f) const override;

		void	render(	GfxDevice * pDevice, const Rect& canvas, State state, 
						float value = 1.f, float value2 = -1.f, int animPos = 0,
						float* pStateFractions = nullptr) const override;

		Rect	dirtyRect(	const Rect& canvas, State newState, State oldState, float newValue = 1.f, float oldValue = 1.f,
							float newValue2 = -1.f, float oldValue2 = -1.f, int newAnimPos = 0, int oldAnimPos = 0,
							float* pNewStateFractions = nullptr, float* pOldStateFractions = nullptr) const override;

	private:

		TileSkin();
		TileSkin(Surface * pSurface);
		~TileSkin() {};

		void		_updateOpaqueFlags();
		void		_updateUnsetStateSurfaces();
		void		_updateUnsetStateColors();

		BlendMode	m_blendMode = BlendMode::Blend;

		Bitmask<uint32_t>	m_stateSurfaceMask = 1;
		Bitmask<uint32_t>	m_stateColorMask = 1;

		Surface_p	m_stateSurfaces[StateEnum_Nb];
		Color		m_stateColors[StateEnum_Nb];
		bool		m_bStateOpaque[StateEnum_Nb];
		Gradient	m_gradient;
		bool		m_bGradient = false;
	};


} // namespace wg
#endif //WG_TILESKIN_DOT_H
