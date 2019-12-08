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

#ifndef	WG_CSLOTARRAY_IMPL_DOT_H
#define	WG_CSLOTARRAY_IMPL_DOT_H
#pragma once

#include <wg_cslotarray.h>

//#define INSTANTIATE_CHILDREN(SlotType)		template class CSlotArray< SlotType >;

namespace wg
{
	class Widget;


	//____ add() _________________________________________________________________

	template < class SlotType>
	SlotArrayIterator<SlotType> CSlotArray<SlotType>::add(Widget * pWidget)
	{
		pWidget->releaseFromParent();								// Always release first, in case widget already was in our array.

		SlotType * pSlot = _addEmpty();
		pSlot->_setWidget(pWidget);
		m_pHolder->_didAddSlots(pSlot, 1);
		return iterator(pSlot);
	}

	template < class SlotType>
	SlotArrayIterator<SlotType> CSlotArray<SlotType>::add(const Widget_p pWidgets[], int amount)
	{
		//TODO: Add assert

		for (int i = 0; i < amount; i++)
			pWidgets[i]->releaseFromParent();

		SlotType * pSlot = _addEmpty(amount);

		for (int i = 0; i < amount; i++)
			pSlot[i]._setWidget(pWidgets[i]);
		m_pHolder->_didAddSlots(pSlot, amount);
		return iterator(pSlot);
	}

	//____ insert() ______________________________________________________________

	template < class SlotType>
	void CSlotArray<SlotType>::insert(int index, Widget * pWidget)
	{
		//TODO: Add assert

		SlotType * pSlot = _insertEmpty(index);

		this->_releaseGuardPointer(pWidget, &pSlot);
		pSlot->_setWidget(pWidget);
		m_pHolder->_didAddSlots(pSlot, 1);
	}

	template < class SlotType>
	SlotArrayIterator<SlotType> CSlotArray<SlotType>::insert(const SlotIterator& pos, Widget * pWidget)
	{
		//TODO: Add assert

		SlotType * pSlot = static_cast<SlotType*>(pos._slot());
		pSlot = _insertEmpty(pSlot);

		this->_releaseGuardPointer(pWidget, &pSlot);
		pSlot->_setWidget(pWidget);
		m_pHolder->_didAddSlots(pSlot, 1);
		return iterator(pSlot);
	}

	template < class SlotType>
	void CSlotArray<SlotType>::insert(int index, const Widget_p pWidgets[], int amount)
	{
		//TODO: Add assert

		SlotType * pSlot = _insertEmpty(index, amount);

		for (int i = 0; i < amount; i++)
		{
			this->_releaseGuardPointer(pWidgets[i], &pSlot);
			pSlot[i]._setWidget(pWidgets[i]);
		}
		m_pHolder->_didAddSlots(pSlot, amount);
	}

	template < class SlotType>
	SlotArrayIterator<SlotType> CSlotArray<SlotType>::insert(const SlotIterator& pos, const Widget_p pWidgets[], int amount)
	{
		//TODO: Add assert

		SlotType * pSlot = static_cast<SlotType*>(pos._slot());
		pSlot = _insertEmpty(pSlot, amount);

		for (int i = 0; i < amount; i++)
		{
			this->_releaseGuardPointer(pWidgets[i], &pSlot);
			pSlot[i]._setWidget(pWidgets[i]);
		}
		m_pHolder->_didAddSlots(pSlot, amount);
		return iterator(pSlot);
	}

	//____ remove() ______________________________________________________________

	template < class SlotType>
	void CSlotArray<SlotType>::remove(int index)
	{
		//TODO: Add assert

		SlotType * pSlot = _slot(index);
		m_pHolder->_willRemoveSlots(pSlot, 1);
	}

	template < class SlotType>
	SlotArrayIterator<SlotType> CSlotArray<SlotType>::remove(const SlotIterator& pos)
	{
		//TODO: Add assert

		SlotType * pSlot = static_cast<SlotType*>(pos._slot());
		m_pHolder->_willRemoveSlots(pSlot, 1);
		return iterator(_remove(pSlot));
	}

	template < class SlotType>
	void CSlotArray<SlotType>::remove(int index, int amount)
	{
		//TODO: Add assert

		SlotType * pSlot = _slot(index);
		m_pHolder->_willRemoveSlots(pSlot, amount);
	}

	template < class SlotType>
	SlotArrayIterator<SlotType> CSlotArray<SlotType>::remove(const SlotIterator& beg, const SlotIterator& end)
	{
		//TODO: Add assert

		SlotType * pBeg = static_cast<SlotType*>(beg._slot());
		SlotType * pEnd = static_cast<SlotType*>(end._slot());

		m_pHolder->_willRemoveSlots(pBeg, pEnd-pBeg);
		return iterator(_remove(pBeg, pEnd));
	}

