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
#ifndef WG_MSGROUTER_DOT_H
#define WG_MSGROUTER_DOT_H

#include <deque>
#include <map>
#include <vector>

#ifndef WG_MSG_DOT_H
#	include <wg_msg.h>
#endif

#ifndef WG_PANEL_DOT_H
#	include <wg_panel.h>
#endif

#ifndef WG_KEY_DOT_H
#	include <wg_key.h>
#endif

namespace wg 
{
	
	class RootPanel;
	
	
	class MsgRouter;
	typedef	StrongPtr<MsgRouter,Object_p>		MsgRouter_p;
	typedef	WeakPtr<MsgRouter,Object_wp>	MsgRouter_wp;
	
	class MsgRouter : public Object
	{
	friend class Widget;
	friend class RootPanel;
	
	public:
		static MsgRouter_p	create() { return MsgRouter_p(new MsgRouter()); }
	
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static MsgRouter_p	cast( const Object_p& pObject );
	
		bool		post( const Msg_p& pMsg );
	
		void		dispatch();
		
		//----
	
		bool		broadcastTo( const Receiver_p& pReceiver );
		bool		endBroadcast( const Receiver_p& pReceiver );
		
		RouteId		addRoute( const Object_p& pSource, const Receiver_p& pReceiver );
		RouteId		addRoute( const Object_p& pSource, MsgType filter, const Receiver_p& pReceiver );
		RouteId		addRoute( MsgType type, const Receiver_p& pReceiver );
	
		RouteId		addRoute( MsgType type, Receiver * pReceiver );		// For calls from constructors.
	
		bool		deleteRoute( RouteId handle );
		int			deleteRoutesTo( const Receiver_p& pReceiver );
		int			deleteRoutesFrom( const Object_p& pSource );
		int			deleteRoutesFrom( MsgType type );
	
		int			clearRoutes();
		int			garbageCollectRoutes();
	
		//----
	
	private:
		MsgRouter();
		~MsgRouter();
	
		class	Route;
	
		void 		_dispatchQueued();
	
	
		void		_broadcast( const Msg_p& pMsg );
		void		_dispatchToSourceRoutes( const Msg_p& pMsg );
		void		_dispatchToTypeRoutes( const Msg_p& pMsg );
			
		RouteId	_addRoute( const Object_p& pSource, Route * pRoute );
		RouteId	_addRoute( MsgType type, Route * pRoute );
	
		//
			
		class Route : public Link
		{
		friend class MsgRouter;
		public:
			Route( Receiver * pReceiver, MsgType filter );
			virtual ~Route();
	
			LINK_METHODS(Route);
	
			void 	dispatch( const Msg_p& pMsg );
			bool 	isAlive() const;
			Receiver *	receiver() const;
	
		protected:
			RouteId				m_handle;
			Receiver_wp			m_pReceiver;
			MsgType				m_filter;					// Filter for dispatching. Message needs to be the same if filter != Dummy
		};
	
		std::deque<Msg_p>			m_msgQueue;
		bool						m_bIsProcessing;		// Set when we are inside dispatch().
		std::deque<Msg_p>::iterator	m_insertPos;			// Position where we insert messages being queued when processing.
	
		RouteId				m_routeCounter;					// Increment by one for each new callbackHandle, gives unique IDs.
		Chain<Route>			m_broadcasts;
	
		std::map<Object_wp,Chain<Route> >	m_sourceRoutes;
		std::map<MsgType,Chain<Route> >		m_typeRoutes;
	};
	
	

} // namespace wg
#endif //WG_MSGROUTER_DOT_H
