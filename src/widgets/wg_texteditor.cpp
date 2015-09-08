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

namespace wg 
{
	
	const char TextEditor::CLASSNAME[] = {"TextEditor"};
	
	
	
	//____ TextEditor() _________________________________________________________________
	
	TextEditor::TextEditor() : m_text(this), text(&m_text)
	{
		m_maxLines		= 0;
	
		m_text.setLineWidth( size().w );
		m_text.setAutoEllipsis(isAutoEllipsisDefault());
		m_text.setEditMode( WG_TEXT_STATIC );
		m_bResetCursorOnFocus = true;
		m_tickRouteId = 0;
	}
	
	
	//____ Destructor _____________________________________________________________
	
	TextEditor::~TextEditor()
	{
		if( m_tickRouteId )
			Base::msgRouter()->deleteRoute( m_tickRouteId );
	}
	
	
	//____ isInstanceOf() _________________________________________________________
	
	bool TextEditor::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return Widget::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * TextEditor::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	TextEditor_p TextEditor::cast( const Object_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return TextEditor_p( static_cast<TextEditor*>(pObject.rawPtr()) );
	
		return 0;
	}
	
	//_______________________________________________________________
	void TextEditor::setEditMode(TextEditMode mode)
	{
		m_text.setEditMode(mode);
	}
	
	//____ matchingHeight() _______________________________________________________
	
	int TextEditor::matchingHeight( int width ) const
	{
		int textHeight = m_text.heightForWidth( width );
	
		if( m_pSkin )
			textHeight += m_pSkin->contentPadding().h;
	
		return textHeight;
	}
	
	//____ preferredSize() _____________________________________________________________
	
	Size TextEditor::preferredSize() const
	{
		Size contentSize = m_text.unwrappedSize();
	
		if( m_pSkin )
			return m_pSkin->sizeForContent(contentSize);
		else
			return contentSize;
	}
	
	//____ pointerStyle() ________________________________________
	
	WgPointerStyle TextEditor::pointerStyle() const
	{
		if( m_text.getMarkedLink() )
			return WG_POINTER_HAND;
	
		return m_pointerStyle;
	}
	
	//____ tooltipString() _____________________________________________________
	
	String TextEditor::tooltipString() const
	{
		if( !m_tooltip.isEmpty() )
			return m_tooltip;
		else
		{
			Size sz = size();
			if( sz.w < m_text.width() || sz.h < m_text.height() )
				return m_text.getBuffer();
		}
	
		return 0;
	}
	
	//____ _onRender() ________________________________________________________
	
	void TextEditor::_onRender( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, const Rect& _clip )
	{
		Widget::_onRender(pDevice,_canvas,_window,_clip);
	
		Rect canvas;
		if( m_pSkin )
			canvas = m_pSkin->contentRect(_canvas, m_state);
		else
			canvas = _canvas;
	
		pDevice->printText( _clip, &m_text, canvas );
	}
	
	//____ _onRefresh() _______________________________________________________
	
	void TextEditor::_onRefresh( void )
	{
		//TODO: Implement more I believe...
	
		Widget::_onRefresh();
	}
	
	//____ _onStateChanged() ______________________________________________________
	
	void TextEditor::_onStateChanged( State oldState )
	{
		Widget::_onStateChanged(oldState);
	
		m_text.setState(m_state);
		_requestRender(); //TODO: Only requestRender if skin or text appearance has changed.
	
		if( isEditable() )
		{
			if( m_state.isFocused() && !oldState.isFocused() )
			{
				m_text.showCursor();
				m_tickRouteId = Base::msgRouter()->addRoute( WG_MSG_TICK, this );
				if(	m_bResetCursorOnFocus )
					m_text.goEot();
				_requestRender();
			}
			if( !m_state.isFocused() && oldState.isFocused() )
			{
				m_text.hideCursor();
				Base::msgRouter()->deleteRoute( m_tickRouteId );
				m_tickRouteId = 0;
				_requestRender();
			}
		}
	}
	
	
	//____ _onMsg() ______________________________________________________________
	
