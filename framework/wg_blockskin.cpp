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

#include <algorithm>


//____ create() _______________________________________________________________

WgBlockSkinPtr WgBlockSkin::Create()
{
	return WgBlockSkinPtr(new WgBlockSkin());
}


WgBlockSkinPtr WgBlockSkin::CreateStatic( wg::Surface * pSurface, WgRect block, WgBorders frame )
{
	if( !pSurface || frame.width() >= block.w || frame.height() >= block.h ||
		block.x < 0 || pSurface->size().w < block.right() ||
		block.y < 0 || pSurface->size().h < block.bottom() )
		return 0;

	WgBlockSkin * pSkin = new WgBlockSkin();
	pSkin->SetSurface( pSurface );
	pSkin->SetBlockGeo( block.size(), frame );
	pSkin->SetAllBlocks( block.pos() );
	return WgBlockSkinPtr(pSkin);
}

WgBlockSkinPtr WgBlockSkin::CreateEnable( wg::Surface * pSurface, WgSize blockSize, WgCoord ofsEnabled, WgCoord ofsDisabled, WgBorders frame )
{
	if( !pSurface || frame.width() >= blockSize.w || frame.height() >= blockSize.h ||
		pSurface->size().w < ofsEnabled.x + blockSize.w ||
		pSurface->size().w < ofsDisabled.x + blockSize.w ||
		pSurface->size().h < ofsEnabled.y + blockSize.h ||
		pSurface->size().h < ofsDisabled.y + blockSize.h )
		return 0;

	WgBlockSkin * pSkin = new WgBlockSkin();
	pSkin->SetSurface( pSurface );
	pSkin->SetBlockGeo( blockSize, frame );
	pSkin->SetAllBlocks( ofsEnabled );
	pSkin->SetDisabledBlock( ofsDisabled );
	return WgBlockSkinPtr(pSkin);
}

WgBlockSkinPtr WgBlockSkin::CreateClickable( wg::Surface * pSurface, WgSize blockGeo, WgCoord blockStartOfs, WgSize blockPitch, WgBorders blockFrame )
{
	// if( !pSurface || blockFrame.width() >= blockGeo.w || blockFrame.height() >= blockGeo.h ||
	// 	pSurface->PixelSize().w < blockStartOfs.x + blockGeo.w + blockPitch.w*3 ||
	// 	pSurface->PixelSize().h < blockStartOfs.y + blockGeo.h + blockPitch.h*3 )
	// 	return 0;

	WgBlockSkin * pSkin = new WgBlockSkin();
	pSkin->SetSurface( pSurface );
	pSkin->SetBlockGeo( blockGeo, blockFrame );

	WgCoord blockOfs = blockStartOfs;

	pSkin->SetAllBlocks( blockOfs );
	blockOfs += WgCoord(blockPitch.w,blockPitch.h);
	pSkin->SetHoveredBlocks( blockOfs );
	blockOfs += WgCoord(blockPitch.w,blockPitch.h);
	pSkin->SetPressedBlocks( blockOfs );
	blockOfs += WgCoord(blockPitch.w,blockPitch.h);
	pSkin->SetDisabledBlock( blockOfs );
	return WgBlockSkinPtr(pSkin);
}

WgBlockSkinPtr WgBlockSkin::CreateSelectable( wg::Surface * pSurface, WgSize blockGeo, WgCoord blockStartOfs, WgSize blockPitch, WgBorders blockFrame )
{
	WgBlockSkin * pSkin = new WgBlockSkin();
	pSkin->SetSurface( pSurface );
	pSkin->SetBlockGeo( blockGeo, blockFrame );

	WgCoord blockOfs = blockStartOfs;

	pSkin->SetAllBlocks( blockOfs );
	blockOfs += WgCoord(blockPitch.w,blockPitch.h);
	pSkin->SetSelectedBlocks( blockOfs );
	blockOfs += WgCoord(blockPitch.w,blockPitch.h);
	pSkin->SetDisabledBlock( blockOfs );
	return WgBlockSkinPtr(pSkin);
}

