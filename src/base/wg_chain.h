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

namespace wg 
{
	
	class WgLink;
	
	//____ Class WgChainImp _________________________________________________________
	
	class WgChainImp
	{
		friend	class WgLink;
	
	protected:
		WgChainImp();
		virtual ~WgChainImp();
	
		bool	isMemberOf( WgLink * pLink ) const;		///< Check if specified link is a member.
		void	clear();
	
		void	pushFront( WgLink * pLink );		///< Push link onto top of chain.
		WgLink* popFront();									///< Retrieve link from top of chain (disconnects it).
	
		void	pushBack( WgLink * pLink );		///< Add link to end of chain.
		WgLink* popBack();									///< Retrieve link from end of chain (disconnects it).
	
		WgLink* get( int index ) const;
	
		/*
		void	pushBack( WgChainImp * pChain );	///< Push all links from specified chain to back of us.
		void	pushFront( WgChainImp * pChain );	///< Push all links from specified chain to our front.
	
		void	pushBack( WgChainImp * pChain, WgLink * pBeg, WgLink * pEnd );		///< Push all links from specified chain to back of us.
		void	pushFront( WgChainImp * pChain, WgLink * pBeg, WgLink * pEnd );	///< Push all links from specified chain to our front.
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
		inline bool	isMemberOf( T* pLink ) const { return WgChainImp::isMemberOf(pLink); };
		inline void	clear() { WgChainImp::clear(); };
	
		inline T*		first() const { return (T*) m_pFirst; };
		inline T*		last() const { return (T*) m_pLast; };
	
		inline void		pushFront( T* pLink ) { WgChainImp::pushFront( pLink ); };
		inline T*		popFront() { return (T*) WgChainImp::popFront(); };
	
		inline void		pushBack( T* pLink ) { WgChainImp::pushBack( pLink ); };
		inline T*		popBack() { return (T*) WgChainImp::popBack(); };
	
		inline T*		get( int index ) const { return (T*) WgChainImp::get(index); };
	
		inline int		size() const { return m_size; }
	
		inline int		isEmpty() const { return (m_size==0); }
	};
	
	
	
	//____ Class WgLink _____________________________________________________________
	
	#define	LINK_METHODS( type ) \
		inline bool	moveUp() { return WgLink::_moveUp(); }; \
		inline bool	moveDown() { return WgLink::_moveDown(); }; \
		inline bool	moveFirst( WgChain<type> * pChain = 0 ) { return WgLink::_moveFirst( pChain ); }; \
		inline bool	moveLast( WgChain<type> * pChain = 0 ) { return WgLink::_moveLast( pChain ); }; \
		inline bool	moveAfter( type * pLink ) { return WgLink::_moveAfter( pLink ); }; \
		inline bool	moveBefore( type * pLink) { return WgLink::_moveBefore( pLink ); }; \
		inline bool	disconnect() {return WgLink::_disconnect(); }; \
		inline type *	prev() const { return (type *) m_pPrev; }; \
		inline type *	next() const { return (type *) m_pNext; }; \
		inline WgChain<type> * chain() const { return (WgChain<type> *) m_pChain; }; \
		inline int		index() const { return WgLink::_index(); };
	
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
	
	
	//____ WgChainImp::pushFront() _______________________________________________
	
	inline void	WgChainImp::pushFront( WgLink * pLink )
	{
		pLink->_moveFirst( this );
	}
	
	//____ WgChainImp::popFront() ________________________________________________
	
	inline WgLink * WgChainImp::popFront()
	{
		WgLink * pLink = m_pFirst;
		if( pLink )
			pLink->_disconnect();
		return pLink;
	}
	
	//____ WgChainImp::pushBack() ________________________________________________
	
	inline void	WgChainImp::pushBack( WgLink * pLink )
	{
		pLink->_moveLast( this );
	}
	
	//____ WgChainImp::popBack() _________________________________________________
	
	inline WgLink * WgChainImp::popBack()
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

} // namespace wg
#endif // WG_CHAIN_DOT_H
