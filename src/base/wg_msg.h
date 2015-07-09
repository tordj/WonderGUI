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

#ifndef WG_MSG_DOT_H
#define WG_MSG_DOT_H

#include <string>

#ifndef WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

#ifndef WG_USERDEFINES_DOT_H
#	include <wg_userdefines.h>
#endif

#ifndef WG_GEO_DOT_H
#	include <wg_geo.h>
#endif

#ifndef WG_POINTERS_DOT_H
#	include <wg_pointers.h>
#endif

#ifndef WG_WIDGET_DOT_H
#	include <wg_widget.h>
#endif

/*#ifndef WG_EDITTEXT_DOT_H
#	include <wg_edittext.h>
#endif
*/

class WgMsgRouter;
class WgWidget;
class WgButton;
class WgCheckBox;
class WgRadioButton;
class WgAnimPlayer;
class WgTablist;
class WgValueDisplay;
class WgValueEditor;
class WgScrollbar;
class WgMenu;
class WgPopupLayer;
class WgModalLayer;


class WgLegacyText;
class WgLegacyModText;
class WgEditText;

typedef	WgIStrongPtr<WgLegacyText,WgInterfacePtr>	WgLegacyTextPtr;
typedef	WgIWeakPtr<WgLegacyText,WgInterfaceWeakPtr> WgLegacyTextWeakPtr;

typedef	WgIStrongPtr<WgLegacyModText,WgLegacyTextPtr>	WgLegacyModTextPtr;
typedef	WgIWeakPtr<WgLegacyModText,WgLegacyTextWeakPtr>	WgLegacyModTextWeakPtr;

typedef	WgIStrongPtr<WgEditText,WgLegacyModTextPtr>	WgEditTextPtr;
typedef	WgIWeakPtr<WgEditText,WgLegacyModTextWeakPtr>	WgEditTextWeakPtr;


typedef WgWeakPtr<WgWidget,WgReceiverWeakPtr> WgWidgetWeakPtr;

class WgMsg;
typedef	WgStrongPtr<WgMsg,WgObjectPtr>		WgMsgPtr;
typedef	WgWeakPtr<WgMsg,WgObjectWeakPtr>	WgMsgWeakPtr;

class WgFocusGainedMsg;
typedef	WgStrongPtr<WgFocusGainedMsg,WgMsgPtr>		WgFocusGainedMsgPtr;
typedef	WgWeakPtr<WgFocusGainedMsg,WgMsgWeakPtr>	WgFocusGainedMsgWeakPtr;

class WgFocusLostMsg;
typedef	WgStrongPtr<WgFocusLostMsg,WgMsgPtr>		WgFocusLostMsgPtr;
typedef	WgWeakPtr<WgFocusLostMsg,WgMsgWeakPtr>	WgFocusLostMsgWeakPtr;

class WgMouseEnterMsg;
typedef	WgStrongPtr<WgMouseEnterMsg,WgMsgPtr>		WgMouseEnterMsgPtr;
typedef	WgWeakPtr<WgMouseEnterMsg,WgMsgWeakPtr>	WgMouseEnterMsgWeakPtr;

class WgMouseLeaveMsg;
typedef	WgStrongPtr<WgMouseLeaveMsg,WgMsgPtr>		WgMouseLeaveMsgPtr;
typedef	WgWeakPtr<WgMouseLeaveMsg,WgMsgWeakPtr>	WgMouseLeaveMsgWeakPtr;

class WgMouseMoveMsg;
typedef	WgStrongPtr<WgMouseMoveMsg,WgMsgPtr>		WgMouseMoveMsgPtr;
typedef	WgWeakPtr<WgMouseMoveMsg,WgMsgWeakPtr>	WgMouseMoveMsgWeakPtr;

class WgMousePositionMsg;
typedef	WgStrongPtr<WgMousePositionMsg,WgMsgPtr>		WgMousePositionMsgPtr;
typedef	WgWeakPtr<WgMousePositionMsg,WgMsgWeakPtr>	WgMousePositionMsgWeakPtr;

class WgMouseButtonMsg;
typedef	WgStrongPtr<WgMouseButtonMsg,WgMsgPtr>		WgMouseButtonMsgPtr;
typedef	WgWeakPtr<WgMouseButtonMsg,WgMsgWeakPtr>	WgMouseButtonMsgWeakPtr;

class WgMousePressMsg;
typedef	WgStrongPtr<WgMousePressMsg,WgMouseButtonMsgPtr>		WgMousePressMsgPtr;
typedef	WgWeakPtr<WgMousePressMsg,WgMouseButtonMsgWeakPtr>	WgMousePressMsgWeakPtr;

class WgMouseReleaseMsg;
typedef	WgStrongPtr<WgMouseReleaseMsg,WgMouseButtonMsgPtr>		WgMouseReleaseMsgPtr;
typedef	WgWeakPtr<WgMouseReleaseMsg,WgMouseButtonMsgWeakPtr>	WgMouseReleaseMsgWeakPtr;

