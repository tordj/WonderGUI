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

#ifndef WDG_TABLEVIEW_DOT_H
#define WDG_TABLEVIEW_DOT_H

#ifndef WDG_BASECLASS_VIEW_DOT_H
#	include <wdg_baseclass_view.h>
#endif

#include <wg_interface_itemholder.h>
#include <wg_tileholder.h>
#include <wg_item_row.h>
#include <wg_colorset.h>


class WgText;
class WgFont;
class Wdg_TableView;



typedef Sint32(*fpItemCmp)(WgItem*,WgItem*, const WgSortContext& context);




class WgTableColumn : public Wg_Interface_TextHolder
{
friend class Wdg_TableView;

public:
	WgTableColumn();
	WgTableColumn(const WgTableColumn& column);

	inline void		SetID( Uint32 id ) { m_id = id; }
	inline Uint32	GetID() const { return m_id; }

	void			SetItem( WgItem * pItem );
	inline WgItem * GetItem() const { return m_pItem; }

	void			SetItemPos( const WgOrigo& origo, WgCoord ofs );
	WgCoord			GetItemOfs() const { return m_itemOfs; }
	inline WgOrigo	GetItemOrigo() const { return m_itemOrigo; }

	void			SetWidth( int pixels );
	inline Uint32	GetWidth() const { return (Uint32) m_pixelWidth; }

	void			SetContentWidth( int pixels );
	int				GetContentWidth() const;

	void			SetMinWidth( int pixels );
	inline int		GetMinWidth( int pixels ) const { return m_minWidth; }

	void			SetMaxWidth( int pixels );
	inline int		GetMaxWidth( int pixels ) const { return m_maxWidth; }

	void			SetScaleWeight( float weight );
	inline float	GetScaleWeight() const { return m_scaleWeight; }

	void			Hide();
	void			Show();
	inline bool		IsHidden() const { return !m_bVisible; }

	void			Enable();
	void			Disable();
	inline bool		IsDisabled() const { return !m_bEnabled; }

	inline void		SetInitialSortOrder( bool bAscend ) { m_bInitialAscend = bAscend; }
	inline bool		IsInitialAscend() const { return m_bInitialAscend; }

	bool	IsAutoEllipsisDefault() const { return true; };

	void SetSortFunction( fpItemCmp pFunc );
	inline fpItemCmp GetSortFunction() const { return m_fpCompare; }

private:
	WgTableColumn( Wdg_TableView * pOwner );
	~WgTableColumn();

	void			_textModified();
	void			SetTextObj(WgText *text);
	const WgText*	GetTextObj() const;
	WgText *		GetTextObj();

	int				m_id;

	int				m_defWidth;
	int				m_minWidth;
	int				m_maxWidth;
	float			m_scaleWeight;

	float			m_pixelWidth;			// Should always contain a fixed point value except when calculations are performed.
	fpItemCmp		m_fpCompare;
	bool			m_bVisible;
	bool			m_bEnabled;
	bool			m_bInitialAscend;
	Wdg_TableView *	m_pOwner;

	WgItem *		m_pItem;
	WgOrigo			m_itemOrigo;
	WgCoord			m_itemOfs;

};


class WgTableRow : protected WgItemRow
{
friend class Wdg_TableView;

public:
	WgTableRow() {}
	WgTableRow( Uint32 id ) : WgItemRow(id) {}
	virtual ~WgTableRow() {}

	Sint32	Id() const		{ return WgItem::Id(); };
	Sint64	Id64() const	{ return WgItem::Id64(); };
	Uint32	Height() const { return m_height; }

	bool Select( bool bSelectItems = true );
	void Unselect( bool bUnselectItems = true );
	inline bool IsSelected() const { if( m_mode == WG_MODE_SELECTED ) return true; return false; }

	inline	WgTableRow* GetNext() const	{ return (WgTableRow*)WgItemRow::Next(); };
	inline	WgTableRow* GetPrev() const	{ return (WgTableRow*)WgItemRow::Prev(); };

