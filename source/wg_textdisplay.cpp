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

const char WgTextDisplay::CLASSNAME[] = {"TextDisplay"};



//____ WgTextDisplay() _________________________________________________________________

WgTextDisplay::WgTextDisplay()
{
	m_maxLines		= 0;
    text._setHolder( this );

	text.setLineWidth( Size().w );
	text.SetAutoEllipsis(IsAutoEllipsisDefault());
	text.SetEditMode( WG_TEXT_STATIC );
	m_bResetCursorOnFocus = true;
}


//____ Destructor _____________________________________________________________

WgTextDisplay::~WgTextDisplay()
{
}


//____ IsInstanceOf() _________________________________________________________

bool WgTextDisplay::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgWidget::IsInstanceOf(pClassName);
}

//____ ClassName() ____________________________________________________________

const char * WgTextDisplay::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ Cast() _________________________________________________________________

WgTextDisplayPtr WgTextDisplay::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgTextDisplayPtr( static_cast<WgTextDisplay*>(pObject.GetRealPtr()) );

	return 0;
}

//_______________________________________________________________
void WgTextDisplay::SetEditMode(WgTextEditMode mode)
{
	text.SetEditMode(mode);
}

//____ HeightForWidth() _______________________________________________________

int WgTextDisplay::HeightForWidth( int width ) const
{
	int textHeight = text.heightForWidth( width );

	if( m_pSkin )
		textHeight += m_pSkin->ContentPadding().h;

	return textHeight;
}

//____ PreferredSize() _____________________________________________________________

WgSize WgTextDisplay::PreferredSize() const
{
	WgSize contentSize = text.unwrappedSize();

	if( m_pSkin )
		return m_pSkin->SizeForContent(contentSize);
	else
		return contentSize;
}

//____ PointerStyle() ________________________________________

WgPointerStyle WgTextDisplay::PointerStyle() const
{
	if( text.GetMarkedLink() )
		return WG_POINTER_HAND;

	return m_pointerStyle;
}

//____ TooltipString() _____________________________________________________

WgString WgTextDisplay::TooltipString() const
{
	if( !m_tooltip.IsEmpty() )
		return m_tooltip;
	else
	{
		WgSize sz = Size();
		if( sz.w < text.Width() || sz.h < text.Height() )
			return text.getBuffer();
	}

	return 0;
}

//____ _onRender() ________________________________________________________

void WgTextDisplay::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip )
{
	WgWidget::_onRender(pDevice,_canvas,_window,_clip);

	WgRect canvas;
	if( m_pSkin )
		canvas = m_pSkin->ContentRect(_canvas, m_state);
	else
		canvas = _canvas;

	pDevice->PrintText( _clip, &text, canvas );
}

//____ _onRefresh() _______________________________________________________

void WgTextDisplay::_onRefresh( void )
{
	//TODO: Implement more I believe...

	WgWidget::_onRefresh();
}

//____ _onStateChanged() ______________________________________________________

void WgTextDisplay::_onStateChanged( WgState oldState )
{
	WgWidget::_onStateChanged(oldState);

	text.setState(m_state);
	_requestRender(); //TODO: Only requestRender if skin or text appearance has changed.

	if( IsEditable() )
	{
		if( m_state.IsFocused() && !oldState.IsFocused() )
		{
			text.showCursor();
			_startReceiveTicks();
			if(	m_bResetCursorOnFocus )
				text.GoEOF();
			_requestRender();
		}
		if( !m_state.IsFocused() && oldState.IsFocused() )
		{
			text.hideCursor();
			_stopReceiveTicks();
			_requestRender();
		}
	}
}


//____ _onEvent() ______________________________________________________________

