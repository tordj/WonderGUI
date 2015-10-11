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


#include <wg_combobox.h>
#include <wg_menu.h>
#include <wg_geo.h>
#include <wg_gfxdevice.h>
#include <wg_util.h>
#include <wg_msgrouter.h>
#include <wg_popuplayer.h>
#include <wg_base.h>

namespace wg 
{
	
	const char Combobox::CLASSNAME[] = {"Combobox"};
	
	
	//____ Combobox() _________________________________________________________________
	
	Combobox::Combobox( void ) : m_text(this), text(&m_text)
	{
		m_text.setAlignment( WG_WEST );
		m_text.setWrap(false);
		m_text.setAutoEllipsis(isAutoEllipsisDefault());	
	//	m_textColor = m_text.color();
	//	m_textDisabledColor	= 0xFFFF;
	
		m_state = WG_STATE_NORMAL;
	
		m_bResetCursorOnFocus = true;
		m_bPressInInputRect = false;
		m_pMenu		= 0;
		m_pSelectedItem = 0;
		m_textFormat = "%1";
		m_viewOfs		= 0;
		m_maxCharacters = 0;
		
		m_routeId = 0;
		m_tickRouteId = 0;
	}
	
	//____ Destructor _____________________________________________________________
	
	Combobox::~Combobox()
	{
	}
	
	//____ isInstanceOf() _________________________________________________________
	
	bool Combobox::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return Widget::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * Combobox::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	Combobox_p Combobox::cast( const Object_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return Combobox_p( static_cast<Combobox*>(pObject.rawPtr()) );
	
		return 0;
	}
	
	
	//____ preferredSize() __________________________________________________________
	
	Size Combobox::preferredSize() const
	{
		TextAttr attr;
		m_text.getBaseAttr( attr );
		int width = TextTool::lineWidth( attr, "MMMMMMMMMM" );		// Default combobox should fit 10 letter M in textfield
		Size contentSize( m_text.height(), width );
		
		if( m_pSkin )
			return m_pSkin->sizeForContent( contentSize );
		else
			return contentSize;
	}
	
	
	//____ setMenu() ______________________________________________________________
	
	void Combobox::setMenu( const Menu_p& pMenu )
	{
		if( pMenu == m_pMenu )
			return;
	
		m_pMenu = pMenu;
		if( m_pMenu && m_pMenu->GetSelectedItem() )
			_entrySelected(m_pMenu->GetSelectedItem()->getId());
	}
	
	//____ menu() __________________________________________________________
	
	Menu_p Combobox::menu() const
	{
		return m_pMenu;
	}
	
	//____ setEditMode() __________________________________________________________
	
	void Combobox::setEditMode(TextEditMode mode)
	{
		m_text.setEditMode(mode);
	}
	
	//____ setTextFormat() ________________________________________________________
	
	void Combobox::setTextFormat( const CharSeq& str )
	{
		m_textFormat = str;
		if( m_pMenu && m_pMenu->GetSelectedItem() )
			_entrySelected(m_pMenu->GetSelectedItem()->getId());
	}
	
	//____ setPlaceholderText() ___________________________________________________
	
	void Combobox::setPlaceholderText( const CharSeq& str )
	{
		m_placeholderText = str;
		if( m_text.isEmpty() && !m_text.isCursorShowing() )
			_requestRender();
	}
	
	//____ insertTextAtCursor() ___________________________________________________
	
	int Combobox::insertTextAtCursor( const CharSeq& str )
	{
		if( !_isEditable() )
			return 0;
	
		if( !m_state.isFocused() )
			if( !grabFocus() )
				return 0;				// Couldn't get input focus...
	
		int retVal = 0;
	
		if( m_maxCharacters == 0 || str.length() < m_maxCharacters - m_text.length() )
		{
			m_text.putText( str );
			retVal = str.length();
		}
		else
		{
			retVal = m_maxCharacters - m_text.length();
			m_text.putText( CharSeq( str, 0, retVal ) );
		}
	
		Base::msgRouter()->post( new TextEditMsg( text.ptr(),false) );
	
		_adjustViewOfs();
	
		return retVal;
	}
	
	//____ insertCharAtCursor() ___________________________________________________
	
	bool Combobox::insertCharAtCursor( uint16_t c )
	{
		if( !_isEditable() )
			return 0;
	
		if( !m_state.isFocused() )
			if( !grabFocus() )
				return false;				// Couldn't get input focus...
	
		if( m_maxCharacters != 0 && m_maxCharacters < m_text.length() )
			return false;
	
		m_text.putChar( c );
	
		Base::msgRouter()->post( new TextEditMsg( text.ptr(),false) );
	
		_adjustViewOfs();
		return true;
	}
	
