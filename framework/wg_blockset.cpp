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
#include <wg_colorset.h>
#include <wg_geo.h>
#include <wg3_surface.h>
#include <assert.h>
#include <wg_mempool.h>

WgMemPool * WgBlockset::g_pMemPool = 0;


WgBlock::WgBlock( wg::Surface * pSurf, const WgRect& rect, const WgBorders& sourceFrame, const WgBorders& canvasFrame, const WgBorders& padding, WgCoord contentShift, int scale, Uint32 flags )
{
	m_pSurf			= pSurf;
	m_rect			= rect;
	m_sourceFrame	= sourceFrame;
	m_canvasFrame	= canvasFrame;
	m_flags			= flags;
	m_padding		= padding;
	m_contentShift	= contentShift;
	m_scale 		= scale;

	if( m_sourceFrame.left || m_sourceFrame.right || m_sourceFrame.top || m_sourceFrame.bottom )
		m_flags |= WG_HAS_BORDERS;

	if(m_rect.x + m_rect.w > (int)m_pSurf->size().w)
		m_rect.w = m_pSurf->size().w - m_rect.x;
	if(m_rect.y + m_rect.h > (int)m_pSurf->size().h)
		m_rect.h = m_pSurf->size().h - m_rect.y;
}


bool WgBlock::operator==( const WgBlock& b) const
{
	if( m_pSurf == b.m_pSurf && m_rect == b.m_rect /*Ś&& m_frame == b.m_frame && 
	    m_flags == b.m_flags && m_padding == b.m_padding &&
		m_contentShift == b.m_contentShift*/ )
		return true;

	return false;
}

bool WgBlock::operator!=( const WgBlock& b) const
{
	if( m_pSurf == b.m_pSurf && m_rect == b.m_rect && m_sourceFrame == b.m_sourceFrame &&
		m_canvasFrame == b.m_canvasFrame && 
	    m_flags == b.m_flags && m_padding == b.m_padding &&
		m_contentShift == b.m_contentShift )
		return false;

	return true;
}

//____ CreateFromSurface() _____________________________________________________

WgBlocksetPtr WgBlockset::CreateFromSurface( wg::Surface * pSurf, int flags )
{
	WgBlockset * p = _alloc( pSurf, flags );
	p->m_size		= pSurf->size() * pSurf->scale();
	
	return WgBlocksetPtr(p);
}

//____ CreateFromRect() ________________________________________________________

WgBlocksetPtr WgBlockset::CreateFromRect( wg::Surface * pSurf, const WgRect& normal, int flags )
{
	WgBlockset * p = _alloc( pSurf, flags );
	p->m_size			= normal;

	p->m_pos[WG_MODE_NORMAL]		= normal;
	p->m_pos[WG_MODE_MARKED]		= normal;
	p->m_pos[WG_MODE_SELECTED]		= normal;
	p->m_pos[WG_MODE_DISABLED]		= normal;
	p->m_pos[WG_MODE_SPECIAL]		= normal;

	return WgBlocksetPtr(p);
}

//____ CreateFromRects() _______________________________________________________

WgBlocksetPtr WgBlockset::CreateFromRects( wg::Surface * pSurf, const WgRect& normal, const WgCoord& marked, const WgCoord& selected, int flags )
{
	WgBlockset * p = _alloc( pSurf, flags );
	p->m_size			= normal;

	p->m_pos[WG_MODE_NORMAL]		= normal;
	p->m_pos[WG_MODE_MARKED]		= marked;
	p->m_pos[WG_MODE_SELECTED]		= selected;
	p->m_pos[WG_MODE_DISABLED]		= normal;
	p->m_pos[WG_MODE_SPECIAL]		= normal;

	return WgBlocksetPtr(p);
}

WgBlocksetPtr WgBlockset::CreateFromRects( wg::Surface * pSurf, const WgRect& normal, const WgCoord& marked, const WgCoord& selected, const WgCoord& disabled, int flags )
{
	WgBlockset * p = _alloc( pSurf, flags );
	p->m_size			= normal;

	p->m_pos[WG_MODE_NORMAL]		= normal;
	p->m_pos[WG_MODE_MARKED]		= marked;
	p->m_pos[WG_MODE_SELECTED]		= selected;
	p->m_pos[WG_MODE_DISABLED]		= disabled;
	p->m_pos[WG_MODE_SPECIAL]		= normal;

	return WgBlocksetPtr(p);
}

WgBlocksetPtr WgBlockset::CreateFromRects( wg::Surface * pSurf, const WgRect& normal, const WgCoord& marked, const WgCoord& selected, const WgCoord& disabled, const WgCoord& special, int flags )
{
	WgBlockset * p = _alloc( pSurf, flags );
	p->m_size			= normal;

	p->m_pos[WG_MODE_NORMAL]		= normal;
	p->m_pos[WG_MODE_MARKED]		= marked;
	p->m_pos[WG_MODE_SELECTED]		= selected;
	p->m_pos[WG_MODE_DISABLED]		= disabled;
	p->m_pos[WG_MODE_SPECIAL]		= special;

	return WgBlocksetPtr(p);
}

