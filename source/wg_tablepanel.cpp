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

#include	<string.h>
#include	<stdlib.h>

#include	<wg_tablepanel.h>
#include	<wg_eventhandler.h>
#include	<wg_text.h>
#include	<wg_font.h>
#include	<wg_util.h>
#include	<wg_gfxdevice.h>


static const char	c_gizmoType[] = {"TablePanel"};
static const char	c_hookType[] = {"TableHook"};


WgCoord WgTableHook::Pos() const
{
	return Geo();
}

WgSize WgTableHook::Size() const
{
	int w = 0;
	int h = Row()->Height();

	WgTableColumn* pColumn = Column();
	if( pColumn && pColumn->IsVisible() )							// Widgets in hidden columns should have 0 width.
			w = pColumn->RealWidth();

	return WgSize(w,h);
}

WgRect WgTableHook::Geo() const
{
	WgTablePanel * pTable = Row()->Table();

	if( pTable )
	{
		int row = pTable->GetRowNb( Row() );
		int column = ColumnNb();

		return pTable->GetCellGeo( row, column );
	}
	else
		return WgRect();
}

WgCoord WgTableHook::ScreenPos() const
{
	return Pos() + Row()->Table()->ScreenPos();
}

WgRect WgTableHook::ScreenGeo() const
{
	return Geo() + Row()->Table()->ScreenPos();
}


WgHook* WgTableHook::_prevHook() const
{
	return PrevInTable();
}

WgHook* WgTableHook::_nextHook() const
{
	return NextInTable();
}

WgGizmoContainer * WgTableHook::_parent() const
{
	return m_pRow->m_pTable;
}


WgTableHook * WgTableHook::PrevInTable() const
{
	WgTableHook * p = PrevInRow();

	if( !p )
	{
		WgTableRow* pRow = Row()->Prev();
		while( pRow && !p )
		{
			p = pRow->LastHook();
			pRow = pRow->Prev();
		}
	}

	return p;
}

WgTableHook * WgTableHook::NextInTable() const
{
	WgTableHook * p = NextInRow();

	if( !p )
	{
		WgTableRow* pRow = Row()->Next();
		while( pRow && !p )
		{
			p = pRow->FirstHook();
			pRow = pRow->Next();
		}
	}

	return p;
}

WgTableHook * WgTableHook::PrevInRow() const
{
	WgTableRow* pRow = Row();
	int col = ColumnNb()-1;

	while( col >= 0 )
	{
		if( pRow->m_pCells[col].Gizmo() )
			return &pRow->m_pCells[col];
		col--;
	}

	return 0;
}

WgTableHook * WgTableHook::NextInRow() const
{
	WgTableRow* pRow = Row();
	int col = ColumnNb()+1;

	while( col < pRow->m_nCells )
	{
		if( pRow->m_pCells[col].Gizmo() )
			return &pRow->m_pCells[col];
		col++;
	}

	return 0;
}


WgTableHook * WgTableHook::PrevInColumn() const
{
	WgTableRow* pRow = Row()->Prev();
	int col = ColumnNb();

	while( pRow )
	{
		if( pRow->m_nCells > col && pRow->m_pCells[col].Gizmo() )
			return &pRow->m_pCells[col];

		pRow = pRow->Prev();
	}

	return 0;
}

WgTableHook * WgTableHook::NextInColumn() const
{
	WgTableRow* pRow = Row()->Next();
	int col = ColumnNb();

	while( pRow )
	{
		if( pRow->m_nCells > col && pRow->m_pCells[col].Gizmo() )
			return &pRow->m_pCells[col];

		pRow = pRow->Next();
	}

	return 0;
}

WgTableColumn* WgTableHook::Column() const
{
	WgTableRow* pRow = Row();
	int col = this - pRow->m_pCells;

	if( !pRow->Table() )
		return 0;

	return pRow->Table()->GetColumn(col);
}


WgTableHook::WgTableHook( WgTableRow * pRow )
{
	m_pRow	 = pRow;
}

WgTableHook::~WgTableHook()
{
}

const char * WgTableHook::Type( void ) const
{
	return ClassType();
}

const char * WgTableHook::ClassType()
{
	return c_hookType;
}


void WgTableHook::_requestRender()
{
	if( !m_bVisible )
		return;

	WgTablePanel* pTable = Row()->Table();
	if( pTable )
		pTable->_requestRender( Geo() );
}

void WgTableHook::_requestRender( const WgRect& rect )
{
	if( !m_bVisible )
		return;

	WgTablePanel* pTable = Row()->Table();
	if( pTable )
		pTable->_requestRender( rect + Pos() );
}

void WgTableHook::_requestResize()
{
	//TODO: Recalculate height of row...
}


WgTableColumn::WgTableColumn()
{
	m_desiredWidth		= 4;
	m_realWidth			= m_desiredWidth;
	m_fpCompare			= 0;
	m_bVisible			= true;
	m_bEnabled			= true;
	m_bInitialAscend	= true;
	m_id				= 0;
	m_pTable			= 0;
	m_pText				= 0;
	m_pDefaultGizmo		= 0;
}



WgTableColumn::WgTableColumn( WgTablePanel * pOwner )
{
	m_desiredWidth		= 4;
	m_realWidth			= m_desiredWidth;
	m_fpCompare			= 0;
	m_bVisible			= true;
	m_bEnabled			= true;
	m_bInitialAscend	= true;
	m_id				= 0;
	m_pTable			= pOwner;
	m_pText				= 0;
	m_pDefaultGizmo		= 0;
}


WgTableColumn::WgTableColumn(const WgTableColumn& column)
{
	m_desiredWidth = column.m_desiredWidth;
	m_realWidth = m_desiredWidth;
	m_fpCompare = column.m_fpCompare;
	m_bVisible = column.m_bVisible;
	m_bEnabled = column.m_bEnabled;
	m_bInitialAscend = column.m_bInitialAscend;
	m_id = column.m_id;
	m_pTable = 0;
	m_pDefaultGizmo = column.m_pDefaultGizmo;			//TODO: WRONG!!!!!!!!!!!!!

	if(column.m_pText)
	{
		m_pText = new WgText();
		m_pText->SetWrap(false);
		m_pText->setHolder( this );
		m_pText->setProperties( column.m_pText->getProperties() );
		m_pText->setText( m_pText->getText() );
		m_pText->setAlignment( column.m_pText->alignment() );
		m_pText->setTintMode( column.m_pText->tintMode() );
		m_pText->setLineSpaceAdjustment( column.m_pText->lineSpaceAdjustment() );
		m_pText->SetAutoEllipsis( column.m_pText->IsAutoEllipsis() );

	}
	else
	{
		m_pText = 0;
	}
}


WgTableColumn::~WgTableColumn()
{
	if( m_pText )
		delete m_pText;
}


void WgTableColumn::SetWidth( int pixels )
{
	if( pixels < 4 )
		pixels = 4;

	if( m_desiredWidth != pixels )
	{
		int widthdiff = pixels - m_desiredWidth;
		m_desiredWidth = pixels;
		if( m_bVisible && m_pTable )
		{
			m_pTable->_setContentSize( WgSize(m_pTable->m_contentSize.w + widthdiff, m_pTable->m_contentSize.h) );
			m_pTable->_requestRender();
			m_pTable->_updateColumnWidths();
		}
	}

}

