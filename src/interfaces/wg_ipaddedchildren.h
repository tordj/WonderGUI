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

#ifndef	WG_IPADDEDCHILDREN_DOT_H
#define	WG_IPADDEDCHILDREN_DOT_H
#pragma once

#include <wg_slot.h>
#include <wg_ihideablechildren.h>
#include <wg_geo.h>

namespace wg
{

	//____ PaddedChildrenHolder ____________________________________________________________

	class PaddedChildrenHolder : public HideableChildrenHolder  /** @private */
	{
	public:
		virtual void	_repadSlots(Slot * pSlot, int nb, Border padding) = 0;
		virtual void	_repadSlots(Slot * pSlot, int nb, const Border * pPadding) = 0;
	};

	//____ IPaddedChildren ________________________________________________________

	template<class SlotType, class HolderType> class IPaddedChildren : public IHideableChildren<SlotType, HolderType>
	{
	public:
		using		iterator = ChildIterator<SlotType>;
		using		IChildren<SlotType, HolderType>::m_pSlotArray;
		using		IChildren<SlotType, HolderType>::m_pHolder;

		/** @private */

		IPaddedChildren(SlotArray<SlotType> * pSlotArray, HolderType * pHolder) : IHideableChildren<SlotType, HolderType>(pSlotArray, pHolder) {}

		//.____ Geometry ______________________________________________________

		bool		setPadding(int index, Border padding);
		bool		setPadding(iterator it, Border padding);
		bool		setPadding(int index, int amount, Border padding);
		bool		setPadding(iterator beg, iterator end, Border padding);
		bool		setPadding(int index, int amount, const std::initializer_list<Border> padding);
		bool		setPadding(iterator beg, iterator end, const std::initializer_list<Border> padding);



		inline Border	padding(int index) const { return m_pSlotArray->slot(index)->padding; }
		inline Border	padding(iterator it) const { return it._slot()->padding; }
	};	
};

#endif	//WG_IPADDEDCHILDREN_DOT_H
