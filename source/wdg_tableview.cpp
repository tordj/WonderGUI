#include	<string.h>

#include	<wdg_tableview.h>
#include	<wg_text.h>
#include	<wg_item_pixmap.h>
#include	<wg_font.h>
#include	<wg_gfx.h>

#include	<wg_item_text.h>

static const char	Wdg_Type[] = {"TordJ/TableView"};


WgTableColumn::WgTableColumn()
{
	m_fpCompare			= 0;
	m_bVisible			= true;
	m_bEnabled			= true;
	m_bInitialAscend	= true;
	m_id				= 0;
	m_pOwner			= 0;
	m_pText				= 0;
	m_defWidth			= 4;
	m_minWidth			= 4;
	m_maxWidth			= INT_MAX;
	m_scaleWeight		= 1.f;
	m_pixelWidth		= 4;
	m_pItem				= 0;
	m_itemOrigo			= WgOrigo::midLeft();
}



WgTableColumn::WgTableColumn( Wdg_TableView * pOwner )
{
	m_fpCompare			= 0;
	m_bVisible			= true;
	m_bEnabled			= true;
	m_bInitialAscend	= true;
	m_id				= 0;
	m_pOwner			= pOwner;
	m_pText				= 0;
	m_defWidth			= 4;
	m_minWidth			= 4;
	m_maxWidth			= INT_MAX;
	m_scaleWeight		= 1.f;
	m_pixelWidth		= 4;
	m_pItem				= 0;
	m_itemOrigo			= WgOrigo::midLeft();
}


WgTableColumn::WgTableColumn(const WgTableColumn& column)
{
	m_fpCompare = column.m_fpCompare;
	m_bVisible = column.m_bVisible;
	m_bEnabled = column.m_bEnabled;
	m_bInitialAscend = column.m_bInitialAscend;
	m_id = column.m_id;
	m_pOwner = 0;

	m_defWidth			= column.m_defWidth;
	m_minWidth			= column.m_minWidth;
	m_maxWidth			= column.m_maxWidth;
	m_scaleWeight		= column.m_scaleWeight;
	m_pixelWidth		= column.m_pixelWidth;
	m_pItem				= 0;
//	m_pItem				= column.m_pItem;			//NEEDS TO BE CLONED!
	m_itemOrigo			= column.m_itemOrigo;

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

	if( m_pItem )
		delete m_pItem;
}


void WgTableColumn::SetWidth( int pixels )
{
	if( pixels < m_minWidth || pixels > m_maxWidth )
		return;

	if( m_defWidth != pixels )
	{
		Sint32 widthdiff = pixels - m_defWidth;
		m_defWidth = pixels;
		if( m_bVisible && m_pOwner )
		{
			m_pOwner->SetContentSize( m_pOwner->m_contentWidth + widthdiff, m_pOwner->m_contentHeight );
			m_pOwner->RecalcColumnWidths();
			m_pOwner->AdaptCellsToWidth();
			m_pOwner->RequestRender();
		}
	}
}

void WgTableColumn::SetContentWidth( int pixels )
{
	if( m_pOwner )
		pixels += m_pOwner->GetCellPaddingX()*2;

	SetWidth( pixels );
}

int WgTableColumn::GetContentWidth() const
{
	if( m_pOwner )
		return ((int)m_pixelWidth) - m_pOwner->GetCellPaddingX()*2;
	else
		return ((int)m_pixelWidth);
}


void WgTableColumn::SetMinWidth( int pixels )
{
	if( pixels < 4 || pixels > m_maxWidth )
		return;

	m_minWidth = pixels;

	if( m_defWidth < pixels )
		m_defWidth = pixels;

	if( m_pOwner )
		m_pOwner->RecalcColumnWidths();
}


void WgTableColumn::SetMaxWidth( int pixels )
{
	if( pixels < m_minWidth )
		return;

	m_maxWidth = pixels;

	if( m_defWidth > pixels )
		m_defWidth = pixels;

	if( m_pOwner )
		m_pOwner->RecalcColumnWidths();
}


void WgTableColumn::SetScaleWeight( float weight )
{
	if( weight < 0.f )
		return;

	m_scaleWeight = weight;
	if( m_pOwner )
		m_pOwner->RecalcColumnWidths();
}




void WgTableColumn::Hide()
{
	if( m_bVisible )
	{
		m_bVisible = false;

		if( m_pOwner )
		{
			m_pOwner->SetContentSize( m_pOwner->m_contentWidth - (int)m_pixelWidth, m_pOwner->m_contentHeight );
			m_pOwner->RequestRender();
			m_pOwner->RecalcColumnWidths();
		}
	}
}

void WgTableColumn::Show()
{
	if( !m_bVisible )
	{
		m_bVisible = true;

		if( m_pOwner )
		{
			m_pOwner->SetContentSize( m_pOwner->m_contentWidth + (int)m_pixelWidth, m_pOwner->m_contentHeight );
			m_pOwner->RequestRender();
			m_pOwner->RecalcColumnWidths();
		}
	}
}

void WgTableColumn::Enable()
{
	if( !m_bEnabled )
	{
		m_bEnabled = true;
		if( m_bVisible && m_pOwner )
			m_pOwner->RequestRender();
	}
}

void WgTableColumn::Disable()
{
	if( m_bEnabled )
	{
		m_bEnabled = false;
		if( m_bVisible && m_pOwner )
			m_pOwner->RequestRender();
	}
}


void WgTableColumn::SetSortFunction( fpItemCmp pFunc )
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

WgText* WgTableColumn::GetTextObj()
{
	return m_pText;
}

void WgTableColumn::_textModified()
{
	if( m_bVisible && m_pOwner )
		m_pOwner->RequestRender();
}

void WgTableColumn::SetItem( WgItem * pItem )
{
	m_pItem = pItem;
	if( m_bVisible && m_pOwner )
		m_pOwner->RequestRender();
}

void WgTableColumn::SetItemPos( const WgOrigo& origo, WgCoord ofs )
{
	m_itemOrigo = origo;
	m_itemOfs = ofs;
	if( m_bVisible && m_pOwner )
		m_pOwner->RequestRender();
}

void WgTableRow::SetItem( WgItem * pItem, Uint32 col )
{
	while( WgItemRow::NbItems() < col )
		WgItemRow::AddItem( new WgItemPixmap() );

	if( WgItemRow::NbItems() > col )
		WgItemRow::DeleteItem( col );

	WgItemRow::InsertItem( pItem, col );

}

void WgTableRow::AddItem( WgItem * pItem )
{
	WgItemRow::AddItem(pItem);
}

bool WgTableRow::Select( bool bSelectItems )
{
	if( !WgItemRow::Select() )
		return false;

	if( bSelectItems )
	{
		WgItem * pCell = GetFirstItem();
		while( pCell )
		{
			pCell->Select();
			pCell = pCell->GetNext();
		}
	}
	return true;
}


void WgTableRow::Unselect( bool bUnselectItems )
{
	WgItemRow::Unselect();

	if( bUnselectItems )
	{
		WgItem * pCell = GetFirstItem();
		while( pCell )
		{
			pCell->Unselect();
			pCell = pCell->GetNext();
		}
	}
}

