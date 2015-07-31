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

/*
#ifndef WG_EDITTEXT_DOT_H
#	include <wg_edittext.h>
#endif
*/



namespace wg 
{

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

	typedef	WgIStrongPtr<WgLegacyText,WgInterface_p>	WgLegacyText_p;
	typedef	WgIWeakPtr<WgLegacyText,WgInterface_wp> WgLegacyText_wp;

	typedef	WgIStrongPtr<WgLegacyModText,WgLegacyText_p>	WgLegacyModText_p;
	typedef	WgIWeakPtr<WgLegacyModText,WgLegacyText_wp>	WgLegacyModText_wp;

	typedef	WgIStrongPtr<WgEditText,WgLegacyModText_p>	WgEditText_p;
	typedef	WgIWeakPtr<WgEditText,WgLegacyModText_wp>	WgEditText_wp;


	typedef WgWeakPtr<WgWidget,WgReceiver_wp> WgWidget_wp;

	class WgMsg;
	typedef	WgStrongPtr<WgMsg,WgObject_p>		WgMsg_p;
	typedef	WgWeakPtr<WgMsg,WgObject_wp>	WgMsg_wp;

	class WgFocusGainedMsg;
	typedef	WgStrongPtr<WgFocusGainedMsg,WgMsg_p>		WgFocusGainedMsg_p;
	typedef	WgWeakPtr<WgFocusGainedMsg,WgMsg_wp>	WgFocusGainedMsg_wp;

	class WgFocusLostMsg;
	typedef	WgStrongPtr<WgFocusLostMsg,WgMsg_p>		WgFocusLostMsg_p;
	typedef	WgWeakPtr<WgFocusLostMsg,WgMsg_wp>	WgFocusLostMsg_wp;

	class WgMouseEnterMsg;
	typedef	WgStrongPtr<WgMouseEnterMsg,WgMsg_p>		WgMouseEnterMsg_p;
	typedef	WgWeakPtr<WgMouseEnterMsg,WgMsg_wp>	WgMouseEnterMsg_wp;

	class WgMouseLeaveMsg;
	typedef	WgStrongPtr<WgMouseLeaveMsg,WgMsg_p>		WgMouseLeaveMsg_p;
	typedef	WgWeakPtr<WgMouseLeaveMsg,WgMsg_wp>	WgMouseLeaveMsg_wp;

	class WgMouseMoveMsg;
	typedef	WgStrongPtr<WgMouseMoveMsg,WgMsg_p>		WgMouseMoveMsg_p;
	typedef	WgWeakPtr<WgMouseMoveMsg,WgMsg_wp>	WgMouseMoveMsg_wp;

	class WgMousePositionMsg;
	typedef	WgStrongPtr<WgMousePositionMsg,WgMsg_p>		WgMousePositionMsg_p;
	typedef	WgWeakPtr<WgMousePositionMsg,WgMsg_wp>	WgMousePositionMsg_wp;

	class WgMouseButtonMsg;
	typedef	WgStrongPtr<WgMouseButtonMsg,WgMsg_p>		WgMouseButtonMsg_p;
	typedef	WgWeakPtr<WgMouseButtonMsg,WgMsg_wp>	WgMouseButtonMsg_wp;

	class WgMousePressMsg;
	typedef	WgStrongPtr<WgMousePressMsg,WgMouseButtonMsg_p>		WgMousePressMsg_p;
	typedef	WgWeakPtr<WgMousePressMsg,WgMouseButtonMsg_wp>	WgMousePressMsg_wp;

	class WgMouseReleaseMsg;
	typedef	WgStrongPtr<WgMouseReleaseMsg,WgMouseButtonMsg_p>		WgMouseReleaseMsg_p;
	typedef	WgWeakPtr<WgMouseReleaseMsg,WgMouseButtonMsg_wp>	WgMouseReleaseMsg_wp;

	class WgMouseRepeatMsg;
	typedef	WgStrongPtr<WgMouseRepeatMsg,WgMouseButtonMsg_p>		WgMouseRepeatMsg_p;
	typedef	WgWeakPtr<WgMouseRepeatMsg,WgMouseButtonMsg_wp>	WgMouseRepeatMsg_wp;

	class WgMouseClickMsg;
	typedef	WgStrongPtr<WgMouseClickMsg,WgMouseButtonMsg_p>		WgMouseClickMsg_p;
	typedef	WgWeakPtr<WgMouseClickMsg,WgMouseButtonMsg_wp>	WgMouseClickMsg_wp;

