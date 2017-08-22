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

#ifndef	WG_PADDEDCHILDREN_IMPL_DOT_H
#define	WG_PADDEDCHILDREN_IMPL_DOT_H
#pragma once

#include <wg_hideablechildren.h>
#include <wg_hideablechildren.impl.h>

#define INSTANTIATE_PADDEDCHILDREN(SlotType,HolderType) \
	INSTANTIATE_HIDEABLECHILDREN( SlotType, HolderType ) \
	template class PaddedChildren< SlotType, HolderType >;


namespace wg
{
//	using		iterator = ChildIterator<SlotType>;
//	using		Children<SlotType, HolderType>::m_pSlotArray;
//	using		Children<SlotType, HolderType>::m_pHolder;

	template < class SlotType, class HolderType>
	bool PaddedChildren<SlotType, HolderType>::setPadding(int index, Border padding)
	{
		//TODO: Assert

		m_pHolder->_repadSlots(m_pSlotArray->slot(index), 1, padding);
		return true;
	}

	template < class SlotType, class HolderType>
	bool PaddedChildren<SlotType, HolderType>::setPadding(iterator it, Border padding)
	{
		//TODO: Assert

		m_pHolder->_repadSlots(it._slot(), 1, padding);
		return true;
	}

} // namespace wg


#endif //WG_WG_HIDEABLECHILDREN_IMPL_DOT_H
