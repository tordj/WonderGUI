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

#include <wg_togglebutton.h>
#include <wg_surface.h>
#include <wg_gfxdevice.h>
#include <wg_font.h>
#include <wg_util.h>
#include <wg_rootpanel.h>
#include <wg_eventhandler.h>
#include <wg_togglegroup.h>
#include <assert.h>

const char WgToggleButton::CLASSNAME[] = {"ToggleButton"};



//____ WgToggleButton() _________________________________________________________________

WgToggleButton::WgToggleButton()
{
	m_bPressed			= false;
	m_bReturnPressed	= false;
	m_bFlipOnRelease	= false;

	label._setHolder( this );
	label.SetAutoEllipsis(IsAutoEllipsisDefault());

	icon._setHolder( this );

	m_clickArea			= DEFAULT;
}

//____ Destructor _____________________________________________________________

WgToggleButton::~WgToggleButton()
{
	if( m_pToggleGroup )
		m_pToggleGroup->_remove(this);
}


//____ IsInstanceOf() _________________________________________________________

bool WgToggleButton::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgWidget::IsInstanceOf(pClassName);
}

//____ ClassName() ____________________________________________________________

const char * WgToggleButton::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ Cast() _________________________________________________________________

WgToggleButtonPtr WgToggleButton::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgToggleButtonPtr( static_cast<WgToggleButton*>(pObject.GetRealPtr()) );

	return 0;
}

//____ SetSelected() __________________________________________________________

bool WgToggleButton::SetSelected( bool bSelected )
{
	if( m_state.IsSelected() != bSelected )
	{
		WgState oldState = m_state;
		m_state.SetSelected(bSelected);
		_onStateChanged(oldState);
	}
	return true;
}

//____ SetFlipOnRelease() _____________________________________________________

void WgToggleButton::SetFlipOnRelease( bool bFlipOnRelease )
{
	m_bFlipOnRelease = bFlipOnRelease;
}

//____ PreferredSize() __________________________________________________

WgSize WgToggleButton::PreferredSize() const
{
	WgSize iconPreferredSize;
	WgSize textPreferredSize;

	if( label.Length() > 0 )
		textPreferredSize = label.unwrappedSize();

	if( !icon.IsEmpty() )
	{
		iconPreferredSize = icon.Skin()->PreferredSize() + icon.Padding().Size();

		//TODO: Add magic for how icon influences textPreferredSize based on origo, iconBorder, iconScale and bgPreferredSize
	}

	// Apply the skin

	WgSize preferredSize = WgSize::Max( iconPreferredSize, textPreferredSize );

	if( m_pSkin )
		preferredSize = m_pSkin->SizeForContent( preferredSize );

	return preferredSize;
}

//____ _setToggleGroup() ________________________________________________________

void WgToggleButton::_setToggleGroup( WgToggleGroup * pGroup )
{
	if( m_pToggleGroup && pGroup )			// Remove us from current Toggle group if we are not just set to null (then we are being removed by Togglegroup itself).
		m_pToggleGroup->_remove(this);
		
	m_pToggleGroup = pGroup;
}


//____ _onEvent() _____________________________________________________________

