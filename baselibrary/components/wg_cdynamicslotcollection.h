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

#ifndef	WG_CDYNAMICSLOTCOLLECTION_DOT_H
#define	WG_CDYNAMICSLOTCOLLECTION_DOT_H
#pragma once

#include <wg_cstaticslotcollection.h>

namespace wg
{

	//____ CDynamicSlotCollection __________________________________________________________

	class CDynamicSlotCollection;
	typedef	StrongComponentPtr<CDynamicSlotCollection>	CDynamicSlotCollection_p;
	typedef	WeakComponentPtr<CDynamicSlotCollection>	CDynamicSlotCollection_wp;

	class CDynamicSlotCollection : public CStaticSlotCollection
	{

	public:

		using		iterator = SlotIterator;

		//.____ Identification _________________________________________________

		const TypeInfo& typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Content _______________________________________________________

		inline  iterator	pushFront(const Widget_p& pWidget) { return _pushFront(pWidget); }
		inline  iterator	pushFront(const Widget_p pWidgets[], int amount) { return _pushFront(pWidgets, amount); }

		template<typename Iterator>
		iterator			pushFront(const Iterator& begin, const Iterator& end)
		{
			static_assert(std::is_convertible<typename std::iterator_traits<Iterator>::iterator_category, std::input_iterator_tag>::value &&
				std::is_convertible<typename std::iterator_traits<Iterator>::value_type, Widget*>::value,
				"Begin and end parameters must be iterators or raw pointers to some kind of Widget pointers.");

			if (begin != end)
			{
				Iterator it = end;
				--it;
				while (it != begin)
				{
					pushFront(*it);
					--it;
				}
			}

			return _begin_iterator();
		}


		inline  iterator	pushBack(const Widget_p& pWidget)												{ return _pushBack(pWidget); }
		inline  iterator	pushBack(const Widget_p pWidgets[], int amount)							{ return _pushBack(pWidgets,amount); }

		template<typename Iterator>
		iterator			pushBack(const Iterator& begin, const Iterator& end)
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

			return _begin_iterator() + oldSize;
		}

		virtual void		insert(int index, const Widget_p& pWidget) = 0;
		virtual void		insert(int index, const Widget_p pWidgets[], int amount) = 0;
		inline  iterator	insert(const iterator& it, const Widget_p& pWidget)						{ return _insert(it, pWidget); }
		inline  iterator	insert(const iterator& it, const Widget_p pWidgets[], int amount)	{ return _insert(it, pWidgets, amount); }

		virtual void		erase(int index) = 0;
		virtual void		erase(int index, int amount) = 0;
		inline  iterator	erase(const iterator& it)											{ return _erase(it); }
		inline  iterator	erase(const iterator& beg, const iterator& end)					{ return _erase(beg, end); }

		virtual void		clear() = 0;

		//.____ Ordering ______________________________________________________

		virtual void		moveToFront(int index) = 0;
		inline  iterator	moveToFront(const iterator& it)										{ return _moveToFront(it);	}

		virtual void		moveToBack(int index) = 0;
		inline  iterator	moveToBack(const iterator& it)										{ return _moveToBack(it); }

		virtual void		moveBefore(int index, int sibling) = 0;
		inline  iterator	moveBefore(const iterator& it, const iterator& sibling)				{ return _moveBefore(it,sibling); }

		//.____ Appearance ____________________________________________________

		virtual void		hide(int index, int amount) = 0;
		virtual void		hide(const iterator& beg, const iterator& end) = 0;

		virtual void		unhide(int index, int amount) = 0;
		virtual void		unhide(const iterator& beg, const iterator& end) = 0;

		virtual void		hideAll() = 0;
		virtual void		unhideAll() = 0;

		//.____ Misc _______________________________________________________

		inline CDynamicSlotCollection_p ptr() { return CDynamicSlotCollection_p(this); }

		//.____ Operators __________________________________________

		inline iterator operator<<(const Widget_p& pWidget) { return _pushBack(pWidget); }

	protected:
		virtual iterator	_pushFront(Widget * pWidget) = 0;
		virtual iterator	_pushFront(const Widget_p pWidgets[], int amount) = 0;

		virtual iterator	_pushBack(Widget * pWidget) = 0;
		virtual iterator	_pushBack(const Widget_p pWidgets[], int amount) = 0;

		virtual iterator	_insert(const iterator& it, Widget * pWidget) = 0;
		virtual iterator	_insert(const iterator& it, const Widget_p pWidgets[], int amount) = 0;
		virtual iterator	_erase(const iterator& it) = 0;
		virtual iterator	_erase(const iterator& it, const iterator& end) = 0;

		virtual iterator	_moveToFront(const iterator& it) = 0;
		virtual iterator	_moveToBack(const iterator& it) = 0;
		virtual iterator	_moveBefore(const iterator& it, const iterator& sibling) = 0;
	};


} // namespace wg
#endif //WG_CDYNAMICSLOTCOLLECTION_DOT_H
