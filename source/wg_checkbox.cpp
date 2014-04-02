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

#include <wg_checkbox.h>
#include <wg_surface.h>
#include <wg_gfxdevice.h>
#include <wg_font.h>
#include <wg_util.h>
#include <wg_rootpanel.h>
#include <wg_eventhandler.h>
#include <assert.h>

const char WgCheckBox::CLASSNAME[] = {"Checkbox"};



//____ WgCheckBox() _________________________________________________________________

WgCheckBox::WgCheckBox()
{
	m_bPressed			= false;
	m_bReturnPressed	= false;
	m_bFlipOnRelease	= false;

	m_text.setHolder( this );
	m_text.SetAutoEllipsis(IsAutoEllipsisDefault());

	m_clickArea			= DEFAULT;
}

//____ Destructor _____________________________________________________________

WgCheckBox::~WgCheckBox()
{
}


//____ IsInstanceOf() _________________________________________________________

bool WgCheckBox::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgWidget::IsInstanceOf(pClassName);
}

//____ ClassName() ____________________________________________________________

const char * WgCheckBox::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ Cast() _________________________________________________________________

WgCheckBoxPtr WgCheckBox::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgCheckBoxPtr( static_cast<WgCheckBox*>(pObject.GetRealPtr()) );

	return 0;
}

//____ SetIcon() ______________________________________________________________

void WgCheckBox::SetIcon( const WgSkinPtr& pIconSkin, const WgOrigo& origo, 
						  WgBorders borders, float scale, bool bPushText )
{
	m_pIconSkin		= pIconSkin;
	m_iconOrigo		= origo;
	m_iconScale		= scale;
	m_bIconPushText	= bPushText;
	m_iconBorders	= borders;
	_onRefresh();
}

void WgCheckBox::SetIcon( const WgSkinPtr& pIconSkin )
{
	m_pIconSkin		= pIconSkin;
	_onRefresh();
}

//____ SetSelected() __________________________________________________________

bool WgCheckBox::SetSelected( bool bSelected )
{
	if( m_state.IsSelected() != bSelected )
	{
		WgState oldState = m_state;
		m_state.SetSelected(bSelected);
		_onStateChanged(oldState,m_state);
	}
	return true;
}

//____ SetFlipOnRelease() _____________________________________________________

void WgCheckBox::SetFlipOnRelease( bool bFlipOnRelease )
{
	m_bFlipOnRelease = bFlipOnRelease;
}


//____ PreferredSize() __________________________________________________

WgSize WgCheckBox::PreferredSize() const
{
	WgSize iconPreferredSize;
	WgSize textPreferredSize;

	if( m_text.Length() > 0 )
		textPreferredSize = m_text.unwrappedSize();

	if( m_pIconSkin )
	{
		iconPreferredSize = m_pIconSkin->PreferredSize() + m_iconBorders.Size();

		//TODO: Add magic for how icon influences textPreferredSize based on origo, iconBorders, iconScale and bgPreferredSize
	}

	// Apply the skin

	WgSize preferredSize = WgSize::Max( iconPreferredSize, textPreferredSize );

	if( m_pSkin )
		preferredSize = m_pSkin->SizeForContent( preferredSize );

	return preferredSize;
}


//____ _onEvent() _____________________________________________________________

void WgCheckBox::_onEvent( const WgEventPtr& _pEvent, WgEventHandler * pHandler )
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
		_onStateChanged(oldState,m_state);
}

//____ _onStateChanged() ______________________________________________________

void WgCheckBox::_onStateChanged( WgState oldState, WgState newState )
{
	WgWidget::_onStateChanged(oldState,newState);

	m_text.setState( newState );

	if( m_pIconSkin && !m_pIconSkin->IsStateIdentical(newState, oldState) )
		_requestRender();		//TODO: Just request render on icon?

	if( newState.IsSelected() != oldState.IsSelected() )
	{
		WgEventHandler * pHandler = _eventHandler();
		if( pHandler )
			pHandler->QueueEvent( new WgToggleEvent(this, newState.IsSelected() ) );
	}
}

//____ _onSkinChanged() _______________________________________________________