WgBlockSkinPtr WgBlockSkin::CreateClickSelectable( wg::Surface * pSurface, WgSize blockGeo, WgCoord blockStartOfs, WgSize blockPitch, WgBorders blockFrame )
{
	WgBlockSkin * pSkin = new WgBlockSkin();
	pSkin->SetSurface( pSurface );
	pSkin->SetBlockGeo( blockGeo, blockFrame );

	WgCoord blockOfs = blockStartOfs;

	pSkin->SetStateBlock( WgStateEnum::Normal, blockOfs );				// Normal
	pSkin->SetStateBlock( WgStateEnum::Focused, blockOfs );
	blockOfs += WgCoord(blockPitch.w,blockPitch.h);

	pSkin->SetStateBlock( WgStateEnum::Hovered, blockOfs );				// Hovered, not selected
	pSkin->SetStateBlock( WgStateEnum::HoveredFocused, blockOfs );
	pSkin->SetStateBlock( WgStateEnum::Pressed, blockOfs );
	pSkin->SetStateBlock( WgStateEnum::PressedFocused, blockOfs );
	blockOfs += WgCoord(blockPitch.w,blockPitch.h);

	pSkin->SetStateBlock( WgStateEnum::Selected, blockOfs );			// Selected, not hovered
	pSkin->SetStateBlock( WgStateEnum::SelectedFocused, blockOfs );
	blockOfs += WgCoord(blockPitch.w,blockPitch.h);

	pSkin->SetStateBlock( WgStateEnum::SelectedHovered, blockOfs );	// Selected and hovered
	pSkin->SetStateBlock( WgStateEnum::SelectedHoveredFocused, blockOfs );
	pSkin->SetStateBlock( WgStateEnum::SelectedPressed, blockOfs );
	pSkin->SetStateBlock( WgStateEnum::SelectedPressedFocused, blockOfs );
	blockOfs += WgCoord(blockPitch.w,blockPitch.h);

	pSkin->SetStateBlock( WgStateEnum::Disabled, blockOfs );			// Disabled
	return WgBlockSkinPtr(pSkin);
}

/** Usable for state button etc, where a widget is normal/hovered/pressed/disabled as well as selected/unselected
*/
WgBlockSkinPtr WgBlockSkin::CreateClickSelectableWidget( wg::Surface * pSurface, WgSize blockGeo, WgCoord blockStartOfs, WgSize blockPitch, WgBorders blockFrame )
{
	WgBlockSkin * pSkin = new WgBlockSkin();
	pSkin->SetSurface( pSurface );
	pSkin->SetBlockGeo( blockGeo, blockFrame );

	WgCoord blockOfs = blockStartOfs;

	pSkin->SetStateBlock( WgStateEnum::Normal, blockOfs );				// Normal
	pSkin->SetStateBlock( WgStateEnum::Focused, blockOfs );
	blockOfs += WgCoord(blockPitch.w,blockPitch.h);

	pSkin->SetStateBlock( WgStateEnum::Hovered, blockOfs );				// Hovered
	pSkin->SetStateBlock( WgStateEnum::HoveredFocused, blockOfs );
	blockOfs += WgCoord(blockPitch.w,blockPitch.h);

	pSkin->SetStateBlock( WgStateEnum::Pressed, blockOfs );				// Pressed
	pSkin->SetStateBlock( WgStateEnum::PressedFocused, blockOfs );
	blockOfs += WgCoord(blockPitch.w,blockPitch.h);

	pSkin->SetStateBlock( WgStateEnum::Selected, blockOfs );
	pSkin->SetStateBlock( WgStateEnum::SelectedFocused, blockOfs );
	blockOfs += WgCoord(blockPitch.w,blockPitch.h);

	pSkin->SetStateBlock( WgStateEnum::SelectedHovered, blockOfs );
	pSkin->SetStateBlock( WgStateEnum::SelectedHoveredFocused, blockOfs );
	blockOfs += WgCoord(blockPitch.w,blockPitch.h);

	pSkin->SetStateBlock( WgStateEnum::SelectedPressed, blockOfs );
	pSkin->SetStateBlock( WgStateEnum::SelectedPressedFocused, blockOfs );
	blockOfs += WgCoord(blockPitch.w,blockPitch.h);

	pSkin->SetStateBlock( WgStateEnum::Disabled, blockOfs );

	return WgBlockSkinPtr(pSkin);
}




