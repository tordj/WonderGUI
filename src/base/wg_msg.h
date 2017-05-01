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
#pragma once

#include <string>

#include <wg_types.h>
#include <wg_userdefines.h>
#include <wg_geo.h>
#include <wg_pointers.h>
#include <wg_widget.h>
#include <wg_key.h>





namespace wg 
{

	class MsgRouter;
	class Widget;
	class PopupLayer;
	class ModalLayer;


	class Text;
	class ModText;
	class EditText;

	typedef	StrongInterfacePtr<Text,Interface_p>	Text_p;
	typedef	WeakInterfacePtr<Text,Interface_wp> 	Text_wp;

	typedef	StrongInterfacePtr<ModText,Text_p>		ModText_p;
	typedef	WeakInterfacePtr<ModText,Text_wp>		ModText_wp;

	typedef	StrongInterfacePtr<EditText,ModText_p>	EditText_p;
	typedef	WeakInterfacePtr<EditText,ModText_wp>	EditText_wp;


	typedef WeakPtr<Widget,Receiver_wp> Widget_wp;

	class Msg;
	typedef	StrongPtr<Msg,Object_p>		Msg_p;
	typedef	WeakPtr<Msg,Object_wp>	Msg_wp;

	class InputMsg;
	typedef	StrongPtr<InputMsg,Msg_p>		InputMsg_p;
	typedef	WeakPtr<InputMsg,Msg_wp>	InputMsg_wp;


	class FocusGainedMsg;
	typedef	StrongPtr<FocusGainedMsg,Msg_p>		FocusGainedMsg_p;
	typedef	WeakPtr<FocusGainedMsg,Msg_wp>	FocusGainedMsg_wp;

	class FocusLostMsg;
	typedef	StrongPtr<FocusLostMsg,Msg_p>		FocusLostMsg_p;
	typedef	WeakPtr<FocusLostMsg,Msg_wp>	FocusLostMsg_wp;

	class MouseEnterMsg;
	typedef	StrongPtr<MouseEnterMsg,InputMsg_p>		MouseEnterMsg_p;
	typedef	WeakPtr<MouseEnterMsg,InputMsg_wp>	MouseEnterMsg_wp;

	class MouseLeaveMsg;
	typedef	StrongPtr<MouseLeaveMsg,InputMsg_p>		MouseLeaveMsg_p;
	typedef	WeakPtr<MouseLeaveMsg,InputMsg_wp>	MouseLeaveMsg_wp;

	class MouseMoveMsg;
	typedef	StrongPtr<MouseMoveMsg,InputMsg_p>		MouseMoveMsg_p;
	typedef	WeakPtr<MouseMoveMsg,InputMsg_wp>		MouseMoveMsg_wp;

	class MouseButtonMsg;
	typedef	StrongPtr<MouseButtonMsg,InputMsg_p>		MouseButtonMsg_p;
	typedef	WeakPtr<MouseButtonMsg,InputMsg_wp>	MouseButtonMsg_wp;

	class MousePressMsg;
	typedef	StrongPtr<MousePressMsg,MouseButtonMsg_p>		MousePressMsg_p;
	typedef	WeakPtr<MousePressMsg,MouseButtonMsg_wp>	MousePressMsg_wp;

	class MouseReleaseMsg;
	typedef	StrongPtr<MouseReleaseMsg,MouseButtonMsg_p>		MouseReleaseMsg_p;
	typedef	WeakPtr<MouseReleaseMsg,MouseButtonMsg_wp>	MouseReleaseMsg_wp;

	class MouseRepeatMsg;
	typedef	StrongPtr<MouseRepeatMsg,MouseButtonMsg_p>		MouseRepeatMsg_p;
	typedef	WeakPtr<MouseRepeatMsg,MouseButtonMsg_wp>	MouseRepeatMsg_wp;

	class MouseClickMsg;
	typedef	StrongPtr<MouseClickMsg,MouseButtonMsg_p>		MouseClickMsg_p;
	typedef	WeakPtr<MouseClickMsg,MouseButtonMsg_wp>	MouseClickMsg_wp;

	class MouseDoubleClickMsg;
	typedef	StrongPtr<MouseDoubleClickMsg,MouseButtonMsg_p>		MouseDoubleClickMsg_p;
	typedef	WeakPtr<MouseDoubleClickMsg,MouseButtonMsg_wp>	MouseDoubleClickMsg_wp;

