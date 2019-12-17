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

#ifndef WG_SELECTABLESLOT_DOT_H
#define WG_SELECTABLESLOT_DOT_H
#pragma once

#include <wg_slot.h>

namespace wg
{

	//____ SelectableSlot ____________________________________________________________

	class SelectableSlot : public BasicSlot 	/** @private */
	{
		template<class S> friend class CSlotArray;
		template<class S> friend class CSelectableSlotArray;
	public:

		//____ Holder _________________________________________________________

		class Holder : public SlotHolder		/** @private */
		{
		public:
			virtual void	_selectSlots(BasicSlot * pSlot, int nb) = 0;
			virtual void	_unselectSlots(BasicSlot * pSlot, int nb) = 0;
		};

		SelectableSlot(Holder* pHolder) : BasicSlot(pHolder), bVisible(false) {}

		inline void	select() { _holder()->_selectSlots(this, 1); }
		inline void	unselect() { _holder()->_unselectSlots(this, 1); }


	protected:
		inline Holder * _holder() { return static_cast<Holder*>(m_pHolder); }
		inline const Holder * _holder() const { return static_cast<Holder*>(m_pHolder); }

		bool		bVisible;
	};
}
#endif //WG_PADDEDSLOT_DOT_H