void WgTableColumn::SetVisible( bool bVisible )
{
	if( m_bVisible != bVisible )
	{
		m_bVisible = bVisible;

		int width;
		if( bVisible )
			width = m_pTable->m_contentSize.w + m_realWidth;
		else
			width = m_pTable->m_contentSize.w - m_realWidth;

		if( m_pTable )
		{
			m_pTable->_setContentSize( WgSize( width, m_pTable->m_contentSize.h) );
			m_pTable->_requestRender();
			m_pTable->_updateColumnWidths();
		}

	}
}


void WgTableColumn::SetEnabled(bool bEnabled)
{
	if( m_bEnabled != bEnabled )
	{
		m_bEnabled = bEnabled;
		if( m_bVisible && m_pTable )
			m_pTable->_requestRender();
	}
}


void WgTableColumn::SetSortFunction( fpGizmoCmp pFunc )
{
	m_fpCompare = pFunc;
}


void WgTableColumn::SetTextObj(WgText *text)
{
	m_pText = text;
}

const WgText* WgTableColumn::GetTextObj() const
{
	return m_pText;
}

WgText* WgTableColumn::_getTextObj()
{
	return m_pText;
}

void WgTableColumn::_textModified()
{
	if( m_bVisible && m_pTable )
		m_pTable->_requestRender();
}


WgTableHook* WgTableColumn::FirstHook() const
{
	WgTableRow* pRow = m_pTable->FirstRow();
	int column = Index();

	while( pRow )
	{
		WgTableHook* pHook = pRow->GetHook(column);
		if( pHook )
			return pHook;

		pRow = pRow->Next();
	}

	return 0;
}

WgTableHook* WgTableColumn::LastHook() const
{
	WgTableRow* pRow = m_pTable->LastRow();
	int column = Index();

	while( pRow )
	{
		WgTableHook* pHook = pRow->GetHook(column);
		if( pHook )
			return pHook;

		pRow = pRow->Prev();
	}

	return 0;
}

WgTableRow::WgTableRow( Sint64 id, int nCells )
{
	if( nCells < 0 )
		nCells = 0;

	m_nCells = 0;
	m_pCells = 0;

	if( nCells > 0 )
		_growCellsArray(nCells);

	m_id		= id;
	m_height	= 0;
	m_pTable	= 0;
	m_mode		= WG_MODE_NORMAL;
	m_bVisible	= true;
}

WgTableRow::~WgTableRow()
{
	free( m_pCells );		// This array was reserved with calloc() and not new[]!
}


void WgTableRow::SetGizmo( WgGizmo * pGizmo, int cell )
{
	if( m_nCells <= cell )
		_growCellsArray(cell+1);

	if( m_pCells[cell].Gizmo() )
		m_pCells[cell].~WgTableHook();

	new (&m_pCells[cell])WgTableHook(this);

	m_pCells[cell]._attachGizmo(pGizmo);

	int width = m_pTable->m_pColumns[cell].RealWidth();
	int height = pGizmo->HeightForWidth(width);

	m_pCells[cell].m_height = height;
	pGizmo->_onNewSize( WgSize(width, height) );

	if( height > m_height )
	{
		if( m_pTable )
			m_pTable->m_contentSize.h += height - m_height;

		m_height = height;
	}

	//TODO: Meddela table att rad har ändrats.
}

int WgTableRow::AddGizmo( WgGizmo * pGizmo )
{
	// Look for the last empty cell

	int cell = m_nCells;

	while( cell > 0 && m_pCells[cell-1].Gizmo() == 0 )
		cell--;

	//

	SetGizmo( pGizmo, cell );
	return cell;
}

WgGizmo * WgTableRow::ReleaseGizmo( int cell )
{
	if( m_nCells <= cell || !m_pCells[cell].Gizmo() )
		return false;

	WgGizmo * pReleased = m_pCells[cell].Gizmo();
	m_pCells[cell]._attachGizmo(0);
	m_pCells[cell].~WgTableHook();
	new (&m_pCells[cell])WgTableHook(this);		// Is this really necessary?

	return pReleased;
}

bool WgTableRow::DeleteGizmo( int cell )
{
	if( m_nCells <= cell || !m_pCells[cell].Gizmo() )
		return false;

	m_pCells[cell].~WgTableHook();
	new (&m_pCells[cell])WgTableHook(this);

	return true;
}



void WgTableRow::_growCellsArray( int nCells )
{
	// Allocate and copy content

	WgTableHook * p = (WgTableHook*) calloc( nCells, sizeof(WgTableHook) );

	if( m_nCells > 0 )
		memcpy( p, m_pCells, m_nCells*sizeof(WgTableHook) );

	// Go through gizmos and update their hook pointers

	for( int i = 0 ; i < m_nCells ; i++ )
			p[i]._relinkGizmo();

	// Delte old array and set pointers

	if( m_pCells )
		free( m_pCells );

	m_pCells = p;
	m_nCells = nCells;

}


bool WgTableRow::Select()
{
	if( m_mode == WG_MODE_DISABLED )
		return false;

	if( m_mode != WG_MODE_SELECTED )
	{
		m_mode = WG_MODE_SELECTED;

		for( int i = 0 ; i < m_nCells ; i++ )
			m_pCells[i].Gizmo()->SetSelected();
	}
	return true;
}


void WgTableRow::Unselect()
{
	if( m_mode != WG_MODE_NORMAL && m_mode != WG_MODE_DISABLED )
	{
		m_mode = WG_MODE_NORMAL;

		for( int i = 0 ; i < m_nCells ; i++ )
			m_pCells[i].Gizmo()->SetNormal();
	}
}



WgTableHook* WgTableRow::FirstHook() const
{
	for( int i = 0 ; i < m_nCells ; i++ )
		if( m_pCells[i].Gizmo() )
			return &m_pCells[i];

	return 0;
}

WgTableHook* WgTableRow::LastHook() const
{
	for( int i = m_nCells-1 ; i >= 0 ; i-- )
		if( m_pCells[i].Gizmo() )
			return &m_pCells[i];

	return 0;
}

WgTableHook* WgTableRow::GetHook( int cell )
{
	if( cell < m_nCells && m_pCells[cell].Gizmo() )
		return &m_pCells[cell];

	return 0;
}

WgGizmo* WgTableRow::GetGizmo( int cell )
{
	if( cell < m_nCells && m_pCells[cell].Gizmo() )
		return m_pCells[cell].Gizmo();

	return 0;
}



//____ Constructor ____________________________________________________________

