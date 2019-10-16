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

#include <wg_textdisplay.h>
#include <wg_key.h>
#include <wg_font.h>
#include <wg_gfxdevice.h>
#include <wg_eventhandler.h>
#include <wg_util.h>

static const char	c_widgetType[] = {"TextDisplay"};


//____ WgTextDisplay() _________________________________________________________________

WgTextDisplay::WgTextDisplay()
{
	m_pText			= &m_text;
	m_maxLines		= 0;
    m_text.setHolder( this );

	m_text.setLineWidth( PixelSize().w );
	m_text.SetAutoEllipsis(IsAutoEllipsisDefault());
	m_text.SetEditMode( WgTextEditMode::Static );
	m_bResetCursorOnFocus = true;
    m_str.assign("");
}


//____ Destructor _____________________________________________________________

WgTextDisplay::~WgTextDisplay()
{
}


//____ Type() _________________________________________________________________

const char * WgTextDisplay::Type() const
{
	return WgTextDisplay::GetClass();
}

const char * WgTextDisplay::GetClass()
{
	return c_widgetType;
}

//____ GoBOL() ________________________________________________________________
void WgTextDisplay::GoBOL()
{
	if( IsEditable() && m_bFocused )
		m_pText->goBOL();
}

//____ GoEOL() ________________________________________________________________
void WgTextDisplay::GoEOL()
{
	if( IsEditable() && m_bFocused )
		m_pText->goEOL();
}

//____ GoBOF() ________________________________________________________________
void WgTextDisplay::GoBOF()
{
	if( IsEditable() && m_bFocused )
		m_pText->goBOF();
}

//____ GoEOF() ________________________________________________________________
void WgTextDisplay::GoEOF()
{
	if( IsEditable() && m_bFocused )
		m_pText->goEOF();
}


//_______________________________________________________________
void WgTextDisplay::SetEditMode(WgTextEditMode mode)
{
	m_text.SetEditMode(mode);
}

//____ MatchingPixelHeight() _______________________________________________________

int WgTextDisplay::MatchingPixelHeight( int width ) const
{
	int height = 0;

	if (m_pSkin)
		height = m_pSkin->PreferredSize(m_scale).h;

	if (m_text.nbChars() != 0 || m_text.GetEditMode() == WgTextEditMode::Editable )
	{
		WgSize padding;

		if (m_pSkin)
			padding = m_pSkin->ContentPadding(m_scale);

		int heightForText = m_text.heightForWidth(width - padding.w) + padding.h;
		if (heightForText > height)
			height = heightForText;
	}

	return height;
}

//____ PreferredPixelSize() _____________________________________________________________

WgSize WgTextDisplay::PreferredPixelSize() const
{
	WgSize bestSize;

	if (m_pSkin)
		bestSize = m_pSkin->PreferredSize(m_scale);


    if (m_text.nbChars() != 0 || m_text.GetEditMode() == WgTextEditMode::Editable )
	{
		WgSize textSize = m_text.unwrappedSize();

		if (m_pSkin)
			textSize += m_pSkin->ContentPadding(m_scale);

		if (textSize.w > bestSize.w)
			bestSize.w = textSize.w;

		if (textSize.h > bestSize.h)
			bestSize.h = textSize.h;
	}

	return bestSize;
}

//____ GetPointerStyle() ________________________________________

WgPointerStyle WgTextDisplay::GetPointerStyle() const
{
	if( m_text.GetMarkedLink() )
		return WgPointerStyle::Hand;

	return m_pointerStyle;
}

//____ GetTooltipString() _____________________________________________________

WgString WgTextDisplay::GetTooltipString() const
{
	if( !m_tooltip.IsEmpty() )
		return m_tooltip;
	else
	{
		WgSize sz = PixelSize();
		if( sz.w < m_text.width() || sz.h < m_text.height() )
			return m_text.getBuffer();
	}

	return 0;
}

//____ _onRender() ________________________________________________________

