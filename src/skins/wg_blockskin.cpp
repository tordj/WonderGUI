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

namespace wg 
{
	
	using namespace WgUtil;
	
	const char BlockSkin::CLASSNAME[] = {"BlockSkin"};
	
	//____ create() _______________________________________________________________
	
	BlockSkin_p BlockSkin::create()
	{
		return BlockSkin_p(new BlockSkin());
	}
	
	
	BlockSkin_p BlockSkin::createStatic( const Surface_p& pSurface, Rect block, Border frame )
	{
		BlockSkin * pSkin = new BlockSkin();
		pSkin->setSurface( pSurface );
		pSkin->setBlockGeo( block.size(), frame );
		pSkin->setAllBlocks( block.pos() );
		return BlockSkin_p(pSkin);
	}
	
	BlockSkin_p BlockSkin::createEnable( const Surface_p& pSurface, Size blockSize, Coord ofsEnabled, Coord ofsDisabled, Border frame )
	{
		BlockSkin * pSkin = new BlockSkin();
		pSkin->setSurface( pSurface );
		pSkin->setBlockGeo( blockSize, frame );
		pSkin->setAllBlocks( ofsEnabled );
		pSkin->setDisabledBlock( ofsDisabled );
		return BlockSkin_p(pSkin);
	}
	
	BlockSkin_p BlockSkin::createClickable( const Surface_p& pSurface, Size blockGeo, Coord blockStartOfs, Size blockPitch, Border blockFrame )
	{
		BlockSkin * pSkin = new BlockSkin();
		pSkin->setSurface( pSurface );
		pSkin->setBlockGeo( blockGeo, blockFrame );
	
		Coord blockOfs = blockStartOfs;
	
		pSkin->setAllBlocks( blockOfs );
		blockOfs += Coord(blockPitch.w,blockPitch.h);
		pSkin->setHoveredBlocks( blockOfs );
		blockOfs += Coord(blockPitch.w,blockPitch.h);
		pSkin->setPressedBlocks( blockOfs );
		blockOfs += Coord(blockPitch.w,blockPitch.h);
		pSkin->setDisabledBlock( blockOfs );
		return BlockSkin_p(pSkin);
	}
	
	BlockSkin_p BlockSkin::createSelectable( const Surface_p& pSurface, Size blockGeo, Coord blockStartOfs, Size blockPitch, Border blockFrame )
	{
		BlockSkin * pSkin = new BlockSkin();
		pSkin->setSurface( pSurface );
		pSkin->setBlockGeo( blockGeo, blockFrame );
	
		Coord blockOfs = blockStartOfs;
	
		pSkin->setAllBlocks( blockOfs );
		blockOfs += Coord(blockPitch.w,blockPitch.h);
		pSkin->setSelectedBlocks( blockOfs );
		blockOfs += Coord(blockPitch.w,blockPitch.h);
		pSkin->setDisabledBlock( blockOfs );
		return BlockSkin_p(pSkin);
	}
	
	BlockSkin_p BlockSkin::createClickSelectable( const Surface_p& pSurface, Size blockGeo, Coord blockStartOfs, Size blockPitch, Border blockFrame )
	{
		BlockSkin * pSkin = new BlockSkin();
		pSkin->setSurface( pSurface );
		pSkin->setBlockGeo( blockGeo, blockFrame );
	
		Coord blockOfs = blockStartOfs;
	
		pSkin->setStateBlock( WG_STATE_NORMAL, blockOfs );				// Normal
		pSkin->setStateBlock( WG_STATE_FOCUSED, blockOfs );
		blockOfs += Coord(blockPitch.w,blockPitch.h);
	
		pSkin->setStateBlock( WG_STATE_HOVERED, blockOfs );				// Hovered, not selected
		pSkin->setStateBlock( WG_STATE_HOVERED_FOCUSED, blockOfs );
		pSkin->setStateBlock( WG_STATE_PRESSED, blockOfs );
		pSkin->setStateBlock( WG_STATE_PRESSED_FOCUSED, blockOfs );
		blockOfs += Coord(blockPitch.w,blockPitch.h);
	
		pSkin->setStateBlock( WG_STATE_SELECTED, blockOfs );			// Selected, not hovered
		pSkin->setStateBlock( WG_STATE_FOCUSED_SELECTED, blockOfs );
		blockOfs += Coord(blockPitch.w,blockPitch.h);
	
		pSkin->setStateBlock( WG_STATE_HOVERED_SELECTED, blockOfs );	// Selected and hovered
		pSkin->setStateBlock( WG_STATE_HOVERED_FOCUSED_SELECTED, blockOfs );
		pSkin->setStateBlock( WG_STATE_PRESSED_SELECTED, blockOfs );
		pSkin->setStateBlock( WG_STATE_PRESSED_FOCUSED_SELECTED, blockOfs );
		blockOfs += Coord(blockPitch.w,blockPitch.h);
	
		pSkin->setStateBlock( WG_STATE_DISABLED, blockOfs );			// Disabled
		return BlockSkin_p(pSkin);
	}
	