WgTablePanel::WgTablePanel()
{
	m_bShowHeader 		= true;
	m_bAutoScaleHeader	= false;
	m_emptyRowHeight	= 0;

	m_nColumns			= 0;
	m_pColumns			= 0;

	m_nRowColors		= 0;
	m_pRowColors		= 0;

	m_nRowBlocks		= 0;
	m_pRowBlocks		= 0;


	m_sortMarkerAlignment = WG_EAST;
	m_sortMarkerOfs.x	= 0;
	m_sortMarkerOfs.y	= 0;
	m_clickSortPrio		= 0;

	m_lastClickedRow = -1;
	m_lastClickedColumn = -1;

	m_markedRow = -1;
	m_markedColumn = -1;

	m_bSortAscend = true;
	m_contentSize = WgSize(0,0);

	m_bAutoScrollMarked	= false;

	m_lastSortColumn				= 0;
	m_lastSortColumnAscendStatus	= true;

	m_pMarkedHeader = NULL;

	m_selectMode = SELECT_SINGLE_ROW;
	m_nRows = 0;

	m_selectedRowColor = WgColor(128,128,128,85);
}

//____ Destructor _____________________________________________________________

WgTablePanel::~WgTablePanel( void )
{
	DeleteColumns();
	DeleteRowColors();
	DeleteRowBlocks();

	delete [] m_pColumns;
	m_pColumns = 0;							// So that refresh methods called when underlying items are deleted knows...
}


//____ Type() _________________________________________________________________

const char * WgTablePanel::Type( void ) const
{
	return GetClass();
}

const char * WgTablePanel::GetClass( void )
{
	return c_gizmoType;
}

//____ SetHeaderSource() ______________________________________________________

bool WgTablePanel::SetHeaderSource( const WgBlocksetPtr& pHeader )
{
	m_pHeaderGfx		= pHeader;

	if( m_bShowHeader )
		_requestRender();

	return true;
}

//____ SetArrowSource() _______________________________________________________

void WgTablePanel::SetArrowSource( const WgBlocksetPtr& pAscend, const WgBlocksetPtr& pDescend )
{
	m_pAscendGfx	= pAscend;
	m_pDescendGfx	= pDescend;

	if( m_bShowHeader )
		_requestRender();
}

//____ SetArrowPos() __________________________________________________________

void WgTablePanel::SetArrowPos( const WgOrientation alignment, int xOfs, int yOfs )
{
	m_sortMarkerAlignment = alignment;
	m_sortMarkerOfs.x	= xOfs;
	m_sortMarkerOfs.y	= yOfs;

	if( m_bShowHeader )
		_requestRender();
}

//____ SetArrowPos() __________________________________________________________

void WgTablePanel::SetArrowPos( int xOfs, int yOfs )
{
	m_sortMarkerOfs.x	= xOfs;
	m_sortMarkerOfs.y	= yOfs;

	if( m_bShowHeader )
		_requestRender();
}

//____ SetArrowAlignment() __________________________________________________________

void WgTablePanel::SetArrowAlignment( WgOrientation alignment )
{
	m_sortMarkerAlignment = alignment;

	if( m_bShowHeader )
		_requestRender();
}

//____ SetCellPadding() _______________________________________________________

void WgTablePanel::SetCellPadding( WgBorders padding )
{
	m_cellPadding = padding;

	//TODO: This affects geometry of gizmos in cells!!!

	_requestRender();
}

//____ SetEmptyRowHeight() ____________________________________________________

void WgTablePanel::SetEmptyRowHeight( int height )
{
	if( height != m_emptyRowHeight )
	{
		m_emptyRowHeight = height;
		_requestRender();
	}
}

//____ SetRowColors() _________________________________________________________

void WgTablePanel::SetRowColors( WgColor * pRowColors, int nRowColors )
{
	if( m_pRowColors )
		DeleteRowColors();

	if( nRowColors > 0 && pRowColors )
	{
		m_nRowColors = nRowColors;
		m_pRowColors = new WgColor[ m_nRowColors ];

		memcpy( m_pRowColors, pRowColors, sizeof( WgColor ) * m_nRowColors );
	}

	_requestRender();
}

//____ DeleteRowColors() ______________________________________________________
void WgTablePanel::DeleteRowColors()
{
	if( m_pRowColors )
	{
		delete [] m_pRowColors;
		m_pRowColors = 0;
		m_nRowColors = 0;
	}
}

//____ SetRowBlocks() _________________________________________________________

void WgTablePanel::SetRowBlocks( WgBlocksetPtr * pRowBlocks, int nRowBlocks )
{
	if( m_pRowBlocks )
		DeleteRowBlocks();

	if( nRowBlocks > 0 && pRowBlocks )
	{
		m_nRowBlocks = nRowBlocks;
		m_pRowBlocks = new WgBlocksetPtr[ m_nRowBlocks ];

		for( int i = 0 ; i < nRowBlocks ; i++ )
			m_pRowBlocks[i] = pRowBlocks[i];
	}

	_requestRender();
}

//____ DeleteRowBlocks() ______________________________________________________
void WgTablePanel::DeleteRowBlocks()
{
	if( m_pRowBlocks )
	{
		delete [] m_pRowBlocks;
		m_pRowBlocks = 0;
		m_nRowBlocks = 0;
	}
}


//____ DeleteColumns() ____________________________________________________________

void WgTablePanel::DeleteColumns()
{
	_updateMarkedRowColumn(-1, -1);

	delete [] m_pColumns;
	m_pColumns = 0;
	m_nColumns = 0;
}

//____ AddColumn() ____________________________________________________________

//TODO: Add default gizmo to parameters

int WgTablePanel::AddColumn( const WgCharSeq& text, int pixelwidth, WgOrientation headerAlign, int(*fpCompare)(WgGizmo *,WgGizmo *),
							 bool bInitialAscend, bool bEnabled, Sint64 id, WgGizmo * pDefaultGizmo )
{
	WgTableColumn * pCol = new WgTableColumn[m_nColumns+1];

	for( int i = 0 ; i < m_nColumns ; i++ )
	{
		pCol[i] = m_pColumns[i];
		m_pColumns[i].m_pText = 0;			// To avoid deletion of text object further down.
	}
	pCol[m_nColumns].m_desiredWidth = pixelwidth;

	WgText * p = new WgText( text );
	p->SetWrap(false);
	pCol[m_nColumns].SetTextObj(p);

	pCol[m_nColumns]._getTextObj()->setAlignment(headerAlign);
	pCol[m_nColumns].m_fpCompare = fpCompare;
	pCol[m_nColumns].m_bVisible = true;
	pCol[m_nColumns].m_bEnabled = bEnabled;
	pCol[m_nColumns].m_bInitialAscend = bInitialAscend;
	pCol[m_nColumns].m_id = id;
	pCol[m_nColumns].m_pDefaultGizmo = pDefaultGizmo;


	if( m_pColumns )
		delete [] m_pColumns;
	m_pColumns = pCol;
	m_nColumns++;

	_setContentSize( WgSize(m_contentSize.w + pixelwidth, m_contentSize.h) );
	_updateColumnWidths();
	_requestRender();
	return m_nColumns;
}

//____ _updateColumnWidths() ___________________________________________________

