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

#include <wg_event.h>
#include <wg_key.h>
#include <wg_widget.h>
#include <wg_button.h>
#include <wg_checkbox.h>
#include <wg_radiobutton.h>
#include <wg_animplayer.h>
#include <wg_tablist.h>
#include <wg_valuedisplay.h>
#include <wg_valueeditor.h>
#include <wg_scrollbar.h>
#include <wg_textdisplay.h>
#include <wg_lineeditor.h>
#include <wg_menu.h>
#include <wg_tablepanel.h>

namespace WgEvent
{
	//____ Event ______________________________________________________________

	WgWidget * Event::Widget() const
	{
		return m_pWidget.GetRealPtr();
	}

	WgWidget * Event::ForwardedFrom() const
	{
		return m_pForwardedFrom.GetRealPtr();
	}


	bool Event::IsMouseEvent() const
	{
		if( m_type == WG_EVENT_MOUSEWHEEL_ROLL ||
			m_type == WG_EVENT_MOUSE_ENTER ||
			m_type == WG_EVENT_MOUSE_LEAVE ||	
			m_type == WG_EVENT_MOUSE_MOVE ||	
			m_type == WG_EVENT_MOUSE_POSITION ||
			IsMouseButtonEvent() )
			return true;
		else
			return false;
			
	}
	
	bool Event::IsMouseButtonEvent() const
	{
		if( m_type == WG_EVENT_MOUSEBUTTON_CLICK ||
			m_type == WG_EVENT_MOUSEBUTTON_DOUBLE_CLICK ||
			m_type == WG_EVENT_MOUSEBUTTON_DRAG ||
			m_type == WG_EVENT_MOUSEBUTTON_PRESS ||
			m_type == WG_EVENT_MOUSEBUTTON_RELEASE ||
			m_type == WG_EVENT_MOUSEBUTTON_REPEAT )
			return true;
		else
			return false;
	}
	
	bool Event::IsKeyEvent() const
	{
		if( m_type == WG_EVENT_KEY_PRESS ||
			m_type == WG_EVENT_KEY_RELEASE ||
			m_type == WG_EVENT_KEY_REPEAT )
			return true;
		else
			return false;
	}

	void Event::_cloneContentFrom( const Event * pOrg )
	{
		m_type				= pOrg->m_type;
		m_modKeys			= pOrg->m_modKeys;
		m_timestamp			= pOrg->m_timestamp;
		m_bIsForWidget		= pOrg->m_bIsForWidget;
		m_pWidget			= pOrg->m_pWidget;
		m_pointerLocalPos	= pOrg->m_pointerLocalPos;
		m_pointerScreenPos	= pOrg->m_pointerScreenPos;
	}

	//____ MouseButtonEvent ____________________________________________________

	void MouseButtonEvent::_cloneContentFrom( const Event * _pOrg )
	{
		const MouseButtonEvent * pOrg = static_cast<const MouseButtonEvent*>(_pOrg);
		
		m_button = pOrg->m_button;
		Event::_cloneContentFrom( pOrg );
	}

	//____ KeyEvent ____________________________________________________________

	bool KeyEvent::IsCursorKey() const
	{
		if( m_translatedKeyCode == WG_KEY_UP || m_translatedKeyCode == WG_KEY_DOWN ||
			m_translatedKeyCode == WG_KEY_LEFT || m_translatedKeyCode == WG_KEY_RIGHT )
			return true;
		else
			return false;
	}
	
	bool KeyEvent::IsMovementKey() const
	{
		if( m_translatedKeyCode == WG_KEY_PAGE_UP || m_translatedKeyCode == WG_KEY_PAGE_DOWN ||
			m_translatedKeyCode == WG_KEY_HOME || m_translatedKeyCode == WG_KEY_END ||
			IsCursorKey() )
			return true;
		else
			return false;
	}