	//____ clear() _______________________________________________________________

	template < class SlotType>
	void CSlotArray<SlotType>::clear()
	{
		if (isEmpty())
			return;
		m_pHolder->_willRemoveSlots(_begin(), size());
		_clear();
	}

	//____ moveToBack() __________________________________________________________

	template < class SlotType>
	void CSlotArray<SlotType>::moveToBack(int index)
	{
		//TODO: Assert

		SlotType * pFrom = _slot(index);
		SlotType * pTo = _end();

		_move(pFrom, pTo);
		m_pHolder->_didMoveSlots(pFrom, pTo, 1);
	}

	template < class SlotType>
	SlotArrayIterator<SlotType> CSlotArray<SlotType>::moveToBack(const SlotIterator& it)
	{
		//TODO: Assert

		SlotType * pFrom = static_cast<SlotType*>(it._slot());
		SlotType * pTo = _end();

		_move(pFrom, pTo);
		m_pHolder->_didMoveSlots(pFrom, pTo, 1);
		return iterator(_last());
	}

	//____ moveToFront() _________________________________________________________

	template < class SlotType>
	void CSlotArray<SlotType>::moveToFront(int index)
	{
		//TODO: Assert

		SlotType * pFrom = _slot(index);
		SlotType * pTo = _begin();

		_move(pFrom, pTo);
		m_pHolder->_didMoveSlots(pFrom, pTo, 1);
	}

	template < class SlotType>
	SlotArrayIterator<SlotType> CSlotArray<SlotType>::moveToFront(const SlotIterator& it)
	{
		//TODO: Assert

		SlotType * pFrom = static_cast<SlotType*>(it._slot());
		SlotType * pTo = _begin();

		_move(pFrom, pTo);
		m_pHolder->_didMoveSlots(pFrom, pTo, 1);
		return iterator(_begin());
	}

	//____ moveBefore() __________________________________________________________

	template < class SlotType>
	void CSlotArray<SlotType>::moveBefore(int index, int sibling)
	{
		//TODO: Assert

		SlotType * pFrom = _slot(index);
		SlotType * pTo = _slot(sibling);

		if (pFrom < pTo)
			pTo--;

		if (pFrom != pTo)
		{
			_move(pFrom, pTo);
			m_pHolder->_didMoveSlots(pFrom, pTo, 1);
		}
	}

	template < class SlotType>
	SlotArrayIterator<SlotType> CSlotArray<SlotType>::moveBefore(const SlotIterator& it, const SlotIterator& sibling)
	{
		//TODO: Assert

		SlotType * pFrom = static_cast<SlotType*>(it._slot());
		SlotType * pTo = static_cast<SlotType*>(sibling._slot());

		if (pFrom < pTo)
			pTo--;

		if (pFrom != pTo)
		{
			_move(pFrom, pTo);
			m_pHolder->_didMoveSlots(pFrom, pTo, 1);
		}

		return iterator(pTo);
	}

	//____ hide() ________________________________________________________________

	template < class SlotType>
	void CSlotArray<SlotType>::hide(int index)
	{
		//TODO: Assert

		m_pHolder->_hideSlots(_slot(index), 1);
	};

	template < class SlotType>
	void CSlotArray<SlotType>::hide(const SlotIterator& it)
	{
		//TODO: Assert

		SlotType * pSlot = static_cast<SlotType*>(it._slot());
		m_pHolder->_hideSlots(pSlot, 1);
	};


	template < class SlotType>
	void CSlotArray<SlotType>::hide(int index, int amount)
	{
		//TODO: Assert

		m_pHolder->_hideSlots(_slot(index), amount);
	};

	template < class SlotType>
	void CSlotArray<SlotType>::hide(const SlotIterator& beg, const SlotIterator& end)
	{
		//TODO: Assert

		SlotType * pBeg = static_cast<SlotType*>(beg._slot());
		SlotType * pEnd = static_cast<SlotType*>(end._slot());
		m_pHolder->_hideSlots(pBeg, pEnd-pBeg);
	};

	//____ hideAll() _____________________________________________________________

	template < class SlotType>
	void CSlotArray<SlotType>::hideAll()
	{
		m_pHolder->_hideSlots(_begin(), size());
	};

	//____ unhide() ______________________________________________________________

