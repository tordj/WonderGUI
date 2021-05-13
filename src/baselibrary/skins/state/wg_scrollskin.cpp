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

	ScrollSkin_p ScrollSkin::create()
	{
		return ScrollSkin_p(new ScrollSkin());
	}

	ScrollSkin_p ScrollSkin::create(Surface* pSurface, pts windowLength, StateBits scrollState, int scrollTime, Direction scrollDirection, 
									std::initializer_list<State> stateBlocks, pts spacing)
	{
		//TODO: Sanity checks and error handling

		Rect firstBlock = pSurface->pointSize();
		int nBlocks = stateBlocks.size();

		int totalSpacing = spacing * (nBlocks - 1);

		if (scrollDirection == Direction::Up || scrollDirection == Direction::Down)
			firstBlock.w = (firstBlock.w - totalSpacing) / nBlocks;
		else
			firstBlock.h = (firstBlock.h - totalSpacing) / nBlocks;

		auto pSkin = ScrollSkin_p(new ScrollSkin(pSurface, windowLength, scrollState, scrollTime, scrollDirection, firstBlock.size()) );

		Axis axis = scrollDirection == Direction::Up || scrollDirection == Direction::Down ? Axis::X : Axis::Y;
		pSkin->setBlocks(stateBlocks, axis, spacing, firstBlock.pos());

		return pSkin;
	}

	ScrollSkin_p ScrollSkin::create(Surface* pSurface, pts windowLength, StateBits scrollState, int scrollTime, Direction scrollDirection,
									Rect firstBlock, std::initializer_list<State> stateBlocks, pts spacing)
	{
		//TODO: Sanity checks and error handling

		auto pSkin = ScrollSkin_p(new ScrollSkin(pSurface, windowLength, scrollState, scrollTime, scrollDirection, firstBlock.size()) );

		Axis axis = scrollDirection == Direction::Up || scrollDirection == Direction::Down ? Axis::X : Axis::Y;
		pSkin->setBlocks(stateBlocks, axis, spacing, firstBlock.pos());

		return pSkin;
	}



	//____ constructor ________________________________________________________

	ScrollSkin::ScrollSkin()
	{
		//TODO: Add methods for everything not yet set!

		m_bOpaque = false;

		for (int i = 0; i < StateEnum_Nb; i++)
		{
			m_bStateOpaque[i] = false;
			m_stateColors[i] = Color::White;
		}
	}

	ScrollSkin::ScrollSkin(Surface* pSurface, pts windowLength, StateBits scrollState, int scrollTime, Direction scrollDirection, 
							Size blockSize) :
		m_pSurface(pSurface),
		m_windowLength(windowLength),
		m_scrollState(scrollState),
		m_scrollTime(scrollTime),
		m_scrollDirection(scrollDirection),
		m_blockSize(blockSize)
	{	
		m_bOpaque = pSurface->isOpaque();

		for (int i = 0; i < StateEnum_Nb; i++)
		{
			m_bStateOpaque[i] = m_bOpaque;
			m_stateColors[i] = Color::White;
		}

		m_transitionTimes[int(scrollState)] = scrollTime;

	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& ScrollSkin::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setBlock() _____________________________________________________________

	void ScrollSkin::setBlock(Coord ofs)
	{
		m_stateBlocks[0] = ofs;
		m_stateBlockMask = 1;

		_updateUnsetStateBlocks();
	}

	void ScrollSkin::setBlock(State state, Coord ofs)
	{
		int i = _stateToIndex(state);

		m_stateBlocks[i] = ofs;
		m_stateBlockMask.setBit(i);
		_updateUnsetStateBlocks();
	}

	//____ setBlocks() ________________________________________________________

	void ScrollSkin::setBlocks(std::initializer_list<State> stateBlocks, Axis axis, pts spacing, Coord blockStartOfs)
	{
		Coord pitch = axis == Axis::X ? Coord(m_blockSize.w + spacing, 0) : Coord(0, m_blockSize.h + spacing);

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

	Rect ScrollSkin::block(State state) const
	{
		return { m_stateBlocks[_stateToIndex(state)], m_blockSize };
	}

	//____ setColor() __________________________________________________________

	void ScrollSkin::setColor(HiColor tint)
	{
		m_stateColors[0] = tint;
		m_stateColorMask = 1;

		_updateUnsetStateColors();
		_updateOpaqueFlags();
	}

	void ScrollSkin::setColor(State state, HiColor tint)
	{
		int i = _stateToIndex(state);

		m_stateColors[i] = tint;
		m_stateColorMask.setBit(i);
		_updateUnsetStateColors();
		_updateOpaqueFlags();
	}

	void ScrollSkin::setColor(std::initializer_list< std::tuple<State, HiColor> > stateTints)
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

	HiColor ScrollSkin::color(State state) const
	{
		return m_stateColors[_stateToIndex(state)];
	}

	//____ setGradient() ______________________________________________________

	void ScrollSkin::setGradient(const Gradient& gradient)
	{
		m_gradient = gradient;
		m_bGradient = true;
		_updateOpaqueFlags();
	}


	//____ setBlendMode() _____________________________________________________

	void ScrollSkin::setBlendMode(BlendMode mode)
	{
		m_blendMode = mode;
		_updateOpaqueFlags();
	}

	//____ _preferredSize() ________________________________________________________

	SizeSPX ScrollSkin::_preferredSize(int scale) const
	{
		// Preferred size is to map each point of the surface to a point of the skinarea,
		// independent of differences in scale.

		Size sz;

		if (m_scrollDirection == Direction::Up || m_scrollDirection == Direction::Down)
			sz = { m_blockSize.w, m_windowLength };
		else
			sz = { m_windowLength, m_blockSize.h };

		return align(ptsToSpx(sz, scale));
	}

	//____ _render() _______________________________________________________________

	void ScrollSkin::_render(GfxDevice* pDevice, const RectSPX& canvas, int scale, State state, float value, float value2, int animPos, float* pStateFractions) const
	{
		if (!m_pSurface)
			return;

		int idx = _stateToIndex(state);
		RenderSettingsWithGradient settings(pDevice, m_layer, m_blendMode, m_stateColors[idx], canvas, m_gradient, m_bGradient);

		Coord blockOfs = m_stateBlocks[idx];
		pDevice->setBlitSource(m_pSurface);

		RectSPX source = _partInCanvas(scale, state, pStateFractions);

		pDevice->stretchBlit(canvas, source);
	}

	//____ _partInCanvas() _______________________________________________________

	RectSPX ScrollSkin::_partInCanvas(int scale, State state, float* pStateFractions) const
	{
		int idx = _stateToIndex(state);
		Coord blockOfs = m_stateBlocks[idx];

		Rect source = { blockOfs, m_blockSize };

		State	scrollStateMask;
		switch (m_scrollState)
		{
		case StateBits::Focused:
			scrollStateMask = StateEnum::Focused;
			break;
		case StateBits::Hovered:
			scrollStateMask = StateEnum::Hovered;
			break;
		case StateBits::Pressed:
			scrollStateMask = StateEnum::Pressed;
			break;
		case StateBits::Selected:
			scrollStateMask = StateEnum::Selected;
			break;
		case StateBits::Targeted:
			scrollStateMask = StateEnum::Targeted;
			break;
		case StateBits::Disabled:
			scrollStateMask = StateEnum::Disabled;
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


		if (m_scrollDirection == Direction::Up || m_scrollDirection == Direction::Down)
		{
			source.h = m_windowLength;
			source.y += offset * (m_blockSize.h - m_windowLength);
		}
		else
		{
			source.w = m_windowLength;
			source.x += offset * (m_blockSize.w - m_windowLength);
		}

		return ptsToSpx(source,scale);
	}


	//____ _markTest() _____________________________________________________________

	bool ScrollSkin::_markTest(const CoordSPX& _ofs, const RectSPX& canvas, int scale, State state, int opacityTreshold, float value, float value2) const
	{
		//TODO: Take blendMode and tint (incl gradient) into account.

		//TODO: Implement!

		return true;
	}

	//____ _isOpaque() _____________________________________________________________

	bool ScrollSkin::_isOpaque(State state) const
	{
		return m_bStateOpaque[_stateToIndex(state)];
	}

	bool ScrollSkin::_isOpaque(const RectSPX& rect, const SizeSPX& canvasSize, int scale, State state) const
	{
		return m_bStateOpaque[_stateToIndex(state)];
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
		else if (m_bGradient && !m_gradient.isOpaque())
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

	void ScrollSkin::_updateUnsetStateBlocks()
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

	void ScrollSkin::_updateUnsetStateColors()
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


}
