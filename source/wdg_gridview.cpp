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

#include <wdg_gridview.h>
#include <wg_gfx.h>

static const char	Wdg_Type[] = {"TordJ/GridView"};


//____ NewWidgetOfMyType() __________________________________________________________

WgWidget * Wdg_GridView::NewWidgetOfMyType() const
{
	return new Wdg_GridView;
}


//____ ~Wdg_GridView() ________________________________________________________

Wdg_GridView::~Wdg_GridView()
{
}


//____ Init() _________________________________________________________________

void Wdg_GridView::Init()
{
	m_itemOrigo		= WgOrigo::midCenter();
	m_bStretchItem	= true;
	m_bFixedSize	= 0;
	m_bPackCells	= false;

	m_bOpaqueCells	= false;

	m_cellSize		= WgMinMax2D( 4,4, 4096,4096 );
	m_cellPaddingX	= 0;
	m_cellPaddingY	= 0;

	m_biggestCell	= WgSize(0,0);

	m_layout		= Vertical;

	m_pLastMarkedItem = 0;
}



//____ Type() _________________________________________________________________

const char * Wdg_GridView::Type( void ) const
{
	return GetMyType();
}

const char * Wdg_GridView::GetMyType( void )
{
	return Wdg_Type;
}


//____ SetSelectionLasso() ____________________________________________________

void Wdg_GridView::SetSelectionLasso( WgBlockSetPtr pLasso )
{
	m_pLasso = pLasso;
}

//____ SetCellOverlay() _______________________________________________________

void Wdg_GridView::SetCellOverlay( WgBlockSetPtr pOverlay )
{
	if( pOverlay != m_pCellOverlay )
	{
		m_pCellOverlay = pOverlay;
		RequestRender();
	}
}

//____ SetCellUnderlay() ______________________________________________________

void Wdg_GridView::SetCellUnderlay( WgBlockSetPtr pUnderlay )
{
	if( pUnderlay != m_pCellUnderlay )
	{
		m_pCellUnderlay = pUnderlay;
		RequestRender();
	}
}

//____ SetItemOrigo() _________________________________________________________

void Wdg_GridView::SetItemOrigo( const WgOrigo& origo )
{
	if( origo != m_itemOrigo )
	{
		m_itemOrigo = origo;
		RequestRender();
	}
}

//____ SetItemStretch() _______________________________________________________

void Wdg_GridView::SetItemStretch( bool bStretchToCell )
{
	if( bStretchToCell != m_bStretchItem )
	{
		m_bStretchItem = bStretchToCell;
		RequestRender();
	}
}

//____ SetItemLayout() _______________________________________________________

void Wdg_GridView::SetItemLayout(ItemLayout layout)
{
	if( layout != m_layout || IsFixedSize() )
	{
		m_bFixedSize = 0;
		m_layout = layout;
		Refresh();
	}
}

//____ SetFixedSize() _______________________________________________________
bool Wdg_GridView::SetFixedSize( Uint32 size )
{
	if( m_layout == Vertical )
		return SetFixedWidth(size);
	return SetFixedHeight(size);
}

//____ SetFixedWidth() _______________________________________________________
bool Wdg_GridView::SetFixedWidth( Uint32 width )
{
	if( width < m_cellSize.minW )
		return false;
	if( m_bFixedSize != width || m_layout != Vertical )
	{
		m_bFixedSize = width;
		m_layout = Vertical;
		Refresh();
	}
	return true;
}

//____ SetFixedHeight() _______________________________________________________
bool Wdg_GridView::SetFixedHeight( Uint32 height )
{
	if( height < m_cellSize.minH )
		return false;
	if( m_bFixedSize != height || m_layout != Horizontal )
	{
		m_bFixedSize = height;
		m_layout = Horizontal;
		Refresh();
	}
	return true;
}

//____ SetCellPacking() _______________________________________________________

void Wdg_GridView::SetCellPacking( bool bPack )
{
	if( bPack != m_bPackCells )
	{
		m_bPackCells = bPack;
		Refresh();
	}
}

//____ SetPointerOpacity() ____________________________________________________

void Wdg_GridView::SetCellPointerOpacity( bool bOpaque )
{
	m_bOpaqueCells = bOpaque;
}

//____ SetCellSize() __________________________________________________________

bool Wdg_GridView::SetCellSize( WgMinMax2D& minMax )
{
	if(IsFixedSize())
	{
		if(	(m_layout == Vertical && m_bFixedSize < minMax.minW) ||
			(m_layout == Horizontal && m_bFixedSize < minMax.minH) )
			return false;
	}

	if( minMax != m_cellSize )
	{
		m_cellSize = minMax;
		Refresh();
	}

	return true;
}

