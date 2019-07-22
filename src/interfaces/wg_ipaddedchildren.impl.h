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

#ifndef	WG_IPADDEDCHILDREN_IMPL_DOT_H
#define	WG_IPADDEDCHILDREN_IMPL_DOT_H
#pragma once

#include <wg_ihideablechildren.h>
#include <wg_ihideablechildren.impl.h>

#define INSTANTIATE_PADDEDCHILDREN(SlotType,HolderType) \
	INSTANTIATE_HIDEABLECHILDREN( SlotType, HolderType ) \
	template class IPaddedChildren< SlotType, HolderType >;


namespace wg
{
//	using		iterator = ChildIterator<SlotType>;
//	using		Children<SlotType, HolderType>::m_pSlotArray;
//	using		Children<SlotType, HolderType>::m_pHolder;

	template < class SlotType, class HolderType>
	bool IPaddedChildren<SlotType, HolderType>::setPadding(int index, BorderI padding)
	{
		//TODO: Assert

		m_pHolder->_repadSlots(m_pSlotArray->slot(index), 1, padding);
		return true;
	}

	template < class SlotType, class HolderType>
	bool IPaddedChildren<SlotType, HolderType>::setPadding(iterator it, BorderI padding)
	{
		//TODO: Assert

		m_pHolder->_repadSlots(it._slot(), 1, padding);
		return true;
	}

	template < class SlotType, class HolderType>
	bool IPaddedChildren<SlotType, HolderType>::setPadding(int index, int amount, BorderI padding)
	{
		//TODO: Assert

		m_pHolder->_repadSlots(m_pSlotArray->slot(index), amount, padding);
		return true;
	}

	template < class SlotType, class HolderType>
	bool IPaddedChildren<SlotType, HolderType>::setPadding(iterator beg, iterator end, BorderI padding)
	{
		//TODO: Assert

		auto pBeg = beg._slot();
		auto pEnd = end._slot();
		m_pHolder->_repadSlots(pBeg, int(pEnd-pBeg), padding);
		return true;
	}

	template < class SlotType, class HolderType>
	bool IPaddedChildren<SlotType, HolderType>::setPadding(int index, int amount, const std::initializer_list<BorderI> padding)
	{
		//TODO: Assert

		m_pHolder->_repadSlots(m_pSlotArray->slot(index), amount, padding.begin());
		return true;
	}

	template < class SlotType, class HolderType>
	bool IPaddedChildren<SlotType, HolderType>::setPadding(iterator beg, iterator end, const std::initializer_list<BorderI> padding)
	{
		//TODO: Assert

		auto pBeg = beg._slot();
		auto pEnd = end._slot();
		m_pHolder->_repadSlots(pBeg, int(pEnd - pBeg), padding.begin());
		return true;
	}


} // namespace wg


#endif //WG_IPADDEDCHILDREN_IMPL_DOT_H