WgRect WgTableRow::RequestItemGeo( WgItem * pItem )
{
	int colNb = GetItemPos(pItem);
	int rowNb = m_pMyHolder->GetItemPos(this);

	WgRect cellGeo = ((Wdg_TableView*)m_pMyHolder)->GetCellGeo(rowNb,colNb);
	WgRect screenGeo = ((Wdg_TableView*)m_pMyHolder)->ScreenGeometry();

	return cellGeo + screenGeo.Pos();
}



//____ NewOfMyType() __________________________________________________________

WgWidget * Wdg_TableView::NewOfMyType() const
{
	return new Wdg_TableView;
}

//____ Init() _________________________________________________________________

void Wdg_TableView::Init( void )
{
	m_bShowHeader 		= true;
	m_bAutoScaleHeader	= false;
	m_emptyRowHeight	= 0;

	m_nColumns			= 0;
	m_pColumns			= 0;

	m_sortMarkerOrigo	= WgOrigo::midRight();
	m_sortMarkerOfs.x	= 0;
	m_sortMarkerOfs.y	= 0;
	m_clickSortPrio		= 0;

	m_cellPaddingX		= 0;
	m_cellPaddingY		= 0;

	m_pLastMarkedHeaderItem = 0;

	m_pLastMarkedItem = 0;
	m_lastClickedRow = -1;
	m_lastClickedColumn = -1;

	m_bAutoScrollMarked	= false;

	m_lastSortColumn				= 0;
	m_lastSortColumnAscendStatus	= true;

	m_pMarkedHeader = NULL;
	m_pPressedHeader = NULL;

	m_markedRow = -1;
	m_markedColumn = -1;
}

//____ Destructor _____________________________________________________________

Wdg_TableView::~Wdg_TableView( void )
{
	RemoveColumns();

	delete [] m_pColumns;
	m_pColumns = 0;							// So that refresh methods called when underlying items are deleted knows...
}


//____ Type() _________________________________________________________________

const char * Wdg_TableView::Type( void ) const
{
	return GetMyType();
}

const char * Wdg_TableView::GetMyType( void )
{
	return Wdg_Type;
}

//____ SetColumnOrder() _______________________________________________________

bool Wdg_TableView::SetColumnOrder( std::vector<int> columns )
{
	if( columns.size() != m_nColumns )
		return false;

	// Some quick and simple error check to make us reasonably 
	// confident that no column is specified twice and that no 
	// non-existing columns are specified.

	int total = 0;
	int acceptedTotal = 0;
	for( int i = 0 ; i < m_nColumns ; i++ )
	{
		total += columns[i];
		acceptedTotal += i;
	}

	if( total != acceptedTotal )
		return false;

	// Copy content

	for( int i = 0 ; i < m_nColumns ; i++ )
		m_columnOrder[i] = columns[i];

	// Needed since we now might have an new last visible column

	RecalcColumnWidths();

	return true;
}

//____ ResetColumnOrder() _____________________________________________________

void Wdg_TableView::ResetColumnOrder()
{
	assert( m_columnOrder.size() == m_nColumns );

	for( int i = 0 ; i < m_nColumns ; i++ )
		m_columnOrder[i] = i;
}

//____ SetHeaderSource() ______________________________________________________

bool Wdg_TableView::SetHeaderSource( const WgBlockSetPtr& pHeaderNormalColumn, const WgBlockSetPtr& pHeaderSelectedColumn )
{
	m_pHeaderGfxNormal		= pHeaderNormalColumn;
	m_pHeaderGfxSelected	= pHeaderSelectedColumn;

	if( m_bShowHeader )
		RequestRender();

	return true;
}

/*
//____ SetLineMarkSource() ____________________________________________________

void Wdg_TableView::SetLineMarkSource( WgBlockSetPtr pGfx )
{
	m_pMarkedLineGfx = pGfx;
	RequestRender();
}
*/

//____ SetArrowSource() _______________________________________________________

void Wdg_TableView::SetArrowSource( const WgBlockSetPtr& pAscend, const WgBlockSetPtr& pDescend )
{
	m_pAscendGfx	= pAscend;
	m_pDescendGfx	= pDescend;

	if( m_bShowHeader )
		RequestRender();
}

//____ SetArrowPos() __________________________________________________________

void Wdg_TableView::SetArrowPos( const WgOrigo& origo, Sint32 xOfs, Sint32 yOfs )
{
	m_sortMarkerOrigo 	= origo;
	m_sortMarkerOfs.x	= xOfs;
	m_sortMarkerOfs.y	= yOfs;

	if( m_bShowHeader )
		RequestRender();
}

//____ SetArrowPos() __________________________________________________________

void Wdg_TableView::SetArrowPos( Sint32 xOfs, Sint32 yOfs )
{
	m_sortMarkerOfs.x	= xOfs;
	m_sortMarkerOfs.y	= yOfs;

	if( m_bShowHeader )
		RequestRender();
}

//____ SetArrowOrigo() __________________________________________________________

void Wdg_TableView::SetArrowOrigo( WgOrigo origo )
{
	m_sortMarkerOrigo 	= origo;

	if( m_bShowHeader )
		RequestRender();
}

//____ SetCellPadding() _______________________________________________________

void Wdg_TableView::SetCellPadding( Uint8 x, Uint8 y )
{
	m_cellPaddingX = x;
	m_cellPaddingY = y;

	RequestRender();
}

//____ SetEmptyRowHeight() ____________________________________________________

void Wdg_TableView::SetEmptyRowHeight( Uint32 height )
{
	if( height != m_emptyRowHeight )
	{
		m_emptyRowHeight = height;
		RequestRender();
	}
}

//____ SetRowColors() _________________________________________________________

void Wdg_TableView::SetRowColors( const WgColorSetPtr& pOddColors, const WgColorSetPtr& pEvenColors )
{
	m_pRowColors[0] = pOddColors;
	m_pRowColors[1] = pEvenColors;

	RequestRender();
}

//____ RemoveRowColors() ______________________________________________________
void Wdg_TableView::RemoveRowColors()
{
	m_pRowColors[0] = 0;
	m_pRowColors[1] = 0;

	RequestRender();
}

//____ SetRowBlocks() _________________________________________________________

void Wdg_TableView::SetRowBlocks( const WgBlockSetPtr& pOddBlocks, const WgBlockSetPtr& pEvenBlocks )
{
	m_pRowBlocks[0] = pOddBlocks;
	m_pRowBlocks[1] = pEvenBlocks;

	RequestRender();
}

//____ RemoveRowBlocks() ______________________________________________________
void Wdg_TableView::RemoveRowBlocks()
{
	m_pRowBlocks[0] = 0;
	m_pRowBlocks[1] = 0;

	RequestRender();
}


//____ RemoveColumns() ____________________________________________________________

void Wdg_TableView::RemoveColumns()
{
	UpdateMarkedRowColumn(-1, -1);

	delete [] m_pColumns;
	m_pColumns = 0;
	m_nColumns = 0;

	m_columnOrder.clear();
}

//____ RecalcColumnWidths() ___________________________________________________

