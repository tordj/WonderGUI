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

class WgRootPanel;
typedef	WgStrongPtr<WgRootPanel,WgObjectPtr>		WgRootPanelPtr;
typedef	WgWeakPtr<WgRootPanel,WgObjectWeakPtr>		WgRootPanelWeakPtr;

class WgInputHandler;
typedef	WgStrongPtr<WgInputHandler,WgReceiverPtr>		WgInputHandlerPtr;
typedef	WgWeakPtr<WgInputHandler,WgReceiverWeakPtr>		WgInputHandlerWeakPtr;

class WgInputHandler : public WgReceiver
{
public:
	static WgInputHandlerPtr	Create() { return new WgInputHandler(); }

	bool						IsInstanceOf( const char * pClassName ) const;
	const char *				ClassName( void ) const;
	static const char			CLASSNAME[];
	static WgInputHandlerPtr	Cast( const WgObjectPtr& pObject );

	void SetPointer( const WgRootPanelPtr& pRoot, WgCoord pos );
	void SetButton( WgMouseButton button, bool bPressed );
	void SetWheelRoll( int wheel, int steps );
	void SetFocused( const WgRootPanelPtr& pRoot );
	void SetKey( short nativeKeyCode, bool bPressed );
	
	void OnMsg( const WgMsgPtr& pMsg );
	
protected:
	WgInputHandler();
	~WgInputHandler();

	void 		_updateMarkedWidget(bool bPostMouseMoveMsgs);
	WgWidget *	_updateEnteredWidgets( WgWidget * pMarkedWidget );

	void		_processMouseButtonPress( WgMouseButton button );
	void		_processMouseButtonRelease( WgMouseButton button );

	int			_widgetPosInList( const WgWidget * pWidget, const std::vector<WgWidgetWeakPtr>& list );



	WgRouteId			m_tickRoute;
	int64_t				m_timeStamp;
	
	WgCoord				m_pointerPos;
	
	WgRootPanelWeakPtr	m_pMarkedRoot;		// Root widget the pointer currently is "inside". 
	WgWidgetWeakPtr		m_pMarkedWidget;	// Widget the pointer currently is "inside". Empty if outside a modal widget.

	std::vector<WgWidgetWeakPtr>	m_vEnteredWidgets;	// All widgets that pointer is considered to be inside (= markedWidget + its ancestors).
	

	WgPointerStyle	m_pointerStyle;
	WgModifierKeys	m_modKeys;


	// Current button states

	bool						m_bButtonPressed[WG_MAX_BUTTONS+1];

	WgMousePressMsgPtr			m_pLatestPressMsgs[WG_MAX_BUTTONS+1];			// Saved info for the last time each button was pressed.
	WgMouseReleaseMsgPtr		m_pLatestReleaseMsgs[WG_MAX_BUTTONS+1];	// Saved info for the last time each button was released.

	WgWidgetWeakPtr				m_latestPressWidgets[WG_MAX_BUTTONS+1];		// Widget that received the latest press, for each button.
};


#endif //WG_INPUTHANDLER_DOT_H