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

#ifndef	WG_IPADDEDSLOTVECTOR_IMPL_DOT_H
#define	WG_IPADDEDSLOTVECTOR_IMPL_DOT_H
#pragma once

#include <wg_cpaddedslotvector.h>
#include <wg_cdynamicslotvector.impl.h>
#include <wg_util.h>


namespace wg
{
//	template class CSlotVector< SlotType >;

	template<class SlotType> const TypeInfo CPaddedSlotVector<SlotType>::TYPEINFO = { "CPaddedSlotVector<Unknow>", &CDynamicSlotVector<SlotType>::TYPEINFO };

	//____ setPadding() _______________________________________________________

	template<class SlotType>
	bool CPaddedSlotVector<SlotType>::setPadding(int index, int amount, Border padding)
	{
		//TODO: Assert

		_holder()->_repadSlots(CDynamicSlotVector<SlotType>::_slot(index), amount, padding);
		return true;
	}

	template<class SlotType>
	bool CPaddedSlotVector<SlotType>::setPadding(const SlotIterator& beg, const SlotIterator& end, Border padding)
	{
		//TODO: Assert

		auto pBeg = static_cast<SlotType*>(beg._slot());
		auto pEnd = static_cast<SlotType*>(end._slot());
		_holder()->_repadSlots(pBeg, int(pEnd - pBeg), padding);
		return true;
	}

	template<class SlotType>
	bool CPaddedSlotVector<SlotType>::setPadding(int index, int amount, const std::initializer_list<Border> padding)
	{
		//TODO: Assert

		_holder()->_repadSlots(CDynamicSlotVector<SlotType>::_slot(index), amount, padding.begin());
		return true;
	}

	template<class SlotType>
	bool CPaddedSlotVector<SlotType>::setPadding(const SlotIterator& beg, const SlotIterator& end, const std::initializer_list<Border> padding)
	{
		//TODO: Assert

		auto pBeg = static_cast<SlotType*>(beg._slot());
		auto pEnd = static_cast<SlotType*>(end._slot());
		_holder()->_repadSlots(pBeg, int(pEnd - pBeg), padding.begin());
		return true;
	}

} // namespace wg


#endif //WG_CPADDEDSLOTVECTOR_IMPL_DOT_H
