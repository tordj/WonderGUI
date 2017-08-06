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

#ifndef	WG_SELECTABLECHILDREN_IMPL_DOT_H
#define	WG_SELECTABLECHILDREN_IMPL_DOT_H
#pragma once

#include <wg_hideablechildren.h>
#include <wg_hideablechildren.impl.h>

#define INSTANTIATE_SELECTABLECHILDREN(SlotType,HolderType) \
	INSTANTIATE_HIDEABLECHILDREN( ##SlotType, ##HolderType ) \
	template class SelectableChildren< ##SlotType, ##HolderType >;


namespace wg
{
	template < class SlotType, class HolderType>
	void SelectableChildren<SlotType, HolderType>::select(int index)
	{
		//TODO: Assert

		SlotType * pSlot = m_pSlotArray->slot(index);
		if (pSlot->bVisible)
			m_pHolder->_selectSlots(pSlot, 1);
	}

	template < class SlotType, class HolderType>
	void SelectableChildren<SlotType, HolderType>::select(iterator it)
	{
		//TODO: Assert

		SlotType * pSlot = it._slot();
		if (pSlot->bVisible)
			m_pHolder->_selectSlots(pSlot, 1);
	}

	template < class SlotType, class HolderType>
	void SelectableChildren<SlotType, HolderType>::unselect(int index)
	{
		//TODO: Assert

		SlotType * pSlot = m_pSlotArray->slot(index);
		if (pSlot->bVisible)
			m_pHolder->_unselectSlots(pSlot, 1);
	}

	template < class SlotType, class HolderType>
	void SelectableChildren<SlotType, HolderType>::unselect(iterator it)
	{
		//TODO: Assert

		SlotType * pSlot = it._slot();
		if (pSlot->bVisible)
			m_pHolder->_unselectSlots(pSlot, 1);
	}

	template < class SlotType, class HolderType>
	bool SelectableChildren<SlotType, HolderType>::isSelected(int index)
	{
		//TODO: Assert

		return m_pSlotArray->slot(index)->pWidget->state().isSelected();
	}

	template < class SlotType, class HolderType>
	bool SelectableChildren<SlotType, HolderType>::isSelected(iterator it)
	{
		//TODO: Assert

		return it._slot()->pWidget->state().isSelected();
	}

} // namespace wg


#endif //WG_WG_HIDEABLECHILDREN_IMPL_DOT_H
