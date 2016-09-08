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

#include <wg_legacytexteditor.h>
#include <wg_key.h>
#include <wg_font.h>
#include <wg_gfxdevice.h>
#include <wg_msgrouter.h>
#include <wg_base.h>

namespace wg 
{
	
	const char LegacyTextEditor::CLASSNAME[] = {"LegacyTextEditor"};
	
	
	
	//____ LegacyTextEditor() _________________________________________________________________
	
	LegacyTextEditor::LegacyTextEditor() : m_text(this), text(&m_text)
	{
		m_maxLines		= 0;
	
		m_text.setLineWidth( size().w );
		m_text.setAutoEllipsis(isAutoEllipsisDefault());
		m_text.setEditMode( TextEditMode::Static );
		m_bResetCursorOnFocus = true;
		m_tickRouteId = 0;
	}
	
	
	//____ Destructor _____________________________________________________________
	
	LegacyTextEditor::~LegacyTextEditor()
	{
		if( m_tickRouteId )
			Base::msgRouter()->deleteRoute( m_tickRouteId );
	}
	
	
	//____ isInstanceOf() _________________________________________________________
	
	bool LegacyTextEditor::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return Widget::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * LegacyTextEditor::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	LegacyTextEditor_p LegacyTextEditor::cast( const Object_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return LegacyTextEditor_p( static_cast<LegacyTextEditor*>(pObject.rawPtr()) );
	
		return 0;
	}
	
	//_______________________________________________________________
	void LegacyTextEditor::setEditMode(TextEditMode mode)
	{
		m_text.setEditMode(mode);
	}
	
	//____ matchingHeight() _______________________________________________________
	
	int LegacyTextEditor::matchingHeight( int width ) const
	{
		int textHeight = m_text.heightForWidth( width );
	
		if( m_pSkin )
			textHeight += m_pSkin->contentPadding().h;
	
		return textHeight;
	}
	
	//____ preferredSize() _____________________________________________________________
	
	Size LegacyTextEditor::preferredSize() const
	{
		Size contentSize = m_text.unwrappedSize();
	
		if( m_pSkin )
			return m_pSkin->sizeForContent(contentSize);
		else
			return contentSize;
	}
	
	//____ pointerStyle() ________________________________________
	
	PointerStyle LegacyTextEditor::pointerStyle() const
	{
		if( m_text.getMarkedLink() )
			return PointerStyle::Hand;
	
		return m_pointerStyle;
	}
	
	//____ tooltipString() _____________________________________________________
	
	String LegacyTextEditor::tooltipString() const
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
	
	void LegacyTextEditor::_onRender( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, const Rect& _clip )
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
	
	void LegacyTextEditor::_onRefresh( void )
	{
		//TODO: Implement more I believe...
	
		Widget::_onRefresh();
	}
	
	//____ _onStateChanged() ______________________________________________________
	
	void LegacyTextEditor::_onStateChanged( State oldState )
	{
		Widget::_onStateChanged(oldState);
	
		m_text.setState(m_state);
		_requestRender(); //TODO: Only requestRender if skin or text appearance has changed.
	
		if( isEditable() )
		{
			if( m_state.isFocused() && !oldState.isFocused() )
			{
				m_text.showCursor();
				m_tickRouteId = Base::msgRouter()->addRoute( MsgType::Tick, this );
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
	
	void LegacyTextEditor::_onMsg( const Msg_p& pMsg )
	{
		Widget::_onMsg(pMsg);
	
		MsgType type 				= pMsg->type();
	
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
	/*
		bool bChanged = m_text.onAction( action, button_key, globalGeo(), Coord(info.x, info.y) );
		if( bChanged )
			RequestRender();
	*/
	
		// Swallow message depending on rules.
	
		if( pMsg->isMouseButtonMsg() && isSelectable() )
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
	}
	
	
	//____ _onCloneContent() _______________________________________________________
	
	void LegacyTextEditor::_onCloneContent( const Widget * _pOrg )
	{
		const LegacyTextEditor * pOrg = static_cast<const LegacyTextEditor*>(_pOrg);
	
		m_text = pOrg->m_text;
		m_maxLines = pOrg->m_maxLines;
	}
	
	//____ _onSkinChanged() _______________________________________________________
	
	void LegacyTextEditor::_onSkinChanged( const Skin_p& pOldSkin, const Skin_p& pNewSkin )
	{
		Widget::_onSkinChanged(pOldSkin,pNewSkin);
		m_text.setColorSkin(pNewSkin);
	}
	
	//____ _onNewSize() ________________________________________________
	
	void LegacyTextEditor::_onNewSize( const Size& size )
	{
		int width = size.w;
	
		if( m_pSkin )
			width -= m_pSkin->contentPadding().w;
	
		m_text.setLineWidth( width );
	}
	
	
	
	//____ insertTextAtCursor() ___________________________________________________
	
	int LegacyTextEditor::insertTextAtCursor( const CharSeq& str )
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
	
	bool LegacyTextEditor::insertCharAtCursor( uint16_t c )
	{
		if( !isEditable() )
			return 0;
	
		if( !m_state.isFocused() )
			if( !grabFocus() )
				return false;				// Couldn't get input focus...
	
		return _insertCharAtCursor(c);
	}
	
	//____ _insertCharAtCursor() ___________________________________________
	
	bool LegacyTextEditor::_insertCharAtCursor( uint16_t c )
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
	
	void LegacyTextEditor::_onFieldDirty( Field * pField )
	{
		_requestRender();
	}

	void LegacyTextEditor::_onFieldDirty( Field * pField, const Rect& rect )
	{
		_requestRender();
	}

	
	//____ _onFieldResize() ________________________________________________________
	
	void LegacyTextEditor::_onFieldResize( Field * pField )
	{
		m_bResetCursorOnFocus = true;
		_requestResize();
		_requestRender();
	}
	

} // namespace wg
