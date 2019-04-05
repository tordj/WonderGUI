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

#ifndef	WG_IDYNAMICCHILDREN_IMPL_DOT_H
#define	WG_IDYNAMICCHILDREN_IMPL_DOT_H
#pragma once

#include <wg_ichildren.h>
#include <wg_ichildren.impl.h>

#define INSTANTIATE_DYNAMICCHILDREN(SlotType,HolderType) \
	INSTANTIATE_CHILDREN( SlotType, HolderType ) \
	template class IDynamicChildren< SlotType, HolderType >;

namespace wg
{

	template < class SlotType, class HolderType>
	ChildIterator<SlotType> IDynamicChildren<SlotType, HolderType>::add(Widget * pWidget)
	{
		SlotType * pSlot = m_pSlotArray->add();
		pSlot->replaceWidget(m_pHolder->_widgetHolder(), pWidget);
		m_pHolder->_didAddSlots(pSlot, 1);
		return iterator(pSlot);
	}

	template < class SlotType, class HolderType>
	ChildIterator<SlotType> IDynamicChildren<SlotType, HolderType>::add(const Widget_p pWidgets[], int amount)
	{
		//TODO: Add assert

		SlotType * pSlot = m_pSlotArray->add(amount);

		for (int i = 0; i < amount; i++)
			pSlot[i].replaceWidget(m_pHolder->_widgetHolder(), pWidgets[i]);
		m_pHolder->_didAddSlots(pSlot, amount);
		return iterator(pSlot);
	}

	template < class SlotType, class HolderType>
	ChildIterator<SlotType> IDynamicChildren<SlotType, HolderType>::insert(int index, Widget * pWidget)
	{
		//TODO: Add assert

		SlotType * pSlot = m_pSlotArray->insert(index);
		pSlot->replaceWidget(m_pHolder->_widgetHolder(), pWidget);
		m_pHolder->_didAddSlots(pSlot, 1);
		return iterator(pSlot);
	}

	template < class SlotType, class HolderType>
	ChildIterator<SlotType> IDynamicChildren<SlotType, HolderType>::insert(iterator pos, Widget * pWidget)
	{
		//TODO: Add assert

		SlotType * pSlot = m_pSlotArray->insert(pos._slot());
		pSlot->replaceWidget(m_pHolder->_widgetHolder(), pWidget);
		m_pHolder->_didAddSlots(pSlot, 1);
		return iterator(pSlot);
	}

	template < class SlotType, class HolderType>
	ChildIterator<SlotType> IDynamicChildren<SlotType, HolderType>::insert(int index, const Widget_p pWidgets[], int amount)
	{
		//TODO: Add assert

		SlotType * pSlot = m_pSlotArray->insert(index, amount);

		for (int i = 0; i < amount; i++)
			pSlot[i].replaceWidget(m_pHolder->_widgetHolder(), pWidgets[i]);
		m_pHolder->_didAddSlots(pSlot, amount);
		return iterator(pSlot);
	}

	template < class SlotType, class HolderType>
	ChildIterator<SlotType> IDynamicChildren<SlotType, HolderType>::insert(iterator pos, const Widget_p pWidgets[], int amount)
	{
		//TODO: Add assert

		SlotType * pSlot = m_pSlotArray->insert(pos._slot(), amount);

		for (int i = 0; i < amount; i++)
			pSlot[i].replaceWidget(m_pHolder->_widgetHolder(), pWidgets[i]);
		m_pHolder->_didAddSlots(pSlot, amount);
		return iterator(pSlot);
	}

	template < class SlotType, class HolderType>
	ChildIterator<SlotType> IDynamicChildren<SlotType, HolderType>::remove(int index)
	{
		//TODO: Add assert

		SlotType * pSlot = m_pSlotArray->slot(index);
		m_pHolder->_willRemoveSlots(pSlot, 1);
		return iterator(m_pSlotArray->remove(index));
	}

	template < class SlotType, class HolderType>
	ChildIterator<SlotType> IDynamicChildren<SlotType, HolderType>::remove(iterator pos)
	{
		//TODO: Add assert

		m_pHolder->_willRemoveSlots(pos._slot(), 1);
		return iterator(m_pSlotArray->remove(pos._slot()));
	}

