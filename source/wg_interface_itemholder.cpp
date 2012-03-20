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


#include <wg_interface_itemholder.h>

#include <assert.h>

//____ Constructor() __________________________________________________________

Wg_Interface_ItemHolder::Wg_Interface_ItemHolder()
{
	m_itemSpacing	= 0;
	m_bCellInclusiveItemSpacing = false;
	m_nItems		= 0;
	m_itemMarkColor	= WgColor(0,0,0);
	m_bSortAscend	= true;
	m_bSelectable	= true;
	m_pSortFunction = 0;
	m_pFocusedItem	= 0;
}

Wg_Interface_ItemHolder::~Wg_Interface_ItemHolder()
{
	for(WgItem * p = m_items.First(); p; p = p->Next())
		p->m_pMyHolder = 0;	// so the item doesn't remove itself in the destructor
}

//____ SetSelectable() ________________________________________________________

void Wg_Interface_ItemHolder::SetSelectable( bool bSelectable )
{
	m_bSelectable = bSelectable;
}

//____ SetSortFunction() ______________________________________________________

void Wg_Interface_ItemHolder::SetSortFunction(Sint32 (*fp)(WgItem *, WgItem *))
{
	m_pSortFunction = fp;
}

//____ AddItem() ______________________________________________________________

void Wg_Interface_ItemHolder::AddItem( WgItem * pItem )
{
	pItem->m_pMyHolder = this;
	m_items.PushBack( pItem );
	m_nItems++;
	ItemAdded(pItem);
}

//____ InsertItem() ___________________________________________________________

void Wg_Interface_ItemHolder::InsertItem( WgItem * pItem, Uint32 pos )
{
	WgItem * p = m_items.Get(pos);

	if( p )
		pItem->MoveBefore( p );
	else
		m_items.PushBack(pItem);

	pItem->m_pMyHolder = this;
	m_nItems++;
	ItemAdded(pItem);
}

//____ HasItem() ___________________________________________________________

bool Wg_Interface_ItemHolder::HasItem( WgItem * pItem ) const
{
	return m_items.IsMemberOf( pItem );
}

//____ GetItemPos() ___________________________________________________________

Sint32 Wg_Interface_ItemHolder::GetItemPos( WgItem * pItem )
{
	if( m_items.IsMemberOf( pItem ) )
		return pItem->Index();
	else
		return -1;
}

//____ FindItem() _____________________________________________________________

WgItem * Wg_Interface_ItemHolder::FindItem( Uint32 id )
{
	WgItem * pItem = m_items.First();

	while( pItem )
	{
		if( pItem->Id() == id )
			return pItem;

		pItem = pItem->GetNext();
	}
	return 0;
}

//____ GetFirstMarkedItem() ___________________________________________________

WgItem * Wg_Interface_ItemHolder::GetFirstSelectedItem()
{
	WgItem * pItem = m_items.First();

	while( pItem )
	{
		if( pItem->IsSelected() )
			return pItem;

		pItem = pItem->GetNext();
	}
	return 0;
}

//____ GetItem() ______________________________________________________________

WgItem* Wg_Interface_ItemHolder::GetItem( Uint32 pos )
{
	return m_items.Get(pos);
}

const WgItem* Wg_Interface_ItemHolder::GetItem( Uint32 pos ) const
{
	return m_items.Get(pos);
}


//____ RemoveItem() ___________________________________________________________

WgItem* Wg_Interface_ItemHolder::RemoveItem( Uint32 pos )
{
	WgItem * p = m_items.Get(pos);
	if( p )
	{
		if( p == m_pFocusedItem )
			m_pFocusedItem = 0;

		p->Disconnect();
		p->m_pMyHolder = 0;
		m_nItems--;
		refreshItems();
	}
	return p;
}

//____ RemoveItem() ___________________________________________________________

bool Wg_Interface_ItemHolder::RemoveItem( WgItem * pItem )
{
	if( !m_items.IsMemberOf(pItem) )
		return false;

	if( pItem == m_pFocusedItem )
		m_pFocusedItem = 0;

	pItem->Disconnect();
	pItem->m_pMyHolder = 0;
	m_nItems--;
	refreshItems();
	return true;
}