	void KeyEvent::_cloneContentFrom( const Event * _pOrg )
	{
		const KeyEvent * pOrg = static_cast<const KeyEvent*>(_pOrg);

		m_nativeKeyCode		= pOrg->m_nativeKeyCode;
		m_translatedKeyCode	= pOrg->m_translatedKeyCode;
		Event::_cloneContentFrom( pOrg );
	}
	
	//____ FocusGained ________________________________________________________

	FocusGained::FocusGained()
	{
		m_type = WG_EVENT_FOCUS_GAINED;
	}

	//____ FocusLost __________________________________________________________

	FocusLost::FocusLost()
	{
		m_type = WG_EVENT_FOCUS_LOST;
	}

	//____ MouseEnter _______________________________________________________

	MouseEnter::MouseEnter( const WgCoord& pos )
	{
		m_type = WG_EVENT_MOUSE_ENTER;

		m_pointerLocalPos = pos;
		m_pointerScreenPos = pos;
	}

	MouseEnter::MouseEnter( WgWidget * pWidget )
	{
		m_type = WG_EVENT_MOUSE_ENTER;
		m_bIsForWidget	= true;
		m_pWidget 		= pWidget;
	}

	//____ MouseLeave ________________________________________________________

	MouseLeave::MouseLeave()
	{
		m_type = WG_EVENT_MOUSE_LEAVE;
	}

	MouseLeave::MouseLeave( WgWidget * pWidget )
	{
		m_type = WG_EVENT_MOUSE_LEAVE;
		m_bIsForWidget	= true;
		m_pWidget 		= pWidget;
	}

	//____ MouseMove __________________________________________________________

	MouseMove::MouseMove( const WgCoord& pos )
	{
		m_type = WG_EVENT_MOUSE_MOVE;

		m_pointerLocalPos = pos;
		m_pointerScreenPos = pos;
	}

	MouseMove::MouseMove( WgWidget * pWidget )
	{
		m_type = WG_EVENT_MOUSE_MOVE;
		m_bIsForWidget	= true;
		m_pWidget 		= pWidget;
	}

	//____ MouseButtonPress ______________________________________________________

	MouseButtonPress::MouseButtonPress( int button ) : MouseButtonEvent(button)
	{
		m_type = WG_EVENT_MOUSEBUTTON_PRESS;
	}

	MouseButtonPress::MouseButtonPress( int button, WgWidget * pWidget ) : MouseButtonEvent(button)
	{
		m_type			= WG_EVENT_MOUSEBUTTON_PRESS;
		m_bIsForWidget	= true;
		m_pWidget 		= pWidget;
	}

	//____ MouseButtonRepeat ______________________________________________________

	MouseButtonRepeat::MouseButtonRepeat( int button ) : MouseButtonEvent(button)
	{
		m_type = WG_EVENT_MOUSEBUTTON_REPEAT;
	}

	MouseButtonRepeat::MouseButtonRepeat( int button, WgWidget * pWidget ) : MouseButtonEvent(button)
	{
		m_type			= WG_EVENT_MOUSEBUTTON_REPEAT;
		m_bIsForWidget	= true;
		m_pWidget 		= pWidget;
	}

	//____ MouseButtonRelease _____________________________________________________

	MouseButtonRelease::MouseButtonRelease( int button ) : MouseButtonEvent(button)
	{
		m_type = WG_EVENT_MOUSEBUTTON_RELEASE;

		m_bPressInside = true;			// Always assumed to have had the press inside our window.
		m_bReleaseInside = true;			// Always assumed to be inside our window now.
	}

	MouseButtonRelease::MouseButtonRelease( int button, WgWidget * pWidget, bool bPressInside, bool bReleaseInside ) : MouseButtonEvent(button)
	{
		m_type			= WG_EVENT_MOUSEBUTTON_RELEASE;
		m_bIsForWidget	= true;
		m_pWidget 		= pWidget;

		m_bPressInside = bPressInside;
		m_bReleaseInside = bReleaseInside;
	}

