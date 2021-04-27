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

		//.____ Creation __________________________________________

		static BlockSkin_p	create();
		static BlockSkin_p create(Surface * pSurface, BorderI frame = { 0 } );
		static BlockSkin_p	create(Surface * pSurface, RectI block, BorderI frame = { 0 } );
		static BlockSkin_p	create(Surface * pSurface, RectI firstBlock, std::initializer_list<State> stateBlocks, BorderI frame = { 0 }, Axis axis = Axis::Y, int spacing = 0);
		static BlockSkin_p	create(Surface * pSurface, std::initializer_list<State> stateBlocks, BorderI frame = { 0 }, Axis axis = Axis::Y, int spacing = 0);



		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;


		//.____ Appearance _________________________________________________

		void		setBlock(CoordI ofs);
		void		setBlock(State state, CoordI ofs);
		void		setBlocks(std::initializer_list<State> stateBlocks, Axis axis = Axis::Y, int spacing = 0, CoordI blockStartOfs = { 0,0 });
		RectI		block(State state) const;

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

		void		setBlockSize(SizeI size);
		SizeI		blockSize() const { return m_ninePatch.block*4/m_pSurface->qpixPerPoint(); }

		void		setFrame(BorderI frame);
		BorderI		frame() const { return m_ninePatch.frame*4/m_pSurface->qpixPerPoint(); }

		bool		setRigidPartX(int ofs, int length, YSections sections);
		bool		setRigidPartY(int ofs, int length, XSections sections);


		//.____ Geometry _________________________________________________

		Size		minSize() const override;
		Size		preferredSize() const override;

		Size		sizeForContent(const Size& contentSize) const override;

		//.____ Misc ____________________________________________________

		bool		isOpaque( State state ) const override;
		bool		isOpaque(const Rect& rect, const Size& canvasSize, State state) const override;

		bool		markTest(	const Coord& ofs, const Rect& canvas, State state, int opacityTreshold, 
								float value = 1.f, float value2 = -1.f) const override;

		void		render(	GfxDevice * pDevice, const Rect& canvas, State state, 
							float value = 1.f, float value2 = -1.f, int animPos = 0,
							float* pStateFractions = nullptr) const override;

		Rect	dirtyRect(	const Rect& canvas, State newState, State oldState, float newValue = 1.f, float oldValue = 1.f,
							float newValue2 = -1.f, float oldValue2 = -1.f, int newAnimPos = 0, int oldAnimPos = 0,
							float* pNewStateFractions = nullptr, float* pOldStateFractions = nullptr) const override;


		//.____ Deprecated ____________________________________________________

		static BlockSkin_p createStatic(Surface * pSurface, RectI block, BorderI frame = BorderI(0));
		static BlockSkin_p createClickable(Surface * pSurface, SizeI blockGeo, CoordI blockStartOfs, SizeI blockPitch, BorderI blockFrame = BorderI(0));
		static BlockSkin_p createStaticFromSurface(Surface * pSurface, BorderI frame = BorderI(0));


	private:

		BlockSkin();
		BlockSkin(Surface * pSurface, RectI block, BorderI frame);
		~BlockSkin() {};

		void		_updateOpaqueFlags();
		void		_updateUnsetStateBlocks();
		void		_updateUnsetStateColors();

		NinePatch	m_ninePatch;		// Block offset is undefined.
		Surface_p	m_pSurface;
		Gradient	m_gradient;
		bool		m_bGradient = false;

		BlendMode	m_blendMode = BlendMode::Undefined;

		Bitmask<uint32_t>	m_stateBlockMask = 1;
		Bitmask<uint32_t>	m_stateColorMask = 1;

		CoordI		m_stateBlocks[StateEnum_Nb];
		HiColor		m_stateColors[StateEnum_Nb];
		bool		m_bStateOpaque[StateEnum_Nb];
	};


} // namespace wg
#endif //WG_BLOCKSKIN_DOT_H
