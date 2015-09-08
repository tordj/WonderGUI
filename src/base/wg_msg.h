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

	class MsgRouter;
	class Widget;
	class Button;
	class CheckBox;
	class RadioButton;
	class AnimPlayer;
	class Tablist;
	class ValueDisplay;
	class ValueEditor;
	class Scrollbar;
	class Menu;
	class PopupLayer;
	class ModalLayer;


	class LegacyText;
	class LegacyModText;
	class EditText;

	typedef	WgIStrongPtr<LegacyText,Interface_p>	LegacyText_p;
	typedef	WgIWeakPtr<LegacyText,Interface_wp> LegacyText_wp;

	typedef	WgIStrongPtr<LegacyModText,LegacyText_p>	LegacyModText_p;
	typedef	WgIWeakPtr<LegacyModText,LegacyText_wp>	LegacyModText_wp;

	typedef	WgIStrongPtr<EditText,LegacyModText_p>	EditText_p;
	typedef	WgIWeakPtr<EditText,LegacyModText_wp>	EditText_wp;


	typedef WgWeakPtr<Widget,Receiver_wp> Widget_wp;

	class Msg;
	typedef	WgStrongPtr<Msg,Object_p>		Msg_p;
	typedef	WgWeakPtr<Msg,Object_wp>	Msg_wp;

	class FocusGainedMsg;
	typedef	WgStrongPtr<FocusGainedMsg,Msg_p>		FocusGainedMsg_p;
	typedef	WgWeakPtr<FocusGainedMsg,Msg_wp>	FocusGainedMsg_wp;

	class FocusLostMsg;
	typedef	WgStrongPtr<FocusLostMsg,Msg_p>		FocusLostMsg_p;
	typedef	WgWeakPtr<FocusLostMsg,Msg_wp>	FocusLostMsg_wp;

	class MouseEnterMsg;
	typedef	WgStrongPtr<MouseEnterMsg,Msg_p>		MouseEnterMsg_p;
	typedef	WgWeakPtr<MouseEnterMsg,Msg_wp>	MouseEnterMsg_wp;

	class MouseLeaveMsg;
	typedef	WgStrongPtr<MouseLeaveMsg,Msg_p>		MouseLeaveMsg_p;
	typedef	WgWeakPtr<MouseLeaveMsg,Msg_wp>	MouseLeaveMsg_wp;

	class MouseMoveMsg;
	typedef	WgStrongPtr<MouseMoveMsg,Msg_p>		MouseMoveMsg_p;
	typedef	WgWeakPtr<MouseMoveMsg,Msg_wp>	MouseMoveMsg_wp;

	class MousePositionMsg;
	typedef	WgStrongPtr<MousePositionMsg,Msg_p>		MousePositionMsg_p;
	typedef	WgWeakPtr<MousePositionMsg,Msg_wp>	MousePositionMsg_wp;

	class MouseButtonMsg;
	typedef	WgStrongPtr<MouseButtonMsg,Msg_p>		MouseButtonMsg_p;
	typedef	WgWeakPtr<MouseButtonMsg,Msg_wp>	MouseButtonMsg_wp;

	class MousePressMsg;
	typedef	WgStrongPtr<MousePressMsg,MouseButtonMsg_p>		MousePressMsg_p;
	typedef	WgWeakPtr<MousePressMsg,MouseButtonMsg_wp>	MousePressMsg_wp;

	class MouseReleaseMsg;
	typedef	WgStrongPtr<MouseReleaseMsg,MouseButtonMsg_p>		MouseReleaseMsg_p;
	typedef	WgWeakPtr<MouseReleaseMsg,MouseButtonMsg_wp>	MouseReleaseMsg_wp;

	class MouseRepeatMsg;
	typedef	WgStrongPtr<MouseRepeatMsg,MouseButtonMsg_p>		MouseRepeatMsg_p;
	typedef	WgWeakPtr<MouseRepeatMsg,MouseButtonMsg_wp>	MouseRepeatMsg_wp;

	class MouseClickMsg;
	typedef	WgStrongPtr<MouseClickMsg,MouseButtonMsg_p>		MouseClickMsg_p;
	typedef	WgWeakPtr<MouseClickMsg,MouseButtonMsg_wp>	MouseClickMsg_wp;

	class MouseDoubleClickMsg;
	typedef	WgStrongPtr<MouseDoubleClickMsg,MouseButtonMsg_p>		MouseDoubleClickMsg_p;
	typedef	WgWeakPtr<MouseDoubleClickMsg,MouseButtonMsg_wp>	MouseDoubleClickMsg_wp;

	class MouseDragMsg;
	typedef	WgStrongPtr<MouseDragMsg,MouseButtonMsg_p>		MouseDragMsg_p;
	typedef	WgWeakPtr<MouseDragMsg,MouseButtonMsg_wp>	MouseDragMsg_wp;

	class KeyMsg;
	typedef	WgStrongPtr<KeyMsg,Msg_p>		KeyMsg_p;
	typedef	WgWeakPtr<KeyMsg,Msg_wp>	KeyMsg_wp;

	class KeyPressMsg;
	typedef	WgStrongPtr<KeyPressMsg,KeyMsg_p>		KeyPressMsg_p;
	typedef	WgWeakPtr<KeyPressMsg,KeyMsg_wp>	KeyPressMsg_wp;

	class KeyReleaseMsg;
	typedef	WgStrongPtr<KeyReleaseMsg,KeyMsg_p>		KeyReleaseMsg_p;
	typedef	WgWeakPtr<KeyReleaseMsg,KeyMsg_wp>	KeyReleaseMsg_wp;

	class KeyRepeatMsg;
	typedef	WgStrongPtr<KeyRepeatMsg,KeyMsg_p>		KeyRepeatMsg_p;
	typedef	WgWeakPtr<KeyRepeatMsg,KeyMsg_wp>	KeyRepeatMsg_wp;

	class CharacterMsg;
	typedef	WgStrongPtr<CharacterMsg,Msg_p>		CharacterMsg_p;
	typedef	WgWeakPtr<CharacterMsg,Msg_wp>	CharacterMsg_wp;

	class WheelRollMsg;
	typedef	WgStrongPtr<WheelRollMsg,Msg_p>		WheelRollMsg_p;
	typedef	WgWeakPtr<WheelRollMsg,Msg_wp>	WheelRollMsg_wp;

	class TickMsg;
	typedef	WgStrongPtr<TickMsg,Msg_p>		TickMsg_p;
	typedef	WgWeakPtr<TickMsg,Msg_wp>	TickMsg_wp;

	class PointerChangeMsg;
	typedef	WgStrongPtr<PointerChangeMsg,Msg_p>		PointerChangeMsg_p;
	typedef	WgWeakPtr<PointerChangeMsg,Msg_wp>	PointerChangeMsg_wp;

	class SelectMsg;
	typedef	WgStrongPtr<SelectMsg,Msg_p>		SelectMsg_p;
	typedef	WgWeakPtr<SelectMsg,Msg_wp>	SelectMsg_wp;

	class ToggleMsg;
	typedef	WgStrongPtr<ToggleMsg,Msg_p>		ToggleMsg_p;
	typedef	WgWeakPtr<ToggleMsg,Msg_wp>	ToggleMsg_wp;

	class ValueUpdateMsg;
	typedef	WgStrongPtr<ValueUpdateMsg,Msg_p>		ValueUpdateMsg_p;
	typedef	WgWeakPtr<ValueUpdateMsg,Msg_wp>	ValueUpdateMsg_wp;

	class RangeUpdateMsg;
	typedef	WgStrongPtr<RangeUpdateMsg,Msg_p>		RangeUpdateMsg_p;
	typedef	WgWeakPtr<RangeUpdateMsg,Msg_wp>	RangeUpdateMsg_wp;

	class TextEditMsg;
	typedef	WgStrongPtr<TextEditMsg,Msg_p>		TextEditMsg_p;
	typedef	WgWeakPtr<TextEditMsg,Msg_wp>	TextEditMsg_wp;

	class ItemMsg;
	typedef	WgStrongPtr<ItemMsg,Msg_p>		ItemMsg_p;
	typedef	WgWeakPtr<ItemMsg,Msg_wp>	ItemMsg_wp;

	class ItemListMsg;
	typedef	WgStrongPtr<ItemListMsg,Msg_p>		ItemListMsg_p;
	typedef	WgWeakPtr<ItemListMsg,Msg_wp>	ItemListMsg_wp;

	class ItemsSelectMsg;
	typedef	WgStrongPtr<ItemsSelectMsg,ItemListMsg_p>		ItemsSelectMsg_p;
	typedef	WgWeakPtr<ItemsSelectMsg,ItemListMsg_wp>	ItemsSelectMsg_wp;

	class ItemsUnselectMsg;
	typedef	WgStrongPtr<ItemsUnselectMsg,ItemListMsg_p>		ItemsUnselectMsg_p;
	typedef	WgWeakPtr<ItemsUnselectMsg,ItemListMsg_wp>	ItemsUnselectMsg_wp;

	class ItemToggleMsg;
	typedef	WgStrongPtr<ItemToggleMsg,ItemMsg_p>		ItemToggleMsg_p;
	typedef	WgWeakPtr<ItemToggleMsg,ItemMsg_wp>	ItemToggleMsg_wp;

	class ItemMousePressMsg;
	typedef	WgStrongPtr<ItemMousePressMsg,ItemMsg_p>		ItemMousePressMsg_p;
	typedef	WgWeakPtr<ItemMousePressMsg,ItemMsg_wp>	ItemMousePressMsg_wp;

	class PopupClosedMsg;
	typedef	WgStrongPtr<PopupClosedMsg,Msg_p>		PopupClosedMsg_p;
	typedef	WgWeakPtr<PopupClosedMsg,Msg_wp>	PopupClosedMsg_wp;

	class ModalMoveOutsideMsg;
	typedef	WgStrongPtr<ModalMoveOutsideMsg,Msg_p>		ModalMoveOutsideMsg_p;
	typedef	WgWeakPtr<ModalMoveOutsideMsg,Msg_wp>	ModalMoveOutsideMsg_wp;

	class ModalBlockedPressMsg;
	typedef	WgStrongPtr<ModalBlockedPressMsg,Msg_p>		ModalBlockedPressMsg_p;
	typedef	WgWeakPtr<ModalBlockedPressMsg,Msg_wp>	ModalBlockedPressMsg_wp;

	class ModalBlockedReleaseMsg;
	typedef	WgStrongPtr<ModalBlockedReleaseMsg,Msg_p>		ModalBlockedReleaseMsg_p;
	typedef	WgWeakPtr<ModalBlockedReleaseMsg,Msg_wp>	ModalBlockedReleaseMsg_wp;

	//

	class LinkMsg;
	typedef	WgStrongPtr<LinkMsg,Msg_p>		LinkMsg_p;
	typedef	WgWeakPtr<LinkMsg,Msg_wp>	LinkMsg_wp;

	class LinkSelectMsg;
	typedef	WgStrongPtr<LinkSelectMsg,LinkMsg_p>		LinkSelectMsg_p;
	typedef	WgWeakPtr<LinkSelectMsg,LinkMsg_wp>		LinkSelectMsg_wp;

	class LinkMouseEnterMsg;
	typedef	WgStrongPtr<LinkMouseEnterMsg,LinkMsg_p>		LinkMouseEnterMsg_p;
	typedef	WgWeakPtr<LinkMouseEnterMsg,LinkMsg_wp>		LinkMouseEnterMsg_wp;

	class LinkMouseLeaveMsg;
	typedef	WgStrongPtr<LinkMouseLeaveMsg,LinkMsg_p>		LinkMouseLeaveMsg_p;
	typedef	WgWeakPtr<LinkMouseLeaveMsg,LinkMsg_wp>		LinkMouseLeaveMsg_wp;

	class LinkMouseButtonMsg;
	typedef	WgStrongPtr<LinkMouseButtonMsg,LinkMsg_p>		LinkMouseButtonMsg_p;
	typedef	WgWeakPtr<LinkMouseButtonMsg,LinkMsg_wp>		LinkMouseButtonMsg_wp;

	class LinkMousePressMsg;
	typedef	WgStrongPtr<LinkMousePressMsg,LinkMouseButtonMsg_p>		LinkMousePressMsg_p;
	typedef	WgWeakPtr<LinkMousePressMsg,LinkMouseButtonMsg_wp>		LinkMousePressMsg_wp;

	class LinkMouseRepeatMsg;
	typedef	WgStrongPtr<LinkMouseRepeatMsg,LinkMouseButtonMsg_p>		LinkMouseRepeatMsg_p;
	typedef	WgWeakPtr<LinkMouseRepeatMsg,LinkMouseButtonMsg_wp>		LinkMouseRepeatMsg_wp;

	class LinkMouseReleaseMsg;
	typedef	WgStrongPtr<LinkMouseReleaseMsg,LinkMouseButtonMsg_p>		LinkMouseReleaseMsg_p;
	typedef	WgWeakPtr<LinkMouseReleaseMsg,LinkMouseButtonMsg_wp>		LinkMouseReleaseMsg_wp;

	class LinkMouseClickMsg;
	typedef	WgStrongPtr<LinkMouseClickMsg,LinkMouseButtonMsg_p>		LinkMouseClickMsg_p;
	typedef	WgWeakPtr<LinkMouseClickMsg,LinkMouseButtonMsg_wp>		LinkMouseClickMsg_wp;

	class LinkMouseDoubleClickMsg;
	typedef	WgStrongPtr<LinkMouseDoubleClickMsg,LinkMouseButtonMsg_p>		LinkMouseDoubleClickMsg_p;
	typedef	WgWeakPtr<LinkMouseDoubleClickMsg,LinkMouseButtonMsg_wp>		LinkMouseDoubleClickMsg_wp;

	//____ Msg _________________________________________________________________
	/**
	 * @brief Base class for all messages.
	 *
	 * Base class for all messages.
	 */


	class Msg : public Object
	{
		friend class MsgRouter;
		friend class Widget;

		public:

			bool				isInstanceOf( const char * pClassName ) const;
			const char *		className( void ) const;
			static const char	CLASSNAME[];
			static Msg_p	cast( const Object_p& pObject );

			MsgType			type() const { return m_type; }
			int64_t				timestamp() const { return m_timestamp; }
			bool				hasSource() const { return m_pSource; } 
			Object *			sourceRawPtr() const { return m_pSource.rawPtr(); }
			Object_wp		source() const { return m_pSource; } 
			WgModifierKeys		modKeys() const { return m_modKeys; }
			Coord				pointerPos() const { return m_pointerPos; }
				
			bool				isMouseMsg() const;
			bool				isMouseButtonMsg() const;
			bool				isKeyMsg() const;

			bool				hasCopyTo() { return m_pCopyTo; }
			Receiver_p		getCopyTo() { return m_pCopyTo; }
			bool				hasRepost() { return m_pRepostSource; }

			void				setCopyTo( const Receiver_p& pReceiver );
			void				setRepost( const Object_p& pSource, const Receiver_p& pCopyTo );
			void				swallow();
			bool				doRepost();

		protected:
			Msg() : m_type(WG_MSG_DUMMY), m_modKeys(WG_MODKEY_NONE), m_timestamp(0) {}
			virtual ~Msg() {}
			
			MsgType			m_type;				// Type of message
			int64_t				m_timestamp;		// Timestamp of posting this message.
			Object_p			m_pSource;			// The source of this message, if any.
			Receiver_p		m_pCopyTo;			// Receiver to receive a copy of this message, if any.
			Object_p			m_pRepostSource;	// Object to repost this message from, if any.
			Receiver_p		m_pRepostCopyTo;	// Receiver to copy this message to when reposting, if any.

			WgModifierKeys		m_modKeys;			// Modifier keys pressed when message posted.
			Coord				m_pointerPos;		// Screen position of pointer.
	};

	//____ MouseButtonMsg ______________________________________________________
	/**
	 * @brief Base class for all mouse button messages.
	 * 
	 */

	class MouseButtonMsg : public Msg
	{
		friend class InputHandler;
		friend class MsgRouter;
	public:
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static MouseButtonMsg_p	cast( const Object_p& pObject );

		WgMouseButton		button() const { return m_button; }
	protected:
		MouseButtonMsg(WgMouseButton button) : m_button(button) {}

		WgMouseButton		m_button;
	};

	//____ KeyMsg ______________________________________________________________
	/**
	 * @brief Base class for all keyboard messages.
	 *
	 * Base class for all keyboard messages.
	 */

	class KeyMsg : public Msg
	{
		friend class InputHandler;
		friend class MsgRouter;
	public:
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static KeyMsg_p	cast( const Object_p& pObject );

		int		nativeKeyCode() const { return m_nativeKeyCode; }
		int		translatedKeyCode() const { return m_translatedKeyCode; }
		bool	isCursorKey() const;
		bool	isMovementKey() const;
	protected:
		KeyMsg( int nativeKeyCode, int translatedKeyCode ) : m_nativeKeyCode(nativeKeyCode), m_translatedKeyCode(translatedKeyCode) {}

		int		m_nativeKeyCode;
		int		m_translatedKeyCode;
	};

	//____ FocusGainedMsg ______________________________________________________

	class FocusGainedMsg : public Msg
	{
		friend class InputHandler;
		friend class MsgRouter;
	public:
		static FocusGainedMsg_p	create( Widget * pWidget ) { return new FocusGainedMsg( pWidget ); }

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static FocusGainedMsg_p	cast( const Object_p& pObject );
	protected:
		FocusGainedMsg( Widget * pWidget );
	};

	//____ FocusLostMsg ________________________________________________________

	class FocusLostMsg : public Msg
	{
		friend class InputHandler;
		friend class MsgRouter;
	public:
		static FocusLostMsg_p	create( Widget * pWidget ) { return new FocusLostMsg( pWidget ); }

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static FocusLostMsg_p	cast( const Object_p& pObject );
	protected:
		FocusLostMsg( Widget * pWidget );
	};

	//____ MouseEnterMsg _______________________________________________________

	class MouseEnterMsg : public Msg
	{
		friend class InputHandler;
		friend class MsgRouter;
	public:
		static MouseEnterMsg_p			create( const Coord& pos ) { return new MouseEnterMsg(pos); }

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static MouseEnterMsg_p	cast( const Object_p& pObject );
	protected:
		MouseEnterMsg( const Coord& pos );
		MouseEnterMsg( Widget * pWidget );
	};

	//____ MouseLeaveMsg _______________________________________________________

	class MouseLeaveMsg : public Msg
	{
		friend class InputHandler;
		friend class MsgRouter;
	public:
		static MouseLeaveMsg_p			create() { return new MouseLeaveMsg(); }

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static MouseLeaveMsg_p	cast( const Object_p& pObject );
	protected:
		MouseLeaveMsg();
		MouseLeaveMsg( Widget * pWidget );
	};

	//____ MouseMoveMsg ________________________________________________________

	class MouseMoveMsg : public Msg
	{
		friend class InputHandler;
		friend class MsgRouter;
	public:
		static MouseMoveMsg_p			create( const Coord& pos ) { return new MouseMoveMsg(pos); }

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static MouseMoveMsg_p	cast( const Object_p& pObject );
	protected:
		MouseMoveMsg( Widget * pWidget );
		MouseMoveMsg( const Coord& pos );
	};

	//____ MousePressMsg _______________________________________________________

	class MousePressMsg : public MouseButtonMsg
	{
		friend class InputHandler;
		friend class MsgRouter;
	public:
		static MousePressMsg_p			create( WgMouseButton button ) { return new MousePressMsg(button); }

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static MousePressMsg_p	cast( const Object_p& pObject );
	protected:
		MousePressMsg( WgMouseButton button, Widget * pWidget );
		MousePressMsg( WgMouseButton button );
	};

	//____ MouseReleaseMsg _____________________________________________________

	class MouseReleaseMsg : public MouseButtonMsg
	{
		friend class InputHandler;
		friend class MsgRouter;
	public:
		static MouseReleaseMsg_p		create( WgMouseButton button ) { return new MouseReleaseMsg(button); }

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static MouseReleaseMsg_p	cast( const Object_p& pObject );

		bool			pressInside() const;
		bool			releaseInside() const;

	protected:
		MouseReleaseMsg( WgMouseButton button );
		MouseReleaseMsg( WgMouseButton button, Widget * pWidget, bool bPressInside, bool bReleaseInside );

		bool			m_bPressInside;
		bool			m_bReleaseInside;
	};

	//____ KeyPressMsg _________________________________________________________

	class KeyPressMsg : public KeyMsg
	{
		friend class InputHandler;
		friend class MsgRouter;
	public:
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static KeyPressMsg_p	cast( const Object_p& pObject );
	protected:
		KeyPressMsg( int native_keycode, int translated_keycode, Widget * pWidget );
	};

	//____ KeyReleaseMsg _______________________________________________________

	class KeyReleaseMsg : public KeyMsg
	{
		friend class InputHandler;
		friend class MsgRouter;
	public:
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static KeyReleaseMsg_p	cast( const Object_p& pObject );
	protected:
		KeyReleaseMsg( int nativeKeyCode, int translatedKeyCode, Widget * pWidget );
	};

	//____ CharacterMsg ________________________________________________________

	class CharacterMsg : public Msg
	{
		friend class InputHandler;
		friend class MsgRouter;
	public:
		static CharacterMsg_p			create( unsigned short character ) { return new CharacterMsg(character); }

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static CharacterMsg_p	cast( const Object_p& pObject );

		unsigned short	character() const;
	protected:
		CharacterMsg( unsigned short character );
		CharacterMsg( unsigned short character, Widget * pWidget );
	protected:
		unsigned short	m_char;
	};

	//____ WheelRollMsg ________________________________________________________

	class WheelRollMsg : public Msg
	{
		friend class InputHandler;
		friend class MsgRouter;
	public:
		static WheelRollMsg_p			create( int wheel, Coord distance ) { return new WheelRollMsg(wheel,distance); }

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static WheelRollMsg_p	cast( const Object_p& pObject );

		int			wheel() const;
		Coord		distance() const;
	protected:
		WheelRollMsg( int wheel, Coord distance );
		WheelRollMsg( int wheel, Coord distance, Widget * pWidget );

		int			m_wheel;
		Coord		m_distance;
	};

	//____ TickMsg _____________________________________________________________

	class TickMsg : public Msg
	{
		friend class InputHandler;
		friend class MsgRouter;
	public:
		static TickMsg_p				create( int ms ) { return new TickMsg(ms); }

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static TickMsg_p	cast( const Object_p& pObject );

		int				millisec() const;
	protected:
		TickMsg( int ms );
		TickMsg( int ms, Receiver * pReceiver );

		int			m_millisec;
	};

	//____ PointerChangeMsg ____________________________________________________

	class PointerChangeMsg : public Msg
	{
		friend class InputHandler;
		friend class MsgRouter;
	public:
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static PointerChangeMsg_p	cast( const Object_p& pObject );

		WgPointerStyle	style() const;
			
	protected:
		PointerChangeMsg( WgPointerStyle style );
			
		WgPointerStyle	m_style;
	};


	//____ SelectMsg ___________________________________________________________

	class SelectMsg : public Msg
	{
	public:
		SelectMsg( Object * pSource );

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static SelectMsg_p	cast( const Object_p& pObject );
	};

	//____ ToggleMsg ___________________________________________________________

	class ToggleMsg : public Msg
	{
	public:
		ToggleMsg( Object * pSource, bool bSet );

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static ToggleMsg_p	cast( const Object_p& pObject );

		bool		isSet() const;

	private:
		bool	m_bSet;
	};

	//____ ValueUpdateMsg ______________________________________________________

	class ValueUpdateMsg : public Msg
	{
	public:
		ValueUpdateMsg( Object * pSource, int64_t value, double fraction, bool bFinal );

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static ValueUpdateMsg_p	cast( const Object_p& pObject );

		int64_t		value() const;
		double		fraction() const;
		bool		isFinal() const;

	protected:
		int64_t		m_value;
		double		m_fraction;
		bool		m_bFinal;
	};

	//____ RangeUpdateMsg ______________________________________________________

	class RangeUpdateMsg : public Msg
	{
	public:
		RangeUpdateMsg( Object * pSource, int intOfs, int intLength, double fracOfs, double fracLength, bool bFinal ); 

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static RangeUpdateMsg_p	cast( const Object_p& pObject );

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

	//____ TextEditMsg _________________________________________________________

	class TextEditMsg : public Msg
	{
	public:
		TextEditMsg( const EditText_p& pText, bool bFinal );

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static TextEditMsg_p	cast( const Object_p& pObject );

		EditText_p		text() const;
		bool				isFinal() const;

	protected:
		EditText_p		m_pText;
		bool				m_bFinal;
	};


	//____

	class ItemInfo
	{
	public:
		ItemInfo(): id(-1) {}
		ItemInfo( int _id, const Object_p& _pObject ) : id(_id), pObject(_pObject) {}

		int				id;
		Object_p		pObject;
	};

	//____ ItemMsg ____________________________________________________________

	class ItemMsg : public Msg
	{
	public:
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static ItemMsg_p	cast( const Object_p& pObject );

		int				itemId() const;
		Object_p		itemObject() const;

	protected:
		ItemMsg( Object * pSource, int itemId, const Object_p& pItemObject );

		ItemInfo		m_item;
	};

	//____ ItemToggleMsg ______________________________________________________

	class ItemToggleMsg : public ItemMsg
	{
	public:
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static ItemToggleMsg_p	cast( const Object_p& pObject );

		bool		isSet() const;

	//protected:
		ItemToggleMsg( Object * pSource, int itemId, const Object_p& pItemObject, bool bSet );

		bool		m_bSet;
	};

	//____ ItemMousePressMsg ______________________________________________________

	class ItemMousePressMsg : public ItemMsg
	{
	public:
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static ItemMousePressMsg_p	cast( const Object_p& pObject );

		WgMouseButton		button() const;
	//protected:
		ItemMousePressMsg( Object * pSource, int itemId, const Object_p& pItemObject, WgMouseButton button );

		WgMouseButton		m_button;
	};


	//____ ItemListMsg ___________________________________________________________

	class ItemListMsg : public Msg
	{
	public:
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static ItemListMsg_p	cast( const Object_p& pObject );

		int					nbItems() const;
		const ItemInfo *	items() const;

	protected:
		ItemListMsg( Object * pSource, int nbItems, ItemInfo * pItems );
		virtual ~ItemListMsg();
		int				m_nbItems;
		ItemInfo *	m_pItems;
	};


	//____ ItemsSelectMsg _____________________________________________________

	class ItemsSelectMsg : public ItemListMsg
	{
	public:
		ItemsSelectMsg( Object * pSource, int nbItems, ItemInfo * pItems );

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static ItemsSelectMsg_p	cast( const Object_p& pObject );
	};

	//____ ItemsUnselectMsg _____________________________________________________

	class ItemsUnselectMsg : public ItemListMsg
	{
	public:
		ItemsUnselectMsg( Object * pSource, int nbItems, ItemInfo * pItems );

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static ItemsUnselectMsg_p	cast( const Object_p& pObject );
	};

	//____ PopupClosedMsg ______________________________________________________

	class PopupClosedMsg : public Msg
	{
		friend class PopupLayer;
	public:
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static PopupClosedMsg_p	cast( const Object_p& pObject );

	protected:
		PopupClosedMsg( Widget * pPopup, const Widget_wp& pCaller );

	};

	//____ WidgetModalLayer messages _________________________________________________

	class ModalMoveOutsideMsg : public Msg
	{
		friend class ModalLayer;
	public:
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static ModalMoveOutsideMsg_p	cast( const Object_p& pObject );

	protected:
		ModalMoveOutsideMsg( Widget * pWidget );
	};

	class ModalBlockedPressMsg : public MouseButtonMsg
	{
		friend class ModalLayer;
	public:
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static ModalBlockedPressMsg_p	cast( const Object_p& pObject );

	protected:
		ModalBlockedPressMsg( WgMouseButton button, Widget * pModalWidget );
	};

	class ModalBlockedReleaseMsg : public MouseButtonMsg
	{
		friend class ModalLayer;
	public:
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static ModalBlockedReleaseMsg_p	cast( const Object_p& pObject );

	protected:
		ModalBlockedReleaseMsg( WgMouseButton button, Widget * pModalWidget );
	};

	//____ Link messages _________________________________________________________

	class LinkMsg : public Msg
	{
	public:
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static LinkMsg_p	cast( const Object_p& pObject );
	};

	class LinkSelectMsg : public LinkMsg
	{
	public:
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static LinkSelectMsg_p	cast( const Object_p& pObject );

		LinkSelectMsg( const TextLink_p& pLink );
	};


	class LinkMouseEnterMsg : public LinkMsg
	{
	public:
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static LinkMouseEnterMsg_p	cast( const Object_p& pObject );

		LinkMouseEnterMsg( const TextLink_p& pLink );
	};

	class LinkMouseLeaveMsg : public LinkMsg
	{
	public:
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static LinkMouseLeaveMsg_p	cast( const Object_p& pObject );

		LinkMouseLeaveMsg(  const TextLink_p& pLink );
	};

	class LinkMouseButtonMsg : public LinkMsg
	{
	public:
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static LinkMouseButtonMsg_p	cast( const Object_p& pObject );

		WgMouseButton		button() const;
	protected:
		WgMouseButton		m_button;
	};

	class LinkMousePressMsg : public LinkMouseButtonMsg
	{
	public:
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static LinkMousePressMsg_p	cast( const Object_p& pObject );

		LinkMousePressMsg(  const TextLink_p& pLink, WgMouseButton button );
	};

	class LinkMouseRepeatMsg : public LinkMouseButtonMsg
	{
	public:
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static LinkMouseRepeatMsg_p	cast( const Object_p& pObject );

		LinkMouseRepeatMsg( const TextLink_p& pLink, WgMouseButton button );
	};

	class LinkMouseReleaseMsg : public LinkMouseButtonMsg
	{
	public:
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static LinkMouseReleaseMsg_p	cast( const Object_p& pObject );

		LinkMouseReleaseMsg( const TextLink_p& pLink, WgMouseButton button );
	};

	class LinkMouseClickMsg : public LinkMouseButtonMsg
	{
	public:
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static LinkMouseClickMsg_p	cast( const Object_p& pObject );

		LinkMouseClickMsg(  const TextLink_p& pLink, WgMouseButton button );
	};

	class LinkMouseDoubleClickMsg : public LinkMouseButtonMsg
	{
	public:
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static LinkMouseDoubleClickMsg_p	cast( const Object_p& pObject );

		LinkMouseDoubleClickMsg(  const TextLink_p& pLink, WgMouseButton button );
	};

	//____ Internally posted messages ____________________________________________

	class MousePositionMsg : public Msg
	{
		friend class MsgRouter;
	public:
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static MousePositionMsg_p	cast( const Object_p& pObject );

	protected:
		MousePositionMsg();
	};

	class MouseDragMsg : public MouseButtonMsg
	{
		friend class InputHandler;
		friend class MsgRouter;
	protected:
		MouseDragMsg( WgMouseButton button, const Coord& startPos, const Coord& prevPos, const Coord& currPos );
		MouseDragMsg( WgMouseButton button, Widget * pWidget, const Coord& orgPos, const Coord& prevPos, const Coord& currPos );
	public:
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static MouseDragMsg_p	cast( const Object_p& pObject );

		Coord			draggedTotal() const;
		Coord			draggedNow() const;
		Coord			startPos() const;
		Coord			prevPos() const;
		Coord			currPos() const;
	protected:
		Coord			m_startPos;
		Coord			m_prevPos;
		Coord			m_currPos;
	};

	class MouseRepeatMsg : public MouseButtonMsg
	{
		friend class InputHandler;
		friend class MsgRouter;
	protected:
		MouseRepeatMsg( WgMouseButton button, Widget * pWidget );
		MouseRepeatMsg( WgMouseButton button );
	public:
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static MouseRepeatMsg_p	cast( const Object_p& pObject );
	};

	class MouseClickMsg : public MouseButtonMsg
	{
		friend class InputHandler;
		friend class MsgRouter;
	protected:
		MouseClickMsg( WgMouseButton button );
		MouseClickMsg( WgMouseButton button, Widget * pWidget );
	public:
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static MouseClickMsg_p	cast( const Object_p& pObject );
	};

	class MouseDoubleClickMsg : public MouseButtonMsg
	{
		friend class InputHandler;
		friend class MsgRouter;
	public:
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static MouseDoubleClickMsg_p	cast( const Object_p& pObject );

	protected:
		MouseDoubleClickMsg( WgMouseButton button );
		MouseDoubleClickMsg( WgMouseButton button, Widget * pWidget );
	};

	class KeyRepeatMsg : public KeyMsg
	{
		friend class InputHandler;
		friend class MsgRouter;
	public:
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static KeyRepeatMsg_p	cast( const Object_p& pObject );

	protected:
		KeyRepeatMsg( int native_keycode, int translated_keycode, Widget * pWidget );
	};

} // namespace wg
#endif //WG_MSG_DOT_H