void WgTextDisplay::_onRender( wg::GfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window )
{
    WgWidget::_onRender(pDevice,_canvas,_window);

	WgRect canvas = m_pSkin ? m_pSkin->ContentRect(_canvas, m_state, m_scale) : _canvas;

	//    m_text.setMode(WG_MODE_NORMAL);
	//    m_text.setMode(WgUtil::StateToMode(m_state));

	WgText * pText = &m_text;

	if( m_bFocused && IsEditable() )
		m_text.showCursor();
	else
		m_text.hideCursor();

    WgGfxDevice::PrintText( pDevice, pText, canvas );

	if( pText != &m_text )
		delete pText;
}

//____ _onRefresh() _______________________________________________________

void WgTextDisplay::_onRefresh( void )
{
	//TODO: Implement more I believe...

	_requestRender();
}

//____ _onEvent() ______________________________________________________________

void WgTextDisplay::_onEvent( const WgEvent::Event * pEvent, WgEventHandler * pHandler )
{
	bool bSwallowed = false;

	WgState	oldState = m_state;

	int type 				= pEvent->Type();
	WgModifierKeys modKeys 	= pEvent->ModKeys();

	if( type == WG_EVENT_TICK )
	{
		if( IsSelectable() && m_bFocused )
		{
			m_pText->incTime( ((const WgEvent::Tick*)(pEvent))->Millisec() );
			_requestRender();					//TODO: Should only render the cursor and selection!
		}
		return;
	}


	if (type == WG_EVENT_MOUSE_ENTER)
	{
		m_state.setHovered(true);
	}

	if (type == WG_EVENT_MOUSE_LEAVE)
	{
		m_state.setHovered(false);
	}

	if(type == WG_EVENT_MOUSEBUTTON_PRESS)
	{
		m_state.setPressed(true);
	}

	if (type == WG_EVENT_MOUSEBUTTON_RELEASE)
	{
		m_state.setPressed(false);
	}


	if( m_bFocused && (type == WG_EVENT_MOUSEBUTTON_PRESS || type == WG_EVENT_MOUSEBUTTON_DRAG) && ((const WgEvent::MouseButtonEvent*)(pEvent))->Button() == 1 )
	{

		if( IsSelectable() && (modKeys & WG_MODKEY_SHIFT) )
		{
			m_pText->setSelectionMode(true);
		}

        WgRect textCanvas = PixelSize();
        if( m_pSkin )
            textCanvas = m_pSkin->ContentRect( textCanvas, WgStateEnum::Normal, m_scale );

		m_pText->CursorGotoCoord( pEvent->PointerPixelPos(), textCanvas );
        _bringCursorInView();

		if(IsSelectable() && type == WG_EVENT_MOUSEBUTTON_PRESS && !(modKeys & WG_MODKEY_SHIFT))
		{
			m_pText->clearSelection();
			m_pText->setSelectionMode(true);
		}

		bSwallowed = true;
	}
	else if( type == WG_EVENT_MOUSEBUTTON_RELEASE  )
	{
		if (m_bFocused && ((const WgEvent::MouseButtonEvent*)(pEvent))->Button() == 1)
		{
			m_pText->setSelectionMode(false);
			bSwallowed = true;
		}
	}
	else if( !m_bFocused && IsEditable() && type == WG_EVENT_MOUSEBUTTON_PRESS && ((const WgEvent::MouseButtonEvent*)(pEvent))->Button() == 1 )
	{
		GrabFocus();
        _bringCursorInView();
		bSwallowed = true;
	}


	if( type == WG_EVENT_CHARACTER )
	{
		if( IsEditable() && m_bFocused )
		{
			int  chr = static_cast<const WgEvent::Character*>(pEvent)->Char();

			if( chr >= 32 && chr != 127)
			{
				_insertCharAtCursor(chr);
			}
			else if( chr == 13 )
			{
				_insertCharAtCursor('\n');
			}
			else if( chr == '\t' && m_bTabLock )
			{
				_insertCharAtCursor( '\t' );
			}
            
            _bringCursorInView();
			bSwallowed = true;
		}
	}

	if( type == WG_EVENT_KEY_RELEASE && m_bFocused )
	{
		switch( static_cast<const WgEvent::KeyEvent*>(pEvent)->TranslatedKeyCode() )
		{
			case WG_KEY_SHIFT:
				if(!pHandler->IsMouseButtonPressed(1))
					m_pText->setSelectionMode(false);
				bSwallowed = true;
				break;
		}
	}

	if( (type == WG_EVENT_KEY_PRESS || type == WG_EVENT_KEY_REPEAT) && IsEditable() && m_bFocused )
	{
		switch( static_cast<const WgEvent::KeyEvent*>(pEvent)->TranslatedKeyCode() )
		{
			case WG_KEY_LEFT:
				if( modKeys & WG_MODKEY_SHIFT )
					m_pText->setSelectionMode(true);

				if( modKeys & WG_MODKEY_CTRL )
					m_pText->gotoPrevWord();
				else
					m_pText->goLeft();
				bSwallowed = true;
                _bringCursorInView();
				break;
			case WG_KEY_RIGHT:
				if( modKeys & WG_MODKEY_SHIFT )
					m_pText->setSelectionMode(true);

				if( modKeys & WG_MODKEY_CTRL )
					m_pText->gotoNextWord();
				else
					m_pText->goRight();
				bSwallowed = true;
                _bringCursorInView();
				break;

			case WG_KEY_UP:
				if( modKeys & WG_MODKEY_SHIFT )
					m_pText->setSelectionMode(true);

				m_pText->CursorGoUp( 1, ScreenPixelGeo() );
				bSwallowed = true;
                _bringCursorInView();
				break;

			case WG_KEY_DOWN:
				if( modKeys & WG_MODKEY_SHIFT )
					m_pText->setSelectionMode(true);

				m_pText->CursorGoDown( 1, ScreenPixelGeo() );
				bSwallowed = true;
                _bringCursorInView();
				break;

			case WG_KEY_BACKSPACE:
				if(m_pText->hasSelection())
					m_pText->delSelection();
				else if( modKeys & WG_MODKEY_CTRL )
					m_pText->delPrevWord();
				else
					m_pText->delPrevChar();
				bSwallowed = true;
                _bringCursorInView();
				break;

			case WG_KEY_DELETE:
				if(m_pText->hasSelection())
					m_pText->delSelection();
				else if( modKeys & WG_MODKEY_CTRL )
					m_pText->delNextWord();
				else
					m_pText->delNextChar();
				bSwallowed = true;
                _bringCursorInView();
				break;

			case WG_KEY_HOME:
				if( modKeys & WG_MODKEY_SHIFT )
					m_pText->setSelectionMode(true);

				if( modKeys & WG_MODKEY_CTRL )
					m_pText->goBOF();
				else
					m_pText->goBOL();
				bSwallowed = true;
                _bringCursorInView();
				break;

			case WG_KEY_END:
				if( modKeys & WG_MODKEY_SHIFT )
					m_pText->setSelectionMode(true);

				if( modKeys & WG_MODKEY_CTRL )
					m_pText->goEOF();
				else
					m_pText->goEOL();
				bSwallowed = true;
                _bringCursorInView();
				break;

			default:
				break;
		}
	}

	// Let text object handle its actions.
/*
	bool bChanged = m_text.OnAction( action, button_key, ScreenPixelGeo(), WgCoord(info.x, info.y) );
	if( bChanged )
		RequestRender();
*/

	//

	if (oldState != m_state)
	{
		if (m_pSkin && !m_pSkin->IsStateIdentical(m_state, oldState))
			_requestRender();

		m_text.setMode(WgUtil::StateToMode(m_state));
	}


	// Forward event depending on rules.

	if (!bSwallowed)
	{
        WgWidget::_onEvent( pEvent, pHandler );
	}
}

