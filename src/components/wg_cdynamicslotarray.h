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

#ifndef	WG_CDYNAMICSLOTARRAY_DOT_H
#define	WG_CDYNAMICSLOTARRAY_DOT_H
#pragma once

#include <wg_cdynamicslotcollection.h>
#include <wg_cstaticslotarray.h>

namespace wg
{


	//____ CDynamicSlotArray _________________________________________________________

	template<class SlotType> class CDynamicSlotArray : public CDynamicSlotCollection
	{
	public:

		class Holder : public SlotType::Holder	/** @private */
		{
		public:
			virtual void	_didAddSlots(StaticSlot * pSlot, int nb) = 0;
			virtual void	_didMoveSlots(StaticSlot * pFrom, StaticSlot * pTo, int nb) = 0;
			virtual void	_willRemoveSlots(StaticSlot * pSlot, int nb) = 0;

			virtual void	_hideSlots(StaticSlot * pSlot, int nb) = 0;
			virtual void	_unhideSlots(StaticSlot * pSlot, int nb) = 0;

			//		virtual void	_willReplaceWidgets(StaticSlot * pSlot, int nb) = 0;
			//		virtual void	_didReplaceWidgets(StaticSlot * pSlot, int nb) = 0;


		};



		using		iterator = SlotArrayIterator<SlotType>;

		//.____ Operators __________________________________________

		inline iterator operator<<(Widget * pWidget) { return add(pWidget); }

		inline SlotType& operator[](int index) { return m_pArray[index]; }
		inline const SlotType& operator[](int index) const { return m_pArray[index]; }

		//.____ Content _______________________________________________________

		inline int		size() const override { return m_size; }
		inline bool		isEmpty() const override { return (m_size == 0); }

		inline SlotType& at(int index) const
		{
//			if (index < 0 || index >= m_pSlotArray->size())
//				return nullptr;

			return m_pArray[index];
		}

		inline int		index(Widget * pChild) const override
		{
			if (pChild->_holder() && pChild->_holder()->_container() == m_pHolder->_object())
				return _index(static_cast<SlotType*>(pChild->_slot()));

			return -1;
		}

		inline int		capacity() const override { return m_capacity; }
		inline void		setCapacity(int capacity) override { if (capacity != m_capacity) _reallocArray(capacity); }

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
		
		inline bool		isVisible(int index) override { return _slot(index)->bVisible; }

		inline bool		isVisible(const SlotIterator& it) override { return static_cast<const iterator&>(it)._slot()->bVisible; }


		//.____ Misc _______________________________________________________

		inline iterator	begin() const { return iterator(_begin()); }
		inline iterator	end() const { return iterator(_end()); }


	protected:
		CDynamicSlotArray(Holder * pHolder) : m_pHolder(pHolder), m_pArray(nullptr), m_size(0), m_capacity(0) {}
		~CDynamicSlotArray() { _killBlock(_begin(), _end()); free(m_pArray); }

		SlotIterator	_begin_iterator() override;
		SlotIterator	_end_iterator() override;
		StaticSlot&		_at(int index) override;

		Object *		_object() override;
		const Object *	_object() const override;

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


	//////
		inline Holder *		_holder() { return m_pHolder; }
		inline const Holder *	_holder() const { return m_pHolder; }

		SlotType*		_addEmpty() { if (m_size == m_capacity) _reallocArray(((m_capacity + 1) * 2)); _initBlock(_end()); return &m_pArray[m_size++]; }
		SlotType*		_addEmpty(int entries) { if (m_size + entries > m_capacity) _reallocArray(m_capacity + entries); _initBlock(_end(), _end() + entries); int ofs = m_size; m_size += entries; return &m_pArray[ofs]; }

		SlotType*		_insertEmpty(int index) { return _insertBlock(&m_pArray[index], 1); }
		SlotType*		_insertEmpty(int index, int entries) { return _insertBlock(&m_pArray[index], entries); }
		SlotType*		_insertEmpty(SlotType * pPos) { return _insertBlock(pPos, 1); }
		SlotType*		_insertEmpty(SlotType * pPos, int entries) { return _insertBlock(pPos, entries); }

		SlotType*		_remove(int index) { return _deleteBlock(&m_pArray[index], &m_pArray[index + 1]); }
		SlotType*		_remove(int index, int entries) { return _deleteBlock(&m_pArray[index], &m_pArray[index + entries]); }
		SlotType*		_remove(SlotType * pPos) { return _deleteBlock(pPos, pPos + 1); }
		SlotType*		_remove(SlotType * pPos, SlotType * pEnd) { return _deleteBlock(pPos, pEnd); }

		void			_move(int index, int newIndex) { _move(&m_pArray[index], &m_pArray[newIndex]); }
		void			_move(SlotType * pFrom, SlotType * pTo);

		bool			_contains(const SlotType * pSlot) const { return (pSlot >= m_pArray && pSlot <= _last()); }

		void			_clear() { _killBlock(_begin(), _end()); free(m_pArray); m_pArray = 0; m_capacity = 0; m_size = 0; }


		SlotType*	_slot(int index) const { return &m_pArray[index]; }


		SlotType*	_prev(const SlotType* pSlot) const { if (pSlot > m_pArray) return const_cast<SlotType*>(pSlot) - 1; return 0; }
		SlotType*	_next(const SlotType* pSlot) const { if (pSlot < &m_pArray[m_size - 1]) return const_cast<SlotType*>(pSlot) + 1; return 0; }

		int			_index(const SlotType* pSlot) const { return int(pSlot - m_pArray); }

		SlotType *	_find(const Widget* pWidget) const;

		SlotType*	_first() const { return m_pArray; }
		SlotType*	_last() const { return m_pArray + (m_size - 1); }

		SlotType*	_begin() const { return m_pArray; }
		SlotType*	_end() const { return m_pArray + m_size; }

		void		_reorder(int order[]);

	private:

		void	_reallocArray(int capacity);

		void	_reallocBlock(SlotType * pBeg, SlotType * pEnd);

		SlotType* _deleteBlock(SlotType * pBeg, SlotType * pEnd);

		SlotType*	_insertBlock(SlotType * pPos, int entries);

		inline void	_killBlock(SlotType * pBlock)
		{
			pBlock->~SlotType();
		}

		void	_killBlock(SlotType * pBeg, SlotType * pEnd);

		inline void	_initBlock(SlotType * pBlock)
		{
			new (pBlock) SlotType(m_pHolder);
		}

		void	_initBlock(SlotType * pBeg, SlotType * pEnd);

		int			m_capacity;
		int			m_size;
		SlotType *	m_pArray;

		Holder *	m_pHolder;
	};

} // namespace wg
#endif //WG_CDYNAMICSLOTARRAY_DOT_H
