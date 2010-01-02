
#include <wg_mempool.h>
#include <malloc.h>

Uint32	WgMemPool::g_allocatedEver = 0;

WgMemPool::WgMemPool( Uint32 entriesPerBlock, Uint32 entrySize )
{
	m_nEntriesPerBlock	= entriesPerBlock;
	m_entrySize			= entrySize;
}

WgMemPool::~WgMemPool()
{
}

void * WgMemPool::AllocEntry()
{
	g_allocatedEver++;

	Block * pBlock = m_blocks.getFirst();
	if(pBlock == 0)
		pBlock = AddBlock();

	if( pBlock->nAllocEntries == pBlock->maxEntries )
	{
		m_blocks.push_back(pBlock);			// This block is full so we put it in the back.

		pBlock = m_blocks.getFirst();
		if( pBlock->nAllocEntries == pBlock->maxEntries )
		{
			AddBlock();						// We don't have any free entries left in any block.
											// so we need to create a new one.
			pBlock = m_blocks.getFirst();
		}
	}

	return pBlock->AllocEntry();
}

void WgMemPool::FreeEntry( void * pEntry )
{
	if( pEntry == 0 )
		return;

	Block * pBlock = m_blocks.getFirst();

	while( (pBlock && pEntry < pBlock->pMemBlock) || (pEntry >= ((Uint8*)pBlock->pMemBlock) + pBlock->blockSize) )
	{
		pBlock = pBlock->getNext();
	}

	if( !pBlock )
	{
		return;								// ERROR!!! ENTRY HAS NOT BEEN RESERVED THROUGH US!!!!!!!!!!
	}

	if( pBlock->nAllocEntries == pBlock->maxEntries )
		m_blocks.push_front(pBlock);			// Full block will get an entry free, needs to be among the free ones...

	pBlock->FreeEntry(pEntry);

	if( pBlock->nAllocEntries == 0 )
		delete pBlock;
}


WgMemPool::Block *WgMemPool::AddBlock()
{
	Block * pBlock = new Block( m_nEntriesPerBlock, m_entrySize );
	m_blocks.push_front( pBlock );
	return pBlock;
}




WgMemPool::Block::Block( Uint32 _nEntries, Uint32 _entrySize )
{
	pMemBlock		= malloc( _nEntries*_entrySize );
	blockSize		= _nEntries*_entrySize;
	nAllocEntries	= 0;
	nCleanEntries	= 0;
	maxEntries		= _nEntries;
	firstFreeEntry	= 0;
	entrySize		= _entrySize;
}

WgMemPool::Block::~Block()
{
	free( pMemBlock );
}

void * WgMemPool::Block::AllocEntry()
{
	if( nAllocEntries == maxEntries )
		return 0;

	void * p = ((Uint8*)pMemBlock) + firstFreeEntry*entrySize;
	nAllocEntries++;

	if( firstFreeEntry == nCleanEntries )
	{
		firstFreeEntry++;
		nCleanEntries++;
	}
	else
	{
		firstFreeEntry = * ((Uint32*)p);
	}
	return p;
}


bool WgMemPool::Block::FreeEntry( void * pEntry )
{
	if( pEntry < pMemBlock || pEntry >= ((Uint8*)pMemBlock) + blockSize )
		return false;

	* ((Uint32*)pEntry) = firstFreeEntry;
	firstFreeEntry = ((Uint32) (((char*)pEntry) - ((char*)pMemBlock))) / entrySize;

	nAllocEntries--;
	return true;
}


