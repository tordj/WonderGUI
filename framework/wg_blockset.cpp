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

#include <wg_blockset.h>

/*
static wg::BlockSkin_p CreateFromSurface( wg::Surface * pSurf, int flags = 0 );
static wg::BlockSkin_p CreateFromRect( wg::Surface * pSurf, const WgRect& normal, int flags = 0 );
static wg::BlockSkin_p CreateFromRects( wg::Surface * pSurf, const WgRect& normal, const WgCoord& marked, const WgCoord& selected, int flags = 0 );
static wg::BlockSkin_p CreateFromRects( wg::Surface * pSurf, const WgRect& normal, const WgCoord& marked, const WgCoord& selected, const WgCoord& disabled, int flags = 0 );
static wg::BlockSkin_p CreateFromRects( wg::Surface * pSurf, const WgRect& normal, const WgCoord& marked, const WgCoord& selected, const WgCoord& disabled, const WgCoord& special, int flags = 0 );
static wg::BlockSkin_p CreateFromRow( wg::Surface * pSurf, const WgRect& rect, int nBlocks, int spacing=0, int flags = 0 );
static wg::BlockSkin_p CreateFromColumn( wg::Surface * pSurf, const WgRect& rect, int nBlocks, int spacing=0, int flags = 0 );
*/


//____ CreateFromSurface() _____________________________________________________

wg::BlockSkin_p WgBlockset::CreateFromSurface( wg::Surface * pSurf, int flags )
{
    return wg::BlockSkin::create(pSurf);
}

//____ CreateFromRect() ________________________________________________________

wg::BlockSkin_p WgBlockset::CreateFromRect( wg::Surface * pSurf, const WgRect& normal, int flags )
{
    return wg::BlockSkin::create(pSurf, normal);
}

//____ CreateFromRects() _______________________________________________________

wg::BlockSkin_p WgBlockset::CreateFromRects( wg::Surface * pSurf, const WgRect& normal, const WgCoord& marked, const WgCoord& selected, int flags )
{
    auto p = wg::BlockSkin::create();

    p->setBlock(wg::StateEnum::Hovered, marked);
    p->setBlock(wg::StateEnum::Pressed, selected);
    p->setBlock(wg::StateEnum::Selected, selected);

    return p;
/*
	WgBlockset * p = _alloc( pSurf, flags );
	p->m_size			= normal;

	p->m_pos[WG_MODE_NORMAL]		= normal;
	p->m_pos[WG_MODE_MARKED]		= marked;
	p->m_pos[WG_MODE_SELECTED]		= selected;
	p->m_pos[WG_MODE_DISABLED]		= normal;
	p->m_pos[WG_MODE_SPECIAL]		= normal;

	return wg::BlockSkin_p(p);
*/
}

wg::BlockSkin_p WgBlockset::CreateFromRects( wg::Surface * pSurf, const WgRect& normal, const WgCoord& marked, const WgCoord& selected, const WgCoord& disabled, int flags )
{
    auto p = wg::BlockSkin::create(pSurf, normal);

    p->setBlock(wg::StateEnum::Hovered, marked);
    p->setBlock(wg::StateEnum::Pressed, selected);
    p->setBlock(wg::StateEnum::Selected, selected);
    p->setBlock(wg::StateEnum::Disabled, disabled);

    return p;
/*
    WgBlockset * p = _alloc( pSurf, flags );
	p->m_size			= normal;

	p->m_pos[WG_MODE_NORMAL]		= normal;
	p->m_pos[WG_MODE_MARKED]		= marked;
	p->m_pos[WG_MODE_SELECTED]		= selected;
	p->m_pos[WG_MODE_DISABLED]		= disabled;
	p->m_pos[WG_MODE_SPECIAL]		= normal;

	return wg::BlockSkin_p(p);
*/
}

wg::BlockSkin_p WgBlockset::CreateFromRects( wg::Surface * pSurf, const WgRect& normal, const WgCoord& marked, const WgCoord& selected, const WgCoord& disabled, const WgCoord& special, int flags )
{
    auto p = wg::BlockSkin::create(pSurf, normal);
    
    p->setBlock(wg::StateEnum::Hovered, marked);
    p->setBlock(wg::StateEnum::Pressed, selected);
    p->setBlock(wg::StateEnum::Selected, selected);
    p->setBlock(wg::StateEnum::Disabled, disabled);
    p->setBlock(wg::StateEnum::SelectedHovered, special);
    p->setBlock(wg::StateEnum::SelectedPressed, special);

    return p;
/*
    WgBlockset * p = _alloc( pSurf, flags );
	p->m_size			= normal;

	p->m_pos[WG_MODE_NORMAL]		= normal;
	p->m_pos[WG_MODE_MARKED]		= marked;
	p->m_pos[WG_MODE_SELECTED]		= selected;
	p->m_pos[WG_MODE_DISABLED]		= disabled;
	p->m_pos[WG_MODE_SPECIAL]		= special;

	return wg::BlockSkin_p(p);
*/
}