class WgMouseRepeatMsg;
typedef	WgStrongPtr<WgMouseRepeatMsg,WgMouseButtonMsgPtr>		WgMouseRepeatMsgPtr;
typedef	WgWeakPtr<WgMouseRepeatMsg,WgMouseButtonMsgWeakPtr>	WgMouseRepeatMsgWeakPtr;

class WgMouseClickMsg;
typedef	WgStrongPtr<WgMouseClickMsg,WgMouseButtonMsgPtr>		WgMouseClickMsgPtr;
typedef	WgWeakPtr<WgMouseClickMsg,WgMouseButtonMsgWeakPtr>	WgMouseClickMsgWeakPtr;

class WgMouseDoubleClickMsg;
typedef	WgStrongPtr<WgMouseDoubleClickMsg,WgMouseButtonMsgPtr>		WgMouseDoubleClickMsgPtr;
typedef	WgWeakPtr<WgMouseDoubleClickMsg,WgMouseButtonMsgWeakPtr>	WgMouseDoubleClickMsgWeakPtr;

class WgMouseDragMsg;
typedef	WgStrongPtr<WgMouseDragMsg,WgMouseButtonMsgPtr>		WgMouseDragMsgPtr;
typedef	WgWeakPtr<WgMouseDragMsg,WgMouseButtonMsgWeakPtr>	WgMouseDragMsgWeakPtr;

class WgKeyMsg;
typedef	WgStrongPtr<WgKeyMsg,WgMsgPtr>		WgKeyMsgPtr;
typedef	WgWeakPtr<WgKeyMsg,WgMsgWeakPtr>	WgKeyMsgWeakPtr;

class WgKeyPressMsg;
typedef	WgStrongPtr<WgKeyPressMsg,WgKeyMsgPtr>		WgKeyPressMsgPtr;
typedef	WgWeakPtr<WgKeyPressMsg,WgKeyMsgWeakPtr>	WgKeyPressMsgWeakPtr;

class WgKeyReleaseMsg;
typedef	WgStrongPtr<WgKeyReleaseMsg,WgKeyMsgPtr>		WgKeyReleaseMsgPtr;
typedef	WgWeakPtr<WgKeyReleaseMsg,WgKeyMsgWeakPtr>	WgKeyReleaseMsgWeakPtr;

class WgKeyRepeatMsg;
typedef	WgStrongPtr<WgKeyRepeatMsg,WgKeyMsgPtr>		WgKeyRepeatMsgPtr;
typedef	WgWeakPtr<WgKeyRepeatMsg,WgKeyMsgWeakPtr>	WgKeyRepeatMsgWeakPtr;

class WgCharacterMsg;
typedef	WgStrongPtr<WgCharacterMsg,WgMsgPtr>		WgCharacterMsgPtr;
typedef	WgWeakPtr<WgCharacterMsg,WgMsgWeakPtr>	WgCharacterMsgWeakPtr;

class WgWheelRollMsg;
typedef	WgStrongPtr<WgWheelRollMsg,WgMsgPtr>		WgWheelRollMsgPtr;
typedef	WgWeakPtr<WgWheelRollMsg,WgMsgWeakPtr>	WgWheelRollMsgWeakPtr;

class WgTickMsg;
typedef	WgStrongPtr<WgTickMsg,WgMsgPtr>		WgTickMsgPtr;
typedef	WgWeakPtr<WgTickMsg,WgMsgWeakPtr>	WgTickMsgWeakPtr;

class WgPointerChangeMsg;
typedef	WgStrongPtr<WgPointerChangeMsg,WgMsgPtr>		WgPointerChangeMsgPtr;
typedef	WgWeakPtr<WgPointerChangeMsg,WgMsgWeakPtr>	WgPointerChangeMsgWeakPtr;

class WgSelectMsg;
typedef	WgStrongPtr<WgSelectMsg,WgMsgPtr>		WgSelectMsgPtr;
typedef	WgWeakPtr<WgSelectMsg,WgMsgWeakPtr>	WgSelectMsgWeakPtr;

class WgToggleMsg;
typedef	WgStrongPtr<WgToggleMsg,WgMsgPtr>		WgToggleMsgPtr;
typedef	WgWeakPtr<WgToggleMsg,WgMsgWeakPtr>	WgToggleMsgWeakPtr;

class WgValueUpdateMsg;
typedef	WgStrongPtr<WgValueUpdateMsg,WgMsgPtr>		WgValueUpdateMsgPtr;
typedef	WgWeakPtr<WgValueUpdateMsg,WgMsgWeakPtr>	WgValueUpdateMsgWeakPtr;

class WgRangeUpdateMsg;
typedef	WgStrongPtr<WgRangeUpdateMsg,WgMsgPtr>		WgRangeUpdateMsgPtr;
typedef	WgWeakPtr<WgRangeUpdateMsg,WgMsgWeakPtr>	WgRangeUpdateMsgWeakPtr;

