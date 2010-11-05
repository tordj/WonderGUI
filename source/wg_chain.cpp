//-----------------------------------------------------------------------------
//
//								>>> wg_chain.cpp <<<
//
//					(c) 2002 Tord Jansson (tord.jansson@gmail.com)
//
//	Simple double-linked list implementation, done *my* way!
//
//	This source code may be freely copied, modifed and used in any software
//  project, both free (as in speech) and proprietary. Commercial use and/or
//  distribution does not require any license.
//
//-----------------------------------------------------------------------------

#include	<wg_chain.h>


//-----------------------------------------------------------------------------
WgChainImp::WgChainImp()
{
	m_pFirst = 0;
	m_pLast = 0;
	m_size = 0;
}

//-----------------------------------------------------------------------------
WgChainImp::~WgChainImp()
{
	Clear();
}


//-----------------------------------------------------------------------------
WgLink* WgChainImp::Get( int index ) const
{
	if( index >= m_size )
		return 0;

	WgLink * pLink = m_pFirst;

	for( int i = 0 ; i < index ; i++ )
		pLink = pLink->m_pNext;

	return pLink;
}


//-----------------------------------------------------------------------------
bool WgChainImp::IsMemberOf( WgLink * _pLink ) const
{
	return _pLink != 0 && _pLink->m_pChain == this;
}

//-----------------------------------------------------------------------------
void WgChainImp::Clear()
{
	while( m_pFirst )
		delete	m_pFirst;

	m_size = 0;
}



//-----------------------------------------------------------------------------
WgLink::WgLink( WgChainImp * pChain )
{
	Reset();
	if( pChain )
		MoveFirst( pChain );
}

//-----------------------------------------------------------------------------
WgLink::~WgLink()
{
	Disconnect();
}

//-----------------------------------------------------------------------------
int WgLink::Index() const
{
	if( !m_pChain )
		return -1;

	WgLink * pLink = m_pChain->m_pFirst;
	Sint32	index = 0;

	while( pLink != this )
	{
		pLink = pLink->m_pNext;
		index++;
	}
	return index;
}

//-----------------------------------------------------------------------------
bool WgLink::MoveUp()
{
	if( !m_pPrev )
		return false;

	m_pPrev->m_pNext = m_pNext;
	if( m_pNext )
		m_pNext->m_pPrev = m_pPrev;
	else
		m_pChain->m_pLast = m_pPrev;

	m_pNext = m_pPrev;
	m_pPrev = m_pPrev->m_pPrev;

	m_pNext->m_pPrev = this;

	if( m_pPrev )
		m_pPrev->m_pNext = this;
	else
		m_pChain->m_pFirst = this;

	return true;
}

//-----------------------------------------------------------------------------
bool WgLink::MoveDown()
{
	if( !m_pNext )
		return false;

	m_pNext->m_pPrev = m_pPrev;
	if( m_pPrev )
		m_pPrev->m_pNext = m_pNext;
	else
		m_pChain->m_pFirst = m_pNext;

	m_pPrev = m_pNext;
	m_pNext = m_pNext->m_pNext;

	m_pPrev->m_pNext = this;

	if( m_pNext )
		m_pNext->m_pPrev = this;
	else
		m_pChain->m_pLast = this;

	return true;
}

//-----------------------------------------------------------------------------
bool WgLink::MoveFirst( WgChainImp * pChain )
{
	if( !pChain )
	{
		if( !m_pChain )
			return false;
		pChain = m_pChain;
	}

	if( m_pChain )
		Unlink();

	m_pChain = pChain;
	m_pPrev = 0;
	m_pNext = pChain->m_pFirst;

	pChain->m_pFirst = this;
	if( m_pNext )
		m_pNext->m_pPrev = this;
	else
		pChain->m_pLast = this;

	m_pChain->m_size++;
	return true;
}

//-----------------------------------------------------------------------------
bool WgLink::MoveLast( WgChainImp * pChain )
{
	if( !pChain )
	{
		if( !m_pChain )
			return false;
		pChain = m_pChain;
	}

	if( m_pChain )
		Unlink();

	m_pChain = pChain;
	m_pNext = 0;
	m_pPrev = pChain->m_pLast;

	pChain->m_pLast = this;
	if( m_pPrev )
		m_pPrev->m_pNext = this;
	else
		pChain->m_pFirst = this;

	m_pChain->m_size++;
	return true;
}

//-----------------------------------------------------------------------------
bool WgLink::MoveAfter( WgLink * pLink )
{
	if( !pLink || !pLink->m_pChain )
		return false;

	if( pLink == this )
		return true;

	if( m_pChain )
		Unlink();

	m_pChain = pLink->m_pChain;
	m_pNext = pLink->m_pNext;
	m_pPrev = pLink;

	pLink->m_pNext = this;
	if( m_pNext )
		m_pNext->m_pPrev = this;
	else
	{
		m_pNext = 0;
		m_pChain->m_pLast = this;
	}

	m_pChain->m_size++;
	return true;
}

//-----------------------------------------------------------------------------
bool WgLink::MoveBefore( WgLink * pLink )
{
	if( !pLink || !pLink->m_pChain )
		return false;

	if( pLink == this )
		return true;

	if( m_pChain )
		Unlink();

	m_pChain = pLink->m_pChain;
	m_pNext = pLink;

	if( pLink->m_pPrev )
	{
		m_pPrev = pLink->m_pPrev;
		pLink->m_pPrev->m_pNext = this;
	}
	else
	{
		m_pPrev = 0;
		m_pChain->m_pFirst = this;
	}
	pLink->m_pPrev = this;
	m_pChain->m_size++;
	return true;
}

//-----------------------------------------------------------------------------
bool WgLink::Disconnect()
{
	if( m_pChain )
	{
		Unlink();
		Reset();
		return true;
	}

	return false;
}