void WgTextDisplay::_onEvent( const WgEventPtr& pEvent, WgEventHandler * pHandler )
{
	WgWidget::_onEvent(pEvent,pHandler);

	int type 				= pEvent->Type();
	WgModifierKeys modKeys 	= pEvent->ModKeys();

	if( type == WG_EVENT_TICK )
	{
		if( IsSelectable() && m_state.IsFocused() )
		{
			text.incTime( WgTickEvent::Cast(pEvent)->Millisec() );
			_requestRender();					//TODO: Should only render the cursor and selection!
		}
		return;
	}



	if( m_state.IsFocused() && (type == WG_EVENT_MOUSE_PRESS || type == WG_EVENT_MOUSE_DRAG) && WgMouseButtonEvent::Cast(pEvent)->Button() == WG_BUTTON_LEFT )
	{

		if( IsSelectable() && (modKeys & WG_MODKEY_SHIFT) )
		{
			text.setSelectionMode(true);
		}

		text.CursorGotoCoord( pEvent->PointerPos(), Geo() );

		if(IsSelectable() && type == WG_EVENT_MOUSE_PRESS && !(modKeys & WG_MODKEY_SHIFT))
		{
			text.ClearSelection();
			text.setSelectionMode(true);
		}
	}
	else if( type == WG_EVENT_MOUSE_RELEASE  )
	{
		if(m_state.IsFocused() && WgMouseButtonEvent::Cast(pEvent)->Button() == WG_BUTTON_LEFT)
			text.setSelectionMode(false);
	}
	else if( !m_state.IsFocused() && IsEditable() && type == WG_EVENT_MOUSE_PRESS && WgMouseButtonEvent::Cast(pEvent)->Button() == WG_BUTTON_LEFT )
	{
		GrabFocus();
	}


	if( type == WG_EVENT_CHARACTER )
	{
		if( IsEditable() )
		{
			int  chr = WgCharacterEvent::Cast(pEvent)->Char();

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
		}
	}

	if( type == WG_EVENT_KEY_RELEASE )
	{
		switch( WgKeyEvent::Cast(pEvent)->TranslatedKeyCode() )
		{
			case WG_KEY_SHIFT:
				if(!pHandler->IsMouseButtonPressed(1))
					text.setSelectionMode(false);
			break;
		}
	}

	if( (type == WG_EVENT_KEY_PRESS || type == WG_EVENT_KEY_REPEAT) && IsEditable() )
	{
		switch( WgKeyEvent::Cast(pEvent)->TranslatedKeyCode() )
		{
			case WG_KEY_LEFT:
				if( modKeys & WG_MODKEY_SHIFT )
					text.setSelectionMode(true);

				if( modKeys & WG_MODKEY_CTRL )
					text.gotoPrevWord();
				else
					text.goLeft();
				break;
			case WG_KEY_RIGHT:
				if( modKeys & WG_MODKEY_SHIFT )
					text.setSelectionMode(true);

				if( modKeys & WG_MODKEY_CTRL )
					text.gotoNextWord();
				else
					text.goRight();
				break;

			case WG_KEY_UP:
				if( modKeys & WG_MODKEY_SHIFT )
					text.setSelectionMode(true);

				text.CursorGoUp( 1, GlobalGeo() );
				break;

			case WG_KEY_DOWN:
				if( modKeys & WG_MODKEY_SHIFT )
					text.setSelectionMode(true);

				text.CursorGoDown( 1, GlobalGeo() );
				break;

			case WG_KEY_BACKSPACE:
				if(text.hasSelection())
					text.delSelection();
				else if( modKeys & WG_MODKEY_CTRL )
					text.delPrevWord();
				else
					text.delPrevChar();
				break;

			case WG_KEY_DELETE:
				if(text.hasSelection())
					text.delSelection();
				else if( modKeys & WG_MODKEY_CTRL )
					text.delNextWord();
				else
					text.delNextChar();
				break;

			case WG_KEY_HOME:
				if( modKeys & WG_MODKEY_SHIFT )
					text.setSelectionMode(true);

				if( modKeys & WG_MODKEY_CTRL )
					text.GoBOF();
				else
					text.GoBOL();
				break;

			case WG_KEY_END:
				if( modKeys & WG_MODKEY_SHIFT )
					text.setSelectionMode(true);

				if( modKeys & WG_MODKEY_CTRL )
					text.GoEOF();
				else
					text.GoEOL();
				break;

			default:
				break;
		}
	}

	// Let text object handle its actions.
/*
	bool bChanged = text.OnAction( action, button_key, GlobalGeo(), WgCoord(info.x, info.y) );
	if( bChanged )
		RequestRender();
*/

	// Swallow event depending on rules.

	if( pEvent->IsMouseButtonEvent() && IsSelectable() )
	{
		if( WgMouseButtonEvent::Cast(pEvent)->Button() == WG_BUTTON_LEFT )
			pHandler->SwallowEvent(pEvent);
	}
	else if( pEvent->IsKeyEvent() && IsEditable() )
	{
		int key = WgKeyEvent::Cast(pEvent)->TranslatedKeyCode();
		if( WgKeyEvent::Cast(pEvent)->IsMovementKey() == true ||
			key == WG_KEY_DELETE || key == WG_KEY_BACKSPACE || key == WG_KEY_RETURN || (key == WG_KEY_TAB && m_bTabLock) )
				pHandler->SwallowEvent(pEvent);
		
		//TODO: Would be good if we didn't forward any character-creating keys either...
	}
	else if( type == WG_EVENT_CHARACTER )
		pHandler->SwallowEvent(pEvent);
}


