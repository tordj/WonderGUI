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

#ifndef WG_SLOTARRAY_DOT_H
#define WG_SLOTARRAY_DOT_H
#pragma once

#include <stdlib.h>
#include <memory.h>

#include <wg_slot.h>

namespace wg 
{
	class Object;
	class Widget;

	template<class SlotType> class SlotArray		/** @private */
	{
	public:
		SlotArray() : m_pArray(0), m_size(0), m_capacity(0) {}
		SlotArray(int capacity) : m_size(0), m_capacity(capacity) { m_pArray = (SlotType*) malloc( sizeof(SlotType)*capacity ); }
		~SlotArray() { _killBlock( begin(), end() ); free(m_pArray); }

		int			size() const { return m_size; }
		bool		isEmpty() const { return m_size == 0; }
		int			capacity() const { return m_capacity; }

		inline SlotType& operator[](int index) { return m_pArray[index]; }
		inline const SlotType& operator[](int index) const { return m_pArray[index]; }

		SlotType*	slot(int index) const { return &m_pArray[index]; }
	
		SlotType*	add() { if( m_size == m_capacity ) _reallocArray( ((m_capacity+1)*2) ); _initBlock(end()); return &m_pArray[m_size++]; }
		SlotType*	add(int entries) { if( m_size+entries > m_capacity ) _reallocArray( m_capacity+entries ); _initBlock(end(),end()+entries); int ofs = m_size ; m_size += entries; return &m_pArray[ofs]; }
		
		SlotType*	insert(int index) { return _insertBlock( &m_pArray[index], 1); }
		SlotType*	insert(int index, int entries) { return _insertBlock(&m_pArray[index], entries ); }
		SlotType*	insert(SlotType * pPos) { return _insertBlock(pPos, 1); }
		SlotType*	insert(SlotType * pPos, int entries) { return _insertBlock(pPos, entries); }

		SlotType*	remove(int index) { return _deleteBlock( &m_pArray[index], &m_pArray[index+1]); }
		SlotType*	remove(int index, int entries) { return _deleteBlock(&m_pArray[index], &m_pArray[index + entries]); }
		SlotType*	remove(SlotType * pPos) { return _deleteBlock(pPos, pPos+1); }
		SlotType*	remove(SlotType * pPos, SlotType * pEnd) { return _deleteBlock(pPos, pEnd); }

		void		move(int index, int newIndex) { move(&m_pArray[index], &m_pArray[newIndex]); }
		void		move(SlotType * pFrom, SlotType * pTo)
		{
			if (pFrom < pTo)
			{
				SlotType temp = * pFrom;
				memmove((void*)pFrom, &pFrom[1], sizeof(SlotType) * (pTo-pFrom));
				* pTo = temp;
				_reallocBlock(pFrom, pTo + 1);
			}
			else
			{
				SlotType temp = * pFrom;
				memmove((void*)&pFrom[1], pFrom, sizeof(SlotType) * (pFrom-pTo));
				* pTo = temp;
				_reallocBlock(pTo, pFrom + 1);
			}
		}


		void		clear() { _killBlock( begin(), end() ); free(m_pArray); m_pArray = 0; m_capacity = 0; m_size = 0; }
		void		setCapacity(int capacity) { if( capacity != m_capacity ) _reallocArray(capacity); }
	
		SlotType*	prev( const SlotType* pSlot ) const { if( pSlot > m_pArray ) return const_cast<SlotType*>(pSlot)-1; return 0; }
		SlotType*	next( const SlotType* pSlot ) const { if( pSlot < &m_pArray[m_size-1] ) return const_cast<SlotType*>(pSlot)+1; return 0; }
	
		int			index( const SlotType* pSlot ) const { return m_pArray - pSlot; }
		int			isInArray( const SlotType* pSlot ) const { if( pSlot >= m_pArray && pSlot < &m_pArray[m_size] ) return true; return false; }

		SlotType*	first() const { return m_pArray; }
		SlotType*	last() const { return m_pArray + (m_size-1); }

		SlotType*	begin() const { return m_pArray; }
		SlotType*	end() const { return m_pArray + m_size; }
	
		void	reorder( int order[] )
		{
			if( m_size == 0 ) 
				return;
	
			int size = sizeof(SlotType)*m_capacity;
			SlotType* pOld = m_pArray;
			m_pArray = (SlotType*) malloc( size );
			_initBlock(begin(),end());
			for( int i = 0 ; i < m_size ; i++ )
			{
				int ofs = order[i];
				m_pArray[i] = pOld[ofs];
			}
			_reallocBlock( begin(), end() );
			free( pOld );
		}
	
	protected:
	
		void	_reallocArray( int capacity )
		{
			int size = sizeof(SlotType)*capacity;
			SlotType* pNew = (SlotType*) malloc( size );
			SlotType* pOld = m_pArray;
			m_pArray = pNew;
			m_capacity = capacity;
	
			if( pOld )
			{
				memcpy( (void*)pNew, pOld, sizeof(SlotType)*m_size );
				_reallocBlock( begin(), end() );
				free( pOld );
			}
		}
	
		void	_reallocBlock(SlotType * pBeg, SlotType * pEnd)
		{
			while (pBeg < pEnd)
			{
				pBeg->relink();
				pBeg++;
			}
		}

		SlotType* _deleteBlock(SlotType * pBeg, SlotType * pEnd)
		{
			_killBlock(pBeg, pEnd);
			int blocksToMove = end() - pEnd;
			if( blocksToMove > 0 )
			{
				memmove(pBeg, pEnd, sizeof(SlotType) * blocksToMove);
				_reallocBlock(pBeg, pBeg+blocksToMove);
			}
			m_size -= pEnd - pBeg;
			return pBeg;
		}


		SlotType*	_insertBlock(SlotType * pPos, int entries)
		{
			if (entries <= m_capacity - m_size)
			{
				memmove((void*)&pPos[entries], pPos, sizeof(SlotType) * (end() - pPos));
			}
			else
			{
				int grow = entries - (m_capacity - m_size);
				if (grow == 1)
					m_capacity = ((m_capacity + 1) * 3) / 2;
				else
					m_capacity += grow;

				SlotType* pNew = (SlotType*)malloc(sizeof(SlotType) * m_capacity);
				SlotType* pOld = m_pArray;
				SlotType* pNewPos = pNew + (pPos - pOld);

				memcpy((void*)pNew, pOld, sizeof(SlotType) * (pPos-pOld));
				memcpy((void*)&pNewPos[entries], pPos, sizeof(SlotType) * (end() - pPos));
				if (pOld)
					free(pOld);
				m_pArray = pNew;
				_reallocBlock(pNew, pNewPos);
				pPos = pNewPos;
			}
			m_size += entries;
			_reallocBlock(&pPos[entries], end());
			_initBlock(pPos, &pPos[entries]);
			return pPos;
		}

		void	_killBlock(SlotType * pBlock)
		{
			pBlock->~SlotType();
		}


		void	_killBlock(SlotType * pBeg, SlotType * pEnd)
		{
			while (pBeg < pEnd)
				(pBeg++)->~SlotType();
		}

		inline void	_initBlock(SlotType * pBlock)
		{
			new (pBlock) SlotType();
		}

		void	_initBlock(SlotType * pBeg, SlotType * pEnd)
		{
			while (pBeg < pEnd)
				new (pBeg++) SlotType();
		}

	
		int			m_capacity;
		int			m_size;
		SlotType *	m_pArray;
	};
	
	
	

} // namespace wg
#endif //WG_SLOTARRAY_DOT_H
