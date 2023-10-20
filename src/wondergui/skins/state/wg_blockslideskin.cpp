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

#include <wg_blockslideskin.h>
#include <wg_geo.h>
#include <wg_util.h>
#include <wg_skin.impl.h>

#include <cassert>

namespace wg
{

	using namespace Util;

	const TypeInfo BlockSlideSkin::TYPEINFO = { "BlockSlideSkin", &Skin::TYPEINFO };

	//____ create() _______________________________________________________________

	BlockSlideSkin_p BlockSlideSkin::create( const Blueprint& blueprint )
	{
		return BlockSlideSkin_p(new BlockSlideSkin(blueprint));
	}

	//____ constructor ________________________________________________________

	BlockSlideSkin::BlockSlideSkin( const Blueprint& bp) : StateSkin(bp)
	{	
		m_pSurface = bp.surface;
		m_slideDirection = bp.slideDirection;
		m_slideDuration = bp.slideDuration;
		m_slideState = bp.slideState;

		m_gradient	= bp.gradient;
		m_blendMode = bp.blendMode;

		m_transitionTimes[int(m_slideState)] = m_slideDuration;

		m_stateColors[0] = bp.color;



		Axis slideAxis = bp.slideDirection == Direction::Up || bp.slideDirection == Direction::Down ? Axis::Y : Axis::X;


		Rect lane = bp.firstLane;
		
		if (lane.isEmpty())
		{

			int nStateBlocks = 1;
			for ( auto& entry : bp.states)
			{
				if (entry.state != State::Default && entry.data.blockless == false)
					nStateBlocks++;
			}


			if (slideAxis == Axis::X)
			{
				lane.w = m_pSurface->pointWidth();
				lane.h = (m_pSurface->pointHeight() - bp.spacing * (nStateBlocks - 1)) / nStateBlocks;
			}
			else
			{
				lane.w = (m_pSurface->pointWidth() - bp.spacing * (nStateBlocks - 1)) / nStateBlocks;
				lane.h = m_pSurface->pointHeight();
			}
		}
		
		Rect block = lane;
		if( slideAxis == Axis::X )
		{
			m_slideDistance = lane.w - bp.blockLength;
			block.w = bp.blockLength;
		}
		else
		{
			m_slideDistance = lane.h - bp.blockLength;
			block.h = bp.blockLength;
		}

		m_blockSize = block.size();

		if (m_slideDirection == Direction::Up)
			block.y += m_slideDistance;
		else if (m_slideDirection == Direction::Left)
			block.x += m_slideDistance;

		m_stateBlocks[0] = block.pos();


		Coord pitch = slideAxis == Axis::X ? Coord(0, block.h + bp.spacing) : Coord(block.w + bp.spacing, 0);

		int ofs = 0;
		for ( auto& stateInfo : bp.states )
		{
			{
				int index = stateInfo.state;

				if (stateInfo.data.contentShift.x != 0 || stateInfo.data.contentShift.y != 0)
				{
					m_contentShiftStateMask.setBit(index);
					m_contentShift[index] = stateInfo.data.contentShift;
					m_bContentShifting = true;
				}

				if ( !stateInfo.data.blockless && stateInfo.state != State::Default )
				{
					ofs++;
					m_stateBlockMask.setBit(index);
					m_stateBlocks[index] = block.pos() + pitch * ofs;
				}

				if (stateInfo.data.color != HiColor::Undefined)
				{
					m_stateColorMask.setBit(index);
					m_stateColors[index] = stateInfo.data.color;
				}

			}
		}

		_updateContentShift();
		_updateUnsetStateBlocks();
		_updateUnsetStateColors();
		_updateOpaqueFlags();
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& BlockSlideSkin::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ _defaultSize() ________________________________________________________

	SizeSPX BlockSlideSkin::_defaultSize(int scale) const
	{
		// Default size is when each point of the surface maps to a point of the skinarea,
		// independent of differences in scale.

		return align(ptsToSpx(m_blockSize, scale))  + align(ptsToSpx(m_margin, scale));
	}

	//____ _render() _______________________________________________________________

	void BlockSlideSkin::_render(GfxDevice* pDevice, const RectSPX& _canvas, int scale, State state, float value, float value2, int animPos, float* pStateFractions) const
	{
		if (!m_pSurface)
			return;

		RectSPX canvas = _canvas - align(ptsToSpx(m_margin, scale));

		int idx = state;
		RenderSettingsWithGradient settings(pDevice, m_layer, m_blendMode, m_stateColors[idx], canvas, m_gradient);

		pDevice->setBlitSource(m_pSurface);

		RectSPX source = _partInCanvas(scale, state, pStateFractions);

		pDevice->stretchBlit(canvas, source);
	}

	//____ _partInCanvas() _______________________________________________________

	RectSPX BlockSlideSkin::_partInCanvas(int scale, State state, float* pStateFractions) const
	{
		int idx = state;
		Coord blockOfs = m_stateBlocks[idx];

		Rect source = { blockOfs, m_blockSize };

		State	slideStateMask;
		switch (m_slideState)
		{
		case StateBits::Focused:
			slideStateMask = State::Focused;
			break;
		case StateBits::Hovered:
			slideStateMask = State::Hovered;
			break;
		case StateBits::Pressed:
			slideStateMask = State::Pressed;
			break;
		case StateBits::Selected:
			slideStateMask = State::Selected;
			break;
		case StateBits::Targeted:
			slideStateMask = State::Targeted;
			break;
		case StateBits::Disabled:
			slideStateMask = State::Disabled;
			break;
		default:
			assert(false);
		}

		bool bMaximized = ((slideStateMask.mask() & state.mask()) != 0);

		float offset = 0.f;
		if (pStateFractions != nullptr)
			offset = pStateFractions[int(m_slideState)];
		else if (bMaximized)
			offset = 1.f;

		switch (m_slideDirection)
		{
		case Direction::Up:
			source.y -= offset * m_slideDistance;
			break;
		case Direction::Down:
			source.y += offset * m_slideDistance;
			break;
		case Direction::Left:
			source.x -= offset * m_slideDistance;
			break;
		case Direction::Right:
			source.x += offset * m_slideDistance;
			break;
		}

		return ptsToSpx(source,scale);
	}


	//____ _markTest() _____________________________________________________________

	bool BlockSlideSkin::_markTest(const CoordSPX& ofs, const RectSPX& _canvas, int scale, State state, float value, float value2, int alphaOverride) const
	{
		//TODO: Take blendMode and tint (incl gradient) into account.

		RectSPX canvas = _canvas - align(ptsToSpx(m_margin, scale));

		if( !canvas.contains(ofs) )
			return false;

		//TODO: Implement the rest!

		return true;
	}

	//____ _isOpaque() _____________________________________________________________

	bool BlockSlideSkin::_isOpaque(State state) const
	{
		return m_bStateOpaque[state];
	}

	bool BlockSlideSkin::_isOpaque(const RectSPX& rect, const SizeSPX& canvasSize, int scale, State state) const
	{
		return m_bStateOpaque[state];
	}

	//____ _dirtyRect() ______________________________________________________

	RectSPX BlockSlideSkin::_dirtyRect(const RectSPX& _canvas, int scale, State newState, State oldState, float newValue, float oldValue,
		float newValue2, float oldValue2, int newAnimPos, int oldAnimPos,
		float* pNewStateFractions, float* pOldStateFractions) const
	{
		//TODO: Implement!

		RectSPX canvas = _canvas - align(ptsToSpx(m_margin, scale));

		return canvas;

/*
		if (oldState == newState)
			return Rect();

		int i1 = _stateToIndex(newState);
		int i2 = _stateToIndex(oldState);

		if (m_stateBlocks[i1] != m_stateBlocks[i2])
			return canvas;

		return StateSkin::dirtyRect(canvas, newState, oldState, newValue, oldValue, newValue2, oldValue2,
			newAnimPos, oldAnimPos, pNewStateFractions, pOldStateFractions);
*/
	}

	//____ _transitioningStates() ______________________________________________

	Bitmask<uint8_t> BlockSlideSkin::_transitioningStates() const
	{
		Bitmask<uint8_t> states;
		states.setBit(int(m_slideState));

		return states;
	}

	//____ _transitionTimes() __________________________________________________

	const int* BlockSlideSkin::_transitionTimes() const
	{
		return m_transitionTimes;
	}

	//____ _updateOpaqueFlags() ________________________________________________

	void BlockSlideSkin::_updateOpaqueFlags()
	{
		bool bTintDecides = false;

		if (!m_pSurface || !m_margin.isEmpty())
			m_bOpaque = false;
		else if (m_blendMode == BlendMode::Replace)
			m_bOpaque = true;
		else if (!m_gradient.isUndefined() && !m_gradient.isOpaque())
			m_bOpaque = false;
		else if (m_blendMode == BlendMode::Blend)
		{
			m_bOpaque = m_pSurface->isOpaque();
			bTintDecides = m_bOpaque;
		}
		else
			m_bOpaque = false;

		if (bTintDecides)
		{
			for (int i = 0; i < State::IndexAmount; i++)
			{
				m_bStateOpaque[i] = m_stateColors[i].a == 4096;
				if (m_stateColors[i].a != 4096)
					m_bOpaque = false;
			}
		}
		else
		{
			for (int i = 0; i < State::IndexAmount; i++)
				m_bStateOpaque[i] = m_bOpaque;
		}
	}

	//____ _updateUnsetStateBlocks() _______________________________________________

	void BlockSlideSkin::_updateUnsetStateBlocks()
	{
		for (int i = 0; i < State::IndexAmount; i++)
		{
			if (!m_stateBlockMask.bit(i))
			{
				int bestAlternative = bestStateIndexMatch(i, m_stateBlockMask);
				m_stateBlocks[i] = m_stateBlocks[bestAlternative];
			}
		}
	}

	//____ _updateUnsetStateColors() _______________________________________________

	void BlockSlideSkin::_updateUnsetStateColors()
	{
		for (int i = 0; i < State::IndexAmount; i++)
		{
			if (!m_stateColorMask.bit(i))
			{
				int bestAlternative = bestStateIndexMatch(i, m_stateColorMask);
				m_stateColors[i] = m_stateColors[bestAlternative];
			}
		}
	}


}