WgBlockSkinPtr WgBlockSkin::CreateStaticFromSurface( wg::Surface * pSurface, WgBorders frame )
{
	WgBlockSkin * pSkin = new WgBlockSkin();
	pSkin->SetSurface( pSurface );
	pSkin->SetBlockGeo( pSurface->size()/pSurface->scale(), frame );
	pSkin->SetAllBlocks( WgCoord(0,0) );
	return WgBlockSkinPtr(pSkin);
}

WgBlockSkinPtr WgBlockSkin::CreateEnableFromSurface( wg::Surface * pSurface, int blockSpacing, WgBorders blockFrame )
{
	WgBlockSkin * pSkin = new WgBlockSkin();
	pSkin->SetSurface( pSurface );

	WgSize	sz = WgSize( ((pSurface->size().w/pSurface->scale())-blockSpacing)/2, (pSurface->size().h/pSurface->scale()) );

	pSkin->SetBlockGeo( sz, blockFrame );
	pSkin->SetAllBlocks( WgCoord(0,0) );
	pSkin->SetDisabledBlock( WgCoord(sz.w+blockSpacing,0) );
	return WgBlockSkinPtr(pSkin);
}

WgBlockSkinPtr WgBlockSkin::CreateClickableFromSurface( wg::Surface * pSurface, int blockSpacing, WgBorders blockFrame )
{
	WgSize	blockSize = WgSize( (pSurface->size().w-blockSpacing*3)/4, pSurface->size().h );
	return CreateClickable( pSurface, blockSize, WgCoord(0,0), WgSize(blockSize.w+blockSpacing,0), blockFrame );
}

WgBlockSkinPtr WgBlockSkin::CreateSelectableFromSurface( wg::Surface * pSurface, int blockSpacing, WgBorders blockFrame )
{
	WgSize	blockSize( (pSurface->size().w-blockSpacing*2)/3, pSurface->size().h );
	return CreateSelectable( pSurface, blockSize, WgCoord(0,0), WgSize(blockSize.w+blockSpacing,0), blockFrame );
}

WgBlockSkinPtr WgBlockSkin::CreateClickSelectableFromSurface( wg::Surface * pSurface, int blockSpacing, WgBorders blockFrame )
{
	WgSize	blockSize( (pSurface->size().w-blockSpacing*4)/5, pSurface->size().h );
	return CreateClickSelectable( pSurface, blockSize, WgCoord(0,0), WgSize(blockSize.w+blockSpacing,0), blockFrame );
}

//____ Constructor ____________________________________________________________

WgBlockSkin::WgBlockSkin()
{

	m_pSurface = 0;
	m_tiledSections = 0;
	m_bOpaqueBlocks = false;

	for( int i = 0 ; i < WG_NB_STATES ; i++ )
	{
		m_state[i].invisibleSections = 0;
		m_state[i].opaqueSections = 0;
	}
}


//____ setBlockGeo() __________________________________________________________

bool WgBlockSkin::SetBlockGeo( WgSize size, WgBorders frame )
{
    // NOTE! size is in pixels, frame is in base-size pixels. This is API inconsistency!
    
	if( size.w <= frame.width() || size.h <= frame.height() )
		return false;

	m_dimensions	= (size*m_scale)/WG_SCALE_BASE;
	m_frame			= frame;
	return true;
}

//____ setSurface() ______________________________________________________

void WgBlockSkin::SetSurface( wg::Surface * pSurf )
{
	m_pSurface = pSurf;
	if( m_pSurface )
	{
		m_bOpaqueBlocks = pSurf->isOpaque();
		m_scale = m_pSurface->scale()*4096;
	}
	else
	{
		m_bOpaqueBlocks = false;
		m_scale = WG_SCALE_BASE;		
	}
}

//____ setStateBlock() ________________________________________________________

void WgBlockSkin::SetStateBlock( WgStateEnum state, const WgCoord& ofs )
{
	m_state[WgUtil::_stateToIndex(state)].ofs = (ofs*m_scale)/WG_SCALE_BASE;
}

