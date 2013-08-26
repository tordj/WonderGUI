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

#include <wg_tileholder.h>
#include <wg_gfxdevice.h>



//____ SetMinTileSize() _______________________________________________________

void WgTileHolder::SetMinTileSize( WgSize size )
{
	m_minTileSize = size;
	_tilesModified();
}

//____ SetTileBlocks() ________________________________________________________

void WgTileHolder::SetTileBlocks( const WgBlocksetPtr& pOddBlocks, const WgBlocksetPtr& pEvenBlocks )
{
	m_pTileBlocks[0] = pEvenBlocks;
	m_pTileBlocks[1] = pOddBlocks;
	_tilesModified();
}

//____ RemoveTileBlocks() _____________________________________________________

void WgTileHolder::RemoveTileBlocks()
{
	m_pTileBlocks[0] = 0;
	m_pTileBlocks[1] = 0;
	_tilesModified();
}

//____ SetTileColors() ________________________________________________________

void WgTileHolder::SetTileColors( const WgColorsetPtr& pOddColors, const WgColorsetPtr& pEvenColors )
{
	m_pTileColors[0] = pEvenColors;
	m_pTileColors[1] = pOddColors;
	_tilesModified();
}

//____ RemoveTileColors() _____________________________________________________

void WgTileHolder::RemoveTileColors()
{
	m_pTileColors[0] = 0;
	m_pTileColors[1] = 0;
	_tilesModified();
}

//____ _cloneContent() ________________________________________________________

void WgTileHolder::_cloneContent( const WgTileHolder * _pOrg )
{
	m_pTileColors[0] = _pOrg->m_pTileColors[0];
	m_pTileColors[1] = _pOrg->m_pTileColors[1];
	m_pTileBlocks[0] = _pOrg->m_pTileBlocks[0];
	m_pTileBlocks[1] = _pOrg->m_pTileBlocks[1];

	m_minTileSize = _pOrg->m_minTileSize;
}


//____ _renderTile() __________________________________________________________

void WgTileHolder::_renderTile( WgGfxDevice * pDevice, const WgRect& clip, const WgRect& dest, int _tileNb, WgMode mode )
{
	int tileNb = _tileNb & 0x1;

	if( m_pTileColors[tileNb] )
	{
		WgColor color = m_pTileColors[tileNb]->Color(mode);
		if( 0 != color.a )
			pDevice->Fill( clip, color );
	}

	if( m_pTileBlocks[tileNb] )
	{
		WgBlocksetPtr p = m_pTileBlocks[tileNb];
		if( p && !p->IsModeSkipable(mode) )
			pDevice->ClipBlitBlock(clip, p->GetBlock(mode, dest.Size() ), dest );
	}
}

