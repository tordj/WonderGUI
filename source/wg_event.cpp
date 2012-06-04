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
#include <wg_gizmo.h>
#include <wg_gizmo_button.h>
#include <wg_gizmo_checkbox.h>
#include <wg_gizmo_radiobutton.h>
#include <wg_gizmo_animation.h>
#include <wg_gizmo_tablist.h>
#include <wg_gizmo_value.h>
#include <wg_gizmo_editvalue.h>
#include <wg_gizmo_slider.h>
#include <wg_gizmo_text.h>
#include <wg_gizmo_editline.h>
#include <wg_gizmo_menu.h>
#include <wg_gizmo_table.h>

namespace WgEvent
{
	//____ Event ______________________________________________________________

	WgGizmo * Event::Gizmo() const
	{
		return m_pGizmo.GetRealPtr();
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
			m_type == WG_EVENT_MOUSEBUTTON_DOUBLECLICK ||
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
		m_bIsForGizmo		= pOrg->m_bIsForGizmo;
		m_pGizmo			= pOrg->m_pGizmo;
		m_pointerLocalPos	= pOrg->m_pointerLocalPos;
		m_pointerScreenPos	= pOrg->m_pointerScreenPos;
	}

	//____ MouseButtonEvent ____________________________________________________

	void MouseButtonEvent::_cloneContentFrom( const MouseButtonEvent * pOrg )
	{
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

	void KeyEvent::_cloneContentFrom( const KeyEvent * pOrg )
	{
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

	MouseEnter::MouseEnter( WgGizmo * pGizmo )
	{
		m_type = WG_EVENT_MOUSE_ENTER;
		m_bIsForGizmo	= true;
		m_pGizmo 		= pGizmo;
	}

	//____ MouseLeave ________________________________________________________

	MouseLeave::MouseLeave()
	{
		m_type = WG_EVENT_MOUSE_LEAVE;
	}

	MouseLeave::MouseLeave( WgGizmo * pGizmo )
	{
		m_type = WG_EVENT_MOUSE_LEAVE;
		m_bIsForGizmo	= true;
		m_pGizmo 		= pGizmo;
	}

	//____ MouseMove __________________________________________________________

	MouseMove::MouseMove( const WgCoord& pos )
	{
		m_type = WG_EVENT_MOUSE_MOVE;

		m_pointerLocalPos = pos;
		m_pointerScreenPos = pos;
	}

	MouseMove::MouseMove( WgGizmo * pGizmo )
	{
		m_type = WG_EVENT_MOUSE_MOVE;
		m_bIsForGizmo	= true;
		m_pGizmo 		= pGizmo;
	}

	//____ MouseButtonPress ______________________________________________________

	MouseButtonPress::MouseButtonPress( int button ) : MouseButtonEvent(button)
	{
		m_type = WG_EVENT_MOUSEBUTTON_PRESS;
	}

	MouseButtonPress::MouseButtonPress( int button, WgGizmo * pGizmo ) : MouseButtonEvent(button)
	{
		m_type			= WG_EVENT_MOUSEBUTTON_PRESS;
		m_bIsForGizmo	= true;
		m_pGizmo 		= pGizmo;
	}

	//____ MouseButtonRepeat ______________________________________________________

	MouseButtonRepeat::MouseButtonRepeat( int button ) : MouseButtonEvent(button)
	{
		m_type = WG_EVENT_MOUSEBUTTON_REPEAT;
	}

	MouseButtonRepeat::MouseButtonRepeat( int button, WgGizmo * pGizmo ) : MouseButtonEvent(button)
	{
		m_type			= WG_EVENT_MOUSEBUTTON_REPEAT;
		m_bIsForGizmo	= true;
		m_pGizmo 		= pGizmo;
	}

	//____ MouseButtonRelease _____________________________________________________

	MouseButtonRelease::MouseButtonRelease( int button ) : MouseButtonEvent(button)
	{
		m_type = WG_EVENT_MOUSEBUTTON_RELEASE;

		m_bPressInside = true;			// Always assumed to have had the press inside our window.
		m_bReleaseInside = true;			// Always assumed to be inside our window now.
	}

	MouseButtonRelease::MouseButtonRelease( int button, WgGizmo * pGizmo, bool bPressInside, bool bReleaseInside ) : MouseButtonEvent(button)
	{
		m_type			= WG_EVENT_MOUSEBUTTON_RELEASE;
		m_bIsForGizmo	= true;
		m_pGizmo 		= pGizmo;

		m_bPressInside = bPressInside;
		m_bReleaseInside = bReleaseInside;
	}

	void MouseButtonRelease::_cloneContentFrom( const MouseButtonRelease * pOrg )
	{
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

	MouseButtonClick::MouseButtonClick( int button, WgGizmo * pGizmo ) : MouseButtonEvent(button)
	{
		m_type = WG_EVENT_MOUSEBUTTON_CLICK;
		m_bIsForGizmo	= true;
		m_pGizmo 		= pGizmo;
	}


	//____ MouseButtonDoubleClick _____________________________________________________

	MouseButtonDoubleClick::MouseButtonDoubleClick( int button ) : MouseButtonEvent(button)
	{
		m_type = WG_EVENT_MOUSEBUTTON_DOUBLECLICK;
	}

	MouseButtonDoubleClick::MouseButtonDoubleClick( int button, WgGizmo * pGizmo ) : MouseButtonEvent(button)
	{
		m_type = WG_EVENT_MOUSEBUTTON_DOUBLECLICK;
		m_bIsForGizmo	= true;
		m_pGizmo 		= pGizmo;
	}

	//____ KeyPress ___________________________________________________________

	KeyPress::KeyPress( int native_keycode ) : KeyEvent(native_keycode)
	{
		m_type 			= WG_EVENT_KEY_PRESS;
	}

	KeyPress::KeyPress( int native_keycode, WgGizmo * pGizmo ) : KeyEvent(native_keycode)
	{
		m_type 			= WG_EVENT_KEY_PRESS;
		m_bIsForGizmo	= true;
		m_pGizmo		= pGizmo;
	}

	//____ KeyRepeat ___________________________________________________________

	KeyRepeat::KeyRepeat( int native_keycode ) : KeyEvent(native_keycode)
	{
		m_type 			= WG_EVENT_KEY_REPEAT;
	}

	KeyRepeat::KeyRepeat( int native_keycode, WgGizmo * pGizmo ) : KeyEvent(native_keycode)
	{
		m_type 			= WG_EVENT_KEY_REPEAT;
		m_bIsForGizmo	= true;
		m_pGizmo 		= pGizmo;
	}

	//____ KeyRelease ________________________________________________________

	KeyRelease::KeyRelease( int native_keycode ) : KeyEvent(native_keycode)
	{
		m_type 		= WG_EVENT_KEY_RELEASE;
	}

	KeyRelease::KeyRelease( int native_keycode, WgGizmo * pGizmo ) : KeyEvent(native_keycode)
	{
		m_type 			= WG_EVENT_KEY_RELEASE;
		m_bIsForGizmo	= true;
		m_pGizmo		= pGizmo;
	}

	//____ Character __________________________________________________________

	Character::Character( unsigned short character )
	{
		m_type = WG_EVENT_CHARACTER;
		m_char = character;
	}

	Character::Character( unsigned short character, WgGizmo * pGizmo )
	{
		m_type			= WG_EVENT_CHARACTER;
		m_char			= character;
		m_bIsForGizmo	= true;
		m_pGizmo		= pGizmo;
	}

	void Character::_cloneContentFrom( const Character * pOrg )
	{
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

	MouseWheelRoll::MouseWheelRoll( int wheel, int distance, WgGizmo * pGizmo )
	{
		m_type			= WG_EVENT_MOUSEWHEEL_ROLL;
		m_wheel			= wheel;
		m_distance		= distance;
		m_bIsForGizmo	= true;
		m_pGizmo		= pGizmo;
	}

	void MouseWheelRoll::_cloneContentFrom( const MouseWheelRoll * pOrg )
	{
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

	Tick::Tick( int ms, WgGizmo * pGizmo )
	{
		m_type 			= WG_EVENT_TICK;
		m_millisec 		= ms;
		m_bIsForGizmo	= true;
		m_pGizmo 		= pGizmo;
	}

	void Tick::_cloneContentFrom( const Tick * pOrg )
	{
		m_millisec = pOrg->m_millisec;
		Event::_cloneContentFrom( pOrg );
	}
	
	int Tick::Millisec() const
	{
		return m_millisec;
	}

	//____ ButtonPress _________________________________________________________

	ButtonPress::ButtonPress( WgGizmoButton * pGizmo )
	{
		m_type 			= WG_EVENT_BUTTON_PRESS;
		m_pGizmo 		= pGizmo;
	}

	WgGizmoButton * ButtonPress::Button() const
	{
		WgGizmo * pGizmo = m_pGizmo.GetRealPtr();
		if( pGizmo )
			return static_cast<WgGizmoButton*>(pGizmo);
		else
			return 0;
	}


	//____ Checkbox event methods ______________________________________________

	WgGizmoCheckbox * CheckboxEvent::Checkbox() const
	{
		WgGizmo * pGizmo = m_pGizmo.GetRealPtr();
		if( pGizmo )
			return static_cast<WgGizmoCheckbox*>(pGizmo);
		else
			return 0;
	}

	CheckboxCheck::CheckboxCheck( WgGizmoCheckbox * pGizmo )
	{
		m_type = WG_EVENT_CHECKBOX_CHECK;
		m_pGizmo = pGizmo;
	}

	CheckboxUncheck::CheckboxUncheck( WgGizmoCheckbox * pGizmo )
	{
		m_type = WG_EVENT_CHECKBOX_UNCHECK;
		m_pGizmo = pGizmo;
	}

	CheckboxToggle::CheckboxToggle( WgGizmoCheckbox * pGizmo, bool bChecked )
	{
		m_type = WG_EVENT_CHECKBOX_TOGGLE;
		m_pGizmo = pGizmo;
		m_bChecked = bChecked;
	}

	bool CheckboxToggle::IsChecked() const
	{
		return m_bChecked;
	}

	//____ Radiobutton event methods ___________________________________________

	WgGizmoRadiobutton * RadiobuttonEvent::Radiobutton() const
	{
		WgGizmo * pGizmo = m_pGizmo.GetRealPtr();
		if( pGizmo )
			return static_cast<WgGizmoRadiobutton*>(pGizmo);
		else
			return 0;
	}

	RadiobuttonSelect::RadiobuttonSelect( WgGizmoRadiobutton * pGizmo )
	{
		m_type = WG_EVENT_RADIOBUTTON_SELECT;
		m_pGizmo = pGizmo;
	}

	RadiobuttonUnselect::RadiobuttonUnselect( WgGizmoRadiobutton * pGizmo )
	{
		m_type = WG_EVENT_RADIOBUTTON_UNSELECT;
		m_pGizmo = pGizmo;
	}

	RadiobuttonToggle::RadiobuttonToggle( WgGizmoRadiobutton * pGizmo, bool bSelected )
	{
		m_type = WG_EVENT_RADIOBUTTON_TOGGLE;
		m_pGizmo = pGizmo;
		m_bSelected = bSelected;
	}

	bool RadiobuttonToggle::IsSelected() const
	{
		return m_bSelected;
	}

	//____ Animation event methods _____________________________________________

	AnimationUpdate::AnimationUpdate( WgGizmoAnimation * pGizmo, int frame, float fraction )
	{
		m_type = WG_EVENT_ANIMATION_UPDATE;
		m_pGizmo = pGizmo;
		m_frame = frame;
		m_fraction = fraction;
	}

	WgGizmoAnimation * AnimationUpdate::Animation() const
	{
		WgGizmo * pGizmo = m_pGizmo.GetRealPtr();
		if( pGizmo )
			return static_cast<WgGizmoAnimation*>(pGizmo);
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

	WgGizmoTablist * TablistEvent::Tablist() const
	{
		WgGizmo * pGizmo = m_pGizmo.GetRealPtr();
		if( pGizmo )
			return static_cast<WgGizmoTablist*>(pGizmo);
		else
			return 0;
	}

	TabSelect::TabSelect( WgGizmoTablist * pGizmo, int tabId )
	{
		m_type 		= WG_EVENT_TAB_SELECT;
		m_pGizmo 	= pGizmo;
		m_tabId 	= tabId;
	}

	int TabSelect::TabId() const
	{
		return m_tabId;
	}

	TabPress::TabPress( WgGizmoTablist * pGizmo, int tabId, int mouseButton )
	{
		m_type 		= WG_EVENT_TAB_PRESS;
		m_pGizmo 	= pGizmo;
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

	WgGizmoEditvalue * EditvalueEvent::Editvalue() const
	{
		WgGizmo * pGizmo = m_pGizmo.GetRealPtr();
		if( pGizmo )
			return static_cast<WgGizmoEditvalue*>(pGizmo);
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

	EditvalueModify::EditvalueModify( WgGizmoEditvalue * pGizmo, int64_t value, double fraction )
	{
		m_type = WG_EVENT_EDITVALUE_MODIFY;
		m_pGizmo = pGizmo;
		m_value = value;
		m_fraction = fraction;
	}

	EditvalueSet::EditvalueSet( WgGizmoEditvalue * pGizmo, int64_t value, double fraction )
	{
		m_type = WG_EVENT_EDITVALUE_SET;
		m_pGizmo = pGizmo;
		m_value = value;
		m_fraction = fraction;
	}

	//____ Slider event methods _______________________________________________

	SliderEvent::SliderEvent( WgGizmoSlider * pGizmo, float pos, float length )
	{
		m_pGizmo 	= pGizmo;
		m_pos 		= pos;
		m_length 	= length;
	}

	WgGizmoSlider* SliderEvent::Slider() const
	{
		WgGizmo * pGizmo = m_pGizmo.GetRealPtr();
		if( pGizmo )
			return static_cast<WgGizmoSlider*>(pGizmo);
		else
			return 0;
	}

	float SliderEvent::Pos() const
	{
		return m_pos;
	}

	float SliderEvent::Length() const
	{
		return m_length;
	}

	SliderMove::SliderMove( WgGizmoSlider* pGizmo, float pos, float length ) : SliderEvent( pGizmo, pos, length )
	{
		m_type = WG_EVENT_SLIDER_MOVE;
	}

	SliderStepFwd::SliderStepFwd( WgGizmoSlider* pGizmo, float pos, float length ) : SliderEvent( pGizmo, pos, length )
	{
		m_type = WG_EVENT_SLIDER_STEP_FWD;
	}

	SliderStepBwd::SliderStepBwd( WgGizmoSlider* pGizmo, float pos, float length ) : SliderEvent( pGizmo, pos, length )
	{
		m_type = WG_EVENT_SLIDER_STEP_BWD;
	}

	SliderJumpFwd::SliderJumpFwd( WgGizmoSlider* pGizmo, float pos, float length ) : SliderEvent( pGizmo, pos, length )
	{
		m_type = WG_EVENT_SLIDER_JUMP_FWD;
	}

	SliderJumpBwd::SliderJumpBwd( WgGizmoSlider* pGizmo, float pos, float length ) : SliderEvent( pGizmo, pos, length )
	{
		m_type = WG_EVENT_SLIDER_JUMP_BWD;
	}

	SliderWheelRolled::SliderWheelRolled( WgGizmoSlider* pGizmo, int distance, float pos, float length )  : SliderEvent( pGizmo, pos, length )
	{
		m_type = WG_EVENT_SLIDER_WHEEL_ROLL;
		m_distance = distance;
	}
	
	int SliderWheelRolled::Distance() const
	{
		return m_distance;
	}

	//____ Text event methods __________________________________________________

	WgInterfaceEditText * TextEvent::Interface() const
	{
		WgGizmo * pGizmo = m_pGizmo.GetRealPtr();
		if( pGizmo )
		{
			if( pGizmo->Type() == WgGizmoText::GetMyType() )
				return static_cast<WgGizmoText*>(pGizmo);
			else if( pGizmo->Type() == WgGizmoEditline::GetMyType() )
				return static_cast<WgGizmoEditline*>(pGizmo);
		}
		return 0;
	}

	const WgText * TextEvent::Text() const
	{
		WgGizmo * pGizmo = m_pGizmo.GetRealPtr();
		if( pGizmo )
			return m_pText;
		else
			return 0;
	}

	TextModify::TextModify( WgGizmo * pGizmo, WgText * pText )
	{
		m_type 		= WG_EVENT_TEXT_MODIFY;
		m_pGizmo 	= pGizmo;
		m_pText 	= pText;
	}

	TextSet::TextSet( WgGizmo * pGizmo, WgText * pText )
	{
		m_type 		= WG_EVENT_TEXT_SET;
		m_pGizmo 	= pGizmo;
		m_pText 	= pText;
	}

	//____ Menu event methods __________________________________________________

	WgGizmoMenu *	MenuItemEvent::Menu() const
	{
		WgGizmo * pGizmo = m_pGizmo.GetRealPtr();
		if( pGizmo && pGizmo->Type() == WgGizmoMenu::GetMyType() )
			return static_cast<WgGizmoMenu*>(pGizmo);

		return 0;
	}

	int MenuItemEvent::ItemId() const
	{
		return m_itemId;
	}

	MenuItemSelected::MenuItemSelected( WgGizmoMenu * pMenu, int menuItemId )
	{
		m_type		= WG_EVENT_MENUITEM_SELECTED;
		m_pGizmo	= pMenu;
		m_itemId	= menuItemId;
	}

	MenuItemChecked::MenuItemChecked( WgGizmoMenu * pMenu, int menuItemId )
	{
		m_type		= WG_EVENT_MENUITEM_CHECKED;
		m_pGizmo	= pMenu;
		m_itemId	= menuItemId;
	}

	MenuItemUnchecked::MenuItemUnchecked( WgGizmoMenu * pMenu, int menuItemId )
	{
		m_type		= WG_EVENT_MENUITEM_UNCHECKED;
		m_pGizmo	= pMenu;
		m_itemId	= menuItemId;
	}


	//____ Modal event methods ___________________________________________________

	ModalMoveOutside::ModalMoveOutside( WgGizmo * pGizmo )
	{
		m_type = WG_EVENT_MODAL_MOVE_OUTSIDE;
		m_bIsForGizmo	= true;
		m_pGizmo		= pGizmo;
	}

	ModalBlockedPress::ModalBlockedPress( int button, WgGizmo * pGizmo ) : MouseButtonEvent(button)
	{
		m_type			= WG_EVENT_MODAL_BLOCKED_PRESS;
		m_bIsForGizmo	= true;
		m_pGizmo 		= pGizmo;
	}

	ModalBlockedRelease::ModalBlockedRelease( int button, WgGizmo * pGizmo ) : MouseButtonEvent(button)
	{
		m_type			= WG_EVENT_MODAL_BLOCKED_RELEASE;
		m_bIsForGizmo	= true;
		m_pGizmo 		= pGizmo;
	}

	//____ Table event methods _________________________________________________

	WgGizmoTable * TableCellEvent::Table() const
	{
		WgGizmo * pGizmo = m_pGizmo.GetRealPtr();
		if( pGizmo && pGizmo->Type() == WgGizmoMenu::GetMyType() )
			return static_cast<WgGizmoTable*>(pGizmo);

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
	
	WgGizmo * TableCellEvent::CellContent() const
	{
		return m_pCellContent.GetRealPtr();
	}
		
	TableCellMarked::TableCellMarked( WgGizmoTable * pTable, int row, int column, WgGizmo * pCellContent )
	{
		m_type 			= WG_EVENT_TABLE_CELL_MARKED;
		m_pGizmo 		= pTable;
		m_row 			= row;
		m_column 		= column;
		m_pCellContent 	= pCellContent;
	}

	TableCellUnmarked::TableCellUnmarked( WgGizmoTable * pTable, int row, int column, WgGizmo * pCellContent )
	{
		m_type 			= WG_EVENT_TABLE_CELL_UNMARKED;
		m_pGizmo 		= pTable;
		m_row 			= row;
		m_column 		= column;
		m_pCellContent 	= pCellContent;
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

	MouseButtonDrag::MouseButtonDrag( int button, WgGizmo * pGizmo, const WgCoord& startPos, const WgCoord& prevPos, const WgCoord& currPos ) : MouseButtonEvent(button)
	{
		m_type = WG_EVENT_MOUSEBUTTON_DRAG;
		m_bIsForGizmo		= true;
		m_pGizmo 			= pGizmo;

		m_startPos = startPos;
		m_prevPos = prevPos;
		m_currPos = currPos;
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