	template < class SlotType>
	void CSlotArray<SlotType>::unhide(int index)
	{
		//TODO: Assert

		m_pHolder->_unhideSlots(_slot(index), 1);
	};

	template < class SlotType>
	void CSlotArray<SlotType>::unhide(const SlotIterator& it)
	{
		//TODO: Assert

		SlotType * pSlot = static_cast<SlotType*>(it._slot());
		m_pHolder->_unhideSlots(pSlot, 1);
	};

	template < class SlotType>
	void CSlotArray<SlotType>::unhide(int index, int amount)
	{
		//TODO: Assert

		m_pHolder->_unhideSlots(_slot(index), amount);
	};

	template < class SlotType>
	void CSlotArray<SlotType>::unhide(const SlotIterator& beg, const SlotIterator& end)
	{
		//TODO: Assert

		SlotType * pBeg = static_cast<SlotType*>(beg._slot());
		SlotType * pEnd = static_cast<SlotType*>(end._slot());
		m_pHolder->_unhideSlots(pBeg, pEnd-pBeg);
	};

	//____ unhideAll() ________________________________________________________

	template < class SlotType>
	void CSlotArray<SlotType>::unhideAll()
	{
		m_pHolder->_unhideSlots(_begin(), size());
	};

	//____ _begin_iterator() ___________________________________________________________

	template < class SlotType>
	SlotIterator CSlotArray<SlotType>::_begin_iterator() const 
	{ 
		return iterator(_begin()); 
	}

	//____ _end_iterator() _____________________________________________________________

	template < class SlotType>
	SlotIterator CSlotArray<SlotType>::_end_iterator() const 
	{ 
		return iterator(_end()); 
	}

	//____ _at() _____________________________________________________________

	template < class SlotType>
	BasicSlot& CSlotArray<SlotType>::_at(int index) const
	{
		return *_slot(index);
	}

	//____ _object() __________________________________________________________

	template < class SlotType>
	Object * CSlotArray<SlotType>::_object()
	{
		return m_pHolder->_object();
	}

	template < class SlotType>
	const Object * CSlotArray<SlotType>::_object() const
	{
		return m_pHolder->_object();
	}


	//____ _releaseGuardPointer() _____________________________________________

	template < class SlotType>
	void CSlotArray<SlotType>::_releaseGuardPointer(Widget * pToRelease, SlotType ** pPointerToGuard)
	{
		Container * pParent = pToRelease->_parent();

		if (pParent)
		{
			BasicSlot * pReleaseFromSlot = pToRelease->_slot();

			if (_contains(static_cast<SlotType*>(pReleaseFromSlot)))
			{
				// We are releasing a widget from our own slot array, so we need to make sure pointer still is correct afterwards.

				int ofs = (int)((*pPointerToGuard) - _first());
				if (*pPointerToGuard > pReleaseFromSlot)
					ofs--;

				pToRelease->releaseFromParent();
				*pPointerToGuard = _first() + ofs;
			}
			else
				pToRelease->releaseFromParent();
		}
	}




	template < class SlotType>
	SlotIterator CSlotArray<SlotType>::_add(Widget * pWidget)
	{
		pWidget->releaseFromParent();								// Always release first, in case widget already was in our array.

		SlotType * pSlot = _addEmpty();
		pSlot->_setWidget(pWidget);
		m_pHolder->_didAddSlots(pSlot, 1);
		return iterator(pSlot);
	}

	template < class SlotType>
	SlotIterator CSlotArray<SlotType>::_add(const Widget_p pWidgets[], int amount)
	{
		//TODO: Add assert

		for (int i = 0; i < amount; i++)
			pWidgets[i]->releaseFromParent();

		SlotType * pSlot = _addEmpty(amount);

		for (int i = 0; i < amount; i++)
			pSlot[i]._setWidget(pWidgets[i]);
		m_pHolder->_didAddSlots(pSlot, amount);
		return iterator(pSlot);
	}

	template < class SlotType>
	SlotIterator CSlotArray<SlotType>::_insert(const SlotIterator& it, Widget * pWidget)
	{
		//TODO: Add assert

		SlotType * pSlot = static_cast<SlotType*>(it._slot());
		pSlot = _insertEmpty(pSlot);

		this->_releaseGuardPointer(pWidget, &pSlot);
		pSlot->_setWidget(pWidget);
		m_pHolder->_didAddSlots(pSlot, 1);
		return iterator(pSlot);
	}

