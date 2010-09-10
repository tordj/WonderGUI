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

#include	<wg_gizmo_table.h>
#include	<wg_text.h>
#include	<wg_font.h>
#include	<wg_gfx.h>


static const char	c_gizmoType[] = {"TordJ/Table"};


//____ WgTableColumn2::Iterator _______________________________________________
/*
void WgTableColumn2::Iterator::next()
{
	while( m_pRow )
	{
		m_pRow = m_pRow->getNext();
		if( m_pRow->m_nCells >= m_pos && m_pRow->m_pCells[m_pos].Gizmo() )
		{
			m_pHook = &m_pRow->m_pCells[m_pos];
			m_pGizmo = m_pRow->m_pCells[m_pos].Gizmo();
			return;
		}
	}

	m_pHook = 0;
	m_pGizmo = 0;
}

void WgTableColumn2::Iterator::prev()
{
	while( m_pRow )
	{
		m_pRow = m_pRow->getPrev();
		if( m_pRow->m_nCells >= m_pos && m_pRow->m_pCells[m_pos].Gizmo() )
		{
			m_pHook = &m_pRow->m_pCells[m_pos];
			m_pGizmo = m_pRow->m_pCells[m_pos].Gizmo();
			return;
		}
	}

	m_pHook = 0;
	m_pGizmo = 0;
}

//____ WgTableRow2::Iterator __________________________________________________

void WgTableRow2::Iterator::next()
{
	while( m_pos < m_pRow->m_nCells )
	{
		m_pos++;
		if( m_pRow->m_pCells[m_pos].Gizmo() )
		{
			m_pHook = &m_pRow->m_pCells[m_pos];
			m_pGizmo = m_pRow->m_pCells[m_pos].Gizmo();
			return;
		}
	}

	m_pHook = 0;
	m_pGizmo = 0;
}

void WgTableRow2::Iterator::prev()
{
	if( m_pHook )
	{
		while( m_pos >= 0 )
		{
			m_pos--;
			if( m_pRow->m_pCells[m_pos].Gizmo() )
			{
				m_pHook = &m_pRow->m_pCells[m_pos];
				m_pGizmo = m_pRow->m_pCells[m_pos].Gizmo();
				return;
			}
		}

		m_pHook = 0;
		m_pGizmo = 0;
	}
}
//____ WgGizmoTable::Iterator _________________________________________________

void WgGizmoTable::Iterator::next()
{
	while( m_pRow )
	{
		while( m_pos < m_pRow->m_nCells )
		{
			m_pos++;
			if( m_pRow->m_pCells[m_pos].Gizmo() )
			{
				m_pHook = &m_pRow->m_pCells[m_pos];
				m_pGizmo = m_pRow->m_pCells[m_pos].Gizmo();
				return;
			}

		}
		m_pos = 0;
		m_pRow = m_pRow->getNext();
	}

	m_pHook = 0;
	m_pGizmo = 0;
}

void WgGizmoTable::Iterator::prev()
{
	while( m_pRow )
	{
		while( m_pos >= 0 )
		{
			m_pos--;
			if( m_pRow->m_pCells[m_pos].Gizmo() )
			{
				m_pHook = &m_pRow->m_pCells[m_pos];
				m_pGizmo = m_pRow->m_pCells[m_pos].Gizmo();
				return;
			}

		}
		m_pRow = m_pRow->getPrev();
		if( m_pRow )
			m_pos = m_pRow->m_nCells;
	}

	m_pHook = 0;
	m_pGizmo = 0;
}
*/


WgCord WgTableHook::Pos() const
{
}

WgSize WgTableHook::Size() const
{
}

WgRect WgTableHook::Geo() const
{
}

WgCord WgTableHook::ScreenPos() const
{
}

WgRect WgTableHook::ScreenGeo() const
{
}


WgGizmoHook* WgTableHook::PrevHook() const
{
}

WgGizmoHook* WgTableHook::NextHook() const
{
}


WgEmitter* WgTableHook::GetEmitter()
{
}

WgWidget* WgTableHook::GetRoot()
{
}

WgTableHook::WgTableHook()
{
}


WgTableHook::WgTableHook( WgGizmo * pGizmo, WgTableRow2 * pRow )
{
}

WgTableHook::~WgTableHook()
{
}


WgGizmo* WgTableHook::ReleaseGizmo()
{
}
	
void WgTableHook::RequestRender()
{
}

void WgTableHook::RequestRender( const WgRect& rect )
{
}

void WgTableHook::RequestResize()
{
	// Nothing here, Gizmo in cell doesn't get to demand its own size...
}

bool WgTableHook::RequestFocus()
{
}

bool WgTableHook::ReleaseFocus()
{
}




WgTableColumn2::WgTableColumn2()
{
	m_pixelWidth		= 4;
	m_fpCompare			= 0;
	m_bVisible			= true;
	m_bEnabled			= true;
	m_bInitialAscend	= true;
	m_id				= 0;
	m_pOwner			= 0;
	m_pText				= 0;
	m_pDefaultGizmo		= 0;
}



WgTableColumn2::WgTableColumn2( WgGizmoTable * pOwner )
{
	m_pixelWidth		= 4;
	m_fpCompare			= 0;
	m_bVisible			= true;
	m_bEnabled			= true;
	m_bInitialAscend	= true;
	m_id				= 0;
	m_pOwner			= pOwner;
	m_pText				= 0;
	m_pDefaultGizmo		= 0;
}


WgTableColumn2::WgTableColumn2(const WgTableColumn2& column)
{
	m_pixelWidth = column.m_pixelWidth;
	m_fpCompare = column.m_fpCompare;
	m_bVisible = column.m_bVisible;
	m_bEnabled = column.m_bEnabled;
	m_bInitialAscend = column.m_bInitialAscend;
	m_id = column.m_id;
	m_pOwner = 0;
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
	}
	else
	{
		m_pText = 0;
	}
}


WgTableColumn2::~WgTableColumn2()
{
	if( m_pText )
		delete m_pText;
}


