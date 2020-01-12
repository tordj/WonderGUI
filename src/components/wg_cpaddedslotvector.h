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

#ifndef	WG_CPADDEDSLOTVECTOR_DOT_H
#define	WG_CPADDEDSLOTVECTOR_DOT_H
#pragma once

#include <wg_cdynamicslotvector.h>

namespace wg
{
	//____ CPaddedSlotVector ________________________________________________________

	template<class SlotType> class CPaddedSlotVector : public CDynamicSlotVector<SlotType>
	{
	public:

		class Holder : public CDynamicSlotVector<SlotType>::Holder  /** @private */
		{
		public:
			virtual void	_repadSlots(StaticSlot * pSlot, int nb, BorderI padding) = 0;
			virtual void	_repadSlots(StaticSlot * pSlot, int nb, const BorderI * pPadding) = 0;
		};

		/** @private */

		CPaddedSlotVector(Holder * pHolder) : CDynamicSlotVector<SlotType>(pHolder) {}

		//.____ Geometry ______________________________________________________

		bool		setPadding(int index, int amount, Border padding);
		bool		setPadding(const SlotIterator& beg, const SlotIterator& end, Border padding);
		bool		setPadding(int index, int amount, const std::initializer_list<Border> padding);
		bool		setPadding(const SlotIterator& beg, const SlotIterator& end, const std::initializer_list<Border> padding);

	protected:
		Holder *		_holder() { return static_cast<Holder*>(CDynamicSlotVector<SlotType>::_holder()); }
		const Holder *	_holder() const { return static_cast<const Holder*>(CDynamicSlotVector<SlotType>::_holder()); }
	};





} // namespace wg
#endif //WG_CPADDEDSLOTVECTOR_DOT_H
