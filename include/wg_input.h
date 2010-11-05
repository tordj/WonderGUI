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

#ifndef WG_INPUT_DOT_H
#define WG_INPUT_DOT_H


#ifndef WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

#ifndef	WG_USERDEFINES_DOT_H
#	include <wg_userdefines.h>
#endif

#ifndef WG_KEY_DOT_H
#	include <wg_key.h>
#endif

#include <vector>

//#include "widget.h"

class WgWidget;
class WgGizmo;
class WgPointerSpy;
class Wdg_Root;

struct WgActionDetails
{
	bool removeWidget(WgWidget * pWidget);

	Uint32			timestamp;					// Timestamp (milliseconds) of action.
	WgModifierKeys	modifier;					// Status of modifier keys during action.
	Uint32			nWidgets;					// Number of widgets receiving the action.
	WgWidget *		aWidgets[WG_MAX_WIDGETS];	// The widgets receiving the action.
	WgGizmo *		aGizmos[WG_MAX_WIDGETS];	// The gizmos receiving the action.

	union
	{
		struct		// pointer
		{
			Sint32	x, y;						// X & Y position of pointer for action.
		};
		struct		// key
		{
			WgKey	keycode;					// Translated keycode of key pressed.
			Uint16	native_keycode;				// Native keycode of key pressed.
			Uint16	character;					// Character resulting from key press.
		};
		struct		// wheel
		{
			Sint32	rolldistance;
		};
	};
};


struct WgKeyDetails
{
	Uint32			timestamp;					// Timestamp (milliseconds) of key press.
	WgModifierKeys	modifier;					// Status of modifier keys during key press.
	WgKey			keycode;					// Keycode of key pressed (translated to WGKEY_x ).
	Uint16			native_keycode;				// Keycode of key pressed (native format).
	Uint16			character;					// Character resulting from key press.
};


// Contains list of currently depressed keys

struct	WgKeyDownStack
{
	Uint32			nKeysDown;
	WgKeyDetails	aKeysDown[WG_MAX_KEYS_DOWN];// No use saving complete ActionDetails...
};

//____ WgInput ________________________________________________________________

class WgInput
{
	friend class Wdg_Root;
	friend class WgWidget;
	friend class WgGizmo;

public:
	WgInput();
	~WgInput();

	inline Sint32 pointerX() { return m_pointerX; }
	inline Sint32 pointerY() { return m_pointerY; }

	void	map_keycode( WgKey wg_keycode, Uint16 native_keycode );

	void	begin_events( Uint32 time_ms );
	void	end_events();

	void	pointer_setPos( Sint32 x, Sint32 y );
	void	pointer_move( Sint32 x, Sint32 y );

	void	button_press( Uint8 button );
	void	button_release( Uint8 button );

	void	key_press( Uint16 native_keycode, Uint16 character = 0 );
	void	key_release( Uint16 native_keycode );

	void	character( Uint16 character, Uint16 native_keycode_for_repeat = 0 );

	void	key_release_all();
	void	button_release_all();

	void	wheel_roll( Uint8 wheel, Sint32 distance );

	void	flush_event_queue();

	bool	setButtonRepeat( Uint32 delay, Uint32 rate );
	bool	setKeyRepeat( Uint32 delay, Uint32 rate );
	bool	setDoubleClick( Uint32 treshold );
	Uint32	getKeyRepeatRate() const { return m_keyRepeatRate; }
	Uint32	getKeyRepeatDelay() const{ return m_keyRepeatDelay; }

	inline WgWidget *	getFocusedWidget() const { return m_pFocusedWidget; }
	inline WgGizmo *	getFocusedGizmo() const { return m_pFocusedGizmo; }

	inline void				setModifierKeys( WgModifierKeys modifier ) { m_modifierKeys = modifier; }
	inline WgModifierKeys	getModifierKeys( void ) const { return m_modifierKeys; }

	void					addPointerSpy( WgPointerSpy * pSpy );
	bool					removePointerSpy( WgPointerSpy * pSpy );

	// If called during a widget-callback, The get...Info() methods returns the
	// previous ActionDetails structures.

	bool	isButtonDown( Uint8 button ) const;
	bool	isKeyDown( Uint16 native_keycode ) const;
	bool	isKeyDown( WgKey wg_keycode ) const;
	bool	isPointerInside( WgWidget * pWidget ) const;
	bool	isPointerInside( WgGizmo * pGizmo ) const;

	bool	wasButtonPressInside( Uint8 button, WgWidget * pWidget ) const;
	bool	wasButtonPressInside( Uint8 button, WgGizmo * pGizmo ) const;
	bool	wasButtonReleaseInside( Uint8 button, WgWidget * pWidget ) const;
	bool	wasButtonReleaseInside( Uint8 button, WgGizmo * pGizmo ) const;