void Wdg_TableView::RecalcColumnWidths()
{
	for( int i = 0 ; i < m_nColumns ; i++ )
		m_pColumns[i].m_pixelWidth = (float) m_pColumns[i].m_defWidth;


	int wantedWidth = ViewPixelLenX();

	if( m_bAutoScaleHeader )
	{
		// Get some values we need

		int totalDefWidth = 0;
		float totalPixelWeight = 0;

		for( int i = 0 ; i < m_nColumns ; i++ )
		{
			if( m_pColumns[i].m_bVisible )
			{
				totalDefWidth	+= m_pColumns[i].m_defWidth;
				totalPixelWeight += m_pColumns[i].m_scaleWeight * m_pColumns[i].m_defWidth;
			}
		}

		float	extraWidth = (float) (wantedWidth - totalDefWidth);

		// Resize columns

		float	nextTotalWeight = totalPixelWeight;					// Total weight for next iteration
		bool	bKeepGoing = true;

		while( extraWidth >= 1.f && bKeepGoing )		// Take care of growing
		{
			bKeepGoing = false;
			for( int i = 0 ; i < m_nColumns ; i++ )
			{
				if( m_pColumns[i].m_bVisible && m_pColumns[i].m_defWidth != 0 && m_pColumns[i].m_pixelWidth < m_pColumns[i].m_maxWidth )
				{
					bKeepGoing = true;

					float part = ((m_pColumns[i].m_scaleWeight * m_pColumns[i].m_defWidth)/totalPixelWeight) * extraWidth;
					if( part + m_pColumns[i].m_pixelWidth > m_pColumns[i].m_maxWidth )
					{
						extraWidth -= m_pColumns[i].m_maxWidth - m_pColumns[i].m_pixelWidth;
						m_pColumns[i].m_pixelWidth = (float) m_pColumns[i].m_maxWidth;

						part = m_pColumns[i].m_maxWidth - m_pColumns[i].m_pixelWidth;
						nextTotalWeight -= m_pColumns[i].m_scaleWeight * m_pColumns[i].m_defWidth;
					}
					else
					{
						extraWidth -= part;
						m_pColumns[i].m_pixelWidth += part;
					}
				}
			}
			totalPixelWeight = nextTotalWeight;
		}
/*		else if( wantedWidth < totalDefWidth )	// Take care of shrinking
		{
			for( unsigned int i = 0 ; i < m_nColumns ; i++ )
			{
				if( m_pColumns[i].m_bVisible )
				{
					int width = (int) (m_pColumns[i].m_defWidth - (m_pColumns[i].m_scaleWeight * m_pColumns[i].m_defWidth)/totalPixelWeight*(totalDefWidth-wantedWidth));
					if( width < m_pColumns[i].m_minWidth )
						width = m_pColumns[i].m_minWidth;

					m_pColumns[i].m_pixelWidth = width;
				}
			}
		}
*/
		TweakColumnWidths( wantedWidth );
	}

	ExtendLastColumn( wantedWidth );
}


//____ ExtendLastColumn() _____________________________________________________

void Wdg_TableView::ExtendLastColumn( int targetWidth )
{
	int totalWidth = 0;
	for( int i = 0 ; i < m_nColumns ; i++ )
		totalWidth += (int) m_pColumns[i].m_pixelWidth;

	if( totalWidth < targetWidth && m_nColumns > 0 )
	{
		// Find the rightmost visible column.

		int col = m_nColumns-1;
		WgTableColumn * p = &m_pColumns[GetColumnAtPosition(col)];
		
		while( !p->m_bVisible )
		{
			if( col == 0 )
				return;								// No visible column to extend.
			col--;
			p = &m_pColumns[GetColumnAtPosition(col)];
		}

		p->m_pixelWidth += (float) (targetWidth - totalWidth);
	}
}


//____ TweakColumnWidths() ____________________________________________________

void Wdg_TableView::TweakColumnWidths( int targetWidth )
{
	// Make table the width of target width by adding pixels to the columns closest to being wider.

	int widthDiff = targetWidth;
	for( int i = 0 ; i < m_nColumns ; i++ )
		widthDiff -= (int) m_pColumns[i].m_pixelWidth;

	for( int loop = 0 ; loop < widthDiff ; loop++ )
	{
		float bestDecimals = 0.f;
		int	  bestColumn = -1;

		for( int i = 0 ; i < m_nColumns ; i++ )
		{
			float decimals = m_pColumns[i].m_pixelWidth - (int)m_pColumns[i].m_pixelWidth;
			if( decimals >= bestDecimals && m_pColumns[i].m_pixelWidth < m_pColumns[i].m_maxWidth )
			{
				bestDecimals = decimals;
				bestColumn = i;
			}
		}
		
		if( bestColumn != -1 )
			m_pColumns[bestColumn].m_pixelWidth = (float) ((int) (m_pColumns[bestColumn].m_pixelWidth + 1.f));
	}

	// Remove the deciamals

	for( int i = 0 ; i < m_nColumns ; i++ )
		m_pColumns[i].m_pixelWidth = (float) ((int) m_pColumns[i].m_pixelWidth);
}




//____ AddColumn() ____________________________________________________________

Uint32 Wdg_TableView::AddColumn( const char * pText, Uint32 pixelwidth, WgOrigo& headerAlign, fpItemCmp fpCompare, bool bInitialAscend, bool bEnabled, int id )
{
	WgTableColumn * pCol = new WgTableColumn[m_nColumns+1];

	for( int i = 0 ; i < m_nColumns ; i++ )
	{
		pCol[i] = m_pColumns[i];
		m_pColumns[i].m_pText = 0;			// To avoid deletion of text object further down.
		m_pColumns[i].m_pItem = 0;
	}
	pCol[m_nColumns].m_defWidth = pixelwidth;

	WgText * p = new WgText( pText );
	p->SetWrap(false);
	pCol[m_nColumns].SetTextObj(p);

	pCol[m_nColumns].GetTextObj()->setAlignment(headerAlign);
	pCol[m_nColumns].m_fpCompare = fpCompare;
	pCol[m_nColumns].m_bVisible = true;
	pCol[m_nColumns].m_bEnabled = bEnabled;
	pCol[m_nColumns].m_bInitialAscend = bInitialAscend;
	pCol[m_nColumns].m_id = id;
	pCol[m_nColumns].m_pOwner = this;

	if( m_pColumns )
		delete [] m_pColumns;
	m_pColumns = pCol;
	m_columnOrder.push_back(m_nColumns);
	m_nColumns++;


	RecalcColumnWidths();
	SetContentSize( m_contentWidth + pixelwidth, m_contentHeight );
	RequestRender();

	return m_nColumns-1;
}


//____ FindColumn() ___________________________________________________________

WgTableColumn* Wdg_TableView::FindColumn(Uint32 id) const
{
	for( int i = 0 ; i < m_nColumns ; i++ )
		if( m_pColumns[i].m_id == (int) id )
			return &m_pColumns[i];

	return 0;
}

//____ GetColumnAtPosition() ___________________________________________________

int Wdg_TableView::GetColumnAtPosition( int position ) const
{
	return m_columnOrder[position];
}

//____ GetItemAtPosition() _____________________________________________________

WgItem * Wdg_TableView::GetItemAtPosition( int position, WgTableRow * pRow )
{
	int ofs = m_columnOrder[position];

	WgItem * p = pRow->GetFirstItem();

	for( int i = 0 ; i < ofs && p != 0 ; i++ )
		p = p->Next();

	return p;
}

//____ GetPositionOfColumn() __________________________________________________

