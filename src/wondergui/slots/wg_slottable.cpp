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

#include <wg_slottable.h>

using namespace wg;


//____ clear() ________________________________________________________________

void SlotTable::clear()
{
	for( auto& slot : m_slots )
		slot._clearWidget();
	
	m_pTableHolder->_refreshTable();
}

//____ at() ___________________________________________________________________

DynamicSlot& SlotTable::at(int row, int column)
{
	if( row < 0 || row >= m_nRows || column < 0 || column >= m_nColumns )
	{
		Base::throwError(ErrorLevel::Error, ErrorCode::OutOfRange, "Row or column is out of range.", m_pTableHolder->_object(), nullptr, __func__, __FILE__, __LINE__);
		return m_dummySlot;
	}
	
	return m_slots[row*m_nColumns+column];
}

const DynamicSlot& SlotTable::at(int row, int column) const
{
	if( row < 0 || row >= m_nRows || column < 0 || column >= m_nColumns )
	{
		Base::throwError(ErrorLevel::Error, ErrorCode::OutOfRange, "Row or column is out of range.", m_pTableHolder->_object(), nullptr, __func__, __FILE__, __LINE__);
		return m_dummySlot;
	}
	
	return m_slots[row* m_nColumns +column];
}
		
//____ replaceRow() ___________________________________________________________

int SlotTable::replaceRow(int index, const std::initializer_list<Widget_p>& widgets)
{
	if( index < 0 || index >= m_nRows )
	{
		Base::throwError(ErrorLevel::Error, ErrorCode::OutOfRange, "Row index is out of range.", m_pTableHolder->_object(), nullptr, __func__, __FILE__, __LINE__);
		return 0;
	}
	
	DynamicSlot * p = m_slots.data() + index * m_nColumns;
	
	int nb = std::min(int(widgets.size()), m_nColumns );
	
	for( auto it = widgets.begin() ; it < widgets.begin() + nb ; it++ )
	{
		p->_setWidget(*it);
		p++;
	}
	
	_slotsUpdated(index*m_nColumns, Axis::X, nb);
	return nb;
}

//____ replaceColumn() ________________________________________________________

int SlotTable::replaceColumn(int index, const std::initializer_list<Widget_p>& widgets)
{
	if( index < 0 || index >= m_nColumns )
	{
		Base::throwError(ErrorLevel::Error, ErrorCode::OutOfRange, "Column index is out of range.", m_pTableHolder->_object(), nullptr, __func__, __FILE__, __LINE__);
		return 0;
	}
	
	DynamicSlot * p = m_slots.data() + index;

	int nb = std::min(int(widgets.size()), m_nRows );
	
	for( auto it = widgets.begin() ; it < widgets.begin() + nb ; it++ )
	{
		p->_setWidget(*it);
		p += m_nColumns;
	}
	
	_slotsUpdated(index, Axis::Y, nb);
	return nb;
}

//____ clearRows() ____________________________________________________________

void SlotTable::clearRows(int index, int nb )
{
	if( index < 0 || index + nb > m_nRows )
	{
		Base::throwError(ErrorLevel::Error, ErrorCode::OutOfRange, "Specified range is out of range.", m_pTableHolder->_object(), nullptr, __func__, __FILE__, __LINE__);
		return;
	}

	// Turn index and nb from specifying rows to slots.

	index *= m_nColumns;		
	nb *= m_nColumns;

	//

	DynamicSlot* p = m_slots.data() + index;

	for( int x = 0 ; x < nb ; x++ )
		p[x]._clearWidget();

	_slotsUpdated(index, Axis::X, nb);
}

//____ clearColumns() _________________________________________________________

void SlotTable::clearColumns(int index, int nb )
{
	if( index < 0 || index + nb > m_nColumns )
	{
		Base::throwError(ErrorLevel::Error, ErrorCode::OutOfRange, "Specified range is out of range.", m_pTableHolder->_object(), nullptr, __func__, __FILE__, __LINE__);
		return;
	}
	
	for( int x = index ; x < index + nb ; x++ )
	{
		DynamicSlot * p = m_slots.data() + x;

		for( int y = 0 ; y < m_nRows ; y++ )
			p[y*m_nColumns]._clearWidget();
	}
	
	_slotsUpdated(index, Axis::Y, nb*m_nRows);
}

//____ replaceWidgets() _______________________________________________________

int SlotTable::replaceWidgets(iterator it, Axis axis, const std::initializer_list<Widget_p>& widgets)
{
	int ofs = it - m_slots.begin();
		
	auto itWidgets = widgets.begin();
	
	int nb = 0;
	
	if( axis == Axis::X )
	{
		int ofs2 = ofs;
		while( itWidgets != widgets.end() )
		{
			m_slots[ofs2]._setWidget(* itWidgets++);
			ofs2++;
			nb++;
			if( ofs2 == m_slots.size() )
				break;
		}
	}
	else
	{
		int x = ofs % m_nColumns;
		int y = ofs / m_nColumns;
		
		while( itWidgets != widgets.end() )
		{
			m_slots[x + y * m_nColumns]._setWidget(* itWidgets++);
			y++;
			nb++;
			if( y == m_nRows )
			{
				y = 0;
				x++;
				if( x == m_nColumns )
					break;
			}
		}
	}
	
	_slotsUpdated(ofs, axis, nb );
	return nb;
}

//____ clearWidgets() _________________________________________________________

