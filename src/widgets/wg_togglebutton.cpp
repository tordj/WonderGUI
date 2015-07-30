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
#include <wg_msgrouter.h>
#include <wg_togglegroup.h>
#include <assert.h>

const char WgToggleButton::CLASSNAME[] = {"ToggleButton"};



//____ WgToggleButton() _________________________________________________________________

WgToggleButton::WgToggleButton() : m_label(this), m_icon(this), label(&m_label), icon(&m_icon)
{
	m_bPressed			= false;
	m_bReturnPressed	= false;
	m_bFlipOnRelease	= false;

	m_clickArea			= DEFAULT;
}

//____ Destructor _____________________________________________________________

WgToggleButton::~WgToggleButton()
{
	if( m_pToggleGroup )
		m_pToggleGroup->_remove(this);
}


//____ isInstanceOf() _________________________________________________________

bool WgToggleButton::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgWidget::isInstanceOf(pClassName);
}

//____ className() ____________________________________________________________

const char * WgToggleButton::className( void ) const
{ 
	return CLASSNAME; 
}

//____ cast() _________________________________________________________________

WgToggleButtonPtr WgToggleButton::cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgToggleButtonPtr( static_cast<WgToggleButton*>(pObject.rawPtr()) );

	return 0;
}

//____ setSelected() __________________________________________________________

bool WgToggleButton::setSelected( bool bSelected )
{
	if( m_state.isSelected() != bSelected )
	{
		WgState oldState = m_state;
		m_state.setSelected(bSelected);
		_onStateChanged(oldState);
	}
	return true;
}

//____ SetFlipOnRelease() _____________________________________________________

void WgToggleButton::SetFlipOnRelease( bool bFlipOnRelease )
{
	m_bFlipOnRelease = bFlipOnRelease;
}

//____ preferredSize() __________________________________________________

WgSize WgToggleButton::preferredSize() const
{
	WgSize iconPreferredSize;
	WgSize textPreferredSize;

	if( !m_label.isEmpty() )
		textPreferredSize = m_label.preferredSize();

	if( !m_icon.isEmpty() )
	{
		iconPreferredSize = m_icon.skin()->preferredSize() + m_icon.padding().size();

		//TODO: Add magic for how icon influences textPreferredSize based on origo, iconBorder, iconScale and bgPreferredSize
	}

	// Apply the skin

	WgSize preferredSize = WgSize::max( iconPreferredSize, textPreferredSize );

	if( m_pSkin )
		preferredSize = m_pSkin->sizeForContent( preferredSize );

	return preferredSize;
}

//____ _setToggleGroup() ________________________________________________________

void WgToggleButton::_setToggleGroup( WgToggleGroup * pGroup )
{
	if( m_pToggleGroup && pGroup )			// Remove us from current Toggle group if we are not just set to null (then we are being removed by Togglegroup itself).
		m_pToggleGroup->_remove(this);
		
	m_pToggleGroup = pGroup;
}


//____ _onMsg() _____________________________________________________________

void WgToggleButton::_onMsg( const WgMsgPtr& _pMsg )
{
	WgState oldState = m_state;

	switch( _pMsg->type() )
	{
		case WG_MSG_KEY_PRESS:
			if( WgKeyMsg::cast(_pMsg)->translatedKeyCode() == WG_KEY_RETURN )
			{
				m_bReturnPressed = true;
				_pMsg->swallow();
			}
			break;

		case WG_MSG_KEY_REPEAT:
			if( WgKeyMsg::cast(_pMsg)->translatedKeyCode() == WG_KEY_RETURN )
				_pMsg->swallow();
			break;

		case WG_MSG_KEY_RELEASE:
			if( WgKeyMsg::cast(_pMsg)->translatedKeyCode() == WG_KEY_RETURN )
			{
				m_bReturnPressed = false;
				_pMsg->swallow();
			}
			break;
	
		case WG_MSG_MOUSE_ENTER:
			m_state.setHovered(true);
			break;
		case WG_MSG_MOUSE_LEAVE:
			m_state.setHovered(false);
			break;
		case WG_MSG_MOUSE_PRESS:
			if( WgMousePressMsg::cast(_pMsg)->button() == WG_BUTTON_LEFT )
			{
				m_bPressed = true;
				_pMsg->swallow();
			}
			break;
		case WG_MSG_MOUSE_RELEASE:
			if( WgMouseReleaseMsg::cast(_pMsg)->button() == WG_BUTTON_LEFT )
			{
				m_bPressed = false;
				_pMsg->swallow();
			}
			break;
		case WG_MSG_MOUSE_CLICK:
		case WG_MSG_MOUSE_DOUBLE_CLICK:
		case WG_MSG_MOUSE_REPEAT:
		case WG_MSG_MOUSE_DRAG:
			if( WgMouseButtonMsg::cast(_pMsg)->button() == WG_BUTTON_LEFT )
				_pMsg->swallow();
			break;

		case WG_MSG_FOCUS_GAINED:
			m_state.setFocused(true);
			break;
		case WG_MSG_FOCUS_LOST:
			m_state.setFocused(false);
			m_bReturnPressed = false;
			m_bPressed = false;
			break;
	}

	// Set pressed if return or mouse button 1 is pressed

	if( m_bReturnPressed || (m_bPressed && m_state.isHovered()) )
		m_state.setPressed(true);
	else
		m_state.setPressed(false);

	// Possibly flip selected

	if( m_state.isPressed() != oldState.isPressed() )
	{
		if( m_state.isPressed() != m_bFlipOnRelease )
			m_state.setSelected( !m_state.isSelected() );
	}

	//

	if( m_state != oldState )
		_onStateChanged(oldState);
}

