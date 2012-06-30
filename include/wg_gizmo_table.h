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

#ifndef WG_HOOK_DOT_H
#	include <wg_hook.h>
#endif

#ifndef WG_INTERFACE_TEXTHOLDER_DOT_H
#	include <wg_interface_textholder.h>
#endif

#ifndef WG_GIZMO_CONTAINER_DOT_H
#	include <wg_gizmo_container.h>
#endif

class WgText;
class WgFont;
class WgGizmoTable;
class WgTableRow;
class WgTableColumn;

typedef int(*fpGizmoCmp)(WgGizmo*,WgGizmo*);



//____ WgTableHook ____________________________________________________________

class WgTableHook : public WgHook
{
	friend class WgTableRow;
	friend class WgGizmoTable;

	WgCoord			Pos() const;
	WgSize			Size() const;
	WgRect			Geo() const;
	WgCoord			ScreenPos() const;
	WgRect			ScreenGeo() const;

	WgTableHook*	PrevInTable() const;
	WgTableHook*	NextInTable() const;

	WgTableHook*	PrevInRow() const;
	WgTableHook*	NextInRow() const;

	WgTableHook*	PrevInColumn() const;
	WgTableHook*	NextInColumn() const;

	WgTableRow*	Row() const { return m_pRow; }
	WgTableColumn*	Column() const;
	inline int		ColumnNb() const;

	inline WgGizmoTable *	Parent() const;

	// Needs to be here for now since Emitters are inherrited by Widgets. Shouldn't be hooks business in the future...

protected:
	WgTableHook( WgTableRow * pRow );
	~WgTableHook();

	void	_requestRender();
	void	_requestRender( const WgRect& rect );
	void	_requestResize();

	WgHook *	_prevHook() const;
	WgHook *	_nextHook() const;
	WgGizmoParent * _parent() const;

	int				m_height;		// Minimum height needed for this Gizmo.
	WgTableRow *	m_pRow;			//
};

//____ WgTableColumn _________________________________________________________

class WgTableColumn : public Wg_Interface_TextHolder, public WgGizmoCollection
{
friend class WgGizmoTable;
friend class WgTableHook;

public:
	WgTableColumn();
	WgTableColumn(const WgTableColumn& column);

	void			SetId( Sint64 id ) { m_id = id; }
	Sint64			Id() const { return m_id; }

	void			SetWidth( int pixels );
	int				DesiredWidth() const { return m_desiredWidth; }
	int				RealWidth() const { return m_realWidth; }

	void			Hide();
	void			Show();
	bool			IsHidden() const { return !m_bVisible; }

	void			Enable();
	void			Disable();
	bool			IsDisabled() const { return !m_bEnabled; }

	void			SetInitialSortOrder( bool bAscend ) { m_bInitialAscend = bAscend; }
	bool			IsInitialAscend() const { return m_bInitialAscend; }

	inline int		Index() const;

	bool			IsAutoEllipsisDefault() const { return true; };


	WgTableHook*	FirstHook() const;
	WgTableHook*	LastHook() const;

	void 			SetSortFunction( fpGizmoCmp pFunc );
	fpGizmoCmp 		SortFunction() const { return m_fpCompare; }

	void			SetTextObj(WgText *text);
	const WgText*	GetTextObj() const;

private:
	WgTableColumn( WgGizmoTable * pOwner );
	~WgTableColumn();

	WgHook*	_firstHook() const { return FirstHook(); }
	WgHook*	_lastHook() const { return LastHook(); }

	void			_textModified();
	WgText *		_getTextObj();


	Sint64			m_id;
	int				m_desiredWidth;			// Columns desired width in pixels.
	int				m_realWidth;			// Columns real current width in pixels.

//	int				m_pixelWidth;
	fpGizmoCmp		m_fpCompare;
	bool			m_bVisible;
	bool			m_bEnabled;
	bool			m_bInitialAscend;
	bool			m_bWidthChanged;
	WgGizmoTable *	m_pTable;

	WgGizmo *		m_pDefaultGizmo;


};

//____ WgTableRow ____________________________________________________________

