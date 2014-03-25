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


template<class H> class WgHookArray
{
public:
	WgHookArray() : m_pArray(0), m_size(0), m_capacity(0) {}
	WgHookArray(int capacity) : m_size(0), m_capacity(capacity) { m_pArray = (H*) malloc( sizeof(H)*capacity ); _initBlock(0,capacity); }
	~WgHookArray() { _killBlock( 0, m_size ); free(m_pArray); }

	int		Size() const { return m_size; };
	int		Capacity() const { return m_capacity; }

	H*		Hook(int index) const { return &m_pArray[index]; }

	H*		Insert(int index) { _insertBlock( index, 1); return &m_pArray[index]; }
	H*		Insert(int index, int entries) { _insertBlock( index, entries ); return &m_pArray[index]; }
	H*		Add() { if( m_capacity+1 > m_size ) _reallocArray( ((m_capacity+1)*3) / 2 ); _initBlock(m_size,1); return &m_pArray[m_size++]; }
	H*		Add(int entries) { if( m_capacity+entries > m_size ) _reallocArray( m_capacity+entries ); _initBlock(m_size,entries); int ofs = m_size ; m_size += entries; return &m_pArray[ofs]; }
	void	Remove(int index) { _deleteBlock(index,1); }
	void	Remove(int index, int entries) { _deleteBlock(index,entries); }

	void	Clear() { _killBlock( 0, m_size ); free(m_pArray); m_pArray = 0; m_capacity = 0; m_size = 0; }
	void	SetCapacity(int capacity) { if( capacity != m_capacity ) _realloc(capacity); }

	H*		Prev( const H* pHook ) const { if( pHook > m_pArray ) return const_cast<H*>(pHook)-1; return 0; }
	H*		Next( const H* pHook ) const { if( pHook < &m_pArray[m_size] ) return const_cast<H*>(pHook)+1; return 0; }

	int		Index( const H* pHook ) const { return m_pArray - pHook; }
	int		IsInArray( const H* pHook ) const { if( pHook >= m_pArray && pHook < &m_pArray[m_size] ) return true; return false; }

protected:

	void	_reallocArray( int capacity )
	{
		int size = sizeof(H)*capacity;
		H* pNew = (H*) malloc( size );
		H* pOld = m_pArray;

		memcpy( pNew, pOld, m_size );
		m_pArray = pNew;
		m_capacity = capacity;
		_reallocBlock( 0, m_size );
		free( pOld );
	}

	void	_reallocBlock( int index, int entries )
	{
		for( int i = 0 ; i < entries ; i++ )
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
			memmove( &m_pArray[index], &m_pArray[index+entries], sizeof(H) * blocksToMove );
			_reallocBlock( index, blocksToMove );
		}
		m_size -= entries;
	}

	void	_insertBlock( int index, int entries )
	{
		if( entries <= m_capacity - m_size )
		{
			memmove( &m_pArray[index+entries], &m_pArray[index], sizeof(H) * entries );
		}
		else
		{
			int grow = entries - (m_capacity - m_size);
			if( grow == 1 )
				m_capacity = ((m_capacity+1)*2) / 3;
			else
				m_capacity += grow;

			H* pNew = (H*) malloc( m_capacity );
			H* pOld = m_pArray;

			memcpy( pNew, pOld, sizeof(H) * index );
			memcpy( pNew + index + entries, pOld + index, sizeof(H) * (m_size - index) );
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

	void	_initBlock( int index, int entries )
	{
		for( int i = 0 ; i < entries ; i++ )
			new (&m_pArray[index++]) H();
	}

	int		m_capacity;
	int		m_size;
	H*		m_pArray;
};



#endif //WG_HOOKARRAY_DOT_H
