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

using namespace WgUtil;

const char WgBlockSkin::CLASSNAME[] = {"BlockSkin"};

//____ Create() _______________________________________________________________

WgBlockSkinPtr WgBlockSkin::Create()
{
	return WgBlockSkinPtr(new WgBlockSkin());
}


WgBlockSkinPtr WgBlockSkin::CreateStatic( const WgSurfacePtr& pSurface, WgRect block, WgBorder frame )
{
	WgBlockSkin * pSkin = new WgBlockSkin();
	pSkin->SetSurface( pSurface );
	pSkin->SetBlockGeo( block.Size(), frame );
	pSkin->SetAllBlocks( block.Pos() );
	return WgBlockSkinPtr(pSkin);
}

WgBlockSkinPtr WgBlockSkin::CreateEnable( const WgSurfacePtr& pSurface, WgSize blockSize, WgCoord ofsEnabled, WgCoord ofsDisabled, WgBorder frame )
{
	WgBlockSkin * pSkin = new WgBlockSkin();
	pSkin->SetSurface( pSurface );
	pSkin->SetBlockGeo( blockSize, frame );
	pSkin->SetAllBlocks( ofsEnabled );
	pSkin->SetDisabledBlock( ofsDisabled );
	return WgBlockSkinPtr(pSkin);
}