//____ CreateFromRow() _________________________________________________________

wg::BlockSkin_p WgBlockset::CreateFromRow( wg::Surface * pSurf, const WgRect& rect, int nBlocks, int padding, int flags )
{
    int w = (rect.w - (nBlocks-1)*padding)/nBlocks;
	int h = rect.h;

    wg::CoordI ofs = { w + padding, 0 };

    auto p = wg::BlockSkin::create(pSurf, rect);
    
    if( nBlocks > 1 )
        p->setBlock(wg::StateEnum::Hovered, rect + ofs);

    if( nBlocks > 2 )
    {
        p->setBlock(wg::StateEnum::Pressed, rect + ofs*2);
        p->setBlock(wg::StateEnum::Selected, rect + ofs*2);
    }
    
    if( nBlocks > 3 )
        p->setBlock(wg::StateEnum::Disabled, rect + ofs*3);

    if( nBlocks > 4 )
    {
        p->setBlock(wg::StateEnum::SelectedHovered, rect + ofs*4);
        p->setBlock(wg::StateEnum::SelectedPressed, rect + ofs*4);
    }
    
    return p;
/*
	WgBlockset * p = _alloc( pSurf, flags );
	p->m_size			= {w,h};

	p->m_pos[WG_MODE_NORMAL]		= rect;
	p->m_pos[WG_MODE_MARKED]		= rect;
	p->m_pos[WG_MODE_SELECTED]		= rect;
	p->m_pos[WG_MODE_DISABLED]		= rect;
	p->m_pos[WG_MODE_SPECIAL]		= rect;

	if( nBlocks > 1 )
		p->m_pos[WG_MODE_MARKED].x	= rect.x + ofs;

	if( nBlocks > 2 )
		p->m_pos[WG_MODE_SELECTED].x	= rect.x + ofs*2;

	if( nBlocks > 3 )
		p->m_pos[WG_MODE_DISABLED].x	= rect.x + ofs*3;

	if( nBlocks > 4 )
		p->m_pos[WG_MODE_SPECIAL].x	= rect.x + ofs*4;

	return wg::BlockSkin_p(p);
*/
}

//____ CreateFromColumn() ______________________________________________________

wg::BlockSkin_p WgBlockset::CreateFromColumn( wg::Surface * pSurf, const WgRect& rect, int nBlocks, int padding, int flags )
{
	int w = rect.w;
	int h = (rect.h - (nBlocks-1)*padding)/nBlocks;


    wg::CoordI ofs = { 0, h + padding };
    
    auto p = wg::BlockSkin::create(pSurf, rect);
    
    if( nBlocks > 1 )
        p->setBlock(wg::StateEnum::Hovered, rect + ofs);
    
    if( nBlocks > 2 )
    {
        p->setBlock(wg::StateEnum::Pressed, rect + ofs*2);
        p->setBlock(wg::StateEnum::Selected, rect + ofs*2);
    }
    
    if( nBlocks > 3 )
        p->setBlock(wg::StateEnum::Disabled, rect + ofs*3);
    
    if( nBlocks > 4 )
    {
        p->setBlock(wg::StateEnum::SelectedHovered, rect + ofs*4);
        p->setBlock(wg::StateEnum::SelectedPressed, rect + ofs*4);
    }
    
    return p;

/*
    int ofs = h + padding;

    WgBlockset * p = _alloc( pSurf, flags );
	p->m_size			= {w,h};

	p->m_pos[WG_MODE_NORMAL]		= rect;
	p->m_pos[WG_MODE_MARKED]		= rect;
	p->m_pos[WG_MODE_SELECTED]		= rect;
	p->m_pos[WG_MODE_DISABLED]		= rect;
	p->m_pos[WG_MODE_SPECIAL]		= rect;

	if( nBlocks > 1 )
		p->m_pos[WG_MODE_MARKED].y	= rect.y + ofs;

	if( nBlocks > 2 )
		p->m_pos[WG_MODE_SELECTED].y	= rect.y + ofs*2;

	if( nBlocks > 3 )
		p->m_pos[WG_MODE_DISABLED].y	= rect.y + ofs*3;

	if( nBlocks > 4 )
		p->m_pos[WG_MODE_SPECIAL].y	= rect.y + ofs*4;

	return wg::BlockSkin_p(p);
 */
}