	void MouseButtonRelease::_cloneContentFrom( const Event * _pOrg )
	{
		const MouseButtonRelease * pOrg = static_cast<const MouseButtonRelease*>(_pOrg);
		
		m_bPressInside		= pOrg->m_bPressInside;
		m_bReleaseInside	= pOrg->m_bReleaseInside;
		Event::_cloneContentFrom( pOrg );
	}


	bool MouseButtonRelease::PressInside() const
	{
		return m_bPressInside;
	}

	bool MouseButtonRelease::ReleaseInside() const
	{
		return m_bReleaseInside;
	}



	//____ MouseButtonClick _____________________________________________________

	MouseButtonClick::MouseButtonClick( int button ) : MouseButtonEvent(button)
	{
		m_type = WG_EVENT_MOUSEBUTTON_CLICK;
	}

	MouseButtonClick::MouseButtonClick( int button, WgWidget * pWidget ) : MouseButtonEvent(button)
	{
		m_type = WG_EVENT_MOUSEBUTTON_CLICK;
		m_bIsForWidget	= true;
		m_pWidget 		= pWidget;
	}


	//____ MouseButtonDoubleClick _____________________________________________________

	MouseButtonDoubleClick::MouseButtonDoubleClick( int button ) : MouseButtonEvent(button)
	{
		m_type = WG_EVENT_MOUSEBUTTON_DOUBLE_CLICK;
	}

	MouseButtonDoubleClick::MouseButtonDoubleClick( int button, WgWidget * pWidget ) : MouseButtonEvent(button)
	{
		m_type = WG_EVENT_MOUSEBUTTON_DOUBLE_CLICK;
		m_bIsForWidget	= true;
		m_pWidget 		= pWidget;
	}

	//____ KeyPress ___________________________________________________________

	KeyPress::KeyPress( int native_keycode ) : KeyEvent(native_keycode)
	{
		m_type 			= WG_EVENT_KEY_PRESS;
	}

	KeyPress::KeyPress( int native_keycode, WgWidget * pWidget ) : KeyEvent(native_keycode)
	{
		m_type 			= WG_EVENT_KEY_PRESS;
		m_bIsForWidget	= true;
		m_pWidget		= pWidget;
	}

	//____ KeyRepeat ___________________________________________________________

	KeyRepeat::KeyRepeat( int native_keycode ) : KeyEvent(native_keycode)
	{
		m_type 			= WG_EVENT_KEY_REPEAT;
	}

	KeyRepeat::KeyRepeat( int native_keycode, WgWidget * pWidget ) : KeyEvent(native_keycode)
	{
		m_type 			= WG_EVENT_KEY_REPEAT;
		m_bIsForWidget	= true;
		m_pWidget 		= pWidget;
	}

	//____ KeyRelease ________________________________________________________

	KeyRelease::KeyRelease( int native_keycode ) : KeyEvent(native_keycode)
	{
		m_type 		= WG_EVENT_KEY_RELEASE;
	}

	KeyRelease::KeyRelease( int native_keycode, WgWidget * pWidget ) : KeyEvent(native_keycode)
	{
		m_type 			= WG_EVENT_KEY_RELEASE;
		m_bIsForWidget	= true;
		m_pWidget		= pWidget;
	}

	//____ Character __________________________________________________________

	Character::Character( unsigned short character )
	{
		m_type = WG_EVENT_CHARACTER;
		m_char = character;
	}

	Character::Character( unsigned short character, WgWidget * pWidget )
	{
		m_type			= WG_EVENT_CHARACTER;
		m_char			= character;
		m_bIsForWidget	= true;
		m_pWidget		= pWidget;
	}

	void Character::_cloneContentFrom( const Event * _pOrg )
	{
		const Character * pOrg = static_cast<const Character*>(_pOrg);
		
		m_char			= pOrg->m_char;
		Event::_cloneContentFrom( pOrg );
	}

	unsigned short Character::Char() const
	{
		return m_char;
	}

	//____ MouseWheelRoll __________________________________________________________

