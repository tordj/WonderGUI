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

#include <wdg_listview.h>
#include <wg_gfx.h>
#include <wg_item_row.h>

static const char	Wdg_Type[] = {"TordJ/ListView"};


//____ NewOfMyType() __________________________________________________________

WgWidget * Wdg_ListView::NewOfMyType() const
{
	return new Wdg_ListView;
}


//____ ~Wdg_ListView() ________________________________________________________

Wdg_ListView::~Wdg_ListView()
{
}


//____ Init() _________________________________________________________________

void Wdg_ListView::Init()
{
	m_pLastMarkedItem = 0;

	m_bAutoScrollMarked = false;

	m_pLineMarkSurf		= 0;
	m_lineMarkSrc		= WgRect( 0,0,0,0 );
	m_lineMarkTileOfs	= 0;
	m_lineMarkTileLen	= 0;
	m_bLineMarkStretch	= false;
}



//____ Type() _________________________________________________________________

const char * Wdg_ListView::Type( void ) const
{
	return GetMyType();
}

const char * Wdg_ListView::GetMyType( void )
{
	return Wdg_Type;
}



//____ UpdateContentSize() __________________________________________________________

void Wdg_ListView::UpdateContentSize()
{
	Uint32	contentWidth = 0;
	Uint32	contentHeight = 0;

	WgItem * p = m_items.First();

	if( p )
		contentHeight -= m_itemSpacing;		// We will get one spacing too much...

	while( p )
	{
		if( p->Width() > contentWidth )
			contentWidth = p->Width();

		contentHeight += p->Height() + m_itemSpacing;
		p = p->GetNext();
	}

	SetContentSize( contentWidth, contentHeight );
}

//____ refreshItems() __________________________________________________________

void Wdg_ListView::refreshItems()
{
	UpdateContentSize();

	// Check so we still have our m_pLastMarkedItem...

	if( !DoWeHaveItem(m_pLastMarkedItem) )
		m_pLastMarkedItem = 0;

	RequestRender();
}

//____ ItemSizeModified() _________________________________________________________

void Wdg_ListView::ItemSizeModified( WgItem * pItem, Sint32 widthDiff , Sint32 heightDiff )
{
	// this is wrong. an item's size changing doesn't necessarily mean
	// that the content size is changing.
	//SetContentSize( m_contentWidth+widthDiff, m_contentHeight+heightDiff );
	//RequestRender();		//TODO: Only render what is necessary (if anything).

	// recalculate content size.

	if( heightDiff != 0 )
		refreshItems();
	else
		RequestRender();
}

//____ ItemVisibilityModified() _______________________________________________

void Wdg_ListView::ItemVisibilityModified( WgItem * pItem, bool bVisible )
{
	//TODO: More specific and optimized handling.
	
	refreshItems();
}


//____ ItemAdded() ____________________________________________________________

void Wdg_ListView::ItemAdded( WgItem * pItem )
{
	pItem->AdaptToWidth( ViewPixelLenX() );
	refreshItems();						// Slow way of doing this, can be optimized a lot.
}

//____ ItemMarkChanged() ________________________________________________

void Wdg_ListView::ItemMarkChanged( WgItem * pItem, bool bMarked )
{
	if( m_bAutoScrollMarked )
	{
		if( bMarked )	// make sure marked item is in view
			ScrollIntoView( pItem );
	}
}

//____ ScrollIntoView() ________________________________________________
void Wdg_ListView::ScrollIntoView( WgItem* pItem )
{
	if( !pItem )
		return;

	WgCord pos;

	if(!GetItemPixelPos(pItem, pos))
		return;

	Uint32 viewOfsMin = ViewPixelOfsY();
	Uint32 viewOfsMax = ViewPixelOfsY() + ViewPixelLenY();
	Uint32 itemPosY = (Uint32)pos.y;

	if( itemPosY < viewOfsMin )							// is the row above the view?
		SetViewPixelOfsY( itemPosY );
	else if( itemPosY + pItem->Height() > viewOfsMax )	// is the row below the view?
		SetViewPixelOfsY( itemPosY + pItem->Height() - ViewPixelLenY() );
}

bool Wdg_ListView::GetItemPixelPos( WgItem * pItem, WgCord& pos )
{
	// calc y pos of this row
	Uint32 itemPosY = 0;

	WgItem * p = m_items.First();
	while( p && p != pItem )
	{
		itemPosY += p->Height() + m_itemSpacing;
		p = p->Next();
	}

	pos = WgCord(0, itemPosY);

	if(!p)
		return false;

	return true;
}

void Wdg_ListView::SetLineMarkSource(WgSurface* pSurf, const WgRect& rect, Uint8 tileOfs, Uint8 tileLen, bool bStretch )
{
	m_pLineMarkSurf = pSurf;
	m_lineMarkSrc = rect;
	m_lineMarkTileOfs = tileOfs;
	m_lineMarkTileLen = tileLen;
	m_bLineMarkStretch = bStretch;
}

//____ DoMyOwnGeometryChangeSubclass() ________________________________________________

void Wdg_ListView::DoMyOwnGeometryChangeSubclass( WgRect& oldGeo, WgRect& newGeo )
{
	if( oldGeo.w != newGeo.w )
	{
		AdaptItemsToWidth( newGeo.w );
		UpdateContentSize();
	}
}


//____ GetMarkedItem() ________________________________________________________

