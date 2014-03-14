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
#ifndef WG_EVENTHANDLER_DOT_H
#define WG_EVENTHANDLER_DOT_H

#include <deque>
#include <map>
#include <vector>

#ifndef WG_EVENT_DOT_H
#	include <wg_event.h>
#endif

#ifndef WG_EVENTFILTER_DOT_H
#	include <wg_eventfilter.h>
#endif

#ifndef WG_PANEL_DOT_H
#	include <wg_panel.h>
#endif

#ifndef WG_KEY_DOT_H
#	include <wg_key.h>
#endif

class WgRootPanel;

class WgEventListener
{
public:
	virtual ~WgEventListener() {};

	virtual void OnEvent( const WgEventPtr& pEvent ) = 0;
};


class WgEventHandler;
typedef	WgSmartPtr<WgEventHandler,WgObjectPtr>		WgEventHandlerPtr;
typedef	WgWeakPtr<WgEventHandler,WgObjectWeakPtr>	WgEventHandlerWeakPtr;

typedef unsigned int	WgCallbackHandle;

class WgEventHandler : public WgObject
{
friend class WgWidget;
friend class WgRootPanel;

public:
	static WgEventHandlerPtr	Create(const WgRootPanelPtr& pRoot) { return WgEventHandlerPtr(new WgEventHandler(pRoot.GetRealPtr())); }

	bool						IsInstanceOf( const char * pClassName ) const;
	const char *				ClassName( void ) const;
	static const char			CLASSNAME[];
	static WgEventHandlerPtr	Cast( const WgObjectPtr& pObject );


	bool	QueueEvent( const WgEventPtr& pEvent );

	void	ProcessEvents();

	bool	SwallowEvent( const WgEventPtr& pEvent );
	bool	ForwardEvent( const WgEventPtr& pEvent, const WgWidgetPtr& pWidget );

	//----

	bool	SetFocusGroup( const WgPanelPtr& pFocusGroup );
	bool	SetKeyboardFocus( const WgWidgetPtr& pFocus );

	WgPanelPtr	FocusGroup() const { return m_keyFocusGroup.GetRealPtr(); }
	WgWidgetPtr	KeyboardFocus() const { return m_keyFocusWidget.GetRealPtr(); }

	//----

	bool	IsMouseButtonPressed( int button );
	bool	IsKeyPressed( int native_keycode );
	WgModifierKeys ModKeys() const { return m_modKeys; }

	bool	IsWindowFocused() const { return m_bWindowFocus; }

	//----

	WgCallbackHandle	AddCallback( void(*fp)( const WgEventPtr& pEvent) );
	WgCallbackHandle	AddCallback( void(*fp)( const WgEventPtr& pEvent, int param), int param );
	WgCallbackHandle	AddCallback( void(*fp)( const WgEventPtr& pEvent, void * pParam), void * pParam );
	WgCallbackHandle	AddCallback( void(*fp)( const WgEventPtr& pEvent, const WgObjectPtr& pParam), const WgObjectPtr& pParam );
	WgCallbackHandle	AddCallback( WgEventListener * pListener );

	WgCallbackHandle	AddCallback( const WgEventFilter& filter, void(*fp)( const WgEventPtr& pEvent) );
	WgCallbackHandle	AddCallback( const WgEventFilter& filter, void(*fp)( const WgEventPtr& pEvent, int param ), int param );
	WgCallbackHandle	AddCallback( const WgEventFilter& filter, void(*fp)( const WgEventPtr& pEvent, void * pParam), void * pParam );
	WgCallbackHandle	AddCallback( const WgEventFilter& filter, void(*fp)( const WgEventPtr& pEvent, const WgObjectPtr& pParam), const WgObjectPtr& pParam );
	WgCallbackHandle	AddCallback( const WgEventFilter& filter, WgEventListener * pListener );

	bool	DeleteCallback( WgCallbackHandle handle );

	int		DeleteCallbacksTo( const WgEventListener * pListener );
	int		DeleteCallbacksTo( void(*fp)( const WgEventPtr& pEvent) );
	int		DeleteCallbacksTo( void(*fp)( const WgEventPtr& pEvent, int param) );
	int		DeleteCallbacksTo( void(*fp)( const WgEventPtr& pEvent, void * pParam) );
	int		DeleteCallbacksTo( void(*fp)( const WgEventPtr& pEvent, const WgObjectPtr& pParam) );

