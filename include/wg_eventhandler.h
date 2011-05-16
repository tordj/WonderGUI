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

#ifndef WG_KEY_DOT_H
#	include <wg_key.h>
#endif

class WgRoot;



/*

	pEventHandler->AddCallback( WgEvent::ButtonDrag::Filter(pTitleBar, 2 ), WgFlexGeo::cbDrag, pWindow );

	pEventHandler->AddCallback( WgEvent::ButtonDragFilter(pTitleBar, 2 ), WgFlexGeo::cbDrag, pWindow );

	pEventHandler->AddCallback( WgEventFilter::ButtonDrag(pTitleBar 2 ), WgFlexGeo::cbDrag, pWindow );


*/




class WgEventFilter
{
public:
	virtual bool			FilterEvent( const WgEvent::Event& _event ) const = 0;

	WgGizmo*				Gizmo() const;
	inline WgGizmoWeakPtr	GizmoWeakPtr() const { return m_pGizmo; }

private:

	WgEventFilter();

	WgGizmoWeakPtr 	m_pGizmo;

	union
	{
		void * 	pData;
		int		data;

	};

/*

	WgEventFilter() : m_bGizmo(false), m_type(WG_EVENT_DUMMY), m_pGizmo(0) {}
	WgEventFilter( WgEventId eventType ) : m_bGizmo(false), m_type(eventType), m_pGizmo(0) {}
	WgEventFilter( WgEventId eventType, WgGizmo * pGizmo ) : m_bGizmo(true), m_type(eventType), m_pGizmo(pGizmo) {}
	WgEventFilter( WgGizmo * pGizmo ) : m_bGizmo(true), m_type(WG_EVENT_DUMMY), m_pGizmo(pGizmo) {}

	bool	FilterEvent( const WgEvent::Event& _event ) const;

	inline bool			FiltersGizmo() const { return m_bGizmo; }
	inline bool			FiltersType() const { return (m_type!=WG_EVENT_DUMMY); }
	inline WgEventId	EventType() const { return m_type; }

private:
*/

};


class WgEventListener
{
public:
	virtual ~WgEventListener() {};

	virtual void OnEvent( const WgEvent::Event& _event ) = 0;
};



class WgEventHandler
{
public:
	WgEventHandler( int64_t startTime, WgRoot * pRoot );
	~WgEventHandler();

	bool	QueueEvent( const WgEvent::Event& _event );

	void	ProcessEvents();

	void	AddCallback( void(*fp)( const WgEvent::Event& _event) );
	void	AddCallback( void(*fp)( const WgEvent::Event& _event, void * pParam), void * pParam );
	void	AddCallback( void(*fp)( const WgEvent::Event& _event, WgGizmo * pDest), WgGizmo * pDest );
	void	AddCallback( WgEventListener * pListener );

	void	AddCallback( const WgEventFilter& filter, void(*fp)( const WgEvent::Event& _event) );
	void	AddCallback( const WgEventFilter& filter, void(*fp)( const WgEvent::Event& _event, void * pParam), void * pParam );
	void	AddCallback( const WgEventFilter& filter, void(*fp)( const WgEvent::Event& _event, WgGizmo * pDest), WgGizmo * pDest );
	void	AddCallback( const WgEventFilter& filter, WgEventListener * pListener );

	int		DeleteCallbacksTo( const WgGizmo * pGizmo );
	int		DeleteCallbacksTo( const WgEventListener * pListener );
	int		DeleteCallbacksTo( void(*fp)( const WgEvent::Event& _event) );
	int		DeleteCallbacksTo( void(*fp)( const WgEvent::Event& _event, void * pParam) );

	int		DeleteCallbacksOn( const WgGizmo * pGizmo );
	int		DeleteCallbacksOn( const WgEventId type );
	int		DeleteCallbacksOn( const WgGizmo * pGizmo, WgEventId type );

	int		DeleteCallback( const WgEventFilter& filter, const WgGizmo * pGizmo );
	int		DeleteCallback( const WgEventFilter& filter, const WgEventListener * pListener );
	int		DeleteCallback( const WgEventFilter& filter, void(*fp)( const WgEvent::Event& _event) );
	int		DeleteCallback( const WgEventFilter& filter, void(*fp)( const WgEvent::Event& _event, void * pParam) );


	int		DeleteAllCallbacks();
	int		DeleteDeadCallbacks();


private:
	class	Callback;


	void	_finalizeEvent( WgEvent::Event& _event );
	void	_processGeneralEvent( WgEvent::Event& _event );
	void	_processEventCallbacks( WgEvent::Event& _event );

	void	_processTick( WgEvent::Tick * pEvent );

	void	_processPointerEnter( WgEvent::PointerEnter * pEvent );
	void	_processPointerMove( WgEvent::PointerMove * pEvent );
	void	_processPointerPlaced( WgEvent::PointerPlaced * pEvent );
	void	_processPointerExit( WgEvent::PointerExit * pEvent );