	MouseWheelRoll::MouseWheelRoll( int wheel, int distance )
	{
		m_type = WG_EVENT_MOUSEWHEEL_ROLL;
		m_wheel = wheel;
		m_distance = distance;
	}

	MouseWheelRoll::MouseWheelRoll( int wheel, int distance, WgWidget * pWidget )
	{
		m_type			= WG_EVENT_MOUSEWHEEL_ROLL;
		m_wheel			= wheel;
		m_distance		= distance;
		m_bIsForWidget	= true;
		m_pWidget		= pWidget;
	}

	void MouseWheelRoll::_cloneContentFrom( const Event * _pOrg )
	{
		const MouseWheelRoll * pOrg = static_cast<const MouseWheelRoll*>(_pOrg);

		m_wheel			= pOrg->m_wheel;
		m_distance		= pOrg->m_distance;
		Event::_cloneContentFrom( pOrg );
	}


	int MouseWheelRoll::Wheel() const
	{
		return m_wheel;
	}

	int MouseWheelRoll::Distance() const
	{
		return m_distance;
	}

	//____ Tick _______________________________________________________________

	Tick::Tick( int ms )
	{
		m_type = WG_EVENT_TICK;
		m_millisec = ms;
	}

	Tick::Tick( int ms, WgWidget * pWidget )
	{
		m_type 			= WG_EVENT_TICK;
		m_millisec 		= ms;
		m_bIsForWidget	= true;
		m_pWidget 		= pWidget;
	}

	void Tick::_cloneContentFrom( const Event * _pOrg )
	{
		const Tick * pOrg = static_cast<const Tick*>(_pOrg);
				
		m_millisec = pOrg->m_millisec;
		Event::_cloneContentFrom( pOrg );
	}
	
	int Tick::Millisec() const
	{
		return m_millisec;
	}

	//____ PointerChange _______________________________________________________________

	PointerChange::PointerChange( WgPointerStyle style )
	{
		m_type = WG_EVENT_POINTER_CHANGE;
		m_style = style;
	}

	void PointerChange::_cloneContentFrom( const Event * _pOrg )
	{
		const PointerChange * pOrg = static_cast<const PointerChange*>(_pOrg);

		m_style = pOrg->m_style;
		Event::_cloneContentFrom( pOrg );
	}
	
	WgPointerStyle PointerChange::Style() const
	{
		return m_style;
	}


	//____ ButtonPress _________________________________________________________

	ButtonPress::ButtonPress( WgButton * pWidget )
	{
		m_type 			= WG_EVENT_BUTTON_PRESS;
		m_pWidget 		= pWidget;
	}

	WgButton * ButtonPress::Button() const
	{
		WgWidget * pWidget = m_pWidget.GetRealPtr();
		if( pWidget )
			return static_cast<WgButton*>(pWidget);
		else
			return 0;
	}


	//____ Checkbox event methods ______________________________________________

	WgCheckBox * CheckboxEvent::Checkbox() const
	{
		WgWidget * pWidget = m_pWidget.GetRealPtr();
		if( pWidget )
			return static_cast<WgCheckBox*>(pWidget);
		else
			return 0;
	}

	CheckboxCheck::CheckboxCheck( WgCheckBox * pWidget )
	{
		m_type = WG_EVENT_CHECKBOX_CHECK;
		m_pWidget = pWidget;
	}

	CheckboxUncheck::CheckboxUncheck( WgCheckBox * pWidget )
	{
		m_type = WG_EVENT_CHECKBOX_UNCHECK;
		m_pWidget = pWidget;
	}

	CheckboxToggle::CheckboxToggle( WgCheckBox * pWidget, bool bChecked )
	{
		m_type = WG_EVENT_CHECKBOX_TOGGLE;
		m_pWidget = pWidget;
		m_bChecked = bChecked;
	}

	bool CheckboxToggle::IsChecked() const
	{
		return m_bChecked;
	}

	//____ Radiobutton event methods ___________________________________________

