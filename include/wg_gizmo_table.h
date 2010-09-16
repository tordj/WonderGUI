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

#ifndef  WG_BLOCKSET_DOT_H
#	include <wg_blockset.h>
#endif

#ifndef WG_GIZMO_HOOK_DOT_H
#	include <wg_gizmo_hook.h>
#endif

#ifndef WG_GIZMO_DOT_H
#	include <wg_gizmo.h>
#endif

#ifndef WG_INTERFACE_TEXTHOLDER_DOT_H
#	include <wg_interface_textholder.h>
#endif

#ifndef WG_GIZMO_COLLECTION_DOT_H
#	include <wg_gizmo_collection.h>
#endif

class WgText;
class WgFont;
class WgTableRow2;
class WgTableColumn2;

typedef int(*fpGizmoCmp)(WgGizmo*,WgGizmo*);



//____ WgTableHook ____________________________________________________________

class WgTableHook : public WgGizmoHook, public WgEmitter
{
	friend class WgTableRow2;

	WgCord	Pos() const;
	WgSize	Size() const;
	WgRect	Geo() const;
	WgCord	ScreenPos() const;
	WgRect	ScreenGeo() const;

	WgGizmoHook*	PrevHook() const;
	WgGizmoHook*	NextHook() const;

	WgTableHook*	PrevInTable() const;
	WgTableHook*	NextInTable() const;

	WgTableHook*	PrevInRow() const;
	WgTableHook*	NextInRow() const;

	WgTableHook*	PrevInColumn() const;
	WgTableHook*	NextInColumn() const;

	inline WgTableRow2*	GetRow() const;
	WgTableColumn2*	GetColumn() const;
	inline int		GetColumnNb() const;



	// Needs to be here for now since Emitters are inherrited by Widgets. Shouldn't be hooks business in the future...

	WgEmitter* 	GetEmitter();
	WgWidget*	GetRoot();			// Should in the future not return a widget, but a gizmo.

protected:
	WgTableHook( WgGizmo * pGizmo, WgTableRow2 * pRow );
	~WgTableHook();

	WgGizmo* ReleaseGizmo();
	

	void	RequestRender();
	void	RequestRender( const WgRect& rect );
	void	RequestResize();

	bool	RequestFocus();
	bool	ReleaseFocus();
};

//____ WgTableColumn2 _________________________________________________________

class WgTableColumn2 : public Wg_Interface_TextHolder, public WgGizmoCollection
{
friend class WgGizmoTable;
friend class WgTableHook;

public:
	WgTableColumn2();
	WgTableColumn2(const WgTableColumn2& column);

	inline void		setID( Sint64 id ) { m_id = id; }
	inline Sint64	getID() const { return m_id; }

	void			setWidth( int pixels );
	inline int		getWidth() const { return m_pixelWidth; }

	void			hide();
	void			show();
	inline bool		isHidden() const { return !m_bVisible; }

	void			enable();
	void			disable();
	inline bool		isDisabled() const { return !m_bEnabled; }

	inline void		setInitialSortOrder( bool bAscend ) { m_bInitialAscend = bAscend; }
	inline bool		isInitialAscend() const { return m_bInitialAscend; }

	inline int		index() const;


	WgTableHook*	FirstHook() const;
	WgTableHook*	LastHook() const;

	void setSortFunction( fpGizmoCmp pFunc );
	inline fpGizmoCmp getSortFunction() const { return m_fpCompare; }

private:
	WgTableColumn2( WgGizmoTable * pOwner );
	~WgTableColumn2();

	WgGizmoHook*	_firstHook() const { return FirstHook(); }
	WgGizmoHook*	_lastHook() const { return LastHook(); }

	void			TextModified();
	void			SetTextObj(WgText *text);
	const WgText*	GetTextObj() const;
	WgText *		GetTextObj();


	Sint64			m_id;
	int				m_pixelWidth;
	fpGizmoCmp		m_fpCompare;
	bool			m_bVisible;
	bool			m_bEnabled;
	bool			m_bInitialAscend;
	WgGizmoTable *	m_pTable;

	WgGizmo *		m_pDefaultGizmo;

	
};

//____ WgTableRow2 ____________________________________________________________

class WgTableRow2 : protected WgLink, public WgGizmoCollection
{
friend class WgGizmoTable;
friend class WgTableHook;

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

	WgTableHook*	FirstHook() const;		// Gets first hook with a gizmo.
	WgTableHook*	LastHook() const;		// Gets last hook with a gizmo.
	WgTableHook*	GetHook( int cell );	// Returns 0 if hook has no gizmo.
	WgGizmo*		GetGizmo( int cell );

	void			SetGizmo( WgGizmo * pGizmo, int cell );
	int				AddGizmo( WgGizmo * pGizmo );

	WgGizmoTable*	GetTable() const { return m_pTable; }
protected:
		LINK_METHODS( WgTableRow2 );

private:
	WgGizmoHook*	_firstHook() const { return FirstHook(); }
	WgGizmoHook*	_lastHook() const { return LastHook(); }

	int			m_nCells;
	WgTableHook* m_pCells;
	WgGizmoTable* m_pTable;

	Sint64		m_id;
	int			m_height;
	WgMode		m_mode;
	bool		m_bVisible;
};

//____ WgGizmoTable ___________________________________________________________

