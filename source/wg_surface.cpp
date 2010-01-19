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

#include <new>

#include <wg_surface.h>
#include <wg_geo.h>

#include <wg_blockset.h>
#include <wg_mempool.h>

WgMemPool * WgSurface::g_pBlockSetMemPool = 0;

//____ WgSurface() ____________________________________________________________

WgSurface::WgSurface()
{
	m_lockStatus = UNLOCKED;

	if( !g_pBlockSetMemPool )
		g_pBlockSetMemPool = new WgMemPool( 16, sizeof(WgBlockSet) );
}

//____ ~WgSurface() ____________________________________________________________

WgSurface::~WgSurface()
{
}

//____ Dimensions() ____________________________________________________________

WgRect WgSurface::Dimensions() const
{
	return WgRect( 0, 0, GetWidth(), GetHeight() );
}

//____ defineBlockSet() ________________________________________________________

WgBlockSetPtr WgSurface::defineBlockSet(	const WgRect& normal, const WgRect& marked,
											const WgRect& selected, const WgRect& disabled,
											const WgRect& special, const WgBorders& gfxBorders, 
											const WgBorders& contentBorders, Uint32 flags ) const
{

	WgBlockSet * p =new(g_pBlockSetMemPool->AllocEntry())
							WgBlockSet(g_pBlockSetMemPool, this, normal, marked, selected, disabled, special, gfxBorders, contentBorders, flags );

//	m_blockSets.push_back( p );
	return WgBlockSetPtr(p);
}

WgBlockSetPtr WgSurface::defineBlockSet(	const WgRect& normal, const WgRect& marked,
											const WgRect& selected, const WgRect& disabled,
											const WgBorders& gfxBorders, const WgBorders& contentBorders, Uint32 flags ) const
{

	WgBlockSet * p =new(g_pBlockSetMemPool->AllocEntry())
							WgBlockSet(g_pBlockSetMemPool, this, normal, marked, selected, disabled, normal, gfxBorders, contentBorders, flags );

//	m_blockSets.push_back( p );
	return WgBlockSetPtr(p);
}

WgBlockSetPtr WgSurface::defineBlockSet(	const WgRect& normal, const WgRect& marked,
										const WgRect& selected, const WgBorders& gfxBorders,
										const WgBorders& contentBorders, Uint32 flags ) const
{
	WgBlockSet * p = new( g_pBlockSetMemPool->AllocEntry())
							WgBlockSet(g_pBlockSetMemPool, this, normal, marked, selected, normal, normal, gfxBorders, contentBorders, flags );

//	m_blockSets.push_back( p );
	return WgBlockSetPtr(p);
}

WgBlockSetPtr WgSurface::defineBlockSet( const WgRect& normal, const WgRect& disabled,
										const WgBorders& gfxBorders, const WgBorders& contentBorders, Uint32 flags ) const
{
	WgBlockSet * p = new( g_pBlockSetMemPool->AllocEntry())
							WgBlockSet(g_pBlockSetMemPool, this, normal, normal, normal, disabled, normal, gfxBorders, contentBorders, flags );

//	m_blockSets.push_back( p );
	return WgBlockSetPtr(p);
}

WgBlockSetPtr WgSurface::defineBlockSet( const WgRect& normal, const WgBorders& gfxBorders, const WgBorders& contentBorders, Uint32 flags ) const
{
	WgBlockSet * p = new( g_pBlockSetMemPool->AllocEntry())
							WgBlockSet(g_pBlockSetMemPool, this, normal, normal, normal, normal, normal, gfxBorders, contentBorders, flags );

//	m_blockSets.push_back( p );
	return WgBlockSetPtr(p);
}

WgBlockSetPtr WgSurface::defineBlockSet( const WgHorrTile5& tile, const WgBorders& gfxBorders, const WgBorders& contentBorders, Uint32 flags ) const
{
	int	w = (tile.w - tile.skip*4) / 5;
	int h = tile.h;
	int	skip = w + tile.skip;

	return defineBlockSet(	WgRect(tile.x, tile.y, w, h),
							WgRect(tile.x+skip, tile.y, w, h),
							WgRect(tile.x+skip*2, tile.y, w, h),
							WgRect(tile.x+skip*3, tile.y, w, h),
							WgRect(tile.x+skip*4, tile.y, w, h),
							gfxBorders, contentBorders, flags );
}