class WgTextEditMsg;
typedef	WgStrongPtr<WgTextEditMsg,WgMsgPtr>		WgTextEditMsgPtr;
typedef	WgWeakPtr<WgTextEditMsg,WgMsgWeakPtr>	WgTextEditMsgWeakPtr;

class WgItemMsg;
typedef	WgStrongPtr<WgItemMsg,WgMsgPtr>		WgItemMsgPtr;
typedef	WgWeakPtr<WgItemMsg,WgMsgWeakPtr>	WgItemMsgWeakPtr;

class WgItemListMsg;
typedef	WgStrongPtr<WgItemListMsg,WgMsgPtr>		WgItemListMsgPtr;
typedef	WgWeakPtr<WgItemListMsg,WgMsgWeakPtr>	WgItemListMsgWeakPtr;

class WgItemsSelectMsg;
typedef	WgStrongPtr<WgItemsSelectMsg,WgItemListMsgPtr>		WgItemsSelectMsgPtr;
typedef	WgWeakPtr<WgItemsSelectMsg,WgItemListMsgWeakPtr>	WgItemsSelectMsgWeakPtr;

class WgItemsUnselectMsg;
typedef	WgStrongPtr<WgItemsUnselectMsg,WgItemListMsgPtr>		WgItemsUnselectMsgPtr;
typedef	WgWeakPtr<WgItemsUnselectMsg,WgItemListMsgWeakPtr>	WgItemsUnselectMsgWeakPtr;

class WgItemToggleMsg;
typedef	WgStrongPtr<WgItemToggleMsg,WgItemMsgPtr>		WgItemToggleMsgPtr;
typedef	WgWeakPtr<WgItemToggleMsg,WgItemMsgWeakPtr>	WgItemToggleMsgWeakPtr;

class WgItemMousePressMsg;
typedef	WgStrongPtr<WgItemMousePressMsg,WgItemMsgPtr>		WgItemMousePressMsgPtr;
typedef	WgWeakPtr<WgItemMousePressMsg,WgItemMsgWeakPtr>	WgItemMousePressMsgWeakPtr;

class WgPopupClosedMsg;
typedef	WgStrongPtr<WgPopupClosedMsg,WgMsgPtr>		WgPopupClosedMsgPtr;
typedef	WgWeakPtr<WgPopupClosedMsg,WgMsgWeakPtr>	WgPopupClosedMsgWeakPtr;

class WgModalMoveOutsideMsg;
typedef	WgStrongPtr<WgModalMoveOutsideMsg,WgMsgPtr>		WgModalMoveOutsideMsgPtr;
typedef	WgWeakPtr<WgModalMoveOutsideMsg,WgMsgWeakPtr>	WgModalMoveOutsideMsgWeakPtr;

class WgModalBlockedPressMsg;
typedef	WgStrongPtr<WgModalBlockedPressMsg,WgMsgPtr>		WgModalBlockedPressMsgPtr;
typedef	WgWeakPtr<WgModalBlockedPressMsg,WgMsgWeakPtr>	WgModalBlockedPressMsgWeakPtr;

class WgModalBlockedReleaseMsg;
typedef	WgStrongPtr<WgModalBlockedReleaseMsg,WgMsgPtr>		WgModalBlockedReleaseMsgPtr;
typedef	WgWeakPtr<WgModalBlockedReleaseMsg,WgMsgWeakPtr>	WgModalBlockedReleaseMsgWeakPtr;

//

class WgLinkMsg;
typedef	WgStrongPtr<WgLinkMsg,WgMsgPtr>		WgLinkMsgPtr;
typedef	WgWeakPtr<WgLinkMsg,WgMsgWeakPtr>	WgLinkMsgWeakPtr;

class WgLinkSelectMsg;
typedef	WgStrongPtr<WgLinkSelectMsg,WgLinkMsgPtr>		WgLinkSelectMsgPtr;
typedef	WgWeakPtr<WgLinkSelectMsg,WgLinkMsgWeakPtr>		WgLinkSelectMsgWeakPtr;

class WgLinkMouseEnterMsg;
typedef	WgStrongPtr<WgLinkMouseEnterMsg,WgLinkMsgPtr>		WgLinkMouseEnterMsgPtr;
typedef	WgWeakPtr<WgLinkMouseEnterMsg,WgLinkMsgWeakPtr>		WgLinkMouseEnterMsgWeakPtr;

class WgLinkMouseLeaveMsg;
typedef	WgStrongPtr<WgLinkMouseLeaveMsg,WgLinkMsgPtr>		WgLinkMouseLeaveMsgPtr;
typedef	WgWeakPtr<WgLinkMouseLeaveMsg,WgLinkMsgWeakPtr>		WgLinkMouseLeaveMsgWeakPtr;

