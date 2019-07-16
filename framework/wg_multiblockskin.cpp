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
#include <wg3_util.h>

#include <assert.h>
#include <algorithm>

//____ create() _______________________________________________________________

WgMultiBlockSkinPtr WgMultiBlockSkin::Create(WgSize blockSize, WgBorders frame)
{
	return WgMultiBlockSkinPtr(new WgMultiBlockSkin(blockSize,frame));
}


//____ Constructor ____________________________________________________________

WgMultiBlockSkin::WgMultiBlockSkin(WgSize blockSize, WgBorders frame)
{
	m_blockSizePoints = blockSize;
	m_frame = frame;
	m_bIsOpaque = false;

	for( int i = 0 ; i < WG_NB_STATES ; i++ )
	m_bStateOpaque[i] = false;
}


//____ AddLayer() _____________________________________________________________

int WgMultiBlockSkin::AddLayer(WgSurface * pSurf, WgCoord ofs)
{
	// HACK!

	m_scale = pSurf->ScaleFactor();
	m_blockSize = (m_blockSizePoints*m_scale) / WG_SCALE_BASE;

	//

	m_layers.emplace_back();
	auto& layer = m_layers.back();

	layer.blendMode = WgBlendMode::Blend;
	layer.pSurface = pSurf;

	if (pSurf->IsOpaque())
		m_bIsOpaque = true;


	for (int i = 0; i < WG_NB_STATES; i++)
	{
		layer.blockOfs[i] = ofs*m_scale / WG_SCALE_BASE;
		layer.tintColor[i] = WgColor::White;

		_updateStateOpacity(i);
	}


	return m_layers.size();
}

int WgMultiBlockSkin::AddLayer(WgSurface * pSurf, WgCoord blockStartOfs, WgSize blockPitch, std::initializer_list<WgState> stateBlocks)
{
	// HACK!

	m_scale = pSurf->ScaleFactor();
	m_blockSize = (m_blockSizePoints*m_scale) / WG_SCALE_BASE;

	//

	blockStartOfs = blockStartOfs * m_scale / WG_SCALE_BASE;
	blockPitch = blockPitch * m_scale / WG_SCALE_BASE;

	//

	m_layers.emplace_back();
	auto& layer = m_layers.back();

	layer.blendMode = WgBlendMode::Blend;
	layer.pSurface = pSurf;
    layer.stateBlockMask = 0;
    
	if (pSurf->IsOpaque())
		m_bIsOpaque = true;

	//

	for (int i = 0; i < WG_NB_STATES; i++)
		layer.tintColor[i] = WgColor::White;

	//

	int ofs = 0;
	for (WgStateEnum state : stateBlocks)
	{
		int index = WgUtil::_stateToIndex(state);
        
        layer.stateBlockMask.setBit(index);
		layer.blockOfs[index] = blockStartOfs + WgCoord(blockPitch.w*ofs, blockPitch.h*ofs);
		ofs++;
	}

	//

    assert(layer.stateBlockMask.bit(0) == true);                // A block for state normal is required.
    
	// Fill in fallback states and update opacity flag

	for (int i = 0; i < WG_NB_STATES; i++)
	{
        if (!layer.stateBlockMask.bit(i))
		{
            int fallbackIndex = wg::Util::bestStateIndexMatch(i, layer.stateBlockMask);
			layer.blockOfs[i] = layer.blockOfs[fallbackIndex];
		}

		_updateStateOpacity(i);
	}

	//

	return m_layers.size();
}

//____ SetLayerTint() _________________________________________________________

bool WgMultiBlockSkin::SetLayerTint(int layerIdx, WgColor tintColor)
{
	auto& layer = m_layers.at(layerIdx-1);

	for (int i = 0; i < WG_NB_STATES; i++)
	{
		uint8_t		oldAlpha = layer.tintColor[i].a;

		layer.tintColor[i] = tintColor;

		if (oldAlpha != layer.tintColor[i].a)
			_updateStateOpacity(i);
	}
	return true;
}


