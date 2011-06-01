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
	inline bool	MoveUp() { return WgLink::_moveUp(); }; \
	inline bool	MoveDown() { return WgLink::_moveDown(); }; \
	inline bool	MoveFirst( WgChain<type> * pChain = 0 ) { return WgLink::_moveFirst( pChain ); }; \
	inline bool	MoveLast( WgChain<type> * pChain = 0 ) { return WgLink::_moveLast( pChain ); }; \
	inline bool	MoveAfter( type * pLink ) { return WgLink::_moveAfter( pLink ); }; \
	inline bool	MoveBefore( type * pLink) { return WgLink::_moveBefore( pLink ); }; \
	inline bool	Disconnect() {return WgLink::_disconnect(); }; \
	inline type *	Prev() const { return (type *) m_pPrev; }; \
	inline type *	Next() const { return (type *) m_pNext; }; \
	inline WgChain<type> * Chain() const { return (WgChain<type> *) m_pChain; }; \
	inline int		Index() const { return WgLink::_index(); };

#define	PROTECTED_LINK_METHODS( type ) \
	inline bool	_moveUp() { return WgLink::_moveUp(); }; \
	inline bool	_moveDown() { return WgLink::_moveDown(); }; \
	inline bool	_moveFirst( WgChain<type> * pChain = 0 ) { return WgLink::_moveFirst( pChain ); }; \
	inline bool	_moveLast( WgChain<type> * pChain = 0 ) { return WgLink::_moveLast( pChain ); }; \
	inline bool	_moveAfter( type * pLink ) { return WgLink::_moveAfter( pLink ); }; \
	inline bool	_moveBefore( type * pLink) { return WgLink::_moveBefore( pLink ); }; \
	inline bool	_disconnect() {return WgLink::_disconnect(); }; \
	inline type *	_prev() const { return (type *) m_pPrev; }; \
	inline type *	_next() const { return (type *) m_pNext; }; \
	inline WgChain<type> * _chain() const { return (WgChain<type> *) m_pChain; }; \
	inline int		_index() const { return WgLink::_index(); };



class WgLink
{
	friend	class WgChainImp;
//	friend	class Chain;

protected:
	WgLink( WgChainImp * pChain = 0 );
	virtual ~WgLink();

	bool		_moveUp();						///< Move link one step closer to top.
	bool		_moveDown();						///< Move link one step closer to bottom.
	bool		_moveFirst( WgChainImp * pChain = 0 );///< Move link to top of current or specified chain.
	bool		_moveLast( WgChainImp * pChain = 0 ); ///< Move link to bottom of current or specified chain.
	bool		_moveAfter( WgLink * pLink );		///< Move link to position right after specified link in specified links chain.
	bool		_moveBefore( WgLink * pLink );		///< Move link to position right before specified link in specified links chain.

    int			_index() const;					///< Get position in chain (0+), -1 if not in a chain.
	bool		_disconnect();					///< Disconnects Link from its chain.

protected:
	WgChainImp *	m_pChain;

	WgLink *		m_pNext;
	WgLink *		m_pPrev;

private:
	void		_reset();
	void		_unlink();

};


//____ WgChainImp::PushFront() _______________________________________________

inline void	WgChainImp::PushFront( WgLink * pLink )
{
	pLink->_moveFirst( this );
}

//____ WgChainImp::PopFront() ________________________________________________

inline WgLink * WgChainImp::PopFront()
{
	WgLink * pLink = m_pFirst;
	if( pLink )
		pLink->_disconnect();
	return pLink;
}

//____ WgChainImp::PushBack() ________________________________________________

inline void	WgChainImp::PushBack( WgLink * pLink )
{
	pLink->_moveLast( this );
}

//____ WgChainImp::PopBack() _________________________________________________

inline WgLink * WgChainImp::PopBack()
{
	WgLink * pLink = m_pLast;
	if( pLink )
		pLink->_disconnect();
	return pLink;
}


//____ WgLink::_unlink() _________________________________________________________

inline void WgLink::_unlink()
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

//____ WgLink::_reset() __________________________________________________________

inline void WgLink::_reset()
{
	m_pChain	= 0;
	m_pPrev		= 0;
	m_pNext		= 0;
}

//______________________________________________________________________________
#endif // WG_CHAIN_DOT_H