	template < class SlotType>
	SlotIterator CSlotArray<SlotType>::_insert(const SlotIterator& it, const Widget_p pWidgets[], int amount)
	{
		//TODO: Add assert

		SlotType * pSlot = static_cast<SlotType*>(it._slot());
		pSlot = _insertEmpty(pSlot, amount);

		for (int i = 0; i < amount; i++)
		{
			this->_releaseGuardPointer(pWidgets[i], &pSlot);
			pSlot[i]._setWidget(pWidgets[i]);
		}
		m_pHolder->_didAddSlots(pSlot, amount);
		return iterator(pSlot);
	}

	template < class SlotType>
	SlotIterator CSlotArray<SlotType>::_remove(const SlotIterator& it)
	{
		//TODO: Add assert

		SlotType * pSlot = static_cast<SlotType*>(it._slot());
		m_pHolder->_willRemoveSlots(pSlot, 1);
		return iterator(_remove(pSlot));
	}

	template < class SlotType>
	SlotIterator CSlotArray<SlotType>::_remove(const SlotIterator& beg, const SlotIterator& end)
	{
		//TODO: Add assert

		SlotType * pBeg = static_cast<SlotType*>(beg._slot());
		SlotType * pEnd = static_cast<SlotType*>(end._slot());
		m_pHolder->_willRemoveSlots(pBeg, pEnd-pBeg);
		return iterator(_remove(pBeg, pEnd));
	}

	template < class SlotType>
	SlotIterator CSlotArray<SlotType>::_moveToFront(const SlotIterator& it)
	{
		//TODO: Assert

		SlotType * pFrom = static_cast<SlotType*>(it._slot());
		SlotType * pTo = _begin();

		_move(pFrom, pTo);
		m_pHolder->_didMoveSlots(pFrom, pTo, 1);
		return iterator(_begin());
	}

	template < class SlotType>
	SlotIterator CSlotArray<SlotType>::_moveToBack(const SlotIterator& it)
	{
		//TODO: Assert

		SlotType * pFrom = static_cast<SlotType*>(it._slot());
		SlotType * pTo = _end();

		_move(pFrom, pTo);
		m_pHolder->_didMoveSlots(pFrom, pTo, 1);
		return iterator(_last());
	}

	template < class SlotType>
	SlotIterator CSlotArray<SlotType>::_moveBefore(const SlotIterator& it, const SlotIterator& sibling)
	{
		//TODO: Assert

		SlotType * pFrom = static_cast<SlotType*>(it._slot());
		SlotType * pTo = static_cast<SlotType*>(sibling._slot());

		if (pFrom < pTo)
			pTo--;

		if (pFrom != pTo)
		{
			_move(pFrom, pTo);
			m_pHolder->_didMoveSlots(pFrom, pTo, 1);
		}

		return iterator(pTo);
	}



	template < class SlotType>
	void CSlotArray<SlotType>::_move(SlotType * pFrom, SlotType * pTo)
	{
		SlotType temp = std::move(*pFrom);
		if (pFrom < pTo)
		{
			if (SlotType::safe_to_relocate)
			{
				memmove((void*)pFrom, &pFrom[1], sizeof(SlotType) * (pTo - pFrom));
				_reallocBlock(pFrom, pTo);
			}
			else
			{
				for (int i = 0; i < pTo - pFrom; i++)
					pFrom[i] = std::move(pFrom[i + 1]);
			}
		}
		else
		{
			if (SlotType::safe_to_relocate)
			{
				memmove((void*)&pTo[1], pTo, sizeof(SlotType) * (pFrom - pTo));
				_reallocBlock(pTo, pFrom);
			}
			else
			{
				for (int i = int(pFrom - pTo); i > 0; i--)
					pTo[i] = std::move(pTo[i - 1]);
			}
		}
		*pTo = std::move(temp);
	}

	template < class SlotType>
	void CSlotArray<SlotType>::_reorder(int order[])
	{
		if (m_size == 0)
			return;

		int size = sizeof(SlotType)*m_capacity;
		SlotType* pOld = m_pArray;
		m_pArray = (SlotType*)malloc(size);
		for (int i = 0; i < m_size; i++)
		{
			int ofs = order[i];
			new (&m_pArray[i]) SlotType(std::move(pOld[ofs]));
		}
		free(pOld);
	}

	template < class SlotType>
	SlotType * CSlotArray<SlotType>::_find(const Widget* pWidget) const
	{
		for (auto p = _begin(); p < _end(); p++)
			if (p->_widget() == pWidget)
				return p;

		return nullptr;
	}