	int		DeleteCallbacksOn( const WgWidgetPtr& pWidget );
	int		DeleteCallbacksOn( const WgEventType type );
	int		DeleteCallbacksOn( const WgWidgetPtr& pWidget, WgEventType type );

	int		DeleteAllCallbacks();
	int		DeleteDeadCallbacks();

	//----

private:
	WgEventHandler( WgRootPanel * pRoot );
	~WgEventHandler();

	class	Callback;

	void 	_postTickEvents( int ticks );
	void 	_processEventQueue();


	void	_finalizeEvent( const WgEventPtr& pEvent );
	void	_processGeneralEvent( const WgEventPtr& pEvent );
	void	_processGlobalEventCallbacks( const WgEventPtr& pEvent );
	void	_processWidgetEventCallbacks( const WgEventPtr& pEvent, WgWidget * pWidget );

	void	_processTick( WgTickEvent * pEvent );

	void	_processFocusGained( WgFocusGainedEvent * pEvent );
	void	_processFocusLost( WgFocusLostEvent * pEvent );

	void	_processMouseEnter( WgMouseEnterEvent * pEvent );
	void	_processMouseMove( WgMouseMoveEvent * pEvent );
	void	_processMousePosition( WgMousePositionEvent * pEvent );
	void	_processMouseLeave( WgMouseLeaveEvent * pEvent );

	void	_processMouseButtonPress( WgMousePressEvent * pEvent );
	void	_processMouseButtonRepeat( WgMouseRepeatEvent * pEvent );
	void	_processMouseButtonDrag( WgMouseDragEvent * pEvent );
	void	_processMouseButtonRelease( WgMouseReleaseEvent * pEvent );
	void	_processMouseButtonClick( WgMouseClickEvent * pEvent );
	void	_processMouseButtonDoubleClick( WgMouseDoubleClickEvent * pEvent );

	void	_processMouseWheelRoll( WgWheelRollEvent * pEvent );

	void	_processKeyPress( WgKeyPressEvent * pEvent );
	void	_processKeyRepeat( WgKeyRepeatEvent * pEvent );
	void	_processKeyRelease( WgKeyReleaseEvent * pEvent );

	void	_processCharacter( WgCharacterEvent * pEvent );

	bool	_isWidgetInList( const WgWidget * pWidget, const std::vector<WgWidgetWeakPtr>& list );

	WgCallbackHandle	_addCallback( Callback * pCallback );
	WgCallbackHandle	_addCallback( const WgEventFilter& filter, Callback * pCallback );
	int		_deleteCallbacksTo( const void * pReceiver );
	int		_deleteCallbacksOnType( WgEventType type, WgChain<Callback> * pChain );

	void 	_updateMarkedWidget(bool bPostMouseMoveEvents);
	void	_addTickReceiver( WgWidget * pWidget );

	void	_setWidgetFocused( WgWidget * pWidget, bool bFocused );


	//

	WgRootPanelWeakPtr	m_pRoot;

	std::deque<WgEventPtr>					m_eventQueue;
	bool									m_bIsProcessing;		// Set when we are inside ProcessEvents().
	std::deque<WgEventPtr>::iterator		m_insertPos;			// Position where we insert events being queued when processing.
	WgEventPtr								m_pEventProcessing;		// Current event being processed.
	WgWidgetPtr								m_pNextEventReceiver;	// Next widget to receive the event, from bubbling or forwarding.

	int64_t			m_time;
	WgCoord			m_pointerPos;
	WgPointerStyle	m_pointerStyle;
	WgModifierKeys	m_modKeys;

	std::vector<WgWidgetWeakPtr>	m_vTickWidgets;		// Widgets that have requested periodic tick-events (i.e. on every processEvents() ).

	// Current mouse state

	WgWidgetWeakPtr				m_pMarkedWidget;		// Widget the pointer currently is "inside". Empty if outside a modal widget.

	// Current button states

	bool						m_bButtonPressed[WG_MAX_BUTTONS+1];

	WgMousePressEventPtr		m_pLatestPressEvents[WG_MAX_BUTTONS+1];			// Saved info for the last time each button was pressed.
	WgMouseReleaseEventPtr		m_pLatestReleaseEvents[WG_MAX_BUTTONS+1];	// Saved info for the last time each button was released.