//____ _bringCursorInView() _______________________________________________________

void WgTextDisplay::_bringCursorInView()
{
    WgCursorInstance * pCursor = m_pText->GetCursor();

    WgCoord cursorPos = { pCursor->ofsX(), pCursor->ofsY() };

    WgTextAttr attr;
    m_pText->GetBaseAttr(attr);
    
    auto pGlyphs = attr.pFont->GetGlyphset(attr.style, attr.size);
    int glyphHeight = pGlyphs->GetHeight(attr.size);
    int glyphWidth = pGlyphs->GetWhitespaceAdvance(attr.size);

    if(m_pSkin)
        cursorPos += m_pSkin->ContentOfs(m_state, m_scale);

    WgRect mustHaveArea = { cursorPos.x - glyphWidth, cursorPos.y, glyphWidth*2, glyphHeight };
    WgRect niceToHaveArea = { cursorPos.x - glyphWidth, cursorPos.y - glyphHeight/2, glyphWidth*2, glyphHeight*2 };

    WgRect canvas = WgRect( 0,0, PixelSize() );
    _requestInView( WgRect(mustHaveArea,canvas), WgRect(niceToHaveArea, canvas) );
}

//____ _onCloneContent() _______________________________________________________

void WgTextDisplay::_onCloneContent( const WgWidget * _pOrg )
{
	//TODO: Implement!
}