	class MouseDragMsg;
	typedef	StrongPtr<MouseDragMsg,MouseButtonMsg_p>		MouseDragMsg_p;
	typedef	WeakPtr<MouseDragMsg,MouseButtonMsg_wp>	MouseDragMsg_wp;

	class KeyMsg;
	typedef	StrongPtr<KeyMsg,InputMsg_p>		KeyMsg_p;
	typedef	WeakPtr<KeyMsg,InputMsg_wp>	KeyMsg_wp;

	class KeyPressMsg;
	typedef	StrongPtr<KeyPressMsg,KeyMsg_p>		KeyPressMsg_p;
	typedef	WeakPtr<KeyPressMsg,KeyMsg_wp>	KeyPressMsg_wp;

	class KeyReleaseMsg;
	typedef	StrongPtr<KeyReleaseMsg,KeyMsg_p>		KeyReleaseMsg_p;
	typedef	WeakPtr<KeyReleaseMsg,KeyMsg_wp>	KeyReleaseMsg_wp;

	class KeyRepeatMsg;
	typedef	StrongPtr<KeyRepeatMsg,KeyMsg_p>		KeyRepeatMsg_p;
	typedef	WeakPtr<KeyRepeatMsg,KeyMsg_wp>	KeyRepeatMsg_wp;

	class TextInputMsg;
	typedef	StrongPtr<TextInputMsg,Msg_p>		TextInputMsg_p;
	typedef	WeakPtr<TextInputMsg,Msg_wp>	TextInputMsg_wp;

	class EditCommandMsg;
	typedef	StrongPtr<EditCommandMsg,Msg_p>		EditCommandMsg_p;
	typedef	WeakPtr<EditCommandMsg,Msg_wp>	EditCommandMsg_wp;

	class WheelRollMsg;
	typedef	StrongPtr<WheelRollMsg,InputMsg_p>		WheelRollMsg_p;
	typedef	WeakPtr<WheelRollMsg,InputMsg_wp>	WheelRollMsg_wp;

	class TickMsg;
	typedef	StrongPtr<TickMsg,Msg_p>		TickMsg_p;
	typedef	WeakPtr<TickMsg,Msg_wp>	TickMsg_wp;

	class PointerChangeMsg;
	typedef	StrongPtr<PointerChangeMsg,Msg_p>		PointerChangeMsg_p;
	typedef	WeakPtr<PointerChangeMsg,Msg_wp>	PointerChangeMsg_wp;

	class SelectMsg;
	typedef	StrongPtr<SelectMsg,Msg_p>		SelectMsg_p;
	typedef	WeakPtr<SelectMsg,Msg_wp>	SelectMsg_wp;

	class ToggleMsg;
	typedef	StrongPtr<ToggleMsg,Msg_p>		ToggleMsg_p;
	typedef	WeakPtr<ToggleMsg,Msg_wp>	ToggleMsg_wp;

	class ValueUpdateMsg;
	typedef	StrongPtr<ValueUpdateMsg,Msg_p>		ValueUpdateMsg_p;
	typedef	WeakPtr<ValueUpdateMsg,Msg_wp>	ValueUpdateMsg_wp;

	class RangeUpdateMsg;
	typedef	StrongPtr<RangeUpdateMsg,Msg_p>		RangeUpdateMsg_p;
	typedef	WeakPtr<RangeUpdateMsg,Msg_wp>	RangeUpdateMsg_wp;

	class TextEditMsg;
	typedef	StrongPtr<TextEditMsg,Msg_p>		TextEditMsg_p;
	typedef	WeakPtr<TextEditMsg,Msg_wp>	TextEditMsg_wp;

	class ItemMsg;
	typedef	StrongPtr<ItemMsg,Msg_p>		ItemMsg_p;
	typedef	WeakPtr<ItemMsg,Msg_wp>	ItemMsg_wp;

	class ItemListMsg;
	typedef	StrongPtr<ItemListMsg,Msg_p>		ItemListMsg_p;
	typedef	WeakPtr<ItemListMsg,Msg_wp>	ItemListMsg_wp;

