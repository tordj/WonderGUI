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

#ifndef WG_MSGFILTER_DOT_H
#define WG_MSGFILTER_DOT_H

#ifndef WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

#ifndef WG_POINTERS_DOT_H
#	include <wg_pointers.h>
#endif

#ifndef WG_KEY_DOT_H
#	include <wg_key.h>
#endif

#ifndef WG_WIDGET_DOT_H
#	include <wg_widget.h>
#endif


class WgMsgFilter;

typedef	bool(*WgFilterFuncPtr)(const WgMsg_p&, const WgMsgFilter& );


class WgMsgFilter
{
	friend class WgMsgRouter;
	
public:
	WgMsgFilter() : m_msgType(WG_MSG_DUMMY), m_pFilterFunc(_filterDummy) {}	//Not meant to be public, but has to for compatibility with older compilers.
	
	static WgMsgFilter	tick();
	static WgMsgFilter	pointerChange();

	static WgMsgFilter	focusGained();
	static WgMsgFilter	focusLost();

	static WgMsgFilter	mouseEnter();

	static WgMsgFilter	mouseLeave();

	static WgMsgFilter	mouseMove();

	static WgMsgFilter	mousePress();
	static WgMsgFilter	mousePress( WgMouseButton button);

	static WgMsgFilter	mouseRelease();
	static WgMsgFilter	mouseRelease( WgMouseButton button);

	static WgMsgFilter	mouseDrag();
	static WgMsgFilter	mouseDrag( WgMouseButton button);

	static WgMsgFilter	mouseRepeat();
	static WgMsgFilter	mouseRepeat( WgMouseButton button);

	static WgMsgFilter	mouseClick();
	static WgMsgFilter	mouseClick( WgMouseButton button);

	static WgMsgFilter	mouseDoubleClick();
	static WgMsgFilter	mouseDoubleClick( WgMouseButton button);

	static WgMsgFilter	keyPress();
	static WgMsgFilter	keyPress( WgKey translated_keycode );

	static WgMsgFilter	keyRelease();
	static WgMsgFilter	keyRelease( WgKey translated_keycode );

	static WgMsgFilter	keyRepeat();
	static WgMsgFilter	keyRepeat( WgKey keycode );

	static WgMsgFilter	keyPressNative( int native_keycode );

	static WgMsgFilter	keyReleaseNative( int native_keycode );

	static WgMsgFilter	keyRepeatNative( int native_keycode );

	static WgMsgFilter	character();
	static WgMsgFilter	character( unsigned short character);

	static WgMsgFilter	wheelRoll();
	static WgMsgFilter	wheelRoll( int wheel );

	static WgMsgFilter	select();

	static WgMsgFilter	toggle();

	static WgMsgFilter	valueUpdate();

	static WgMsgFilter	rangeUpdate();

	static WgMsgFilter	textEdit();

	static WgMsgFilter	itemToggle();
	static WgMsgFilter	itemToggle( int id );

	static WgMsgFilter	itemMousePress();
	static WgMsgFilter	itemMousePress( WgMouseButton button );
	static WgMsgFilter	itemMousePress( WgMouseButton button, int id );

	static WgMsgFilter	itemsSelect();

	static WgMsgFilter	itemsUnselect();

	//TODO: Add missing filters!


	inline bool				filterMsg( const WgMsg_p& pMsg ) const { return m_pFilterFunc( pMsg, *this ); }
	inline WgMsgType 		msgType() const { return m_msgType; }
private:
	WgMsgFilter( WgMsgType msgType, WgFilterFuncPtr pFunc, int data1 = 0, int data2 = 0 )
				: m_msgType(msgType), m_pFilterFunc(pFunc), m_data1(data1), m_data2(data2) 
	{
	}

	WgMsgType			m_msgType;			// Only message type filter accepts, if a specific.
	WgFilterFuncPtr 	m_pFilterFunc;		// Pointer at filter function.
	int					m_data1;			// Additional data for filter function.
	int					m_data2;			// Additional data for filter function.


	static bool _filterDummy( const WgMsg_p& pMsg, const WgMsgFilter& filter );
	static bool _filterType( const WgMsg_p& pMsg, const WgMsgFilter& filter );

	static bool _filterPointerMsgs( const WgMsg_p& pMsg, const WgMsgFilter& filter );
	static bool _filterMouseButtonMsgs( const WgMsg_p& pMsg, const WgMsgFilter& filter );
	static bool _filterKeyMsgs( const WgMsg_p& pMsg, const WgMsgFilter& filter );
	static bool _filterNativeKeyMsgs( const WgMsg_p& pMsg, const WgMsgFilter& filter );
	static bool _filterCharacterMsgs( const WgMsg_p& pMsg, const WgMsgFilter& filter );
	static bool _filterWheelRollMsgs( const WgMsg_p& pMsg, const WgMsgFilter& filter );
	static bool _filterItemToggleMsgs( const WgMsg_p& pMsg, const WgMsgFilter& filter );
	static bool _filterItemMousePressMsgs( const WgMsg_p& pMsg, const WgMsgFilter& filter );
};


#endif // WG_MSGFILTER_DOT_H
