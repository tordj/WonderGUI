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

#ifndef WG_DYNARRAY_DOT_H
#define WG_DYNARRAY_DOT_H
//=============================================================================


#ifndef	WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

//____ Template: WgDynArray ___________________________________________________

template<class T>
class WgDynArray
{
public:
	WgDynArray();
	~WgDynArray();

	void		clear();

	void		insert( Uint32 offset, T value );
	void		remove( Uint32 offset );

	void		insertMultiple( Uint32 offset, Uint32 nEntries, const T * pEntries );
	void		removeMultiple( Uint32 offset, Uint32 nEntries );

	void		setSize( Uint32 newSize );
	void		setGrowStepSize( Uint32 size );
	void		setShrinkStepSize( Uint32 size );

	inline	T*			array() { return m_pArray; };
	inline	Uint32	entries() { return m_nEntries; };
	inline	Uint32	size() { return m_size; };

	void		push( T value );
	T				pop();

	void		pushMultiple( Uint32 n, const T* pValues );
	void		popMultiple( Uint32 n );

	const T&	operator[](Uint32 offset) const { return m_pArray[offset]; }
	T&			operator[](Uint32 offset)		{ return m_pArray[offset]; }

private:

	enum { DEF_GROW_STEP_SIZE		= 16 };
	enum { DEF_SHRINK_STEP_SIZE	= 16 };

	T	*			m_pArray;
	Uint32	m_nEntries;
	Uint32	m_size;
	Uint32	m_growStepSize;
	Uint32	m_shrinkStepSize;
};


//____ WgDynArray::WgDynArray()________________________________________________

template<class T> WgDynArray<T>::WgDynArray(void)
{
	m_pArray					= 0;
	m_nEntries				= 0;
	m_size						= 0;
	m_growStepSize		= DEF_GROW_STEP_SIZE;
	m_shrinkStepSize	= DEF_SHRINK_STEP_SIZE;
}

//____ WgDynArray::~WgDynArray()_______________________________________________

template<class T> WgDynArray<T>::~WgDynArray()
{
	if( m_pArray )
		delete [] m_pArray;
}

//____ WgDynArray::clear()_____________________________________________________

template<class T> void WgDynArray<T>::clear()
{
	if( m_pArray )
		delete [] m_pArray;

	m_pArray		= 0;
	m_nEntries	= 0;
	m_size			= 0;
}

//____ WgDynArray::insert()____________________________________________________

template<class T> void WgDynArray<T>::insert( Uint32 _offset, T _value )
{
	if( m_nEntries == m_size )
	{
		T * pNewArray = new T[m_size + m_growStepSize];

		int i;
		for( i = 0 ; i < (int) _offset ; i++ )
			pNewArray[i] = m_pArray[i];

		pNewArray[i] = _value;
		
		for( ; i < (int) m_nEntries ; i++ )
			pNewArray[i+1] = m_pArray[i];
						
		if( m_pArray )
			delete [] m_pArray;

		m_size += m_growStepSize;
		m_pArray = pNewArray;
		m_nEntries++;
	}
	else
	{
		for( int i = (int) m_nEntries; i > (int) _offset ; i-- )
			m_pArray[i] = m_pArray[i-1];

		m_pArray[_offset] = _value;
		m_nEntries++;
	}
}

//____ WgDynArray::remove()____________________________________________________

template<class T> void WgDynArray<T>::remove( Uint32 _offset )
{

	if( m_size - (m_nEntries-1) >= m_growStepSize + m_shrinkStepSize )
	{
		T * pNewArray = new T[m_size - m_shrinkStepSize];

		for( int i = 0 ; i < (int) _offset ; i++ )
			pNewArray[i] = m_pArray[i];

		for( int i = _offset ; i < (int) m_nEntries-1 ; i++ )
			pNewArray[i] = m_pArray[i+1];
				
		if( m_pArray )
			delete [] m_pArray;

		m_size -= m_shrinkStepSize;
		m_pArray = pNewArray;

		m_nEntries--;
	}
	else
	{
		for( int i = _offset ; i < (int) m_nEntries-1 ; i++ )
			m_pArray[i] = m_pArray[i+1];

		m_nEntries--;
	}
}

//____ WgDynArray::insertMultiple() ___________________________________________

template<class T> void WgDynArray<T>::insertMultiple( Uint32 _offset, Uint32 _nEntries, const T * _pEntries )
{
	if( m_nEntries + _nEntries > m_size )
	{
		int	newSize = ((m_nEntries + _nEntries + m_growStepSize -1)/m_growStepSize)*m_growStepSize;			
		T * pNewArray = new T[newSize];

		int i;
		for( i = 0 ; i < (int) _offset ; i++ )
			pNewArray[i] = m_pArray[i];

		for( int j = 0 ; j < (int) _nEntries ; j++ )						
			pNewArray[i++] = _pEntries[j];
		
		for( int j = _offset ; j < (int) m_nEntries ; j++ )
			pNewArray[i++] = m_pArray[j];
						
		if( m_pArray )
			delete [] m_pArray;

		m_size		= newSize;
		m_pArray	= pNewArray;
		m_nEntries += _nEntries;
	}
	else
	{
		for( int i = _offset ; i < (int) m_nEntries ; i++ )
			m_pArray[i+_nEntries] = m_pArray[i];

		for( int i = 0 ; i < (int) _nEntries ; i++ )
			m_pArray[_offset+i] = _pEntries[i];

		m_nEntries += _nEntries;
	}
}

