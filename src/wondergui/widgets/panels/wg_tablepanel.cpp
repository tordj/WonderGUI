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
#include <wg_packlayout.h>
#include <wg_util.h>


#include <assert.h>


namespace wg
{
using namespace Util;

const TypeInfo TablePanel::TYPEINFO = { "TablePanel", &Container::TYPEINFO };

//____ constructor ____________________________________________________________

TablePanel::TablePanel() : rows(this), columns(this), slots(this, this)
{

}

//____ destructor _____________________________________________________________

TablePanel::~TablePanel()
{

}

//____ typeInfo() ____________________________________________________________

const TypeInfo& TablePanel::typeInfo(void) const
{
	return TYPEINFO;
}

//____ hideRows() _________________________________________________________

void TablePanel::hideRows(int index, int amount)
{
	_hideRows(&rows[index], amount);
}

void TablePanel::hideRows(row_iterator beg, row_iterator end)
{
	_hideRows(&*beg, int(end - beg));
}

//____ unhideRows() ___________________________________________________________

void TablePanel::unhideRows(int index, int amount)
{
	_unhideRows(&rows[index], amount);
}

void TablePanel::unhideRows(row_iterator beg, row_iterator end)
{
	_unhideRows(&*beg, int(end - beg));
}

//____ hideColumns() __________________________________________________________

void TablePanel::hideColumns(int index, int amount)
{
	_hideColumns(&columns[index], amount);
}

void TablePanel::hideColumns(column_iterator beg, column_iterator end)
{
	_hideColumns(&*beg, int(end - beg));
}

//____ unhideColumns() ________________________________________________________

void TablePanel::unhideColumns(int index, int amount)
{
	_unhideColumns(&columns[index], amount);
}

void TablePanel::unhideColumns(column_iterator beg, column_iterator end)
{
	_unhideColumns(&*beg, int(end - beg));
}

//____ setRowWeight() _________________________________________________________

void TablePanel::setRowWeight(int index, int amount, float weight)
{
	_reweightRows(&rows[index], amount, weight);
}

void TablePanel::setRowWeight(row_iterator  beg, row_iterator end, float weight)
{
	_reweightRows( &*beg, int(end - beg), weight);
}

void TablePanel::setRowWeight(int index, int amount, std::initializer_list<float> weights)
{
	_reweightRows(&rows[index], amount, weights.begin() );
}

void TablePanel::setRowWeight(row_iterator beg, row_iterator end, std::initializer_list<float> weights)
{
	_reweightRows(&*beg, int(end - beg), weights.begin());
}

//____ setColumnWeight() ______________________________________________________

void TablePanel::setColumnWeight(int index, int amount, float weight)
{
	_reweightColumns(&columns[index], amount, weight);
}

void TablePanel::setColumnWeight(column_iterator  beg, column_iterator end, float weight)
{
	_reweightColumns(&*beg, int(end - beg), weight);
}

void TablePanel::setColumnWeight(int index, int amount, std::initializer_list<float> weights)
{
	_reweightColumns(&columns[index], amount, weights.begin());
}

void TablePanel::setColumnWeight(column_iterator beg, column_iterator end, std::initializer_list<float> weights)
{
	_reweightColumns(&*beg, int(end - beg), weights.begin());
}

//____ setRowLayout() _________________________________________________________

void TablePanel::setRowLayout(PackLayout* pLayout)
{
	if (pLayout != m_pLayoutY)
	{
		m_pLayoutY = pLayout;
				
		bool bRowsChanged = _refreshRows();
		
		if( bRowsChanged )
		{
			_updateModifiedChildSizes();
			_requestRender();
		}
	}
}

//____ setColumnLayout() _________________________________________________________

void TablePanel::setColumnLayout(PackLayout* pLayout)
{
	if (pLayout != m_pLayoutX)
	{
		m_pLayoutX = pLayout;
		
		bool bColumnsChanged = false;
		bool matchingRowHeightChanged = false;

		bColumnsChanged = _refreshColumns();
		
		if( bColumnsChanged )
		{
			matchingRowHeightChanged = _refreshRowHeightForColumnWidth();
		
			if( matchingRowHeightChanged )
				_refreshRows();
		
			_updateModifiedChildSizes();
			_requestRender();
		}
	}
}

//____ setRowSpacing() ________________________________________________________

void TablePanel::setRowSpacing( pts before, pts between, pts after )
{
	if( before == m_spacingY[0] && between == m_spacingY[1] && after == m_spacingY[2] )
		return;
	
	SizeSPX oldPadding = _sumOfPadding(m_scale);
	
	m_spacingY[0] = before;
	m_spacingY[1] = between;
	m_spacingY[2] = after;
	
	SizeSPX newPadding = _sumOfPadding(m_scale);

	m_defaultSize += newPadding - oldPadding;
	m_minSize += newPadding - oldPadding;

	_requestResize();
	_requestRender();
}

//____ setColumnSpacing() ________________________________________________________

void TablePanel::setColumnSpacing( pts before, pts between, pts after )
{
	if( before == m_spacingX[0] && between == m_spacingX[1] && after == m_spacingX[2] )
		return;
	
	SizeSPX oldPadding = _sumOfPadding(m_scale);
	
	m_spacingX[0] = before;
	m_spacingX[1] = between;
	m_spacingX[2] = after;
	
	SizeSPX newPadding = _sumOfPadding(m_scale);

	m_defaultSize += newPadding - oldPadding;
	m_minSize += newPadding - oldPadding;

	SizeSPX oldSize = m_size;
	_requestResize();
	if( m_size != oldSize )
	{
		bool bColumnsChanged = _refreshColumns();
		
		if( bColumnsChanged )
		{
			bool matchingRowHeightChanged = _refreshRowHeightForColumnWidth();
			
			if( matchingRowHeightChanged )
				_refreshRows();
			
			_updateModifiedChildSizes();
		}
		_requestRender();
	}
}

//____ setRowSkins() __________________________________________________________

void TablePanel::setRowSkins( Skin * pSkin1, Skin * pSkin2 )
{
	if( pSkin1 == nullptr && pSkin2 != nullptr )
	{
		//TODO: Error handling
		
		return;
	}

	if( pSkin2 == nullptr )
		pSkin2 = pSkin1;
	
	if( pSkin1 == m_pRowSkins[0] && pSkin2 == m_pRowSkins[1] )
		return;

	if(pSkin2 != pSkin1)
	{
		if( pSkin1->margin() + pSkin1->padding() != pSkin2->margin() + pSkin2->padding() )
		{
			//TODO: Error handling
			
			return;
		}
	}
		
	bool bGeoChanged = false;
	
	if( pSkin1 == nullptr && (m_pRowSkins[0]->_hasPadding() || m_pRowSkins[0]->_hasMargin()) )
		bGeoChanged = true;
	else if( m_pRowSkins[0] == nullptr && (pSkin1->_hasPadding() || pSkin1->_hasMargin()) )
		bGeoChanged = true;
	else
		bGeoChanged = pSkin1->_contentBorder(m_scale, m_state) != m_pRowSkins[0]->_contentBorder(m_scale, m_state);
	
	m_pRowSkins[0] = pSkin1;
	m_pRowSkins[1] = pSkin2;
	
	if( bGeoChanged )
	{
		// This can be optimized in many ways if needed in the future.
		
		_updateMinDefaultSize();
		_refreshColumns();
		_refreshRows();
		_requestResize();
		_requestRender();
	}
	else
		_requestRender();
}

 
//____ _matchingHeight() ______________________________________________________

spx TablePanel::_matchingHeight(spx width, int scale) const
{

	// Shortcuts for known matches

	if (scale == m_scale)
	{
		if (width == m_defaultSize.w || !m_pLayoutX )
			return m_defaultSize.h;

		if (width == m_size.w)
		{
			spx height = 0;

			for (auto& row : rows)
			{
				if (row.m_bVisible)
					height += row.m_cache.heightForColumnWidth;
			}

			return height + _sumOfPadding(scale).h;
		}
	}

	// Reserve memory for column widths

	SizeSPX totalPadding = _sumOfPadding(scale);

	if (m_pLayoutX)
	{
		// Get column widths

		int colWidthBytes = sizeof(spx) * m_nVisibleColumns;
		spx* pColumnWidths = reinterpret_cast<spx*>(Base::memStackAlloc(colWidthBytes));

		int itemBytes = sizeof(PackLayout::Item) * m_nVisibleColumns;
		PackLayout::Item* pItems = reinterpret_cast<PackLayout::Item*>(Base::memStackAlloc(itemBytes));

		auto pOut = pItems;

		if (scale == m_scale)
		{
			for (auto& column : columns)
			{
				if (column.m_bVisible)
				{
					pOut->def = column.m_cache.defaultWidth;
					pOut->min = column.m_cache.minWidth;
					pOut->max = column.m_cache.maxWidth;
					pOut->weight = column.m_weight * 65536;
					pOut++;
				}
			}
		}
		else
		{
			for (int i = 0 ; i < columns.size() ; i++)
			{
				if (columns[i].m_bVisible)
				{
					TablePanelColumn::Cache cache;

					_refreshColumnCache(i, cache, scale);

					pOut->def = cache.defaultWidth;
					pOut->min = cache.minWidth;
					pOut->max = cache.maxWidth;
					pOut->weight = columns[i].m_weight * 65536;
					pOut++;
				}
			}
		}

		spx availableWidth = width - totalPadding.w;
		m_pLayoutX->getItemSizes(pColumnWidths, availableWidth, m_scale, m_nVisibleColumns, pItems);

		Base::memStackFree(itemBytes);

		auto slotIt = slots.begin();

		spx tableHeight = 0;

		for (auto& row : rows)
		{
			if (row.m_bVisible)
			{
				spx rowHeight = 0;

				for (int i = 0 ; i < columns.size() ; i++)
				{
					if( columns[i].m_bVisible && !slotIt->isEmpty() )
					{
						spx height = slotIt->_widget()->_matchingHeight(pColumnWidths[i], scale);
						rowHeight = std::max(rowHeight, height);
					}
					slotIt++;
				}

				tableHeight += rowHeight;
			}
			else
				slotIt += columns.size();
		}

		Base::memStackFree(colWidthBytes);

		return tableHeight + totalPadding.h;
	}
	else
	{
		// Without a Layouter for columns, we can ignore columns and just go on default height
		// Also, we only get here if scale is different from m_scale.

		auto slotIt = slots.begin();

		spx tableHeight = 0;

		for (auto& row : rows)
		{
			if (row.m_bVisible)
			{
				spx rowHeight = 0;

				for (int i = 0; i < columns.size(); i++)
				{
					if (columns[i].m_bVisible && !slotIt->isEmpty())
					{
						spx height = slotIt->_widget()->_defaultSize(scale).h;
						rowHeight = std::max(rowHeight, height);
					}
					slotIt++;
				}

				tableHeight += rowHeight;
			}
			else
				slotIt += columns.size();
		}

		return tableHeight + totalPadding.h;
	}
}

//____ _matchingWidth() _______________________________________________________

spx TablePanel::_matchingWidth(spx height, int scale) const
{
	return _defaultSize(scale).w;
}

//____ _defaultSize() _________________________________________________________

SizeSPX TablePanel::_defaultSize(int scale) const
{
	if( scale == m_scale )
		return m_defaultSize;
	else
	{
		// Reserve array for visible slots default sizes

		int arrayBytes = sizeof(SizeSPX) * m_nVisibleColumns * m_nVisibleRows;

		auto pDefaultSizes = reinterpret_cast<SizeSPX*>(Base::memStackAlloc(arrayBytes));

		// Fill in default sizes

		auto slotIt = slots.begin();

		auto pWriteSizes = pDefaultSizes;

		for (auto& row : rows)
		{
			if (row.m_bVisible)
			{
				for (auto& column : columns)
				{
					if (slotIt->isEmpty())
						*pWriteSizes++ = { 0,0 };
					else
						*pWriteSizes++ = slotIt->_widget()->_defaultSize(scale);

					slotIt++;
				}
			}
			else
				slotIt += columns.size();
		}

		SizeSPX	defSize;

		// Calc default width for each column and add them together

		auto pReadSizes = pDefaultSizes;

		for( int col = 0; col < m_nVisibleColumns; col++ )
		{
			spx colDef = 0;

			for (int row = 0; row < m_nVisibleRows; row++)
				colDef = std::max(colDef, pDefaultSizes[row * m_nVisibleColumns + col].w);

			defSize.w += colDef;
		}

		// Calc default height for each row and add them together

		for (int row = 0; row < m_nVisibleColumns; row++)
		{
			spx rowDef = 0;

			for (int col = 0; col < m_nVisibleRows; col++)
				rowDef = std::max(rowDef, pDefaultSizes[row * m_nVisibleColumns + col].h);

			defSize.h += rowDef;
		}

		Base::memStackFree(arrayBytes);

		return defSize + _sumOfPadding(m_scale);
	}
}

//____ _minSize() _____________________________________________________________

SizeSPX TablePanel::_minSize(int scale) const
{
	if (scale == m_scale)
		return m_defaultSize;
	else
	{
		// Reserve array for visible slots min sizes

		int arrayBytes = sizeof(SizeSPX) * m_nVisibleColumns * m_nVisibleRows;

		auto pDefaultSizes = reinterpret_cast<SizeSPX*>(Base::memStackAlloc(arrayBytes));

		// Fill in min sizes

		auto slotIt = slots.begin();

		auto pWriteSizes = pDefaultSizes;

		for (auto& row : rows)
		{
			if (row.m_bVisible)
			{
				for (auto& column : columns)
				{
					if (slotIt->isEmpty())
						*pWriteSizes++ = { 0,0 };
					else
						*pWriteSizes++ = slotIt->_widget()->_minSize(scale);

					slotIt++;
				}
			}
			else
				slotIt += columns.size();
		}

		SizeSPX	minSize;

		// Calc default width for each column and add them together

		auto pReadSizes = pDefaultSizes;

		for (int col = 0; col < m_nVisibleColumns; col++)
		{
			spx colDef = 0;

			for (int row = 0; row < m_nVisibleRows; row++)
				colDef = std::max(colDef, pDefaultSizes[row * m_nVisibleColumns + col].w);

			minSize.w += colDef;
		}

		// Calc default height for each row and add them together

		for (int row = 0; row < m_nVisibleColumns; row++)
		{
			spx rowDef = 0;

			for (int col = 0; col < m_nVisibleRows; col++)
				rowDef = std::max(rowDef, pDefaultSizes[row * m_nVisibleColumns + col].h);

			minSize.h += rowDef;
		}

		Base::memStackFree(arrayBytes);

		return minSize + _sumOfPadding(m_scale);
	}
}

//____ _maxSize() _____________________________________________________________

SizeSPX TablePanel::_maxSize(int scale) const
{
	return { spx_max, spx_max };
}

//____ _slotGeo() _____________________________________________________________

RectSPX TablePanel::_slotGeo(const StaticSlot* pSlot) const
{
	int slotIdx = int(static_cast<const DynamicSlot*>(pSlot) - slots.data());
	int row = slotIdx / columns.size();
	int col = slotIdx % columns.size();

	RectSPX geo;
	geo.w = columns[col].m_width;
	geo.h = rows[row].m_height;

	geo += m_skin.contentOfs(m_scale, State::Default);

	BorderSPX rowBorder = m_pRowSkins[0] ? m_pRowSkins[0]->_contentBorder(m_scale, State::Default) : BorderSPX();

	geo.x += ptsToSpx(m_spacingX[0], m_scale) + rowBorder.left;
	geo.y += ptsToSpx(m_spacingY[0], m_scale) + rowBorder.top;

	for (int i = 0; i < col; i++)
	{
		if (columns[i].m_bVisible)
			geo.x += columns[i].m_width + ptsToSpx(m_spacingX[1], m_scale);
	}
		
	for (int i = 0; i < row; i++)
	{
		if (rows[i].m_bVisible)
			geo.y += rows[i].m_height + ptsToSpx(m_spacingY[1], m_scale) + rowBorder.height();
	}

	return geo;
}

//____ _childOverflowChanged() ________________________________________________

void TablePanel::_childOverflowChanged( StaticSlot * pSlot, const BorderSPX& oldOverflow, const BorderSPX& newOverflow )
{
	//TODO: Implement!!!
}

//____ _isChildVisible() ______________________________________________________

bool TablePanel::_isChildVisible( const StaticSlot * pSlot ) const
{
	int slotIdx = int(static_cast<const DynamicSlot*>(pSlot) - slots.data());
	int row = slotIdx / columns.size();
	int col = slotIdx % columns.size();

	return (columns[col].m_bVisible && rows[row].m_bVisible);
}


//____ _childRequestRender() __________________________________________________

void TablePanel::_childRequestRender(StaticSlot * pSlot, const RectSPX& rect)
{
	RectSPX geo = _slotGeo(pSlot);
	_requestRender(rect + geo.pos());
}

//____ _childRequestResize() __________________________________________________

void TablePanel::_childRequestResize(StaticSlot * pSlot)
{
	int ofs = int(static_cast<DynamicSlot*>(pSlot) - slots.data());

	SizeSPX oldSize = pSlot->_widget()->_size();

	_refreshSlots(ofs, Axis::X, 1);

	// Make sure we fullfil the contract of always calling _resize() on _requestResize().

	if (pSlot->_widget()->_size() == oldSize)
		pSlot->_widget()->_resize(oldSize, m_scale);
}

//____ _prevChild() ___________________________________________________________

Widget * TablePanel::_prevChild(const StaticSlot * _pSlot) const
{
	auto pSlot = static_cast<const DynamicSlot*>(_pSlot);
	
	auto pFirstSlot = slots.data();

	pSlot--;
	while (pSlot >= pFirstSlot)
	{
		if (!pSlot->isEmpty())
			return pSlot->_widget();
		pSlot--;
	}

	return nullptr;
}

//____ _nextChild() ___________________________________________________________

Widget * TablePanel::_nextChild(const StaticSlot * _pSlot) const
{
	auto pSlot = static_cast<const DynamicSlot*>(_pSlot);

	auto pEnd = slots.data() + slots.slots();;

	pSlot++;
	while (pSlot < pEnd)
	{
		if (!pSlot->isEmpty())
			return pSlot->_widget();
		pSlot++;
	}

	return nullptr;
}

//____ _releaseChild() ________________________________________________________

void TablePanel::_releaseChild(StaticSlot * pSlot)
{
	pSlot->_clearWidget();

	int ofs = int(static_cast<DynamicSlot*>(pSlot) - slots.data());
	_refreshSlots(ofs, Axis::X, 1);
}


//____ _replaceChild() ________________________________________________________

void TablePanel::_replaceChild(StaticSlot * pSlot, Widget * pNewChild)
{
	pSlot->_setWidget(pNewChild);

	int ofs = int(static_cast<DynamicSlot*>(pSlot) - slots.data());
	_refreshSlots(ofs, Axis::X, 1);
}

//____ _firstChild() __________________________________________________________

Widget * TablePanel::_firstChild() const
{
	auto it = slots.begin();
	while (it != slots.end())
	{
		if (!it->isEmpty())
			return it->_widget();
		it++;
	}

	return nullptr;
}

//____ _lastChild() ___________________________________________________________

Widget * TablePanel::_lastChild() const
{
	auto it = slots.end() -1;
	while (it >= slots.begin())
	{
		if (!it->isEmpty())
			return it->_widget();
		it--;
	}

	return nullptr;
}

//____ _firstSlotWithGeo() ____________________________________________________

void TablePanel::_firstSlotWithGeo( SlotWithGeo& package ) const
{
	auto it = slots.begin();
	while (it != slots.end())
	{
		if (!it->isEmpty())
		{
			package.pSlot = &*it;
			package.geo = _slotGeo(&*it);		// This is slow, but this method should not be used so much anyway.
			return;
		}
		it++;
	}

	package.pSlot = nullptr;
	package.geo.clear();
}

//____ _nextSlotWithGeo() _____________________________________________________

void TablePanel::_nextSlotWithGeo( SlotWithGeo& package ) const
{
	auto it = slots.begin() + ( static_cast<const DynamicSlot*>(package.pSlot) - slots.data());
	it++;

	while (it != slots.end())
	{
		if (!it->isEmpty())
		{
			package.pSlot = &*it;
			package.geo = _slotGeo(&*it);		// This is slow, but this method should not be used so much anyway.
			return;
		}
	}

	package.pSlot = nullptr;
	package.geo.clear();
}


//____ _slotTypeInfo() ________________________________________________________

const TypeInfo& TablePanel::_slotTypeInfo(const StaticSlot * pSlot) const
{
	return DynamicSlot::TYPEINFO;
}
 
//____ _render() ______________________________________________________________

void TablePanel::_render(GfxDevice* pDevice, const RectSPX& _canvas, const RectSPX& _window)
{
	m_skin.render( pDevice, _canvas, m_scale, m_state );

	RectSPX canvas = m_skin.contentRect( _canvas, m_scale, m_state );

	BorderSPX rowBorder = m_pRowSkins[0] != nullptr ? m_pRowSkins[0]->_contentBorder(m_scale, m_state) : BorderSPX();
	
	spx rowOfs = 0;

	Skin * rowSkin1 = m_pRowSkins[0];
	Skin * rowSkin2 = m_pRowSkins[1] != nullptr ? m_pRowSkins[1] : m_pRowSkins[0];

	auto slotIt = slots.begin();
	
	for( auto& row : rows )
	{
		RectSPX rowGeo = { canvas.x, canvas.y + ptsToSpx(m_spacingY[0], m_scale) + rowOfs, canvas.w, row.m_height + rowBorder.height() };

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
			widgetPos.x = rowGeo.x + rowBorder.left + ptsToSpx(m_spacingX[0], m_scale);
			widgetPos.y = rowGeo.y + rowBorder.top;

			
			for( auto& column : columns )
			{
				if( column.m_bVisible && !slotIt->isEmpty() )
				{
					RectSPX widgetCanvas = { widgetPos.x, widgetPos.y, column.m_width, row.m_height };
					
					slotIt->_widget()->_render(pDevice, widgetCanvas, widgetCanvas);
				}
				
				slotIt++;
				widgetPos.x += column.m_width + ptsToSpx(m_spacingX[1], m_scale);
			}
		}
		else
		{
			slotIt += columns.size();
		}
		
		// Finish up
		
		rowOfs += rowGeo.h + ptsToSpx(m_spacingY[1], m_scale);
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

		for (int i = 0; i < columns.size(); i++)
			_refreshColumnCache(i, columns[i].m_cache, scale);

		_refreshColumns();

		for( int i = 0 ; i < rows.size() ; i++  )
			_refreshRowCache(i, rows[i].m_cache, scale);

		_refreshRows();

		_updateAllChildSizes();
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
		
		if( oldSize.h != size.h || matchingRowHeightChanged )
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
				
		// Get column widths from our PackLayout
		
		spx totalPadding = m_skin.contentBorderSize(m_scale).w + ptsToSpx(m_spacingX[0], m_scale) + ptsToSpx(m_spacingX[1], m_scale) * std::max(0, m_nVisibleColumns-1) + ptsToSpx(m_spacingX[2], m_scale);

		if( m_pRowSkins[0] != nullptr )
			totalPadding += m_pRowSkins[0]->_contentBorderSize(m_scale).w;
		
		spx availableWidth = m_size.w - totalPadding;
		
		int arrayBytes = (sizeof(PackLayout::Item)+sizeof(spx)) * m_nVisibleColumns;
		PackLayout::Item* pItem = reinterpret_cast<PackLayout::Item*>(Base::memStackAlloc(arrayBytes));
		spx* pOutput = (spx*) &pItem[m_nVisibleColumns];
		
		for( auto& column : columns )
		{
			if( column.m_bVisible )
			{
				pItem->def = column.m_cache.defaultWidth;
				pItem->min = column.m_cache.minWidth;
				pItem->max = column.m_cache.maxWidth;
				pItem->weight = column.m_weight*65536;
				pItem++;
			}
		}
		
		pLayout->getItemSizes(pOutput, availableWidth, m_scale, m_nVisibleColumns, pItem);
		
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
				if( column.m_width != column.m_cache.defaultWidth )
				{
					column.m_width = column.m_cache.defaultWidth;
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
		// Get out row height from our PackLayout
		
		spx totalPadding = m_skin.contentBorderSize(m_scale).h + ptsToSpx(m_spacingY[0], m_scale) + ptsToSpx(m_spacingY[1], m_scale) * std::max(0, m_nVisibleRows-1) + ptsToSpx(m_spacingY[2], m_scale);
		if( m_pRowSkins[0] != nullptr )
			totalPadding += m_pRowSkins[0]->_contentBorderSize(m_scale).h * m_nVisibleRows;
		
		spx availableHeight = m_size.h - totalPadding;
		
		int arrayBytes = (sizeof(PackLayout::Item)+sizeof(spx)) * m_nVisibleRows;
		PackLayout::Item* pItem = reinterpret_cast<PackLayout::Item*>(Base::memStackAlloc(arrayBytes));
		spx* pOutput = (spx*) &pItem[m_nVisibleRows];
		
		for( auto& row : rows )
		{
			if( row.m_bVisible )
			{
				pItem->def = row.m_cache.heightForColumnWidth;
				pItem->min = row.m_cache.minHeight;
				pItem->max = row.m_cache.maxHeight;
				pItem->weight = row.m_weight*65536;
				pItem++;
			}
		}
		
		m_pLayoutY->getItemSizes(pOutput, availableHeight, m_scale, m_nVisibleRows, pItem);
		
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
				if( row.m_height != row.m_cache.heightForColumnWidth )
				{
					row.m_height = row.m_cache.heightForColumnWidth;
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

			if( height != row.m_cache.heightForColumnWidth )
			{
				row.m_cache.heightForColumnWidth = height;
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

		row.m_bModified = false;
	}

	for (auto& column : columns)
		column.m_bModified = false;
}

//____ _updateModifiedChildSizes() ____________________________________________

void TablePanel::_updateAllChildSizes()
{
	auto slotIt = slots.begin();

	for (auto& row : rows)
	{
		for (auto& column : columns)
		{
			if (!slotIt->isEmpty())
				slotIt->_widget()->_resize({ column.m_width,row.m_height }, m_scale);

			slotIt++;
		}

		row.m_bModified = false;
	}

	for (auto& column : columns)
		column.m_bModified = false;
}


//____ _refreshRowCache() ______________________________________________________

bool TablePanel::_refreshRowCache( int row, TablePanelRow::Cache& cache, int scale ) const
{
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
			spx myDefaultHeight = p->_defaultSize(scale).h;
			spx myMinHeight = p->_minSize(scale).h;
			spx myMaxHeight = p->_maxSize(scale).h;
			spx myHeightForWidth = p->_matchingHeight(column.m_width, scale);
			
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

	if (defaultHeight != cache.defaultHeight || minHeight != cache.minHeight || 
		maxHeight != cache.maxHeight || heightForWidth != cache.heightForColumnWidth)
	{
		cache.defaultHeight = defaultHeight;
		cache.minHeight = minHeight;
		cache.maxHeight = maxHeight;
		cache.heightForColumnWidth = heightForWidth;

		return true;
	}
	else
		return false;
}

//____ _refreshColumnCache() ___________________________________________________

bool TablePanel::_refreshColumnCache( int column, TablePanelColumn::Cache& cache, int scale ) const
{
	auto colIt = columns.begin() + column;
	auto slotIt = slots.begin() + column;

	spx	defaultWidth = 0;
	spx minWidth = 0;
	spx maxWidth = spx_max;

	for (auto& row : rows)
	{
		Widget* p = slotIt->_widget();
		if (p)
		{
			spx myDefaultWidth = p->_defaultSize(scale).w;
			spx myMinWidth = p->_minSize(scale).w;
			spx myMaxWidth = p->_maxSize(scale).w;

			if (myDefaultWidth > defaultWidth)
				defaultWidth = myDefaultWidth;

			if (myMinWidth > minWidth)
				minWidth = myMinWidth;

			if (myMaxWidth < maxWidth)
				maxWidth = myMaxWidth;
		}

		slotIt += rows.size();
	}

	if (defaultWidth != cache.defaultWidth || minWidth != cache.minWidth ||
		maxWidth != cache.maxWidth )
	{
		cache.defaultWidth = defaultWidth;
		cache.minWidth = minWidth;
		cache.maxWidth = maxWidth;

		return true;
	}
	else
		return false;
}

//____ _sumOfPadding() ________________________________________________________

SizeSPX TablePanel::_sumOfPadding(int scale) const
{
	SizeSPX sizeAddition = m_skin.contentBorderSize(m_scale);

	if (m_pRowSkins[0])
	{
		auto rowBorder = m_pRowSkins[0]->_contentBorderSize(m_scale);
		sizeAddition.w += rowBorder.w;
		sizeAddition.h += rowBorder.h * m_nVisibleRows;
	}
	
	int nBetweenSpacingsX = std::max(0,m_nVisibleColumns-1);
	int nBetweenSpacingsY = std::max(0,m_nVisibleRows-1);

	sizeAddition.w += ptsToSpx(m_spacingX[0], m_scale) + ptsToSpx(m_spacingX[1], m_scale) * nBetweenSpacingsX + ptsToSpx(m_spacingX[2], m_scale);
	sizeAddition.h += ptsToSpx(m_spacingY[0], m_scale) + ptsToSpx(m_spacingY[1], m_scale) * nBetweenSpacingsY + ptsToSpx(m_spacingY[2], m_scale);

	return sizeAddition;
}


//____ _updateMinMaxDefaultSize() _____________________________________________

void TablePanel::_updateMinDefaultSize()
{
	SizeSPX minSize;
	SizeSPX defSize;

	for (auto& column : columns)
	{
		if (column.m_bVisible)
		{
			minSize.w += column.m_cache.minWidth;
			defSize.w += column.m_cache.defaultWidth;
		}
	}

	for (auto& row : rows)
	{
		if (row.m_bVisible)
		{
			minSize.h += row.m_cache.minHeight;
			defSize.h += row.m_cache.defaultHeight;
		}
	}

	SizeSPX sizeAddition = _sumOfPadding(m_scale);

	minSize += sizeAddition;
	defSize += sizeAddition;

	m_minSize = minSize;
	m_defaultSize = defSize;
}


//____ _refreshSlots() ________________________________________________________

// Called when widgets of the slots have changed.

void TablePanel::_refreshSlots(int ofs, Axis axis, int nSlots)
{
	int firstRow, lastRow;
	int firstColumn, lastColumn;

	if (axis == Axis::X)
	{
		firstRow = ofs / rows.size();
		lastRow = (ofs + nSlots - 1) / rows.size();

		if (lastRow != firstRow)
		{
			firstColumn = 0;
			lastColumn = columns.size() - 1;
		}
		else
		{
			firstColumn = ofs % rows.size();
			lastColumn = (ofs + nSlots - 1) % rows.size();
		}
	}
	else
	{
		firstRow = ofs / rows.size();
		lastRow = firstRow + nSlots - 1;

		if (lastRow >= rows.size())
		{
			firstRow = 0;
			lastRow = rows.size() - 1;
		}

		firstColumn = ofs % rows.size();
		lastColumn = (ofs / rows.size() + nSlots - 1) % rows.size();
	}

	SizeSPX defaultSize = m_defaultSize;

	bool bRowCacheChanged = false;
	bool bColumnCacheChanged = false;

	for (int row = firstRow; row <= lastRow; row++)
	{
		spx oldDefault = rows[row].m_cache.defaultHeight;
		bRowCacheChanged = _refreshRowCache(row, rows[row].m_cache, m_scale) || bRowCacheChanged;
		defaultSize.h += rows[row].m_cache.defaultHeight - oldDefault;
	}

	for (int column = firstColumn; column <= lastColumn; column++)
	{
		spx oldDefault = columns[column].m_cache.defaultWidth;
		bColumnCacheChanged = _refreshColumnCache(column, columns[column].m_cache, m_scale) || bColumnCacheChanged;
		defaultSize.w += columns[column].m_cache.defaultWidth - oldDefault;
	}

	bool bRefreshGeo = false;

	if (defaultSize != m_defaultSize)
	{
		m_defaultSize = defaultSize;
		bRefreshGeo = true;
	}

	if (bRowCacheChanged)
		bRefreshGeo = _refreshRows() || bRefreshGeo;

	if (bColumnCacheChanged)
		bRefreshGeo = _refreshColumns() || bRefreshGeo;

	if (bRefreshGeo)
	{
		_requestResize();
		_requestRender();
	}

	// Make sure widgets in our modified slots have the right size and scale.

	for (int i = ofs; i < ofs + nSlots; i++)
	{
		auto slotIt = slots.begin() + i;

		Widget* pWidget = slotIt->_widget();

		if (pWidget)
		{
			int row = i / columns.size();
			int column = i % columns.size();

			SizeSPX slotSize = { columns[column].m_width, rows[row].m_height };

			if (pWidget->_size() != slotSize || pWidget->_scale() != m_scale)
				pWidget->_resize(slotSize, m_scale);
		}
	}
}

//____ _object() ______________________________________________________________

Object*	TablePanel::_object()
{
	return this;
}

//____ _didAddEntries() _______________________________________________________

void TablePanel::_didAddEntries(TablePanelRow* pEntry, int nb)
{
	int ofs = int(pEntry - &*rows.begin());

	slots._insertRows(ofs, nb);

	int nbVisible = 0;
	for (int i = 0; i < nb; i++)
	{
		if (pEntry->m_bVisible)
		{
			_refreshRowCache(ofs + i, rows[ofs+i].m_cache, m_scale);
			nbVisible++;
		}
	}

	if (nbVisible > 0)
	{
		m_nVisibleRows += nbVisible;
		
		_updateMinDefaultSize();

		_requestRender();
		_requestResize();
	}
}

void TablePanel::_didAddEntries(TablePanelColumn* pEntry, int nb)
{
	int ofs = int(pEntry - &*columns.begin());

	slots._insertColumns(ofs, nb);

	int nbVisible = 0;
	for (int i = 0; i < nb; i++)
	{
		if (pEntry->m_bVisible)
		{
			_refreshColumnCache(ofs + i, columns[ofs+i].m_cache, m_scale);
			nbVisible++;
		}
	}

	if (nbVisible > 0)
	{
		m_nVisibleColumns += nbVisible;
		_updateMinDefaultSize();

		_requestRender();
		_requestResize();
	}
}

//____ _didMoveEntries() ______________________________________________________

void TablePanel::_didMoveEntries(TablePanelRow* pFrom, TablePanelRow* pTo, int nb)
{
	//TODO: Implement!!!
}

void TablePanel::_didMoveEntries(TablePanelColumn* pFrom, TablePanelColumn* pTo, int nb)
{
	//TODO: Implement!!!
}

//____ _willEraseEntries() ____________________________________________________

void TablePanel::_willEraseEntries(TablePanelRow* pEntry, int nb)
{
	int ofs = int(pEntry - &*rows.begin());

	slots._deleteRows(ofs, nb);

	int nbVisible = 0;
	for (int i = 0; i < nb; i++)
	{
		if (pEntry->m_bVisible)
			nbVisible++;
	}

	if (nbVisible > 0)
	{
		m_nVisibleRows -= nbVisible;

		_updateMinDefaultSize();
		_requestRender();
		_requestResize();
	}
}

void TablePanel::_willEraseEntries(TablePanelColumn* pEntry, int nb)
{

	int ofs = int(pEntry - &*columns.begin());

	slots._deleteColumns(ofs, nb);

	int nbVisible = 0;
	for (int i = 0; i < nb; i++)
	{
		if (pEntry->m_bVisible)
			nbVisible++;
	}

	if (nbVisible > 0)
	{
		m_nVisibleColumns -= nbVisible;

		_updateMinDefaultSize();
		_requestRender();
		_requestResize();
	}
}



void TablePanel::_hideRows(TablePanelRow* pStart, int nb)
{

}

void TablePanel::_unhideRows(TablePanelRow* pStart, int nb)
{

}

void TablePanel::_hideColumns(TablePanelColumn* pStart, int nb)
{

}

void TablePanel::_unhideColumns(TablePanelColumn* pStart, int nb)
{

}

void TablePanel::_reweightRows(TablePanelRow* pEntry, int nb, float weight)
{

}

void TablePanel::_reweightRows(TablePanelRow* pEntry, int nb, const float* pWeights)
{

}

void TablePanel::_reweightColumns(TablePanelColumn* pEntry, int nb, float weight)
{

}

void TablePanel::_reweightColumns(TablePanelColumn* pEntry, int nb, const float* pWeights)
{

}



} // namespace wg