//____ setAllBlocks() _________________________________________________________

void WgBlockSkin::SetAllBlocks( const WgCoord& ofs )
{
	for( int i = 0 ; i < WG_NB_STATES ; i++ )
		m_state[i].ofs = (ofs*m_scale)/WG_SCALE_BASE;

}

//____ setDisabledBlock() _____________________________________________________

void WgBlockSkin::SetDisabledBlock( const WgCoord& ofs )
{
	m_state[WgUtil::_stateToIndex(WgStateEnum::Disabled)].ofs = (ofs*m_scale)/WG_SCALE_BASE;
}

//____ setHoveredBlocks() _____________________________________________________

void WgBlockSkin::SetHoveredBlocks( const WgCoord& ofs )
{
	m_state[WgUtil::_stateToIndex(WgStateEnum::Hovered)].ofs = (ofs*m_scale)/WG_SCALE_BASE;
	m_state[WgUtil::_stateToIndex(WgStateEnum::SelectedHovered)].ofs = (ofs*m_scale)/WG_SCALE_BASE;
	m_state[WgUtil::_stateToIndex(WgStateEnum::HoveredFocused)].ofs = (ofs*m_scale)/WG_SCALE_BASE;
	m_state[WgUtil::_stateToIndex(WgStateEnum::SelectedHoveredFocused)].ofs = (ofs*m_scale)/WG_SCALE_BASE;

	SetPressedBlocks( ofs );
}

//____ setPressedBlocks() _____________________________________________________

void WgBlockSkin::SetPressedBlocks( const WgCoord& ofs )
{
	m_state[WgUtil::_stateToIndex(WgStateEnum::Pressed)].ofs = (ofs*m_scale)/WG_SCALE_BASE;
	m_state[WgUtil::_stateToIndex(WgStateEnum::SelectedPressed)].ofs = (ofs*m_scale)/WG_SCALE_BASE;
	m_state[WgUtil::_stateToIndex(WgStateEnum::PressedFocused)].ofs = (ofs*m_scale)/WG_SCALE_BASE;
	m_state[WgUtil::_stateToIndex(WgStateEnum::SelectedPressedFocused)].ofs = (ofs*m_scale)/WG_SCALE_BASE;
}

//____ setSelectedBlocks() ____________________________________________________

void WgBlockSkin::SetSelectedBlocks( const WgCoord& ofs )
{
	m_state[WgUtil::_stateToIndex(WgStateEnum::Selected)].ofs = (ofs*m_scale)/WG_SCALE_BASE;
	m_state[WgUtil::_stateToIndex(WgStateEnum::SelectedFocused)].ofs = (ofs*m_scale)/WG_SCALE_BASE;
	m_state[WgUtil::_stateToIndex(WgStateEnum::SelectedHovered)].ofs = (ofs*m_scale)/WG_SCALE_BASE;
	m_state[WgUtil::_stateToIndex(WgStateEnum::SelectedHoveredFocused)].ofs = (ofs*m_scale)/WG_SCALE_BASE;
	m_state[WgUtil::_stateToIndex(WgStateEnum::SelectedPressed)].ofs = (ofs*m_scale)/WG_SCALE_BASE;
	m_state[WgUtil::_stateToIndex(WgStateEnum::SelectedPressedFocused)].ofs = (ofs*m_scale)/WG_SCALE_BASE;
}

//____ setTiled() _____________________________________________________________

void WgBlockSkin::SetTiled( bool bTiled )
{
	_setBitFlag( m_tiledSections, (int)WgOrigo::North, bTiled );
	_setBitFlag( m_tiledSections, (int)WgOrigo::South, bTiled );
	_setBitFlag( m_tiledSections, (int)WgOrigo::East, bTiled );
	_setBitFlag( m_tiledSections, (int)WgOrigo::West, bTiled );
	_setBitFlag( m_tiledSections, (int)WgOrigo::Center, bTiled );
}

//____ setTiledTopBorder() ____________________________________________________

void WgBlockSkin::SetTiledTopBorder( bool bTiled )
{
	_setBitFlag( m_tiledSections, (int)WgOrigo::North, bTiled );
}

