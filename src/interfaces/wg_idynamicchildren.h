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

#ifndef	WG_IDYNAMICCHILDREN_DOT_H
#define	WG_IDYNAMICCHILDREN_DOT_H
#pragma once

#include <wg_ichildren.h>

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

	//____ IDynamicChildren _________________________________________________________

	template<class SlotType, class HolderType> class IDynamicChildren : public IChildrenSubclass<SlotType,HolderType>
	{
	public:

		using		iterator = ChildIteratorSubclass<SlotType>;
		using		IChildrenSubclass<SlotType,HolderType>::m_pSlotArray;
		using		IChildrenSubclass<SlotType,HolderType>::m_pHolder;

		/** @private */

		IDynamicChildren( SlotArray<SlotType> * pSlotArray, HolderType * pHolder ) : IChildrenSubclass<SlotType,HolderType>(pSlotArray,pHolder) {}

		//.____ Operators __________________________________________

		inline iterator operator<<(Widget * pWidget) { return add(pWidget); }

		//.____ Content _______________________________________________________

		inline int		capacity() const { return m_pSlotArray->capacity(); }
		inline void		setCapacity(int capacity) { m_pSlotArray->setCapacity(capacity); }

		iterator add(Widget * pWidget);
		iterator add(const Widget_p pWidgets[], int amount);

		template<typename Iterator>
		iterator add(const Iterator& begin, const Iterator& end)
		{
//			static_assert(std::is_same<typename std::iterator_traits<Iterator>::value_type, SomeType>::value, "");

//			static_assert(sizeof(std::iterator_traits<Iterator>) > 1, "Not an iterator.");
//			static_assert(sizeof(std::iterator_traits<Iterator>::value_type) == 1, "Not an iterator.");
//			static_assert(std::is_object<std::iterator_traits<Iterator>>, "Not an iterator.");

			static_assert(std::is_convertible<typename std::iterator_traits<Iterator>::iterator_category, std::input_iterator_tag>::value &&
				std::is_convertible<typename std::iterator_traits<Iterator>::value_type, Widget*>::value,
				"Begin and end parameters must be iterators or raw pointers to some kind of Widget pointers.");


/*
			static_assert(std::is_same<std::random_access_iterator_tag, typename std::iterator_traits<Iterator>::iterator_category>::value &&
				std::is_convertible<typename std::iterator_traits<Iterator>::value_type, Widget*>::value,
//				std::is_convertible<decltype(*begin),Widget* >::value,
				"Begin and end parameters must be iterators or raw pointers to Widget shared pointers (Widget_p).");
*/
			Iterator it = begin;
			while (it != end)
			{
				add(*it++);
			}

			return iterator(nullptr, m_pHolder);		//TODO: Correct return value!!! This one is tricky...
		}


/*		template<typename Container >
		iterator add( typename Container::iterator begin, typename Container::iterator end)
		{
			return nullptr;
		};
*/

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
#endif //WG_IDYNAMICCHILDREN_DOT_H