WgBlockSkinPtr WgBlockSkin::CreateClickable( const WgSurfacePtr& pSurface, WgSize blockGeo, WgCoord blockStartOfs, WgSize blockPitch, WgBorder blockFrame )
{
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

WgBlockSkinPtr WgBlockSkin::CreateSelectable( const WgSurfacePtr& pSurface, WgSize blockGeo, WgCoord blockStartOfs, WgSize blockPitch, WgBorder blockFrame )
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

WgBlockSkinPtr WgBlockSkin::CreateClickSelectable( const WgSurfacePtr& pSurface, WgSize blockGeo, WgCoord blockStartOfs, WgSize blockPitch, WgBorder blockFrame )
{
	WgBlockSkin * pSkin = new WgBlockSkin();
	pSkin->SetSurface( pSurface );
	pSkin->SetBlockGeo( blockGeo, blockFrame );

	WgCoord blockOfs = blockStartOfs;

	pSkin->SetStateBlock( WG_STATE_NORMAL, blockOfs );				// Normal
	pSkin->SetStateBlock( WG_STATE_FOCUSED, blockOfs );
	blockOfs += WgCoord(blockPitch.w,blockPitch.h);

	pSkin->SetStateBlock( WG_STATE_HOVERED, blockOfs );				// Hovered, not selected
	pSkin->SetStateBlock( WG_STATE_HOVERED_FOCUSED, blockOfs );
	pSkin->SetStateBlock( WG_STATE_PRESSED, blockOfs );
	pSkin->SetStateBlock( WG_STATE_PRESSED_FOCUSED, blockOfs );
	blockOfs += WgCoord(blockPitch.w,blockPitch.h);

	pSkin->SetStateBlock( WG_STATE_SELECTED, blockOfs );			// Selected, not hovered
	pSkin->SetStateBlock( WG_STATE_FOCUSED_SELECTED, blockOfs );
	blockOfs += WgCoord(blockPitch.w,blockPitch.h);

	pSkin->SetStateBlock( WG_STATE_HOVERED_SELECTED, blockOfs );	// Selected and hovered
	pSkin->SetStateBlock( WG_STATE_HOVERED_FOCUSED_SELECTED, blockOfs );
	pSkin->SetStateBlock( WG_STATE_PRESSED_SELECTED, blockOfs );
	pSkin->SetStateBlock( WG_STATE_PRESSED_FOCUSED_SELECTED, blockOfs );
	blockOfs += WgCoord(blockPitch.w,blockPitch.h);

	pSkin->SetStateBlock( WG_STATE_DISABLED, blockOfs );			// Disabled
	return WgBlockSkinPtr(pSkin);
}

/** Usable for state button etc, where a widget is normal/hovered/pressed/disabled as well as selected/unselected
*/
WgBlockSkinPtr WgBlockSkin::CreateClickSelectableWidget( const WgSurfacePtr& pSurface, WgSize blockGeo, WgCoord blockStartOfs, WgSize blockPitch, WgBorder blockFrame )
{
	WgBlockSkin * pSkin = new WgBlockSkin();
	pSkin->SetSurface( pSurface );
	pSkin->SetBlockGeo( blockGeo, blockFrame );

	WgCoord blockOfs = blockStartOfs;

	pSkin->SetStateBlock( WG_STATE_NORMAL, blockOfs );				// Normal
	pSkin->SetStateBlock( WG_STATE_FOCUSED, blockOfs );
	blockOfs += WgCoord(blockPitch.w,blockPitch.h);

	pSkin->SetStateBlock( WG_STATE_HOVERED, blockOfs );				// Hovered
	pSkin->SetStateBlock( WG_STATE_HOVERED_FOCUSED, blockOfs );
	blockOfs += WgCoord(blockPitch.w,blockPitch.h);

	pSkin->SetStateBlock( WG_STATE_PRESSED, blockOfs );				// Pressed
	pSkin->SetStateBlock( WG_STATE_PRESSED_FOCUSED, blockOfs );
	blockOfs += WgCoord(blockPitch.w,blockPitch.h);

	pSkin->SetStateBlock( WG_STATE_SELECTED, blockOfs );
	pSkin->SetStateBlock( WG_STATE_FOCUSED_SELECTED, blockOfs );
	blockOfs += WgCoord(blockPitch.w,blockPitch.h);

	pSkin->SetStateBlock( WG_STATE_HOVERED_SELECTED, blockOfs );
	pSkin->SetStateBlock( WG_STATE_HOVERED_FOCUSED_SELECTED, blockOfs );
	blockOfs += WgCoord(blockPitch.w,blockPitch.h);

	pSkin->SetStateBlock( WG_STATE_PRESSED_SELECTED, blockOfs );
	pSkin->SetStateBlock( WG_STATE_PRESSED_FOCUSED_SELECTED, blockOfs );
	blockOfs += WgCoord(blockPitch.w,blockPitch.h);

	pSkin->SetStateBlock( WG_STATE_DISABLED, blockOfs );
	blockOfs += WgCoord(blockPitch.w,blockPitch.h);

	pSkin->SetStateBlock( WG_STATE_DISABLED_SELECTED, blockOfs );

	return WgBlockSkinPtr(pSkin);
}




WgBlockSkinPtr WgBlockSkin::CreateStaticFromSurface( const WgSurfacePtr& pSurface, WgBorder frame )
{
	WgBlockSkin * pSkin = new WgBlockSkin();
	pSkin->SetSurface( pSurface );
	pSkin->SetBlockGeo( pSurface->Size(), frame );
	pSkin->SetAllBlocks( WgCoord(0,0) );
	return WgBlockSkinPtr(pSkin);
}

WgBlockSkinPtr WgBlockSkin::CreateEnableFromSurface( const WgSurfacePtr& pSurface, int blockSpacing, WgBorder blockFrame )
{
	WgBlockSkin * pSkin = new WgBlockSkin();
	pSkin->SetSurface( pSurface );

	WgSize	sz = WgSize( (pSurface->Width()-blockSpacing)/2, pSurface->Height() );

	pSkin->SetBlockGeo( sz, blockFrame );
	pSkin->SetAllBlocks( WgCoord(0,0) );
	pSkin->SetDisabledBlock( WgCoord(sz.w+blockSpacing,0) );
	return WgBlockSkinPtr(pSkin);
}

WgBlockSkinPtr WgBlockSkin::CreateClickableFromSurface( const WgSurfacePtr& pSurface, int blockSpacing, WgBorder blockFrame )
{
	WgSize	blockSize = WgSize( (pSurface->Width()-blockSpacing*3)/4, pSurface->Height() );
	return CreateClickable( pSurface, blockSize, WgCoord(0,0), WgSize(blockSize.w+blockSpacing,0), blockFrame );
}

WgBlockSkinPtr WgBlockSkin::CreateSelectableFromSurface( const WgSurfacePtr& pSurface, int blockSpacing, WgBorder blockFrame )
{
	WgSize	blockSize( (pSurface->Width()-blockSpacing*2)/3, pSurface->Height() );
	return CreateSelectable( pSurface, blockSize, WgCoord(0,0), WgSize(blockSize.w+blockSpacing,0), blockFrame );
}

WgBlockSkinPtr WgBlockSkin::CreateClickSelectableFromSurface( const WgSurfacePtr& pSurface, int blockSpacing, WgBorder blockFrame )
{
	WgSize	blockSize( (pSurface->Width()-blockSpacing*4)/5, pSurface->Height() );
	return CreateClickSelectable( pSurface, blockSize, WgCoord(0,0), WgSize(blockSize.w+blockSpacing,0), blockFrame );
}

//____ Constructor ____________________________________________________________

WgBlockSkin::WgBlockSkin()
{

	m_pSurface = 0;
	m_tiledSections = 0;
	m_bIsOpaque = false;

	for( int i = 0 ; i < WG_NB_STATES ; i++ )
	{
		m_state[i].invisibleSections = 0;
		m_state[i].opaqueSections = 0;
	}
}

//____ IsInstanceOf() _________________________________________________________

bool WgBlockSkin::IsInstanceOf( const char * pClassName ) const
{
	if( pClassName==CLASSNAME )
		return true;

	return WgExtendedSkin::IsInstanceOf(pClassName);
}

//____ ClassName() ____________________________________________________________

const char * WgBlockSkin::ClassName( void ) const
{
	return CLASSNAME;
}

//____ Cast() _________________________________________________________________

WgBlockSkinPtr WgBlockSkin::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgBlockSkinPtr( static_cast<WgBlockSkin*>(pObject.RawPtr()) );

	return 0;
}


