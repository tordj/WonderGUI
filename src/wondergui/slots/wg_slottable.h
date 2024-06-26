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

#ifndef	WG_SLOTTABLE_DOT_H
#define	WG_SLOTTABLE_DOT_H
#pragma once

#include <wg_base.h>

namespace wg
{




	//____ SlotTable _________________________________________________________

	class SlotTable
	{
	public:

		class Holder
		{
			virtual void	_refreshTable() = 0;
			virtual void	_refreshSlots(int ofs, Axis axis, int nSlots) = 0;
		};

		class RowAccess
		{
			friend class SlotTable;
			
		public:
			inline DynamicSlot& operator[](int index) { return m_pSlot[index]; }
			inline const DynamicSlot&	operator[](int index) const { return m_pSlot[index]; }

		private:
			RowAccess(DynamicSlot * pSlot ) : m_pSlot(pSlot) {}
			
			DynamicSlot * m_pSlot;
		};
		
		
		using		iterator = DynamicSlot *;
		using		const_iterator = const DynamicSlot *;

		SlotTable(SlotHolder * pHolder, Holder * pTableHolder ) : m_pHolder(pHolder), m_pTableHolder(pTableHolder) {}
		~SlotTable() {  }

		
		iterator			begin() { return m_slots.begin(); }
		const_iterator		begin() const { return m_slots.begin(); }

		iterator			end() { return m_slots.end(); }
		const_iterator		end() const { return m_slots.end(); }
		
		void				clear();

		int					rows() const { return m_nRows; }
		int					columns() const { return m_nColumns; }
		
		DynamicSlot&		at(int row, int column);
		const DynamicSlot&	at(int row, int column) const;
				
		void				replaceRow(int index, const std::initializer_list<Widget_p>& widgets);
		void				replaceColumn(int index, const std::initializer_list<Widget_p>& widgets);

		void				clearRows(int index, int nb = 1 );
		void				clearColumns(int index, int nb = 1 );

		void				replaceWidgets(iterator it, Axis axis, const std::initializer_list<Widget_p>& widgets);
		void				clearWidgets(iterator it, Axis axis, int nb );

		//.____ Internal ______________________________________________________

		void				_resize( int nbRows, int nbColumns, int rowIncision, int columnIncision );
		void				_reserve( int rows, int columns );

		void				_slotsUpdated(int start, Axis axis, int nb);

		
		//.____ Operators __________________________________________

//		inline SlotTable<DynamicSlot>& operator<<(const Widget_p& pWidget) { pushBack(pWidget); return *this; }

		inline RowAccess 		operator[](int index) { return RowAccess(&m_slots[index*m_columnCapacity]); }
		inline const RowAccess	operator[](int index) const { return RowAccess(&m_slots[index*m_columnCapacity]); }

	protected:
		
		inline SlotHolder *			_holder() { return m_pHolder; }
		inline const SlotHolder *	_holder() const { return m_pHolder; }

		void		_moveRows( int fromIndex, int toIndex, int nb );
		void		_clearRows( int startIndex int nb );

		void		_moveColumns( int fromIndex, int toIndex, int nb );
		void		_clearColumns( int startIndex int nb );

		
	private:

		int		m_nRows = 0;
		int		m_nColumns = 0;
		int		m_rowCapacity = 0;
		int		m_columnCapacity = 0;
		
		DynamicSlot		m_dummySlot;
		
		std::vector<DynamicSlot>	m_slots;

		Holder *					m_pTableHolder;
		SlotHolder * 				m_pHolder;
	};


} // namespace wg
#endif //WG_SLOTTABLE_DOT_H