	//____ _closeMenu() ___________________________________________________________
	
	void Combobox::_closeMenu()
	{
		if( parent() )
		{
			PopupLayer * pLayer = parent()->_getPopupLayer();
			if( pLayer )
				pLayer->closePopup( m_pMenu );
	
			Base::msgRouter()->deleteRoute( m_routeId );
		}
	}
	
	//____ _onMsg() _____________________________________________________________
	
	void Combobox::_onMsg( const Msg_p& _pMsg )
	{
		State oldState = m_state;
	
		switch( _pMsg->type() )
		{
			case WG_MSG_TICK:
				if( _isEditable() && m_state.isFocused() )
				{
					m_text.incTime( TickMsg::cast(_pMsg)->timediff() );
					_requestRender();					//TODO: Should only render the cursor and selection!
				}
			break;
	
			case WG_MSG_POPUP_CLOSED:
				m_state.setPressed(false);
			break;
	
			case WG_MSG_ITEMS_SELECT:
			break;
	
			case WG_MSG_MOUSE_ENTER:
				m_state.setHovered(true);
				break;
	
			case WG_MSG_MOUSE_LEAVE:
				if( !m_state.isPressed() )
					m_state.setHovered(false);
				break;
	
			case WG_MSG_MOUSE_MOVE:
			{
				Coord pos = MouseMoveMsg::cast(_pMsg)->pointerPos() - globalPos();
				Rect inputRect = m_pSkin ? m_pSkin->contentRect(size(),m_state): Rect( 0,0, size() );
	
				if( _isSelectable() && inputRect.contains( pos ) )
				{
					m_pointerStyle = WG_POINTER_IBEAM;
				}
				else
				{
					m_pointerStyle = WG_POINTER_DEFAULT;
				}
				break;
			}
	
	
	
			case WG_MSG_MOUSE_PRESS:
			{
				MousePressMsg_p pMsg = MousePressMsg::cast(_pMsg);
				Coord pos = pMsg->pointerPos() - globalPos();
	
				if( pMsg->button() == WG_BUTTON_LEFT )
				{
					Rect inputRect = m_pSkin ? m_pSkin->contentRect( size(), m_state ): Rect( 0,0, size() );
	
					if( m_state.isPressed() && m_pMenu )
					{
						m_bPressInInputRect = false;
						_closeMenu();
					}
					else if( _isEditable() && inputRect.contains(pos) )
					{
						if( !m_state.isFocused() )
						{
							grabFocus();
							if( m_state.isFocused() )
								m_bFocusPress = true;		// Current button press brought focus.
						}
	
						m_bPressInInputRect = true;
	
						if( m_state.isFocused() )
						{
							if( _isSelectable() && (pMsg->modKeys() & WG_MODKEY_SHIFT))
							{
								m_text.setSelectionMode(true);
							}
	
	
							int x = pos.x;
							int y = pos.y;
							x += m_viewOfs;
	
							m_text.cursorGotoCoord( Coord(x, 0), Rect(inputRect.x,0,1000000,1000000) );
					
							if(_isSelectable() && !(pMsg->modKeys() & WG_MODKEY_SHIFT))
							{
								m_text.clearSelection();
								m_text.setSelectionMode(true);
							}
							_adjustViewOfs();
						}
	
					}
					else if( m_pMenu )
					{
						m_bPressInInputRect = false;
	
						if( parent() )
						{
							PopupLayer * pLayer = parent()->_getPopupLayer();
							if( pLayer )
							{
								pLayer->openPopup( m_pMenu, this, m_pHook->globalGeo() - pLayer->globalPos(), WG_SOUTHWEST );
	//TODO: Fix					m_routeId = Base::msgRouter()->addRoute( MsgFilter::itemsSelect(), m_pMenu, cbEntrySelected, this );
							}
						}
	
						m_state.setPressed(true);
					}
					_pMsg->swallow();
				}
				break;
			}
	
			case WG_MSG_MOUSE_DRAG:
			{
				MouseDragMsg_p pMsg = MouseDragMsg::cast(_pMsg);
				if( pMsg->button() == WG_BUTTON_LEFT )
				{
					if( m_state.isFocused() && m_bPressInInputRect )
					{
						if( _isSelectable() && (pMsg->modKeys() & WG_MODKEY_SHIFT) )
						{
							m_text.setSelectionMode(true);
						}
	
						int x = pMsg->pointerPos().x - globalPos().x + m_viewOfs;
						int leftBorder = m_pSkin ? m_pSkin->contentRect( size(), m_state ).x : 0;
	
						m_text.cursorGotoCoord( Coord(x, 0), Rect(leftBorder,0,1000000,1000000) );
						_adjustViewOfs();
					}
					_pMsg->swallow();
				}
				break;
			}
	
			case WG_MSG_MOUSE_RELEASE:
			{
				MouseReleaseMsg_p pMsg = MouseReleaseMsg::cast(_pMsg);
				if( pMsg->button() == WG_BUTTON_LEFT )
				{
					if( m_state.isFocused() )
					{
						m_text.setSelectionMode(false);
						if( m_bFocusPress )
						{
							m_bFocusPress = false;
							if( !m_text.hasSelection() )
								m_text.selectAll();
						}
					}
					_pMsg->swallow();
				}
				break;
			}
	
			case WG_MSG_WHEEL_ROLL:
			{		
				WheelRollMsg_p pMsg = WheelRollMsg::cast(_pMsg);
				if( !m_state.isFocused() && m_pMenu && m_pMenu->getItemCount() != 0 )
				{
					MenuItem * pItem = m_pSelectedItem;
					int distance = pMsg->distance().y;
	
					if( !pItem )
					{
						if( distance > 0 )
						{
							pItem = m_pMenu->GetLastItem();
							distance--;
						}
						if( distance < 0 )
						{
							pItem = m_pMenu->GetFirstItem();
							distance++;
						}
					}
	
					while( distance > 0 && pItem->prev() )
					{
						pItem = pItem->prev();
						distance--;
					}
	
					while( distance < 0 && pItem->next() )
					{
						pItem = pItem->next();
						distance++;
					}
	
					m_pMenu->selectItem( pItem );
					_entrySelected( pItem->getId() );
					_pMsg->swallow();
				}
				break;
			}
	
			case WG_MSG_TEXT_INPUT:
			{
				TextInputMsg_p pMsg = TextInputMsg::cast(_pMsg);
				if( _isEditable() && m_state.isFocused() )
				{
	
					if(m_text.hasSelection())
						m_text.delSelection();
					m_text.setSelectionMode(false);
	
					// by default - no max limit
					if( m_maxCharacters == 0 || m_maxCharacters >= m_text.length() + pMsg->text().length() )
						m_text.putText( pMsg->text() );
	
					Base::msgRouter()->post( new TextEditMsg(text.ptr(),false) );
					_adjustViewOfs();
				}
				break;
			}
	
			case WG_MSG_KEY_RELEASE:
				if( m_state.isFocused() )
				{
					KeyReleaseMsg_p pMsg = KeyReleaseMsg::cast(_pMsg);
					switch( pMsg->translatedKeyCode() )
					{
						case WG_KEY_SHIFT:
							if(!Base::inputHandler()->isButtonPressed(WG_BUTTON_LEFT))
								m_text.setSelectionMode(false);
							_pMsg->swallow();
						break;
					}
				}
			break;
	
			case WG_MSG_KEY_PRESS:
			case WG_MSG_KEY_REPEAT:
			{
				KeyMsg_p pMsg = KeyMsg::cast(_pMsg);
	
				if( pMsg->translatedKeyCode() == WG_KEY_ESCAPE && m_state.isPressed() )
				{
					_closeMenu();
				}
				else if( _isEditable() && m_state.isFocused() )
				{
					switch( pMsg->translatedKeyCode() )
					{
						case WG_KEY_LEFT:
							if( pMsg->modKeys() & WG_MODKEY_SHIFT )
								m_text.setSelectionMode(true);
	
							if( pMsg->modKeys() & WG_MODKEY_CTRL )
								m_text.gotoPrevWord();
							else
								m_text.goLeft();
							_pMsg->swallow();
							break;
						case WG_KEY_RIGHT:
							if( pMsg->modKeys() & WG_MODKEY_SHIFT )
								m_text.setSelectionMode(true);
	
							if( pMsg->modKeys() & WG_MODKEY_CTRL )
									m_text.gotoNextWord();
							else
								m_text.goRight();
							_pMsg->swallow();
							break;
	
						case WG_KEY_BACKSPACE:
							if(m_text.hasSelection())
								m_text.delSelection();
							else if( pMsg->modKeys() & WG_MODKEY_CTRL )
								m_text.delPrevWord();
							else
								m_text.delPrevChar();
							
							Base::msgRouter()->post( new TextEditMsg(text.ptr(), false) ); //TODO: Should only emit if text really has changed
							_pMsg->swallow();
							break;
	
						case WG_KEY_DELETE:
							if(m_text.hasSelection())
								m_text.delSelection();
							else if( pMsg->modKeys() & WG_MODKEY_CTRL )
								m_text.delNextWord();
							else
								m_text.delNextChar();
							Base::msgRouter()->post( new TextEditMsg(text.ptr(), false) );		//TODO: Should only emit if text really has changed
							_pMsg->swallow();
							break;
	
						case WG_KEY_HOME:
	
							//
							//	I am not sure if this is the proper way to this, but in my opinion, the default
							//	"actions" has to be separated from any modifier key action combination
							//
							switch( pMsg->modKeys() )
							{
	
							case WG_MODKEY_CTRL:
								break;
	
							default: // no modifier key was pressed
								if( pMsg->modKeys() & WG_MODKEY_SHIFT )
									m_text.setSelectionMode(true);
	
								m_text.goBol();
								break;
							}
							_pMsg->swallow();
							break;
	
						case WG_KEY_END:
	
							//
				 			//	I am not sure if this is the proper way to this, but in my opinion, the default
			 				//	"actions" has to be separated from any modifier key action combination
							//
							switch( pMsg->modKeys() )
							{
	
							case WG_MODKEY_CTRL:
								break;
	
							default: // no modifier key was pressed
								if( pMsg->modKeys() & WG_MODKEY_SHIFT )
									m_text.setSelectionMode(true);
	
								m_text.goEol();
								break;
							}
	
							_pMsg->swallow();
							break;
					}
					_adjustViewOfs();
				}
			}
			break;
		}
	
		if( m_state != oldState )
			_onStateChanged( oldState );
	}
	
