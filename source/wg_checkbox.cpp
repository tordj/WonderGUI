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

static const char	c_widgetType[] = {"Checkbox"};



//____ WgCheckBox() _________________________________________________________________

WgCheckBox::WgCheckBox()
{
	m_bPressed			= false;
	m_bReturnPressed	= false;
	m_bFlipOnRelease	= false;

	m_pText				= &m_text;
	m_text.setHolder( this );
	m_text.SetAutoEllipsis(IsAutoEllipsisDefault());

	m_clickArea			= DEFAULT;
}

//____ Destructor _____________________________________________________________

WgCheckBox::~WgCheckBox()
{
}


//____ Type() _________________________________________________________________

const char * WgCheckBox::Type( void ) const
{
	return GetClass();
}

const char * WgCheckBox::GetClass( void )
{
	return c_widgetType;
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

	if( m_text.nbChars() > 0 )
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

void WgCheckBox::_onEvent( const WgEvent::Event * _pEvent, WgEventHandler * pHandler )
{
	WgState oldState = m_state;

	switch( _pEvent->Type() )
	{
		case WG_EVENT_KEY_PRESS:
			if( static_cast<const WgEvent::KeyPress*>(_pEvent)->TranslatedKeyCode() == WG_KEY_RETURN )
			{
				m_bReturnPressed = true;
				_pEvent->Swallow();
			}
			break;

		case WG_EVENT_KEY_REPEAT:
			if( static_cast<const WgEvent::KeyPress*>(_pEvent)->TranslatedKeyCode() == WG_KEY_RETURN )
				_pEvent->Swallow();
			break;

		case WG_EVENT_KEY_RELEASE:
			if( static_cast< const WgEvent::KeyPress*>(_pEvent)->TranslatedKeyCode() == WG_KEY_RETURN )
			{
				m_bReturnPressed = false;
				_pEvent->Swallow();
			}
			break;
	
		case WG_EVENT_MOUSE_ENTER:
			m_state.SetHovered(true);
			break;
		case WG_EVENT_MOUSE_LEAVE:
			m_state.SetHovered(false);
			break;
		case WG_EVENT_MOUSEBUTTON_PRESS:
			if( static_cast<const WgEvent::MouseButtonPress*>(_pEvent)->Button() == 1 )
			{
				m_bPressed = true;
				_pEvent->Swallow();
			}
			break;
		case WG_EVENT_MOUSEBUTTON_RELEASE:
			if( static_cast<const WgEvent::MouseButtonRelease*>(_pEvent)->Button() == 1 )
			{
				m_bPressed = false;
				_pEvent->Swallow();
			}
			break;
		case WG_EVENT_MOUSEBUTTON_CLICK:
		case WG_EVENT_MOUSEBUTTON_DOUBLE_CLICK:
		case WG_EVENT_MOUSEBUTTON_REPEAT:
		case WG_EVENT_MOUSEBUTTON_DRAG:
			if( static_cast<const WgEvent::MouseButtonEvent*>(_pEvent)->Button() == 1 )
				_pEvent->Swallow();
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

	m_pText->setState( newState );

	if( m_pIconSkin && !m_pIconSkin->IsStateIdentical(newState, oldState) )
		_requestRender();		//TODO: Just request render on icon?

	if( newState.IsSelected() != oldState.IsSelected() )
	{
		WgEventHandler * pHandler = _eventHandler();
		if( pHandler )
		{
			if( newState.IsSelected() )
				pHandler->QueueEvent( new WgEvent::CheckboxCheck( this ) );
			else
				pHandler->QueueEvent( new WgEvent::CheckboxUncheck( this ) );

			pHandler->QueueEvent( new WgEvent::CheckboxToggle(this, newState.IsSelected() ) );
		}
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

 	if( m_text.nbLines()!= 0 )
	{
		WgRect	textRect = _getTextRect( contentRect, iconRect );
		pDevice->PrintText( _clip, m_pText, textRect );
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

	Wg_Interface_TextHolder::_onCloneContent( pOrg );
	WgIconHolder::_onCloneContent( pOrg );
}

//____ _textModified() _________________________________________________________

void WgCheckBox::_textModified()
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