void WgTableColumn2::setWidth( int pixels )
{
	if( pixels < 4 )
		return;

	if( m_pixelWidth != pixels )
	{
		Sint32 widthdiff = pixels - m_pixelWidth;
		m_pixelWidth = pixels;
		if( m_bVisible && m_pOwner )
		{
			m_pOwner->SetContentSize( m_pOwner->m_contentWidth + widthdiff, m_pOwner->m_contentHeight );
			m_pOwner->RequestRender();
		}
	}

}

void WgTableColumn2::hide()
{
	if( m_bVisible )
	{
		m_bVisible = false;

		if( m_pOwner )
		{
			m_pOwner->SetContentSize( m_pOwner->m_contentWidth - m_pixelWidth, m_pOwner->m_contentHeight );
			m_pOwner->RequestRender();
		}
	}
}

void WgTableColumn2::show()
{
	if( !m_bVisible )
	{
		m_bVisible = true;

		if( m_pOwner )
		{
			m_pOwner->SetContentSize( m_pOwner->m_contentWidth + m_pixelWidth, m_pOwner->m_contentHeight );
			m_pOwner->RequestRender();
		}
	}
}

void WgTableColumn2::enable()
{
	if( !m_bEnabled )
	{
		m_bEnabled = true;
		if( m_bVisible && m_pOwner )
			m_pOwner->RequestRender();
	}
}

void WgTableColumn2::disable()
{
	if( m_bEnabled )
	{
		m_bEnabled = false;
		if( m_bVisible && m_pOwner )
			m_pOwner->RequestRender();
	}
}


void WgTableColumn2::setSortFunction( fpItemCmp pFunc )
{
	m_fpCompare = pFunc;
}


void WgTableColumn2::setTextObj(WgText *text)
{
	m_pText = text;
}

const WgText* WgTableColumn2::getTextObj() const
{
	return m_pText;
}

WgText* WgTableColumn2::getTextObj()
{
	return m_pText;
}

void WgTableColumn2::textModified()
{
	if( m_bVisible && m_pOwner )
		m_pOwner->RequestRender();
}


WgTableRow2::WgTableRow2( Sint64 id, int nCells )
{
	if( nCells < 0 )
		nCells = 0;

	m_nCells = nCells;


	if( m_nCells > 0 )
		m_pCells = new WgTableHook[nCells];
	else
		m_pCells = 0;

	m_id		= id;
	m_height	= 0;
	m_mode		= WG_MODE_NORMAL;
	m_bVisible	= true;
}

WgTableRow2::~WgTableRow2()
{
	delete [] m_pCells;
}

void WgTableRow2::SetGizmo( WgGizmo * pGizmo, int col )
{
	if( m_nCells <= col )
		ResizeCellsArray(col+1);

	if( m_pCells[col].Gizmo() )
		m_pCells[col].~WgTableHook();

	new (&m_pCells[col])WgTableHook(pGizmo, this);

	//TODO: Meddela table att rad har ändrats.
}

int WgTableRow2::AddGizmo( WgGizmo * pGizmo )
{
	// Look for the last empty cell

	int cell = m_nCells;
	
	while( cell > 0 && m_pCells[cell-1] == 0 )
		cell--;

	// Resize if necessary

	if( m_nCells == cell )
		ResizeCellsArray(cell+1);
	
	//TODO: Meddela table att rad har ändrats.
	
	return cell;
}

bool WgTableRow2::Select()
{
	if( m_mode == WG_MODE_DISABLED )
		return false;

	if( m_mode != WG_MODE_SELECTED )
	{
		m_mode = WG_MODE_SELECTED;

		for( int i = 0 ; i < m_nCells ; i++ )
			m_pCell[i].Gizmo()->SetSelected();
	}
	return true;
}


void WgTableRow2::Unselect()
{
	if( m_mode == WG_MODE_DISABLED )
		return false;

	if( m_mode != WG_MODE_NORMAL )
	{
		m_mode = WG_MODE_NORMAL;

		for( int i = 0 ; i < m_nCells ; i++ )
			m_pCell[i].Gizmo()->SetNormal();
	}
}


//____ Constructor ____________________________________________________________

void WgGizmoTable::WgGizmoTable()
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


	m_sortMarkerOrigo	= WgOrigo::midRight();
	m_sortMarkerOfs.x	= 0;
	m_sortMarkerOfs.y	= 0;
	m_clickSortPrio		= 0;

	m_cellPaddingX		= 0;
	m_cellPaddingY		= 0;

	m_lastClickedRow = -1;
	m_lastClickedColumn = -1;

	m_bAutoScrollMarked	= false;

	m_lastSortColumn				= 0;
	m_lastSortColumnAscendStatus	= true;

	m_pMarkedHeader = NULL;

	m_selectMode = SELECT_SINGLE_ROW;
	m_nRows = 0;

	m_selectedRowColor = WgColor(128,128,128,85);
}

//____ Destructor _____________________________________________________________

WgGizmoTable::~WgGizmoTable( void )
{
	RemoveColumns();
	RemoveRowColors();
	RemoveRowBlocks();

	delete [] m_pColumns;
	m_pColumns = 0;							// So that refresh methods called when underlying items are deleted knows...
}


//____ Type() _________________________________________________________________

const char * WgGizmoTable::Type( void ) const
{
	return GetMyType();
}

const char * WgGizmoTable::GetMyType( void )
{
	return c_gizmoType;
}

//____ SetHeaderSource() ______________________________________________________

bool WgGizmoTable::SetHeaderSource( const WgBlockSetPtr& pHeader )
{
	m_pHeaderGfx		= pHeader;

	if( m_bShowHeader )
		RequestRender();

	return true;
}

//____ SetLineMarkSource() ____________________________________________________

void WgGizmoTable::SetLineMarkSource( WgBlockSetPtr pGfx )
{
	m_selectedRowGfx = pGfx;
	RequestRender();
}

//____ SetArrowSource() _______________________________________________________

void WgGizmoTable::SetArrowSource( const WgBlockSetPtr& pAscend, const WgBlockSetPtr& pDescend )
{
	m_pAscendGfx	= pAscend;
	m_pDescendGfx	= pDescend;

	if( m_bShowHeader )
		RequestRender();
}

