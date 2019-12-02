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

#include <wg_slotholder.h>
#include <wg_slot.h>
#include <wg_container.h>

namespace wg
{
	//____ _childGlobalPos() _______________________________________________________

	CoordI SlotHolder::_childGlobalPos(BasicSlot * pSlot) const
	{
		auto pParentSlot = ((Container*)this)->_slot();

		return _childPos(pSlot) + pParentSlot->_holder()->_childGlobalPos(pParentSlot);
	}

	//____ _isChildVisible() ________________________________________________________

	bool SlotHolder::_isChildVisible(BasicSlot * pSlot) const
	{
		return true;
	}

	//____ _childWindowSection() ____________________________________________________

	RectI SlotHolder::_childWindowSection(BasicSlot * pSlot) const
	{
		return RectI(0, 0, pSlot->_size());
	}

	//____ _container() ____________________________________________________________

	Container * SlotHolder::_container()
	{
		return static_cast<Container*>(this);
	}

	//____ _root() ___________________________________________________________________

	RootPanel * SlotHolder::_root()
	{
		auto pParentSlot = ((Container*)this)->_slot();

		return pParentSlot ? pParentSlot->_holder()->_root() : nullptr;
	}

	//____ _object() _________________________________________________________________

	Object * SlotHolder::_object()
	{
		return static_cast<Container*>(this);
	}

	//____ _childRequestFocus() ______________________________________________________

	bool SlotHolder::_childRequestFocus(BasicSlot * pSlot, Widget * pWidget)
	{
		auto pParentSlot = ((Container*)this)->_slot();

		return pParentSlot ? pParentSlot->_holder()->_childRequestFocus(pParentSlot, pWidget) : false;
	}

	//____ _ChildReleaseFocus() ______________________________________________________

	bool SlotHolder::_childReleaseFocus(BasicSlot * pSlot, Widget * pWidget)
	{
		auto pParentSlot = ((Container*)this)->_slot();

		return pParentSlot ? pParentSlot->_holder()->_childReleaseFocus(pParentSlot, pWidget) : false;
	}

	//____ _childRequestInView() _____________________________________________________

	void SlotHolder::_childRequestInView(BasicSlot * pSlot)
	{
		auto pParentSlot = ((Container*)this)->_slot();
		if (pParentSlot)
		{
			RectI area(_childPos(pSlot), pSlot->_size());
			pParentSlot->_holder()->_childRequestInView(pParentSlot, area, area);
		}
	}

	void SlotHolder::_childRequestInView(BasicSlot * pSlot, const RectI& mustHaveArea, const RectI& niceToHaveArea)
	{
		auto pParentSlot = ((Container*)this)->_slot();
		if (pParentSlot)
		{
			CoordI pos(_childPos(pSlot));
			pParentSlot->_holder()->_childRequestInView(pParentSlot, mustHaveArea + pos, niceToHaveArea + pos);
		}
	}

} // namespace wg
