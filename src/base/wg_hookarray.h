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

#ifndef WG_HOOKARRAY_DOT_H
#define WG_HOOKARRAY_DOT_H

#include <stdlib.h>
#include <memory.h>

namespace wg 
{
	
	template<class H> class HookArray
	{
	public:
		HookArray() : m_pArray(0), m_size(0), m_capacity(0) {}
		HookArray(int capacity) : m_size(0), m_capacity(capacity) { m_pArray = (H*) malloc( sizeof(H)*capacity ); }
		~HookArray() { _killBlock( 0, m_size ); free(m_pArray); }
	
		int		size() const { return m_size; }
		bool	isEmpty() const { return m_size == 0; }
		int		capacity() const { return m_capacity; }
	
		H*		hook(int index) const { return &m_pArray[index]; }
	
		H*		insert(int index) { _insertBlock( index, 1); return &m_pArray[index]; }
		H*		insert(int index, int entries) { _insertBlock( index, entries ); return &m_pArray[index]; }
		H*		add() { if( m_size == m_capacity ) _reallocArray( ((m_capacity+1)*3) / 2 ); _initBlock(m_size); return &m_pArray[m_size++]; }
		H*		add(int entries) { if( m_size+entries > m_capacity ) _reallocArray( m_capacity+entries ); _initBlock(m_size,entries); int ofs = m_size ; m_size += entries; return &m_pArray[ofs]; }
		void	remove(int index) { _deleteBlock(index,1); }
		void	remove(int index, int entries) { _deleteBlock(index,entries); }
	
		void	clear() { _killBlock( 0, m_size ); free(m_pArray); m_pArray = 0; m_capacity = 0; m_size = 0; }
		void	setCapacity(int capacity) { if( capacity != m_capacity ) _reallocArray(capacity); }
	
		H*		prev( const H* pHook ) const { if( pHook > m_pArray ) return const_cast<H*>(pHook)-1; return 0; }
		H*		next( const H* pHook ) const { if( pHook < &m_pArray[m_size-1] ) return const_cast<H*>(pHook)+1; return 0; }
	
		int		index( const H* pHook ) const { return m_pArray - pHook; }
		int		isInArray( const H* pHook ) const { if( pHook >= m_pArray && pHook < &m_pArray[m_size] ) return true; return false; }
	
		H*		begin() const { return m_pArray; }
		H*		end() const { return m_pArray + m_size; }
	
		void	reorder( int order[] )
		{
			if( m_size == 0 ) 
				return;
	
			int size = sizeof(H)*m_capacity;
			H* pOld = m_pArray;
			m_pArray = (H*) malloc( size );
			_initBlock(0,m_size);
			for( int i = 0 ; i < m_size ; i++ )
			{
				int ofs = order[i];
				m_pArray[i] = pOld[ofs];
			}
			_reallocBlock( 0, m_size );
			free( pOld );
		}
	
	protected:
	
		void	_reallocArray( int capacity )
		{
			int size = sizeof(H)*capacity;
			H* pNew = (H*) malloc( size );
			H* pOld = m_pArray;
			m_pArray = pNew;
			m_capacity = capacity;
	
			if( pOld )
			{
				memcpy( (void*)pNew, pOld, sizeof(H)*m_size );
				_reallocBlock( 0, m_size );
				free( pOld );
			}
		}
	
		void	_reallocBlock( int index, int entries )
		{
			int end = index + entries;
			while( index < end )
			{
				H * p = &m_pArray[index++];
				p->_relinkWidget();
				if( p->m_pPtrHub )
					p->m_pPtrHub->pObj = p;
			}
		}
	
		void	_deleteBlock( int index, int entries )
		{
			_killBlock( index, entries );
			if( index + entries < m_size )
			{
				int blocksToMove = m_size - index - entries;
				memmove( (void*)&m_pArray[index], &m_pArray[index+entries], sizeof(H) * blocksToMove );
				_reallocBlock( index, blocksToMove );
			}
			m_size -= entries;
		}
	
		void	_insertBlock( int index, int entries )
		{
			if( entries <= m_capacity - m_size )
			{
				memmove( (void*)&m_pArray[index+entries], &m_pArray[index], sizeof(H) * (m_size - index) );
			}
			else
			{
				int grow = entries - (m_capacity - m_size);
				if( grow == 1 )
					m_capacity = ((m_capacity+1)*3) / 2;
				else
					m_capacity += grow;
	
				H* pNew = (H*) malloc( sizeof(H) * m_capacity );
				H* pOld = m_pArray;
	
				memcpy( (void*)pNew, pOld, sizeof(H) * index );
                memcpy( (void*)&pNew[index + entries], pOld + index, sizeof(H) * (m_size - index) );
				free( pOld );
				m_pArray = pNew;
				_reallocBlock( 0, index );
			}
			_reallocBlock( index + entries, m_size - index );
			_initBlock( index, entries );
			m_size += entries;
		}
	
		void	_killBlock( int index, int entries )
		{
			for( int i = 0 ; i < entries ; i++ )
				m_pArray[index++].~H();
		}
	
		inline void	_initBlock( int index )
		{
				new (&m_pArray[index]) H();
		}
	
	
		void	_initBlock( int index, int entries )
		{
			for( int i = 0 ; i < entries ; i++ )
				new (&m_pArray[index++]) H();
		}
	
		int		m_capacity;
		int		m_size;
		H*		m_pArray;
	};
	
	
	

} // namespace wg
#endif //WG_HOOKARRAY_DOT_H
