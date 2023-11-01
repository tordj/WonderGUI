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

#ifndef	WG_DYNAMICSLOTTABLE_DOT_H
#define	WG_DYNAMICSLOTTABLE_DOT_H
#pragma once

#include <wg_base.h>

namespace wg
{




	//____ DynamicSlotTable _________________________________________________________

	template<class SlotType> 
	class DynamicSlotTable
	{
	public:

		using		iterator = SlotType*;
		using		const_iterator = const SlotType*;

		DynamicSlotTable(SlotHolder * pHolder) : m_pHolder(pHolder) {}
		~DynamicSlotTable() {}


		struct Blueprint
		{
			bool	visible = true;
			float	weight = 1.f;
		};

		void		reserve(int n);

		int			insert(int index, const Blueprint& blueprint);
		int			insert(int index, const Blueprint& blueprint, const std::initializer_list<Widget_p>& entries);

		void		erase(int index);
		void		erase(int index, int amount);

		int			pushBack(int index, const Blueprint& blueprint);
		int			pushBack(int index, const Blueprint& blueprint, const std::initializer_list<Widget_p>& entries);


		int			addColumns(int amount);
		int			addColumns(const std::initializer_list<const Blueprint& blueprint>& entries);

		int			addColumn(const Blueprint& blueprint);
		int			addColumn(int amount, const Blueprint& blueprint);

		int			insertColumn(int index, const Blueprint& blueprint);
		int			insertColumn(int index, int amount, const Blueprint& blueprint);

		void		eraseColumn(int index);
		void		eraseColumn(int index, int amount);


		void		clear();


		int			nbRows() const { return m_nbRows; }
		int			nbColumns() const { return m_nbColumns; }



		
	protected:

		inline SlotHolder *		_holder() { return m_pHolder; }
		inline const SlotHolder *	_holder() const { return m_pHolder; }


	private:

		int						m_nbColumns;
		int						m_nbRows;

		std::vector<SlotType>	m_slots;

		std::vector<int>		m_order;



		SlotHolder *			m_pHolder;
	};




} // namespace wg
#endif //WG_DYNAMICSLOTTABLE_DOT_H