	WgRadioButton * RadiobuttonEvent::Radiobutton() const
	{
		WgWidget * pWidget = m_pWidget.GetRealPtr();
		if( pWidget )
			return static_cast<WgRadioButton*>(pWidget);
		else
			return 0;
	}

	RadiobuttonSelect::RadiobuttonSelect( WgRadioButton * pWidget )
	{
		m_type = WG_EVENT_RADIOBUTTON_SELECT;
		m_pWidget = pWidget;
	}

	RadiobuttonUnselect::RadiobuttonUnselect( WgRadioButton * pWidget )
	{
		m_type = WG_EVENT_RADIOBUTTON_UNSELECT;
		m_pWidget = pWidget;
	}

	RadiobuttonToggle::RadiobuttonToggle( WgRadioButton * pWidget, bool bSelected )
	{
		m_type = WG_EVENT_RADIOBUTTON_TOGGLE;
		m_pWidget = pWidget;
		m_bSelected = bSelected;
	}

	bool RadiobuttonToggle::IsSelected() const
	{
		return m_bSelected;
	}

	//____ Animation event methods _____________________________________________

	AnimationUpdate::AnimationUpdate( WgAnimPlayer * pWidget, int frame, float fraction )
	{
		m_type = WG_EVENT_ANIMATION_UPDATE;
		m_pWidget = pWidget;
		m_frame = frame;
		m_fraction = fraction;
	}

	WgAnimPlayer * AnimationUpdate::Animation() const
	{
		WgWidget * pWidget = m_pWidget.GetRealPtr();
		if( pWidget )
			return static_cast<WgAnimPlayer*>(pWidget);
		else
			return 0;
	}

	int AnimationUpdate::Frame() const
	{
		return m_frame;
	}

	float AnimationUpdate::Fraction() const
	{
		return m_fraction;
	}

	//____ Tablist event methods _______________________________________________

	WgTablist * TablistEvent::Tablist() const
	{
		WgWidget * pWidget = m_pWidget.GetRealPtr();
		if( pWidget )
			return static_cast<WgTablist*>(pWidget);
		else
			return 0;
	}

	TabSelect::TabSelect( WgTablist * pWidget, int tabId )
	{
		m_type 		= WG_EVENT_TAB_SELECT;
		m_pWidget 	= pWidget;
		m_tabId 	= tabId;
	}

	int TabSelect::TabId() const
	{
		return m_tabId;
	}

	TabPress::TabPress( WgTablist * pWidget, int tabId, int mouseButton )
	{
		m_type 		= WG_EVENT_TAB_PRESS;
		m_pWidget 	= pWidget;
		m_tabId 	= tabId;
		m_button	= mouseButton;
	}

	int TabPress::TabId() const
	{
		return m_tabId;
	}

	int TabPress::MouseButton() const
	{
		return m_button;
	}

	//____ Editvalue event methods ___________________________________________

	WgValueEditor * EditvalueEvent::Editvalue() const
	{
		WgWidget * pWidget = m_pWidget.GetRealPtr();
		if( pWidget )
			return static_cast<WgValueEditor*>(pWidget);
		else
			return 0;
	}

	int64_t EditvalueEvent::Value() const
	{
		return m_value;
	}

	double EditvalueEvent::Fraction() const
	{
		return m_fraction;
	}

	EditvalueModify::EditvalueModify( WgValueEditor * pWidget, int64_t value, double fraction )
	{
		m_type = WG_EVENT_EDITVALUE_MODIFY;
		m_pWidget = pWidget;
		m_value = value;
		m_fraction = fraction;
	}

	EditvalueSet::EditvalueSet( WgValueEditor * pWidget, int64_t value, double fraction )
	{
		m_type = WG_EVENT_EDITVALUE_SET;
		m_pWidget = pWidget;
		m_value = value;
		m_fraction = fraction;
	}

	//____ Scrollbar event methods _______________________________________________

	ScrollbarEvent::ScrollbarEvent( WgScrollbar * pWidget, float pos, float length )
	{
		m_pWidget 	= pWidget;
		m_pos 		= pos;
		m_length 	= length;
	}