//____ setTiledBottomBorder() _________________________________________________

void WgBlockSkin::SetTiledBottomBorder( bool bTiled )
{
	_setBitFlag( m_tiledSections, (int)WgOrigo::South, bTiled );
}

//____ setTiledLeftBorder() ___________________________________________________

void WgBlockSkin::SetTiledLeftBorder( bool bTiled )
{
	_setBitFlag( m_tiledSections, (int)WgOrigo::West, bTiled );
}

//____ setTiledRightBorder() __________________________________________________

void WgBlockSkin::SetTiledRightBorder( bool bTiled )
{
	_setBitFlag( m_tiledSections, (int)WgOrigo::East, bTiled );
}

//____ setTiledCenter() _______________________________________________________

void WgBlockSkin::SetTiledCenter( bool bTiled )
{
	_setBitFlag( m_tiledSections, (int)WgOrigo::Center, bTiled );
}

//____ setTint() ______________________________________________________________

void WgBlockSkin::SetTint(WgColor tint)
{
	m_tintColor = tint;
}

//____ setBlendMode() _________________________________________________________

void WgBlockSkin::SetBlendMode(WgBlendMode blendMode)
{
	m_blendMode = blendMode;
	m_bHasBlendMode = true;
}



//____ optimizeRenderMethods() ________________________________________________

void WgBlockSkin::OptimizeRenderMethods()
{
    //TODO: Scaling support has probably broken this method!
    
	// Handle non-alpha surfaces the easy way

	if( m_pSurface->isOpaque() )
	{
		for( int i = 0 ; i < WG_NB_STATES ; i++ )
		{
			m_state[i].invisibleSections = 0;
			m_state[i].opaqueSections = ALL_SECTIONS;
		}
		return;
	}

	// Clear all flags

	for( int i = 0 ; i < WG_NB_STATES ; i++ )
	{
		m_state[i].invisibleSections = 0;
		m_state[i].opaqueSections = 0;
	}

	// Bail out if we have no surface

	if( !m_pSurface )
		return;

	//

	m_pSurface->lock( WgAccessMode::ReadOnly );

	for( int i = 0 ; i < WG_NB_STATES ; i++ )
	{
		int x1 = m_state[i].ofs.x;
		int x2 = m_state[i].ofs.x + m_frame.left;
		int x3 = m_state[i].ofs.x + m_dimensions.w - m_frame.right;

		int y1 = m_state[i].ofs.y;
		int y2 = m_state[i].ofs.y + m_frame.top;
		int y3 = m_state[i].ofs.y + m_dimensions.h - m_frame.bottom;

		int centerW = m_dimensions.w - m_frame.width();
		int	centerH = m_dimensions.h - m_frame.height();

		if( m_frame.top > 0 )
		{
			if( m_frame.left > 0 )
				_scanStateBlockSectionArea( &m_state[i], WgOrigo::NorthWest, WgRect(x1, y1, m_frame.left, m_frame.top) );

			_scanStateBlockSectionArea( &m_state[i], WgOrigo::North, WgRect(x2, y1, centerW, m_frame.top) );

			if( m_frame.right > 0 )
				_scanStateBlockSectionArea( &m_state[i], WgOrigo::NorthEast, WgRect(x3, y1, m_frame.right, m_frame.top) );
		}

		if( centerH > 0 )
		{
			if( m_frame.left > 0 )
				_scanStateBlockSectionArea( &m_state[i], WgOrigo::West, WgRect(x1, y2, m_frame.left, centerH) );

			_scanStateBlockSectionArea( &m_state[i], WgOrigo::Center, WgRect(x2, y2, centerW, centerH) );

			if( m_frame.right > 0 )
				_scanStateBlockSectionArea( &m_state[i], WgOrigo::East, WgRect(x3, y2, m_frame.right, centerH) );
		}

		if( m_frame.bottom > 0 )
		{
			if( m_frame.left > 0 )
				_scanStateBlockSectionArea( &m_state[i], WgOrigo::SouthWest, WgRect(x1, y3, m_frame.left, m_frame.bottom) );

			_scanStateBlockSectionArea( &m_state[i], WgOrigo::South, WgRect(x2, y3, centerW, m_frame.bottom) );

			if( m_frame.right > 0 )
				_scanStateBlockSectionArea( &m_state[i], WgOrigo::SouthEast, WgRect(x3, y3, m_frame.right, m_frame.bottom) );
		}
	}

	m_pSurface->unlock();
}

