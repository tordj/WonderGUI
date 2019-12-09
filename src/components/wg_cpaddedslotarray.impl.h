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

#ifndef	WG_IPADDEDSLOTARRAY_IMPL_DOT_H
#define	WG_IPADDEDSLOTARRAY_IMPL_DOT_H
#pragma once

#include <wg_cpaddedslotarray.h>
#include <wg_cslotarray.impl.h>
#include <wg_util.h>


namespace wg
{
	template class CSlotArray< SlotType >;

	//____ setPadding() _______________________________________________________

	template<class SlotType>
	bool CPaddedSlotArray<SlotType>::setPadding(int index, Border padding)
	{
		//TODO: Assert

		_holder()->_repadSlots(_slot(index), 1, Util::qpixToRaw(padding));
		return true;
	}

	template<class SlotType>
	bool CPaddedSlotArray<SlotType>::setPadding(const SlotIterator& it, Border padding)
	{
		//TODO: Assert

		_holder()->_repadSlots(it._slot(), 1, Util::qpixToRaw(padding));
		return true;
	}

	template<class SlotType>
	bool CPaddedSlotArray<SlotType>::setPadding(int index, int amount, Border padding)
	{
		//TODO: Assert

		_holder()->_repadSlots(_slot(index), amount, Util::qpixToRaw(padding));
		return true;
	}

	template<class SlotType>
	bool CPaddedSlotArray<SlotType>::setPadding(const SlotIterator& beg, const SlotIterator& end, Border padding)
	{
		//TODO: Assert

		auto pBeg = static_cast<SlotType*>(beg._slot());
		auto pEnd = static_cast<SlotType*>(end._slot());
		_holder()->_repadSlots(pBeg, int(pEnd - pBeg), Util::qpixToRaw(padding));
		return true;
	}

	template<class SlotType>
	bool CPaddedSlotArray<SlotType>::setPadding(int index, int amount, const std::initializer_list<Border> padding)
	{
		//TODO: Assert

		_holder()->_repadSlots(_slot(index), amount, reinterpret_cast<const BorderI *>(padding.begin()));
		return true;
	}

	template<class SlotType>
	bool CPaddedSlotArray<SlotType>::setPadding(const SlotIterator& beg, const SlotIterator& end, const std::initializer_list<Border> padding)
	{
		//TODO: Assert

		auto pBeg = static_cast<SlotType*>(beg._slot());
		auto pEnd = static_cast<SlotType*>(end._slot());
		_holder()->_repadSlots(pBeg, int(pEnd - pBeg), reinterpret_cast<const BorderI *>(padding.begin()));
		return true;
	}

} // namespace wg


#endif //WG_IPADDEDSLOTARRAY_IMPL_DOT_H
