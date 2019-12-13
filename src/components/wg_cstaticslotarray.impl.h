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

#ifndef	WG_CSTATICSLOTARRAY_IMPL_DOT_H
#define	WG_CSTATICSLOTARRAY_IMPL_DOT_H
#pragma once

#include <wg_cstaticslotarray.h>

//#define INSTANTIATE_CHILDREN(SlotType)		template class CStaticSlotArray< SlotType >;

namespace wg
{
	class Widget;

	//____ _begin_iterator() ___________________________________________________________

	template < class SlotType>
	SlotIterator CStaticSlotArray<SlotType>::_begin_iterator()
	{ 
		return iterator(_begin()); 
	}

	//____ _end_iterator() _____________________________________________________________

	template < class SlotType>
	SlotIterator CStaticSlotArray<SlotType>::_end_iterator()
	{ 
		return iterator(_end()); 
	}

	//____ _at() _____________________________________________________________

	template < class SlotType>
	BasicSlot& CStaticSlotArray<SlotType>::_at(int index)
	{
		return *_slot(index);
	}

	//____ _object() __________________________________________________________

	template < class SlotType>
	Object * CStaticSlotArray<SlotType>::_object()
	{
		return m_pHolder->_object();
	}

	template < class SlotType>
	const Object * CStaticSlotArray<SlotType>::_object() const
	{
		return m_pHolder->_object();
	}


	//____ _releaseGuardPointer() _____________________________________________

	template < class SlotType>
	void CStaticSlotArray<SlotType>::_releaseGuardPointer(Widget * pToRelease, SlotType ** pPointerToGuard)
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
	void CStaticSlotArray<SlotType>::_move(SlotType * pFrom, SlotType * pTo)
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
	void CStaticSlotArray<SlotType>::_reorder(int order[])
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
	SlotType * CStaticSlotArray<SlotType>::_find(const Widget* pWidget) const
	{
		for (auto p = _begin(); p < _end(); p++)
			if (p->_widget() == pWidget)
				return p;

		return nullptr;
	}

	template < class SlotType>
	void CStaticSlotArray<SlotType>::_reallocArray(int capacity)
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
	void CStaticSlotArray<SlotType>::_reallocBlock(SlotType * pBeg, SlotType * pEnd)
	{
		while (pBeg < pEnd)
		{
			pBeg->_relink();
			pBeg++;
		}
	}

	template < class SlotType>
	SlotType* CStaticSlotArray<SlotType>::_deleteBlock(SlotType * pBeg, SlotType * pEnd)
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
	SlotType* CStaticSlotArray<SlotType>::_insertBlock(SlotType * pPos, int entries)
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
	void CStaticSlotArray<SlotType>::_killBlock(SlotType * pBeg, SlotType * pEnd)
	{
		while (pBeg < pEnd)
			(pBeg++)->~SlotType();
	}

	template < class SlotType>
	void CStaticSlotArray<SlotType>::_initBlock(SlotType * pBeg, SlotType * pEnd)
	{
		while (pBeg < pEnd)
			new (pBeg++) SlotType(m_pHolder);
	}




} // namespace wg

#endif //WG_CSLOTARRAY_IMPL_DOT_H