	template < class SlotType>
	void CSlotArray<SlotType>::_reallocArray(int capacity)
	{
		int size = sizeof(SlotType)*capacity;
		SlotType* pNew = (SlotType*)malloc(size);
		SlotType* pOld = m_pArray;
		m_pArray = pNew;
		m_capacity = capacity;

		if (pOld)
		{
			if (SlotType::safe_to_relocate)
			{
				memcpy((void*)pNew, pOld, sizeof(SlotType)*m_size);
				_reallocBlock(_begin(), _end());
			}
			else
			{
				for (int i = 0; i < m_size; i++)
					new (&pNew[i]) SlotType(std::move(pOld[i]));
			}
			free(pOld);
		}
	}

	template < class SlotType>
	void CSlotArray<SlotType>::_reallocBlock(SlotType * pBeg, SlotType * pEnd)
	{
		while (pBeg < pEnd)
		{
			pBeg->_relink();
			pBeg++;
		}
	}

	template < class SlotType>
	SlotType* CSlotArray<SlotType>::_deleteBlock(SlotType * pBeg, SlotType * pEnd)
	{
		int blocksToMove = int(_end() - pEnd);
		if (SlotType::safe_to_relocate)
		{
			_killBlock(pBeg, pEnd);

			if (blocksToMove > 0)
			{
				memmove(pBeg, pEnd, sizeof(SlotType) * blocksToMove);
				_reallocBlock(pBeg, pBeg + blocksToMove);
			}
		}
		else
		{
			for (int i = 0; i < blocksToMove; i++)
				pBeg[i] = std::move(pEnd[i]);

			_killBlock(pBeg + blocksToMove, pEnd);
		}
		m_size -= pEnd - pBeg;
		return pBeg;
	}


	template < class SlotType>
	SlotType* CSlotArray<SlotType>::_insertBlock(SlotType * pPos, int entries)
	{
		if (entries <= m_capacity - m_size)
		{
			int nToMove = (int)(_end() - pPos);
			if (nToMove > 0)
			{
				if (SlotType::safe_to_relocate)
				{
					memmove((void*)&pPos[entries], pPos, sizeof(SlotType) * nToMove);
					_reallocBlock(&pPos[entries], &pPos[entries + nToMove]);
				}
				else
				{

					int nMoveToNew = min(entries, nToMove);
					int nMoveToExisting = nToMove - nMoveToNew;

					SlotType * pFrom = _end();
					SlotType * pTo = pFrom + entries;

					// Move first batch of entries to uninitialized objects

					for (int i = 0; i < nMoveToNew; i++)
					{
						new (--pTo) SlotType(std::move(*(--pFrom)));
						/*							new (--pTo) SlotType();
						*pTo = * (--pFrom);
						pTo->relink();
						*/
					}

					// Move the rest to existing objects

					for (int i = 0; i < nMoveToExisting; i++)
						*(--pTo) = std::move(*(--pFrom));

				}
			}
		}
		else
		{
			int grow = entries - (m_capacity - m_size);
			if (grow == 1)
				m_capacity = (m_capacity + 1) * 2;
			else
				m_capacity += grow;

			SlotType* pNew = (SlotType*)malloc(sizeof(SlotType) * m_capacity);
			SlotType* pOld = m_pArray;
			SlotType* pNewPos = pNew + (pPos - pOld);

			if (SlotType::safe_to_relocate)
			{
				memcpy((void*)pNew, pOld, sizeof(SlotType) * (pPos - pOld));
				memcpy((void*)&pNewPos[entries], pPos, sizeof(SlotType) * (_end() - pPos));
				_reallocBlock(pNew, pNewPos);
				_reallocBlock(&pNewPos[entries], pNew + m_size + entries);
			}
			else
			{
				for (int i = 0; i < pPos - pOld; i++)
				{
					new (&pNew[i]) SlotType(std::move(pOld[i]));
				}

				SlotType * pDest = &pNewPos[entries];
				for (int i = 0; i < _end() - pPos; i++)
				{
					new (&pDest[i]) SlotType(std::move(pPos[i]));
				}
			}

			if (pOld)
				free(pOld);

			m_pArray = pNew;
			pPos = pNewPos;
		}

		m_size += entries;
		_initBlock(pPos, &pPos[entries]);
		return pPos;
	}

	template < class SlotType>
	void CSlotArray<SlotType>::_killBlock(SlotType * pBeg, SlotType * pEnd)
	{
		while (pBeg < pEnd)
			(pBeg++)->~SlotType();
	}

	template < class SlotType>
	void CSlotArray<SlotType>::_initBlock(SlotType * pBeg, SlotType * pEnd)
	{
		while (pBeg < pEnd)
			new (pBeg++) SlotType(m_pHolder);
	}




} // namespace wg

#endif //WG_CSLOTARRAY_IMPL_DOT_H