	class WgMouseDoubleClickMsg;
	typedef	WgStrongPtr<WgMouseDoubleClickMsg,WgMouseButtonMsg_p>		WgMouseDoubleClickMsg_p;
	typedef	WgWeakPtr<WgMouseDoubleClickMsg,WgMouseButtonMsg_wp>	WgMouseDoubleClickMsg_wp;

	class WgMouseDragMsg;
	typedef	WgStrongPtr<WgMouseDragMsg,WgMouseButtonMsg_p>		WgMouseDragMsg_p;
	typedef	WgWeakPtr<WgMouseDragMsg,WgMouseButtonMsg_wp>	WgMouseDragMsg_wp;

	class WgKeyMsg;
	typedef	WgStrongPtr<WgKeyMsg,WgMsg_p>		WgKeyMsg_p;
	typedef	WgWeakPtr<WgKeyMsg,WgMsg_wp>	WgKeyMsg_wp;

	class WgKeyPressMsg;
	typedef	WgStrongPtr<WgKeyPressMsg,WgKeyMsg_p>		WgKeyPressMsg_p;
	typedef	WgWeakPtr<WgKeyPressMsg,WgKeyMsg_wp>	WgKeyPressMsg_wp;

	class WgKeyReleaseMsg;
	typedef	WgStrongPtr<WgKeyReleaseMsg,WgKeyMsg_p>		WgKeyReleaseMsg_p;
	typedef	WgWeakPtr<WgKeyReleaseMsg,WgKeyMsg_wp>	WgKeyReleaseMsg_wp;

	class WgKeyRepeatMsg;
	typedef	WgStrongPtr<WgKeyRepeatMsg,WgKeyMsg_p>		WgKeyRepeatMsg_p;
	typedef	WgWeakPtr<WgKeyRepeatMsg,WgKeyMsg_wp>	WgKeyRepeatMsg_wp;

	class WgCharacterMsg;
	typedef	WgStrongPtr<WgCharacterMsg,WgMsg_p>		WgCharacterMsg_p;
	typedef	WgWeakPtr<WgCharacterMsg,WgMsg_wp>	WgCharacterMsg_wp;

	class WgWheelRollMsg;
	typedef	WgStrongPtr<WgWheelRollMsg,WgMsg_p>		WgWheelRollMsg_p;
	typedef	WgWeakPtr<WgWheelRollMsg,WgMsg_wp>	WgWheelRollMsg_wp;

	class WgTickMsg;
	typedef	WgStrongPtr<WgTickMsg,WgMsg_p>		WgTickMsg_p;
	typedef	WgWeakPtr<WgTickMsg,WgMsg_wp>	WgTickMsg_wp;

	class WgPointerChangeMsg;
	typedef	WgStrongPtr<WgPointerChangeMsg,WgMsg_p>		WgPointerChangeMsg_p;
	typedef	WgWeakPtr<WgPointerChangeMsg,WgMsg_wp>	WgPointerChangeMsg_wp;

	class WgSelectMsg;
	typedef	WgStrongPtr<WgSelectMsg,WgMsg_p>		WgSelectMsg_p;
	typedef	WgWeakPtr<WgSelectMsg,WgMsg_wp>	WgSelectMsg_wp;

	class WgToggleMsg;
	typedef	WgStrongPtr<WgToggleMsg,WgMsg_p>		WgToggleMsg_p;
	typedef	WgWeakPtr<WgToggleMsg,WgMsg_wp>	WgToggleMsg_wp;

	class WgValueUpdateMsg;
	typedef	WgStrongPtr<WgValueUpdateMsg,WgMsg_p>		WgValueUpdateMsg_p;
	typedef	WgWeakPtr<WgValueUpdateMsg,WgMsg_wp>	WgValueUpdateMsg_wp;

	class WgRangeUpdateMsg;
	typedef	WgStrongPtr<WgRangeUpdateMsg,WgMsg_p>		WgRangeUpdateMsg_p;
	typedef	WgWeakPtr<WgRangeUpdateMsg,WgMsg_wp>	WgRangeUpdateMsg_wp;

	class WgTextEditMsg;
	typedef	WgStrongPtr<WgTextEditMsg,WgMsg_p>		WgTextEditMsg_p;
	typedef	WgWeakPtr<WgTextEditMsg,WgMsg_wp>	WgTextEditMsg_wp;