//____ _onAlphaTest() ______________________________________________________

bool WgTextDisplay::_onAlphaTest( const WgCoord& ofs )
{
	return true;																				// Accept all at least for now...
}

//____ _onEnable() ___________________________________________________

void WgTextDisplay::_onEnable( void )
{
	m_text.setMode(WG_MODE_NORMAL);
	_requestRender();
}

//____ _onDisable() ___________________________________________________

void WgTextDisplay::_onDisable( void )
{
	m_text.setMode(WG_MODE_DISABLED);
	_requestRender();
}

//____ _onNewSize() ________________________________________________

void WgTextDisplay::_onNewSize( const WgSize& size )
{
	WgRect	contentRect(0, 0, PixelSize());

	if (m_pSkin)
		contentRect = m_pSkin->ContentRect(contentRect, WgStateEnum::Normal, m_scale);

	m_text.setLineWidth( contentRect.w );
}


//____ _onGotInputFocus() ______________________________________________

void WgTextDisplay::_onGotInputFocus()
{
	WgWidget::_onGotInputFocus();

	if( IsEditable() ) // render with cursor on
	{
		_startReceiveTicks();
		if(	m_bResetCursorOnFocus )
			m_pText->goEOF();
		_requestRender();
	}
}

//____ _onLostInputFocus() ______________________________________________

void WgTextDisplay::_onLostInputFocus()
{
	WgWidget::_onLostInputFocus();

	m_bResetCursorOnFocus = false;
	if( IsEditable() )
	{
		_queueEvent(new WgEvent::TextSet(this, m_pText));
		_stopReceiveTicks();
		_requestRender();
        _queueEvent(new WgEvent::TextSet(this, m_pText));
	}
}

//____ _setScale() _____________________________________________________________

void WgTextDisplay::_setScale( int scale )
{
	WgWidget::_setScale(scale);

	m_text.SetScale(scale);

	_requestResize();
}



//____ _textModified() _________________________________________________________

void WgTextDisplay::_textModified()
{
	m_bResetCursorOnFocus = true;
    _requestResize();
	_requestRender();
    _queueEvent(new WgEvent::TextModify(this, m_pText));
}


//____ SetSkin() ______________________________________________________________

void WgTextDisplay::SetSkin(const WgSkinPtr& pSkin)
{
	if (pSkin != m_pSkin)
	{
		m_pSkin = pSkin;
		_requestResize();
		_requestRender();
	}
}

//____ InsertTextAtCursor() ___________________________________________________

Uint32 WgTextDisplay::InsertTextAtCursor( const WgCharSeq& str )
{
	if( !IsEditable() )
		return 0;

	if( m_bFocused )
		if( !GrabFocus() )
			return 0;				// Couldn't get input focus...

	Uint32 nChars = m_pText->putText( str );

	if( m_maxLines != 0 && m_maxLines < (int) m_pText->nbSoftLines() )
	{
		m_pText->unputText( nChars );
		nChars = 0;
	}

	return nChars;
}

//____ InsertCharAtCursor() ___________________________________________________

bool WgTextDisplay::InsertCharAtCursor( Uint16 c )
{
	if( !IsEditable() )
		return 0;

	if( m_bFocused )
		if( !GrabFocus() )
			return false;				// Couldn't get input focus...

	return _insertCharAtCursor(c);
}

//____ _insertCharAtCursor() ___________________________________________

bool WgTextDisplay::_insertCharAtCursor( Uint16 c )
{
	if(m_pText->hasSelection())
		m_pText->delSelection();
	m_pText->setSelectionMode(false);

	if( m_pText->putChar( c ) == false )
		return false;

	if( m_maxLines != 0 && m_maxLines < (int) m_pText->nbSoftLines() )
	{
		m_pText->delPrevChar();
		return false;
	}

	return true;
}
