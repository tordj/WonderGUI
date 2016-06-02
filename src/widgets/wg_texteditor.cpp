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
#include <wg_gfxdevice.h>
#include <wg_msgrouter.h>
#include <wg_base.h>

namespace wg 
{
	
	const char TextEditor::CLASSNAME[] = {"TextEditor"};
	
	
	
	//____ TextEditor() _________________________________________________________________
	
	TextEditor::TextEditor() : m_text(this), text(&m_text)
	{
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
		
	//____ matchingHeight() _______________________________________________________
	
	int TextEditor::matchingHeight( int width ) const
	{
		int textHeight = m_text.matchingHeight( width );
	
		if( m_pSkin )
			textHeight += m_pSkin->contentPadding().h;
	
		return textHeight;
	}
	
	//____ preferredSize() _____________________________________________________________
	
	Size TextEditor::preferredSize() const
	{
		Size contentSize = m_text.preferredSize();
	
		if( m_pSkin )
			return m_pSkin->sizeForContent(contentSize);
		else
			return contentSize;
	}
	
	//____ _render() ________________________________________________________
	
	void TextEditor::_render( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, const Rect& _clip )
	{
		Widget::_render(pDevice,_canvas,_window,_clip);
	
		Rect canvas;
		if( m_pSkin )
			canvas = m_pSkin->contentRect(_canvas, m_state);
		else
			canvas = _canvas;

		m_text.onRender( pDevice, canvas, _clip );	
	}
	
	//____ _refresh() _______________________________________________________
	
	void TextEditor::_refresh( void )
	{
		//TODO: Implement more I believe...
	
		Widget::_refresh();
	}
	
	//____ _setState() ______________________________________________________
	
	void TextEditor::_setState( State state )
	{
		Widget::_setState(state);
	
		m_text.setState(state);
		_requestRender(); //TODO: Only requestRender if skin or text appearance has changed.
/*	
		if( isEditable() )
		{
			if( m_state.isFocused() && !oldState.isFocused() )
			{
				m_text.showCaret();
				m_tickRouteId = Base::msgRouter()->addRoute( MsgType::Tick, this );
				if(	m_bResetCaretOnFocus )
					m_text.goEot();
				_requestRender();
			}
			if( !m_state.isFocused() && oldState.isFocused() )
			{
				m_text.hideCaret();
				Base::msgRouter()->deleteRoute( m_tickRouteId );
				m_tickRouteId = 0;
				_requestRender();
			}
		}
*/
	}
	
	
	//____ _receive() ______________________________________________________________
	
	void TextEditor::_receive( const Msg_p& pMsg )
	{
		Widget::_receive( pMsg );
		m_text.receive( pMsg );
	
		MsgType type 				= pMsg->type();



/*	
		if( type == MsgType::Tick )
		{
			if( isSelectable() && m_state.isFocused() )
			{
				m_text.incTime( TickMsg::cast(pMsg)->timediff() );
				_requestRender();					//TODO: Should only render the cursor and selection!
			}
			return;
		}
	
	
	
		if( m_state.isFocused() && (type == MsgType::MousePress || type == MsgType::MouseDrag) && MouseButtonMsg::cast(pMsg)->button() == MouseButton::Left )
		{
			MouseButtonMsg_p p = MouseButtonMsg::cast(pMsg);
			
			ModifierKeys modKeys 	= p->modKeys();
			Coord pointerPos = p->pointerPos();
	
			if( isSelectable() && (modKeys & MODKEY_SHIFT) )
			{
				m_text.setSelectionMode(true);
			}
	
			m_text.cursorGotoCoord( pointerPos, globalGeo() );
	
			if(isSelectable() && type == MsgType::MousePress && !(modKeys & MODKEY_SHIFT))
			{
				m_text.clearSelection();
				m_text.setSelectionMode(true);
			}
		}
		else if( type == MsgType::MouseRelease  )
		{
			if(m_state.isFocused() && MouseButtonMsg::cast(pMsg)->button() == MouseButton::Left)
				m_text.setSelectionMode(false);
		}
		else if( !m_state.isFocused() && isEditable() && type == MsgType::MousePress && MouseButtonMsg::cast(pMsg)->button() == MouseButton::Left )
		{
			grabFocus();
		}
	
	
		if( type == MsgType::TextInput )
		{
			if( isEditable() )
			{
				String text = TextInputMsg::cast(pMsg)->text();
	
				for( int i = 0 ; i < text.length() ; i++ )
				{
					unsigned short chr = text.chars()[i].getGlyph();
	
					if( chr >= 32 && chr != 127)
					{
						_insertCharAtCaret(chr);
					}
					else if( chr == 13 )
					{
						_insertCharAtCaret('\n');
					}
					else if( chr == '\t' && m_bTabLock )
					{
						_insertCharAtCaret( '\t' );
					}
				}
			}
		}
	
		if( type == MsgType::KeyRelease )
		{
			switch( KeyMsg::cast(pMsg)->translatedKeyCode() )
			{
				case Key::Shift:
					if(!Base::inputHandler()->isButtonPressed(MouseButton::Left))
						m_text.setSelectionMode(false);
				break;
			}
		}
	
		if( (type == MsgType::KeyPress || type == MsgType::KeyRepeat) && isEditable() )
		{
			KeyMsg_p p = KeyMsg::cast(pMsg);
			ModifierKeys modKeys = p->modKeys();
			
			switch( p->translatedKeyCode() )
			{
				case Key::Left:
					if( modKeys & MODKEY_SHIFT )
						m_text.setSelectionMode(true);
	
					if( modKeys & MODKEY_CTRL )
						m_text.gotoPrevWord();
					else
						m_text.goLeft();
					break;
				case Key::Right:
					if( modKeys & MODKEY_SHIFT )
						m_text.setSelectionMode(true);
	
					if( modKeys & MODKEY_CTRL )
						m_text.gotoNextWord();
					else
						m_text.goRight();
					break;
	
				case Key::Up:
					if( modKeys & MODKEY_SHIFT )
						m_text.setSelectionMode(true);
	
					m_text.cursorGoUp( 1, globalGeo() );
					break;
	
				case Key::Down:
					if( modKeys & MODKEY_SHIFT )
						m_text.setSelectionMode(true);
	
					m_text.cursorGoDown( 1, globalGeo() );
					break;
	
				case Key::Backspace:
					if(m_text.hasSelection())
						m_text.delSelection();
					else if( modKeys & MODKEY_CTRL )
						m_text.delPrevWord();
					else
						m_text.delPrevChar();
					break;
	
				case Key::Delete:
					if(m_text.hasSelection())
						m_text.delSelection();
					else if( modKeys & MODKEY_CTRL )
						m_text.delNextWord();
					else
						m_text.delNextChar();
					break;
	
				case Key::Home:
					if( modKeys & MODKEY_SHIFT )
						m_text.setSelectionMode(true);
	
					if( modKeys & MODKEY_CTRL )
						m_text.goBot();
					else
						m_text.goBol();
					break;
	
				case Key::End:
					if( modKeys & MODKEY_SHIFT )
						m_text.setSelectionMode(true);
	
					if( modKeys & MODKEY_CTRL )
						m_text.goEot();
					else
						m_text.goEol();
					break;
	
				default:
					break;
			}
		}
	
		// Let text object handle its actions.

//		bool bChanged = m_text.onAction( action, button_key, globalGeo(), Coord(info.x, info.y) );
//		if( bChanged )
//			RequestRender();

	
		// Swallow message depending on rules.
	
		if( pMsg->isMouseButtreceive() && isSelectable() )
		{
			if( MouseButtonMsg::cast(pMsg)->button() == MouseButton::Left )
				pMsg->swallow();
		}
		else if( pMsg->isKeyMsg() && isEditable() )
		{
			Key key = KeyMsg::cast(pMsg)->translatedKeyCode();
			if( KeyMsg::cast(pMsg)->isMovementKey() == true ||
				key == Key::Delete || key == Key::Backspace || key == Key::Return || (key == Key::Tab && m_bTabLock) )
					pMsg->swallow();
			
			//TODO: Would be good if we didn't forward any character-creating keys either...
		}
		else if( type == MsgType::TextInput )
			pMsg->swallow();
*/			
	}
	
	
	//____ _cloneContent() _______________________________________________________
	
	void TextEditor::_cloneContent( const Widget * _pOrg )
	{
		Widget::_cloneContent( _pOrg );

		const TextEditor * pOrg = static_cast<const TextEditor*>(_pOrg);
	
		m_text = pOrg->m_text;
	}
	
	//____ _setSkin() _______________________________________________________
	
	void TextEditor::_setSkin( const Skin_p& pSkin )
	{
		//TODO: Possibly notify text about new canvas size.
		
		Widget::_setSkin(pSkin);
	}
	
	//____ _setSize() ________________________________________________
	
	void TextEditor::_setSize( const Size& size )
	{
		Widget::_setSize( size );
		
		int width = size.w;
	
		if( m_pSkin )
			m_text.onNewSize(size - m_pSkin->contentPadding());
		else
			m_text.onNewSize(size);
	}
	
	//____ _onFieldDirty() _________________________________________________________
	
	void TextEditor::_onFieldDirty( Field * pField )
	{
		_requestRender();
	}

	void TextEditor::_onFieldDirty( Field * pField, const Rect& rect )
	{
		_requestRender();
	}
	
	//____ _onFieldResize() ________________________________________________________
	
	void TextEditor::_onFieldResize( Field * pField )
	{
		_requestResize();
		_requestRender();
	}
	

} // namespace wg