//____ SetCellPadding() _______________________________________________________

void Wdg_GridView::SetCellPadding( Uint8 x, Uint8 y )
{
	if( x != m_cellPaddingX || y != m_cellPaddingY )
	{
		m_cellPaddingX = x;
		m_cellPaddingY = y;
		Refresh();
	}
}

//____ ScrollIntoView() _________________________________________________________
void Wdg_GridView::ScrollIntoView(WgItem* pItem )
{
	if( !pItem || pItem->IsHidden() )
		return;

	ItemArray itemArray;
	WgSize content(m_contentWidth, m_contentHeight);
	bool bSuccess = GetFirstRowOrCol(&itemArray, content);
	bool bFound = false;
	WgRect itemRect;

	while(!bFound && bSuccess)
	{
		itemRect = WgRect(itemArray.rect.x, itemArray.rect.y, itemArray.rect.w, itemArray.rect.h);
		for(WgItem* p = itemArray.pFirstItem; p != itemArray.pNextFirst; p = p->Next())
		{
			WgSize itemSize = CalcCellSize(p);
			itemRect.w = itemSize.w;
			itemRect.h = itemSize.h;

			if(p == pItem)
			{
				bFound = true;
				break;
			}

			if(m_layout == Vertical)
				itemRect.x += itemRect.w;
			else
				itemRect.y += itemRect.h;
		}
		bSuccess = GetNextRowOrCol(&itemArray, content);
	}

	if(!bFound)
		return;

	int viewLenX = ViewPixelLenX();
	int viewLenY = ViewPixelLenY();

	if( itemRect.x < (int)ViewPixelOfsX() )
		SetViewPixelOfsX( itemRect.x );
	else if( itemRect.x + itemRect.w > (int)ViewPixelOfsX() + viewLenX )
		SetViewPixelOfsX( itemRect.x + itemRect.w - viewLenX );

	if( itemRect.y < (int)ViewPixelOfsY() )
		SetViewPixelOfsY( itemRect.y );
	else if( itemRect.y + itemRect.h > (int)ViewPixelOfsY() + viewLenY )
		SetViewPixelOfsY( itemRect.y + itemRect.h - viewLenY );
}


//____ refreshItems() _________________________________________________________

void Wdg_GridView::refreshItems()
{
	CalcBiggestCell();
	UpdateContentSize();

	// Check so we still have our m_pLastMarkedItem...

	WgItem * p = m_items.First();
	while( p )
	{
		if( p == m_pLastMarkedItem )
			break;
		p = p->Next();
	}
	if( p == 0 )
		m_pLastMarkedItem = 0;

	RequestRender();
}

//____ ItemSizeModified() _________________________________________________________

void Wdg_GridView::ItemSizeModified( WgItem * pItem, Sint32 widthDiff , Sint32 heightDiff )
{
	if( widthDiff || heightDiff )
	{
		if( m_bPackCells || UpdateMaxCellSize(pItem) )
			UpdateContentSize();
	}

	//TODO: Optimize. Only needs to render if item is visible or above and changed size (thus moving visible items).

	RequestRender();
}

//____ ItemVisibilityModified() _______________________________________________

void Wdg_GridView::ItemVisibilityModified( WgItem * pItem, bool bVisible )
{
	//TODO: More specific and optimized handling.
	
	refreshItems();
}


//____ ItemAdded() ____________________________________________________________

void Wdg_GridView::ItemAdded( WgItem * pItem )
{
	UpdateMaxCellSize( pItem );
	UpdateContentSize();

	//TODO: Optimize. Only needs to render if item is visible or above (thus moving visible items).

	RequestRender();
}

//____ ItemMarkChanged() ______________________________________________________

void Wdg_GridView::ItemMarkChanged( WgItem * pItem, bool bMarked )
{
	//TODO: Optimize. Only needs to render if item is visible.

	RequestRender();
}

//____ GetMarkedItem() ________________________________________________________