//____ SetBlockGeo() __________________________________________________________

bool WgBlockSkin::SetBlockGeo( WgSize size, WgBorder frame )
{
	if( size.w <= frame.Width() || size.h <= frame.Height() )
		return false;

	m_dimensions	= size;
	m_frame			= frame;
	return true;
}

//____ SetSurface() ______________________________________________________

void WgBlockSkin::SetSurface( const WgSurfacePtr& pSurf )
{
	m_pSurface = pSurf;
	if( m_pSurface )
		m_bIsOpaque = pSurf->IsOpaque();
	else
		m_bIsOpaque = false;
}

//____ SetStateBlock() ________________________________________________________

void WgBlockSkin::SetStateBlock( WgStateEnum state, const WgCoord& ofs )
{
	m_state[_stateToIndex(state)].ofs = ofs;
}

//____ SetAllBlocks() _________________________________________________________

void WgBlockSkin::SetAllBlocks( const WgCoord& ofs )
{
	for( int i = 0 ; i < WG_NB_STATES ; i++ )
		m_state[i].ofs = ofs;

}

//____ SetDisabledBlock() _____________________________________________________

void WgBlockSkin::SetDisabledBlock( const WgCoord& ofs )
{
	m_state[_stateToIndex(WG_STATE_DISABLED)].ofs = ofs;
}

//____ SetHoveredBlocks() _____________________________________________________

void WgBlockSkin::SetHoveredBlocks( const WgCoord& ofs )
{
	m_state[_stateToIndex(WG_STATE_HOVERED)].ofs = ofs;
	m_state[_stateToIndex(WG_STATE_HOVERED_SELECTED)].ofs = ofs;
	m_state[_stateToIndex(WG_STATE_HOVERED_FOCUSED)].ofs = ofs;
	m_state[_stateToIndex(WG_STATE_HOVERED_FOCUSED_SELECTED)].ofs = ofs;

	SetPressedBlocks( ofs );
}

//____ SetPressedBlocks() _____________________________________________________

void WgBlockSkin::SetPressedBlocks( const WgCoord& ofs )
{
	m_state[_stateToIndex(WG_STATE_PRESSED)].ofs = ofs;
	m_state[_stateToIndex(WG_STATE_PRESSED_SELECTED)].ofs = ofs;
	m_state[_stateToIndex(WG_STATE_PRESSED_FOCUSED)].ofs = ofs;
	m_state[_stateToIndex(WG_STATE_PRESSED_FOCUSED_SELECTED)].ofs = ofs;
}