void WgTablePanel::_updateColumnWidths()
{
	// Calculate scale factor

	float scaleFactor = 1.f;
	bool  bAnyColumnWidthChanged = false;

	if( m_bAutoScaleHeader && m_contentSize.w != 0 )
		scaleFactor = (float)m_gizmoSize.w / (float)m_contentSize.w;

	// Set m_realWidth for each column, flag if changed.

	for( int i = 0 ; i < m_nColumns ; i++ )
	{
		if( m_pColumns[i].m_bVisible )
		{
			int newWidth = (int) (m_pColumns[i].m_desiredWidth * scaleFactor);
			if( m_pColumns[i].m_realWidth != newWidth )
			{
				m_pColumns[i].m_bWidthChanged = true;
				m_pColumns[i].m_realWidth = newWidth;
			}

		}
		else
			m_pColumns[i].m_realWidth = 0;
	}

	// Go through rows, notify gizmos of new width, update row height and content height.

	WgTableRow* pRow = m_rows.First();
	while( pRow )
	{
		bool	bRecalcHeight = false;

		for( int n = 0 ; n < pRow->m_nCells ; n++ )
		{
			WgTableHook * pHook = &pRow->m_pCells[n];
			if( m_pColumns[n].m_bWidthChanged && pHook->Gizmo() )
			{
				int w = m_pColumns[n].m_realWidth;
				int h = pHook->Gizmo()->HeightForWidth(w);
				pHook->Gizmo()->_onNewSize( WgSize( w, h ) );

				if( h != pHook->m_height )
				{
					pHook->m_height = h;
					bRecalcHeight = true;
				}

			}
		}

		if( bRecalcHeight )
		{
			int newHeight = 0;
			for( int n = 0 ; n < pRow->m_nCells ; n++ )
			{
				if( pRow->m_pCells[n].m_height > newHeight )
					newHeight = pRow->m_pCells[n].m_height;
			}

			if( newHeight != pRow->m_height )
			{
				m_contentSize.h += newHeight - pRow->m_height;
				pRow->m_height = newHeight;
			}
		}

		pRow = pRow->Next();
	}

	// Clear the width changed flags

	for( int i = 0 ; i < m_nColumns ; i++ )
		m_pColumns[i].m_bWidthChanged = false;

}


//____ FindColumn() ___________________________________________________________

WgTableColumn* WgTablePanel::FindColumn(Sint64 id) const
{
	for( int i = 0 ; i < m_nColumns ; i++ )
		if( m_pColumns[i].m_id == id )
			return &m_pColumns[i];

	return 0;
}


//____ GetSortColumn() _____________________________________________________

bool WgTablePanel::GetSortColumn( int order, int& columnIndex, bool& bAscend ) const
{
	if( order >= c_nSortColumns )
		return false;

	if( 0xFFFF == m_sortStack[ order ].column )
		return false;

	columnIndex = m_sortStack[ order ].column;
	bAscend = m_sortStack[ order ].bAscend;

	return true;
}

//____ FirstHook() ____________________________________________________________

WgTableHook* WgTablePanel::FirstHook() const
{
	WgTableRow* pRow = FirstRow();

	while( pRow )
	{
		WgTableHook * pHook = pRow->FirstHook();
		if( pHook )
			return pHook;
		pRow = pRow->Next();
	}

	return 0;
}

//____ LastHook() _____________________________________________________________

WgTableHook* WgTablePanel::LastHook() const
{
	WgTableRow* pRow = LastRow();

	while( pRow )
	{
		WgTableHook * pHook = pRow->LastHook();
		if( pHook )
			return pHook;
		pRow = pRow->Prev();
	}

	return 0;
}

//____ GetCellContent() ________________________________________________________

WgGizmo * WgTablePanel::GetCellContent( int row, int column )
{
	WgTableRow * pRow = m_rows.Get(row);
	if( pRow )
		return pRow->GetGizmo(column);
	else
		return 0;
}

//____ GetCellGeo() ___________________________________________________________

WgRect WgTablePanel::GetCellGeo( int row, int column )
{
	if( column >= m_nColumns )
		return WgRect();							// Invalid column;

	WgRect	r( 0, 0, 0, 0 );

	// Adjust for header

	if( m_bShowHeader && m_pHeaderGfx )
		r.y += m_pHeaderGfx->Height();


	WgTableRow * pRow = m_rows.First();

	// Forward to our row
	for( int i = 0 ; i < row ; i++ )
	{
		if( !pRow )
			return WgRect();							// Invalid row.

		if( pRow->IsVisible() )
			r.y += pRow->Height() + m_cellPadding.Height();

		pRow = pRow->Next();
	}
	r.y += m_cellPadding.top;


	// Set cell height
	if( pRow->IsVisible() )
		r.h = pRow->Height();
	else
		r.h = 0;

	// Go through columns, determine x-pos.

	for( int i = 0 ; i < column ; i++ )
	{
		if( m_pColumns[i].m_bVisible )
			r.x += m_pColumns[i].m_realWidth;
	}

	// Determine width

	r.w = m_pColumns[column].m_realWidth;
	if( column == m_nColumns-1 && r.x + r.w < m_gizmoSize.w )
		r.w =  m_gizmoSize.w - r.x;		// Last column stretches to end of tableview...


	// Apply padding

	r.x += m_cellPadding.left;
	r.w -= m_cellPadding.Width();

	if( r.w < 0 )
		r.w = 0;

	return r;
}

//____ DeleteChild() __________________________________________________________

bool WgTablePanel::DeleteChild( WgGizmo * pGizmo )
{
	if( !pGizmo || pGizmo->Parent() != this )
		return false;

	WgTableHook * pHook = static_cast<WgTableHook*>(pGizmo->Hook());
	WgTableRow * pRow = pHook->Row();

	return pRow->DeleteGizmo( pHook->ColumnNb() );
}

//____ ReleaseChild() _________________________________________________________

WgGizmo * WgTablePanel::ReleaseChild( WgGizmo * pGizmo )
{
	if( !pGizmo || pGizmo->Parent() != this )
		return 0;

	WgTableHook * pHook = static_cast<WgTableHook*>(pGizmo->Hook());
	WgTableRow * pRow = pHook->Row();

	return pRow->ReleaseGizmo( pHook->ColumnNb() );
}

//____ DeleteAllChildren() ______________________________________________________

bool WgTablePanel::DeleteAllChildren()
{
	//TODO: Implement

	return false;
}

//____ ReleaseAllChildren() _____________________________________________________

bool WgTablePanel::ReleaseAllChildren()
{
	//TODO: Implement

	return false;
}



//____ AddRow() _______________________________________________________________

int WgTablePanel::AddRow( WgTableRow* pRow )
{
	int index = m_nRows;
	_connectRow(pRow, 0);
	return index;
}


//____ InsertRow() ____________________________________________________________

int WgTablePanel::InsertRow( WgTableRow* pRow, int pos )
{
	WgTableRow* p = m_rows.Get(pos);

	int index = p?pos:m_nRows;
	_connectRow( pRow, p );
	return index;
}

//____ InsertRowSorted() ______________________________________________________

void WgTablePanel::InsertRowSorted( WgTableRow* pRow )
{
	if( m_nRows == 0 )
		_connectRow( pRow, 0 );
	else
	{
		WgTableRow * pFirst = m_rows.First();
		WgTableRow * pLast = m_rows.Last();

		int diffFirst = CompareRows( pRow, pFirst );
		int diffLast = CompareRows( pRow, pLast );

		if( !m_bSortAscend )
		{
			diffFirst = -diffFirst;
			diffLast = -diffLast;
		}

		if( diffFirst < 0 )
			_connectRow( pRow, pFirst );
		else if( diffLast >= 0 )
			_connectRow( pRow, 0 );		// If inserted row is equal to last row it should be placed last.
		else
			_connectRow( pRow, _findRowInsertSpot( pFirst, pLast, pRow, m_nRows ) );
	}
}