WgItem* Wdg_GridView::GetMarkedItem( Uint32 x, Uint32 y )
{
	x += m_viewPixOfsX;
	y += m_viewPixOfsY;

	ItemArray itemArray;
	WgSize content(m_contentWidth, m_contentHeight);
	bool bSuccess = GetFirstRowOrCol(&itemArray, content);

	while(bSuccess)
	{
		if(itemArray.rect.Contains(x, y))
		{
			WgRect itemRect(itemArray.rect.x, itemArray.rect.y, 0, 0);
			for(WgItem* p = itemArray.pFirstItem; p != itemArray.pNextFirst; p = p->Next())
			{
				WgSize itemSize = CalcCellSize(p);
				itemRect.w = itemSize.w;
				itemRect.h = itemSize.h;

				if(itemRect.Contains(x, y))
					return p->GetMarkedItem(x-itemRect.x, y-itemRect.y);

				if(m_layout == Vertical)
					itemRect.x += itemRect.w;
				else
					itemRect.y += itemRect.h;
			}
			return 0;
		}
		else
		{
			bSuccess = GetNextRowOrCol(&itemArray, content);
		}
	}
	return 0;
}

//____ DoMyOwnGeometryChangeSubclass() ________________________________________

void Wdg_GridView::DoMyOwnGeometryChangeSubclass( WgRect& oldGeo, WgRect& newGeo )
{
	if(IsFixedSize() == false)
	{
		if( (m_layout == Vertical && newGeo.w != oldGeo.w) ||
			(m_layout == Horizontal && newGeo.h != oldGeo.h) )
			UpdateContentSize();
	}
}

//____ DoMyOwnRefresh() _______________________________________________________

void Wdg_GridView::DoMyOwnRefresh( void )
{
	refreshItems();
}

//____ DoMyOwnUpdate() ________________________________________________________

void Wdg_GridView::DoMyOwnUpdate( const WgUpdateInfo& _updateInfo )
{
}

//____ DoMyOwnActionRespond() _________________________________________________

void Wdg_GridView::DoMyOwnActionRespond( WgInput::UserAction _action, int _button_key, const WgActionDetails& _info, const WgInput& _inputObj )
{
	int x = _info.x;
	int y = _info.y;

	WgItem* pCursorItem;
	if(_action == WgInput::POINTER_OVER )
	{
		Abs2local( &x, &y );
		pCursorItem = GetMarkedItem(x, y);
	}
	else
		pCursorItem = m_pLastMarkedItem;

	if( pCursorItem != m_pLastMarkedItem )
	{
		RequestRender();

		if(m_pLastMarkedItem)
			m_pLastMarkedItem->ActionRespond( GetEmitter(), WgInput::POINTER_EXIT, _button_key, _info, _inputObj );

		if( pCursorItem )
			pCursorItem->ActionRespond( GetEmitter(), WgInput::POINTER_ENTER, _button_key, _info, _inputObj );
	}
	else if( pCursorItem )
		pCursorItem->ActionRespond( GetEmitter(), _action, _button_key, _info, _inputObj );

	m_pLastMarkedItem = pCursorItem;
}

//____ DoMyOwnRender() ________________________________________________________

void Wdg_GridView::DoMyOwnRender( const WgRect& _window, const WgRect& _clip, Uint8 _layer )
{
	WgRect r = _window;

	r.x -= m_viewPixOfsX;
	r.y -= m_viewPixOfsY;

	ItemArray itemArray;
	WgSize content(m_contentWidth, m_contentHeight);
	bool bSuccess = GetFirstRowOrCol(&itemArray, content);

	while(bSuccess)
	{
		WgRect itemRect(itemArray.rect.x + r.x, itemArray.rect.y + r.y, itemArray.rect.w, itemArray.rect.h);
		if(itemRect.IntersectsWith(_clip))
		{
			for(WgItem* p = itemArray.pFirstItem; p != itemArray.pNextFirst; p = p->Next())
			{
				WgSize itemSize = CalcCellSize(p);
				itemRect.w = itemSize.w;
				itemRect.h = itemSize.h;

				if(itemRect.IntersectsWith(_clip))
					RenderItem(p, _clip, itemRect);

				if(m_layout == Vertical)
					itemRect.x += itemRect.w;
				else
					itemRect.y += itemRect.h;
			}
		}
		bSuccess = GetNextRowOrCol(&itemArray, content);
	}
}

void Wdg_GridView::RenderItem(WgItem*p, const WgRect& _clip, const WgRect& cellRect)
{
	// Render underlay (if any)

	if( m_pCellUnderlay )
		WgGfx::clipBlitBlock( _clip, m_pCellUnderlay->GetBlock(p->GetMode(), cellRect ), cellRect );
	// Render item

	p->Render( CalcItemRect( p, cellRect ) , _clip );

	// Render overlay (if any)

	if( m_pCellOverlay )
		WgGfx::clipBlitBlock( _clip, m_pCellOverlay->GetBlock(p->GetMode(), cellRect ), cellRect );

	//
}