WgItem* Wdg_ListView::GetMarkedItem( Uint32 x, Uint32 y )
{
	y += m_viewPixOfsY;							// Transform from relative widget cordinates to content-cordinates.

	WgItem * p = m_items.First();

	while( p )
	{
		if( p->Height() > y )
			return p->GetMarkedItem(x, y);

		y -= p->Height() +  m_itemSpacing;

		if( y < 0 )
			return 0;							// Fell in spacing between items.

		p = p->GetNext();
	}
	return 0;
}


//____ DoMyOwnActionRespond() __________________________________________________

void Wdg_ListView::DoMyOwnActionRespond( WgInput::UserAction _action, int _button_key, const WgActionDetails& _info, const WgInput& _inputObj )
{
	int x = _info.x;
	int y = _info.y;

	Abs2local( &x, &y );

	if( _action == WgInput::POINTER_EXIT && m_pLastMarkedItem != 0 )
	{
		m_pLastMarkedItem->ActionRespond( GetEmitter(), _action, _button_key, _info, _inputObj );
		m_pLastMarkedItem = 0;
		return;
	}

	WgItem * pItem = GetMarkedItem( (Uint32) x, (Uint32) y );

	if( pItem != m_pLastMarkedItem && m_pLastMarkedItem != 0 )
	{
		m_pLastMarkedItem->ActionRespond( GetEmitter(), WgInput::POINTER_EXIT, _button_key, _info, _inputObj );
		m_pLastMarkedItem = 0;
	}

	if( pItem )
	{
		// Make sure item gets a POINTER_ENTER.

		if( _action == WgInput::POINTER_OVER && !m_pLastMarkedItem )
			pItem->ActionRespond( GetEmitter(), WgInput::POINTER_ENTER, _button_key, _info, _inputObj );

		//

		// HACK. Remove when message loop is implemented
		// pItem can be deleted in the ActionResponse callback. Make sure it still exist // Martin
		pItem->ActionRespond( GetEmitter(), _action, _button_key, _info, _inputObj );
		if(DoWeHaveItem(pItem))
			m_pLastMarkedItem = pItem;
		else
			m_pLastMarkedItem = 0;
	}
}


//____ DoWeHaveItem() _________________________________________________________

bool Wdg_ListView::DoWeHaveItem( const WgItem * pItem )
{
	// This is a quick-and-dirty, ListView-specific implmentation that can't even handle
	// recursion. But who cares, both direct callbacks and items are about to get deprecated.
	// Just needed to solve one specific issue as easily as possible...

	WgItem* p = m_items.First();
	while( p )
	{
		if( p == pItem )
			return true;

		if( p->Type() == WgItemRow::GetMyType() )
		{
			WgItem * p2 = ((WgItemRow*)p)->GetFirstItem();
			while( p2 )
			{
				if( p2 == pItem )
					return true;

				p2 = p2->GetNext();
			}

		}

		p = p->GetNext();
	}

	return false;
}



//____ DoMyOwnRender() ________________________________________________________

void Wdg_ListView::DoMyOwnRender( const WgRect& _window, const WgRect& _clip, Uint8 _layer )
{
	WgRect r = _window;

	r.x -= m_viewPixOfsX;

	if( r.w < (Sint32) m_contentWidth )
		r.w = m_contentWidth;

	r.y -= m_viewPixOfsY;
	r.h = m_contentHeight;


	WgItem * p = m_items.First();

	// Skip Rows that are above clipping area.

	while( p && r.y + (Sint32) p->Height() < _clip.y )
	{
		r.y += p->Height() + m_itemSpacing;
		p = p->GetNext();
	}

	// Draw items that are (at least partly) visible.

	while( p && r.y < _clip.y + _clip.h )
	{
		r.h = p->Height();
		if( p->IsSelected() )
		{
			WgRect	u;
			if( u.intersection( r, _clip ) )
			{
				WgGfx::fillRect( u, m_itemMarkColor );
				if(HasLineMarkSource() == true)
					WgGfx::clipBlitRow(u, m_pLineMarkSurf, m_lineMarkSrc, m_lineMarkTileOfs, m_lineMarkTileLen, r.x, r.y, r.w, true);
			}
		}

		if( m_bCellInclusiveItemSpacing )
			r.h += m_itemSpacing;

		p->Render( r, _clip );
		r.y += p->Height() + m_itemSpacing;
		p = p->GetNext();
	}
}


//____ DoMyOwnCloning() _______________________________________________________

void Wdg_ListView::DoMyOwnCloning( WgWidget * _pClone, const WgWidget * _pCloneRoot, const WgWidget * _pBranchRoot )
{
}

//____ DoMyOwnMarkTest() ___________________________________________________

bool Wdg_ListView::DoMyOwnMarkTest( int _x, int _y )
{
	return true;
}

//____ GetPointerStyle() ________________________________________

WgPointerStyle Wdg_ListView::GetPointerStyle() const
{
	WgPointerStyle style = WG_POINTER_DEFAULT;

	if( m_pLastMarkedItem )
		style = m_pLastMarkedItem->GetPointerStyle();

	if( style == WG_POINTER_DEFAULT )
		style = m_pointerStyle;

	return style; 
}


//____ RequestItemGeo() _______________________________________________________

WgRect Wdg_ListView::RequestItemGeo( WgItem * pItem )
{
	WgRect r = ScreenGeometry();

	if( r.w < (Sint32) m_contentWidth )
		r.w = m_contentWidth;

	r.x -= m_viewPixOfsX;
	r.y -= m_viewPixOfsY;
	r.h = m_contentHeight;

	WgItem * p = m_items.First();
	while( p )
	{
		if( p == pItem )
		{
			r.h = p->Height();
			return r;
		}

		r.y += p->Height() + m_itemSpacing;
		p = p->Next();
	}

	return WgRect();
}


