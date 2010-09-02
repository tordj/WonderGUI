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

#ifndef WG_GIZMO_TABLE_DOT_H
#define WG_GIZMO_TABLE_DOT_H


#ifndef WG_COLOR_DOT_H
#	include <wg_color.h>
#endif

#ifndef WG_GIZMO_HOOK_DOT_H
#	include <wg_gizmo_hook.h>
#endif

class WgText;
class WgFont;
class Wdg_TableView;

typedef int(*fpCellCmp)(WgGizmo*,WgGizmo*);

//____ WgTableHook ____________________________________________________________

class WgTableHook : public WgGizmoHook, public WgEmitter
{
	WgCord	Pos() const;
	WgSize	Size() const;
	WgRect	Geo() const;
	WgCord	ScreenPos() const;
	WgRect	ScreenGeo() const;

	WgGizmoHook * PrevHook() const;
	WgGizmoHook * NextHook() const;

	// Needs to be here for now since Emitters are inherrited by Widgets. Shouldn't be hooks business in the future...

	WgEmitter* 	GetEmitter();
	WgWidget*	GetRoot();			// Should in the future not return a widget, but a gizmo.

protected:
	WgTableHook();
	WgTableHook( WgGizmo * pGizmo, WgTableRow2 * pRow );
	~WgTableHook();

	WgGizmo* ReleaseGizmo();
	

	void	RequestRender();
	void	RequestRender( const WgRect& rect );
	void	RequestResize();

	bool	RequestFocus();
	bool	ReleaseFocus();
}

//____ WgTableColumn2 _________________________________________________________

class WgTableColumn2 : public Wg_Interface_TextHolder, public WgGizmoCollection
{
friend class WgGizmoTable;

public:
	WgTableColumn2();
	WgTableColumn2(const WgTableColumn& column);

	inline void		SetID( Sint64 id ) { m_id = id; }
	inline Sint64	GetID() const { return m_id; }

	void			SetWidth( int pixels );
	inline int		GetWidth() const { return m_pixelWidth; }

	void			Hide();
	void			Show();
	inline bool		IsHidden() const { return !m_bVisible; }

	void			Enable();
	void			Disable();
	inline bool		IsDisabled() const { return !m_bEnabled; }

	inline void		SetInitialSortOrder( bool bAscend ) { m_bInitialAscend = bAscend; }
	inline bool		IsInitialAscend() const { return m_bInitialAscend; }

	WgGizmoHook *	FirstHook() const;
	WgGizmoHook *	LastHook() const;


	void SetSortFunction( fpItemCmp pFunc );
	inline fpItemCmp GetSortFunction() const { return m_fpCompare; }

private:
	WgTableColumn2( Wdg_TableView * pOwner );
	~WgTableColumn2();

	void			TextModified();
	void			SetTextObj(WgText *text);
	const WgText*	GetTextObj() const;
	WgText *		GetTextObj();

	Sint64			m_id;
	int				m_pixelWidth;
	fpItemCmp		m_fpCompare;
	bool			m_bVisible;
	bool			m_bEnabled;
	bool			m_bInitialAscend;
	WgGizmoTable *	m_pOwner;

	WgGizmo *		m_pDefaultGizmo;
};

//____ WgTableRow2 ____________________________________________________________

class WgTableRow2 : protected WgLink, public WgGizmoCollection
{
friend class WgGizmoTable;

public:
	WgTableRow2( Sint64 id = 0, int nCells = 2 );
	virtual ~WgTableRow2();

	Sint64	Id() const	{ return m_id; };
	int		Height() const { return m_height; }

	bool Select();
	void Unselect();
	inline bool IsSelected() const { if( m_mode == WG_MODE_SELECTED ) return true; return false; }

	inline	WgTableRow2* GetNext() const	{ return getNext(); };
	inline	WgTableRow2* GetPrev() const	{ return getPrev(); };

	inline	void	SetVisible(bool bVisible);
	inline	void	Show() { SetVisible(true); }
	inline	void	Hide() { SetVisible(false); }
	inline	bool	IsVisible() const { return m_bVisible; }
	inline	bool	IsHidden() const { return !m_bVisible; }

	WgGizmoHook *	FirstHook() const;
	WgGizmoHook *	LastHook() const;


	void	SetGizmo( WgGizmo * pGizmo, int col );
	int		AddGizmo( WgGizmo * pGizmo );

protected:
		LINK_METHODS( WgTableRow2 );

private:

	int			m_nCells;
	WgTableHook*m_pCells;

	Sint64		m_id;
	int			m_height;
	WgMode		m_mode;
	bool		m_bVisible;
};

class WgGizmoTable : public WgGizmo
{
friend class WgTableColumn2;

public:
	WgGizmoTable();
	virtual ~WgGizmoTable();
	virtual const char * Type() const;
	static const char * GetMyType();