	class WgItemMsg;
	typedef	WgStrongPtr<WgItemMsg,WgMsg_p>		WgItemMsg_p;
	typedef	WgWeakPtr<WgItemMsg,WgMsg_wp>	WgItemMsg_wp;

	class WgItemListMsg;
	typedef	WgStrongPtr<WgItemListMsg,WgMsg_p>		WgItemListMsg_p;
	typedef	WgWeakPtr<WgItemListMsg,WgMsg_wp>	WgItemListMsg_wp;

	class WgItemsSelectMsg;
	typedef	WgStrongPtr<WgItemsSelectMsg,WgItemListMsg_p>		WgItemsSelectMsg_p;
	typedef	WgWeakPtr<WgItemsSelectMsg,WgItemListMsg_wp>	WgItemsSelectMsg_wp;

	class WgItemsUnselectMsg;
	typedef	WgStrongPtr<WgItemsUnselectMsg,WgItemListMsg_p>		WgItemsUnselectMsg_p;
	typedef	WgWeakPtr<WgItemsUnselectMsg,WgItemListMsg_wp>	WgItemsUnselectMsg_wp;

	class WgItemToggleMsg;
	typedef	WgStrongPtr<WgItemToggleMsg,WgItemMsg_p>		WgItemToggleMsg_p;
	typedef	WgWeakPtr<WgItemToggleMsg,WgItemMsg_wp>	WgItemToggleMsg_wp;

	class WgItemMousePressMsg;
	typedef	WgStrongPtr<WgItemMousePressMsg,WgItemMsg_p>		WgItemMousePressMsg_p;
	typedef	WgWeakPtr<WgItemMousePressMsg,WgItemMsg_wp>	WgItemMousePressMsg_wp;

	class WgPopupClosedMsg;
	typedef	WgStrongPtr<WgPopupClosedMsg,WgMsg_p>		WgPopupClosedMsg_p;
	typedef	WgWeakPtr<WgPopupClosedMsg,WgMsg_wp>	WgPopupClosedMsg_wp;

	class WgModalMoveOutsideMsg;
	typedef	WgStrongPtr<WgModalMoveOutsideMsg,WgMsg_p>		WgModalMoveOutsideMsg_p;
	typedef	WgWeakPtr<WgModalMoveOutsideMsg,WgMsg_wp>	WgModalMoveOutsideMsg_wp;

	class WgModalBlockedPressMsg;
	typedef	WgStrongPtr<WgModalBlockedPressMsg,WgMsg_p>		WgModalBlockedPressMsg_p;
	typedef	WgWeakPtr<WgModalBlockedPressMsg,WgMsg_wp>	WgModalBlockedPressMsg_wp;

	class WgModalBlockedReleaseMsg;
	typedef	WgStrongPtr<WgModalBlockedReleaseMsg,WgMsg_p>		WgModalBlockedReleaseMsg_p;
	typedef	WgWeakPtr<WgModalBlockedReleaseMsg,WgMsg_wp>	WgModalBlockedReleaseMsg_wp;

	//

	class WgLinkMsg;
	typedef	WgStrongPtr<WgLinkMsg,WgMsg_p>		WgLinkMsg_p;
	typedef	WgWeakPtr<WgLinkMsg,WgMsg_wp>	WgLinkMsg_wp;

	class WgLinkSelectMsg;
	typedef	WgStrongPtr<WgLinkSelectMsg,WgLinkMsg_p>		WgLinkSelectMsg_p;
	typedef	WgWeakPtr<WgLinkSelectMsg,WgLinkMsg_wp>		WgLinkSelectMsg_wp;

	class WgLinkMouseEnterMsg;
	typedef	WgStrongPtr<WgLinkMouseEnterMsg,WgLinkMsg_p>		WgLinkMouseEnterMsg_p;
	typedef	WgWeakPtr<WgLinkMouseEnterMsg,WgLinkMsg_wp>		WgLinkMouseEnterMsg_wp;

	class WgLinkMouseLeaveMsg;
	typedef	WgStrongPtr<WgLinkMouseLeaveMsg,WgLinkMsg_p>		WgLinkMouseLeaveMsg_p;
	typedef	WgWeakPtr<WgLinkMouseLeaveMsg,WgLinkMsg_wp>		WgLinkMouseLeaveMsg_wp;

