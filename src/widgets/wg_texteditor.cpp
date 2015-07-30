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

#include <wg_texteditor.h>
#include <wg_key.h>
#include <wg_font.h>
#include <wg_gfxdevice.h>
#include <wg_msgrouter.h>
#include <wg_base.h>

const char WgTextEditor::CLASSNAME[] = {"TextEditor"};



//____ WgTextEditor() _________________________________________________________________

WgTextEditor::WgTextEditor() : m_text(this), text(&m_text)
{
	m_maxLines		= 0;

	m_text.setLineWidth( size().w );
	m_text.setAutoEllipsis(IsAutoEllipsisDefault());
	m_text.setEditMode( WG_TEXT_STATIC );
	m_bResetCursorOnFocus = true;
	m_tickRouteId = 0;
}


//____ Destructor _____________________________________________________________

WgTextEditor::~WgTextEditor()
{
	if( m_tickRouteId )
		WgBase::msgRouter()->deleteRoute( m_tickRouteId );
}


//____ isInstanceOf() _________________________________________________________

bool WgTextEditor::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgWidget::isInstanceOf(pClassName);
}

//____ className() ____________________________________________________________

const char * WgTextEditor::className( void ) const
{ 
	return CLASSNAME; 
}

//____ cast() _________________________________________________________________

WgTextEditorPtr WgTextEditor::cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgTextEditorPtr( static_cast<WgTextEditor*>(pObject.rawPtr()) );

	return 0;
}

//_______________________________________________________________
void WgTextEditor::setEditMode(WgTextEditMode mode)
{
	m_text.setEditMode(mode);
}

//____ matchingHeight() _______________________________________________________

int WgTextEditor::matchingHeight( int width ) const
{
	int textHeight = m_text.heightForWidth( width );

	if( m_pSkin )
		textHeight += m_pSkin->contentPadding().h;

	return textHeight;
}

//____ preferredSize() _____________________________________________________________

WgSize WgTextEditor::preferredSize() const
{
	WgSize contentSize = m_text.unwrappedSize();

	if( m_pSkin )
		return m_pSkin->sizeForContent(contentSize);
	else
		return contentSize;
}

//____ PointerStyle() ________________________________________

WgPointerStyle WgTextEditor::PointerStyle() const
{
	if( m_text.getMarkedLink() )
		return WG_POINTER_HAND;

	return m_pointerStyle;
}

//____ TooltipString() _____________________________________________________

WgString WgTextEditor::TooltipString() const
{
	if( !m_tooltip.isEmpty() )
		return m_tooltip;
	else
	{
		WgSize sz = size();
		if( sz.w < m_text.width() || sz.h < m_text.height() )
			return m_text.getBuffer();
	}

	return 0;
}

//____ _onRender() ________________________________________________________

void WgTextEditor::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip )
{
	WgWidget::_onRender(pDevice,_canvas,_window,_clip);

	WgRect canvas;
	if( m_pSkin )
		canvas = m_pSkin->contentRect(_canvas, m_state);
	else
		canvas = _canvas;

	pDevice->printText( _clip, &m_text, canvas );
}

//____ _onRefresh() _______________________________________________________

void WgTextEditor::_onRefresh( void )
{
	//TODO: Implement more I believe...

	WgWidget::_onRefresh();
}

//____ _onStateChanged() ______________________________________________________

void WgTextEditor::_onStateChanged( WgState oldState )
{
	WgWidget::_onStateChanged(oldState);

	m_text.setState(m_state);
	_requestRender(); //TODO: Only requestRender if skin or text appearance has changed.

	if( isEditable() )
	{
		if( m_state.isFocused() && !oldState.isFocused() )
		{
			m_text.showCursor();
			m_tickRouteId = WgBase::msgRouter()->addRoute( WG_MSG_TICK, this );
			if(	m_bResetCursorOnFocus )
				m_text.goEot();
			_requestRender();
		}
		if( !m_state.isFocused() && oldState.isFocused() )
		{
			m_text.hideCursor();
			WgBase::msgRouter()->deleteRoute( m_tickRouteId );
			m_tickRouteId = 0;
			_requestRender();
		}
	}
}


