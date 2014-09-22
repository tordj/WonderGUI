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

#include <wg_button.h>
#include <wg_gfxdevice.h>
#include <wg_util.h>
#include <wg_key.h>
#include <wg_event.h>
#include <wg_eventhandler.h>

const char WgButton::CLASSNAME[] = {"Button"};

//____ Constructor ____________________________________________________________

WgButton::WgButton()
{
	label.SetAlignment( WG_CENTER );
	label.setLineWidth(Size().w);					// We start with no textborders...
	label.SetAutoEllipsis(IsAutoEllipsisDefault());
	label._setHolder(this);

	icon._setHolder(this);

 	m_state				= WG_STATE_NORMAL;

	m_bDownOutside	 = false;
	m_bPressed 		 = false;
	m_bReturnPressed = false;
}

//____ Destructor _____________________________________________________________

WgButton::~WgButton()
{
}

//____ IsInstanceOf() _________________________________________________________

bool WgButton::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgWidget::IsInstanceOf(pClassName);
}

//____ ClassName() ____________________________________________________________

const char * WgButton::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ Cast() _________________________________________________________________

WgButtonPtr WgButton::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgButtonPtr( static_cast<WgButton*>(pObject.GetRealPtr()) );

	return 0;
}

//____ TextAreaWidth() _____________________________________________________

int WgButton::TextAreaWidth()
{
	WgRect	contentRect(0,0,Size());

	if( m_pSkin )
		contentRect = m_pSkin->SizeForContent( contentRect );

	WgRect textRect = icon.GetTextRect( contentRect, icon.GetIconRect( contentRect ) );

	return textRect.w;
}

//____ HeightForWidth() _______________________________________________________

int WgButton::HeightForWidth( int width ) const
{
	int height = 0;

	if( m_pSkin )
		height = m_pSkin->PreferredSize().h;

	if( label.Length() != 0 )
	{
		WgSize padding;

		if( m_pSkin )
			padding = m_pSkin->ContentPadding();

		int heightForText = label.heightForWidth(width-padding.w) + padding.h;
		if( heightForText > height )
			height = heightForText;
	}

	//TODO: Take icon into account.

	return height;
}


//____ PreferredSize() _____________________________________________________________

WgSize WgButton::PreferredSize() const
{
	WgSize preferred;

	if( label.Length() != 0 )
		preferred = label.unwrappedSize();
	
	if( m_pSkin )
		preferred = m_pSkin->SizeForContent(preferred);

	//TODO: Take icon into account.

	return preferred;
}

//____ _onStateChanged() ______________________________________________________

void WgButton::_onStateChanged( WgState oldState )
{
	WgWidget::_onStateChanged(oldState);

	if(icon.Skin() && !icon.Skin()->IsStateIdentical(m_state,oldState))
			_requestRender();

	label.setState(m_state);

	//TODO: Request render if text properties have changed.

}

//____ _onSkinChanged() _______________________________________________________

void WgButton::_onSkinChanged( const WgSkinPtr& pOldSkin, const WgSkinPtr& pNewSkin )
{
	WgWidget::_onSkinChanged(pOldSkin,pNewSkin);
	label.SetColorSkin(pNewSkin);
}


//____ _onNewSize() ____________________________________________________________

void WgButton::_onNewSize( const WgSize& size )
{
	WgRect	contentRect(0,0,Size());

	if( m_pSkin )
		contentRect -= m_pSkin->ContentPadding();

	WgRect textRect = icon.GetTextRect( contentRect, icon.GetIconRect( contentRect ) );

	label.setLineWidth(textRect.w);
}


//____ _onRender() _____________________________________________________________

void WgButton::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip )
{
	WgWidget::_onRender(pDevice,_canvas,_window,_clip);

	WgRect	contentRect = _canvas;

	if( m_pSkin )
		contentRect = m_pSkin->ContentRect(_canvas, m_state);

	// Get icon and text rect from content rect

	WgRect iconRect = icon.GetIconRect( contentRect );
	WgRect textRect = icon.GetTextRect( contentRect, iconRect );

	// Render icon

	if( icon.Skin() )
		icon.Skin()->Render( pDevice, iconRect, m_state, _clip );

	// Print text

 	if( !label.IsEmpty() )
		pDevice->PrintText( WgRect(textRect,_clip), &label, textRect );
}

//____ _onEvent() ______________________________________________________________

