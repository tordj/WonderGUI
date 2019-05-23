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

#ifndef WG_CHILDITERATOR_DOT_H
#define WG_CHILDITERATOR_DOT_H
#pragma once

#include <wg_slot.h>

namespace wg
{
	class Object;
	class Widget;

	//____ ChildrenHolder ___________________________________________________

	class ChildrenHolder /** @private */
	{
	public:
		virtual Object * _object() = 0;
		virtual Slot *	_incSlot(Slot * pSlot) const = 0;
		virtual Slot *	_decSlot(Slot * pSlot) const = 0;
	};


	//____ ChildIterator _______________________________________________________

	class ChildIterator
	{
	public:

		// Iterator traits, previously from std::iterator.
		using value_type = Slot;
		using difference_type = std::ptrdiff_t;
		using pointer = Widget * ;
		using reference = Widget & ;
		using iterator_category = std::bidirectional_iterator_tag;

		//.____ Creation ___________________________________________________

		ChildIterator() = default;
		explicit ChildIterator(Slot* pSlot,ChildrenHolder* pHolder)
		{
			this->pSlot = pSlot;
			this->pHolder = pHolder;
		}

		//.____ Operators ______________________________________________________

		inline reference operator*() const
		{
			return *pSlot->pWidget;
		}

		inline pointer operator->() const
		{
			return pSlot->pWidget;
		}


		inline ChildIterator& operator++()
		{
			pSlot = pHolder->_incSlot(pSlot);
			return *this;
		}

		inline ChildIterator operator++(int)
		{
			ChildIterator tmp = *this;
			pSlot = pHolder->_incSlot(pSlot);
			return tmp;
		}

		inline ChildIterator& operator--()
		{
			pSlot = pHolder->_decSlot(pSlot);
			return *this;
		}

		inline ChildIterator operator--(int)
		{
			ChildIterator tmp = *this;
			pSlot = pHolder->_decSlot(pSlot);
			return tmp;
		}

		inline bool operator==(const ChildIterator& rhs)
		{
			return pSlot == rhs.pSlot;
		}

		inline bool operator!=(const ChildIterator& rhs)
		{
			return pSlot != rhs.pSlot;
		}

		inline bool operator< (const ChildIterator& rhs) { return pSlot < rhs.pSlot; }
		inline bool operator> (const ChildIterator& rhs) { return pSlot > rhs.pSlot; }
		inline bool operator<=(const ChildIterator& rhs) { return pSlot <= rhs.pSlot; }
		inline bool operator>=(const ChildIterator& rhs) { return pSlot >= rhs.pSlot; }


		//.____ Internal _______________________________________________________

		Slot * _slot() const
		{
			return pSlot;
		}

	protected:

		Slot *				pSlot;
		ChildrenHolder *	pHolder;
	};



	//____ ChildIteratorSubclass _______________________________________________________

	template<class SlotType> class ChildIteratorSubclass : public ChildIterator
	{
	public:

		// Iterator traits, previously from std::iterator.
		using value_type = SlotType;
		using difference_type = std::ptrdiff_t;
		using pointer = Widget*;
		using reference = Widget&;
		using iterator_category = std::bidirectional_iterator_tag;

		//.____ Creation ___________________________________________________

		ChildIteratorSubclass() = default;
		explicit ChildIteratorSubclass(SlotType* pSlot, ChildrenHolder* pHolder ) : ChildIterator(pSlot,pHolder)
		{
		}

		//.____ Operators ______________________________________________________

		inline ChildIterator& operator++()
		{
			pSlot = (static_cast<SlotType*>(pSlot) + 1);
			return *this;
		}

		inline ChildIterator operator++(int)
		{
			ChildIterator tmp = *this;
			pSlot = (static_cast<SlotType*>(pSlot) + 1);
			return tmp;
		}

		inline ChildIterator& operator--()
		{
			pSlot = (static_cast<SlotType*>(pSlot) - 1);
			return *this;
		}

		inline ChildIterator operator--(int)
		{
			ChildIterator tmp = *this;
			pSlot = (static_cast<SlotType*>(pSlot) - 1);
			return tmp;
		}

		//.____ Internal _______________________________________________________

		SlotType * _slot() const
		{
			return (SlotType*) pSlot;
		}

	private:

	};


} // namespace wg
#endif //WG_CHILDITERATOR_DOT_H