int Wdg_TableView::GetPositionOfColumn( int col )
{
	for( int i = 0 ; i < m_nColumns ; i++ )
		if( col == m_columnOrder[i] )
			return i;

	assert(false);			// Should never get here.
	return -1;
}



//____ GetSortColumn() _____________________________________________________

bool Wdg_TableView::GetSortColumn( Uint32 order, Uint32& columnIndex, bool& bAscend )
{
	if( order >= (Uint32) c_nSortColumns )
		return false;

	if( 0xFFFF == m_sortStack[ order ].column )
		return false;

	columnIndex = m_sortStack[ order ].column;
	bAscend = m_sortStack[ order ].bAscend;

	return true;
}

//____ GetCell() ______________________________________________________________

WgItem* Wdg_TableView::GetCell( Uint32 row, Uint32 column )
{
	WgTableRow* pRow = GetRow(row);
	if( pRow )
		return pRow->GetItem(column);

	return 0;
}

//____ GetCellGeo() ___________________________________________________________

WgRect Wdg_TableView::GetCellGeo( int row, int column )
{

	WgRect	r( 0 - m_viewPixOfsX, 0 - m_viewPixOfsY, 0, 0 );

	// Adjust for header

	r.y += GetHeaderHeight();


	WgTableRow * pRow = (WgTableRow *) m_items.First();

	// Forward to our row
	for( int i = 0 ; i < row ; i++ )
	{
		if( !pRow )
			return WgRect();							// Invalid row.

		if( pRow->IsVisible() )
			r.y += pRow->Height() + m_cellPaddingY*2;

		pRow = (WgTableRow *) pRow->Next();
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

	for( int i = 0 ; i <= column ; i++ )
	{
		if( m_pColumns[i].m_bVisible )
		{
			r.w = (int) m_pColumns[i].m_pixelWidth;
//			if( i == m_nColumns-1 && r.x + r.w < _window.x + _window.w )
//				r.w = _window.x + _window.w - r.x;		// Last column stretches to end of tableview...

			r.x += m_cellPaddingX;
			r.w -= m_cellPaddingX*2;

			if( i != column )
				r.x += ((int) m_pColumns[i].m_pixelWidth) - m_cellPaddingX;		// One cellpadding already added...
		}
	}

	return r;
}

//____ GetHeaderGeo() _________________________________________________________

WgRect Wdg_TableView::GetHeaderGeo( int column )
{
	if( column >= (int) m_nColumns )
		return WgRect();							// Invalid column;


	WgRect	r( 0 - m_viewPixOfsX, 0 - m_viewPixOfsY, 0, GetHeaderHeight() );

	// Go through columns, determine x-pos and width. 

	for( int i = 0 ; i <= column ; i++ )
	{
		if( m_pColumns[i].m_bVisible )
		{
			r.w = (int) m_pColumns[i].m_pixelWidth;

			r.x += m_cellPaddingX;
			r.w -= m_cellPaddingX*2;

			if( i != column )
				r.x += ((int) m_pColumns[i].m_pixelWidth) - m_cellPaddingX;		// One cellpadding already added...
		}
	}

	return r;
}



WgTableRow *	Wdg_TableView::RemoveRow( Uint32 pos )
{
	if( pos == m_markedRow )
		UpdateMarkedRowColumn(-1,m_markedColumn);

	WgTableRow* pRow = (WgTableRow*)RemoveItem(pos);
	if(pRow == m_pLastMarkedItem)
		m_pLastMarkedItem = 0;
	return pRow;
}

bool Wdg_TableView::RemoveRow( WgTableRow * pRow )
{
	if( GetRowNb(pRow) == m_markedRow )
		UpdateMarkedRowColumn(-1,m_markedColumn);

	if(pRow == m_pLastMarkedItem)
		m_pLastMarkedItem = 0;
	return RemoveItem( pRow );
}

void Wdg_TableView::RemoveAllRows()
{
	UpdateMarkedRowColumn(-1,m_markedColumn);

	m_pLastMarkedItem = 0;
	return RemoveAllItems();
}

bool Wdg_TableView::DeleteRow( Uint32 pos )
{
	if( pos == m_markedRow )
		UpdateMarkedRowColumn(-1,m_markedColumn);

	if(m_pLastMarkedItem == m_items.Get(pos))
		m_pLastMarkedItem = 0;
	return DeleteItem(pos);
}

bool Wdg_TableView::DeleteRow( WgTableRow * pRow )
{
	if( GetRowNb(pRow) == m_markedRow )
		UpdateMarkedRowColumn(-1,m_markedColumn);

	if(pRow == m_pLastMarkedItem)
		m_pLastMarkedItem = 0;
	return DeleteItem(pRow);
}

void Wdg_TableView::DeleteAllRows()
{
	UpdateMarkedRowColumn(-1,m_markedColumn);

	m_pLastMarkedItem = 0;
	return DeleteAllItems();
}


//____ GetPointerStyle() ________________________________________

WgPointerStyle Wdg_TableView::GetPointerStyle() const
{
	WgPointerStyle style = WG_POINTER_DEFAULT;

	if( m_pLastMarkedItem )
		style = m_pLastMarkedItem->GetPointerStyle();

	if( style == WG_POINTER_DEFAULT )
		style = m_pointerStyle;

	return style; 
}


//____ SetClickSortPrio() _____________________________________________________

bool Wdg_TableView::SetClickSortPrio( Uint8 prio )
{
	if( prio >= c_nSortColumns )
		return false;

	m_clickSortPrio = prio;
	return true;
}


//____ SortRows() _____________________________________________________________

bool Wdg_TableView::SortRows( Uint32 column, bool bAscend, Uint8 prio )
{
	if( (int) column >= m_nColumns || prio > c_nSortColumns )
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


//	SetSortOrder( bAscend );
 	SortItems();
	return true;
}

//____ CompareItems() _________________________________________________________

Sint32 Wdg_TableView::CompareItems( WgItem * pItem1, WgItem * pItem2 )
{
	WgSortContext	context;

	for( int n = 0 ; n < c_nSortColumns ; n++ )
	{
		int col = m_sortStack[n].column;

		if( col < m_nColumns && m_pColumns[col].m_fpCompare != 0 )
		{
			context.bAscend = m_sortStack[n].bAscend;

			int diff = m_pColumns[col].m_fpCompare( GetExtendedCellContent( (WgTableRow*)pItem1, col ), GetExtendedCellContent( (WgTableRow*)pItem2, col ), context );

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

//____ GetExtendedCellContent() _______________________________________________

WgItem * Wdg_TableView::GetExtendedCellContent( WgTableRow * pRow, int col )
{
	WgItem * pContent = pRow->GetItem(col);	

	// Look through previous column positions in table to find the one extending over our cell.

	if( !pContent )
	{
		int pos = GetPositionOfColumn(col);
	
		while( pos > 0 && !pContent )
		{
			pos--;
			pContent = GetItemAtPosition( pos, pRow );
		}
	}
	
	return pContent;
}


//____ SetHeaderTextProp() ____________________________________________________

bool Wdg_TableView::SetHeaderTextProp( const WgTextPropPtr& pProp )
{
	m_pHeaderProps = pProp;
	RequestRender();
	return true;
}


//____ ShowHeader() ___________________________________________________________

void Wdg_TableView::ShowHeader( bool bShow )
{
	if( bShow != m_bShowHeader )
	{
		int newHeight = m_contentHeight;
		if( bShow )
		{
			m_bShowHeader = bShow;
			newHeight += GetHeaderHeight();
		}
		else
		{
			newHeight -= GetHeaderHeight();
			m_bShowHeader = bShow;
		}


		SetContentSize( m_contentWidth, newHeight );
		RequestRender();
	}
}

//____ GetHeaderHeight() ______________________________________________________

int Wdg_TableView::GetHeaderHeight() const
{
	if( !m_bShowHeader )
		return 0;

	int h = 0;

	if( m_pHeaderGfxNormal )
		h = m_pHeaderGfxNormal->Height();

	if( m_pHeaderGfxSelected && m_pHeaderGfxSelected->Height() > h )
		h = m_pHeaderGfxSelected->Height();

	return h;
}


//____ UpdateContentSize() _________________________________________________________

void Wdg_TableView::UpdateContentSize()
{
	Uint32	contentWidth = 0;
	Uint32	contentHeight = 0;

	// Calc contentHeight

	WgItem * p = m_items.First();

	while( p )
	{
		if( p->IsVisible() )
			contentHeight += p->Height() + m_cellPaddingY*2;
		p = p->Next();
	}

	// Calc contentWidth

	for( int i = 0 ; i < m_nColumns ; i++ )
	{
		if( m_pColumns[i].m_bVisible )
			contentWidth += (int) m_pColumns[i].m_pixelWidth;
	}

	// Possibly add header to height (if we have one)

	contentHeight += GetHeaderHeight();

	// Set size and request render

	SetContentSize( contentWidth, contentHeight );
}


//____ refreshItems() _________________________________________________________

void Wdg_TableView::refreshItems()
{
	if( m_pColumns == 0 )
		return;

	UpdateContentSize();

	// Check so we still have our m_pLastMarkedItem...

	// hmm.... well this only compares m_pLastMarkedItem to all the WgTableRows,
	// which seems wrong.  / Viktor

	if( m_pLastMarkedItem )
	{
		WgItem * p = m_items.First();

		while( p )
		{
			if( p == m_pLastMarkedItem )
			{
				if( p->IsHidden() )
					p = 0;
				break;
			}
			p = p->Next();
		}
		if( p == 0 )
			m_pLastMarkedItem = 0;
	}

	RequestRender();
}

//____ ItemSizeModified() _________________________________________________________

void Wdg_TableView::ItemSizeModified( WgItem * pItem, Sint32 widthDiff , Sint32 heightDiff )
{
	if( m_pColumns == 0 )
		return;

//	UpdateContentSize();

	if( ! pItem->IsHidden() )
	{
		SetContentSize(m_contentWidth, m_contentHeight+heightDiff);
		RequestRender();		//TODO: This can be optimized, we might not need to render the whole view.
	}

}

//____ ItemVisibilityModified() _______________________________________________

void Wdg_TableView::ItemVisibilityModified( WgItem * pItem, bool bVisible )
{
	if( m_pColumns == 0 )
		return;

	int heightDiff = pItem->Height() + m_cellPaddingY*2;
	if( !bVisible )
		heightDiff = -heightDiff;

	SetContentSize(m_contentWidth, m_contentHeight+heightDiff);
	RequestRender();		//TODO: This can be optimized, we might not need to render the whole view.
}


//____ ItemMarkChanged() ________________________________________________

void Wdg_TableView::ItemMarkChanged( WgItem * pItem, bool bMarked )
{
	if( m_bAutoScrollMarked )
	{
		if( bMarked )		// make sure marked item is in view
			ScrollIntoView( (WgTableRow*)pItem );
	}
}

//____ ScrollIntoView() ________________________________________________
void Wdg_TableView::ScrollIntoView( WgTableRow* pRow )
{
	if( !pRow || pRow->IsHidden() )
		return;

	// calc y pos of this row
	Uint32 itemPosY = 0;

	WgItem * p = m_items.First();
	while( p && p != pRow )
	{
		if(p->IsVisible())
			itemPosY += p->Height() + m_cellPaddingY*2;
		p = p->Next();
	}

	if( !p )
		return;		// item not found


	Uint32 viewOfsMin = ViewPixelOfsY();
	Uint32 viewOfsMax = ViewPixelOfsY() + ViewPixelLenY();

	if( itemPosY < viewOfsMin )							// is the row above the view?
		SetViewPixelOfsY( itemPosY );
	else if( GetHeaderHeight() + itemPosY + pRow->Height() + m_cellPaddingY*2 > viewOfsMax )	// is the row below the view?
		SetViewPixelOfsY( GetHeaderHeight() + itemPosY + pRow->Height() + m_cellPaddingY*2 - ViewPixelLenY() );
}

//____ DoMyOwnGeometryChange() ________________________________________________

void Wdg_TableView::DoMyOwnGeometryChange( WgRect& oldGeo, WgRect& newGeo )
{
	Wdg_Baseclass_View::DoMyOwnGeometryChange( oldGeo, newGeo );
}


//____ DoMyOwnGeometryChangeSubclass() ________________________________________________

void Wdg_TableView::DoMyOwnGeometryChangeSubclass( WgRect& oldGeo, WgRect& newGeo )
{
	if( oldGeo.w != newGeo.w )
	{
//		AdaptItemsToWidth( newGeo.w );		// this won't do anything since the items are WgTableRows
		RecalcColumnWidths();
		AdaptCellsToWidth();
		UpdateContentSize();
	}
}

//____ AdaptCellsToWidth() ____________________________________________________

void Wdg_TableView::AdaptCellsToWidth()
{
	WgTableRow * pRow = (WgTableRow *) m_items.First();
	while( pRow )
	{
		WgItem *	pCell = pRow->GetFirstItem();

		for( int i = 0 ; i < m_nColumns && pCell != 0 ; i++ )
		{
			if( m_pColumns[i].m_bVisible )
			{
				Sint32 width = (int) m_pColumns[i].m_pixelWidth;
				width -= m_cellPaddingX*2;
				pCell->AdaptToWidth( width );

			}
			pCell = pCell->GetNext();
		}

		pRow->refreshItems();
		pRow = (WgTableRow *) pRow->GetNext();
	}
}


//____ GetMarkedItem() ________________________________________________________

WgItem* Wdg_TableView::GetMarkedItem( Uint32 x, Uint32 y )
{
	Uint32		yOfs = 0;
	Uint32		xOfs = 0;
	int			column = 0;
	WgTableRow *	pRow = 0;

	// Get our marked row, column and offsets within cell.

	if( GetMarkedRow(y, pRow, yOfs ) == -1 )
		return 0;									// No row marked, so consequently no item marked either.

	column = GetMarkedColumn(x, xOfs);
	if( column == -1 )
		return 0;									// No column marked, so consequently no item marked either.


	// Cell found, check so we are inside the cells padding.

	int scaledW = (int) m_pColumns[column].m_pixelWidth;
	if( xOfs < m_cellPaddingX || (int) xOfs - m_cellPaddingX > scaledW )
		return 0;

	if( yOfs < m_cellPaddingY || yOfs - m_cellPaddingY > pRow->Height() )
		return 0;


	// Step through items until we get the right one...

	WgItem * p = pRow->GetFirstItem();
	if( p == 0 )
		return 0;									// No items in this row.

	for( int i = 0 ; i < column ; i++ )
	{
		p = p->Next();
		if( p == 0 )
			return 0;								// Items ended on this row before we reached our column.
	}


	// Return the item

	return p;
}

//____ GetMarkedRow() _________________________________________________________

int Wdg_TableView::GetMarkedRow( Uint32 y, WgTableRow*& pSaveRow, Uint32& saveYOfs )
{
	pSaveRow	= 0;
	saveYOfs	= 0;
	int row		= 0;

	// Transform from relative widget cordinates to content-cordinates.

	int headerHeight = GetHeaderHeight();
	if( (int)y < headerHeight )
		return -1;	// on header.
	y -=  headerHeight;

	y += m_viewPixOfsY;

	//

	if( y < 0 )
		return -1;

	//

	WgItem * p	= m_items.First();

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
		p = p->Next();
		row++;
	}
	return -1;									// Rows ended before mark-point.
}

//____ GetMarkedColumn() ______________________________________________________

int Wdg_TableView::GetMarkedColumn( Uint32 x, Uint32& saveXOfs )
{
	saveXOfs	= 0;

	// Transform from relative widget cordinates to content-cordinates.

	x += m_viewPixOfsX;

	//

	if( x < 0 )
		return -1;

	// Loop through columns to find out which one we are inside.

	for( int _col = 0 ; _col < m_nColumns ; _col++ )
	{
		int col = GetColumnAtPosition( _col );

		if( m_pColumns[col].m_bVisible )
		{
			Uint32 scaledW = (unsigned int) m_pColumns[col].m_pixelWidth;
			if( x < scaledW )
			{
				saveXOfs	= x;
				return col;
			}

			x -= scaledW - 1;
		}
	}

	return -1;									// Columns ended before mark-point.
}


//____ ItemAdded() _________________________________________________________

void Wdg_TableView::ItemAdded( WgItem * pItem )
{
	// Set size before refreshItems(), so it can modify size relatively (although its a bit inefficient...)

	if( !((WgTableRow*)pItem)->IsHidden() )
		SetContentSize( m_contentWidth, m_contentHeight + pItem->Height() + m_cellPaddingY*2 );

	// This should really be done BEFORE the item is added, now we might affect content size again,
	// several times over in worst case!

	WgItemRow * pRow = (WgItemRow*) pItem;
	WgItem * pI = pRow->GetFirstItem();

	for( int n = 0 ; n < m_nColumns && pI ; n++ )
	{
		pI->AdaptToWidth( m_pColumns[n].GetWidth() - m_cellPaddingX*2 );
		pI = pI->Next();
	}

	pRow->refreshItems();

	// Request render

	RequestRender();
}

void Wdg_TableView::SetAutoScaleHeaders(bool autoScaleHeaders)
{
	if(m_bAutoScaleHeader != autoScaleHeaders)
	{
		m_bAutoScaleHeader = autoScaleHeaders;
		RequestRender();
	}
}

//____ _headerItemGeo() _____________________________________________________________

WgRect Wdg_TableView::_headerItemGeo( WgTableColumn * pHeader, const WgRect& headerGeo ) const
{
	WgRect itemGeo;
	if( pHeader && pHeader->m_pItem && headerGeo.w >0 && headerGeo.h > 0 )
	{
		itemGeo.w = pHeader->m_pItem->Width();
		itemGeo.h = pHeader->m_pItem->Height();

		itemGeo.x = pHeader->m_itemOrigo.calcOfsX(headerGeo.w, itemGeo.w) + pHeader->m_itemOfs.x + headerGeo.x;
		itemGeo.y = pHeader->m_itemOrigo.calcOfsY(headerGeo.h, itemGeo.h) + pHeader->m_itemOfs.y + headerGeo.y;
	}

	return itemGeo;
}

//____ DoMyOwnRender() ________________________________________________________
void Wdg_TableView::DoMyOwnRender( const WgRect& _window, const WgRect& _clip, Uint8 _layer )
{
	WgRect	r( _window.x - m_viewPixOfsX, _window.y - m_viewPixOfsY, m_contentWidth, m_contentHeight );
	WgRect	clipView = _clip;

	if( r.w < _window.w )
		r.w = _window.w;

	// Draw header (if any)

	int headerHeight = GetHeaderHeight();
	if( headerHeight > 0 )
	{
		WgRect	r2 = r;
		r2.y = _window.y;						// Header is glued to top of view.
		r2.h = headerHeight;
		for( int _i = 0 ; _i < m_nColumns ; _i++ )
		{
			int i = GetColumnAtPosition(_i);

			if( !m_pColumns[i].m_bVisible )
				continue;

            // don't draw columns that are outside of the window
			if( r2.x >= _window.x + _window.w )
				break;

			r2.w = (int)m_pColumns[i].m_pixelWidth +1;		// +1 to compensate for pixel overlap-hack further down.
			//if( i == m_nColumns-1 && r2.x + r2.w < _window.x + _window.w )
			if( i == m_nColumns-1 )
				r2.w = (int)m_pColumns[i].m_pixelWidth;		// Don't compensate on last column header
//				r2.w = _window.x + _window.w - r2.x;		// Last column header stretches to end of tableview...

			WgMode mode = WG_MODE_NORMAL;

			if(&m_pColumns[i]==m_pMarkedHeader )
			{
				if( m_pPressedHeader == m_pMarkedHeader )
					mode = WG_MODE_SELECTED;
				else if( m_pPressedHeader == 0 )
					mode = WG_MODE_MARKED;
			}
			WgBlockSetPtr pHeaderGfx;
			if( i==m_lastSortColumn )				// See if column is the selected one.
				pHeaderGfx = m_pHeaderGfxSelected;
			else
				pHeaderGfx = m_pHeaderGfxNormal;
			
			WgGfx::clipBlitBlock( _clip, pHeaderGfx->GetBlock(mode,r2), r2 );

			// Print text

			if( pHeaderGfx )
				m_pColumns[i].SetTextBaseColors(pHeaderGfx->TextColors());

			WgRect rText = r2;
			if( pHeaderGfx )
				rText.Shrink( pHeaderGfx->ContentBorders() );

			m_pColumns[i].GetTextObj()->setProperties( m_pHeaderProps );
			WgGfx::printText( _clip, m_pColumns[i].GetTextObj(), rText );

			// Draw header item

			if( m_pColumns[i].GetItem() )
			{
				WgRect window = rText;
				WgRect itemRect = _headerItemGeo( &m_pColumns[i], window );

				WgItem * pItem = m_pColumns[i].GetItem();
				pItem->Render( itemRect, WgRect( window, _clip ) );
			}

			// Draw sorting arrows

			if( i == m_lastSortColumn && m_pAscendGfx && m_pDescendGfx )
			{
				WgBlock block;

				if( m_lastSortColumnAscendStatus )
					block = m_pAscendGfx->GetBlock(mode);
				else
					block = m_pDescendGfx->GetBlock(mode);

				Sint32 dx = (Sint32) (r2.x + m_sortMarkerOfs.x + r2.w * m_sortMarkerOrigo.anchorX() - block.Width() * m_sortMarkerOrigo.hotspotX());
				Sint32 dy = (Sint32) (r2.y + m_sortMarkerOfs.y + r2.h * m_sortMarkerOrigo.anchorY() - block.Height() * m_sortMarkerOrigo.hotspotY());

				WgGfx::clipBlitBlock( _clip, block, WgRect( dx, dy, block.Width(), block.Height()) );
			}

			//

			r2.x += r2.w - 1;	// HACK: Overlap last pixel to avoid double separator graphics between two headers
		}

		r.y += headerHeight;

		// Modify clipping rectangle for view content (we don't want to draw over header)

		if(  _clip.y < _window.y + headerHeight)
		{
			Sint32 diff = _window.y + headerHeight - _clip.y;
			clipView.y += diff;
			clipView.h -= diff;
			if( clipView.h < 1 )
				return;
		}

	}




	// Start drawing cell contents.

	WgTableRow * pRow = (WgTableRow *) m_items.First();
	int iVisibleRowNb = 0;
	int iRealRowNb = 0;

	// Skip rows that are above clipping area.
	r.y += m_cellPaddingY;
	while( pRow )
	{
		if( pRow->IsVisible() )
		{
			if( r.y + (Sint32) pRow->Height() >= clipView.y )
				 break;
			r.y += pRow->Height() + m_cellPaddingY*2;
			iVisibleRowNb++;
		}
		iRealRowNb++;
		pRow = (WgTableRow *) pRow->GetNext();
	}
	r.y -= m_cellPaddingY;


	// Draw cell contents for (at least partly) visible rows.
	while( pRow )
	{
		if( pRow->IsHidden() )
		{
			pRow = (WgTableRow *) pRow->GetNext();
			iRealRowNb++;
			continue;
		}

		if( r.y >= clipView.y + clipView.h )
			break;

		r.h = pRow->Height() + m_cellPaddingY*2;

		WgRect	u;
		if( u.Intersection( r, clipView ) )
		{
			DrawRowBg( u, pRow, iVisibleRowNb, iRealRowNb, r );
		}

		WgRect		rc = r;

		rc.y += m_cellPaddingY;
		rc.h = pRow->Height();
		for( int _i = 0 ; _i < m_nColumns ; _i++ )
		{
			WgItem * pCell = GetItemAtPosition( _i, pRow );
			int i = GetColumnAtPosition( _i );

			if( pCell && m_pColumns[i].m_bVisible )
			{
				// don't draw columns that are outside of the window
				if( rc.x >= _window.x + _window.w )
					break;

				rc.w = (int)m_pColumns[i].m_pixelWidth;

				// Extend cell over any following empty columns.

				for( int j = _i+1 ; j < m_nColumns ; j++ )
				{
					WgItem * pNext = GetItemAtPosition( j, pRow );
					if( pNext )
						break;

					rc.w += (int)m_pColumns[GetColumnAtPosition(j)].m_pixelWidth;
				}
								
				//

				rc.x += m_cellPaddingX;
				rc.w -= m_cellPaddingX*2;

				WgRect clip2( rc, clipView );

				//

				pCell->Render( rc, clip2 );
				rc.x += ((int)m_pColumns[i].m_pixelWidth) - m_cellPaddingX;		// One cellpadding already added...
//				rc.x -= 1;	// HACK: Overlap last pixel to avoid double separator graphics between two headers
			}
		}

		r.y += pRow->Height() + m_cellPaddingY*2;
		pRow = (WgTableRow *) pRow->GetNext();
		iVisibleRowNb++;
		iRealRowNb++;
	}

	// Possibly fill with empty rows

	if( m_emptyRowHeight != 0 && pRow == 0 )
	{
		while( r.y <= clipView.y + clipView.h )
		{
			r.h = m_emptyRowHeight;

			WgRect	u;
			if( u.Intersection( r, clipView ) )
				DrawRowBg( u, pRow, iVisibleRowNb, iVisibleRowNb, r );

			r.y += r.h;
			iVisibleRowNb++;
		}
	}

}

//____ DrawRowBg() ____________________________________________________________

void Wdg_TableView::DrawRowBg( const WgRect& clip, WgTableRow * pRow, int iVisibleRowNb, int iRealRowNb, const WgRect& dest )
{
	WgMode mode = WG_MODE_NORMAL;
	if( pRow->IsSelected() )
	{
		if( iRealRowNb == m_markedRow )
			mode = WG_MODE_SPECIAL;
		else
			mode = WG_MODE_SELECTED;
	}
	else if( iRealRowNb == m_markedRow )
		mode = WG_MODE_MARKED;

	if( m_pRowColors[iVisibleRowNb%2] )
	{
		WgColor color = m_pRowColors[iVisibleRowNb%2]->Color(mode);
		if( 0 != color.a )
			WgGfx::fillRect( clip, color );
	}

	if( m_pRowBlocks[iVisibleRowNb%2] )
	{
		WgBlockSetPtr p = m_pRowBlocks[iVisibleRowNb%2];
		if( p && !p->IsModeSkipable(mode) )
			WgGfx::clipBlitBlock(clip, p->GetBlock(mode, dest.Size() ), dest );
	}
}

//____ DoMyOwnCloning() _______________________________________________________

void Wdg_TableView::DoMyOwnCloning( WgWidget * _pClone, const WgWidget * _pCloneRoot, const WgWidget * _pBranchRoot )
{
	Wdg_Baseclass_View::DoMyOwnCloning(_pClone, _pCloneRoot, _pBranchRoot);

	Wdg_TableView * pClone = (Wdg_TableView *) _pClone;

	pClone->Wg_Interface_ItemHolder::DoMyOwnCloning(this);

	pClone->m_clickSortPrio = m_clickSortPrio;
	pClone->m_bShowHeader = m_bShowHeader;
	pClone->m_bAutoScrollMarked = m_bAutoScrollMarked;
	pClone->m_nColumns = m_nColumns;
	pClone->m_pColumns = 0;
	if(m_nColumns && m_pColumns)
	{
		pClone->m_pColumns = new WgTableColumn[m_nColumns];
		for(int i = 0; i < m_nColumns; i++)
			pClone->m_pColumns[i] = WgTableColumn(m_pColumns[i]);
	}

	pClone->m_pRowColors[0] = m_pRowColors[0];
	pClone->m_pRowColors[1] = m_pRowColors[1];

	pClone->m_pRowBlocks[0] = m_pRowBlocks[0];
	pClone->m_pRowBlocks[1] = m_pRowBlocks[1];

	pClone->m_pHeaderProps = m_pHeaderProps;
	pClone->m_sortMarkerOrigo = m_sortMarkerOrigo;
	pClone->m_sortMarkerOfs = m_sortMarkerOfs;
	pClone->m_cellPaddingX = m_cellPaddingX;
	pClone->m_cellPaddingY = m_cellPaddingY;
	pClone->m_pAscendGfx = m_pAscendGfx;
	pClone->m_pDescendGfx = m_pDescendGfx;
	pClone->m_lastSortColumn = m_lastSortColumn;
	pClone->m_lastSortColumnAscendStatus = m_lastSortColumnAscendStatus;
	pClone->m_pHeaderGfxNormal = m_pHeaderGfxNormal;
	pClone->m_pHeaderGfxSelected = m_pHeaderGfxSelected;
	pClone->m_pMarkedHeader = m_pMarkedHeader;
//	pClone->m_pMarkedLineGfx = m_pMarkedLineGfx;

	pClone->m_emptyRowHeight = m_emptyRowHeight;
}

//____ DoMyOwnDisOrEnable() ___________________________________________________

void Wdg_TableView::DoMyOwnDisOrEnable( void )
{
/*	if( m_bEnabled )
		m_text.setMode(WG_MODE_NORMAL);
	else
		m_text.setMode(WG_MODE_DISABLED);
*/
	RequestRender();
}

//____ GetHeaderColumnAt() ____________________________________________________

WgTableColumn *Wdg_TableView::GetHeaderColumnAt(int x, int y, int * wpOfsX ) const
{
	if(x < 0 || y < 0)
		return NULL;

	if( GetHeaderHeight() > y )
	{
		int xOfs = x;
		for( int _col = 0 ; _col < m_nColumns ; _col++ )
		{
			int col = GetColumnAtPosition(_col);

			if( m_pColumns[col].m_bVisible )
			{
				int w = (int) m_pColumns[col].m_pixelWidth;
				if( xOfs < w )
				{
					if( wpOfsX )
						wpOfsX[0] = xOfs;
					return &m_pColumns[col];
				}
				xOfs -= w - 1;
			}
		}
	}
	return NULL;
}

//____ DoMyOwnActionRespond() _________________________________________________

void Wdg_TableView::DoMyOwnActionRespond( WgInput::UserAction _action, int _button_key, const WgActionDetails& _info, const WgInput& _inputObj )
{
	int x = _info.x;
	int y = _info.y;

	Abs2local( &x, &y );

	switch( _action )
	{
		case WgInput::BUTTON_PRESS:
		{
			WgTableColumn * pCol = GetHeaderColumnAt( x, y );			// Press on header?
			if( pCol )
			{
				if( GetMarkedHeaderItem( x, y ) )
					break;

				m_pPressedHeader = pCol;

				int col = pCol - m_pColumns;
				Emit( WgSignal::TableHeaderPress(_button_key), col );
				bool	bAscend = pCol->m_bInitialAscend;
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


			Uint32 saveOfs;
			WgTableRow * pSaveRow;
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

		case WgInput::BUTTON_RELEASE:
			if( m_pPressedHeader )
			{			
				m_pPressedHeader = 0;
				RequestRender();
			}

		break;

		case WgInput::BUTTON_CLICK:
		{
			Uint32 saveOfs;
			WgTableRow * pSaveRow;
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
			WgTableRow * pSaveRow;
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
			WgTableColumn *col = GetHeaderColumnAt(x, y);
			if( col && col->IsDisabled() )
				col = NULL;

			if( col && GetMarkedHeaderItem( x, y ) )
				col = NULL;

			if( m_pMarkedHeader != col )
			{
				m_pMarkedHeader = col;
				RequestRender();
			}


			Uint32 saveOfs;
			WgTableRow * pSaveRow;
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


	// Action respond for items.


	if( _action == WgInput::POINTER_EXIT )
	{
		m_lastClickedColumn = -1;
		m_lastClickedRow = -1;
	}

	if( _action == WgInput::POINTER_EXIT && m_pLastMarkedItem != 0 )
	{
		m_pLastMarkedItem->ActionRespond( this, _action, _button_key, _info, _inputObj );
		m_pLastMarkedItem = 0;
		return;
	}

	switch( _action )
	{
		case WgInput::KEY_DOWN:
		case WgInput::KEY_PRESS:
		case WgInput::KEY_RELEASE:
		case WgInput::KEY_REPEAT:
		case WgInput::CHARACTER:
		{
			if( m_pFocusedItem )
				m_pFocusedItem->ActionRespond( this, _action, _button_key, _info, _inputObj );
		}
		break;


		default:
			break;
	}


	{

		WgItem * pItem = GetMarkedItem( (Uint32) x, (Uint32) y );

		if( pItem != m_pLastMarkedItem && m_pLastMarkedItem != 0 )
		{
			m_pLastMarkedItem->ActionRespond( this, WgInput::POINTER_EXIT, _button_key, _info, _inputObj );
			m_pLastMarkedItem = 0;
//			pItem = 0;	// Is this correct???
//			return;	// hmmm... should this return really be here?
		}

		if( pItem )
		{
			pItem->ActionRespond( this, _action, _button_key, _info, _inputObj );

			// HACK. Remove when message loop is implemented
			// pItem can be deleted in the ActionResponse callback. Make sure it still exist // Martin
			m_pLastMarkedItem = 0;
			WgTableRow* pRow = (WgTableRow*)m_items.First();
			while( pRow )
			{
				if( pRow->HasItem( pItem ) )
				{
					m_pLastMarkedItem = pItem;
					break;
				}
				pRow = pRow->GetNext();
			}
		}
		else
		{
			m_pLastMarkedItem = 0;
		}
	}

	// Action respond for header items

	WgItem * pItem = GetMarkedHeaderItem( x, y );

	if( pItem != m_pLastMarkedHeaderItem && m_pLastMarkedHeaderItem != 0 )
		m_pLastMarkedHeaderItem->ActionRespond( this, WgInput::POINTER_EXIT, _button_key, _info, _inputObj );

	if( pItem && m_pLastMarkedHeaderItem != pItem )
		pItem->ActionRespond( this, WgInput::POINTER_ENTER, _button_key, _info, _inputObj );

	if( pItem && _action != WgInput::POINTER_ENTER )							// Pointer enter already handled above.
		pItem->ActionRespond( this, _action, _button_key, _info, _inputObj );

	m_pLastMarkedHeaderItem = pItem;

}

//____ GetMarkedHeaderItem() __________________________________________________

WgItem * Wdg_TableView::GetMarkedHeaderItem( int x, int y ) const
{
	int ofsX;
	WgTableColumn * pHeader = GetHeaderColumnAt( x, y, &ofsX );

	if( pHeader )
	{
		WgRect	headerGeo( 0, 0, (int) pHeader->m_pixelWidth, GetHeaderHeight() );
		if( m_pHeaderGfxNormal )
			headerGeo -= m_pHeaderGfxNormal->ContentBorders();

		WgRect	itemGeo = _headerItemGeo( pHeader, headerGeo );

		if( itemGeo.Contains(ofsX,y) )
			return pHeader->m_pItem;
	}

	return 0;
}

//____ UpdateMarkedRowColumn() ________________________________________________

void Wdg_TableView::UpdateMarkedRowColumn( int row, int column )
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

		RequestRender();									//TODO: Lazy and slow to always do this (even if there is no color/block difference) 
															// and on whole area when only two rows are affected.
	}
}


//____ DoMyOwnMarkTest() ___________________________________________________

bool Wdg_TableView::DoMyOwnMarkTest( int _x, int _y )
{
	return true;
}

//____ GetTooltipString() _________________________________________________

WgString Wdg_TableView::GetTooltipString() const
{
	WgString tooltip;
	if( m_pLastMarkedItem != 0 )
		tooltip = m_pLastMarkedItem->GetTooltipString();
	
	if( tooltip.IsEmpty() && m_markedRow != -1 )
	{
		const WgTableRow * p = GetRow(m_markedRow);
		if( p )
			tooltip = p->GetTooltipString();
	}
	return tooltip;
}


//____ RequestItemGeo() _______________________________________________________

WgRect Wdg_TableView::RequestItemGeo( WgItem * pItem )
{
	// We should never get here since we have overloaded WgItemRow::RequestItemGeo().

	assert(false);

	return WgRect();
}
