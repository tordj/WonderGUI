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

#ifndef WG_INTERFACE_ITEMHOLDER_DOT_H
#define	WG_INTERFACE_ITEMHOLDER_DOT_H

#include <wg_item.h>
#include <wg_geo.h>
#include <wg_color.h>

class WgEmitter;

class Wg_Interface_ItemHolder
{
	friend class WgItem;

public:
	Wg_Interface_ItemHolder();
	virtual ~Wg_Interface_ItemHolder();

	void	AddItem( WgItem * pItem );
	void	InsertItem( WgItem * pItem, Uint32 pos );
	void	InsertItemSorted( WgItem * pItem );

	bool	HasItem( WgItem * pItem ) const;

	Sint32	GetItemPos( WgItem * pItem );
	WgItem*	GetItem( Uint32 pos );
	const WgItem*	GetItem( Uint32 pos ) const;
	WgItem *FindItem( Uint32 id );
	WgItem* GetFirstSelectedItem();

	WgItem*	RemoveItem( Uint32 pos );
	bool	RemoveItem( WgItem * pItem );
	void	RemoveAllItems();

	bool	DeleteItem( Uint32 pos );
	bool	DeleteItem( WgItem * pItem );
	void	DeleteAllItems();

	void	SortItems();
	void	SetSortOrder( bool bAscend );

	void	SetItemMarkColor( const WgColor& color );
	WgColor	GetItemMarkColor() const { return m_itemMarkColor; }

	inline WgItem*	GetFirstItem()	{ return m_items.First();};
	inline WgItem*	GetLastItem()	{ return m_items.Last();};

	void	SetItemSpacing( Uint32 spacing );
	inline Uint32 ItemSpacing()		{ return m_itemSpacing; };
	inline Uint32 NbItems()			{ return m_nItems; };

	virtual Sint32 CompareItems(WgItem * pItem1, WgItem * pItem2 );		// Used by sortItems() and InsertItemSorted().

	void	SetSortFunction( Sint32(*fp)(WgItem *,WgItem *) );


protected:
	virtual void refreshItems() = 0;
	virtual void ItemSizeModified( WgItem * pItem, Sint32 widthDiff , Sint32 heightDiff ) = 0;
	virtual void ItemVisibilityModified( WgItem * pItem, bool bVisible ) = 0;
	virtual void ItemAdded( WgItem * pItem ) { refreshItems();	}; // Slow fallback method for making sure everyting is up-to-date...
	virtual void ItemMarkChanged( WgItem * pItem, bool bMarked ) {};
	virtual WgItem* GetMarkedItem( Uint32 x, Uint32 y ) = 0;

	virtual bool	GrabInputFocus() = 0;
	virtual bool	RemoveInputFocus() = 0;

	bool SetFocusedItem( WgItem * pItem );
	bool UnsetFocusedItem( WgItem * pItem );

	void InsertItemSortedBetween( WgItem * pFirst, WgItem * pLast, WgItem * pItem, Uint32 nItems );

	void AdaptItemsToWidth( Uint32 w );
	void AdaptItemsToHeight( Uint32 h );

	void DoMyOwnCloning( Wg_Interface_ItemHolder * pFrom );

//	virtual WgSize	RequestItemSize( WgItem * pItem ) = 0;


//	virtual void ItemAdded( WgItem * pItem ) = 0;
//	virtual void ItemRemoved( WgItem * pItem ) = 0;

	WgChain<WgItem>	m_items;
	Uint32			m_itemSpacing;
	Uint32			m_nItems;
	WgColor			m_itemMarkColor;
	bool			m_bSortAscend;

	WgItem *		m_pFocusedItem;

	Sint32(*m_pSortFunction)(WgItem *,WgItem *);
};



#endif //WG_INTERFACE_ITEMHOLDER_DOT_H