	//____ _onStateChanged() ______________________________________________________
	
	void Combobox::_onStateChanged( State oldState )
	{
		Widget::_onStateChanged( oldState );
	
		m_text.setState( m_state );
	
		// Check if we got focus
	
		if( m_state.isFocused() && !oldState.isFocused() )
		{
			if( _isEditable() )
			{
				m_tickRouteId = Base::msgRouter()->addRoute( WG_MSG_TICK, this );
				m_text.showCursor();
				if( m_bResetCursorOnFocus )
				{
					m_text.goEol();
					m_text.selectAll();
				}
			}
		}
	
		// Check if we lost focus
	
		if( !m_state.isFocused() && oldState.isFocused() )
		{
			if( _isEditable() )
			{
				Base::msgRouter()->deleteRoute( m_tickRouteId );
				m_text.hideCursor();
				m_text.clearSelection();
				m_bResetCursorOnFocus = true;
				Base::msgRouter()->post( new TextEditMsg( text.ptr(),true ) );	//TODO: Should only do if text was really changed!
			}
		}
	}
	
	//____ _onSkinChanged() _______________________________________________________
	
	void Combobox::_onSkinChanged( const Skin_p& pOldSkin, const Skin_p& pNewSkin )
	{
		Widget::_onSkinChanged(pOldSkin,pNewSkin);
		m_text.setColorSkin(pNewSkin);
	}
	
	
	
