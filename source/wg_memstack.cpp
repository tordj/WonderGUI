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

WgMemStack::BlockHeader::BlockHeader(int bytes)
{
	pBlock = new char[bytes];
	size = bytes;
	allocated = 0;
}

WgMemStack::BlockHeader::~BlockHeader()
{
	delete [] pBlock;
}


//____ Constructor ____________________________________________________________

WgMemStack::WgMemStack( int blockSize ) : m_blockSize(blockSize)
{
}



//____ Alloc() ________________________________________________________________

char * WgMemStack::Alloc( int bytes )
{
	BlockHeader * p = m_blocks.Last();

	if( !p || p->size - p->allocated < bytes )
	{
		int size = m_blockSize>=bytes?m_blockSize:bytes;
		p = new BlockHeader(size);
		m_blocks.PushBack(p);
	}

	char * pBytes = p->pBlock + p->allocated;
	p->allocated += bytes;
	return pBytes;
}

//____ Release() ______________________________________________________________

void WgMemStack::Release( int bytes )
{
	assert( !m_blocks.IsEmpty() && bytes <= m_blocks.Last()->allocated );

	m_blocks.Last()->allocated -= bytes;
	if( m_blocks.Last()->allocated == 0 && m_blocks.First() != m_blocks.Last() )
		delete m_blocks.PopBack();
}

//____ Clear() ________________________________________________________________

void WgMemStack::Clear()
{
	m_blocks.Clear();
}