void WgToggleButton::_onEvent( const WgEventPtr& _pEvent, WgEventHandler * pHandler )
{
	WgState oldState = m_state;

	switch( _pEvent->Type() )
	{
		case WG_EVENT_KEY_PRESS:
			if( WgKeyEvent::Cast(_pEvent)->TranslatedKeyCode() == WG_KEY_RETURN )
			{
				m_bReturnPressed = true;
				pHandler->SwallowEvent(_pEvent);
			}
			break;

		case WG_EVENT_KEY_REPEAT:
			if( WgKeyEvent::Cast(_pEvent)->TranslatedKeyCode() == WG_KEY_RETURN )
				pHandler->SwallowEvent(_pEvent);
			break;

		case WG_EVENT_KEY_RELEASE:
			if( WgKeyEvent::Cast(_pEvent)->TranslatedKeyCode() == WG_KEY_RETURN )
			{
				m_bReturnPressed = false;
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
		case WG_EVENT_MOUSE_DOUBLE_CLICK:
		case WG_EVENT_MOUSE_REPEAT:
		case WG_EVENT_MOUSE_DRAG:
			if( WgMouseButtonEvent::Cast(_pEvent)->Button() == WG_BUTTON_LEFT )
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

	// Set pressed if return or mouse button 1 is pressed

	if( m_bReturnPressed || (m_bPressed && m_state.IsHovered()) )
		m_state.SetPressed(true);
	else
		m_state.SetPressed(false);

	// Possibly flip selected

	if( m_state.IsPressed() != oldState.IsPressed() )
	{
		if( m_state.IsPressed() != m_bFlipOnRelease )
			m_state.SetSelected( !m_state.IsSelected() );
	}

	//

	if( m_state != oldState )
		_onStateChanged(oldState);
}

//____ _onStateChanged() ______________________________________________________

void WgToggleButton::_onStateChanged( WgState oldState )
{
	// If state has changed from selected to unselected we need to check with Togglegroup
	
	if( !m_state.IsSelected() && oldState.IsSelected() && m_pToggleGroup && !m_pToggleGroup->_unselect(this) )
		m_state.SetSelected(true);
	
	//
	
	WgWidget::_onStateChanged(oldState);

	label.setState( m_state );

	if( !icon.IsEmpty() && !icon.Skin()->IsStateIdentical(m_state, oldState) )
		_requestRender();		//TODO: Just request render on icon?

	if( m_state.IsSelected() != oldState.IsSelected() )
	{
		WgEventHandler * pHandler = _eventHandler();
		if( pHandler )
			pHandler->QueueEvent( new WgToggleEvent(this, m_state.IsSelected() ) );

		if( m_pToggleGroup && m_state.IsSelected() )
			m_pToggleGroup->_select(this);
	}
}

//____ _onSkinChanged() _______________________________________________________

void WgToggleButton::_onSkinChanged( const WgSkinPtr& pOldSkin, const WgSkinPtr& pNewSkin )
{
	WgWidget::_onSkinChanged(pOldSkin,pNewSkin);
	label.SetColorSkin(pNewSkin);
}


//____ _onRender() ________________________________________________________

void WgToggleButton::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip )
{
	WgWidget::_onRender(pDevice,_canvas,_window,_clip);

	// Get the content rect and icon rect

	WgRect contentRect	= _canvas;
	if( m_pSkin )
		contentRect = m_pSkin->ContentRect(_canvas, m_state );

	WgRect iconRect		= icon.GetIconRect( contentRect );

	// Blit icon

	if( icon.IsEmpty() && iconRect.w > 0 && iconRect.h > 0 )
		icon.Skin()->Render( pDevice, iconRect, m_state, _clip );

	// Print text

 	if( label.Lines()!= 0 )
	{
		WgRect	textRect = icon.GetTextRect( contentRect, iconRect );
		pDevice->PrintText( _clip, &label, textRect );
	}
}

//____ _onRefresh() _______________________________________________________

void WgToggleButton::_onRefresh( void )
{
	WgWidget::_onRefresh();

	//TODO: Handling of icon and text?
}

//____ _onNewSize() _______________________________________________________

void WgToggleButton::_onNewSize( const WgSize& size )
{
	WgRect contentRect	= WgRect(0,0,size);
	if( m_pSkin )
		contentRect = m_pSkin->ContentRect(contentRect, m_state );

	label.setLineWidth( icon.GetTextRect( contentRect, icon.GetIconRect( contentRect )).w );
}


//____ _onCloneContent() _______________________________________________________

void WgToggleButton::_onCloneContent( const WgWidget * _pOrg )
{
	WgToggleButton * pOrg = (WgToggleButton *) _pOrg;

	m_bFlipOnRelease	= pOrg->m_bFlipOnRelease;
	m_clickArea			= pOrg->m_clickArea;

	label.clone( &pOrg->label );
	icon.OnCloneContent( &pOrg->icon );
}

//____ _fieldModified() _________________________________________________________

void WgToggleButton::_fieldModified( WgTextField * pField )
{
	_requestResize();
	_requestRender();
}

void WgToggleButton::_fieldModified( WgIconField * pField )
{
	_requestResize();
	_requestRender();
}

//____ _markTestTextArea() ______________________________________________________

bool WgToggleButton::_markTestTextArea( int _x, int _y )
{
	WgRect contentRect	= WgRect(0,0,Size());
	if( m_pSkin )
		contentRect = m_pSkin->ContentRect(contentRect, m_state );

	contentRect = icon.GetTextRect( contentRect, icon.GetIconRect( contentRect ) );

	if( label.CoordToOfs( WgCoord(_x,_y), contentRect ) != -1 )
		return true;

	return false;
}

//____ _onAlphaTest() ______________________________________________________

bool WgToggleButton::_onAlphaTest( const WgCoord& ofs )
{
	WgSize	bgSize		= Size();

	WgRect	contentRect = WgRect(0,0,bgSize);
	if( m_pSkin )
		contentRect = m_pSkin->ContentRect( contentRect, m_state );

	WgRect	iconRect	= icon.GetIconRect( contentRect );

	switch( m_clickArea )
	{
		case DEFAULT:		// Full geometry of icon (no alpha test) + text + area between + alpha test on background.
		{
			// Extend iconRect so it connects with textArea before we compare

			WgRect	textRect = icon.GetTextRect( contentRect, iconRect);

			if( iconRect.x + iconRect.w < textRect.x )
				iconRect.w = textRect.x - iconRect.x;

			if( iconRect.x > textRect.Right() )
			{
				iconRect.w += iconRect.x - textRect.Right();
				iconRect.x = textRect.Right();
			}

			if( iconRect.y + iconRect.h < textRect.y )
				iconRect.h = textRect.y - iconRect.y;

			if( iconRect.y > textRect.Bottom() )
			{
				iconRect.h += iconRect.y - textRect.Bottom();
				iconRect.y = textRect.Bottom();
			}

			//

			if( WgWidget::_onAlphaTest( ofs ) || _markTestTextArea( ofs.x, ofs.y ) || iconRect.Contains( ofs ) )
				return true;

			return false;
		}
		case ALPHA:			// Alpha test on background and icon.
		{
			if( WgWidget::_onAlphaTest( ofs ) ||
				( !m_icon.IsEmpty() && m_icon.Skin()->MarkTest( ofs, iconRect, m_state, m_markOpacity )) )
				return true;

			return false;
		}
		case GEO:			// Full geometry of Widget is clickable.
			return true;
		case ICON:			// Only the icon (alpha test) is clickable.
		{
			if( !icon.IsEmpty() && icon.Skin()->MarkTest( ofs, iconRect, m_state, m_markOpacity ) )
				return true;

			return false;
		}
		case TEXT:			// Only the text is clickable.
		{
			if( _markTestTextArea( ofs.x, ofs.y ) )
				return true;

			return false;
		}

		default:
			assert(false);			// Garbage in m_clickArea
			return false;
	};
}