class WgTableRow : protected WgLink, public WgGizmoCollection
{
friend class WgGizmoTable;
friend class WgTableHook;
friend class WgChain<WgTableRow>;

public:
	WgTableRow( Sint64 id = 0, int nCells = 2 );
	virtual ~WgTableRow();

	Sint64	Id() const	{ return m_id; };
	int		Height() const { return m_height; }

	bool 			Select();
	void 			Unselect();
	bool 			IsSelected() const { if( m_mode == WG_MODE_SELECTED ) return true; return false; }

	WgTableRow* 	Next() const	{ return (WgTableRow*) WgLink::m_pNext; };
	WgTableRow* 	Prev() const	{ return (WgTableRow*) WgLink::m_pNext; };

	void			SetVisible(bool bVisible);
	void			Show() { SetVisible(true); }
	void			Hide() { SetVisible(false); }
	bool			IsVisible() const { return m_bVisible; }
	bool			IsHidden() const { return !m_bVisible; }

	WgTableHook*	FirstHook() const;		// Gets first hook with a gizmo.
	WgTableHook*	LastHook() const;		// Gets last hook with a gizmo.
	WgTableHook*	GetHook( int cell );	// Returns 0 if hook has no gizmo.
	WgGizmo*		GetGizmo( int cell );

	void			SetGizmo( WgGizmo * pGizmo, int cell );
	int				AddGizmo( WgGizmo * pGizmo );
	WgGizmo *		ReleaseGizmo( int cell );
	bool			DeleteGizmo( int cell );

	WgGizmoTable*	Table() const { return m_pTable; }
protected:
//		PROTECTED_LINK_METHODS( WgTableRow );

private:

	WgHook*	_firstHook() const { return FirstHook(); }
	WgHook*	_lastHook() const { return LastHook(); }

	void			_growCellsArray( int nCells );

	int				m_nCells;
	WgTableHook*	m_pCells;
	WgGizmoTable*	m_pTable;

	Sint64			m_id;
	int				m_height;
	WgMode			m_mode;
	bool			m_bVisible;
};

//____ WgGizmoTable ___________________________________________________________

class WgGizmoTable : public WgGizmoContainer
{
friend class WgTableColumn;
friend class WgTableRow;
friend class WgTableHook;

public:
	WgGizmoTable();
	virtual ~WgGizmoTable();
	virtual const char * Type() const;
	static const char * GetMyType();
	virtual WgGizmo * NewOfMyType() const { return new WgGizmoTable(); };


	//____ Methods __________________________________________

	bool			SetHeaderTextprop( const WgTextpropPtr& pProp );
	WgTextpropPtr 	GetHeaderTextprop() const { return m_pHeaderProps; };

	bool			SetHeaderSource( const WgBlocksetPtr& pHeader );
	WgBlocksetPtr 	GetHeaderSource() const { return m_pHeaderGfx; }

	void			SetArrowSource( const WgBlocksetPtr& pAscend, const WgBlocksetPtr& pDescend );
	WgBlocksetPtr	GetArrowAscend() const { return m_pAscendGfx; }
	WgBlocksetPtr	GetArrowDescend() const { return m_pDescendGfx; }
	void			SetArrowPos( const WgOrientation alignment, int xOfs, int yOfs );
	void			SetArrowPos( int xOfs, int yOfs );
	int				GetArrowPosX() const { return m_sortMarkerOfs.x; }
	int				GetArrowPosY() const { return m_sortMarkerOfs.y; }

	void			SetArrowAlignment( WgOrientation alignment );
	WgOrientation	GetArrowAlignment() const { return m_sortMarkerAlignment; }

	void			SetCellPadding( WgBorders padding );
	WgBorders		GetCellPadding() const	{ return m_cellPadding; }

	void			SetAutoScaleHeaders(bool autoScaleHeaders);
	bool			GetAutoScaleHeaders() const { return m_bAutoScaleHeader; }
	float			CalcHeaderScaleFactor() const;

	int				GetHeaderHeight() const	{ return (m_bShowHeader && m_pHeaderGfx) ? m_pHeaderGfx->Height(0) : 0; }