	WgWidgetWeakPtr				m_latestPressWidgets[WG_MAX_BUTTONS+1];		// Widget that received the latest press, for each button.
	WgWidgetWeakPtr				m_previousPressWidgets[WG_MAX_BUTTONS+1];	// Widget that received the second latest press, for each button,
																			// used for double-click handling.

	// Current keyboard state

	struct KeyDownInfo
	{
		WgKeyPressEventPtr 		pEvent;
		WgWidgetWeakPtr			pWidget;					// Widget that received the press event.
	};

	std::vector<KeyDownInfo*>	m_keysDown;				// One entry for each currently depressed key, in order of being pressed.

	bool						m_bWindowFocus;			// Set if we have window focus.
	WgPanelWeakPtr				m_keyFocusGroup;		// Current focus group (if any).
	WgWidgetWeakPtr				m_keyFocusWidget;		// Widget currently having the keyboard focus.

	std::map<WgWidgetWeakPtr,WgWidgetWeakPtr>	m_focusGroupMap;	// Mapping focus groups (key) with their currently focused Widget (value).


	// Callbacks

	class Callback : public WgLink
	{
	friend class WgEventHandler;
	public:
		virtual ~Callback() {};

		LINK_METHODS(Callback);

		virtual void 			ProcessEvent( const WgEventPtr& pEvent ) = 0;
		virtual bool 			IsAlive() const = 0;
		virtual void * 			Receiver() const = 0;
		inline const WgEventFilter& 	Filter() const { return m_filter; }

	protected:
		WgEventFilter		m_filter;
		WgCallbackHandle	m_handle;
	};


	class ObjectParamCallback : public Callback
	{
	public:
		ObjectParamCallback( const WgEventFilter& filter, void(*fp)(const WgEventPtr& pEvent, const WgObjectPtr& pParam), const WgObjectPtr& pParam );

		void 	ProcessEvent( const WgEventPtr& pEvent );
		bool 	IsAlive() const;
		void *	Receiver() const;

	private:
		void(*m_pFunction)(const WgEventPtr& pEvent, const WgObjectPtr& pParam);
		WgObjectWeakPtr	m_pObject;
	};

	class IntParamCallback : public Callback
	{
	public:
		IntParamCallback( const WgEventFilter& filter, void(*fp)(const WgEventPtr& pEvent, int param), int param );

		void 	ProcessEvent( const WgEventPtr& pEvent );
		bool 	IsAlive() const;
		void *	Receiver() const;

	private:
		void(*m_pFunction)( const WgEventPtr& pEvent, int param);
		int		m_param;
	};


	class VoidPtrParamCallback : public Callback
	{
	public:
		VoidPtrParamCallback( const WgEventFilter& filter, void(*fp)(const WgEventPtr& pEvent, void * pParam), void * pParam );

		void 	ProcessEvent( const WgEventPtr& pEvent );
		bool 	IsAlive() const;
		void *	Receiver() const;

	private:
		void(*m_pFunction)( const WgEventPtr& pEvent, void * pParam);
		void *			m_pParam;
	};

	class NoParamCallback : public Callback
	{
	public:
		NoParamCallback( const WgEventFilter& filter, void(*fp)(const WgEventPtr& pEvent) );

		void 	ProcessEvent( const WgEventPtr& pEvent );
		bool 	IsAlive() const;
		void *	Receiver() const;

	private:
		void(*m_pFunction)( const WgEventPtr& pEvent);
	};

	class ListenerCallback : public Callback
	{
	public:
		ListenerCallback( const WgEventFilter& filter, WgEventListener * pListener );

		void 	ProcessEvent( const WgEventPtr& pEvent );
		bool 	IsAlive() const;
		void *	Receiver() const;

	private:
		WgEventListener *	m_pListener;
	};

	WgCallbackHandle		m_handleCounter;					// Increment by one for each new callbackHandle, gives unique IDs.

	WgChain<Callback>						m_globalCallbacks;	// Callbacks called for every event.

	std::map<WgWidgetWeakPtr,WgChain<Callback> >	m_widgetCallbacks;	// Callbacks for Widget-specific events.


};




#endif //WG_EVENTHANDLER_DOT_H
