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

#include <wg_blockskin.h>
#include <wg_gfxdevice.h>
#include <wg_geo.h>
#include <wg_util.h>

namespace wg
{

	using namespace Util;

	const char BlockSkin::CLASSNAME[] = {"BlockSkin"};

	//____ create() _______________________________________________________________

	BlockSkin_p BlockSkin::create()
	{
		return BlockSkin_p(new BlockSkin());
	}


	BlockSkin_p BlockSkin::create(Surface * pSurface, BorderI frame )
	{
		if (pSurface == nullptr || frame.width() >= pSurface->size().w || frame.height() >= pSurface->size().h  )
			return nullptr;

		return BlockSkin_p( new BlockSkin(pSurface, pSurface->size(), frame) );
	}

	BlockSkin_p BlockSkin::create(Surface * pSurface, RectI block, BorderI frame)
	{
		if (pSurface == nullptr)
			return nullptr;

		SizeI surfSize = pSurface->size();

		if( frame.width() >= surfSize.w || frame.height() >= surfSize.h ||
			block.x < 0 || block.y < 0 || block.right() > surfSize.w || block.bottom() > surfSize.h )
			return nullptr;

		return BlockSkin_p(new BlockSkin(pSurface, block, frame));
	}

	BlockSkin_p	BlockSkin::create(Surface * pSurface, RectI firstBlock, const std::initializer_list<State>& stateBlocks, BorderI frame, Orientation orientation, int spacing)
	{
		if (pSurface == nullptr || stateBlocks.size() < 1 )
			return nullptr;

		SizeI surfSize = pSurface->size();
		int nBlocks = (int) stateBlocks.size();
		RectI blockArea = firstBlock;
		if (orientation == Orientation::Horizontal)
			blockArea.w += (nBlocks-1) * (firstBlock.w + spacing);
		else
			blockArea.h += (nBlocks - 1) * (firstBlock.h + spacing);

		if (frame.width() >= firstBlock.w || frame.height() >= firstBlock.h ||
			blockArea.x < 0 || blockArea.y < 0 || blockArea.right() > surfSize.w || blockArea.bottom() > surfSize.h)
			return nullptr;

		BlockSkin * p = new BlockSkin(pSurface, firstBlock, frame);

		p->setBlocks(stateBlocks, orientation, spacing, firstBlock.pos());
		return BlockSkin_p(p);
	}



	BlockSkin_p BlockSkin::createStatic( Surface * pSurface, RectI block, BorderI frame )
	{
		return create(pSurface, block, frame);
	}

	BlockSkin_p BlockSkin::createEnable( Surface * pSurface, SizeI blockSize, CoordI ofsEnabled, CoordI ofsDisabled, BorderI frame )
	{
		auto p = create(pSurface, { ofsEnabled,blockSize }, frame);
		if( p )
			p->setBlock(StateEnum::Disabled, ofsDisabled);
		return BlockSkin_p(p);
	}

	BlockSkin_p BlockSkin::createClickable( Surface * pSurface, SizeI blockGeo, CoordI blockStartOfs, SizeI blockPitch, BorderI blockFrame )
	{
		auto p = create(pSurface, blockGeo, blockFrame);
		if (p)
		{
			Orientation o;
			int spacing;
			if (blockPitch.w > 0)
			{
				o = Orientation::Horizontal;
				spacing = blockPitch.w - blockGeo.w;
			}
			else
			{
				o = Orientation::Vertical;
				spacing = blockPitch.h - blockGeo.h;
			}

			p->setBlocks({ StateEnum::Normal, StateEnum::Hovered, StateEnum::Pressed, StateEnum::Disabled }, o, spacing, blockStartOfs);
		}
		return BlockSkin_p(p);
	}

	BlockSkin_p BlockSkin::createSelectable( Surface * pSurface, SizeI blockGeo, CoordI blockStartOfs, SizeI blockPitch, BorderI blockFrame )
	{
		auto p = create(pSurface, blockGeo, blockFrame);
		if (p)
		{
			Orientation o;
			int spacing;
			if (blockPitch.w > 0)
			{
				o = Orientation::Horizontal;
				spacing = blockPitch.w - blockGeo.w;
			}
			else
			{
				o = Orientation::Vertical;
				spacing = blockPitch.h - blockGeo.h;
			}

			p->setBlocks({ StateEnum::Normal, StateEnum::Selected, StateEnum::Disabled }, o, spacing, blockStartOfs);
		}
		return BlockSkin_p(p);
	}