//____ DoMyOwnCloning() _______________________________________________________

void Wdg_GridView::DoMyOwnCloning( WgWidget * _pClone, const WgWidget * _pCloneRoot,
								   const WgWidget * _pBranchRoot )
{

}

//____ DoMyOwnMarkTest() ___________________________________________________

bool Wdg_GridView::DoMyOwnMarkTest( int _x, int _y )
{
	return true;
}


//____ UpdateContentSize() ____________________________________________________

void Wdg_GridView::UpdateContentSize()
{
	WgSize contentSize(0, 0);

	if(m_layout == Vertical)
	{
		contentSize.w = IsFixedSize() ? m_bFixedSize : 0;
		contentSize.w = WgMax(WgMax(contentSize.w, m_biggestCell.w), m_geo.w);

		ItemArray itemArray;
		bool bSuccess = GetFirstRowOrCol(&itemArray, contentSize);
		while(bSuccess)
		{
			contentSize.h += itemArray.rect.h + m_itemSpacing;
			bSuccess = GetNextRowOrCol(&itemArray, contentSize);
		}

		if( contentSize.h != 0 )
			contentSize.h -= m_itemSpacing;
	}
	else
	{
		contentSize.h = IsFixedSize() ? m_bFixedSize : 0;
		contentSize.h = WgMax(WgMax(contentSize.h, m_biggestCell.h), m_geo.h);

		ItemArray itemArray;
		bool bSuccess = GetFirstRowOrCol(&itemArray, contentSize);
		while(bSuccess)
		{
			contentSize.w += itemArray.rect.w + m_itemSpacing;
			bSuccess = GetNextRowOrCol(&itemArray, contentSize);
		}

		if( contentSize.w != 0 )
			contentSize.w -= m_itemSpacing;
	}

	SetContentSize( contentSize.w, contentSize.h );
}

//____ CalcBiggestCell() ______________________________________________________

void Wdg_GridView::CalcBiggestCell()
{
	WgSize z(0,0);

	WgItem * p = m_items.First();
	while( p )
	{
		Sint32 w = p->Width() + m_cellPaddingX*2;
		Sint32 h = p->Height() + m_cellPaddingY*2;

		if( w > z.w )
			z.w = w;

		if( h > z.h )
			z.h = h;

		p = p->GetNext();
	}

	z.ConstrainTo( m_cellSize );
	m_biggestCell = z;
}

//____ CalcPackedCellSize() _________________________________________________________

WgSize Wdg_GridView::CalcPackedCellSize( const WgItem * pItem )
{
	WgSize z;

	z.w = pItem->Width() + m_cellPaddingX*2;
	z.h = pItem->Height() + m_cellPaddingY*2;

	z.ConstrainTo( m_cellSize );

	return z;
}

//____ CalcCellSize() _________________________________________________________

WgSize Wdg_GridView::CalcCellSize( const WgItem * pItem )
{
	if(m_bPackCells)
		return CalcPackedCellSize(pItem);

	return m_biggestCell;
}

//____ UpdateMaxCellSize() _________________________________________________________

bool Wdg_GridView::UpdateMaxCellSize(const WgItem * pItem)
{
	bool bUpdateNeeded = false;

	WgSize z = CalcPackedCellSize(pItem);

	if( z.w > m_biggestCell.w )
	{
		m_biggestCell.w = z.w;
		bUpdateNeeded = true;
	}
	if( z.h > m_biggestCell.h )
	{
		m_biggestCell.h = z.h;
		bUpdateNeeded = true;
	}

	return bUpdateNeeded;
}

//____ CalcItemRect() _________________________________________________________

WgRect Wdg_GridView::CalcItemRect( const WgItem * pItem, const WgRect& cellRect )
{
	WgRect itemRect( cellRect.x + m_cellPaddingX, cellRect.y + m_cellPaddingY,
					 cellRect.w - m_cellPaddingX*2, cellRect.h - m_cellPaddingY*2 );

	if( !m_bStretchItem )
	{
		if( itemRect.w > (Sint32)pItem->Width() )
		{
			itemRect.x += m_itemOrigo.calcOfsX( itemRect.w, pItem->Width() );
			itemRect.w = pItem->Width();
		}

		if( itemRect.h > (Sint32)pItem->Height() )
		{
			itemRect.y += m_itemOrigo.calcOfsY( itemRect.h, pItem->Height() );
			itemRect.h = pItem->Height();
		}
	}

	return itemRect;
}


