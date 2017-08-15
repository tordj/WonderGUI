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

#ifndef	WG_DYNAMICCHILDREN_DOT_H
#define	WG_DYNAMICCHILDREN_DOT_H
#pragma once

#include <wg_children.h>

namespace wg 
{

//	template<class SlotType, class HolderType> class Children;
//	typedef	StrongInterfacePtr<Children<class SlotType,class HolderType>,Interface_p>		Children_p;
//	typedef	WeakInterfacePtr<Children<class SlotType, class HolderType>,Interface_wp>		Children_wp;

	//____ DynamicChildrenHolder _________________________________________________________

	class DynamicChildrenHolder	: public ChildrenHolder	/** @private */
	{
	public:
		virtual WidgetHolder* _widgetHolder() = 0;

		virtual void	_didAddSlots( Slot * pSlot, int nb ) = 0;
		virtual void	_didMoveSlots(Slot * pFrom, Slot * pTo, int nb) = 0;
		virtual void	_willRemoveSlots( Slot * pSlot, int nb ) = 0;
	};

	//____ DynamicChildren _________________________________________________________

	template<class SlotType, class HolderType> class DynamicChildren : public Children<SlotType,HolderType>
	{		
	public:

		using		iterator = ChildIterator<SlotType>;
		using		Children<SlotType,HolderType>::m_pSlotArray;
		using		Children<SlotType,HolderType>::m_pHolder;
		
		/** @private */

		DynamicChildren( SlotArray<SlotType> * pSlotArray, HolderType * pHolder ) : Children<SlotType,HolderType>(pSlotArray,pHolder) {}

		//.____ Operators __________________________________________

		inline iterator operator<<(Widget * pWidget) { return add(pWidget); }	

		//.____ Content _______________________________________________________

		iterator add(Widget * pWidget);
		iterator add(const Widget_p pWidgets[], int amount);
		
		iterator insert(int index, Widget * pWidget);
		iterator insert(iterator pos, Widget * pWidget);
		iterator insert(int index, const Widget_p pWidgets[], int amount);
		iterator insert(iterator pos, const Widget_p pWidgets[], int amount);

		iterator remove(int index);
		iterator remove(iterator pos);
		iterator remove(int index, int amount);
		iterator remove(iterator beg, iterator end);

		void clear();

		//.____ Ordering ______________________________________________________

		void		moveToFront(int index);
		iterator	moveToFront(iterator it);

		void		moveToBack(int index);
		iterator	moveToBack(iterator it);

		void		moveBefore(int index, int sibling);
		iterator	moveBefore(iterator it, iterator sibling);

	};
	

} // namespace wg
#endif //WG_CHILDREN_DOT_H