	inline	void	SetVisible(bool bVisible) { WgItemRow::SetVisible(bVisible); }
	inline	void	Show() { WgItemRow::SetVisible(true); }
	inline	void	Hide() { WgItemRow::SetVisible(false); }
	inline	bool	IsVisible() const { return WgItemRow::IsVisible(); }
	inline	bool	IsHidden() const { return !WgItemRow::IsVisible(); }

	inline	WgItem* GetFirstItem() { return WgItemRow::GetFirstItem(); };
	inline	WgItem* GetLastItem() { return WgItemRow::GetLastItem(); };

	inline	WgItem* GetItem(int column) { return WgItemRow::GetItem(column); }

	inline void SetHeightModify(int pixels) { WgItemRow::SetHeightModify(pixels); }
	

	void	SetItem( WgItem * pItem, Uint32 col );
	void	AddItem( WgItem * pItem );

/*
	void	SetTooltipString( const WgString& str )		{ WgItemRow::SetTooltipString( str ); }
	void	SetTooltipString( const WgChar* pString )	{ WgItemRow::SetTooltipString( pString ); }
	void	SetTooltipString( const Uint16* pString )	{ WgItemRow::SetTooltipString( pString ); }
	void	SetTooltipString( const char* pString )		{ WgItemRow::SetTooltipString( pString ); }
*/
protected:
	virtual WgRect	RequestItemGeo( WgItem * pItem );		// Overloading version from WgItemRow

};

class Wdg_TableView : public Wdg_Baseclass_View, public Wg_Interface_ItemHolder, public WgTileHolder
{
friend class WgTableColumn;

public:
	WIDGET_CONSTRUCTORS(Wdg_TableView,Wdg_Baseclass_View)
	virtual ~Wdg_TableView();
	virtual const char * Type() const;
	static const char * GetMyType();

	//____ Methods __________________________________________

	bool	SetColumnOrder( std::vector<int> columns );
	inline const std::vector<int>& GetColumnOrder() const { return m_columnOrder; }
	void	ResetColumnOrder();

	bool	SetHeaderTextProp( const WgTextPropPtr& pProp );
	inline	WgTextPropPtr GetHeaderTextProp() const { return m_pHeaderProps; };

	bool	SetHeaderSource( const WgBlockSetPtr& pHeaderNormalColumn, const WgBlockSetPtr& pHeaderSelectedColumn );
	WgBlockSetPtr GetHeaderSourceNormal() const { return m_pHeaderGfxNormal; }
	WgBlockSetPtr GetHeaderSourceSelected() const { return m_pHeaderGfxSelected; }

	void	SetArrowSource( const WgBlockSetPtr& pAscend, const WgBlockSetPtr& pDescend );
	WgBlockSetPtr GetArrowAscend() const { return m_pAscendGfx; }
	WgBlockSetPtr GetArrowDescend() const { return m_pDescendGfx; }
	void	SetArrowPos( const WgOrigo& origo, Sint32 xOfs, Sint32 yOfs );
	void	SetArrowPos( Sint32 xOfs, Sint32 yOfs );
	Sint32	GetArrowPosX() const { return m_sortMarkerOfs.x; }
	Sint32	GetArrowPosY() const { return m_sortMarkerOfs.y; }

	void	SetArrowOrigo( WgOrigo origo );
	WgOrigo	GetArrowOrigo() const { return m_sortMarkerOrigo; }

	void	SetCellPadding( Uint8 x, Uint8 y );

	Uint8	GetCellPaddingX()	{ return m_cellPaddingX; }
	Uint8	GetCellPaddingY()	{ return m_cellPaddingY; }

	void	SetAutoScaleHeaders(bool autoScaleHeaders);
	bool	GetAutoScaleHeaders() const { return m_bAutoScaleHeader; }

	int		GetHeaderHeight() const;

	void	SetEmptyRowHeight( Uint32 height );
	Uint32	GetEmptyRowHeight() { return m_emptyRowHeight; }

	Uint32	AddColumn( const char * pText, Uint32 pixelwidth, WgOrigo& origo = WgOrigo::midLeft(), fpItemCmp fpCompare = 0, bool bInitialAscend = true, bool bEnabled = true, int id = 0 );
	void	RemoveColumns();
//	bool	SetColumnWidth( Uint32 column, Uint32 pixelwidth );
//	bool	SetColumnVisible( Uint32 column, bool bVisible );

