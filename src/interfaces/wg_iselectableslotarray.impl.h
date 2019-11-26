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

#ifndef	WG_ISELECTABLESLOTARRAY_IMPL_DOT_H
#define	WG_ISELECTABLESLOTARRAY_IMPL_DOT_H
#pragma once

#include <wg_iselectableslotarray.h>
#include <wg_islotarray.impl.h>

namespace wg
{
	//____ select() ___________________________________________________________

	template <class SlotType>
	void ISelectableSlotArray<SlotType>::select(int index)
	{
		//TODO: Assert

		_holder()->_selectSlots(m_pSlotArray->slot(index), 1);
	}

	template <class SlotType>
	void ISelectableSlotArray<SlotType>::select(const SlotIterator&  it)
	{
		//TODO: Assert

		_holder()->_selectSlots(it._slot(), 1);
	}

	template <class SlotType>
	void ISelectableSlotArray<SlotType>::select(int index, int amount)
	{
		//TODO: Assert

		_holder()->_selectSlots(m_pSlotArray->slot(index), amount);
	};

	template <class SlotType>
	void ISelectableSlotArray<SlotType>::select(const SlotIterator&  beg, const SlotIterator&  end)
	{
		//TODO: Assert

		auto pBeg = static_cast<SlotType*>(beg._slot());
		auto pEnd = static_cast<SlotType*>(end._slot());
		_holder()->_selectSlots(pBeg, pEnd-pBeg);
	};

	//____ selectAll() ________________________________________________________

	template <class SlotType>
	void ISelectableSlotArray<SlotType>::selectAll()
	{
		_holder()->_selectSlots(m_pSlotArray->slot(0), m_pSlotArray->size());
	};

	//____ unselect() _________________________________________________________

	template <class SlotType>
	void ISelectableSlotArray<SlotType>::unselect(int index)
	{
		//TODO: Assert

		_holder()->_unselectSlots(m_pSlotArray->slot(index), 1);
	}

	template <class SlotType>
	void ISelectableSlotArray<SlotType>::unselect(const SlotIterator&  it)
	{
		//TODO: Assert

		_holder()->_unselectSlots(it._slot(), 1);
	}

	template <class SlotType>
	void ISelectableSlotArray<SlotType>::unselect(int index, int amount)
	{
		//TODO: Assert

		_holder()->_unselectSlots(m_pSlotArray->slot(index), amount);
	};

	template <class SlotType>
	void ISelectableSlotArray<SlotType>::unselect(const SlotIterator&  beg, const SlotIterator&  end)
	{
		//TODO: Assert

		auto pBeg = static_cast<SlotType*>(beg._slot());
		auto pEnd = static_cast<SlotType*>(end._slot());
		_holder()->_unselectSlots(pBeg, pEnd-pBeg);
	};

	//____ unselectAll() ______________________________________________________

	template <class SlotType>
	void ISelectableSlotArray<SlotType>::unselectAll()
	{
		_holder()->_unselectSlots(m_pSlotArray->slot(0), m_pSlotArray->size());
	};

	//____ isSelected() ________________________________________________________

	template <class SlotType>
	bool ISelectableSlotArray<SlotType>::isSelected(int index)
	{
		//TODO: Assert

		return m_pSlotArray->slot(index)->_widget()->state().isSelected();
	}

	template <class SlotType>
	bool ISelectableSlotArray<SlotType>::isSelected(const SlotIterator&  it)
	{
		//TODO: Assert

		return it._slot()->_widget()->state().isSelected();
	}

} // namespace wg


#endif //WG_ISELECTABLESLOTARRAY_IMPL_DOT_H
