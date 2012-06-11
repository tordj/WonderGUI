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



#include <wg_gizmo_button.h>
#include <wg_gfxdevice.h>
#include <wg_util.h>
#include <wg_key.h>
#include <wg_event.h>
#include <wg_eventhandler.h>

static const char	c_gizmoType[] = {"Button"};

//____ Constructor ____________________________________________________________

WgGizmoButton::WgGizmoButton()
{
	m_pText = &m_text;
	m_text.setAlignment( WG_CENTER );
	m_text.setLineWidth(Size().w);					// We start with no textborders...
	m_text.SetAutoEllipsis(IsAutoEllipsisDefault());

 	m_mode				= WG_MODE_NORMAL;

	m_bDownOutside	 = false;
	m_bPressed 		 = false;
	m_bReturnPressed = false;
	m_bPointerInside = false;
}

//____ Destructor _____________________________________________________________

WgGizmoButton::~WgGizmoButton()
{
}

//____ Type() _________________________________________________________________

const char * WgGizmoButton::Type( void ) const
{
	return GetMyType();
}

//____ GetMyType() ____________________________________________________________

const char * WgGizmoButton::GetMyType()
{
	return c_gizmoType;
}

//____ SetSource() ____________________________________________________________

bool WgGizmoButton::SetSource( const WgBlockSetPtr& pGfx )
{
	m_pBgGfx = pGfx;

	if( pGfx && pGfx->IsOpaque() )
		m_bOpaque = true;
	else
		m_bOpaque = false;

	_requestRender();
	return true;
}

//____ SetIcon() ______________________________________________________________

void WgGizmoButton::SetIcon( const WgBlockSetPtr& pIconGfx )
{
	m_pIconGfx = pIconGfx;
	_iconModified();
}

bool WgGizmoButton::SetIcon( const WgBlockSetPtr& pIconGfx, WgOrientation orientation, WgBorders borders, float scale, bool bPushText )
{
	if( scale < 0 || scale > 1.f )
		return false;

	m_pIconGfx = pIconGfx;
	m_iconOrientation = orientation;
	m_iconBorders = borders;
	m_iconScale = scale;
	m_bIconPushText = bPushText;

	_iconModified();
	return true;
}


//____ GetTextAreaWidth() _____________________________________________________

Uint32 WgGizmoButton::GetTextAreaWidth()
{
	WgRect	contentRect(0,0,Size());

	if( m_pBgGfx )
		contentRect.Shrink(m_pBgGfx->ContentBorders());

	WgRect textRect = _getTextRect( contentRect, _getIconRect( contentRect, m_pIconGfx ) );

	return textRect.w;
}

//____ HeightForWidth() _______________________________________________________

int WgGizmoButton::HeightForWidth( int width ) const
{
	int height = 0;

	if( m_pBgGfx )
		height = m_pBgGfx->Height();

	if( m_text.nbChars() != 0 )
	{
		WgBorders borders;

		if( m_pBgGfx )
			borders = m_pBgGfx->ContentBorders();

		int heightForText = m_text.heightForWidth(width-borders.Width()) + borders.Height();
		if( heightForText > height )
			height = heightForText;
	}

	//TODO: Take icon into account.

	return height;
}


//____ DefaultSize() _____________________________________________________________

WgSize WgGizmoButton::DefaultSize() const
{
	WgSize bestSize;

	if( m_pBgGfx )
		bestSize = m_pBgGfx->Size();

	if( m_text.nbChars() != 0 )
	{
		WgSize textSize = m_text.unwrappedSize();

		if( m_pBgGfx )
			textSize += m_pBgGfx->ContentBorders();

		if( textSize.w > bestSize.w )
			bestSize.w = textSize.w;

		if( textSize.h > bestSize.h )
			bestSize.h = textSize.h;
	}

	//TODO: Take icon into account.

	return bestSize;
}


//____ _onEnable() _____________________________________________________________

void WgGizmoButton::_onEnable()
{
	m_mode = WG_MODE_NORMAL;
	_requestRender();
}

//____ _onDisable() ____________________________________________________________

void WgGizmoButton::_onDisable()
{
	m_mode = WG_MODE_DISABLED;
	_requestRender();
}

//____ _onNewSize() ____________________________________________________________

void WgGizmoButton::_onNewSize( const WgSize& size )
{
	WgRect	contentRect(0,0,Size());

	if( m_pBgGfx )
		contentRect.Shrink(m_pBgGfx->ContentBorders());

	WgRect textRect = _getTextRect( contentRect, _getIconRect( contentRect, m_pIconGfx ) );

	m_text.setLineWidth(textRect.w);
}


//____ _onRender() _____________________________________________________________

void WgGizmoButton::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip, Uint8 _layer )
{
	WgRect cli = _clip;
	WgRect can = _canvas;
	WgRect win = _window;

	WgBlock	block;

	if( m_pBgGfx )
		block = m_pBgGfx->GetBlock(m_mode, _canvas);

	// Render background

	pDevice->ClipBlitBlock( _clip, block, _canvas );

	// Get content rect with displacement.

	WgRect contentRect = block.ContentRect(_canvas );

	// Get icon and text rect from content rect

	WgRect iconRect = _getIconRect( contentRect, m_pIconGfx );
	WgRect textRect = _getTextRect( contentRect, iconRect );

	// Render icon

	if( m_pIconGfx )
		pDevice->ClipBlitBlock( _clip, m_pIconGfx->GetBlock(m_mode, iconRect.Size()), iconRect );

	// Print text

 	if( !m_text.IsEmpty() )
	{
		m_text.setMode(m_mode);

		if( m_pBgGfx )
			m_text.SetBgBlockColors( m_pBgGfx->TextColors() );

		WgRect clip(textRect,_clip);
		pDevice->PrintText( clip, &m_text, textRect );
	}
}