	Uint32	NbColumns() const	{ return m_nColumns; }
	WgTableColumn* GetColumn(Uint32 index) const { return &m_pColumns[index]; }
	WgTableColumn* FindColumn(Uint32 id) const;

	bool	SortRows( Uint32 column, bool bAscend, Uint8 prio = 0 );
	bool	SetClickSortPrio( Uint8 prio );
	Uint8	GetClickSortPrio() const { return m_clickSortPrio; }

	Uint32	GetSortColumns()	{ return c_nSortColumns; }
	bool	GetSortColumn( Uint32 order, Uint32& columnIndex, bool& bAscend );

	inline void			AddRow( WgTableRow * pRow )  { AddItem( pRow );  }
	inline void			InsertRow( WgTableRow * pRow, Uint32 pos ) { InsertItem( pRow, pos ); }
	inline void			InsertRowSorted( WgTableRow * pRow ) { InsertItemSorted( pRow ); }
	inline Sint32		GetRowNb( WgTableRow * pRow ) { return GetItemPos( pRow ); }
	inline WgTableRow *	GetRow( Uint32 pos ) { return (WgTableRow *) GetItem(pos); }
	inline const WgTableRow *	GetRow( Uint32 pos ) const { return (WgTableRow *) GetItem(pos); }
	WgTableRow *		RemoveRow( Uint32 pos );
	bool				RemoveRow( WgTableRow * pRow );
	void				RemoveAllRows();
	bool				DeleteRow( Uint32 pos );
	bool				DeleteRow( WgTableRow * pRow );
	void				DeleteAllRows();
	inline WgTableRow *	FindRow( Sint32 id ) { return (WgTableRow*) FindItem(id); }
/*
	inline void			SetLineMarkColor( WgColor c ) { SetItemMarkColor(c); }
	void				SetLineMarkSource( WgBlockSetPtr pBlock );
	bool				HasLineMarkSource() const { if( m_pMarkedLineGfx ) return true; return false; }
	WgBlockSetPtr		GetLineMarkSource() const { return m_pMarkedLineGfx; }
	WgColor				GetLineMarkColor() const { return GetItemMarkColor(); }
*/
	inline WgTableRow *	GetFirstRow() { return (WgTableRow *) GetFirstItem(); }
	inline WgTableRow *	GetLastRow() { return (WgTableRow *) GetLastItem(); }
	inline Uint32		NbRows() { return NbItems(); }
	inline Sint32		CompareRows(WgTableRow * p1, WgTableRow * p2) { return CompareItems(p1,p2); }

	void	ScrollIntoView( WgTableRow* pRow );

	void	SetAutoScrollMarked( bool bAutoScroll )			{ m_bAutoScrollMarked = bAutoScroll; }
	bool	GetAutoScrollMarked() const						{ return m_bAutoScrollMarked; }

	void	ShowHeader( bool bShow );
	bool	ShowHeader() const { return m_bShowHeader; }

	WgRect	GetCellGeo( int row, int column );
	WgRect	GetHeaderGeo( int column );

	WgItem*	GetCell( Uint32 row, Uint32 column );

	WgString			GetTooltipString() const;
	WgItem*				GetTooltipItem() const;
	WgPointerStyle		GetPointerStyle() const;

	inline bool	GrabInputFocus() { return WgWidget::GrabInputFocus(); }
	inline bool	RemoveInputFocus() { return WgWidget::GrabInputFocus(); }

protected:
	WgWidget * NewOfMyType() const;

	void	DoMyOwnGeometryChange( WgRect& oldGeo, WgRect& newGeo );
	void	DoMyOwnGeometryChangeSubclass( WgRect& oldGeo, WgRect& newGeo );
	void	DoMyOwnActionRespond( WgInput::UserAction action, int button_key, const WgActionDetails& info, const WgInput& inputObj );
	void	DoMyOwnRender( const WgRect& _window, const WgRect& _clip, Uint8 _layer );
//	void	DoMyOwnRefresh( void );
	void	DoMyOwnCloning( WgWidget * _pClone, const WgWidget * _pCloneRoot, const WgWidget * _pBranchRoot );
	bool	DoMyOwnMarkTest( int _x, int _y );
	void	DoMyOwnDisOrEnable( void );

private:
	void 	Init();

