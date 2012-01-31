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

#ifndef	WG_TILEHOLDER_DOT_H
#define	WG_TILEHOLDER_DOT_H

#ifndef WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

#ifndef WG_GEO_DOT_H
#	include <wg_geo.h>
#endif

#ifndef WG_BLOCKSET_DOT_H
#	include <wg_blockset.h>
#endif

#ifndef WG_COLORSET_DOT_H
#	include <wg_colorset.h>
#endif


class WgGfxDevice;

class WgTileHolder
{
public:
	virtual ~WgTileHolder() {};

	void			SetMinTileSize( WgSize size );
	WgSize			MinTileSize() const { return m_minTileSize; }
	int				MinTileWidth() const { return m_minTileSize.w; }
	int				MinTileHeight() const { return m_minTileSize.h; }

	void			SetTileBlocks( const WgBlockSetPtr& pBlocks ) { SetTileBlocks(pBlocks,pBlocks); }
	void			SetTileBlocks( const WgBlockSetPtr& pOddBlocks, const WgBlockSetPtr& pEvenBlocks );
	WgBlockSetPtr	OddTileBlocks() const { return m_pTileBlocks[0]; }
	WgBlockSetPtr	EvenTileBlocks() const { return m_pTileBlocks[1]; }
	void			RemoveTileBlocks();

	void			SetTileColors( const WgColorSetPtr& pColors ) { SetTileColors(pColors,pColors); }
	void			SetTileColors( const WgColorSetPtr& pOddColors, const WgColorSetPtr& pEvenColors );
	WgColorSetPtr	OddTileColors() const { return m_pTileColors[0]; }
	WgColorSetPtr	EvenTileColors() const { return m_pTileColors[1]; }
	void			RemoveTileColors();

protected:

	virtual void	_tilesModified() = 0;

	void			_renderTile( WgGfxDevice * pDevice, const WgRect& clip, const WgRect& dest, int tileNb, WgMode mode );
	void			_cloneContent( const WgTileHolder * _pOrg );

	WgColorSetPtr 	m_pTileColors[2];
	WgBlockSetPtr	m_pTileBlocks[2];

	WgSize			m_minTileSize;			// Set minimum size of a tile.
};

#endif	// WG_TILEHOLDER_DOT_H