	const WgActionDetails * getPressDetails( Uint8 button ) const;
	const WgActionDetails * getReleaseDetails( Uint8 button ) const;
	const WgActionDetails * getMouseOverDetails( void ) const;
	const WgKeyDownStack *	getKeyDownDetails( void ) const;

	// Only meant for use internally and by Widgets.

	enum	UserAction
	{
		POINTER_ENTER,
		POINTER_OVER,
		POINTER_EXIT,
		BUTTON_PRESS,
		BUTTON_DOWN,
		BUTTON_REPEAT,
		BUTTON_RELEASE,
		BUTTON_RELEASE_OUTSIDE,
		BUTTON_CLICK,
		BUTTON_DOUBLECLICK,
		WHEEL_ROLL,
		KEY_PRESS,
		KEY_DOWN,
		KEY_REPEAT,
		KEY_RELEASE,
		CHARACTER,
		BUTTON_PRESS_OUTSIDE_MODAL,			// Can NOT be intercepted or BLOCKED!
		BUTTON_RELEASE_OUTSIDE_MODAL,		// Can NOT be intercepted or BLOCKED!
		POINTER_OUTSIDE_MODAL,				// Can NOT be intercepted or BLOCKED!
		WHEEL_ROLL_OUTSIDE_MODAL			// Can NOT be intercepted or BLOCKED!
	};


protected:


private:

	enum WgInputEvent
	{
		WG_BUTTON_PRESS,
		WG_BUTTON_RELEASE,
		WG_KEY_PRESS,
		WG_KEY_RELEASE,
		WG_CHAR,
		WG_WHEEL_ROLL,
	};

	struct WgInputEventData
	{
		WgInputEvent	event;
		Uint32			timestamp;
		WgModifierKeys	modifier;
		Sint32			pointerX;
		Sint32			pointerY;
		Sint32			data0;
		Sint32			data1;
	};
	std::vector<WgInputEventData> m_eventQueue;

	void	button_press_( WgInputEventData ed );
	void	button_release_( WgInputEventData ed );
	void	key_press_( WgInputEventData ed );
	void	key_release_( WgInputEventData ed );
	void	character_( WgInputEventData ed );
	void	wheel_roll_( WgInputEventData ed );

	WgKey	translate_keycode( Uint16 native_keycode );

	void	setRootWidget( Wdg_Root * pRootWidget );
	void	setFocusedWidget( WgWidget * pWidget );
	void	setFocusedGizmo( WgGizmo * pGizmo );

	bool	widgetInStack( const WgWidget * pWidget, const WgActionDetails& info ) const; // Check if widget is in Events widget stack.
	bool	gizmoInStack( const WgGizmo * pGizmo, const WgActionDetails& info ) const; // Check if widget is in Events widget stack.
	void	widgetRemoved( WgWidget * pWidget );
	void	gizmoRemoved( WgGizmo * pGizmo );

	Wdg_Root	* 	m_pRootWidget;				///< Pointer at root widget which we are a part of.
	WgWidget	*	m_pFocusedWidget;			///< Pointer at widget with keyboard focus. Default is root widget.
	WgGizmo		*	m_pFocusedGizmo;			///< Pointer at widget with keyboard focus. Default is root widget.

	Uint32			m_time;						///< Accumulative time counter in milliseconds

	Uint32			m_doubleClickTreshold;		///< Maximum millseconds between first and second press to count as a doubleclick.

	Uint32			m_buttonRepeatDelay;
	Uint32			m_buttonRepeatRate;

	Uint32			m_keyRepeatDelay;
	Uint32			m_keyRepeatRate;

	Sint32			m_pointerX, m_pointerY;
	WgModifierKeys	m_modifierKeys;

	bool			m_bButtonDown[WG_MAX_BUTTONS];
	Uint32			m_repeatTime[WG_MAX_BUTTONS];

	WgActionDetails	m_position;
	WgActionDetails	m_pressed[WG_MAX_BUTTONS];
	WgActionDetails	m_released[WG_MAX_BUTTONS];
	WgActionDetails	m_currentPosition;

	WgKeyDownStack	m_keys;
	Uint32			m_keyRepeatTime[WG_MAX_KEYS_DOWN];

	WgPointerSpy**	m_pPointerSpies;
	Uint32			m_nPointerSpies;

	Uint32			m_hoverStartTime;

	Uint16			m_keycodeMap[WGKEY_ARRAYSIZE];

};





#endif // WG_INPUT_DOT_H