//____ _findRowInsertSpot() ____________________________________________________

WgTableRow* WgTablePanel::_findRowInsertSpot( WgTableRow* pFirst, WgTableRow* pLast, WgTableRow* pRow, Uint32 nRows )
{
	assert( nRows >= 2 && pFirst != pLast );

	if( nRows <= 2 )				// Should never be less than two but we handle it as a failsafe
									// in case we screw up some calling code in the future...
	{								// Only two items means that we have found our position for insertion.
		return pLast;
	}
	else
	{
		// Step to middle item

		WgTableRow* pMiddle = pFirst;
		int steps = nRows/2;
		for( int i = 0 ; i < steps ; i++ )
			pMiddle = pMiddle->Next();

		// Compare against middle item

		bool bUp = false;
		if( CompareRows( pRow, pMiddle ) < 0 )
			bUp = true;

		if( !m_bSortAscend )
			bUp = !bUp;

		// subdivide and recurse

		if ( bUp )
			return _findRowInsertSpot( pFirst, pMiddle, pRow, steps+1);
		else
			return _findRowInsertSpot( pMiddle, pLast, pRow, nRows-steps);
	}

}



//____ _connectRow() _____________________________________________________________

void WgTablePanel::_connectRow( WgTableRow* pRow, WgTableRow* pPlaceBefore )
{
	m_nRows++;
	pRow->m_pTable = this;

	if( pPlaceBefore )
		pRow->_moveBefore(pPlaceBefore);
	else
		m_rows.PushBack(pRow);

	m_contentSize.h += pRow->Height();

	_requestRender();			//TODO: Just request render on this row and following.
	_requestResize();
}

//____ _disconnectRow() ___________________________________________________________

void WgTablePanel::_disconnectRow( WgTableRow* pRow )
{
	m_nRows--;
	pRow->m_pTable = 0;
	pRow->_disconnect();

	m_contentSize.h -= pRow->Height();

	_requestRender();			//TODO: Just request render on this row and following.
	_requestResize();
}


//____ GetRowNb() _____________________________________________________________

int WgTablePanel::GetRowNb( WgTableRow* pRow ) const
{
	if( m_rows.IsMemberOf( pRow ) )
		return pRow->_index();
	else
		return -1;
}

//____ GetRow() _______________________________________________________________

WgTableRow* WgTablePanel::GetRow( int pos )
{
	return m_rows.Get(pos);
}

//____ FindRow() ______________________________________________________________

WgTableRow* WgTablePanel::FindRow( int id )
{
	WgTableRow* pRow = m_rows.First();

	while( pRow )
	{
		if( pRow->Id() == id )
			return pRow;

		pRow = pRow->Next();
	}
	return 0;
}

//____ SetSelectedRowColor() __________________________________________________

void WgTablePanel::SetSelectedRowColor( WgColor c )
{
	if( c != m_selectedRowColor )
	{
		m_selectedRowColor = c;
		_requestRender();
	}
}

//____ SetSelectedRowBg() _________________________________________________

void WgTablePanel::SetSelectedRowBg( WgBlocksetPtr pBlock )
{
	if( pBlock != m_pSelectedRowGfx )
	{
		m_pSelectedRowGfx = pBlock;
		_requestRender();							//TODO: More optimized handling...
	}
}


//____ ReleaseRow() ____________________________________________________________

WgTableRow * WgTablePanel::ReleaseRow( int pos )
{
	if( pos == m_markedRow )
		_updateMarkedRowColumn(-1,m_markedColumn);

	WgTableRow* pRow = m_rows.Get(pos);
	if( pRow )
		_disconnectRow(pRow);
	return pRow;
}

bool WgTablePanel::ReleaseRow( WgTableRow * pRow )
{
	if( GetRowNb(pRow) == m_markedRow )
		_updateMarkedRowColumn(-1,m_markedColumn);

	if( pRow && m_rows.IsMemberOf(pRow) )
	{
		_disconnectRow(pRow);
		return true;
	}
	return false;
}

//____ ReleaseAllRows() ________________________________________________________

void WgTablePanel::ReleaseAllRows()
{
	_updateMarkedRowColumn(-1,m_markedColumn);

	while( m_rows.First() )
		_disconnectRow( m_rows.First() );
}

//____ DeleteRow() ____________________________________________________________

bool WgTablePanel::DeleteRow( int pos )
{
	if( pos == m_markedRow )
		_updateMarkedRowColumn(-1,m_markedColumn);


	WgTableRow* pRow = m_rows.Get(pos);
	if( pRow )
	{
		_disconnectRow(pRow);
		delete pRow;
		return true;
	}
	return false;
}

bool WgTablePanel::DeleteRow( WgTableRow * pRow )
{
	if( GetRowNb(pRow) == m_markedRow )
		_updateMarkedRowColumn(-1,m_markedColumn);

	if( pRow && m_rows.IsMemberOf(pRow) )
	{
		_disconnectRow(pRow);
		delete pRow;
		return true;
	}
	return false;
}

//____ DeleteAllRows() ________________________________________________________

void WgTablePanel::DeleteAllRows()
{
	_updateMarkedRowColumn(-1,m_markedColumn);

	m_rows.Clear();
	m_nRows = 0;

	_updateContentSize();
	_requestRender();
	return;
}


//____ SetClickSortPrio() _____________________________________________________

bool WgTablePanel::SetClickSortPrio( int prio )
{
	if( prio >= c_nSortColumns )
		return false;

	m_clickSortPrio = prio;
	return true;
}


//____ SortRows() _____________________________________________________________

bool WgTablePanel::SortRows( int column, bool bAscend, int prio )
{
	if( column >= m_nColumns || prio > c_nSortColumns )
		return false;


	int i = 0;
	while( i < c_nSortColumns-1 )
	{
		if( m_sortStack[i].column == column )
			break;
		i++;
	}

	while( i > prio )
	{
		m_sortStack[i] = m_sortStack[i-1];
		i--;
	}

	m_sortStack[i].column			= column;
	m_sortStack[i].bAscend			= bAscend;

	if( prio == m_clickSortPrio )
	{
		m_lastSortColumn				= column;
		m_lastSortColumnAscendStatus	= bAscend;
	}

	// From old SortItems()...

	WgTableRow * pRow = m_rows.First();
	int nRows = 1;

	while( pRow )
	{
		WgTableRow * pNext = pRow->Next();

		if( nRows >= 2 )
		{
			WgTableRow* pFirst = m_rows.First();
			WgTableRow* pLast = pRow;

			bool bFirst = false;
			if( CompareRows( pRow, pFirst ) < 0 )
				bFirst = true;

			if( !m_bSortAscend )
				bFirst = !bFirst;

			if( bFirst )
				m_rows.PushFront( pRow );
			else
				pRow->_moveBefore(_findRowInsertSpot( pFirst, pLast, pRow, m_nRows ));
		}

		nRows++;
		pRow = pNext;
	}

	_requestRender();
	return true;
}