	class WgLinkMouseButtonMsg;
	typedef	WgStrongPtr<WgLinkMouseButtonMsg,WgLinkMsg_p>		WgLinkMouseButtonMsg_p;
	typedef	WgWeakPtr<WgLinkMouseButtonMsg,WgLinkMsg_wp>		WgLinkMouseButtonMsg_wp;

	class WgLinkMousePressMsg;
	typedef	WgStrongPtr<WgLinkMousePressMsg,WgLinkMouseButtonMsg_p>		WgLinkMousePressMsg_p;
	typedef	WgWeakPtr<WgLinkMousePressMsg,WgLinkMouseButtonMsg_wp>		WgLinkMousePressMsg_wp;

	class WgLinkMouseRepeatMsg;
	typedef	WgStrongPtr<WgLinkMouseRepeatMsg,WgLinkMouseButtonMsg_p>		WgLinkMouseRepeatMsg_p;
	typedef	WgWeakPtr<WgLinkMouseRepeatMsg,WgLinkMouseButtonMsg_wp>		WgLinkMouseRepeatMsg_wp;

	class WgLinkMouseReleaseMsg;
	typedef	WgStrongPtr<WgLinkMouseReleaseMsg,WgLinkMouseButtonMsg_p>		WgLinkMouseReleaseMsg_p;
	typedef	WgWeakPtr<WgLinkMouseReleaseMsg,WgLinkMouseButtonMsg_wp>		WgLinkMouseReleaseMsg_wp;

	class WgLinkMouseClickMsg;
	typedef	WgStrongPtr<WgLinkMouseClickMsg,WgLinkMouseButtonMsg_p>		WgLinkMouseClickMsg_p;
	typedef	WgWeakPtr<WgLinkMouseClickMsg,WgLinkMouseButtonMsg_wp>		WgLinkMouseClickMsg_wp;

	class WgLinkMouseDoubleClickMsg;
	typedef	WgStrongPtr<WgLinkMouseDoubleClickMsg,WgLinkMouseButtonMsg_p>		WgLinkMouseDoubleClickMsg_p;
	typedef	WgWeakPtr<WgLinkMouseDoubleClickMsg,WgLinkMouseButtonMsg_wp>		WgLinkMouseDoubleClickMsg_wp;

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

			bool				isInstanceOf( const char * pClassName ) const;
			const char *		className( void ) const;
			static const char	CLASSNAME[];
			static WgMsg_p	cast( const WgObject_p& pObject );

			WgMsgType			type() const { return m_type; }
			int64_t				timestamp() const { return m_timestamp; }
			bool				hasSource() const { return m_pSource; } 
			WgObject *			sourceRawPtr() const { return m_pSource.rawPtr(); }
			WgObject_wp		source() const { return m_pSource; } 
			WgModifierKeys		modKeys() const { return m_modKeys; }
			WgCoord				pointerPos() const { return m_pointerPos; }
				
			bool				isMouseMsg() const;
			bool				isMouseButtonMsg() const;
			bool				isKeyMsg() const;

			bool				hasCopyTo() { return m_pCopyTo; }
			WgReceiver_p		getCopyTo() { return m_pCopyTo; }
			bool				hasRepost() { return m_pRepostSource; }

			void				setCopyTo( const WgReceiver_p& pReceiver );
			void				setRepost( const WgObject_p& pSource, const WgReceiver_p& pCopyTo );
			void				swallow();
			bool				doRepost();

		protected:
			WgMsg() : m_type(WG_MSG_DUMMY), m_modKeys(WG_MODKEY_NONE), m_timestamp(0) {}
			virtual ~WgMsg() {}
			
			WgMsgType			m_type;				// Type of message
			int64_t				m_timestamp;		// Timestamp of posting this message.
			WgObject_p			m_pSource;			// The source of this message, if any.
			WgReceiver_p		m_pCopyTo;			// Receiver to receive a copy of this message, if any.
			WgObject_p			m_pRepostSource;	// Object to repost this message from, if any.
			WgReceiver_p		m_pRepostCopyTo;	// Receiver to copy this message to when reposting, if any.

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
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static WgMouseButtonMsg_p	cast( const WgObject_p& pObject );

		WgMouseButton		button() const { return m_button; }
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
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static WgKeyMsg_p	cast( const WgObject_p& pObject );

