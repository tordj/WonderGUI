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

typedef	bool(*WgFilterFuncPtr)(const WgMsgPtr&, const WgMsgFilter& );


class WgMsgFilter
{
	friend class WgMsgRouter;
	
public:
	WgMsgFilter() : m_msgType(WG_MSG_DUMMY), m_pFilterFunc(_filterDummy) {}	//Not meant to be public, but has to for compatibility with older compilers.
	
	static WgMsgFilter	Tick();
	static WgMsgFilter	PointerChange();

	static WgMsgFilter	FocusGained();
	static WgMsgFilter	FocusLost();

	static WgMsgFilter	MouseEnter();

	static WgMsgFilter	MouseLeave();

	static WgMsgFilter	MouseMove();

	static WgMsgFilter	MousePress();
	static WgMsgFilter	MousePress( WgMouseButton button);

	static WgMsgFilter	MouseRelease();
	static WgMsgFilter	MouseRelease( WgMouseButton button);

	static WgMsgFilter	MouseDrag();
	static WgMsgFilter	MouseDrag( WgMouseButton button);

	static WgMsgFilter	MouseRepeat();
	static WgMsgFilter	MouseRepeat( WgMouseButton button);

	static WgMsgFilter	MouseClick();
	static WgMsgFilter	MouseClick( WgMouseButton button);

	static WgMsgFilter	MouseDoubleClick();
	static WgMsgFilter	MouseDoubleClick( WgMouseButton button);

	static WgMsgFilter	KeyPress();
	static WgMsgFilter	KeyPress( WgKey translated_keycode );

	static WgMsgFilter	KeyRelease();
	static WgMsgFilter	KeyRelease( WgKey translated_keycode );

	static WgMsgFilter	KeyRepeat();
	static WgMsgFilter	KeyRepeat( WgKey keycode );

	static WgMsgFilter	KeyPressNative( int native_keycode );

	static WgMsgFilter	KeyReleaseNative( int native_keycode );

	static WgMsgFilter	KeyRepeatNative( int native_keycode );

	static WgMsgFilter	Character();
	static WgMsgFilter	Character( unsigned short character);

	static WgMsgFilter	WheelRoll();
	static WgMsgFilter	WheelRoll( int wheel );

	static WgMsgFilter	Select();

	static WgMsgFilter	Toggle();

	static WgMsgFilter	ValueUpdate();

	static WgMsgFilter	RangeUpdate();

	static WgMsgFilter	TextEdit();

	static WgMsgFilter	ItemToggle();
	static WgMsgFilter	ItemToggle( int id );

	static WgMsgFilter	ItemMousePress();
	static WgMsgFilter	ItemMousePress( WgMouseButton button );
	static WgMsgFilter	ItemMousePress( WgMouseButton button, int id );

	static WgMsgFilter	ItemsSelect();

	static WgMsgFilter	ItemsUnselect();

	//TODO: Add missing filters!


	inline bool				FilterMsg( const WgMsgPtr& pMsg ) const { return m_pFilterFunc( pMsg, *this ); }
	inline WgMsgType 		MsgType() const { return m_msgType; }
private:
	WgMsgFilter( WgMsgType msgType, WgFilterFuncPtr pFunc, int data1 = 0, int data2 = 0 )
				: m_msgType(msgType), m_pFilterFunc(pFunc), m_data1(data1), m_data2(data2) 
	{
	}

	WgMsgType			m_msgType;			// Only message type filter accepts, if a specific.
	WgFilterFuncPtr 	m_pFilterFunc;		// Pointer at filter function.
	int					m_data1;			// Additional data for filter function.
	int					m_data2;			// Additional data for filter function.


	static bool _filterDummy( const WgMsgPtr& pMsg, const WgMsgFilter& filter );
	static bool _filterType( const WgMsgPtr& pMsg, const WgMsgFilter& filter );

	static bool _filterPointerMsgs( const WgMsgPtr& pMsg, const WgMsgFilter& filter );
	static bool _filterMouseButtonMsgs( const WgMsgPtr& pMsg, const WgMsgFilter& filter );
	static bool _filterKeyMsgs( const WgMsgPtr& pMsg, const WgMsgFilter& filter );
	static bool _filterNativeKeyMsgs( const WgMsgPtr& pMsg, const WgMsgFilter& filter );
	static bool _filterCharacterMsgs( const WgMsgPtr& pMsg, const WgMsgFilter& filter );
	static bool _filterWheelRollMsgs( const WgMsgPtr& pMsg, const WgMsgFilter& filter );
	static bool _filterItemToggleMsgs( const WgMsgPtr& pMsg, const WgMsgFilter& filter );
	static bool _filterItemMousePressMsgs( const WgMsgPtr& pMsg, const WgMsgFilter& filter );
};


#endif // WG_MSGFILTER_DOT_H
