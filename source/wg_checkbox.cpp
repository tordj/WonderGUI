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
	m_bChecked			= false;
	m_bOver				= false;
	m_bPressed			= false;
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


//____ SetSkin() ____________________________________________________________

bool WgCheckBox::SetSkin( const WgSkinPtr& pSkin )
{
	m_pSkin = pSkin;

	_onRefresh();
	return true;
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

//____ SetChecked() _____________________________________________________________

bool WgCheckBox::SetChecked( bool bChecked )
{
	if( m_bChecked != bChecked )
	{
		m_bChecked = bChecked;

		WgEventHandler * pHandler = _eventHandler();
		if( pHandler )
		{
			if( bChecked )
				pHandler->QueueEvent( new WgEvent::CheckboxCheck( this ) );
			else
				pHandler->QueueEvent( new WgEvent::CheckboxUncheck( this ) );

			pHandler->QueueEvent( new WgEvent::CheckboxToggle(this, bChecked ) );
		}
		_requestRender();
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
	WgSize bgPreferredSize;
	WgSize textPreferredSize;

	if( m_text.nbChars() > 0 )
		textPreferredSize = m_text.unwrappedSize();

	if( m_pSkin )
	{
		bgPreferredSize = m_pSkin->PreferredSize();
		textPreferredSize += m_pSkin->ContentPadding();
	}

	if( m_pIconSkin )
	{
		iconPreferredSize = m_pIconSkin->PreferredSize() + m_iconBorders.Size();

		//TODO: Add magic for how icon influences textPreferredSize based on origo, iconBorders, iconScale and bgPreferredSize
	}

	WgSize preferredSize = WgSize::Max( WgSize::Max(iconPreferredSize,bgPreferredSize), textPreferredSize);

	return preferredSize;
}


//____ _onEnable() _________________________________________________
void WgCheckBox::_onEnable()
{
	_requestRender();
}

//____ _onDisable() _________________________________________________
void WgCheckBox::_onDisable()
{
	m_bOver = false;
	m_bPressed = false;

	_requestRender();
}

//____ _onEvent() _____________________________________________________________

void WgCheckBox::_onEvent( const WgEvent::Event * pEvent, WgEventHandler * pHandler )
{
	switch( pEvent->Type() )
	{
		case WG_EVENT_MOUSE_ENTER:
			if( !m_bOver )
			{
				m_bOver = true;
				_requestRender();
			}
			break;

		case WG_EVENT_MOUSE_LEAVE:
			if( m_bOver )
			{
				m_bOver = false;
				_requestRender();
			}
			break;

		case WG_EVENT_MOUSEBUTTON_PRESS:
		{
			int button = static_cast<const WgEvent::MouseButtonPress*>(pEvent)->Button();
			if( button == 1 )
			{
				if( !m_bPressed )
				{
					if( !m_bFlipOnRelease )
						SetChecked( !m_bChecked );
					m_bPressed = true;
					_requestRender();
				}
			}
			else
				pHandler->ForwardEvent( pEvent );
			break;
		}

		case WG_EVENT_MOUSEBUTTON_CLICK:
		case WG_EVENT_MOUSEBUTTON_DOUBLE_CLICK:
		case WG_EVENT_MOUSEBUTTON_REPEAT:
		case WG_EVENT_MOUSEBUTTON_DRAG:
		{
			int button = static_cast<const WgEvent::MouseButtonEvent*>(pEvent)->Button();
			if( button != 1 )
				pHandler->ForwardEvent( pEvent );
			break;
		}

		case WG_EVENT_MOUSEBUTTON_RELEASE:
		{
			int button = static_cast<const WgEvent::MouseButtonPress*>(pEvent)->Button();
			if( button == 1 )
			{
				if( m_bPressed )
				{
					m_bPressed = false;
					if( m_bFlipOnRelease )
						SetChecked( !m_bChecked );
					_requestRender();
				}
			}
			else
				pHandler->ForwardEvent( pEvent );
			break;
		}

        default:
			pHandler->ForwardEvent( pEvent );
            break;
	}
}

//____ GetTextAreaWidth() _____________________________________________________

Uint32 WgCheckBox::GetTextAreaWidth()
{
	WgSize widgetSize = Size();

	WgSize	iconSize;

	if( m_pIconSkin )
		iconSize = m_pIconSkin->PreferredSize();

	return _getTextRect( widgetSize, _getIconRect( WgRect(0,0,widgetSize), iconSize ) ).w;
}


//____ _onRender() ________________________________________________________

void WgCheckBox::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip )
{
	// Get correct state

	WgState	state = WG_STATE_NORMAL;
	if( !m_bEnabled )
		state = WG_STATE_DISABLED;
	else if( m_bOver )
	{
		state.setHovered(true);
		if( m_bPressed )
			state.setPressed(true);
	}

	if( m_bChecked )
		state.setSelected(true);

	// Blit background

	if( m_pSkin )
		m_pSkin->Render( pDevice, _canvas, state, _clip );

	// Get the content rect and icon rect

	WgRect contentRect	= _canvas;
	if( m_pSkin )
		contentRect = m_pSkin->ContentRect(_canvas, state );

	WgRect iconRect		= _getIconRect( contentRect, m_pIconSkin );

	// Blit icon

	if( m_pIconSkin && iconRect.w > 0 && iconRect.h > 0 )
		m_pIconSkin->Render( pDevice, iconRect, state, _clip );

	// Print text

 	if( m_text.nbLines()!= 0 )
	{
		WgRect	textRect = _getTextRect( contentRect, iconRect );
		m_pText->setState( state );

		if( m_pSkin )
			m_text.SetColorSkin( m_pSkin );

		pDevice->PrintText( _clip, m_pText, textRect );
	}
}

//____ _onRefresh() _______________________________________________________

void WgCheckBox::_onRefresh( void )
{
	if( m_pSkin && m_pSkin->IsOpaque() )
		m_bOpaque = true;
	else
		m_bOpaque = false;

	_requestRender();
}

//____ _onNewSize() _______________________________________________________

void WgCheckBox::_onNewSize( const WgSize& size )
{
	m_text.setLineWidth( GetTextAreaWidth() );
}


//____ _onCloneContent() _______________________________________________________

void WgCheckBox::_onCloneContent( const WgWidget * _pOrg )
{
	WgCheckBox * pOrg = (WgCheckBox *) _pOrg;

	m_bChecked			= pOrg->m_bChecked;
	m_bOver				= false;
	m_bPressed			= false;
	m_bFlipOnRelease	= pOrg->m_bFlipOnRelease;

	m_pSkin				= pOrg->m_pSkin;
	m_pIconSkin			= pOrg->m_pIconSkin;

	m_text				= pOrg->m_text;
	m_clickArea			= pOrg->m_clickArea;

	Wg_Interface_TextHolder::_onCloneContent( pOrg );
	WgIconHolder::_onCloneContent( pOrg );
}

//____ _textModified() _________________________________________________________

void WgCheckBox::_textModified()
{
	//TODO: Need to do more here, like possibly request resize.

	_requestRender();
}

//____ _iconModified() ________________________________________________________

void WgCheckBox::_iconModified()
{
	//TODO: Need to do more here, like possibly request resize.

	_requestRender();
}



//____ _markTestTextArea() ______________________________________________________

bool WgCheckBox::_markTestTextArea( int _x, int _y )
{
	WgRect	contentRect = _getTextRect( Size(), _getIconRect( Size(), m_pIconSkin ) );

	if( m_text.CoordToOfs( WgCoord(_x,_y), contentRect ) != -1 )
		return true;

	return false;
}

//____ _onAlphaTest() ______________________________________________________

bool WgCheckBox::_onAlphaTest( const WgCoord& ofs )
{
	WgState state = WG_STATE_NORMAL;

	if( !m_bEnabled )
		state = WG_STATE_DISABLED;
	else if( m_bOver )
	{
		state.setHovered(true);
		if( m_bPressed )
			state.setSelected(true);
	}

	if( m_bChecked )
		state.setSelected(true);


	WgSize	bgSize		= Size();
	WgRect	iconRect	= _getIconRect( bgSize, m_pIconSkin );

	switch( m_clickArea )
	{
		case DEFAULT:		// Full geometry of icon (no alpha test) + text + area between + alpha test on background.
		{
			// Extend iconRect so it connects with textArea before we compare

			WgRect	textRect = _getTextRect( bgSize, iconRect);

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

			if( (m_pSkin && m_pSkin->MarkTest( ofs, WgRect(0,0,bgSize), state, m_markOpacity )) ||
				_markTestTextArea( ofs.x, ofs.y ) ||
				iconRect.Contains( ofs ) )
				return true;

			return false;
		}
		case ALPHA:			// Alpha test on background and icon.
		{
			if( (m_pSkin && m_pSkin->MarkTest( ofs, WgRect(0,0,bgSize), state, m_markOpacity )) ||
				(m_pIconSkin && m_pIconSkin->MarkTest( ofs, iconRect, state, m_markOpacity )) )
				return true;

			return false;
		}
		case GEO:			// Full geometry of Widget is clickable.
			return true;
		case ICON:			// Only the icon (alpha test) is clickable.
		{
			if( m_pIconSkin && m_pIconSkin->MarkTest( ofs, iconRect, state, m_markOpacity ) )
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
