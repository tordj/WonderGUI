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

	class SelectableSlot : public DynamicSlot
	{
	public:

		/** @private */
		SelectableSlot(SlotHolder* pHolder) : DynamicSlot(pHolder) {}

		//.____ Identification ________________________________________________

		const static TypeInfo	TYPEINFO;

		//.____ State __________________________________________________________

		inline void	select() { _holder()->_selectSlots(this, 1); }
		inline void	unselect() { _holder()->_selectSlots(this, 1); }
		inline void	setSelected(bool bSelected) { if (bSelected) _holder()->_selectSlots(this, 1); else _holder()->_unselectSlots(this, 1); }
		inline bool isSelected() const { return m_bSelected; }

		//.____ Appearance ____________________________________________________

		inline void hide() { _holder()->_hideSlots(this, 1); }
		inline void unhide() {  _holder()->_unhideSlots(this, 1); }
		inline void setVisible(bool bVisible) { if (bVisible) _holder()->_unhideSlots(this, 1); else _holder()->_unhideSlots(this, 1); }
		inline bool isVisible() const { return m_bVisible; }

		//.____ Operators __________________________________________

		inline void operator=(Widget * pWidget) { setWidget(pWidget); }


	protected:
		SelectableSlot(SelectableSlot&& o) = default;
		SelectableSlot& operator=(SelectableSlot&& o) = default;


		inline SlotHolder * _holder() { return static_cast<SlotHolder*>(m_pHolder); }
		inline const SlotHolder * _holder() const { return static_cast<SlotHolder*>(m_pHolder); }

		bool		m_bVisible = false;
		bool		m_bSelected = false;
	};
}
#endif //WG_PADDEDSLOT_DOT_H