	class ItemsSelectMsg;
	typedef	StrongPtr<ItemsSelectMsg,ItemListMsg_p>		ItemsSelectMsg_p;
	typedef	WeakPtr<ItemsSelectMsg,ItemListMsg_wp>	ItemsSelectMsg_wp;

	class ItemsUnselectMsg;
	typedef	StrongPtr<ItemsUnselectMsg,ItemListMsg_p>		ItemsUnselectMsg_p;
	typedef	WeakPtr<ItemsUnselectMsg,ItemListMsg_wp>	ItemsUnselectMsg_wp;

	class ItemToggleMsg;
	typedef	StrongPtr<ItemToggleMsg,ItemMsg_p>		ItemToggleMsg_p;
	typedef	WeakPtr<ItemToggleMsg,ItemMsg_wp>	ItemToggleMsg_wp;

	class ItemMousePressMsg;
	typedef	StrongPtr<ItemMousePressMsg,ItemMsg_p>		ItemMousePressMsg_p;
	typedef	WeakPtr<ItemMousePressMsg,ItemMsg_wp>	ItemMousePressMsg_wp;

	class PopupClosedMsg;
	typedef	StrongPtr<PopupClosedMsg,Msg_p>		PopupClosedMsg_p;
	typedef	WeakPtr<PopupClosedMsg,Msg_wp>	PopupClosedMsg_wp;

	class ModalMoveOutsideMsg;
	typedef	StrongPtr<ModalMoveOutsideMsg,Msg_p>		ModalMoveOutsideMsg_p;
	typedef	WeakPtr<ModalMoveOutsideMsg,Msg_wp>	ModalMoveOutsideMsg_wp;

	class ModalBlockedPressMsg;
	typedef	StrongPtr<ModalBlockedPressMsg,Msg_p>		ModalBlockedPressMsg_p;
	typedef	WeakPtr<ModalBlockedPressMsg,Msg_wp>	ModalBlockedPressMsg_wp;

	class ModalBlockedReleaseMsg;
	typedef	StrongPtr<ModalBlockedReleaseMsg,Msg_p>		ModalBlockedReleaseMsg_p;
	typedef	WeakPtr<ModalBlockedReleaseMsg,Msg_wp>	ModalBlockedReleaseMsg_wp;


	//____ Msg _________________________________________________________________
	/**
	 * @brief Base class for all messages.
	 *
	 * Base class for all messages.
	 */


	class Msg : public Object
	{

		public:
		
			//.____ Identification __________________________________________

			bool				isInstanceOf( const char * pClassName ) const;
			const char *		className( void ) const;
			static const char	CLASSNAME[];
			static Msg_p		cast( const Object_p& pObject );
			MsgType				type() const { return m_type; }
			bool				isMouseMsg() const;
			bool				isMouseButtonMsg() const;
			bool				isKeyMsg() const;

			//.____ Content ____________________________________________________

			bool				hasSource() const { return m_pSource; } 
			Object *			sourceRawPtr() const { return m_pSource.rawPtr(); }
			Object_wp			source() const { return m_pSource; } 
				
			bool				hasCopyTo() { return m_pCopyTo; }
			Receiver_p			getCopyTo() { return m_pCopyTo; }
			bool				hasRepost() { return m_pRepostSource; }

			//.____ Control ____________________________________________________

			void				setCopyTo( const Receiver_p& pReceiver );
			void				setRepost( const Object_p& pFromSource, const Receiver_p& pCopyTo );
			void				swallow();
			bool				doRepost();

		protected:
			Msg() : m_type(MsgType::Dummy) {}
			virtual ~Msg() {}
			
			MsgType				m_type;				// Type of message
			Object_p			m_pSource;			// The source of this message, if any.
			Receiver_p			m_pCopyTo;			// Receiver to receive a copy of this message, if any.
			Object_p			m_pRepostSource;	// Object to repost this message from, if any.
			Receiver_p			m_pRepostCopyTo;	// Receiver to copy this message to when reposting, if any.

	};


	//____ InputMsg ______________________________________________________
	/**
	 * @brief Base class for all mouse and key messages.
	 * 
	 */

	class InputMsg : public Msg
	{
	public:

		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static InputMsg_p	cast( const Object_p& pObject );

		//.____ Content ____________________________________________________

		char				inputId() const { return m_inputId; }
		int64_t				timestamp() const { return m_timestamp; }
		ModifierKeys		modKeys() const { return m_modKeys; }
		Coord				pointerPos() const { return m_pointerPos; }