//____ RemoveAllItems() _______________________________________________________

void Wg_Interface_ItemHolder::RemoveAllItems()
{
	WgItem * pItem = m_items.First();
	while( pItem )
	{
		pItem->Disconnect();
		pItem->m_pMyHolder = 0;
		pItem = m_items.First();
	}
	m_nItems = 0;
	m_pFocusedItem = 0;
	refreshItems();
}

//____ DeleteItem() ___________________________________________________________

bool Wg_Interface_ItemHolder::DeleteItem( Uint32 pos )
{
	return DeleteItem(m_items.Get(pos));
}

//____ DeleteItem() ___________________________________________________________

bool Wg_Interface_ItemHolder::DeleteItem( WgItem * pItem )
{
	if(pItem && pItem->m_pMyHolder == this)
	{
		if( pItem == m_pFocusedItem )
			m_pFocusedItem = 0;

		pItem->Disconnect();
		pItem->m_pMyHolder = 0;	// so the item doesn't remove itself in the destructor
		delete pItem;
		m_nItems--;
		refreshItems();
		return true;
	}
	return false;
}

//____ DeleteAllItems() _______________________________________________________

void Wg_Interface_ItemHolder::DeleteAllItems()
{
	for(WgItem * p = m_items.First(); p; p = p->Next())
		p->m_pMyHolder = 0;	// so the item doesn't remove itself in the destructor
	m_items.Clear();
	m_nItems = 0;
	m_pFocusedItem = 0;
	refreshItems();
}


//____ SortItems() ____________________________________________________________

void Wg_Interface_ItemHolder::SortItems()
{

	WgItem * pItem = m_items.First();
	int nItems = 1;

	while( pItem )
	{
		WgItem * pNext = pItem->GetNext();

		if( nItems >= 2 )
		{
			WgItem * pFirst = m_items.First();
			WgItem * pLast = pItem;

			bool bFirst = false;
			if( CompareItems( pItem, pFirst ) < 0 )
				bFirst = true;

			if( !m_bSortAscend )
				bFirst = !bFirst;

			if( bFirst )
				m_items.PushFront( pItem );
			else
				InsertItemSortedBetween(pFirst, pLast, pItem, nItems );
		}

		nItems++;
		pItem = pNext;
	}

	refreshItems();
}

//____ InsertItemSorted() _____________________________________________________

void Wg_Interface_ItemHolder::InsertItemSorted( WgItem * pItem )
{
	if( m_nItems == 0 )
	{
		m_items.PushBack(pItem);
	}
	else
	{

		WgItem * pFirst = m_items.First();
		WgItem * pLast = m_items.Last();

		// First check if we should be put first or last in list. Equal to last should put us last.

		bool bInserted = false;
		if( m_bSortAscend )
		{
			if( CompareItems( pItem, pFirst ) < 0 )
			{
				pItem->MoveBefore( pFirst );
				bInserted = true;
			}
			else if( CompareItems( pItem, pLast ) >= 0 )
			{
				pItem->MoveAfter( pLast );
				bInserted = true;
			}
		}
		else
		{
			if( CompareItems( pItem, pFirst ) > 0 )
			{
				pItem->MoveBefore( pFirst );
				bInserted = true;
			}
			else if( CompareItems( pItem, pLast ) <= 0 )
			{
				pItem->MoveAfter( pLast );
				bInserted = true;
			}
		}

		// If it wasn't inserted first or last it needs to be inserted between pFirst and pLast

		if( !bInserted )
			InsertItemSortedBetween(pFirst, pLast, pItem, m_nItems );
	}

	//

	pItem->m_pMyHolder = this;
	m_nItems++;
	ItemAdded(pItem);
/*

	WgItem * pPlace = m_items.First();

	if( m_bSortAscend )
	{
		while( pPlace )
		{
			if( CompareItems( pItem, pPlace ) < 0 )
			{
				pItem->moveBefore( pPlace );
				break;
			}
			pPlace = pPlace->GetNext();
		}
	}
	else
	{
		while( pPlace )
		{
			if( CompareItems( pItem, pPlace ) > 0 )
			{
				pItem->moveBefore( pPlace );
				break;
			}
			pPlace = pPlace->GetNext();
		}
	}

	if( !pPlace )
		m_items.push_back(pItem);

	pItem->m_pMyHolder = this;
	m_nItems++;
	ItemAdded(pItem);
*/
}