//____ WgDynArray::removeMultiple() ___________________________________________

template<class T> void WgDynArray<T>::removeMultiple( Uint32 _offset, Uint32 _nEntries )
{
	if( m_size - (m_nEntries-_nEntries) >= m_shrinkStepSize*2 )
	{
		int newSize = ((m_nEntries-_nEntries + m_shrinkStepSize*2-1)/m_shrinkStepSize)*m_shrinkStepSize;
						
		T * pNewArray = new T[newSize];

		int i;
		for( i = 0 ; i < (int) _offset ; i++ )
			pNewArray[i] = m_pArray[i];

		for( int j = _offset + _nEntries ; j < (int) m_nEntries ; j++ )
			pNewArray[i++] = m_pArray[j];
				
		if( m_pArray )
			delete [] m_pArray;

		m_size = newSize;
		m_pArray = pNewArray;

		m_nEntries -= _nEntries;
	}
	else
	{
		int i = _offset;
		for( int j = _offset + _nEntries ; j < (int) m_nEntries ; j++ )
			m_pArray[i++] = m_pArray[j];

		m_nEntries -= _nEntries;
	}
}

//____ WgDynArray::setSize()___________________________________________________

template<class T> void WgDynArray<T>::setSize( Uint32 _newSize )
{
	if( _newSize == m_size )
		return;

	if( _newSize < m_nEntries )
		_newSize = m_nEntries;

	if( _newSize == 0 )
		clear();

	T * pNewArray = new T[_newSize];

	for( int i = 0 ; i < (int) m_nEntries ; i++ )
		pNewArray[i] = m_pArray[i];

	if( m_pArray )
		delete [] m_pArray;

	m_size = _newSize;
	m_pArray = pNewArray;
}


//____ WgDynArray::setGrowStepSize()___________________________________________

template<class T> void WgDynArray<T>::setGrowStepSize( Uint32 _size )
{
	if( _size == 0 )
		_size = 1;

	m_growStepSize = _size;
}

//____ WgDynArray::setShrinkStepSize()_________________________________________

template<class T> void WgDynArray<T>::setShrinkStepSize( Uint32 _size )
{
	if( _size == 0 )
		_size = 1;

	m_shrinkStepSize = _size;
}

//____ WgDynArray::push()______________________________________________________

template<class T> void WgDynArray<T>::push( T value )
{
	if( m_nEntries == m_size )
	{
		T * pNewArray = new T[m_size + m_growStepSize];

		if( m_pArray )
		{
			for( int i = 0 ; i < (int) m_nEntries ; i++ )
				pNewArray[i] = m_pArray[i];

			delete [] m_pArray;
		}

		m_size += m_growStepSize;
		m_pArray = pNewArray;
	}

	m_pArray[m_nEntries++] = value;
}

//____ WgDynArray::pop()_______________________________________________________

template<class T> T WgDynArray<T>::pop()
{
	T val = m_pArray[m_nEntries--];

	if( m_size - m_nEntries >= m_growStepSize + m_shrinkStepSize )
	{
		T * pNewArray = new T[m_size - m_shrinkStepSize];

		for( int i = 0 ; i < (int) m_nEntries ; i++ )
			pNewArray[i] = m_pArray[i];

		if( m_pArray )
			delete [] m_pArray;

		m_size -= m_shrinkStepSize;
		m_pArray = pNewArray;
	}
	return val;
}


//____ WgDynArray::pushMultiple()______________________________________________

template<class T> void WgDynArray<T>::pushMultiple( Uint32 _nEntries, const T* _pEntries )
{
	if( m_nEntries + _nEntries > m_size )
	{
		int	newSize = m_size + ((_nEntries-1)/m_growStepSize)*m_growStepSize + m_growStepSize;			
		T * pNewArray = new T[newSize];

		int i;
		for( i = 0 ; i < (int) m_nEntries ; i++ )
			pNewArray[i] = m_pArray[i];

		for( int j = 0 ; j < (int) _nEntries ; j++ )						
			pNewArray[i++] = _pEntries[j];
		
		if( m_pArray )
			delete [] m_pArray;

		m_size		= newSize;
		m_pArray	= pNewArray;
		m_nEntries += _nEntries;
	}
	else
	{
		for( int i = 0 ; i < (int) _nEntries ; i++ )
			m_pArray[m_nEntries++] = _pEntries[i];
	}
}

//____ WgDynArray::popMultiple()_______________________________________________

template<class T> void WgDynArray<T>::popMultiple( Uint32 _nEntries )
{	
	if( m_size - (m_nEntries-_nEntries) >= m_shrinkStepSize*2 )
	{
		int newSize = ((m_nEntries-_nEntries + m_shrinkStepSize*2-1)/m_shrinkStepSize)*m_shrinkStepSize;
						
		T * pNewArray = new T[newSize];
		m_nEntries -= _nEntries;

		for( int i = 0 ; i < (int) m_nEntries ; i++ )
			pNewArray[i] = m_pArray[i];

		if( m_pArray )
			delete [] m_pArray;

		m_size = newSize;
		m_pArray = pNewArray;
	}
	else
	{
		m_nEntries -= _nEntries;
	}
}

//=============================================================================
#endif // WG_DYNARRAY_DOT_H