//____ CreateFromRow() _________________________________________________________

WgBlocksetPtr WgBlockset::CreateFromRow( wg::Surface * pSurf, const WgRect& rect, int nBlocks, int padding, int flags )
{
	int w = (rect.w - (nBlocks-1)*padding)/nBlocks;
	int h = rect.h;

	int ofs = w + padding;

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

	return WgBlocksetPtr(p);
}

//____ CreateFromColumn() ______________________________________________________

WgBlocksetPtr WgBlockset::CreateFromColumn( wg::Surface * pSurf, const WgRect& rect, int nBlocks, int padding, int flags )
{
	int w = rect.w;
	int h = (rect.h - (nBlocks-1)*padding)/nBlocks;

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

	return WgBlocksetPtr(p);
}


//____ _alloc() ________________________________________________________________

WgBlockset * WgBlockset::_alloc( wg::Surface * pSurf, int flags )
{
	if( !g_pMemPool )
		g_pMemPool = new WgMemPool( 16, sizeof(WgBlockset) );

	return new(g_pMemPool->AllocEntry())WgBlockset(g_pMemPool, pSurf, (Uint32) flags );
}


//____ Constructor() ___________________________________________________________

WgBlockset::WgBlockset(	WgMemPool * pPool, wg::Surface * pSurf, Uint32 flags ) : WgRefCountedPooled(pPool)
{
	m_flags						= _verifyFlags(flags);
	m_base.pSurf				= pSurf;
}


//____ AddAlternative() _______________________________________________________________

bool WgBlockset::AddAlternative( wg::Surface * pSurf )
{
	if( !pSurf || pSurf->scale() <= m_base.pSurf->scale() )
		return false;

	LinkedAlt * p = new LinkedAlt();

	p->data.pSurf			= pSurf;

	m_altChain.PushBack( p );
	return true;
}


//____ GetAlt() _______________________________________________________________

WgBlockset::Alt_Data* WgBlockset::_getAlt( int n )
{
	if( n == 0 )
		return &m_base;

	int nb = 1;
	LinkedAlt * p = m_altChain.First();
	while( p )
	{
		if( nb == n )
			return &p->data;

		nb++;
		p = p->Next();
	}
	return 0;
}

const WgBlockset::Alt_Data* WgBlockset::_getAlt( int n ) const
{
	if( n == 0 )
		return &m_base;

	int nb = 1;
	LinkedAlt * p = m_altChain.First();
	while( p )
	{
		if( nb == n )
			return &p->data;

		nb++;
		p = p->Next();
	}
	return 0;
}

//____ _getAltForScale() _______________________________________________________

const WgBlockset::Alt_Data*	WgBlockset::_getAltForScale( int _scale ) const
{
    float scale = _scale / 4096.f;
    
	if( scale <= m_base.pSurf->scale() )
		return &m_base;

	LinkedAlt * p = m_altChain.Last();
	while( p )
	{
		if( p->data.pSurf->scale() <= scale )
			return &p->data;
		p = p->Prev();
	}
	return &m_base;
}


//____ TextColor() _____________________________________________________________

WgColor WgBlockset::TextColor( WgMode mode ) const
{
	if( !m_pTextColors )
		return WgColor::Transparent;

	return m_pTextColors->Color(mode);
}


//____ Rect() __________________________________________________________________

WgRect WgBlockset::Rect( WgMode mode, int scale ) const
{
	return WgRect( m_pos[mode] * scale / WG_SCALE_BASE, m_size * scale / WG_SCALE_BASE );
}

//____ Size() __________________________________________________________________

WgSize WgBlockset::Size( int scale ) const
{
	return m_size * scale / WG_SCALE_BASE;
}

//____ Width() _________________________________________________________________

int WgBlockset::Width( int scale ) const
{
    return m_size.w * scale / WG_SCALE_BASE;
}

//____ Height() ________________________________________________________________

int WgBlockset::Height( int scale ) const
{
    return m_size.h * scale / WG_SCALE_BASE;
}

//____ MinSize() _______________________________________________________________

WgSize WgBlockset::MinSize( int scale ) const
{
	return m_frame.scale(scale).size();
}

//____ MinWidth() ______________________________________________________________

int WgBlockset::MinWidth( int scale ) const
{
    return m_frame.scale(scale).width();
}

//____ MinHeight() _____________________________________________________________

int WgBlockset::MinHeight( int scale ) const
{
    return m_frame.scale(scale).height();
}

//____ Surface() _______________________________________________________________

