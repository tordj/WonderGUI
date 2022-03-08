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

		//____ Blueprint ______________________________________________________

		struct StateData
		{
			HiColor			color = HiColor::Undefined;
			Coord			contentShift;
			bool			hasBlock = true;
		};

		struct StateBP
		{
			State			state = StateEnum::Normal;
			StateData		data;
		};

		struct Blueprint
		{

			BlendMode		blendMode = BlendMode::Undefined;

			Rect			blockOne;
			pts				blockSpacing = 0;
			HiColor			color = HiColor::Undefined;

			Gradient		gradient;
			int				layer = -1;
			int				markAlpha = 1;
			Border			overflow;
			Border			padding;

			Direction		scrollDirection = Direction::Right;
			pts				scrollDistance = 0;
			int				scrollDuration = 250;
			StateBits		scrollState = StateBits::Selected;			

			StateBP			states[StateEnum_Nb];
			Surface_p		surface;
		};

		//.____ Creation __________________________________________

		static ScrollSkin_p	create( const Blueprint& blueprint );

		//.____ Identification __________________________________________

		const TypeInfo& typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;


		//.____ Internal _________________________________________________

		SizeSPX		_preferredSize(int scale) const override;

		bool		_isOpaque(State state) const override;
		bool		_isOpaque(const RectSPX& rect, const SizeSPX& canvasSize, int scale, State state) const override;

		bool		_markTest(	const CoordSPX& ofs, const RectSPX& canvas, int scale, State state,
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
		ScrollSkin( const Blueprint& blueprint);
		~ScrollSkin() {};

		void		_updateOpaqueFlags();
		void		_updateUnsetStateBlocks();
		void		_updateUnsetStateColors();

		RectSPX		_partInCanvas(int scale, State state, float* pStateFractions) const;

		Surface_p	m_pSurface;
		Direction	m_scrollDirection = Direction::Right;
		Size		m_blockSize;
		pts			m_scrollDistance = 0;
		int			m_scrollDuration = 250;						// Millisec
		StateBits	m_scrollState = StateBits::Selected;

		Gradient	m_gradient;

		BlendMode	m_blendMode;

		Bitmask<uint32_t>	m_stateBlockMask = 1;
		Bitmask<uint32_t>	m_stateColorMask = 1;

		Coord		m_stateBlocks[StateEnum_Nb];
		HiColor		m_stateColors[StateEnum_Nb];
		bool		m_bStateOpaque[StateEnum_Nb];

		int			m_transitionTimes[StateBits_Nb] = { 0,0,0,0,0,0 };
	};


} // namespace wg
#endif //WG_BLOCKSKIN_DOT_H