//____ SetSelectedBlocks() ____________________________________________________

void WgBlockSkin::SetSelectedBlocks( const WgCoord& ofs )
{
	m_state[_stateToIndex(WG_STATE_SELECTED)].ofs = ofs;
	m_state[_stateToIndex(WG_STATE_FOCUSED_SELECTED)].ofs = ofs;
	m_state[_stateToIndex(WG_STATE_HOVERED_SELECTED)].ofs = ofs;
	m_state[_stateToIndex(WG_STATE_HOVERED_FOCUSED_SELECTED)].ofs = ofs;
	m_state[_stateToIndex(WG_STATE_PRESSED_SELECTED)].ofs = ofs;
	m_state[_stateToIndex(WG_STATE_PRESSED_FOCUSED_SELECTED)].ofs = ofs;
}

//____ SetTiled() _____________________________________________________________

void WgBlockSkin::SetTiled( bool bTiled )
{
	_setBitFlag( m_tiledSections, WG_NORTH, bTiled );
	_setBitFlag( m_tiledSections, WG_SOUTH, bTiled );
	_setBitFlag( m_tiledSections, WG_EAST, bTiled );
	_setBitFlag( m_tiledSections, WG_WEST, bTiled );
	_setBitFlag( m_tiledSections, WG_CENTER, bTiled );
}

//____ SetTiledTopBorder() ____________________________________________________

void WgBlockSkin::SetTiledTopBorder( bool bTiled )
{
	_setBitFlag( m_tiledSections, WG_NORTH, bTiled );
}

//____ SetTiledBottomBorder() _________________________________________________

void WgBlockSkin::SetTiledBottomBorder( bool bTiled )
{
	_setBitFlag( m_tiledSections, WG_SOUTH, bTiled );
}

//____ SetTiledLeftBorder() ___________________________________________________

void WgBlockSkin::SetTiledLeftBorder( bool bTiled )
{
	_setBitFlag( m_tiledSections, WG_WEST, bTiled );
}

//____ SetTiledRightBorder() __________________________________________________

void WgBlockSkin::SetTiledRightBorder( bool bTiled )
{
	_setBitFlag( m_tiledSections, WG_EAST, bTiled );
}

//____ SetTiledCenter() _______________________________________________________

void WgBlockSkin::SetTiledCenter( bool bTiled )
{
	_setBitFlag( m_tiledSections, WG_CENTER, bTiled );
}

//____ OptimizeRenderMethods() ________________________________________________

void WgBlockSkin::OptimizeRenderMethods()
{
	// Handle non-alpha surfaces the easy way

	if( m_pSurface->IsOpaque() )
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

	m_pSurface->Lock(WG_READ_ONLY);

	for( int i = 0 ; i < WG_NB_STATES ; i++ )
	{
		WgRect r = WgRect(m_state[i].ofs,m_dimensions);

		int x1 = m_state[i].ofs.x;
		int x2 = m_state[i].ofs.x + m_frame.left;
		int x3 = m_state[i].ofs.x + m_dimensions.w - m_frame.right;

		int y1 = m_state[i].ofs.y;
		int y2 = m_state[i].ofs.y + m_frame.top;
		int y3 = m_state[i].ofs.y + m_dimensions.h - m_frame.bottom;

		int centerW = m_dimensions.w - m_frame.Width();
		int	centerH = m_dimensions.h - m_frame.Height();

		if( m_frame.top > 0 )
		{
			if( m_frame.left > 0 )
				_scanStateBlockSectionArea( &m_state[i], WG_NORTHWEST, WgRect(x1, y1, m_frame.left, m_frame.top) );

			_scanStateBlockSectionArea( &m_state[i], WG_NORTH, WgRect(x2, y1, centerW, m_frame.top) );

			if( m_frame.right > 0 )
				_scanStateBlockSectionArea( &m_state[i], WG_NORTHEAST, WgRect(x3, y1, m_frame.right, m_frame.top) );
		}

		if( centerH > 0 )
		{
			if( m_frame.left > 0 )
				_scanStateBlockSectionArea( &m_state[i], WG_WEST, WgRect(x1, y2, m_frame.left, centerH) );

			_scanStateBlockSectionArea( &m_state[i], WG_CENTER, WgRect(x2, y2, centerW, centerH) );

			if( m_frame.right > 0 )
				_scanStateBlockSectionArea( &m_state[i], WG_EAST, WgRect(x3, y2, m_frame.right, centerH) );
		}

		if( m_frame.bottom > 0 )
		{
			if( m_frame.left > 0 )
				_scanStateBlockSectionArea( &m_state[i], WG_SOUTHWEST, WgRect(x1, y3, m_frame.left, m_frame.bottom) );

			_scanStateBlockSectionArea( &m_state[i], WG_SOUTH, WgRect(x2, y3, centerW, m_frame.bottom) );

			if( m_frame.right > 0 )
				_scanStateBlockSectionArea( &m_state[i], WG_SOUTHEAST, WgRect(x3, y3, m_frame.right, m_frame.bottom) );
		}
	}

	m_pSurface->Unlock();
}