	protected:
		InputMsg(char inputId, ModifierKeys modKeys, Coord pointerPos, int64_t timestamp) : m_inputId(inputId), m_modKeys(modKeys), m_pointerPos(pointerPos), m_timestamp(timestamp) {}

		char				m_inputId;			// Id of InputHandler posting this message, so we can separate multiple input sources from each other.
		ModifierKeys		m_modKeys;			// Modifier keys pressed when message posted.
		Coord				m_pointerPos;		// Screen position of pointer.
		int64_t				m_timestamp;		// Timestamp of input event.
	};


	//____ MouseButtonMsg ______________________________________________________
	/**
	 * @brief Base class for all mouse button messages.
	 * 
	 */

	class MouseButtonMsg : public InputMsg
	{
	public:
		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static MouseButtonMsg_p	cast( const Object_p& pObject );

		//.____ Content ____________________________________________________

		MouseButton		button() const { return m_button; }

	protected:
		MouseButtonMsg(char inputId, MouseButton button, ModifierKeys modKeys, Coord pointerPos, int64_t timestamp) : InputMsg(inputId, modKeys, pointerPos, timestamp), m_button(button) {}

		MouseButton		m_button;
	};


	//____ KeyMsg ______________________________________________________________
	/**
	 * @brief Base class for all keyboard messages.
	 *
	 * Base class for all keyboard messages.
	 */

	class KeyMsg : public InputMsg
	{
	public:
		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static KeyMsg_p		cast( const Object_p& pObject );

		//.____ Content ____________________________________________________

		int		nativeKeyCode() const { return m_nativeKeyCode; }
		Key		translatedKeyCode() const { return m_translatedKeyCode; }
		bool	isCursorKey() const;
		bool	isMovementKey() const;

	protected:
		KeyMsg( char inputId, int nativeKeyCode, Key translatedKeyCode, ModifierKeys modKeys, Coord pointerPos, int64_t timestamp) : InputMsg(inputId, modKeys, pointerPos, timestamp), m_nativeKeyCode(nativeKeyCode), m_translatedKeyCode(translatedKeyCode) {}

		int		m_nativeKeyCode;
		Key		m_translatedKeyCode;
	};

	//____ FocusGainedMsg ______________________________________________________

	class FocusGainedMsg : public InputMsg
	{
		friend class InputHandler;
	public:
		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static FocusGainedMsg_p	cast( const Object_p& pObject );

	protected:
		FocusGainedMsg( char inputId, Widget * pWidget, ModifierKeys modKeys, Coord pointerPos, int64_t timestamp);
	};

	//____ FocusLostMsg ________________________________________________________

	class FocusLostMsg : public InputMsg
	{
		friend class InputHandler;
	public:
		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static FocusLostMsg_p	cast( const Object_p& pObject );

	protected:
		FocusLostMsg( char inputId, Widget * pWidget, ModifierKeys modKeys, Coord pointerPos, int64_t timestamp);
	};

	//____ MouseEnterMsg _______________________________________________________

	class MouseEnterMsg : public InputMsg
	{
	public:
		//.____ Creation __________________________________________

		inline static MouseEnterMsg_p create( char inputId, Object * pSource, ModifierKeys modKeys, Coord pointerPos, int64_t timestamp )
			{ return new MouseEnterMsg( inputId, pSource, modKeys, pointerPos, timestamp ); }

		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static MouseEnterMsg_p	cast( const Object_p& pObject );
	protected:
		MouseEnterMsg( char inputId, Object * pSource, ModifierKeys modKeys, Coord pointerPos, int64_t timestamp );
	};

	//____ MouseLeaveMsg _______________________________________________________

	class MouseLeaveMsg : public InputMsg
	{
	public:
		//.____ Creation __________________________________________

		inline static MouseLeaveMsg_p create( char inputId, Object * pSource, ModifierKeys modKeys, Coord pointerPos, int64_t timestamp )
			{ return new MouseLeaveMsg( inputId, pSource, modKeys, pointerPos, timestamp ); }

		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static MouseLeaveMsg_p	cast( const Object_p& pObject );
	protected:
		MouseLeaveMsg( char inputId, Object * pSource, ModifierKeys modKeys, Coord pointerPos, int64_t timestamp );
	};