class WgLinkMouseButtonMsg;
typedef	WgStrongPtr<WgLinkMouseButtonMsg,WgLinkMsgPtr>		WgLinkMouseButtonMsgPtr;
typedef	WgWeakPtr<WgLinkMouseButtonMsg,WgLinkMsgWeakPtr>		WgLinkMouseButtonMsgWeakPtr;

class WgLinkMousePressMsg;
typedef	WgStrongPtr<WgLinkMousePressMsg,WgLinkMouseButtonMsgPtr>		WgLinkMousePressMsgPtr;
typedef	WgWeakPtr<WgLinkMousePressMsg,WgLinkMouseButtonMsgWeakPtr>		WgLinkMousePressMsgWeakPtr;

class WgLinkMouseRepeatMsg;
typedef	WgStrongPtr<WgLinkMouseRepeatMsg,WgLinkMouseButtonMsgPtr>		WgLinkMouseRepeatMsgPtr;
typedef	WgWeakPtr<WgLinkMouseRepeatMsg,WgLinkMouseButtonMsgWeakPtr>		WgLinkMouseRepeatMsgWeakPtr;

class WgLinkMouseReleaseMsg;
typedef	WgStrongPtr<WgLinkMouseReleaseMsg,WgLinkMouseButtonMsgPtr>		WgLinkMouseReleaseMsgPtr;
typedef	WgWeakPtr<WgLinkMouseReleaseMsg,WgLinkMouseButtonMsgWeakPtr>		WgLinkMouseReleaseMsgWeakPtr;

class WgLinkMouseClickMsg;
typedef	WgStrongPtr<WgLinkMouseClickMsg,WgLinkMouseButtonMsgPtr>		WgLinkMouseClickMsgPtr;
typedef	WgWeakPtr<WgLinkMouseClickMsg,WgLinkMouseButtonMsgWeakPtr>		WgLinkMouseClickMsgWeakPtr;

class WgLinkMouseDoubleClickMsg;
typedef	WgStrongPtr<WgLinkMouseDoubleClickMsg,WgLinkMouseButtonMsgPtr>		WgLinkMouseDoubleClickMsgPtr;
typedef	WgWeakPtr<WgLinkMouseDoubleClickMsg,WgLinkMouseButtonMsgWeakPtr>		WgLinkMouseDoubleClickMsgWeakPtr;

//____ WgMsg _________________________________________________________________
/**
 * @brief Base class for all messages.
 *
 * Base class for all messages.
 */


class WgMsg : public WgObject
{
	friend class WgMsgRouter;
	friend class WgWidget;

	public:

		bool				IsInstanceOf( const char * pClassName ) const;
		const char *		ClassName( void ) const;
		static const char	CLASSNAME[];
		static WgMsgPtr	Cast( const WgObjectPtr& pObject );

		WgMsgType			Type() const { return m_type; }
		int64_t				Timestamp() const { return m_timestamp; }
		bool				HasSource() const { return m_pSource; } 
		WgObject *			SourceRawPtr() const { return m_pSource.RawPtr(); }
		WgObjectWeakPtr		Source() const { return m_pSource; } 
		WgModifierKeys		ModKeys() const { return m_modKeys; }
		WgCoord				PointerPos() const { return m_pointerPos; }
			
		bool				IsMouseMsg() const;
		bool				IsMouseButtonMsg() const;
		bool				IsKeyMsg() const;

		bool				HasCopyTo() { return m_pCopyTo; }
		WgReceiverPtr		GetCopyTo() { return m_pCopyTo; }
		bool				HasRepost() { return m_pRepostSource; }

		void				SetCopyTo( const WgReceiverPtr& pReceiver );
		void				SetRepost( const WgObjectPtr& pSource, const WgReceiverPtr& pCopyTo );
		void				Swallow();
		bool				DoRepost();

	protected:
		WgMsg() : m_type(WG_MSG_DUMMY), m_modKeys(WG_MODKEY_NONE), m_timestamp(0) {}
		virtual ~WgMsg() {}
		
		WgMsgType			m_type;				// Type of message
		int64_t				m_timestamp;		// Timestamp of posting this message.
		WgObjectPtr			m_pSource;			// The source of this message, if any.
		WgReceiverPtr		m_pCopyTo;			// Receiver to receive a copy of this message, if any.
		WgObjectPtr			m_pRepostSource;	// Object to repost this message from, if any.
		WgReceiverPtr		m_pRepostCopyTo;	// Receiver to copy this message to when reposting, if any.

		WgModifierKeys		m_modKeys;			// Modifier keys pressed when message posted.
		WgCoord				m_pointerPos;		// Screen position of pointer.
};

//____ WgMouseButtonMsg ______________________________________________________
/**
 * @brief Base class for all mouse button messages.
 * 
 */

class WgMouseButtonMsg : public WgMsg
{
	friend class WgInputHandler;
	friend class WgMsgRouter;
public:
	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgMouseButtonMsgPtr	Cast( const WgObjectPtr& pObject );