//_____ _scanStateBlockSectionArea() __________________________________________

void WgBlockSkin::_scanStateBlockSectionArea( StateData * pState, WgOrigo section, const WgRect& sectionArea )
{
	const WgPixelFormat * pFormat = m_pSurface->PixelFormat();
	int pitch = m_pSurface->Pitch();

	if( pFormat->type != WG_PIXEL_ARGB_8 )
		return;												// Only supports ARGB_8 for the moment.

	unsigned char * p = ((unsigned char*)m_pSurface->Pixels()) + sectionArea.x * pFormat->bits/8 + sectionArea.y * pitch;

	unsigned int alphaAcc = 0;

	for( int y = 0 ; y < sectionArea.h ; y++ )
	{
		for( int x = 0 ; x < sectionArea.w ; x++ )
			alphaAcc += (int) p[x*4+3];

		p += pitch;
	}

	if( alphaAcc == 0 )
		pState->invisibleSections |= 1 << section;
	else if( alphaAcc == ((unsigned)(sectionArea.w * sectionArea.h)) * 255 )
		pState->opaqueSections |= 1 << section;
}



//____ Render() _______________________________________________________________

void WgBlockSkin::Render( WgGfxDevice * pDevice, const WgRect& _canvas, WgState state, const WgRect& _clip ) const
{
	if( !m_pSurface )
		return;

	const StateData * pState = &m_state[_stateToIndex(state)];
	if( pState->invisibleSections == ALL_SECTIONS )
		return;

	//

	// Shortcuts & optimizations for common special cases.

	WgSize borderSize = m_frame.Size();

	if( _clip.Contains( _canvas ) && borderSize.w <= _canvas.Size().w && borderSize.h <= _canvas.Size().h )
	{
		_renderNoClip( pDevice, pState, _canvas );
		return;
	}

	const WgRect&	src		= WgRect(pState->ofs, m_dimensions);

	if( src.w == _canvas.w && src.h == _canvas.h )
	{
		pDevice->ClipBlit( _clip, m_pSurface, src, _canvas.x, _canvas.y );
		return;
	}

	if( borderSize.w == 0 && borderSize.h == 0 )
	{
		if( m_tiledSections & (1 << WG_CENTER) )
			pDevice->ClipTileBlit( _clip, m_pSurface, src, _canvas );
		else
			pDevice->ClipStretchBlit( _clip, m_pSurface, src, _canvas );
		return;
	}

	if( src.w == _canvas.w )
	{
		pDevice->ClipBlitVertBar( _clip, m_pSurface, src, m_frame,
						 (m_tiledSections & (1<<WG_CENTER)) != 0, _canvas.x, _canvas.y, _canvas.h );
		return;
	}

	if( src.h == _canvas.h )
	{
		pDevice->ClipBlitHorrBar( _clip, m_pSurface, src, m_frame,
						 (m_tiledSections & (1<<WG_CENTER)) != 0, _canvas.x, _canvas.y, _canvas.w );
		return;
	}

	const WgBorder& borders = m_frame;

	// Render upper row (top-left corner, top stretch area and top-right corner)

	if( borders.top > 0 )
	{
		WgRect rect( src.x, src.y, src.w, borders.top );

		pDevice->ClipBlitHorrBar( _clip, m_pSurface, rect, borders, (m_tiledSections & (1<<WG_NORTH)) != 0,
								_canvas.x, _canvas.y, _canvas.w );
	}

	// Render lowest row (bottom-left corner, bottom stretch area and bottom-right corner)

	if( borders.bottom > 0 )
	{
		WgRect rect( src.x, src.y + src.h - borders.bottom, src.w, borders.bottom );

		pDevice->ClipBlitHorrBar( _clip, m_pSurface, rect, borders, (m_tiledSections & (1<<WG_SOUTH)) != 0,
								_canvas.x, _canvas.y + _canvas.h - borders.bottom, _canvas.w );
	}

	// Render left and right stretch areas

	if( _canvas.h > (int) borders.Height() )
	{
		if( borders.left > 0 )
		{
			WgRect sr( src.x, src.y + borders.top, borders.left, src.h - borders.Height() );
			WgRect dr( _canvas.x, _canvas.y + borders.top, borders.left, _canvas.h - borders.Height() );

			if( m_tiledSections & (1<<WG_WEST) )
				pDevice->ClipTileBlit( _clip, m_pSurface, sr, dr );
			else
				pDevice->ClipStretchBlit( _clip, m_pSurface, sr, dr );
		}

		if( borders.right > 0 )
		{
			WgRect sr(	src.x + src.w - borders.right, src.y + borders.top,
						borders.right, src.h - borders.Height() );
			WgRect dr(	_canvas.x + _canvas.w - borders.right, _canvas.y + borders.top,
						borders.right, _canvas.h - borders.Height() );

			if( m_tiledSections & (1<<WG_EAST) )
				pDevice->ClipTileBlit( _clip, m_pSurface, sr, dr );
			else
				pDevice->ClipStretchBlit( _clip, m_pSurface, sr, dr );
		}
	}


	// Render middle stretch area

	if( (_canvas.h > borders.top + borders.bottom) && (_canvas.w > borders.left + borders.right ) )
	{
		WgRect sr(	src.x + borders.left, src.y + borders.top,
					src.w - borders.Width(), src.h - borders.Height() );

		WgRect dr(	_canvas.x + borders.left, _canvas.y + borders.top,
					_canvas.w - borders.Width(), _canvas.h - borders.Height() );

		if( m_tiledSections & (1<<WG_CENTER) )
			pDevice->ClipTileBlit( _clip, m_pSurface, sr, dr );
		else
			pDevice->ClipStretchBlit( _clip, m_pSurface, sr, dr );
	}
}

