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

