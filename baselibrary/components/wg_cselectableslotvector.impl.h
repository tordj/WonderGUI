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

#ifndef	WG_CSELECTABLESLOTVECTOR_IMPL_DOT_H
#define	WG_CSELECTABLESLOTVECTOR_IMPL_DOT_H
#pragma once

#include <wg_cselectableslotvector.h>
#include <wg_cdynamicslotvector.impl.h>

namespace wg
{
	template<class SlotType> const TypeInfo CSelectableSlotVector<SlotType>::TYPEINFO = { "CSelectableSlotVector<Unknow>", &CDynamicSlotVector<SlotType>::TYPEINFO };

	//____ typeInfo() _________________________________________________________

	template <class SlotType>
	const TypeInfo& CSelectableSlotVector<SlotType>::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ select() ___________________________________________________________

	template <class SlotType>
	void CSelectableSlotVector<SlotType>::select(int index, int amount)
	{
		//TODO: Assert

		_holder()->_selectSlots(CDynamicSlotVector<SlotType>::_slot(index), amount);
	};

	template <class SlotType>
	void CSelectableSlotVector<SlotType>::select(const SlotIterator&  beg, const SlotIterator&  end)
	{
		//TODO: Assert

		auto pBeg = static_cast<SlotType*>(beg._slot());
		auto pEnd = static_cast<SlotType*>(end._slot());
		_holder()->_selectSlots(pBeg, int(pEnd-pBeg) );
	};

	//____ selectAll() ________________________________________________________

	template <class SlotType>
	void CSelectableSlotVector<SlotType>::selectAll()
	{
		_holder()->_selectSlots(CDynamicSlotVector<SlotType>::_slot(0), CDynamicSlotVector<SlotType>::size());
	};

	//____ unselect() _________________________________________________________

	template <class SlotType>
	void CSelectableSlotVector<SlotType>::unselect(int index, int amount)
	{
		//TODO: Assert

		_holder()->_unselectSlots(CDynamicSlotVector<SlotType>::_slot(index), amount);
	};

	template <class SlotType>
	void CSelectableSlotVector<SlotType>::unselect(const SlotIterator&  beg, const SlotIterator&  end)
	{
		//TODO: Assert

		auto pBeg = static_cast<SlotType*>(beg._slot());
		auto pEnd = static_cast<SlotType*>(end._slot());
		_holder()->_unselectSlots(pBeg, int(pEnd-pBeg) );
	};

	//____ unselectAll() ______________________________________________________

	template <class SlotType>
	void CSelectableSlotVector<SlotType>::unselectAll()
	{
		_holder()->_unselectSlots(CDynamicSlotVector<SlotType>::_begin(), CDynamicSlotVector<SlotType>::size());
	};


} // namespace wg


#endif //WG_CSELECTABLESLOTVECTOR_IMPL_DOT_H
