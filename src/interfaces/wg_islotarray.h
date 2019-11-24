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

#ifndef	WG_ISLOTARRAY_DOT_H
#define	WG_ISLOTARRAY_DOT_H
#pragma once

#include <wg_idynamicslots.h>
#include <wg_slotarray.h>

namespace wg
{

	//____ SlotArrayHolder _________________________________________________________

	class SlotArrayHolder	/** @private */
	{
	public:
		virtual WidgetHolder* _widgetHolder() = 0;

		virtual Object * _object() = 0;

		virtual void	_didAddSlots(Slot * pSlot, int nb) = 0;
		virtual void	_didMoveSlots(Slot * pFrom, Slot * pTo, int nb) = 0;
		virtual void	_willRemoveSlots(Slot * pSlot, int nb) = 0;

		virtual void	_hideSlots(Slot * pSlot, int nb) = 0;
		virtual void	_unhideSlots(Slot * pSlot, int nb) = 0;
	};


	//____ SlotArrayIterator _______________________________________________________

	template<class SlotType> class SlotArrayIterator : public SlotIterator
	{
	public:

		//.____ Operators ____________________________________________________________

		inline SlotArrayIterator<SlotType>& operator++()
		{
			pSlot = _slot() + 1;
			return *this;
		};

		inline SlotArrayIterator<SlotType> operator++(int)
		{
			SlotArrayIterator<SlotType> tmp = *this;
			pSlot = _slot() + 1;
			return tmp;
		}

		inline SlotArrayIterator<SlotType>& operator--()
		{
			pSlot = _slot() - 1;
			return *this;
		}

		inline SlotArrayIterator<SlotType> operator--(int)
		{
			SlotArrayIterator<SlotType> tmp = *this;
			pSlot = _slot()-1;
			return tmp;
		}

		inline SlotArrayIterator<SlotType> operator+(int amount) const
		{
			SlotArrayIterator<SlotType> it = *this;
			it._inc(amount);
			return it;
		}

		inline SlotArrayIterator<SlotType> operator-(int amount) const
		{
			SlotArrayIterator<SlotType> it = *this;
			it._dec(amount);
			return it;
		}



		//.____ Internal _______________________________________________________________

		SlotArrayIterator(SlotType * pSlot)
		{
			this->pSlot = pSlot;
			this->pHolder = nullptr;
			this->pExtra = nullptr;
		}

		inline SlotType* _slot() const { return static_cast<SlotType*>(pSlot); }

	protected:

		void	_inc() override { pSlot = (static_cast<SlotType*>(pSlot) + 1); }
		void	_dec() override { pSlot = (static_cast<SlotType*>(pSlot) - 1); }

		void	_inc( int amount ) override { pSlot = (static_cast<SlotType*>(pSlot) + amount); }
		void	_dec( int amount ) override { pSlot = (static_cast<SlotType*>(pSlot) - amount); }
	};



	//____ ISlotArray _________________________________________________________