	/** Usable for state button etc, where a widget is normal/hovered/pressed/disabled as well as selected/unselected
	*/
	BlockSkin_p BlockSkin::createClickSelectableWidget( const Surface_p& pSurface, Size blockGeo, Coord blockStartOfs, Size blockPitch, Border blockFrame )
	{
		BlockSkin * pSkin = new BlockSkin();
		pSkin->setSurface( pSurface );
		pSkin->setBlockGeo( blockGeo, blockFrame );
	
		Coord blockOfs = blockStartOfs;
	
		pSkin->setStateBlock( WG_STATE_NORMAL, blockOfs );				// Normal
		pSkin->setStateBlock( WG_STATE_FOCUSED, blockOfs );
		blockOfs += Coord(blockPitch.w,blockPitch.h);
	
		pSkin->setStateBlock( WG_STATE_HOVERED, blockOfs );				// Hovered
		pSkin->setStateBlock( WG_STATE_HOVERED_FOCUSED, blockOfs );
		blockOfs += Coord(blockPitch.w,blockPitch.h);
	
		pSkin->setStateBlock( WG_STATE_PRESSED, blockOfs );				// Pressed
		pSkin->setStateBlock( WG_STATE_PRESSED_FOCUSED, blockOfs );
		blockOfs += Coord(blockPitch.w,blockPitch.h);
	
		pSkin->setStateBlock( WG_STATE_SELECTED, blockOfs );
		pSkin->setStateBlock( WG_STATE_FOCUSED_SELECTED, blockOfs );
		blockOfs += Coord(blockPitch.w,blockPitch.h);
	
		pSkin->setStateBlock( WG_STATE_HOVERED_SELECTED, blockOfs );
		pSkin->setStateBlock( WG_STATE_HOVERED_FOCUSED_SELECTED, blockOfs );
		blockOfs += Coord(blockPitch.w,blockPitch.h);
	
		pSkin->setStateBlock( WG_STATE_PRESSED_SELECTED, blockOfs );
		pSkin->setStateBlock( WG_STATE_PRESSED_FOCUSED_SELECTED, blockOfs );
		blockOfs += Coord(blockPitch.w,blockPitch.h);
	
		pSkin->setStateBlock( WG_STATE_DISABLED, blockOfs );
		blockOfs += Coord(blockPitch.w,blockPitch.h);
	
		pSkin->setStateBlock( WG_STATE_DISABLED_SELECTED, blockOfs );
	
		return BlockSkin_p(pSkin);
	}
	
	
	
	
	BlockSkin_p BlockSkin::createStaticFromSurface( const Surface_p& pSurface, Border frame )
	{
		BlockSkin * pSkin = new BlockSkin();
		pSkin->setSurface( pSurface );
		pSkin->setBlockGeo( pSurface->size(), frame );
		pSkin->setAllBlocks( Coord(0,0) );
		return BlockSkin_p(pSkin);
	}
	
	BlockSkin_p BlockSkin::createEnableFromSurface( const Surface_p& pSurface, int blockSpacing, Border blockFrame )
	{
		BlockSkin * pSkin = new BlockSkin();
		pSkin->setSurface( pSurface );
	
		Size	sz = Size( (pSurface->width()-blockSpacing)/2, pSurface->height() );
	
		pSkin->setBlockGeo( sz, blockFrame );
		pSkin->setAllBlocks( Coord(0,0) );
		pSkin->setDisabledBlock( Coord(sz.w+blockSpacing,0) );
		return BlockSkin_p(pSkin);
	}
	
