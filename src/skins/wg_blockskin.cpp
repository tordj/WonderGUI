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
		int nBlocks = stateBlocks.size();
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
	}

	BlockSkin::BlockSkin(Surface * pSurface, Rect block, Border frame)
	{
		m_pSurface			= pSurface;
		m_dimensions		= block.size();
		m_frame				= frame;
		m_bOpaque			= pSurface->isOpaque();

		for( int i = 0 ; i < StateEnum_Nb ; i++ )
			m_stateBlocks[i] = block.size();
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

		_updateUnsetStates();
	}

	void BlockSkin::setBlock(State state, Coord ofs)
	{
		int i = _stateToIndex(state);

		m_stateBlocks[i] = ofs;
		m_stateBlockMask.setBit(i);
		_updateUnsetStates();
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
		_updateUnsetStates();
	}

	//____ block() ____________________________________________________________

	Rect BlockSkin::block(State state) const
	{
		return { m_stateBlocks[_stateToIndex(state)], m_dimensions };
	}

	//____ setBlendMode() _____________________________________________________

	void BlockSkin::setBlendMode(BlendMode mode)
	{
		m_blendMode = mode;
		_updateOpaqueFlag();
	}

	//____ setSurface() _______________________________________________________

	void BlockSkin::setSurface(Surface * pSurf)
	{
		m_pSurface = pSurf;
		_updateOpaqueFlag();
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
	
	void BlockSkin::render( GfxDevice * pDevice, const Rect& _canvas, State state, const Rect& _clip ) const
	{
		if( !m_pSurface )
			return;
	
		Coord blockOfs = m_stateBlocks[_stateToIndex(state)];
		pDevice->clipBlitNinePatch(_clip, m_pSurface, { blockOfs,m_dimensions }, m_frame, _canvas, m_frame);
	}
	
	//____ minSize() ______________________________________________________________
	
	Size BlockSkin::minSize() const
	{
		Size content = ExtendedSkin::minSize();
		Size frame = m_frame.size();
		return Size( wg::max(content.w, frame.w), wg::max(content.h, frame.h) );
	}
	
	//____ preferredSize() ________________________________________________________
	
	Size BlockSkin::preferredSize() const
	{
		Size sz = ExtendedSkin::preferredSize();
		return Size( wg::max(m_dimensions.w,sz.w),wg::max(m_dimensions.h,sz.h) );
	}
	
	//____ sizeForContent() _______________________________________________________
	
	Size BlockSkin::sizeForContent( const Size contentSize ) const
	{
		Size sz = ExtendedSkin::sizeForContent(contentSize);
		Size min = m_frame.size();
	
		return Size( wg::max(sz.w,min.w), wg::max(sz.h,min.h) );
	}
	
	//____ markTest() _____________________________________________________________
	
	bool BlockSkin::markTest( const Coord& _ofs, const Rect& canvas, State state, int opacityTreshold ) const
	{
		if( !m_pSurface || !canvas.contains(_ofs) )
			return false;
	
		int alpha;
		if( isOpaque( state ) )
			alpha = 255;
		else
		{
			Coord ofs = _ofs - canvas.pos();
	
			// Determine in which section the cordinate is (0-2 for x and y).
	
			int	xSection = 0;
			int ySection = 0;
	
			if( ofs.x >= canvas.w - m_frame.right )
				xSection = 2;
			else if( ofs.x > m_frame.left )
				xSection = 1;
	
			if( ofs.y >= canvas.h - m_frame.bottom )
				ySection = 2;
			else if( ofs.y > m_frame.top )
				ySection = 1;
	
			// Convert ofs.x to X-offset in bitmap, taking stretch/tile section into account.
	
			if( xSection == 2 )
			{
				ofs.x = m_dimensions.w - (canvas.w - ofs.x);
			}
			else if( xSection == 1 )
			{
				int tileAreaWidth = m_dimensions.w - m_frame.width();
		
				double screenWidth = canvas.w - m_frame.width();	// Width of stretch-area on screen.
				ofs.x = (int) ((ofs.x-m_frame.left)/screenWidth * tileAreaWidth + m_frame.left);
			}
	
	
			// Convert ofs.y to Y-offset in bitmap, taking stretch/tile section into account.
	
			if( ySection == 2 )
			{
				ofs.y = m_dimensions.h - (canvas.h - ofs.y);
			}
			else if( ySection == 1 )
			{
				int tileAreaHeight = m_dimensions.h - m_frame.height();
	
				double screenHeight = canvas.h - m_frame.height();	// Height of stretch-area on screen.
				ofs.y = (int) ((ofs.y-m_frame.top)/screenHeight * tileAreaHeight + m_frame.top);
			}
	
			Coord srcOfs = m_stateBlocks[_stateToIndex(state)];
	
			alpha = m_pSurface->alpha(srcOfs.x+ofs.x, srcOfs.y+ofs.y);
		}
	
		return ( alpha >= opacityTreshold);
	}
	
	//____ isOpaque() _____________________________________________________________
	
	bool BlockSkin::isOpaque() const
	{
		return m_bOpaque;
	}
	
	bool BlockSkin::isOpaque( State state ) const
	{
		return m_bOpaque;	
	}
	
	bool BlockSkin::isOpaque( const Rect& rect, const Size& canvasSize, State state ) const
	{
		return m_bOpaque;
	}
	
	//____ isStateIdentical() _____________________________________________________
	
	bool BlockSkin::isStateIdentical( State state, State comparedTo ) const
	{
		int i1 = _stateToIndex(state);
		int i2 = _stateToIndex(comparedTo);
	
		return ( m_stateBlocks[i1] == m_stateBlocks[i2] && ExtendedSkin::isStateIdentical(state,comparedTo) );
	}
	
	//____ _updateOpaqueFlag() ________________________________________________

	void BlockSkin::_updateOpaqueFlag()
	{
		if (!m_pSurface)
			m_bOpaque = false;
		else if (m_blendMode == BlendMode::Replace)
			m_bOpaque = true;
		else if (m_blendMode == BlendMode::Blend)
			m_bOpaque = m_pSurface->isOpaque();
		else
			m_bOpaque = false;
	}

	//____ _updateUnsetStates() _______________________________________________

	void BlockSkin::_updateUnsetStates()
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



} // namespace wg
