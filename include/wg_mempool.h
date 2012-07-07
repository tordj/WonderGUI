#ifndef WG_MEMPOOL_DOT_H
#define WG_MEMPOOL_DOT_H


#include <wg_types.h>
#include <wg_chain.h>

class WgMemPool
{
public:
	WgMemPool( Uint32 entriesPerBlock, Uint32 entrySize );
	virtual ~WgMemPool();

	void *	AllocEntry();
	void	FreeEntry( void * pEntry );

private:

	class Block;

	Block *	_addBlock();



	// Block of entries.

	class Block : public WgLink
	{
	public:
		Block( Uint32 nEntries, Uint32 entrySize );
		~Block();

		LINK_METHODS( Block );


		void * allocEntry();
		bool freeEntry( void * pEntry );

		void *		pMemBlock;			// Memory area containing our entries.
		int			blockSize;			// Size of memory area containing our entries.
		Uint32		nAllocEntries;		// Number of entires currently in used.
		Uint32		nCleanEntries;		// Number of clean entries, all entries after this
										// are free AND uninitzialised.
		Uint32		maxEntries;			// Number of entries in block.
		Uint32		firstFreeEntry;		// Number of first free entry, entry contains number of next
										// unless firstFreeEntry==nCleanEntries.
		Uint32		entrySize;			// Size of each individual entry.

	};

	WgChain<Block>	m_blocks;
	Uint32			m_nEntriesPerBlock;
	Uint32			m_entrySize;

	static Uint32	g_allocatedEver;
};




#endif //WG_MEMPOOL_DOT_H