//____ _onMsg() ______________________________________________________________

void WgTextEditor::_onMsg( const WgMsgPtr& pMsg )
{
	WgWidget::_onMsg(pMsg);

	int type 				= pMsg->type();
	WgModifierKeys modKeys 	= pMsg->modKeys();

	if( type == WG_MSG_TICK )
	{
		if( isSelectable() && m_state.isFocused() )
		{
			m_text.incTime( WgTickMsg::cast(pMsg)->millisec() );
			_requestRender();					//TODO: Should only render the cursor and selection!
		}
		return;
	}



	if( m_state.isFocused() && (type == WG_MSG_MOUSE_PRESS || type == WG_MSG_MOUSE_DRAG) && WgMouseButtonMsg::cast(pMsg)->button() == WG_BUTTON_LEFT )
	{

		if( isSelectable() && (modKeys & WG_MODKEY_SHIFT) )
		{
			m_text.setSelectionMode(true);
		}

		m_text.cursorGotoCoord( pMsg->pointerPos(), globalGeo() );

		if(isSelectable() && type == WG_MSG_MOUSE_PRESS && !(modKeys & WG_MODKEY_SHIFT))
		{
			m_text.clearSelection();
			m_text.setSelectionMode(true);
		}
	}
	else if( type == WG_MSG_MOUSE_RELEASE  )
	{
		if(m_state.isFocused() && WgMouseButtonMsg::cast(pMsg)->button() == WG_BUTTON_LEFT)
			m_text.setSelectionMode(false);
	}
	else if( !m_state.isFocused() && isEditable() && type == WG_MSG_MOUSE_PRESS && WgMouseButtonMsg::cast(pMsg)->button() == WG_BUTTON_LEFT )
	{
		GrabFocus();
	}


	if( type == WG_MSG_CHARACTER )
	{
		if( isEditable() )
		{
			int  chr = WgCharacterMsg::cast(pMsg)->character();

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

	if( type == WG_MSG_KEY_RELEASE )
	{
		switch( WgKeyMsg::cast(pMsg)->translatedKeyCode() )
		{
			case WG_KEY_SHIFT:
				if(!WgBase::msgRouter()->isMouseButtonPressed(1))
					m_text.setSelectionMode(false);
			break;
		}
	}

	if( (type == WG_MSG_KEY_PRESS || type == WG_MSG_KEY_REPEAT) && isEditable() )
	{
		switch( WgKeyMsg::cast(pMsg)->translatedKeyCode() )
		{
			case WG_KEY_LEFT:
				if( modKeys & WG_MODKEY_SHIFT )
					m_text.setSelectionMode(true);

				if( modKeys & WG_MODKEY_CTRL )
					m_text.gotoPrevWord();
				else
					m_text.goLeft();
				break;
			case WG_KEY_RIGHT:
				if( modKeys & WG_MODKEY_SHIFT )
					m_text.setSelectionMode(true);

				if( modKeys & WG_MODKEY_CTRL )
					m_text.gotoNextWord();
				else
					m_text.goRight();
				break;

			case WG_KEY_UP:
				if( modKeys & WG_MODKEY_SHIFT )
					m_text.setSelectionMode(true);

				m_text.cursorGoUp( 1, globalGeo() );
				break;

			case WG_KEY_DOWN:
				if( modKeys & WG_MODKEY_SHIFT )
					m_text.setSelectionMode(true);

				m_text.cursorGoDown( 1, globalGeo() );
				break;

			case WG_KEY_BACKSPACE:
				if(m_text.hasSelection())
					m_text.delSelection();
				else if( modKeys & WG_MODKEY_CTRL )
					m_text.delPrevWord();
				else
					m_text.delPrevChar();
				break;

			case WG_KEY_DELETE:
				if(m_text.hasSelection())
					m_text.delSelection();
				else if( modKeys & WG_MODKEY_CTRL )
					m_text.delNextWord();
				else
					m_text.delNextChar();
				break;

			case WG_KEY_HOME:
				if( modKeys & WG_MODKEY_SHIFT )
					m_text.setSelectionMode(true);

				if( modKeys & WG_MODKEY_CTRL )
					m_text.goBot();
				else
					m_text.goBol();
				break;

			case WG_KEY_END:
				if( modKeys & WG_MODKEY_SHIFT )
					m_text.setSelectionMode(true);

				if( modKeys & WG_MODKEY_CTRL )
					m_text.goEot();
				else
					m_text.goEol();
				break;

			default:
				break;
		}
	}

	// Let text object handle its actions.
/*
	bool bChanged = m_text.onAction( action, button_key, globalGeo(), WgCoord(info.x, info.y) );
	if( bChanged )
		RequestRender();
*/

	// Swallow message depending on rules.

	if( pMsg->isMouseButtonMsg() && isSelectable() )
	{
		if( WgMouseButtonMsg::cast(pMsg)->button() == WG_BUTTON_LEFT )
			pMsg->swallow();
	}
	else if( pMsg->isKeyMsg() && isEditable() )
	{
		int key = WgKeyMsg::cast(pMsg)->translatedKeyCode();
		if( WgKeyMsg::cast(pMsg)->isMovementKey() == true ||
			key == WG_KEY_DELETE || key == WG_KEY_BACKSPACE || key == WG_KEY_RETURN || (key == WG_KEY_TAB && m_bTabLock) )
				pMsg->swallow();
		
		//TODO: Would be good if we didn't forward any character-creating keys either...
	}
	else if( type == WG_MSG_CHARACTER )
		pMsg->swallow();
}


//____ _onCloneContent() _______________________________________________________

void WgTextEditor::_onCloneContent( const WgWidget * _pOrg )
{
	const WgTextEditor * pOrg = static_cast<const WgTextEditor*>(_pOrg);

	m_text = pOrg->m_text;
	m_maxLines = pOrg->m_maxLines;
}

//____ _onSkinChanged() _______________________________________________________

void WgTextEditor::_onSkinChanged( const WgSkinPtr& pOldSkin, const WgSkinPtr& pNewSkin )
{
	WgWidget::_onSkinChanged(pOldSkin,pNewSkin);
	m_text.setColorSkin(pNewSkin);
}

//____ _onNewSize() ________________________________________________

void WgTextEditor::_onNewSize( const WgSize& size )
{
	int width = size.w;

	if( m_pSkin )
		width -= m_pSkin->contentPadding().w;

	m_text.setLineWidth( width );
}



//____ InsertTextAtCursor() ___________________________________________________

int WgTextEditor::InsertTextAtCursor( const WgCharSeq& str )
{
	if( !isEditable() )
		return 0;

	if( !m_state.isFocused() )
		if( !GrabFocus() )
			return 0;				// Couldn't get input focus...

	int nChars = m_text.putText( str );

	if( m_maxLines != 0 && m_maxLines < m_text.nbSoftLines() )
	{
		m_text.unputText( nChars );
		nChars = 0;
	}

	return nChars;
}

//____ InsertCharAtCursor() ___________________________________________________

bool WgTextEditor::InsertCharAtCursor( Uint16 c )
{
	if( !isEditable() )
		return 0;

	if( !m_state.isFocused() )
		if( !GrabFocus() )
			return false;				// Couldn't get input focus...

	return _insertCharAtCursor(c);
}

//____ _insertCharAtCursor() ___________________________________________

bool WgTextEditor::_insertCharAtCursor( Uint16 c )
{
	if(m_text.hasSelection())
		m_text.delSelection();
	m_text.setSelectionMode(false);

	if( m_text.putChar( c ) == false )
		return false;

	if( m_maxLines != 0 && m_maxLines < (int) m_text.nbSoftLines() )
	{
		m_text.delPrevChar();
		return false;
	}

	return true;
}

//____ _onFieldDirty() _________________________________________________________

void WgTextEditor::_onFieldDirty( WgField * pField )
{
	_requestRender();
}

//____ _onFieldResize() ________________________________________________________

void WgTextEditor::_onFieldResize( WgField * pField )
{
	m_bResetCursorOnFocus = true;
	_requestResize();
	_requestRender();
}

