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


WgBlockSet::WgBlockSet(	WgMemPool * pPool, const WgSurface * pSurf, const WgRect& normal, 
					   const WgRect& marked, const WgRect& selected, const WgRect& disabled, 
					   const WgRect& special, const WgBorders& gfxBorders, 
					   const WgBorders& contentBorders, Uint32 flags ) : WgRefCountedPooled(pPool)
{
	m_base.pSurf				= pSurf;
	m_base.gfxBorders			= gfxBorders;
	m_base.contentBorders		= contentBorders;
	m_flags						= VerifyFlags(flags);
	m_base.w					= normal.w;
	m_base.h					= normal.h;

	m_base.x[WG_MODE_NORMAL]	= normal.x;
	m_base.x[WG_MODE_MARKED]	= marked.x;
	m_base.x[WG_MODE_SELECTED]	= selected.x;
	m_base.x[WG_MODE_DISABLED]	= disabled.x;
	m_base.x[WG_MODE_SPECIAL]	= special.x;

	m_base.y[WG_MODE_NORMAL]	= normal.y;
	m_base.y[WG_MODE_MARKED]	= marked.y;
	m_base.y[WG_MODE_SELECTED]	= selected.y;
	m_base.y[WG_MODE_DISABLED]	= disabled.y;
	m_base.y[WG_MODE_SPECIAL]	= special.y;
}

WgBlockSet::WgBlockSet(	WgMemPool * pPool, const WgSurface * pSurf, const WgBorders& gfxBorders, const WgBorders& contentBorders, Uint32 flags ) : WgRefCountedPooled(pPool)
{
	m_base.pSurf				= pSurf;
	m_base.gfxBorders			= gfxBorders;
	m_base.contentBorders		= contentBorders;
	m_flags						= VerifyFlags(flags);
	m_base.w					= 0;
	m_base.h					= 0;

	m_base.x[WG_MODE_NORMAL]	= 0;
	m_base.x[WG_MODE_MARKED]	= 0;
	m_base.x[WG_MODE_SELECTED]	= 0;
	m_base.x[WG_MODE_DISABLED]	= 0;
	m_base.x[WG_MODE_SPECIAL]	= 0;

	m_base.y[WG_MODE_NORMAL]	= 0;
	m_base.y[WG_MODE_MARKED]	= 0;
	m_base.y[WG_MODE_SELECTED]	= 0;
	m_base.y[WG_MODE_DISABLED]	= 0;
	m_base.y[WG_MODE_SPECIAL]	= 0;
}

//____ AddAlternative() _______________________________________________________________

bool WgBlockSet::AddAlternative( WgSize activationSize, const WgSurface * pSurf, const WgRect& normal, const WgRect& marked, 
						const WgRect& selected, const WgRect& disabled, const WgRect& special, 
						const WgBorders& gfxBorders, const WgBorders& contentBorders )
{
	if( activationSize.w == 0 && activationSize.h == 0 )
		return false;

	LinkedAlt * p = new LinkedAlt();

	p->activationSize		= activationSize;
	p->data.contentBorders	= contentBorders;
	p->data.gfxBorders		= gfxBorders;
	p->data.w				= normal.w;
	p->data.h				= normal.h;
	p->data.pSurf			= pSurf;

	p->data.x[WG_MODE_NORMAL]	= normal.x;
	p->data.x[WG_MODE_MARKED]	= marked.x;
	p->data.x[WG_MODE_SELECTED]	= selected.x;
	p->data.x[WG_MODE_DISABLED]	= disabled.x;
	p->data.x[WG_MODE_SPECIAL]	= special.x;

	p->data.y[WG_MODE_NORMAL]	= normal.y;
	p->data.y[WG_MODE_MARKED]	= marked.y;
	p->data.y[WG_MODE_SELECTED]	= selected.y;
	p->data.y[WG_MODE_DISABLED]	= disabled.y;
	p->data.y[WG_MODE_SPECIAL]	= special.y;



	LinkedAlt * pOther = m_altChain.getFirst();
	while( pOther )
	{
		if( pOther->activationSize.w == p->activationSize.w && pOther->activationSize.h == p->activationSize.h )
		{
			delete p;
			return false;			// Already have a Alt with this activation size.
		}

		if( pOther->activationSize.w < p->activationSize.w || pOther->activationSize.h < p->activationSize.h )
		{
			if( pOther->activationSize.w > p->activationSize.w || pOther->activationSize.h > p->activationSize.h )
			{
				delete p;
				return false;			// Can't determine Alt-order, conflicting activation-sizes (smaller in one dimension but bigger in the other).
			}

			p->moveBefore( pOther );
			return true;
		}

		pOther = pOther->getNext();
	}

	m_altChain.push_back( p );
	return true;
}

//____ GetNbAlternatives() ____________________________________________________

int WgBlockSet::GetNbAlternatives() const
{
	int nb = 1;
	LinkedAlt * p = m_altChain.getFirst();
	while( p )
	{
		nb++;
		p = p->getNext();
	}
	return nb;
}

//____ GetActivationSize() ____________________________________________________

WgSize WgBlockSet::GetActivationSize( int alt ) const
{
	if( alt == 0 )
		return WgSize(0,0);

	int nb = 1;
	LinkedAlt * p = m_altChain.getFirst();
	while( p )
	{
		if( nb == alt )
			return p->activationSize;

		nb++;
		p = p->getNext();
	}

	return WgSize(0,0);
}


//____ GetAlt() _______________________________________________________________

WgBlockSet::Alt_Data* WgBlockSet::GetAlt( int n )
{
	if( n == 0 )
		return &m_base;

	int nb = 1;
	LinkedAlt * p = m_altChain.getFirst();
	while( p )
	{
		if( nb == n )
			return &p->data;

		nb++;
		p = p->getNext();
	}
	return 0;
}