	BlockSkin_p BlockSkin::createClickSelectable( Surface * pSurface, SizeI blockGeo, CoordI blockStartOfs, SizeI blockPitch, BorderI blockFrame )
	{
		auto p = create(pSurface, blockGeo, blockFrame);
		if (p)
		{
			Orientation o;
			int spacing;
			if (blockPitch.w > 0)
			{
				o = Orientation::Horizontal;
				spacing = blockPitch.w - blockGeo.w;
			}
			else
			{
				o = Orientation::Vertical;
				spacing = blockPitch.h - blockGeo.h;
			}

			p->setBlocks({ StateEnum::Normal, StateEnum::Hovered, StateEnum::Selected, StateEnum::SelectedHovered, StateEnum::Disabled }, o, spacing, blockStartOfs);
		}
		return BlockSkin_p(p);
	}

	/** Usable for state button etc, where a widget is normal/hovered/pressed/disabled as well as selected/unselected
	*/
	BlockSkin_p BlockSkin::createClickSelectableWidget( Surface * pSurface, SizeI blockGeo, CoordI blockStartOfs, SizeI blockPitch, BorderI blockFrame )
	{
		auto p = create(pSurface, blockGeo, blockFrame);
		if (p)
		{
			Orientation o;
			int spacing;
			if (blockPitch.w > 0)
			{
				o = Orientation::Horizontal;
				spacing = blockPitch.w - blockGeo.w;
			}
			else
			{
				o = Orientation::Vertical;
				spacing = blockPitch.h - blockGeo.h;
			}

			p->setBlocks({ StateEnum::Normal, StateEnum::Hovered, StateEnum::Pressed, StateEnum::Selected, StateEnum::SelectedHovered, StateEnum::SelectedPressed, StateEnum::Disabled, StateEnum::DisabledSelected }, o, spacing, blockStartOfs);
		}
		return BlockSkin_p(p);
	}


	BlockSkin_p BlockSkin::createStaticFromSurface( Surface * pSurface, BorderI frame )
	{
		return create(pSurface, frame);
	}

	BlockSkin_p BlockSkin::createEnableFromSurface( Surface * pSurface, int blockSpacing, BorderI blockFrame )
	{
		BlockSkin * pSkin = new BlockSkin();
		pSkin->setSurface( pSurface );

		SizeI	sz = SizeI( (pSurface->width()-blockSpacing)/2, pSurface->height() );

		pSkin->setBlockGeo( sz, blockFrame );
		pSkin->setAllBlocks( CoordI(0,0) );
		pSkin->setDisabledBlock( CoordI(sz.w+blockSpacing,0) );
		return BlockSkin_p(pSkin);
	}

	BlockSkin_p BlockSkin::createClickableFromSurface( Surface * pSurface, int blockSpacing, BorderI blockFrame )
	{
		SizeI	blockSize = SizeI( (pSurface->width()-blockSpacing*3)/4, pSurface->height() );
		return createClickable( pSurface, blockSize, CoordI(0,0), SizeI(blockSize.w+blockSpacing,0), blockFrame );
	}

	BlockSkin_p BlockSkin::createSelectableFromSurface( Surface * pSurface, int blockSpacing, BorderI blockFrame )
	{
		SizeI	blockSize( (pSurface->width()-blockSpacing*2)/3, pSurface->height() );
		return createSelectable( pSurface, blockSize, CoordI(0,0), SizeI(blockSize.w+blockSpacing,0), blockFrame );
	}

	BlockSkin_p BlockSkin::createClickSelectableFromSurface( Surface * pSurface, int blockSpacing, BorderI blockFrame )
	{
		SizeI	blockSize( (pSurface->width()-blockSpacing*4)/5, pSurface->height() );
		return createClickSelectable( pSurface, blockSize, CoordI(0,0), SizeI(blockSize.w+blockSpacing,0), blockFrame );
	}

	//____ Constructor ____________________________________________________________

	BlockSkin::BlockSkin()
	{
		m_pSurface = nullptr;
		m_bOpaque = false;

		for (int i = 0; i < StateEnum_Nb; i++)
			m_stateColors[i] = Color::White;
	}

	BlockSkin::BlockSkin(Surface * pSurface, RectI block, BorderI frame)
	{
		m_pSurface			= pSurface;
		m_dimensions		= block.size();
		m_frame				= frame;
		m_bOpaque			= pSurface->isOpaque();

		for( int i = 0 ; i < StateEnum_Nb ; i++ )
		{
			m_stateBlocks[i] = block.pos();
			m_stateColors[i] = Color::White;
		}
	}


	//____ isInstanceOf() _________________________________________________________