	template < class SlotType, class HolderType>
	ChildIterator<SlotType> IDynamicChildren<SlotType, HolderType>::remove(int index, int amount)
	{
		//TODO: Add assert

		SlotType * pSlot = m_pSlotArray->slot(index);
		m_pHolder->_willRemoveSlots(pSlot, amount);
		return iterator(m_pSlotArray->remove(index, amount));
	}

	template < class SlotType, class HolderType>
	ChildIterator<SlotType> IDynamicChildren<SlotType, HolderType>::remove(iterator beg, iterator end)
	{
		//TODO: Add assert

		m_pHolder->_willRemoveSlots(beg._slot(), int(end._slot() - beg._slot()) );
		return iterator(m_pSlotArray->remove(beg._slot(), end._slot()));
	}

	template < class SlotType, class HolderType>
	void IDynamicChildren<SlotType, HolderType>::clear()
	{
		if (m_pSlotArray->isEmpty())
			return;
		m_pHolder->_willRemoveSlots(m_pSlotArray->begin(), m_pSlotArray->size());
		m_pSlotArray->clear();
	}

	template < class SlotType, class HolderType>
	void IDynamicChildren<SlotType, HolderType>::moveToBack(int index)
	{
		//TODO: Assert

		SlotType * pFrom = m_pSlotArray->slot(index);
		SlotType * pTo = m_pSlotArray->end();

		m_pSlotArray->move(pFrom, pTo);
		m_pHolder->_didMoveSlots(pFrom, pTo, 1);
	}

	template < class SlotType, class HolderType>
	ChildIterator<SlotType> IDynamicChildren<SlotType, HolderType>::moveToBack(iterator it)
	{
		//TODO: Assert

		SlotType * pFrom = it._slot();
		SlotType * pTo = m_pSlotArray->end();

		m_pSlotArray->move(pFrom, pTo);
		m_pHolder->_didMoveSlots(pFrom, pTo, 1);
		return iterator(m_pSlotArray->last());
	}

	template < class SlotType, class HolderType>
	void IDynamicChildren<SlotType, HolderType>::moveToFront(int index)
	{
		//TODO: Assert

		SlotType * pFrom = m_pSlotArray->slot(index);
		SlotType * pTo = m_pSlotArray->begin();

		m_pSlotArray->move(pFrom, pTo);
		m_pHolder->_didMoveSlots(pFrom, pTo, 1);
	}

	template < class SlotType, class HolderType>
	ChildIterator<SlotType> IDynamicChildren<SlotType, HolderType>::moveToFront(iterator it)
	{
		//TODO: Assert

		SlotType * pFrom = it._slot();
		SlotType * pTo = m_pSlotArray->begin();

		m_pSlotArray->move(pFrom, pTo);
		m_pHolder->_didMoveSlots(pFrom, pTo, 1);
		return iterator(m_pSlotArray->begin());
	}

	template < class SlotType, class HolderType>
	void IDynamicChildren<SlotType, HolderType>::moveBefore(int index, int sibling)
	{
		//TODO: Assert

		if (index < sibling)
			sibling--;

		if (index != sibling)
		{
			SlotType * pFrom = m_pSlotArray->slot(index);
			SlotType * pTo = m_pSlotArray->slot(sibling);

			m_pSlotArray->move(pFrom, pTo);
			m_pHolder->_didMoveSlots(pFrom, pTo, 1);
		}
	}

	template < class SlotType, class HolderType>
	ChildIterator<SlotType> IDynamicChildren<SlotType, HolderType>::moveBefore(iterator it, iterator sibling)
	{
		//TODO: Assert

		if (it < sibling)
			sibling--;

		if (it != sibling)
		{
			SlotType * pFrom = it._slot();
			SlotType * pTo = sibling._slot();

			m_pSlotArray->move(pFrom, pTo);
			m_pHolder->_didMoveSlots(pFrom, pTo, 1);
		}

		return sibling;
	}

} // namespace wg

#endif //WG_IDYNAMICCHILDREN_IMPL_DOT_H
