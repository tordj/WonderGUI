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
class WgGizmoButton;
class WgGizmoCheckbox;
class WgGizmoRadiobutton;
class WgGizmoAnimation;
class WgGizmoTablist;
class WgGizmoValue;
class WgGizmoEditvalue;
class WgGizmoDragbar;
class WgInterfaceEditText;
class WgText;

typedef class WgWeakPtr<WgGizmo> WgGizmoWeakPtr;

namespace WgEvent
{
	class Event
	{
		friend class ::WgEventHandler;

		public:
			inline WgEventType		Type() const { return m_type; }
			inline int64_t			Timestamp() const { return m_timestamp; }
					WgGizmo *		Gizmo() const;									// Inlining this would demand include of wg_gizmo.h.
			inline WgGizmoWeakPtr	GizmoWeakPtr() const { return m_pGizmo; }
			inline bool				IsForGizmo() const { return m_bIsForGizmo; }
			inline WgModifierKeys	ModKeys() const { return m_modKeys; }
			inline WgCoord			PointerPos() const { return m_pointerLocalPos; }
			inline WgCoord			PointerScreenPos() const { return m_pointerScreenPos; }

		protected:
			Event() : m_type(WG_EVENT_DUMMY), m_modKeys(WG_MODKEY_NONE), m_timestamp(0), m_bIsForGizmo(false) {}
			virtual ~Event() {}

			WgEventType		m_type;				// Type of event
			WgModifierKeys	m_modKeys;			// Modifier keys pressed when event posted.
			int64_t			m_timestamp;		// Timestamp of posting this event
			bool			m_bIsForGizmo;		// Set if this event is for a specific Gizmo (m_pGizmo set at creation, even if weak pointer now is null).
			WgGizmoWeakPtr	m_pGizmo;			// Gizmo to receive this event.
			WgCoord			m_pointerLocalPos;	// Gizmo-relative position of pointer. Same as m_pointerScreenPos if Gizmo not set.
			WgCoord			m_pointerScreenPos;	// Screen position of pointer.
	};

	class MouseButtonEvent : public Event
	{
		friend class ::WgEventHandler;
	public:
		inline int		Button() const { return m_button; }
	protected:
		MouseButtonEvent(int button) : m_button(button) {}

		int		m_button;
	};

	class KeyEvent : public Event
	{
		friend class ::WgEventHandler;
	public:
		inline int		NativeKeyCode() const { return m_nativeKeyCode; }
		inline int		TranslatedKeyCode() const { return m_translatedKeyCode; }
	protected:
		KeyEvent( int nativeKeyCode ) : m_nativeKeyCode(nativeKeyCode), m_translatedKeyCode(0) {}

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

		int			m_millisec;
	};

	//____ WgGizmoButton events _______________________________________________

	class ButtonPress : public Event
	{
	public:
		ButtonPress( WgGizmoButton * pGizmo );
		WgGizmoButton *	Button() const;
	};

	//____ WgGizmoCheckbox events ______________________________________________

	class CheckboxEvent : public Event
	{
	public:
		WgGizmoCheckbox * Checkbox() const;
	};

	class CheckboxCheck : public CheckboxEvent
	{
	public:
		CheckboxCheck( WgGizmoCheckbox * pGizmo );
	};

	class CheckboxUncheck : public CheckboxEvent
	{
	public:
		CheckboxUncheck( WgGizmoCheckbox * pGizmo );
	};

	class CheckboxToggle : public CheckboxEvent
	{
	public:
		CheckboxToggle( WgGizmoCheckbox * pGizmo, bool bChecked );
		bool		IsChecked() const;

	private:
		bool	m_bChecked;
	};

	//____ WgGizmoRadiobutton events ___________________________________________

	class RadiobuttonEvent : public Event
	{
	public:
		WgGizmoRadiobutton * Radiobutton() const;
	};

	class RadiobuttonSelect : public RadiobuttonEvent
	{
	public:
		RadiobuttonSelect( WgGizmoRadiobutton * pGizmo );
	};