//____ CompareRows() __________________________________________________________

int WgTablePanel::CompareRows( WgTableRow* pRow1, WgTableRow* pRow2 ) const
{
	for( int n = 0 ; n < c_nSortColumns ; n++ )
	{
		int col = m_sortStack[n].column;

		if( col < m_nColumns && m_pColumns[col].m_fpCompare != 0 )
		{
			WgGizmo* p1 = 0;
			WgGizmo* p2 = 0;
			int diff;

			if( pRow1->m_nCells >= col )
				p1 = pRow1->m_pCells[col].Gizmo();

			if( pRow2->m_nCells >= col )
				p2 = pRow2->m_pCells[col].Gizmo();

			if( p1 == p2 )
				diff = 0;
			else if( p1 == 0 )
				diff = 1;
			else if( p2 == 0 )
				diff = -1;
			else
				diff = m_pColumns[col].m_fpCompare( p1, p2 );

			if( diff != 0 )
			{
				if( !m_sortStack[n].bAscend )
					diff = -diff;

				return diff;
			}
		}
	}

	return 0;

//	return m_pColumns[m_sortColumn].fpCompare( ((WgTableRow*)pItem1)->GetItem(m_sortColumn), ((WgTableRow*)pItem2)->GetItem(m_sortColumn) );
}


//____ SetHeaderTextprop() ____________________________________________________

bool WgTablePanel::SetHeaderTextprop( const WgTextpropPtr& pProp )
{
	m_pHeaderProps = pProp;
	_requestRender();
	return true;
}


//____ ShowHeader() ___________________________________________________________

void WgTablePanel::ShowHeader( bool bShow )
{
	if( bShow != m_bShowHeader )
	{
		m_bShowHeader = bShow;

		int newHeight = m_contentSize.h;

		if( m_pHeaderGfx )
		{
			if( bShow )
				newHeight += m_pHeaderGfx->Height();
			else
				newHeight -= m_pHeaderGfx->Height();
		}

		_setContentSize( WgSize( m_contentSize.w, newHeight ) );
		_requestRender();
	}
}

//____ _setContentSize() ____________________________________________________________

void WgTablePanel::_setContentSize( const WgSize& size )
{
	if( m_contentSize != size )
	{
		m_contentSize = size;
		_requestResize();
	}
}


//____ _updateContentSize() _________________________________________________________

void WgTablePanel::_updateContentSize()
{
	WgSize size;

	// Calc height

	WgTableRow * p = m_rows.First();

	while( p )
	{
		if( p->IsVisible() )
			size.h += p->Height() + m_cellPadding.Height();
		p = p->Next();
	}

	// Calc width

	for( int i = 0 ; i < m_nColumns ; i++ )
	{
		if( m_pColumns[i].m_bVisible )
			size.w += m_pColumns[i].m_realWidth;
	}

	// Possibly add header to height

	if( m_bShowHeader && m_pHeaderGfx )
		size.h += m_pHeaderGfx->Height();

	// Set size and request render

	_setContentSize( size );
}

//____ _rowModified() _________________________________________________________

void WgTablePanel::_rowModified( WgTableRow * pRow, int widthDiff , int heightDiff )
{
	//TODO: More specialized and optimized handling.

	_updateContentSize();
	_requestRender();
}

//____ _rowMarkChanged() ________________________________________________

void WgTablePanel::_rowMarkChanged( WgTableRow * pRow, bool bMarked )
{
/*	if( m_bAutoScrollMarked )
	{
		if( bMarked )		// make sure marked item is in view
			ScrollIntoView( pRow );
	}
*/
}

/*
//____ ScrollIntoView() ________________________________________________
void WgTablePanel::ScrollIntoView( WgTableRow* pRow )
{
	if( !pRow || pRow->IsHidden() )
		return;

	// calc y pos of this row
	int rowPosY = 0;

	WgTableRow * p = m_rows.getFirst();
	while( p && p != pRow )
	{
		if(p->IsVisible())
			rowPosY += p->Height() + m_cellPaddingY*2;
		p = p->getNext();
	}

	if( !p )
		return;		// row not found


	int viewOfsMin = ViewPixelOfsY();
	int viewOfsMax = ViewPixelOfsY() + ViewPixelLenY();

	if( rowPosY < viewOfsMin )							// is the row above the view?
		SetViewPixelOfsY( rowPosY );
	else if( GetHeaderHeight() + rowPosY + pRow->Height() > viewOfsMax )	// is the row below the view?
		SetViewPixelOfsY( GetHeaderHeight() + rowPosY + pRow->Height() - ViewPixelLenY() );
}
*/

//____ _onNewSize() ____________________________________________________________

void WgTablePanel::_onNewSize( const WgSize& newSize )
{
	bool bWidthChanged = (newSize.w!=m_gizmoSize.w)?true:false;

	m_gizmoSize = newSize;

	if( bWidthChanged )
		_updateColumnWidths();

	_requestRender();
}

//____ HeightForWidth() _______________________________________________________

int WgTablePanel::HeightForWidth( int width ) const
{
	//TODO: Implement, should recurse through lines and their gizmos checking height needed.

	return DefaultSize().h;		// No recommendation, for the moment
}

//____ WidthForHeight() _______________________________________________________

int WgTablePanel::WidthForHeight( int height ) const
{
	return m_contentSize.w;		// Width remains the same independent of height.
}

//____ DefaultSize() _____________________________________________________________

WgSize WgTablePanel::DefaultSize() const
{
	return m_contentSize;
}

//____ _getMarkedRow() _________________________________________________________

int WgTablePanel::_getMarkedRow( int y, WgTableRow*& pSaveRow, int& saveYOfs )
{
	pSaveRow	= 0;
	saveYOfs	= 0;
	int row		= 0;

	if( y < 0 )
		return -1;

	// Adjust for header.

	if( m_bShowHeader && m_pHeaderGfx )
	{
		if( y < m_pHeaderGfx->Height() )
			return -1;	// on header.
		y -=  m_pHeaderGfx->Height();
	}

	//

	WgTableRow* p	= m_rows.First();

	while( p )
	{
		if( p->IsVisible() )
		{
			if( y < (int) (p->Height() +  m_cellPadding.Height()) )
			{
				pSaveRow	= p;
				saveYOfs	= y;

				return row;
			}

			y -= p->Height() +  m_cellPadding.Height();
		}
		p = p->Next();
		row++;
	}
	return -1;									// Rows ended before mark-point.
}

//____ _getMarkedColumn() ______________________________________________________

int WgTablePanel::_getMarkedColumn( int x, int& saveXOfs )
{
	saveXOfs	= 0;

	//

	if( x < 0 )
		return -1;

	// Loop through columns to find out which one we are inside.

	for( int col = 0 ; col < m_nColumns ; col++ )
	{
		if( m_pColumns[col].m_bVisible )
		{
			int w = m_pColumns[col].m_realWidth;
			if( x < w )
			{
				saveXOfs	= x;
				return col;
			}

			x -= w;
		}
	}

	return -1;									// Columns ended before mark-point.
}


