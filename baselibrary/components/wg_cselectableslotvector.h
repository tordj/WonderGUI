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

#ifndef	WG_CSELECTABLESLOTVECTOR_DOT_H
#define	WG_CSELECTABLESLOTVECTOR_DOT_H
#pragma once

#include <wg_slot.h>
#include <wg_cdynamicslotvector.h>

namespace wg
{

	//____ CSelectableSlotVector ________________________________________________________

	template<class SlotType> class CSelectableSlotVector : public CDynamicSlotVector<SlotType>
	{
	public:

//		using		iterator = SlotArrayIterator<SlotType>;

		/** @private */

		CSelectableSlotVector(SlotHolder * pHolder) : CDynamicSlotVector<SlotType>(pHolder) {}

		//.____ Identification _________________________________________________

		const TypeInfo& typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Control _______________________________________________________

		void	select(int index, int amount);
		void	select(const SlotIterator& beg, const SlotIterator& end);

		void	unselect(int index, int amount);
		void	unselect(const SlotIterator& beg, const SlotIterator& end);

		void	selectAll();
		void	unselectAll();

	protected:
		SlotHolder * _holder() { return static_cast<SlotHolder*>(CDynamicSlotVector<SlotType>::_holder()); }
		const SlotHolder * _holder() const { return static_cast<const SlotHolder*>(CDynamicSlotVector<SlotType>::_holder()); }


	};

};

#endif	//WG_CSELECTABLESLOTVECTOR_DOT_H