//____ SetArrowPos() __________________________________________________________

void WgGizmoTable::SetArrowPos( const WgOrigo& origo, int xOfs, int yOfs )
{
	m_sortMarkerOrigo 	= origo;
	m_sortMarkerOfs.x	= xOfs;
	m_sortMarkerOfs.y	= yOfs;

	if( m_bShowHeader )
		RequestRender();
}

//____ SetArrowPos() __________________________________________________________

void WgGizmoTable::SetArrowPos( int xOfs, int yOfs )
{
	m_sortMarkerOfs.x	= xOfs;
	m_sortMarkerOfs.y	= yOfs;

	if( m_bShowHeader )
		RequestRender();
}

//____ SetArrowOrigo() __________________________________________________________

void WgGizmoTable::SetArrowOrigo( WgOrigo origo )
{
	m_sortMarkerOrigo 	= origo;

	if( m_bShowHeader )
		RequestRender();
}

//____ SetCellPadding() _______________________________________________________

void WgGizmoTable::SetCellPadding( int x, int y )
{
	m_cellPaddingX = x;
	m_cellPaddingY = y;

	RequestRender();
}

//____ SetEmptyRowHeight() ____________________________________________________

void WgGizmoTable::SetEmptyRowHeight( int height )
{
	if( height != m_emptyRowHeight )
	{
		m_emptyRowHeight = height;
		RequestRender();
	}
}

//____ SetRowColors() _________________________________________________________

void WgGizmoTable::SetRowColors( WgColor * pRowColors, int nRowColors )
{
	if( m_pRowColors )
		RemoveRowColors();

	if( nRowColors > 0 && pRowColors )
	{
		m_nRowColors = nRowColors;
		m_pRowColors = new WgColor[ m_nRowColors ];

		memcpy( m_pRowColors, pRowColors, sizeof( WgColor ) * m_nRowColors );
	}

	RequestRender();
}

//____ RemoveRowColors() ______________________________________________________
void WgGizmoTable::RemoveRowColors()
{
	if( m_pRowColors )
	{
		delete [] m_pRowColors;
		m_pRowColors = 0;
		m_nRowColors = 0;
	}
}

//____ SetRowBlocks() _________________________________________________________

void WgGizmoTable::SetRowBlocks( WgBlockSetPtr * pRowBlocks, int nRowBlocks )
{
	if( m_pRowBlocks )
		RemoveRowBlocks();

	if( nRowBlocks > 0 && pRowBlocks )
	{
		m_nRowBlocks = nRowBlocks;
		m_pRowBlocks = new WgBlockSetPtr[ m_nRowBlocks ];

		for( int i = 0 ; i < nRowBlocks ; i++ )
			m_pRowBlocks[i] = pRowBlocks[i];
	}

	RequestRender();
}

//____ RemoveRowBlocks() ______________________________________________________
void WgGizmoTable::RemoveRowBlocks()
{
	if( m_pRowBlocks )
	{
		delete [] m_pRowBlocks;
		m_pRowBlocks = 0;
		m_nRowBlocks = 0;
	}
}


//____ RemoveColumns() ____________________________________________________________

void WgGizmoTable::RemoveColumns()
{
	UpdateMarkedRowColumn(-1, -1);

	delete [] m_pColumns;
	m_pColumns = 0;
	m_nColumns = 0;
}

//____ AddColumn() ____________________________________________________________

//TODO: Add default gizmo to parameters

int WgGizmoTable::AddColumn( const WgCharSeq& text, int pixelwidth, WgOrigo& headerAlign, int(*fpCompare)(WgGizmo *,WgGizmo *), 
							 bool bInitialAscend, bool bEnabled, Sint64 id, WgGizmo * pDefaultGizmo )
{
	WgTableColumn2 * pCol = new WgTableColumn2[m_nColumns+1];

	for( int i = 0 ; i < m_nColumns ; i++ )
	{
		pCol[i] = m_pColumns[i];
		m_pColumns[i].m_pText = 0;			// To avoid deletion of text object further down.
	}
	pCol[m_nColumns].m_pixelWidth = pixelwidth;

	WgText * p = new WgText( pText );
	p->SetWrap(false);
	pCol[m_nColumns].SetTextObj(p);

	pCol[m_nColumns].GetTextObj()->setAlignment(headerAlign);
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

	SetContentSize( m_contentWidth + pixelwidth, m_contentHeight );
	RequestRender();
	return m_nColumns;
}


//____ FindColumn() ___________________________________________________________

WgTableColumn* WgGizmoTable::FindColumn(Sint64 id) const
{
	for( int i = 0 ; i < m_nColumns ; i++ )
		if( m_pColumns[i].m_id == id )
			return &m_pColumns[i];

	return 0;
}


//____ GetSortColumn() _____________________________________________________

bool WgGizmoTable::GetSortColumn( int order, Uint32& columnIndex, bool& bAscend )
{
	if( order >= c_nSortColumns )
		return false;

	if( 0xFFFF == m_sortStack[ order ].column )
		return false;

	columnIndex = m_sortStack[ order ].column;
	bAscend = m_sortStack[ order ].bAscend;

	return true;
}


//____ GetCellGeo() ___________________________________________________________

WgRect WgGizmoTable::GetCellGeo( int row, int column )
{

	WgRect	r( 0, 0, 0, 0 );

	float scale = CalcHeaderScaleFactor();

	// Adjust for header

	if( m_bShowHeader && m_pHeaderGfx )
		r.y += m_pHeaderGfx->GetHeight();


	WgTableRow2 * pRow = m_rows.getFirst();

	// Forward to our row
	for( int i = 0 ; i < row ; i++ )
	{
		if( !pRow )
			return WgRect();							// Invalid row.

		if( pRow->IsVisible() )
			r.y += pRow->Height() + m_cellPaddingY*2;

		pRow = pRow->GetNext();
	}
	r.y += m_cellPaddingY;


	// Set cell height
	if( pRow->IsHidden() )
		r.h = 0;
	else
		r.h = pRow->Height();

	// Go through columns, determine x-pos and width. 

	if( column >= (int) m_nColumns )
		return WgRect();							// Invalid column;

	r.x = (int)(r.x * scale);
	for( int i = 0 ; i <= column ; i++ )
	{
		if( m_pColumns[i].m_bVisible )
		{
			r.w = (int)(m_pColumns[i].m_pixelWidth * scale);
//			if( i == m_nColumns-1 && r.x + r.w < _window.x + _window.w )
//				r.w = _window.x + _window.w - r.x;		// Last column stretches to end of tableview...

			r.x += m_cellPaddingX;
			r.w -= m_cellPaddingX*2;

			if( i != column )
				r.x += (int)(m_pColumns[i].m_pixelWidth * scale) - m_cellPaddingX;		// One cellpadding already added...
		}
	}

	return r;
}