	//____ _onRender() ________________________________________________________
	
	void Combobox::_onRender( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, const Rect& _clip )
	{
		Widget::_onRender( pDevice, _canvas, _window, _clip );
	
		// Print the text
	
		Rect r( _canvas );
		if( m_pSkin )
			r = m_pSkin->contentRect(_canvas, m_state);
	
		Rect	textClip( r, _clip );
	
		bool bPlaceholder = false;
		if( !m_placeholderText.isEmpty() && m_text.isEmpty() && !m_text.isCursorShowing() )
		{
			bPlaceholder = true;
			m_text.set( m_placeholderText );
		}
	
		r.x -= m_viewOfs;
		r.w += m_viewOfs;
		pDevice->printText( textClip, &m_text, r );
	
		if( bPlaceholder )
			m_text.clear();
	}
	
	//____ _onRefresh() _______________________________________________________
	
	void Combobox::_onRefresh( void )
	{
		Widget::_onRefresh();
	}
	
	//____ _onCloneContent() _______________________________________________________
	
	void Combobox::_onCloneContent( const Widget * _pOrg )
	{
	
		const Combobox * pOrg = static_cast<const Combobox*>(_pOrg);
	
		m_textFormat		= pOrg->m_textFormat;
		m_placeholderText	= pOrg->m_placeholderText;
		m_pMenu				= pOrg->m_pMenu;
		m_pSelectedItem		= pOrg->m_pSelectedItem;
		m_text				= pOrg->m_text;
		m_maxCharacters		= pOrg->m_maxCharacters;
	}
	
	
	//____ _adjustViewOfs() ________________________________________________________
	