	BlockSkin_p BlockSkin::createClickableFromSurface( const Surface_p& pSurface, int blockSpacing, Border blockFrame )
	{
		Size	blockSize = Size( (pSurface->width()-blockSpacing*3)/4, pSurface->height() );
		return createClickable( pSurface, blockSize, Coord(0,0), Size(blockSize.w+blockSpacing,0), blockFrame );
	}
	
	BlockSkin_p BlockSkin::createSelectableFromSurface( const Surface_p& pSurface, int blockSpacing, Border blockFrame )
	{
		Size	blockSize( (pSurface->width()-blockSpacing*2)/3, pSurface->height() );
		return createSelectable( pSurface, blockSize, Coord(0,0), Size(blockSize.w+blockSpacing,0), blockFrame );
	}
	
	BlockSkin_p BlockSkin::createClickSelectableFromSurface( const Surface_p& pSurface, int blockSpacing, Border blockFrame )
	{
		Size	blockSize( (pSurface->width()-blockSpacing*4)/5, pSurface->height() );
		return createClickSelectable( pSurface, blockSize, Coord(0,0), Size(blockSize.w+blockSpacing,0), blockFrame );
	}
	
	//____ Constructor ____________________________________________________________
	
	BlockSkin::BlockSkin()
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
	
	//____ isInstanceOf() _________________________________________________________
	
	bool BlockSkin::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;
	
		return ExtendedSkin::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * BlockSkin::className( void ) const
	{
		return CLASSNAME;
	}
	
	//____ cast() _________________________________________________________________
	
	BlockSkin_p BlockSkin::cast( const Object_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return BlockSkin_p( static_cast<BlockSkin*>(pObject.rawPtr()) );
	
		return 0;
	}
	
	
	//____ setBlockGeo() __________________________________________________________
	
	bool BlockSkin::setBlockGeo( Size size, Border frame )
	{
		if( size.w <= frame.width() || size.h <= frame.height() )
			return false;
	
		m_dimensions	= size;
		m_frame			= frame;
		return true;
	}
	
	//____ setSurface() ______________________________________________________
	
	void BlockSkin::setSurface( const Surface_p& pSurf )
	{
		m_pSurface = pSurf;
		if( m_pSurface )
			m_bIsOpaque = pSurf->isOpaque();
		else
			m_bIsOpaque = false;
	}
	
	//____ setStateBlock() ________________________________________________________
	
	void BlockSkin::setStateBlock( StateEnum state, const Coord& ofs )
	{
		m_state[_stateToIndex(state)].ofs = ofs;
	}
	
	//____ setAllBlocks() _________________________________________________________
	
	void BlockSkin::setAllBlocks( const Coord& ofs )
	{
		for( int i = 0 ; i < WG_NB_STATES ; i++ )
			m_state[i].ofs = ofs;
	
	}
	
	//____ setDisabledBlock() _____________________________________________________
	
	void BlockSkin::setDisabledBlock( const Coord& ofs )
	{
		m_state[_stateToIndex(WG_STATE_DISABLED)].ofs = ofs;
	}
	
	//____ setHoveredBlocks() _____________________________________________________
	
	void BlockSkin::setHoveredBlocks( const Coord& ofs )
	{
		m_state[_stateToIndex(WG_STATE_HOVERED)].ofs = ofs;
		m_state[_stateToIndex(WG_STATE_HOVERED_SELECTED)].ofs = ofs;
		m_state[_stateToIndex(WG_STATE_HOVERED_FOCUSED)].ofs = ofs;
		m_state[_stateToIndex(WG_STATE_HOVERED_FOCUSED_SELECTED)].ofs = ofs;
	
		setPressedBlocks( ofs );
	}
	
	//____ setPressedBlocks() _____________________________________________________
	
	void BlockSkin::setPressedBlocks( const Coord& ofs )
	{
		m_state[_stateToIndex(WG_STATE_PRESSED)].ofs = ofs;
		m_state[_stateToIndex(WG_STATE_PRESSED_SELECTED)].ofs = ofs;
		m_state[_stateToIndex(WG_STATE_PRESSED_FOCUSED)].ofs = ofs;
		m_state[_stateToIndex(WG_STATE_PRESSED_FOCUSED_SELECTED)].ofs = ofs;
	}
	
	//____ setSelectedBlocks() ____________________________________________________
	