WgBlockSet::Alt_Data* WgBlockSet::GetAlt( WgSize destSize )
{
	Alt_Data * pData = &m_base;

	LinkedAlt * p = m_altChain.getFirst();
	while( p )
	{
		if( destSize.w > p->activationSize.w && destSize.h > p->activationSize.h )
			break;

		pData = &p->data;
		p = p->getNext();
	}
	return pData;
}

const WgBlockSet::Alt_Data* WgBlockSet::GetAlt( int n ) const
{
	if( n == 0 )
		return &m_base;

	int nb = 1;
	LinkedAlt * p = m_altChain.getFirst();
	while( p )
	{
		if( nb == n )
			return &p->data;

		nb++;
		p = p->getNext();
	}
	return 0;
}

const WgBlockSet::Alt_Data* WgBlockSet::GetAlt( WgSize destSize ) const
{
	const Alt_Data * pData = &m_base;

	LinkedAlt * p = m_altChain.getFirst();
	while( p )
	{
		if( destSize.w > p->activationSize.w && destSize.h > p->activationSize.h )
			break;

		pData = &p->data;
		p = p->getNext();
	}
	return pData;
}


//____ SetPos() _______________________________________________________________

bool WgBlockSet::SetPos( WgMode mode, WgCord pos, int alt )
{
	Alt_Data * p = GetAlt(alt);
	if( !p )
		return false;

	p->x[mode] = pos.x;
	p->y[mode] = pos.y;
	return true;
}

//____ SetSize() ______________________________________________________________

bool WgBlockSet::SetSize( WgSize size, int alt )
{
	Alt_Data * p = GetAlt(alt);
	if( !p )
		return false;

	p->w = size.w;
	p->h = size.h;
	return true;
}

//____ GetRect() ______________________________________________________________

WgRect WgBlockSet::GetRect( WgMode mode, int alt ) const
{
	const Alt_Data * p = GetAlt(alt);
	if( !p )
		return WgRect();

	return WgRect( p->x[mode], p->y[mode], p->w, p->h );
}

//____ GetSize() ______________________________________________________________

WgSize WgBlockSet::GetSize( int alt ) const
{
	const Alt_Data * p = GetAlt(alt);
	if( !p )
		return WgSize();

	return WgSize( p->w, p->h );
}


//____ GetWidth() ______________________________________________________________

int WgBlockSet::GetWidth( int alt ) const
{
	const Alt_Data * p = GetAlt(alt);
	if( !p )
		return 0;

	return p->w;
}

//____ GetHeight() ____________________________________________________________

int WgBlockSet::GetHeight( int alt ) const
{
	const Alt_Data * p = GetAlt(alt);
	if( !p )
		return 0;

	return p->h;
}

//____ GetMinSize() ___________________________________________________________

WgSize WgBlockSet::GetMinSize( int alt ) const 
{ 
	const Alt_Data * p = GetAlt(alt);
	if( !p )
		return WgSize();

	return p->gfxBorders.size(); 
}

//____ GetMinWidth() __________________________________________________________

int WgBlockSet::GetMinWidth( int alt ) const 
{ 
	const Alt_Data * p = GetAlt(alt);
	if( !p )
		return 0;

	return p->gfxBorders.width(); 
}

//____ GetMinHeight() _________________________________________________________

int WgBlockSet::GetMinHeight( int alt ) const
{ 
	const Alt_Data * p = GetAlt(alt);
	if( !p )
		return 0;

	return p->gfxBorders.height(); 
}

//____ GetSurface() ___________________________________________________________

const WgSurface * WgBlockSet::GetSurface( int alt ) const 
{ 
	const Alt_Data * p = GetAlt(alt);
	if( !p )
		return 0;

	return p->pSurf; 
}


//____ GetGfxBorders() ________________________________________________________

WgBorders WgBlockSet::GetGfxBorders( int alt ) const 
{ 
	const Alt_Data * p = GetAlt(alt);
	if( !p )
		return WgBorders();

	return p->gfxBorders; 
}

//____ SetGfxBorders() ________________________________________________________

void WgBlockSet::SetGfxBorders( const WgBorders& borders, int alt )
{ 
	Alt_Data * p = GetAlt(alt);
	if( !p )
		return;

	p->gfxBorders = borders;
}

//____ GetContentBorders() ____________________________________________________

WgBorders WgBlockSet::GetContentBorders( int alt ) const 
{ 
	const Alt_Data * p = GetAlt(alt);
	if( !p )
		return WgBorders();

	return p->contentBorders; 
}

//____ SetContentBorders() ____________________________________________________

void WgBlockSet::SetContentBorders( const WgBorders& borders, int alt ) 
{ 
	Alt_Data * p = GetAlt(alt);
	if( !p )
		return;

	p->contentBorders = borders; 
}


//____ SetTile() ______________________________________________________________

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

//____ SetScale() _____________________________________________________________

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

//____ VerifyFlags() __________________________________________________________

Uint32 WgBlockSet::VerifyFlags(Uint32 _flags)
{
	if((_flags & (WG_SCALE_CENTER | WG_TILE_CENTER)) == (WG_SCALE_CENTER | WG_TILE_CENTER))
	{
		assert(0);
		return _flags &~ WG_TILE_CENTER;
	}
	return _flags;
}

//____ SameBlock() ____________________________________________________________

bool WgBlockSet::SameBlock( WgMode one, WgMode two, int alt )
{ 
	Alt_Data * p = GetAlt(alt);
	if( !p )
		return false;

	if( p->x[one] == p->x[two] && p->y[one] == p->y[two] ) 
		return true; 

	return false; 
}