	WgScrollbar* ScrollbarEvent::Scrollbar() const
	{
		WgWidget * pWidget = m_pWidget.GetRealPtr();
		if( pWidget )
			return static_cast<WgScrollbar*>(pWidget);
		else
			return 0;
	}

	float ScrollbarEvent::Pos() const
	{
		return m_pos;
	}

	float ScrollbarEvent::Length() const
	{
		return m_length;
	}

	float ScrollbarEvent::Value() const
	{
		float max = 1.f - m_length;

		if( max == 0.f )
			return 0.5f;		// is this logically correct?
		else
			return m_pos / max;
	}

	ScrollbarMove::ScrollbarMove( WgScrollbar* pWidget, float pos, float length ) : ScrollbarEvent( pWidget, pos, length )
	{
		m_type = WG_EVENT_SCROLLBAR_MOVE;
	}

	ScrollbarStepFwd::ScrollbarStepFwd( WgScrollbar* pWidget, float pos, float length ) : ScrollbarEvent( pWidget, pos, length )
	{
		m_type = WG_EVENT_SCROLLBAR_STEP_FWD;
	}

	ScrollbarStepBwd::ScrollbarStepBwd( WgScrollbar* pWidget, float pos, float length ) : ScrollbarEvent( pWidget, pos, length )
	{
		m_type = WG_EVENT_SCROLLBAR_STEP_BWD;
	}

	ScrollbarJumpFwd::ScrollbarJumpFwd( WgScrollbar* pWidget, float pos, float length ) : ScrollbarEvent( pWidget, pos, length )
	{
		m_type = WG_EVENT_SCROLLBAR_JUMP_FWD;
	}

	ScrollbarJumpBwd::ScrollbarJumpBwd( WgScrollbar* pWidget, float pos, float length ) : ScrollbarEvent( pWidget, pos, length )
	{
		m_type = WG_EVENT_SCROLLBAR_JUMP_BWD;
	}

	ScrollbarWheelRolled::ScrollbarWheelRolled( WgScrollbar* pWidget, int distance, float pos, float length )  : ScrollbarEvent( pWidget, pos, length )
	{
		m_type = WG_EVENT_SCROLLBAR_WHEEL_ROLL;
		m_distance = distance;
	}
	
	int ScrollbarWheelRolled::Distance() const
	{
		return m_distance;
	}

	//____ Text event methods __________________________________________________

	WgInterfaceEditText * TextEvent::Interface() const
	{
		WgWidget * pWidget = m_pWidget.GetRealPtr();
		if( pWidget )
		{
			if( pWidget->Type() == WgTextDisplay::GetClass() )
				return static_cast<WgTextDisplay*>(pWidget);
			else if( pWidget->Type() == WgLineEditor::GetClass() )
				return static_cast<WgLineEditor*>(pWidget);
		}
		return 0;
	}

	const WgText * TextEvent::Text() const
	{
		WgWidget * pWidget = m_pWidget.GetRealPtr();
		if( pWidget )
			return m_pText;
		else
			return 0;
	}

	TextModify::TextModify( WgWidget * pWidget, WgText * pText )
	{
		m_type 		= WG_EVENT_TEXT_MODIFY;
		m_pWidget 	= pWidget;
		m_pText 	= pText;
	}

	TextSet::TextSet( WgWidget * pWidget, WgText * pText )
	{
		m_type 		= WG_EVENT_TEXT_SET;
		m_pWidget 	= pWidget;
		m_pText 	= pText;
	}

	//____ Menu event methods __________________________________________________

	WgMenu *	MenuitemEvent::Menu() const
	{
		WgWidget * pWidget = m_pWidget.GetRealPtr();
		if( pWidget && pWidget->Type() == WgMenu::GetClass() )
			return static_cast<WgMenu*>(pWidget);

		return 0;
	}

	int MenuitemEvent::ItemId() const
	{
		return m_itemId;
	}