//____ AddRow() _______________________________________________________________

int WgGizmoTable::AddRow( WgTableRow2* pRow )
{
	m_rows.push_back(pRow);

	int index = m_nRows;
	m_nRows++;
	RowAdded(pRow);
	return index;
}

//____ InsertRow() ____________________________________________________________

int WgGizmoTable::InsertRow( WgTableRow2* pRow, int pos )
{
	WgTableRow2* p = m_rows.get(pos);
	int index;

	if( p )
	{
		pRow->moveBefore( p );
		index = pos;
	}
	else
	{
		m_rows.push_back(pItem);
		index = m_nRows;
	}

	m_nRows++;
	RowAdded(pRow);
	return index;
}

//____ InsertRowSorted() ______________________________________________________

void WgGizmoTable::InsertRowSorted( WgTableRow2* pRow )
{
	if( m_nRows == 0 )
	{
		m_rows.push_back(pRow);
	}
	else
	{

		WgTableRow2 * pFirst = m_rows.getFirst();
		WgTableRow2 * pLast = m_rows.getLast();

		// First check if we should be put first or last in list. Equal to last should put us last.

		bool bInserted = false;
		if( m_bSortAscend )
		{
			if( CompareRows( pRow, pFirst ) < 0 )
		s	{
				pRow->moveBefore( pFirst );
				bInserted = true;
			}
			else if( CompareRows( pRow, pLast ) >= 0 )
			{
				pRow->moveAfter( pLast );
				bInserted = true;
			}
		}
		else
		{
			if( CompareItems( pRow, pFirst ) > 0 )
			{
				pRow->moveBefore( pFirst );
				bInserted = true;
			}
			else if( CompareItems( pRow, pLast ) <= 0 )
			{
				pRow->moveAfter( pLast );
				bInserted = true;
			}
		}

		// If it wasn't inserted first or last it needs to be inserted between pFirst and pLast

		if( !bInserted )
			InsertItemSortedBetween(pFirst, pLast, pRow, m_nRows );
	}

	//

	m_nRows++;
	RowAdded(pRow);
}

//____ GetRowNb() _____________________________________________________________

int WgGizmoTable::GetRowNb( WgTableRow2* pRow )
{
	if( m_rows.isMember( pRow ) )
		return pRow->getIndex();
	else
		return -1;
}

//____ GetRow() _______________________________________________________________

WgTableRow2* WgGizmoTable::GetRow( int pos )
{
	return m_rows.get(pos);
}

//____ FindRow() ______________________________________________________________

WgTableRow2* WgGizmoTable::FindRow( int id )
{
	WgTableRow2* pRow = m_rows.getFirst();

	while( pRow )
	{
		if( pRow->Id() == id )
			return pRow;

		pRow = pRow->getNext();
	}
	return 0;
}

//____ SetSelectedRowColor() __________________________________________________

void WgGizmoTable::SetSelectedRowColor( WgColor c )
{
	if( c != m_selectedRowColor )
	{
		m_selectedRowColor = c;
		RequestRender();
	}
}

//____ SetSelectedRowSource() _________________________________________________

void WgGizmoTable::SetSelectedRowSource( WgBlockSetPtr pBlock )
{
	if( pBlock != m_pSelectedRowGfx )
	{
		m_pSelectRowGfx = pBlock;
		RequestRender();							//TODO: More optimized handling...
	}
}


//____ RemoveRow() ____________________________________________________________

WgTableRow2 * WgGizmoTable::RemoveRow( int pos )
{
	if( pos == m_markedRow )
		UpdateMarkedRowColumn(-1,m_markedColumn);

	WgTableRow2* pRow = m_rows.get(pos);
	if( pRow )
	{
		pRow->disconnect();
		m_nRows--;
	}
	return pRow;
}

bool WgGizmoTable::RemoveRow( WgTableRow2 * pRow )
{
	if( GetRowNb(pRow) == m_markedRow )
		UpdateMarkedRowColumn(-1,m_markedColumn);

	if( pRow && m_rows.isMember(pRow) )
	{
		pRow->disconnect();
		m_nRows--;
		return true;
	}
	return false;
}

//____ RemoveAllRows() ________________________________________________________

void WgGizmoTable::RemoveAllRows()
{
	UpdateMarkedRowColumn(-1,m_markedColumn);

	while( m_rows.getFirst() )
		m_rows.getFirst()->disconnect();

	m_nRows = 0;
}

//____ DeleteRow() ____________________________________________________________

bool WgGizmoTable::DeleteRow( int pos )
{
	if( pos == m_markedRow )
		UpdateMarkedRowColumn(-1,m_markedColumn);

	WgTableRow2* pRow = m_rows.get(pos);
	if( pRow )
	{
		delete pRow;
		m_nRows--;
		return true;
	}
	return false;
}

bool WgGizmoTable::DeleteRow( WgTableRow2 * pRow )
{
	if( GetRowNb(pRow) == m_markedRow )
		UpdateMarkedRowColumn(-1,m_markedColumn);

	if( pRow && m_rows.isMember(pRow) )
	{
		delete pRow;
		m_nRows--;
		return true;
	}
	return false;
}

//____ DeleteAllRows() ________________________________________________________

void WgGizmoTable::DeleteAllRows()
{
	UpdateMarkedRowColumn(-1,m_markedColumn);

	m_rows.clear();
	m_nRows = 0;
	return;
}


