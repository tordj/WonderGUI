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
#include <wg_msg.h>
#include <wg_msgrouter.h>
#include <wg_base.h>
#include <wg_rootpanel.h>
#include <wg_panel.h>

namespace wg 
{
	
	const char MsgRouter::CLASSNAME[] = {"MsgRouter"};
	
	
	//____ Constructor ____________________________________________________________
	
	MsgRouter::MsgRouter()
	{
		m_bIsProcessing			= false;
		
		m_routeCounter = 1;				// We start on 1
	}
	
	//____ Destructor _____________________________________________________________
	
	MsgRouter::~MsgRouter()
	{
	}
	
	//____ isInstanceOf() _________________________________________________________
	
	bool MsgRouter::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return Object::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * MsgRouter::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	MsgRouter_p MsgRouter::cast( const Object_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return MsgRouter_p( static_cast<MsgRouter*>(pObject.rawPtr()) );
	
		return 0;
	}
	
	//____ broadcastTo() ___________________________________________________________
	
	bool  MsgRouter::broadcastTo( const Receiver_p& pReceiver )
	{
		Route * p = new Route( pReceiver.rawPtr() );
		m_broadcasts.pushBack( p );
		return true;
	}
		
	//____ endBroadcast() __________________________________________________________
	
	bool  MsgRouter::endBroadcast( const Receiver_p& _pReceiver )
	{
		Receiver * pReceiver = _pReceiver.rawPtr();
		Route * p = m_broadcasts.first();
		while( p )
		{
			if( p->receiver() == pReceiver )
			{
				delete p;
				return true;
			}
			
			p = p->next();
		}	
		return false;
	}
	
	
	//____ addRoute() __________________________________________________________
	
	RouteId MsgRouter::addRoute( const Object_p& pSource, const Receiver_p& pReceiver )
	{
		Route * p = new Route( pReceiver.rawPtr() );
		return _addRoute( pSource, p );
	}
	
	
	RouteId MsgRouter::addRoute( MsgType msgType, const Receiver_p& pReceiver )
	{
		Route * p = new Route( pReceiver.rawPtr() );
		return _addRoute( msgType, p );	
	}
	
	RouteId MsgRouter::addRoute( MsgType msgType, Receiver * pReceiver )
	{
		Route * p = new Route( pReceiver );
		return _addRoute( msgType, p );	
	}

	
	//____ deleteRoutesTo() _______________________________________________________
	
	int MsgRouter::deleteRoutesTo( const Receiver_p& _pReceiver )
	{
		Receiver * pReceiver = _pReceiver.rawPtr();
		int nDeleted = 0;
	
		// Delete from source routes
	
		for( auto it = m_sourceRoutes.begin() ; it != m_sourceRoutes.end() ; it++ )
		{
			Route * p = it->second.first();
			while( p )
			{
				if( p->receiver() == pReceiver )
				{
					Route * pNext = p->next();
					delete p;					// Receiver is dead, delete route.
					nDeleted++;
					p = pNext;
				}
				else
					p = p->next();
	
			}
		}
	
		// Delete from type routes
	
		for( auto it = m_typeRoutes.begin() ; it != m_typeRoutes.end() ; it++ )
		{
			Route * p = it->second.first();
			while( p )
			{
				if( p->receiver() == pReceiver )
				{
					Route * pNext = p->next();
					delete p;					// Receiver is dead, delete route.
					nDeleted++;
					p = pNext;
				}
				else
					p = p->next();
			}
		}
	
		return nDeleted;
	}
	
	//____ deleteRoutesFrom() _______________________________________________________
	
	int MsgRouter::deleteRoutesFrom( const Object_p& pSource )
	{
		auto it = m_sourceRoutes.find(Object_wp(pSource.rawPtr()) );
	
		if( it == m_sourceRoutes.end() )
			return 0;
	
		int nDeleted = it->second.size();
		m_sourceRoutes.erase(it);
		return nDeleted;
	}
	
	int MsgRouter::deleteRoutesFrom( MsgType msgType )
	{
		auto it = m_typeRoutes.find(msgType);
	
		if( it == m_typeRoutes.end() )
			return 0;
	
		int nDeleted = it->second.size();
		m_typeRoutes.erase(it);
		return nDeleted;
	}
	
	
	//____ deleteRoute() ______________________________________________________
	
	bool MsgRouter::deleteRoute( RouteId id )
	{
		for( auto it = m_typeRoutes.begin() ; it != m_typeRoutes.end() ; ++it )
		{
			Route * p = it->second.first();
			while( p )
			{
				if( p->m_handle == id )
				{
					delete p;
					return true;
				}
				p = p->next();
			}
		}
	
		for( auto it = m_sourceRoutes.begin() ; it != m_sourceRoutes.end() ; ++it )
		{
			Route * p = it->second.first();
			while( p )
			{
				if( p->m_handle == id )
				{
					delete p;
					return true;
				}
				p = p->next();
			}
		}
	
		return false;
	}
	
	//____ clearRoutes() _______________________________________________________
	
	int MsgRouter::clearRoutes()
	{
		m_sourceRoutes.clear();
		m_typeRoutes.clear();
		return false;
	}
	
	//____ garbageCollectRoutes() __________________________________________________
	