bool Wdg_GridView::GetFirstRowOrCol( ItemArray* pArray, const WgSize &contentSize )
{
	pArray->pFirstItem = 0;
	pArray->pNextFirst = m_items.First();
	pArray->rect.x = 0;
	pArray->rect.y = 0;
	pArray->rect.w = 0;
	pArray->rect.h = 0;
	return GetNextRowOrCol(pArray, contentSize);
}

bool Wdg_GridView::GetNextRowOrCol( ItemArray* pArray, const WgSize &contentSize )
{
	if(pArray->pNextFirst == 0)
		return false;

	if(pArray->pFirstItem)
	{
		if(m_layout == Vertical)
		{
			pArray->rect.x = 0;
			pArray->rect.y += pArray->rect.h + m_itemSpacing;
		}
		else
		{
			pArray->rect.x += pArray->rect.w + m_itemSpacing;
			pArray->rect.y = 0;
		}
	}

	pArray->pFirstItem = pArray->pNextFirst;
	pArray->nItems = 1;

	WgItem *pItem = pArray->pFirstItem;

	if( m_bPackCells )
	{
		pArray->rect.w = pItem->Width() + m_cellPaddingX*2;
		pArray->rect.h = pItem->Height() + m_cellPaddingY*2;
	}
	else
	{
		pArray->rect.w = m_biggestCell.w;
		pArray->rect.h = m_biggestCell.h;
	}

	pItem = pItem->GetNext();

	if(m_layout == Vertical)
	{
		if( m_bPackCells )
		{
			while( pItem )
			{
				int w = pItem->Width() + m_cellPaddingX*2 + m_itemSpacing;
				if( pArray->rect.w + w > contentSize.w )
					break;

				pArray->rect.w += w;

				int h = pItem->Height() + m_cellPaddingY*2;
				if( h > (Sint32)pArray->rect.h )
					pArray->rect.h = h;

				pArray->nItems++;
				pItem = pItem->GetNext();
			}
		}
		else
		{
			while( pItem && pArray->rect.w + m_biggestCell.w < contentSize.w )
			{
				pArray->rect.w += m_biggestCell.w;
				pArray->nItems++;
				pItem = pItem->GetNext();
			}
		}
	}
	else
	{
		if( m_bPackCells )
		{
			while( pItem )
			{
				int h = pItem->Height() + m_cellPaddingY*2 + m_itemSpacing;
				if( pArray->rect.h + h > contentSize.h )
					break;

				pArray->rect.h += h;

				int w = pItem->Width() + m_cellPaddingX*2;
				if( w > (Sint32)pArray->rect.w )
					pArray->rect.w = w;

				pArray->nItems++;
				pItem = pItem->GetNext();
			}
		}
		else
		{
			while( pItem && pArray->rect.h + m_biggestCell.h < contentSize.h )
			{
				pArray->rect.h += m_biggestCell.h;
				pArray->nItems++;
				pItem = pItem->GetNext();
			}
		}
	}

	pArray->pNextFirst = pItem;

	return true;
}

//____ GetTooltipString() _____________________________________________________

WgString Wdg_GridView::GetTooltipString() const
{
	WgString tooltip;
	if( m_pLastMarkedItem != 0 )
		tooltip = m_pLastMarkedItem->GetTooltipString();

	return tooltip;
}

//____ GetTooltipItem() _______________________________________________________

WgItem* Wdg_GridView::GetTooltipItem() const
{
	if( m_pLastMarkedItem != 0 )
		return m_pLastMarkedItem;

	return 0;
}


//____ RequestItemGeo() _______________________________________________________

WgRect Wdg_GridView::RequestItemGeo( WgItem * pItem )
{
	WgRect r = ScreenGeometry();

	r.x -= m_viewPixOfsX;
	r.y -= m_viewPixOfsY;

	ItemArray itemArray;
	WgSize content(m_contentWidth, m_contentHeight);
	bool bSuccess = GetFirstRowOrCol(&itemArray, content);

	while(bSuccess)
	{
		WgRect itemRect(itemArray.rect.x + r.x, itemArray.rect.y + r.y, itemArray.rect.w, itemArray.rect.h);

		for(WgItem* p = itemArray.pFirstItem; p != itemArray.pNextFirst; p = p->Next())
		{
			WgSize itemSize = CalcCellSize(p);
			itemRect.w = itemSize.w;
			itemRect.h = itemSize.h;

			if(p == pItem)
				return itemRect;

			if(m_layout == Vertical)
				itemRect.x += itemRect.w;
			else
				itemRect.y += itemRect.h;
		}

		bSuccess = GetNextRowOrCol(&itemArray, content);
	}

	return WgRect();
}