//____ SetClickSortPrio() _____________________________________________________

bool WgGizmoTable::SetClickSortPrio( int prio )
{
	if( prio >= c_nSortColumns )
		return false;

	m_clickSortPrio = prio;
	return true;
}


//____ SortRows() _____________________________________________________________

bool WgGizmoTable::SortRows( int column, bool bAscend, int prio )
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

 	SortItems();
	return true;
}

//____ CompareRows() __________________________________________________________

int WgGizmoTable::CompareRows( WgTableRow2* pRow1, WgTableRow2* pRow2 )
{
	for( int n = 0 ; n < c_nSortColumns ; n++ )
	{
		int col = m_sortStack[n].column;

		if( col < m_nColumns && m_pColumns[col].m_fpCompare != 0 )
		{
			WgGizmo* p1 = 0, p2 = 0;
			int diff;

			if( pRow1->m_nCells >= col )
				p1 = pRow1->m_pCells[col];

			if( pRow2->m_nCells >= col )
				p2 = pRow2->m_pCells[col];

			if( p1 == p2 )
				diff = 0;
			else if( p1 == 0 )
				diff = 1;
			else if( p2 == 0 )
				diff = -1;

			int diff = m_pColumns[col].m_fpCompare( p1, p2 );

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


//____ SetHeaderTextProp() ____________________________________________________

bool WgGizmoTable::SetHeaderTextProp( const WgTextPropPtr& pProp )
{
	m_pHeaderProps = pProp;
	RequestRender();
	return true;
}


//____ ShowHeader() ___________________________________________________________

void WgGizmoTable::ShowHeader( bool bShow )
{
	if( bShow != m_bShowHeader )
	{
		m_bShowHeader = bShow;

		int newHeight = m_contentHeight;

		if( m_pHeaderGfx )
		{
			if( bShow )
				newHeight += m_pHeaderGfx->GetHeight();
			else
				newHeight -= m_pHeaderGfx->GetHeight();
		}

		SetContentSize( m_contentWidth, newHeight );
		RequestRender();
	}
}

//____ UpdateContentSize() _________________________________________________________

void WgGizmoTable::UpdateContentSize()
{
	Uint32	contentWidth = 0;
	Uint32	contentHeight = 0;

	// Calc contentHeight

	WgTableRow2 * p = m_rows.getFirst();

	while( p )
	{
		if( p->IsVisible() )
			contentHeight += p->Height() + m_cellPaddingY*2;
		p = p->getNext();
	}

	// Calc contentWidth

	float scale = CalcHeaderScaleFactor();
	for( Uint32 i = 0 ; i < m_nColumns ; i++ )
	{
		if( m_pColumns[i].m_bVisible )
			contentWidth += (int)(m_pColumns[i].m_pixelWidth * scale);
	}

	// Possibly add header to height

	if( m_bShowHeader && m_pHeaderGfx )
		contentHeight += m_pHeaderGfx->GetHeight();

	// Set size and request render

	SetContentSize( contentWidth, contentHeight );
}


//____ refreshItems() _________________________________________________________

void WgGizmoTable::refreshItems()
{
	if( m_pColumns == 0 )
		return;

	UpdateContentSize();

	RequestRender();
}

//____ RowModified() _________________________________________________________

void WgGizmoTable::RowModified( WgTableRow2 * pRow, int widthDiff , int heightDiff )
{

	//TODO: More specialized and optimized handling.

	refreshItems();
}

//____ RowMarkChanged() ________________________________________________

void WgGizmoTable::RowMarkChanged( WgTableRow2 * pRow, bool bMarked )
{
	if( m_bAutoScrollMarked )
	{
		if( bMarked )		// make sure marked item is in view
			ScrollIntoView( pRow );
	}
}

//____ ScrollIntoView() ________________________________________________
void WgGizmoTable::ScrollIntoView( WgTableRow2* pRow )
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


	Uint32 viewOfsMin = ViewPixelOfsY();
	Uint32 viewOfsMax = ViewPixelOfsY() + ViewPixelLenY();

	if( rowPosY < viewOfsMin )							// is the row above the view?
		SetViewPixelOfsY( rowPosY );
	else if( GetHeaderHeight() + rowPosY + pRow->Height() > viewOfsMax )	// is the row below the view?
		SetViewPixelOfsY( GetHeaderHeight() + rowPosY + pRow->Height() - ViewPixelLenY() );
}

//____ OnNewSize() ____________________________________________________________

void WgGizmoTable::OnNewSize( const WgSize& newSize )
{
	if( oldGeo.w != newGeo.w )
	{
		AdaptItemsToWidth( newGeo.w );		// this won't do anything since the items are WgTableRows
		UpdateContentSize();

		float scale = CalcHeaderScaleFactor();

		// so we'll manually go through and tell every row's item to adapt to the new width.
		// something like this is probably required in SetColumnWidth() as well,
		// however none of this is a viable solution of course.
		WgTableRow * pRow = (WgTableRow *) m_items.getFirst();
		while( pRow )
		{
			WgItem *	pCell = pRow->GetFirstItem();

			Sint32 x = 0;
			for( Uint32 i = 0 ; i < m_nColumns && pCell != 0 ; i++ )
			{
				if( m_pColumns[i].m_bVisible )
				{
					Sint32 width = (int)(m_pColumns[i].m_pixelWidth * scale);
					if( i == m_nColumns-1 )
						width = newGeo.w - x - m_cellPaddingX*2;		// Last column stretches to end of tableview...

					pCell->AdaptToWidth( width );

					x += m_cellPaddingX*2 + width;
				}
				pCell = pCell->GetNext();
			}

			pRow = (WgTableRow *) pRow->GetNext();
		}
	}
}



//____ GetMarkedRow() _________________________________________________________

int WgGizmoTable::GetMarkedRow( Uint32 y, WgTableRow*& pSaveRow, Uint32& saveYOfs )
{
	pSaveRow	= 0;
	saveYOfs	= 0;
	int row		= 0;

	// Transform from relative widget cordinates to content-cordinates.

	if( m_bShowHeader && m_pHeaderGfx )
	{
		if( (int)y < m_pHeaderGfx->GetHeight() )
			return -1;	// on header.
		y -=  m_pHeaderGfx->GetHeight();
	}

	y += m_viewPixOfsY;

	//

	if( y < 0 )
		return -1;

	//

	WgItem * p	= m_items.getFirst();

	while( p )
	{
		if( p->IsVisible() )
		{
			if( y < p->Height() +  m_cellPaddingY*2 )
			{
				pSaveRow	= (WgTableRow*) p;
				saveYOfs	= y;

				return row;
			}

			y -= p->Height() +  m_cellPaddingY*2;
		}
		p = p->getNext();
		row++;
	}
	return -1;									// Rows ended before mark-point.
}

//____ GetMarkedColumn() ______________________________________________________

int WgGizmoTable::GetMarkedColumn( Uint32 x, Uint32& saveXOfs )
{
	saveXOfs	= 0;

	// Transform from relative widget cordinates to content-cordinates.

	x += m_viewPixOfsX;

	//

	if( x < 0 )
		return -1;

	// Loop through columns to find out which one we are inside.

	float scale = CalcHeaderScaleFactor();

	for( Uint32 col = 0 ; col < m_nColumns ; col++ )
	{
		if( m_pColumns[col].m_bVisible )
		{
			Uint32 scaledW = (Uint32)(m_pColumns[col].m_pixelWidth * scale);
			if( x < scaledW )
			{
				saveXOfs	= x;
				return col;
			}

			x -= scaledW;
		}
	}

	return -1;									// Columns ended before mark-point.
}


//____ SetAutoScaleHeaders() __________________________________________________
void WgGizmoTable::SetAutoScaleHeaders(bool autoScaleHeaders)
{
	if(m_bAutoScaleHeader != autoScaleHeaders)
	{
		m_bAutoScaleHeader = autoScaleHeaders;
		RequestRender();
	}
}

//____ CalcHeaderScaleFactor() ________________________________________________
float WgGizmoTable::CalcHeaderScaleFactor()
{
	if(!m_bAutoScaleHeader)
		return 1.f;

	int w = 0;
	for( Uint32 i = 0 ; i < m_nColumns ; i++ )
	{
		if( !m_pColumns[i].m_bVisible )
			continue;
		w += m_pColumns[i].m_pixelWidth;
	}

	if(w == 0)
		return 1.f;

	return (float)m_geo.w / (float)w;
}


//____ OnRender() _____________________________________________________________
void WgGizmoTable::OnRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip, Uint8 _layer )
{
	WgRect	r = _canvas;
	WgRect	clipView = _clip;

	if( r.w < _window.w )
		r.w = _window.w;

	float scale = CalcHeaderScaleFactor();

	// Draw header (if any)

	if( m_bShowHeader && m_pHeaderGfx )
	{
		WgRect	headerArea( _canvas.x, _window.y, _canvas.w, _m_pHeaderGfx->GetHeight );
		WgRect  r2 = headerArea;

		for( Uint32 i = 0 ; i < m_nColumns ; i++ )
		{
			if( !m_pColumns[i].m_bVisible )
				continue;

            // don't draw columns that are outside of the window
			if( r2.x >= _window.x + _window.w )
				break;

			r2.w = (int)(m_pColumns[i].m_pixelWidth * scale);
			//if( i == m_nColumns-1 && r2.x + r2.w < _window.x + _window.w )
			if( i == m_nColumns-1 )
				r2.w = _window.x + _window.w - r2.x;		// Last column header stretches to end of tableview...

			WgMode mode = WG_MODE_NORMAL;

			if(&m_pColumns[i] == m_pMarkedHeader)
				mode = WG_MODE_MARKED;
			else if( i == m_lastSortColumn )
				mode = WG_MODE_SPECIAL;

			pDevice->ClipBlitBlock( _clip, m_pHeaderGfx->GetBlock(mode), r2 );

			if( i == m_lastSortColumn && m_pAscendGfx && m_pDescendGfx )
			{
				WgBlock block;

				if( m_lastSortColumnAscendStatus )
					block = m_pAscendGfx->GetBlock(mode);
				else
					block = m_pDescendGfx->GetBlock(mode);

				Sint32 dx = (Sint32) (r2.x + m_sortMarkerOfs.x + r2.w * m_sortMarkerOrigo.anchorX() - block.GetWidth() * m_sortMarkerOrigo.hotspotX());
				Sint32 dy = (Sint32) (r2.y + m_sortMarkerOfs.y + r2.h * m_sortMarkerOrigo.anchorY() - block.GetHeight() * m_sortMarkerOrigo.hotspotY());

				pDevice->ClipBlitBlock( _clip, block, WgRect( dx, dy, block.GetWidth(), block.GetHeight()) );
			}

			WgRect rText = r2;
			if( m_pHeaderGfx )
				rText.Shrink( m_pHeaderGfx->GetContentBorders() );

			m_pColumns[i].GetTextObj()->setProperties( m_pHeaderProps );
			pDevice->PrintText( _clip, m_pColumns[i].GetTextObj(), rText );

			r2.x += r2.w - 1;	// HACK: Overlap last pixel to avoid double separator graphics between two headers
		}

		r.y += m_pHeaderGfx->GetHeight();

		// Modify clipping rectangle for view content (we don't want to draw over header)

		if(  _clip.y < _window.y + m_pHeaderGfx->GetHeight())
		{
			Sint32 diff = _window.y + m_pHeaderGfx->GetHeight() - _clip.y;
			clipView.y += diff;
			clipView.h -= diff;
			if( clipView.h < 1 )
				return;
		}
	}

	// Start drawing cell contents.

	WgTableRow2 * pRow = m_rows.getFirst();
	int iRowColor = 0;

	// Skip rows that are above clipping area.
	r.y += m_cellPaddingY;
	while( pRow )
	{
		if( pRow->IsVisible() )
		{
			if( r.y + (Sint32) pRow->Height() >= clipView.y )
				 break;
			r.y += pRow->Height() + m_cellPaddingY*2;
			iRowColor++;
		}
		pRow = pRow->GetNext();
	}
	r.y -= m_cellPaddingY;


	// Draw cell contents for (at least partly) visible rows.
	while( pRow )
	{
		if( pRow->IsHidden() )
		{
			pRow = pRow->GetNext();
			continue;
		}

		if( r.y >= clipView.y + clipView.h )
			break;

		r.h = pRow->Height() + m_cellPaddingY*2;

		WgRect	u;
		if( u.Intersection( r, clipView ) )
		{
			if( pRow->IsSelected() )
			{
				if(HasLineMarkSource() == true)
					pDevice->ClipBlitBlock(u, m_selectedRowGfx->GetBlock(WG_MODE_NORMAL), r );
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
					WgBlockSetPtr p = m_pRowBlocks[ iRowColor % m_nRowBlocks ];
					if( p )
						pDevice->ClipBlitBlock(u, p->GetBlock(WG_MODE_NORMAL), r );
				}

			}
		}

		WgRect		rc = r;

		rc.x = (int)(rc.x * scale);
		rc.y += m_cellPaddingY;
		rc.h = pRow->Height();
		for( Uint32 i = 0 ; i < m_nColumns ; i++ )
		{
			WgGizmoHook * pHook = 0;
			if( pRow->m_nCells > i && pRow->m_pCells[i].Gizmo() != 0 )
				pHook = &pRow->m_pCells[i];

//			if( pHook == 0 )									//TODO: Make it right!
//				pHook = m_pColumns[i].m_pDefaultGizmo;

			if( m_pColumns[i].m_bVisible && pGizmo != 0 )
			{
				// don't draw columns that are outside of the window
				if( rc.x >= _window.x + _window.w )
					break;

				rc.w = (int)(m_pColumns[i].m_pixelWidth * scale);
				if( i == m_nColumns-1 && rc.x + rc.w < _window.x + _window.w )
					rc.w = _window.x + _window.w - rc.x;		// Last column stretches to end of tableview...

				rc.x += m_cellPaddingX;
				rc.w -= m_cellPaddingX*2;

				WgRect clip2( rc, clipView );

				//

				pHook->DoRender( pDevice, rc, clip2 );
				rc.x += (int)(m_pColumns[i].m_pixelWidth * scale) - m_cellPaddingX;		// One cellpadding already added...
			}
			pCell = pCell->NextHook();
		}

		r.y += pRow->Height() + m_cellPaddingY*2;
		pRow = (WgTableRow *) pRow->GetNext();
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
					WgBlockSetPtr p = m_pRowBlocks[ iRowColor % m_nRowBlocks ];
					if( p )
						pDevice->ClipBlitBlock(u, p->GetBlock(WG_MODE_NORMAL), r );
				}
			}

			r.y += r.h;
			iRowColor++;
		}
	}
}


