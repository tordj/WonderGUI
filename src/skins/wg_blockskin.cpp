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


	BlockSkin_p BlockSkin::create(Surface * pSurface, Border frame )
	{
		if (pSurface == nullptr || frame.width() >= pSurface->size().w || frame.height() >= pSurface->size().h  )
			return nullptr;

		return BlockSkin_p( new BlockSkin(pSurface, pSurface->size(), frame) );
	}

	BlockSkin_p BlockSkin::create(Surface * pSurface, Rect block, Border frame)
	{
		if (pSurface == nullptr)
			return nullptr;

		Size surfSize = pSurface->size();

		if( frame.width() >= surfSize.w || frame.height() >= surfSize.h ||
			block.x < 0 || block.y < 0 || block.right() > surfSize.w || block.bottom() > surfSize.h )
			return nullptr;

		return BlockSkin_p(new BlockSkin(pSurface, block, frame));
	}

	BlockSkin_p	BlockSkin::create(Surface * pSurface, Rect firstBlock, const std::initializer_list<State>& stateBlocks, Border frame, Orientation orientation, int spacing)
	{
		if (pSurface == nullptr || stateBlocks.size() < 1 )
			return nullptr;

		Size surfSize = pSurface->size();
		int nBlocks = (int) stateBlocks.size();
		Rect blockArea = firstBlock;
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



	BlockSkin_p BlockSkin::createStatic( Surface * pSurface, Rect block, Border frame )
	{
		return create(pSurface, block, frame);
	}

	BlockSkin_p BlockSkin::createEnable( Surface * pSurface, Size blockSize, Coord ofsEnabled, Coord ofsDisabled, Border frame )
	{
		auto p = create(pSurface, { ofsEnabled,blockSize }, frame);
		if( p )
			p->setBlock(StateEnum::Disabled, ofsDisabled);
		return BlockSkin_p(p);
	}

	BlockSkin_p BlockSkin::createClickable( Surface * pSurface, Size blockGeo, Coord blockStartOfs, Size blockPitch, Border blockFrame )
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

	BlockSkin_p BlockSkin::createSelectable( Surface * pSurface, Size blockGeo, Coord blockStartOfs, Size blockPitch, Border blockFrame )
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

	BlockSkin_p BlockSkin::createClickSelectable( Surface * pSurface, Size blockGeo, Coord blockStartOfs, Size blockPitch, Border blockFrame )
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
	BlockSkin_p BlockSkin::createClickSelectableWidget( Surface * pSurface, Size blockGeo, Coord blockStartOfs, Size blockPitch, Border blockFrame )
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


	BlockSkin_p BlockSkin::createStaticFromSurface( Surface * pSurface, Border frame )
	{
		return create(pSurface, frame);
	}

	BlockSkin_p BlockSkin::createEnableFromSurface( Surface * pSurface, int blockSpacing, Border blockFrame )
	{
		BlockSkin * pSkin = new BlockSkin();
		pSkin->setSurface( pSurface );

		Size	sz = Size( (pSurface->width()-blockSpacing)/2, pSurface->height() );

		pSkin->setBlockGeo( sz, blockFrame );
		pSkin->setAllBlocks( Coord(0,0) );
		pSkin->setDisabledBlock( Coord(sz.w+blockSpacing,0) );
		return BlockSkin_p(pSkin);
	}

	BlockSkin_p BlockSkin::createClickableFromSurface( Surface * pSurface, int blockSpacing, Border blockFrame )
	{
		Size	blockSize = Size( (pSurface->width()-blockSpacing*3)/4, pSurface->height() );
		return createClickable( pSurface, blockSize, Coord(0,0), Size(blockSize.w+blockSpacing,0), blockFrame );
	}

	BlockSkin_p BlockSkin::createSelectableFromSurface( Surface * pSurface, int blockSpacing, Border blockFrame )
	{
		Size	blockSize( (pSurface->width()-blockSpacing*2)/3, pSurface->height() );
		return createSelectable( pSurface, blockSize, Coord(0,0), Size(blockSize.w+blockSpacing,0), blockFrame );
	}

	BlockSkin_p BlockSkin::createClickSelectableFromSurface( Surface * pSurface, int blockSpacing, Border blockFrame )
	{
		Size	blockSize( (pSurface->width()-blockSpacing*4)/5, pSurface->height() );
		return createClickSelectable( pSurface, blockSize, Coord(0,0), Size(blockSize.w+blockSpacing,0), blockFrame );
	}

	//____ Constructor ____________________________________________________________

	BlockSkin::BlockSkin()
	{
		m_pSurface = nullptr;
		m_bOpaque = false;

		for (int i = 0; i < StateEnum_Nb; i++)
			m_stateColors[i] = Color::White;
	}

	BlockSkin::BlockSkin(Surface * pSurface, Rect block, Border frame)
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

	bool BlockSkin::setBlockGeo( Size size, Border frame )
	{
		if( size.w <= frame.width() || size.h <= frame.height() )
			return false;

		m_dimensions	= size;
		m_frame			= frame;
		return true;
	}

	//____ setStateBlock() ________________________________________________________

	void BlockSkin::setStateBlock( StateEnum state, const Coord& ofs )
	{
		setBlock(state, ofs);
	}

	//____ setAllBlocks() _________________________________________________________

	void BlockSkin::setAllBlocks( const Coord& ofs )
	{
		setBlock(ofs);
	}

	//____ setDisabledBlock() _____________________________________________________

	void BlockSkin::setDisabledBlock( const Coord& ofs )
	{
		setBlock(StateEnum::Disabled, ofs);
	}

	//____ setHoveredBlocks() _____________________________________________________

	void BlockSkin::setHoveredBlocks( const Coord& ofs )
	{
		setBlock(StateEnum::Hovered, ofs);
	}

	//____ setPressedBlocks() _____________________________________________________

	void BlockSkin::setPressedBlocks( const Coord& ofs )
	{
		setBlock(StateEnum::Pressed, ofs);
	}

	//____ setSelectedBlocks() ____________________________________________________

	void BlockSkin::setSelectedBlocks( const Coord& ofs )
	{
		setBlock(StateEnum::Selected, ofs);
	}

	//____ setBlock() _____________________________________________________________

	void BlockSkin::setBlock(Coord ofs)
	{
		m_stateBlocks[0] = ofs;
		m_stateBlockMask = 1;

		_updateUnsetStateBlocks();
	}

	void BlockSkin::setBlock(State state, Coord ofs)
	{
		int i = _stateToIndex(state);

		m_stateBlocks[i] = ofs;
		m_stateBlockMask.setBit(i);
		_updateUnsetStateBlocks();
	}

	//____ setBlocks() ________________________________________________________

	void BlockSkin::setBlocks(const std::initializer_list<State>& stateBlocks, Orientation orientation, int spacing, Coord blockStartOfs )
	{
		Coord pitch = orientation == Orientation::Horizontal ? Coord(m_dimensions.w + spacing, 0 ) : Coord(0, m_dimensions.h + spacing);

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

	Rect BlockSkin::block(State state) const
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

	void BlockSkin::setBlockSize(Size size)
	{
		m_dimensions = size;
	}

	//____ setFrame() ____________________________________________________________

	void BlockSkin::setFrame(Border frame)
	{
		m_frame = frame;
	}

	//____ render() _______________________________________________________________

	void BlockSkin::render( GfxDevice * pDevice, const Rect& _canvas, State state ) const
	{
		if( !m_pSurface )
			return;

		BlendMode savedBlendMode = BlendMode::Undefined;
		if (m_blendMode != BlendMode::Undefined)
		{
			savedBlendMode = pDevice->blendMode();
			pDevice->setBlendMode(m_blendMode);
		}

		Coord blockOfs = m_stateBlocks[_stateToIndex(state)];
		pDevice->setBlitSource(m_pSurface);
		pDevice->blitNinePatch(_canvas, toPixels(m_frame), { blockOfs,m_dimensions }, m_frame );

		if (m_blendMode != BlendMode::Undefined)
			pDevice->setBlendMode(savedBlendMode);
	}

	//____ minSize() ______________________________________________________________

	SizeQ BlockSkin::minSize() const
	{
		SizeQ content = ExtendedSkin::minSize();
		SizeQ frame = pixelAligned(m_frame);
		return SizeQ::max( content, frame );
	}

	//____ preferredSize() ________________________________________________________

	SizeQ BlockSkin::preferredSize() const
	{
		SizeQ content = ExtendedSkin::minSize();
		SizeQ frame = pixelAligned(m_frame);
		return SizeQ::max(content, frame);
	}

	//____ sizeForContent() _______________________________________________________

	SizeQ BlockSkin::sizeForContent( const SizeQ contentSize ) const
	{
		SizeQ sz = ExtendedSkin::sizeForContent(contentSize);
		SizeQ min = pixelAligned(m_frame);

		return SizeQ::max(sz, min);
	}

	//____ markTest() _____________________________________________________________

	bool BlockSkin::markTest( const Coord& _ofs, const Rect& canvas, State state, int opacityTreshold ) const
	{
		Coord srcOfs = m_stateBlocks[_stateToIndex(state)];
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

	bool BlockSkin::isOpaque( const Rect& rect, const Size& canvasSize, State state ) const
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