//_____ _scanStateBlockSectionArea() __________________________________________

void WgBlockSkin::_scanStateBlockSectionArea( StateData * pState, WgOrigo section, const WgRect& sectionArea )
{
	const WgPixelFormat * pFormat = m_pSurface->pixelDescription();
	int pitch = m_pSurface->pitch();

	if( pFormat->format != WgPixelType::BGRA_8 )
		return;												// Only supports ARGB_8 for the moment.

	unsigned char * p = ((unsigned char*)m_pSurface->pixels()) + sectionArea.x * pFormat->bits/8 + sectionArea.y * pitch;

	unsigned int alphaAcc = 0;

	for( int y = 0 ; y < sectionArea.h ; y++ )
	{
		for( int x = 0 ; x < sectionArea.w ; x++ )
			alphaAcc += (int) p[x*4+3];

		p += pitch;
	}

	if( alphaAcc == 0 )
		pState->invisibleSections |= 1 << (int)section;
	else if( alphaAcc == ((unsigned)(sectionArea.w * sectionArea.h)) * 255 )
		pState->opaqueSections |= 1 << (int)section;
}



//____ render() _______________________________________________________________

void WgBlockSkin::Render( wg::GfxDevice * pDevice, WgState state, const WgRect& _canvas, int scale ) const
{
	if( !m_pSurface )
		return;

	const StateData * pState = &m_state[WgUtil::_stateToIndex(state)];
	if( pState->invisibleSections == ALL_SECTIONS )
		return;

	WgColor oldTint;
	if (m_tintColor != WgColor::White)
	{
		oldTint = pDevice->tintColor();
		pDevice->setTintColor(oldTint*m_tintColor);
	}

	WgBlendMode oldBlendMode;
	if (m_bHasBlendMode)
	{
		oldBlendMode = pDevice->blendMode();
		pDevice->setBlendMode(m_blendMode);
	}

    const WgRect&	src		= WgRect(pState->ofs, m_dimensions);


    const WgBorders&    sourceBorders = m_frame.scale(m_scale);
    const WgBorders     canvasBorders = m_frame.scale(scale);

    pDevice->setBlitSource(m_pSurface);
    pDevice->blitNinePatch(_canvas, canvasBorders, src, sourceBorders);

	if (m_bHasBlendMode)
		pDevice->setBlendMode(oldBlendMode);

	if (m_tintColor != WgColor::White)
		pDevice->setTintColor(oldTint);
}


//____ minSize() ______________________________________________________________

WgSize WgBlockSkin::MinSize(int scale) const
{
	WgSize content = WgExtendedSkin::MinSize(scale);
	WgSize frame = m_frame.scale(scale).size();
	return WgSize( std::max(content.w, frame.w), std::max(content.h, frame.h) );
}

//____ preferredSize() ________________________________________________________

WgSize WgBlockSkin::PreferredSize(int scale) const
{
	WgSize sz = WgExtendedSkin::PreferredSize(scale);
	return WgSize( std::max((m_dimensions.w*scale) / m_scale,sz.w), std::max((m_dimensions.h*scale) / m_scale,sz.h) );
}

//____ sizeForContent() _______________________________________________________

WgSize WgBlockSkin::SizeForContent( const WgSize contentSize, int scale ) const
{
	WgSize sz = WgExtendedSkin::SizeForContent(contentSize, scale);
	WgSize min = m_frame.scale(scale).size();

	return WgSize( std::max(sz.w,min.w), std::max(sz.h,min.h) );
}

//____ markTest() _____________________________________________________________