//____ InsertItemSortedBetween() _____________________________________________________

void Wg_Interface_ItemHolder::InsertItemSortedBetween( WgItem * pFirst, WgItem * pLast, WgItem * pItem, Uint32 nItems )
{
	assert( nItems >= 2 && pFirst != pLast );

	if( nItems <= 2 )				// Should never be less than two but we handle it as a failsafe
									// in case we screw up some calling code in the future...
	{								// Only two items means that we have found our position for insertion.
		pItem->MoveAfter( pFirst );
		return;
	}
	else
	{
		// Step to middle item

		WgItem * pMiddle = pFirst;
		int steps = nItems/2;
		for( int i = 0 ; i < steps ; i++ )
			pMiddle = pMiddle->GetNext();

		// Compare against middle item

		bool bUp = false;
		if( CompareItems( pItem, pMiddle ) < 0 )
			bUp = true;

		if( !m_bSortAscend )
			bUp = !bUp;

		// subdivide and recurse

		if ( bUp )
			InsertItemSortedBetween( pFirst, pMiddle, pItem, steps+1);
		else
			InsertItemSortedBetween( pMiddle, pLast, pItem, nItems-steps);
	}

}




//____ CompareItems() _________________________________________________________

Sint32 Wg_Interface_ItemHolder::CompareItems( WgItem * pItem1, WgItem * pItem2 )
{
	if( m_pSortFunction != 0 )
		return m_pSortFunction(pItem1, pItem2);

	return 0;
}

//____ SetSortOrder() _________________________________________________________

void Wg_Interface_ItemHolder::SetSortOrder( bool bAscend )
{
	m_bSortAscend = bAscend;
}



//____ SetItemMarkColor() _____________________________________________________

void Wg_Interface_ItemHolder::SetItemMarkColor( const WgColor& color )
{
	if( color.argb != m_itemMarkColor.argb )
	{
		m_itemMarkColor = color;
		refreshItems();
	}
}


//____ SetItemSpacing() _______________________________________________________

void Wg_Interface_ItemHolder::SetItemSpacing( Uint32 spacing, bool bCellInclusive )
{
	if( spacing != m_itemSpacing || bCellInclusive != m_bCellInclusiveItemSpacing )
	{
		m_itemSpacing = spacing;
		m_bCellInclusiveItemSpacing = bCellInclusive;
		refreshItems();
	}
}

//____ AdaptItemsToWidth() ____________________________________________________

void Wg_Interface_ItemHolder::AdaptItemsToWidth( Uint32 w )
{
	WgItem * p = m_items.First();

	while( p )
	{
		p->AdaptToWidth( w );
		p = p->GetNext();
	}
}

//____ AdaptItemsToHeight() ___________________________________________________

void Wg_Interface_ItemHolder::AdaptItemsToHeight( Uint32 h )
{
	WgItem * p = m_items.First();

	while( p )
	{
		p->AdaptToHeight( h );
		p = p->GetNext();
	}
}

//____ DoMyOwnCloning() _______________________________________________________
void Wg_Interface_ItemHolder::DoMyOwnCloning( Wg_Interface_ItemHolder * pFrom )
{
	m_itemSpacing = pFrom->m_itemSpacing;
	m_itemMarkColor	= pFrom->m_itemMarkColor;
	m_bSortAscend = pFrom->m_bSortAscend;
}

//____ SetFocusedItem() _______________________________________________________

bool Wg_Interface_ItemHolder::SetFocusedItem( WgItem * pItem )
{
	if( m_pFocusedItem && m_pFocusedItem != pItem )
		m_pFocusedItem->_onLostInputFocus();

	m_pFocusedItem = pItem;

	if( pItem )
		pItem->_onGotInputFocus();

	return GrabInputFocus();
}

bool Wg_Interface_ItemHolder::UnsetFocusedItem( WgItem * pItem )
{
	if( pItem && pItem == m_pFocusedItem )
		m_pFocusedItem->_onLostInputFocus();

	m_pFocusedItem = 0;

	return RemoveInputFocus();
}

