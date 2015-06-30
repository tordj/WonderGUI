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

class WgRootPanel;


class WgMsgRouter;
typedef	WgStrongPtr<WgMsgRouter,WgObjectPtr>		WgMsgRouterPtr;
typedef	WgWeakPtr<WgMsgRouter,WgObjectWeakPtr>	WgMsgRouterWeakPtr;

typedef unsigned int	WgRouteId;

class WgMsgRouter : public WgObject
{
friend class WgWidget;
friend class WgRootPanel;

public:
	static WgMsgRouterPtr	Create() { return WgMsgRouterPtr(new WgMsgRouter()); }

	bool					IsInstanceOf( const char * pClassName ) const;
	const char *			ClassName( void ) const;
	static const char		CLASSNAME[];
	static WgMsgRouterPtr	Cast( const WgObjectPtr& pObject );

	void	SetRoot( const WgRootPanelPtr& pRoot ); // Temporary fix...


	bool	Post( const WgMsgPtr& pMsg );

	void	Dispatch();

	//----

	bool	SetFocusGroup( const WgPanelPtr& pFocusGroup );
	bool	SetKeyboardFocus( const WgWidgetPtr& pFocus );

	WgPanelPtr	FocusGroup() const { return m_keyFocusGroup.RawPtr(); }
	WgWidgetPtr	KeyboardFocus() const { return m_keyFocusWidget.RawPtr(); }

	//----

	bool	IsMouseButtonPressed( int button ) const;
	bool	IsAnyMouseButtonPressed() const;
	bool	IsKeyPressed( int native_keycode ) const;
	WgModifierKeys ModKeys() const { return m_modKeys; }

	bool	IsWindowFocused() const { return m_bWindowFocus; }

	//----

	void	BroadcastTo( const WgReceiverPtr& pReceiver );
	void	BroadcastTo( const WgMsgFilter& filter, const WgReceiverPtr& pReceiver );
	bool	EndBroadcast( const WgReceiverPtr& pReceiver );
	
	WgRouteId	AddRoute( const WgObjectPtr& pSource, const WgReceiverPtr& pReceiver );
	WgRouteId	AddRoute( const WgMsgFilter& filter, const WgObjectPtr& pSource, const WgReceiverPtr& pReceiver );

	void	AddTickReceiver( const WgReceiverPtr& pReceiver );
	bool	RemoveTickReceiver( const WgReceiverPtr& pReceiver );



	bool	DeleteRoute( WgRouteId handle );
	int		DeleteRoutesTo( const WgReceiverPtr& pReceiver );
	int		DeleteRoutesFrom( const WgObjectPtr& pSource );

	int		ClearRoutes();
	int		GarbageCollectRoutes();

	//----

private:
	WgMsgRouter();
	~WgMsgRouter();

	class	Route;

	void 	_postTickMsgs( int ticks );
	void 	_dispatchQueued();


	void	_finalizeMsg( const WgMsgPtr& pMsg );
	void	_processGeneralMsg( const WgMsgPtr& pMsg );
	void	_broadcast( const WgMsgPtr& pMsg );
	void	_dispatchToRoutes( const WgMsgPtr& pMsg );

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

	bool	_isReceiverInList( const WgReceiver * pWidget, const std::vector<WgReceiverWeakPtr>& list );
	int		_widgetPosInList( const WgWidget * pWidget, const std::vector<WgWidgetWeakPtr>& list );

	WgRouteId	_addRoute( const WgObjectPtr& pSource, Route * pRoute );

	void 		_updateMarkedWidget(bool bPostMouseMoveMsgs);
	WgWidget *	_updateEnteredWidgets( WgWidget * pMarkedWidget );

	void	_setWidgetFocused( WgWidget * pWidget, bool bFocused );


	//

	WgRootPanelWeakPtr	m_pRoot;

	std::deque<WgMsgPtr>					m_msgQueue;
	bool									m_bIsProcessing;		// Set when we are inside Dispatch().
	std::deque<WgMsgPtr>::iterator			m_insertPos;			// Position where we insert messages being queued when processing.

	int64_t			m_time;
	WgCoord			m_pointerPos;
	WgPointerStyle	m_pointerStyle;
	WgModifierKeys	m_modKeys;

	// Current mouse state

	WgWidgetWeakPtr					m_pMarkedWidget;	// Widget the pointer currently is "inside". Empty if outside a modal widget.

	std::vector<WgWidgetWeakPtr>	m_vEnteredWidgets;	// All widgets that pointer is considered to be inside (= markedWidget + its ancestors).


	// Current button states

	bool						m_bButtonPressed[WG_MAX_BUTTONS+1];

	WgMousePressMsgPtr			m_pLatestPressMsgs[WG_MAX_BUTTONS+1];			// Saved info for the last time each button was pressed.
	WgMouseReleaseMsgPtr		m_pLatestReleaseMsgs[WG_MAX_BUTTONS+1];	// Saved info for the last time each button was released.

	WgWidgetWeakPtr				m_latestPressWidgets[WG_MAX_BUTTONS+1];		// Widget that received the latest press, for each button.
	WgWidgetWeakPtr				m_previousPressWidgets[WG_MAX_BUTTONS+1];	// Widget that received the second latest press, for each button,
																			// used for double-click handling.

	// Current keyboard state

	struct KeyDownInfo
	{
		WgKeyPressMsgPtr 		pMsg;
		WgWidgetWeakPtr			pWidget;					// Widget that received the press event.
	};

	std::vector<KeyDownInfo*>	m_keysDown;				// One entry for each currently depressed key, in order of being pressed.

	bool						m_bWindowFocus;			// Set if we have window focus.
	WgPanelWeakPtr				m_keyFocusGroup;		// Current focus group (if any).
	WgWidgetWeakPtr				m_keyFocusWidget;		// Widget currently having the keyboard focus.

	std::map<WgWidgetWeakPtr,WgWidgetWeakPtr>	m_focusGroupMap;	// Mapping focus groups (key) with their currently focused Widget (value).



	class Route : public WgLink
	{
	friend class WgMsgRouter;
	public:
		Route( const WgMsgFilter& filter, WgReceiver * pReceiver );
		virtual ~Route();

		LINK_METHODS(Route);

		void 	Dispatch( const WgMsgPtr& pMsg );
		bool 	IsAlive() const;
		WgReceiver *	Receiver() const;
		inline const WgMsgFilter& 	Filter() const { return m_filter; }

	protected:
		WgMsgFilter			m_filter;
		WgRouteId			m_handle;
		WgReceiverWeakPtr	m_pReceiver;
	};


	WgRouteId		m_routeCounter;					// Increment by one for each new callbackHandle, gives unique IDs.

	WgChain<Route>					m_broadcasts;
	std::vector<WgReceiverWeakPtr>	m_vTickReceivers;		// Receivers that have requested periodic tick-events (i.e. on every processMsgs() ).

	std::map<WgObjectWeakPtr,WgChain<Route> >	m_routes;

};


#endif //WG_MSGROUTER_DOT_H
