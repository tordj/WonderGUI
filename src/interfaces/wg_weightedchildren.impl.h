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

#ifndef	WG_WEIGHTEDCHILDREN_IMPL_DOT_H
#define	WG_WEIGHTEDCHILDREN_IMPL_DOT_H
#pragma once

#include <wg_paddedchildren.h>
#include <wg_paddedchildren.impl.h>

#define INSTANTIATE_WEIGHTEDCHILDREN(SlotType,HolderType) \
	INSTANTIATE_PADDEDCHILDREN( SlotType, HolderType ) \
	template class WeightedChildren< SlotType, HolderType >;


namespace wg
{
//	using		iterator = ChildIterator<SlotType>;
//	using		Children<SlotType, HolderType>::m_pSlotArray;
//	using		Children<SlotType, HolderType>::m_pHolder;


	template < class SlotType, class HolderType>
	bool WeightedChildren<SlotType, HolderType>::setWeight(int index, float weight)
	{
		if (index < 0 || index >= m_pSlotArray->size() || weight < 0.f)
			return false;

		auto pSlot = m_pSlotArray->slot(index);
		m_pHolder->_reweightSlots(pSlot, 1, weight);
		return true;
	}

	template < class SlotType, class HolderType>
	bool WeightedChildren<SlotType, HolderType>::setWeight(iterator it, float weight)
	{
		//TODO: Add assert

		auto pSlot = it._slot();
		m_pHolder->_reweightSlots(pSlot, 1, weight);
		return true;
	}

	template < class SlotType, class HolderType>
	bool WeightedChildren<SlotType, HolderType>::setWeight(int index, int amount, float weight)
	{
		if (index < 0 || amount <= 0 || index + amount >= m_pSlotArray->size() || weight < 0.f)
			return false;

		auto pSlot = m_pSlotArray->slot(index);
		m_pHolder->_reweightSlots(pSlot, amount, weight);
		return true;
	}

	template < class SlotType, class HolderType>
	bool WeightedChildren<SlotType, HolderType>::setWeight(iterator beg, iterator end, float weight)
	{
		//TODO: Add assert

		auto pBeg = beg._slot();
		auto pEnd = beg._slot();
		m_pHolder->_reweightSlots(pBeg, pEnd - pBeg, weight);
		return true;
	}

	template < class SlotType, class HolderType>
	bool WeightedChildren<SlotType, HolderType>::setWeight(int index, int amount, const std::initializer_list<float> weights)
	{
		if (index < 0 || amount <= 0 || index + amount >= m_pSlotArray->size() || amount > (int) weights.size())
			return false;

		auto pSlot = m_pSlotArray->slot(index);
		m_pHolder->_reweightSlots(pSlot, amount, weights.begin());
		return true;
	}

	template < class SlotType, class HolderType>
	bool WeightedChildren<SlotType, HolderType>::setWeight(iterator beg, iterator end, const std::initializer_list<float> weights)
	{
		//TODO: Add assert

		auto pBeg = beg._slot();
		auto pEnd = beg._slot();
		m_pHolder->_reweightSlots(pBeg, pEnd - pBeg, weights.begin());
		return true;
	}

	template < class SlotType, class HolderType>
	float WeightedChildren<SlotType, HolderType>::weight(int index) const
	{
		if (index < 0 || index >= m_pSlotArray->size())
			return 0.f;

		return m_pSlotArray->slot(index)->weight;

	}

	template < class SlotType, class HolderType>
	float WeightedChildren<SlotType, HolderType>::weight(iterator it) const
	{
		//TODO: Add assert

		return it._slot()->weight;
	}



} // namespace wg


#endif //WG_WG_WEIGHTEDCHILDREN_IMPL_DOT_H