void SlotTable::clearWidgets(iterator it, Axis axis, int nb )
{
	int ofs = it - m_slots.begin();

	if (ofs + nb > m_slots.size())
	{
		Base::throwError(ErrorLevel::Error, ErrorCode::OutOfRange, "Specified range is out of range.", m_pTableHolder->_object(), nullptr, __func__, __FILE__, __LINE__);
		return;
	}

	if (axis == Axis::X)
	{
		for (int i = ofs; i < ofs + nb; i++)
			m_slots[i]._clearWidget();
	}
	else
	{
		int x = ofs % m_nColumns;
		int y = ofs / m_nColumns;

		for (int i = 0; i < nb; i++)
		{
			m_slots[x + y * m_nColumns]._clearWidget();
			y++;
			if (y == m_nRows)
			{
				y = 0;
				x++;
			}
		}
	}

	_slotsUpdated(ofs, axis, nb);
}

//____ _resize() ______________________________________________________________

void SlotTable::_resize( int nRows, int nColumns, int rowIncision, int columnIncision )
{

	if( nRows > m_nRows || nColumns > m_nColumns )
	{
		
	}
	else
	{
		// Handle change in rows.
		
		if( nRows < m_nRows )
		{
			assert( rowIncision <= nRows );
			
			int rowsRemoved = m_nRows - nRows;
			int rowsToMove = m_nRows - (rowIncision + rowsRemoved);
			int rowsToClear = rowsRemoved - rowsToMove;				// Move clears the slot, so we only need to clear the ones that have not been moved from.
			
			if( rowsToMove > 0 )
				_moveRows( rowIncision + rowsRemoved, rowIncision, rowsToMove );

			if( rowsToClear > 0 )
				_clearRows( m_nRows - rowsToClear, rowsToClear );
		}
		else if( nRows > m_nRows )
		{
			assert( rowIncision <= m_nRows );

			int rowsAdded = nRows - m_nRows;
			int rowsToMove = m_nRows - rowIncision;
			
			if( rowsToMove > 0 )
				_moveRows( rowIncision, rowIncision + rowsAdded, rowsToMove );
		}
		
		m_nRows = nRows;
		
		
		// Handle change in columns
		
		if( nColumns < m_nColumns )
		{
			assert( columnIncision <= nColumns );
			
			int columnsRemoved = m_nColumns - nColumns;
			int columnsToMove = m_nColumns - (columnIncision + columnsRemoved);
			int columnsToClear = columnsRemoved - columnsToMove;				// Move clears the slot, so we only need to clear the ones that have not been moved from.
			
			if( columnsToMove > 0 )
				_moveColumns( columnIncision + columnsRemoved, columnIncision, columnsToMove );

			if( columnsToClear > 0 )
				_clearColumns( m_nColumns - columnsToClear, columnsToClear );
		}
		else if( nColumns > m_nColumns )
		{
			assert( columnIncision <= m_nColumns );

			int columnsAdded = nColumns - m_nColumns;
			int columnsToMove = m_nColumns - columnIncision;
			
			if( columnsToMove > 0 )
				_moveColumns( columnIncision, columnIncision + columnsAdded, columnsToMove );
		}
		
		m_nColumns = nColumns;
	}

	m_slots.resize(nRows * nColumns);
}


//____ _reserve() _____________________________________________________________

void SlotTable::_reserve( int rows, int columns )
{
	m_slots.reserve(rows * columns);	
}


//____ _slotsUpdated() ________________________________________________________

void SlotTable::_slotsUpdated(int start, Axis axis, int nb)
{
	m_pTableHolder->_refreshSlots(start,axis,nb);
}

//____ _moveRows() ____________________________________________________________

void SlotTable::_moveRows( int fromIndex, int toIndex, int nb )
{
	DynamicSlot * pSrc = m_slots.data() + fromIndex * m_nColumns;
	DynamicSlot * pDst = m_slots.data() + toIndex * m_nColumns;

	if( pDst < pSrc || nb < 2 )
	{
		for( int i = 0 ; i < nb * m_nColumns ; i++ )
			* pDst++ = std::move(* pSrc++);			
	}
	else
	{
		pSrc += nb * m_nColumns;
		pDst += nb * m_nColumns;

		for (int i = 0; i < nb * m_nColumns; i++)
			* --pDst = std::move(* --pSrc);
	}
}

//____ _clearRows() ____________________________________________________________

void SlotTable::_clearRows( int startIndex, int nb )
{
	DynamicSlot * p = m_slots.data() + startIndex * m_nColumns;

	for( int i = 0 ; i < nb * m_nColumns ; i++ )
		p[i]._clearWidget();
}

//____ _moveColumns() ____________________________________________________________

void SlotTable::_moveColumns( int fromIndex, int toIndex, int nb )
{
	DynamicSlot * pSrc = m_slots.data() + fromIndex;
	DynamicSlot * pDst = m_slots.data() + toIndex;

	if( pDst < pSrc || nb < 2 )
	{
		for( int y = 0 ; y < m_nRows ; y++ )
		{
			for( int x = 0 ; x < nb ; x++ )
				pDst[x] = std::move(pSrc[x]);
			
			pSrc += m_nColumns;
			pDst += m_nColumns;
		}
	}
	else
	{
		pSrc += nb;
		pDst += nb;

		for( int y = 0 ; y < m_nRows ; y++ )
		{
			for( int x = nb-1 ; x >= 0 ; x++ )
				pDst[x] = std::move(pSrc[x]);

			pSrc += m_nColumns;
			pDst += m_nColumns;
		}
	}
}

//____ _clearColumns() ____________________________________________________________

void SlotTable::_clearColumns( int startIndex, int nb )
{
	DynamicSlot * p = m_slots.data() + startIndex;

		for( int y = 0 ; y < m_nRows ; y++ )
		{
			for( int x = 0 ; x < nb ; x++ )
				p[x]._clearWidget();
			
			p += m_nColumns;
		}
}