//____ _onStateChanged() ______________________________________________________

void WgToggleButton::_onStateChanged( WgState oldState )
{
	// If state has changed from selected to unselected we need to check with Togglegroup
	
	if( !m_state.isSelected() && oldState.isSelected() && m_pToggleGroup && !m_pToggleGroup->_unselect(this) )
		m_state.setSelected(true);
	
	//
	
	WgWidget::_onStateChanged(oldState);

	m_label.setState( m_state );

	if( !m_icon.isEmpty() && !m_icon.skin()->isStateIdentical(m_state, oldState) )
		_requestRender();		//TODO: Just request render on icon?

	if( m_state.isSelected() != oldState.isSelected() )
	{
		WgBase::msgRouter()->post( new WgToggleMsg(this, m_state.isSelected() ) );

		if( m_pToggleGroup && m_state.isSelected() )
			m_pToggleGroup->_select(this);
	}
}

//____ _onSkinChanged() _______________________________________________________

void WgToggleButton::_onSkinChanged( const WgSkinPtr& pOldSkin, const WgSkinPtr& pNewSkin )
{
	WgWidget::_onSkinChanged(pOldSkin,pNewSkin);
}


//____ _onRender() ________________________________________________________

void WgToggleButton::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip )
{
	WgWidget::_onRender(pDevice,_canvas,_window,_clip);

	// Get the content rect and icon rect

	WgRect contentRect	= _canvas;
	if( m_pSkin )
		contentRect = m_pSkin->contentRect(_canvas, m_state );

	WgRect iconRect		= m_icon.getIconRect( contentRect );

	// Blit icon

	if( m_icon.isEmpty() && iconRect.w > 0 && iconRect.h > 0 )
		m_icon.skin()->render( pDevice, iconRect, m_state, _clip );

	// Print text

 	if( !m_label.isEmpty() )
	{
		WgRect	textRect = m_icon.getTextRect( contentRect, iconRect );
		m_label.onRender( pDevice, textRect, _clip );
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
		contentRect = m_pSkin->contentRect(contentRect, m_state );

	m_label.onNewSize( m_icon.getTextRect( contentRect, m_icon.getIconRect( contentRect )) );
}


//____ _onCloneContent() _______________________________________________________

void WgToggleButton::_onCloneContent( const WgWidget * _pOrg )
{
	WgToggleButton * pOrg = (WgToggleButton *) _pOrg;

	m_bFlipOnRelease	= pOrg->m_bFlipOnRelease;
	m_clickArea			= pOrg->m_clickArea;

	m_icon.onCloneContent( &pOrg->m_icon );
}

//____ _markTestTextArea() ______________________________________________________

bool WgToggleButton::_markTestTextArea( int _x, int _y )
{
	WgRect contentRect	= WgRect(0,0,size());
	if( m_pSkin )
		contentRect = m_pSkin->contentRect(contentRect, m_state );

	contentRect = m_icon.getTextRect( contentRect, m_icon.getIconRect( contentRect ) );

	if( m_label.coordToChar( WgCoord(_x,_y) - contentRect.pos() ) != -1 )
		return true;

	return false;
}

//____ _onAlphaTest() ______________________________________________________

bool WgToggleButton::_onAlphaTest( const WgCoord& ofs )
{
	WgSize	bgSize		= size();

	WgRect	contentRect = WgRect(0,0,bgSize);
	if( m_pSkin )
		contentRect = m_pSkin->contentRect( contentRect, m_state );

	WgRect	iconRect	= m_icon.getIconRect( contentRect );

	switch( m_clickArea )
	{
		case DEFAULT:		// Full geometry of icon (no alpha test) + text + area between + alpha test on background.
		{
			// Extend iconRect so it connects with textArea before we compare

			WgRect	textRect = m_icon.getTextRect( contentRect, iconRect);

			if( iconRect.x + iconRect.w < textRect.x )
				iconRect.w = textRect.x - iconRect.x;

			if( iconRect.x > textRect.right() )
			{
				iconRect.w += iconRect.x - textRect.right();
				iconRect.x = textRect.right();
			}

			if( iconRect.y + iconRect.h < textRect.y )
				iconRect.h = textRect.y - iconRect.y;

			if( iconRect.y > textRect.bottom() )
			{
				iconRect.h += iconRect.y - textRect.bottom();
				iconRect.y = textRect.bottom();
			}

			//

			if( WgWidget::_onAlphaTest( ofs, bgSize ) || _markTestTextArea( ofs.x, ofs.y ) || iconRect.contains( ofs ) )
				return true;

			return false;
		}
		case ALPHA:			// Alpha test on background and icon.
		{
			if( WgWidget::_onAlphaTest( ofs, bgSize ) ||
				( !m_icon.isEmpty() && m_icon.skin()->markTest( ofs, iconRect, m_state, m_markOpacity )) )
				return true;

			return false;
		}
		case GEO:			// Full geometry of Widget is clickable.
			return true;
		case ICON:			// Only the icon (alpha test) is clickable.
		{
			if( !m_icon.isEmpty() && m_icon.skin()->markTest( ofs, iconRect, m_state, m_markOpacity ) )
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

//____ _onFieldDirty() _________________________________________________________

void WgToggleButton::_onFieldDirty( WgField * pField )
{
	_requestRender();
}

//____ _onFieldResize() ________________________________________________________

void WgToggleButton::_onFieldResize( WgField * pField )
{
	_requestResize();
	_requestRender();
}