void WgButton::_onEvent( const WgEventPtr& _pEvent, WgEventHandler * pHandler )
{
	WgState oldState = m_state;

	switch( _pEvent->Type() )
	{
		case WG_EVENT_KEY_PRESS:
			if( WgKeyPressEvent::Cast(_pEvent)->TranslatedKeyCode() == WG_KEY_RETURN )
			{
				m_bReturnPressed = true;
				pHandler->SwallowEvent(_pEvent);
			}
			break;

		case WG_EVENT_KEY_REPEAT:
			if( WgKeyRepeatEvent::Cast(_pEvent)->TranslatedKeyCode() == WG_KEY_RETURN )
				pHandler->SwallowEvent(_pEvent);
			break;

		case WG_EVENT_KEY_RELEASE:
			if( WgKeyReleaseEvent::Cast(_pEvent)->TranslatedKeyCode() == WG_KEY_RETURN )
			{
				m_bReturnPressed = false;
				pHandler->QueueEvent( new WgSelectEvent(this) );
				pHandler->SwallowEvent(_pEvent);
			}
			break;
	
		case WG_EVENT_MOUSE_ENTER:
			m_state.SetHovered(true);
			break;
		case WG_EVENT_MOUSE_LEAVE:
			m_state.SetHovered(false);
			break;
		case WG_EVENT_MOUSE_PRESS:
			if( WgMousePressEvent::Cast(_pEvent)->Button() == WG_BUTTON_LEFT )
			{
				m_bPressed = true;
				pHandler->SwallowEvent(_pEvent);
			}
			break;
		case WG_EVENT_MOUSE_RELEASE:
			if( WgMouseReleaseEvent::Cast(_pEvent)->Button() == WG_BUTTON_LEFT )
			{
				m_bPressed = false;
				pHandler->SwallowEvent(_pEvent);
			}
			break;
		case WG_EVENT_MOUSE_CLICK:
			if( WgMouseClickEvent::Cast(_pEvent)->Button() == WG_BUTTON_LEFT )
			{
				pHandler->QueueEvent( new WgSelectEvent(this) );
				pHandler->SwallowEvent(_pEvent);
			}
			break;
		case WG_EVENT_MOUSE_DOUBLE_CLICK:
		case WG_EVENT_MOUSE_REPEAT:
		case WG_EVENT_MOUSE_DRAG:
			if( WgMouseButtonEvent::Cast(_pEvent)->Button() ==WG_BUTTON_LEFT )
				pHandler->SwallowEvent(_pEvent);
			break;

		case WG_EVENT_FOCUS_GAINED:
			m_state.SetFocused(true);
			break;
		case WG_EVENT_FOCUS_LOST:
			m_state.SetFocused(false);
			m_bReturnPressed = false;
			m_bPressed = false;
			break;
	}


	if( m_bReturnPressed || (m_bPressed && (m_bDownOutside || m_state.IsHovered() )) )
		m_state.SetPressed(true);
	else
		m_state.SetPressed(false);

	if( m_state != oldState )
		_onStateChanged(oldState);
}


//____ _onRefresh() ____________________________________________________________

void WgButton::_onRefresh( void )
{
	WgWidget::_onRefresh();

	//TODO: Handling of icon and text.
}

//____ SetDownWhenMouseOutside() _______________________________________________

void WgButton::SetDownWhenMouseOutside( bool bDown )
{
		m_bDownOutside		= bDown;
}

//____ _onCloneContent() _______________________________________________________

void WgButton::_onCloneContent( const WgWidget * _pOrg )
{
	WgButton * pOrg = (WgButton *) _pOrg;

	icon.OnCloneContent( &pOrg->icon );

	label.clone(&pOrg->label);
	m_bDownOutside	= pOrg->m_bDownOutside;
}

//____ _onAlphaTest() ___________________________________________________________

bool WgButton::_onAlphaTest( const WgCoord& ofs, const WgSize& sz )
{
	if( icon.Skin() )
	{
		//TODO: Test against icon.
	}

	return WgWidget::_onAlphaTest(ofs,sz);
}

//____ _fieldModified() __________________________________________________________

void WgButton::_fieldModified( WgTextField * pField )
{
	_requestResize();
	_requestRender();
}

void WgButton::_fieldModified( WgIconField * pField )
{
	_requestResize();
	_requestRender();
}
