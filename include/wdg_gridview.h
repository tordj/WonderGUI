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

#ifndef WDG_GRIDVIEW_DOT_H
#define WDG_GRIDVIEW_DOT_H

#include <wdg_baseclass_view.h>
#include <wg_interface_itemholder.h>

class Wdg_GridView : public Wdg_Baseclass_View, public Wg_Interface_ItemHolder
{
public:
	WIDGET_CONSTRUCTORS(Wdg_GridView,Wdg_Baseclass_View)
	virtual ~Wdg_GridView();
	virtual const char * Type() const;
	static const char * GetMyType();

	enum ItemLayout
	{
		Vertical,
		Horizontal,
	};

	void	SetSelectionLasso( WgBlockSetPtr pLasso );
	void	SetCellOverlay( WgBlockSetPtr pOverlay );
	void	SetCellUnderlay( WgBlockSetPtr pUnderlay );
	void	SetItemOrigo( WgOrigo& origo );
	void	SetItemStretch( bool bStretchToCell );
	void	SetItemLayout( ItemLayout layout );	// implies non-fixed size
	bool	SetFixedSize( Uint32 size );		// set fixed size according to current layout (either fixed width or fixed height)
	bool	SetFixedWidth( Uint32 width );		// implies Vertical layout
	bool	SetFixedHeight( Uint32 height );	// implies Horizontal layout
	void	SetCellPacking( bool bPack );
	void	SetCellPointerOpacity( bool bOpaque );
	bool	SetCellSize( WgMinMax2D& minMax );
	void	SetCellPadding( Uint8 x, Uint8 y );
	void	ScrollIntoView( WgItem* pItem );

	WgBlockSetPtr 	GetSelectionLasso( ) const { return m_pLasso; }
	WgBlockSetPtr 	GetCellOverlay( ) const { return m_pCellOverlay; }
	WgBlockSetPtr 	GetCellUnderlay( ) const { return m_pCellUnderlay; }
	WgOrigo			GetItemOrigo( ) const { return m_itemOrigo; }
	bool			GetItemStretch( ) const { return m_bStretchItem; }
	ItemLayout		GetItemLayout( ) const { return m_layout; }
	Uint32			GetFixedSize( ) const { return m_bFixedSize; }
	bool			GetCellPacking( ) const { return m_bPackCells; }
	bool			GetCellPointerOpacity( ) const { return m_bOpaqueCells; }
	WgMinMax2D		GetCellSize( ) const { return m_cellSize; }
	Uint8			GetCellPaddingX( ) const { return m_cellPaddingX; }
	Uint8			GetCellPaddingY( ) const { return m_cellPaddingY; }

	inline bool	GrabInputFocus() { return WgWidget::GrabInputFocus(); }
	inline bool	RemoveInputFocus() { return WgWidget::GrabInputFocus(); }


protected:
	WgWidget * NewOfMyType() const;

	void refreshItems();
	void ItemModified( WgItem * pItem, Sint32 widthDiff , Sint32 heightDiff );
	void ItemAdded( WgItem * pItem );
	void ItemMarkChanged( WgItem * pItem, bool bMarked );
	WgItem* GetMarkedItem( Uint32 x, Uint32 y );


	void DoMyOwnGeometryChangeSubclass( WgRect& oldGeo, WgRect& newGeo );
	void DoMyOwnRefresh( void );
	void DoMyOwnUpdate( const WgUpdateInfo& _updateInfo );
	void DoMyOwnActionRespond( WgInput::UserAction _action, int _button_key, const WgActionDetails& _info, const WgInput& _inputObj );
	void DoMyOwnRender( const WgRect& _window, const WgRect& _clip, Uint8 _layer );
	void DoMyOwnCloning( WgWidget * _pClone, const WgWidget * _pCloneRoot,
								 const WgWidget * _pBranchRoot );
	bool DoMyOwnMarkTest( int _x, int _y );
//	void DoMyOwnDisOrEnable( void ); 


	struct GridRow
	{
		Uint32		height;
		Uint32		width;
		WgItem *	pFirstItem;
		Uint32		nItems;
	};

	struct ItemArray
	{
		WgRect	rect;
		WgItem *pFirstItem;
		WgItem *pNextFirst;
		Uint32	nItems;
	};



private:
	void		Init();

	void		CalcBiggestCell();
	bool		GetFirstRowOrCol( ItemArray* pArray, const WgSize &contentSize );
	bool		GetNextRowOrCol( ItemArray* pArray, const WgSize &contentSize );
	WgSize		CalcPackedCellSize( const WgItem * pItem );
	WgSize		CalcCellSize( const WgItem * pItem );
	bool		UpdateMaxCellSize(const WgItem * pItem);
	WgRect		CalcItemRect( const WgItem * pItem, const WgRect& cellRect );
	void		UpdateContentSize();
	void		RenderItem(WgItem*p, const WgRect& _clip, const WgRect& cellRect);
	bool		IsFixedSize()		{ return m_bFixedSize != 0; }

	WgBlockSetPtr	m_pCellUnderlay;// Block to be rendered under the item for each cell. Use BlockSets WG_SKIP-flags wisely.
	WgBlockSetPtr	m_pCellOverlay;	// Block to be rendered over the item for each cell. Use BlockSets WG_SKIP-flags wisely.
	WgBlockSetPtr	m_pLasso;		// Block used for showing selection-rectangle when selecting multiple items. Only NORMAL is used.


	WgOrigo		m_itemOrigo;		// Origo of item inside cell.
	bool		m_bStretchItem;		// Should items be stretched to fill their cell?
	Uint32		m_bFixedSize; 		// Should canvas width or height (depending on layout) remain a fixed size?
	bool		m_bPackCells;		// If set, the cells are packed as closely as possible, otherwise
									// they are given equal space.
	bool		m_bOpaqueCells;		// Are cells opaque for pointer?

	WgMinMax2D	m_cellSize;			// Constraints the size of each grid-cell containing item.
	Uint8		m_cellPaddingX;		// Inner padding of cell and it's items. m_itemSpacing holds outer padding.
	Uint8		m_cellPaddingY;		// " -

	WgSize		m_biggestCell;		// Size of the biggest cell (after m_cellSize applied), good to know if they are not packed...

	ItemLayout	m_layout;

	WgItem*		m_pLastMarkedItem;
};



#endif // WDG_GRIDVIEW_DOT_H