//____ OnCloneContent() _______________________________________________________

void WgGizmoTable::OnCloneContent( const WgGizmo * _pOrg )
{
	WgGizmoTable * pOrg = (WgGizmoTable *) _pOrg;

	// For the moment we don't clone the content

	m_rows.Clear();
	m_nRows = 0;


	m_selectMode = pOrg->m_selectMode;

	for( int i = 0 ; i < c_nSortColumns ; i++ )
		m_sortStack[i] = pOrg->m_sortStack;


	m_clickSortPrio		= pOrg->m_clickSortPrio;

	m_bShowHeader		= pOrg->m_bShowHeader;
	m_bAutoScaleHeader	= pOrg->m_bAutoScaleHeader;
	m_bAutoScrollMarked = pOrg->m_bAutoScrollMarked;

	m_nColumns			= pOrg->m_nColumns;

	if( m_nColumns > 0 )
	{
		m_pColumns = new WgTableColumn2[m_nColumns];
		for( int i = 0 ; i < m_nColumns ; i++ )
			new (&m_pColumns[i]) WgTableColumn2(pOrg->m_pColumns[i]);
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
		m_pRowBlocks = new WgBlockSetPtr[ m_nRowBlocks ];
		for( unsigned int i = 0 ; i < m_nRowBlocks ; i++ )
			m_pRowBlocks[i] = pOrg->m_pRowBlocks[i];					// Can't memcpy this... smartpointers...
	}
	else
		m_pRowBlocks = 0;


	m_emptyRowHeight	= pOrg->m_emptyRowHeight;

	m_pHeaderProps		= pOrg->m_pHeaderProps;

	m_sortMarkerOrigo	= pOrg->m_sortMarkerOrigo;
	m_sortMarkerOfs		= pOrg->m_sortMarkerOfs;

	m_cellPaddingX		= pOrg->m_cellPaddingX;
	m_cellPaddingY		= pOrg->m_cellPaddingY;

	m_pAscendGfx		= pOrg->m_pAscendGfx;
	m_pDescendGfx		= pOrg->m_pDescendGfx;

	m_lastSortColumn	= pOrg->m_lastSortColumn;
	m_lastSortColumnAscendStatus = pOrg->m_lastSortColumnAscendStatus;

	m_lastClickedRow	= 0;
	m_lastClickedColumn	= 0;

	m_pHeaderGfx = pOrg->m_pHeaderGfx;

	m_pMarkedHeader = 0;

	m_selectedRowGfx = pOrg->m_selectedRowGfx;
	m_selectedRowColor = pOrg->m_selectedRowColor;
}


