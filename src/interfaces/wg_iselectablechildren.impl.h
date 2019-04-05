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

#ifndef	WG_ISELECTABLECHILDREN_IMPL_DOT_H
#define	WG_ISELECTABLECHILDREN_IMPL_DOT_H
#pragma once

#include <wg_ihideablechildren.h>
#include <wg_ihideablechildren.impl.h>

#define INSTANTIATE_SELECTABLECHILDREN(SlotType,HolderType) \
	INSTANTIATE_HIDEABLECHILDREN( SlotType, HolderType ) \
	template class ISelectableChildren< SlotType, HolderType >;


namespace wg
{
	template < class SlotType, class HolderType>
	void ISelectableChildren<SlotType, HolderType>::select(int index)
	{
		//TODO: Assert

		m_pHolder->_selectSlots(m_pSlotArray->slot(index), 1);
	}

	template < class SlotType, class HolderType>
	void ISelectableChildren<SlotType, HolderType>::select(iterator it)
	{
		//TODO: Assert

		m_pHolder->_selectSlots(it._slot(), 1);
	}

	template < class SlotType, class HolderType>
	void ISelectableChildren<SlotType, HolderType>::select(int index, int amount)
	{
		//TODO: Assert

		m_pHolder->_selectSlots(m_pSlotArray->slot(index), amount);
	};

	template < class SlotType, class HolderType>
	void ISelectableChildren<SlotType, HolderType>::select(iterator beg, iterator end)
	{
		//TODO: Assert

		m_pHolder->_selectSlots(beg._slot(), int(end._slot() - beg._slot()) );
	};

	template < class SlotType, class HolderType>
	void ISelectableChildren<SlotType, HolderType>::selectAll()
	{
		m_pHolder->_selectSlots(m_pSlotArray->slot(0), m_pSlotArray->size());
	};


	template < class SlotType, class HolderType>
	void ISelectableChildren<SlotType, HolderType>::unselect(int index)
	{
		//TODO: Assert

		m_pHolder->_unselectSlots(m_pSlotArray->slot(index), 1);
	}

	template < class SlotType, class HolderType>
	void ISelectableChildren<SlotType, HolderType>::unselect(iterator it)
	{
		//TODO: Assert

		m_pHolder->_unselectSlots(it._slot(), 1);
	}

	template < class SlotType, class HolderType>
	void ISelectableChildren<SlotType, HolderType>::unselect(int index, int amount)
	{
		//TODO: Assert

		m_pHolder->_unselectSlots(m_pSlotArray->slot(index), amount);
	};

	template < class SlotType, class HolderType>
	void ISelectableChildren<SlotType, HolderType>::unselect(iterator beg, iterator end)
	{
		//TODO: Assert

		m_pHolder->_unselectSlots(beg._slot(), int(end._slot() - beg._slot()) );
	};

	template < class SlotType, class HolderType>
	void ISelectableChildren<SlotType, HolderType>::unselectAll()
	{
		m_pHolder->_unselectSlots(m_pSlotArray->slot(0), m_pSlotArray->size());
	};


	template < class SlotType, class HolderType>
	bool ISelectableChildren<SlotType, HolderType>::isSelected(int index)
	{
		//TODO: Assert

		return m_pSlotArray->slot(index)->pWidget->state().isSelected();
	}

	template < class SlotType, class HolderType>
	bool ISelectableChildren<SlotType, HolderType>::isSelected(iterator it)
	{
		//TODO: Assert

		return it._slot()->pWidget->state().isSelected();
	}

} // namespace wg


#endif //WG_ISELECTABLECHILDREN_IMPL_DOT_H