	void			SetEmptyRowHeight( int height );
	int				GetEmptyRowHeight() const { return m_emptyRowHeight; }

	void			SetRowBlocks( WgBlocksetPtr * pRowBlocks, int nRowSource );
	int				GetRowBlockCount() const { return m_nRowBlocks; }
	WgBlocksetPtr* 	GetRowBlocks( ) { return m_pRowBlocks; }
	void			DeleteRowBlocks();

	void			SetRowColors( WgColor * pRowColors, int nRowColors );
	int				GetRowColorCount() const { return m_nRowColors; }
	WgColor*		GetRowColors( ) { return m_pRowColors; }
	void			DeleteRowColors();

	int				AddColumn( const WgCharSeq& text, int pixelwidth, WgOrientation headerAlign = WG_WEST, int(*fpCompare)(WgGizmo*,WgGizmo*) = 0, bool bInitialAscend = true, bool bEnabled = true, Sint64 id = 0, WgGizmo * pDefaultGizmo = 0 );
	void			DeleteColumns();
//	bool	SetColumnWidth( Uint32 column, Uint32 pixelwidth );
//	bool	SetColumnVisible( Uint32 column, bool bVisible );

	int				NbColumns() const	{ return m_nColumns; }
	WgTableColumn* GetColumn(int index) const { return (index<m_nColumns)?&m_pColumns[index]:0; }
	WgTableColumn* FindColumn(Sint64 id) const;

	bool			SortRows( int column, bool bAscend, int prio = 0 );
	bool			SetClickSortPrio( int prio );
	int				GetClickSortPrio() const { return m_clickSortPrio; }

	int				GetSortColumns() const { return c_nSortColumns; }
	bool			GetSortColumn( int order, int& columnIndex, bool& bAscend ) const;

	int				AddRow( WgTableRow* pRow );
	int				InsertRow( WgTableRow* pRow, int pos );
	void			InsertRowSorted( WgTableRow* pRow );
	int				GetRowNb( WgTableRow* pRow ) const;
	WgTableRow*	GetRow( int pos );
	WgTableRow*	ReleaseRow( int pos );
	bool			ReleaseRow( WgTableRow* pRow );
	void			ReleaseAllRows();
	bool			DeleteRow( int pos );
	bool			DeleteRow( WgTableRow* pRow );
	void			DeleteAllRows();
	WgTableRow*	FindRow( int id );
	void			SetSelectedRowColor( WgColor c );
	void			SetSelectedRowBg( WgBlocksetPtr pBlock );
	bool			HasSelectedRowBg() const { if( m_pSelectedRowGfx ) return true; return false; }
	WgBlocksetPtr	GetSelectedRowBg() const { return m_pSelectedRowGfx; }
	WgColor			GetSelectedRowColor() const { return m_selectedRowColor; }

	WgTableRow*	FirstRow() const { return m_rows.First(); }
	WgTableRow*	LastRow() const { return m_rows.Last(); }
	int				NbRows() const { return m_nRows; }
	int				CompareRows(WgTableRow* p1, WgTableRow* p2) const;

	// Overloaded from WgGizmoParent

	bool			DeleteChild( WgGizmo * pGizmo );
	WgGizmo *		ReleaseChild( WgGizmo * pGizmo );

	bool			DeleteAllChildren();
	bool			ReleaseAllChildren();

//	void	ScrollIntoView( WgTableRow* pRow );

	void			SetAutoScrollMarked( bool bAutoScroll )			{ m_bAutoScrollMarked = bAutoScroll; }
	bool			GetAutoScrollMarked() const						{ return m_bAutoScrollMarked; }

	void			ShowHeader( bool bShow );
	bool			GetShowHeader() const { return m_bShowHeader; }


	WgTableHook*	FirstHook() const;
	WgTableHook*	LastHook() const;

	// Overloaded from WgGizmo

	int				HeightForWidth( int width ) const;
	int				WidthForHeight( int height ) const;

	WgSize			DefaultSize() const;

	// Overloaded from container

	WgGizmo * 		FindGizmo( const WgCoord& ofs, WgSearchMode mode );

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

	void			SetSelectMode( SelectMode mode );
	SelectMode		GetSelectMode() const { return m_selectMode; }