//____ _renderNoClip() ________________________________________________________

void WgBlockSkin::_renderNoClip( WgGfxDevice * pDevice, const StateData * pState, const WgRect& _canvas ) const
{
	WgSize borderSize = m_frame.Size();
	const WgRect&	src		= WgRect(pState->ofs, m_dimensions);

	if( src.w == _canvas.w && src.h == _canvas.h )
	{
		pDevice->Blit( m_pSurface, src, _canvas.x, _canvas.y );
		return;
	}

	if( borderSize.w == 0 && borderSize.h == 0 )
	{
		if( m_tiledSections & (1 << WG_CENTER) )
			pDevice->TileBlit( m_pSurface, src, _canvas );
		else
			pDevice->StretchBlit( m_pSurface, src, _canvas );
		return;
	}

	if( src.w == _canvas.w )
	{
		pDevice->BlitVertBar( m_pSurface, src, m_frame,
						 (m_tiledSections & (1<<WG_CENTER)) != 0, _canvas.x, _canvas.y, _canvas.h );
		return;
	}

	if( src.h == _canvas.h )
	{
		pDevice->BlitHorrBar( m_pSurface, src, m_frame,
						 (m_tiledSections & (1<<WG_CENTER)) != 0, _canvas.x, _canvas.y, _canvas.w );
		return;
	}

	const WgBorder& borders = m_frame;

	// Render upper row (top-left corner, top stretch area and top-right corner)

	if( borders.top > 0 )
	{
		WgRect rect( src.x, src.y, src.w, borders.top );

		pDevice->BlitHorrBar( m_pSurface, rect, borders, (m_tiledSections & (1<<WG_NORTH)) != 0,
								_canvas.x, _canvas.y, _canvas.w );
	}

	// Render lowest row (bottom-left corner, bottom stretch area and bottom-right corner)

	if( borders.bottom > 0 )
	{
		WgRect rect( src.x, src.y + src.h - borders.bottom, src.w, borders.bottom );

		pDevice->BlitHorrBar( m_pSurface, rect, borders, (m_tiledSections & (1<<WG_SOUTH)) != 0,
								_canvas.x, _canvas.y + _canvas.h - borders.bottom, _canvas.w );
	}

	// Render left and right stretch areas

	if( _canvas.h > (int) borders.Height() )
	{
		if( borders.left > 0 )
		{
			WgRect sr( src.x, src.y + borders.top, borders.left, src.h - borders.Height() );
			WgRect dr( _canvas.x, _canvas.y + borders.top, borders.left, _canvas.h - borders.Height() );

			if( m_tiledSections & (1<<WG_WEST) )
				pDevice->TileBlit( m_pSurface, sr, dr );
			else
				pDevice->StretchBlit( m_pSurface, sr, dr );
		}

		if( borders.right > 0 )
		{
			WgRect sr(	src.x + src.w - borders.right, src.y + borders.top,
						borders.right, src.h - borders.Height() );
			WgRect dr(	_canvas.x + _canvas.w - borders.right, _canvas.y + borders.top,
						borders.right, _canvas.h - borders.Height() );

			if( m_tiledSections & (1<<WG_EAST) )
				pDevice->TileBlit( m_pSurface, sr, dr );
			else
				pDevice->StretchBlit( m_pSurface, sr, dr );
		}
	}


	// Render middle stretch area

	if( (_canvas.h > borders.top + borders.bottom) && (_canvas.w > borders.left + borders.right ) )
	{
		WgRect sr(	src.x + borders.left, src.y + borders.top,
					src.w - borders.Width(), src.h - borders.Height() );

		WgRect dr(	_canvas.x + borders.left, _canvas.y + borders.top,
					_canvas.w - borders.Width(), _canvas.h - borders.Height() );

		if( m_tiledSections & (1<<WG_CENTER) )
			pDevice->TileBlit( m_pSurface, sr, dr );
		else
			pDevice->StretchBlit( m_pSurface, sr, dr );
	}
}