class WgGizmoTable : public WgGizmo, public WgGizmoCollection
{
friend class WgTableColumn2;
friend class WgTableRow2;
friend class WgTableHook;

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

	int		AddColumn( const WgCharSeq& text, int pixelwidth, WgOrigo& origo = WgOrigo::midLeft(), int(*fpCompare)(WgGizmo*,WgGizmo*) = 0, bool bInitialAscend = true, bool bEnabled = true, Sint64 id = 0, WgGizmo * pDefaultGizmo = 0 );
	void	RemoveColumns();
//	bool	SetColumnWidth( Uint32 column, Uint32 pixelwidth );
//	bool	SetColumnVisible( Uint32 column, bool bVisible );

	int		NbColumns() const	{ return m_nColumns; }
	WgTableColumn2* GetColumn(int index) const { return (index<m_nColumns)?&m_pColumns[index]:0; }
	WgTableColumn2* FindColumn(Sint64 id) const;

	bool	SortRows( int column, bool bAscend, int prio = 0 );
	bool	SetClickSortPrio( int prio );
	int		GetClickSortPrio() const { return m_clickSortPrio; }

	int		GetSortColumns()	{ return c_nSortColumns; }
	bool	GetSortColumn( int order, int& columnIndex, bool& bAscend );

	int				AddRow( WgTableRow2* pRow );
	int				InsertRow( WgTableRow2* pRow, int pos );
	void			InsertRowSorted( WgTableRow2* pRow );
	int				GetRowNb( WgTableRow2* pRow );
	WgTableRow2*	GetRow( int pos );
	WgTableRow2*	RemoveRow( int pos );
	bool			RemoveRow( WgTableRow2* pRow );
	void			RemoveAllRows();
	bool			DeleteRow( int pos );
	bool			DeleteRow( WgTableRow2* pRow );
	void			DeleteAllRows();
	WgTableRow2*	FindRow( int id );
	void			SetSelectedRowColor( WgColor c );
	void			SetSelectedRowBg( WgBlockSetPtr pBlock );
	bool			HasSelectedRowBg() const { if( m_pSelectedRowGfx ) return true; return false; }
	WgBlockSetPtr	GetSelectedRowBg() const { return m_pSelectedRowGfx; }
	WgColor			GetSelectedRowColor() const { return m_selectedRowColor; }

	inline WgTableRow2*	GetFirstRow() { return m_rows.getFirst(); }
	inline WgTableRow2*	GetLastRow() { return m_rows.getLast(); }
	inline int			NbRows() { return m_nRows; }
	inline int			CompareRows(WgTableRow2* p1, WgTableRow2* p2);

	void	ScrollIntoView( WgTableRow2* pRow );

	void	SetAutoScrollMarked( bool bAutoScroll )			{ m_bAutoScrollMarked = bAutoScroll; }
	bool	GetAutoScrollMarked() const						{ return m_bAutoScrollMarked; }

	void	ShowHeader( bool bShow );
	bool	ShowHeader() const { return m_bShowHeader; }

//	WgItem	GetCell( Uint32 row, Uint32 column );

	virtual WgString	GetTooltipString() const;

	WgTableHook*		FirstGizmo() const;
	WgTableHook*		LastGizmo() const;

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
	SelectMode	GetSelectMode() const { return m_selectMode; }

	WgRect	GetCellGeo( int row, int column );

protected:

	void	OnCloneContent( const WgGizmo * _pOrg );
	void	OnRender( WgGfxDevice * pDevice, const WgRect& _window, const WgRect& _clip, Uint8 _layer );
	void	OnNewSize( const WgSize& size );
	void	OnAction( WgEmitter * pEmitter, WgInput::UserAction action, int button_key, const WgActionDetails& info, const WgInput& inputObj );
	bool	OnMarkTest( const WgCord& ofs );
	void	OnEnable();
	void	OnDisable();

private:
	WgGizmoHook*	_firstHook() const { return FirstHook(); }
	WgGizmoHook*	_lastHook() const { return LastHook(); }


	void		refreshRows();
	void		RowModified( WgTableRow2* pRow, int widthDiff , int heightDiff );
	void		RowMarkChanged( WgTableRow2* pRow, bool bMarked );
	WgTableRow2*	GetMarkedRow( int x, int y );

	void		ConnectRow( WgTableRow2* pRow, WgTableRow2* pPlaceBefore = 0 );
	void		DisconnectRow( WgTableRow2* pRow );
	WgTableRow2* FindRowInsertSpot( WgTableRow2* pFirst, WgTableRow2* pLast, WgTableRow2* pRow, Uint32 nRows );

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

	WgTableColumn2*	GetHeaderColumnAt(int x, int y);
	WgTableColumn2*	m_pMarkedHeader;						// Header currently marked by mouse


	WgBlockSetPtr	m_pSelectedRowGfx;
	WgColor			m_selectedRowColor;
};



WgTableRow2* WgTableHook::GetRow() const 
{
	return static_cast<WgTableRow2*>(m_pCollection);
}

inline int WgTableHook::GetColumnNb() const
{
	return this - GetRow()->m_pCells;
}

inline int WgTableColumn2::index() const 
{ 
	return this - m_pTable->m_pColumns; 
}



#endif // WG_GIZMO_TABLE_DOT_H