	void BlockSkin::setSelectedBlocks( const Coord& ofs )
	{
		m_state[_stateToIndex(WG_STATE_SELECTED)].ofs = ofs;
		m_state[_stateToIndex(WG_STATE_FOCUSED_SELECTED)].ofs = ofs;
		m_state[_stateToIndex(WG_STATE_HOVERED_SELECTED)].ofs = ofs;
		m_state[_stateToIndex(WG_STATE_HOVERED_FOCUSED_SELECTED)].ofs = ofs;
		m_state[_stateToIndex(WG_STATE_PRESSED_SELECTED)].ofs = ofs;
		m_state[_stateToIndex(WG_STATE_PRESSED_FOCUSED_SELECTED)].ofs = ofs;
	}
	
	//____ setTiled() _____________________________________________________________
	
	void BlockSkin::setTiled( bool bTiled )
	{
		_setBitFlag( m_tiledSections, WG_NORTH, bTiled );
		_setBitFlag( m_tiledSections, WG_SOUTH, bTiled );
		_setBitFlag( m_tiledSections, WG_EAST, bTiled );
		_setBitFlag( m_tiledSections, WG_WEST, bTiled );
		_setBitFlag( m_tiledSections, WG_CENTER, bTiled );
	}
	
	//____ setTiledTopBorder() ____________________________________________________
	
	void BlockSkin::setTiledTopBorder( bool bTiled )
	{
		_setBitFlag( m_tiledSections, WG_NORTH, bTiled );
	}
	
	//____ setTiledBottomBorder() _________________________________________________
	
	void BlockSkin::setTiledBottomBorder( bool bTiled )
	{
		_setBitFlag( m_tiledSections, WG_SOUTH, bTiled );
	}
	
	//____ setTiledLeftBorder() ___________________________________________________
	
	void BlockSkin::setTiledLeftBorder( bool bTiled )
	{
		_setBitFlag( m_tiledSections, WG_WEST, bTiled );
	}
	
	//____ setTiledRightBorder() __________________________________________________
	
	void BlockSkin::setTiledRightBorder( bool bTiled )
	{
		_setBitFlag( m_tiledSections, WG_EAST, bTiled );
	}
	
	//____ setTiledCenter() _______________________________________________________
	
	void BlockSkin::setTiledCenter( bool bTiled )
	{
		_setBitFlag( m_tiledSections, WG_CENTER, bTiled );
	}
	
	//____ optimizeRenderMethods() ________________________________________________
	
	void BlockSkin::optimizeRenderMethods()
	{
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
	
		m_pSurface->lock(WG_READ_ONLY);
	
		for( int i = 0 ; i < WG_NB_STATES ; i++ )
		{
			Rect r = Rect(m_state[i].ofs,m_dimensions);
	
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
					_scanStateBlockSectionArea( &m_state[i], WG_NORTHWEST, Rect(x1, y1, m_frame.left, m_frame.top) );
	
				_scanStateBlockSectionArea( &m_state[i], WG_NORTH, Rect(x2, y1, centerW, m_frame.top) );
	
				if( m_frame.right > 0 )
					_scanStateBlockSectionArea( &m_state[i], WG_NORTHEAST, Rect(x3, y1, m_frame.right, m_frame.top) );
			}
	
			if( centerH > 0 )
			{
				if( m_frame.left > 0 )
					_scanStateBlockSectionArea( &m_state[i], WG_WEST, Rect(x1, y2, m_frame.left, centerH) );
	
				_scanStateBlockSectionArea( &m_state[i], WG_CENTER, Rect(x2, y2, centerW, centerH) );
	
				if( m_frame.right > 0 )
					_scanStateBlockSectionArea( &m_state[i], WG_EAST, Rect(x3, y2, m_frame.right, centerH) );
			}
	
			if( m_frame.bottom > 0 )
			{
				if( m_frame.left > 0 )
					_scanStateBlockSectionArea( &m_state[i], WG_SOUTHWEST, Rect(x1, y3, m_frame.left, m_frame.bottom) );
	
				_scanStateBlockSectionArea( &m_state[i], WG_SOUTH, Rect(x2, y3, centerW, m_frame.bottom) );
	
				if( m_frame.right > 0 )
					_scanStateBlockSectionArea( &m_state[i], WG_SOUTHEAST, Rect(x3, y3, m_frame.right, m_frame.bottom) );
			}
		}
	