	template<class SlotType> class ISlotArray : public IDynamicSlots
	{
	public:

		using		iterator = SlotArrayIterator<SlotType>;

		//.____ Operators __________________________________________

		inline iterator operator<<(Widget * pWidget) { return add(pWidget); }

		Widget& operator[](int index) const override { return *m_pSlotArray->slot(index)->pWidget; }

		//.____ Content _______________________________________________________

		inline int		size() const override { return m_pSlotArray->size(); }
		inline int		isEmpty() const override { return m_pSlotArray->isEmpty(); }

		inline Widget_p	at(int index) const override
		{
			if (index < 0 || index >= m_pSlotArray->size())
				return nullptr;

			return Widget_p(m_pSlotArray->slot(index)->pWidget);
		}

		inline int		index(Widget * pChild) const override
		{
			if (pChild->_holder() && pChild->_holder()->_childParent() == m_pHolder->_object())
				return m_pSlotArray->index(static_cast<SlotType*>(pChild->_slot()));

			return -1;
		}

		inline int		capacity() const override { return m_pSlotArray->capacity(); }
		inline void		setCapacity(int capacity) override { m_pSlotArray->setCapacity(capacity); }

		iterator		add(Widget * pWidget);
		iterator		add(const Widget_p pWidgets[], int amount);

		template<typename Iterator>
		iterator		add(const Iterator& beg, const Iterator& end)
		{
			static_assert(std::is_convertible<typename std::iterator_traits<Iterator>::iterator_category, std::input_iterator_tag>::value &&
				std::is_convertible<typename std::iterator_traits<Iterator>::value_type, Widget*>::value,
				"Begin and end parameters must be iterators or raw pointers to some kind of Widget pointers.");

			int oldSize = size();

			Iterator it = beg;
			while (it != end)
			{
				add(*it++);
			}

			return begin() + oldSize;
		}

		void			insert(int index, Widget * pWidget) override;
		iterator		insert(const SlotIterator& it, Widget * pWidget);
		void			insert(int index, const Widget_p pWidgets[], int amount) override;
		iterator		insert(const SlotIterator& it, const Widget_p pWidgets[], int amount);

		void			remove(int index) override;
		iterator		remove(const SlotIterator& pos);
		void			remove(int index, int amount) override;
		iterator		remove(const SlotIterator& beg, const SlotIterator& end);

		void			clear() override;

		//.____ Ordering ______________________________________________________

		void			moveToFront(int index) override;
		iterator		moveToFront(const SlotIterator& it);

		void			moveToBack(int index) override;
		iterator		moveToBack(const SlotIterator& it);

		void			moveBefore(int index, int sibling) override;
		iterator		moveBefore(const SlotIterator& it, const SlotIterator& sibling);

		//.____ Appearance ____________________________________________________

		void			hide(int index) override;
		void			hide(const SlotIterator& it) override;
		void			hide(int index, int amount) override;
		void			hide(const SlotIterator& beg, const SlotIterator& end) override;

		void			unhide(int index) override;
		void			unhide(const SlotIterator& it) override;
		void			unhide(int index, int amount) override;
		void			unhide(const SlotIterator& beg, const SlotIterator& end) override;
			
		void			hideAll() override;

		void			unhideAll() override;
		
		inline bool		isVisible(int index) override { return m_pSlotArray->slot(index)->bVisible; }

		inline bool		isVisible(const SlotIterator& it) override { return static_cast<const iterator&>(it)._slot()->bVisible; }


		//.____ Misc _______________________________________________________

		inline iterator	begin() const { return iterator(m_pSlotArray->begin()); }
		inline iterator	end() const { return iterator(m_pSlotArray->end()); }


	protected:
		ISlotArray(SlotArray<SlotType> * pSlotArray, SlotArrayHolder * pHolder) : m_pSlotArray(pSlotArray), m_pHolder(pHolder) {}

		SlotIterator	_begin() const override;
		SlotIterator	_end() const override;

		Object *		_object() const override;

		SlotIterator	_add(Widget * pWidget) override;
		SlotIterator	_add(const Widget_p pWidgets[], int amount) override;

		SlotIterator	_insert(const SlotIterator& it, Widget * pWidget) override;
		SlotIterator	_insert(const SlotIterator& it, const Widget_p pWidgets[], int amount) override;
		SlotIterator	_remove(const SlotIterator& it) override;
		SlotIterator	_remove(const SlotIterator& beg, const SlotIterator& end) override;

		SlotIterator	_moveToFront(const SlotIterator& it) override;
		SlotIterator	_moveToBack(const SlotIterator& it) override;
		SlotIterator	_moveBefore(const SlotIterator& it, const SlotIterator& sibling) override;

		void			_releaseGuardPointer(Widget * pToRelease, SlotType ** pPointerToGuard);

		SlotArray<SlotType> *	m_pSlotArray;
		SlotArrayHolder *		m_pHolder;
	};

} // namespace wg
#endif //WG_ISLOTARRAY_DOT_H