	MenuitemSelect::MenuitemSelect( WgMenu * pMenu, int menuItemId )
	{
		m_type		= WG_EVENT_MENUITEM_SELECT;
		m_pWidget	= pMenu;
		m_itemId	= menuItemId;
	}

	MenuitemCheck::MenuitemCheck( WgMenu * pMenu, int menuItemId )
	{
		m_type		= WG_EVENT_MENUITEM_CHECK;
		m_pWidget	= pMenu;
		m_itemId	= menuItemId;
	}

	MenuitemUncheck::MenuitemUncheck( WgMenu * pMenu, int menuItemId )
	{
		m_type		= WG_EVENT_MENUITEM_UNCHECK;
		m_pWidget	= pMenu;
		m_itemId	= menuItemId;
	}

	//____ MenuLayer event methods ____________________________________________

	MenuClosed::MenuClosed( WgWidget * pMenu, const WgWidgetWeakPtr& pCaller )
	{
		m_type		= WG_EVENT_MENU_CLOSED;
		m_pWidget	= pCaller;
		m_bIsForWidget	= true;
		m_pMenu		= pMenu;
	}

	WgWidget * MenuClosed::Menu() const
	{
		return m_pMenu.GetRealPtr();
	}

	WgWidget * MenuClosed::Caller() const
	{
		return m_pWidget.GetRealPtr();
	}

	//____ Modal event methods ___________________________________________________

	ModalMoveOutside::ModalMoveOutside( WgWidget * pWidget )
	{
		m_type = WG_EVENT_MODAL_MOVE_OUTSIDE;
		m_bIsForWidget	= true;
		m_pWidget		= pWidget;
	}

	ModalBlockedPress::ModalBlockedPress( int button, WgWidget * pWidget ) : MouseButtonEvent(button)
	{
		m_type			= WG_EVENT_MODAL_BLOCKED_PRESS;
		m_bIsForWidget	= true;
		m_pWidget 		= pWidget;
	}

	ModalBlockedRelease::ModalBlockedRelease( int button, WgWidget * pWidget ) : MouseButtonEvent(button)
	{
		m_type			= WG_EVENT_MODAL_BLOCKED_RELEASE;
		m_bIsForWidget	= true;
		m_pWidget 		= pWidget;
	}

	//____ Table event methods _________________________________________________

	WgTablePanel * TableCellEvent::Table() const
	{
		WgWidget * pWidget = m_pWidget.GetRealPtr();
		if( pWidget && pWidget->Type() == WgMenu::GetClass() )
			return static_cast<WgTablePanel*>(pWidget);

		return 0;		
	}
	
	int TableCellEvent::Row() const
	{
		return m_row;
	}
	
	int TableCellEvent::Column() const
	{
		return m_column;
	}
	
	WgWidget * TableCellEvent::CellContent() const
	{
		return m_pCellContent.GetRealPtr();
	}
		
	TableCellMarked::TableCellMarked( WgTablePanel * pTable, int row, int column, WgWidget * pCellContent )
	{
		m_type 			= WG_EVENT_TABLE_CELL_MARK;
		m_pWidget 		= pTable;
		m_row 			= row;
		m_column 		= column;
		m_pCellContent 	= pCellContent;
	}

	TableCellUnmarked::TableCellUnmarked( WgTablePanel * pTable, int row, int column, WgWidget * pCellContent )
	{
		m_type 			= WG_EVENT_TABLE_CELL_UNMARK;
		m_pWidget 		= pTable;
		m_row 			= row;
		m_column 		= column;
		m_pCellContent 	= pCellContent;
	}

	//____ Link event methods __________________________________________________

	std::string LinkEvent::Link() const
	{
		return m_link;
	}

	LinkMark::LinkMark( WgWidget * pWidget, std::string link )
	{
		m_type			= WG_EVENT_LINK_MARK;
		m_pWidget		= pWidget;
		m_link			= link;
	}