	bool BlockSkin::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return ExtendedSkin::isInstanceOf(pClassName);
	}

	//____ className() ____________________________________________________________

	const char * BlockSkin::className( void ) const
	{
		return CLASSNAME;
	}

	//____ cast() _________________________________________________________________

	BlockSkin_p BlockSkin::cast( Object * pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return BlockSkin_p( static_cast<BlockSkin*>(pObject) );

		return 0;
	}


	//____ setBlockGeo() __________________________________________________________

	bool BlockSkin::setBlockGeo( SizeI size, BorderI frame )
	{
		if( size.w <= frame.width() || size.h <= frame.height() )
			return false;

		m_dimensions	= size;
		m_frame			= frame;
		return true;
	}

	//____ setStateBlock() ________________________________________________________

	void BlockSkin::setStateBlock( StateEnum state, const CoordI& ofs )
	{
		setBlock(state, ofs);
	}

	//____ setAllBlocks() _________________________________________________________

	void BlockSkin::setAllBlocks( const CoordI& ofs )
	{
		setBlock(ofs);
	}

	//____ setDisabledBlock() _____________________________________________________

	void BlockSkin::setDisabledBlock( const CoordI& ofs )
	{
		setBlock(StateEnum::Disabled, ofs);
	}

	//____ setHoveredBlocks() _____________________________________________________

	void BlockSkin::setHoveredBlocks( const CoordI& ofs )
	{
		setBlock(StateEnum::Hovered, ofs);
	}

	//____ setPressedBlocks() _____________________________________________________

	void BlockSkin::setPressedBlocks( const CoordI& ofs )
	{
		setBlock(StateEnum::Pressed, ofs);
	}

	//____ setSelectedBlocks() ____________________________________________________

	void BlockSkin::setSelectedBlocks( const CoordI& ofs )
	{
		setBlock(StateEnum::Selected, ofs);
	}

	//____ setBlock() _____________________________________________________________

	void BlockSkin::setBlock(CoordI ofs)
	{
		m_stateBlocks[0] = ofs;
		m_stateBlockMask = 1;

		_updateUnsetStateBlocks();
	}

	void BlockSkin::setBlock(State state, CoordI ofs)
	{
		int i = _stateToIndex(state);

		m_stateBlocks[i] = ofs;
		m_stateBlockMask.setBit(i);
		_updateUnsetStateBlocks();
	}

	//____ setBlocks() ________________________________________________________

	void BlockSkin::setBlocks(const std::initializer_list<State>& stateBlocks, Orientation orientation, int spacing, CoordI blockStartOfs )
	{
		CoordI pitch = orientation == Orientation::Horizontal ? CoordI(m_dimensions.w + spacing, 0 ) : CoordI(0, m_dimensions.h + spacing);

		int ofs = 0;
		for (StateEnum state : stateBlocks)
		{
			int index = _stateToIndex(state);
			m_stateBlockMask.setBit(index);
			m_stateBlocks[index] = blockStartOfs + pitch * ofs;
			ofs++;
		}
		_updateUnsetStateBlocks();
	}

	//____ block() ____________________________________________________________

	RectI BlockSkin::block(State state) const
	{
		return { m_stateBlocks[_stateToIndex(state)], m_dimensions };
	}

	//____ setTint() __________________________________________________________

	void BlockSkin::setTint(Color tint)
	{
		m_stateColors[0] = tint;
		m_stateColorMask = 1;

		_updateUnsetStateColors();
		_updateOpaqueFlags();
	}

	void BlockSkin::setTint(State state, Color tint)
	{
		int i = _stateToIndex(state);

		m_stateColors[i] = tint;
		m_stateColorMask.setBit(i);
		_updateUnsetStateColors();
		_updateOpaqueFlags();
	}

	void BlockSkin::setTint(const std::initializer_list< std::tuple<State, Color> >& stateTints)
	{
		for (auto& state : stateTints)
		{
			int i = _stateToIndex(std::get<0>(state));
			m_stateColorMask.setBit(i);
			m_stateColors[i] = std::get<1>(state);
		}

		_updateUnsetStateColors();
		_updateOpaqueFlags();
	}

	//____ tint() _____________________________________________________________

	Color BlockSkin::tint(State state) const
	{
		return m_stateColors[_stateToIndex(state)];
	}



	//____ setBlendMode() _____________________________________________________

	void BlockSkin::setBlendMode(BlendMode mode)
	{
		m_blendMode = mode;
		_updateOpaqueFlags();
	}

	//____ setSurface() _______________________________________________________

	void BlockSkin::setSurface(Surface * pSurf)
	{
		m_pSurface = pSurf;
		_updateOpaqueFlags();
	}

	//____ setBlockSize() _____________________________________________________

	void BlockSkin::setBlockSize(SizeI size)
	{
		m_dimensions = size;
	}

	//____ setFrame() ____________________________________________________________

	void BlockSkin::setFrame(BorderI frame)
	{
		m_frame = frame;
	}

	//____ render() _______________________________________________________________

	void BlockSkin::render( GfxDevice * pDevice, const RectI& _canvas, State state ) const
	{
		if( !m_pSurface )
			return;

		BlendMode savedBlendMode = BlendMode::Undefined;
		if (m_blendMode != BlendMode::Undefined)
		{
			savedBlendMode = pDevice->blendMode();
			pDevice->setBlendMode(m_blendMode);
		}

		CoordI blockOfs = m_stateBlocks[_stateToIndex(state)];
		pDevice->setBlitSource(m_pSurface);
		pDevice->blitNinePatch(_canvas, toPixels(m_frame), { blockOfs,m_dimensions }, m_frame );

		if (m_blendMode != BlendMode::Undefined)
			pDevice->setBlendMode(savedBlendMode);
	}

	//____ minSize() ______________________________________________________________

	Size BlockSkin::minSize() const
	{
		Size content = ExtendedSkin::minSize();
		Size frame = pixelAligned(m_frame);
		return Size::max( content, frame );
	}

	//____ preferredSize() ________________________________________________________

	Size BlockSkin::preferredSize() const
	{
		Size content = ExtendedSkin::minSize();
		Size frame = pixelAligned(m_frame);
		return Size::max(content, frame);
	}

	//____ sizeForContent() _______________________________________________________

	Size BlockSkin::sizeForContent( const Size contentSize ) const
	{
		Size sz = ExtendedSkin::sizeForContent(contentSize);
		Size min = pixelAligned(m_frame);

		return Size::max(sz, min);
	}

	//____ markTest() _____________________________________________________________

	bool BlockSkin::markTest( const CoordI& _ofs, const RectI& canvas, State state, int opacityTreshold ) const
	{
		CoordI srcOfs = m_stateBlocks[_stateToIndex(state)];
		return markTestNinePatch(_ofs, m_pSurface, { srcOfs,m_dimensions }, canvas, opacityTreshold, m_frame);
	}

	//____ isOpaque() _____________________________________________________________

	bool BlockSkin::isOpaque() const
	{
		return m_bOpaque;
	}

	bool BlockSkin::isOpaque( State state ) const
	{
		return m_bStateOpaque[_stateToIndex(state)];
	}

	bool BlockSkin::isOpaque( const RectI& rect, const SizeI& canvasSize, State state ) const
	{
		return m_bStateOpaque[_stateToIndex(state)];
	}

	//____ isStateIdentical() _____________________________________________________

	bool BlockSkin::isStateIdentical( State state, State comparedTo ) const
	{
		int i1 = _stateToIndex(state);
		int i2 = _stateToIndex(comparedTo);

		return ( m_stateBlocks[i1] == m_stateBlocks[i2] && ExtendedSkin::isStateIdentical(state,comparedTo) );
	}

	//____ _updateOpaqueFlags() ________________________________________________

	void BlockSkin::_updateOpaqueFlags()
	{
		bool bTintDecides = false;

		if (!m_pSurface)
			m_bOpaque = false;
		else if (m_blendMode == BlendMode::Replace)
			m_bOpaque = true;
		else if (m_blendMode == BlendMode::Blend || m_blendMode == BlendMode::Undefined )		// Assumes that incoming BlendMide is Blend.
		{
			m_bOpaque = m_pSurface->isOpaque();
			bTintDecides = m_bOpaque;
		}
		else
			m_bOpaque = false;

		if (bTintDecides)
		{
			for (int i = 0; i < StateEnum_Nb; i++)
				m_bStateOpaque[i] = m_stateColors[i].a == 255;
		}
		else
		{
			for (int i = 0; i < StateEnum_Nb; i++)
				m_bStateOpaque[i] = m_bOpaque;
		}
	}

	//____ _updateUnsetStateBlocks() _______________________________________________

	void BlockSkin::_updateUnsetStateBlocks()
	{
		for (int i = 0; i < StateEnum_Nb; i++)
		{
			if (!m_stateBlockMask.bit(i))
			{
				int bestAlternative = bestStateIndexMatch(i, m_stateBlockMask);
				m_stateBlocks[i] = m_stateBlocks[bestAlternative];
			}
		}
	}

	//____ _updateUnsetStateColors() _______________________________________________

	void BlockSkin::_updateUnsetStateColors()
	{
		for (int i = 0; i < StateEnum_Nb; i++)
		{
			if (!m_stateColorMask.bit(i))
			{
				int bestAlternative = bestStateIndexMatch(i, m_stateColorMask);
				m_stateColors[i] = m_stateColors[bestAlternative];
			}
		}
	}


} // namespace wg
