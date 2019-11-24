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

#ifndef	WG_IDYNAMICSLOTS_DOT_H
#define	WG_IDYNAMICSLOTS_DOT_H
#pragma once

#include <wg_islots.h>

namespace wg
{

	//____ IDynamicSlots __________________________________________________________

	class IDynamicSlots;
	typedef	StrongInterfacePtr<IDynamicSlots>	IDynamicSlots_p;
	typedef	WeakInterfacePtr<IDynamicSlots>		IDynamicSlots_wp;

	class IDynamicSlots : public ISlots
	{

	public:

		using		iterator = SlotIterator;

		//.____ Operators __________________________________________

		inline iterator operator<<(Widget * pWidget) { return add(pWidget); }

		//.____ Content _______________________________________________________

		virtual int			capacity() const = 0;
		virtual void		setCapacity(int capacity) = 0;

		inline  iterator	add(Widget * pWidget)												{ return _add(pWidget); }
		inline  iterator	add(const Widget_p pWidgets[], int amount)							{ return _add(pWidgets,amount); }

		template<typename Iterator>
		iterator			add(const Iterator& begin, const Iterator& end)
		{
			static_assert(std::is_convertible<typename std::iterator_traits<Iterator>::iterator_category, std::input_iterator_tag>::value &&
				std::is_convertible<typename std::iterator_traits<Iterator>::value_type, Widget*>::value,
				"Begin and end parameters must be iterators or raw pointers to some kind of Widget pointers.");

			int oldSize = size();

			Iterator it = begin;
			while (it != end)
			{
				add(*it++);
			}

			return _begin() + oldSize;
		}

		virtual void		insert(int index, Widget * pWidget) = 0;
		virtual void		insert(int index, const Widget_p pWidgets[], int amount) = 0;
		inline  iterator	insert(const iterator& it, Widget * pWidget)						{ return _insert(it, pWidget); }
		inline  iterator	insert(const iterator& it, const Widget_p pWidgets[], int amount)	{ return _insert(it, pWidgets, amount); }

		virtual void		remove(int index) = 0;
		virtual void		remove(int index, int amount) = 0;
		inline  iterator	remove(const iterator& it)											{ return _remove(it); }
		inline  iterator	remove(const iterator& beg, const iterator& end)					{ return _remove(beg, end); }

		virtual void		clear() = 0;

		//.____ Ordering ______________________________________________________

		virtual void		moveToFront(int index) = 0;
		inline  iterator	moveToFront(const iterator& it)										{ return _moveToFront(it);	}

		virtual void		moveToBack(int index) = 0;
		inline  iterator	moveToBack(const iterator& it)										{ return _moveToBack(it); }

		virtual void		moveBefore(int index, int sibling) = 0;
		inline  iterator	moveBefore(const iterator& it, const iterator& sibling)				{ return _moveBefore(it,sibling); }

		//.____ Appearance ____________________________________________________

		virtual void		hide(int index) = 0;
		virtual void		hide(int index, int amount) = 0;
		virtual void		hide(const iterator& it) = 0;
		virtual void		hide(const iterator& beg, const iterator& end) = 0;

		virtual void		unhide(int index) = 0;
		virtual void		unhide(int index, int amount) = 0;
		virtual void		unhide(const iterator& it) = 0;
		virtual void		unhide(const iterator& beg, const iterator& end) = 0;

		virtual void		hideAll() = 0;
		virtual void		unhideAll() = 0;

		virtual bool		isVisible(int index) = 0;
		virtual bool		isVisible(const iterator& it) = 0;

		//.____ Misc _______________________________________________________

		inline IDynamicSlots_p ptr() { return IDynamicSlots_p(this); }

	protected:

		virtual iterator	_add(Widget * pWidget) = 0;
		virtual iterator	_add(const Widget_p pWidgets[], int amount) = 0;

		virtual iterator	_insert(const iterator& it, Widget * pWidget) = 0;
		virtual iterator	_insert(const iterator& it, const Widget_p pWidgets[], int amount) = 0;
		virtual iterator	_remove(const iterator& it) = 0;
		virtual iterator	_remove(const iterator& it, const iterator& end) = 0;

		virtual iterator	_moveToFront(const iterator& it) = 0;
		virtual iterator	_moveToBack(const iterator& it) = 0;
		virtual iterator	_moveBefore(const iterator& it, const iterator& sibling) = 0;
	};


} // namespace wg
#endif //WG_IDYNAMICSLOTS_DOT_H