bool WgMultiBlockSkin::SetLayerTint(int layerIdx, std::initializer_list<WgStateColor> stateColors)
{
	auto& layer = m_layers.at(layerIdx-1);

    // Set default color for normal state.
	layer.tintColor[0] = WgColor::White;

	//

	for (WgStateColor stateColor : stateColors)
	{
		int index = WgUtil::_stateToIndex(stateColor.state);

		uint8_t		oldAlpha = layer.tintColor[index].a;

		layer.tintColor[index] = stateColor.color;

		if (oldAlpha != stateColor.color.a)
			_updateStateOpacity(index);

        layer.stateColorMask.setBit(index);
	}

	// Fill in fallback states and update opacity flag

	for (int i = 0; i < WG_NB_STATES; i++)
	{
        if (!layer.stateColorMask.bit(i))
		{
			uint8_t		oldAlpha = layer.tintColor[i].a;

            int fallbackIndex = wg::Util::bestStateIndexMatch(i, layer.stateBlockMask);
			layer.tintColor[i] = layer.tintColor[fallbackIndex];

			if (oldAlpha != layer.tintColor[i].a )
				_updateStateOpacity(i);
		}
	}

	//

	return true;
}

//____ SetLayerBlendMode() ____________________________________________________

bool WgMultiBlockSkin::SetLayerBlendMode(int layerIdx, WgBlendMode blendMode)
{
	auto& layer = m_layers.at(layerIdx - 1);

	if (layer.blendMode != blendMode)
	{
		layer.blendMode = blendMode;

		for (int i = 0; i < WG_NB_STATES; i++)
			_updateStateOpacity(i);
	}

	return true;
}



//____ render() _______________________________________________________________

void WgMultiBlockSkin::Render( wg::GfxDevice * pDevice, WgState state, const WgRect& _canvas, int scale ) const
{
	if (m_layers.empty() || m_blockSize.w <= 0 || m_blockSize.h <= 0 )
		return;

	int stateIndex = WgUtil::_stateToIndex(state);

	WgBlendMode orgBlendMode = pDevice->blendMode();
	WgColor		orgTintColor = pDevice->tintColor();

	WgBlendMode blendMode = orgBlendMode;
	WgColor		tintColor = orgTintColor;
	WgColor		mixedTint = orgTintColor;

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


        
		const WgRect&	src = WgRect(layer.blockOfs[stateIndex], m_blockSize);

		const WgBorders&    sourceBorders = m_frame.scale(m_scale);
		const WgBorders     canvasBorders = m_frame.scale(scale);

        pDevice->setBlitSource(layer.pSurface->RealSurface());
        pDevice->blitNinePatch(_canvas, canvasBorders, src, sourceBorders);
        
	}

	if (mixedTint != orgTintColor)
		pDevice->setTintColor(orgTintColor);

	if (blendMode != orgBlendMode)
		pDevice->setBlendMode(orgBlendMode);
}


//____ minSize() ______________________________________________________________

WgSize WgMultiBlockSkin::MinSize(int scale) const
{
	WgSize content = WgExtendedSkin::MinSize(scale);
	WgSize frame = m_frame.scale(scale).size();
	return WgSize( std::max(content.w, frame.w), std::max(content.h, frame.h) );
}

//____ preferredSize() ________________________________________________________

WgSize WgMultiBlockSkin::PreferredSize(int scale) const
{
	WgSize sz = WgExtendedSkin::PreferredSize(scale);
	return WgSize( std::max((m_blockSize.w*scale) / m_scale,sz.w), std::max((m_blockSize.h*scale) / m_scale,sz.h) );
}

//____ sizeForContent() _______________________________________________________

WgSize WgMultiBlockSkin::SizeForContent( const WgSize contentSize, int scale ) const
{
	WgSize sz = WgExtendedSkin::SizeForContent(contentSize, scale);
	WgSize min = m_frame.scale(scale).size();

	return WgSize( std::max(sz.w,min.w), std::max(sz.h,min.h) );
}

//____ markTest() _____________________________________________________________

