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

#ifndef WG_EVENT_DOT_H
#define WG_EVENT_DOT_H

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

#ifndef WG_SMARTPTR_DOT_H
#	include <wg_smartptr.h>
#endif



class WgEventHandler;
class WgGizmo;
class WgButton;
class WgCheckBox;
class WgRadioButton;
class WgAnimPlayer;
class WgTablist;
class WgValueDisplay;
class WgValueEditor;
class WgGizmoSlider;
class WgMenu;
class WgModalPanel;
class WgTablePanel;
class WgInterfaceEditText;
class WgText;

typedef class WgWeakPtr<WgGizmo> WgGizmoWeakPtr;

namespace WgEvent
{
	class Event
	{
		friend class ::WgEventHandler;
		friend class ::WgGizmo;

		public:
			WgEventType		Type() const { return m_type; }
			int64_t			Timestamp() const { return m_timestamp; }
			bool			IsForGizmo() const { return m_bIsForGizmo; }
			WgGizmo *		Gizmo() const;									// Inlining this would demand include of wg_gizmo.h.
			WgGizmoWeakPtr	GizmoWeakPtr() const { return m_pGizmo; }
			WgGizmo *		ForwardedFrom() const;
			WgModifierKeys	ModKeys() const { return m_modKeys; }
			WgCoord			PointerPos() const { return m_pointerLocalPos; }
			WgCoord			PointerScreenPos() const { return m_pointerScreenPos; }
			
			bool			IsMouseEvent() const;
			bool			IsMouseButtonEvent() const;
			bool			IsKeyEvent() const;

		protected:
			Event() : m_type(WG_EVENT_DUMMY), m_modKeys(WG_MODKEY_NONE), m_timestamp(0), m_bIsForGizmo(false) {}
			virtual ~Event() {}

			virtual void 	_cloneContentFrom( const Event * pOrg );			// Only subclassed for the standard event types.
			
			WgEventType		m_type;				// Type of event
			WgModifierKeys	m_modKeys;			// Modifier keys pressed when event posted.
			int64_t			m_timestamp;		// Timestamp of posting this event
			bool			m_bIsForGizmo;		// Set if this event is for a specific Gizmo (m_pGizmo set at creation, even if weak pointer now is null).
			WgGizmoWeakPtr	m_pGizmo;			// Gizmo to receive this event.
			WgGizmoWeakPtr	m_pForwardedFrom;	// Gizmo this event was forwarded from.
			WgCoord			m_pointerLocalPos;	// Gizmo-relative position of pointer. Same as m_pointerScreenPos if Gizmo not set.
			WgCoord			m_pointerScreenPos;	// Screen position of pointer.
	};

	class MouseButtonEvent : public Event
	{
		friend class ::WgEventHandler;
	public:
		int		Button() const { return m_button; }
	protected:
		MouseButtonEvent(int button) : m_button(button) {}
		virtual void 	_cloneContentFrom( const Event * pOrg );

		int		m_button;
	};

	class KeyEvent : public Event
	{
		friend class ::WgEventHandler;
	public:
		int		NativeKeyCode() const { return m_nativeKeyCode; }
		int		TranslatedKeyCode() const { return m_translatedKeyCode; }
		bool	IsCursorKey() const;
		bool	IsMovementKey() const;
	protected:
		KeyEvent( int nativeKeyCode ) : m_nativeKeyCode(nativeKeyCode), m_translatedKeyCode(0) {}
		virtual void 	_cloneContentFrom( const Event * pOrg );

		int		m_nativeKeyCode;
		int		m_translatedKeyCode;
	};

	class FocusGained : public Event
	{
		friend class ::WgEventHandler;
	public:
		FocusGained();
	};

	class FocusLost : public Event
	{
		friend class ::WgEventHandler;
	public:
		FocusLost();
	};

	class MouseEnter : public Event
	{
		friend class ::WgEventHandler;
	public:
		MouseEnter( const WgCoord& pos );
	protected:
		MouseEnter( WgGizmo * pGizmo );
	};

	class MouseLeave : public Event
	{
		friend class ::WgEventHandler;
	public:
		MouseLeave();
	protected:
		MouseLeave( WgGizmo * pGizmo );
	};


	class MouseMove : public Event
	{
		friend class ::WgEventHandler;
	protected:
		MouseMove( WgGizmo * pGizmo );
	public:
		MouseMove( const WgCoord& pos );
	};

