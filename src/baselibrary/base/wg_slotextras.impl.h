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

#ifndef WG_SLOTEXTRAS_IMPL_DOT_H
#define WG_SLOTEXTRAS_IMPL_DOT_H
#pragma once

#include <wg_slotextras.h>


namespace wg
{
	//____ setPadding() _______________________________________________________

	template<class SlotType, class IteratorType, class HolderType>
	bool PaddedSlotCollectionMethods<SlotType,IteratorType,HolderType>::setPadding(int index, int amount, Border padding)
	{
		//TODO: Assert

		_holder()->_repadSlots(_slot(index), amount, padding);
		return true;
	}

	template<class SlotType, class IteratorType, class HolderType>
	bool PaddedSlotCollectionMethods<SlotType,IteratorType,HolderType>::setPadding(const IteratorType& beg, const IteratorType& end, Border padding)
	{
		//TODO: Assert

		auto pBeg = static_cast<SlotType*>(beg._slot());
		auto pEnd = static_cast<SlotType*>(end._slot());
		_holder()->_repadSlots(pBeg, int(pEnd - pBeg), padding);
		return true;
	}

	template<class SlotType, class IteratorType, class HolderType>
	bool PaddedSlotCollectionMethods<SlotType,IteratorType,HolderType>::setPadding(int index, int amount, std::initializer_list<Border> padding)
	{
		//TODO: Assert

		_holder()->_repadSlots(_slot(index), amount, padding.begin());
		return true;
	}

	template<class SlotType, class IteratorType, class HolderType>
	bool PaddedSlotCollectionMethods<SlotType,IteratorType,HolderType>::setPadding(const IteratorType& beg, const IteratorType& end, std::initializer_list<Border> padding)
	{
		//TODO: Assert

		auto pBeg = static_cast<SlotType*>(beg._slot());
		auto pEnd = static_cast<SlotType*>(end._slot());
		_holder()->_repadSlots(pBeg, int(pEnd - pBeg), padding.begin());
		return true;
	}


	//____ select() ___________________________________________________________

	template<class SlotType, class IteratorType, class HolderType>
	void SelectableSlotCollectionMethods<SlotType,IteratorType,HolderType>::select(int index, int amount)
	{
		//TODO: Error handling!

		_holder()->_selectSlots(_slot(index), amount);
	};

	template<class SlotType, class IteratorType, class HolderType>
	void SelectableSlotCollectionMethods<SlotType,IteratorType,HolderType>::select(const IteratorType& beg, const IteratorType& end)
	{
		//TODO: Error handling!

		auto pBeg = static_cast<SlotType*>(beg._slot());
		auto pEnd = static_cast<SlotType*>(end._slot());
		_holder()->_selectSlots(pBeg, int(pEnd - pBeg));
	};

	//____ selectAll() ________________________________________________________

	template<class SlotType, class IteratorType, class HolderType>
	void SelectableSlotCollectionMethods<SlotType,IteratorType,HolderType>::selectAll()
	{
		_holder()->_selectSlots(_slot(0), _size());
	};

	//____ unselect() _________________________________________________________

	template<class SlotType, class IteratorType, class HolderType>
	void SelectableSlotCollectionMethods<SlotType,IteratorType,HolderType>::unselect(int index, int amount)
	{
		//TODO: Error handling!

		_holder()->_unselectSlots(_slot(index), amount);
	};

	template<class SlotType, class IteratorType, class HolderType>
	void SelectableSlotCollectionMethods<SlotType,IteratorType,HolderType>::unselect(const IteratorType& beg, const IteratorType& end)
	{
		//TODO: Error handling!

		auto pBeg = static_cast<SlotType*>(beg._slot());
		auto pEnd = static_cast<SlotType*>(end._slot());
		_holder()->_unselectSlots(pBeg, int(pEnd - pBeg));
	};

	//____ unselectAll() ______________________________________________________

	template<class SlotType, class IteratorType, class HolderType>
	void SelectableSlotCollectionMethods<SlotType,IteratorType,HolderType>::unselectAll()
	{
		_holder()->_unselectSlots( _slot(0), _size() );
	};


}



#endif //WG_SLOTEXTRAS_IMPL_DOT_H
