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

#ifndef WG_MSGFILTER_DOT_H
#	include <wg_msgfilter.h>
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
	typedef	WgStrongPtr<MsgRouter,Object_p>		MsgRouter_p;
	typedef	WgWeakPtr<MsgRouter,Object_wp>	MsgRouter_wp;
	
	class MsgRouter : public Object
	{
	friend class Widget;
	friend class RootPanel;
	
	public:
		static MsgRouter_p	create() { return MsgRouter_p(new MsgRouter()); }
	
		bool					isInstanceOf( const char * pClassName ) const;
		const char *			className( void ) const;
		static const char		CLASSNAME[];
		static MsgRouter_p	cast( const Object_p& pObject );
	
		void	setRoot( const RootPanel_p& pRoot ); // Temporary fix...
	
	
		bool	post( const Msg_p& pMsg );
	
		void	dispatch();
	
		//----
	
		bool	setFocusGroup( const Panel_p& pFocusGroup );
		bool	setKeyboardFocus( const Widget_p& pFocus );
	
		Panel_p	focusGroup() const { return m_keyFocusGroup.rawPtr(); }
		Widget_p	keyboardFocus() const { return m_keyFocusWidget.rawPtr(); }
	
		//----
	
		bool	isMouseButtonPressed( int button ) const;
		bool	isAnyMouseButtonPressed() const;
		bool	isKeyPressed( int native_keycode ) const;
		WgModifierKeys modKeys() const { return m_modKeys; }
	
		bool	isWindowFocused() const { return m_bWindowFocus; }
	
		//----
	
		bool	broadcastTo( const Receiver_p& pReceiver );
		bool	broadcastTo( const MsgFilter& filter, const Receiver_p& pReceiver );
		bool	endBroadcast( const Receiver_p& pReceiver );
		
		WgRouteId	addRoute( const Object_p& pSource, const Receiver_p& pReceiver );
		WgRouteId	addRoute( const MsgFilter& filter, const Object_p& pSource, const Receiver_p& pReceiver );
	
		WgRouteId	addRoute( MsgType type, const Receiver_p& pReceiver );
		WgRouteId	addRoute( const MsgFilter& filter, MsgType type, const Receiver_p& pReceiver );
	
		WgRouteId	addRoute( MsgType type, Receiver * pReceiver );		// For calls from constructors.
	
	
		bool	deleteRoute( WgRouteId handle );
		int		deleteRoutesTo( const Receiver_p& pReceiver );
		int		deleteRoutesFrom( const Object_p& pSource );
		int		deleteRoutesFrom( MsgType type );
	
		int		clearRoutes();
		int		garbageCollectRoutes();
	
		//----
	
	private:
		MsgRouter();
		~MsgRouter();
	
		class	Route;
	
		void 	_dispatchQueued();
	
	
		void	_finalizeMsg( const Msg_p& pMsg );
		void	_processGeneralMsg( const Msg_p& pMsg );
		void	_broadcast( const Msg_p& pMsg );
		void	_dispatchToSourceRoutes( const Msg_p& pMsg );
		void	_dispatchToTypeRoutes( const Msg_p& pMsg );
	
		void	_processTick( TickMsg * pMsg );
	
		void	_processFocusGained( FocusGainedMsg * pMsg );
		void	_processFocusLost( FocusLostMsg * pMsg );
	
		void	_processMouseEnter( MouseEnterMsg * pMsg );
		void	_processMouseMove( MouseMoveMsg * pMsg );
		void	_processMousePosition( MousePositionMsg * pMsg );
		void	_processMouseLeave( MouseLeaveMsg * pMsg );
	
		void	_processMouseButtonPress( MousePressMsg * pMsg );
		void	_processMouseButtonRepeat( MouseRepeatMsg * pMsg );
		void	_processMouseButtonDrag( MouseDragMsg * pMsg );
		void	_processMouseButtonRelease( MouseReleaseMsg * pMsg );
		void	_processMouseButtonClick( MouseClickMsg * pMsg );
		void	_processMouseButtonDoubleClick( MouseDoubleClickMsg * pMsg );
	
		void	_processMouseWheelRoll( WheelRollMsg * pMsg );
	
		void	_processKeyPress( KeyPressMsg * pMsg );
		void	_processKeyRepeat( KeyRepeatMsg * pMsg );
		void	_processKeyRelease( KeyReleaseMsg * pMsg );
	
		void	_processCharacter( CharacterMsg * pMsg );
	
		int		_widgetPosInList( const Widget * pWidget, const std::vector<Widget_wp>& list );
	
		WgRouteId	_addRoute( const Object_p& pSource, Route * pRoute );
		WgRouteId	_addRoute( MsgType type, Route * pRoute );
	
		void 		_updateMarkedWidget(bool bPostMouseMoveMsgs);
		Widget *	_updateEnteredWidgets( Widget * pMarkedWidget );
	
		void	_setWidgetFocused( Widget * pWidget, bool bFocused );
	
	
		//
	
		RootPanel_wp	m_pRoot;
	
		std::deque<Msg_p>			m_msgQueue;
		bool							m_bIsProcessing;		// Set when we are inside dispatch().
		std::deque<Msg_p>::iterator	m_insertPos;			// Position where we insert messages being queued when processing.
	
		int64_t			m_time;
		Coord			m_pointerPos;
		WgPointerStyle	m_pointerStyle;
		WgModifierKeys	m_modKeys;
	
		// Current mouse state
	
		Widget_wp					m_pMarkedWidget;	// Widget the pointer currently is "inside". Empty if outside a modal widget.
	
		std::vector<Widget_wp>	m_vEnteredWidgets;	// All widgets that pointer is considered to be inside (= markedWidget + its ancestors).
	
	
		// Current button states
	
		bool						m_bButtonPressed[WG_MAX_BUTTONS+1];
	
		MousePressMsg_p			m_pLatestPressMsgs[WG_MAX_BUTTONS+1];			// Saved info for the last time each button was pressed.
		MouseReleaseMsg_p		m_pLatestReleaseMsgs[WG_MAX_BUTTONS+1];	// Saved info for the last time each button was released.
	
		Widget_wp				m_latestPressWidgets[WG_MAX_BUTTONS+1];		// Widget that received the latest press, for each button.
		Widget_wp				m_previousPressWidgets[WG_MAX_BUTTONS+1];	// Widget that received the second latest press, for each button,
																				// used for double-click handling.
	
		// Current keyboard state
	
		struct KeyDownInfo
		{
			KeyPressMsg_p 		pMsg;
			Widget_wp			pWidget;					// Widget that received the press event.
		};
	
		std::vector<KeyDownInfo*>	m_keysDown;				// One entry for each currently depressed key, in order of being pressed.
	
		bool						m_bWindowFocus;			// Set if we have window focus.
		Panel_wp				m_keyFocusGroup;		// Current focus group (if any).
		Widget_wp				m_keyFocusWidget;		// Widget currently having the keyboard focus.
	
		std::map<Widget_wp,Widget_wp>	m_focusGroupMap;	// Mapping focus groups (key) with their currently focused Widget (value).
	
	
	
		class Route : public Link
		{
		friend class MsgRouter;
		public:
			Route( const MsgFilter& filter, Receiver * pReceiver );
			virtual ~Route();
	
			LINK_METHODS(Route);
	
			void 	dispatch( const Msg_p& pMsg );
			bool 	isAlive() const;
			Receiver *	receiver() const;
			inline const MsgFilter& 	filter() const { return m_filter; }
	
		protected:
			MsgFilter			m_filter;
			WgRouteId			m_handle;
			Receiver_wp	m_pReceiver;
		};
	
	
		WgRouteId				m_routeCounter;					// Increment by one for each new callbackHandle, gives unique IDs.
		Chain<Route>			m_broadcasts;
	
		std::map<Object_wp,Chain<Route> >	m_sourceRoutes;
		std::map<MsgType,Chain<Route> >			m_typeRoutes;
	};
	
	

} // namespace wg
#endif //WG_MSGROUTER_DOT_H