	class MouseButtonPress : public MouseButtonEvent
	{
		friend class ::WgEventHandler;
	protected:
		MouseButtonPress( int button, WgGizmo * pGizmo );
	public:
		MouseButtonPress( int button );
	};

	class MouseButtonRelease : public MouseButtonEvent
	{
		friend class ::WgEventHandler;
	public:
		MouseButtonRelease( int button );

		bool			PressInside() const;
		bool			ReleaseInside() const;

	protected:
		MouseButtonRelease( int button, WgGizmo * pGizmo, bool bPressInside, bool bReleaseInside );
		virtual void 	_cloneContentFrom( const Event * pOrg );

		bool			m_bPressInside;
		bool			m_bReleaseInside;
	};

	class KeyPress : public KeyEvent
	{
		friend class ::WgEventHandler;
	public:
		KeyPress( int native_keycode );
	protected:
		KeyPress( int native_keycode, WgGizmo * pGizmo );
	};

	class KeyRelease : public KeyEvent
	{
		friend class ::WgEventHandler;
	public:
		KeyRelease( int native_keycode );
	protected:
		KeyRelease( int native_keycode, WgGizmo * pGizmo );
	};

	class Character : public Event
	{
		friend class ::WgEventHandler;
	public:
		Character( unsigned short character );

		unsigned short	Char() const;
	protected:
		Character( unsigned short character, WgGizmo * pGizmo );
		virtual void 	_cloneContentFrom( const Event * pOrg );
	protected:
		unsigned short	m_char;
	};

	class MouseWheelRoll : public Event
	{
		friend class ::WgEventHandler;
	public:
		MouseWheelRoll( int wheel, int distance );

		int			Wheel() const;
		int			Distance() const;
	protected:
		MouseWheelRoll( int wheel, int distance, WgGizmo * pGizmo );
		virtual void 	_cloneContentFrom( const Event * pOrg );

		int			m_wheel;
		int			m_distance;
	};

	class Tick : public Event
	{
		friend class ::WgEventHandler;
	public:
		Tick( int ms );

		int				Millisec() const;
	protected:
		Tick( int ms, WgGizmo * pGizmo );
		virtual void 	_cloneContentFrom( const Event * pOrg );

		int			m_millisec;
	};

	class PointerChange : public Event
	{
		friend class ::WgEventHandler;
	public:
		WgPointerStyle	Style() const;
		
	protected:
		PointerChange( WgPointerStyle style );
		virtual void 	_cloneContentFrom( const Event * pOrg );
		
		WgPointerStyle	m_style;
	};


	//____ WgButton events _______________________________________________

	class ButtonPress : public Event
	{
	public:
		ButtonPress( WgButton * pGizmo );
		WgButton *	Button() const;
	};

	//____ WgCheckBox events ______________________________________________

	class CheckboxEvent : public Event
	{
	public:
		WgCheckBox * Checkbox() const;
	};

	class CheckboxCheck : public CheckboxEvent
	{
	public:
		CheckboxCheck( WgCheckBox * pGizmo );
	};

	class CheckboxUncheck : public CheckboxEvent
	{
	public:
		CheckboxUncheck( WgCheckBox * pGizmo );
	};

	class CheckboxToggle : public CheckboxEvent
	{
	public:
		CheckboxToggle( WgCheckBox * pGizmo, bool bChecked );
		bool		IsChecked() const;

	private:
		bool	m_bChecked;
	};

	//____ WgRadioButton events ___________________________________________

	class RadiobuttonEvent : public Event
	{
	public:
		WgRadioButton * Radiobutton() const;
	};

	class RadiobuttonSelect : public RadiobuttonEvent
	{
	public:
		RadiobuttonSelect( WgRadioButton * pGizmo );
	};

	class RadiobuttonUnselect : public RadiobuttonEvent
	{
	public:
		RadiobuttonUnselect( WgRadioButton * pGizmo );
	};

	class RadiobuttonToggle : public RadiobuttonEvent
	{
	public:
		RadiobuttonToggle( WgRadioButton * pGizmo, bool bSelected );
		bool	IsSelected() const;
	private:
		bool	m_bSelected;
	};

	//____ WgAnimPlayer events _____________________________________________

	class AnimationUpdate : public Event
	{
	public:
		AnimationUpdate( WgAnimPlayer * pGizmo, int frame, float fraction );
		WgAnimPlayer * Animation() const;
		int		Frame() const;
		float	Fraction() const;
	private:
		int		m_frame;
		float	m_fraction;
	};

