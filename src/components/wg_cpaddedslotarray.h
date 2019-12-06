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

#ifndef	WG_CPADDEDSLOTARRAY_DOT_H
#define	WG_CPADDEDSLOTARRAY_DOT_H
#pragma once

#include <wg_cslotarray.h>

namespace wg
{
	//____ CPaddedSlotArray ________________________________________________________

	template<class SlotType> class CPaddedSlotArray : public CSlotArray<SlotType>
	{
	public:

		class Holder : public CSlotArray<SlotType>::Holder  /** @private */
		{
		public:
			virtual void	_repadSlots(BasicSlot * pSlot, int nb, BorderI padding) = 0;
			virtual void	_repadSlots(BasicSlot * pSlot, int nb, const BorderI * pPadding) = 0;
		};


//		using		iterator = SlotArrayIterator<SlotType>;
		using		CSlotArray<SlotType>::m_pSlotArray;
		using		CSlotArray<SlotType>::m_pHolder;

		/** @private */

		CPaddedSlotArray(SlotArray<SlotType> * pSlotArray, Holder * pHolder) : CSlotArray<SlotType>(pSlotArray, pHolder) {}

		//.____ Geometry ______________________________________________________

		bool		setPadding(int index, Border padding);
		bool		setPadding(const SlotIterator& it, Border padding);
		bool		setPadding(int index, int amount, Border padding);
		bool		setPadding(const SlotIterator& beg, const SlotIterator& end, Border padding);
		bool		setPadding(int index, int amount, const std::initializer_list<Border> padding);
		bool		setPadding(const SlotIterator& beg, const SlotIterator& end, const std::initializer_list<Border> padding);

		inline Border	padding(int index) const { return Util::rawToQpix(m_pSlotArray->slot(index)->padding); }
		inline Border	padding(const SlotIterator& it) const { return Util::rawToQpix(static_cast<SlotType*>(it._slot())->padding); }

	protected:
		Holder * _holder() const { return static_cast<Holder*>(m_pHolder); }
	};





} // namespace wg
#endif //WG_CPADDEDSLOTARRAY_DOT_H
