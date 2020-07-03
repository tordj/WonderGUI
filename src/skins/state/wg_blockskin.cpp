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

	const TypeInfo BlockSkin::TYPEINFO = { "BlockSkin", &StateSkin::TYPEINFO };

	//____ create() _______________________________________________________________

	BlockSkin_p BlockSkin::create()
	{
		return BlockSkin_p(new BlockSkin());
	}


	BlockSkin_p BlockSkin::create(Surface * pSurface, BorderI frame )
	{
		BorderI pixelFrame = (frame*pSurface->qpixPerPoint()) / 4;

		if (pSurface == nullptr || pixelFrame.width() >= pSurface->size().w || pixelFrame.height() >= pSurface->size().h  )
			return nullptr;

		return BlockSkin_p( new BlockSkin(pSurface, pSurface->size(), pixelFrame) );
	}

	BlockSkin_p BlockSkin::create(Surface * pSurface, RectI block, BorderI frame)
	{
		if (pSurface == nullptr)
			return nullptr;

		SizeI surfSize = pSurface->size();
		BorderI pixelFrame = frame*pSurface->qpixPerPoint() / 4;
		RectI pixelBlock = block*pSurface->qpixPerPoint() / 4;

		if( pixelFrame.width() >= surfSize.w || pixelFrame.height() >= surfSize.h ||
			pixelBlock.x < 0 || pixelBlock.y < 0 || pixelBlock.right() > surfSize.w || pixelBlock.bottom() > surfSize.h )
			return nullptr;

		return BlockSkin_p(new BlockSkin(pSurface, pixelBlock, pixelFrame));
	}

	BlockSkin_p	BlockSkin::create(Surface * pSurface, RectI _firstBlock, const std::initializer_list<State>& stateBlocks, BorderI _frame, Axis axis, int _spacing)
	{
		if (pSurface == nullptr || stateBlocks.size() < 1 )
			return nullptr;

		// Get pixel values

		SizeI surfSize = pSurface->size();
		RectI firstBlock = _firstBlock*pSurface->qpixPerPoint() / 4;
		BorderI frame = _frame*pSurface->qpixPerPoint() / 4;
		int   spacing = _spacing*pSurface->qpixPerPoint() / 4;

		// Check so all blocks fit

		int nBlocks = (int) stateBlocks.size();
		RectI blockArea = firstBlock;
		if (axis == Axis::X)
			blockArea.w += (nBlocks-1) * (firstBlock.w + spacing);
		else
			blockArea.h += (nBlocks - 1) * (firstBlock.h + spacing);

		if (frame.width() >= firstBlock.w || frame.height() >= firstBlock.h ||
			blockArea.x < 0 || blockArea.y < 0 || blockArea.right() > surfSize.w || blockArea.bottom() > surfSize.h)
			return nullptr;

		// Create the skin

		BlockSkin * p = new BlockSkin(pSurface, firstBlock, frame);

		p->setBlocks(stateBlocks, axis, _spacing, _firstBlock.pos());
		return BlockSkin_p(p);
	}

	BlockSkin_p BlockSkin::create(Surface * pSurface, const std::initializer_list<State>& stateBlocks, BorderI _frame, Axis axis, int _spacing)
	{
		if (pSurface == nullptr || stateBlocks.size() < 1)
			return nullptr;


		SizeI	surfSize = pSurface->size();
		int		nBlocks = stateBlocks.size();
		BorderI frame = _frame * pSurface->qpixPerPoint() / 4;
		int   spacing = _spacing * pSurface->qpixPerPoint() / 4;

		// Check so blocks fit evenly

		int		length = axis == Axis::X ? surfSize.w : surfSize.h;

		if ((length - (nBlocks - 1) * spacing) % nBlocks != 0)
			return nullptr;

		// Create the skin

		int blockLen = (length - (nBlocks - 1) * spacing) / nBlocks;

		SizeI blockSize = axis == Axis::X ? SizeI(blockLen, surfSize.h) : SizeI(surfSize.w, blockLen);

		BlockSkin * p = new BlockSkin(pSurface, blockSize, frame);
		p->setBlocks(stateBlocks, axis, spacing, CoordI(0, 0));
		return BlockSkin_p(p);
	}

	BlockSkin_p BlockSkin::createStatic( Surface * pSurface, RectI block, BorderI frame )
	{
		return create(pSurface, block, frame);
	}


	BlockSkin_p BlockSkin::createClickable( Surface * pSurface, SizeI blockGeo, CoordI blockStartOfs, SizeI blockPitch, BorderI blockFrame )
	{
		auto p = create(pSurface, blockGeo, blockFrame);
		if (p)
		{
			Axis o;
			int spacing;
			if (blockPitch.w > 0)
			{
				o = Axis::X;
				spacing = blockPitch.w - blockGeo.w;
			}
			else
			{
				o = Axis::Y;
				spacing = blockPitch.h - blockGeo.h;
			}

			p->setBlocks({ StateEnum::Normal, StateEnum::Hovered, StateEnum::Pressed, StateEnum::Disabled }, o, spacing, blockStartOfs);
		}
		return BlockSkin_p(p);
	}

	BlockSkin_p BlockSkin::createStaticFromSurface( Surface * pSurface, BorderI frame )
	{
		return create(pSurface, frame);
	}

	//____ constructor ____________________________________________________________

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



	//____ typeInfo() _________________________________________________________

	const TypeInfo& BlockSkin::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setBlock() _____________________________________________________________

	void BlockSkin::setBlock(CoordI ofs)
	{
		ofs = ofs*m_pSurface->qpixPerPoint() / 4;

		m_stateBlocks[0] = ofs;
		m_stateBlockMask = 1;

		_updateUnsetStateBlocks();
	}

	void BlockSkin::setBlock(State state, CoordI ofs)
	{
		ofs = ofs*m_pSurface->qpixPerPoint() / 4;

		int i = _stateToIndex(state);

		m_stateBlocks[i] = ofs;
		m_stateBlockMask.setBit(i);
		_updateUnsetStateBlocks();
	}

	//____ setBlocks() ________________________________________________________

	void BlockSkin::setBlocks(const std::initializer_list<State>& stateBlocks, Axis axis, int _spacing, CoordI _blockStartOfs )
	{
		CoordI blockStartOfs = _blockStartOfs*m_pSurface->qpixPerPoint() / 4;
		int spacing = _spacing*m_pSurface->qpixPerPoint() / 4;

		CoordI pitch = axis == Axis::X ? CoordI(m_dimensions.w + spacing, 0 ) : CoordI(0, m_dimensions.h + spacing);

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
		return { m_stateBlocks[_stateToIndex(state)], m_dimensions*4/m_pSurface->qpixPerPoint() };
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
		m_dimensions = size*m_pSurface->qpixPerPoint() / 4;
	}

	//____ setFrame() ____________________________________________________________

	void BlockSkin::setFrame(BorderI frame)
	{
		m_frame = frame*m_pSurface->qpixPerPoint() / 4;
	}

	//____ render() _______________________________________________________________

	void BlockSkin::render( GfxDevice * pDevice, const Rect& canvas, State state, float fraction ) const
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
		pDevice->blitNinePatch( canvas.px(), pointsToPixels(m_frame*4/m_pSurface->qpixPerPoint()), { blockOfs,m_dimensions }, m_frame );

		if (m_blendMode != BlendMode::Undefined)
			pDevice->setBlendMode(savedBlendMode);
	}

	//____ minSize() ______________________________________________________________

	Size BlockSkin::minSize() const
	{
		Size content = Border(m_contentPadding).aligned();
		Size frame = Border(m_frame*4/m_pSurface->qpixPerPoint()).aligned();
		return Size::max( content, frame );
	}

	//____ preferredSize() ________________________________________________________

	Size BlockSkin::preferredSize() const
	{
        //This takes the scale of the surface into account
        // Preferred size is to map each point of the surface to a point of the skinarea.
        
        return Size((m_dimensions*4)/m_pSurface->qpixPerPoint());
	}

	//____ sizeForContent() _______________________________________________________

	Size BlockSkin::sizeForContent( const Size& contentSize ) const
	{
		Size sz = StateSkin::sizeForContent(contentSize);
		Size min = Border(m_frame*4/m_pSurface->qpixPerPoint()).aligned();

		return Size::max(sz, min);
	}

	//____ markTest() _____________________________________________________________

	bool BlockSkin::markTest( const Coord& _ofs, const Rect& canvas, State state, int opacityTreshold, float fraction) const
	{
		CoordI srcOfs = m_stateBlocks[_stateToIndex(state)];
		return markTestNinePatch(_ofs, m_pSurface, { srcOfs,m_dimensions }, canvas, opacityTreshold, m_frame);
	}

	//____ isOpaque() _____________________________________________________________

	bool BlockSkin::isOpaque( State state ) const
	{
		return m_bStateOpaque[_stateToIndex(state)];
	}

	bool BlockSkin::isOpaque( const Rect& rect, const Size& canvasSize, State state ) const
	{
		return m_bStateOpaque[_stateToIndex(state)];
	}

	//____ isStateIdentical() _____________________________________________________

	bool BlockSkin::isStateIdentical( State state, State comparedTo, float fraction) const
	{
		int i1 = _stateToIndex(state);
		int i2 = _stateToIndex(comparedTo);

		return ( m_stateBlocks[i1] == m_stateBlocks[i2] && StateSkin::isStateIdentical(state,comparedTo) );
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
