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

#include <wg_msg.h>
#include <wg_key.h>
#include <wg_widget.h>
#include <wg_ctexteditor.h>
#include <wg_payload.h>

namespace wg
{

	//____ Msg ______________________________________________________________

	const char Msg::CLASSNAME[] = {"Msg"};

	bool Msg::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return Object::isInstanceOf(pClassName);
	}

	const char * Msg::className( void ) const
	{
		return CLASSNAME;
	}

	void Msg::setCopyTo( Receiver * pReceiver )
	{
		m_pCopyTo = pReceiver;
	}

	void Msg::setRepost( Object * pSource, Receiver * pCopyTo )
	{
		m_pRepostSource = pSource;
		m_pRepostCopyTo = pCopyTo;
	}

	void Msg::swallow()
	{
		m_pRepostSource = 0;
		m_pRepostCopyTo = 0;
	}

	bool Msg::doRepost()
	{
		if( m_pRepostSource )
		{
			if (!m_bReposted)
			{
				m_pOriginalSource = m_pSource;
				m_bReposted = true;
			}

			m_pSource = m_pRepostSource;
			m_pCopyTo = m_pRepostCopyTo;

			m_pRepostSource = 0;
			m_pRepostCopyTo = 0;

			return true;
		}
		return false;
	}



	bool Msg::isMouseMsg() const
	{
		if( m_type == MsgType::WheelRoll ||
			m_type == MsgType::MouseEnter ||
			m_type == MsgType::MouseLeave ||
			m_type == MsgType::MouseMove ||
			isMouseButtonMsg() )
			return true;
		else
			return false;

	}


	bool Msg::isMouseButtonMsg() const
	{
		if( m_type == MsgType::MouseClick ||
			m_type == MsgType::MouseDoubleClick ||
			m_type == MsgType::MouseDrag ||
			m_type == MsgType::MousePress ||
			m_type == MsgType::MouseRelease ||
			m_type == MsgType::MouseRepeat )
			return true;
		else
			return false;
	}

	bool Msg::isKeyMsg() const
	{
		if( m_type == MsgType::KeyPress ||
			m_type == MsgType::KeyRelease ||
			m_type == MsgType::KeyRepeat )
			return true;
		else
			return false;
	}

	//____ InputMsg __________________________________________________________

	const char InputMsg::CLASSNAME[] = {"InputMsg"};

	bool InputMsg::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return Msg::isInstanceOf(pClassName);
	}

	const char * InputMsg::className( void ) const
	{
		return CLASSNAME;
	}



	//____ KeyMsg ____________________________________________________________

	const char KeyMsg::CLASSNAME[] = {"KeyMsg"};

	bool KeyMsg::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return InputMsg::isInstanceOf(pClassName);
	}

	const char * KeyMsg::className( void ) const
	{
		return CLASSNAME;
	}

	bool KeyMsg::isCursorKey() const
	{
		if( m_translatedKeyCode == Key::Up || m_translatedKeyCode == Key::Down ||
			m_translatedKeyCode == Key::Left || m_translatedKeyCode == Key::Right )
			return true;
		else
			return false;
	}

	bool KeyMsg::isMovementKey() const
	{
		if( m_translatedKeyCode == Key::PageUp || m_translatedKeyCode == Key::PageDown ||
			m_translatedKeyCode == Key::Home || m_translatedKeyCode == Key::End ||
			isCursorKey() )
			return true;
		else
			return false;
	}

	//____ MouseButtonMsg _____________________________________________________

	const char MouseButtonMsg::CLASSNAME[] = {"MouseButtonMsg"};

	bool MouseButtonMsg::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return InputMsg::isInstanceOf(pClassName);
	}

	const char * MouseButtonMsg::className( void ) const
	{
		return CLASSNAME;
	}


	//____ FocusGainedMsg ________________________________________________________

	const char FocusGainedMsg::CLASSNAME[] = {"FocusGainedMsg"};

	FocusGainedMsg::FocusGainedMsg( char inputId, Widget * pWidget, ModifierKeys modKeys, Coord pointerPos, int64_t timestamp) : InputMsg(inputId, modKeys, pointerPos, timestamp)
	{
		m_type = MsgType::FocusGained;
		m_pSource	= pWidget;
		m_pCopyTo 	= pWidget;
	}

	bool FocusGainedMsg::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return Msg::isInstanceOf(pClassName);
	}

	const char * FocusGainedMsg::className( void ) const
	{
		return CLASSNAME;
	}


	//____ FocusLostMsg __________________________________________________________

	const char FocusLostMsg::CLASSNAME[] = {"FocusLostMsg"};

	FocusLostMsg::FocusLostMsg( char inputId, Widget * pWidget, ModifierKeys modKeys, Coord pointerPos, int64_t timestamp) : InputMsg(inputId, modKeys, pointerPos, timestamp)
	{
		m_type = MsgType::FocusLost;
		m_pSource	= pWidget;
		m_pCopyTo 	= pWidget;
	}

	bool FocusLostMsg::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return Msg::isInstanceOf(pClassName);
	}

	const char * FocusLostMsg::className( void ) const
	{
		return CLASSNAME;
	}


	//____ MouseEnterMsg _______________________________________________________

	const char MouseEnterMsg::CLASSNAME[] = {"MouseEnterMsg"};

	MouseEnterMsg::MouseEnterMsg( char inputId, Object * pObject, ModifierKeys modKeys, Coord pointerPos, int64_t timestamp ) : InputMsg(inputId, modKeys,pointerPos,timestamp)
	{
		m_type = MsgType::MouseEnter;
		m_pSource	= pObject;
//		m_pCopyTo 	= pWidget;
	}

	bool MouseEnterMsg::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return InputMsg::isInstanceOf(pClassName);
	}

	const char * MouseEnterMsg::className( void ) const
	{
		return CLASSNAME;
	}


	//____ MouseLeaveMsg ________________________________________________________

	const char MouseLeaveMsg::CLASSNAME[] = {"MouseLeaveMsg"};

	MouseLeaveMsg::MouseLeaveMsg( char inputId, Object * pSource, ModifierKeys modKeys, Coord pointerPos, int64_t timestamp ) : InputMsg(inputId, modKeys,pointerPos,timestamp)
	{
		m_type = MsgType::MouseLeave;
		m_pSource		= pSource;
	}

	bool MouseLeaveMsg::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return InputMsg::isInstanceOf(pClassName);
	}

	const char * MouseLeaveMsg::className( void ) const
	{
		return CLASSNAME;
	}

	//____ MouseMoveMsg __________________________________________________________

	const char MouseMoveMsg::CLASSNAME[] = {"MouseMoveMsg"};

	MouseMoveMsg::MouseMoveMsg( char inputId, Object * pSource, ModifierKeys modKeys, Coord pointerPos, int64_t timestamp ) : InputMsg(inputId, modKeys,pointerPos,timestamp)
	{
		m_type = MsgType::MouseMove;
		m_pSource		= pSource;
	}

	bool MouseMoveMsg::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return InputMsg::isInstanceOf(pClassName);
	}

	const char * MouseMoveMsg::className( void ) const
	{
		return CLASSNAME;
	}


	//____ MousePressMsg ______________________________________________________

	const char MousePressMsg::CLASSNAME[] = {"MousePressMsg"};

	MousePressMsg::MousePressMsg( char inputId, MouseButton button, Object * pSource, ModifierKeys modKeys, Coord pointerPos, int64_t timestamp ) : MouseButtonMsg(inputId,button,modKeys,pointerPos,timestamp)
	{
		m_type			= MsgType::MousePress;
		m_pSource		= pSource;
	}

	bool MousePressMsg::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return MouseButtonMsg::isInstanceOf(pClassName);
	}

	const char * MousePressMsg::className( void ) const
	{
		return CLASSNAME;
	}

	//____ MouseRepeatMsg ______________________________________________________

	const char MouseRepeatMsg::CLASSNAME[] = {"MouseRepeatMsg"};

	MouseRepeatMsg::MouseRepeatMsg( char inputId, MouseButton button, Object * pSource, ModifierKeys modKeys, Coord pointerPos, int64_t timestamp ) : MouseButtonMsg(inputId,button,modKeys,pointerPos,timestamp)
	{
		m_type			= MsgType::MouseRepeat;
		m_pSource		= pSource;
	}

	bool MouseRepeatMsg::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return MouseButtonMsg::isInstanceOf(pClassName);
	}

	const char * MouseRepeatMsg::className( void ) const
	{
		return CLASSNAME;
	}

	//____ MouseReleaseMsg _____________________________________________________

	const char MouseReleaseMsg::CLASSNAME[] = {"MouseReleaseMsg"};


	MouseReleaseMsg::MouseReleaseMsg( char inputId, MouseButton button, Object * pSource, bool bReleaseInside, ModifierKeys modKeys, Coord pointerPos, int64_t timestamp ) : MouseButtonMsg(inputId,button,modKeys,pointerPos,timestamp)
	{
		m_type			= MsgType::MouseRelease;
		m_pSource		= pSource;

		m_bReleaseInside = bReleaseInside;
	}

	bool MouseReleaseMsg::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return MouseButtonMsg::isInstanceOf(pClassName);
	}

	const char * MouseReleaseMsg::className( void ) const
	{
		return CLASSNAME;
	}

	bool MouseReleaseMsg::releaseInside() const
	{
		return m_bReleaseInside;
	}



	//____ MouseClickMsg _____________________________________________________

	const char MouseClickMsg::CLASSNAME[] = {"MouseClickMsg"};

	MouseClickMsg::MouseClickMsg( char inputId, MouseButton button, Object * pSource, ModifierKeys modKeys, Coord pointerPos, int64_t timestamp ) : MouseButtonMsg(inputId,button,modKeys,pointerPos,timestamp)
	{
		m_type = MsgType::MouseClick;
		m_pSource		= pSource;
	}

	bool MouseClickMsg::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return MouseButtonMsg::isInstanceOf(pClassName);
	}

	const char * MouseClickMsg::className( void ) const
	{
		return CLASSNAME;
	}

	//____ MouseDoubleClickMsg _____________________________________________________

	const char MouseDoubleClickMsg::CLASSNAME[] = {"MouseDoubleClickMsg"};

	MouseDoubleClickMsg::MouseDoubleClickMsg( char inputId, MouseButton button, Object * pSource, ModifierKeys modKeys, Coord pointerPos, int64_t timestamp ) : MouseButtonMsg(inputId,button,modKeys,pointerPos,timestamp)
	{
		m_type = MsgType::MouseDoubleClick;
		m_pSource		= pSource;
	}

	bool MouseDoubleClickMsg::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return MouseButtonMsg::isInstanceOf(pClassName);
	}

	const char * MouseDoubleClickMsg::className( void ) const
	{
		return CLASSNAME;
	}

	//____ KeyPressMsg ___________________________________________________________

	const char KeyPressMsg::CLASSNAME[] = {"KeyPressMsg"};

	KeyPressMsg::KeyPressMsg( char inputId, int nativeKeyCode, Key translatedKeyCode, Widget * pWidget, ModifierKeys modKeys, Coord pointerPos, int64_t timestamp ) : KeyMsg(inputId,nativeKeyCode,translatedKeyCode,modKeys,pointerPos,timestamp)
	{
		m_type 			= MsgType::KeyPress;
		m_pSource		= pWidget;
		m_pCopyTo		= pWidget;
	}

	bool KeyPressMsg::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return KeyMsg::isInstanceOf(pClassName);
	}

	const char * KeyPressMsg::className( void ) const
	{
		return CLASSNAME;
	}

	//____ KeyRepeatMsg ___________________________________________________________

	const char KeyRepeatMsg::CLASSNAME[] = {"KeyRepeatMsg"};

	KeyRepeatMsg::KeyRepeatMsg( char inputId, int native_keycode, Key translated_keycode, Widget * pWidget, ModifierKeys modKeys, Coord pointerPos, int64_t timestamp ) : KeyMsg(inputId,native_keycode, translated_keycode,modKeys,pointerPos,timestamp)
	{
		m_type 			= MsgType::KeyRepeat;
		m_pSource		= pWidget;
		m_pCopyTo 		= pWidget;
	}

	bool KeyRepeatMsg::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return KeyMsg::isInstanceOf(pClassName);
	}

	const char * KeyRepeatMsg::className( void ) const
	{
		return CLASSNAME;
	}

	//____ KeyReleaseMsg ________________________________________________________

	const char KeyReleaseMsg::CLASSNAME[] = {"KeyReleaseMsg"};

	KeyReleaseMsg::KeyReleaseMsg( char inputId, int native_keycode, Key translated_keycode, Widget * pWidget, ModifierKeys modKeys, Coord pointerPos, int64_t timestamp ) : KeyMsg(inputId,native_keycode,translated_keycode,modKeys,pointerPos,timestamp)
	{
		m_type 			= MsgType::KeyRelease;
		m_pSource		= pWidget;
		m_pCopyTo		= pWidget;
	}

	bool KeyReleaseMsg::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return KeyMsg::isInstanceOf(pClassName);
	}

	const char * KeyReleaseMsg::className( void ) const
	{
		return CLASSNAME;
	}

	//____ TextInputMsg __________________________________________________________

	const char TextInputMsg::CLASSNAME[] = {"TextInputMsg"};


	TextInputMsg::TextInputMsg( char inputId, String text, Widget * pWidget )
	{
		m_type			= MsgType::TextInput;
		m_text			= text;
		m_pSource		= pWidget;
		m_pCopyTo		= pWidget;
		m_inputId		= inputId;
	}

	String TextInputMsg::text() const
	{
		return m_text;
	}

	bool TextInputMsg::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return Msg::isInstanceOf(pClassName);
	}

	const char * TextInputMsg::className( void ) const
	{
		return CLASSNAME;
	}

	//____ EditCommandMsg __________________________________________________________

	const char EditCommandMsg::CLASSNAME[] = {"EditCommandMsg"};


	EditCommandMsg::EditCommandMsg( char inputId, EditCmd command, Widget * pWidget )
	{
		m_type			= MsgType::EditCommand;
		m_command		= command;
		m_pSource		= pWidget;
		m_pCopyTo		= pWidget;
		m_inputId		= inputId;
	}

	bool EditCommandMsg::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return Msg::isInstanceOf(pClassName);
	}

	const char * EditCommandMsg::className( void ) const
	{
		return CLASSNAME;
	}


	//____ WheelRollMsg __________________________________________________________

	const char WheelRollMsg::CLASSNAME[] = {"WheelRollMsg"};

	WheelRollMsg::WheelRollMsg( char inputId, int wheel, Coord distance, Widget * pWidget, ModifierKeys modKeys, Coord pointerPos, int64_t timestamp ) : InputMsg( inputId, modKeys, pointerPos, timestamp )
	{
		m_type			= MsgType::WheelRoll;
		m_wheel			= wheel;
		m_distance		= distance;
		m_pSource		= pWidget;
		m_pCopyTo		= pWidget;
	}

	int WheelRollMsg::wheel() const
	{
		return m_wheel;
	}

	Coord WheelRollMsg::distance() const
	{
		return m_distance;
	}

	bool WheelRollMsg::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return InputMsg::isInstanceOf(pClassName);
	}

	const char * WheelRollMsg::className( void ) const
	{
		return CLASSNAME;
	}

	//____ TickMsg _______________________________________________________________

	const char TickMsg::CLASSNAME[] = {"TickMsg"};

	TickMsg::TickMsg( int64_t timestamp, int timediff )
	{
		m_type = MsgType::Tick;
		m_timestamp = timestamp;
		m_timediff = timediff;
	}

	bool TickMsg::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return Msg::isInstanceOf(pClassName);
	}

	const char * TickMsg::className( void ) const
	{
		return CLASSNAME;
	}

	//____ PointerChangeMsg _______________________________________________________________

	const char PointerChangeMsg::CLASSNAME[] = {"PointerChangeMsg"};

	PointerChangeMsg::PointerChangeMsg( char inputId, PointerStyle style )
	{
		m_type = MsgType::PointerChange;
		m_style = style;
		m_inputId = inputId;
	}

	PointerStyle PointerChangeMsg::style() const
	{
		return m_style;
	}

	bool PointerChangeMsg::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return Msg::isInstanceOf(pClassName);
	}

	const char * PointerChangeMsg::className( void ) const
	{
		return CLASSNAME;
	}


	//____ DragNDropMsg _______________________________________________________

	const char DragNDropMsg::CLASSNAME[] = {"DragNDropMsg"};

	DragNDropMsg::DragNDropMsg( MsgType type, Widget * pSource, int pickCategory, BasicPayload * pPayload, Widget * pPickedFrom, Widget * pFinalReceiver, ModifierKeys modKeys, Coord pointerPos )
	{
		m_type = type;
		m_pSource = pSource;
		m_pCopyTo = pSource;
		m_pFinalRecipient = pFinalReceiver;
		m_modKeys = modKeys;
		m_pointerPos = pointerPos;

		m_pPickedFrom = pPickedFrom;
		m_pickCategory = pickCategory;
		m_pPayload = pPayload;
	}

	bool DragNDropMsg::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return Msg::isInstanceOf(pClassName);
	}

	const char * DragNDropMsg::className( void ) const
	{
		return CLASSNAME;
	}

	BasicPayload_p DragNDropMsg::payload() const
	{
		return m_pPayload;
	};

	//____ DropPickMsg ___________________________________________________

	const char DropPickMsg::CLASSNAME[] = {"DropPickMsg"};

	DropPickMsg::DropPickMsg( Widget * pSource, Coord pickOfs, Widget * pFinalReceiver, ModifierKeys modKeys, Coord pointerPos )
	: DragNDropMsg( MsgType::DropPick, pSource, 0, nullptr, pSource, pFinalReceiver, modKeys, pointerPos ),
	m_dragWidgetPointerOfs(Coord()-pickOfs),
	m_pickOfs(pickOfs)
	{
	}

	bool DropPickMsg::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return DragNDropMsg::isInstanceOf(pClassName);
	}

	const char * DropPickMsg::className( void ) const
	{
		return CLASSNAME;
	}

	void DropPickMsg::setPayload( BasicPayload * pPayload )
	{
		m_pPayload = pPayload;
	}

	void DropPickMsg::setDragWidget( Widget * pWidget, Coord pointerOfs )
	{
		m_pDragWidget = pWidget;
		m_dragWidgetPointerOfs = pointerOfs;
	}

	Widget_p DropPickMsg::dragWidget() const
	{
		return m_pDragWidget;
	}

	Coord DropPickMsg::dragWidgetPointerOfs() const
	{
		return m_dragWidgetPointerOfs;
	}

	CoordI DropPickMsg::dragWidgetPointerOfsRaw() const
	{
		return Util::qpixToRaw(m_dragWidgetPointerOfs);
	}

	//____ DropProbeMsg ___________________________________________________

	const char DropProbeMsg::CLASSNAME[] = { "DropProbeMsg" };

	DropProbeMsg::DropProbeMsg(Widget * pSource, int pickCategory, BasicPayload * pPayload, Widget * pPickedFrom, Widget * pFinalReceiver, ModifierKeys modKeys, Coord pointerPos)
		: DragNDropMsg( MsgType::DropProbe, pSource, pickCategory, pPayload, pPickedFrom, pFinalReceiver, modKeys, pointerPos)
	{
	}

	bool DropProbeMsg::isInstanceOf(const char * pClassName) const
	{
		if (pClassName == CLASSNAME)
			return true;

		return DragNDropMsg::isInstanceOf(pClassName);
	}

	const char * DropProbeMsg::className(void) const
	{
		return CLASSNAME;
	}

	void DropProbeMsg::accept(bool bAccepted)
	{
		m_bAccepted = bAccepted;
	}

	//____ DropEnterMsg ___________________________________________________

	const char DropEnterMsg::CLASSNAME[] = { "DropEnterMsg" };

	DropEnterMsg::DropEnterMsg(Widget * pSource, int pickCategory, BasicPayload * pPayload, Widget * pPickedFrom, Widget * pDragWidget,
							   Widget * pFinalReceiver, ModifierKeys modKeys, Coord pointerPos)
		: DragNDropMsg( MsgType::DropEnter, pSource, pickCategory, pPayload, pPickedFrom, pFinalReceiver, modKeys, pointerPos),
		m_pDragWidget(pDragWidget)
	{
	}

	bool DropEnterMsg::isInstanceOf(const char * pClassName) const
	{
		if (pClassName == CLASSNAME)
			return true;

		return DragNDropMsg::isInstanceOf(pClassName);
	}

	const char * DropEnterMsg::className(void) const
	{
		return CLASSNAME;
	}

	void DropEnterMsg::setDragWidget(Widget * pWidget)
	{
		m_pDragWidget = pWidget;
	}

	Widget_p DropEnterMsg::dragWidget() const
	{
		return m_pDragWidget;
	}


	//____ DropMoveMsg ___________________________________________________

	const char DropMoveMsg::CLASSNAME[] = { "DropMoveMsg" };

	DropMoveMsg::DropMoveMsg(Widget * pSource, int pickCategory, BasicPayload * pPayload, Widget * pPickedFrom, Widget * pDragWidget,
							 Widget * pFinalReceiver, ModifierKeys modKeys, Coord pointerPos)
		: DragNDropMsg( MsgType::DropMove, pSource, pickCategory, pPayload, pPickedFrom, pFinalReceiver, modKeys, pointerPos),
		m_pDragWidget(pDragWidget)
	{
	}

	bool DropMoveMsg::isInstanceOf(const char * pClassName) const
	{
		if (pClassName == CLASSNAME)
			return true;

		return DragNDropMsg::isInstanceOf(pClassName);
	}

	const char * DropMoveMsg::className(void) const
	{
		return CLASSNAME;
	}

	void DropMoveMsg::setDragWidget(Widget * pWidget)
	{
		m_pDragWidget = pWidget;
	}

	Widget_p DropMoveMsg::dragWidget() const
	{
		return m_pDragWidget;
	}

	//____ DropLeaveMsg ___________________________________________________

	const char DropLeaveMsg::CLASSNAME[] = { "DropLeaveMsg" };

	DropLeaveMsg::DropLeaveMsg(Widget * pSource, int pickCategory, BasicPayload * pPayload, Widget * pPickedFrom, ModifierKeys modKeys, Coord pointerPos)
		: DragNDropMsg( MsgType::DropLeave, pSource, pickCategory, pPayload, pPickedFrom, nullptr, modKeys, pointerPos)
	{
	}

	bool DropLeaveMsg::isInstanceOf(const char * pClassName) const
	{
		if (pClassName == CLASSNAME)
			return true;

		return DragNDropMsg::isInstanceOf(pClassName);
	}

	const char * DropLeaveMsg::className(void) const
	{
		return CLASSNAME;
	}

	//____ DropDeliverMsg ___________________________________________________

	const char DropDeliverMsg::CLASSNAME[] = { "DropDeliverMsg" };

	DropDeliverMsg::DropDeliverMsg(Widget * pSource, int pickCategory, BasicPayload * pPayload, Widget * pPickedFrom, Widget * pFinalReceiver, ModifierKeys modKeys, Coord pointerPos)
		: DragNDropMsg( MsgType::DropDeliver, pSource, pickCategory, pPayload, pPickedFrom, pFinalReceiver, modKeys, pointerPos),
		m_bAccepted(false)
	{
	}

	bool DropDeliverMsg::isInstanceOf(const char * pClassName) const
	{
		if (pClassName == CLASSNAME)
			return true;

		return DragNDropMsg::isInstanceOf(pClassName);
	}

	const char * DropDeliverMsg::className(void) const
	{
		return CLASSNAME;
	}

	Widget_p DropDeliverMsg::deliveredTo() const
	{
		return static_cast<Widget*>(m_pSource.rawPtr());
	}

	void DropDeliverMsg::accept(bool bAccept)
	{
		m_bAccepted = bAccept;
	}

	//____ DropCancelMsg ___________________________________________________

	const char DropCancelMsg::CLASSNAME[] = { "DropCancelMsg" };

	DropCancelMsg::DropCancelMsg(Widget * pPickedFrom, int pickCategory, BasicPayload * pPayload, ModifierKeys modKeys, Coord pointerPos)
		: DragNDropMsg( MsgType::DropCancel, pPickedFrom, pickCategory, pPayload, pPickedFrom, nullptr, modKeys, pointerPos)
	{
	}

	bool DropCancelMsg::isInstanceOf(const char * pClassName) const
	{
		if (pClassName == CLASSNAME)
			return true;

		return DragNDropMsg::isInstanceOf(pClassName);
	}

	const char * DropCancelMsg::className(void) const
	{
		return CLASSNAME;
	}

	//____ DropCompleteMsg ___________________________________________________

	const char DropCompleteMsg::CLASSNAME[] = { "DropCompleteMsg" };

	DropCompleteMsg::DropCompleteMsg(Widget * pPicked, Widget * pDeliveree, int pickCategory, BasicPayload * pPayload, ModifierKeys modKeys, Coord pointerPos)
		: DragNDropMsg( MsgType::DropComplete, pPicked, pickCategory, pPayload, pPicked, nullptr, modKeys, pointerPos),
		m_pDeliveree(pDeliveree)
	{
	}

	bool DropCompleteMsg::isInstanceOf(const char * pClassName) const
	{
		if (pClassName == CLASSNAME)
			return true;

		return DragNDropMsg::isInstanceOf(pClassName);
	}

	const char * DropCompleteMsg::className(void) const
	{
		return CLASSNAME;
	}

	Widget_p DropCompleteMsg::deliveredTo() const
	{
		return m_pDeliveree;
	}

	//____ SelectMsg _________________________________________________________

	const char SelectMsg::CLASSNAME[] = {"SelectMsg"};

	SelectMsg::SelectMsg( Object * pSource )
	{
		m_type 			= MsgType::Select;
		m_pSource 		= pSource;
	}

	bool SelectMsg::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return Msg::isInstanceOf(pClassName);
	}

	const char * SelectMsg::className( void ) const
	{
		return CLASSNAME;
	}

	//____ ToggleMsg ______________________________________________

	const char ToggleMsg::CLASSNAME[] = {"ToggleMsg"};

	ToggleMsg::ToggleMsg( Object * pSource, bool bSet )
	{
		m_type = MsgType::Toggle;
		m_pSource = pSource;
		m_bSet = bSet;
	}

	bool ToggleMsg::isSet() const
	{
		return m_bSet;
	}

	bool ToggleMsg::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return Msg::isInstanceOf(pClassName);
	}

	const char * ToggleMsg::className( void ) const
	{
		return CLASSNAME;
	}

	//____ ValueChangeMsg ___________________________________________

	const char ValueUpdateMsg::CLASSNAME[] = {"ValueUpdateMsg"};

	ValueUpdateMsg::ValueUpdateMsg( Object * pSource, int64_t value, double fraction, bool bFinal )
	{
		m_type = MsgType::ValueUpdate;
		m_pSource = pSource;
		m_value = value;
		m_fraction = fraction;
		m_bFinal = bFinal;
	}

	int64_t ValueUpdateMsg::value() const
	{
		return m_value;
	}

	double ValueUpdateMsg::fraction() const
	{
		return m_fraction;
	}

	bool ValueUpdateMsg::isFinal() const
	{
		return m_bFinal;
	}

	bool ValueUpdateMsg::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return Msg::isInstanceOf(pClassName);
	}

	const char * ValueUpdateMsg::className( void ) const
	{
		return CLASSNAME;
	}

	//____ RangeUpdateMsg ___________________________________________

	const char RangeUpdateMsg::CLASSNAME[] = {"RangeUpdateMsg"};

	RangeUpdateMsg::RangeUpdateMsg( Object * pSource, int intOfs, int intLength, double fracOfs, double fracLength, bool bFinal )
	{
		m_type = MsgType::RangeUpdate;
		m_pSource 	= pSource;
		m_ofs		= intOfs;
		m_length	= intLength;
		m_fracOfs	= fracOfs;
		m_fracLength= fracLength;
		m_bFinal	= bFinal;
	}

	bool RangeUpdateMsg::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return Msg::isInstanceOf(pClassName);
	}

	const char * RangeUpdateMsg::className( void ) const
	{
		return CLASSNAME;
	}

	int RangeUpdateMsg::offset() const
	{
		return m_ofs;
	}

	int RangeUpdateMsg::length() const
	{
		return m_length;
	}

	double RangeUpdateMsg::fracOffset() const
	{
		return m_fracOfs;
	}

	double RangeUpdateMsg::fracLength() const
	{
		return m_fracLength;
	}

	bool RangeUpdateMsg::isFinal() const
	{
		return m_bFinal;
	}


	//____ TextEditMsg ________________________________________________________

	const char TextEditMsg::CLASSNAME[] = {"TextEditMsg"};

	CTextEditor_p TextEditMsg::text() const
	{
		return m_pText;
	}

	TextEditMsg::TextEditMsg( CTextEditor * pText, bool bFinal )
	{
		m_type 		= MsgType::TextEdit;
		m_pSource 	= pText->object();
		m_pText 	= pText;
		m_bFinal	= bFinal;
	}

	bool TextEditMsg::isFinal() const
	{
		return m_bFinal;
	}

	bool TextEditMsg::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return Msg::isInstanceOf(pClassName);
	}

	const char * TextEditMsg::className( void ) const
	{
		return CLASSNAME;
	}

	//____ ItemMsg ________________________________________________________

	const char ItemMsg::CLASSNAME[] = {"ItemMsg"};

	ItemMsg::ItemMsg( Object * pSource, int itemId, Object * pItemObject ) : m_item(itemId,pItemObject)
	{
		m_pSource 	= pSource;
	}

	bool ItemMsg::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return Msg::isInstanceOf(pClassName);
	}

	const char * ItemMsg::className( void ) const
	{
		return CLASSNAME;
	}

	int ItemMsg::itemId() const
	{
		return m_item.id;
	}

	Object_p ItemMsg::itemObject() const
	{
		return m_item.pObject;
	}

	//____ ItemToggleMsg ________________________________________________________

	const char ItemToggleMsg::CLASSNAME[] = {"ItemToggleMsg"};

	ItemToggleMsg::ItemToggleMsg( Object * pSource, int itemId, Object * pItemObject, bool bSet ) : ItemMsg(pSource,itemId,pItemObject)
	{
		m_type = MsgType::ItemToggle;
		m_bSet = bSet;
	}

	bool ItemToggleMsg::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return ItemMsg::isInstanceOf(pClassName);
	}

	const char * ItemToggleMsg::className( void ) const
	{
		return CLASSNAME;
	}

	bool ItemToggleMsg::isSet() const
	{
		return m_bSet;
	}

	//____ ItemMousePressMsg ________________________________________________________

	const char ItemMousePressMsg::CLASSNAME[] = {"ItemMousePressMsg"};

	ItemMousePressMsg::ItemMousePressMsg( Object * pSource, int itemId, Object * pItemObject, MouseButton button ) : ItemMsg(pSource,itemId,pItemObject)
	{
		m_type = MsgType::ItemMousePress;
		m_button = button;
	}

	bool ItemMousePressMsg::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return ItemMsg::isInstanceOf(pClassName);
	}

	const char * ItemMousePressMsg::className( void ) const
	{
		return CLASSNAME;
	}

	MouseButton ItemMousePressMsg::button() const
	{
		return m_button;
	}

	//____ ItemListMsg ________________________________________________________

	const char ItemListMsg::CLASSNAME[] = {"ItemListMsg"};

	ItemListMsg::ItemListMsg( Object * pSource, int nbItems, ItemInfo * pItems )
	{
		m_pSource 	= pSource;
		m_nbItems	= nbItems;
		m_pItems	= pItems;

		assert(pItems != nullptr);

	}

	ItemListMsg::~ItemListMsg()
	{
		delete [] m_pItems;
	}

	bool ItemListMsg::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return Msg::isInstanceOf(pClassName);
	}

	const char * ItemListMsg::className( void ) const
	{
		return CLASSNAME;
	}

	int ItemListMsg::nbItems() const
	{
		return m_nbItems;
	}

	const ItemInfo * ItemListMsg::items() const
	{
		return m_pItems;
	}

	//____ ItemsSelectMsg ________________________________________________________

	const char ItemsSelectMsg::CLASSNAME[] = {"ItemsSelectMsg"};

	ItemsSelectMsg::ItemsSelectMsg( Object * pSource, int nbItems, ItemInfo * pItems ) : ItemListMsg(pSource,nbItems,pItems)
	{
		m_type = MsgType::ItemsSelect;
	}

	bool ItemsSelectMsg::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return ItemListMsg::isInstanceOf(pClassName);
	}

	const char * ItemsSelectMsg::className( void ) const
	{
		return CLASSNAME;
	}

	//____ ItemsUnselectMsg ________________________________________________________

	const char ItemsUnselectMsg::CLASSNAME[] = {"ItemsUnselectMsg"};

	ItemsUnselectMsg::ItemsUnselectMsg( Object * pSource, int nbItems, ItemInfo * pItems ) : ItemListMsg(pSource,nbItems,pItems)
	{
		m_type = MsgType::ItemsUnselect;
	}

	bool ItemsUnselectMsg::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return ItemListMsg::isInstanceOf(pClassName);
	}

	const char * ItemsUnselectMsg::className( void ) const
	{
		return CLASSNAME;
	}

	//____ PopupClosedMsg ____________________________________________

	const char PopupClosedMsg::CLASSNAME[] = {"PopupClosedMsg"};

	PopupClosedMsg::PopupClosedMsg( Widget * pPopup, const Widget_wp& pCaller )
	{
		m_type			= MsgType::PopupClosed;
		m_pCopyTo		= pCaller.rawPtr();
		m_pSource		= pPopup;
	}

	bool PopupClosedMsg::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return Msg::isInstanceOf(pClassName);
	}

	const char * PopupClosedMsg::className( void ) const
	{
		return CLASSNAME;
	}

	//____ ModalMoveOutsideMsg ___________________________________________________

	const char ModalMoveOutsideMsg::CLASSNAME[] = {"ModalMoveOutsideMsg"};

	ModalMoveOutsideMsg::ModalMoveOutsideMsg( char inputId, Widget * pSource, ModifierKeys modKeys, Coord pointerPos, int64_t timestamp ) : InputMsg(inputId, modKeys, pointerPos, timestamp)
	{
		m_type = MsgType::ModalMoveOutside;
		m_pSource		= pSource;
	}

	bool ModalMoveOutsideMsg::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return InputMsg::isInstanceOf(pClassName);
	}

	const char * ModalMoveOutsideMsg::className( void ) const
	{
		return CLASSNAME;
	}


	//____ ModalBlockedPressMsg _______________________________________________

	const char ModalBlockedPressMsg::CLASSNAME[] = {"ModalBlockedPressMsg"};

	ModalBlockedPressMsg::ModalBlockedPressMsg( char inputId, MouseButton button, Widget * pSource, ModifierKeys modKeys, Coord pointerPos, int64_t timestamp ) : MouseButtonMsg(inputId, button, modKeys, pointerPos, timestamp )
	{
		m_type			= MsgType::ModalBlockedPress;
		m_pSource 		= pSource;
	}

	bool ModalBlockedPressMsg::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return MouseButtonMsg::isInstanceOf(pClassName);
	}

	const char * ModalBlockedPressMsg::className( void ) const
	{
		return CLASSNAME;
	}

	//____ ModalBlockedReleaseMsg _____________________________________________

	const char ModalBlockedReleaseMsg::CLASSNAME[] = {"ModalBlockedReleaseMsg"};

	ModalBlockedReleaseMsg::ModalBlockedReleaseMsg( char inputId, MouseButton button, Widget * pSource, ModifierKeys modKeys, Coord pointerPos, int64_t timestamp ) : MouseButtonMsg(inputId, button,modKeys,pointerPos,timestamp)
	{
		m_type			= MsgType::ModalBlockedRelease;
		m_pSource 		= pSource;
	}

	bool ModalBlockedReleaseMsg::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return MouseButtonMsg::isInstanceOf(pClassName);
	}

	const char * ModalBlockedReleaseMsg::className( void ) const
	{
		return CLASSNAME;
	}


	//____ MouseDragMsg _________________________________________________________

	const char MouseDragMsg::CLASSNAME[] = {"MouseDragMsg"};


	MouseDragMsg::MouseDragMsg( char inputId, MouseButton button, Object * pSource, const Coord& startPos, const Coord& prevPos, ModifierKeys modKeys, Coord pointerPos, int64_t timestamp ) : MouseButtonMsg(inputId, button,modKeys,pointerPos,timestamp)
	{
		m_type 			= MsgType::MouseDrag;
		m_pSource		= pSource;

		m_startPos = startPos;
		m_prevPos = prevPos;
	}

	bool MouseDragMsg::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return MouseButtonMsg::isInstanceOf(pClassName);
	}

	const char * MouseDragMsg::className( void ) const
	{
		return CLASSNAME;
	}

	Coord MouseDragMsg::draggedTotal() const
	{
		return m_pointerPos - m_startPos;
	}

	Coord MouseDragMsg::draggedNow() const
	{
		return m_pointerPos - m_prevPos;
	}

	Coord MouseDragMsg::currPos() const
	{
		return m_pointerPos;
	}

	Coord MouseDragMsg::startPos() const
	{
		return m_startPos;
	}

	Coord MouseDragMsg::prevPos() const
	{
		return m_prevPos;
	}

	CoordI MouseDragMsg::draggedTotalRaw() const
	{
		return Util::qpixToRaw(m_pointerPos - m_startPos);
	}

	CoordI MouseDragMsg::draggedNowRaw() const
	{
		return Util::qpixToRaw( m_pointerPos - m_prevPos );
	}

	CoordI MouseDragMsg::currPosRaw() const
	{
		return Util::qpixToRaw( m_pointerPos );
	}

	CoordI MouseDragMsg::startPosRaw() const
	{
		return Util::qpixToRaw(m_startPos);
	}

	CoordI MouseDragMsg::prevPosRaw() const
	{
		return Util::qpixToRaw(m_prevPos);
	}


} // namespace wg