//____ MinSize() ______________________________________________________________

WgSize WgBlockSkin::MinSize() const
{
	WgSize content = WgExtendedSkin::MinSize();
	WgSize frame = m_frame.Size();
	return WgSize( WgMax(content.w, frame.w), WgMax(content.h, frame.h) );
}

//____ PreferredSize() ________________________________________________________

WgSize WgBlockSkin::PreferredSize() const
{
	WgSize sz = WgExtendedSkin::PreferredSize();
	return WgSize( WgMax(m_dimensions.w,sz.w),WgMax(m_dimensions.h,sz.h) );
}

//____ SizeForContent() _______________________________________________________

WgSize WgBlockSkin::SizeForContent( const WgSize contentSize ) const
{
	WgSize sz = WgExtendedSkin::SizeForContent(contentSize);
	WgSize min = m_frame.Size();

	return WgSize( WgMax(sz.w,min.w), WgMax(sz.h,min.h) );
}

//____ MarkTest() _____________________________________________________________

bool WgBlockSkin::MarkTest( const WgCoord& _ofs, const WgRect& canvas, WgState state, int opacityTreshold ) const
{
	if( !m_pSurface || !canvas.Contains(_ofs) )
		return false;

	int alpha;
	if( IsOpaque( state ) )
		alpha = 255;
	else
	{
		WgCoord ofs = _ofs - canvas.Pos();

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
			int tileAreaWidth = m_dimensions.w - m_frame.Width();

			bool bTile;

			if( ySection == 0 )
				bTile = (m_tiledSections & (1 << WG_NORTH)) != 0;
			else if( ySection == 1 )
				bTile = (m_tiledSections & (1 << WG_CENTER)) != 0;
			else
				bTile = (m_tiledSections & (1 << WG_SOUTH)) != 0;

			if( bTile )
				ofs.x = ((ofs.x - m_frame.left) % tileAreaWidth) + m_frame.left;
			else
			{
				double screenWidth = canvas.w - m_frame.Width();	// Width of stretch-area on screen.
				ofs.x = (int) ((ofs.x-m_frame.left)/screenWidth * tileAreaWidth + m_frame.left);
			}
		}


		// Convert ofs.y to Y-offset in bitmap, taking stretch/tile section into account.

		if( ySection == 2 )
		{
			ofs.y = m_dimensions.h - (canvas.h - ofs.y);
		}
		else if( ySection == 1 )
		{
			int tileAreaHeight = m_dimensions.h - m_frame.Height();

			bool bTile;

			if( xSection == 0 )
				bTile = (m_tiledSections & (1 << WG_WEST)) != 0;
			else if( xSection == 1 )
				bTile = (m_tiledSections & (1 << WG_CENTER)) != 0;
			else
				bTile = (m_tiledSections & (1 << WG_EAST)) != 0;

			if( bTile )
				ofs.y = ((ofs.y - m_frame.top) % tileAreaHeight) + m_frame.top;
			else
			{
				double screenHeight = canvas.h - m_frame.Height();	// Height of stretch-area on screen.
				ofs.y = (int) ((ofs.y-m_frame.top)/screenHeight * tileAreaHeight + m_frame.top);
			}
		}

		WgCoord srcOfs = m_state[_stateToIndex(state)].ofs;

		alpha = m_pSurface->Alpha(srcOfs.x+ofs.x, srcOfs.y+ofs.y);
	}

	return ( alpha >= opacityTreshold);
}

