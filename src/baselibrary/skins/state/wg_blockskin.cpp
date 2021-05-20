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
#include <wg_skin.impl.h>

namespace wg
{

	using namespace Util;

	const TypeInfo BlockSkin::TYPEINFO = { "BlockSkin", &StateSkin::TYPEINFO };

	//____ create() _______________________________________________________________

	BlockSkin_p BlockSkin::create()
	{
		return BlockSkin_p(new BlockSkin());
	}


	BlockSkin_p BlockSkin::create(Surface * pSurface, Border frame )
	{
		BorderI pixelFrame = roundToPixels(ptsToSpx(frame,pSurface->scale()));

		if (pSurface == nullptr || pixelFrame.width() >= pSurface->pixelSize().w || pixelFrame.height() >= pSurface->pixelSize().h  )
			return nullptr;

		return BlockSkin_p( new BlockSkin(pSurface, pSurface->pointSize(), frame) );
	}

	BlockSkin_p BlockSkin::create(Surface * pSurface, Rect block, Border frame)
	{
		if (pSurface == nullptr)
			return nullptr;

		SizeI surfSize = pSurface->pixelSize();
		BorderI pixelFrame = roundToPixels(ptsToSpx(frame, pSurface->scale()));
		RectI pixelBlock = roundToPixels(ptsToSpx(block, pSurface->scale()));

		if( pixelFrame.width() >= surfSize.w || pixelFrame.height() >= surfSize.h ||
			pixelBlock.x < 0 || pixelBlock.y < 0 || pixelBlock.right() > surfSize.w || pixelBlock.bottom() > surfSize.h )
			return nullptr;

		return BlockSkin_p(new BlockSkin(pSurface, block, frame));
	}

	BlockSkin_p	BlockSkin::create(Surface * pSurface, Rect _firstBlock, std::initializer_list<State> stateBlocks, Border _frame, Axis axis, int _spacing)
	{
		if (pSurface == nullptr || stateBlocks.size() < 1 )
			return nullptr;

		// Get pixel values

		int scale = pSurface->scale();

		SizeI surfSize = pSurface->pixelSize();
		RectI firstBlock = roundToPixels(ptsToSpx(_firstBlock, scale));
		BorderI frame = roundToPixels(ptsToSpx(_frame, scale));
		int   spacing = roundToPixels(ptsToSpx(_spacing, scale));

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

		BlockSkin * p = new BlockSkin(pSurface, _firstBlock, _frame);

		p->setBlocks(stateBlocks, axis, _spacing, _firstBlock.pos());
		return BlockSkin_p(p);
	}

	BlockSkin_p BlockSkin::create(Surface * pSurface, std::initializer_list<State> stateBlocks, Border _frame, Axis axis, int _spacing)
	{
		if (pSurface == nullptr || stateBlocks.size() < 1)
			return nullptr;

		int scale = pSurface->scale();

		SizeI	surfSize = pSurface->pixelSize();
		int		nBlocks = (int) stateBlocks.size();
		BorderI frame = roundToPixels(ptsToSpx(_frame, scale));
		int   spacing = roundToPixels(ptsToSpx(_spacing, scale));

		// Check so blocks fit evenly

		int		length = axis == Axis::X ? surfSize.w : surfSize.h;

		if ((length - (nBlocks - 1) * spacing) % nBlocks != 0)
			return nullptr;

		// Create the skin

		int blockLen = (length - (nBlocks - 1) * spacing) / nBlocks;

		SizeI blockSize = axis == Axis::X ? SizeI(blockLen, surfSize.h) : SizeI(surfSize.w, blockLen);

		BlockSkin * p = new BlockSkin(pSurface, spxToPts(blockSize*64, scale), _frame);
		p->setBlocks(stateBlocks, axis, spacing, Coord(0, 0));
		return BlockSkin_p(p);
	}

	BlockSkin_p BlockSkin::createStatic( Surface * pSurface, Rect block, Border frame )
	{
		return create(pSurface, block, frame);
	}


	BlockSkin_p BlockSkin::createClickable( Surface * pSurface, Size blockGeo, Coord blockStartOfs, Size blockPitch, Border blockFrame )
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

	BlockSkin_p BlockSkin::createStaticFromSurface( Surface * pSurface, Border frame )
	{
		return create(pSurface, frame);
	}

