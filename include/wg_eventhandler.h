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



class WgEventFilter
{
public:
	WgEventFilter() : m_bGizmo(false), m_type(WG_EVENT_DUMMY), m_pGizmo(0) {}
	WgEventFilter( WgEventId eventType ) : m_bGizmo(false), m_type(eventType), m_pGizmo(0) {}
	WgEventFilter( WgEventId eventType, WgGizmo * pGizmo ) : m_bGizmo(true), m_type(eventType), m_pGizmo(pGizmo) {}
	WgEventFilter( WgGizmo * pGizmo ) : m_bGizmo(true), m_type(WG_EVENT_DUMMY), m_pGizmo(pGizmo) {}

	bool	FilterEvent( const WgEvent::Event& _event ) const;

	inline bool			FiltersGizmo() const { return m_bGizmo; }
	inline bool			FiltersType() const { return (m_type!=WG_EVENT_DUMMY); }
	inline WgGizmo*		Gizmo() const { return m_pGizmo; }
	inline WgEventId	EventType() const { return m_type; }

private:

	bool		m_bGizmo;
	WgEventId	m_type;
	WgGizmo * 	m_pGizmo;
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

	bool	DeleteCallback( const WgEventFilter& filter, WgGizmo * pGizmo );
	bool	DeleteCallback( const WgEventFilter& filter, void * pFunction );
	bool	DeleteCallback( const WgEventFilter& filter, WgEventListener * pListener );

	int		DeleteCallbacks( WgGizmo * pGizmo );
	int		DeleteCallbacks( void * pFunction );
	int		DeleteCallbacks( WgEventListener * pListener );
	int		DeleteCallbacks( const WgEventFilter& filter );

	int		DeleteAllCallbacks();

	int		DeleteDeadCallbacks();


private:
	class	Callback;


	void	FinalizeEvent( WgEvent::Event& _event );
	void	ProcessGeneralEvent( WgEvent::Event& _event );
	void	ProcessEventCallbacks( WgEvent::Event& _event );

	void	ProcessTimePass( WgEvent::TimePass * pEvent );

	void	ProcessPointerEnter( WgEvent::PointerEnter * pEvent );
	void	ProcessPointerMove( WgEvent::PointerMove * pEvent );
	void	ProcessPointerPlaced( WgEvent::PointerPlaced * pEvent );
	void	ProcessPointerExit( WgEvent::PointerExit * pEvent );

	void	ProcessButtonPress( WgEvent::ButtonPress * pEvent );
	void	ProcessButtonRepeat( WgEvent::ButtonRepeat * pEvent );
	void	ProcessButtonDrag( WgEvent::ButtonDrag * pEvent );
	void	ProcessButtonRelease( WgEvent::ButtonRelease * pEvent );
	void	ProcessButtonClick( WgEvent::ButtonClick * pEvent );
	void	ProcessButtonDoubleClick( WgEvent::ButtonDoubleClick * pEvent );

	bool	IsGizmoInList( const WgGizmo * pGizmo, const std::vector<WgGizmoWeakPtr>& list );

	void	_addCallback( const WgEventFilter& filter, Callback * pCallback );

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

		virtual void ProcessEvent( const WgEvent::Event& _event ) = 0;
		virtual bool IsAlive() const = 0;

		WgEventId	m_eventType;
	};


	class GizmoCallback : public Callback
	{
	public:
		GizmoCallback( WgEventId eventType, void(*fp)(const WgEvent::Event& _event, WgGizmo * pDest), WgGizmo * pDest );

		void 	ProcessEvent( const WgEvent::Event& _event );
		bool 	IsAlive() const;

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

	private:
		void(*m_pFunction)( const WgEvent::Event& _event);
	};

	class ListenerCallback : public Callback
	{
	public:
		ListenerCallback( WgEventId eventType, WgEventListener * pListener );

		void 	ProcessEvent( const WgEvent::Event& _event );
		bool 	IsAlive() const;

	private:
		WgEventListener *	m_pListener;
	};


	WgChain<Callback>						m_globalCallbacks;	// Callbacks called for every event.

	std::map<WgGizmo*,WgChain<Callback> >	m_gizmoCallbacks;	// Callbacks for Gizmo-specific events.


};




#endif //WG_EVENTHANDLER_DOT_H