	void TextEditor::_onMsg( const Msg_p& pMsg )
	{
		Widget::_onMsg(pMsg);
	
		int type 				= pMsg->type();
		WgModifierKeys modKeys 	= pMsg->modKeys();
	
		if( type == WG_MSG_TICK )
		{
			if( isSelectable() && m_state.isFocused() )
			{
				m_text.incTime( TickMsg::cast(pMsg)->millisec() );
				_requestRender();					//TODO: Should only render the cursor and selection!
			}
			return;
		}
	
	
	
		if( m_state.isFocused() && (type == WG_MSG_MOUSE_PRESS || type == WG_MSG_MOUSE_DRAG) && MouseButtonMsg::cast(pMsg)->button() == WG_BUTTON_LEFT )
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
			if(m_state.isFocused() && MouseButtonMsg::cast(pMsg)->button() == WG_BUTTON_LEFT)
				m_text.setSelectionMode(false);
		}
		else if( !m_state.isFocused() && isEditable() && type == WG_MSG_MOUSE_PRESS && MouseButtonMsg::cast(pMsg)->button() == WG_BUTTON_LEFT )
		{
			grabFocus();
		}
	
	
		if( type == WG_MSG_CHARACTER )
		{
			if( isEditable() )
			{
				int  chr = CharacterMsg::cast(pMsg)->character();
	
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
			switch( KeyMsg::cast(pMsg)->translatedKeyCode() )
			{
				case WG_KEY_SHIFT:
					if(!Base::inputHandler()->isButtonPressed(WG_BUTTON_LEFT))
						m_text.setSelectionMode(false);
				break;
			}
		}
	
		if( (type == WG_MSG_KEY_PRESS || type == WG_MSG_KEY_REPEAT) && isEditable() )
		{
			switch( KeyMsg::cast(pMsg)->translatedKeyCode() )
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
		bool bChanged = m_text.onAction( action, button_key, globalGeo(), Coord(info.x, info.y) );
		if( bChanged )
			RequestRender();
	*/
	
		// Swallow message depending on rules.
	
		if( pMsg->isMouseButtonMsg() && isSelectable() )
		{
			if( MouseButtonMsg::cast(pMsg)->button() == WG_BUTTON_LEFT )
				pMsg->swallow();
		}
		else if( pMsg->isKeyMsg() && isEditable() )
		{
			int key = KeyMsg::cast(pMsg)->translatedKeyCode();
			if( KeyMsg::cast(pMsg)->isMovementKey() == true ||
				key == WG_KEY_DELETE || key == WG_KEY_BACKSPACE || key == WG_KEY_RETURN || (key == WG_KEY_TAB && m_bTabLock) )
					pMsg->swallow();
			
			//TODO: Would be good if we didn't forward any character-creating keys either...
		}
		else if( type == WG_MSG_CHARACTER )
			pMsg->swallow();
	}
	
	
	//____ _onCloneContent() _______________________________________________________
	
	void TextEditor::_onCloneContent( const Widget * _pOrg )
	{
		const TextEditor * pOrg = static_cast<const TextEditor*>(_pOrg);
	
		m_text = pOrg->m_text;
		m_maxLines = pOrg->m_maxLines;
	}
	
	//____ _onSkinChanged() _______________________________________________________
	
	void TextEditor::_onSkinChanged( const Skin_p& pOldSkin, const Skin_p& pNewSkin )
	{
		Widget::_onSkinChanged(pOldSkin,pNewSkin);
		m_text.setColorSkin(pNewSkin);
	}
	
	//____ _onNewSize() ________________________________________________
	
	void TextEditor::_onNewSize( const Size& size )
	{
		int width = size.w;
	
		if( m_pSkin )
			width -= m_pSkin->contentPadding().w;
	
		m_text.setLineWidth( width );
	}
	
	
	
	//____ insertTextAtCursor() ___________________________________________________
	
	int TextEditor::insertTextAtCursor( const CharSeq& str )
	{
		if( !isEditable() )
			return 0;
	
		if( !m_state.isFocused() )
			if( !grabFocus() )
				return 0;				// Couldn't get input focus...
	
		int nChars = m_text.putText( str );
	
		if( m_maxLines != 0 && m_maxLines < m_text.nbSoftLines() )
		{
			m_text.unputText( nChars );
			nChars = 0;
		}
	
		return nChars;
	}
	
	//____ insertCharAtCursor() ___________________________________________________
	
	bool TextEditor::insertCharAtCursor( Uint16 c )
	{
		if( !isEditable() )
			return 0;
	
		if( !m_state.isFocused() )
			if( !grabFocus() )
				return false;				// Couldn't get input focus...
	
		return _insertCharAtCursor(c);
	}
	
	//____ _insertCharAtCursor() ___________________________________________
	
	bool TextEditor::_insertCharAtCursor( Uint16 c )
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
	
	void TextEditor::_onFieldDirty( Field * pField )
	{
		_requestRender();
	}
	
	//____ _onFieldResize() ________________________________________________________
	
	void TextEditor::_onFieldResize( Field * pField )
	{
		m_bResetCursorOnFocus = true;
		_requestResize();
		_requestRender();
	}
	

} // namespace wg