	//____ constructor ____________________________________________________________

	BlockSkin::BlockSkin()
	{
		m_pSurface = nullptr;
		m_bOpaque = false;

		for (int i = 0; i < StateEnum_Nb; i++)
		{
			m_bStateOpaque[i] = false;
			m_stateColors[i] = Color::White;
		}
	}

	BlockSkin::BlockSkin(Surface * pSurface, Rect block, Border frame)
	{
		m_pSurface			= pSurface;
		m_ninePatch.block.setSize( block.size() );
		m_ninePatch.frame	= frame;
		m_bOpaque			= pSurface->isOpaque();

		for( int i = 0 ; i < StateEnum_Nb ; i++ )
		{
			m_bStateOpaque[i] = m_bOpaque;
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

	void BlockSkin::setBlocks(std::initializer_list<State> stateBlocks, Axis axis, int spacing, Coord blockStartOfs )
	{
		Coord pitch = axis == Axis::X ? Coord(m_ninePatch.block.w + spacing, 0 ) : Coord(0, m_ninePatch.block.h + spacing);

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
		return { m_stateBlocks[_stateToIndex(state)], m_ninePatch.block.size() };
	}

	//____ setColor() __________________________________________________________

	void BlockSkin::setColor(HiColor tint)
	{
		m_stateColors[0] = tint;
		m_stateColorMask = 1;

		_updateUnsetStateColors();
		_updateOpaqueFlags();
	}

	void BlockSkin::setColor(State state, HiColor tint)
	{
		int i = _stateToIndex(state);

		m_stateColors[i] = tint;
		m_stateColorMask.setBit(i);
		_updateUnsetStateColors();
		_updateOpaqueFlags();
	}

	void BlockSkin::setColor(std::initializer_list< std::tuple<State, HiColor> > stateTints)
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

	//____ color() _____________________________________________________________

	HiColor BlockSkin::color(State state) const
	{
		return m_stateColors[_stateToIndex(state)];
	}

	//____ setGradient() ______________________________________________________

	void BlockSkin::setGradient(const Gradient& gradient)
	{
		m_gradient = gradient;
		m_bGradient = true;
		_updateOpaqueFlags();
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
		m_ninePatch.block.setSize( size );
	}

	//____ setFrame() ____________________________________________________________

	void BlockSkin::setFrame(Border frame)
	{
		m_ninePatch.frame = frame;
	}

	//____ setRigidPartX() _____________________________________________

	bool BlockSkin::setRigidPartX(pts ofs, pts length, YSections sections)
	{
		pts	midSecLen = m_ninePatch.block.w - m_ninePatch.frame.width();
		ofs -= m_ninePatch.frame.left;

		// Sanity check

		if (length <= 0 || ofs > midSecLen || ofs + length < 0)
		{
			m_ninePatch.rigidPartXOfs = 0;
			m_ninePatch.rigidPartXLength = 0;
			m_ninePatch.rigidPartXSections = YSections::None;
			return false;
		}

		//

		if (ofs < 0)
		{
			length += ofs;
			ofs = 0;
		}

		if (ofs + length > midSecLen)
			length = midSecLen - ofs;

		m_ninePatch.rigidPartXOfs = ofs;
		m_ninePatch.rigidPartXLength = length;
		m_ninePatch.rigidPartXSections = sections;

		return true;
	}

	//____ setRigidPartY() _____________________________________________

	bool BlockSkin::setRigidPartY(pts ofs, pts length, XSections sections)
	{
		pts	midSecLen = m_ninePatch.block.h - m_ninePatch.frame.height();
		ofs -= m_ninePatch.frame.top;

		// Sanity check

		if (length <= 0 || ofs > midSecLen || ofs + length < 0)
		{
			m_ninePatch.rigidPartYOfs = 0;
			m_ninePatch.rigidPartYLength = 0;
			m_ninePatch.rigidPartYSections = XSections::None;
			return false;
		}

		//

		if (ofs < 0)
		{
			length += ofs;
			ofs = 0;
		}

		if (ofs + length > midSecLen)
			length = midSecLen - ofs;

		m_ninePatch.rigidPartYOfs = ofs;
		m_ninePatch.rigidPartYLength = length;
		m_ninePatch.rigidPartYSections = sections;

		return true;
	}


	//____ render() _______________________________________________________________

	void BlockSkin::_render( GfxDevice * pDevice, const RectSPX& canvas, int scale, State state, float value, float value2, int animPos, float* pStateFractions) const
	{
		if( !m_pSurface )
			return;

		int idx = _stateToIndex(state);
		RenderSettingsWithGradient settings(pDevice, m_layer, m_blendMode, m_stateColors[idx], canvas, m_gradient, m_bGradient);

		pDevice->setBlitSource(m_pSurface);

		NinePatch	patch = m_ninePatch;
		patch.block.setPos(m_stateBlocks[idx]);

		pDevice->blitNinePatch(canvas, ptsToSpx(patch.frame,scale), patch, scale);
	}

	//____ _minSize() ______________________________________________________________

	SizeSPX BlockSkin::_minSize(int scale) const
	{
		SizeSPX content = align(ptsToSpx(m_contentPadding,scale));
		SizeSPX frame = align(ptsToSpx(m_ninePatch.frame,scale));
		return SizeSPX::max( content, frame );
	}

	//____ _preferredSize() ________________________________________________________

	SizeSPX BlockSkin::_preferredSize(int scale) const
	{
        //This takes the scale of the surface into account
        // Preferred size is to map each point of the surface to a point of the skinarea.
        
        return align(ptsToSpx(m_ninePatch.block.size(),scale));
	}

	//____ _sizeForContent() _______________________________________________________

	SizeSPX BlockSkin::_sizeForContent( const SizeSPX& contentSize, int scale ) const
	{
		SizeSPX sz = StateSkin::_sizeForContent(contentSize,scale);
		SizeSPX min = align(ptsToSpx(m_ninePatch.frame,scale));

		return SizeSPX::max(sz, min);
	}

	//____ _markTest() _____________________________________________________________

	bool BlockSkin::_markTest( const CoordSPX& _ofs, const RectSPX& canvas, int scale, State state, int opacityTreshold, float value, float value2) const
	{
		//TODO: Take blendMode and tint (incl gradient) into account.

		NinePatch	patch = m_ninePatch;
		patch.block.setPos(m_stateBlocks[_stateToIndex(state)]);

		return markTestNinePatch(_ofs, m_pSurface, patch, canvas, scale, opacityTreshold);
	}

	//____ _isOpaque() _____________________________________________________________

	bool BlockSkin::_isOpaque( State state ) const
	{
		return m_bStateOpaque[_stateToIndex(state)];
	}

	bool BlockSkin::_isOpaque( const RectSPX& rect, const SizeSPX& canvasSize, int scale, State state ) const
	{
		return m_bStateOpaque[_stateToIndex(state)];
	}

	//____ _dirtyRect() ______________________________________________________

	RectSPX BlockSkin::_dirtyRect(const RectSPX& canvas, int scale, State newState, State oldState, float newValue, float oldValue,
		float newValue2, float oldValue2, int newAnimPos, int oldAnimPos,
		float* pNewStateFractions, float* pOldStateFractions) const
	{
		if (oldState == newState)
			return RectSPX();

		int i1 = _stateToIndex(newState);
		int i2 = _stateToIndex(oldState);

		if (m_stateBlocks[i1] != m_stateBlocks[i2])
			return canvas;

		return StateSkin::_dirtyRect(canvas, scale, newState, oldState, newValue, oldValue, newValue2, oldValue2,
			newAnimPos, oldAnimPos, pNewStateFractions, pOldStateFractions);
	}


	//____ _updateOpaqueFlags() ________________________________________________

	void BlockSkin::_updateOpaqueFlags()
	{
		bool bTintDecides = false;

		if (!m_pSurface)
			m_bOpaque = false;
		else if (m_blendMode == BlendMode::Replace)
			m_bOpaque = true;
		else if (m_bGradient && !m_gradient.isOpaque())
			m_bOpaque = false;
		else if (m_blendMode == BlendMode::Blend )
		{
			m_bOpaque = m_pSurface->isOpaque();
			bTintDecides = m_bOpaque;
		}
		else
			m_bOpaque = false;

		if (bTintDecides)
		{
			for (int i = 0; i < StateEnum_Nb; i++)
			{
				m_bStateOpaque[i] = m_stateColors[i].a == 4096;
				if (m_stateColors[i].a != 4096)
					m_bOpaque = false;
			}
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
