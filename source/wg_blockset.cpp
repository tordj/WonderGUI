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


#include <wg_blockset.h>
#include <wg_geo.h>
#include <wg_surface.h>
#include <assert.h>



WgBlock::WgBlock(	const WgSurface * pSurf, const WgRect& rect, const WgBorders& gfxBorders, const WgBorders& contentBorders, Uint32 flags )
{
	m_pSurf			= pSurf;
	m_rect			= rect;
	m_gfxBorders	= gfxBorders;
	m_contentBorders= contentBorders;
	m_flags			= flags;

	if( m_gfxBorders.left || m_gfxBorders.right || m_gfxBorders.top || m_gfxBorders.bottom )
		m_flags |= WG_HAS_BORDERS;

	if(m_rect.x + m_rect.w > (Sint32)m_pSurf->width())
		m_rect.w = m_pSurf->width() - m_rect.x;
	if(m_rect.y + m_rect.h > (Sint32)m_pSurf->height())
		m_rect.h = m_pSurf->height() - m_rect.y;
}


WgBlockSet::WgBlockSet(	WgMemPool * pPool, const WgSurface * _pSurf, const WgRect& _normal, 
					   const WgRect& _marked, const WgRect& _selected, const WgRect& _disabled, 
					   const WgRect& _special, const WgBorders& _gfxBorders, 
					   const WgBorders& _contentBorders, Uint32 _flags ) : WgRefCountedPooled(pPool)
{
	m_pSurf			= _pSurf;
	m_gfxBorders	= _gfxBorders;
	m_contentBorders= _contentBorders;
	m_flags			= VerifyFlags(_flags);
	m_width			= _normal.w;
	m_height		= _normal.h;

	m_x[WG_MODE_NORMAL]		= _normal.x;
	m_x[WG_MODE_MARKED]		= _marked.x;
	m_x[WG_MODE_SELECTED]	= _selected.x;
	m_x[WG_MODE_DISABLED]	= _disabled.x;
	m_x[WG_MODE_SPECIAL]	= _special.x;

	m_y[WG_MODE_NORMAL]		= _normal.y;
	m_y[WG_MODE_MARKED]		= _marked.y;
	m_y[WG_MODE_SELECTED]	= _selected.y;
	m_y[WG_MODE_DISABLED]	= _disabled.y;
	m_y[WG_MODE_SPECIAL]	= _special.y;
}

WgBlockSet::WgBlockSet(	WgMemPool * pPool, const WgSurface * _pSurf, const WgBorders& _gfxBorders, const WgBorders& _contentBorders, Uint32 _flags ) : WgRefCountedPooled(pPool)
{
	m_pSurf			= _pSurf;
	m_gfxBorders	= _gfxBorders;
	m_contentBorders= _contentBorders;
	m_flags			= VerifyFlags(_flags);
	m_width			= 0;
	m_height		= 0;

	m_x[WG_MODE_NORMAL]		= 0;
	m_x[WG_MODE_MARKED]		= 0;
	m_x[WG_MODE_SELECTED]	= 0;
	m_x[WG_MODE_DISABLED]	= 0;
	m_x[WG_MODE_SPECIAL]	= 0;

	m_y[WG_MODE_NORMAL]		= 0;
	m_y[WG_MODE_MARKED]		= 0;
	m_y[WG_MODE_SELECTED]	= 0;
	m_y[WG_MODE_DISABLED]	= 0;
	m_y[WG_MODE_SPECIAL]	= 0;
}

bool WgBlockSet::SetTile(Uint32 place, bool bTile)
{
	if((place & WG_TILE_ALL) != place)
		return false;

	// cannot scale and tile at the same time
	if((place & WG_TILE_CENTER) && IsScaled())
		return false;

	if(bTile)
		m_flags |= place;
	else
		m_flags &= ~place;

	return true;
}

bool WgBlockSet::SetScale(bool bScale)
{
	// cannot scale and tile at the same time
	if((m_flags & WG_TILE_CENTER) && bScale)
		return false;

	if(bScale)
		m_flags |= WG_SCALE_CENTER;
	else
		m_flags &= ~WG_SCALE_CENTER;

	return true;
}

Uint32 WgBlockSet::VerifyFlags(Uint32 _flags)
{
	if((_flags & (WG_SCALE_CENTER | WG_TILE_CENTER)) == (WG_SCALE_CENTER | WG_TILE_CENTER))
	{
		assert(0);
		return _flags &~ WG_TILE_CENTER;
	}
	return _flags;
}
