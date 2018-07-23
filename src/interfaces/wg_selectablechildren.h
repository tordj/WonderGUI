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

#ifndef	WG_SELECTABLECHILDREN_DOT_H
#define	WG_SELECTABLECHILDREN_DOT_H
#pragma once

#include <wg_slot.h>
#include <wg_hideablechildren.h>

namespace wg
{


	//____ SelectableChildrenHolder ____________________________________________________________

	class SelectableChildrenHolder : public HideableChildrenHolder		/** @private */
	{
	public:
		virtual void	_selectSlots(Slot * pSlot, int nb) = 0;
		virtual void	_unselectSlots(Slot * pSlot, int nb) = 0;
	};

	//____ SelectableChildren ________________________________________________________

	template<class SlotType, class HolderType> class SelectableChildren : public HideableChildren<SlotType, HolderType>
	{
	public:
		using		iterator = ChildIterator<SlotType>;
		using		Children<SlotType, HolderType>::m_pSlotArray;
		using		Children<SlotType, HolderType>::m_pHolder;

		/** @private */

		SelectableChildren(SlotArray<SlotType> * pSlotArray, HolderType * pHolder) : HideableChildren<SlotType, HolderType>(pSlotArray, pHolder) {}

		//.____ Control _______________________________________________________

		void	select(int index);
		void	select(iterator it);
		void	select(int index, int amount);
		void	select(iterator beg, iterator end);

		void	unselect(int index);
		void	unselect(iterator it);
		void	unselect(int index, int amount);
		void	unselect(iterator beg, iterator end);

		void	selectAll();
		void	unselectAll();

		bool	isSelected(int index);
		bool	isSelected(iterator it);

	};

  };

#endif	//WG_SELECTABLECHILDREN_DOT_H
