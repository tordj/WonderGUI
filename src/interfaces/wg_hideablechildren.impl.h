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

#ifndef	WG_HIDEABLECHILDREN_IMPL_DOT_H
#define	WG_HIDEABLECHILDREN_IMPL_DOT_H
#pragma once

#include <wg_dynamicchildgroup.h>
#include <wg_dynamicchildgroup.impl.h>

#define INSTANTIATE_HIDEABLECHILDREN(SlotType,HolderType) \
	INSTANTIATE_DYNAMICCHILDGROUP( ##SlotType, ##HolderType ) \
	template class HideableChildren<  ##SlotType, ##HolderType >;


namespace wg
{
//	using		iterator = SlotIterator<SlotType>;
//	using		ChildGroup<SlotType, HolderType>::m_pSlotArray;
//	using		ChildGroup<SlotType, HolderType>::m_pHolder;


	template < class SlotType, class HolderType>
	void HideableChildren<SlotType, HolderType>::hide(int index)
	{
		//TODO: Assert

		m_pHolder->_hideSlots(m_pSlotArray->slot(index), 1);
	};

	template < class SlotType, class HolderType>
	void HideableChildren<SlotType, HolderType>::hide(iterator it)
	{
		//TODO: Assert

		m_pHolder->_hideSlots(it._slot(), 1);
	};


	template < class SlotType, class HolderType>
	void HideableChildren<SlotType, HolderType>::hide(int index, int amount)
	{
		//TODO: Assert

		m_pHolder->_hideSlots(m_pSlotArray->slot(index), amount);
	};

	template < class SlotType, class HolderType>
	void HideableChildren<SlotType, HolderType>::hide(iterator beg, iterator end)
	{
		//TODO: Assert

		m_pHolder->_hideSlots(beg._slot(), end._slot() - beg._slot());
	};


	template < class SlotType, class HolderType>
	void HideableChildren<SlotType, HolderType>::unhide(int index)
	{
		//TODO: Assert

		m_pHolder->_unhideSlots(m_pSlotArray->slot(index), 1);
	};

	template < class SlotType, class HolderType>
	void HideableChildren<SlotType, HolderType>::unhide(iterator it)
	{
		//TODO: Assert

		m_pHolder->_unhideSlots(it._slot(), 1);
	};

	template < class SlotType, class HolderType>
	void HideableChildren<SlotType, HolderType>::unhide(int index, int amount)
	{
		//TODO: Assert

		m_pHolder->_unhideSlots(m_pSlotArray->slot(index), amount);
	};

	template < class SlotType, class HolderType>
	void HideableChildren<SlotType, HolderType>::unhide(iterator beg, iterator end)
	{
		//TODO: Assert

		m_pHolder->_unhideSlots(beg._slot(), end._slot() - beg._slot());
	};


	template < class SlotType, class HolderType>
	bool HideableChildren<SlotType, HolderType>::isVisible(int index)
	{
		//TODO: Assert

		return m_pSlotArray->slot(index)->bVisible;
	}

	template < class SlotType, class HolderType>
	bool HideableChildren<SlotType, HolderType>::isVisible(iterator it)
	{
		//TODO: Assert

		return it._slot()->bVisible;
	}


} // namespace wg


#endif //WG_WG_HIDEABLECHILDREN_IMPL_DOT_H