wg::Surface_p WgBlockset::Surface( int scale ) const
{
	const Alt_Data * p = _getAltForScale(scale);
	if( !p )
		return 0;

	return p->pSurf;
}


//____ Frame() ____________________________________________________________

WgBorders WgBlockset::Frame( int scale ) const
{
	return m_frame.scale(scale);
}


//____ Padding() ____________________________________________________

WgBorders WgBlockset::Padding( int scale ) const
{
	return m_padding.scale(scale);
}


//____ ContentShift() __________________________________________________

WgCoord WgBlockset::ContentShift( WgMode mode, int scale ) const
{
    return (WgCoord(m_contentShift[mode].x, m_contentShift[mode].y)*scale)/WG_SCALE_BASE;
}


//____ SetFrame() ________________________________________________________

void WgBlockset::SetFrame( const WgBorders& frame )
{
	m_frame = frame;
}


//____ SetPadding() ____________________________________________________

void WgBlockset::SetPadding( const WgBorders& padding )
{
	m_padding = padding;
}


//____ SetContentShift() _______________________________________________

bool WgBlockset::SetContentShift( WgMode mode, WgCoord ofs )
{
	if( !(mode == WG_MODE_MARKED || mode == WG_MODE_SELECTED) )
		return false;

	m_contentShift[mode].x = ofs.x;
	m_contentShift[mode].y = ofs.y;
	return true;
}

//____ SetTextColors() _________________________________________________________

void WgBlockset::SetTextColors( const WgColorsetPtr& colors )
{
	m_pTextColors = colors;
}

//____ SetTile() ______________________________________________________________

bool WgBlockset::SetTile(Uint32 place, bool bTile)
{
	if((place & WG_TILE_ALL) != place)
		return false;

	// cannot scale and tile at the same time
	if((place & WG_TILE_CENTER) && (IsScaled() || IsFixedSize()))
		return false;

	if(bTile)
		m_flags |= place;
	else
		m_flags &= ~place;

	return true;
}

//____ SetScale() _____________________________________________________________

bool WgBlockset::SetScale(bool bScale)
{
	// cannot scale and tile at the same time
	if((IsFixedSize() || HasTiledCenter()) && bScale)
		return false;

	if(bScale)
		m_flags |= WG_SCALE_CENTER;
	else
		m_flags &= ~WG_SCALE_CENTER;

	return true;
}

//____ SetFixedSize() _____________________________________________________________

bool WgBlockset::SetFixedSize(bool bFixedSize)
{
	if( IsScaled() || HasTiledCenter() )
		return false;

	if(bFixedSize)
		m_flags |= WG_FIXED_CENTER;
	else
		m_flags &= ~WG_FIXED_CENTER;

	return true;
}


//____ VerifyFlags() __________________________________________________________

Uint32 WgBlockset::_verifyFlags(Uint32 _flags)
{
	// Make sure not more than one of SCALE/TILE/FIXED is set on center.

	int centerFlags = _flags & (WG_SCALE_CENTER | WG_TILE_CENTER | WG_FIXED_CENTER);

	if( centerFlags != 0 && centerFlags != WG_SCALE_CENTER && centerFlags != WG_TILE_CENTER && centerFlags != WG_FIXED_CENTER )
	{
		assert(0);
		return _flags &~ (WG_SCALE_CENTER | WG_TILE_CENTER | WG_FIXED_CENTER);		// Fall back to stretch.
	}
	return _flags;
}

//____ SameBlock() ____________________________________________________________

bool WgBlockset::SameBlock( WgMode one, WgMode two )
{
	if( m_pos[one] == m_pos[two] )
		return true;

	return false;
}

//____ _getBlock() ____________________________________________________________

WgBlock WgBlockset::_getBlock(WgMode m, const Alt_Data * p, int scale) const
{
	if( !p )
		return WgBlock();

	const Uint32 SKIP_MASK = WG_SKIP_NORMAL | WG_SKIP_MARKED | WG_SKIP_SELECTED | WG_SKIP_DISABLED | WG_SKIP_SPECIAL;
	Uint32 flags = m_flags & ~SKIP_MASK;
	flags |= IsModeSkipable(m) ? WG_SKIP_NORMAL : 0;	// reuse bit

    int scaleFactor = (int)(p->pSurf->scale() * 4096);

	WgRect source((m_pos[m] * scaleFactor) / WG_SCALE_BASE, (m_size*scaleFactor) / WG_SCALE_BASE );
	WgCoord shift( (m_contentShift[m].x * scale) >> WG_SCALE_BINALS, (m_contentShift[m].y * scale) >> WG_SCALE_BINALS );

	return WgBlock( p->pSurf, source, m_frame.scale(scaleFactor), m_frame.scale(scale), m_padding.scale(scale), shift, scale, flags );
}