bool WgBlockSkin::MarkTest( const WgCoord& _ofs, const WgSize& canvas, WgState state, int opacityTreshold, int scale ) const
{
	if( !m_pSurface )
		return false;

	int alpha;
	if( IsOpaque( state ) )
		alpha = 255;
	else
	{
		WgCoord ofs = _ofs;

        WgBorders canvasFrame = m_frame.scale(scale);
        WgBorders sourceFrame = m_frame.scale(m_scale);
	
		// Determine in which section the cordinate is (0-2 for x and y).

		int	xSection = 0;
		int ySection = 0;

		if( ofs.x >= canvas.w - canvasFrame.right )
			xSection = 2;
		else if( ofs.x > canvasFrame.left )
			xSection = 1;

		if( ofs.y >= canvas.h - canvasFrame.bottom )
			ySection = 2;
		else if( ofs.y > canvasFrame.top )
			ySection = 1;

		// Convert ofs.x to X-offset in bitmap, taking stretch/tile section into account.

        if( xSection == 0 )
        {
            ofs.x = (ofs.x * m_scale) / scale;
        }
		else if( xSection == 2 )
		{
            ofs.x = ofs.x - (canvas.w - canvasFrame.right);           // Offset in right border of canvas
            ofs.x = (ofs.x * m_scale) / scale;            // Scale from canvas to source coordinates
            ofs.x += m_dimensions.w - sourceFrame.right;          // Add offset for right border
		}
		else if( xSection == 1 )
		{
			int tileAreaWidth = m_dimensions.w - sourceFrame.width();

			bool bTile;

			if( ySection == 0 )
				bTile = (m_tiledSections & (1 << (int)WgOrigo::North)) != 0;
			else if( ySection == 1 )
				bTile = (m_tiledSections & (1 << (int)WgOrigo::Center)) != 0;
			else
				bTile = (m_tiledSections & (1 << (int)WgOrigo::South)) != 0;

			if( bTile )
            {
                ofs.x = ofs.x - canvasFrame.left;               // Offset in middle section of canvas
                ofs.x = (ofs.x * m_scale) / scale;        // Scale from canvas to source offset
                ofs.x = (ofs.x % tileAreaWidth) + sourceFrame.left; // Get offset in source
            }
			else
			{
                int canvasStretchWidth = canvas.w - canvasFrame.width();	// Width of stretch-area on screen.

                ofs.x = ofs.x - canvasFrame.left;               // Offset in middle section of canvas
                ofs.x = (int)((ofs.x / (float)canvasStretchWidth)*tileAreaWidth) + sourceFrame.left;
			}
		}


		// Convert ofs.y to Y-offset in bitmap, taking stretch/tile section into account.

        if( ySection == 0 )
        {
            ofs.y = (ofs.y * m_scale) / scale;
        }
		if( ySection == 2 )
		{
            ofs.y = ofs.y - (canvas.h - canvasFrame.bottom);           // Offset in bottom border of canvas
            ofs.y = (ofs.y * m_scale) / scale;            // Scale from canvas to source coordinates
            ofs.y += m_dimensions.h - sourceFrame.bottom;          // Add offset for bottom border
		}
		else if( ySection == 1 )
		{
			int tileAreaHeight = m_dimensions.h - sourceFrame.height();

			bool bTile;

			if( xSection == 0 )
				bTile = (m_tiledSections & (1 << (int)WgOrigo::West)) != 0;
			else if( xSection == 1 )
				bTile = (m_tiledSections & (1 << (int)WgOrigo::Center)) != 0;
			else
				bTile = (m_tiledSections & (1 << (int)WgOrigo::East)) != 0;

			if( bTile )
            {
                ofs.y = ofs.y - canvasFrame.top;               // Offset in middle section of canvas
                ofs.y = (ofs.y * m_scale) / scale;        // Scale from canvas to source offset
                ofs.y = (ofs.y % tileAreaHeight) + sourceFrame.top; // Get offset in source
            }
			else
			{
                int canvasStretchHeight = canvas.h - canvasFrame.height();	// Height of stretch-area on screen.

                ofs.y = ofs.y - canvasFrame.top;               // Offset in middle section of canvas
                ofs.y = (int)((ofs.y / (float)canvasStretchHeight)*tileAreaHeight) + sourceFrame.top;
			}
		}

		WgCoord srcOfs = m_state[WgUtil::_stateToIndex(state)].ofs;

		alpha = m_pSurface->alpha(srcOfs.x+ofs.x, srcOfs.y+ofs.y);
	}

	return ( alpha >= opacityTreshold);
}

