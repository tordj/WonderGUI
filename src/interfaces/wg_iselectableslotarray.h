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

#ifndef	WG_ISELECTABLESLOTARRAY_DOT_H
#define	WG_ISELECTABLESLOTARRAY_DOT_H
#pragma once

#include <wg_slot.h>
#include <wg_islotarray.h>

namespace wg
{


	//____ SelectableSlotArrayHolder ____________________________________________________________

	class SelectableSlotArrayHolder : public SlotArrayHolder		/** @private */
	{
	public:
		virtual void	_selectSlots(Slot * pSlot, int nb) = 0;
		virtual void	_unselectSlots(Slot * pSlot, int nb) = 0;
	};

	//____ ISelectableSlotArray ________________________________________________________

	template<class SlotType> class ISelectableSlotArray : public ISlotArray<SlotType>
	{
	public:
//		using		iterator = SlotArrayIterator<SlotType>;
		using		ISlotArray<SlotType>::m_pSlotArray;
		using		ISlotArray<SlotType>::m_pHolder;

		/** @private */

		ISelectableSlotArray(SlotArray<SlotType> * pSlotArray, SelectableSlotArrayHolder * pHolder) : ISlotArray<SlotType>(pSlotArray,pHolder) {}

		//.____ Control _______________________________________________________

		void	select(int index);
		void	select(const SlotIterator& it);
		void	select(int index, int amount);
		void	select(const SlotIterator& beg, const SlotIterator& end);

		void	unselect(int index);
		void	unselect(const SlotIterator& it);
		void	unselect(int index, int amount);
		void	unselect(const SlotIterator& beg, const SlotIterator& end);

		void	selectAll();
		void	unselectAll();

		bool	isSelected(int index);
		bool	isSelected(const SlotIterator& it);

	protected:
		SelectableSlotArrayHolder * _holder() const { return static_cast<SelectableSlotArrayHolder*>(m_pHolder); }


	};

};

#endif	//WG_ISELECTABLESLOTARRAY_DOT_H
