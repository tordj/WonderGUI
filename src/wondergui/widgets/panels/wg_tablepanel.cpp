/*=========================================================================

						 >>> WonderGUI <<<

  This file is part of Tord Jansson's WonderGUI Graphics Toolkit
  and copyright (c) Tord Jansson, Sweden [tord.jansson@gmail.com].

							-----------

  The WonderGUI Graphics Toolkit is free software you can redistribute
  this file and/or modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation either
  version 2 of the License, or (at your option) any later version.

							-----------

  The WonderGUI Graphics Toolkit is also available for use in commercial
  closed-source projects under a separate license. Interested parties
  should contact Tord Jansson [tord.jansson@gmail.com] for details.

=========================================================================*/

#include <wg_tablepanel.h>
#include <wg_skinslot.h>
#include <wg_patches.h>
#include <wg_util.h>


#include <assert.h>


namespace wg
{
	using namespace Util;

	const TypeInfo TablePanel::TYPEINFO = { "TablePanel", &ContainerPanel::TYPEINFO };

	//____ typeInfo() ____________________________________________________________

	const TypeInfo& TablePanel::typeInfo(void) const
	{
		return TYPEINFO;
	}

/*

void TablePanel::hideRows(int index, int amount)
{
	
}

void TablePanel::hideRows(row_iterator beg, row_iterator end)
{
	
}

void TablePanel::unhideRows(int index, int amount)
{
	
}

void TablePanel::unhideRows(row_iterator beg, row_iterator end)
{
	
}

void TablePanel::hideColumns(int index, int amount)
{
	
}

void TablePanel::hideColumns(column_iterator beg, column_iterator end)
{
	
}

bool TablePanel::setRowWeight(int index, int amount, float weight)
{
	
}

bool TablePanel::setRowWeight(row_iterator  beg, row_iterator end, float weight)
{
	
}

bool TablePanel::setRowWeight(int index, int amount, std::initializer_list<float> weights)
{
	
}

bool TablePanel::setRowWeight(row_iterator beg, row_iterator end, std::initializer_list<float> weights)
{
	
}

void TablePanel::unhideColumns(int index, int amount)
{
	
}

void TablePanel::unhideColumns(column_iterator beg, column_iterator end)
{
	
}

bool TablePanel::seColumnWeight(int index, int amount, float weight)
{
	
}

bool TablePanel::setColumnWeight(column_iterator  beg, column_iterator end, float weight)
{
	
}

bool TablePanel::setColumnWeight(int index, int amount, std::initializer_list<float> weights)
{
	
}

bool TablePanel::setColumnWeight(column_iterator beg, column_iterator end, std::initializer_list<float> weights)
{
	
}

void TablePanel::setRowLayout(PackLayout* pLayout)
{
	
}

void TablePanel::setColumnLayout(PackLayout* pLayout)
{
	
}

void TablePanel::setRowSpacing( pts between )
{
	
}

void TablePanel::setRowSpacing( pts before, pts between, pts after )
{
	
}

void TablePanel::setColumnSpacing( pts between )
{
	
}

void TablePanel::setColumnSpacing( pts before, pts between, pts after )
{
	
}

void TablePanel::setRowSkins( Skin * pSkin1, Skin * pSkin2 = nullptr )
{
	
}

spx TablePanel::_matchingHeight(spx width, int scale) const
{
	
}

spx TablePanel::_matchingWidth(spx height, int scale) const
{
	
}

SizeSPX TablePanel::_defaultSize(int scale) const
{
	
}

SizeSPX TablePanel::_minSize(int scale) const
{
	
}

SizeSPX TablePanel::_maxSize(int scale) const
{
	
}

RectSPX TablePanel::_slotGeo( const StaticSlot * pSlot ) const
{
	
}

void TablePanel::_childOverflowChanged( StaticSlot * pSlot, const BorderSPX& oldOverflow, const BorderSPX& newOverflow )
{
	
}

bool TablePanel::_isChildVisible( const StaticSlot * pSlot ) const
{
	
}

void TablePanel::_childRequestRender(StaticSlot * pSlot, const RectSPX& rect)
{
	
}

void TablePanel::_childRequestResize(StaticSlot * pSlot)
{
	
}

Widget * TablePanel::_prevChild(const StaticSlot * pSlot) const
{
	
}

Widget * TablePanel::_nextChild(const StaticSlot * pSlot) const
{
	
}

void TablePanel::_releaseChild(StaticSlot * pSlot)
{
	
}
void TablePanel::_replaceChild(StaticSlot * pSlot, Widget * pNewChild)
{
	
}

Widget * TablePanel::_firstChild() const
{
	
}

Widget * TablePanel::_lastChild() const
{
	
}

void TablePanel::_firstSlotWithGeo( SlotWithGeo& package ) const
{
	
}

void TablePanel::_nextSlotWithGeo( SlotWithGeo& package ) const
{
	
}

const TypeInfo& TablePanel::_slotTypeInfo(const StaticSlot * pSlot) const
{
	
}

*/
 
//____ _render() ______________________________________________________________

void TablePanel::_render(GfxDevice* pDevice, const RectSPX& _canvas, const RectSPX& _window)
{
	m_skin.render( pDevice, _canvas, m_scale, m_state );

	RectSPX canvas = m_skin.contentRect( _canvas, m_scale, m_state );

	BorderSPX rowBorder = m_pSkin[0] != nullptr ? m_pSkin[0]->_contentBorder(m_scale, m_state) : BorderSPX();
	
	spx rowOfs = 0;

	Skin * rowSkin1 = m_pRowSkin[0];
	Skin * rowSkin2 = m_pRowSkin[1] != nullptr ? m_pRowSkin[1] : m_pRowSkin[0];

	
	int rowSkinOfs = canvas.y + m_spacingY[0];
	int rowSkinInc = m_pSkin[1] != nullptr ? 1 : 0;
	
	auto slotIt = slots.begin();
	
	for( auto& row : rows )
	{
		RectSPX rowGeo = { canvas.x, rowOfs, canvas.w, row.m_height + rowBorder.height() };

		if( row.m_bVisible )
		{
			// Render row skin
			
			if( rowSkin1 != nullptr )
			{
				rowSkin1->_render(pDevice, rowGeo, m_scale, m_state);
				std::swap(rowSkin1, rowSkin2);
			}
			
			// Render widgets in row
			
			CoordSPX widgetPos;
			widgetPos.x = rowGeo.x + rowBorder.left + m_spacingX[0];
			widgetPos.y = rowGeo.y + rowBorder.top;

			
			for( auto& column : columns )
			{
				if( column.m_bVisible && !slotIt->isEmpty() )
				{
					RectSPX widgetCanvas = { widgetPos.x, widgetPos.y, column.m_width, row.m_height };
					
					slotIt->_widget()->_render(pDevice, widgetCanvas, widgetCanvas);
				}
				
				slotIt++;
				widgetPos.x += column.m_width + m_spacingX[1];
			}
		}
		else
		{
			slotIt += columns.size();
		}
		
		// Finish up
		
		rowOfs += rowGeo.h + m_spacingY[1];
	}
}

//____ _resize() ______________________________________________________________

void TablePanel::_resize( const SizeSPX& size, int scale )
{
	SizeSPX oldSize = m_size;
	bool bScaleChanged = scale != m_scale;
	
	Container::_resize(size, scale);
	
	if( bScaleChanged )
	{
		
	}
	else
	{
		bool bColumnsChanged = false;
		bool matchingRowHeightChanged = false;
		bool bRowsChanged = false;

		if( oldSize.w != size.w )
			bColumnsChanged = _refreshColumns();
		
		if( bColumnsChanged )
			matchingRowHeightChanged = _refreshRowHeightForColumnWidth();
		
		if( oldSize.h != size.h || macthingRowHeightChanged )
			bRowsChanged = _refreshRows();
		
		if( bRowsChanged || bColumnsChanged )
			_updateModifiedChildSizes();
	}
}

//____ _refreshColumnLayout() ______________________________________________________

/* 	Refreshes m_width for all columns.

	Precondition: All other members of columns (m_matchingWidth, m_weight, m_bVisible etc) needs to be up to date.
 */

bool TablePanel::_refreshColumns()
{
	bool bLayoutChanged = false;

	if( m_pLayoutX )
	{
		auto pLayout = m_pLayoutX;
		
		// Count visible columns
		
		int nVisibleColumns = 0;
		for( auto& column : columns )
		{
			if( column.m_bVisible )
				nVisibleColumns++;
		}
		
		// Get column widths from our PackLayout
		
		spx totalPadding = m_skin.contentBorderSize(m_scale).w + m_spacingX[0] + m_spacingX[1] * std::max(0, nVisibleColumns-1) + m_spacingX[2];
		if( m_pSkin[0] != nullptr )
			totalPadding += m_pSkin[0]->contentBorderSize(m_scale).w;
		
		spx availableWidth = m_size.w - totalPadding;
		
		int arrayBytes = (sizeof(PackLayout::Item)+sizeof(spx)) * nVisibleColumns;
		PackLayout::Item* pItem = reinterpret_cast<PackLayout::Item*>(Base::memStackAlloc(arrayBytes));
		spx* pOutput = (spx*) &pItem[nVisibleColumns];
		
		for( auto& column : columns )
		{
			if( column.m_bVisible )
			{
				pItem->def = column.m_defaultWidth;
				pItem->min = column.m_minWidth;
				pItem->max = column.m_maxWidth;
				pItem->weight = column.m_weight*65536;
				pItem++;
			}
		}
		
		pLayout->getItemSizes(pOutput, availableWidth, m_scale, nVisibleColumns, pItemArea);
		
		// Update column widths and flag changes
				
		for( auto& column : columns )
		{
			if( column.m_bVisible )
			{
				spx newWidth = * pOutput++;
				
				if( column.m_width != newWidth )
				{
					column.m_width = newWidth;
					column.m_bModified = true;
					bLayoutChanged = true;
				}
			}
		}
		
		// Release temporary memory area

		Base::memStackFree(arrayBytes);
	}
	else
	{
		for( auto& column : columns )
		{
			if( column.m_bVisible )
			{
				if( column.m_width != column.m_defaultWidth )
				{
					column.m_width = column.defaultWidth;
					column.m_bModified = true;
					bLayoutChanged = true;
				}
			}
		}
	}
		
	return bLayoutChanged;
}

//____ _refreshRows() _________________________________________________________

bool TablePanel::_refreshRows()
{
	bool bLayoutChanged = false;
	
	if( m_pLayoutY )
	{
		auto pLayout = m_pLayoutY;
		
		// Count visible rows
		
		int nVisibleRows = 0;
		for( auto& row : rows )
		{
			if( rows.m_bVisible )
				nVisibleRows++;
		}
		
		// Get out row height from our PackLayout
		
		spx totalPadding = m_skin.contentBorderSize(m_scale).h + m_spacingY[0] + m_spacingY[1] * std::max(0, nVisibleRows-1) + m_spacingY[2];
		if( m_pSkin[0] != nullptr )
			totalPadding += m_pSkin[0]->contentBorderSize(m_scale).h;
		
		spx availableHeight = m_size.h - totalPadding;
		
		int arrayBytes = (sizeof(PackLayout::Item)+sizeof(spx)) * nVisibleRows;
		PackLayout::Item* pItem = reinterpret_cast<PackLayout::Item*>(Base::memStackAlloc(arrayBytes));
		spx* pOutput = (spx*) &pItem[nVisibleRows];
		
		for( auto& row : rows )
		{
			if( row.m_bVisible )
			{
				pItem->def = row.m_heightForColumnWidth;
				pItem->min = row.m_minHeight;
				pItem->max = row.m_maxHeight;
				pItem->weight = row.m_weight*65536;
				pItem++;
			}
		}
		
		pLayout->getItemSizes(pOutput, availableHeight, m_scale, nVisibleRows, pItemArea);
		
		// Update row widths and flag changes
		
		for( auto& row : rows )
		{
			if( row.m_bVisible )
			{
				spx newHeight = * pOutput++;
				
				if( row.m_height != newHeight )
				{
					row.m_height = newHeight;
					row.m_bModified = true;
					bLayoutChanged = true;
				}
			}
		}
		
		// Release temporary memory area
		
		Base::memStackFree(arrayBytes);
	}
	else
	{
		for( auto& row : rows )
		{
			if( row.m_bVisible )
			{
				if( row.m_height != row.m_heightForColumnWidth )
				{
					row.m_height = column.m_heightForColumnWidth;
					row.m_bModified = true;
					bLayoutChanged = true;
				}
			}
		}
	}
	
	return bLayoutChanged;
}

//____ _refreshRowHeightForColumnWidth() ______________________________________

bool TablePanel::_refreshRowHeightForColumnWidth()
{
	bool bLayoutChanged = false;

	auto slotIt = slots.begin();
	
	for( auto& row : rows )
	{
		if( row.m_bVisible )
		{
			spx height = 0;
			
			for( auto& column : columns )
			{
				if( column.m_bVisible && !slotIt->isEmpty() )
				{
					spx h = slotIt->_widget()->_matchingHeight(column.m_width, m_scale);
					if( h > height )
						height = h;
				}
				
				slotIt++;
			}

			if( height != row.m_heightForColumnWidth )
			{
				row.m_heightForColumnWidth = height;
				bLayoutChanged = true;
			}
		}
		else
			slotIt += columns.size();
	}
	
	return bLayoutChanged;
}

//____ _updateModifiedChildSizes() ____________________________________________

void TablePanel::_updateModifiedChildSizes()
{
	auto slotIt = slots.begin();
	
	for( auto& row : rows )
	{
		for( auto& column : columns )
		{
			if( !slotIt->isEmpty() && (row.m_bModified || column.m_bModified) )
				slotIt->_widget()->_resize({column.m_width,row.m_height}, m_scale);
			
			slotIt++;
		}
	}
}

//____ _refreshRowData() ______________________________________________________

bool TablePanel::_refreshRowData( int row )
{
	auto rowIt = rows.begin() + row;
	auto slotIt = slots.begin() + row * columns.size();

	spx	defaultHeight = 0;
	spx minHeight = 0;
	spx maxHeight = spx_max;
	spx heightForWidth = 0;
	
	for( auto& column : columns )
	{
		Widget * p = slotIt->_widget();
		if( p )
		{
			spx myDefaultHeight = p->_defaultSize(m_scale).h;
			spx myMinHeight = p->_minSize(m_scale).h;
			spx myMaxHeight = p->_maxSize(m_scale).h;
			spx myHeightForWidth = p->_matchingHeight(column.m_width, m_scale);
			
			if( myDefaultHeight > defaultHeight )
				defaultHeight = myDefaultHeight;
			
			if( myMinHeight > minHeight )
				minHeight = myMinHeight;
			
			if( myMaxHeight < maxHeight )
				maxHeight = myMaxHeight;
			
			if( myHeightForWidth > heightForWidth )
				heightForWidth = myHeightForWidth;
		}
		
		slotIt++;
	}

	if( defaultHeight != rowIt->m_defaultHeight || minHeight != rowIt->m_minHeight || maxHeight)
	
	
}

//____ _refreshColumnData() ___________________________________________________

bool TablePanel::_refreshColumnData( int column )
{
	
}



//____ _refreshSlots() ________________________________________________________

void TablePanel::_refreshSlots(int ofs, Axis axis, int nSlots)
{

	
	
}

//____ _object() ______________________________________________________________

Object*	TablePanel::_object()
{
	return this;
}



/*

void			_didAddEntries(TablePanelRow * pEntry, int nb) override;
void			_didMoveEntries(TablePanelRow * pFrom, TablePanelRow * pTo, int nb) override;
void			_willEraseEntries(TablePanelRow * pEntry, int nb) override;

void			_didAddEntries(TablePanelColumn * pEntry, int nb) override;
void			_didMoveEntries(TablePanelColumn * pFrom, TablePanelColumn * pTo, int nb) override;
void			_willEraseEntries(TablePanelColumn * pEntry, int nb) override;



void			_hideRows(TablePanelRow * pStart, int nb);
void			_unhideRows(TablePanelRow * pStart, int nb);

void			_hideColumns(TablePanelColumn * pStart, int nb);
void			_unhideColumns(TablePanelColumn * pStart, int nb);

void			_reweightRows(TablePanelRow * pEntry, int nb, float weight);
void			_reweightRows(TablePanelRow * pEntry, int nb, const float * pWeights);

void			_reweightColumns(TablePanelColumn * pEntry, int nb, float weight);
void			_reweightColumns(TablePanelColumn * pEntry, int nb, const float * pWeights);

*/

} // namespace wg
