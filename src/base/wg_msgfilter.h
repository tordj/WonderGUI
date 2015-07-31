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

namespace wg 
{
	
	
	class MsgFilter;
	
	typedef	bool(*WgFilterFuncPtr)(const Msg_p&, const MsgFilter& );
	
	
	class MsgFilter
	{
		friend class MsgRouter;
		
	public:
		MsgFilter() : m_msgType(WG_MSG_DUMMY), m_pFilterFunc(_filterDummy) {}	//Not meant to be public, but has to for compatibility with older compilers.
		
		static MsgFilter	tick();
		static MsgFilter	pointerChange();
	
		static MsgFilter	focusGained();
		static MsgFilter	focusLost();
	
		static MsgFilter	mouseEnter();
	
		static MsgFilter	mouseLeave();
	
		static MsgFilter	mouseMove();
	
		static MsgFilter	mousePress();
		static MsgFilter	mousePress( WgMouseButton button);
	
		static MsgFilter	mouseRelease();
		static MsgFilter	mouseRelease( WgMouseButton button);
	
		static MsgFilter	mouseDrag();
		static MsgFilter	mouseDrag( WgMouseButton button);
	
		static MsgFilter	mouseRepeat();
		static MsgFilter	mouseRepeat( WgMouseButton button);
	
		static MsgFilter	mouseClick();
		static MsgFilter	mouseClick( WgMouseButton button);
	
		static MsgFilter	mouseDoubleClick();
		static MsgFilter	mouseDoubleClick( WgMouseButton button);
	
		static MsgFilter	keyPress();
		static MsgFilter	keyPress( WgKey translated_keycode );
	
		static MsgFilter	keyRelease();
		static MsgFilter	keyRelease( WgKey translated_keycode );
	
		static MsgFilter	keyRepeat();
		static MsgFilter	keyRepeat( WgKey keycode );
	
		static MsgFilter	keyPressNative( int native_keycode );
	
		static MsgFilter	keyReleaseNative( int native_keycode );
	
		static MsgFilter	keyRepeatNative( int native_keycode );
	
		static MsgFilter	character();
		static MsgFilter	character( unsigned short character);
	
		static MsgFilter	wheelRoll();
		static MsgFilter	wheelRoll( int wheel );
	
		static MsgFilter	select();
	
		static MsgFilter	toggle();
	
		static MsgFilter	valueUpdate();
	
		static MsgFilter	rangeUpdate();
	
		static MsgFilter	textEdit();
	
		static MsgFilter	itemToggle();
		static MsgFilter	itemToggle( int id );
	
		static MsgFilter	itemMousePress();
		static MsgFilter	itemMousePress( WgMouseButton button );
		static MsgFilter	itemMousePress( WgMouseButton button, int id );
	
		static MsgFilter	itemsSelect();
	
		static MsgFilter	itemsUnselect();
	
		//TODO: Add missing filters!
	
	
		inline bool				filterMsg( const Msg_p& pMsg ) const { return m_pFilterFunc( pMsg, *this ); }
		inline MsgType 		msgType() const { return m_msgType; }
	private:
		MsgFilter( MsgType msgType, WgFilterFuncPtr pFunc, int data1 = 0, int data2 = 0 )
					: m_msgType(msgType), m_pFilterFunc(pFunc), m_data1(data1), m_data2(data2) 
		{
		}
	
		MsgType			m_msgType;			// Only message type filter accepts, if a specific.
		WgFilterFuncPtr 	m_pFilterFunc;		// Pointer at filter function.
		int					m_data1;			// Additional data for filter function.
		int					m_data2;			// Additional data for filter function.
	
	
		static bool _filterDummy( const Msg_p& pMsg, const MsgFilter& filter );
		static bool _filterType( const Msg_p& pMsg, const MsgFilter& filter );
	
		static bool _filterPointerMsgs( const Msg_p& pMsg, const MsgFilter& filter );
		static bool _filterMouseButtonMsgs( const Msg_p& pMsg, const MsgFilter& filter );
		static bool _filterKeyMsgs( const Msg_p& pMsg, const MsgFilter& filter );
		static bool _filterNativeKeyMsgs( const Msg_p& pMsg, const MsgFilter& filter );
		static bool _filterCharacterMsgs( const Msg_p& pMsg, const MsgFilter& filter );
		static bool _filterWheelRollMsgs( const Msg_p& pMsg, const MsgFilter& filter );
		static bool _filterItemToggleMsgs( const Msg_p& pMsg, const MsgFilter& filter );
		static bool _filterItemMousePressMsgs( const Msg_p& pMsg, const MsgFilter& filter );
	};
	
	

} // namespace wg
#endif // WG_MSGFILTER_DOT_H