	void	_processButtonPress( WgEvent::ButtonPress * pEvent );
	void	_processButtonRepeat( WgEvent::ButtonRepeat * pEvent );
	void	_processButtonDrag( WgEvent::ButtonDrag * pEvent );
	void	_processButtonRelease( WgEvent::ButtonRelease * pEvent );
	void	_processButtonClick( WgEvent::ButtonClick * pEvent );
	void	_processButtonDoubleClick( WgEvent::ButtonDoubleClick * pEvent );

	bool	_isGizmoInList( const WgGizmo * pGizmo, const std::vector<WgGizmoWeakPtr>& list );

	void	_addCallback( const WgEventFilter& filter, Callback * pCallback );
	int		_deleteCallbacksTo( const void * pReceiver );
	int		_deleteCallbacksOnType( WgEventId type, WgChain<Callback> * pChain );
	int		_deleteCallback( const WgEventFilter& filter, const void * pReceiver );

	//

	WgRoot *		m_pRoot;

	std::deque<WgEvent::Event>				m_eventQueue;
	bool									m_bIsProcessing;	// Set when we are inside ProcessEvents().
	std::deque<WgEvent::Event>::iterator	m_insertPos;		// Position where we insert events being queued when processing.

	int64_t			m_time;
	WgCord			m_pointerPos;
	WgModifierKeys	m_modKeys;

	// Settings for keyboard/pointer input

	int				m_doubleClickTimeTreshold;		// Maximum millseconds between first and second press to count as a doubleclick.
	int				m_doubleClickDistanceTreshold;	// Maximum distance the pointer may move between first and second press to count as a doubleclick.

	int				m_buttonRepeatDelay;
	int				m_buttonRepeatRate;

	int				m_keyRepeatDelay;
	int				m_keyRepeatRate;


	std::map<int,WgKey>	m_keycodeMap;		// Maps native keycodes to WgKey.

	// Current mouse state

	std::vector<WgGizmoWeakPtr>	m_vMarkedGizmos;	// Gizmos the pointer currently is "inside".

	// Current button states

	WgEvent::Event *			m_pLatestButtonEvents[WG_MAX_BUTTONS+1];	// Pointer at event object in either m_latestPress or m_latestRelease.

	WgEvent::ButtonPress		m_latestPress[WG_MAX_BUTTONS+1];			// Saved info for the last time each button was pressed.
	WgEvent::ButtonRelease		m_latestRelease[WG_MAX_BUTTONS+1];			// Saved info for the last time each button was released.

	std::vector<WgGizmoWeakPtr>	m_latestPressGizmos[WG_MAX_BUTTONS+1];		// List of gizmos who received the latest press, for each button.
	std::vector<WgGizmoWeakPtr>	m_previousPressGizmos[WG_MAX_BUTTONS+1];	// List of gizmos who received the second latest press, for each button,
																			// used for double-click handling.

	// Current keyboard state



	// Callbacks

	class Callback : public WgLink
	{
	public:
		virtual ~Callback() {};

		LINK_METHODS(Callback);

		virtual void 		ProcessEvent( const WgEvent::Event& _event ) = 0;
		virtual bool 		IsAlive() const = 0;
		virtual void * 		Receiver() const = 0;
		inline WgEventId	EventType() const { return m_eventType; }

		WgEventId	m_eventType;
	};


	class GizmoCallback : public Callback
	{
	public:
		GizmoCallback( WgEventId eventType, void(*fp)(const WgEvent::Event& _event, WgGizmo * pDest), WgGizmo * pDest );

		void 	ProcessEvent( const WgEvent::Event& _event );
		bool 	IsAlive() const;
		void *	Receiver() const;

	private:
		void(*m_pFunction)(const WgEvent::Event& _event, WgGizmo * pDest);
		WgGizmoWeakPtr	m_pGizmo;		// Destination Gizmo, not source as in the event.
	};

	class FunctionCallbackParam : public Callback
	{
	public:
		FunctionCallbackParam( WgEventId eventType, void(*fp)(const WgEvent::Event& _event, void * pParam), void * pParam );

		void 	ProcessEvent( const WgEvent::Event& _event );
		bool 	IsAlive() const;
		void *	Receiver() const;

	private:
		void(*m_pFunction)( const WgEvent::Event& _event, void * pParam);
		void *			m_pParam;
	};

	class FunctionCallback : public Callback
	{
	public:
		FunctionCallback( WgEventId eventType, void(*fp)(const WgEvent::Event& _event) );

		void 	ProcessEvent( const WgEvent::Event& _event );
		bool 	IsAlive() const;
		void *	Receiver() const;

	private:
		void(*m_pFunction)( const WgEvent::Event& _event);
	};

	class ListenerCallback : public Callback
	{
	public:
		ListenerCallback( WgEventId eventType, WgEventListener * pListener );

		void 	ProcessEvent( const WgEvent::Event& _event );
		bool 	IsAlive() const;
		void *	Receiver() const;

	private:
		WgEventListener *	m_pListener;
	};


	WgChain<Callback>						m_globalCallbacks;	// Callbacks called for every event.

	std::map<WgGizmoWeakPtr,WgChain<Callback> >	m_gizmoCallbacks;	// Callbacks for Gizmo-specific events.


};




#endif //WG_EVENTHANDLER_DOT_H