	int MsgRouter::garbageCollectRoutes()
	{
		int nDeleted = 0;
	
		// Delete any dead global callbacks
	
		Route * p = m_broadcasts.first();
		while( p )
		{
			if( p->isAlive() )
				p = p->next();
			else
			{
				Route * pNext = p->next();
				delete p;
				nDeleted++;
				p = pNext;
			}
		}
	
		// Delete any dead source routes.
		// These can be dead by either source or receiver having been deleted.
	
		{
			auto it = m_sourceRoutes.begin();
	
			while( it != m_sourceRoutes.end() )
			{
				if( !it->first )
				{
					nDeleted += it->second.size();
					it = m_sourceRoutes.erase(it);		// Sender is dead, delete whole branch of callbacks.
				}
				else
				{
					Route * p = it->second.first();
					while( p )
					{
						if( p->isAlive() )
							p = p->next();
						else
						{
							Route * pNext = p->next();
							delete p;					// Receiver is dead, delete callback.
							nDeleted++;
							p = pNext;
						}
					}
					++it;
				}
			}
		}
		// Delete any dead type routes.
		// These can be dead by receiver having been deleted.
	
		for( auto it = m_typeRoutes.begin(); it != m_typeRoutes.end() ; it++ )
		{
			Route * p = it->second.first();
			while( p )
			{
				if( p->isAlive() )
					p = p->next();
				else
				{
					Route * pNext = p->next();
					delete p;					// Receiver is dead, delete callback.
					nDeleted++;
					p = pNext;
				}
			}
		}
	
		return nDeleted;
	}
	
	//____ _addRoute() _________________________________________________________
	
	RouteId MsgRouter::_addRoute( const Object_p& pSource, Route * pRoute )
	{
		if( !pSource )
			return 0;
	
		Chain<Route>& chain = m_sourceRoutes[pSource.rawPtr()];
		chain.pushBack(pRoute);
		pRoute->m_handle = m_routeCounter++;
		return pRoute->m_handle;
	}
	
	//____ _addRoute() _________________________________________________________
	
	RouteId MsgRouter::_addRoute( MsgType type, Route * pRoute )
	{
		if( type == MsgType::Dummy || type >= MsgType::Max )
			return 0;
	
		Chain<Route>& chain = m_typeRoutes[type];
		chain.pushBack(pRoute);
		pRoute->m_handle = m_routeCounter++;
		return pRoute->m_handle;
	}
	
	
	//____ post() ___________________________________________________________
	
	bool MsgRouter::post( const Msg_p& pMsg )
	{
		if( m_bIsProcessing )
		{
			// Msgs that are posted as a direct result of another event being processed
			// are inserted before the next event in the queue.
			// If two or more events are posted by the same event being processed,
			// they need to be queued in the order of posting.
	
			m_insertPos = m_msgQueue.insert( m_insertPos, pMsg );
			m_insertPos++;
		}
		else
		{
			// Msgs being posted outside processing loop are simply added to the
			// queue.
	
			m_msgQueue.push_back( pMsg );
		}
	
		return true;
	}
	
	//____ dispatch() ________________________________________________________
	
	void MsgRouter::dispatch()
	{
		m_bIsProcessing = true;
	
		m_insertPos = m_msgQueue.begin();	// Insert any POINTER_ENTER/EXIT right at beginning.
	
		_dispatchQueued();
	
		m_bIsProcessing = false;
	}
	
	
	//____ _dispatchQueued() ___________________________________________________
	
	void MsgRouter::_dispatchQueued()
	{
		while( !m_msgQueue.empty() )
		{
			Msg_p& pMsg = m_msgQueue.front();
			m_insertPos = m_msgQueue.begin()+1;	// Insert position set to right after current event.
		
			do
			{
				if( pMsg->hasCopyTo()  )
					pMsg->getCopyTo()->receive( pMsg );
				
				if( pMsg->hasSource() )
					_dispatchToSourceRoutes( pMsg );
	
				_dispatchToTypeRoutes( pMsg );
				_broadcast( pMsg );			
			}
			while( pMsg->doRepost() );
	
			m_msgQueue.pop_front();
		}
	
		m_insertPos = m_msgQueue.begin();		// Insert position set right to start.
	}
	
	
	//____ _broadcast() ________________________________________________
	
	void MsgRouter::_broadcast( const Msg_p& pMsg )
	{
		Route * pRoute = m_broadcasts.first();
	
		while( pRoute )
		{
			pRoute->dispatch( pMsg );
			pRoute = pRoute->next();
		}
	}
	
	
	//____ _dispatchToTypeRoutes() __________________________________________________
	
	void MsgRouter::_dispatchToTypeRoutes( const Msg_p& pMsg )
	{
		auto it = m_typeRoutes.find(pMsg->type());
		if( it != m_typeRoutes.end() )
		{
			Route * pRoute = it->second.first();
	
			while( pRoute )
			{
				pRoute->dispatch( pMsg );
				pRoute = pRoute->next();
			}
		}	
	}
	
	//____ _dispatchToSourceRoutes() ________________________________________________
	
	void MsgRouter::_dispatchToSourceRoutes( const Msg_p& pMsg )
	{
		Object * pSource = pMsg->sourceRawPtr();
	
		if( pSource )
		{
			auto it = m_sourceRoutes.find(Object_wp(pSource));
			if( it != m_sourceRoutes.end() )
			{
				Route * pRoute = it->second.first();
				while( pRoute )
				{
					pRoute->dispatch( pMsg );
					pRoute = pRoute->next();
				}
			}
		}
	}
		
	
	MsgRouter::Route::Route( Receiver * pReceiver )
	{
		m_pReceiver = pReceiver;
		pReceiver->_onRouteAdded();
	}
	
	MsgRouter::Route::~Route()
	{
		if( m_pReceiver )
			m_pReceiver->_onRouteRemoved();
	}
	
	void MsgRouter::Route::dispatch( const Msg_p& pMsg )
	{
		m_pReceiver->receive( pMsg );
	}
	
	bool MsgRouter::Route::isAlive() const
	{
		return true;
	}
	
	Receiver * MsgRouter::Route::receiver() const
	{
		return m_pReceiver.rawPtr();
	}
	

} // namespace wg