	//____ MouseMoveMsg ________________________________________________________

	class MouseMoveMsg : public InputMsg
	{
	public:
		//.____ Creation __________________________________________

		inline static MouseMoveMsg_p create( char inputId, Object * pSource, ModifierKeys modKeys, Coord pointerPos, int64_t timestamp )
			{ return new MouseMoveMsg( inputId, pSource, modKeys, pointerPos, timestamp ); }

		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static MouseMoveMsg_p	cast( const Object_p& pObject );
	protected:
		MouseMoveMsg( char inputId, Object * pSource, ModifierKeys modKeys, Coord pointerPos, int64_t timestamp );
	};

	//____ MousePressMsg _______________________________________________________

	class MousePressMsg : public MouseButtonMsg
	{
	public:
		//.____ Creation __________________________________________

		inline static MousePressMsg_p create( char inputId, MouseButton button, Object * pSource, ModifierKeys modKeys, Coord pointerPos, int64_t timestamp )
			{ return new MousePressMsg( inputId, button, pSource, modKeys, pointerPos, timestamp ); }

		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static MousePressMsg_p	cast( const Object_p& pObject );
	protected:
		MousePressMsg( char inputId, MouseButton button, Object * pSource, ModifierKeys modKeys, Coord pointerPos, int64_t timestamp );
	};

	//____ MouseReleaseMsg _____________________________________________________

	class MouseReleaseMsg : public MouseButtonMsg
	{
	public:
		//.____ Creation __________________________________________

		inline static MouseReleaseMsg_p create( char inputId, MouseButton button, Object * pSource, bool bReleaseInside, ModifierKeys modKeys, Coord pointerPos, int64_t timestamp )
			{ return new MouseReleaseMsg( inputId, button, pSource, bReleaseInside, modKeys, pointerPos, timestamp ); }

		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static MouseReleaseMsg_p	cast( const Object_p& pObject );

		//.____ Content ________________________________________________________

		bool			releaseInside() const;

	protected:
		MouseReleaseMsg( char inputId, MouseButton button, Object * pSource, bool bReleaseInside, ModifierKeys modKeys, Coord pointerPos, int64_t timestamp );

		bool			m_bReleaseInside;
	};

	//____ KeyPressMsg _________________________________________________________

	class KeyPressMsg : public KeyMsg
	{
		friend class InputHandler;
	public:
		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static KeyPressMsg_p	cast( const Object_p& pObject );
	protected:
		KeyPressMsg( char inputId, int native_keycode, Key translated_keycode, Widget * pWidget, ModifierKeys modKeys, Coord pointerPos, int64_t timestamp );
	};

	//____ KeyReleaseMsg _______________________________________________________

	class KeyReleaseMsg : public KeyMsg
	{
		friend class InputHandler;
	public:
		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static KeyReleaseMsg_p	cast( const Object_p& pObject );
	protected:
		KeyReleaseMsg( char inputId, int nativeKeyCode, Key translatedKeyCode, Widget * pWidget, ModifierKeys modKeys, Coord pointerPos, int64_t timestamp );
	};

	//____ TextInputMsg ________________________________________________________

	class TextInputMsg : public Msg
	{
		friend class InputHandler;
	public:
		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static TextInputMsg_p	cast( const Object_p& pObject );

		//.____ Content ________________________________________________________

		String	text() const;
		char	inputId() const { return m_inputId; }

	protected:
		TextInputMsg( char inputId, String text, Widget * pWidget );
	protected:
		char	m_inputId;
		String	m_text;
	};

	//____ EditCommandMsg ________________________________________________________

	class EditCommandMsg : public Msg
	{
		friend class InputHandler;
	public:
		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static EditCommandMsg_p	cast( const Object_p& pObject );

		//.____ Content ________________________________________________________

		EditCmd	command() const { return m_command; }
		char	inputId() const { return m_inputId; }

	protected:
		EditCommandMsg( char inputId, EditCmd command, Widget * pWidget );
	protected:
		char	m_inputId;
		EditCmd	m_command;
	};


	//____ WheelRollMsg ________________________________________________________

	class WheelRollMsg : public InputMsg
	{
		friend class InputHandler;
	public:
		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static WheelRollMsg_p	cast( const Object_p& pObject );