	//____ WgTablist events _______________________________________________

	class TablistEvent : public Event
	{
	public:
		WgTablist *	Tablist() const;
	};

	class TabSelect : public TablistEvent
	{
	public:
		TabSelect( WgTablist * pGizmo, int tabId );
		int		TabId() const;
	private:
		int		m_tabId;
	};

	class TabPress : public TablistEvent
	{
	public:
		TabPress( WgTablist * pGizmo, int tabId, int mouseButton );
		int		TabId() const;
		int		MouseButton() const;
	private:
		int		m_tabId;
		int		m_button;
	};

	//____ WgValueEditor events _____________________________________

	class EditvalueEvent : public Event
	{
		friend class EditvalueModify;
		friend class EditvalueSet;
	public:
		WgValueEditor * Editvalue() const;
		int64_t		Value() const;
		double		Fraction() const;
	protected:
		int64_t		m_value;
		double		m_fraction;
	};

	class EditvalueModify : public EditvalueEvent
	{
	public:
		EditvalueModify( WgValueEditor * pGizmo, int64_t value, double fraction );
	};

	class EditvalueSet : public EditvalueEvent
	{
	public:
		EditvalueSet( WgValueEditor * pGizmo, int64_t value, double fraction );
	};

	//____ WgGizmoSlider events ________________________________________________

	class SliderEvent : public Event
	{
	public:
		WgGizmoSlider* Slider() const;
		float			Pos() const;
		float			Length() const;
		float			Value() const;

	protected:
		SliderEvent( WgGizmoSlider * pGizmo, float pos, float length );
		float			m_pos;
		float			m_length;
	};

	class SliderMove : public SliderEvent
	{
	public:
		SliderMove( WgGizmoSlider* pGizmo, float pos, float length );
	};

	class SliderStepFwd : public SliderEvent
	{
	public:
		SliderStepFwd( WgGizmoSlider* pGizmo, float pos, float length );
	};

	class SliderStepBwd : public SliderEvent
	{
	public:
		SliderStepBwd( WgGizmoSlider* pGizmo, float pos, float length );
	};

	class SliderJumpFwd : public SliderEvent
	{
	public:
		SliderJumpFwd( WgGizmoSlider* pGizmo, float pos, float length );
	};

	class SliderJumpBwd : public SliderEvent
	{
	public:
		SliderJumpBwd( WgGizmoSlider* pGizmo, float pos, float length );
	};

	class SliderWheelRolled : public SliderEvent
	{
	public:
		SliderWheelRolled( WgGizmoSlider* pGizmo, int distance, float pos, float length );
		int			Distance() const;
	protected:
		float		m_distance;
	};

	//____ WgGizmoKnob events _________________________________________________

	class KnobTurn : public Event
	{
	public:
		KnobTurn( WgGizmo * pGizmo, int pos, float fraction ); //TODO: Change to WgGizmoKnob* when we have impoted WgGizmoKnob to main code.
//		WgGizmoKnob * Knob() const;		TODO: Add when we have imported WgGizmoKnob to main code.
		float	Fraction() const;
		int		Pos() const;
	private:
		float	m_fraction;
		float	m_pos;
	};

	//____ Text events ________________________________________

	class TextEvent : public Event
	{
	public:
		WgInterfaceEditText * Interface() const;
		const WgText *		  Text() const;
	protected:
		WgText *	m_pText;
	};

	class TextModify : public TextEvent
	{
	public:
		TextModify( WgGizmo * pGizmo, WgText * pText );
	};

	class TextSet : public TextEvent
	{
	public:
		TextSet( WgGizmo * pGizmo, WgText * pText );
	};

	//____ WgMenu events __________________________________________________

	class MenuitemEvent : public Event
	{
	public:
		WgMenu *	Menu() const;
		int				ItemId() const;

	protected:
		int				m_itemId;
	};

	class MenuitemSelect : public MenuitemEvent
	{
	public:
		MenuitemSelect( WgMenu * pMenu, int menuItemId );
	};

	class MenuitemCheck : public MenuitemEvent
	{
	public:
		MenuitemCheck( WgMenu * pMenu, int menuItemId );
	};

	class MenuitemUncheck : public MenuitemEvent
	{
	public:
		MenuitemUncheck( WgMenu * pMenu, int menuItemId );
	};

