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
#ifndef WG_BLOCKSKIN2_DOT_H
#define WG_BLOCKSKIN2_DOT_H
#pragma once

#include <wg_extendedskin.h>
#include <wg_surface.h>


	
class WgBlockSkin;
typedef	WgSmartChildPtr<WgBlockSkin, WgExtendedSkinPtr>	WgBlockSkinPtr;

	
class WgBlockSkin : public WgExtendedSkin
{
	//TODO: Add sanity-checking to all Set-methods.
	//TODO: Optimize rendering based on invisibleSections and opaqueSections!
	
public:

	//.____ Creation __________________________________________

	static WgBlockSkinPtr Create();
	static WgBlockSkinPtr CreateStatic( WgSurface * pSurface, WgRect block, WgBorders frame = WgBorders(0) );
	static WgBlockSkinPtr CreateEnable( WgSurface * pSurface, WgSize blockSize, WgCoord ofsEnabled, WgCoord ofsDisabled, WgBorders frame = WgBorders(0) );
	static WgBlockSkinPtr CreateClickable( WgSurface * pSurface, WgSize blockGeo, WgCoord blockStartOfs, WgSize blockPitch, WgBorders blockFrame = WgBorders(0) );
	static WgBlockSkinPtr CreateSelectable( WgSurface * pSurface, WgSize blockGeo, WgCoord blockStartOfs, WgSize blockPitch, WgBorders blockFrame = WgBorders(0) );
	static WgBlockSkinPtr CreateClickSelectable(WgSurface * pSurface, WgSize blockGeo, WgCoord blockStartOfs, WgSize blockPitch, WgBorders blockFrame = WgBorders(0) );
	static WgBlockSkinPtr CreateClickSelectableWidget( WgSurface * pSurface, WgSize blockGeo, WgCoord blockStartOfs, WgSize blockPitch, WgBorders blockFrame = WgBorders(0) );
	
	static WgBlockSkinPtr CreateStaticFromSurface( WgSurface * pSurface, WgBorders frame = WgBorders(0) );
	static WgBlockSkinPtr CreateEnableFromSurface( WgSurface * pSurface, int blockSpacing, WgBorders blockFrame = WgBorders(0) );
	static WgBlockSkinPtr CreateClickableFromSurface( WgSurface * pSurface, int blockSpacing, WgBorders blockFrame = WgBorders(0) );
	static WgBlockSkinPtr CreateSelectableFromSurface( WgSurface * pSurface, int blockSpacing, WgBorders blockFrame = WgBorders(0) );
	static WgBlockSkinPtr CreateClickSelectableFromSurface( WgSurface * pSurface, int blockSpacing, WgBorders blockFrame = WgBorders(0) );
	
	//.____ Rendering ________________________________________________

    void	Render( wg::GfxDevice * pDevice, WgState state,  const WgRect& _canvas, int scale ) const override;
	
	//.____ Geometry _________________________________________________

	WgSize	MinSize( int scale ) const override;
	WgSize	PreferredSize( int scale ) const override;
	
	WgSize	SizeForContent( const WgSize contentSize, int scale ) const override;

	//.____ Appearance _________________________________________________

	void	SetSurface( WgSurface * pSurf );
	bool	SetBlockGeo( WgSize size, WgBorders frame = WgBorders(0) );
	void	SetStateBlock( WgStateEnum state, const WgCoord& ofs );
	
	void	SetAllBlocks( const WgCoord& ofs );
	void	SetDisabledBlock( const WgCoord& ofs );
	void	SetHoveredBlocks( const WgCoord& ofs );
	void	SetPressedBlocks( const WgCoord& ofs );
	void	SetSelectedBlocks( const WgCoord& ofs );
	
	void	SetTiled( bool bTiled );
	void	SetTiledTopBorder( bool bTiled );
	void	SetTiledBottomBorder( bool bTiled );
	void	SetTiledLeftBorder( bool bTiled );
	void	SetTiledRightBorder( bool bTiled );
	void	SetTiledCenter( bool bTiled );

	void	SetTint( WgColor tint );
//	bool	SetTint(std::initializer_list<WgStateColor> stateColors);

	void	SetBlendMode( WgBlendMode blendMode );
	WgBlendMode	BlendMode() const { return m_blendMode; }


	//.____ Misc ____________________________________________________	

	bool	MarkTest( const WgCoord& ofs, const WgSize& canvas, WgState state, int opacityTreshold, int scale ) const override;
	
	bool	IsOpaque() const override;
	bool	IsOpaque( WgState state ) const override;
	bool	IsOpaque( const WgRect& WgRect, const WgSize& canvasSize, WgState state, int scale ) const override;
	
	bool	IsStateIdentical( WgState state, WgState comparedTo ) const;
	
	void	OptimizeRenderMethods();

private:
	struct StateData
	{
		WgCoord	ofs;
		int		opaqueSections;
		int		invisibleSections;
	};
	
	WgBlockSkin();
	~WgBlockSkin() {};

	void	_setBitFlag( int& bitmask, int bit, bool bSet );
//	void	_renderNoClip( WgGfxDevice * pDevice, const StateData * pState, const WgRect& _canvas ) const;
	void	_scanStateBlockSectionArea( StateData * pState, WgOrigo section, const WgRect& sectionArea );
	
	static const int ALL_SECTIONS = 0x1FF;
	
	WgSurface *		m_pSurface;
	WgSize			m_dimensions;                   // Stored in pixels
	WgBorders		m_frame;                        // Stored in points
	int				m_tiledSections;
	bool			m_bOpaqueBlocks;
	int 			m_scale = WG_SCALE_BASE;
	
	WgColor			m_tintColor = WgColor::White;
	WgBlendMode		m_blendMode = WgBlendMode::Blend;
	bool			m_bHasBlendMode = false;

	StateData		m_state[WG_NB_STATES];
};
	

#endif //WG_BLOCKSKIN2_DOT_H