		//.____ Content ________________________________________________________

		int			wheel() const;
		Coord		distance() const;
	protected:
		WheelRollMsg( char inputId, int wheel, Coord distance, Widget * pWidget, ModifierKeys modKeys, Coord pointerPos, int64_t timestamp );

		int			m_wheel;
		Coord		m_distance;
	};

	//____ TickMsg _____________________________________________________________

	class TickMsg : public Msg
	{
	public:
		//.____ Creation __________________________________________

		inline static TickMsg_p	create( int64_t timestamp, int timediff ) { return new TickMsg(timestamp, timediff); }

		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static TickMsg_p	cast( const Object_p& pObject );

		//.____ Content ________________________________________________________

		inline int			timediff() const { return m_timediff; }
		inline int64_t		timestamp() const { return m_timestamp; }
	protected:
		TickMsg( int64_t timestamp, int ms );

		int64_t		m_timestamp;
		int			m_timediff;
	};

	//____ PointerChangeMsg ____________________________________________________

	class PointerChangeMsg : public Msg
	{
		friend class InputHandler;
	public:
		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static PointerChangeMsg_p	cast( const Object_p& pObject );

		//.____ Content ________________________________________________________

		PointerStyle	style() const;
		inline char		inputId() const { return m_inputId; }
			
	protected:
		PointerChangeMsg( char inputId, PointerStyle style );
			
		char			m_inputId;	
		PointerStyle	m_style;
	};


	//____ SelectMsg ___________________________________________________________

	class SelectMsg : public Msg
	{
	public:
		//.____ Creation __________________________________________

		inline static SelectMsg_p	create( Object * pSource ) { return new SelectMsg(pSource); }

		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static SelectMsg_p	cast( const Object_p& pObject );
	
	protected:
		SelectMsg( Object * pSource );	
	};

	//____ ToggleMsg ___________________________________________________________

	class ToggleMsg : public Msg
	{
	public:
		//.____ Creation __________________________________________

		inline static ToggleMsg_p	create( Object * pSource, bool bSet ) { return new ToggleMsg(pSource,bSet); }

		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static ToggleMsg_p	cast( const Object_p& pObject );

		//.____ Content _________________________________________________

		bool		isSet() const;

	protected:
		ToggleMsg( Object * pSource, bool bSet );

		bool	m_bSet;
	};

	//____ ValueUpdateMsg ______________________________________________________

	class ValueUpdateMsg : public Msg
	{
	public:
		//.____ Creation __________________________________________

		inline static ValueUpdateMsg_p	create( Object * pSource, int64_t value, double fraction, bool bFinal ) { return new ValueUpdateMsg(pSource, value, fraction, bFinal ); }

		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static ValueUpdateMsg_p	cast( const Object_p& pObject );

		//.____ Content ________________________________________________________

		int64_t		value() const;
		double		fraction() const;
		bool		isFinal() const;

	protected:
		ValueUpdateMsg( Object * pSource, int64_t value, double fraction, bool bFinal );

		int64_t		m_value;
		double		m_fraction;
		bool		m_bFinal;
	};

	//____ RangeUpdateMsg ______________________________________________________

	class RangeUpdateMsg : public Msg
	{
	public:
		//.____ Creation __________________________________________

		inline static RangeUpdateMsg_p create( Object * pSource, int intOfs, int intLength, double fracOfs, double fracLength, bool bFinal ) 
										{ return new RangeUpdateMsg( pSource, intOfs, intLength, fracOfs, fracLength, bFinal ); } 

		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static RangeUpdateMsg_p	cast( const Object_p& pObject );

		//.____ Content ________________________________________________________

		int			offset() const;
		int			length() const;
		double		fracOffset() const;
		double		fracLength() const;
		bool		isFinal() const;

	protected:
		RangeUpdateMsg( Object * pSource, int intOfs, int intLength, double fracOfs, double fracLength, bool bFinal ); 

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
		//.____ Creation __________________________________________

		inline static TextEditMsg_p create( const EditText_p& pText, bool bFinal ) { return new TextEditMsg( pText, bFinal ); }

		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static TextEditMsg_p	cast( const Object_p& pObject );

		//.____ Content ________________________________________________________

		EditText_p			text() const;
		bool				isFinal() const;

	protected:
		TextEditMsg( const EditText_p& pText, bool bFinal );

