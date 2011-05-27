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

	bool	IsMemberOf( WgLink * pLink ) const;		///< Check if specified link is a member.
	void	Clear();

	void	PushFront( WgLink * pLink );		///< Push link onto top of chain.
	WgLink* PopFront();									///< Retrieve link from top of chain (disconnects it).

	void	PushBack( WgLink * pLink );		///< Add link to end of chain.
	WgLink* PopBack();									///< Retrieve link from end of chain (disconnects it).

	WgLink* Get( int index ) const;
	
	/*
	void	PushBack( WgChainImp * pChain );	///< Push all links from specified chain to back of us.
	void	PushFront( WgChainImp * pChain );	///< Push all links from specified chain to our front.

	void	PushBack( WgChainImp * pChain, WgLink * pBeg, WgLink * pEnd );		///< Push all links from specified chain to back of us.
	void	PushFront( WgChainImp * pChain, WgLink * pBeg, WgLink * pEnd );	///< Push all links from specified chain to our front.
*/

protected:
	WgLink *	m_pFirst;
	WgLink *	m_pLast;
	int			m_size;
};



//____ Class WgChain ____________________________________________________________

template<class T>
class WgChain : public WgChainImp
{
public:
	inline bool	IsMemberOf( T* pLink ) const { return WgChainImp::IsMemberOf(pLink); };
	inline void	Clear() { WgChainImp::Clear(); };

	inline T*		First() const { return (T*) m_pFirst; };
	inline T*		Last() const { return (T*) m_pLast; };

	inline void		PushFront( T* pLink ) { WgChainImp::PushFront( pLink ); };
	inline T*		PopFront() { return (T*) WgChainImp::PopFront(); };

	inline void		PushBack( T* pLink ) { WgChainImp::PushBack( pLink ); };
	inline T*		PopBack() { return (T*) WgChainImp::PopBack(); };

	inline T*		Get( int index ) const { return (T*) WgChainImp::Get(index); };

	inline int		Size() const { return m_size; }
};



//____ Class WgLink _____________________________________________________________

#define	LINK_METHODS( type ) \
	inline bool	MoveUp() { return WgLink::MoveUp(); }; \
	inline bool	MoveDown() { return WgLink::MoveDown(); }; \
	inline bool	MoveFirst( WgChain<type> * pChain = 0 ) { return WgLink::MoveFirst( pChain ); }; \
	inline bool	MoveLast( WgChain<type> * pChain = 0 ) { return WgLink::MoveLast( pChain ); }; \
	inline bool	MoveAfter( type * pLink ) { return WgLink::MoveAfter( pLink ); }; \
	inline bool	MoveBefore( type * pLink) { return WgLink::MoveBefore( pLink ); }; \
	inline bool	Disconnect() {return WgLink::Disconnect(); }; \
	inline type *	Prev() const { return (type *) m_pPrev; }; \
	inline type *	Next() const { return (type *) m_pNext; }; \
	inline WgChain<type> * Chain() const { return (WgChain<type> *) m_pChain; }; \
	inline int		Index() const { return WgLink::Index(); };


class WgLink
{
	friend	class WgChainImp;
//	friend	class Chain;

protected:
	WgLink( WgChainImp * pChain = 0 );
	virtual ~WgLink();

	bool		MoveUp();						///< Move link one step closer to top.
	bool		MoveDown();						///< Move link one step closer to bottom.
	bool		MoveFirst( WgChainImp * pChain = 0 );///< Move link to top of current or specified chain.
	bool		MoveLast( WgChainImp * pChain = 0 ); ///< Move link to bottom of current or specified chain.
	bool		MoveAfter( WgLink * pLink );		///< Move link to position right after specified link in specified links chain.
	bool		MoveBefore( WgLink * pLink );		///< Move link to position right before specified link in specified links chain.

    int			Index() const;					///< Get position in chain (0+), -1 if not in a chain.
	bool		Disconnect();					///< Disconnects Link from its chain.

protected:
	WgChainImp *	m_pChain;

	WgLink *		m_pNext;
	WgLink *		m_pPrev;

private:
	void		Reset();
	void		Unlink();

};


//____ WgChainImp::PushFront() _______________________________________________

inline void	WgChainImp::PushFront( WgLink * pLink )
{
	pLink->MoveFirst( this );
}

//____ WgChainImp::PopFront() ________________________________________________

inline WgLink * WgChainImp::PopFront()
{
	WgLink * pLink = m_pFirst;
	if( pLink )
		pLink->Disconnect();
	return pLink;
}

//____ WgChainImp::PushBack() ________________________________________________

inline void	WgChainImp::PushBack( WgLink * pLink )
{
	pLink->MoveLast( this );
}

//____ WgChainImp::PopBack() _________________________________________________

inline WgLink * WgChainImp::PopBack()
{
	WgLink * pLink = m_pLast;
	if( pLink )
		pLink->Disconnect();
	return pLink;
}


//____ WgLink::Unlink() _________________________________________________________

inline void WgLink::Unlink()
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

//____ WgLink::Reset() __________________________________________________________

inline void WgLink::Reset()
{
	m_pChain	= 0;
	m_pPrev		= 0;
	m_pNext		= 0;
}

//______________________________________________________________________________
#endif // WG_CHAIN_DOT_H
