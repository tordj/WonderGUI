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

#ifndef WG_SKIN_DOT_H
#	include <wg_skin.h>
#endif


class WgGfxDevice;

class WgTileHolder
{
public:
	virtual ~WgTileHolder() {};

	void			SetMinTileSize( WgSize size );
	WgSize			MinTileSize() const { return m_minTileSize; }

	void			SetTileSkin( const WgSkinPtr& pSkin ) { SetTileSkin(pSkin,pSkin); }
	void			SetTileSkins( const WgSkinPtr& pOddSkin, const WgSkinPtr& pEvenSkin );
	WgSkinPtr		OddTileSkin() const { return m_pTileSkin[0]; }
	WgSkinPtr		EvenTileSkin() const { return m_pTileSkin[1]; }
	void			RemoveTileSkins();

protected:

	virtual void	_tilesModified() = 0;

	void			_renderTile( WgGfxDevice * pDevice, const WgRect& clip, const WgRect& dest, int tileNb, WgState state );
	void			_cloneContent( const WgTileHolder * _pOrg );

	WgSkinPtr		m_pTileSkin[2];

	WgSize			m_minTileSize;			// Set minimum size of a tile.
};

#endif	// WG_TILEHOLDER_DOT_H