//____ _onEvent() ______________________________________________________________

void WgGizmoButton::_onEvent( const WgEvent::Event * pEvent, WgEventHandler * pHandler )
{
	switch( pEvent->Type() )
	{
		case	WG_EVENT_KEY_PRESS:
		{
			if( static_cast<const WgEvent::KeyPress*>(pEvent)->TranslatedKeyCode() == WG_KEY_RETURN )
				m_bReturnPressed = true;
			else
				pHandler->ForwardEvent( pEvent );
			break;
		}

		case	WG_EVENT_KEY_REPEAT:
		{
			if( static_cast<const WgEvent::KeyPress*>(pEvent)->TranslatedKeyCode() == WG_KEY_RETURN )
			{
			}
			else
				pHandler->ForwardEvent( pEvent );
			break;
		}


		case	WG_EVENT_KEY_RELEASE:
		{
			if( static_cast< const WgEvent::KeyPress*>(pEvent)->TranslatedKeyCode() == WG_KEY_RETURN )
			{
				m_bReturnPressed = false;
				pHandler->QueueEvent( new WgEvent::ButtonPress(this) );
			}
			else
				pHandler->ForwardEvent( pEvent );
			break;
		}

		case	WG_EVENT_MOUSE_ENTER:
			m_bPointerInside = true;
			break;

		case	WG_EVENT_MOUSE_LEAVE:
			m_bPointerInside = false;
			break;

		case WG_EVENT_MOUSEBUTTON_PRESS:
		{
			int button = static_cast<const WgEvent::MouseButtonPress*>(pEvent)->Button();
			if( button == 1 )
				m_bPressed = true;
			else
				pHandler->ForwardEvent( pEvent );
			break;
		}
		case WG_EVENT_MOUSEBUTTON_RELEASE:
		{
			int button = static_cast<const WgEvent::MouseButtonRelease*>(pEvent)->Button();
			if( button == 1 )
				m_bPressed = false;
			else
				pHandler->ForwardEvent( pEvent );
			break;
		}

		case WG_EVENT_MOUSEBUTTON_CLICK:
		{
			int button = static_cast<const WgEvent::MouseButtonClick*>(pEvent)->Button();
			if( button == 1 )
				pHandler->QueueEvent( new WgEvent::ButtonPress(this) );
			else
				pHandler->ForwardEvent( pEvent );
			break;
		}

		case WG_EVENT_MOUSEBUTTON_DOUBLE_CLICK:
		case WG_EVENT_MOUSEBUTTON_REPEAT:
		case WG_EVENT_MOUSEBUTTON_DRAG:
		{
			int button = static_cast<const WgEvent::MouseButtonEvent*>(pEvent)->Button();
			if( button != 1 )
				pHandler->ForwardEvent( pEvent );
			break;
		}


        default:
			pHandler->ForwardEvent( pEvent );
            break;

	}

	WgMode newMode = _getRenderMode();
	if( newMode != m_mode )
	{
		m_mode = newMode;
		_requestRender();
	}

}


//_____ _getRenderMode() ________________________________________________________

WgMode WgGizmoButton::_getRenderMode()
{
	if( !IsEnabled() )
		return WG_MODE_DISABLED;

	if( m_bReturnPressed || (m_bPressed && (m_bPointerInside || m_bDownOutside)) )
		return WG_MODE_SELECTED;

	if( m_bPointerInside )
		return WG_MODE_MARKED;

	return WG_MODE_NORMAL;
}


//____ _onRefresh() ____________________________________________________________

void WgGizmoButton::_onRefresh( void )
{
	if( m_pBgGfx )
	{
		if( m_pBgGfx->IsOpaque() )
			m_bOpaque = true;
		else
			m_bOpaque = false;

		_requestRender();
	}
}

//____ SetDownWhenMouseOutside() _______________________________________________

void WgGizmoButton::SetDownWhenMouseOutside( bool bDown )
{
		m_bDownOutside		= bDown;
}

//____ _onCloneContent() _______________________________________________________

void WgGizmoButton::_onCloneContent( const WgGizmo * _pOrg )
{

	WgGizmoButton * pOrg = (WgGizmoButton *) _pOrg;

	pOrg->Wg_Interface_TextHolder::_cloneInterface( this );
	WgIconHolder::_onCloneContent( pOrg );

	m_text.setText(&pOrg->m_text);
	m_pText = &m_text;
	m_text.setHolder( this );

	m_pBgGfx		= pOrg->m_pBgGfx;
	m_pIconGfx		= pOrg->m_pIconGfx;
	m_mode			= pOrg->m_mode;
}

//____ _onAlphaTest() ___________________________________________________________

bool WgGizmoButton::_onAlphaTest( const WgCoord& ofs )
{
	if( !m_pBgGfx )
		return false;

	WgSize	sz = Size();

	//TODO: Take icon into account.

	return	WgUtil::MarkTestBlock( ofs, m_pBgGfx->GetBlock(m_mode,sz), WgRect(0,0,sz) );
}

//____ _onGotInputFocus() ______________________________________________________

void WgGizmoButton::_onGotInputFocus()
{
	m_bFocused = true;
	_requestRender();
}

//____ _onLostInputFocus() _____________________________________________________

void WgGizmoButton::_onLostInputFocus()
{
	m_bFocused = false;
	m_bReturnPressed = false;
	_requestRender();
}


//____ _textModified() __________________________________________________________

void WgGizmoButton::_textModified()
{
	_requestRender();
}

//____ _iconModified() __________________________________________________________

void WgGizmoButton::_iconModified()
{
	//TODO: Should possibly refresh size too.
	_requestRender();
}
