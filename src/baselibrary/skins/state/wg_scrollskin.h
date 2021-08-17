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
#ifndef WG_SCROLLSKIN_DOT_H
#define WG_SCROLLSKIN_DOT_H
#pragma once

#include <wg_stateskin.h>
#include <wg_surface.h>
#include <wg_gradient.h>

namespace wg
{


	class ScrollSkin;
	typedef	StrongPtr<ScrollSkin>	ScrollSkin_p;
	typedef	WeakPtr<ScrollSkin>		ScrollSkin_wp;


	class ScrollSkin : public StateSkin
	{
		//TODO: Add sanity-checking to all Set-methods.
		//TODO: Optimize rendering based on invisibleSections and opaqueSections!

	public:

		//.____ Creation __________________________________________

		static ScrollSkin_p	create();
		static ScrollSkin_p	create(Surface* pSurface, pts windowLength, StateBits scrollState, int scrollTime, Direction scrollDirection, std::initializer_list<State> stateBlocks = { StateEnum::Normal}, pts spacing = 0);
		static ScrollSkin_p	create(Surface* pSurface, pts windowLength, StateBits scrollState, int scrollTime, Direction scrollDirection, Rect firstBlock, std::initializer_list<State> stateBlocks = { StateEnum::Normal }, pts spacing = 0);

		//.____ Identification __________________________________________

		const TypeInfo& typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;


		//.____ Appearance _________________________________________________

		void		setBlock(Coord ofs);
		void		setBlock(State state, Coord ofs);
		void		setBlocks(std::initializer_list<State> stateBlocks, Axis axis = Axis::Y, pts spacing = 0, Coord blockStartOfs = { 0,0 });
		Rect		block(State state) const;

		void		setColor(HiColor tint);
		void		setColor(State state, HiColor tint);
		void		setColor(std::initializer_list< std::tuple<State, HiColor> > stateTints);
		HiColor		color(State state) const;

		void		setGradient(const Gradient& gradient);
		Gradient	gradient() const { return m_gradient; }

		void		setBlendMode(BlendMode mode);
		BlendMode	blendMode() const { return m_blendMode; }

		//.____ Internal _________________________________________________

		SizeSPX		_preferredSize(int scale) const override;

		bool		_isOpaque(State state) const override;
		bool		_isOpaque(const RectSPX& rect, const SizeSPX& canvasSize, int scale, State state) const override;

		bool		_markTest(	const CoordSPX& ofs, const RectSPX& canvas, int scale, State state, int opacityTreshold,
								float value = 1.f, float value2 = -1.f) const override;

		void		_render(	GfxDevice* pDevice, const RectSPX& canvas, int scale, State state,
								float value = 1.f, float value2 = -1.f, int animPos = 0,
								float* pStateFractions = nullptr) const override;

		RectSPX		_dirtyRect(	const RectSPX& canvas, int scale, State newState, State oldState, float newValue = 1.f, float oldValue = 1.f,
								float newValue2 = -1.f, float oldValue2 = -1.f, int newAnimPos = 0, int oldAnimPos = 0,
								float* pNewStateFractions = nullptr, float* pOldStateFractions = nullptr) const override;

		Bitmask<uint8_t>	_transitioningStates() const override;
		const int*			_transitionTimes() const override;

	private:

		ScrollSkin();
		ScrollSkin(Surface* pSurface, pts windowLength, StateBits scrollState, int scrollTime, Direction scrollDirection, Size blockSize);
		~ScrollSkin() {};

		void		_updateOpaqueFlags();
		void		_updateUnsetStateBlocks();
		void		_updateUnsetStateColors();

		RectSPX		_partInCanvas(int scale, State state, float* pStateFractions) const;

		Surface_p	m_pSurface;
		Direction	m_scrollDirection = Direction::Right;
		Size		m_blockSize;
		pts			m_windowLength = 0;
		int			m_scrollTime = 250;						// Millisec
		StateBits	m_scrollState = StateBits::Selected;

		Gradient	m_gradient;

		BlendMode	m_blendMode = BlendMode::Undefined;

		Bitmask<uint32_t>	m_stateBlockMask = 1;
		Bitmask<uint32_t>	m_stateColorMask = 1;

		Coord		m_stateBlocks[StateEnum_Nb];
		HiColor		m_stateColors[StateEnum_Nb];
		bool		m_bStateOpaque[StateEnum_Nb];

		int			m_transitionTimes[StateBits_Nb] = { 0,0,0,0,0,0 };
	};


} // namespace wg
#endif //WG_BLOCKSKIN_DOT_H
