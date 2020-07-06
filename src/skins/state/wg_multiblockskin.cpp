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

#include <wg_multiblockskin.h>
#include <wg_gfxdevice.h>
#include <wg_geo.h>
#include <wg_util.h>

#include <assert.h>

namespace wg
{
	using namespace Util;

	const TypeInfo MultiBlockStateSkin::TYPEINFO = { "MultiBlockStateSkin", &StateSkin::TYPEINFO };

	//____ create() _______________________________________________________________

	MultiBlockStateSkin_p MultiBlockStateSkin::create(SizeI blockSize, BorderI frame)
	{
		return MultiBlockStateSkin_p(new MultiBlockStateSkin(blockSize,frame));
	}


	//____ constructor ____________________________________________________________

	MultiBlockStateSkin::MultiBlockStateSkin(SizeI blockSize, BorderI frame)
	{
		m_blockSizePoints	= blockSize;
		m_frame				= frame;
		m_bOpaque			= false;

		for( int i = 0 ; i < StateEnum_Nb ; i++ )
		m_bStateOpaque[i] = false;
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& MultiBlockStateSkin::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ addLayer() _____________________________________________________________

	int MultiBlockStateSkin::addLayer(Surface * pSurf, CoordI ofs)
	{
		// HACK!

		m_blockSize = (m_blockSizePoints*pSurf->qpixPerPoint()) / 4;

		//

		m_layers.emplace_back();
		auto& layer = m_layers.back();

		layer.blendMode = BlendMode::Blend;
		layer.pSurface = pSurf;
		layer.stateBlockMask = 1;               // Only normal state is set.

		if (pSurf->isOpaque())
			m_bOpaque = true;


		for (int i = 0; i < StateEnum_Nb; i++)
		{
			layer.blockOfs[i] = ofs*pSurf->qpixPerPoint()/4;
			layer.tintColor[i] = Color::White;

			_updateStateOpacity(i);
		}


		return (int) m_layers.size();
	}

	int MultiBlockStateSkin::addLayer(Surface * pSurf, CoordI blockStartOfs, SizeI blockPitch, const std::initializer_list<State>& stateBlocks)
	{
		// HACK!

		m_blockSize = (m_blockSizePoints*pSurf->qpixPerPoint()) / 4;

		//

		blockStartOfs = (blockStartOfs*pSurf->qpixPerPoint()) / 4;
		blockPitch = (blockPitch*pSurf->qpixPerPoint()) / 4;

		//

		m_layers.emplace_back();
		auto& layer = m_layers.back();

		layer.blendMode = BlendMode::Blend;
		layer.pSurface = pSurf;
		layer.stateBlockMask = 0;

		if (pSurf->isOpaque())
			m_bOpaque = true;

		//

		for (int i = 0; i < StateEnum_Nb; i++)
			layer.tintColor[i] = Color::White;

		//

		int ofs = 0;
		for (StateEnum state : stateBlocks)
		{
			int index = _stateToIndex(state);

			layer.stateBlockMask.setBit(index);
			layer.blockOfs[index] = blockStartOfs + CoordI( blockPitch.w*ofs, blockPitch.h*ofs );
			ofs++;
		}

		//

		assert(layer.stateBlockMask.bit(0) == true);				// A block for state normal is required.

		// Fill in fallback states and update opacity flag

		for (int i = 0; i < StateEnum_Nb; i++)
		{
			if (!layer.stateBlockMask.bit(i))
			{
				int fallbackIndex = bestStateIndexMatch(i, layer.stateBlockMask);
				layer.blockOfs[i] = layer.blockOfs[fallbackIndex];
			}

			_updateStateOpacity(i);
		}

		//

		return (int) m_layers.size();
	}

	int MultiBlockStateSkin::addLayer(Surface * pSurf, const std::initializer_list<State>& stateBlocks, Axis axis, int spacing, CoordI blockStartOfs )
	{
		SizeI blockPitch;

		if (axis == Axis::X)
			blockPitch.w = m_blockSizePoints.w + spacing;
		else
			blockPitch.h = m_blockSizePoints.h + spacing;

		return addLayer(pSurf, blockStartOfs, blockPitch, stateBlocks);
	}



	//____ setLayerTint() _________________________________________________________

	bool MultiBlockStateSkin::setLayerTint(int layerIdx, Color tintColor)
	{
		auto& layer = m_layers.at(layerIdx-1);

		for (int i = 0; i < StateEnum_Nb; i++)
		{
			uint8_t		oldAlpha = layer.tintColor[i].a;

			layer.tintColor[i] = tintColor;

			if (oldAlpha != layer.tintColor[i].a)
				_updateStateOpacity(i);
		}
		return true;
	}


	bool MultiBlockStateSkin::setLayerTint(int layerIdx, const std::initializer_list< std::pair<State,Color> >& stateColors)
	{
		auto& layer = m_layers.at(layerIdx-1);

		// Set default color for normal state.

		layer.tintColor[0] = Color::White;

		//

		for (auto& stateColor : stateColors)
		{
			int index = _stateToIndex(stateColor.first);

			uint8_t		oldAlpha = layer.tintColor[index].a;

			layer.tintColor[index] = stateColor.second;

			if (oldAlpha != stateColor.second.a)
				_updateStateOpacity(index);

			layer.stateColorMask.setBit(index);
		}

		// Fill in fallback states and update opacity flag

		for (int i = 0; i < StateEnum_Nb; i++)
		{
			if (!layer.stateColorMask.bit(i))
			{
				uint8_t		oldAlpha = layer.tintColor[i].a;

				int fallbackIndex = bestStateIndexMatch(i, layer.stateBlockMask);
				layer.tintColor[i] = layer.tintColor[fallbackIndex];

				if (oldAlpha != layer.tintColor[i].a )
					_updateStateOpacity(i);
			}
		}

		//

		return true;
	}

	//____ SetLayerBlendMode() ____________________________________________________

	bool MultiBlockStateSkin::setLayerBlendMode(int layerIdx, BlendMode blendMode)
	{
		auto& layer = m_layers.at(layerIdx - 1);

		if (layer.blendMode != blendMode)
		{
			layer.blendMode = blendMode;

			for (int i = 0; i < StateEnum_Nb; i++)
				_updateStateOpacity(i);
		}

		return true;
	}

	//____ isStateIdentical() _____________________________________________________

	bool MultiBlockStateSkin::isStateIdentical(State state, State comparedTo, float fraction, float fraction2) const
	{
		int i1 = _stateToIndex(state);
		int i2 = _stateToIndex(comparedTo);

		for (auto& layer : m_layers)
		{
			if (layer.blockOfs[i1] != layer.blockOfs[i2] || layer.tintColor[i1] != layer.tintColor[i2])
				return false;
		}

		return StateSkin::isStateIdentical(state, comparedTo);
	}


	//____ render() _______________________________________________________________

	void MultiBlockStateSkin::render( GfxDevice * pDevice, const Rect& _canvas, State state, float fraction, float fraction2) const
	{
		if (m_layers.empty() || m_blockSize.w <= 0 || m_blockSize.h <= 0 )
			return;

		RectI canvas = _canvas.px();

		int stateIndex = _stateToIndex(state);

		BlendMode	orgBlendMode = pDevice->blendMode();
		Color		orgTintColor = pDevice->tintColor();

		BlendMode	blendMode = orgBlendMode;
		Color		tintColor = orgTintColor;
		Color		mixedTint = orgTintColor;

		for (auto& layer : m_layers)
		{
			if (layer.blendMode != blendMode)
			{
				blendMode = layer.blendMode;
				pDevice->setBlendMode(blendMode);
			}

			if (layer.tintColor[stateIndex] != tintColor )
			{
				tintColor = layer.tintColor[stateIndex];
				mixedTint = orgTintColor * tintColor;
				pDevice->setTintColor(mixedTint);
			}

			pDevice->setBlitSource(layer.pSurface);

			const RectI&	src = RectI(layer.blockOfs[stateIndex], m_blockSize);

			const BorderI&    sourceBorders = m_frame*layer.pSurface->qpixPerPoint();
			const BorderI     canvasBorders = pointsToPixels(m_frame);

			pDevice->blitNinePatch( canvas, canvasBorders, src, sourceBorders );
		}

		if (mixedTint != orgTintColor)
			pDevice->setTintColor(orgTintColor);

		if (blendMode != orgBlendMode)
			pDevice->setBlendMode(orgBlendMode);
	}


	//____ minSize() ______________________________________________________________

	Size MultiBlockStateSkin::minSize() const
	{
		Size content = StateSkin::minSize();
		Size frame = Border(m_frame).aligned();
		return Size::max(content, frame);
	}

	//____ preferredSize() ________________________________________________________

	Size MultiBlockStateSkin::preferredSize() const
	{
        // Preferred size is to map each point of the surface to a pixel of the skinarea.
        
        return m_blockSizePoints;
	}

	//____ sizeForContent() _______________________________________________________

	Size MultiBlockStateSkin::sizeForContent( const Size& contentSize ) const
	{
		Size sz = StateSkin::sizeForContent(contentSize);
		Size min = Border(m_frame).aligned();

		return SizeI::max(sz, min);
	}

	//____ markTest() _____________________________________________________________

	bool MultiBlockStateSkin::markTest( const Coord& _ofs, const Rect& canvas, State state, int opacityTreshold, float fraction, float fraction2) const
	{
		if (!canvas.contains(_ofs) || m_layers.empty() || m_blockSize.w <= 0 || m_blockSize.h <= 0)
			return false;

		if( m_bOpaque )
			return true;

		int stateIndex = _stateToIndex(state);

		for (auto& layer : m_layers)
		{
			CoordI srcOfs = layer.blockOfs[stateIndex];
			bool bMarked = markTestNinePatch(_ofs, layer.pSurface, { srcOfs,m_blockSize }, canvas, opacityTreshold, m_frame);
			if (bMarked)
				return true;
		}

		return false;
	}

	//____ _updateStateOpacity() __________________________________________________

	void MultiBlockStateSkin::_updateStateOpacity(int stateIdx)
	{
		bool bOpaque = false;

		for (auto& layer : m_layers)
		{
			if (layer.blendMode == BlendMode::Replace || (layer.blendMode == BlendMode::Blend && layer.pSurface->isOpaque() && layer.tintColor[stateIdx].a == 255) )
			{
				bOpaque = true;
				break;
			}
		}

		if (bOpaque != m_bStateOpaque[stateIdx])
		{
			m_bStateOpaque[stateIdx] = bOpaque;

			if (bOpaque != m_bOpaque)
			{
				m_bOpaque = true;
				for( int i = 0 ; i < StateEnum_Nb ; i++ )
					if (m_bStateOpaque[i] )
					{
						m_bOpaque = false;
						break;
					}
			}
		}
	}

}
