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
#ifndef WG_MULTIBLOCKSKIN2_DOT_H
#define WG_MULTIBLOCKSKIN2_DOT_H
#pragma once

#include <wg_extendedskin.h>
#include <wg3_surface.h>
#include <wg3_types.h>

#include <wg_color.h>

#include <vector>

class WgMultiBlockSkin;
typedef	WgSmartChildPtr<WgMultiBlockSkin, WgExtendedSkinPtr>	WgMultiBlockSkinPtr;



struct WgStateColor
{
	WgState		state;
    WgColor		color;
};

	
class WgMultiBlockSkin : public WgExtendedSkin
{
	//TODO: Add sanity-checking to all Set-methods.
	//TODO: Optimize rendering based on invisibleSections and opaqueSections!
	
public:

	//.____ Creation __________________________________________

	static WgMultiBlockSkinPtr Create(WgSize blockSize, WgBorders frame = WgBorders(0) );
	
	//.____ Rendering ________________________________________________

    void	Render( wg::GfxDevice * pDevice, WgState state,  const WgRect& _canvas, int scale ) const override;
	
	//.____ Geometry _________________________________________________

	WgSize	MinSize( int scale ) const override;
	WgSize	PreferredSize( int scale ) const override;
	
	WgSize	SizeForContent( const WgSize contentSize, int scale ) const override;

	//.____ Appearance _________________________________________________

    int		AddLayer(wg::Surface * pSurf, WgCoord ofs);
    int		AddLayer(wg::Surface * pSurf, WgCoord blockStartOfs, WgSize blockPitch, std::initializer_list<WgState> stateBlocks);

	bool	SetLayerTint(int layerIdx, WgColor tintColor);
	bool	SetLayerTint(int layerIdx, std::initializer_list<WgStateColor> stateColors);
	bool	SetLayerBlendMode(int layerIdx, WgBlendMode blendMode);

	
	//.____ Misc ____________________________________________________	

	bool	MarkTest( const WgCoord& ofs, const WgSize& canvas, WgState state, int opacityTreshold, int scale ) const override;
	
	bool	IsOpaque() const override;
	bool	IsOpaque( WgState state ) const override;
	bool	IsOpaque( const WgRect& WgRect, const WgSize& canvasSize, WgState state, int scale ) const override;
	
	bool	IsStateIdentical( WgState state, WgState comparedTo ) const;

private:

	struct LayerData
	{
        wg::Surface *	pSurface;
		WgBlendMode		blendMode;
        wg::Bitmask<uint32_t>   stateBlockMask = 0;
        wg::Bitmask<uint32_t>   stateColorMask = 1;         // Normal state always exist for colors and is by default white.
        
        WgCoord			blockOfs[WG_NB_STATES];		// Block for each state
		WgColor			tintColor[WG_NB_STATES];
//		WgOrigo			placementOrigo;
//		WgCoord			placementOfs;
//		WgSize			dimensions;                   // Stored in pixels
//		int				m_tiledSections;
	};

	
	WgMultiBlockSkin(WgSize blockSize, WgBorders frame);
	~WgMultiBlockSkin() {};

	void			_updateStateOpacity( int stateIdx );

	WgSize			m_blockSizePoints;					// Stored in points

	WgSize			m_blockSize;						// Stored in pixels
	int 			m_scale = WG_SCALE_BASE;
	bool			m_bIsOpaque;
	WgBorders		m_frame;                        // Stored in points

	std::vector<LayerData>	m_layers;

	bool			m_bStateOpaque[WG_NB_STATES];

};
	

#endif //WG_MULTIBLOCKSKIN2_DOT_H
