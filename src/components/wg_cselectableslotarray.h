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

#ifndef	WG_CSELECTABLESLOTARRAY_DOT_H
#define	WG_CSELECTABLESLOTARRAY_DOT_H
#pragma once

#include <wg_slot.h>
#include <wg_cdynamicslotarray.h>

namespace wg
{

	//____ CSelectableSlotArray ________________________________________________________

	template<class SlotType> class CSelectableSlotArray : public CDynamicSlotArray<SlotType>
	{
	public:

		class Holder : public CDynamicSlotArray<SlotType>::Holder		/** @private */
		{
		public:
			virtual void	_selectSlots(StaticSlot * pSlot, int nb) = 0;
			virtual void	_unselectSlots(StaticSlot * pSlot, int nb) = 0;
		};


//		using		iterator = SlotArrayIterator<SlotType>;

		/** @private */

		CSelectableSlotArray(Holder * pHolder) : CDynamicSlotArray<SlotType>(pHolder) {}

		//.____ Control _______________________________________________________

		void	select(int index, int amount);
		void	select(const SlotIterator& beg, const SlotIterator& end);

		void	unselect(int index, int amount);
		void	unselect(const SlotIterator& beg, const SlotIterator& end);

		void	selectAll();
		void	unselectAll();

	protected:
		Holder * _holder() { return static_cast<Holder*>(CDynamicSlotArray<SlotType>::_holder()); }
		const Holder * _holder() const { return static_cast<const Holder*>(CDynamicSlotArray<SlotType>::_holder()); }


	};

};

#endif	//WG_CSELECTABLESLOTARRAY_DOT_H