//____ _onCloneContent() _______________________________________________________

void WgTextDisplay::_onCloneContent( const WgWidget * _pOrg )
{
	const WgTextDisplay * pOrg = static_cast<const WgTextDisplay*>(_pOrg);

	text = pOrg->text;
	m_maxLines = pOrg->m_maxLines;
}

//____ _onSkinChanged() _______________________________________________________

void WgTextDisplay::_onSkinChanged( const WgSkinPtr& pOldSkin, const WgSkinPtr& pNewSkin )
{
	WgWidget::_onSkinChanged(pOldSkin,pNewSkin);
	text.SetColorSkin(pNewSkin);
}

//____ _onNewSize() ________________________________________________

void WgTextDisplay::_onNewSize( const WgSize& size )
{
	int width = size.w;

	if( m_pSkin )
		width -= m_pSkin->ContentPadding().w;

	text.setLineWidth( width );
}


//____ _fieldModified() _________________________________________________________

void WgTextDisplay::_fieldModified( WgTextField * pField )
{
	m_bResetCursorOnFocus = true;
    _requestResize();
	_requestRender();
}

//____ InsertTextAtCursor() ___________________________________________________

int WgTextDisplay::InsertTextAtCursor( const WgCharSeq& str )
{
	if( !IsEditable() )
		return 0;

	if( !m_state.IsFocused() )
		if( !GrabFocus() )
			return 0;				// Couldn't get input focus...

	int nChars = text.putText( str );

	if( m_maxLines != 0 && m_maxLines < text.nbSoftLines() )
	{
		text.unputText( nChars );
		nChars = 0;
	}

	return nChars;
}

//____ InsertCharAtCursor() ___________________________________________________

bool WgTextDisplay::InsertCharAtCursor( Uint16 c )
{
	if( !IsEditable() )
		return 0;

	if( !m_state.IsFocused() )
		if( !GrabFocus() )
			return false;				// Couldn't get input focus...

	return _insertCharAtCursor(c);
}

//____ _insertCharAtCursor() ___________________________________________

bool WgTextDisplay::_insertCharAtCursor( Uint16 c )
{
	if(text.hasSelection())
		text.delSelection();
	text.setSelectionMode(false);

	if( text.putChar( c ) == false )
		return false;

	if( m_maxLines != 0 && m_maxLines < (int) text.nbSoftLines() )
	{
		text.delPrevChar();
		return false;
	}

	return true;
}
