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
	WgMemPool( int entriesPerBlock, int entrySize );
	virtual ~WgMemPool();

	void *	AllocEntry();
	void	FreeEntry( void * pEntry );

	inline int		EntriesAllocated() const { return m_nAllocEntries; }
	inline int		EntriesAvailable() const { return m_blocks.Size()*m_nEntriesPerBlock - m_nAllocEntries; }
	inline int		Capacity() const { return m_blocks.Size()*m_nEntriesPerBlock; }
	inline bool		IsEmpty() const { return (m_nAllocEntries == 0); }


private:

	class Block;

	Block *	_addBlock();



	// Block of entries.

	class Block : public WgLink
	{
	public:
		Block( int nEntries, int entrySize );
		~Block();

		LINK_METHODS( Block );


		void * allocEntry();
		bool freeEntry( void * pEntry );

		void *		pMemBlock;			// Memory area containing our entries.
		int			blockSize;			// Size of memory area containing our entries.
		int			nAllocEntries;		// Number of entires currently in used.
		int			nCleanEntries;		// Number of clean entries, all entries after this
										// are free AND uninitzialised.
		int			maxEntries;			// Number of entries in block.
		int			firstFreeEntry;		// Number of first free entry, entry contains number of next
										// unless firstFreeEntry==nCleanEntries.
		int			entrySize;			// Size of each individual entry.

	};

	WgChain<Block>	m_blocks;
	int				m_nEntriesPerBlock;
	int				m_entrySize;
	int				m_nAllocEntries;
};




#endif //WG_MEMPOOL_DOT_H