	WgMouseButton		Button() const { return m_button; }
protected:
	WgMouseButtonMsg(WgMouseButton button) : m_button(button) {}

	WgMouseButton		m_button;
};

//____ WgKeyMsg ______________________________________________________________
/**
 * @brief Base class for all keyboard messages.
 *
 * Base class for all keyboard messages.
 */

class WgKeyMsg : public WgMsg
{
	friend class WgInputHandler;
	friend class WgMsgRouter;
public:
	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgKeyMsgPtr	Cast( const WgObjectPtr& pObject );

	int		NativeKeyCode() const { return m_nativeKeyCode; }
	int		TranslatedKeyCode() const { return m_translatedKeyCode; }
	bool	IsCursorKey() const;
	bool	IsMovementKey() const;
protected:
	WgKeyMsg( int nativeKeyCode ) : m_nativeKeyCode(nativeKeyCode), m_translatedKeyCode(0) {}

	int		m_nativeKeyCode;
	int		m_translatedKeyCode;
};

//____ WgFocusGainedMsg ______________________________________________________

class WgFocusGainedMsg : public WgMsg
{
	friend class WgInputHandler;
	friend class WgMsgRouter;
public:
	static WgFocusGainedMsgPtr	Create() { return new WgFocusGainedMsg(); }

	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgFocusGainedMsgPtr	Cast( const WgObjectPtr& pObject );
protected:
	WgFocusGainedMsg();
};

//____ WgFocusLostMsg ________________________________________________________

class WgFocusLostMsg : public WgMsg
{
	friend class WgInputHandler;
	friend class WgMsgRouter;
public:
	static WgFocusLostMsgPtr	Create() { return new WgFocusLostMsg(); }

	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgFocusLostMsgPtr	Cast( const WgObjectPtr& pObject );
protected:
	WgFocusLostMsg();
};

//____ WgMouseEnterMsg _______________________________________________________

class WgMouseEnterMsg : public WgMsg
{
	friend class WgInputHandler;
	friend class WgMsgRouter;
public:
	static WgMouseEnterMsgPtr			Create( const WgCoord& pos ) { return new WgMouseEnterMsg(pos); }

	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgMouseEnterMsgPtr	Cast( const WgObjectPtr& pObject );
protected:
	WgMouseEnterMsg( const WgCoord& pos );
	WgMouseEnterMsg( WgWidget * pWidget );
};

//____ WgMouseLeaveMsg _______________________________________________________

class WgMouseLeaveMsg : public WgMsg
{
	friend class WgInputHandler;
	friend class WgMsgRouter;
public:
	static WgMouseLeaveMsgPtr			Create() { return new WgMouseLeaveMsg(); }

	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgMouseLeaveMsgPtr	Cast( const WgObjectPtr& pObject );
protected:
	WgMouseLeaveMsg();
	WgMouseLeaveMsg( WgWidget * pWidget );
};

//____ WgMouseMoveMsg ________________________________________________________

class WgMouseMoveMsg : public WgMsg
{
	friend class WgInputHandler;
	friend class WgMsgRouter;
public:
	static WgMouseMoveMsgPtr			Create( const WgCoord& pos ) { return new WgMouseMoveMsg(pos); }

	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgMouseMoveMsgPtr	Cast( const WgObjectPtr& pObject );
protected:
	WgMouseMoveMsg( WgWidget * pWidget );
	WgMouseMoveMsg( const WgCoord& pos );
};

//____ WgMousePressMsg _______________________________________________________

class WgMousePressMsg : public WgMouseButtonMsg
{
	friend class WgInputHandler;
	friend class WgMsgRouter;
public:
	static WgMousePressMsgPtr			Create( WgMouseButton button ) { return new WgMousePressMsg(button); }

	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgMousePressMsgPtr	Cast( const WgObjectPtr& pObject );
protected:
	WgMousePressMsg( WgMouseButton button, WgWidget * pWidget );
	WgMousePressMsg( WgMouseButton button );
};

//____ WgMouseReleaseMsg _____________________________________________________

class WgMouseReleaseMsg : public WgMouseButtonMsg
{
	friend class WgInputHandler;
	friend class WgMsgRouter;
public:
	static WgMouseReleaseMsgPtr		Create( WgMouseButton button ) { return new WgMouseReleaseMsg(button); }

	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgMouseReleaseMsgPtr	Cast( const WgObjectPtr& pObject );

	bool			PressInside() const;
	bool			ReleaseInside() const;

protected:
	WgMouseReleaseMsg( WgMouseButton button );
	WgMouseReleaseMsg( WgMouseButton button, WgWidget * pWidget, bool bPressInside, bool bReleaseInside );

	bool			m_bPressInside;
	bool			m_bReleaseInside;
};

//____ WgKeyPressMsg _________________________________________________________

