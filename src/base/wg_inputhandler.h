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
#ifndef WG_INPUTHANDLER_DOT_H
#define WG_INPUTHANDLER_DOT_H

#ifndef WG_POINTERS_DOT_H
#	include <wg_pointers.h>
#endif

#ifndef WG_GEO_DOT_H
#	include <wg_geo.h>
#endif

#ifndef WG_WIDGET_DOT_H
#	include <wg_widget.h>
#endif

#ifndef WG_KEY_DOT_H
#	include <wg_key.h>
#endif

#ifndef WG_MSG_DOT_H
#	include <wg_msg.h>
#endif

#ifndef WG_ROOTPANEL_DOT_H
#	include <wg_rootpanel.h>
#endif


/* LEFT TO DO:
 * 
 * 
 * Automatic garbage collection on msgRouter.
 * 
 * Character handling.
 */


namespace wg 
{
	
	class RootPanel;
	typedef	StrongPtr<RootPanel,Object_p>		RootPanel_p;
	typedef	WeakPtr<RootPanel,Object_wp>		RootPanel_wp;
	
	class InputHandler;
	typedef	StrongPtr<InputHandler,Receiver_p>		InputHandler_p;
	typedef	WeakPtr<InputHandler,Receiver_wp>		InputHandler_wp;
	
	class InputHandler : public Receiver
	{
		
	friend class RootPanel;	
	public:
		static InputHandler_p	create() { return new InputHandler(); }
	
		bool					isInstanceOf( const char * pClassName ) const;
		const char *			className( void ) const;
		static const char		CLASSNAME[];
		static InputHandler_p	cast( const Object_p& pObject );
	
		void 		setPointer( const RootPanel_p& pRoot, Coord pos, int64_t timestamp = 0 );
		void 		setButton( MouseButton button, bool bPressed, int64_t timestamp = 0 );
		void 		setWheelRoll( int wheel, Coord distance, int64_t timestamp = 0 );

		void 		setFocusedWindow( const RootPanel_p& pRoot );

		void 		setKey( int nativeKeyCode, bool bPressed, int64_t timestamp = 0 );
		
		bool		putText( const CharSeq& text );
		
		Widget_p 	focusedWidget() const { return _focusedWidget(); }
		RootPanel_p focusedWindow() const { return m_pFocusedRoot.rawPtr(); }
		
		bool		isButtonPressed( MouseButton button ) const;
		bool		isAnyButtonPressed() const;
		
		bool		isKeyPressed( int nativeKeyCode ) const;
	
		
		void 		receive( const Msg_p& pMsg );


		void		mapKey( int native_keycode, Key translated_keycode );
		void		unmapKey( int native_keycode );
		void		clearKeyMap();
		Key			translateKey( int native_keycode );

		void		mapCommand( int native_keycode, ModifierKeys modKeys, EditCmd command );
		void		unmapCommand( int native_keycode, ModifierKeys modKeys );
		void		clearCommandMap();
		EditCmd		translateCommand( int native_keycode, ModifierKeys modKeys );


		bool		setButtonRepeat( int delay, int rate );
		bool		setKeyRepeat( int delay, int rate );

		inline int	buttonRepeatDelay() { return m_buttonRepeatDelay; }
		inline int	buttonRepeatRate() { return m_buttonRepeatRate; }

		inline int	keyRepeatDelay() { return m_keyRepeatDelay; }
		inline int	keyRepeatRate() { return m_keyRepeatRate; }

		inline bool	setDoubleClickTresholds( int millisec, int pixels );
		inline int	doubleClickTimeTreshold() { return m_doubleClickTimeTreshold; }
		inline int	doubleClickDistanceTreshold() { return m_doubleClickDistanceTreshold; }

		
	protected:
		InputHandler();
		~InputHandler();
	
		Widget *	_updateEnteredWidgets( Widget * pMarkedWidget, int64_t timestamp );
	
		void		_processButtonPress( MouseButton button, int64_t timestamp );
		void		_processButtonRelease( MouseButton button, int64_t timestamp );
		void		_handleButtonRepeats( int64_t timestamp );

		void 		_processKeyPress( int nativeKeyCode, int64_t timestamp );
		void 		_processKeyRelease( int nativeKeyCode, int64_t timestamp );
		void		_handleKeyRepeats( int64_t timestamp );
		

		bool		_focusChanged( RootPanel * pRoot, Widget * pOldFocused, Widget * pNewFocused );
	
		int			_widgetPosInList( const Widget * pWidget, const std::vector<Widget_wp>& list );
	
		void 		_setFocused( Widget * pWidget );
		void 		_setUnfocused( Widget * pWidget );

		Widget *	_focusedWidget() const;
	
	
		char			m_inputId;
		RouteId			m_tickRoute;
		int64_t			m_timeStamp;
		
		Coord			m_pointerPos;
		
		RootPanel_wp	m_pMarkedRoot;		// Root widget the pointer currently is "inside". 
		Widget_wp		m_pMarkedWidget;	// Widget the pointer currently is "inside". Empty if outside a modal widget.

		RootPanel_wp	m_pFocusedRoot;		// RootPanel that is focused (has the focused widget).
	
		std::vector<Widget_wp>	m_vEnteredWidgets;	// All widgets that pointer is considered to be inside (= markedWidget + its ancestors).
		
	
		PointerStyle	m_pointerStyle;
		ModifierKeys	m_modKeys;
	
	
		// Current button states
	
		bool			m_bButtonPressed[MouseButton_Max+1];
	
		Widget_wp		m_latestPressWidgets[MouseButton_Max+1];		// Widget that received the latest press, for each button.
		int64_t			m_latestPressTimestamps[MouseButton_Max+1];	// Timestamp of the latest press, for each button.
		Coord			m_latestPressPosition[MouseButton_Max+1];	// Coord of the latest press, for each button.
		bool			m_latestPressDoubleClick[MouseButton_Max+1];	// Set if latest press resulted in a double-click.	

		// Current key states


		struct KeyDownInfo
		{
			int			nativeKeyCode;
			Key		translatedKeyCode;
			int64_t		pressTimestamp;	
			Widget_wp	pWidget;
		};
	
		std::vector<KeyDownInfo>	m_keysDown;				// One entry for each currently depressed key, in order of being pressed.


		// Settings for keyboard/pointer input
	
		int				m_doubleClickTimeTreshold;		// Maximum millseconds between first and second press to count as a doubleclick.
		int				m_doubleClickDistanceTreshold;	// Maximum distance the pointer may move between first and second press to count as a doubleclick.

		int				m_buttonRepeatDelay;
		int				m_buttonRepeatRate;

		int				m_keyRepeatDelay;
		int				m_keyRepeatRate;

		std::map<int,Key>	m_keycodeMap;		// Maps native keycodes to Key.
	
		struct CommandEntry
		{
			ModifierKeys	modKeys;
			EditCmd			command;
		};

		std::map<int,std::vector<CommandEntry>> m_commandMap;

	};
	
	

} // namespace wg
#endif //WG_INPUTHANDLER_DOT_H