//____ IsOpaque() _____________________________________________________________

bool WgBlockSkin::IsOpaque() const
{
	return m_bIsOpaque;
}

bool WgBlockSkin::IsOpaque( WgState state ) const
{
	if( m_bIsOpaque )
		return true;

	return (m_state[_stateToIndex(state)].opaqueSections == ALL_SECTIONS);
}

bool WgBlockSkin::IsOpaque( const WgRect& rect, const WgSize& canvasSize, WgState state ) const
{
	// Quick exit in optimal case

	if( m_bIsOpaque )
		return true;

	// Semi-quick exit

	int index = _stateToIndex(state);

	if( rect.w == canvasSize.w && rect.h == canvasSize.h )
		return (m_state[index].opaqueSections == ALL_SECTIONS);

	WgRect center = WgRect(canvasSize) - m_frame;
	if( center.Contains(rect) )
	{
		if( m_state[index].opaqueSections & (1<<WG_CENTER) )
			return true;
		else
			return false;
	}

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
			bitmask |= (1<<WG_NORTHWEST);
		if( rect.x < center.x + center.w && rect.x + rect.w > center.x )
			bitmask |= (1<<WG_NORTH);
		if( rect.x + rect.w > center.x + center.w )
			bitmask |= (1<<WG_NORTHEAST);
	}

	if( rect.y < center.y + center.h && rect.y + rect.h > center.y )
	{
		if( rect.x < center.x )
			bitmask |= (1<<WG_WEST);
		if( rect.x < center.x + center.w && rect.x + rect.w > center.x )
			bitmask |= (1<<WG_CENTER);
		if( rect.x + rect.w > center.x + center.w )
			bitmask |= (1<<WG_EAST);
	}

	if( rect.y + rect.h > center.y + center.h )
	{
		if( rect.x < center.x )
			bitmask |= (1<<WG_SOUTHWEST);
		if( rect.x < center.x + center.w && rect.x + rect.w > center.x )
			bitmask |= (1<<WG_SOUTH);
		if( rect.x + rect.w > center.x + center.w )
			bitmask |= (1<<WG_SOUTHEAST);
	}


	if( (m_state[index].opaqueSections & bitmask) == bitmask )
		return true;

	return false;
}

//____ IsStateIdentical() _____________________________________________________

bool WgBlockSkin::IsStateIdentical( WgState state, WgState comparedTo ) const
{
	int i1 = _stateToIndex(state);
	int i2 = _stateToIndex(comparedTo);

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