	WgRect	_headerItemGeo( WgTableColumn * pHeader, const WgRect& headerGeo ) const;
	void	_tilesModified();

	int		GetColumnAtPosition( int position ) const;
	WgItem * GetItemAtPosition( int position, WgTableRow * pRow );
	int		GetPositionOfColumn( int col );
	WgItem *GetExtendedCellContent( WgTableRow * pRow, int col );

	void	DrawRowBg( const WgRect& clip, WgTableRow * pRow, int iVisibleRowNb, int RealRowNb, const WgRect& dest );

	void	TweakColumnWidths( int targetWidth );
	void	ExtendLastColumn( int targetWidth );
	void	AdaptCellsToWidth();

	void		refreshItems();
	void		ItemSizeModified( WgItem * pItem, Sint32 widthDiff , Sint32 heightDiff );
	void		ItemVisibilityModified( WgItem * pItem, bool bVisible );
	void		ItemMarkChanged( WgItem * pItem, bool bMarked );
	WgItem*		GetMarkedItem( Uint32 x, Uint32 y );
	Sint32		CompareItems(WgItem * pItem1, WgItem * pItem2 );
	void		ItemAdded( WgItem * pItem );

	WgItem*		GetMarkedHeaderItem( int x, int y ) const;


	int			GetMarkedColumn( Uint32 x, Uint32& saveXOfs );
	int			GetMarkedRow( Uint32 y, WgTableRow*& pSaveRow, Uint32& saveYOfs );

	void		UpdateContentSize();
	void		UpdateMarkedRowColumn( int row, int column );
	void		RecalcColumnWidths();

	WgRect		RequestItemGeo( WgItem * pItem );


	struct SortInfo
	{
		SortInfo() { column = 0xFFFF; bAscend = true; }

		Uint16		column;
		bool		bAscend;
	};

	static const int c_nSortColumns = 4;
	SortInfo		m_sortStack[c_nSortColumns];			// 0xFFFF = none

	Uint8			m_clickSortPrio;

	bool			m_bShowHeader;
	bool			m_bAutoScaleHeader;

	bool			m_bAutoScrollMarked;

	int				m_nColumns;
	WgTableColumn *	m_pColumns;

	std::vector<int> m_columnOrder;

	Uint32			m_emptyRowHeight;						// Set if empty rows should fill out the view.

	WgTextPropPtr	m_pHeaderProps;

	WgOrigo			m_sortMarkerOrigo;
	WgCoord8			m_sortMarkerOfs;

	Uint8			m_cellPaddingX;
	Uint8			m_cellPaddingY;

	WgBlockSetPtr	m_pAscendGfx;
	WgBlockSetPtr	m_pDescendGfx;

	Uint8			m_lastSortColumn;
	bool			m_lastSortColumnAscendStatus;

	WgItem *		m_pLastMarkedHeaderItem;

	WgItem *		m_pLastMarkedItem;
	int				m_lastClickedRow;
	int				m_lastClickedColumn;

	int				m_markedRow;
	int				m_markedColumn;

	// Graphics block for the header-pieces

	enum
	{
		HEADER_SRC_NORMAL = 0,
		HEADER_SRC_OVER,
		HEADER_SRC_DOWN,
		HEADER_SRC_HIGHLIGHT,
	};

	WgBlockSetPtr	m_pHeaderGfxNormal;
	WgBlockSetPtr	m_pHeaderGfxSelected;

	WgTableColumn*	GetHeaderColumnAt(int x, int y, int * wpOfsX = 0) const;
	WgTableColumn*	m_pMarkedHeader;						// Header currently marked by mouse
	WgTableColumn*	m_pPressedHeader;						// Header who have received current mouse button press


//	WgBlockSetPtr	m_pMarkedLineGfx;
};

#endif // WDG_TABLEVIEW_DOT_H