class WgKeyPressMsg : public WgKeyMsg
{
	friend class WgInputHandler;
	friend class WgMsgRouter;
public:
	static WgKeyPressMsgPtr			Create( int native_keycode ) { return new WgKeyPressMsg(native_keycode); }

	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgKeyPressMsgPtr	Cast( const WgObjectPtr& pObject );
protected:
	WgKeyPressMsg( int native_keycode );
	WgKeyPressMsg( int native_keycode, WgWidget * pWidget );
};

//____ WgKeyReleaseMsg _______________________________________________________

class WgKeyReleaseMsg : public WgKeyMsg
{
	friend class WgInputHandler;
	friend class WgMsgRouter;
public:
	static WgKeyReleaseMsgPtr			Create( int native_keycode ) { return new WgKeyReleaseMsg(native_keycode); }

	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgKeyReleaseMsgPtr	Cast( const WgObjectPtr& pObject );
protected:
	WgKeyReleaseMsg( int native_keycode );
	WgKeyReleaseMsg( int native_keycode, WgWidget * pWidget );
};

//____ WgCharacterMsg ________________________________________________________

class WgCharacterMsg : public WgMsg
{
	friend class WgInputHandler;
	friend class WgMsgRouter;
public:
	static WgCharacterMsgPtr			Create( unsigned short character ) { return new WgCharacterMsg(character); }

	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgCharacterMsgPtr	Cast( const WgObjectPtr& pObject );

	unsigned short	Char() const;
protected:
	WgCharacterMsg( unsigned short character );
	WgCharacterMsg( unsigned short character, WgWidget * pWidget );
protected:
	unsigned short	m_char;
};

//____ WgWheelRollMsg ________________________________________________________

class WgWheelRollMsg : public WgMsg
{
	friend class WgInputHandler;
	friend class WgMsgRouter;
public:
	static WgWheelRollMsgPtr			Create( int wheel, int distance ) { return new WgWheelRollMsg(wheel,distance); }

	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgWheelRollMsgPtr	Cast( const WgObjectPtr& pObject );

	int			Wheel() const;
	int			Distance() const;
protected:
	WgWheelRollMsg( int wheel, int distance );
	WgWheelRollMsg( int wheel, int distance, WgWidget * pWidget );

	int			m_wheel;
	int			m_distance;
};

//____ WgTickMsg _____________________________________________________________

class WgTickMsg : public WgMsg
{
	friend class WgInputHandler;
	friend class WgMsgRouter;
public:
	static WgTickMsgPtr				Create( int ms ) { return new WgTickMsg(ms); }

	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgTickMsgPtr	Cast( const WgObjectPtr& pObject );

	int				Millisec() const;
protected:
	WgTickMsg( int ms );
	WgTickMsg( int ms, WgReceiver * pReceiver );

	int			m_millisec;
};

//____ WgPointerChangeMsg ____________________________________________________

class WgPointerChangeMsg : public WgMsg
{
	friend class WgInputHandler;
	friend class WgMsgRouter;
public:
	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgPointerChangeMsgPtr	Cast( const WgObjectPtr& pObject );

	WgPointerStyle	Style() const;
		
protected:
	WgPointerChangeMsg( WgPointerStyle style );
		
	WgPointerStyle	m_style;
};


//____ WgSelectMsg ___________________________________________________________

class WgSelectMsg : public WgMsg
{
public:
	WgSelectMsg( WgObject * pSource );

	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgSelectMsgPtr	Cast( const WgObjectPtr& pObject );
};

//____ WgToggleMsg ___________________________________________________________

class WgToggleMsg : public WgMsg
{
public:
	WgToggleMsg( WgObject * pSource, bool bSet );

	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgToggleMsgPtr	Cast( const WgObjectPtr& pObject );

	bool		IsSet() const;

private:
	bool	m_bSet;
};

//____ WgValueUpdateMsg ______________________________________________________

class WgValueUpdateMsg : public WgMsg
{
public:
	WgValueUpdateMsg( WgObject * pSource, int64_t value, double fraction, bool bFinal );

	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgValueUpdateMsgPtr	Cast( const WgObjectPtr& pObject );

	int64_t		Value() const;
	double		Fraction() const;
	bool		IsFinal() const;

protected:
	int64_t		m_value;
	double		m_fraction;
	bool		m_bFinal;
};

//____ WgRangeUpdateMsg ______________________________________________________

class WgRangeUpdateMsg : public WgMsg
{
public:
	WgRangeUpdateMsg( WgObject * pSource, int intOfs, int intLength, double fracOfs, double fracLength, bool bFinal ); 

	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgRangeUpdateMsgPtr	Cast( const WgObjectPtr& pObject );

	int			Offset() const;
	int			Length() const;
	double		FracOffset() const;
	double		FracLength() const;
	bool		IsFinal() const;

	int			m_ofs;
	int			m_length;
	double		m_fracOfs;
	double		m_fracLength;
	bool		m_bFinal;
};

//____ WgTextEditMsg _________________________________________________________

