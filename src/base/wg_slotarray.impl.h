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

#ifndef WG_SLOTARRAY_IMPL_DOT_H
#define WG_SLOTARRAY_IMPL_DOT_H
#pragma once

#include <stdlib.h>
#include <memory.h>

#include <wg_slotarray.h>


namespace wg
{
	template < class SlotType>
	void SlotArray<SlotType>::move(SlotType * pFrom, SlotType * pTo)
	{
		if (pFrom < pTo)
		{
			SlotType temp = std::move(*pFrom);
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
			*pTo = std::move(temp);
		}
		else
		{
			SlotType temp = std::move(*pFrom);
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
			*pTo = std::move(temp);
		}
	}

	template < class SlotType>
	void SlotArray<SlotType>::reorder(int order[])
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
	SlotType *	SlotArray<SlotType>::find(const Widget* pWidget) const
	{
		for (auto p = begin(); p < end(); p++)
			if (p->pWidget == pWidget)
				return p;

		return nullptr;
	}

	template < class SlotType>
	void SlotArray<SlotType>::_reallocArray(int capacity)
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
				_reallocBlock(begin(), end());
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
	void SlotArray<SlotType>::_reallocBlock(SlotType * pBeg, SlotType * pEnd)
	{
		while (pBeg < pEnd)
		{
			pBeg->relink();
			pBeg++;
		}
	}

	template < class SlotType>
	SlotType* SlotArray<SlotType>::_deleteBlock(SlotType * pBeg, SlotType * pEnd)
	{
		int blocksToMove = int(end() - pEnd);
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

			_killBlock(pBeg+blocksToMove, pEnd);
		}
		m_size -= pEnd - pBeg;
		return pBeg;
	}


	template < class SlotType>
	SlotType* SlotArray<SlotType>::_insertBlock(SlotType * pPos, int entries)
	{
		if (entries <= m_capacity - m_size)
		{
			int nToMove = (int)(end() - pPos);
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

					SlotType * pFrom = end();
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
				memcpy((void*)&pNewPos[entries], pPos, sizeof(SlotType) * (end() - pPos));
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
				for (int i = 0; i < end() - pPos; i++)
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
	void SlotArray<SlotType>::_killBlock(SlotType * pBeg, SlotType * pEnd)
	{
		while (pBeg < pEnd)
			(pBeg++)->~SlotType();
	}

	template < class SlotType>
	void SlotArray<SlotType>::_initBlock(SlotType * pBeg, SlotType * pEnd)
	{
		while (pBeg < pEnd)
			new (pBeg++) SlotType();
	}



}

#endif //WG_SLOTARRAY_IMPL_DOT_H
