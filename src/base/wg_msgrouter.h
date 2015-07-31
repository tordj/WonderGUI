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
	
	class WgRootPanel;
	
	
	class WgMsgRouter;
	typedef	WgStrongPtr<WgMsgRouter,WgObject_p>		WgMsgRouter_p;
	typedef	WgWeakPtr<WgMsgRouter,WgObject_wp>	WgMsgRouter_wp;
	
	class WgMsgRouter : public WgObject
	{
	friend class WgWidget;
	friend class WgRootPanel;
	
	public:
		static WgMsgRouter_p	create() { return WgMsgRouter_p(new WgMsgRouter()); }
	
		bool					isInstanceOf( const char * pClassName ) const;
		const char *			className( void ) const;
		static const char		CLASSNAME[];
		static WgMsgRouter_p	cast( const WgObject_p& pObject );
	
		void	setRoot( const WgRootPanel_p& pRoot ); // Temporary fix...
	
	
		bool	post( const WgMsg_p& pMsg );
	
		void	dispatch();
	
		//----
	
		bool	setFocusGroup( const WgPanel_p& pFocusGroup );
		bool	setKeyboardFocus( const WgWidget_p& pFocus );
	
		WgPanel_p	focusGroup() const { return m_keyFocusGroup.rawPtr(); }
		WgWidget_p	keyboardFocus() const { return m_keyFocusWidget.rawPtr(); }
	
		//----
	
		bool	isMouseButtonPressed( int button ) const;
		bool	isAnyMouseButtonPressed() const;
		bool	isKeyPressed( int native_keycode ) const;
		WgModifierKeys modKeys() const { return m_modKeys; }
	
		bool	isWindowFocused() const { return m_bWindowFocus; }
	
		//----
	
		bool	broadcastTo( const WgReceiver_p& pReceiver );
		bool	broadcastTo( const WgMsgFilter& filter, const WgReceiver_p& pReceiver );
		bool	endBroadcast( const WgReceiver_p& pReceiver );
		
		WgRouteId	addRoute( const WgObject_p& pSource, const WgReceiver_p& pReceiver );
		WgRouteId	addRoute( const WgMsgFilter& filter, const WgObject_p& pSource, const WgReceiver_p& pReceiver );
	
		WgRouteId	addRoute( WgMsgType type, const WgReceiver_p& pReceiver );
		WgRouteId	addRoute( const WgMsgFilter& filter, WgMsgType type, const WgReceiver_p& pReceiver );
	
		WgRouteId	addRoute( WgMsgType type, WgReceiver * pReceiver );		// For calls from constructors.
	
	
		bool	deleteRoute( WgRouteId handle );
		int		deleteRoutesTo( const WgReceiver_p& pReceiver );
		int		deleteRoutesFrom( const WgObject_p& pSource );
		int		deleteRoutesFrom( WgMsgType type );
	
		int		clearRoutes();
		int		garbageCollectRoutes();
	
		//----
	
	private:
		WgMsgRouter();
		~WgMsgRouter();
	
		class	Route;
	
		void 	_dispatchQueued();
	
	
		void	_finalizeMsg( const WgMsg_p& pMsg );
		void	_processGeneralMsg( const WgMsg_p& pMsg );
		void	_broadcast( const WgMsg_p& pMsg );
		void	_dispatchToSourceRoutes( const WgMsg_p& pMsg );
		void	_dispatchToTypeRoutes( const WgMsg_p& pMsg );
	
		void	_processTick( WgTickMsg * pMsg );
	
		void	_processFocusGained( WgFocusGainedMsg * pMsg );
		void	_processFocusLost( WgFocusLostMsg * pMsg );
	
		void	_processMouseEnter( WgMouseEnterMsg * pMsg );
		void	_processMouseMove( WgMouseMoveMsg * pMsg );
		void	_processMousePosition( WgMousePositionMsg * pMsg );
		void	_processMouseLeave( WgMouseLeaveMsg * pMsg );
	
		void	_processMouseButtonPress( WgMousePressMsg * pMsg );
		void	_processMouseButtonRepeat( WgMouseRepeatMsg * pMsg );
		void	_processMouseButtonDrag( WgMouseDragMsg * pMsg );
		void	_processMouseButtonRelease( WgMouseReleaseMsg * pMsg );
		void	_processMouseButtonClick( WgMouseClickMsg * pMsg );
		void	_processMouseButtonDoubleClick( WgMouseDoubleClickMsg * pMsg );
	
		void	_processMouseWheelRoll( WgWheelRollMsg * pMsg );
	
		void	_processKeyPress( WgKeyPressMsg * pMsg );
		void	_processKeyRepeat( WgKeyRepeatMsg * pMsg );
		void	_processKeyRelease( WgKeyReleaseMsg * pMsg );
	
		void	_processCharacter( WgCharacterMsg * pMsg );
	
		int		_widgetPosInList( const WgWidget * pWidget, const std::vector<WgWidget_wp>& list );
	
		WgRouteId	_addRoute( const WgObject_p& pSource, Route * pRoute );
		WgRouteId	_addRoute( WgMsgType type, Route * pRoute );
	
		void 		_updateMarkedWidget(bool bPostMouseMoveMsgs);
		WgWidget *	_updateEnteredWidgets( WgWidget * pMarkedWidget );
	
		void	_setWidgetFocused( WgWidget * pWidget, bool bFocused );
	
	
		//
	
		WgRootPanel_wp	m_pRoot;
	
		std::deque<WgMsg_p>			m_msgQueue;
		bool							m_bIsProcessing;		// Set when we are inside dispatch().
		std::deque<WgMsg_p>::iterator	m_insertPos;			// Position where we insert messages being queued when processing.
	
		int64_t			m_time;
		WgCoord			m_pointerPos;
		WgPointerStyle	m_pointerStyle;
		WgModifierKeys	m_modKeys;
	
		// Current mouse state
	
		WgWidget_wp					m_pMarkedWidget;	// Widget the pointer currently is "inside". Empty if outside a modal widget.
	
		std::vector<WgWidget_wp>	m_vEnteredWidgets;	// All widgets that pointer is considered to be inside (= markedWidget + its ancestors).
	
	
		// Current button states
	
		bool						m_bButtonPressed[WG_MAX_BUTTONS+1];
	
		WgMousePressMsg_p			m_pLatestPressMsgs[WG_MAX_BUTTONS+1];			// Saved info for the last time each button was pressed.
		WgMouseReleaseMsg_p		m_pLatestReleaseMsgs[WG_MAX_BUTTONS+1];	// Saved info for the last time each button was released.
	
		WgWidget_wp				m_latestPressWidgets[WG_MAX_BUTTONS+1];		// Widget that received the latest press, for each button.
		WgWidget_wp				m_previousPressWidgets[WG_MAX_BUTTONS+1];	// Widget that received the second latest press, for each button,
																				// used for double-click handling.
	
		// Current keyboard state
	
		struct KeyDownInfo
		{
			WgKeyPressMsg_p 		pMsg;
			WgWidget_wp			pWidget;					// Widget that received the press event.
		};
	
		std::vector<KeyDownInfo*>	m_keysDown;				// One entry for each currently depressed key, in order of being pressed.
	
		bool						m_bWindowFocus;			// Set if we have window focus.
		WgPanel_wp				m_keyFocusGroup;		// Current focus group (if any).
		WgWidget_wp				m_keyFocusWidget;		// Widget currently having the keyboard focus.
	
		std::map<WgWidget_wp,WgWidget_wp>	m_focusGroupMap;	// Mapping focus groups (key) with their currently focused Widget (value).
	
	
	
		class Route : public WgLink
		{
		friend class WgMsgRouter;
		public:
			Route( const WgMsgFilter& filter, WgReceiver * pReceiver );
			virtual ~Route();
	
			LINK_METHODS(Route);
	
			void 	dispatch( const WgMsg_p& pMsg );
			bool 	isAlive() const;
			WgReceiver *	receiver() const;
			inline const WgMsgFilter& 	filter() const { return m_filter; }
	
		protected:
			WgMsgFilter			m_filter;
			WgRouteId			m_handle;
			WgReceiver_wp	m_pReceiver;
		};
	
	
		WgRouteId				m_routeCounter;					// Increment by one for each new callbackHandle, gives unique IDs.
		WgChain<Route>			m_broadcasts;
	
		std::map<WgObject_wp,WgChain<Route> >	m_sourceRoutes;
		std::map<WgMsgType,WgChain<Route> >			m_typeRoutes;
	};
	
	

} // namespace wg
#endif //WG_MSGROUTER_DOT_H