WgBlockSetPtr WgSurface::defineBlockSet( const WgHorrTile4& tile, const WgBorders& gfxBorders, const WgBorders& contentBorders, Uint32 flags ) const
{
	int	w = (tile.w - tile.skip*3) / 4;
	int h = tile.h;
	int	skip = w + tile.skip;

	return defineBlockSet(	WgRect(tile.x, tile.y, w, h),
							WgRect(tile.x+skip, tile.y, w, h),
							WgRect(tile.x+skip*2, tile.y, w, h),
							WgRect(tile.x+skip*3, tile.y, w, h),
							gfxBorders, contentBorders, flags );
}

WgBlockSetPtr WgSurface::defineBlockSet( const WgHorrTile3& tile, const WgBorders& gfxBorders, const WgBorders& contentBorders, Uint32 flags ) const
{
	int	w = (tile.w - tile.skip*2) / 3;
	int h = tile.h;
	int	skip = w + tile.skip;

	return defineBlockSet(	WgRect(tile.x, tile.y, w, h),
							WgRect(tile.x+skip, tile.y, w, h),
							WgRect(tile.x+skip*2, tile.y, w, h),
							gfxBorders, contentBorders, flags );
}

WgBlockSetPtr WgSurface::defineBlockSet( const WgHorrTile2& tile, const WgBorders& gfxBorders, const WgBorders& contentBorders, Uint32 flags ) const
{
	int	w = (tile.w - tile.skip) / 2;
	int h = tile.h;
	int	skip = w + tile.skip;

	return defineBlockSet(	WgRect(tile.x, tile.y, w, h),
							WgRect(tile.x+skip, tile.y, w, h),
							gfxBorders, contentBorders, flags );
}

WgBlockSetPtr WgSurface::defineBlockSet( const WgVertTile5& tile, const WgBorders& gfxBorders, const WgBorders& contentBorders, Uint32 flags ) const
{
	int	w = tile.w;
	int h = (tile.h - tile.skip*4) / 5;
	int	skip = h + tile.skip;

	return defineBlockSet(	WgRect(tile.x, tile.y, w, h),
							WgRect(tile.x, tile.y+skip, w, h),
							WgRect(tile.x, tile.y+skip*2, w, h),
							WgRect(tile.x, tile.y+skip*3, w, h),
							WgRect(tile.x, tile.y+skip*4, w, h),
							gfxBorders, contentBorders, flags );
}

WgBlockSetPtr WgSurface::defineBlockSet( const WgVertTile4& tile, const WgBorders& gfxBorders, const WgBorders& contentBorders, Uint32 flags ) const
{
	int	w = tile.w;
	int h = (tile.h - tile.skip*3) / 4;
	int	skip = h + tile.skip;

	return defineBlockSet(	WgRect(tile.x, tile.y, w, h),
							WgRect(tile.x, tile.y+skip, w, h),
							WgRect(tile.x, tile.y+skip*2, w, h),
							WgRect(tile.x, tile.y+skip*3, w, h),
							gfxBorders, contentBorders, flags );
}

WgBlockSetPtr WgSurface::defineBlockSet( const WgVertTile3& tile, const WgBorders& gfxBorders, const WgBorders& contentBorders, Uint32 flags ) const
{
	int	w = tile.w;
	int h = (tile.h - tile.skip*2) / 3;
	int	skip = h + tile.skip;

	return defineBlockSet(	WgRect(tile.x, tile.y, w, h),
							WgRect(tile.x, tile.y+skip, w, h),
							WgRect(tile.x, tile.y+skip*2, w, h),
							gfxBorders, contentBorders, flags );
}

WgBlockSetPtr WgSurface::defineBlockSet( const WgVertTile2& tile, const WgBorders& gfxBorders, const WgBorders& contentBorders, Uint32 flags ) const
{
	int	w = tile.w;
	int h = tile.h / 2;
	int	skip = h + tile.skip;

	return defineBlockSet(	WgRect(tile.x, tile.y, w, h),
							WgRect(tile.x, tile.y+skip, w, h),
							gfxBorders, contentBorders, flags );
}