	WgRect			GetCellGeo( int row, int column );
	WgGizmo *		GetCellContent( int row, int column );

protected:

	void			_onCloneContent( const WgGizmo * _pOrg );
	void			_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip, Uint8 _layer );
	void			_onNewSize( const WgSize& size );
//	bool			_onAlphaTest( const WgCoord& ofs );		//TODO: Implement for performance!

private:
	WgHook*			_firstHook() const { return FirstHook(); }
	WgHook*			_lastHook() const { return LastHook(); }

	WgHook* 		_firstHookWithGeo( WgRect& geo ) const;
	WgHook* 		_nextHookWithGeo( WgRect& geo, WgHook * pHook ) const;

	WgHook* 		_lastHookWithGeo( WgRect& geo ) const;
	WgHook* 		_prevHookWithGeo( WgRect& geo, WgHook * pHook ) const;

	//

	void			_refreshRows();
	void			_rowModified( WgTableRow* pRow, int widthDiff , int heightDiff );
	void			_rowMarkChanged( WgTableRow* pRow, bool bMarked );
	WgTableRow*	_getMarkedRow( int x, int y );

	void			_connectRow( WgTableRow* pRow, WgTableRow* pPlaceBefore = 0 );
	void			_disconnectRow( WgTableRow* pRow );
	WgTableRow* 	_findRowInsertSpot( WgTableRow* pFirst, WgTableRow* pLast, WgTableRow* pRow, Uint32 nRows );

	int				_getMarkedColumn( int x, int& saveXOfs );
	int				_getMarkedRow( int y, WgTableRow*& pSaveRow, int& saveYOfs );

	void			_updateContentSize();
	void			_updateColumnWidths();
	void			_updateMarkedRowColumn( int row, int column );

	void			_setContentSize( const WgSize& size );

	struct SortInfo
	{
		SortInfo() { column = 0xFFFF; bAscend = true; }

		Uint16		column;
		bool		bAscend;
	};

	WgChain<WgTableRow>	m_rows;
	int						m_nRows;

	WgSize			m_gizmoSize;					// Current size of this table, as decided by our parent.
	WgSize			m_contentSize;					// Size of the content of the table, considered best size for gizmo.

	SelectMode		m_selectMode;

	static const int c_nSortColumns = 4;
	SortInfo		m_sortStack[c_nSortColumns];			// 0xFFFF = none

	int				m_clickSortPrio;
	bool			m_bSortAscend;

	bool			m_bShowHeader;
	bool			m_bAutoScaleHeader;

	bool			m_bAutoScrollMarked;

	int				m_nColumns;
	WgTableColumn*	m_pColumns;

	int				m_nRowColors;
	WgColor*		m_pRowColors;

	int				m_nRowBlocks;
	WgBlocksetPtr*	m_pRowBlocks;

	int				m_emptyRowHeight;						// Set if empty rows should fill out the view.

	WgTextpropPtr	m_pHeaderProps;

	WgOrientation	m_sortMarkerAlignment;
	WgCoord8		m_sortMarkerOfs;

	WgBorders		m_cellPadding;

	WgBlocksetPtr	m_pAscendGfx;
	WgBlocksetPtr	m_pDescendGfx;

	int				m_lastSortColumn;
	bool			m_lastSortColumnAscendStatus;


	WgTableRow*	m_pLastMarkedRow;
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

	WgBlocksetPtr	m_pHeaderGfx;

	WgTableColumn*	_getHeaderColumnAt( const WgCoord& pos );
	WgTableColumn*	m_pMarkedHeader;						// Header currently marked by mouse


	WgBlocksetPtr	m_pSelectedRowGfx;
	WgColor			m_selectedRowColor;
};


//____ inline methods __________________________________________________________

int WgTableHook::ColumnNb() const
{
	return this - Row()->m_pCells;
}

WgGizmoTable * WgTableHook::Parent() const
{
	return m_pRow->m_pTable;
}

int WgTableColumn::Index() const
{
	return this - m_pTable->m_pColumns;
}


#endif // WG_GIZMO_TABLE_DOT_H