//____ OnEnable() _____________________________________________________________

void WgGizmoTable::OnEnable( void )
{
	RequestRender();
}

//____ OnDisable() ____________________________________________________________

void WgGizmoTable::OnDisable( void )
{
	RequestRender();
}

//____ GetHeaderColumnAt() ____________________________________________________

WgTableColumn2 *WgGizmoTable::GetHeaderColumnAt(int x, int y)
{
	if(x < 0 || y < 0)
		return NULL;

	if( m_bShowHeader && m_pHeaderGfx && m_pHeaderGfx->GetHeight() > y )
	{
		float scale = CalcHeaderScaleFactor();
		int xOfs = x;
		for( Uint32 col = 0 ; col < m_nColumns ; col++ )
		{
			if( m_pColumns[col].m_bVisible )
			{
				int scaledW = (int)(m_pColumns[col].m_pixelWidth * scale);
				if( xOfs < scaledW || col == m_nColumns-1 )	// Last column header stretches to end of tableview...
					return &m_pColumns[col];
				xOfs -= scaledW;
			}
		}
	}
	return NULL;
}

//____ OnAction() _____________________________________________________________

void WgGizmoTable::OnAction( WgEmitter * pEmitter, WgInput::UserAction _action, int _button_key, const WgActionDetails& _info, const WgInput& _inputObj );
{
	int x = _info.x;
	int y = _info.y;

	Abs2local( &x, &y );

	switch( _action )
	{
		case WgInput::BUTTON_PRESS:
		{
			if( m_bShowHeader && m_pHeaderGfx && m_pHeaderGfx->GetHeight() > y )		// Press on header?
			{
				float scale = CalcHeaderScaleFactor();
				int xOfs = x;
				for( Uint32 col = 0 ; col < m_nColumns ; col++ )
				{
					int scaledW = (int)(m_pColumns[col].m_pixelWidth * scale);
					if( m_pColumns[col].m_bVisible && (xOfs < scaledW || col == m_nColumns-1) )	// Last column header stretches to end of tableview...
					{
						if( m_pColumns[col].m_bEnabled )
						{
							bool	bAscend = m_pColumns[col].m_bInitialAscend;
							if( col == m_lastSortColumn )
								bAscend = !m_lastSortColumnAscendStatus;

							// If we have clicked on the header of a column that is higher than clickSortPrio, we
							// should just flip bAscend right away since it's already sorted.

							for( int i = 0 ; i < m_clickSortPrio ; i++ )
							{
								if( m_sortStack[i].column == col )
								{
									bAscend = !m_sortStack[i].bAscend;
									break;
								}
							}

							//

							SortRows( col, bAscend, m_clickSortPrio );
						}
						break;
					}
					xOfs -= scaledW;
				}
				break;
			}

			Uint32 saveOfs;
			WgTableRow2 * pSaveRow;
			int row = GetMarkedRow(y, pSaveRow, saveOfs );
			if( row != -1 )
			{
				Emit( WgSignal::TableRowPress(_button_key), row );
			}

			int column = GetMarkedColumn(x, saveOfs);
			if( column != -1 )
			{
				Emit( WgSignal::TableColumnPress(_button_key), column );
				if( row != -1 )
				{
					Emit( WgSignal::TableCellPress(_button_key), row, column );
				}
			}
		}
		break;

		case WgInput::BUTTON_CLICK:
		{
			Uint32 saveOfs;
			WgTableRow2 * pSaveRow;
			int row = GetMarkedRow(y, pSaveRow, saveOfs );
			if( row != -1 )
			{
				Emit( WgSignal::TableRowClick(_button_key), row );
			}

			int column = GetMarkedColumn(x, saveOfs);
			if( column != -1 )
			{
				Emit( WgSignal::TableColumnClick(_button_key), column );
				if( row != -1 )
					Emit( WgSignal::TableCellClick(_button_key), row, column );
			}

			m_lastClickedRow = row;
			m_lastClickedColumn = column;

		}
		break;

		case WgInput::BUTTON_DOUBLECLICK:
		{
			Uint32 saveOfs;
			WgTableRow2 * pSaveRow;
			int row = GetMarkedRow(y, pSaveRow, saveOfs );
			if( row != -1 )
			{
				if(row == m_lastClickedRow)
					Emit( WgSignal::TableRowDoubleClick(_button_key), row );
			}

			int column = GetMarkedColumn(x, saveOfs);
			if( column != -1 )
			{
				if( column == m_lastClickedColumn )
				{
					Emit( WgSignal::TableColumnDoubleClick(_button_key), column );
					if( row != -1 )
						Emit( WgSignal::TableCellDoubleClick(_button_key), row, column );
				}
			}

		}
		break;

		case WgInput::POINTER_OVER:
		{
			WgTableColumn2 *col = GetHeaderColumnAt(x, y);
			if( m_pMarkedHeader != col )
			{
				m_pMarkedHeader = col;
				RequestRender();
			}


			Uint32 saveOfs;
			WgTableRow2 * pSaveRow;
			int row = GetMarkedRow(y, pSaveRow, saveOfs );
			int column = GetMarkedColumn(x, saveOfs);
			UpdateMarkedRowColumn( row, column );
		}
		break;

		case WgInput::POINTER_EXIT:
			if( m_pMarkedHeader != NULL )
			{
				m_pMarkedHeader = NULL;
				RequestRender();
			}
			UpdateMarkedRowColumn( -1, -1 );

		break;

        default:
            break;
	}
}

