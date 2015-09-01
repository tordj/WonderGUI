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

#ifndef WG_MSG_DOT_H
#	include <wg_msg.h>
#endif

namespace wg 
{
	
	class RootPanel;
	typedef	WgStrongPtr<RootPanel,Object_p>		RootPanel_p;
	typedef	WgWeakPtr<RootPanel,Object_wp>		RootPanel_wp;
	
	class InputHandler;
	typedef	WgStrongPtr<InputHandler,Receiver_p>		InputHandler_p;
	typedef	WgWeakPtr<InputHandler,Receiver_wp>		InputHandler_wp;
	
	class InputHandler : public Receiver
	{
	public:
		static InputHandler_p	create() { return new InputHandler(); }
	
		bool						isInstanceOf( const char * pClassName ) const;
		const char *				className( void ) const;
		static const char			CLASSNAME[];
		static InputHandler_p	cast( const Object_p& pObject );
	
		void setPointer( const RootPanel_p& pRoot, Coord pos );
		void setButton( WgMouseButton button, bool bPressed );
		void setWheelRoll( int wheel, Coord distance );
		void setFocused( const RootPanel_p& pRoot );
		void setKey( short nativeKeyCode, bool bPressed );
		
		void onMsg( const Msg_p& pMsg );
		
	protected:
		InputHandler();
		~InputHandler();
	
		void 		_updateMarkedWidget(bool bPostMouseMoveMsgs);
		Widget *	_updateEnteredWidgets( Widget * pMarkedWidget );
	
		void		_processMouseButtonPress( WgMouseButton button );
		void		_processMouseButtonRelease( WgMouseButton button );
	
		void		_handleMouseButtonRepeats( int millisec );
	
		int			_widgetPosInList( const Widget * pWidget, const std::vector<Widget_wp>& list );
	
	
	
		WgRouteId			m_tickRoute;
		int64_t				m_timeStamp;
		
		Coord				m_pointerPos;
		
		RootPanel_wp	m_pMarkedRoot;		// Root widget the pointer currently is "inside". 
		Widget_wp		m_pMarkedWidget;	// Widget the pointer currently is "inside". Empty if outside a modal widget.
	
		std::vector<Widget_wp>	m_vEnteredWidgets;	// All widgets that pointer is considered to be inside (= markedWidget + its ancestors).
		
	
		WgPointerStyle	m_pointerStyle;
		WgModifierKeys	m_modKeys;
	
	
		// Current button states
	
		bool					m_bButtonPressed[WG_MAX_BUTTONS+1];
	
		Widget_wp				m_latestPressWidgets[WG_MAX_BUTTONS+1];		// Widget that received the latest press, for each button.
		int64_t					m_latestPressTimestamps[WG_MAX_BUTTONS+1];	// Timestamp of the latest press, for each button.
		Coord					m_latestPressPosition[WG_MAX_BUTTONS+1];	// Coord of the latest press, for each button.
		bool					m_latestPressDoubleClick[WG_MAX_BUTTONS+1];	// Set if latest press resulted in a double-click.	
	};
	
	

} // namespace wg
#endif //WG_INPUTHANDLER_DOT_H
