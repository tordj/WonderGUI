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
#ifndef WG3_BLOCKSKIN_DOT_H
#define WG3_BLOCKSKIN_DOT_H
#pragma once

#include <wg3_extendedskin.h>
#include <wg3_surface.h>

namespace wg
{


	class BlockSkin;
	typedef	StrongPtr<BlockSkin>	BlockSkin_p;


	class BlockSkin : public ExtendedSkin
	{
		//TODO: Add sanity-checking to all Set-methods.
		//TODO: Optimize rendering based on invisibleSections and opaqueSections!

	public:

		//.____ Creation __________________________________________

		static BlockSkin_p	create();
		static BlockSkin_p 	create(Surface * pSurface, BorderI frame = { 0 } );
		static BlockSkin_p	create(Surface * pSurface, RectI block, BorderI frame = { 0 } );
		static BlockSkin_p	create(Surface * pSurface, RectI firstBlock, const std::initializer_list<State>& stateBlocks, BorderI frame = { 0 }, Orientation orientation = Orientation::Vertical, int spacing = 0);
		static BlockSkin_p	create(Surface * pSurface, const std::initializer_list<State>& stateBlocks, BorderI frame = { 0 }, Orientation orientation = Orientation::Vertical, int spacing = 0);



		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;


		//.____ Appearance _________________________________________________

		virtual void	setBlock(CoordI ofs);
		virtual void	setBlock(State state, CoordI ofs);
		virtual void	setBlocks(const std::initializer_list<State>& stateBlocks, Orientation orientation = Orientation::Vertical, int spacing = 0, CoordI blockStartOfs = { 0,0 });
		virtual RectI	block(State state) const;

		virtual void	setTint(Color tint);
		virtual void	setTint(State state, Color tint);
		virtual void	setTint(const std::initializer_list< std::tuple<State, Color> >& stateTints);
		virtual Color	tint(State state) const;

		void			setBlendMode(BlendMode mode);
		BlendMode		blendMode() const { return m_blendMode; }

		void			setSurface( Surface * pSurf );
		Surface_p		surface() const { return m_pSurface; }

		void			setBlockSize(SizeI size);
		SizeI			blockSize() const { return m_dimensions*4/m_pSurface->qpixPerPoint(); }

		void			setFrame(BorderI frame);
		BorderI			frame() const { return m_frame*4/m_pSurface->qpixPerPoint(); }

		//.____ Misc ____________________________________________________


		bool	isOpaque() const override;
		bool	isOpaque( State state ) const override;

		bool	isStateIdentical( State state, State comparedTo ) const override;

		//.____ Deprecated ____________________________________________________

		static BlockSkin_p createStatic(Surface * pSurface, RectI block, BorderI frame = BorderI(0));
		static BlockSkin_p createClickable(Surface * pSurface, SizeI blockGeo, CoordI blockStartOfs, SizeI blockPitch, BorderI blockFrame = BorderI(0));
		static BlockSkin_p createStaticFromSurface(Surface * pSurface, BorderI frame = BorderI(0));

		//.____ Internal _______________________________________

		void	_render(GfxDevice * pDevice, const RectI& _canvas, State state) const override;

		SizeI	_minSize() const override;
		SizeI	_preferredSize() const override;

		SizeI	_sizeForContent(const SizeI contentSize) const override;

		bool	_markTest(const CoordI& ofs, const RectI& canvas, State state, int opacityTreshold) const override;
		bool	_isOpaque(const RectI& rect, const SizeI& canvasSize, State state) const override;


	private:

		BlockSkin();
		BlockSkin(Surface * pSurface, RectI block, BorderI frame);
		~BlockSkin() {};

		void		_updateOpaqueFlags();
		void		_updateUnsetStateBlocks();
		void		_updateUnsetStateColors();

		Surface_p	m_pSurface;
		SizeI		m_dimensions;   // Pixels
		BorderI		m_frame;        // Pixels

		bool		m_bOpaque;

		BlendMode	m_blendMode = BlendMode::Undefined;

		Bitmask<uint32_t>	m_stateBlockMask = 1;
		Bitmask<uint32_t>	m_stateColorMask = 1;

		CoordI	m_stateBlocks[StateEnum_Nb];
		Color	m_stateColors[StateEnum_Nb];
		bool	m_bStateOpaque[StateEnum_Nb];
	};


} // namespace wg
#endif //WG3_BLOCKSKIN_DOT_H