	void Combobox::_adjustViewOfs()
	{
		// Possibly move viewOfs so that:
		//	1 Cursor remains inside view.
		//  2 At least one character is displayed before the cursor
		//  3 At least one character is displayed after the cursor (if there is one).
	
		if( m_state.isFocused() && m_text.properties() && m_text.properties()->font() )
		{
			Caret_p pCursor = TextTool::getCursor( &m_text );
			if( !pCursor )
				return;
	
			int cursCol	= m_text.column();
	
			TextAttr	attr;
			m_text.getBaseAttr( attr );
	
			int cursAdvance	= pCursor->advance(m_text.cursorMode() );
			int cursBearing	= pCursor->bearingX(m_text.cursorMode() );
			int cursWidth	= pCursor->width(m_text.cursorMode() );
	
			int cursOfs;		// Cursor offset from beginning of line in pixels.
			int maxOfs;			// Max allowed view offset in pixels.
			int minOfs;			// Min allowed view offset in pixels.
	
			int geoWidth = size().w;
			if( m_pSkin )
				geoWidth -= m_pSkin->contentPadding().w;
	
			int	lineWidth = m_text.getSoftLineWidth( 0 ) + cursBearing+cursWidth;
	
			// Calculate cursOfs
	
			cursOfs	= m_text.getSoftLineWidthPart( 0, 0, cursCol );
	
			// Calculate maxOfs
	
			if( cursCol > 0 )
			{
				maxOfs = m_text.getSoftLineWidthPart( 0, 0, cursCol-1 );
	
				if( lineWidth < maxOfs + geoWidth )
					maxOfs = WgMax( lineWidth - geoWidth, 0 );
			}
			else
				maxOfs = cursOfs;
	
	
			// Calculate minOfs
	
			if( cursCol < m_text.getLine(0)->nChars )
				minOfs = m_text.getSoftLineWidthPart( 0, 0, cursCol+1 ) + cursAdvance - geoWidth;	// Not 100% right, cursor might affect linewidth different from its own width.
			else
				minOfs = cursOfs + cursBearing + cursWidth - geoWidth;
	
			// Check boundaries and update
	
			if( m_viewOfs > maxOfs )
				m_viewOfs = maxOfs;
	
			if( m_viewOfs < minOfs )
				m_viewOfs = minOfs;
	
	
		}
		else
			m_viewOfs = 0;				// Show beginning of line when cursor disappears.
	}
	
	//____ _onAlphaTest() ______________________________________________________
	
	bool Combobox::_onAlphaTest( const Coord& ofs, const Size& sz )
	{
		//TODO: Should we treat text-box as opaque for mouse?
	
		return Widget::_onAlphaTest(ofs, sz);
	}
	
	//____ _entrySelected() ________________________________________________________
	
	void Combobox::_entrySelected( int itemId )
	{
		MenuItem * pItem = m_pMenu->FindItem(itemId);
		if( !pItem )
			return ;
	
		m_pSelectedItem = pItem;
		if(pItem && pItem->getType() != SEPARATOR)
		{
			CharBuffer	buff;
	
			buff.pushBack(m_textFormat);
	
			int ofs = buff.findFirst( "%1" );
			if( ofs >= 0 )
			{
				Textprop_p pProp = buff[ofs].getProperties();
	
				const Char * pEntry = ((MenuEntry*)pItem)->getText().chars();
				uint32_t len = TextTool::strlen( pEntry );
	
				buff.replace( ofs, 2, pEntry );
	
				Char * p = buff.beginWrite() + ofs;
	
				for( unsigned int i = 0 ; i < len ; i++ )
				{
					if( p[i].propHandle() == 0 )
						p[i].setProperties( pProp );
				}
	
				buff.endWrite();
	
			}
			m_text.set( &buff );
			_adjustViewOfs();
		}
	}
	
	
	//____ _onFieldDirty() _________________________________________________________
	
	void Combobox::_onFieldDirty( Field * pField )
	{
		_requestRender();
	}
	
	//____ _onFieldResize() ________________________________________________________
	
	void Combobox::_onFieldResize( Field * pField )
	{
		m_bResetCursorOnFocus = true;
		_requestResize();
		_requestRender();
	}
	

} // namespace wg