	class RadiobuttonUnselect : public RadiobuttonEvent
	{
	public:
		RadiobuttonUnselect( WgGizmoRadiobutton * pGizmo );
	};

	class RadiobuttonToggle : public RadiobuttonEvent
	{
	public:
		RadiobuttonToggle( WgGizmoRadiobutton * pGizmo, bool bSelected );
		bool	IsSelected() const;
	private:
		bool	m_bSelected;
	};

	//____ WgGizmoAnimation events _____________________________________________

	class AnimationUpdate : public Event
	{
	public:
		AnimationUpdate( WgGizmoAnimation * pGizmo, int frame, float fraction );
		WgGizmoAnimation * Animation() const;
		int		Frame() const;
		float	Fraction() const;
	private:
		int		m_frame;
		float	m_fraction;
	};

	//____ WgGizmoTablist events _______________________________________________
	
	class TablistEvent : public Event
	{
	public:
		WgGizmoTablist *	Tablist() const;
	};
	
	class TabSelect : public TablistEvent
	{
	public:
		TabSelect( WgGizmoTablist * pGizmo, int tabId );
		int		TabId() const;
	private:
		int		m_tabId;
	};

	class TabPress : public TablistEvent
	{
	public:
		TabPress( WgGizmoTablist * pGizmo, int tabId, int mouseButton );
		int		TabId() const;
		int		MouseButton() const;
	private:
		int		m_tabId;
		int		m_button;
	};

	//____ WgGizmoEditvalue events _____________________________________

	class EditvalueEvent : public Event
	{
	public:
		WgGizmoEditvalue * Editvalue() const;
		int64_t		Value() const;
		double		Fraction() const;
	protected:
		int64_t		m_value;
		double		m_fraction;
	};

	class EditvalueModify : public EditvalueEvent
	{
	public:
		EditvalueModify( WgGizmoEditvalue * pGizmo, int64_t value, double fraction );
	};

	class EditvalueSet : public EditvalueEvent
	{
	public:
		EditvalueSet( WgGizmoEditvalue * pGizmo, int64_t value, double fraction );
	};

	//____ WgGizmoDragbar events ________________________________________________

	class DragbarEvent : public Event
	{
	public:
		WgGizmoDragbar* Dragbar() const;
		float			Pos() const;
		float			Length() const;
		
	protected:
		DragbarEvent( WgGizmoDragbar * pGizmo, float pos, float length );
		float			m_pos;
		float			m_length;
	};

	class DragbarMove : public DragbarEvent
	{
	public:
		DragbarMove( WgGizmoDragbar* pGizmo, float pos, float length );
	};

	class DragbarStepUp : public DragbarEvent
	{
	public:
		DragbarStepUp( WgGizmoDragbar* pGizmo, float pos, float length );
	};

	class DragbarStepDown : public DragbarEvent
	{
	public:
		DragbarStepDown( WgGizmoDragbar* pGizmo, float pos, float length );
	};

	class DragbarPageUp : public DragbarEvent
	{
	public:
		DragbarPageUp( WgGizmoDragbar* pGizmo, float pos, float length );
	};

	class DragbarPageDown : public DragbarEvent
	{
	public:
		DragbarPageDown( WgGizmoDragbar* pGizmo, float pos, float length );
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


	class MouseMoveOutsideModal : public Event
	{
		friend class ::WgEventHandler;
	protected:
		MouseMoveOutsideModal( WgGizmo * pGizmo );
	};

	class MouseButtonPressOutsideModal : public MouseButtonEvent
	{
		friend class ::WgEventHandler;
	protected:
		MouseButtonPressOutsideModal( int button, WgGizmo * pModalGizmo );
	};

	class MouseButtonReleaseOutsideModal : public MouseButtonEvent
	{
		friend class ::WgEventHandler;
	protected:
		MouseButtonReleaseOutsideModal( int button, WgGizmo * pModalGizmo );

	};

}



#endif //WG_EVENT_DOT_H