		m_pSurface->unlock();
	}
	
	//_____ _scanStateBlockSectionArea() __________________________________________
	
	void BlockSkin::_scanStateBlockSectionArea( StateData * pState, WgOrigo section, const Rect& sectionArea )
	{
		const WgPixelFormat * pFormat = m_pSurface->PixelFormat();
		int pitch = m_pSurface->pitch();
	
		if( pFormat->type != WG_PIXEL_RGBA_8 )
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
			pState->invisibleSections |= 1 << section;
		else if( alphaAcc == ((unsigned)(sectionArea.w * sectionArea.h)) * 255 )
			pState->opaqueSections |= 1 << section;
	}
	
	
	
	//____ render() _______________________________________________________________
	
	void BlockSkin::render( GfxDevice * pDevice, const Rect& _canvas, State state, const Rect& _clip ) const
	{
		if( !m_pSurface )
			return;
	
		const StateData * pState = &m_state[_stateToIndex(state)];
		if( pState->invisibleSections == ALL_SECTIONS )
			return;
	
		//
	
		// Shortcuts & optimizations for common special cases.
	
		Size borderSize = m_frame.size();
	
		if( _clip.contains( _canvas ) && borderSize.w <= _canvas.size().w && borderSize.h <= _canvas.size().h )
		{
			_renderNoClip( pDevice, pState, _canvas );
			return;
		}
	
		const Rect&	src		= Rect(pState->ofs, m_dimensions);
	
		if( src.w == _canvas.w && src.h == _canvas.h )
		{
			pDevice->clipBlit( _clip, m_pSurface, src, _canvas.x, _canvas.y );
			return;
		}
	
		if( borderSize.w == 0 && borderSize.h == 0 )
		{
			if( m_tiledSections & (1 << WG_CENTER) )
				pDevice->clipTileBlit( _clip, m_pSurface, src, _canvas );
			else
				pDevice->clipStretchBlit( _clip, m_pSurface, src, _canvas );
			return;
		}
	
		if( src.w == _canvas.w )
		{
			pDevice->clipBlitVertBar( _clip, m_pSurface, src, m_frame,
							 (m_tiledSections & (1<<WG_CENTER)) != 0, _canvas.x, _canvas.y, _canvas.h );
			return;
		}
	
		if( src.h == _canvas.h )
		{
			pDevice->clipBlitHorrBar( _clip, m_pSurface, src, m_frame,
							 (m_tiledSections & (1<<WG_CENTER)) != 0, _canvas.x, _canvas.y, _canvas.w );
			return;
		}
	
		const Border& borders = m_frame;
	
		// Render upper row (top-left corner, top stretch area and top-right corner)
	
		if( borders.top > 0 )
		{
			Rect rect( src.x, src.y, src.w, borders.top );
	
			pDevice->clipBlitHorrBar( _clip, m_pSurface, rect, borders, (m_tiledSections & (1<<WG_NORTH)) != 0,
									_canvas.x, _canvas.y, _canvas.w );
		}
	
		// Render lowest row (bottom-left corner, bottom stretch area and bottom-right corner)
	
		if( borders.bottom > 0 )
		{
			Rect rect( src.x, src.y + src.h - borders.bottom, src.w, borders.bottom );
	
			pDevice->clipBlitHorrBar( _clip, m_pSurface, rect, borders, (m_tiledSections & (1<<WG_SOUTH)) != 0,
									_canvas.x, _canvas.y + _canvas.h - borders.bottom, _canvas.w );
		}
	
		// Render left and right stretch areas
	
		if( _canvas.h > (int) borders.height() )
		{
			if( borders.left > 0 )
			{
				Rect sr( src.x, src.y + borders.top, borders.left, src.h - borders.height() );
				Rect dr( _canvas.x, _canvas.y + borders.top, borders.left, _canvas.h - borders.height() );
	
				if( m_tiledSections & (1<<WG_WEST) )
					pDevice->clipTileBlit( _clip, m_pSurface, sr, dr );
				else
					pDevice->clipStretchBlit( _clip, m_pSurface, sr, dr );
			}
	
			if( borders.right > 0 )
			{
				Rect sr(	src.x + src.w - borders.right, src.y + borders.top,
							borders.right, src.h - borders.height() );
				Rect dr(	_canvas.x + _canvas.w - borders.right, _canvas.y + borders.top,
							borders.right, _canvas.h - borders.height() );
	
				if( m_tiledSections & (1<<WG_EAST) )
					pDevice->clipTileBlit( _clip, m_pSurface, sr, dr );
				else
					pDevice->clipStretchBlit( _clip, m_pSurface, sr, dr );
			}
		}
	
	
		// Render middle stretch area
	
		if( (_canvas.h > borders.top + borders.bottom) && (_canvas.w > borders.left + borders.right ) )
		{
			Rect sr(	src.x + borders.left, src.y + borders.top,
						src.w - borders.width(), src.h - borders.height() );
	
			Rect dr(	_canvas.x + borders.left, _canvas.y + borders.top,
						_canvas.w - borders.width(), _canvas.h - borders.height() );
	
			if( m_tiledSections & (1<<WG_CENTER) )
				pDevice->clipTileBlit( _clip, m_pSurface, sr, dr );
			else
				pDevice->clipStretchBlit( _clip, m_pSurface, sr, dr );
		}
	}
	
	//____ _renderNoClip() ________________________________________________________
	
	void BlockSkin::_renderNoClip( GfxDevice * pDevice, const StateData * pState, const Rect& _canvas ) const
	{
		Size borderSize = m_frame.size();
		const Rect&	src		= Rect(pState->ofs, m_dimensions);
	
		if( src.w == _canvas.w && src.h == _canvas.h )
		{
			pDevice->blit( m_pSurface, src, _canvas.x, _canvas.y );
			return;
		}
	
		if( borderSize.w == 0 && borderSize.h == 0 )
		{
			if( m_tiledSections & (1 << WG_CENTER) )
				pDevice->tileBlit( m_pSurface, src, _canvas );
			else
				pDevice->stretchBlit( m_pSurface, src, _canvas );
			return;
		}
	
		if( src.w == _canvas.w )
		{
			pDevice->blitVertBar( m_pSurface, src, m_frame,
							 (m_tiledSections & (1<<WG_CENTER)) != 0, _canvas.x, _canvas.y, _canvas.h );
			return;
		}
	
		if( src.h == _canvas.h )
		{
			pDevice->blitHorrBar( m_pSurface, src, m_frame,
							 (m_tiledSections & (1<<WG_CENTER)) != 0, _canvas.x, _canvas.y, _canvas.w );
			return;
		}
	
		const Border& borders = m_frame;
	
		// Render upper row (top-left corner, top stretch area and top-right corner)
	
		if( borders.top > 0 )
		{
			Rect rect( src.x, src.y, src.w, borders.top );
	
			pDevice->blitHorrBar( m_pSurface, rect, borders, (m_tiledSections & (1<<WG_NORTH)) != 0,
									_canvas.x, _canvas.y, _canvas.w );
		}
	
		// Render lowest row (bottom-left corner, bottom stretch area and bottom-right corner)
	
		if( borders.bottom > 0 )
		{
			Rect rect( src.x, src.y + src.h - borders.bottom, src.w, borders.bottom );
	
			pDevice->blitHorrBar( m_pSurface, rect, borders, (m_tiledSections & (1<<WG_SOUTH)) != 0,
									_canvas.x, _canvas.y + _canvas.h - borders.bottom, _canvas.w );
		}
	
		// Render left and right stretch areas
	
		if( _canvas.h > (int) borders.height() )
		{
			if( borders.left > 0 )
			{
				Rect sr( src.x, src.y + borders.top, borders.left, src.h - borders.height() );
				Rect dr( _canvas.x, _canvas.y + borders.top, borders.left, _canvas.h - borders.height() );
	
				if( m_tiledSections & (1<<WG_WEST) )
					pDevice->tileBlit( m_pSurface, sr, dr );
				else
					pDevice->stretchBlit( m_pSurface, sr, dr );
			}
	
			if( borders.right > 0 )
			{
				Rect sr(	src.x + src.w - borders.right, src.y + borders.top,
							borders.right, src.h - borders.height() );
				Rect dr(	_canvas.x + _canvas.w - borders.right, _canvas.y + borders.top,
							borders.right, _canvas.h - borders.height() );
	
				if( m_tiledSections & (1<<WG_EAST) )
					pDevice->tileBlit( m_pSurface, sr, dr );
				else
					pDevice->stretchBlit( m_pSurface, sr, dr );
			}
		}
	
	
		// Render middle stretch area
	
		if( (_canvas.h > borders.top + borders.bottom) && (_canvas.w > borders.left + borders.right ) )
		{
			Rect sr(	src.x + borders.left, src.y + borders.top,
						src.w - borders.width(), src.h - borders.height() );
	
			Rect dr(	_canvas.x + borders.left, _canvas.y + borders.top,
						_canvas.w - borders.width(), _canvas.h - borders.height() );
	
			if( m_tiledSections & (1<<WG_CENTER) )
				pDevice->tileBlit( m_pSurface, sr, dr );
			else
				pDevice->stretchBlit( m_pSurface, sr, dr );
		}
	}
	
	
	
	//____ minSize() ______________________________________________________________
	
	Size BlockSkin::minSize() const
	{
		Size content = ExtendedSkin::minSize();
		Size frame = m_frame.size();
		return Size( WgMax(content.w, frame.w), WgMax(content.h, frame.h) );
	}
	
	//____ preferredSize() ________________________________________________________
	
	Size BlockSkin::preferredSize() const
	{
		Size sz = ExtendedSkin::preferredSize();
		return Size( WgMax(m_dimensions.w,sz.w),WgMax(m_dimensions.h,sz.h) );
	}
	
	//____ sizeForContent() _______________________________________________________
	
	Size BlockSkin::sizeForContent( const Size contentSize ) const
	{
		Size sz = ExtendedSkin::sizeForContent(contentSize);
		Size min = m_frame.size();
	
		return Size( WgMax(sz.w,min.w), WgMax(sz.h,min.h) );
	}
	
	//____ markTest() _____________________________________________________________
	
	bool BlockSkin::markTest( const Coord& _ofs, const Rect& canvas, State state, int opacityTreshold ) const
	{
		if( !m_pSurface || !canvas.contains(_ofs) )
			return false;
	
		int alpha;
		if( isOpaque( state ) )
			alpha = 255;
		else
		{
			Coord ofs = _ofs - canvas.pos();
	
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
				int tileAreaWidth = m_dimensions.w - m_frame.width();
	
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
					double screenWidth = canvas.w - m_frame.width();	// Width of stretch-area on screen.
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
				int tileAreaHeight = m_dimensions.h - m_frame.height();
	
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
					double screenHeight = canvas.h - m_frame.height();	// Height of stretch-area on screen.
					ofs.y = (int) ((ofs.y-m_frame.top)/screenHeight * tileAreaHeight + m_frame.top);
				}
			}
	
			Coord srcOfs = m_state[_stateToIndex(state)].ofs;
	
			alpha = m_pSurface->alpha(srcOfs.x+ofs.x, srcOfs.y+ofs.y);
		}
	
		return ( alpha >= opacityTreshold);
	}
	
	//____ isOpaque() _____________________________________________________________
	
	bool BlockSkin::isOpaque() const
	{
		return m_bIsOpaque;
	}
	
	bool BlockSkin::isOpaque( State state ) const
	{
		if( m_bIsOpaque )
			return true;
	
		return (m_state[_stateToIndex(state)].opaqueSections == ALL_SECTIONS);
	}
	
	bool BlockSkin::isOpaque( const Rect& rect, const Size& canvasSize, State state ) const
	{
		// Quick exit in optimal case
	
		if( m_bIsOpaque )
			return true;
	
		// Semi-quick exit
	
		int index = _stateToIndex(state);
	
		if( rect.w == canvasSize.w && rect.h == canvasSize.h )
			return (m_state[index].opaqueSections == ALL_SECTIONS);
	
		Rect center = Rect(canvasSize) - m_frame;
		if( center.contains(rect) )
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
	
	//____ isStateIdentical() _____________________________________________________
	
	bool BlockSkin::isStateIdentical( State state, State comparedTo ) const
	{
		int i1 = _stateToIndex(state);
		int i2 = _stateToIndex(comparedTo);
	
		return ( m_state[i1].ofs == m_state[i2].ofs && ExtendedSkin::isStateIdentical(state,comparedTo) );
	}
	
	//____ _setBitFlag() __________________________________________________________
	
	void BlockSkin::_setBitFlag( int& bitmask, int bit, bool bSet )
	{
		if( bSet )
			bitmask |= 1 << bit;
		else
			bitmask &= ~(1 << bit);
	
	}

} // namespace wg