//____ UpdateMarkedRowColumn() ________________________________________________

void WgGizmoTable::UpdateMarkedRowColumn( int row, int column )
{
	// Update m_markedRow/Column right away so change has happened before signals are emitted.

	int oldRow		= m_markedRow;
	int oldColumn	= m_markedColumn;

	m_markedRow = row;
	m_markedColumn = column;

	//

	if( row != oldRow )
	{
		if( oldRow != -1 )
			Emit( WgSignal::TableRowUnmarked(), oldRow );
		if( row != -1 )
			Emit( WgSignal::TableRowMarked(), row );
	}

	if( column != oldColumn )
	{
		if( oldColumn != -1 )
			Emit( WgSignal::TableColumnUnmarked(), oldColumn );
		if( column != -1 )
			Emit( WgSignal::TableColumnMarked(), column );
	}

	if( column != oldColumn || row != oldRow )
	{
		if( oldRow != -1 && oldColumn != -1 )
			Emit( WgSignal::TableCellUnmarked(), oldRow, oldColumn );

		if( row != -1 && column != -1 )
			Emit( WgSignal::TableCellMarked(), row, column );
	}
}


//____ OnMarkTest() ___________________________________________________

bool WgGizmoTable::OnMarkTest( const WgCord& ofs )
{
	return true;
}