	LinkUnmark::LinkUnmark( WgWidget * pWidget, std::string link )
	{
		m_type			= WG_EVENT_LINK_UNMARK;
		m_pWidget		= pWidget;
		m_link			= link;
	}

	int LinkButtonEvent::Button() const
	{
		return m_button;
	}

	LinkPress::LinkPress( WgWidget * pWidget, std::string link, int button )
	{
		m_type			= WG_EVENT_LINK_PRESS;
		m_pWidget		= pWidget;
		m_link			= link;
		m_button		= button;
	}

	LinkRepeat::LinkRepeat( WgWidget * pWidget, std::string link, int button )
	{
		m_type			= WG_EVENT_LINK_REPEAT;
		m_pWidget		= pWidget;
		m_link			= link;
		m_button		= button;
	}

	LinkRelease::LinkRelease( WgWidget * pWidget, std::string link, int button )
	{
		m_type			= WG_EVENT_LINK_RELEASE;
		m_pWidget		= pWidget;
		m_link			= link;
		m_button		= button;
	}

	LinkClick::LinkClick( WgWidget * pWidget, std::string link, int button )
	{
		m_type			= WG_EVENT_LINK_CLICK;
		m_pWidget		= pWidget;
		m_link			= link;
		m_button		= button;
	}

	LinkDoubleClick::LinkDoubleClick( WgWidget * pWidget, std::string link, int button )
	{
		m_type			= WG_EVENT_LINK_DOUBLE_CLICK;
		m_pWidget		= pWidget;
		m_link			= link;
		m_button		= button;
	}

	//____ Knob event methods ____________________________________________________

	KnobTurn::KnobTurn( WgWidget * pWidget, int pos, float fraction )
	{
		m_type		= WG_EVENT_KNOB_TURN;
		m_pWidget	= pWidget;
		m_pos		= pos;
		m_fraction	= fraction;
	}

	float KnobTurn::Fraction() const
	{
		return m_fraction;
	}

	int KnobTurn::Pos() const
	{
		return m_pos;
	}



	//____ MousePosition _______________________________________________________

	MousePosition::MousePosition()
	{
		m_type = WG_EVENT_MOUSE_POSITION;
	}

	//____ MouseButtonDrag _________________________________________________________

	MouseButtonDrag::MouseButtonDrag( int button, const WgCoord& startPos, const WgCoord& prevPos, const WgCoord& currPos ) : MouseButtonEvent(button)
	{

		m_type = WG_EVENT_MOUSEBUTTON_DRAG;

		m_startPos = startPos;
		m_prevPos = prevPos;
		m_currPos = currPos;
	}

	MouseButtonDrag::MouseButtonDrag( int button, WgWidget * pWidget, const WgCoord& startPos, const WgCoord& prevPos, const WgCoord& currPos ) : MouseButtonEvent(button)
	{
		m_type = WG_EVENT_MOUSEBUTTON_DRAG;
		m_bIsForWidget		= true;
		m_pWidget 			= pWidget;

		m_startPos = startPos;
		m_prevPos = prevPos;
		m_currPos = currPos;
	}

	void MouseButtonDrag::_cloneContentFrom( const Event * _pOrg )
	{
		const MouseButtonDrag * pOrg = static_cast<const MouseButtonDrag*>(_pOrg);
		
		m_startPos = pOrg->m_startPos;
		m_prevPos = pOrg->m_prevPos;
		m_currPos = pOrg->m_currPos;
		MouseButtonEvent::_cloneContentFrom( pOrg );
	}


	WgCoord MouseButtonDrag::DraggedTotal() const
	{
		return m_currPos - m_startPos;
	}

	WgCoord MouseButtonDrag::DraggedNow() const
	{
		return m_currPos - m_prevPos;
	}

	WgCoord MouseButtonDrag::CurrPos() const
	{
		return m_currPos;
	}

	WgCoord MouseButtonDrag::StartPos() const
	{
		return m_startPos;
	}

	WgCoord MouseButtonDrag::PrevPos() const
	{
		return m_prevPos;
	}

};