		int		nativeKeyCode() const { return m_nativeKeyCode; }
		int		translatedKeyCode() const { return m_translatedKeyCode; }
		bool	isCursorKey() const;
		bool	isMovementKey() const;
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
		static WgFocusGainedMsg_p	create() { return new WgFocusGainedMsg(); }

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static WgFocusGainedMsg_p	cast( const WgObject_p& pObject );
	protected:
		WgFocusGainedMsg();
	};

	//____ WgFocusLostMsg ________________________________________________________

	class WgFocusLostMsg : public WgMsg
	{
		friend class WgInputHandler;
		friend class WgMsgRouter;
	public:
		static WgFocusLostMsg_p	create() { return new WgFocusLostMsg(); }

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static WgFocusLostMsg_p	cast( const WgObject_p& pObject );
	protected:
		WgFocusLostMsg();
	};

	//____ WgMouseEnterMsg _______________________________________________________

	class WgMouseEnterMsg : public WgMsg
	{
		friend class WgInputHandler;
		friend class WgMsgRouter;
	public:
		static WgMouseEnterMsg_p			create( const WgCoord& pos ) { return new WgMouseEnterMsg(pos); }

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static WgMouseEnterMsg_p	cast( const WgObject_p& pObject );
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
		static WgMouseLeaveMsg_p			create() { return new WgMouseLeaveMsg(); }

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static WgMouseLeaveMsg_p	cast( const WgObject_p& pObject );
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
		static WgMouseMoveMsg_p			create( const WgCoord& pos ) { return new WgMouseMoveMsg(pos); }

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static WgMouseMoveMsg_p	cast( const WgObject_p& pObject );
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
		static WgMousePressMsg_p			create( WgMouseButton button ) { return new WgMousePressMsg(button); }

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static WgMousePressMsg_p	cast( const WgObject_p& pObject );
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
		static WgMouseReleaseMsg_p		create( WgMouseButton button ) { return new WgMouseReleaseMsg(button); }

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static WgMouseReleaseMsg_p	cast( const WgObject_p& pObject );

		bool			pressInside() const;
		bool			releaseInside() const;

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
		static WgKeyPressMsg_p			create( int native_keycode ) { return new WgKeyPressMsg(native_keycode); }

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static WgKeyPressMsg_p	cast( const WgObject_p& pObject );
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
		static WgKeyReleaseMsg_p			create( int native_keycode ) { return new WgKeyReleaseMsg(native_keycode); }

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static WgKeyReleaseMsg_p	cast( const WgObject_p& pObject );
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
		static WgCharacterMsg_p			create( unsigned short character ) { return new WgCharacterMsg(character); }

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static WgCharacterMsg_p	cast( const WgObject_p& pObject );

		unsigned short	character() const;
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
		static WgWheelRollMsg_p			create( int wheel, int distance ) { return new WgWheelRollMsg(wheel,distance); }

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static WgWheelRollMsg_p	cast( const WgObject_p& pObject );

		int			wheel() const;
		int			distance() const;
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
		static WgTickMsg_p				create( int ms ) { return new WgTickMsg(ms); }

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static WgTickMsg_p	cast( const WgObject_p& pObject );

		int				millisec() const;
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
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static WgPointerChangeMsg_p	cast( const WgObject_p& pObject );

		WgPointerStyle	style() const;
			
	protected:
		WgPointerChangeMsg( WgPointerStyle style );
			
		WgPointerStyle	m_style;
	};


	//____ WgSelectMsg ___________________________________________________________

	class WgSelectMsg : public WgMsg
	{
	public:
		WgSelectMsg( WgObject * pSource );

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static WgSelectMsg_p	cast( const WgObject_p& pObject );
	};

	//____ WgToggleMsg ___________________________________________________________

	class WgToggleMsg : public WgMsg
	{
	public:
		WgToggleMsg( WgObject * pSource, bool bSet );

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static WgToggleMsg_p	cast( const WgObject_p& pObject );

		bool		isSet() const;

	private:
		bool	m_bSet;
	};

	//____ WgValueUpdateMsg ______________________________________________________

	class WgValueUpdateMsg : public WgMsg
	{
	public:
		WgValueUpdateMsg( WgObject * pSource, int64_t value, double fraction, bool bFinal );

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static WgValueUpdateMsg_p	cast( const WgObject_p& pObject );

		int64_t		value() const;
		double		fraction() const;
		bool		isFinal() const;

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

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static WgRangeUpdateMsg_p	cast( const WgObject_p& pObject );

		int			offset() const;
		int			length() const;
		double		fracOffset() const;
		double		fracLength() const;
		bool		isFinal() const;

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
		WgTextEditMsg( const WgEditText_p& pText, bool bFinal );

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static WgTextEditMsg_p	cast( const WgObject_p& pObject );

		WgEditText_p		text() const;
		bool				isFinal() const;

	protected:
		WgEditText_p		m_pText;
		bool				m_bFinal;
	};


	//____

	class WgItemInfo
	{
	public:
		WgItemInfo(): id(-1) {}
		WgItemInfo( int _id, const WgObject_p& _pObject ) : id(_id), pObject(_pObject) {}

		int				id;
		WgObject_p		pObject;
	};

	//____ WgItemMsg ____________________________________________________________

	class WgItemMsg : public WgMsg
	{
	public:
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static WgItemMsg_p	cast( const WgObject_p& pObject );

		int				itemId() const;
		WgObject_p		itemObject() const;

	protected:
		WgItemMsg( WgObject * pSource, int itemId, const WgObject_p& pItemObject );

		WgItemInfo		m_item;
	};

	//____ WgItemToggleMsg ______________________________________________________

	class WgItemToggleMsg : public WgItemMsg
	{
	public:
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static WgItemToggleMsg_p	cast( const WgObject_p& pObject );

		bool		isSet() const;

	//protected:
		WgItemToggleMsg( WgObject * pSource, int itemId, const WgObject_p& pItemObject, bool bSet );

		bool		m_bSet;
	};

	//____ WgItemMousePressMsg ______________________________________________________

	class WgItemMousePressMsg : public WgItemMsg
	{
	public:
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static WgItemMousePressMsg_p	cast( const WgObject_p& pObject );

		WgMouseButton		button() const;
	//protected:
		WgItemMousePressMsg( WgObject * pSource, int itemId, const WgObject_p& pItemObject, WgMouseButton button );

		WgMouseButton		m_button;
	};


	//____ WgItemListMsg ___________________________________________________________

	class WgItemListMsg : public WgMsg
	{
	public:
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static WgItemListMsg_p	cast( const WgObject_p& pObject );

		int					nbItems() const;
		const WgItemInfo *	items() const;

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

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static WgItemsSelectMsg_p	cast( const WgObject_p& pObject );
	};

	//____ WgItemsUnselectMsg _____________________________________________________

	class WgItemsUnselectMsg : public WgItemListMsg
	{
	public:
		WgItemsUnselectMsg( WgObject * pSource, int nbItems, WgItemInfo * pItems );

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static WgItemsUnselectMsg_p	cast( const WgObject_p& pObject );
	};

	//____ WgPopupClosedMsg ______________________________________________________

	class WgPopupClosedMsg : public WgMsg
	{
		friend class WgPopupLayer;
	public:
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static WgPopupClosedMsg_p	cast( const WgObject_p& pObject );

	protected:
		WgPopupClosedMsg( WgWidget * pPopup, const WgWidget_wp& pCaller );

	};

	//____ WgWidgetModalLayer messages _________________________________________________

	class WgModalMoveOutsideMsg : public WgMsg
	{
		friend class WgModalLayer;
	public:
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static WgModalMoveOutsideMsg_p	cast( const WgObject_p& pObject );

	protected:
		WgModalMoveOutsideMsg( WgWidget * pWidget );
	};

	class WgModalBlockedPressMsg : public WgMouseButtonMsg
	{
		friend class WgModalLayer;
	public:
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static WgModalBlockedPressMsg_p	cast( const WgObject_p& pObject );

	protected:
		WgModalBlockedPressMsg( WgMouseButton button, WgWidget * pModalWidget );
	};

	class WgModalBlockedReleaseMsg : public WgMouseButtonMsg
	{
		friend class WgModalLayer;
	public:
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static WgModalBlockedReleaseMsg_p	cast( const WgObject_p& pObject );

	protected:
		WgModalBlockedReleaseMsg( WgMouseButton button, WgWidget * pModalWidget );
	};

	//____ Link messages _________________________________________________________

	class WgLinkMsg : public WgMsg
	{
	public:
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static WgLinkMsg_p	cast( const WgObject_p& pObject );
	};

	class WgLinkSelectMsg : public WgLinkMsg
	{
	public:
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static WgLinkSelectMsg_p	cast( const WgObject_p& pObject );

		WgLinkSelectMsg( const WgTextLink_p& pLink );
	};


	class WgLinkMouseEnterMsg : public WgLinkMsg
	{
	public:
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static WgLinkMouseEnterMsg_p	cast( const WgObject_p& pObject );

		WgLinkMouseEnterMsg( const WgTextLink_p& pLink );
	};

	class WgLinkMouseLeaveMsg : public WgLinkMsg
	{
	public:
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static WgLinkMouseLeaveMsg_p	cast( const WgObject_p& pObject );

		WgLinkMouseLeaveMsg(  const WgTextLink_p& pLink );
	};

	class WgLinkMouseButtonMsg : public WgLinkMsg
	{
	public:
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static WgLinkMouseButtonMsg_p	cast( const WgObject_p& pObject );

		WgMouseButton		button() const;
	protected:
		WgMouseButton		m_button;
	};

	class WgLinkMousePressMsg : public WgLinkMouseButtonMsg
	{
	public:
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static WgLinkMousePressMsg_p	cast( const WgObject_p& pObject );

		WgLinkMousePressMsg(  const WgTextLink_p& pLink, WgMouseButton button );
	};

	class WgLinkMouseRepeatMsg : public WgLinkMouseButtonMsg
	{
	public:
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static WgLinkMouseRepeatMsg_p	cast( const WgObject_p& pObject );

		WgLinkMouseRepeatMsg( const WgTextLink_p& pLink, WgMouseButton button );
	};

	class WgLinkMouseReleaseMsg : public WgLinkMouseButtonMsg
	{
	public:
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static WgLinkMouseReleaseMsg_p	cast( const WgObject_p& pObject );

		WgLinkMouseReleaseMsg( const WgTextLink_p& pLink, WgMouseButton button );
	};

	class WgLinkMouseClickMsg : public WgLinkMouseButtonMsg
	{
	public:
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static WgLinkMouseClickMsg_p	cast( const WgObject_p& pObject );

		WgLinkMouseClickMsg(  const WgTextLink_p& pLink, WgMouseButton button );
	};

	class WgLinkMouseDoubleClickMsg : public WgLinkMouseButtonMsg
	{
	public:
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static WgLinkMouseDoubleClickMsg_p	cast( const WgObject_p& pObject );

		WgLinkMouseDoubleClickMsg(  const WgTextLink_p& pLink, WgMouseButton button );
	};

	//____ Internally posted messages ____________________________________________

	class WgMousePositionMsg : public WgMsg
	{
		friend class WgMsgRouter;
	public:
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static WgMousePositionMsg_p	cast( const WgObject_p& pObject );

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
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static WgMouseDragMsg_p	cast( const WgObject_p& pObject );

		WgCoord			draggedTotal() const;
		WgCoord			draggedNow() const;
		WgCoord			startPos() const;
		WgCoord			prevPos() const;
		WgCoord			currPos() const;
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
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static WgMouseRepeatMsg_p	cast( const WgObject_p& pObject );
	};

	class WgMouseClickMsg : public WgMouseButtonMsg
	{
		friend class WgInputHandler;
		friend class WgMsgRouter;
	protected:
		WgMouseClickMsg( WgMouseButton button );
		WgMouseClickMsg( WgMouseButton button, WgWidget * pWidget );
	public:
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static WgMouseClickMsg_p	cast( const WgObject_p& pObject );
	};

	class WgMouseDoubleClickMsg : public WgMouseButtonMsg
	{
		friend class WgInputHandler;
		friend class WgMsgRouter;
	public:
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static WgMouseDoubleClickMsg_p	cast( const WgObject_p& pObject );

	protected:
		WgMouseDoubleClickMsg( WgMouseButton button );
		WgMouseDoubleClickMsg( WgMouseButton button, WgWidget * pWidget );
	};

	class WgKeyRepeatMsg : public WgKeyMsg
	{
		friend class WgInputHandler;
		friend class WgMsgRouter;
	public:
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static WgKeyRepeatMsg_p	cast( const WgObject_p& pObject );

	protected:
		WgKeyRepeatMsg( int native_keycode );
		WgKeyRepeatMsg( int native_keycode, WgWidget * pWidget );
	};

} // namespace wg
#endif //WG_MSG_DOT_H