	//____ WgGizmoModalLayer events _________________________________________________

	class ModalMoveOutside : public Event
	{
		friend class ::WgModalPanel;
	protected:
		ModalMoveOutside( WgGizmo * pGizmo );
	};

	class ModalBlockedPress : public MouseButtonEvent
	{
		friend class ::WgModalPanel;
	protected:
		ModalBlockedPress( int button, WgGizmo * pModalGizmo );
	};

	class ModalBlockedRelease : public MouseButtonEvent
	{
		friend class ::WgModalPanel;
	protected:
		ModalBlockedRelease( int button, WgGizmo * pModalGizmo );
	};

	//____ WgTablePanel events _________________________________________________

	class TableCellEvent : public Event
	{
	public:
		WgTablePanel * 	Table() const;
		int				Row() const;
		int				Column() const;
		WgGizmo *		CellContent() const;
		
	protected:
		int				m_row;
		int				m_column;
		WgGizmoWeakPtr	m_pCellContent;
	};

	class TableCellMarked : public TableCellEvent
	{
		friend class ::WgTablePanel;
	protected:
		TableCellMarked( WgTablePanel * pTable, int row, int column, WgGizmo * pCellContent );
	};

	class TableCellUnmarked : public TableCellEvent
	{
		friend class ::WgTablePanel;
	protected:
		TableCellUnmarked( WgTablePanel * pTable, int row, int column, WgGizmo * pCellContent );
	};

	//____ Link events _________________________________________________________

	class LinkEvent : public Event
	{
	public:
		std::string		Link() const;
	protected:
		std::string		m_link;
	};

	class LinkMark : public LinkEvent
	{
	public:
		LinkMark( WgGizmo * pGizmo, std::string link );
	};

	class LinkUnmark : public LinkEvent
	{
	public:
		LinkUnmark( WgGizmo * pGizmo, std::string link );
	};

	class LinkButtonEvent : public LinkEvent
	{
	public:
		int			Button() const;
	protected:
		int			m_button;
	};

	class LinkPress : public LinkButtonEvent
	{
	public:
		LinkPress( WgGizmo * pGizmo, std::string link, int button );
	};

	class LinkRepeat : public LinkButtonEvent
	{
	public:
		LinkRepeat( WgGizmo * pGizmo, std::string link, int button );
	};

	class LinkRelease : public LinkButtonEvent
	{
	public:
		LinkRelease( WgGizmo * pGizmo, std::string link, int button );
	};

	class LinkClick : public LinkButtonEvent
	{
	public:
		LinkClick( WgGizmo * pGizmo, std::string link, int button );
	};

	class LinkDoubleClick : public LinkButtonEvent
	{
	public:
		LinkDoubleClick( WgGizmo * pGizmo, std::string link, int button );
	};

	//____ Internally posted events ____________________________________________

	class MousePosition : public Event
	{
		friend class ::WgEventHandler;
	protected:
		MousePosition();
	};

	class MouseButtonDrag : public MouseButtonEvent
	{
		friend class ::WgEventHandler;
	protected:
		MouseButtonDrag( int button, const WgCoord& startPos, const WgCoord& prevPos, const WgCoord& currPos );
		MouseButtonDrag( int button, WgGizmo * pGizmo, const WgCoord& orgPos, const WgCoord& prevPos, const WgCoord& currPos );
		virtual void 	_cloneContentFrom( const Event * pOrg );
	public:
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

	class MouseButtonRepeat : public MouseButtonEvent
	{
		friend class ::WgEventHandler;
	protected:
		MouseButtonRepeat( int button, WgGizmo * pGizmo );
	public:
		MouseButtonRepeat( int button );
	};

	class MouseButtonClick : public MouseButtonEvent
	{
		friend class ::WgEventHandler;
	protected:
		MouseButtonClick( int button );
		MouseButtonClick( int button, WgGizmo * pGizmo );
	};

	class MouseButtonDoubleClick : public MouseButtonEvent
	{
		friend class ::WgEventHandler;
	protected:
		MouseButtonDoubleClick( int button );
		MouseButtonDoubleClick( int button, WgGizmo * pGizmo );
	};

	class KeyRepeat : public KeyEvent
	{
		friend class ::WgEventHandler;
	protected:
		KeyRepeat( int native_keycode );
		KeyRepeat( int native_keycode, WgGizmo * pGizmo );
	};

}



#endif //WG_EVENT_DOT_H