void WgCheckBox::_onSkinChanged( const WgSkinPtr& pOldSkin, const WgSkinPtr& pNewSkin )
{
	WgWidget::_onSkinChanged(pOldSkin,pNewSkin);
	m_text.SetColorSkin(pNewSkin);
}


//____ _onRender() ________________________________________________________

void WgCheckBox::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip )
{
	WgWidget::_onRender(pDevice,_canvas,_window,_clip);

	// Get the content rect and icon rect

	WgRect contentRect	= _canvas;
	if( m_pSkin )
		contentRect = m_pSkin->ContentRect(_canvas, m_state );

	WgRect iconRect		= _getIconRect( contentRect, m_pIconSkin );

	// Blit icon

	if( m_pIconSkin && iconRect.w > 0 && iconRect.h > 0 )
		m_pIconSkin->Render( pDevice, iconRect, m_state, _clip );

	// Print text

 	if( m_text.Lines()!= 0 )
	{
		WgRect	textRect = _getTextRect( contentRect, iconRect );
		pDevice->PrintText( _clip, &m_text, textRect );
	}
}

//____ _onRefresh() _______________________________________________________

void WgCheckBox::_onRefresh( void )
{
	WgWidget::_onRefresh();

	//TODO: Handling of icon and text?
}

//____ _onNewSize() _______________________________________________________

void WgCheckBox::_onNewSize( const WgSize& size )
{
	WgRect contentRect	= WgRect(0,0,size);
	if( m_pSkin )
		contentRect = m_pSkin->ContentRect(contentRect, m_state );

	WgSize	iconSize;
	if( m_pIconSkin )
		iconSize = m_pIconSkin->PreferredSize();

	m_text.setLineWidth( _getTextRect( contentRect, _getIconRect( contentRect, iconSize )).w );
}


//____ _onCloneContent() _______________________________________________________

void WgCheckBox::_onCloneContent( const WgWidget * _pOrg )
{
	WgCheckBox * pOrg = (WgCheckBox *) _pOrg;


	m_bFlipOnRelease	= pOrg->m_bFlipOnRelease;
	m_pIconSkin			= pOrg->m_pIconSkin;

	m_text				= pOrg->m_text;
	m_clickArea			= pOrg->m_clickArea;

	WgIconHolder::_onCloneContent( pOrg );
}

//____ _textModified() _________________________________________________________

void WgCheckBox::_textModified( WgText * pText )
{
	_requestResize();
	_requestRender();
}

//____ _iconModified() ________________________________________________________

void WgCheckBox::_iconModified()
{
	_requestResize();
	_requestRender();
}

//____ _markTestTextArea() ______________________________________________________

bool WgCheckBox::_markTestTextArea( int _x, int _y )
{
	WgRect contentRect	= WgRect(0,0,Size());
	if( m_pSkin )
		contentRect = m_pSkin->ContentRect(contentRect, m_state );

	contentRect = _getTextRect( contentRect, _getIconRect( contentRect, m_pIconSkin ) );

	if( m_text.CoordToOfs( WgCoord(_x,_y), contentRect ) != -1 )
		return true;

	return false;
}

//____ _onAlphaTest() ______________________________________________________

bool WgCheckBox::_onAlphaTest( const WgCoord& ofs )
{
	WgSize	bgSize		= Size();

	WgRect	contentRect = WgRect(0,0,bgSize);
	if( m_pSkin )
		contentRect = m_pSkin->ContentRect( contentRect, m_state );

	WgRect	iconRect	= _getIconRect( contentRect, m_pIconSkin );

	switch( m_clickArea )
	{
		case DEFAULT:		// Full geometry of icon (no alpha test) + text + area between + alpha test on background.
		{
			// Extend iconRect so it connects with textArea before we compare

			WgRect	textRect = _getTextRect( contentRect, iconRect);

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
				(m_pIconSkin && m_pIconSkin->MarkTest( ofs, iconRect, m_state, m_markOpacity )) )
				return true;

			return false;
		}
		case GEO:			// Full geometry of Widget is clickable.
			return true;
		case ICON:			// Only the icon (alpha test) is clickable.
		{
			if( m_pIconSkin && m_pIconSkin->MarkTest( ofs, iconRect, m_state, m_markOpacity ) )
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
