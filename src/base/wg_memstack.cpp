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
#include <assert.h>
#include <wg_memstack.h>

namespace wg
{

	MemStack::BlockHeader::BlockHeader(int bytes)
	{
		pBlock = new char[bytes];
		size = bytes;
		allocated = 0;
	}

	MemStack::BlockHeader::~BlockHeader()
	{
		delete [] pBlock;
	}


	//____ constructor ____________________________________________________________

	MemStack::MemStack( int blockSize ) : m_blockSize(blockSize)
	{
	}



	//____ alloc() ________________________________________________________________

	char * MemStack::alloc( int bytes )
	{
		BlockHeader * p = m_blocks.last();

		if( !p || p->size - p->allocated < bytes )
		{
			int size = m_blockSize>=bytes?m_blockSize:bytes;
			p = new BlockHeader(size);
			m_blocks.pushBack(p);
		}

		char * pBytes = p->pBlock + p->allocated;
		p->allocated += bytes;
		return pBytes;
	}

	//____ release() ______________________________________________________________

	void MemStack::release( int bytes )
	{
		if (bytes == 0)
			return;

		assert( !m_blocks.isEmpty() && bytes <= m_blocks.last()->allocated && bytes > 0 );

		m_blocks.last()->allocated -= bytes;
		if( m_blocks.last()->allocated == 0 && m_blocks.first() != m_blocks.last() )
			delete m_blocks.popBack();
	}

	//____ clear() ________________________________________________________________

	void MemStack::clear()
	{
		m_blocks.clear();
	}

	//____ isEmpty() ______________________________________________________________

	bool MemStack::isEmpty() const
	{
		if( m_blocks.isEmpty() || (m_blocks.size() == 1 && m_blocks.first()->allocated == 0) )
			return true;

		return false;
	}

} // namespace wg
