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

		/** @private */

		CPaddedSlotVector(SlotHolder * pHolder) : CDynamicSlotVector<SlotType>(pHolder) {}

		//.____ Identification _________________________________________________

		const TypeInfo& typeInfo(void) const override { return TYPEINFO; }
		const static TypeInfo	TYPEINFO;

		//.____ Geometry ______________________________________________________

		bool		setPadding(int index, int amount, Border padding);
		bool		setPadding(const SlotIterator& beg, const SlotIterator& end, Border padding);
		bool		setPadding(int index, int amount, std::initializer_list<Border> padding);
		bool		setPadding(const SlotIterator& beg, const SlotIterator& end, std::initializer_list<Border> padding);

	protected:
		SlotHolder *		_holder() { return static_cast<SlotHolder*>(CDynamicSlotVector<SlotType>::_holder()); }
		const SlotHolder *	_holder() const { return static_cast<const SlotHolder*>(CDynamicSlotVector<SlotType>::_holder()); }
	};





} // namespace wg
#endif //WG_CPADDEDSLOTVECTOR_DOT_H