//____ isOpaque() _____________________________________________________________

bool WgBlockSkin::IsOpaque() const
{
	return (m_bOpaqueBlocks && m_tintColor.a == 255);
}

bool WgBlockSkin::IsOpaque( WgState state ) const
{
	if( m_bOpaqueBlocks && m_tintColor.a == 255)
		return true;

	return (m_state[WgUtil::_stateToIndex(state)].opaqueSections == ALL_SECTIONS);
}

bool WgBlockSkin::IsOpaque( const WgRect& rect, const WgSize& canvasSize, WgState state, int scale ) const
{
	// Quick exit in optimal case

	if( m_bOpaqueBlocks && m_tintColor.a == 255)
		return true;

	// Semi-quick exit

	int index = WgUtil::_stateToIndex(state);

	if( rect.w == canvasSize.w && rect.h == canvasSize.h )
		return (m_state[index].opaqueSections == ALL_SECTIONS);

	WgRect center = WgRect(canvasSize) - m_frame.scale(scale);
	if( center.contains(rect) )
        return ( m_state[index].opaqueSections & (1<<(int)WgOrigo::Center) ) != 0;

	//
/*
	To be implemented optimized solution.

	int xParts = 0;
	int yParts = 0;

	if( rect.x < center.x )
		xParts |= 1;
	if( rect.x < center.x + center.w && rect.x + rect.w > center.x )
		xParts |= 2;
	if( rect.x + rect.w > center.x + center.w )
		xParts |= 4;

	if( rect.y < center.y )
		yParts |= 1;
	if( rect.y < center.y + center.h && rect.y + rect.h > center.y )
		yParts |= 2;
	if( rect.y + rect.h > center.y + center.h )
		yParts |= 4;

	int bitmask = lookupTab[xParts][yParts];

*/
	int bitmask = 0;

	if( rect.y < center.y )
	{
		if( rect.x < center.x )
			bitmask |= (1<<(int)WgOrigo::NorthWest);
		if( rect.x < center.x + center.w && rect.x + rect.w > center.x )
			bitmask |= (1<<(int)WgOrigo::North);
		if( rect.x + rect.w > center.x + center.w )
			bitmask |= (1<<(int)WgOrigo::NorthEast);
	}

	if( rect.y < center.y + center.h && rect.y + rect.h > center.y )
	{
		if( rect.x < center.x )
			bitmask |= (1<<(int)WgOrigo::West);
		if( rect.x < center.x + center.w && rect.x + rect.w > center.x )
			bitmask |= (1<<(int)WgOrigo::Center);
		if( rect.x + rect.w > center.x + center.w )
			bitmask |= (1<<(int)WgOrigo::East);
	}

	if( rect.y + rect.h > center.y + center.h )
	{
		if( rect.x < center.x )
			bitmask |= (1<<(int)WgOrigo::SouthWest);
		if( rect.x < center.x + center.w && rect.x + rect.w > center.x )
			bitmask |= (1<<(int)WgOrigo::South);
		if( rect.x + rect.w > center.x + center.w )
			bitmask |= (1<<(int)WgOrigo::SouthEast);
	}


	if( (m_state[index].opaqueSections & bitmask) == bitmask )
		return true;

	return false;
}

//____ IsStateIdentical() _____________________________________________________

bool WgBlockSkin::IsStateIdentical( WgState state, WgState comparedTo ) const
{
	int i1 = WgUtil::_stateToIndex(state);
	int i2 = WgUtil::_stateToIndex(comparedTo);

	return ( m_state[i1].ofs == m_state[i2].ofs && WgExtendedSkin::IsStateIdentical(state,comparedTo) );
}


//____ _setBitFlag() __________________________________________________________

void WgBlockSkin::_setBitFlag( int& bitmask, int bit, bool bSet )
{
	if( bSet )
		bitmask |= 1 << bit;
	else
		bitmask &= ~(1 << bit);

}

