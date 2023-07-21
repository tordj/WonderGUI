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

#include <wg_scrollskin.h>
#include <wg_geo.h>
#include <wg_util.h>
#include <wg_skin.impl.h>

#include <cassert>

namespace wg
{

	using namespace Util;

	const TypeInfo ScrollSkin::TYPEINFO = { "ScrollSkin", &Skin::TYPEINFO };

	//____ create() _______________________________________________________________

	ScrollSkin_p ScrollSkin::create( const Blueprint& blueprint )
	{
		return ScrollSkin_p(new ScrollSkin(blueprint));
	}

	//____ constructor ________________________________________________________

	ScrollSkin::ScrollSkin( const Blueprint& blueprint)
	{	
		m_pSurface = blueprint.surface;
		m_scrollDirection = blueprint.scrollDirection;
		m_scrollDuration = blueprint.scrollDuration;
		m_scrollState = blueprint.scrollState;

		m_gradient	= blueprint.gradient;
		m_blendMode = blueprint.blendMode;
		m_layer		= blueprint.layer;
		m_markAlpha	= blueprint.markAlpha;
		m_overflow	= blueprint.overflow;

		m_transitionTimes[int(m_scrollState)] = m_scrollDuration;

		m_stateColors[0] = blueprint.color;



		Axis scrollAxis = blueprint.scrollDirection == Direction::Up || blueprint.scrollDirection == Direction::Down ? Axis::Y : Axis::X;


		Rect block = blueprint.firstBlock;

		if (block.isEmpty())
		{

			int nStateBlocks = 1;
			for ( auto& entry : blueprint.states)
			{
				if (entry.state != State::Normal && entry.data.blockless == false)
					nStateBlocks++;
			}


			if (scrollAxis == Axis::X)
			{
				block.w = m_pSurface->pointWidth() - blueprint.scrollDistance;
				block.h = (m_pSurface->pointHeight() - blueprint.spacing * (nStateBlocks - 1)) / nStateBlocks;
			}
			else
			{
				block.w = (m_pSurface->pointWidth() - blueprint.spacing * (nStateBlocks - 1)) / nStateBlocks;
				block.h = m_pSurface->pointHeight() - blueprint.scrollDistance;
			}
		}

		m_blockSize = block.size();
		m_scrollDistance = blueprint.scrollDistance;

		if (m_scrollDirection == Direction::Up)
			block.y += m_scrollDistance;
		else if (m_scrollDirection == Direction::Left)
			block.x += m_scrollDistance;

		m_stateBlocks[0] = block.pos();


		Coord pitch = scrollAxis == Axis::X ? Coord(0, block.h + blueprint.spacing) : Coord(block.w + blueprint.spacing, 0);

		int ofs = 0;
		for ( auto& stateInfo : blueprint.states )
		{
			{
				int index = stateInfo.state;

				if (stateInfo.data.contentShift.x != 0 || stateInfo.data.contentShift.y != 0)
				{
					m_contentShiftStateMask.setBit(index);
					m_contentShift[index] = stateInfo.data.contentShift;
					m_bContentShifting = true;
				}

				if ( !stateInfo.data.blockless && stateInfo.state != State::Normal )
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

	const TypeInfo& ScrollSkin::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ _defaultSize() ________________________________________________________

	SizeSPX ScrollSkin::_defaultSize(int scale) const
	{
		// Default size is when each point of the surface maps to a point of the skinarea,
		// independent of differences in scale.

		return align(ptsToSpx(m_blockSize, scale));
	}

	//____ _render() _______________________________________________________________

	void ScrollSkin::_render(GfxDevice* pDevice, const RectSPX& canvas, int scale, State state, float value, float value2, int animPos, float* pStateFractions) const
	{
		if (!m_pSurface)
			return;

		int idx = state;
		RenderSettingsWithGradient settings(pDevice, m_layer, m_blendMode, m_stateColors[idx], canvas, m_gradient);

		Coord blockOfs = m_stateBlocks[idx];
		pDevice->setBlitSource(m_pSurface);

		RectSPX source = _partInCanvas(scale, state, pStateFractions);

		pDevice->stretchBlit(canvas, source);
	}

	//____ _partInCanvas() _______________________________________________________

	RectSPX ScrollSkin::_partInCanvas(int scale, State state, float* pStateFractions) const
	{
		int idx = state;
		Coord blockOfs = m_stateBlocks[idx];

		Rect source = { blockOfs, m_blockSize };

		State	scrollStateMask;
		switch (m_scrollState)
		{
		case StateBits::Focused:
			scrollStateMask = State::Focused;
			break;
		case StateBits::Hovered:
			scrollStateMask = State::Hovered;
			break;
		case StateBits::Pressed:
			scrollStateMask = State::Pressed;
			break;
		case StateBits::Selected:
			scrollStateMask = State::Selected;
			break;
		case StateBits::Targeted:
			scrollStateMask = State::Targeted;
			break;
		case StateBits::Disabled:
			scrollStateMask = State::Disabled;
			break;
		default:
			assert(false);
		}

		bool bMaximized = ((scrollStateMask.mask() & state.mask()) != 0);

		float offset = 0.f;
		if (pStateFractions != nullptr)
			offset = pStateFractions[int(m_scrollState)];
		else if (bMaximized)
			offset = 1.f;

		switch (m_scrollDirection)
		{
		case Direction::Up:
			source.y -= offset * m_scrollDistance;
			break;
		case Direction::Down:
			source.y += offset * m_scrollDistance;
			break;
		case Direction::Left:
			source.x -= offset * m_scrollDistance;
			break;
		case Direction::Right:
			source.x += offset * m_scrollDistance;
			break;
		}

		return ptsToSpx(source,scale);
	}


	//____ _markTest() _____________________________________________________________

	bool ScrollSkin::_markTest(const CoordSPX& _ofs, const RectSPX& canvas, int scale, State state, float value, float value2, int alphaOverride) const
	{
		//TODO: Take blendMode and tint (incl gradient) into account.

		//TODO: Implement!

		return true;
	}

	//____ _isOpaque() _____________________________________________________________

	bool ScrollSkin::_isOpaque(State state) const
	{
		return m_bStateOpaque[state];
	}

	bool ScrollSkin::_isOpaque(const RectSPX& rect, const SizeSPX& canvasSize, int scale, State state) const
	{
		return m_bStateOpaque[state];
	}

	//____ _dirtyRect() ______________________________________________________

	RectSPX ScrollSkin::_dirtyRect(const RectSPX& canvas, int scale, State newState, State oldState, float newValue, float oldValue,
		float newValue2, float oldValue2, int newAnimPos, int oldAnimPos,
		float* pNewStateFractions, float* pOldStateFractions) const
	{
		//TODO: Implement!

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

	Bitmask<uint8_t> ScrollSkin::_transitioningStates() const
	{
		Bitmask<uint8_t> states;
		states.setBit(int(m_scrollState));

		return states;
	}

	//____ _transitionTimes() __________________________________________________

	const int* ScrollSkin::_transitionTimes() const
	{
		return m_transitionTimes;
	}

	//____ _updateOpaqueFlags() ________________________________________________

	void ScrollSkin::_updateOpaqueFlags()
	{
		bool bTintDecides = false;

		if (!m_pSurface)
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

	void ScrollSkin::_updateUnsetStateBlocks()
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

	void ScrollSkin::_updateUnsetStateColors()
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