class WgTextEditMsg : public WgMsg
{
public:
	WgTextEditMsg( const WgEditTextPtr& pText, bool bFinal );

	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgTextEditMsgPtr	Cast( const WgObjectPtr& pObject );

	WgEditTextPtr		Text() const;
	bool				IsFinal() const;

protected:
	WgEditTextPtr		m_pText;
	bool				m_bFinal;
};


//____

class WgItemInfo
{
public:
	WgItemInfo(): id(-1) {}
	WgItemInfo( int _id, const WgObjectPtr& _pObject ) : id(_id), pObject(_pObject) {}

	int				id;
	WgObjectPtr		pObject;
};

//____ WgItemMsg ____________________________________________________________

class WgItemMsg : public WgMsg
{
public:
	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgItemMsgPtr	Cast( const WgObjectPtr& pObject );

	int				ItemId() const;
	WgObjectPtr		ItemObject() const;

protected:
	WgItemMsg( WgObject * pSource, int itemId, const WgObjectPtr& pItemObject );

	WgItemInfo		m_item;
};

//____ WgItemToggleMsg ______________________________________________________

class WgItemToggleMsg : public WgItemMsg
{
public:
	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgItemToggleMsgPtr	Cast( const WgObjectPtr& pObject );

	bool		IsSet() const;

//protected:
	WgItemToggleMsg( WgObject * pSource, int itemId, const WgObjectPtr& pItemObject, bool bSet );

	bool		m_bSet;
};

//____ WgItemMousePressMsg ______________________________________________________

class WgItemMousePressMsg : public WgItemMsg
{
public:
	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgItemMousePressMsgPtr	Cast( const WgObjectPtr& pObject );

	WgMouseButton		Button() const;
//protected:
	WgItemMousePressMsg( WgObject * pSource, int itemId, const WgObjectPtr& pItemObject, WgMouseButton button );

	WgMouseButton		m_button;
};


//____ WgItemListMsg ___________________________________________________________

class WgItemListMsg : public WgMsg
{
public:
	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgItemListMsgPtr	Cast( const WgObjectPtr& pObject );

	int					NbItems() const;
	const WgItemInfo *	Items() const;

protected:
	WgItemListMsg( WgObject * pSource, int nbItems, WgItemInfo * pItems );
	virtual ~WgItemListMsg();
	int				m_nbItems;
	WgItemInfo *	m_pItems;
};


//____ WgItemsSelectMsg _____________________________________________________

class WgItemsSelectMsg : public WgItemListMsg
{
public:
	WgItemsSelectMsg( WgObject * pSource, int nbItems, WgItemInfo * pItems );

	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgItemsSelectMsgPtr	Cast( const WgObjectPtr& pObject );
};

//____ WgItemsUnselectMsg _____________________________________________________

class WgItemsUnselectMsg : public WgItemListMsg
{
public:
	WgItemsUnselectMsg( WgObject * pSource, int nbItems, WgItemInfo * pItems );

	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgItemsUnselectMsgPtr	Cast( const WgObjectPtr& pObject );
};

//____ WgPopupClosedMsg ______________________________________________________

class WgPopupClosedMsg : public WgMsg
{
	friend class WgPopupLayer;
public:
	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgPopupClosedMsgPtr	Cast( const WgObjectPtr& pObject );

protected:
	WgPopupClosedMsg( WgWidget * pPopup, const WgWidgetWeakPtr& pCaller );

};

//____ WgWidgetModalLayer messages _________________________________________________

class WgModalMoveOutsideMsg : public WgMsg
{
	friend class WgModalLayer;
public:
	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgModalMoveOutsideMsgPtr	Cast( const WgObjectPtr& pObject );

protected:
	WgModalMoveOutsideMsg( WgWidget * pWidget );
};

class WgModalBlockedPressMsg : public WgMouseButtonMsg
{
	friend class WgModalLayer;
public:
	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgModalBlockedPressMsgPtr	Cast( const WgObjectPtr& pObject );

protected:
	WgModalBlockedPressMsg( WgMouseButton button, WgWidget * pModalWidget );
};

class WgModalBlockedReleaseMsg : public WgMouseButtonMsg
{
	friend class WgModalLayer;
public:
	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgModalBlockedReleaseMsgPtr	Cast( const WgObjectPtr& pObject );

protected:
	WgModalBlockedReleaseMsg( WgMouseButton button, WgWidget * pModalWidget );
};

//____ Link messages _________________________________________________________

class WgLinkMsg : public WgMsg
{
public:
	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgLinkMsgPtr	Cast( const WgObjectPtr& pObject );
};

class WgLinkSelectMsg : public WgLinkMsg
{
public:
	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgLinkSelectMsgPtr	Cast( const WgObjectPtr& pObject );

	WgLinkSelectMsg( const WgTextLinkPtr& pLink );
};


class WgLinkMouseEnterMsg : public WgLinkMsg
{
public:
	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgLinkMouseEnterMsgPtr	Cast( const WgObjectPtr& pObject );