		EditText_p			m_pText;
		bool				m_bFinal;
	};


	//____ ItemInfo _______________________________

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

		MouseButton		button() const;
	//protected:
		ItemMousePressMsg( Object * pSource, int itemId, const Object_p& pItemObject, MouseButton button );

		MouseButton		m_button;
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
		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static PopupClosedMsg_p	cast( const Object_p& pObject );

	protected:
		PopupClosedMsg( Widget * pPopup, const Widget_wp& pCaller );

	};

	//____ WidgetModalLayer messages _________________________________________________

	class ModalMoveOutsideMsg : public InputMsg
	{
		friend class ModalLayer;
	public:
		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static ModalMoveOutsideMsg_p	cast( const Object_p& pObject );

	protected:
		ModalMoveOutsideMsg( char inputId, Widget * pModalWidget, ModifierKeys modKeys, Coord pointerPos, int64_t timestamp );
	};

	class ModalBlockedPressMsg : public MouseButtonMsg
	{
		friend class ModalLayer;
	public:
		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static ModalBlockedPressMsg_p	cast( const Object_p& pObject );

	protected:
		ModalBlockedPressMsg( char inputId, MouseButton button, Widget * pModalWidget, ModifierKeys modKeys, Coord pointerPos, int64_t timestamp );
	};

	class ModalBlockedReleaseMsg : public MouseButtonMsg
	{
		friend class ModalLayer;
	public:
		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static ModalBlockedReleaseMsg_p	cast( const Object_p& pObject );

	protected:
		ModalBlockedReleaseMsg( char inputId, MouseButton button, Widget * pModalWidget, ModifierKeys modKeys, Coord pointerPos, int64_t timestamp );
	};


	//____ Internally posted messages ____________________________________________

	class MouseDragMsg : public MouseButtonMsg
	{
	public:

		inline static MouseDragMsg_p create( char inputId, MouseButton button, Object * pSource, const Coord& orgPos, const Coord& prevPos, ModifierKeys modKeys, Coord pointerPos, int64_t timestamp )
						{ return new MouseDragMsg( inputId, button, pSource, orgPos, prevPos, modKeys, pointerPos, timestamp ); }
		

		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static MouseDragMsg_p	cast( const Object_p& pObject );

		//.____ Content ______________________________________________

		Coord			draggedTotal() const;
		Coord			draggedNow() const;
		Coord			startPos() const;
		Coord			prevPos() const;
		Coord			currPos() const;
	protected:
		MouseDragMsg( char inputId, MouseButton button, Object * pSource, const Coord& orgPos, const Coord& prevPos, ModifierKeys modKeys, Coord pointerPos, int64_t timestamp );

		Coord			m_startPos;
		Coord			m_prevPos;
	};

	class MouseRepeatMsg : public MouseButtonMsg
	{
		friend class InputHandler;
	public:
		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static MouseRepeatMsg_p	cast( const Object_p& pObject );

	protected:
		MouseRepeatMsg( char inputId, MouseButton button, Widget * pWidget, ModifierKeys modKeys, Coord pointerPos, int64_t timestamp );
	};

	class MouseClickMsg : public MouseButtonMsg
	{
		friend class InputHandler;
	public:
		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static MouseClickMsg_p	cast( const Object_p& pObject );

	protected:
		MouseClickMsg( char inputId, MouseButton button, Widget * pWidget, ModifierKeys modKeys, Coord pointerPos, int64_t timestamp );
	};

	class MouseDoubleClickMsg : public MouseButtonMsg
	{
		friend class InputHandler;
	public:
		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static MouseDoubleClickMsg_p	cast( const Object_p& pObject );

	protected:
		MouseDoubleClickMsg( char inputId, MouseButton button, Widget * pWidget, ModifierKeys modKeys, Coord pointerPos, int64_t timestamp );
	};

	class KeyRepeatMsg : public KeyMsg
	{
		friend class InputHandler;
	public:
		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static KeyRepeatMsg_p	cast( const Object_p& pObject );

	protected:
		KeyRepeatMsg( char inputId, int native_keycode, Key translated_keycode, Widget * pWidget, ModifierKeys modKeys, Coord pointerPos, int64_t timestamp );
	};

} // namespace wg
#endif //WG_MSG_DOT_H
