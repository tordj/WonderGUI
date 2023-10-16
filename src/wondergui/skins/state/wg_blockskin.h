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
#ifndef WG_BLOCKSKIN_DOT_H
#define WG_BLOCKSKIN_DOT_H
#pragma once

#include <wg_stateskin.h>
#include <wg_surface.h>
#include <wg_gradient.h>

#include <vector>

namespace wg
{


	class BlockSkin;
	typedef	StrongPtr<BlockSkin>	BlockSkin_p;
	typedef	WeakPtr<BlockSkin>		BlockSkin_wp;


	class BlockSkin : public StateSkin
	{
		//TODO: Add sanity-checking to all Set-methods.
		//TODO: Optimize rendering based on invisibleSections and opaqueSections!

	public:

		//____ Blueprint ______________________________________________________

		struct StateData
		{
			HiColor			color = HiColor::Undefined;
			Coord			contentShift;
			bool			reuseBlock = false;
			State			reuseBlockFrom = State::Default;
		};

		struct StateBP
		{
			StateBP() {}
			StateBP( State state ) : state(state) {}
			StateBP( State state, StateData data ) : state(state), data(data) {}
			StateBP( State state, HiColor color, bool reuseBlock = false, State reuseBlockFrom = State::Default ) : state(state) { data.color = color; data.reuseBlock = reuseBlock; data.reuseBlockFrom = reuseBlockFrom; }
			StateBP( State state, Coord contentShift, bool reuseBlock = false, State reuseBlockFrom = State::Default ) : state(state)	{ data.contentShift = contentShift; data.reuseBlock = reuseBlock; data.reuseBlockFrom = reuseBlockFrom; }

			State			state = State::Default;
			StateData		data;
		};

		struct RigidPartXBlueprint
		{
			pts			begin = 0;
			pts			length = 0;
			YSections	sections = YSections::None;
		};

		struct RigidPartYBlueprint
		{
			pts			begin = 0;
			pts			length = 0;
			XSections	sections = XSections::None;
		};

		struct Blueprint
		{
			Axis		axis = Axis::Y;
			BlendMode	blendMode = BlendMode::Undefined;
			HiColor		color = HiColor::Undefined;
			Finalizer_p	finalizer = nullptr;
			Rect		firstBlock;
			Border		frame;
			Gradient	gradient;
			int			layer = -1;
			Border		margin;
			int			markAlpha = 1;
			Border		overflow;
			Border		padding;

			RigidPartXBlueprint	rigidPartX;
			RigidPartYBlueprint rigidPartY;

			pts			spacing = 0;
			std::vector<StateBP>	states;
			
			Surface_p	surface;
		};


		//.____ Creation __________________________________________

		static BlockSkin_p	create(const Blueprint& blueprint);
		static BlockSkin_p  create(Surface * pSurface, Border frame = { 0 } );
		static BlockSkin_p	create(Surface * pSurface, Rect block, Border frame = { 0 } );
	//protected:
		static BlockSkin_p	create(Surface * pSurface, Rect firstBlock, std::initializer_list<State> stateBlocks, Border frame = { 0 }, Axis axis = Axis::Y, int spacing = 0);
		static BlockSkin_p	create(Surface * pSurface, std::initializer_list<State> stateBlocks, Border frame = { 0 }, Axis axis = Axis::Y, int spacing = 0);
	//public:


		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;


		//.____ Appearance _________________________________________________

	protected:
		void		setBlock(Coord ofs);
		void		setBlock(State state, Coord ofs);
		void		setBlocks(std::initializer_list<State> stateBlocks, Axis axis = Axis::Y, int spacing = 0, Coord blockStartOfs = { 0,0 });
		Rect		block(State state) const;

		void		setColor(HiColor tint);
		void		setColor(State state, HiColor tint);
		void		setColor(std::initializer_list< std::tuple<State, HiColor> > stateTints);
		HiColor		color(State state) const;

		void		setGradient(const Gradient& gradient);
		Gradient	gradient() const { return m_gradient; }

		void		setBlendMode(BlendMode mode);
		BlendMode	blendMode() const { return m_blendMode; }

		void		setSurface( Surface * pSurf );
		Surface_p	surface() const { return m_pSurface; }

		void		setBlockSize(Size size);
		Size		blockSize() const { return m_ninePatch.block.size(); }

		void		setFrame(Border frame);
		Border		frame() const { return m_ninePatch.frame; }

		bool		setRigidPartX(pts ofs, pts length, YSections sections);
		bool		setRigidPartY(pts ofs, pts length, XSections sections);
	public:

		//.____ Deprecated ____________________________________________________

		static BlockSkin_p createStatic(Surface * pSurface, Rect block, Border frame = Border(0));
		static BlockSkin_p createClickable(Surface * pSurface, Size blockGeo, Coord blockStartOfs, Size blockPitch, Border blockFrame = Border(0));
		static BlockSkin_p createStaticFromSurface(Surface * pSurface, Border frame = Border(0));

		//.____ Internal ________________________________________________________

		SizeSPX		_minSize(int scale) const override;
		SizeSPX		_defaultSize(int scale) const override;

		SizeSPX		_sizeForContent(const SizeSPX& contentSize, int scale) const override;

		bool		_isOpaque(State state) const override;
		bool		_isOpaque(const RectSPX& rect, const SizeSPX& canvasSize, int scale, State state) const override;

		bool		_markTest(const CoordSPX& ofs, const RectSPX& canvas, int scale, State state,
							float value = 1.f, float value2 = -1.f, int alphaOverride = -1 ) const override;

		void		_render(GfxDevice* pDevice, const RectSPX& canvas, int scale, State state,
							float value = 1.f, float value2 = -1.f, int animPos = 0,
							float* pStateFractions = nullptr) const override;

		RectSPX		_dirtyRect(const RectSPX& canvas, int scale, State newState, State oldState, float newValue = 1.f, float oldValue = 1.f,
							float newValue2 = -1.f, float oldValue2 = -1.f, int newAnimPos = 0, int oldAnimPos = 0,
							float* pNewStateFractions = nullptr, float* pOldStateFractions = nullptr) const override;


	private:

		BlockSkin();
		BlockSkin(Surface * pSurface, Rect block, Border frame);
		BlockSkin(const Blueprint& blueprint);
		~BlockSkin() {};

		void		_updateOpaqueFlags();
		void		_updateUnsetStateBlocks();
		void		_updateUnsetStateColors();

		NinePatch	m_ninePatch;		// Block offset is undefined.
		Surface_p	m_pSurface;
		Gradient	m_gradient;

		BlendMode	m_blendMode = BlendMode::Undefined;

		Bitmask<uint32_t>	m_stateBlockMask = 1;
		Bitmask<uint32_t>	m_stateColorMask = 1;

		Coord		m_stateBlocks[State::IndexAmount];
		HiColor		m_stateColors[State::IndexAmount];
		bool		m_bStateOpaque[State::IndexAmount];
	};


} // namespace wg
#endif //WG_BLOCKSKIN_DOT_H