	WgLinkMouseEnterMsg( const WgTextLinkPtr& pLink );
};

class WgLinkMouseLeaveMsg : public WgLinkMsg
{
public:
	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgLinkMouseLeaveMsgPtr	Cast( const WgObjectPtr& pObject );

	WgLinkMouseLeaveMsg(  const WgTextLinkPtr& pLink );
};

class WgLinkMouseButtonMsg : public WgLinkMsg
{
public:
	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgLinkMouseButtonMsgPtr	Cast( const WgObjectPtr& pObject );

	WgMouseButton		Button() const;
protected:
	WgMouseButton		m_button;
};

class WgLinkMousePressMsg : public WgLinkMouseButtonMsg
{
public:
	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgLinkMousePressMsgPtr	Cast( const WgObjectPtr& pObject );

	WgLinkMousePressMsg(  const WgTextLinkPtr& pLink, WgMouseButton button );
};

class WgLinkMouseRepeatMsg : public WgLinkMouseButtonMsg
{
public:
	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgLinkMouseRepeatMsgPtr	Cast( const WgObjectPtr& pObject );

	WgLinkMouseRepeatMsg( const WgTextLinkPtr& pLink, WgMouseButton button );
};

class WgLinkMouseReleaseMsg : public WgLinkMouseButtonMsg
{
public:
	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgLinkMouseReleaseMsgPtr	Cast( const WgObjectPtr& pObject );

	WgLinkMouseReleaseMsg( const WgTextLinkPtr& pLink, WgMouseButton button );
};

class WgLinkMouseClickMsg : public WgLinkMouseButtonMsg
{
public:
	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgLinkMouseClickMsgPtr	Cast( const WgObjectPtr& pObject );

	WgLinkMouseClickMsg(  const WgTextLinkPtr& pLink, WgMouseButton button );
};

class WgLinkMouseDoubleClickMsg : public WgLinkMouseButtonMsg
{
public:
	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgLinkMouseDoubleClickMsgPtr	Cast( const WgObjectPtr& pObject );

	WgLinkMouseDoubleClickMsg(  const WgTextLinkPtr& pLink, WgMouseButton button );
};

//____ Internally posted messages ____________________________________________

class WgMousePositionMsg : public WgMsg
{
	friend class WgMsgRouter;
public:
	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgMousePositionMsgPtr	Cast( const WgObjectPtr& pObject );

protected:
	WgMousePositionMsg();
};

class WgMouseDragMsg : public WgMouseButtonMsg
{
	friend class WgInputHandler;
	friend class WgMsgRouter;
protected:
	WgMouseDragMsg( WgMouseButton button, const WgCoord& startPos, const WgCoord& prevPos, const WgCoord& currPos );
	WgMouseDragMsg( WgMouseButton button, WgWidget * pWidget, const WgCoord& orgPos, const WgCoord& prevPos, const WgCoord& currPos );
public:
	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgMouseDragMsgPtr	Cast( const WgObjectPtr& pObject );

	WgCoord			DraggedTotal() const;
	WgCoord			DraggedNow() const;
	WgCoord			StartPos() const;
	WgCoord			PrevPos() const;
	WgCoord			CurrPos() const;
protected:
	WgCoord			m_startPos;
	WgCoord			m_prevPos;
	WgCoord			m_currPos;
};

class WgMouseRepeatMsg : public WgMouseButtonMsg
{
	friend class WgInputHandler;
	friend class WgMsgRouter;
protected:
	WgMouseRepeatMsg( WgMouseButton button, WgWidget * pWidget );
	WgMouseRepeatMsg( WgMouseButton button );
public:
	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgMouseRepeatMsgPtr	Cast( const WgObjectPtr& pObject );
};

class WgMouseClickMsg : public WgMouseButtonMsg
{
	friend class WgInputHandler;
	friend class WgMsgRouter;
protected:
	WgMouseClickMsg( WgMouseButton button );
	WgMouseClickMsg( WgMouseButton button, WgWidget * pWidget );
public:
	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgMouseClickMsgPtr	Cast( const WgObjectPtr& pObject );
};

class WgMouseDoubleClickMsg : public WgMouseButtonMsg
{
	friend class WgInputHandler;
	friend class WgMsgRouter;
public:
	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgMouseDoubleClickMsgPtr	Cast( const WgObjectPtr& pObject );

protected:
	WgMouseDoubleClickMsg( WgMouseButton button );
	WgMouseDoubleClickMsg( WgMouseButton button, WgWidget * pWidget );
};

class WgKeyRepeatMsg : public WgKeyMsg
{
	friend class WgInputHandler;
	friend class WgMsgRouter;
public:
	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgKeyRepeatMsgPtr	Cast( const WgObjectPtr& pObject );

protected:
	WgKeyRepeatMsg( int native_keycode );
	WgKeyRepeatMsg( int native_keycode, WgWidget * pWidget );
};




#endif //WG_MSG_DOT_H