//____ SetAutoScaleHeaders() __________________________________________________
void WgTablePanel::SetAutoScaleHeaders(bool autoScaleHeaders)
{
	if(m_bAutoScaleHeader != autoScaleHeaders)
	{
		m_bAutoScaleHeader = autoScaleHeaders;
		_requestRender();
	}
}

//____ CalcHeaderScaleFactor() ________________________________________________
float WgTablePanel::CalcHeaderScaleFactor() const
{
	if(!m_bAutoScaleHeader)
		return 1.f;

	if(m_contentSize.w == 0)
		return 1.f;

	return (float)m_gizmoSize.w / (float)m_contentSize.w;
}

//____ FindGizmo() ____________________________________________________________

WgGizmo * WgTablePanel::FindGizmo( const WgCoord& ofs, WgSearchMode mode )
{
	// Check cell content

	int	xOfs;
	int yOfs;
	WgTableRow* pRow;

	int col = _getMarkedColumn( ofs.x, xOfs );
	int	row = _getMarkedRow( ofs.y, pRow, yOfs );

	WgGizmo * pGizmo = 0;

	if( col != -1 && row != -1 && pRow->GetHook(col)->IsVisible() )
	{
		if( xOfs < m_cellPadding.left || xOfs >= m_pColumns[col].m_realWidth - m_cellPadding.right ||
			yOfs < m_cellPadding.top || yOfs >= pRow->Height() - m_cellPadding.bottom )
		{
			pGizmo = pRow->GetGizmo(col);
			if( mode == WG_SEARCH_GEOMETRY || pGizmo->MarkTest( WgCoord( xOfs-m_cellPadding.left, yOfs-m_cellPadding.top) )  )
				return pGizmo;
		}
	}

	// Check ourselves

	if( mode == WG_SEARCH_GEOMETRY || MarkTest( ofs ) )
		return this;
	else
		return 0;
}


//____ _onRender() _____________________________________________________________
void WgTablePanel::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip, Uint8 _layer )
{
	WgRect	r = _canvas;
	WgRect	clipView = _clip;

	if( r.w < _window.w )
		r.w = _window.w;

	// Draw header (if any)

	if( m_bShowHeader && m_pHeaderGfx )
	{
		WgRect	headerArea( _canvas.x, _window.y, _canvas.w, m_pHeaderGfx->Height() );
		WgRect  r2 = headerArea;

		for( int i = 0 ; i < m_nColumns ; i++ )
		{
			if( !m_pColumns[i].m_bVisible )
				continue;

            // don't draw columns that are outside of the window
			if( r2.x >= _window.x + _window.w )
				break;

			r2.w = m_pColumns[i].m_realWidth;
			//if( i == m_nColumns-1 && r2.x + r2.w < _window.x + _window.w )
			if( i == m_nColumns-1 )
				r2.w = _window.x + _window.w - r2.x;		// Last column header stretches to end of tableview...

			WgMode mode = WG_MODE_NORMAL;

			if(&m_pColumns[i] == m_pMarkedHeader)
				mode = WG_MODE_MARKED;
			else if( i == m_lastSortColumn )
				mode = WG_MODE_SPECIAL;

			pDevice->ClipBlitBlock( _clip, m_pHeaderGfx->GetBlock(mode,r2), r2 );

			if( i == m_lastSortColumn && m_pAscendGfx && m_pDescendGfx )
			{
				WgBlock block;

				if( m_lastSortColumnAscendStatus )
					block = m_pAscendGfx->GetBlock(mode);
				else
					block = m_pDescendGfx->GetBlock(mode);

				WgRect dest = WgUtil::OrientationToRect( m_sortMarkerAlignment, r2.Size(), block.Size() );
				dest += m_sortMarkerOfs;
				
				pDevice->ClipBlitBlock( _clip, block, dest );
			}

			WgRect rText = r2;
			if( m_pHeaderGfx )
				rText.Shrink( m_pHeaderGfx->Padding() );

			m_pColumns[i]._getTextObj()->setProperties( m_pHeaderProps );
			pDevice->PrintText( _clip, m_pColumns[i]._getTextObj(), rText );

			r2.x += r2.w - 1;	// HACK: Overlap last pixel to avoid double separator graphics between two headers
		}

		r.y += m_pHeaderGfx->Height();

		// Modify clipping rectangle for view content (we don't want to draw over header)

		if(  _clip.y < _window.y + m_pHeaderGfx->Height())
		{
			int diff = _window.y + m_pHeaderGfx->Height() - _clip.y;
			clipView.y += diff;
			clipView.h -= diff;
			if( clipView.h < 1 )
				return;
		}
	}

	// Start drawing cell contents.

	WgTableRow * pRow = m_rows.First();
	int iRowColor = 0;

	// Skip rows that are above clipping area.
	r.y += m_cellPadding.top;
	while( pRow )
	{
		if( pRow->IsVisible() )
		{
			if( r.y + (int) pRow->Height() >= clipView.y )
				 break;
			r.y += pRow->Height() + m_cellPadding.Height();
			iRowColor++;
		}
		pRow = pRow->Next();
	}
	r.y -= m_cellPadding.top;


	// Draw cell contents for (at least partly) visible rows.
	while( pRow )
	{
		if( !pRow->IsVisible() )
		{
			pRow = pRow->Next();
			continue;
		}

		if( r.y >= clipView.y + clipView.h )
			break;

		r.h = pRow->Height() + m_cellPadding.Height();

		WgRect	u;
		if( u.Intersection( r, clipView ) )
		{
			if( pRow->IsSelected() )
			{
				if(HasSelectedRowBg() == true)
					pDevice->ClipBlitBlock(u, m_pSelectedRowGfx->GetBlock(WG_MODE_NORMAL,r), r );
				else
					pDevice->Fill( u, m_selectedRowColor );
			}
			else
			{
				if( m_nRowColors > 0 )
				{
					WgColor color = m_pRowColors[ iRowColor % m_nRowColors ];
					if( 0 != color.a )
						pDevice->Fill( u, color );
				}

				if( m_nRowBlocks > 0 )
				{
					WgBlocksetPtr p = m_pRowBlocks[ iRowColor % m_nRowBlocks ];
					if( p )
						pDevice->ClipBlitBlock(u, p->GetBlock(WG_MODE_NORMAL,r), r );
				}

			}
		}

		WgRect		rc = r;

		rc.y += m_cellPadding.top;
		rc.h = pRow->Height();
		for( int i = 0 ; i < m_nColumns ; i++ )
		{
			WgTableHook * pHook = 0;
			if( pRow->m_nCells > i && pRow->m_pCells[i].Gizmo() != 0 )
				pHook = &pRow->m_pCells[i];

//			if( pHook == 0 )									//TODO: Make it right!
//				pHook = m_pColumns[i].m_pDefaultGizmoHook;

			if( m_pColumns[i].m_bVisible && pHook->IsVisible() && pHook->Gizmo() != 0 )
			{
				// don't draw columns that are outside of the window
				if( rc.x >= _window.x + _window.w )
					break;

				rc.w = m_pColumns[i].m_realWidth;
				if( i == m_nColumns-1 && rc.x + rc.w < _window.x + _window.w )
					rc.w = _window.x + _window.w - rc.x;		// Last column stretches to end of tableview...

				rc.x += m_cellPadding.left;
				rc.w -= m_cellPadding.Width();

				WgRect clip2( rc, clipView );

				//

				pHook->Gizmo()->_onRender( pDevice, rc, rc, clip2, _layer );
				rc.x += m_pColumns[i].m_realWidth - m_cellPadding.left;		// Left cellpadding already added...
			}
		}

		r.y += pRow->Height() + m_cellPadding.Height();
		pRow = (WgTableRow *) pRow->Next();
		iRowColor++;
	}

	// Possibly fill with empty rows

	if( m_emptyRowHeight != 0 && pRow == 0 )
	{
		while( r.y <= clipView.y + clipView.h )
		{
			r.h = m_emptyRowHeight;

			WgRect	u;
			if( u.Intersection( r, clipView ) )
			{
				if( m_nRowColors > 0 )
				{
					WgColor color = m_pRowColors[ iRowColor % m_nRowColors ];
					if( 0 != color.a )
						pDevice->Fill( u, color );
				}

				if( m_nRowBlocks > 0 )
				{
					WgBlocksetPtr p = m_pRowBlocks[ iRowColor % m_nRowBlocks ];
					if( p )
						pDevice->ClipBlitBlock(u, p->GetBlock(WG_MODE_NORMAL), r );
				}
			}

			r.y += r.h;
			iRowColor++;
		}
	}
}