bool WgMultiBlockSkin::MarkTest( const WgCoord& _ofs, const WgSize& canvas, WgState state, int opacityTreshold, int scale ) const
{
	if (m_layers.empty() || m_blockSize.w <= 0 || m_blockSize.h <= 0)
		return false;

	int stateIndex = WgUtil::_stateToIndex(state);

	for (auto& layer : m_layers)
	{
		int alpha = 0;
		if (IsOpaque(state))
			alpha = 255;
		else if( layer.blendMode == WgBlendMode::Blend )
		{
			WgCoord ofs = _ofs;

			WgBorders canvasFrame = m_frame.scale(scale);
			WgBorders sourceFrame = m_frame.scale(m_scale);

			// Determine in which section the cordinate is (0-2 for x and y).

			int	xSection = 0;
			int ySection = 0;

			if (ofs.x >= canvas.w - canvasFrame.right)
				xSection = 2;
			else if (ofs.x > canvasFrame.left)
				xSection = 1;

			if (ofs.y >= canvas.h - canvasFrame.bottom)
				ySection = 2;
			else if (ofs.y > canvasFrame.top)
				ySection = 1;

			// Convert ofs.x to X-offset in bitmap, taking stretch/tile section into account.

			if (xSection == 0)
			{
				ofs.x = (ofs.x * m_scale) / scale;
			}
			else if (xSection == 2)
			{
				ofs.x = ofs.x - (canvas.w - canvasFrame.right);           // Offset in right border of canvas
				ofs.x = (ofs.x * m_scale) / scale;            // Scale from canvas to source coordinates
				ofs.x += m_blockSize.w - sourceFrame.right;          // Add offset for right border
			}
			else if (xSection == 1)
			{
				int tileAreaWidth = m_blockSize.w - sourceFrame.width();

				int canvasStretchWidth = canvas.w - canvasFrame.width();	// Width of stretch-area on screen.

				ofs.x = ofs.x - canvasFrame.left;               // Offset in middle section of canvas
				ofs.x = (ofs.x * m_scale) / scale;        // Scale from canvas to source offset
				ofs.x = (int)((ofs.x / (float)canvasStretchWidth)*tileAreaWidth) + sourceFrame.left;
			}


			// Convert ofs.y to Y-offset in bitmap, taking stretch/tile section into account.

			if (ySection == 0)
			{
				ofs.y = (ofs.y * m_scale) / scale;
			}
			if (ySection == 2)
			{
				ofs.y = ofs.y - (canvas.h - canvasFrame.bottom);           // Offset in bottom border of canvas
				ofs.y = (ofs.y * m_scale) / scale;            // Scale from canvas to source coordinates
				ofs.y += m_blockSize.h - sourceFrame.bottom;          // Add offset for bottom border
			}
			else if (ySection == 1)
			{
				int tileAreaHeight = m_blockSize.h - sourceFrame.height();

				int canvasStretchHeight = canvas.h - canvasFrame.height();	// Height of stretch-area on screen.

				ofs.y = ofs.y - canvasFrame.top;               // Offset in middle section of canvas
				ofs.y = (ofs.y * m_scale) / scale;        // Scale from canvas to source offset
				ofs.y = (int)((ofs.y / (float)canvasStretchHeight)*tileAreaHeight) + sourceFrame.top;
			}

			WgCoord srcOfs = layer.blockOfs[stateIndex];

			alpha = layer.pSurface->GetOpacity(srcOfs.x + ofs.x, srcOfs.y + ofs.y);
		}

		if (alpha >= opacityTreshold)
			return true;
	}

	return false;
}

//____ isOpaque() _____________________________________________________________

bool WgMultiBlockSkin::IsOpaque() const
{
	return m_bIsOpaque;
}

bool WgMultiBlockSkin::IsOpaque( WgState state ) const
{
	return m_bIsOpaque;
}

bool WgMultiBlockSkin::IsOpaque( const WgRect& rect, const WgSize& canvasSize, WgState state, int scale ) const
{
	return m_bIsOpaque;
}

//____ isStateIdentical() _____________________________________________________

bool WgMultiBlockSkin::IsStateIdentical( WgState state, WgState comparedTo ) const
{
	int i1 = WgUtil::_stateToIndex(state);
	int i2 = WgUtil::_stateToIndex(comparedTo);

	for (auto& layer : m_layers)
	{
		if (layer.blockOfs[i1] != layer.blockOfs[i2] || layer.tintColor[i1] != layer.tintColor[i2])
			return false;
	}

	return WgExtendedSkin::IsStateIdentical(state, comparedTo);
}

//____ _updateStateOpacity() __________________________________________________

void WgMultiBlockSkin::_updateStateOpacity(int stateIdx)
{
	bool bOpaque = false;

	for (auto& layer : m_layers)
	{
		if (layer.blendMode == WgBlendMode::Replace || (layer.blendMode == WgBlendMode::Blend && layer.pSurface->IsOpaque() && layer.tintColor[stateIdx].a == 255) )
		{
			bOpaque = true;
			break;
		}
	}

	if (bOpaque != m_bStateOpaque[stateIdx])
	{
		m_bStateOpaque[stateIdx] = bOpaque;

		if (bOpaque != m_bIsOpaque)
		{
			m_bIsOpaque = true;
			for( int i = 0 ; i < WG_NB_STATES ; i++ )
				if (m_bStateOpaque[i] )
				{
					m_bIsOpaque = false;
					break;
				}
		}
	}
}

