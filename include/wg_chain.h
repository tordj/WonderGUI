//-----------------------------------------------------------------------------
//
//								>>> wg_chain.h <<<
//
//					(c) 2002 Tord Jansson (tord.jansson@gmail.com)
//
//	Simple double-linked list implementation, done _MY WAY_!
//
//	This source code may be freely copied, modifed and used in any software
//  project, both free (as in speech) and proprietary. Commercial use and/or
//  distribution does not require any license.
//
//-----------------------------------------------------------------------------
#ifndef WG_CHAIN_DOT_H
#define	WG_CHAIN_DOT_H

#ifndef	WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

class WgLink;

//____ Class WgChainImp _________________________________________________________

class WgChainImp
{
	friend	class WgLink;

protected:
	WgChainImp();
	virtual ~WgChainImp();

	bool	isMember( WgLink * pLink ) const;		///< Check if specified link is a member.
	void	clear();

	void	push_front( WgLink * pLink );		///< Push link onto top of chain.
	WgLink* pop_front();									///< Retrieve link from top of chain (disconnects it).

	void	push_back( WgLink * pLink );		///< Add link to end of chain.
	WgLink* pop_back();									///< Retrieve link from end of chain (disconnects it).

	WgLink* get( Uint32 index ) const;
	
	/*
	void	push_back( WgChainImp * pChain );	///< Push all links from specified chain to back of us.
	void	push_front( WgChainImp * pChain );	///< Push all links from specified chain to our front.

	void	push_back( WgChainImp * pChain, WgLink * pBeg, WgLink * pEnd );		///< Push all links from specified chain to back of us.
	void	push_front( WgChainImp * pChain, WgLink * pBeg, WgLink * pEnd );	///< Push all links from specified chain to our front.
*/

protected:
	WgLink *	m_pFirst;
	WgLink *	m_pLast;
	Uint32		m_size;
};



//____ Class WgChain ____________________________________________________________

template<class T>
class WgChain : public WgChainImp
{
public:
	inline bool	isMember( T* pLink ) const { return WgChainImp::isMember(pLink); };
	inline void	clear() { WgChainImp::clear(); };

	inline T*		getFirst() const { return (T*) m_pFirst; };
	inline T*		getLast() const { return (T*) m_pLast; };

	inline void	push_front( T* pLink ) { WgChainImp::push_front( pLink ); };
	inline T*		pop_front() { return (T*) WgChainImp::pop_front(); };

	inline void	push_back( T* pLink ) { WgChainImp::push_back( pLink ); };
	inline T*		pop_back() { return (T*) WgChainImp::pop_back(); };

	inline T*		get( Uint32 index ) const { return (T*) WgChainImp::get(index); };

	inline Uint32	size() const { return m_size; }
};



//____ Class WgLink _____________________________________________________________

#define	LINK_METHODS( type ) \
	inline bool	moveUp() { return WgLink::moveUp(); }; \
	inline bool	moveDown() { return WgLink::moveDown(); }; \
	inline bool	moveFirst( WgChain<type> * pChain = 0 ) { return WgLink::moveFirst( pChain ); }; \
	inline bool	moveLast( WgChain<type> * pChain = 0 ) { return WgLink::moveLast( pChain ); }; \
	inline bool	moveAfter( type * pLink ) { return WgLink::moveAfter( pLink ); }; \
	inline bool	moveBefore( type * pLink) { return WgLink::moveBefore( pLink ); }; \
	inline bool	disconnect() {return WgLink::disconnect(); }; \
	inline type *	getPrev() const { return (type *) m_pPrev; }; \
	inline type *	getNext() const { return (type *) m_pNext; }; \
	inline WgChain<type> * getChain() const { return (WgChain<type> *) m_pChain; }; \
	inline Sint32	getIndex() const { return WgLink::getIndex(); };


class WgLink
{
	friend	class WgChainImp;
//	friend	class Chain;

protected:
	WgLink( WgChainImp * pChain = 0 );
	virtual ~WgLink();

	bool		moveUp();						///< Move link one step closer to top.
	bool		moveDown();						///< Move link one step closer to bottom.
	bool		moveFirst( WgChainImp * pChain = 0 );///< Move link to top of current or specified chain.
	bool		moveLast( WgChainImp * pChain = 0 ); ///< Move link to bottom of current or specified chain.
	bool		moveAfter( WgLink * pLink );		///< Move link to position right after specified link in specified links chain.
	bool		moveBefore( WgLink * pLink );		///< Move link to position right before specified link in specified links chain.

    Sint32		getIndex() const;					///< Get position in chain (0+), -1 if not in a chain.
	bool		disconnect();					///< Disconnects Link from its chain.

protected:
	WgChainImp *	m_pChain;

	WgLink *		m_pNext;
	WgLink *		m_pPrev;

private:
	void		reset();
	void		unlink();

};


//____ WgChainImp::push_front() _______________________________________________

inline void	WgChainImp::push_front( WgLink * pLink )
{
	pLink->moveFirst( this );
}

//____ WgChainImp::pop_front() ________________________________________________

inline WgLink * WgChainImp::pop_front()
{
	WgLink * pLink = m_pFirst;
	if( pLink )
		pLink->disconnect();
	return pLink;
}

//____ WgChainImp::push_back() ________________________________________________

inline void	WgChainImp::push_back( WgLink * pLink )
{
	pLink->moveLast( this );
}

//____ WgChainImp::pop_back() _________________________________________________

inline WgLink * WgChainImp::pop_back()
{
	WgLink * pLink = m_pLast;
	if( pLink )
		pLink->disconnect();
	return pLink;
}


//____ WgLink::unlink() _________________________________________________________

inline void WgLink::unlink()
{
	if( m_pPrev )
		m_pPrev->m_pNext = m_pNext;
	else
		m_pChain->m_pFirst = m_pNext;

	if( m_pNext )
		m_pNext->m_pPrev = m_pPrev;
	else
		m_pChain->m_pLast = m_pPrev;

	m_pChain->m_size--;
}

//____ WgLink::reset() __________________________________________________________

inline void WgLink::reset()
{
	m_pChain	= 0;
	m_pPrev		= 0;
	m_pNext		= 0;
}

//______________________________________________________________________________
#endif // WG_CHAIN_DOT_H