//____ _onCloneContent() _______________________________________________________

void WgTablePanel::_onCloneContent( const WgGizmo * _pOrg )
{
	WgTablePanel * pOrg = (WgTablePanel *) _pOrg;

	// For the moment we don't clone the content

	m_rows.Clear();
	m_nRows = 0;

	m_contentSize	= pOrg->m_contentSize;		//TODO: We need to clone content for this to get right!!!!

	m_selectMode = pOrg->m_selectMode;

	for( int i = 0 ; i < c_nSortColumns ; i++ )
		m_sortStack[i] = pOrg->m_sortStack[i];


	m_clickSortPrio		= pOrg->m_clickSortPrio;
	m_bSortAscend		= pOrg->m_bSortAscend;

	m_bShowHeader		= pOrg->m_bShowHeader;
	m_bAutoScaleHeader	= pOrg->m_bAutoScaleHeader;
	m_bAutoScrollMarked = pOrg->m_bAutoScrollMarked;

	m_nColumns			= pOrg->m_nColumns;

	if( m_nColumns > 0 )
	{
		m_pColumns = new WgTableColumn[m_nColumns];
		for( int i = 0 ; i < m_nColumns ; i++ )
			new (&m_pColumns[i]) WgTableColumn(pOrg->m_pColumns[i]);
	}
	else
		m_pColumns = 0;


	m_nRowColors = pOrg->m_nRowColors;
	if( m_nRowColors > 0 && m_pRowColors )
	{
		m_pRowColors = new WgColor[ m_nRowColors ];
		memcpy( m_pRowColors, pOrg->m_pRowColors, sizeof( WgColor ) * m_nRowColors );
	}
	else
		m_pRowColors = 0;


	m_nRowBlocks = pOrg->m_nRowBlocks;
	if( m_nRowBlocks > 0 && m_pRowBlocks )
	{
		m_pRowBlocks = new WgBlocksetPtr[ m_nRowBlocks ];
		for( int i = 0 ; i < m_nRowBlocks ; i++ )
			m_pRowBlocks[i] = pOrg->m_pRowBlocks[i];					// Can't memcpy this... smartpointers...
	}
	else
		m_pRowBlocks = 0;


	m_emptyRowHeight	= pOrg->m_emptyRowHeight;

	m_pHeaderProps		= pOrg->m_pHeaderProps;

	m_sortMarkerAlignment= pOrg->m_sortMarkerAlignment;
	m_sortMarkerOfs		= pOrg->m_sortMarkerOfs;

	m_cellPadding		= pOrg->m_cellPadding;

	m_pAscendGfx		= pOrg->m_pAscendGfx;
	m_pDescendGfx		= pOrg->m_pDescendGfx;

	m_lastSortColumn	= pOrg->m_lastSortColumn;
	m_lastSortColumnAscendStatus = pOrg->m_lastSortColumnAscendStatus;

	m_lastClickedRow	= 0;
	m_lastClickedColumn	= 0;

	m_pHeaderGfx = pOrg->m_pHeaderGfx;

	m_pMarkedHeader = 0;

	m_pSelectedRowGfx = pOrg->m_pSelectedRowGfx;
	m_selectedRowColor = pOrg->m_selectedRowColor;
}


//____ GetHeaderColumnAt() ____________________________________________________

WgTableColumn *WgTablePanel::_getHeaderColumnAt( const WgCoord& pos )
{
	if(pos.x < 0 || pos.y < 0)
		return NULL;

	if( m_bShowHeader && m_pHeaderGfx && m_pHeaderGfx->Height() > pos.y )
	{
		int xOfs = pos.x;
		for( int col = 0 ; col < m_nColumns ; col++ )
		{
			if( m_pColumns[col].m_bVisible )
			{
				int w = m_pColumns[col].m_realWidth;
				if( xOfs < w || col == m_nColumns-1 )					// Last column header stretches to end of tableview...
					return &m_pColumns[col];
				xOfs -= w;
			}
		}
	}
	return NULL;
}

//____ _updateMarkedRowColumn() ________________________________________________

void WgTablePanel::_updateMarkedRowColumn( int row, int column )
{
	// Update m_markedRow/Column right away so change has happened before signals are emitted.

	int oldRow		= m_markedRow;
	int oldColumn	= m_markedColumn;

	m_markedRow = row;
	m_markedColumn = column;

	//

	WgEventHandler * pHandler = _eventHandler();

	if( pHandler && (column != oldColumn || row != oldRow) )
	{
		if( oldRow != -1 && oldColumn != -1 )
			pHandler->QueueEvent( new WgEvent::TableCellUnmarked(this, oldRow, oldColumn, GetCellContent(oldRow, oldColumn) ) );

		if( row != -1 && column != -1 )
			pHandler->QueueEvent( new WgEvent::TableCellMarked(this, row, column, GetCellContent(row, column) ) );
	}
}

//____ _firstHookWithGeo() _____________________________________________________

WgHook* WgTablePanel::_firstHookWithGeo( WgRect& geo ) const
{
	//TODO: Implement!!!

	return 0;
}

//____ _nextHookWithGeo() _____________________________________________________

WgHook* WgTablePanel::_nextHookWithGeo( WgRect& geo, WgHook * pHook ) const
{
	//TODO: Implement!!!

	return 0;
}

//____ _lastHookWithGeo() _____________________________________________________

WgHook* WgTablePanel::_lastHookWithGeo( WgRect& geo ) const
{
	//TODO: Implement!!!

	return 0;
}

//____ _prevHookWithGeo() _____________________________________________________

WgHook* WgTablePanel::_prevHookWithGeo( WgRect& geo, WgHook * pHook ) const
{
	//TODO: Implement!!!

	return 0;
}