	//____ Methods __________________________________________

	bool	SetHeaderTextProp( const WgTextPropPtr& pProp );
	inline	WgTextPropPtr GetHeaderTextProp() const { return m_pHeaderProps; };

	bool	SetHeaderSource( const WgBlockSetPtr& pHeader );
	WgBlockSetPtr GetHeaderSource() const { return m_pHeaderGfx; }

	void	SetArrowSource( const WgBlockSetPtr& pAscend, const WgBlockSetPtr& pDescend );
	WgBlockSetPtr GetArrowAscend() const { return m_pAscendGfx; }
	WgBlockSetPtr GetArrowDescend() const { return m_pDescendGfx; }
	void	SetArrowPos( const WgOrigo& origo, int xOfs, int yOfs );
	void	SetArrowPos( int xOfs, int yOfs );
	int		GetArrowPosX() const { return m_sortMarkerOfs.x; }
	int		GetArrowPosY() const { return m_sortMarkerOfs.y; }

	void	SetArrowOrigo( WgOrigo origo );
	WgOrigo	GetArrowOrigo() const { return m_sortMarkerOrigo; }

	void	SetCellPadding( int x, int y );

	int		GetCellPaddingX()	{ return m_cellPaddingX; }
	int		GetCellPaddingY()	{ return m_cellPaddingY; }

	void	SetAutoScaleHeaders(bool autoScaleHeaders);
	bool	GetAutoScaleHeaders() const { return m_bAutoScaleHeader; }
	float	CalcHeaderScaleFactor();

	int		GetHeaderHeight()	{ return (m_bShowHeader && m_pHeaderGfx) ? m_pHeaderGfx->GetHeight(0) : 0; }

	void	SetEmptyRowHeight( int height );
	int		GetEmptyRowHeight() { return m_emptyRowHeight; }

	void	SetRowBlocks( WgBlockSetPtr * pRowBlocks, int nRowSource );
	int		GetRowBlockCount() { return m_nRowBlocks; }
	WgBlockSetPtr* GetRowBlocks( ) { return m_pRowBlocks; }
	void	RemoveRowBlocks();

	void	SetRowColors( WgColor * pRowColors, int nRowColors );
	int		GetRowColorCount() { return m_nRowColors; }
	WgColor*GetRowColors( ) { return m_pRowColors; }
	void	RemoveRowColors();

	int		AddColumn( const WgCharSeq& text, int pixelwidth, WgOrigo& origo = WgOrigo::midLeft(), int(*fpCompare)(WgItem *,WgItem *) = 0, bool bInitialAscend = true, bool bEnabled = true, Sint64 id = 0, WgGizmo * pDefaultGizmo = 0 );
	void	RemoveColumns();
//	bool	SetColumnWidth( Uint32 column, Uint32 pixelwidth );
//	bool	SetColumnVisible( Uint32 column, bool bVisible );

	int		NbColumns() const	{ return m_nColumns; }
	WgTableColumn* GetColumn(int index) const { return &m_pColumns[index]; }
	WgTableColumn* FindColumn(Sint64 id) const;

	bool	SortRows( int column, bool bAscend, int prio = 0 );
	bool	SetClickSortPrio( int prio );
	int		GetClickSortPrio() const { return m_clickSortPrio; }

	int		GetSortColumns()	{ return c_nSortColumns; }
	bool	GetSortColumn( int order, int& columnIndex, bool& bAscend );

	inline int			AddRow( WgTableRow2* pRow )  { return AddItem( pRow );  }
	inline int			InsertRow( WgTableRow2* pRow, int pos ) { return InsertItem( pRow, pos ); }
	inline void			InsertRowSorted( WgTableRow2* pRow ) { InsertItemSorted( pRow ); }
	inline int			GetRowNb( WgTableRow2* pRow ) { return GetItemPos( pRow ); }
	inline WgTableRow2*	GetRow( int pos ) { return (WgTableRow2*) GetItem(pos); }
	WgTableRow2*		RemoveRow( int pos );
	bool				RemoveRow( WgTableRow2* pRow );
	void				RemoveAllRows();
	bool				DeleteRow( int pos );
	bool				DeleteRow( WgTableRow2* pRow );
	void				DeleteAllRows();
	inline WgTableRow2*	FindRow( int id ) { return (WgTableRow2*) FindItem(id); }
	inline void			SetLineMarkColor( WgColor c ) { SetItemMarkColor(c); }
	void				SetLineMarkSource( WgBlockSetPtr pBlock );
	bool				HasLineMarkSource() const { if( m_pMarkedLineGfx ) return true; return false; }
	WgBlockSetPtr		GetLineMarkSource() const { return m_pMarkedLineGfx; }
	WgColor				GetLineMarkColor() const { return GetItemMarkColor(); }

	inline WgTableRow2*	GetFirstRow() { return (WgTableRow2*) GetFirstItem(); }
	inline WgTableRow2*	GetLastRow() { return (WgTableRow2*) GetLastItem(); }
	inline int			NbRows() { return NbItems(); }
	inline int			CompareRows(WgTableRow2* p1, WgTableRow2* p2);

	void	ScrollIntoView( WgTableRow2* pRow );

	void	SetAutoScrollMarked( bool bAutoScroll )			{ m_bAutoScrollMarked = bAutoScroll; }
	bool	GetAutoScrollMarked() const						{ return m_bAutoScrollMarked; }

	void	ShowHeader( bool bShow );
	bool	ShowHeader() const { return m_bShowHeader; }

//	WgItem	GetCell( Uint32 row, Uint32 column );

	virtual WgString	GetTooltipString() const;

	// Overloaded from WgGizmo

	Uint32	HeightForWidth( Uint32 width ) const;
	Uint32	WidthForHeight( Uint32 height ) const;

	WgSize	MinSize() const;
	WgSize	BestSize() const;
	WgSize	MaxSize() const;

	bool	IsView() const { return false; }
	bool	IsContainer() const { return true; }

	// New methods!

	enum SelectMode
	{
		SELECT_SINGLE_CELL,
		SELECT_SINGLE_ROW,
		SELECT_SINGLE_COLUMN,
		SELECT_MULTI_CELL,
		SELECT_MULTI_ROW,
		SELECT_MULTI_COLUMN
	};

	void	SetSelectMode( SelectMode mode );
	SelectMode	SelectMode() const { return m_selectMode; }

protected:

	void	OnCloneContent( const WgGizmo * _pOrg );
	void	OnRender( WgGfxDevice * pDevice, const WgRect& _window, const WgRect& _clip, Uint8 _layer );
	void	OnNewSize( const WgSize& size );
	void	OnAction( WgEmitter * pEmitter, WgInput::UserAction action, int button_key, const WgActionDetails& info, const WgInput& inputObj );
	bool	OnMarkTest( const WgCord& ofs );
	void	OnEnable();
	void	OnDisable();

private:

	void		refreshRows();
	void		RowModified( WgTableRow* pRow, int widthDiff , int heightDiff );
	void		RowMarkChanged( WgTableRow* pItem, bool bMarked );
	WgTableRow*	GetMarkedRow( int x, int y );
	int			CompareRows(WgTableRow* pRow1, WgTableRow* pRow2 );
	void		RowAdded( WgTableRow* pRow );

	int			GetMarkedColumn( int x, int& saveXOfs );
	int			GetMarkedRow( int y, WgTableRow2*& pSaveRow, int& saveYOfs );

	void		UpdateContentSize();

	struct SortInfo
	{
		SortInfo() { column = 0xFFFF; bAscend = true; }

		Uint16		column;
		bool		bAscend;
	};

	WgChain<WgTableRow2>	m_rows;
	int						m_nRows;

	SelectMode		m_selectMode;

	static const int c_nSortColumns = 4;
	SortInfo		m_sortStack[c_nSortColumns];			// 0xFFFF = none

	int				m_clickSortPrio;

	bool			m_bShowHeader;
	bool			m_bAutoScaleHeader;

	bool			m_bAutoScrollMarked;

	int				m_nColumns;
	WgTableColumn2*	m_pColumns;

	int				m_nRowColors;
	WgColor*		m_pRowColors;

	int				m_nRowBlocks;
	WgBlockSetPtr*	m_pRowBlocks;

	int				m_emptyRowHeight;						// Set if empty rows should fill out the view.

	WgTextPropPtr	m_pHeaderProps;

	WgOrigo			m_sortMarkerOrigo;
	WgCord8			m_sortMarkerOfs;

	int				m_cellPaddingX;
	int				m_cellPaddingY;

	WgBlockSetPtr	m_pAscendGfx;
	WgBlockSetPtr	m_pDescendGfx;

	int				m_lastSortColumn;
	bool			m_lastSortColumnAscendStatus;


	WgTableRow2*	m_pLastMarkedRow;
	int				m_lastClickedRow;
	int				m_lastClickedColumn;

	// Graphics block for the header-pieces

	enum
	{
		HEADER_SRC_NORMAL = 0,
		HEADER_SRC_OVER,
		HEADER_SRC_DOWN,
		HEADER_SRC_HIGHLIGHT,
	};

	WgBlockSetPtr	m_pHeaderGfx;

	WgTableColumn*	GetHeaderColumnAt(int x, int y);
	WgTableColumn*	m_pMarkedHeader;						// Header currently marked by mouse


	WgBlockSetPtr	m_pMarkedLineGfx;
};

#endif // WG_GIZMO_TABLE_DOT_H
